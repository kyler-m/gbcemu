#include "cpu.h"
#include "memory.h"
#include "rom.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TO_P(I)	(uintptr_t *)&I;
#define C_NZ	flags->Z
#define C_Z		!flags->Z
#define C_NC	flags->C
#define C_C 	!flags->C

typedef uint8_t u8;
typedef uint16_t u16;

struct flag_t {
	unsigned int pad	: 4;
	unsigned int C 		: 1;
	unsigned int H 		: 1;
	unsigned int N 		: 1;
	unsigned int Z 		: 1;
};

struct bitfield8_t {
	int b0 : 1;
	int b1 : 1;
	int b2 : 1;
	int b3 : 1;
	int b4 : 1;
	int b5 : 1;
	int b6 : 1;
	int b7 : 1;
};

struct bitfield16_t {
	int b0 : 1;
	int b1 : 1;
	int b2 : 1;
	int b3 : 1;
	int b4 : 1;
	int b5 : 1;
	int b6 : 1;
	int b7 : 1;
	int b8 : 1;
	int b9 : 1;
	int b10 : 1;
	int b11 : 1;
	int b12 : 1;
	int b13 : 1;
	int b14 : 1;
	int b15 : 1;
};

union fetched_t {
	char b1;
	uint16_t b2;
};

static union fetched_t fetch(int);
static void post_check_init(void);
static void fetch_decode(void);
static void fetch_decode_1b(unsigned char);
static void fetch_decode_2b(void);
static void pre_execution_check(void);
static char *reg2addr(uint16_t);

/* Instructions */
static void i_nop(void);
static void i_halt(void);
static void i_ld_rtor(uintptr_t *, uintptr_t *, int, int);
static void i_accum_add(uintptr_t *, int);
static void i_push(uintptr_t *);
static void i_pop(uintptr_t *);
static void i_and(uintptr_t *, int);
static void i_stop(void);
static void i_ei(void);
static void i_di(void);
static void i_rla(void);
static void i_rlca(void);
static void i_cpl(void);
static void i_ccf(void);
static void i_xor(uintptr_t *, int);
static void i_or(uintptr_t *, int);
static void i_pcall(void);
static void i_ret(void);
static void i_ld_nn(uintptr_t *);
static void i_ld_n(uintptr_t *);
static void i_ldi_hl(void);
static void i_ld_ca(void);
static void i_bit(uintptr_t *, int, int);
static void i_sla(uintptr_t *, int);
static void i_inc_n(uintptr_t *);
static void i_ld_hl(uintptr_t *);
static void i_ldh(void);
static void i_ld_a(uintptr_t *, int);
static void i_rl(uintptr_t *, int);
static void i_dec(uintptr_t *, int);
static void i_inc(uintptr_t *, int);
static void shutdown(const char *);

static void general_add(uintptr_t *, uintptr_t *, int, int);

/* CPU registers */
static uint8_t A, B, C, D, E, F, H, L;
static uint16_t SP, PC = 0, AF, BC, DE, HL;

/* Pointers to registers */
static uintptr_t *A_p = TO_P(A); 
static uintptr_t *B_p = TO_P(B); 
static uintptr_t *C_p = TO_P(C);
static uintptr_t *D_p = TO_P(D);
static uintptr_t *E_p = TO_P(E);
static uintptr_t *F_p = TO_P(F);
static uintptr_t *H_p = TO_P(H);
static uintptr_t *L_p = TO_P(L);
static uintptr_t *AF_p = TO_P(AF);
static uintptr_t *BC_p = TO_P(BC);
static uintptr_t *DE_p = TO_P(DE);
static uintptr_t *HL_p = TO_P(HL);
static uintptr_t *PC_p = TO_P(PC);
static uintptr_t *SP_p = TO_P(SP);

static struct flag_t *flags = (struct flag_t *)&F;
static bool verbose = false;
static bool stopped = false;
static bool halted = false;

void init_cpu(bool verbose_execution)
{
	verbose = verbose_execution;
}

void begin_execution(void)
{
	pre_execution_check();
	post_check_init();

	//shutdown("shutting down\n");

	for (;;)
		fetch_decode();
}

static char *reg2addr(uint16_t reg)
{
	unsigned long long f = reg;
	return (void *)f;
}

/*
 * Run the bootstrap to check the ROM.
 */
static void pre_execution_check(void)
{
	if (verbose)
		printf("cpu: running pre-execution check with bootstrap\n");
}

static void fetch_decode(void)
{
	unsigned char byte1 = fetch(1).b1;

	if (byte1 == 0xCB)
		fetch_decode_2b();
	else
		fetch_decode_1b(byte1);
}

static void fetch_decode_1b(unsigned char byte)
{
	if (verbose)
		printf("cpu: decoding 1b opc %02X\n", byte);
	switch (byte) {
		#include "opcodes_no_prefix.table"
		default: shutdown("1b opc not found\n");
	}
}

