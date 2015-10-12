#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
	uint8_t		a;
	uint8_t		b;
	uint8_t		c;
	uint8_t 	d;
	uint8_t		e;
	uint8_t		f; //flag register
	uint8_t		h;
	uint8_t		l;
	uint16_t	sp; //stack pointer
	uint16_t	pc; //program counter
} registers;

typedef struct {
	char	mem[]; //start of RAM space
} RAM;

#define RAMBYTES	8000

//zero flag set
#define zflagisset(f)	((f & 0x80) >> 0x7)

//subtract flag set
#define sflagisset(f)	((f & 0x40) >> 0x6)

//half carry flag set
#define hflagisset(f)	((f & 0x20) >> 0x5)

//carry flag set
#define cflagisset(f)	((f & 0x10) >> 0x4)

#define setz(f)		(f | 0x80)
#define sets(f)		(f | 0x40)
#define seth(f)		(f | 0x20)
#define setc(f)		(f | 0x10)
#define clear(f)	(f & 0x0)

#define CBOP(p)		(p & 0xCB00)

void	init_registers(registers *reg);
void	init_ram(RAM *ram);
void	nop();
void	add(uint8_t *dest, uint8_t *src, uint8_t *flag);
void	sub(uint8_t *dest, uint8_t *src, uint8_t *flag);
void	ld_mem(RAM *ram, uint8_t *mem_offset, uint8_t *reg);
void	ld_reg(RAM *ram, uint8_t *reg, uint8_t *mem_offset);
void 	inc(uint8_t *reg, uint8_t *f);
void 	inc(uint8_t *reg_hi, uint8_t *reg_lo, uint8_t *f);
void 	dec(uint8_t *reg, uint8_t *f);
void	lxor(uint8_t *dest, uint8_t *src, uint8_t *f);
void	lor(uint8_t *dest, uint8_t *src, uint8_t *f);
void	land(uint8_t *dest, uint8_t *src, uint8_t *f);
void	ld(uint8_t *dest, uint8_t *src);
void	cp(uint8_t *reg, uint8_t *n, uint8_t *f);
void	swap(uint8_t *reg, uint8_t *f);
void	ret(uint16_t *sp, uint16_t *pc);
void	set(uint8_t *reg, uint8_t b);
void	res(uint8_t *reg, uint8_t b);
void	bit(uint8_t *reg, uint8_t b, uint8_t *f);
void	rlc(uint8_t *reg, uint8_t *f);
void	rrc(uint8_t *reg, uint8_t *f);
int	fetch_decode(RAM *ram, registers *reg);
int	decode1B(RAM *ram, registers *reg, uint8_t opc);
int	decode2B(RAM *ram, registers *reg);


/**
 * Set the registers to their initial states.
 */
void init_registers(registers *reg) 
{
	reg = (registers *) malloc(sizeof(registers));
	
	reg->a = 0x0;
	reg->b = 0x0;
	reg->c = 0x0;
	reg->d = 0x0;
	reg->e = 0x0;
	reg->f = 0x0;
	reg->h = 0x0;
	reg->l = 0x0;
	reg->sp = 0xFFFE;
	reg->pc = 0x100;
}

/**
 * Set up the RAM space. Later needs to be the actual cartridge loading into memory.
 */
void init_ram(RAM *ram)
{
	ram = (RAM *) malloc(RAMBYTES);
}

/**
 * Determine if the instruction is 1 or 2 bytes and branch as necessary.
 */
int fetch_decode(RAM *ram, registers *reg)
{
	uint8_t opc;
	
	opc = ram->mem[reg->pc++];
	
	//check to see if it's a CB prefix, if so then it's just a prefix
	if (CBOP(opc))
		return decode2B(ram, reg);
	else
		return decode1B(ram, reg, opc);
}

/**
 * Decode a 1 byte instruction.
 *
 * Return -1 on failure, 0 on success.
 */
