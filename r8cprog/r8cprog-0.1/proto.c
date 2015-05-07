/* $Id: proto.c,v 1.4 2015/01/27 12:00:33 clare Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "proto.h"
#include "serial.h"

uint8_t dev_srd;
uint8_t dev_srd1;

int dev_sync()
{
    int i;
    uint8_t ch;

    printf("synchronizing\n");
    for (i=0; i<16; i++)
    {
        ser_putch(0);
	ser_drain();
    }
    ser_flush_log();
    ser_putch(0xB0);
    ch = ser_getch();
    if (ch == 0xB0)
    {
        ser_flush_log();
        printf("synchro ok\n");
	return 0;
    }
    else
    {
        ser_flush_log();
        printf("synchro failure\n");
	return -1;
    }
}

int dev_speed(int newspeed)
{
    uint8_t cmd, res;

    switch (newspeed)
    {
    case 9600:
        cmd = 0xB0;
	goto basic;
    case 19200:
        cmd = 0xB1;
	goto basic;
    case 38400:
        cmd = 0xB2;
	goto basic;
    case 57600:
        cmd = 0xB3;
	goto basic;
	break;
    case 115200:
        cmd = 0xB4;
    basic:
        ser_putch(cmd);
	res = ser_getch();
	ser_flush_log();
	if (res != cmd)
	{
	    fprintf(stderr, "synchro failure\n");
	    goto bad;
	}
	ser_speed(newspeed);
	printf("synchro ok to %dbps\n", newspeed);
	break;
    }
    return 0;
bad:
    return -1;
}

int dev_version()
{
    int i;

    ser_putch(0xFB);
    for (i=0; i<8; i++)
        ser_getch();
    ser_flush_log();
}

int dev_status()
{
    uint8_t srd;
    uint8_t srd1;

    ser_putch(0x70);
    srd  = ser_getch();
    srd1 = ser_getch();
    ser_flush_log();
    dev_srd  = srd;
    dev_srd1 = srd1;

    return (uint16_t)srd1 << 8 | srd;
}

int dev_poll_flash()
{
    int res;
    int cnt;

    cnt = 0;
    for (;;)
    {
        usleep(20*1000);
        ser_putch(0x70);
        res = ser_pollch();
        if (res == -1)
	{
	    cnt++;
            continue; /* timeout */
        }
        if (res == -2)
	    return -1; /* failure */
	ser_getch();
        if (res & 0x80)
	    break;
	cnt++;
    }
    return 0;
}

int dev_id_zero()
{
    ser_putch(0xF5);
    ser_putch(0xDF);
    ser_putch(0xFF);
    ser_putch(0x00);
    ser_putch(0x07);

    ser_putch(0x00);
    ser_putch(0x00);
    ser_putch(0x00);
    ser_putch(0x00);
    ser_putch(0x00);
    ser_putch(0x00);
    ser_putch(0x00);
    ser_flush_log();
}

int dev_id_fill()
{
    ser_putch(0xF5);
    ser_putch(0xDF);
    ser_putch(0xFF);
    ser_putch(0x00);
    ser_putch(0x07);

    ser_putch(0xFF);
    ser_putch(0xFF);
    ser_putch(0xFF);
    ser_putch(0xFF);
    ser_putch(0xFF);
    ser_putch(0xFF);
    ser_putch(0xFF);
    ser_flush_log();
}

int dev_page_erase(uint16_t page)
{
    uint8_t hi;
    uint8_t lo;

    ser_putch(0x50); /* clear status */
    hi = page>>8;
    lo = page;
    ser_putch(0x20); /* block erase */
    ser_putch(lo);
    ser_putch(hi);
    ser_putch(0xD0); /* confirm command */
    ser_flush_log();
    dev_status();
}

int dev_page_write(uint16_t page, uint8_t *ptr)
{
    uint8_t hi;
    uint8_t lo;
    int cnt;

    ser_putch(0x50); /* clear status */
    hi = page>>8;
    lo = page;
    ser_putch(0x41); /* block write */
    ser_putch(lo);
    ser_putch(hi);
    for (cnt=0; cnt<256; cnt++)
        ser_putch(ptr[cnt]);
    ser_flush_log();
    dev_status();
}

int dev_page_read(uint16_t page, uint8_t *ptr)
{
    uint8_t hi;
    uint8_t lo;
    int cnt;

    ser_putch(0x50); /* clear status */
    hi = page>>8;
    lo = page;
    ser_putch(0xFF); /* block read */
    ser_putch(lo);
    ser_putch(hi);
    for (cnt=0; cnt<256; cnt++)
        ptr[cnt] = ser_getch();
    ser_flush_log();
}
