#include "cpu/exec/template-start.h"

#undef REG_IDX
#if DATA_BYTE == 4
#define REG_IDX R_EAX
#elif DATA_BYTE == 2
#define REG_IDX R_AX
#elif DATA_BYTE == 1
#define REG_IDX R_AL
#endif

make_helper(concat(out_im_, SUFFIX)) {
  ioaddr_t addr = instr_fetch(eip + 1, 1);
  pio_write(addr, DATA_BYTE, REG(REG_IDX));
  print_asm("out %s,($0x%x)", REG_NAME(REG_IDX), addr);
  return 2;
}

make_helper(concat(out_, SUFFIX)) {
  pio_write(reg_w(R_DX), DATA_BYTE, REG(REG_IDX));
  print_asm("out %s,(%s)", REG_NAME(REG_IDX), "%%dx");
  return 1;
}

#include "cpu/exec/template-end.h"
