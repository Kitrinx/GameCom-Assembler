#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct file_data {
	uint8_t *data;
	size_t size;
};

struct input_list {
	char **items;
	size_t size;
	size_t cap;
};

struct incbin {
	char *path;
	uint16_t address;
};

struct incbin_list {
	struct incbin *items;
	size_t size;
	size_t cap;
};

struct chunk {
	uint16_t address;
	uint8_t *data;
	size_t size;
	char *name;
};

struct chunks {
	struct chunk *items;
	size_t size;
	size_t cap;
};

struct symbol {
	char *name;
	uint16_t value;
};

struct symbols {
	struct symbol *items;
	size_t size;
	size_t cap;
};

struct fixup {
	uint16_t address;
	int addend;
	char *name;
	char *source;
};

struct fixups {
	struct fixup *items;
	size_t size;
	size_t cap;
};

static void die(const char *fmt, ...)
{
	va_list ap;
	fprintf(stderr, "gamecom-link: error: ");
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fputc('\n', stderr);
	exit(1);
}

static void *xmalloc(size_t size)
{
	void *ptr = malloc(size ? size : 1);
	if (!ptr)
		die("out of memory");
	return ptr;
}

static void *xrealloc(void *ptr, size_t size)
{
	void *out = realloc(ptr, size ? size : 1);
	if (!out)
		die("out of memory");
	return out;
}

static char *xstrdup(const char *text)
{
	size_t len = strlen(text);
	char *out = xmalloc(len + 1);
	memcpy(out, text, len + 1);
	return out;
}

static char *xstrndup(const char *text, size_t len)
{
	char *out = xmalloc(len + 1);
	memcpy(out, text, len);
	out[len] = '\0';
	return out;
}

static char *canon_name(const char *name)
{
	char *out = xstrdup(name);
	size_t i;
	for (i = 0; out[i]; i++)
		out[i] = (char)tolower((unsigned char)out[i]);
	return out;
}

static uint16_t rd16(const uint8_t *p)
{
	return (uint16_t)(p[0] | (p[1] << 8));
}

