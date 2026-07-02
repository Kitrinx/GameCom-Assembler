#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_PROGRAM_OFFSET 0x40000u
#define DEFAULT_BANK_SIZE 0x4000u

struct buffer {
	uint8_t *data;
	size_t size;
};

struct incbin {
	char *path;
	uint32_t offset;
	struct buffer data;
};

struct incbin_list {
	struct incbin *items;
	size_t count;
	size_t capacity;
};

static void fail(const char *message)
{
	fprintf(stderr, "gamecom-pack-hdk: error: %s\n", message);
	exit(1);
}

static void fail_errno(const char *prefix, const char *path)
{
	fprintf(stderr, "gamecom-pack-hdk: error: %s %s: %s\n", prefix, path, strerror(errno));
	exit(1);
}

static unsigned long parse_number(const char *text, unsigned long limit, const char *name)
{
	char *end = NULL;
	unsigned long value;
	errno = 0;
	value = strtoul(text, &end, 0);
	if (errno || end == text || *end != '\0' || value > limit)
		fail(name);
	return value;
}

static char *xstrndup(const char *text, size_t len)
{
	char *out = malloc(len + 1);
	if (!out)
		fail("out of memory");
	memcpy(out, text, len);
	out[len] = '\0';
	return out;
}

static uint32_t checked_end(uint32_t offset, size_t size, const char *message)
{
	if (size > (size_t)(0xffffffffu - offset))
		fail(message);
	return offset + (uint32_t)size;
}

static struct buffer read_file(const char *path)
{
	FILE *fp;
	struct buffer out;
	long end;
	size_t got;

	fp = fopen(path, "rb");
	if (!fp)
		fail_errno("cannot open", path);
	if (fseek(fp, 0, SEEK_END) != 0)
		fail_errno("cannot seek", path);
	end = ftell(fp);
	if (end < 0)
		fail_errno("cannot size", path);
	if (fseek(fp, 0, SEEK_SET) != 0)
		fail_errno("cannot seek", path);
	out.size = (size_t)end;
	out.data = malloc(out.size ? out.size : 1);
	if (!out.data)
		fail("out of memory");
	got = fread(out.data, 1, out.size, fp);
	if (got != out.size)
		fail_errno("cannot read", path);
	fclose(fp);
	return out;
}

static void write_file(const char *path, const uint8_t *data, size_t size)
{
	FILE *fp = fopen(path, "wb");
	if (!fp)
		fail_errno("cannot create", path);
	if (fwrite(data, 1, size, fp) != size)
		fail_errno("cannot write", path);
	if (fclose(fp) != 0)
		fail_errno("cannot close", path);
}

static int has_bin_suffix(const char *name)
{
	size_t len = strlen(name);
	if (len < 4)
		return 0;
	return tolower((unsigned char)name[len - 4]) == '.' &&
		tolower((unsigned char)name[len - 3]) == 'b' &&
		tolower((unsigned char)name[len - 2]) == 'i' &&
		tolower((unsigned char)name[len - 1]) == 'n';
}

static int bank_id_from_name(const char *name)
{
	int seen = 0;
	int value = 0;
	size_t i;
	for (i = 0; name[i] && name[i] != '.'; i++) {
		if (isdigit((unsigned char)name[i])) {
			seen = 1;
			value = value * 10 + (name[i] - '0');
		}
	}
	return seen ? value : -1;
}

static void join_path(char *out, size_t out_size, const char *dir, const char *name)
{
	size_t len = strlen(dir);
	if (len && (dir[len - 1] == '/' || dir[len - 1] == '\\'))
		snprintf(out, out_size, "%s%s", dir, name);
	else
		snprintf(out, out_size, "%s/%s", dir, name);
	if (strlen(out) >= out_size - 1)
		fail("path is too long");
}

static void incbin_push(struct incbin_list *list, const char *spec)
{
	const char *at = strrchr(spec, '@');
	struct incbin item;

	if (!at || at == spec || !at[1])
		fail("bad incbin spec, expected PATH@OFFSET");
	item.path = xstrndup(spec, (size_t)(at - spec));
	item.offset = (uint32_t)parse_number(at + 1, 0xffffffffu, "bad incbin offset");
	item.data.data = NULL;
	item.data.size = 0;
	if (list->count == list->capacity) {
		size_t next_capacity = list->capacity ? list->capacity * 2 : 4;
		struct incbin *next_items = realloc(list->items, next_capacity * sizeof(*next_items));
		if (!next_items)
			fail("out of memory");
		list->items = next_items;
		list->capacity = next_capacity;
	}
	list->items[list->count++] = item;
}

