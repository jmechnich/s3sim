#ifndef SIM__HEADER
#define SIM__HEADER

#ifdef __cplusplus
extern "C" 
{
#endif
  unsigned int cpu_read_byte(unsigned int address);
  unsigned int cpu_read_word(unsigned int address);
  unsigned int cpu_read_long(unsigned int address);
  void cpu_write_byte(unsigned int address, unsigned int value);
  void cpu_write_word(unsigned int address, unsigned int value);
  void cpu_write_long(unsigned int address, unsigned int value);
  unsigned int cpu_read_word_dasm(unsigned int address);
  unsigned int cpu_read_long_dasm(unsigned int address);
  void cpu_pulse_reset(void);
  void cpu_set_fc(unsigned int fc);
  int  cpu_irq_ack(int level);
  void cpu_instr_callback();
#ifdef __cplusplus
}
#endif

#endif /* SIM__HEADER */
