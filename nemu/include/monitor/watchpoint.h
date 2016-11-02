#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
    int NO;
    struct watchpoint* next;
    char* expr;
    int32_t last_val;
} WP;

int check_watch_points();

void display_watch_points();

int delete_watch_point(int NO);

WP* new_wp();

void free_wp(WP* wp);

#endif