int decode1B(RAM *ram, registers *reg, uint8_t opc)
{
	//note: keep opcodes in order so compiler will optimize to jump tables over if-else chains
	switch (opc) {
		
	case 0x00: nop(); break;
	
	case 0x04: inc(&reg->b, &reg->f); break;
	
	case 0x0C: inc(&reg->c, &reg->f); break;
	
	case 0x14: inc(&reg->d, &reg->f); break;
	
	case 0x1C: inc(&reg->e, &reg->f); break;
	
	case 0x24: inc(&reg->h, &reg->f); break;
	
	case 0x2C: inc(&reg->l, &reg->f); break;
	
	//....
	case 0x3C: inc(&reg->a, &reg->f); break;
	
	case 0x40: ld(&reg->b, &reg->b); break;
	case 0x41: ld(&reg->b, &reg->c); break;
	case 0x42: ld(&reg->b, &reg->d); break;
	case 0x43: ld(&reg->b, &reg->e); break;
	case 0x44: ld(&reg->b, &reg->h); break;
	case 0x45: ld(&reg->b, &reg->l); break;
	case 0x46: nop(); break; //TODO
	case 0x47: ld(&reg->b, &reg->a); break;
	case 0x48: ld(&reg->c, &reg->b); break;
	case 0x49: ld(&reg->c, &reg->c); break;
	case 0x4A: ld(&reg->c, &reg->d); break;
	case 0x4B: ld(&reg->c, &reg->e); break;
	case 0x4C: ld(&reg->c, &reg->h); break;
	case 0x4D: ld(&reg->c, &reg->l); break;
	case 0x4E: nop(); break; //TODO
	case 0x4F: ld(&reg->c, &reg->a); break;
	
	case 0x50: ld(&reg->d, &reg->b); break;
	case 0x51: ld(&reg->d, &reg->c); break;
	case 0x52: ld(&reg->d, &reg->d); break;
	case 0x53: ld(&reg->d, &reg->e); break;
	case 0x54: ld(&reg->d, &reg->h); break;
	case 0x55: ld(&reg->d, &reg->l); break;
	case 0x56: nop(); break; //TODO
	case 0x57: ld(&reg->d, &reg->a); break;
	case 0x58: ld(&reg->e, &reg->b); break;
	case 0x59: ld(&reg->e, &reg->c); break;
	case 0x5A: ld(&reg->e, &reg->d); break;
	case 0x5B: ld(&reg->e, &reg->e); break;
	case 0x5C: ld(&reg->e, &reg->h); break;
	case 0x5D: ld(&reg->e, &reg->l); break;
	case 0x5E: nop(); break; //TODO
	case 0x5F: ld(&reg->e, &reg->a); break;
	
	case 0x60: ld(&reg->h, &reg->b); break;
	case 0x61: ld(&reg->h, &reg->c); break;
	case 0x62: ld(&reg->h, &reg->d); break;
	case 0x63: ld(&reg->h, &reg->e); break;
	case 0x64: ld(&reg->h, &reg->h); break;
	case 0x65: ld(&reg->h, &reg->l); break;
	case 0x66: nop(); break; //TODO
	case 0x67: ld(&reg->h, &reg->a); break;
	case 0x68: ld(&reg->l, &reg->b); break;
	case 0x69: ld(&reg->l, &reg->c); break;
	case 0x6A: ld(&reg->l, &reg->d); break;
	case 0x6B: ld(&reg->l, &reg->e); break;
	case 0x6C: ld(&reg->l, &reg->h); break;
	case 0x6D: ld(&reg->l, &reg->l); break;
	case 0x6E: nop(); break; //TODO
	case 0x6F: ld(&reg->l, &reg->a); break;
	
	case 0x76: nop(); break; //TODO
	case 0x77: nop(); break; //TODO
	case 0x78: ld(&reg->a, &reg->b); break;
	case 0x79: ld(&reg->a, &reg->c); break;
	case 0x7A: ld(&reg->a, &reg->d); break;
	case 0x7B: ld(&reg->a, &reg->e); break;
	case 0x7C: ld(&reg->a, &reg->h); break;
	case 0x7D: ld(&reg->a, &reg->l); break;
	case 0x7E: nop(); break; //TODO
	case 0x7F: ld(&reg->a, &reg->a); break;

	case 0x80: add(&reg->a, &reg->b, &reg->f); break;
	case 0x81: add(&reg->a, &reg->c, &reg->f); break;
	case 0x82: add(&reg->a, &reg->d, &reg->f); break;
	case 0x83: add(&reg->a, &reg->e, &reg->f); break;
	case 0x84: add(&reg->a, &reg->h, &reg->f); break;
	case 0x85: add(&reg->a, &reg->l, &reg->f); break;
	case 0x86: nop(); break; //TODO
	case 0x87: add(&reg->a, &reg->a, &reg->f); break;
	
	case 0x90: sub(&reg->a, &reg->b, &reg->f); break;
	case 0x91: sub(&reg->a, &reg->c, &reg->f); break;
	case 0x92: sub(&reg->a, &reg->d, &reg->f); break;
	case 0x93: sub(&reg->a, &reg->e, &reg->f); break;
	case 0x94: sub(&reg->a, &reg->h, &reg->f); break;
	case 0x95: sub(&reg->a, &reg->l, &reg->f); break;
	case 0x96: nop(); break; //TODO
	case 0x97: sub(&reg->a, &reg->a, &reg->f); break;
	
	case 0xA0: land(&reg->a, &reg->b, &reg->f); break;
	case 0xA1: land(&reg->a, &reg->c, &reg->f); break;
	case 0xA2: land(&reg->a, &reg->d, &reg->f); break;
	case 0xA3: land(&reg->a, &reg->e, &reg->f); break;
	case 0xA4: land(&reg->a, &reg->h, &reg->f); break;
	case 0xA5: land(&reg->a, &reg->l, &reg->f); break;
	case 0xA6: nop(); break; //TODO
	case 0xA7: land(&reg->a, &reg->a, &reg->f); break;
	
	case 0xA8: lxor(&reg->a, &reg->b, &reg->f); break;
	case 0xA9: lxor(&reg->a, &reg->c, &reg->f); break;
	case 0xAA: lxor(&reg->a, &reg->d, &reg->f); break;
	case 0xAB: lxor(&reg->a, &reg->e, &reg->f); break;
	case 0xAC: lxor(&reg->a, &reg->h, &reg->f); break;
	case 0xAD: lxor(&reg->a, &reg->l, &reg->f); break;
	case 0xAE: nop(); break; //TODO
	case 0xAF: lxor(&reg->a, &reg->a, &reg->f); break;
	
	case 0xB0: lor(&reg->a, &reg->b, &reg->f); break;
	case 0xB1: lor(&reg->a, &reg->c, &reg->f); break;
	case 0xB2: lor(&reg->a, &reg->d, &reg->f); break;
	case 0xB3: lor(&reg->a, &reg->e, &reg->f); break;
	case 0xB4: lor(&reg->a, &reg->h, &reg->f); break;
	case 0xB5: lor(&reg->a, &reg->l, &reg->f); break;
	case 0xB6: nop(); break; //TODO
	case 0xB7: lor(&reg->a, &reg->a, &reg->f); break;
	
	case 0xB8: cp(&reg->a, &reg->b, &reg->f); break; //cp needs to be implemented
	case 0xB9: cp(&reg->a, &reg->c, &reg->f); break;
	case 0xBA: cp(&reg->a, &reg->d, &reg->f); break;
	case 0xBB: cp(&reg->a, &reg->e, &reg->f); break;
	case 0xBC: cp(&reg->a, &reg->h, &reg->f); break;
	case 0xBD: cp(&reg->a, &reg->l, &reg->f); break;
	case 0xBE: nop(); break; //TODO
	case 0xBF: cp(&reg->a, &reg->a, &reg->f); break;
	
	case 0xC9: ret(&reg->sp, &reg->pc); break;
	
	default: return (-1);
	
	}
	
	return (0);
}

