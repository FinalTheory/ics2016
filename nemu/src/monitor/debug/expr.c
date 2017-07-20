#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

#define DEBUG_EVAL 0

extern swaddr_t query_symbol(const char* str);

enum {
	NOTYPE = 256,
  EQ = 128, NEQ, INT, HEX, SYM, REG, AND, OR, DEREF, NEG
};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {
	/*
	 * Pay attention to the precedence level of different rules.
	 */
	{"0x[0-9a-f]{1,8}", HEX},
  {"[0-9]+", INT},
	{"\\$((e?(([a-d]x|sp|bp|si|di)|([a-d](h|l))))|(eflags|eip))", REG},
  {"[_a-zA-Z]+[_a-zA-Z0-9]*", SYM},
	{"==", EQ},
	{"\\!=", NEQ},
	{"&&", AND},
	{"\\|\\|", OR},
  {"\\*", '*'},
  {"\\/", '/'},
  {"\\+", '+'},
  {"\\-", '-'},
  {"\\(", '('},
  {"\\)", ')'},
	{"\\!", '!'},
	{"[ \\t\\n\\r]+", NOTYPE},
};

static int op_priority[255];

void init_op_priority() {
	op_priority[OR] = 4;
	op_priority[AND] = 5;
	op_priority[EQ] = 6;
	op_priority[NEQ] = 6;
	op_priority['+'] = 7;
	op_priority['-'] = 7;
	op_priority['*'] = 8;
	op_priority['/'] = 8;
	op_priority[DEREF] = 9;
	op_priority['!'] = 10;
	op_priority[NEG] = 10;
}

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	init_op_priority();
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	uint32_t value;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static int is_var(int type) {
	return (type == SYM ||
					type == INT ||
					type == HEX ||
					type == REG);
}

static int is_unary(int type) {
	return (type == '!' ||
					type == NEG ||
					type == DEREF);
}

static bool read_reg_value(char* reg, uint32_t* value) {
	size_t len = strlen(reg);
	for (int i = 0; i < 8; i++) {
		if (strncmp(reg, regsl[i], len) == 0) {
			*value = reg_l(i);
			return true;
		} else if (strncmp(reg, regsw[i], len) == 0) {
			*value = reg_w(i);
			return true;
		} else if (strncmp(reg, regsb[i], len) == 0) {
			*value = reg_b(i);
			return true;
		}
	}
	if (strncmp(reg, "eip", len) == 0) {
		*value = cpu.eip;
		return true;
	} else if (strncmp(reg, "eflags", len) == 0) {
		*value = cpu.eflags.val;
		return true;
	}
	printf("Invalid register name %s\n", tokens[nr_token].str);
	return false;
}

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

#if DEBUG_EVAL
				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
						i, rules[i].regex, position, substr_len, substr_len, substr_start);
#endif
				position += substr_len;

				/* Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */
				
				// save token string and token type
				memcpy(tokens[nr_token].str, substr_start, substr_len);
				tokens[nr_token].str[substr_len] = '\0';
				tokens[nr_token].type = rules[i].token_type;
				uint32_t* value = &tokens[nr_token].value;
				*value = 0;
				switch(rules[i].token_type) {
					case INT:
						sscanf(tokens[nr_token].str, "%u", value);
						break;
					case HEX:
						sscanf(tokens[nr_token].str, "%x", value);
						break;
					case SYM:
						*value = query_symbol(tokens[nr_token].str);
						break;
					case REG:
						if (!read_reg_value(tokens[nr_token].str + 1, value)) {
							Log("Invalid reg expression %s", tokens[nr_token].str);
							return false;
						}
						break;
					default:
						// other symbols, do nothing
						break;
				}
				nr_token++;
				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true; 
}

static char eval_error_buf[512];

const char* eval_error() {
	return eval_error_buf;
}

int stk_dat[1024];
int stk_ptr = 0;

static bool stack_empty() {
	return (bool)(stk_ptr == 0);
}

static void stack_reset() {
	stk_ptr = 0;
}

static void stack_push(int v) {
	stk_dat[stk_ptr++] = v;
}

static int stack_pop() {
	Assert(stk_ptr > 0, "Trying to pop empty stack.");
	return stk_dat[--stk_ptr];
}

static bool match_parentheses(int start, int end) {
	stack_reset();
	for (int i = start; i < end; i++) {
		int t = tokens[i].type;
		if (t == '(') {
			stack_push(t);
		} else if (t == ')') {
			if (stack_empty()) {
				return false;
			} else {
				stack_pop();
			}
		}
	}
	return stack_empty();
}

