#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include "S3Sim.h"

extern "C"
{
  #include "m68k.h"
}

//#define DEBUG


void disassemble_program();

typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned int   LONG;

template<typename T>
T READ(unsigned char* base, unsigned int addr);

template<>
inline
BYTE READ<BYTE>(unsigned char* base, unsigned int addr)
{
  return base[addr] & 0xff;
}

template<>
inline
WORD READ<WORD>(unsigned char* base, unsigned int addr)
{
  return ((base[addr]<<8) | base[addr+1]) & 0xffff;
}

template<>
inline
LONG READ<LONG>(unsigned char* base, unsigned int addr)
{
  return ((base[addr]<<24) | (base[addr+1]<<16) | (base[addr+2]<<8) | (base[addr+3])) & 0xffffffff;
}

template<typename T>
void WRITE(unsigned char* base, unsigned int addr, T value);

template<>
inline
void WRITE<BYTE>(unsigned char* base, unsigned int addr, BYTE value)
{
  base[addr] = value & 0xff;
}

template<>
inline
void WRITE<WORD>(unsigned char* base, unsigned int addr, WORD value)
{
  base[addr]   = value>>8 & 0xff;
  base[addr+1] = value & 0xff;
}

template<>
inline
void WRITE<LONG>(unsigned char* base, unsigned int addr, LONG value)
{
  base[addr]   = value>>24 & 0xff;
  base[addr+1] = value>>16 & 0xff;
  base[addr+2] = value>>8 & 0xff;
  base[addr+3] = value & 0xff;
}

/* Data */
unsigned int  g_fc;                             /* Current function code from CPU */

#define SZ_ROM (1<<22)
#define SZ_RAM (1<<21)
#define SZ_IMP (1<<12)
#define SZ_SCR (1<<8)
#define SZ_CS2 (1<<19)
#define SZ_CS3 (1<<18)

unsigned char g_rom[SZ_ROM]; /* ROM */
unsigned char g_ram[SZ_RAM]; /* RAM */
unsigned char g_cs2[SZ_CS2];
unsigned char g_cs3[SZ_CS3];
unsigned char g_imp[SZ_IMP]; /* IMP */
unsigned char g_scr[SZ_SCR]; /* SCR */

struct MemArea
{
  unsigned char* data;
  unsigned int   size;
};

      
MemArea g_mems[4] = {
    {g_rom,SZ_ROM},
    {g_ram,SZ_RAM},
    {g_cs2,SZ_CS2},
    {g_cs3,SZ_CS3},
};


void dump_meminfo()
{
  static unsigned short _br, _or;
  for(unsigned short i=0; i<4; ++i)
  {
    _br = READ<WORD>(g_imp,0x830+i*4); //BRx
    _or = READ<WORD>(g_imp,0x832+i*4); //ORx
    printf("/CS%d: offset %08x mask %08x\n", i, (_br & 0x1ffc) << 11, (_or & 0x1ffc) << 11);
  }
}

int dump_memfile(const char* filename, unsigned char* data, size_t size)
{
  FILE* fhandle;
  if((fhandle = fopen(filename, "wb")) == NULL)
  {
    printf("Unable to open %s", filename);
    return -1;
  }
  
  
  if(fwrite(data, 1, size, fhandle) <= 0)
  {
    printf("Error  %s", filename);
    fclose(fhandle);
    return -1;
  }
  fclose(fhandle);
  return 0;
}
  
void handle_SIGINT()
{
  char filename[20];
  printf("Writing memory areas to files\n");
  for(unsigned short i=0; i<4; ++i)
  {
    snprintf(filename,100,"memdump.%d.bin",i);
    if(dump_memfile(filename,g_mems[i].data,g_mems[i].size)<0)
        return;
  }
  if(dump_memfile("memdump.scr.bin",g_scr,SZ_SCR)<0)
      return;
  if(dump_memfile("memdump.imp.bin",g_imp,SZ_IMP)<0)
      return;
}