/**
 * Decode a 2 byte instruction.
 *
 * Return -1 on failure, 0 on success.
 */
int decode2B(RAM *ram, registers *reg)
{
	uint8_t opc;
	
	opc = ram->mem[reg->pc++];
	
	switch (opc) {
		
	case 0x00: nop(); break; //rlc opc
	case 0x01: nop(); break;
	case 0x02: nop(); break;
	case 0x03: nop(); break;
	case 0x04: nop(); break;
	case 0x05: nop(); break;
	case 0x06: nop(); break;
	case 0x07: nop(); break;
	case 0x08: nop(); break; //rrc opc
	case 0x09: nop(); break;
	case 0x0A: nop(); break;
	case 0x0B: nop(); break;
	case 0x0C: nop(); break;
	case 0x0D: nop(); break;
	case 0x0E: nop(); break;
	case 0x0F: nop(); break;
	case 0x10: nop(); break; //rl opc
	case 0x11: nop(); break;
	case 0x12: nop(); break;
	case 0x13: nop(); break;
	case 0x14: nop(); break;
	case 0x15: nop(); break;
	case 0x16: nop(); break;
	case 0x17: nop(); break;
	case 0x18: nop(); break; //rr opc
	case 0x19: nop(); break;
	case 0x1A: nop(); break;
	case 0x1B: nop(); break;
	case 0x1C: nop(); break;
	case 0x1D: nop(); break;
	case 0x1E: nop(); break;
	case 0x1F: nop(); break;
	case 0x20: nop(); break; //sla opc
	case 0x21: nop(); break;
	case 0x22: nop(); break;
	case 0x23: nop(); break;
	case 0x24: nop(); break;
	case 0x25: nop(); break;
	case 0x26: nop(); break;
	case 0x27: nop(); break;
	case 0x28: nop(); break; //sra opc
	case 0x29: nop(); break;
	case 0x2A: nop(); break;
	case 0x2B: nop(); break;
	case 0x2C: nop(); break;
	case 0x2D: nop(); break;
	case 0x2E: nop(); break;
	case 0x2F: nop(); break;
	case 0x30: swap(&reg->b, &reg->f); break; //swap opc
	case 0x31: swap(&reg->c, &reg->f); break;
	case 0x32: swap(&reg->d, &reg->f); break;
	case 0x33: swap(&reg->e, &reg->f); break;
	case 0x34: swap(&reg->h, &reg->f); break;
	case 0x35: swap(&reg->l, &reg->f); break;
	case 0x36: nop(); break; //TODO
	case 0x37: swap(&reg->a, &reg->f); break;
	case 0x38: nop(); break; //srl opc
	case 0x39: nop(); break;
	case 0x3A: nop(); break;
	case 0x3B: nop(); break;
	case 0x3C: nop(); break;
	case 0x3D: nop(); break;
	case 0x3E: nop(); break;
	case 0x3F: nop(); break;
	case 0x40: bit(&reg->b, 0x0, &reg->f); break;	
	case 0x41: bit(&reg->c, 0x0, &reg->f); break;
	case 0x42: bit(&reg->d, 0x0, &reg->f); break;
	case 0x43: bit(&reg->e, 0x0, &reg->f); break;
	case 0x44: bit(&reg->h, 0x0, &reg->f); break;
	case 0x45: bit(&reg->l, 0x0, &reg->f); break;
	case 0x46: nop(); break; //TODO
	case 0x47: bit(&reg->a, 0x0, &reg->f); break;
	case 0x48: bit(&reg->b, 0x1, &reg->f); break;	
	case 0x49: bit(&reg->c, 0x1, &reg->f); break;
	case 0x4A: bit(&reg->d, 0x1, &reg->f); break;
	case 0x4B: bit(&reg->e, 0x1, &reg->f); break;
	case 0x4C: bit(&reg->h, 0x1, &reg->f); break;
	case 0x4D: bit(&reg->l, 0x1, &reg->f); break;
	case 0x4E: nop(); break; //TODO
	case 0x4F: bit(&reg->a, 0x1, &reg->f); break;
	case 0x50: bit(&reg->b, 0x2, &reg->f); break;	
	case 0x51: bit(&reg->c, 0x2, &reg->f); break;
	case 0x52: bit(&reg->d, 0x2, &reg->f); break;
	case 0x53: bit(&reg->e, 0x2, &reg->f); break;
	case 0x54: bit(&reg->h, 0x2, &reg->f); break;
	case 0x55: bit(&reg->l, 0x2, &reg->f); break;
	case 0x56: nop(); break; //TODO
	case 0x57: bit(&reg->a, 0x2, &reg->f); break;
	case 0x58: bit(&reg->b, 0x3, &reg->f); break;	
	case 0x59: bit(&reg->c, 0x3, &reg->f); break;
	case 0x5A: bit(&reg->d, 0x3, &reg->f); break;
	case 0x5B: bit(&reg->e, 0x3, &reg->f); break;
	case 0x5C: bit(&reg->h, 0x3, &reg->f); break;
	case 0x5D: bit(&reg->l, 0x3, &reg->f); break;
	case 0x5E: nop(); break; //TODO
	case 0x5F: bit(&reg->a, 0x3, &reg->f); break;
	case 0x60: bit(&reg->b, 0x4, &reg->f); break;	
	case 0x61: bit(&reg->c, 0x4, &reg->f); break;
	case 0x62: bit(&reg->d, 0x4, &reg->f); break;
	case 0x63: bit(&reg->e, 0x4, &reg->f); break;
	case 0x64: bit(&reg->h, 0x4, &reg->f); break;
	case 0x65: bit(&reg->l, 0x4, &reg->f); break;
	case 0x66: nop(); break; //TODO
	case 0x67: bit(&reg->a, 0x4, &reg->f); break;
	case 0x68: bit(&reg->b, 0x5, &reg->f); break;	
	case 0x69: bit(&reg->c, 0x5, &reg->f); break;
	case 0x6A: bit(&reg->d, 0x5, &reg->f); break;
	case 0x6B: bit(&reg->e, 0x5, &reg->f); break;
	case 0x6C: bit(&reg->h, 0x5, &reg->f); break;
	case 0x6D: bit(&reg->l, 0x5, &reg->f); break;
	case 0x6E: nop(); break; //TODO
	case 0x6F: bit(&reg->a, 0x5, &reg->f); break;
	case 0x70: bit(&reg->b, 0x6, &reg->f); break;	
	case 0x71: bit(&reg->c, 0x6, &reg->f); break;
	case 0x72: bit(&reg->d, 0x6, &reg->f); break;
	case 0x73: bit(&reg->e, 0x6, &reg->f); break;
	case 0x74: bit(&reg->h, 0x6, &reg->f); break;
	case 0x75: bit(&reg->l, 0x6, &reg->f); break;
	case 0x76: nop(); break; //TODO
	case 0x77: bit(&reg->a, 0x6, &reg->f); break;
	case 0x78: bit(&reg->b, 0x7, &reg->f); break;	
	case 0x79: bit(&reg->c, 0x7, &reg->f); break;
	case 0x7A: bit(&reg->d, 0x7, &reg->f); break;
	case 0x7B: bit(&reg->e, 0x7, &reg->f); break;
	case 0x7C: bit(&reg->h, 0x7, &reg->f); break;
	case 0x7D: bit(&reg->l, 0x7, &reg->f); break;
	case 0x7E: nop(); break; //TODO
	case 0x7F: bit(&reg->a, 0x7, &reg->f); break;
	case 0x80: res(&reg->b, 0x0); break;	
	case 0x81: res(&reg->c, 0x0); break;
	case 0x82: res(&reg->d, 0x0); break;
	case 0x83: res(&reg->e, 0x0); break;
	case 0x84: res(&reg->h, 0x0); break;
	case 0x85: res(&reg->l, 0x0); break;
	case 0x86: nop(); break; //TODO
	case 0x87: res(&reg->a, 0x0); break;
	case 0x88: res(&reg->b, 0x1); break;	
	case 0x89: res(&reg->c, 0x1); break;
	case 0x8A: res(&reg->d, 0x1); break;
	case 0x8B: res(&reg->e, 0x1); break;
	case 0x8C: res(&reg->h, 0x1); break;
	case 0x8D: res(&reg->l, 0x1); break;
	case 0x8E: nop(); break; //TODO
	case 0x8F: res(&reg->a, 0x1); break;
	case 0x90: res(&reg->b, 0x2); break;	
	case 0x91: res(&reg->c, 0x2); break;
	case 0x92: res(&reg->d, 0x2); break;
	case 0x93: res(&reg->e, 0x2); break;
	case 0x94: res(&reg->h, 0x2); break;
	case 0x95: res(&reg->l, 0x2); break;
	case 0x96: nop(); break; //TODO
	case 0x97: res(&reg->a, 0x2); break;
	case 0x98: res(&reg->b, 0x3); break;	
	case 0x99: res(&reg->c, 0x3); break;
	case 0x9A: res(&reg->d, 0x3); break;
	case 0x9B: res(&reg->e, 0x3); break;
	case 0x9C: res(&reg->h, 0x3); break;
	case 0x9D: res(&reg->l, 0x3); break;
	case 0x9E: nop(); break; //TODO
	case 0x9F: res(&reg->a, 0x3); break;
	case 0xA0: res(&reg->b, 0x4); break;	
	case 0xA1: res(&reg->c, 0x4); break;
	case 0xA2: res(&reg->d, 0x4); break;
	case 0xA3: res(&reg->e, 0x4); break;
	case 0xA4: res(&reg->h, 0x4); break;
	case 0xA5: res(&reg->l, 0x4); break;
	case 0xA6: nop(); break; //TODO
	case 0xA7: res(&reg->a, 0x4); break;
	case 0xA8: res(&reg->b, 0x5); break;	
	case 0xA9: res(&reg->c, 0x5); break;
	case 0xAA: res(&reg->d, 0x5); break;
	case 0xAB: res(&reg->e, 0x5); break;
	case 0xAC: res(&reg->h, 0x5); break;
	case 0xAD: res(&reg->l, 0x5); break;
	case 0xAE: nop(); break; //TODO
	case 0xAF: res(&reg->a, 0x5); break;
	case 0xB0: res(&reg->b, 0x6); break;	
	case 0xB1: res(&reg->c, 0x6); break;
	case 0xB2: res(&reg->d, 0x6); break;
	case 0xB3: res(&reg->e, 0x6); break;
	case 0xB4: res(&reg->h, 0x6); break;
	case 0xB5: res(&reg->l, 0x6); break;
	case 0xB6: nop(); break; //TODO
	case 0xB7: res(&reg->a, 0x6); break;
	case 0xB8: res(&reg->b, 0x7); break;	
	case 0xB9: res(&reg->c, 0x7); break;
	case 0xBA: res(&reg->d, 0x7); break;
	case 0xBB: res(&reg->e, 0x7); break;
	case 0xBC: res(&reg->h, 0x7); break;
	case 0xBD: res(&reg->l, 0x7); break;
	case 0xBE: nop(); break; //TODO
	case 0xBF: res(&reg->a, 0x7); break;
	case 0xC0: set(&reg->b, 0x0); break;	
	case 0xC1: set(&reg->c, 0x0); break;
	case 0xC2: set(&reg->d, 0x0); break;
	case 0xC3: set(&reg->e, 0x0); break;
	case 0xC4: set(&reg->h, 0x0); break;
	case 0xC5: set(&reg->l, 0x0); break;
	case 0xC6: nop(); break; //TODO
	case 0xC7: set(&reg->a, 0x0); break;
	case 0xC8: set(&reg->b, 0x1); break;	
	case 0xC9: set(&reg->c, 0x1); break;
	case 0xCA: set(&reg->d, 0x1); break;
	case 0xCB: set(&reg->e, 0x1); break;
	case 0xCC: set(&reg->h, 0x1); break;
	case 0xCD: set(&reg->l, 0x1); break;
	case 0xCE: nop(); break; //TODO
	case 0xCF: set(&reg->a, 0x1); break;
	case 0xD0: set(&reg->b, 0x2); break;	
	case 0xD1: set(&reg->c, 0x2); break;
	case 0xD2: set(&reg->d, 0x2); break;
	case 0xD3: set(&reg->e, 0x2); break;
	case 0xD4: set(&reg->h, 0x2); break;
	case 0xD5: set(&reg->l, 0x2); break;
	case 0xD6: nop(); break; //TODO
	case 0xD7: set(&reg->a, 0x2); break;
	case 0xD8: set(&reg->b, 0x3); break;	
	case 0xD9: set(&reg->c, 0x3); break;
	case 0xDA: set(&reg->d, 0x3); break;
	case 0xDB: set(&reg->e, 0x3); break;
	case 0xDC: set(&reg->h, 0x3); break;
	case 0xDD: set(&reg->l, 0x3); break;
	case 0xDE: nop(); break; //TODO
	case 0xDF: set(&reg->a, 0x3); break;
	case 0xE0: set(&reg->b, 0x4); break;	
	case 0xE1: set(&reg->c, 0x4); break;
	case 0xE2: set(&reg->d, 0x4); break;
	case 0xE3: set(&reg->e, 0x4); break;
	case 0xE4: set(&reg->h, 0x4); break;
	case 0xE5: set(&reg->l, 0x4); break;
	case 0xE6: nop(); break; //TODO
	case 0xE7: set(&reg->a, 0x4); break;
	case 0xE8: set(&reg->b, 0x5); break;	
	case 0xE9: set(&reg->c, 0x5); break;
	case 0xEA: set(&reg->d, 0x5); break;
	case 0xEB: set(&reg->e, 0x5); break;
	case 0xEC: set(&reg->h, 0x5); break;
	case 0xED: set(&reg->l, 0x5); break;
	case 0xEE: nop(); break; //TODO
	case 0xEF: set(&reg->a, 0x5); break;
	case 0xF0: set(&reg->b, 0x6); break;	
	case 0xF1: set(&reg->c, 0x6); break;
	case 0xF2: set(&reg->d, 0x6); break;
	case 0xF3: set(&reg->e, 0x6); break;
	case 0xF4: set(&reg->h, 0x6); break;
	case 0xF5: set(&reg->l, 0x6); break;
	case 0xF6: nop(); break; //TODO
	case 0xF7: set(&reg->a, 0x6); break;
	case 0xF8: set(&reg->b, 0x7); break;	
	case 0xF9: set(&reg->c, 0x7); break;
	case 0xFA: set(&reg->d, 0x7); break;
	case 0xFB: set(&reg->e, 0x7); break;
	case 0xFC: set(&reg->h, 0x7); break;
	case 0xFD: set(&reg->l, 0x7); break;
	case 0xFE: nop(); break; //TODO
	case 0xFF: set(&reg->a, 0x7); break;
		
	default: return (-1);
		
	}
	
	
	
	return (0);
}

