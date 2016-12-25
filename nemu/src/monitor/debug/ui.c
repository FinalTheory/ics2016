#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"
#include "eval-flex.h"

#include <readline/readline.h>
#include <readline/history.h>
#include <nemu.h>

#define BYTE_PER_LINE 8


typedef union {
  struct {
    swaddr_t prev_epb;
    swaddr_t ret_addr;
    uint32_t args[4];
  };
  uint32_t data[6];
} stack_trace_t;


extern const char *query_address(swaddr_t addr);

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


static void print_cache_stat() {
    int i;
    uint64_t TLB_hit, TLB_total;
    uint64_t L1_hit, L1_total;
    uint64_t L2_hit, L2_total;
    TLB_get_stat(&TLB_hit, &TLB_total);
    get_stat_L1(&L1_hit, &L1_total);
    get_stat_L2(&L2_hit, &L2_total);
    puts("\nCache Statistics");
    for (i = 0; i < 70; i++) { putchar('-'); }
    puts("");
    printf("L1 cache\tHIT: %llu\tTOTAL: %llu\tRATE: %.6f%%\n",
           L1_hit, L1_total, (double)L1_hit / L1_total * 100.);
    printf("L2 cache\tHIT: %llu\tTOTAL: %llu\tRATE: %.6f%%\n",
           L2_hit, L2_total, (double)L2_hit / L2_total * 100.);
    printf("TLB cache\tHIT: %llu\tTOTAL: %llu\tRATE: %.6f%%\n",
           TLB_hit, TLB_total, (double)TLB_hit / TLB_total * 100.);
}

static int cmd_c(char* args) {
    cpu_exec(-1);
    return 0;
}

static int cmd_q(char* args) {
    print_cache_stat();
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
    int i, k;
    if (args == NULL) {
        puts("command \"info\" should have sub command.");
        return 0;
    }
    while (*args && *args == ' ') { args++; }
    if (*args == 'r') {
        display_cpu_status();
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
        printf(" %02x", swaddr_read(dest, R_DS, 1));
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

static int cmd_bt(char* args) {
    assert(sizeof(stack_trace_t) == 24);
    swaddr_t ebp = cpu.ebp;
    uint32_t eip = cpu.eip;
    stack_trace_t trace;
    int idx = 0, i;
    while (ebp) {
        for (i = 0; i < sizeof(stack_trace_t) / 4; i++) {
            trace.data[i] = swaddr_read(ebp + i * 4, R_SS, 4);
        }
        printf("#%d\t0x%08x in %s(", idx++,
               eip, query_address(eip));
        for (i = 0; i < 4; i++) {
            printf("%x, ", trace.args[i]);
        }
        puts("...)");
        ebp = trace.prev_epb;
        eip = trace.ret_addr;
    }
    return 0;
}

static int cmd_cache(char* args) {
    hwaddr_t addr;
    if (args == NULL || sscanf(args, "%x", &addr) != 1) {
        puts("Invalid argument.");
        return 0;
    }
    debug_search_L1(addr);
    debug_search_L2(addr);
    return 0;
}

static int cmd_page(char* args) {
    lnaddr_t addr;
    if (args == NULL || sscanf(args, "%x", &addr) != 1) {
        puts("Invalid argument.");
        return 0;
    }
    if (cpu.cr3.page_directory_base == 0) {
        puts("Page directory is not configured.");
        return 0;
    }
    int present;
    hwaddr_t res = page_translate(addr, &present);
    if (present) {
        printf("Physical ADDR: 0x%08x\n", res);
    } else {
        printf("Address is not mapped.\n");
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
    {"bt",   "Print stack trace",                                 cmd_bt},
    {"cache","Search memory address in L1/L2 cache",              cmd_cache},
    {"page", "Convert linear address to physical address",        cmd_page},
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
