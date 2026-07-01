#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HEADER_SIZE 0x20u
#define HEADER_MARKER_OFFSET 0x00u
#define ENTRY_BANK_OFFSET 0x01u
#define ENTRY_ADDR_OFFSET 0x02u
#define FLAGS_OFFSET 0x04u
#define SIGNATURE_OFFSET 0x05u
#define ICON_BANK_OFFSET 0x0eu
#define ICON_LOC_OFFSET 0x0fu
#define TITLE_OFFSET 0x11u
#define TITLE_SIZE 9u
#define PROGRAM_ID_OFFSET 0x1au
#define CHECKSUM_OFFSET 0x1cu
#define SECURITY_SUM 0x5au
#define DEFAULT_HEADER_OFFSET 0x40000u
#define DEFAULT_MIN_SIZE 0x100000u

static const uint8_t signature[] = { 'T', 'i', 'g', 'e', 'r', 'D', 'M', 'G', 'C' };
static const uint32_t security_table[16][3] = {
	{ 0x033e4u, 0x05757u, 0x06666u },
	{ 0x01245u, 0x03505u, 0x04707u },
	{ 0x02267u, 0x0635au, 0x07abcu },
	{ 0x01ac2u, 0x036bbu, 0x084e3u },
	{ 0x04f27u, 0x056e1u, 0x07fdbu },
	{ 0x008a7u, 0x06b41u, 0x05673u },
	{ 0x00245u, 0x033beu, 0x08b6fu },
	{ 0x01743u, 0x05f7eu, 0x06376u },
	{ 0x02875u, 0x03764u, 0x04fd0u },
	{ 0x0230fu, 0x044e7u, 0x067b1u },
	{ 0x02209u, 0x034f1u, 0x03aa8u },
	{ 0x0200du, 0x033c9u, 0x063ecu },
	{ 0x039a7u, 0x05f4bu, 0x06078u },
	{ 0x01327u, 0x0224cu, 0x07086u },
	{ 0x02903u, 0x04f72u, 0x06600u },
	{ 0x01108u, 0x03abbu, 0x0590au },
};

struct buffer {
	uint8_t *data;
	size_t size;
};

static void fail(const char *message)
{
	fprintf(stderr, "gamecom-romtool: error: %s\n", message);
	exit(1);
}

static void fail_errno(const char *prefix, const char *path)
{
	fprintf(stderr, "gamecom-romtool: error: %s %s: %s\n", prefix, path, strerror(errno));
	exit(1);
}

