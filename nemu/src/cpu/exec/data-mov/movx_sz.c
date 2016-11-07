#include "cpu/exec/helper.h"


#define DATA_BYTE 1

#define DEST_BYTE 2
#include "movx_sz-template.h"
#undef DEST_BYTE

#define DEST_BYTE 4
#include "movx_sz-template.h"
#undef DEST_BYTE

#undef DATA_BYTE


#define DATA_BYTE 2

#define DEST_BYTE 2
#include "movx_sz-template.h"
#undef DEST_BYTE

#define DEST_BYTE 4
#include "movx_sz-template.h"
#undef DEST_BYTE

#undef DATA_BYTE


make_helper_v(movsx_rm2r_b);
make_helper_v(movsx_rm2r_w);

make_helper_v(movzx_rm2r_b);
make_helper_v(movzx_rm2r_w);
