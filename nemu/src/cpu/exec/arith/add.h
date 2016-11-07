#ifndef __ADD_H__
#define __ADD_H__

#include "cpu/exec/helper.h"

make_helper(add_i2a_b);
make_helper(add_i2rm_b);
make_helper(add_r2rm_b);
make_helper(add_rm2r_b);

make_helper(add_i2a_v);
make_helper(add_i2rm_v);
make_helper(add_si2rm_v);
make_helper(add_r2rm_v);
make_helper(add_rm2r_v);


#endif //__ADD_H__