/**
 * No-op.
 */
void nop() { 
}

/**
 * Adds src register to dest register. Z 0 H C
 * TODO set flag
 */
void add(uint8_t *dest, uint8_t *src, uint8_t *flag)
{
	int prev = *dest;
	
	*flag = clear(*flag);
	
	*dest += *src;
	
	if (*dest == 0) {
		*flag = setz(*flag);
	}
	//todo set H, C
}

/**
 * Adds src register to dest register. Z 0 H C
 * TODO set flag
 */
void sub(uint8_t *dest, uint8_t *src, uint8_t *flag)
{
	int prev = *dest;
	
	*flag = clear(*flag);
	
	*dest -= *src;
	
	if (*dest == 0) {
		*flag = setz(*flag);
	}
}


/*
 * Increment the parameter register. Z 0 H (U)
 * TODO set flag
 */
void inc(uint8_t *reg, uint8_t *f)
{
	*reg++;
}

/*
 * Increment the parameter 2 byte register. Z 0 H (U)
 * TODO set flag
 * There are endian-related problems to be fixed here
 */
void inc(uint8_t *reg_hi, uint8_t *reg_lo, uint8_t *f)
{
	uint16_t *reg;
	
	*reg = *reg_hi + (*reg_lo << 0x8);
	printf("%04x\n", *reg);
	*reg_hi = *reg >> 0x8;
	*reg_lo = (*reg << 0x8) >> 0x8;
}

