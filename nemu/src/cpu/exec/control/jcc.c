#include "cpu/exec/helper.h"


#define instr ja
#define condition (!cpu.eflags.CF && !cpu.eflags.ZF)

#include "jcc-template-helper.h"

#undef condition
#undef instr

#define instr jae
#define condition (!cpu.eflags.CF)

#include "jcc-template-helper.h"

#undef condition
#undef instr

#define instr jb
#define condition (cpu.eflags.CF)

#include "jcc-template-helper.h"

#undef condition
#undef instr

#define instr jbe
#define condition (cpu.eflags.CF || cpu.eflags.ZF)

#include "jcc-template-helper.h"

#undef condition
#undef instr

#define instr jg
#define condition (!cpu.eflags.ZF && cpu.eflags.SF == cpu.eflags.OF)

#include "jcc-template-helper.h"

#undef condition
#undef instr

#define instr jge
#define condition (cpu.eflags.SF == cpu.eflags.OF)

#include "jcc-template-helper.h"

#undef condition
#undef instr

#define instr jl
#define condition (cpu.eflags.SF != cpu.eflags.OF)

#include "jcc-template-helper.h"

#undef condition
#undef instr

#define instr jle
#define condition (cpu.eflags.ZF || cpu.eflags.SF != cpu.eflags.OF)

#include "jcc-template-helper.h"

#undef condition
#undef instr

#define instr jne
#define condition (!cpu.eflags.ZF)

#include "jcc-template-helper.h"

#undef condition
#undef instr

#define instr jno
#define condition (!cpu.eflags.OF)

#include "jcc-template-helper.h"

#undef condition
#undef instr

#define instr jnp
#define condition (!cpu.eflags.PF)

#include "jcc-template-helper.h"

#undef condition
#undef instr

#define instr jns
#define condition (!cpu.eflags.SF)

#include "jcc-template-helper.h"

#undef condition
#undef instr

#define instr je
#define condition (cpu.eflags.ZF)

#include "jcc-template-helper.h"

#undef condition
#undef instr

#define instr jo
#define condition (cpu.eflags.OF)

#include "jcc-template-helper.h"

#undef condition
#undef instr

#define instr jp
#define condition (cpu.eflags.PF)

#include "jcc-template-helper.h"

#undef condition
#undef instr

#define instr js
#define condition (cpu.eflags.SF)

#include "jcc-template-helper.h"

#undef condition
#undef instr


#define instr jcxz_w
#define condition (reg_w(R_CX) == 0)
#define DATA_BYTE 1
#include "jcc-template.h"
#undef DATA_BYTE
#undef condition
#undef instr

#define instr jcxz_l
#define condition (reg_l(R_ECX) == 0)
#define DATA_BYTE 1
#include "jcc-template.h"
#undef DATA_BYTE
#undef condition
#undef instr

make_helper(jcxz_v) {
    return (ops_decoded.is_operand_size_16 ?
            concat3(jcxz, _w, _i_b) : concat3(jcxz, _l, _i_b))(eip);
}
