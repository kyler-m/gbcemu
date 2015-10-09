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
void	ld_mem(RAM *ram, uint8_t *mem_offset, uint8_t *reg);
void	ld_reg(RAM *ram, uint8_t *reg, uint8_t *mem_offset);
void 	inc(uint8_t *reg, uint8_t *f);
void 	dec(uint8_t *reg, uint8_t *f);
int	fetch_decode(RAM *ram, int pc);
int	decode1B(RAM *ram, registers *reg, uint8_t opc);
int	decode2B(RAM *ram, registers *reg, uint8_t opc);


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
		return decode2B(ram, reg, opc);
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
	
	case 0x40: ld_reg(ram, &reg->b, &reg->b); break;
	case 0x41: ld_reg(ram, &reg->b, &reg->c); break;
	case 0x42: ld_reg(ram, &reg->b, &reg->d); break;
	case 0x43: ld_reg(ram, &reg->b, &reg->e); break;
	case 0x44: ld_reg(ram, &reg->b, &reg->h); break;
	case 0x45: ld_reg(ram, &reg->b, &reg->l); break;
	case 0x46: nop(); break; //TODO
	case 0x47: ld_reg(ram, &reg->b, &reg->a); break;
	case 0x48: ld_reg(ram, &reg->c, &reg->b); break;
	case 0x49: ld_reg(ram, &reg->c, &reg->c); break;
	case 0x4A: ld_reg(ram, &reg->c, &reg->d); break;
	case 0x4B: ld_reg(ram, &reg->c, &reg->e); break;
	case 0x4C: ld_reg(ram, &reg->c, &reg->h); break;
	case 0x4D: ld_reg(ram, &reg->c, &reg->l); break;
	case 0x4E: nop(); break; //TODO
	case 0x4F: ld_reg(ram, &reg->c, &reg->a); break;
	
	case 0x50: ld_reg(ram, &reg->d, &reg->b); break;
	case 0x51: ld_reg(ram, &reg->d, &reg->c); break;
	case 0x52: ld_reg(ram, &reg->d, &reg->d); break;
	case 0x53: ld_reg(ram, &reg->d, &reg->e); break;
	case 0x54: ld_reg(ram, &reg->d, &reg->h); break;
	case 0x55: ld_reg(ram, &reg->d, &reg->l); break;
	case 0x56: nop(); break; //TODO
	case 0x57: ld_reg(ram, &reg->d, &reg->a); break;
	case 0x58: ld_reg(ram, &reg->e, &reg->b); break;
	case 0x59: ld_reg(ram, &reg->e, &reg->c); break;
	case 0x5A: ld_reg(ram, &reg->e, &reg->d); break;
	case 0x5B: ld_reg(ram, &reg->e, &reg->e); break;
	case 0x5C: ld_reg(ram, &reg->e, &reg->h); break;
	case 0x5D: ld_reg(ram, &reg->e, &reg->l); break;
	case 0x5E: nop(); break; //TODO
	case 0x5F: ld_reg(ram, &reg->e, &reg->a); break;
	
	case 0x60: ld_reg(ram, &reg->h, &reg->b); break;
	case 0x61: ld_reg(ram, &reg->h, &reg->c); break;
	case 0x62: ld_reg(ram, &reg->h, &reg->d); break;
	case 0x63: ld_reg(ram, &reg->h, &reg->e); break;
	case 0x64: ld_reg(ram, &reg->h, &reg->h); break;
	case 0x65: ld_reg(ram, &reg->h, &reg->l); break;
	case 0x66: nop(); break; //TODO
	case 0x67: ld_reg(ram, &reg->h, &reg->a); break;
	case 0x68: ld_reg(ram, &reg->l, &reg->b); break;
	case 0x69: ld_reg(ram, &reg->l, &reg->c); break;
	case 0x6A: ld_reg(ram, &reg->l, &reg->d); break;
	case 0x6B: ld_reg(ram, &reg->l, &reg->e); break;
	case 0x6C: ld_reg(ram, &reg->l, &reg->h); break;
	case 0x6D: ld_reg(ram, &reg->l, &reg->l); break;
	case 0x6E: nop(); break; //TODO
	case 0x6F: ld_reg(ram, &reg->l, &reg->a); break;
	
	case 0x76: nop(); break; //TODO
	case 0x77: nop(); break; //TODO
	case 0x78: ld_reg(ram, &reg->a, &reg->b); break;
	case 0x79: ld_reg(ram, &reg->a, &reg->c); break;
	case 0x7A: ld_reg(ram, &reg->a, &reg->d); break;
	case 0x7B: ld_reg(ram, &reg->a, &reg->e); break;
	case 0x7C: ld_reg(ram, &reg->a, &reg->h); break;
	case 0x7D: ld_reg(ram, &reg->a, &reg->l); break;
	case 0x7E: nop(); break; //TODO
	case 0x7F: ld_reg(ram, &reg->a, &reg->a); break;

	case 0x80: add(&reg->a, &reg->b, &reg->f); break;
	case 0x81: add(&reg->a, &reg->c, &reg->f); break;
	case 0x82: add(&reg->a, &reg->d, &reg->f); break;
	case 0x83: add(&reg->a, &reg->e, &reg->f); break;
	case 0x84: add(&reg->a, &reg->h, &reg->f); break;
	case 0x85: add(&reg->a, &reg->l, &reg->f); break;
	
	}
	
	return (0);
}

/**
 * Decode a 2 byte instruction.
 *
 * Return -1 on failure, 0 on success.
 */
int decode2B(RAM *ram, registers *reg, uint8_t opc_hi)
{
	uint16_t opc;
	
	opc |= (opc_hi << 4);
	opc |= ram->mem[reg->pc++];
	
	//now opc is loaded up with the whole opcode
	//not really necessary
	
	return (0);
}

/**
 * No-op.
 */
void nop() { }

/**
 * Adds src register to dest register. Sets flag byte; outcome: Z 0 H C
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

/*
 * Load reg into 0xFF00 + mem_offset.
 */
void ld_mem(RAM *ram, uint8_t *mem_offset, uint8_t *reg)
{
	ram->mem[0xFF00 + *mem_offset] = *reg;
}

/*
 * Load 0xFF00 + mem_offset into reg.
 */
void ld_reg(RAM *ram, uint8_t *reg, uint8_t *mem_offset)
{
	*reg = ram->mem[0xFF00 + *mem_offset];
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
 * Decrement the parameter register. Z 1 H (U)
 * TODO set flag
 */
void dec(uint8_t *reg, uint8_t *f)
{
	*reg--;
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
	ram.mem[reg.pc] = 0x80;
	//for (;;) 
		if (fetch_decode(&ram, &reg) < 0)
			return (-1);
		
	printf("%lu\n", reg.a);
		
	return (0); 
}