static unsigned long parse_number(const char *text, unsigned long limit, const char *name)
{
	char *end = NULL;
	unsigned long value;
	errno = 0;
	value = strtoul(text, &end, 0);
	if (errno || end == text || *end != '\0')
		fail(name);
	if (value > limit)
		fail(name);
	return value;
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

static uint8_t checksum_for(uint16_t program_id)
{
	return (uint8_t)((((program_id >> 8) + (program_id & 0xffu)) & 0xffu) ^ 0xa5u);
}

static int is_power_of_two(size_t value)
{
	return value && ((value & (value - 1u)) == 0u);
}

static size_t next_power_of_two(size_t value)
{
	size_t out = 1;
	while (out < value) {
		if (out > ((size_t)-1 / 2u))
			fail("requested image is too large");
		out <<= 1;
	}
	return out;
}

static void encode_title(uint8_t *dst, const char *title)
{
	size_t len = strlen(title);
	size_t i;
	if (len > TITLE_SIZE)
		fail("title must be 9 ASCII bytes or shorter");
	for (i = 0; i < TITLE_SIZE; i++) {
		unsigned char ch = (i < len) ? (unsigned char)title[i] : ' ';
		if (ch > 0x7f)
			fail("title must be ASCII");
		dst[i] = ch;
	}
}

static void write_header(uint8_t *image, size_t size, uint32_t header_offset, uint8_t entry_bank,
	uint16_t entry_addr, uint8_t flags, uint8_t icon_bank, uint16_t icon_loc,
	const char *title, uint16_t program_id)
{
	uint8_t checksum = checksum_for(program_id);
	if ((size_t)header_offset + HEADER_SIZE > size)
		fail("header does not fit output image");
	image[header_offset + HEADER_MARKER_OFFSET] = 0x08;
	image[header_offset + ENTRY_BANK_OFFSET] = entry_bank;
	image[header_offset + ENTRY_ADDR_OFFSET] = (uint8_t)(entry_addr >> 8);
	image[header_offset + ENTRY_ADDR_OFFSET + 1u] = (uint8_t)entry_addr;
	image[header_offset + FLAGS_OFFSET] = flags;
	memcpy(image + header_offset + SIGNATURE_OFFSET, signature, sizeof(signature));
	image[header_offset + ICON_BANK_OFFSET] = icon_bank;
	image[header_offset + ICON_LOC_OFFSET] = (uint8_t)(icon_loc >> 8);
	image[header_offset + ICON_LOC_OFFSET + 1u] = (uint8_t)icon_loc;
	encode_title(image + header_offset + TITLE_OFFSET, title);
	image[header_offset + PROGRAM_ID_OFFSET] = (uint8_t)(program_id >> 8);
	image[header_offset + PROGRAM_ID_OFFSET + 1u] = (uint8_t)program_id;
	image[header_offset + CHECKSUM_OFFSET] = checksum;
	memset(image + header_offset + CHECKSUM_OFFSET + 1u, 0, HEADER_SIZE - CHECKSUM_OFFSET - 1u);
}

static void patch_security_one(uint8_t *image, size_t size, uint32_t root, uint8_t checksum,
	uint32_t raw_offset, uint32_t raw_end, int allow_overwrite)
{
	unsigned index = checksum & 0x0fu;
	uint32_t a0 = root + security_table[index][0];
	uint32_t a1 = root + security_table[index][1];
	uint32_t a2 = root + security_table[index][2];
	uint8_t value;
	if ((size_t)a0 >= size || (size_t)a1 >= size || (size_t)a2 >= size)
		fail("security triplet does not fit output image");
	value = (uint8_t)(SECURITY_SUM - image[a0] - image[a1]);
	if (a2 >= raw_offset && a2 < raw_end && image[a2] != value && !allow_overwrite)
		fail("security patch would overwrite raw payload; change program id or pass --allow-security-overwrite");
	image[a2] = value;
	if (((image[a0] + image[a1] + image[a2]) & 0xffu) != SECURITY_SUM)
		fail("internal security patch failure");
}

static uint32_t max_security_end(uint32_t header_offset, const char *mode, uint8_t checksum)
{
	unsigned index = checksum & 0x0fu;
	uint32_t maxoff = security_table[index][0];
	uint32_t end_abs;
	uint32_t end_header;
	if (security_table[index][1] > maxoff)
		maxoff = security_table[index][1];
	if (security_table[index][2] > maxoff)
		maxoff = security_table[index][2];
	end_abs = maxoff + 1u;
	end_header = header_offset + maxoff + 1u;
	if (strcmp(mode, "absolute") == 0)
		return end_abs;
	if (strcmp(mode, "header") == 0)
		return end_header;
	if (strcmp(mode, "both") == 0)
		return end_header > end_abs ? end_header : end_abs;
	fail("security root must be absolute, header, or both");
	return 0;
}

static uint32_t find_header(const uint8_t *image, size_t size, int have_offset, uint32_t requested)
{
	uint32_t candidates[2] = { 0, DEFAULT_HEADER_OFFSET };
	size_t count = have_offset ? 1u : 2u;
	size_t i;
	if (have_offset)
		candidates[0] = requested;
	for (i = 0; i < count; i++) {
		uint32_t off = candidates[i];
		if ((size_t)off + HEADER_SIZE <= size &&
			memcmp(image + off + SIGNATURE_OFFSET, signature, sizeof(signature)) == 0)
			return off;
	}
	fail("no Game.com header signature found");
	return 0;
}

static unsigned security_valid_at(const uint8_t *image, size_t size, uint32_t root, uint8_t checksum)
{
	unsigned index = checksum & 0x0fu;
	uint32_t a0 = root + security_table[index][0];
	uint32_t a1 = root + security_table[index][1];
	uint32_t a2 = root + security_table[index][2];
	if ((size_t)a0 >= size || (size_t)a1 >= size || (size_t)a2 >= size)
		return 0;
	return (((image[a0] + image[a1] + image[a2]) & 0xffu) == SECURITY_SUM);
}

static int command_build(int argc, char **argv)
{
	const char *input = NULL;
	const char *output = NULL;
	const char *title = "GAMECOM  ";
	const char *security_root = "both";
	uint32_t header_offset = DEFAULT_HEADER_OFFSET;
	uint32_t raw_offset = DEFAULT_HEADER_OFFSET;
	int raw_offset_set = 0;
	size_t min_size = DEFAULT_MIN_SIZE;
	uint8_t fill = 0xff;
	uint8_t entry_bank = 0x20;
	uint16_t entry_addr = 0x4020;
	uint8_t flags = 0x03;
	uint8_t icon_bank = 0x00;
	uint16_t icon_loc = 0;
	uint16_t program_id = 0x1a18;
	int allow_overwrite = 0;
	int quiet = 0;
	struct buffer raw;
	uint8_t checksum;
	size_t required;
	size_t image_size;
	uint8_t *image;
	int i;

	for (i = 2; i < argc; i++) {
		if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
			if (++i >= argc)
				fail("missing output path");
			output = argv[i];
		} else if (strcmp(argv[i], "--header-offset") == 0) {
			if (++i >= argc)
				fail("missing header offset");
			header_offset = (uint32_t)parse_number(argv[i], 0xffffffffu, "bad header offset");
			if (!raw_offset_set)
				raw_offset = header_offset;
		} else if (strcmp(argv[i], "--raw-offset") == 0) {
			if (++i >= argc)
				fail("missing raw offset");
			raw_offset = (uint32_t)parse_number(argv[i], 0xffffffffu, "bad raw offset");
			raw_offset_set = 1;
		} else if (strcmp(argv[i], "--min-size") == 0) {
			if (++i >= argc)
				fail("missing minimum size");
			min_size = (size_t)parse_number(argv[i], 0xffffffffu, "bad minimum size");
		} else if (strcmp(argv[i], "--fill") == 0) {
			if (++i >= argc)
				fail("missing fill byte");
			fill = (uint8_t)parse_number(argv[i], 0xffu, "bad fill byte");
		} else if (strcmp(argv[i], "--entry-bank") == 0) {
			if (++i >= argc)
				fail("missing entry bank");
			entry_bank = (uint8_t)parse_number(argv[i], 0xffu, "bad entry bank");
		} else if (strcmp(argv[i], "--entry-addr") == 0) {
			if (++i >= argc)
				fail("missing entry address");
			entry_addr = (uint16_t)parse_number(argv[i], 0xffffu, "bad entry address");
		} else if (strcmp(argv[i], "--flags") == 0) {
			if (++i >= argc)
				fail("missing flags");
			flags = (uint8_t)parse_number(argv[i], 0xffu, "bad flags");
		} else if (strcmp(argv[i], "--icon-bank") == 0) {
			if (++i >= argc)
				fail("missing icon bank");
			icon_bank = (uint8_t)parse_number(argv[i], 0xffu, "bad icon bank");
		} else if (strcmp(argv[i], "--icon-loc") == 0) {
			if (++i >= argc)
				fail("missing icon location");
			icon_loc = (uint16_t)parse_number(argv[i], 0xffffu, "bad icon location");
		} else if (strcmp(argv[i], "--title") == 0) {
			if (++i >= argc)
				fail("missing title");
			title = argv[i];
		} else if (strcmp(argv[i], "--program-id") == 0) {
			if (++i >= argc)
				fail("missing program id");
			program_id = (uint16_t)parse_number(argv[i], 0xffffu, "bad program id");
		} else if (strcmp(argv[i], "--security-root") == 0) {
			if (++i >= argc)
				fail("missing security root");
			security_root = argv[i];
		} else if (strcmp(argv[i], "--allow-security-overwrite") == 0) {
			allow_overwrite = 1;
		} else if (strcmp(argv[i], "--quiet") == 0) {
			quiet = 1;
		} else if (!input) {
			input = argv[i];
		} else {
			fail("unexpected argument");
		}
	}
	if (!input || !output)
		fail("usage: gamecom-romtool build INPUT -o OUTPUT [options]");
	raw = read_file(input);
	if (!raw.size)
		fail("raw input is empty");
	checksum = checksum_for(program_id);
	required = raw_offset + raw.size;
	if (required < (size_t)header_offset + HEADER_SIZE)
		required = (size_t)header_offset + HEADER_SIZE;
	if (required < max_security_end(header_offset, security_root, checksum))
		required = max_security_end(header_offset, security_root, checksum);
	if (required < min_size)
		required = min_size;
	image_size = next_power_of_two(required);
	image = malloc(image_size);
	if (!image)
		fail("out of memory");
	memset(image, fill, image_size);
	memcpy(image + raw_offset, raw.data, raw.size);
	write_header(image, image_size, header_offset, entry_bank, entry_addr, flags, icon_bank, icon_loc, title, program_id);
	if (strcmp(security_root, "absolute") == 0 || strcmp(security_root, "both") == 0)
		patch_security_one(image, image_size, 0, checksum, raw_offset, raw_offset + (uint32_t)raw.size, allow_overwrite);
	if ((strcmp(security_root, "header") == 0 || strcmp(security_root, "both") == 0) && header_offset != 0)
		patch_security_one(image, image_size, header_offset, checksum, raw_offset, raw_offset + (uint32_t)raw.size, allow_overwrite);
	write_file(output, image, image_size);
	if (!quiet) {
		printf("Wrote %s (%zu bytes), header=0x%05x, entry=%02x:%04x, title='", output, image_size,
			header_offset, entry_bank, entry_addr);
		fwrite(image + header_offset + TITLE_OFFSET, 1, TITLE_SIZE, stdout);
		printf("', program_id=0x%04x, checksum=0x%02x.\n", program_id, checksum);
	}
	free(raw.data);
	free(image);
	return 0;
}

