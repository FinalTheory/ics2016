#pragma once

#include "nemu.h"

// Total 4MB cache
// 64 Byte per block
// 16 lines per group

#define GROUP_OFFSET_BITS 12

#define BLOCK_OFFSET_BITS 6

#define LINE_OFFSET_BITS  4

#define USE_DIRTY_BITS 1

#define LEVEL L2

#define read_func   dram_read

#define write_func  dram_write