int main(int argc, char **argv)
{
	const char *input = NULL;
	const char *output = NULL;
	const char *hdk_data_path = NULL;
	const char *gfx_dir = NULL;
	uint32_t image_size = 0;
	uint32_t program_offset = DEFAULT_PROGRAM_OFFSET;
	uint8_t fill = 0xff;
	struct buffer asm_data;
	struct buffer hdk_data = { NULL, 0 };
	size_t overlay_len;
	uint32_t needed_size;
	uint8_t *image;
	struct incbin_list incbins = { NULL, 0, 0 };
	int i;
	size_t j;

	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
			if (++i >= argc)
				fail("missing output path");
			output = argv[i];
		} else if (strcmp(argv[i], "--hdk-data") == 0) {
			if (++i >= argc)
				fail("missing HDK data path");
			hdk_data_path = argv[i];
		} else if (strcmp(argv[i], "--gfx-dir") == 0) {
			if (++i >= argc)
				fail("missing gfx directory");
			gfx_dir = argv[i];
		} else if (strcmp(argv[i], "--size") == 0) {
			if (++i >= argc)
				fail("missing image size");
			image_size = (uint32_t)parse_number(argv[i], 0xffffffffu, "bad image size");
		} else if (strcmp(argv[i], "--program-offset") == 0) {
			if (++i >= argc)
				fail("missing program offset");
			program_offset = (uint32_t)parse_number(argv[i], 0xffffffffu, "bad program offset");
		} else if (strcmp(argv[i], "--fill") == 0) {
			if (++i >= argc)
				fail("missing fill byte");
			fill = (uint8_t)parse_number(argv[i], 0xffu, "bad fill byte");
		} else if (strcmp(argv[i], "--incbin") == 0) {
			if (++i >= argc)
				fail("missing incbin spec");
			incbin_push(&incbins, argv[i]);
		} else if (!input) {
			input = argv[i];
		} else {
			fail("unexpected argument");
		}
	}
	if (!input || !output)
		fail("usage: gamecom-pack-hdk INPUT -o OUTPUT [--hdk-data PATH] [--gfx-dir DIR] [--incbin PATH@OFFSET] [--fill BYTE]");
	asm_data = read_file(input);
	if (hdk_data_path)
		hdk_data = read_file(hdk_data_path);
	overlay_len = asm_data.size > hdk_data.size ? asm_data.size : hdk_data.size;
	needed_size = checked_end(program_offset, overlay_len, "program and HDK filler are too large");
	for (j = 0; j < incbins.count; j++) {
		uint32_t end;
		incbins.items[j].data = read_file(incbins.items[j].path);
		end = checked_end(incbins.items[j].offset, incbins.items[j].data.size, "incbin is outside output image");
		if (end > needed_size)
			needed_size = end;
	}
	if (!image_size)
		image_size = (needed_size <= 0x100000u) ? 0x100000u : 0x200000u;
	if (!image_size || program_offset >= image_size || needed_size > image_size)
		fail("program and HDK filler do not fit output image");
	image = malloc(image_size);
	if (!image)
		fail("out of memory");
	memset(image, fill, image_size);
	for (j = 0; j < overlay_len; j++)
		image[program_offset + (uint32_t)j] = (j < asm_data.size) ? asm_data.data[j] : hdk_data.data[j];
	if (gfx_dir) {
		DIR *dir = opendir(gfx_dir);
		struct dirent *ent;
		if (!dir)
			fail_errno("cannot open directory", gfx_dir);
		while ((ent = readdir(dir)) != NULL) {
			int bank;
			uint32_t offset;
			struct buffer data;
			char path[4096];
			if (!has_bin_suffix(ent->d_name))
				continue;
			bank = bank_id_from_name(ent->d_name);
			if (bank < 0)
				continue;
			offset = (uint32_t)bank * DEFAULT_BANK_SIZE;
			if (offset >= image_size)
				fail("bank file is outside output image");
			join_path(path, sizeof(path), gfx_dir, ent->d_name);
			data = read_file(path);
			if (offset + (data.size < DEFAULT_BANK_SIZE ? data.size : DEFAULT_BANK_SIZE) > image_size)
				fail("bank file does not fit output image");
			memcpy(image + offset, data.data, data.size < DEFAULT_BANK_SIZE ? data.size : DEFAULT_BANK_SIZE);
			free(data.data);
		}
		closedir(dir);
	}
	for (j = 0; j < incbins.count; j++)
		memcpy(image + incbins.items[j].offset, incbins.items[j].data.data, incbins.items[j].data.size);
	write_file(output, image, image_size);
	free(asm_data.data);
	free(hdk_data.data);
	for (j = 0; j < incbins.count; j++) {
		free(incbins.items[j].path);
		free(incbins.items[j].data.data);
	}
	free(incbins.items);
	free(image);
	return 0;
}
