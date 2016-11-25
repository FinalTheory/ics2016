#include <stdio.h>
#include <stdint.h>
#include "FLOAT.h"

extern char _vfprintf_internal;
extern char _ppfs_setargs;
extern char _fpmaxtostr;
extern int __stdio_fwrite(char *buf, int len, FILE *stream);

__attribute__((used)) static int format_FLOAT(FILE *stream, FLOAT f) {
	/* Format a FLOAT argument `f' and write the formating
	 * result to `stream'. Keep the precision of the formating
	 * result with 6 by truncating. For example:
	 *              f          result
	 *         0x00010000    "1.000000"
	 *         0x00013333    "1.199996"
	 */

	/*
	 * Sign flag
	 */
	char __buf[80], *str;
	if (f >= 0) {
		str = __buf;
	} else {
		__buf[0] = '-';
		str = __buf + 1;
		f = -f;
	}
	/*
	 * Positive part
	 */
	int base = 0x00010000;
	int positive = f / base;
	f -= positive * base;
	int l = sprintf(str, "%d", positive), n = 6;
	/*
	 * float part
	 */
	str[l++] = '.';
	while (n--) {
		f *= 10;
		int bit = f / base;
		f -= bit * base;
		str[l++] = (char)(bit + '0');
	}
	str[l++] = 0;
	return __stdio_fwrite(__buf, l, stream);
}