static void fetch_decode_2b(void)
{
	unsigned char byte2 = fetch(1).b1;

	if (verbose)
		printf("cpu: decoding 2b opc CB%02X\n", byte2);
	switch (byte2) {
		#include "opcodes_cb_prefix.table"
		default: shutdown("2b opc not found\n");
	}
}

static union fetched_t fetch(int n)
{
	union fetched_t ret;
	mget(&ret, reg2addr(PC), n);
	PC += n;
	return ret;
}

static void post_check_init(void)
{
	if (verbose)
		printf("cpu: initializing initial registers and memory locations\n");
	#include "initialize_registers.table"
}

static void jump_val(uintptr_t *val, int nb)
{
	if (nb == 1)
		PC = *(uint8_t *)val;
	else
		PC = *(uint16_t *)val;
}

static void jump_ptr(uintptr_t *val, int nb)
{

}

/* Instructions */

static void i_and(uintptr_t *src, int nb)
{
	memset(flags, 0, sizeof *flags);

	if (nb == 1)
		A &= *(uint8_t *)src;
	else
		A &= *(uint16_t *)src;

	if (!A)
		flags->Z = 1;
	flags->H = 1;
}

static void i_nop(void)
{
	if (verbose)
		printf("cpu: nop\n");

	i_stop();
}

static void i_pop(uintptr_t *reg_generic)
{
	uint16_t *reg = (uint16_t *)reg_generic;
	mget(reg, reg2addr(SP), 2);
	SP += 2;
}

static void i_push(uintptr_t *reg_generic)
{
	uint16_t *reg = (uint16_t *)reg_generic;
	SP -= 2;
	mput_region(reg2addr(SP), (char *)reg, 2, true);
}

static void i_ld_rtom(uint8_t sreg)
{

}

//Z 0 H C
static void i_accum_add(uintptr_t *src, int nbytes)
{
	general_add(A_p, src, 1, nbytes);
}

//Z 0 H C
static void general_add(uintptr_t *dest, uintptr_t *src, int nb1, int nb2)
{
	memset(flags, 0, sizeof *flags);
	uint32_t res;

	if (nb1 == 1) {
		uint8_t *t_dest = (uint8_t *)dest;
		if (nb2 == 1) {
			uint8_t *t_src = (uint8_t *)src;
			res = *t_dest + *t_src;
			*t_dest += *t_src;
		} else {
			uint16_t *t_src = (uint16_t *)src;
			res = *t_dest + *t_src;
			*t_dest += *t_src;
		}
	} else {
		uint16_t *t_dest = (uint16_t *)dest;
		if (nb2 == 1) {
			uint8_t *t_src = (uint8_t *)src;
			res = *t_dest + *t_src;
			*t_dest += *t_src;
		} else {
			uint16_t *t_src = (uint16_t *)src;
			res = *t_dest + *t_src;
			*t_dest += *t_src;
		}
	}

	if (!res)
		flags->Z = 1;
	if (res >> 0x4)
		flags->H = 1;
	if (res >> 0x8)
		flags->C = 1;
}

static void i_ld_rtor(uintptr_t *r1, uintptr_t *r2, int nb1, int nb2)
{
	uint16_t val = (nb2 == 1) ? *(uint8_t *)r2 : *(uint16_t *)r2;

	if (nb1 == 1) {
		uint8_t *dest = (uint8_t *)r1;
		*dest = val;
	} else {
		uint16_t *dest = (uint16_t *)r1;
		*dest = val;
	}
}

/*
 * Increment instruction
 * Z 0 H -
 */
static void i_inc(uintptr_t *reg, int nb)
{
	uint32_t res;

	if (verbose)
		printf("cpu: incrementing\n");

	if (nb == 1)
		res = ++(*(uint8_t *)reg);
	else
		res = ++(*(uint16_t *)reg);

	flags->Z = (!res) ? 1 : 0;
	flags->N = 1;
	flags->H = (res >> 0x4) ? 1 : 0;
}

/*
 * Decrement instruction
 * Z 1 H -
 */
static void i_dec(uintptr_t *reg, int nb)
{
	uint32_t res;

	if (verbose)
		printf("cpu: decrementing\n");

	if (nb == 1)
		res = --(*(uint8_t *)reg);
	else
		res = --(*(uint16_t *)reg);

	flags->Z = (!res) ? 1 : 0;
	flags->N = 1;
	flags->H = (res >> 0x4) ? 1 : 0;
}

/* 
 * Halt the CPU until an interrupt.
 */
static void i_halt(void)
{
	if (verbose)
		printf("cpu: simulation is halting\n");
	halted = true;

	for (;;)
		;
}

/*
 * Halt CPU & LCD display until a button is pressed.
 */
