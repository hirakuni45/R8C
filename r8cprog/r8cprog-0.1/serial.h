/* $Id: serial.h,v 1.4 2015/01/25 12:49:24 clare Exp $ */

void ser_fname(uint8_t const *);
int ser_open();
int ser_close();
int ser_speed();
uint8_t ser_getch();
void ser_putch(uint8_t);
void ser_drain(void);
int ser_pollch(void);
void ser_read_log(uint8_t);
void ser_write_log(uint8_t);
void ser_flush_log();
