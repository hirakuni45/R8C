/* $Id: reader.h,v 1.1 2015/01/25 12:49:24 clare Exp $ */

void rd_set_fname(uint8_t *);
void rd_set_addr(int);
void rd_set_size(int);
int rd_open();
int rd_exec();
int rd_close();

extern uint8_t *rd_fname;
