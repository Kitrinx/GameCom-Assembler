#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

struct bytes {
	uint8_t *data;
	size_t size;
	size_t cap;
};

struct line {
	char *path;
	int line_no;
	char *raw;
	char *label;
	char *op;
	char **operands;
	int operand_count;
	uint16_t address;
	size_t span;
	struct bytes out;
};

struct lines {
	struct line *items;
	size_t size;
	size_t cap;
};

struct symbol {
	char *name;
	int value;
};

struct symbols {
	struct symbol *items;
	size_t size;
	size_t cap;
};

struct assembler {
	struct lines lines;
	struct symbols symbols;
	const char *root_path;
	int case_sensitive;
	int resolve_pass;
	char error[512];
};

struct memref {
	int valid;
	int mode;
	int reg;
	int offset;
	int width;
};

struct bitmem {
	int valid;
	int base;
	int offset;
};

struct expr {
	struct assembler *as;
	const char *text;
	size_t pos;
	int pc;
	int ok;
};

static const char *cond_names[] = {
	"f", "lt", "le", "ule", "ov", "mi", "z", "c",
	"t", "ge", "gt", "ugt", "nov", "pl", "nz", "nc",
};

static const char *no_operand_names[] = {
	"stop", "halt", "ret", "iret", "clrc", "comc", "setc", "ei", "di", "nop",
};

static const uint8_t no_operand_values[] = {
	0xf0, 0xf1, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
};

static const char *unary_names[] = {
	"clr", "neg", "com", "rr", "rl", "rrc", "rlc", "srl", "inc", "dec", "sra", "sll", "da", "swap",
};

static const char *byte_alu_names[] = {
	"cmp", "add", "sub", "adc", "sbc", "and", "or", "xor",
};

static const char *word_alu_names[] = {
	"cmpw", "addw", "subw", "adcw", "sbcw", "andw", "orw", "xorw",
};

static const struct {
	const char *name;
	int value;
} predefined[] = {
	{ "PS0", 0x1d }, { "PS1", 0x1e }, { "SYS", 0x19 }, { "IE0", 0x10 }, { "IE1", 0x11 },
	{ "IR0", 0x12 }, { "IR1", 0x13 }, { "P0", 0x14 }, { "P1", 0x15 }, { "P2", 0x16 },
	{ "P0C", 0x20 }, { "P1C", 0x21 }, { "P2C", 0x22 }, { "LCC", 0x30 }, { "LCDC", 0x30 },
	{ "LCH", 0x31 }, { "LCV", 0x32 }, { "DMC", 0x34 }, { "DMX1", 0x35 }, { "DMY1", 0x36 },
	{ "DMDX", 0x37 }, { "DMDY", 0x38 }, { "DMX2", 0x39 }, { "DMY2", 0x3a }, { "DMPL", 0x3b },
	{ "DMBR", 0x3c }, { "DMVP", 0x3d }, { "SGC", 0x40 }, { "SG0L", 0x42 }, { "SG1L", 0x44 },
	{ "SG0TH", 0x46 }, { "SG0TL", 0x47 }, { "SG1TH", 0x48 }, { "SG1TL", 0x49 },
	{ "SG2L", 0x4a }, { "SG2TH", 0x4c }, { "SG2TL", 0x4d }, { "SGDA", 0x4e },
	{ "TM0C", 0x50 }, { "TM0D", 0x51 }, { "TM1C", 0x52 }, { "TM1D", 0x53 },
	{ "CLKT", 0x54 }, { "WDT", 0x5e }, { "WDTC", 0x5f },
	{ "DMA_VECTOR", 0x1000 }, { "TIM0_VECTOR", 0x1002 }, { "EXT_VECTOR", 0x1006 },
	{ "UART_VECTOR", 0x1008 }, { "LCDC_VECTOR", 0x100e }, { "TIM1_VECTOR", 0x1012 },
	{ "CK_VECTOR", 0x1016 }, { "PIO_VECTOR", 0x101a }, { "WDT_VECTOR", 0x101c },
	{ "NMI_VECTOR", 0x101e }, { "BOOT_ENTRY", 0x1020 },
};

static void set_error(struct assembler *as, const char *message)
{
	if (!as->error[0])
		snprintf(as->error, sizeof(as->error), "%s", message);
}

static void set_errorf(struct assembler *as, const char *fmt, const char *arg)
{
	if (!as->error[0])
		snprintf(as->error, sizeof(as->error), fmt, arg);
}

static void *xmalloc(size_t size)
{
	void *ptr = malloc(size ? size : 1);
	if (!ptr) {
		fprintf(stderr, "gamecom-as: error: out of memory\n");
		exit(1);
	}
	return ptr;
}

