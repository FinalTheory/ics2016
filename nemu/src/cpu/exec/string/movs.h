#ifndef __MOVS_H__
#define __MOVS_H__

#include "cpu/exec/helper.h"

make_helper(movs_b);
make_helper(movs_v);

make_helper(cld);
make_helper(std);

#endif //__MOVS_H__
