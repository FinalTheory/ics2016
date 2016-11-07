#ifndef __MOVSZX_H__
#define __MOVSZX_H__

#include "cpu/exec/helper.h"

make_helper(movsx_rm2r_b_v);
make_helper(movsx_rm2r_w_v);

make_helper(movzx_rm2r_b_v);
make_helper(movzx_rm2r_w_v);

#endif //__MOVSZX_H__
