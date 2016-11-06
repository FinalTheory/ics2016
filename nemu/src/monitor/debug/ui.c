#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"
#include "eval-flex.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

#define BYTE_PER_LINE 8

void cpu_exec(uint32_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
    static char* line_read = NULL;

    if (line_read) {
        free(line_read);
        line_read = NULL;
    }

    line_read = readline("(nemu) ");

    if (line_read && *line_read) {
        add_history(line_read);
    }

    return line_read;
}

extern int yyparse();

extern int32_t eval_result;

extern int eval_error;

static char* g_buffer = NULL;

static int g_offset = 0;

int read_input_for_lexer(char* buffer, yy_size_t* bytes_read, int len) {
    size_t bytes_to_read = (size_t)len;
    int bytes_left = (int)strlen(g_buffer) - g_offset;
    int i;
    if (bytes_to_read > bytes_left) {
        bytes_to_read = (size_t)bytes_left;
    }
    for (i = 0; i < bytes_to_read; i++) {
        buffer[i] = g_buffer[g_offset + i];
    }
    *bytes_read = (yy_size_t)bytes_to_read;
    g_offset += bytes_to_read;
    return 0;
}

int32_t eval(char* expr, int32_t* res) {
    if (expr == NULL || *expr == 0) {
        puts("Should input expression.");
        return -1;
    }
    g_buffer = strdup(expr);
    g_offset = 0;
    eval_error = 0;
    eval_result = 0;
    yyscan_t lexer;
    yylex_init(&lexer);
    yyparse();
    *res = eval_result;
    yylex_destroy(lexer);
    free(g_buffer);
    g_buffer = NULL;
    if (eval_error) { return -1; }
    return 0;
}


static int cmd_c(char* args) {
    cpu_exec(-1);
    return 0;
}

static int cmd_q(char* args) {
    return -1;
}

static int cmd_help(char* args);

static int cmd_si(char* args) {
    int n = 1;
    if (args != NULL) {
        sscanf(args, "%d", &n);
        printf("si = %d\n", n);
    }
    cpu_exec((uint32_t)n);
    return 0;
}

static int cmd_info(char* args) {
    int i;
    if (args == NULL) {
        puts("command \"info\" should have sub command.");
        return 0;
    }
    while (*args && *args == ' ') { args++; }
    if (*args == 'r') {
        for (i = 0; i < 8; i += 2) {
            printf("%s: 0x%08x\t%s: 0x%08x\n",
                   regsl[i], cpu.gpr[i]._32,
                   regsl[i + 1], cpu.gpr[i + 1]._32);
        }
        printf("eip: 0x%08x\tEFLAGS: 0x%08x\n",
               cpu.eip, cpu.eflags.val);
        printf("EFLAGS: CF\tPF\tZF\tSF\tIF\tDF\tOF\n");
        printf("        %2d\t%2d\t%2d\t%2d\t%2d\t%2d\t%2d\n",
               cpu.eflags.CF, cpu.eflags.PF, cpu.eflags.ZF,
               cpu.eflags.SF, cpu.eflags.IF,
               cpu.eflags.DF, cpu.eflags.OF);
    } else if (*args == 'w') {
        display_watch_points();
    } else {
        puts("Not implemented!");
    }
    return 0;
}

static int cmd_p(char* args) {
    int32_t res;
    if (eval(args, &res) == 0) {
        printf("0x%08x\n", res);
    }
    return 0;
}

static int cmd_x(char* args) {
    int32_t res;
    int n, i;
    char* pos;
    char* num = strtok_r(args, " ", &pos);
    char* expr = num + strlen(num) + 1;
    sscanf(num, "%d", &n);
    if (eval(expr, &res) == -1) { return 0; }
    swaddr_t addr = (swaddr_t)res;
    for (i = 0; i < n * sizeof(uint32_t); i++) {
        uint32_t dest = addr + i;
        if (i % BYTE_PER_LINE == 0) { printf("0x%08x:", dest); }
        printf(" %02x", swaddr_read(dest, 1));
        if ((i + 1) % BYTE_PER_LINE == 0) { puts(""); }
    }
    if ((i % BYTE_PER_LINE)) { puts(""); }
    return 0;
}

static int cmd_w(char* args) {
    int32_t res;
    char *expr = strdup(args);
    if (eval(args, &res) == 0) {
        WP* node = new_wp();
        node->expr = expr;
        node->last_val = res;
    } else {
        puts("Add watch point failed: invalid expression.");
    }
    return 0;
}

static int cmd_d(char* args) {
    int NO;
    sscanf(args, "%d", &NO);
    if (delete_watch_point(NO) != 0) {
        printf("Failed to delete watch point %d.\n", NO);
    }
    return 0;
}

static struct {
    char* name;
    char* description;

    int (* handler)(char*);
} cmd_table[] = {
    {"help", "Display informations about all supported commands", cmd_help},
    {"c",    "Continue the execution of the program",             cmd_c},
    {"q",    "Exit NEMU",                                         cmd_q},
    {"si",   "Step in for N steps",                               cmd_si},
    {"info", "Print information",                                 cmd_info},
    {"p",    "Evaluate expression",                               cmd_p},
    {"x",    "Print memory value",                                cmd_x},
    {"w",    "Add watch point",                                   cmd_w},
    {"d",    "Delete watch point",                                cmd_d},
    {"bt",   "Print stack trace", NULL},
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char* args) {
    /* extract the first argument */
    char* arg = strtok(NULL, " ");
    int i;

    if (arg == NULL) {
        /* no argument given */
        for (i = 0; i < NR_CMD; i++) {
            printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        }
    } else {
        for (i = 0; i < NR_CMD; i++) {
            if (strcmp(arg, cmd_table[i].name) == 0) {
                printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
                return 0;
            }
        }
        printf("Unknown command '%s'\n", arg);
    }
    return 0;
}

void ui_mainloop() {
    while (1) {
        char* str = rl_gets();
        char* str_end = str + strlen(str);

        /* extract the first token as the command */
        char* cmd = strtok(str, " ");
        if (cmd == NULL) { continue; }

        /* treat the remaining string as the arguments,
         * which may need further parsing
         */
        char* args = cmd + strlen(cmd) + 1;
        if (args >= str_end) {
            args = NULL;
        }

#ifdef HAS_DEVICE
        extern void sdl_clear_event_queue(void);
        sdl_clear_event_queue();
#endif

        int i;
        for (i = 0; i < NR_CMD; i++) {
            if (strcmp(cmd, cmd_table[i].name) == 0) {
                if (cmd_table[i].handler(args) < 0) { return; }
                break;
            }
        }

        if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
    }
}