static int command_verify(int argc, char **argv)
{
	const char *input = NULL;
	const char *require_root = "any";
	uint32_t requested = 0;
	int have_offset = 0;
	int quiet = 0;
	struct buffer image;
	uint32_t header;
	uint16_t program_id;
	uint8_t checksum;
	int checksum_ok;
	unsigned abs_ok;
	unsigned header_ok;
	int i;

	for (i = 2; i < argc; i++) {
		if (strcmp(argv[i], "--header-offset") == 0) {
			if (++i >= argc)
				fail("missing header offset");
			requested = (uint32_t)parse_number(argv[i], 0xffffffffu, "bad header offset");
			have_offset = 1;
		} else if (strcmp(argv[i], "--require-root") == 0) {
			if (++i >= argc)
				fail("missing required root");
			require_root = argv[i];
		} else if (strcmp(argv[i], "--quiet") == 0) {
			quiet = 1;
		} else if (!input) {
			input = argv[i];
		} else {
			fail("unexpected argument");
		}
	}
	if (!input)
		fail("usage: gamecom-romtool verify INPUT [options]");
	image = read_file(input);
	if (!image.size || !is_power_of_two(image.size))
		fail("ROM size is not a nonzero power of two");
	header = find_header(image.data, image.size, have_offset, requested);
	program_id = (uint16_t)((image.data[header + PROGRAM_ID_OFFSET] << 8) | image.data[header + PROGRAM_ID_OFFSET + 1u]);
	checksum = image.data[header + CHECKSUM_OFFSET];
	checksum_ok = (checksum == checksum_for(program_id));
	abs_ok = security_valid_at(image.data, image.size, 0, checksum);
	header_ok = security_valid_at(image.data, image.size, header, checksum);
	if (!quiet) {
		printf("%s: header=0x%05x, entry=%02x:%04x, title='", input, header,
			image.data[header + ENTRY_BANK_OFFSET],
			(image.data[header + ENTRY_ADDR_OFFSET] << 8) | image.data[header + ENTRY_ADDR_OFFSET + 1u]);
		fwrite(image.data + header + TITLE_OFFSET, 1, TITLE_SIZE, stdout);
		printf("', program_id=0x%04x, checksum=0x%02x\n", program_id, checksum);
		printf("checksum_valid=%s\n", checksum_ok ? "true" : "false");
		printf("security_absolute_valid=%s\n", abs_ok ? "true" : "false");
		printf("security_header_valid=%s\n", header_ok ? "true" : "false");
	}
	if (!checksum_ok)
		fail("ROM verification failed");
	if ((strcmp(require_root, "any") == 0 && !(abs_ok || header_ok)) ||
		(strcmp(require_root, "absolute") == 0 && !abs_ok) ||
		(strcmp(require_root, "header") == 0 && !header_ok) ||
		(strcmp(require_root, "both") == 0 && !(abs_ok && header_ok)))
		fail("ROM verification failed");
	if (strcmp(require_root, "any") && strcmp(require_root, "absolute") && strcmp(require_root, "header") && strcmp(require_root, "both"))
		fail("required root must be any, absolute, header, or both");
	free(image.data);
	return 0;
}

int main(int argc, char **argv)
{
	if (argc < 2)
		fail("usage: gamecom-romtool build|verify ...");
	if (strcmp(argv[1], "build") == 0)
		return command_build(argc, argv);
	if (strcmp(argv[1], "verify") == 0)
		return command_verify(argc, argv);
	fail("unknown command");
	return 1;
}
