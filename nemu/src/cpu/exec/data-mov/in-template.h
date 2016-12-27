#include "cpu/exec/template-start.h"

#undef REG_IDX
#if DATA_BYTE == 4
#define REG_IDX R_EAX
#elif DATA_BYTE == 2
#define REG_IDX R_AX
#elif DATA_BYTE == 1
#define REG_IDX R_AL
#endif

make_helper(concat(in_im_, SUFFIX)) {
  ioaddr_t addr = instr_fetch(eip + 1, 1);
  REG(REG_IDX) = (DATA_TYPE)pio_read(addr, DATA_BYTE);
  print_asm("in ($0x%x),%s", addr, REG_NAME(REG_IDX));
  return 2;
}

make_helper(concat(in_, SUFFIX)) {
  ioaddr_t addr = reg_w(R_DX);
  REG(REG_IDX) = (DATA_TYPE)pio_read(addr, DATA_BYTE);
  print_asm("in (%s),%s", "%%dx", REG_NAME(REG_IDX));
  return 1;
}

#include "cpu/exec/template-end.h"
