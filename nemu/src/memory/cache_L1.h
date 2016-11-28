#pragma once

#include "nemu.h"

// Total 64KB cache
// 64 Byte per block
// 8 lines per group

#define GROUP_OFFSET_BITS 7

#define BLOCK_OFFSET_BITS 6

#define LINE_OFFSET_BITS  3

#define USE_DIRTY_BITS 0

#define LEVEL L1

#define read_func   read_L2

#define write_func  write_L2