// return -1 if not found
static int find_dominant(int start, int end) {
	stack_reset();
	int pos = -1;
	int pri = 100;
	for (int i = start; i < end; i++) {
		int t = tokens[i].type;
		if (t == NOTYPE || is_var(t)) { continue; }
		if (t == '(') {
			stack_push(t);
		} else if (t == ')') {
			stack_pop();
		} else {
			// not in parentheses
			if (stack_empty()) {
				if (op_priority[t] <= pri) {
					pri = op_priority[t];
					pos = i;
				}
			}
		}
	}
	return pos;
}

static uint32_t eval_expr(int start, int end, bool* status) {
#if DEBUG_EVAL
  printf("[%d %d]\n", start, end);
#endif
	if (start >= end) {
		sprintf(eval_error_buf, "invalid eval range [%d, %d]", start, end);
		*status = false;
		return 0;
	} else if (start + 1 == end) {
		if (is_var(tokens[start].type)) {
			*status = true;
			return tokens[start].value;
		} else {
			sprintf(eval_error_buf, "invalid token type %d", tokens[start].type);
			*status = false;
			return 0;
		}
	} else if (tokens[start].type == '(' &&
						 tokens[end - 1].type == ')' &&
						 match_parentheses(start + 1, end - 1)) {
		return eval_expr(start + 1, end - 1, status);
	} else {
		int p = find_dominant(start, end);
		if (p == -1) {
			sprintf(eval_error_buf, "dominant operator not found");
			*status = false;
			return 0;
		}
		int t = tokens[p].type;
		if (is_unary(t)) {
			bool st = false;
			uint32_t v = eval_expr(p + 1, end, &st);
			if (st == false) {
				*status = false;
				return 0;
			}
			*status = true;
			switch (t) {
				case '!':
					return !v;
				case NEG:
					return -v;
				case DEREF:
					return swaddr_read((swaddr_t)v, R_DS, sizeof(uint32_t));;
				default:
					*status = false;
					return 0;
			}
		} else {
			bool st1 = false, st2 = false;
			uint32_t v1 = eval_expr(start, p, &st1);
			uint32_t v2 = eval_expr(p + 1, end, &st2);
			if (st1 == false || st2 == false) {
				*status = false;
				return 0;
			}
			*status = true;
			switch (t) {
				case '+':
					return v1 + v2;
				case '-':
					return v1 - v2;
				case '*':
					return v1 * v2;
				case '/':
					return v1 / v2;
				case AND:
					return v1 && v2;
				case OR:
					return v1 || v2;
				case EQ:
					return v1 == v2;
				case NEQ:
					return v1 != v2;
				default:
					sprintf(eval_error_buf, "unknown operator type %d", t);
					*status = false;
					return 0;
			}
		}
	};
}

static void process_unary() {
	for (int i = 0; i < nr_token; i++) {
		// no previous token
		// or previous token is nether a var nor a right parenthesis
		if (i == 0 ||
				(!is_var(tokens[i - 1].type) &&
				 tokens[i - 1].type != ')' )) {
			if (tokens[i].type == '-') {
				tokens[i].type = NEG;
			}
			if (tokens[i].type == '*') {
				tokens[i].type = DEREF;
			}
		}
	}
}

static void remove_notype() {
  int i = 0;
  for (int k = 0; k < nr_token; k++) {
    if (tokens[k].type != NOTYPE) {
      tokens[i++] = tokens[k];
    }
  }
  nr_token = i;
}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}

	remove_notype();

	process_unary();

	if (!match_parentheses(0, nr_token)) {
		*success = false;
    sprintf(eval_error_buf, "expression parentheses do not match");
		return 0;
	}

	return eval_expr(0, nr_token, success);
}

void test_expr(int argc, char *argv[]) {
	char *fname = NULL;
	for (int i = 0; i < argc - 1; i++) {
		if (strcmp(argv[i], "-t") == 0) {
			fname = argv[i + 1];
			break;
		}
	}
	if (fname == NULL) { return; }
	FILE *fp = fopen(fname, "r");
	Assert(fp != NULL, "open test file %s failed.", fname);
	char buf[1024];
	uint32_t ans, res;
	bool status = false;
	while (NULL != fgets(buf, sizeof(buf), fp)) {
		if (buf[0] == '\n') continue;
		buf[strlen(buf) -  1] = '\x00';
		fscanf(fp, "%u", &ans);
		res = expr(buf, &status);
		Assert(status == true, "eval failed: %s", eval_error());
		Assert(ans == res, "result mismatch, expected=%u, actual=%u", ans, res);
	}
}
