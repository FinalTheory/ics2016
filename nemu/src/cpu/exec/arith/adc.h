#ifndef __ADC_H__
#define __ADC_H__

#include "cpu/exec/helper.h"

make_helper(adc_i2a_b);
make_helper(adc_i2rm_b);
make_helper(adc_r2rm_b);
make_helper(adc_rm2r_b);

make_helper(adc_i2a_v);
make_helper(adc_i2rm_v);
make_helper(adc_si2rm_v);
make_helper(adc_r2rm_v);
make_helper(adc_rm2r_v);


#endif //__ADC_H__