/*
 * Decrement the parameter register. Z 1 H (U)
 * TODO set flag
 */
void dec(uint8_t *reg, uint8_t *f)
{
	*reg--;
}

/*
 * Logical or of *dest and *src. Result in dest. Z 0 0 0
 * TODO set flag
 */
void lor(uint8_t *dest, uint8_t *src, uint8_t *f)
{
	*dest |= *src;
}

/*
 * Logical and of *dest and *src. Result in dest. Z 0 1 0
 * TODO set flag
 */
void land(uint8_t *dest, uint8_t *src, uint8_t *f)
{
	*dest &= *src;
}

/*
 * Logical xor of *dest and *src. Result in dest. Z 0 0 0
 * TODO set flag
 */
void lxor(uint8_t *dest, uint8_t *src, uint8_t *f)
{
	*dest ^= *src;
}

/*
 * Load src into destination register.
 */
void ld(uint8_t *dest, uint8_t *src)
{
	*dest = *src;
}

/*
 * Compare reg with n. Z 1 H C
 */
void cp(uint8_t *reg, uint8_t *n, uint8_t *f)
{
	uint8_t res;
	
	res = *reg - *n;
	
	//set flags..
}

/*
 * Pop two bytes from stack and jump to the resulting address.
 */
void ret(uint16_t *sp, uint16_t *pc)
{
	*pc = *sp++;
}