static void modify_vfprintf() {
	/* This function to hijack the formating of "%f"
	 * argument during the execution of `_vfprintf_internal'. Below
	 * is the code section in _vfprintf_internal() relative to the
	 * hijack.
	 */

#if 0
	else if (ppfs->conv_num <= CONV_A) {  /* floating point */
		ssize_t nf;
		nf = _fpmaxtostr(stream,
				(__fpmax_t)
				(PRINT_INFO_FLAG_VAL(&(ppfs->info),is_long_double)
				 ? *(long double *) *argptr
				 : (long double) (* (double *) *argptr)),
				&ppfs->info, FP_OUT );
		if (nf < 0) {
			return -1;
		}
		*count += nf;

		return 0;
	} else if (ppfs->conv_num <= CONV_S) {  /* wide char or string */
#endif

	// Dirty hack here
	const int off_fldt = 0x02e4;
	const int off_fldl = 0x02e8;
	const int off_fstp = 0x02fc;
	const int off_call = 0x0306;

	const uint8_t nop = (uint8_t)0x90;

	uint32_t *p1 = (void *)((uint32_t)&_vfprintf_internal + off_call + 1);

	int8_t *p2 = (void *)((uint32_t)&_vfprintf_internal + off_fstp);

	int8_t *p3 = (void *)((uint32_t)&_vfprintf_internal + off_fldt);

	int8_t *p4 = (void *)((uint32_t)&_vfprintf_internal + off_fldl);

	*p1 += (int)&format_FLOAT - (int)&_fpmaxtostr;

	// shrink stack
	p2[-1] -= 4;
	// pushl (%edx)
	p2[0] = (uint8_t)0xff;
	p2[1] = (uint8_t)0x32;
	// fill nops
	p2[2] = nop;
	p3[0] = p3[1] = nop;
	p4[0] = p4[1] = nop;

	/* You should modify the run-time binary to let the code above
	 * call `format_FLOAT' defined in this source file, instead of
	 * `_fpmaxtostr'. When this function returns, the action of the
	 * code above should do the following:
	 */

#if 0
	else if (ppfs->conv_num <= CONV_A) {  /* floating point */
		ssize_t nf;
		nf = format_FLOAT(stream, *(FLOAT *) *argptr);
		if (nf < 0) {
			return -1;
		}
		*count += nf;

		return 0;
	} else if (ppfs->conv_num <= CONV_S) {  /* wide char or string */
#endif

}

static void modify_ppfs_setargs() {
	/* Implement this function to modify the action of preparing
	 * "%f" arguments for _vfprintf_internal() in _ppfs_setargs().
	 * Below is the code section in _vfprintf_internal() relative to
	 * the modification.
	 */

#if 0
	enum {                          /* C type: */
		PA_INT,                       /* int */
		PA_CHAR,                      /* int, cast to char */
		PA_WCHAR,                     /* wide char */
		PA_STRING,                    /* const char *, a '\0'-terminated string */
		PA_WSTRING,                   /* const wchar_t *, wide character string */
		PA_POINTER,                   /* void * */
		PA_FLOAT,                     /* float */
		PA_DOUBLE,                    /* double */
		__PA_NOARG,                   /* non-glibc -- signals non-arg width or prec */
		PA_LAST
	};

	/* Flag bits that can be set in a type returned by `parse_printf_format'.  */
	/* WARNING -- These differ in value from what glibc uses. */
#define PA_FLAG_MASK		(0xff00)
#define __PA_FLAG_CHAR		(0x0100) /* non-gnu -- to deal with hh */
#define PA_FLAG_SHORT		(0x0200)
#define PA_FLAG_LONG		(0x0400)
#define PA_FLAG_LONG_LONG	(0x0800)
#define PA_FLAG_LONG_DOUBLE	PA_FLAG_LONG_LONG
#define PA_FLAG_PTR		(0x1000) /* TODO -- make dynamic??? */

	while (i < ppfs->num_data_args) {
		switch(ppfs->argtype[i++]) {
			case (PA_INT|PA_FLAG_LONG_LONG):
				GET_VA_ARG(p,ull,unsigned long long,ppfs->arg);
				break;
			case (PA_INT|PA_FLAG_LONG):
				GET_VA_ARG(p,ul,unsigned long,ppfs->arg);
				break;
			case PA_CHAR:	/* TODO - be careful */
				/* ... users could use above and really want below!! */
			case (PA_INT|__PA_FLAG_CHAR):/* TODO -- translate this!!! */
			case (PA_INT|PA_FLAG_SHORT):
			case PA_INT:
				GET_VA_ARG(p,u,unsigned int,ppfs->arg);
				break;
			case PA_WCHAR:	/* TODO -- assume int? */
				/* we're assuming wchar_t is at least an int */
				GET_VA_ARG(p,wc,wchar_t,ppfs->arg);
				break;
				/* PA_FLOAT */
			case PA_DOUBLE:
				GET_VA_ARG(p,d,double,ppfs->arg);
				break;
			case (PA_DOUBLE|PA_FLAG_LONG_DOUBLE):
				GET_VA_ARG(p,ld,long double,ppfs->arg);
				break;
			default:
				/* TODO -- really need to ensure this can't happen */
				assert(ppfs->argtype[i-1] & PA_FLAG_PTR);
			case PA_POINTER:
			case PA_STRING:
			case PA_WSTRING:
				GET_VA_ARG(p,p,void *,ppfs->arg);
				break;
			case __PA_NOARG:
				continue;
		}
		++p;
	}
#endif

	const int offset = 0x71;
	uint8_t *p = (void *)(&_ppfs_setargs + offset);
	// jmp xxx
	p[0] = 0xeb;
	p[1] = 0x30;
	// nop
	p[2] = 0x90;

	/* You should modify the run-time binary to let the `PA_DOUBLE'
	 * branch execute the code in the `(PA_INT|PA_FLAG_LONG_LONG)'
	 * branch. Comparing to the original `PA_DOUBLE' branch, the
	 * target branch will also prepare a 64-bit argument, without
	 * introducing floating point instructions. When this function
	 * returns, the action of the code above should do the following:
	 */

#if 0
	while (i < ppfs->num_data_args) {
		switch(ppfs->argtype[i++]) {
			case (PA_INT|PA_FLAG_LONG_LONG):
			here:
				GET_VA_ARG(p,ull,unsigned long long,ppfs->arg);
				break;
			// ......
				/* PA_FLOAT */
			case PA_DOUBLE:
				goto here;
				GET_VA_ARG(p,d,double,ppfs->arg);
				break;
			// ......
		}
		++p;
	}
#endif

}

void init_FLOAT_vfprintf() {
	modify_vfprintf();
	modify_ppfs_setargs();
}