static uint32_t rd32(const uint8_t *p)
{
	return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static struct file_data read_file(const char *path)
{
	FILE *fp;
	long end;
	struct file_data file = { 0 };
	size_t got;
	fp = fopen(path, "rb");
	if (!fp)
		die("cannot open %s: %s", path, strerror(errno));
	if (fseek(fp, 0, SEEK_END) != 0)
		die("cannot seek %s", path);
	end = ftell(fp);
	if (end < 0)
		die("cannot size %s", path);
	if (fseek(fp, 0, SEEK_SET) != 0)
		die("cannot seek %s", path);
	file.data = xmalloc((size_t)end);
	file.size = (size_t)end;
	got = fread(file.data, 1, file.size, fp);
	fclose(fp);
	if (got != file.size)
		die("cannot read %s", path);
	return file;
}

static void write_file(const char *path, const uint8_t *data, size_t size)
{
	FILE *fp = fopen(path, "wb");
	if (!fp)
		die("cannot create %s: %s", path, strerror(errno));
	if (size && fwrite(data, 1, size, fp) != size)
		die("cannot write %s: %s", path, strerror(errno));
	fclose(fp);
}

static long parse_number(const char *text)
{
	char tmp[64];
	char *end = NULL;
	long value;
	size_t len = strlen(text);
	if (!len || len >= sizeof(tmp))
		die("bad number %s", text);
	if (text[0] == '$') {
		if (len == 1 || len - 1 >= sizeof(tmp))
			die("bad number %s", text);
		memcpy(tmp, text + 1, len);
		errno = 0;
		value = strtol(tmp, &end, 16);
	} else {
		memcpy(tmp, text, len + 1);
		if (len > 1 && (tmp[len - 1] == 'h' || tmp[len - 1] == 'H')) {
			tmp[len - 1] = '\0';
			errno = 0;
			value = strtol(tmp, &end, 16);
		} else {
			errno = 0;
			value = strtol(tmp, &end, 0);
		}
	}
	if (errno || end == tmp || *end)
		die("bad number %s", text);
	return value;
}

static uint16_t parse_address(const char *text)
{
	long value = parse_number(text);
	if (value < 0 || value > 0xffff)
		die("address out of range: %s", text);
	return (uint16_t)value;
}

static void input_push(struct input_list *list, const char *path)
{
	if (list->size == list->cap) {
		list->cap = list->cap ? list->cap * 2 : 8;
		list->items = xrealloc(list->items, list->cap * sizeof(list->items[0]));
	}
	list->items[list->size++] = xstrdup(path);
}

static void incbin_push(struct incbin_list *list, const char *spec)
{
	const char *at = strrchr(spec, '@');
	size_t path_len;
	if (!at || at == spec || !at[1])
		die("incbin must be PATH@ADDR");
	path_len = (size_t)(at - spec);
	if (list->size == list->cap) {
		list->cap = list->cap ? list->cap * 2 : 8;
		list->items = xrealloc(list->items, list->cap * sizeof(list->items[0]));
	}
	list->items[list->size].path = xstrndup(spec, path_len);
	list->items[list->size].address = parse_address(at + 1);
	list->size++;
}

static uint32_t chunk_end(const struct chunk *chunk)
{
	return (uint32_t)chunk->address + chunk->size;
}

static int ranges_overlap(uint32_t a0, uint32_t a1, uint32_t b0, uint32_t b1)
{
	return a0 < b1 && b0 < a1;
}

static void chunk_add(struct chunks *chunks, uint16_t address, const uint8_t *data, size_t size, const char *name)
{
	struct chunk *chunk;
	uint32_t end = (uint32_t)address + size;
	size_t i;
	if (end > 0x10000u)
		die("%s does not fit in the 16-bit address space", name);
	for (i = 0; i < chunks->size; i++) {
		if (ranges_overlap(address, end, chunks->items[i].address, chunk_end(&chunks->items[i])))
			die("%s overlaps %s at %04x", name, chunks->items[i].name, address);
	}
	if (chunks->size == chunks->cap) {
		chunks->cap = chunks->cap ? chunks->cap * 2 : 16;
		chunks->items = xrealloc(chunks->items, chunks->cap * sizeof(chunks->items[0]));
	}
	chunk = &chunks->items[chunks->size++];
	chunk->address = address;
	chunk->data = xmalloc(size);
	if (size)
		memcpy(chunk->data, data, size);
	chunk->size = size;
	chunk->name = xstrdup(name);
}

static uint16_t find_free_address(struct chunks *chunks, uint16_t preferred, size_t size)
{
	uint32_t addr = preferred;
	int moved;
	if (size > 0x10000u)
		die("chunk is too large");
	do {
		size_t i;
		moved = 0;
		if (addr + size > 0x10000u)
			die("no free address range for %zu bytes", size);
		for (i = 0; i < chunks->size; i++) {
			uint32_t start = chunks->items[i].address;
			uint32_t end = chunk_end(&chunks->items[i]);
			if (ranges_overlap(addr, addr + size, start, end)) {
				addr = end;
				moved = 1;
				break;
			}
		}
	} while (moved);
	return (uint16_t)addr;
}

static uint16_t highest_end(struct chunks *chunks)
{
	uint32_t max = 0;
	size_t i;
	for (i = 0; i < chunks->size; i++) {
		uint32_t end = chunk_end(&chunks->items[i]);
		if (end > max)
			max = end;
	}
	if (max > 0xffffu)
		return 0xffff;
	return (uint16_t)max;
}

static struct chunk *find_chunk(struct chunks *chunks, uint16_t address, size_t size)
{
	size_t i;
	uint32_t end = (uint32_t)address + size;
	for (i = 0; i < chunks->size; i++) {
		struct chunk *chunk = &chunks->items[i];
		if ((uint32_t)address >= chunk->address && end <= chunk_end(chunk))
			return chunk;
	}
	return NULL;
}

static void symbol_add(struct symbols *symbols, const char *name, uint16_t value)
{
	char *key = canon_name(name);
	size_t i;
	for (i = 0; i < symbols->size; i++) {
		if (strcmp(symbols->items[i].name, key) == 0) {
			if (symbols->items[i].value != value)
				die("duplicate symbol %s", name);
			free(key);
			return;
		}
	}
	if (symbols->size == symbols->cap) {
		symbols->cap = symbols->cap ? symbols->cap * 2 : 128;
		symbols->items = xrealloc(symbols->items, symbols->cap * sizeof(symbols->items[0]));
	}
	symbols->items[symbols->size].name = key;
	symbols->items[symbols->size].value = value;
	symbols->size++;
}

static int symbol_get(struct symbols *symbols, const char *name, uint16_t *value)
{
	char *key = canon_name(name);
	size_t i;
	for (i = 0; i < symbols->size; i++) {
		if (strcmp(symbols->items[i].name, key) == 0) {
			*value = symbols->items[i].value;
			free(key);
			return 1;
		}
	}
	free(key);
	return 0;
}

static void fixup_add(struct fixups *fixups, uint16_t address, const char *name, int addend, const char *source)
{
	if (fixups->size == fixups->cap) {
		fixups->cap = fixups->cap ? fixups->cap * 2 : 64;
		fixups->items = xrealloc(fixups->items, fixups->cap * sizeof(fixups->items[0]));
	}
	fixups->items[fixups->size].address = address;
	fixups->items[fixups->size].addend = addend;
	fixups->items[fixups->size].name = canon_name(name);
	fixups->items[fixups->size].source = xstrdup(source);
	fixups->size++;
}

static char *read_name(const uint8_t *data, size_t size, size_t *pos)
{
	uint16_t len;
	char *name;
	if (*pos + 2 > size)
		die("truncated object name");
	len = rd16(data + *pos);
	*pos += 2;
	if (*pos + len > size)
		die("truncated object name");
	name = xstrndup((const char *)data + *pos, len);
	*pos += len;
	return name;
}

static int is_native_object(const struct file_data *file)
{
	return file->size >= 10 && memcmp(file->data, "GCO1", 4) == 0;
}

static void load_native_object(const char *path, const struct file_data *file, struct chunks *chunks, struct symbols *symbols, struct fixups *fixups, int symbols_only)
{
	size_t pos = 4;
	uint16_t base;
	uint32_t image_size;
	uint16_t export_count;
	uint16_t fixup_count;
	size_t i;
	if (pos + 6 > file->size)
		die("%s is truncated", path);
	base = rd16(file->data + pos);
	pos += 2;
	image_size = rd32(file->data + pos);
	pos += 4;
	if (pos + image_size > file->size)
		die("%s image is truncated", path);
	if (!symbols_only)
		chunk_add(chunks, base, file->data + pos, image_size, path);
	pos += image_size;
	if (pos + 2 > file->size)
		die("%s export table is truncated", path);
	export_count = rd16(file->data + pos);
	pos += 2;
	for (i = 0; i < export_count; i++) {
		char *name = read_name(file->data, file->size, &pos);
		uint16_t value;
		if (pos + 2 > file->size)
			die("%s export value is truncated", path);
		value = rd16(file->data + pos);
		pos += 2;
		symbol_add(symbols, name, value);
		free(name);
	}
	if (pos + 2 > file->size)
		die("%s fixup table is truncated", path);
	fixup_count = rd16(file->data + pos);
	pos += 2;
	for (i = 0; i < fixup_count; i++) {
		uint32_t offset;
		uint8_t type;
		int addend = 0;
		char *name;
		if (pos + 5 > file->size)
			die("%s fixup is truncated", path);
		offset = rd32(file->data + pos);
		pos += 4;
		type = file->data[pos++];
		if (type == 2) {
			if (pos + 2 > file->size)
				die("%s fixup addend is truncated", path);
			addend = (int16_t)rd16(file->data + pos);
			pos += 2;
		}
		name = read_name(file->data, file->size, &pos);
		if (type != 1 && type != 2)
			die("%s has unsupported fixup type %u", path, type);
		if (offset + 1 >= image_size)
			die("%s has a fixup outside its image", path);
		if (!symbols_only)
			fixup_add(fixups, (uint16_t)(base + offset), name, addend, path);
		free(name);
	}
	if (pos != file->size)
		die("%s has trailing object data", path);
}

static int valid_hdk_name(const uint8_t *name)
{
	size_t i;
	int saw_char = 0;
	for (i = 0; i < 16; i++) {
		if (!name[i])
			break;
		if (!isprint(name[i]) || isspace(name[i]))
			return 0;
		saw_char = 1;
	}
	return saw_char;
}

static size_t find_syscall_code_start(const uint8_t *data, size_t size)
{
	static const uint8_t prefix[] = { 0x1e, 0x06, 0xc7, 0x7a, 0x98, 0x20, 0xf1 };
	size_t i;
	for (i = 0; i + sizeof(prefix) <= size; i++) {
		if (memcmp(data + i, prefix, sizeof(prefix)) == 0)
			return i;
	}
	return (size_t)-1;
}

static void load_legacy_hdk_object(const char *path, const struct file_data *file, struct chunks *chunks, struct symbols *symbols)
{
	struct legacy_symbol {
		char *name;
		uint16_t value;
	} *legacy_symbols = NULL;
	size_t legacy_count = 0;
	size_t legacy_cap = 0;
	size_t code_start;
	size_t code_size;
	size_t pos;
	size_t i;
	uint16_t base;
	code_start = find_syscall_code_start(file->data, file->size);
	if (code_start == (size_t)-1)
		die("%s is not a supported native or HDK syscall object", path);
	pos = 0x40;
	while (pos + 22 <= code_start) {
		uint8_t type = file->data[pos];
		uint16_t value = rd16(file->data + pos + 4);
		char *name;
		if (!(type == 0x84 || type == 0x04 || type == 0x0a))
			break;
		if (!valid_hdk_name(file->data + pos + 6))
			break;
		name = xstrndup((const char *)file->data + pos + 6, strnlen((const char *)file->data + pos + 6, 16));
		if (type == 0x84) {
			if (legacy_count == legacy_cap) {
				legacy_cap = legacy_cap ? legacy_cap * 2 : 64;
				legacy_symbols = xrealloc(legacy_symbols, legacy_cap * sizeof(legacy_symbols[0]));
			}
			legacy_symbols[legacy_count].name = name;
			legacy_symbols[legacy_count].value = value;
			legacy_count++;
		} else {
			free(name);
		}
		pos += 22;
	}
	code_size = file->size - code_start;
	base = find_free_address(chunks, highest_end(chunks), code_size);
	chunk_add(chunks, base, file->data + code_start, code_size, path);
	for (i = 0; i < legacy_count; i++) {
		symbol_add(symbols, legacy_symbols[i].name, (uint16_t)(base + legacy_symbols[i].value));
		free(legacy_symbols[i].name);
	}
	free(legacy_symbols);
}

static void add_incbins(struct chunks *chunks, struct incbin_list *incbins)
{
	size_t i;
	for (i = 0; i < incbins->size; i++) {
		struct file_data file = read_file(incbins->items[i].path);
		chunk_add(chunks, incbins->items[i].address, file.data, file.size, incbins->items[i].path);
		free(file.data);
	}
}

static void resolve_fixups(struct chunks *chunks, struct symbols *symbols, struct fixups *fixups)
{
	size_t i;
	for (i = 0; i < fixups->size; i++) {
		struct fixup *fixup = &fixups->items[i];
		struct chunk *chunk = find_chunk(chunks, fixup->address, 2);
		uint16_t value;
		size_t offset;
		if (!chunk)
			die("%s fixup at %04x is outside a linked chunk", fixup->source, fixup->address);
		if (!symbol_get(symbols, fixup->name, &value))
			die("%s has unresolved symbol %s", fixup->source, fixup->name);
		value = (uint16_t)(value + fixup->addend);
		offset = fixup->address - chunk->address;
		chunk->data[offset] = (uint8_t)(value >> 8);
		chunk->data[offset + 1] = (uint8_t)value;
	}
}

static void write_map(const char *path, struct chunks *chunks, struct symbols *symbols, struct fixups *fixups)
{
	FILE *fp;
	size_t i;
	if (!path)
		return;
	fp = fopen(path, "w");
	if (!fp)
		die("cannot create %s: %s", path, strerror(errno));
	fprintf(fp, "chunks\n");
	for (i = 0; i < chunks->size; i++)
		fprintf(fp, "%04x-%04x %s\n", chunks->items[i].address, (unsigned)(chunk_end(&chunks->items[i]) - 1), chunks->items[i].name);
	fprintf(fp, "\nsymbols\n");
	for (i = 0; i < symbols->size; i++)
		fprintf(fp, "%04x %s\n", symbols->items[i].value, symbols->items[i].name);
	fprintf(fp, "\nfixups\n");
	for (i = 0; i < fixups->size; i++)
		fprintf(fp, "%04x %+d %s %s\n", fixups->items[i].address, fixups->items[i].addend, fixups->items[i].name, fixups->items[i].source);
	fclose(fp);
}

static void write_linked_image(const char *path, struct chunks *chunks, int have_base, uint16_t forced_base)
{
	uint32_t min = 0x10000u;
	uint32_t max = 0;
	uint32_t base;
	uint8_t *image;
	size_t image_size;
	size_t i;
	if (!chunks->size)
		die("no input chunks were linked");
	for (i = 0; i < chunks->size; i++) {
		if (chunks->items[i].address < min)
			min = chunks->items[i].address;
		if (chunk_end(&chunks->items[i]) > max)
			max = chunk_end(&chunks->items[i]);
	}
	base = have_base ? forced_base : min;
	if (base > min)
		die("base address %04x is after first linked byte %04x", (unsigned)base, (unsigned)min);
	image_size = (size_t)(max - base);
	image = xmalloc(image_size);
	memset(image, 0, image_size);
	for (i = 0; i < chunks->size; i++)
		memcpy(image + (chunks->items[i].address - base), chunks->items[i].data, chunks->items[i].size);
	write_file(path, image, image_size);
	free(image);
}

static void usage(void)
{
	fprintf(stderr, "usage: gamecom-link INPUT... -o OUTPUT [--base ADDR] [--map MAP] [--incbin PATH@ADDR] [--symbols-only OBJ]\n");
	exit(1);
}

int main(int argc, char **argv)
{
	struct input_list inputs = { 0 };
	struct input_list legacy_inputs = { 0 };
	struct input_list symbol_inputs = { 0 };
	struct incbin_list incbins = { 0 };
	struct chunks chunks = { 0 };
	struct symbols symbols = { 0 };
	struct fixups fixups = { 0 };
	const char *output = NULL;
	const char *map_path = NULL;
	uint16_t forced_base = 0;
	int have_base = 0;
	int i;
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
			if (++i >= argc)
				usage();
			output = argv[i];
		} else if (strcmp(argv[i], "--base") == 0) {
			if (++i >= argc)
				usage();
			forced_base = parse_address(argv[i]);
			have_base = 1;
		} else if (strcmp(argv[i], "--map") == 0) {
			if (++i >= argc)
				usage();
			map_path = argv[i];
		} else if (strcmp(argv[i], "--incbin") == 0) {
			if (++i >= argc)
				usage();
			incbin_push(&incbins, argv[i]);
		} else if (strcmp(argv[i], "--symbols-only") == 0) {
			if (++i >= argc)
				usage();
			input_push(&symbol_inputs, argv[i]);
		} else if (argv[i][0] == '-') {
			usage();
		} else {
			input_push(&inputs, argv[i]);
		}
	}
	if (!output || !inputs.size)
		usage();
	for (i = 0; i < (int)inputs.size; i++) {
		struct file_data file = read_file(inputs.items[i]);
		if (is_native_object(&file)) {
			load_native_object(inputs.items[i], &file, &chunks, &symbols, &fixups, 0);
		} else {
			input_push(&legacy_inputs, inputs.items[i]);
		}
		free(file.data);
	}
	for (i = 0; i < (int)symbol_inputs.size; i++) {
		struct file_data file = read_file(symbol_inputs.items[i]);
		if (!is_native_object(&file))
			die("%s is not a native object", symbol_inputs.items[i]);
		load_native_object(symbol_inputs.items[i], &file, &chunks, &symbols, &fixups, 1);
		free(file.data);
	}
	for (i = 0; i < (int)legacy_inputs.size; i++) {
		struct file_data file = read_file(legacy_inputs.items[i]);
		load_legacy_hdk_object(legacy_inputs.items[i], &file, &chunks, &symbols);
		free(file.data);
	}
	add_incbins(&chunks, &incbins);
	resolve_fixups(&chunks, &symbols, &fixups);
	write_map(map_path, &chunks, &symbols, &fixups);
	write_linked_image(output, &chunks, have_base, forced_base);
	return 0;
}
