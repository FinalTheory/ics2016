#ifndef __PUSH_H__
#define __PUSH_H__

#include "cpu/exec/helper.h"

make_helper(push_i_v);
make_helper(push_si_v);
make_helper(push_r_v);
make_helper(push_rm_v);
make_helper(pusha_v);

#endif //__PUSH_H__