unsigned int   g_sub_address;
unsigned char* g_sub;
unsigned int decode_address(unsigned int address)
{
  static unsigned int _br, _or, _base, _mask, _pc;
  //printf("%03x 0x%016x\n", g_fc, address);
  if(0xf0 <= address && address < 0xff)
  {
    g_sub = g_scr;
    g_sub_address = address;
    return 1;
  }
  else if( ((address >> 12) ^ (READ<WORD>(g_scr, 0xf2) & 0xfff)) == 0)
  {
    g_sub = g_imp;
    g_sub_address = address & 0xfff;
    if (g_sub_address >= SZ_IMP)
    {
      printf("IMP Error: %04x >= %04x\n", g_sub_address, SZ_IMP);
      exit(1);
    }
    _pc = m68k_get_reg(NULL, M68K_REG_PC);
    printf("%08x IMP: %04x\n", _pc, address);
    return 1;
  }
  else {
    for (unsigned short i=0; i<4; ++i)
    {
      _br  = READ<WORD>(g_imp,0x830 + i*4);
      if(!(_br & 0x1)) continue;
      _or  = READ<WORD>(g_imp,0x832 + i*4);
      if((_or & 0x1) && !(_br & 0xe000)) continue;
      _base = _br & 0x1ffc;
      _mask = _or & 0x1ffc;
      if( (((address>>11) & _mask) ^ (_base & _mask)) == 0)
      {
        //printf("/CS%d: %04x %04x %04x\n", i, _base, _mask, address);
        g_sub_address = address & ~(_base<<11);
        if( g_sub_address >= g_mems[i].size)
        {
          _pc = m68k_get_reg(NULL, M68K_REG_PC);
          printf("%08x /CS%d: %04x %04x %04x\n", _pc, i, _base, _mask, address);
          printf("/CS%d: out of bounds access: %d >= %d\n", i, g_sub_address, g_mems[i].size);
          dump_meminfo();
          exit(1);
        }
        g_sub = g_mems[i].data;
        return 1;
      }
    }
  }

  if(address < SZ_ROM)
  {
    g_sub_address = address;
    g_sub = g_rom;
    return 1;
  }
  
  g_sub_address = 0;
  g_sub = 0;
  return 0;
}

void init_mem()
{
  for(unsigned short i=0; i<4; ++i)
  {
    memset(g_mems[i].data, 0, g_mems[i].size);
  }
  memset(g_scr, 0, SZ_SCR);
  memset(g_imp, 0, SZ_IMP);

  // initialize MC68302 sytem configuration registers
  WRITE<WORD>(g_scr,0xf2,0xbfff);
  WRITE<LONG>(g_scr,0xf4,0xf00);
  // initialize MC68302 internal registers
  WRITE<WORD>(g_imp,0x812,0x0000); //GIMR
  WRITE<WORD>(g_imp,0x830,0xc001); //BR0
  WRITE<WORD>(g_imp,0x832,0xdffd); //OR0
  WRITE<WORD>(g_imp,0x834,0xc000); //BR1
  WRITE<WORD>(g_imp,0x836,0xdffd); //OR1
  WRITE<WORD>(g_imp,0x838,0xc000); //BR2
  WRITE<WORD>(g_imp,0x83a,0xdffd); //OR2
  WRITE<WORD>(g_imp,0x83c,0xc000); //BR3
  WRITE<WORD>(g_imp,0x83e,0xdffd); //OR3
}

/* Exit with an error message.  Use printf syntax. */
void exit_error(const char* fmt, ...)
{
  static int guard_val = 0;
  char buff[100];
  unsigned int pc;
  va_list args;

  if(guard_val)
      return;
  else
      guard_val = 1;

  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
  fprintf(stderr, "\n");
  pc = m68k_get_reg(NULL, M68K_REG_PPC);
  m68k_disassemble(buff, pc, M68K_CPU_TYPE_68000);
  fprintf(stderr, "At %04x: %s\n", pc, buff);

  exit(EXIT_FAILURE);
}


/* Read data from RAM, ROM, or a device */
unsigned int cpu_read_byte(unsigned int address)
{
  if(!decode_address(address))
      exit_error("Error decoding address %08x", address);
  
  return READ<BYTE>(g_sub, g_sub_address);
}

unsigned int cpu_read_word(unsigned int address)
{
  if(!decode_address(address))
      exit_error("Error decoding address %08x", address);
  
  return READ<WORD>(g_sub, g_sub_address);
}

unsigned int cpu_read_long(unsigned int address)
{
  if(!decode_address(address))
      exit_error("Error decoding address %08x", address);
  
  return READ<LONG>(g_sub, g_sub_address);
}


unsigned int cpu_read_word_dasm(unsigned int address)
{
  if(!decode_address(address))
      exit_error("Disassembler attempted to read word from ROM address %08x", address);
  
  return READ<WORD>(g_sub, g_sub_address);
}

unsigned int cpu_read_long_dasm(unsigned int address)
{
  if(!decode_address(address))
      exit_error("Disassembler attempted to read word from ROM address %08x", address);
  
  return READ<LONG>(g_sub, g_sub_address);
}