static void *xrealloc(void *ptr, size_t size)
{
	void *out = realloc(ptr, size ? size : 1);
	if (!out) {
		fprintf(stderr, "gamecom-as: error: out of memory\n");
		exit(1);
	}
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

static char *trim_owned(char *text)
{
	char *start = text;
	char *end;
	while (isspace((unsigned char)*start))
		start++;
	end = start + strlen(start);
	while (end > start && isspace((unsigned char)end[-1]))
		*--end = '\0';
	if (start != text)
		memmove(text, start, strlen(start) + 1);
	return text;
}

static char *trim_copy_range(const char *text, size_t len)
{
	char *out = xstrndup(text, len);
	return trim_owned(out);
}

static int operand_size_hint(const char *text)
{
	char *t = trim_copy_range(text, strlen(text));
	size_t len = strlen(t);
	int hint = 0;
	if (len >= 2 && t[len - 2] == '.') {
		int ch = tolower((unsigned char)t[len - 1]);
		if (ch == 'b')
			hint = 8;
		else if (ch == 'w')
			hint = 16;
	}
	free(t);
	return hint;
}

static char *strip_operand_size_hint_copy(const char *text)
{
	char *t = trim_copy_range(text, strlen(text));
	size_t len = strlen(t);
	if (len >= 2 && t[len - 2] == '.') {
		int ch = tolower((unsigned char)t[len - 1]);
		if (ch == 'b' || ch == 'w') {
			t[len - 2] = '\0';
			trim_owned(t);
		}
	}
	return t;
}

static char *canon_name(struct assembler *as, const char *name)
{
	char *out = xstrdup(name);
	size_t i;
	if (!as->case_sensitive) {
		for (i = 0; out[i]; i++)
			out[i] = (char)tolower((unsigned char)out[i]);
	}
	return out;
}

static int str_eq_lit(const char *a, const char *b)
{
	while (*a && *b) {
		if (tolower((unsigned char)*a) != tolower((unsigned char)*b))
			return 0;
		a++;
		b++;
	}
	return *a == *b;
}

static void bytes_clear(struct bytes *b)
{
	free(b->data);
	b->data = NULL;
	b->size = 0;
	b->cap = 0;
}

static void bytes_emit(struct bytes *b, int value)
{
	if (b->size == b->cap) {
		b->cap = b->cap ? b->cap * 2 : 16;
		b->data = xrealloc(b->data, b->cap);
	}
	b->data[b->size++] = (uint8_t)value;
}

static void bytes_emit_word(struct bytes *b, int value)
{
	bytes_emit(b, (value >> 8) & 0xff);
	bytes_emit(b, value & 0xff);
}

static void lines_push(struct lines *lines, struct line line)
{
	if (lines->size == lines->cap) {
		lines->cap = lines->cap ? lines->cap * 2 : 256;
		lines->items = xrealloc(lines->items, lines->cap * sizeof(lines->items[0]));
	}
	lines->items[lines->size++] = line;
}

static void symbol_set(struct assembler *as, const char *name, int value)
{
	char *key = canon_name(as, name);
	size_t i;
	for (i = 0; i < as->symbols.size; i++) {
		if (strcmp(as->symbols.items[i].name, key) == 0) {
			as->symbols.items[i].value = value;
			free(key);
			return;
		}
	}
	if (as->symbols.size == as->symbols.cap) {
		as->symbols.cap = as->symbols.cap ? as->symbols.cap * 2 : 256;
		as->symbols.items = xrealloc(as->symbols.items, as->symbols.cap * sizeof(as->symbols.items[0]));
	}
	as->symbols.items[as->symbols.size].name = key;
	as->symbols.items[as->symbols.size].value = value;
	as->symbols.size++;
}

static int symbol_get(struct assembler *as, const char *name, int *value)
{
	char *key = canon_name(as, name);
	size_t i;
	for (i = 0; i < as->symbols.size; i++) {
		if (strcmp(as->symbols.items[i].name, key) == 0) {
			*value = as->symbols.items[i].value;
			free(key);
			return 1;
		}
	}
	free(key);
	return 0;
}

static void init_symbols(struct assembler *as)
{
	size_t i;
	char name[16];
	for (i = 0; i < sizeof(predefined) / sizeof(predefined[0]); i++)
		symbol_set(as, predefined[i].name, predefined[i].value);
	for (i = 0; i < 16; i++) {
		snprintf(name, sizeof(name), "SG0W%zu", i);
		symbol_set(as, name, 0x60 + (int)i);
		snprintf(name, sizeof(name), "SG1W%zu", i);
		symbol_set(as, name, 0x70 + (int)i);
	}
}

static int is_symbol_name(const char *text)
{
	size_t i;
	if (!(isalpha((unsigned char)text[0]) || text[0] == '_' || text[0] == '.'))
		return 0;
	for (i = 1; text[i]; i++) {
		if (!(isalnum((unsigned char)text[i]) || text[i] == '_' || text[i] == '.'))
			return 0;
	}
	return 1;
}

static int list_index(const char *name, const char **list, size_t count)
{
	size_t i;
	for (i = 0; i < count; i++) {
		if (str_eq_lit(name, list[i]))
			return (int)i;
	}
	return -1;
}

static int is_directive(const char *name)
{
	static const char *directives[] = {
		"org", ".org", "base", ".base", "db", "defb", "defm", "dm", ".byte", "byte",
		"dw", "defw", ".word", "word", "ds", "defs", ".space", ".res", "fill", ".fill",
		"align", ".align", "equ", ".equ", "set", ".set", "include", "incbin", "end",
		"data", "title", "type", "program", "list", "nlist", "global", "public", "extrn",
		"extern", "eject", "page", "if", "else", "endif", "error",
	};
	return list_index(name, directives, sizeof(directives) / sizeof(directives[0])) >= 0;
}

static int is_mnemonic(const char *name)
{
	static const char *extra[] = {
		"push", "pop", "incw", "decw", "pushw", "popw", "exts", "dm", "btst",
		"mov", "movw", "bclr", "bset", "bbc", "bbs", "bmov", "bcmp", "band",
		"bor", "bxor", "mult", "div", "movm", "dbnz", "br", "jmp", "call",
		"cals", "invalid", "illegal", "ill",
	};
	if (list_index(name, unary_names, sizeof(unary_names) / sizeof(unary_names[0])) >= 0)
		return 1;
	if (list_index(name, byte_alu_names, sizeof(byte_alu_names) / sizeof(byte_alu_names[0])) >= 0)
		return 1;
	if (list_index(name, word_alu_names, sizeof(word_alu_names) / sizeof(word_alu_names[0])) >= 0)
		return 1;
	if (list_index(name, no_operand_names, sizeof(no_operand_names) / sizeof(no_operand_names[0])) >= 0)
		return 1;
	return list_index(name, extra, sizeof(extra) / sizeof(extra[0])) >= 0;
}

static size_t find_top_level_char(const char *text, char target)
{
	int depth = 0;
	char quote = 0;
	int escape = 0;
	size_t i;
	for (i = 0; text[i]; i++) {
		char ch = text[i];
		if (quote) {
			if (escape)
				escape = 0;
			else if (ch == '\\')
				escape = 1;
			else if (ch == quote)
				quote = 0;
			continue;
		}
		if (ch == target && depth == 0)
			return i;
		if (ch == '\'' || ch == '"')
			quote = ch;
		else if (ch == '(')
			depth++;
		else if (ch == ')' && depth > 0)
			depth--;
	}
	return (size_t)-1;
}

static char *strip_comment(const char *raw)
{
	char quote = 0;
	int escape = 0;
	size_t i;
	for (i = 0; raw[i]; i++) {
		char ch = raw[i];
		if (quote) {
			if (escape)
				escape = 0;
			else if (ch == '\\')
				escape = 1;
			else if (ch == quote)
				quote = 0;
			continue;
		}
		if (ch == '\'' || ch == '"')
			quote = ch;
		else if (ch == ';')
			return trim_copy_range(raw, i);
		else if (ch == '/' && raw[i + 1] == '/')
			return trim_copy_range(raw, i);
	}
	return trim_copy_range(raw, strlen(raw));
}

static void split_head(const char *text, char **head, char **rest)
{
	size_t i = 0;
	while (text[i] && !isspace((unsigned char)text[i]))
		i++;
	*head = xstrndup(text, i);
	while (isspace((unsigned char)text[i]))
		i++;
	*rest = xstrdup(text + i);
}

static char **split_operands(const char *text, int *count)
{
	char **ops = NULL;
	int op_count = 0;
	int cap = 0;
	int depth = 0;
	char quote = 0;
	int escape = 0;
	size_t start = 0;
	size_t i;
	for (i = 0;; i++) {
		char ch = text[i];
		if (quote) {
			if (escape)
				escape = 0;
			else if (ch == '\\')
				escape = 1;
			else if (ch == quote)
				quote = 0;
		} else if (ch == '\'' || ch == '"') {
			quote = ch;
		} else if (ch == '(') {
			depth++;
		} else if (ch == ')' && depth > 0) {
			depth--;
		}
		if ((ch == ',' && depth == 0 && !quote) || ch == '\0') {
			char *item = trim_copy_range(text + start, i - start);
			if (item[0]) {
				if (op_count == cap) {
					cap = cap ? cap * 2 : 4;
					ops = xrealloc(ops, (size_t)cap * sizeof(ops[0]));
				}
				ops[op_count++] = item;
			} else {
				free(item);
			}
			start = i + 1;
		}
		if (ch == '\0')
			break;
	}
	*count = op_count;
	return ops;
}

static char *dirname_copy(const char *path)
{
	const char *slash = strrchr(path, '/');
	if (!slash)
		return xstrdup(".");
	return xstrndup(path, (size_t)(slash - path));
}

static int is_absolute_path(const char *path)
{
	return path[0] == '/';
}

static char *join_path(const char *dir, const char *name)
{
	size_t len = strlen(dir);
	size_t need = len + strlen(name) + 2;
	char *out = xmalloc(need);
	if (is_absolute_path(name)) {
		snprintf(out, need, "%s", name);
	} else if (len && dir[len - 1] == '/') {
		snprintf(out, need, "%s%s", dir, name);
	} else {
		snprintf(out, need, "%s/%s", dir, name);
	}
	return out;
}

static char *decode_string_token(const char *text)
{
	size_t len = strlen(text);
	char quote;
	char *out;
	size_t i;
	size_t o = 0;
	if (len < 2 || !((text[0] == '\'' && text[len - 1] == '\'') || (text[0] == '"' && text[len - 1] == '"')))
		return NULL;
	quote = text[0];
	(void)quote;
	out = xmalloc(len);
	for (i = 1; i + 1 < len; i++) {
		if (text[i] == '\\' && i + 1 < len - 1) {
			i++;
			switch (text[i]) {
			case 'n': out[o++] = '\n'; break;
			case 'r': out[o++] = '\r'; break;
			case 't': out[o++] = '\t'; break;
			case '0': out[o++] = '\0'; break;
			default: out[o++] = text[i]; break;
			}
		} else {
			out[o++] = text[i];
		}
	}
	out[o] = '\0';
	return out;
}

static char *string_or_bare(const char *text)
{
	char *decoded = decode_string_token(text);
	if (decoded)
		return decoded;
	return xstrdup(text);
}

static void parse_line(struct assembler *as, const char *path, int line_no, const char *raw)
{
	struct line line;
	char *text = strip_comment(raw);
	char *head = NULL;
	char *rest = NULL;
	size_t colon;
	memset(&line, 0, sizeof(line));
	line.path = xstrdup(path);
	line.line_no = line_no;
	line.raw = xstrdup(raw);
	if (!text[0]) {
		free(text);
		lines_push(&as->lines, line);
		return;
	}
	colon = find_top_level_char(text, ':');
	if (colon != (size_t)-1 && colon > 0) {
		char *candidate = trim_copy_range(text, colon);
		if (is_symbol_name(candidate)) {
			line.label = candidate;
			memmove(text, text + colon + 1, strlen(text + colon + 1) + 1);
			trim_owned(text);
		} else {
			free(candidate);
		}
	}
	if (!text[0]) {
		free(text);
		lines_push(&as->lines, line);
		return;
	}
	split_head(text, &head, &rest);
	if (!line.label && rest[0]) {
		char *rest_head = NULL;
		char *rest_tail = NULL;
		split_head(rest, &rest_head, &rest_tail);
		if (is_symbol_name(head) && (is_directive(rest_head) || is_mnemonic(rest_head))) {
			line.label = head;
			head = rest_head;
			free(rest);
			rest = rest_tail;
		} else {
			free(rest_head);
			free(rest_tail);
		}
	}
	line.op = head;
	line.operands = split_operands(rest, &line.operand_count);
	free(rest);
	free(text);
	lines_push(&as->lines, line);
}

static char *read_text_file(const char *path)
{
	FILE *fp;
	long end;
	char *text;
	size_t got;
	fp = fopen(path, "rb");
	if (!fp)
		return NULL;
	if (fseek(fp, 0, SEEK_END) != 0) {
		fclose(fp);
		return NULL;
	}
	end = ftell(fp);
	if (end < 0) {
		fclose(fp);
		return NULL;
	}
	if (fseek(fp, 0, SEEK_SET) != 0) {
		fclose(fp);
		return NULL;
	}
	text = xmalloc((size_t)end + 1);
	got = fread(text, 1, (size_t)end, fp);
	fclose(fp);
	if (got != (size_t)end) {
		free(text);
		return NULL;
	}
	text[got] = '\0';
	return text;
}

static int load_lines(struct assembler *as, const char *path);

static void parse_loaded_text(struct assembler *as, const char *path, char *text)
{
	char *cursor = text;
	int line_no = 1;
	while (*cursor) {
		char *eol = cursor;
		char saved;
		while (*eol && *eol != '\n' && *eol != '\r')
			eol++;
		saved = *eol;
		*eol = '\0';
		{
			char *stripped = strip_comment(cursor);
			char *head = NULL;
			char *rest = NULL;
			int stop_file = 0;
			split_head(stripped, &head, &rest);
			if (str_eq_lit(head, "include") && rest[0]) {
				int count = 0;
				char **ops = split_operands(rest, &count);
				if (count > 0) {
					char *inc = string_or_bare(ops[0]);
					char *dir = dirname_copy(path);
					char *inc_path = join_path(dir, inc);
					int loaded = load_lines(as, inc_path);
					if (!loaded && !as->error[0] && strcmp(inc_path, inc) != 0)
						loaded = load_lines(as, inc);
					if (!loaded && !as->error[0])
						set_errorf(as, "cannot include %s", inc_path);
					free(inc_path);
					free(dir);
					free(inc);
				}
				while (count--)
					free(ops[count]);
				free(ops);
			} else {
				parse_line(as, path, line_no, cursor);
				if (str_eq_lit(head, "end"))
					stop_file = 1;
			}
			free(head);
			free(rest);
			free(stripped);
			if (stop_file)
				break;
		}
		if (saved == '\r' && eol[1] == '\n')
			eol++;
		cursor = saved ? eol + 1 : eol;
		line_no++;
	}
}

static int load_lines(struct assembler *as, const char *path)
{
	char *text = read_text_file(path);
	if (!text)
		return 0;
	parse_loaded_text(as, path, text);
	free(text);
	return !as->error[0];
}

static void expr_skip(struct expr *ex)
{
	while (isspace((unsigned char)ex->text[ex->pos]))
		ex->pos++;
}

static int parse_expr_or(struct expr *ex);

static int is_hex_suffix_token(const char *start, size_t len)
{
	size_t i;
	if (len < 2)
		return 0;
	if (start[len - 1] != 'h' && start[len - 1] != 'H')
		return 0;
	for (i = 0; i + 1 < len; i++) {
		if (!isxdigit((unsigned char)start[i]))
			return 0;
	}
	return 1;
}

static int parse_based_digits(struct expr *ex, const char *start, size_t len, int base)
{
	char tmp[128];
	char *end = NULL;
	long value;
	if (len >= sizeof(tmp)) {
		ex->ok = 0;
		set_error(ex->as, "numeric literal is too long");
		return 0;
	}
	memcpy(tmp, start, len);
	tmp[len] = '\0';
	errno = 0;
	value = strtol(tmp, &end, base);
	if (errno || end == tmp || *end != '\0') {
		ex->ok = 0;
		set_error(ex->as, "bad numeric literal");
		return 0;
	}
	return (int)value;
}

static int parse_primary(struct expr *ex)
{
	const char *s = ex->text;
	expr_skip(ex);
	if (s[ex->pos] == '(') {
		int value;
		ex->pos++;
		value = parse_expr_or(ex);
		expr_skip(ex);
		if (s[ex->pos] != ')') {
			ex->ok = 0;
			set_error(ex->as, "missing ')' in expression");
			return 0;
		}
		ex->pos++;
		return value;
	}
	if (s[ex->pos] == '$') {
		size_t start = ++ex->pos;
		while (isxdigit((unsigned char)s[ex->pos]))
			ex->pos++;
		if (ex->pos > start)
			return parse_based_digits(ex, s + start, ex->pos - start, 16);
		return ex->pc;
	}
	if (s[ex->pos] == '\'' || s[ex->pos] == '"') {
		char quote = s[ex->pos++];
		int value = 0;
		int count = 0;
		while (s[ex->pos] && s[ex->pos] != quote) {
			if (s[ex->pos] == '\\' && s[ex->pos + 1])
				ex->pos++;
			value = (unsigned char)s[ex->pos++];
			count++;
		}
		if (s[ex->pos] == quote)
			ex->pos++;
		if (count != 1) {
			ex->ok = 0;
			set_error(ex->as, "string expression must be one byte");
		}
		return value;
	}
	if (isdigit((unsigned char)s[ex->pos])) {
		size_t start = ex->pos;
		while (isalnum((unsigned char)s[ex->pos]))
			ex->pos++;
		if (s[start] == '0' && (s[start + 1] == 'x' || s[start + 1] == 'X'))
			return parse_based_digits(ex, s + start, ex->pos - start, 0);
		if (ex->pos > start && (s[ex->pos - 1] == 'h' || s[ex->pos - 1] == 'H'))
			return parse_based_digits(ex, s + start, ex->pos - start - 1, 16);
		if (ex->pos > start && (s[ex->pos - 1] == 'b' || s[ex->pos - 1] == 'B'))
			return parse_based_digits(ex, s + start, ex->pos - start - 1, 2);
		return parse_based_digits(ex, s + start, ex->pos - start, 10);
	}
	if (isalpha((unsigned char)s[ex->pos]) || s[ex->pos] == '_' || s[ex->pos] == '.') {
		size_t start = ex->pos;
		char *name;
		int value;
		while (isalnum((unsigned char)s[ex->pos]) || s[ex->pos] == '_' || s[ex->pos] == '.')
			ex->pos++;
		if (is_hex_suffix_token(s + start, ex->pos - start))
			return parse_based_digits(ex, s + start, ex->pos - start - 1, 16);
		name = xstrndup(s + start, ex->pos - start);
		if (symbol_get(ex->as, name, &value)) {
			free(name);
			return value;
		}
		if (!ex->as->resolve_pass) {
			free(name);
			return 0;
		}
		set_errorf(ex->as, "unknown symbol %s", name);
		free(name);
		ex->ok = 0;
		return 0;
	}
	ex->ok = 0;
	set_error(ex->as, "bad expression");
	return 0;
}

static int parse_unary(struct expr *ex)
{
	expr_skip(ex);
	if (ex->text[ex->pos] == '+') {
		ex->pos++;
		return parse_unary(ex);
	}
	if (ex->text[ex->pos] == '-') {
		ex->pos++;
		return -parse_unary(ex);
	}
	if (ex->text[ex->pos] == '~') {
		ex->pos++;
		return ~parse_unary(ex);
	}
	return parse_primary(ex);
}

static int parse_mul(struct expr *ex)
{
	int value = parse_unary(ex);
	while (ex->ok) {
		char op;
		int rhs;
		expr_skip(ex);
		op = ex->text[ex->pos];
		if (op != '*' && op != '/' && op != '%')
			break;
		ex->pos++;
		if (op == '/' && ex->text[ex->pos] == '/')
			ex->pos++;
		rhs = parse_unary(ex);
		if (!rhs && (op == '/' || op == '%')) {
			ex->ok = 0;
			set_error(ex->as, "division by zero");
			return 0;
		}
		if (op == '*')
			value *= rhs;
		else if (op == '/')
			value /= rhs;
		else
			value %= rhs;
	}
	return value;
}

static int parse_add(struct expr *ex)
{
	int value = parse_mul(ex);
	while (ex->ok) {
		char op;
		int rhs;
		expr_skip(ex);
		op = ex->text[ex->pos];
		if (op != '+' && op != '-')
			break;
		ex->pos++;
		rhs = parse_mul(ex);
		value = (op == '+') ? value + rhs : value - rhs;
	}
	return value;
}

static int parse_shift(struct expr *ex)
{
	int value = parse_add(ex);
	while (ex->ok) {
		int right = 0;
		expr_skip(ex);
		if (ex->text[ex->pos] == '<' && ex->text[ex->pos + 1] == '<') {
			ex->pos += 2;
			right = 1;
		} else if (ex->text[ex->pos] == '>' && ex->text[ex->pos + 1] == '>') {
			ex->pos += 2;
			right = -1;
		} else {
			break;
		}
		if (right > 0)
			value <<= parse_add(ex);
		else
			value >>= parse_add(ex);
	}
	return value;
}

static int parse_and(struct expr *ex)
{
	int value = parse_shift(ex);
	while (ex->ok) {
		expr_skip(ex);
		if (ex->text[ex->pos] != '&')
			break;
		ex->pos++;
		value &= parse_shift(ex);
	}
	return value;
}

static int parse_xor(struct expr *ex)
{
	int value = parse_and(ex);
	while (ex->ok) {
		expr_skip(ex);
		if (ex->text[ex->pos] != '^')
			break;
		ex->pos++;
		value ^= parse_and(ex);
	}
	return value;
}

static int parse_expr_or(struct expr *ex)
{
	int value = parse_xor(ex);
	while (ex->ok) {
		expr_skip(ex);
		if (ex->text[ex->pos] != '|')
			break;
		ex->pos++;
		value |= parse_xor(ex);
	}
	return value;
}

static int eval_expr(struct assembler *as, const char *text, int pc, int *ok)
{
	struct expr ex;
	char *sized = strip_operand_size_hint_copy(text);
	const char *body = sized;
	int value;
	while (isspace((unsigned char)*body))
		body++;
	if (*body == '#')
		body++;
	ex.as = as;
	ex.text = body;
	ex.pos = 0;
	ex.pc = pc;
	ex.ok = 1;
	value = parse_expr_or(&ex);
	expr_skip(&ex);
	if (ex.text[ex.pos]) {
		ex.ok = 0;
		set_error(as, "trailing text in expression");
	}
	if (ok)
		*ok = ex.ok;
	free(sized);
	return value;
}

static int u8(struct assembler *as, int value, const char *what)
{
	if (value < -128 || value > 0xff) {
		set_errorf(as, "byte value out of range for %s", what);
		return 0;
	}
	return value & 0xff;
}

static int u16(struct assembler *as, int value, const char *what)
{
	if (value < -32768 || value > 0xffff) {
		set_errorf(as, "word value out of range for %s", what);
		return 0;
	}
	return value & 0xffff;
}

static int looks_immediate(const char *text)
{
	while (isspace((unsigned char)*text))
		text++;
	return *text == '#';
}

static int imm_value(struct assembler *as, const char *text, int pc)
{
	int ok = 0;
	return eval_expr(as, text, pc, &ok);
}

static int imm8(struct assembler *as, const char *text, int pc)
{
	return u8(as, imm_value(as, text, pc), text);
}

static int parse_register_number_token(char *text, int *out)
{
	size_t len = strlen(text);
	int base = 10;
	char *endp;
	long value;
	if (!len)
		return 0;
	if (text[len - 1] == 'h' || text[len - 1] == 'H') {
		text[--len] = '\0';
		base = 16;
		if (!len)
			return 0;
	}
	errno = 0;
	value = strtol(text, &endp, base);
	if (errno || endp == text || *endp || value < 0 || value > 0xff)
		return 0;
	*out = (int)value;
	return 1;
}

static int direct_register_value(struct assembler *as, const char *text, int *out)
{
	char *t = strip_operand_size_hint_copy(text);
	char *p = t;
	int rr = 0;
	int value;
	if (!strncasecmp(p, "rr", 2)) {
		rr = 1;
		p += 2;
	} else if (tolower((unsigned char)p[0]) == 'r') {
		p += 1;
	} else {
		if (symbol_get(as, text, &value) && value >= 0 && value <= 0xff) {
			*out = value;
			free(t);
			return 1;
		}
		free(t);
		return 0;
	}
	(void)rr;
	if (parse_register_number_token(p, &value)) {
		*out = value;
		free(t);
		return 1;
	}
	free(t);
	return 0;
}

static int direct8(struct assembler *as, const char *text, int pc)
{
	int value;
	int ok = 0;
	if (direct_register_value(as, text, &value))
		return value;
	value = eval_expr(as, text, pc, &ok);
	return u8(as, value, text);
}

static int op_reg(const char *text)
{
	char *t = trim_copy_range(text, strlen(text));
	int value = -1;
	if ((tolower((unsigned char)t[0]) == 'r') && t[1]) {
		if (!parse_register_number_token(t + 1, &value) || value < 0 || value > 7)
			value = -1;
	}
	free(t);
	return value;
}

static int pi_reg(struct assembler *as, const char *text, int pc)
{
	char *t = trim_copy_range(text, strlen(text));
	int value = -1;
	int direct;
	(void)pc;
	if (direct_register_value(as, text, &direct) && direct >= 0x10 && direct <= 0x17)
		value = direct - 0x10;
	free(t);
	return value;
}

static int parse_pair_base(const char *text)
{
	char *t = trim_copy_range(text, strlen(text));
	char *p = t;
	int value = -1;
	if (tolower((unsigned char)p[0]) != 'r' || tolower((unsigned char)p[1]) != 'r') {
		free(t);
		return -1;
	}
	p += 2;
	if (!parse_register_number_token(p, &value))
		value = -1;
	free(t);
	return value;
}

static int pair_to_sel(int value)
{
	static const int values[] = { 0x00, 0x08, 0x02, 0x0a, 0x04, 0x0c, 0x06, 0x0e };
	int i;
	for (i = 0; i < 8; i++) {
		if (values[i] == value)
			return i;
	}
	return -1;
}

static int desc_pair_sel(const char *text)
{
	int value = parse_pair_base(text);
	return value < 0 ? -1 : pair_to_sel(value);
}

static int parse_at_reg(const char *text)
{
	char *t = trim_copy_range(text, strlen(text));
	int value = -1;
	if (t[0] == '@')
		value = op_reg(t + 1);
	free(t);
	return value;
}

static int indirect_byte_reg(const char *text)
{
	char *t = trim_copy_range(text, strlen(text));
	int value = -1;
	if (tolower((unsigned char)t[0]) == 'r' && tolower((unsigned char)t[1]) == 'r') {
		if (!parse_register_number_token(t + 2, &value) || value < 0 || value > 7)
			value = -1;
	}
	free(t);
	return value;
}

static int maybe_expr(struct assembler *as, const char *text, int pc, int *value)
{
	int ok = 0;
	char old_error[sizeof(as->error)];
	memcpy(old_error, as->error, sizeof(old_error));
	*value = eval_expr(as, text, pc, &ok);
	if (!ok) {
		memcpy(as->error, old_error, sizeof(old_error));
		return 0;
	}
	return 1;
}

static int check_width(struct assembler *as, int value, int width, const char *text)
{
	return width == 8 ? u8(as, value, text) : u16(as, value, text);
}

static int split_outer_paren(const char *text, char **before, char **inside, int *plus_after)
{
	size_t len = strlen(text);
	size_t open;
	char close = ')';
	if (plus_after)
		*plus_after = 0;
	if (len >= 3 && ((text[0] == '(' && text[len - 2] == ')') || (text[0] == '[' && text[len - 2] == ']')) && text[len - 1] == '+') {
		*before = xstrdup("");
		*inside = trim_copy_range(text + 1, len - 3);
		if (plus_after)
			*plus_after = 1;
		return 1;
	}
	if (len >= 2 && ((text[0] == '(' && text[len - 1] == ')') || (text[0] == '[' && text[len - 1] == ']'))) {
		*before = xstrdup("");
		*inside = trim_copy_range(text + 1, len - 2);
		return 1;
	}
	open = find_top_level_char(text, '(');
	if (open == (size_t)-1) {
		open = find_top_level_char(text, '[');
		close = ']';
	}
	if (open != (size_t)-1 && len >= 2 && text[len - 1] == close) {
		*before = trim_copy_range(text, open);
		*inside = trim_copy_range(text + open + 1, len - open - 2);
		return 1;
	}
	return 0;
}

static struct memref parse_indirect(struct assembler *as, const char *text, int pc, int word_ptr)
{
	struct memref out = { 0, 0, 0, 0, word_ptr ? 16 : 8 };
	char *t = trim_copy_range(text, strlen(text));
	int (*reg_parser)(const char *) = word_ptr ? desc_pair_sel : op_reg;
	if (t[0] == '@') {
		int reg = reg_parser(t + 1);
		int value;
		if (reg >= 0) {
			out.valid = 1;
			out.mode = 0;
			out.reg = reg;
		} else if (maybe_expr(as, t + 1, pc, &value)) {
			out.valid = 1;
			out.mode = 2;
			out.reg = 0;
			out.offset = check_width(as, value, out.width, t + 1);
		}
		free(t);
		return out;
	}
	if (t[0] == '-' && t[1] == '(') {
		size_t len = strlen(t);
		if (len >= 4 && (t[len - 1] == ')' || (t[len - 1] == '+' && t[len - 2] == ')'))) {
			char *body = trim_copy_range(t + 2, len - (t[len - 1] == '+' ? 4 : 3));
			int reg = reg_parser(body);
			if (reg >= 0) {
				out.valid = 1;
				out.mode = 3;
				out.reg = reg;
			}
			free(body);
			free(t);
			return out;
		}
	}
	{
		char *before = NULL;
		char *inside = NULL;
		int plus_after = 0;
		if (split_outer_paren(t, &before, &inside, &plus_after)) {
			int reg = reg_parser(inside);
			if (plus_after && reg >= 0) {
				out.valid = 1;
				out.mode = 1;
				out.reg = reg;
			} else if (!before[0] && reg >= 0) {
				out.valid = 1;
				out.mode = 0;
				out.reg = reg;
			} else if (before[0] && reg >= 0) {
				if (reg == 0) {
					set_error(as, "indexed r0/rr0 form is reserved; use @absolute");
				} else {
					int ok = 0;
					int value = eval_expr(as, before, pc, &ok);
					out.valid = 1;
					out.mode = 2;
					out.reg = reg;
					out.offset = check_width(as, value, out.width, before);
				}
			} else if (!before[0]) {
				size_t plus = find_top_level_char(inside, '+');
				if (plus != (size_t)-1) {
					char *left = trim_copy_range(inside, plus);
					char *right = trim_copy_range(inside + plus + 1, strlen(inside + plus + 1));
					reg = reg_parser(left);
					if (reg >= 0) {
						int ok = 0;
						int value = eval_expr(as, right, pc, &ok);
						out.valid = 1;
						out.mode = 2;
						out.reg = reg;
						out.offset = check_width(as, value, out.width, right);
					}
					free(left);
					free(right);
				}
			}
			free(before);
			free(inside);
		}
	}
	free(t);
	return out;
}

static struct memref parse_indirect_imm_ref(struct assembler *as, const char *text, int pc)
{
	struct memref out = { 0, 0, 0, 0, 8 };
	char *t = trim_copy_range(text, strlen(text));
	if (t[0] == '@') {
		int reg = indirect_byte_reg(t + 1);
		int value;
		if (reg >= 0) {
			out.valid = 1;
			out.mode = 0;
			out.reg = reg;
		} else if (maybe_expr(as, t + 1, pc, &value)) {
			out.valid = 1;
			out.mode = 2;
			out.reg = 0;
			out.offset = u8(as, value, t + 1);
		}
		free(t);
		return out;
	}
	if (t[0] == '-' && t[1] == '(') {
		size_t len = strlen(t);
		if (len >= 4 && (t[len - 1] == ')' || (t[len - 1] == '+' && t[len - 2] == ')'))) {
			char *body = trim_copy_range(t + 2, len - (t[len - 1] == '+' ? 4 : 3));
			int reg = indirect_byte_reg(body);
			if (reg >= 0) {
				out.valid = 1;
				out.mode = 3;
				out.reg = reg;
			}
			free(body);
			free(t);
			return out;
		}
	}
	{
		char *before = NULL;
		char *inside = NULL;
		int plus_after = 0;
		if (split_outer_paren(t, &before, &inside, &plus_after)) {
			int reg = indirect_byte_reg(inside);
			if (plus_after && reg >= 0) {
				out.valid = 1;
				out.mode = 1;
				out.reg = reg;
			} else if (!before[0] && reg >= 0) {
				out.valid = 1;
				out.mode = 0;
				out.reg = reg;
			} else if (!before[0]) {
				size_t plus = find_top_level_char(inside, '+');
				if (plus != (size_t)-1) {
					char *left = trim_copy_range(inside, plus);
					char *right = trim_copy_range(inside + plus + 1, strlen(inside + plus + 1));
					reg = indirect_byte_reg(left);
					if (reg >= 0) {
						int ok = 0;
						out.valid = 1;
						out.mode = 2;
						out.reg = reg;
						out.offset = u8(as, eval_expr(as, right, pc, &ok), right);
					}
					free(left);
					free(right);
				}
			}
			free(before);
			free(inside);
		}
	}
	free(t);
	return out;
}

static void emit_mem_desc(struct bytes *out, int opcode, int reg, struct memref mem)
{
	int desc = (mem.mode << 6) | (reg << 3) | mem.reg;
	bytes_emit(out, opcode);
	bytes_emit(out, desc);
	if (mem.mode == 2) {
		if (mem.width == 8) {
			bytes_emit(out, mem.offset & 0xff);
		} else {
			bytes_emit_word(out, mem.offset);
		}
	}
}

static int absolute_word_source(struct assembler *as, const char *text, int pc, int *value)
{
	int direct;
	int expr_value;
	int hint = operand_size_hint(text);
	if (looks_immediate(text))
		return 0;
	if (hint == 8)
		return 0;
	if (direct_register_value(as, text, &direct))
		return 0;
	if (!maybe_expr(as, text, pc, &expr_value))
		return 0;
	if (hint == 16 || expr_value > 0xff || expr_value < 0) {
		*value = u16(as, expr_value, text);
		return 1;
	}
	return 0;
}

static int rel8(struct assembler *as, const char *text, int pc, int size)
{
	int value;
	int delta;
	int ok = 0;
	if (!as->resolve_pass)
		return 0;
	value = eval_expr(as, text, pc, &ok);
	delta = value - ((pc + size) & 0xffff);
	if (delta >= -128 && delta <= 127)
		return delta & 0xff;
	if (value >= 0 && value <= 0xff)
		return value;
	set_errorf(as, "relative branch target out of range: %s", text);
	return 0;
}

static int cond_code(struct assembler *as, const char *text)
{
	int idx = list_index(text, cond_names, 16);
	if (idx < 0 && str_eq_lit(text, "eq"))
		idx = 6;
	if (idx < 0 && str_eq_lit(text, "ult"))
		idx = 7;
	if (idx < 0 && str_eq_lit(text, "ne"))
		idx = 14;
	if (idx < 0 && str_eq_lit(text, "uge"))
		idx = 15;
	if (idx < 0)
		set_errorf(as, "unknown condition %s", text);
	return idx < 0 ? 0 : idx;
}

static int bit_index(struct assembler *as, const char *text, int pc)
{
	int value = imm_value(as, text, pc);
	if (value < 0 || value > 7) {
		set_errorf(as, "bit index must be 0-7: %s", text);
		return 0;
	}
	return value;
}

static struct bitmem parse_bit_mem(struct assembler *as, const char *text, int pc)
{
	struct bitmem out = { 0, 0, 0 };
	char *t = trim_copy_range(text, strlen(text));
	int value;
	if (t[0] == '#')
		memmove(t, t + 1, strlen(t));
	if (maybe_expr(as, t, pc, &value) && value >= 0xff00 && value <= 0xffff) {
		out.valid = 1;
		out.base = 0;
		out.offset = value & 0xff;
		free(t);
		return out;
	}
	{
		char *before = NULL;
		char *inside = NULL;
		int plus_after = 0;
		if (split_outer_paren(t, &before, &inside, &plus_after) && before[0] && !plus_after) {
			int ok = 0;
			int base = op_reg(inside);
			out.offset = u8(as, eval_expr(as, before, pc, &ok), before);
			if (str_eq_lit(inside, "ff00h") || str_eq_lit(inside, "$ff00") || str_eq_lit(inside, "0xff00") || str_eq_lit(inside, "ff00")) {
				out.valid = 1;
				out.base = 0;
			} else if (base > 0) {
				out.valid = 1;
				out.base = base;
			}
		}
		free(before);
		free(inside);
	}
	free(t);
	return out;
}

static int encode_indirect_imm(struct assembler *as, struct bytes *out, int opcode, char **ops, int op_count, int pc)
{
	struct memref mem;
	int desc;
	if (op_count != 2)
		return 0;
	mem = parse_indirect_imm_ref(as, ops[0], pc);
	if (!mem.valid)
		return 0;
	desc = (mem.mode << 6) | mem.reg;
	bytes_emit(out, opcode);
	bytes_emit(out, desc);
	if (mem.mode == 2)
		bytes_emit(out, mem.offset & 0xff);
	bytes_emit(out, imm8(as, ops[1], pc));
	return 1;
}

static int encode_byte_indirect_load(struct assembler *as, struct bytes *out, int opcode, char **ops, int pc, int word_ptr)
{
	int dst = op_reg(ops[0]);
	struct memref mem;
	if (dst < 0)
		return 0;
	mem = parse_indirect(as, ops[1], pc, word_ptr);
	if (!mem.valid)
		return 0;
	emit_mem_desc(out, opcode, dst, mem);
	return 1;
}

static int encode_movw(struct assembler *as, struct bytes *out, char **ops, int pc)
{
	int dst_sel = desc_pair_sel(ops[0]);
	if (dst_sel >= 0 && looks_immediate(ops[1])) {
		int value = u16(as, imm_value(as, ops[1], pc), ops[1]);
		bytes_emit(out, 0x78 | dst_sel);
		bytes_emit_word(out, value);
		return 1;
	}
	if (dst_sel >= 0) {
		struct memref mem = parse_indirect(as, ops[1], pc, 1);
		int src_sel;
		if (mem.valid) {
			emit_mem_desc(out, 0x3a, dst_sel, mem);
			return 1;
		}
		src_sel = desc_pair_sel(ops[1]);
		if (src_sel >= 0) {
			bytes_emit(out, 0x3c);
			bytes_emit(out, (dst_sel << 3) | src_sel);
			return 1;
		}
	}
	{
		struct memref mem = parse_indirect(as, ops[0], pc, 1);
		int src_sel = desc_pair_sel(ops[1]);
		if (mem.valid && src_sel >= 0) {
			emit_mem_desc(out, 0x3b, src_sel, mem);
			return 1;
		}
	}
	if (looks_immediate(ops[1])) {
		int value = u16(as, imm_value(as, ops[1], pc), ops[1]);
		bytes_emit(out, 0x4b);
		bytes_emit(out, direct8(as, ops[0], pc));
		bytes_emit_word(out, value);
		return 1;
	}
	bytes_emit(out, 0x4a);
	bytes_emit(out, direct8(as, ops[1], pc));
	bytes_emit(out, direct8(as, ops[0], pc));
	return 1;
}

static int encode_mov(struct assembler *as, struct bytes *out, char **ops, int pc)
{
	int dst_op;
	int src_op;
	struct memref mem;
	int abs;
	if (looks_immediate(ops[1]) && encode_indirect_imm(as, out, 0x5b, ops, 2, pc))
		return 1;
	if (str_eq_lit(ops[0], "ps0") && looks_immediate(ops[1])) {
		bytes_emit(out, 0x2e);
		bytes_emit(out, imm8(as, ops[1], pc));
		return 1;
	}
	dst_op = op_reg(ops[0]);
	if (dst_op >= 0 && looks_immediate(ops[1])) {
		bytes_emit(out, 0xc0 | dst_op);
		bytes_emit(out, imm8(as, ops[1], pc));
		return 1;
	}
	{
		int dst_pi = pi_reg(as, ops[0], pc);
		if (dst_pi >= 0 && looks_immediate(ops[1])) {
			bytes_emit(out, 0xc8 | dst_pi);
			bytes_emit(out, imm8(as, ops[1], pc));
			return 1;
		}
	}
	if (dst_op >= 0) {
		mem = parse_indirect(as, ops[1], pc, 0);
		if (mem.valid) {
			emit_mem_desc(out, 0x28, dst_op, mem);
			return 1;
		}
		mem = parse_indirect(as, ops[1], pc, 1);
		if (mem.valid) {
			emit_mem_desc(out, 0x38, dst_op, mem);
			return 1;
		}
		if (absolute_word_source(as, ops[1], pc, &abs)) {
			bytes_emit(out, 0x38);
			bytes_emit(out, 0x80 | (dst_op << 3));
			bytes_emit_word(out, abs);
			return 1;
		}
		bytes_emit(out, 0xb0 | dst_op);
		bytes_emit(out, direct8(as, ops[1], pc));
		return 1;
	}
	mem = parse_indirect(as, ops[0], pc, 0);
	src_op = op_reg(ops[1]);
	if (mem.valid && src_op >= 0) {
		emit_mem_desc(out, 0x29, src_op, mem);
		return 1;
	}
	mem = parse_indirect(as, ops[0], pc, 1);
	if (mem.valid && src_op >= 0) {
		emit_mem_desc(out, 0x39, src_op, mem);
		return 1;
	}
	if (looks_immediate(ops[1])) {
		bytes_emit(out, 0x58);
		bytes_emit(out, imm8(as, ops[1], pc));
		bytes_emit(out, direct8(as, ops[0], pc));
		return 1;
	}
	if (src_op >= 0) {
		bytes_emit(out, 0xb8 | src_op);
		bytes_emit(out, direct8(as, ops[0], pc));
		return 1;
	}
	bytes_emit(out, 0x48);
	bytes_emit(out, direct8(as, ops[1], pc));
	bytes_emit(out, direct8(as, ops[0], pc));
	return 1;
}

static int encode_bit_modify(struct assembler *as, struct bytes *out, const char *mnemonic, char **ops, int pc)
{
	struct bitmem bm;
	int bit;
	if (!ops[0] || !ops[1])
		return 0;
	bm = parse_bit_mem(as, ops[0], pc);
	bit = bit_index(as, ops[1], pc);
	if (bm.valid) {
		bytes_emit(out, str_eq_lit(mnemonic, "bclr") ? 0x1c : 0x1d);
		bytes_emit(out, (bm.base << 3) | bit);
		bytes_emit(out, bm.offset);
		return 1;
	}
	bytes_emit(out, (str_eq_lit(mnemonic, "bclr") ? 0xa0 : 0xa8) | bit);
	bytes_emit(out, direct8(as, ops[0], pc));
	return 1;
}

static int encode_bit_branch(struct assembler *as, struct bytes *out, const char *mnemonic, char **ops, int pc)
{
	struct bitmem bm;
	int bit;
	if (!ops[0] || !ops[1] || !ops[2])
		return 0;
	bm = parse_bit_mem(as, ops[0], pc);
	bit = bit_index(as, ops[1], pc);
	if (bm.valid) {
		bytes_emit(out, str_eq_lit(mnemonic, "bbc") ? 0x2a : 0x2b);
		bytes_emit(out, (bm.base << 3) | bit);
		bytes_emit(out, bm.offset);
		bytes_emit(out, rel8(as, ops[2], pc, 4));
		return 1;
	}
	bytes_emit(out, (str_eq_lit(mnemonic, "bbc") ? 0x80 : 0x88) | bit);
	bytes_emit(out, direct8(as, ops[0], pc));
	bytes_emit(out, rel8(as, ops[2], pc, 3));
	return 1;
}

static int encode_bitflag(struct assembler *as, struct bytes *out, const char *mnemonic, char **ops, int op_count, int pc)
{
	int mod;
	if (op_count != 3)
		return 0;
	if (str_eq_lit(mnemonic, "bmov") && str_eq_lit(ops[0], "bf")) {
		bytes_emit(out, 0x4e);
		bytes_emit(out, bit_index(as, ops[2], pc));
		bytes_emit(out, direct8(as, ops[1], pc));
		return 1;
	}
	if (str_eq_lit(mnemonic, "bmov") && str_eq_lit(ops[2], "bf")) {
		bytes_emit(out, 0x4e);
		bytes_emit(out, 0x40 | bit_index(as, ops[1], pc));
		bytes_emit(out, direct8(as, ops[0], pc));
		return 1;
	}
	mod = str_eq_lit(mnemonic, "bcmp") ? 0 : str_eq_lit(mnemonic, "band") ? 1 : str_eq_lit(mnemonic, "bor") ? 2 : 3;
	if ((str_eq_lit(mnemonic, "bcmp") || str_eq_lit(mnemonic, "band")) && str_eq_lit(ops[0], "bf")) {
		bytes_emit(out, 0x4f);
		bytes_emit(out, (mod << 6) | bit_index(as, ops[2], pc));
		bytes_emit(out, direct8(as, ops[1], pc));
		return 1;
	}
	if ((str_eq_lit(mnemonic, "bor") || str_eq_lit(mnemonic, "bxor")) && str_eq_lit(ops[2], "bf")) {
		bytes_emit(out, 0x4f);
		bytes_emit(out, (mod << 6) | bit_index(as, ops[1], pc));
		bytes_emit(out, direct8(as, ops[0], pc));
		return 1;
	}
	return 0;
}

static int encode_jmp_call(struct assembler *as, struct bytes *out, int indirect_opcode, char **ops, int op_count, int pc, int is_call)
{
	if (op_count == 1) {
		int sel = desc_pair_sel(ops[0]);
		int value;
		int ok = 0;
		if (sel >= 0) {
			bytes_emit(out, indirect_opcode);
			bytes_emit(out, sel);
			return 1;
		}
		if (ops[0][0] == '@') {
			char *body = trim_copy_range(ops[0] + 1, strlen(ops[0] + 1));
			char *before = NULL;
			char *inside = NULL;
			int plus_after = 0;
			if (split_outer_paren(body, &before, &inside, &plus_after) && before[0] && !plus_after) {
				int reg = op_reg(inside);
				if (reg <= 0)
					set_error(as, "indexed jump/call requires r1-r7 base");
				value = u16(as, eval_expr(as, before, pc, &ok), before);
				bytes_emit(out, indirect_opcode);
				bytes_emit(out, 0x40 | (reg << 3));
				bytes_emit_word(out, value);
				free(before);
				free(inside);
				free(body);
				return 1;
			}
			free(before);
			free(inside);
			value = u16(as, eval_expr(as, body, pc, &ok), body);
			bytes_emit(out, indirect_opcode);
			bytes_emit(out, 0x40);
			bytes_emit_word(out, value);
			free(body);
			return 1;
		}
		value = u16(as, eval_expr(as, ops[0], pc, &ok), ops[0]);
		bytes_emit(out, is_call ? 0x49 : 0x98);
		bytes_emit_word(out, value);
		return 1;
	}
	if (!is_call && op_count == 2) {
		int value;
		int ok = 0;
		bytes_emit(out, 0x90 | cond_code(as, ops[0]));
		value = u16(as, eval_expr(as, ops[1], pc, &ok), ops[1]);
		bytes_emit_word(out, value);
		return 1;
	}
	return 0;
}

static int encode_instruction(struct assembler *as, struct line *line, struct bytes *out, int pc)
{
	const char *m = line->op;
	char **ops = line->operands;
	int n = line->operand_count;
	int idx;

	idx = list_index(m, no_operand_names, sizeof(no_operand_names) / sizeof(no_operand_names[0]));
	if (idx >= 0 && n == 0) {
		bytes_emit(out, no_operand_values[idx]);
		return 1;
	}
	if ((str_eq_lit(m, "invalid") || str_eq_lit(m, "illegal") || str_eq_lit(m, "ill"))) {
		int value = n ? u8(as, eval_expr(as, ops[0], pc, NULL), ops[0]) : 0x59;
		if (!(value == 0x59 || (value >= 0xf2 && value <= 0xf7)))
			set_error(as, "invalid opcode must be 59h or f2h-f7h");
		bytes_emit(out, value);
		return 1;
	}
	idx = list_index(m, unary_names, sizeof(unary_names) / sizeof(unary_names[0]));
	if (idx >= 0 && n == 1) {
		int reg = parse_at_reg(ops[0]);
		if (reg >= 0 && idx < 8) {
			bytes_emit(out, 0x1a);
			bytes_emit(out, (reg << 3) | idx);
			return 1;
		}
		if (reg >= 0 && idx >= 8) {
			bytes_emit(out, 0x1b);
			bytes_emit(out, (reg << 3) | (idx - 8));
			return 1;
		}
		bytes_emit(out, idx);
		bytes_emit(out, direct8(as, ops[0], pc));
		return 1;
	}
	if ((str_eq_lit(m, "push") || str_eq_lit(m, "pop")) && n == 1) {
		int reg = parse_at_reg(ops[0]);
		if (reg >= 0) {
			bytes_emit(out, 0x1b);
			bytes_emit(out, (reg << 3) | (str_eq_lit(m, "push") ? 6 : 7));
		} else {
			bytes_emit(out, str_eq_lit(m, "push") ? 0x0e : 0x0f);
			bytes_emit(out, direct8(as, ops[0], pc));
		}
		return 1;
	}
	if ((str_eq_lit(m, "incw") || str_eq_lit(m, "decw") || str_eq_lit(m, "pushw") || str_eq_lit(m, "popw")) && n == 1) {
		int opcode = str_eq_lit(m, "incw") ? 0x18 : str_eq_lit(m, "decw") ? 0x19 : str_eq_lit(m, "pushw") ? 0x1e : 0x1f;
		bytes_emit(out, opcode);
		bytes_emit(out, direct8(as, ops[0], pc));
		return 1;
	}
	if (str_eq_lit(m, "exts") && n == 1) {
		bytes_emit(out, 0x2c);
		bytes_emit(out, direct8(as, ops[0], pc));
		return 1;
	}
	if (str_eq_lit(m, "dm") && n == 1) {
		bytes_emit(out, looks_immediate(ops[0]) ? 0x2d : 0x3d);
		bytes_emit(out, looks_immediate(ops[0]) ? imm8(as, ops[0], pc) : direct8(as, ops[0], pc));
		return 1;
	}
	if (str_eq_lit(m, "btst") && n == 2) {
		bytes_emit(out, 0x2f);
		bytes_emit(out, direct8(as, ops[0], pc));
		bytes_emit(out, imm8(as, ops[1], pc));
		return 1;
	}
	idx = list_index(m, byte_alu_names, sizeof(byte_alu_names) / sizeof(byte_alu_names[0]));
	if (idx >= 0 && n == 2) {
		int dst;
		int src;
		int abs;
		if (str_eq_lit(m, "cmp") && looks_immediate(ops[1]) && encode_indirect_imm(as, out, 0x5a, ops, n, pc))
			return 1;
		dst = op_reg(ops[0]);
		src = op_reg(ops[1]);
		if (dst >= 0 && src >= 0) {
			bytes_emit(out, 0x10 + idx);
			bytes_emit(out, (dst << 3) | src);
			return 1;
		}
		if (encode_byte_indirect_load(as, out, 0x20 + idx, ops, pc, 0))
			return 1;
		if (encode_byte_indirect_load(as, out, 0x30 + idx, ops, pc, 1))
			return 1;
		if (dst >= 0 && absolute_word_source(as, ops[1], pc, &abs)) {
			bytes_emit(out, 0x30 + idx);
			bytes_emit(out, 0x80 | (dst << 3));
			bytes_emit_word(out, abs);
			return 1;
		}
	}
	if (str_eq_lit(m, "mov") && n == 2)
		return encode_mov(as, out, ops, pc);
	if (str_eq_lit(m, "movw") && n == 2)
		return encode_movw(as, out, ops, pc);
	idx = list_index(m, byte_alu_names, sizeof(byte_alu_names) / sizeof(byte_alu_names[0]));
	if ((idx >= 0 || str_eq_lit(m, "mov")) && n == 2) {
		int base = str_eq_lit(m, "mov") ? 8 : idx;
		if (looks_immediate(ops[1])) {
			bytes_emit(out, 0x50 + base);
			bytes_emit(out, imm8(as, ops[1], pc));
			bytes_emit(out, direct8(as, ops[0], pc));
		} else {
			bytes_emit(out, 0x40 + base);
			bytes_emit(out, direct8(as, ops[1], pc));
			bytes_emit(out, direct8(as, ops[0], pc));
		}
		return 1;
	}
	idx = list_index(m, word_alu_names, sizeof(word_alu_names) / sizeof(word_alu_names[0]));
	if (idx >= 0 && n == 2) {
		if (looks_immediate(ops[1])) {
			int value = u16(as, imm_value(as, ops[1], pc), ops[1]);
			bytes_emit(out, 0x68 + idx);
			bytes_emit(out, direct8(as, ops[0], pc));
			bytes_emit_word(out, value);
		} else {
			bytes_emit(out, 0x60 + idx);
			bytes_emit(out, direct8(as, ops[1], pc));
			bytes_emit(out, direct8(as, ops[0], pc));
		}
		return 1;
	}
	if ((str_eq_lit(m, "bclr") || str_eq_lit(m, "bset")) && n == 2)
		return encode_bit_modify(as, out, m, ops, pc);
	if ((str_eq_lit(m, "bbc") || str_eq_lit(m, "bbs")) && n == 3)
		return encode_bit_branch(as, out, m, ops, pc);
	if ((str_eq_lit(m, "bmov") || str_eq_lit(m, "bcmp") || str_eq_lit(m, "band") || str_eq_lit(m, "bor") || str_eq_lit(m, "bxor")) && n == 3)
		return encode_bitflag(as, out, m, ops, n, pc);
	if (str_eq_lit(m, "mult") && n == 2) {
		bytes_emit(out, looks_immediate(ops[1]) ? 0x4d : 0x4c);
		bytes_emit(out, looks_immediate(ops[1]) ? imm8(as, ops[1], pc) : direct8(as, ops[1], pc));
		bytes_emit(out, direct8(as, ops[0], pc));
		return 1;
	}
	if (str_eq_lit(m, "div") && n == 2) {
		if (looks_immediate(ops[1])) {
			bytes_emit(out, 0x5d);
			bytes_emit(out, imm8(as, ops[1], pc));
			bytes_emit(out, direct8(as, ops[0], pc));
			return 1;
		}
		{
			int dst = desc_pair_sel(ops[0]);
			int src = desc_pair_sel(ops[1]);
			if (dst >= 0 && src >= 0) {
				bytes_emit(out, 0x5c);
				bytes_emit(out, (dst << 3) | src);
				return 1;
			}
		}
	}
	if (str_eq_lit(m, "movm") && n == 3) {
		bytes_emit(out, looks_immediate(ops[2]) ? 0x5f : 0x5e);
		bytes_emit(out, direct8(as, ops[0], pc));
		bytes_emit(out, imm8(as, ops[1], pc));
		bytes_emit(out, looks_immediate(ops[2]) ? imm8(as, ops[2], pc) : direct8(as, ops[2], pc));
		return 1;
	}
	if (str_eq_lit(m, "dbnz") && n == 2) {
		int reg = op_reg(ops[0]);
		if (reg < 0)
			set_error(as, "dbnz requires r0-r7");
		bytes_emit(out, 0x70 | reg);
		bytes_emit(out, rel8(as, ops[1], pc, 2));
		return 1;
	}
	if (str_eq_lit(m, "br")) {
		if (n == 1) {
			bytes_emit(out, 0xd8);
			bytes_emit(out, rel8(as, ops[0], pc, 2));
			return 1;
		}
		if (n == 2) {
			bytes_emit(out, 0xd0 | cond_code(as, ops[0]));
			bytes_emit(out, rel8(as, ops[1], pc, 2));
			return 1;
		}
	}
	if (str_eq_lit(m, "jmp"))
		return encode_jmp_call(as, out, 0x3e, ops, n, pc, 0);
	if (str_eq_lit(m, "call"))
		return encode_jmp_call(as, out, 0x3f, ops, n, pc, 1);
	if (str_eq_lit(m, "cals") && n == 1) {
		int ok = 0;
		int value = u16(as, eval_expr(as, ops[0], pc, &ok), ops[0]);
		if (value < 0x1000 || value > 0x1fff)
			set_error(as, "cals target must be in 1000h-1fffh");
		bytes_emit(out, 0xe0 | ((value >> 8) & 0x0f));
		bytes_emit(out, value & 0xff);
		return 1;
	}
	set_errorf(as, "cannot encode instruction %s", m);
	return 0;
}

static int directive_is_data_dm(struct line *line)
{
	int i;
	if (!str_eq_lit(line->op, "dm"))
		return 0;
	for (i = 0; i < line->operand_count; i++) {
		char *decoded = decode_string_token(line->operands[i]);
		if (decoded) {
			free(decoded);
			return 1;
		}
	}
	return 0;
}

static void encode_directive(struct assembler *as, struct line *line, struct bytes *out, int pc)
{
	const char *op = line->op;
	int i;
	if (str_eq_lit(op, "db") || str_eq_lit(op, "defb") || str_eq_lit(op, ".byte") ||
		str_eq_lit(op, "byte") || str_eq_lit(op, "defm") || directive_is_data_dm(line)) {
		for (i = 0; i < line->operand_count; i++) {
			char *decoded = decode_string_token(line->operands[i]);
			if (decoded) {
				size_t j;
				for (j = 0; j < strlen(decoded); j++)
					bytes_emit(out, (uint8_t)decoded[j]);
				free(decoded);
			} else {
				int ok = 0;
				bytes_emit(out, u8(as, eval_expr(as, line->operands[i], pc, &ok), line->operands[i]));
			}
		}
		return;
	}
	if (str_eq_lit(op, "dw") || str_eq_lit(op, "defw") || str_eq_lit(op, ".word") || str_eq_lit(op, "word")) {
		for (i = 0; i < line->operand_count; i++) {
			int ok = 0;
			bytes_emit_word(out, u16(as, eval_expr(as, line->operands[i], pc, &ok), line->operands[i]));
		}
		return;
	}
	if (str_eq_lit(op, "ds") || str_eq_lit(op, "defs") || str_eq_lit(op, ".space") || str_eq_lit(op, ".res")) {
		int ok = 0;
		int count = line->operand_count > 0 ? eval_expr(as, line->operands[0], pc, &ok) : 0;
		int fill = line->operand_count > 1 ? eval_expr(as, line->operands[1], pc, &ok) : 0;
		for (i = 0; i < count; i++)
			bytes_emit(out, u8(as, fill, "fill"));
		return;
	}
	if (str_eq_lit(op, "fill") || str_eq_lit(op, ".fill")) {
		int ok = 0;
		int count = line->operand_count > 0 ? eval_expr(as, line->operands[0], pc, &ok) : 0;
		int fill = line->operand_count > 1 ? eval_expr(as, line->operands[1], pc, &ok) : 0;
		for (i = 0; i < count; i++)
			bytes_emit(out, u8(as, fill, "fill"));
		return;
	}
	if (str_eq_lit(op, "align") || str_eq_lit(op, ".align")) {
		int ok = 0;
		int align = line->operand_count > 0 ? eval_expr(as, line->operands[0], pc, &ok) : 1;
		int fill = line->operand_count > 1 ? eval_expr(as, line->operands[1], pc, &ok) : 0;
		int pad = align > 0 ? ((-pc) % align + align) % align : 0;
		for (i = 0; i < pad; i++)
			bytes_emit(out, u8(as, fill, "fill"));
		return;
	}
	if (str_eq_lit(op, "incbin")) {
		char *name;
		char *dir;
		char *path;
		FILE *fp;
		long size;
		int ok = 0;
		int offset = 0;
		int count;
		if (line->operand_count < 1) {
			set_error(as, "incbin requires a path");
			return;
		}
		name = string_or_bare(line->operands[0]);
		dir = dirname_copy(line->path);
		path = join_path(dir, name);
		fp = fopen(path, "rb");
		if (!fp) {
			set_errorf(as, "cannot open incbin %s", path);
			free(path);
			free(dir);
			free(name);
			return;
		}
		fseek(fp, 0, SEEK_END);
		size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		offset = line->operand_count > 1 ? eval_expr(as, line->operands[1], pc, &ok) : 0;
		count = line->operand_count > 2 ? eval_expr(as, line->operands[2], pc, &ok) : (int)size - offset;
		if (offset < 0 || count < 0 || offset + count > size) {
			set_error(as, "incbin range outside file");
		} else {
			uint8_t *buf = xmalloc((size_t)count);
			fseek(fp, offset, SEEK_SET);
			if (fread(buf, 1, (size_t)count, fp) != (size_t)count)
				set_errorf(as, "cannot read incbin %s", path);
			for (i = 0; i < count; i++)
				bytes_emit(out, buf[i]);
			free(buf);
		}
		fclose(fp);
		free(path);
		free(dir);
		free(name);
		return;
	}
	if (str_eq_lit(op, "dm")) {
		encode_instruction(as, line, out, pc);
		return;
	}
	set_errorf(as, "unsupported directive %s", op);
}

static void assembler_pass(struct assembler *as, int resolve)
{
	uint16_t code_pc = 0;
	uint16_t data_pc = 0;
	int section_code = 1;
	int stopped = 0;
	size_t i;
	as->resolve_pass = resolve;
	for (i = 0; i < as->lines.size; i++) {
		struct line *line = &as->lines.items[i];
		struct bytes tmp = { 0 };
		uint16_t pc = section_code ? code_pc : data_pc;
		int line_is_equ = line->op && (str_eq_lit(line->op, "equ") || str_eq_lit(line->op, ".equ") || str_eq_lit(line->op, "set") || str_eq_lit(line->op, ".set"));
		line->address = pc;
		line->span = 0;
		bytes_clear(&line->out);
		if (stopped)
			continue;
		if (line->label && !line_is_equ)
			symbol_set(as, line->label, pc);
		if (!line->op)
			continue;
		if (str_eq_lit(line->op, "end")) {
			if (!as->root_path || strcmp(line->path, as->root_path) == 0)
				stopped = 1;
			continue;
		}
		if (str_eq_lit(line->op, "data")) {
			section_code = 0;
			continue;
		}
		if (str_eq_lit(line->op, "program")) {
			section_code = 1;
			continue;
		}
		if (str_eq_lit(line->op, "title") || str_eq_lit(line->op, "type") || str_eq_lit(line->op, "list") ||
			str_eq_lit(line->op, "nlist") || str_eq_lit(line->op, "global") || str_eq_lit(line->op, "public") ||
			str_eq_lit(line->op, "extrn") || str_eq_lit(line->op, "extern") || str_eq_lit(line->op, "eject") ||
			str_eq_lit(line->op, "page") || str_eq_lit(line->op, "if") || str_eq_lit(line->op, "else") ||
			str_eq_lit(line->op, "endif") || str_eq_lit(line->op, "error"))
			continue;
		if (str_eq_lit(line->op, "org") || str_eq_lit(line->op, ".org") || str_eq_lit(line->op, "base") || str_eq_lit(line->op, ".base")) {
			int ok = 0;
			uint16_t new_pc = (uint16_t)eval_expr(as, line->operands[0], pc, &ok);
			if (section_code)
				code_pc = new_pc;
			else
				data_pc = new_pc;
			line->address = new_pc;
			continue;
		}
		if (line_is_equ) {
			int ok = 0;
			const char *name = line->label ? line->label : line->operands[0];
			const char *expr = line->label ? line->operands[0] : line->operands[1];
			symbol_set(as, name, eval_expr(as, expr, pc, &ok));
			continue;
		}
		if (section_code || is_directive(line->op)) {
			if (is_directive(line->op))
				encode_directive(as, line, &tmp, pc);
			else
				encode_instruction(as, line, &tmp, pc);
		}
		if (as->error[0]) {
			char context[sizeof(as->error)];
			snprintf(context, sizeof(context), "%s:%d: %s", line->path, line->line_no, as->error);
			snprintf(as->error, sizeof(as->error), "%s", context);
			bytes_clear(&tmp);
			return;
		}
		line->span = tmp.size;
		if (section_code) {
			line->out = tmp;
			code_pc = (uint16_t)(code_pc + tmp.size);
		} else {
			bytes_clear(&tmp);
			data_pc = (uint16_t)(data_pc + line->span);
		}
	}
}

static int assemble(struct assembler *as, const char *source, const char *base_expr, struct bytes *image, uint16_t *base_out)
{
	size_t iter;
	char *last_sig = NULL;
	(void)last_sig;
	as->root_path = source;
	if (!load_lines(as, source)) {
		if (!as->error[0])
			snprintf(as->error, sizeof(as->error), "cannot read source %s", source);
		return 0;
	}
	for (iter = 0; iter < 16; iter++) {
		assembler_pass(as, 0);
		if (as->error[0])
			return 0;
	}
	assembler_pass(as, 1);
	if (as->error[0])
		return 0;
	assembler_pass(as, 1);
	if (as->error[0])
		return 0;
	{
		int have = 0;
		uint16_t min_addr = 0xffff;
		uint32_t max_addr = 0;
		size_t i;
		for (i = 0; i < as->lines.size; i++) {
			struct line *line = &as->lines.items[i];
			if (line->out.size) {
				have = 1;
				if (line->address < min_addr)
					min_addr = line->address;
				if ((uint32_t)line->address + line->out.size > max_addr)
					max_addr = (uint32_t)line->address + line->out.size;
			}
		}
		if (!have) {
			*base_out = base_expr ? (uint16_t)eval_expr(as, base_expr, 0, NULL) : 0;
			return 1;
		}
		*base_out = base_expr ? (uint16_t)eval_expr(as, base_expr, 0, NULL) : min_addr;
		if (*base_out > min_addr) {
			set_error(as, "base address is after first emitted byte");
			return 0;
		}
		image->size = max_addr - *base_out;
		image->cap = image->size;
		image->data = xmalloc(image->size);
		memset(image->data, 0, image->size);
		for (i = 0; i < as->lines.size; i++) {
			struct line *line = &as->lines.items[i];
			if (line->out.size)
				memcpy(image->data + (line->address - *base_out), line->out.data, line->out.size);
		}
	}
	return 1;
}

static void write_file(const char *path, const uint8_t *data, size_t size)
{
	FILE *fp = fopen(path, "wb");
	if (!fp) {
		fprintf(stderr, "gamecom-as: error: cannot create %s: %s\n", path, strerror(errno));
		exit(1);
	}
	if (fwrite(data, 1, size, fp) != size) {
		fprintf(stderr, "gamecom-as: error: cannot write %s: %s\n", path, strerror(errno));
		exit(1);
	}
	fclose(fp);
}

static void write_listing(struct assembler *as, const char *path)
{
	FILE *fp = fopen(path, "w");
	size_t i;
	if (!fp) {
		fprintf(stderr, "gamecom-as: error: cannot create %s: %s\n", path, strerror(errno));
		exit(1);
	}
	for (i = 0; i < as->lines.size; i++) {
		struct line *line = &as->lines.items[i];
		size_t j;
		fprintf(fp, "%04x  ", line->address);
		for (j = 0; j < line->out.size; j++)
			fprintf(fp, "%02x", line->out.data[j]);
		fprintf(fp, "\t%s\n", line->raw);
	}
	fclose(fp);
}

static void usage(void)
{
	fprintf(stderr, "usage: gamecom-as SOURCE -o OUTPUT [--lst LISTING] [--base EXPR] [--define NAME=EXPR] [--case-sensitive]\n");
	exit(1);
}

int main(int argc, char **argv)
{
	struct assembler as;
	const char *source = NULL;
	const char *output = NULL;
	const char *listing = NULL;
	const char *base_expr = NULL;
	struct bytes image = { 0 };
	uint16_t base = 0;
	int i;
	memset(&as, 0, sizeof(as));
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
			if (++i >= argc)
				usage();
			output = argv[i];
		} else if (strcmp(argv[i], "--lst") == 0) {
			if (++i >= argc)
				usage();
			listing = argv[i];
		} else if (strcmp(argv[i], "--base") == 0) {
			if (++i >= argc)
				usage();
			base_expr = argv[i];
		} else if (strcmp(argv[i], "--case-sensitive") == 0) {
			as.case_sensitive = 1;
		} else if (strcmp(argv[i], "--define") == 0) {
			char *eq;
			if (++i >= argc)
				usage();
			eq = strchr(argv[i], '=');
			if (!eq)
				usage();
			*eq = '\0';
			init_symbols(&as);
			symbol_set(&as, argv[i], eval_expr(&as, eq + 1, 0, NULL));
			*eq = '=';
		} else if (!source) {
			source = argv[i];
		} else {
			usage();
		}
	}
	if (!source || !output)
		usage();
	if (!as.symbols.size)
		init_symbols(&as);
	if (!assemble(&as, source, base_expr, &image, &base)) {
		(void)base;
		fprintf(stderr, "gamecom-as: error: %s\n", as.error[0] ? as.error : "assembly failed");
		return 1;
	}
	write_file(output, image.data, image.size);
	if (listing)
		write_listing(&as, listing);
	return 0;
}
