#ifndef __SUB_H__
#define __SUB_H__

#include "cpu/exec/helper.h"

make_helper(sub_i2a_b);
make_helper(sub_i2rm_b);
make_helper(sub_r2rm_b);
make_helper(sub_rm2r_b);

make_helper(sub_i2a_v);
make_helper(sub_i2rm_v);
make_helper(sub_si2rm_v);
make_helper(sub_r2rm_v);
make_helper(sub_rm2r_v);


#endif //__SUB_H__