static void i_stop(void)
{
	/*  */
	shutdown("got a stop\n");

	/* Halt the CPU (busy loop will be fine) */
	// poll for button?
	for (;;)
		;
}

static void i_di(void)
{

}

static void i_ei(void)
{

}

//Z 0 0 C
static void i_rlca(void)
{
	int carry = flags->C;
	memset(flags, 0, sizeof *flags);

	int bit7 = A >> 0x7;
	A = (A << 0x1) + carry;

	if (!A)
		flags->Z = 1;
	flags->C = bit7;
}

//Z 0 0 C
static void i_rla(void)
{
	memset(flags, 0, sizeof *flags);

	int bit7 = A >> 0x7;
	A <<= 0x1;

	if (!A)
		flags->Z = 1;
	flags->C = bit7;
}

static void i_ccf(void)
{
	flags->C = ~flags->C;
	flags->N = 0;
	flags->H = 0;
}

static void i_cpl(void)
{
	A = ~A;
	flags->N = 1;
	flags->H = 1;
}

//Z 0 0 0
static void i_or(uintptr_t *src, int nb)
{
	memset(flags, 0, sizeof *flags);

	if (nb == 1)
		A |= *(uint8_t *)src;
	else
		A |= *(uint16_t *)src;

	if (!A)
		flags->Z = 1;
}

//Z 0 0 0
static void i_xor(uintptr_t *src, int nb)
{
	if (verbose)
		printf("cpu: xor\n");
	memset(flags, 0, sizeof *flags);

	if (nb == 1)
		A ^= *(uint8_t *)src;
	else
		A ^= *(uint16_t *)src;

	if (!A)
		flags->Z = 1;
}

static void i_pcall(void)
{
	uint16_t val = fetch(2).b2;

	/* Push address onto stack. */
	i_push(PC_p);
	if (verbose)
		printf("cpu: pushing %04X onto stack\n", PC);
	/* Unconditional jump to immediate 16 bit address. */
	PC = val;

	if (verbose)
		printf("cpu: jumping to %04X\n", PC);
}

static void i_ret(void)
{
	i_pop(PC_p);
	if (verbose)
		printf("cpu: returning to %04X\n", PC);
}

static void i_ld_nn(uintptr_t *dest)
{
	uint16_t *typed_dest = (uint16_t *)dest;
	*typed_dest = fetch(2).b2;
	if (verbose)
		printf("cpu: loaded 2b %04X\n", *typed_dest);
}

static void i_ld_n(uintptr_t *dest)
{
	uint8_t *typed_dest = (uint8_t *)dest;
	*typed_dest = fetch(1).b1;
	if (verbose)
		printf("cpu: loaded 1b %02X\n", *typed_dest);
}

static void i_ldi_hl(void)
{
	if (verbose)
		printf("cpu: i_ldi_hl at %p for val %02X\n", reg2addr(HL), A);
	mput(reg2addr(HL), A, true);
	HL++;
}

static void i_bit(uintptr_t *reg, int nb, int n)
{
	//TODO 16bit

	if (verbose)
		printf("cpu: bit %d\n", n);

	uint8_t val = *(uint8_t *)reg;

	flags->Z = val & (0x1 << n);
	flags->N = 0;
	flags->H = 1;
}

static void i_sla(uintptr_t *reg, int nb)
{
	//TODO 16bit

	if (verbose)
		printf("sla\n");

	uint8_t *t_reg = (uint8_t *)reg;
	*t_reg <<= 0x1;
}

static void i_ld_ca(void)
{
	if (verbose)
		printf("cpu: putting %02X into %p\n", A, reg2addr(0xFF00 + C));
	mput(reg2addr(0xFF00 + C), A, true);
}

static void i_inc_n(uintptr_t *reg)
{
	if (verbose)
		printf("cpu: incrementing\n");
	uint8_t *t_reg = (uint8_t *)reg;
	(*t_reg)++;
}

static void i_ld_hl(uintptr_t *reg)
{
	uint8_t *t_reg = (uint8_t *)reg;
	if (verbose)
		printf("cpu: loading HL with %02X\n", *t_reg);
	HL = *t_reg;
}

static void i_ldh(void)
{
	if (verbose)
		printf("cpu: ldh\n");
	uint8_t n = fetch(1).b1;
	mput(reg2addr(0xFF00 + n), A, true);
}

static void i_ld_a(uintptr_t *reg, int nb)
{
	if (verbose)
		printf("cpu: loading into accum\n");

	if (nb == 1)
		A = *(uint8_t *)reg;
	else
		A = *(uint16_t *)reg;
}

static void i_rl(uintptr_t *reg, int nb)
{
	//TODO 2b, carry, etc.

	uint8_t *t_reg = (uint8_t *)reg;
	(*t_reg) <<= 0x1;
}

static void shutdown(const char *msg)
{
	printf("%s", msg);
	exit(0);
}