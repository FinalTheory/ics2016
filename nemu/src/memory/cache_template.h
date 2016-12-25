#pragma once

#include <stdlib.h>

#define make_func(name) concat3(name, _, LEVEL)

#define cache_name concat3(LEVEL, _, cache)

#define cache_hit concat(cache_name, _hit)

#define cache_total concat(cache_name, _total)

#define BLOCK_SIZE        (1 << BLOCK_OFFSET_BITS)

#define NUM_GROUPS        (1 << GROUP_OFFSET_BITS)

#define NUM_LINES         (1 << LINE_OFFSET_BITS)

#define MASK_LOW_BITS(n) (~0u >> (32 - n))

typedef struct {
  uint32_t blk_ofst : BLOCK_OFFSET_BITS;
  uint32_t grp_ofst : GROUP_OFFSET_BITS;
  uint32_t flag : 32 - BLOCK_OFFSET_BITS - GROUP_OFFSET_BITS;
} cache_addr_t;

typedef struct {
  uint32_t flag;
  uint8_t block[BLOCK_SIZE];
#if USE_DIRTY_BITS
  uint8_t dirty : 4;
  uint8_t valid : 4;
#else
  uint8_t valid;
#endif
} cache_line_t;

typedef struct {
  cache_line_t lines[NUM_LINES];
} cache_group_t;

static struct {
  cache_group_t groups[NUM_GROUPS];
} cache_name;

static uint64_t cache_hit = 0;
static uint64_t cache_total = 0;

void make_func(get_stat)(uint64_t *hit, uint64_t *total) {
  *hit = cache_hit;
  *total = cache_total;
}

#define CACHE_DECODE_ADDR \
  cache_addr_t *cache_addr = (void *)&addr; \
  uint32_t blk_ofst = cache_addr->blk_ofst; \
  uint32_t grp_ofst = cache_addr->grp_ofst; \
  uint32_t flag = cache_addr->flag; \
  cache_group_t *grp = &cache_name.groups[grp_ofst];


void make_func(init)() {
  int i, j;
  for (i = 0; i < NUM_GROUPS; i++) {
    cache_group_t *grp = &cache_name.groups[i];
    for (j = 0; j < NUM_LINES; j++) {
      cache_line_t *line = &grp->lines[j];
      line->valid = false;
#if USE_DIRTY_BITS
      line->dirty = false;
#endif
    }
  }
}

static inline
cache_line_t *make_func(search)(hwaddr_t addr) {
  cache_total++;
  CACHE_DECODE_ADDR
  int i;
  for (i = 0; i < NUM_LINES; i++) {
    cache_line_t *line = &grp->lines[i];
    if (line->valid && line->flag == flag) {
      cache_hit++;
      return line;
    }
  }
  return NULL;
}

static inline
cache_line_t *make_func(alloc)(hwaddr_t addr) {
  CACHE_DECODE_ADDR
  int line_ofst = rand() % NUM_LINES, i;
  cache_line_t *line = &grp->lines[line_ofst];
  addr &= ~(MASK_LOW_BITS(BLOCK_OFFSET_BITS));
  uint32_t *block = (void *) line->block;
#if USE_DIRTY_BITS
  // If there is a dirty flag
  // we must first write this block back
  if (line->valid && line->dirty) {
    uint32_t line_addr = (grp_ofst << BLOCK_OFFSET_BITS) |
            (line->flag << (GROUP_OFFSET_BITS + BLOCK_OFFSET_BITS));
    for (i = 0; i < BLOCK_SIZE / sizeof(uint32_t); i++) {
      write_func(line_addr + i * 4, 4, block[i]);
    }
    line->dirty = false;
  }
#endif
  line->valid = true;
  line->flag = flag;
  for (i = 0; i < BLOCK_SIZE / sizeof(uint32_t); i++) {
    block[i] = read_func(addr + i * 4, 4);
  }
  return line;
}

static inline
cache_line_t *make_func(search_and_alloc)(hwaddr_t addr) {
  cache_line_t *line = make_func(search)(addr);
  if (!line) { line = make_func(alloc)(addr); }
  return line;
}

static inline
void make_func(read_internal)(cache_line_t *line, uint32_t blk_ofst,
                              size_t len, uint8_t *dest) {
  int i;
  uint8_t *block = line->block;
  for (i = 0; i < len; i++) {
    dest[i] = block[blk_ofst + i];
  }
}

static inline
void make_func(write_internal)(cache_line_t *line, uint32_t blk_ofst,
                               size_t len, uint8_t *data) {
#if USE_DIRTY_BITS
  line->dirty = true;
#endif
  int i;
  uint8_t *block = line->block;
  for (i = 0; i < len; i++) {
    block[blk_ofst + i] = data[i];
  }
}

uint32_t make_func(read)(hwaddr_t addr, size_t len) {
  CACHE_DECODE_ADDR
  uint32_t data = 0;
  size_t blk_rem = BLOCK_SIZE - blk_ofst;
  cache_line_t *line1 = make_func(search_and_alloc)(addr);
  if (len <= blk_rem) {
    make_func(read_internal)(line1, blk_ofst, len, (uint8_t *) &data);
  } else {
    make_func(read_internal)(line1, blk_ofst, blk_rem, (uint8_t *) &data);
    cache_line_t *line2 = make_func(search_and_alloc)((hwaddr_t) (addr + blk_rem));
    make_func(read_internal)(line2, 0, len - blk_rem, (uint8_t *) &data + blk_rem);
  }
  return data;
}

void make_func(write)(hwaddr_t addr, size_t len, uint32_t data) {
  CACHE_DECODE_ADDR
  // Write back, thus do nothing here
  size_t blk_rem = BLOCK_SIZE - blk_ofst;

#if USE_DIRTY_BITS

  // Here we search a cache line
  // If not found, we automatically allocate a new line
  cache_line_t *line1 = make_func(search_and_alloc)(addr);
  if (len <= blk_rem) {
    make_func(write_internal)(line1, blk_ofst, len, (uint8_t *) &data);
  } else {
    make_func(write_internal)(line1, blk_ofst, blk_rem, (uint8_t *) &data);
    cache_line_t *line2 = make_func(search_and_alloc)((hwaddr_t) (addr + blk_rem));
    make_func(write_internal)(line2, 0, len - blk_rem, (uint8_t *) &data + blk_rem);
  }

#else

  // Write through
  write_func(addr, len, data);
  // Update cache if hit
  cache_line_t *line1 = make_func(search)(addr);
  if (len <= blk_rem) {
    if (line1) { make_func(write_internal)(line1, blk_ofst, len, (uint8_t *) &data); }
  } else {
    if (line1) { make_func(write_internal)(line1, blk_ofst, blk_rem, (uint8_t *) &data); }
    cache_line_t *line2 = make_func(search)((hwaddr_t) (addr + blk_rem));
    if (line2) { make_func(write_internal)(line2, 0, len - blk_rem, (uint8_t *) &data + blk_rem); }
  }

#endif
}

int make_func(debug_search)(hwaddr_t addr) {
  cache_line_t *line = make_func(search)(addr);
  if (line) {
    printf(str(LEVEL) " cache: ADDR[0x%08x] FLAG[0x%08x] valid(%u)"
#if USE_DIRTY_BITS
                      " dirty(%u)\n",
#else
                      "\n",
#endif
           addr, line->flag, line->valid
#if USE_DIRTY_BITS
    , line->dirty);
#else
    );
#endif
    return 1;
  } else {
    printf("Address 0x%08x not found in " str(LEVEL) " cache.\n", addr);
    return 0;
  }
}