/* Write data to RAM or a device */
void cpu_write_byte(unsigned int address, unsigned int value)
{
  if(g_fc & 2)	/* Program */
      exit_error("Attempted to write %02x to ROM address %08x", value&0xff, address);

  if(!decode_address(address))
      exit_error("Error decoding address %08x", address);
  
  WRITE<BYTE>(g_sub, g_sub_address, value);
}

void cpu_write_word(unsigned int address, unsigned int value)
{
  if(g_fc & 2)	/* Program */
      exit_error("Attempted to write %04x to ROM address %08x", value&0xffff, address);

  if(!decode_address(address))
      exit_error("Error decoding address %08x", address);
  
  WRITE<WORD>(g_sub, g_sub_address, value);
}

void cpu_write_long(unsigned int address, unsigned int value)
{
  if(g_fc & 2)	/* Program */
      exit_error("Attempted to write %08x to ROM address %08x", value, address);

  if(!decode_address(address))
      exit_error("Error decoding address %08x", address);
  
  WRITE<LONG>(g_sub, g_sub_address, value);
}

/* Called when the CPU pulses the RESET line */
void cpu_pulse_reset(void)
{
  printf("RESET\n");
}

/* Called when the CPU changes the function code pins */
void cpu_set_fc(unsigned int fc)
{
  g_fc = fc;
}

/* Called when the CPU acknowledges an interrupt */
int cpu_irq_ack(int level)
{
  printf("Interrupt: %x\n", level);
  return M68K_INT_ACK_SPURIOUS;
}

/* Disassembler */
void make_hex(char* buff, unsigned int pc, unsigned int length)
{
  char* ptr = buff;

  for(;length>0;length -= 2)
  {
    sprintf(ptr, "%04x", cpu_read_word_dasm(pc));
    pc += 2;
    ptr += 4;
    if(length > 2)
        *ptr++ = ' ';
  }
}

void disassemble_program()
{
  unsigned int pc;
  unsigned int instr_size;
  char buff[100];
  char buff2[100];

  pc = cpu_read_long_dasm(4);

  while(pc <= 0x16e)
  {
    instr_size = m68k_disassemble(buff, pc, M68K_CPU_TYPE_68000);
    make_hex(buff2, pc, instr_size);
    printf("%03x: %-20s: %s\n", pc, buff2, buff);
    pc += instr_size;
  }
  fflush(stdout);
}

void print_cur_instr()
{
  static char buff[100];
  static char buff2[100];
  static unsigned int pc;
  static unsigned int instr_size;

  pc = m68k_get_reg(NULL, M68K_REG_PC);
  instr_size = m68k_disassemble(buff, pc, M68K_CPU_TYPE_68000);
  make_hex(buff2, pc, instr_size);
  printf("E %03x: %-20s: %s\n", pc, buff2, buff);
  fflush(stdout);
}

void cpu_instr_callback()
{
  static char buff[100];
  static unsigned int pc;
  pc = m68k_get_reg(NULL, M68K_REG_PC);
  m68k_disassemble(buff, pc, M68K_CPU_TYPE_68000);
  if (strcmp(buff,"ILLEGAL") == 0)
  {
    print_cur_instr();
    exit(1);
  }
  

#ifdef DEBUG
  print_cur_instr();
#endif
}

void sig_handler(int signo)
{
  if (signo == SIGINT)
  {
    handle_SIGINT();
    exit(0);
  }
  else if( signo == SIGUSR1)
  {
    print_cur_instr();
  }
}

void register_signal_handlers()
{
  if (signal(SIGINT, sig_handler) == SIG_ERR)
  {
    printf("Error registering handler for SIGINT\n");
    exit(1);
  }
  if (signal(SIGUSR1, sig_handler) == SIG_ERR)
  {
    printf("Error registering handler for SIGUSR1\n");
    exit(1);
  }
}

/* The main loop */
int main(int argc, char* argv[])
{
  FILE* fhandle;

  if(argc != 2)
  {
    printf("Usage: sim <program file>\n");
    exit(-1);
  }

  init_mem();
  
  if((fhandle = fopen(argv[1], "rb")) == NULL)
      exit_error("Unable to open %s", argv[1]);

  if(fread(g_rom, 1, SZ_ROM, fhandle) <= 0)
      exit_error("Error reading %s", argv[1]);

  fclose(fhandle);
  
  register_signal_handlers();
  
//  disassemble_program();
  
  m68k_init();
  m68k_set_cpu_type(M68K_CPU_TYPE_68000);
  m68k_pulse_reset();

  while(true)
  {
    m68k_execute(100000);
  }

  return 0;
}