/*
 * Set bit b in register reg.
 */
void set(uint8_t *reg, uint8_t b)
{
	*reg |= (0x1 << b);
}

/*
 * Reset bit b in register reg.
 */
void res(uint8_t *reg, uint8_t b)
{
	*reg &= ~(0x1 << b);

}

/*
 * Test bit b in register reg. Z 0 1 (U)
 * TODO set flag
 */
void bit(uint8_t *reg, uint8_t b, uint8_t *f)
{
	uint8_t res;
	
	res = *reg & (0x1 << b);

}

/*
 * Rotate the parameter register left. Set old bit 7 to carry. Z 0 0 C
 * TODO set flag
 */
void rlc(uint8_t *reg, uint8_t *f)
{
	*reg << 0x1;
}

/*
 * Rotate the parameter register right. Set old bit 7 to carry. Z 0 0 C
 * TODO set flag
 */
void rrc(uint8_t *reg, uint8_t *f)
{
	*reg >> 0x1;
}

/*
 * Swap the high and low bits of the parameter register. Z 0 0 0
 * TODO set flag
 */
void swap(uint8_t *reg, uint8_t *f)
{
	uint8_t res;
	
	res = (*reg >> 0x4) + (*reg << 0x4);
	*reg = res;
}

//note: might need to take endianess into account

int main() 
{
	RAM ram;
	registers reg;
	
	init_ram(&ram);
	init_registers(&reg);
	reg.a = 0x1;
	reg.b = 0x2;
	ram.mem[reg.pc] = 0x04;
	ram.mem[reg.pc + 1] = 0x90;
	/*for (;;) {
		if (fetch_decode(&ram, &reg) < 0)
			return (-1);
	}*/
	
	fetch_decode(&ram, &reg);
	fetch_decode(&ram, &reg);
		
	printf("%d\n", reg.a);
	
	uint8_t hi = 0xAB;
	uint8_t lo = 0xCD;
	inc(&lo, &hi, NULL);
	
	printf("%02x \t %02x\n", hi, lo);
	
	swap(&hi, NULL);
	swap(&lo, NULL);
	
	printf("%02x \t %02x\n", hi, lo);
		
	return (0); 
}