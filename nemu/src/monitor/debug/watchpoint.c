#include "monitor/watchpoint.h"
#include "monitor/expr.h"
#include <stdlib.h>

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP* head, * free_;

extern int32_t eval(char* expr, int32_t* res);

void init_wp_pool() {
    int i;
    for (i = 0; i < NR_WP; i++) {
        wp_pool[i].NO = i;
        wp_pool[i].next = &wp_pool[i + 1];
    }
    wp_pool[NR_WP - 1].next = NULL;

    head = NULL;
    free_ = wp_pool;
}

int check_watch_points() {
    int32_t res;
    WP* cur = head;
    while (cur != NULL) {
        eval(cur->expr, &res);
        if (res != cur->last_val) {
            printf("Break on No.%d, old = 0x%08x, new = 0x%08x\n",
                   cur->NO, cur->last_val, res);
            cur->last_val = res;
            return 1;
        }
        cur = cur->next;
    }
    return 0;
}

void display_watch_points() {
    WP* cur = head;
    puts("Watch points:");
    puts("Index | Last Value | Expression");
    puts("-------------------------------");
    while (cur != NULL) {
        printf("No.%2d | 0x%08x | %s\n",
               cur->NO, cur->last_val, cur->expr);
        cur = cur->next;
    }
}

int delete_watch_point(int NO) {
    WP* cur = head;
    while (cur != NULL) {
        if (NO == cur->NO) {
            free_wp(cur);
            return 0;
        }
        cur = cur->next;
    }
    return -1;
}

WP* new_wp() {
    Assert(free_ != NULL, "No empty watch point!");
    WP* res = free_;
    free_ = free_->next;
    res->next = head;
    head = res;
    return res;
}

void free_wp(WP* wp) {
    if (wp == head) {
        head = head->next;
    } else {
        WP* prev = head;
        while (prev && prev->next != wp) {
            prev = prev->next;
        }
        Assert(prev != NULL && prev->next == wp,
               "Watch point not found!");
        prev->next = wp->next;
    }
    wp->next = free_;
    free_ = wp;
    free(wp->expr);
}
