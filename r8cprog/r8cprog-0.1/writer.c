/* $Id: writer.c,v 1.2 2015/01/26 14:46:33 clare Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "writer.h"
#include "proto.h"
#include "serial.h"

uint8_t const *wr_fname;
FILE *wr_fp;
int wr_char;
int wr_lnsum;
int wr_lnoff;
int wr_lncnt;
uint8_t wr_lnbuf[256];
wr_page_t *wr_top;

void wr_set_fname(uint8_t const *ptr)
{
    wr_fname = ptr;
}

int wr_open()
{
    FILE *fp;

    fp = fopen(wr_fname, "r");
    if (fp == NULL)
        goto bad;
    wr_fp = fp;
    wr_lncnt = 1; /* XXX */
    return 0;

bad:
    return -1;
}

void wr_nextch()
{
    wr_char = fgetc(wr_fp);
}

int wr_load_line()
{
    int sum = 0;
    int val;

    while (wr_char != ':')
    {
        if (wr_char == '\n')
	    wr_lncnt++;
	wr_nextch();
    }
    wr_lnoff = 0;
    wr_lnsum = 0;
    if (wr_char == ':')
        wr_nextch();
    while (wr_char >= 32)
    {
        val = 0;
	if (wr_char >= '0' && wr_char <= '9')
	    val = (val << 4) + wr_char - '0';
        else if (wr_char >= 'A' && wr_char <= 'F')
	    val = (val << 4) + wr_char - 'A' + 10;
        wr_nextch();

	if (wr_char >= '0' && wr_char <= '9')
	    val = (val << 4) + wr_char - '0';
        else if (wr_char >= 'A' && wr_char <= 'F')
	    val = (val << 4) + wr_char - 'A' + 10;
        wr_nextch();

	if (wr_lnoff >= sizeof(wr_lnbuf))
	{
            fprintf(stderr, "*** buffer overrun in line %d\n", wr_lncnt);
	    goto bad;
	}
        wr_lnbuf[wr_lnoff] = val;
	wr_lnoff++;
	wr_lnsum += val;
    }
    wr_lnsum &= 255;
    if (wr_lnsum != 0)
    {
        fprintf(stderr, "*** checksum error in line %d\n", wr_lncnt);
	goto bad;
    }
    if (wr_char == '\r')
        wr_nextch();
    if (wr_char == '\n')
        wr_nextch(), wr_lncnt++;
    return 0;

bad:
    return -1;
}

wr_page_t *wr_new_page(int page)
{
    wr_page_t *pg;

    pg = malloc(sizeof(*pg));
    pg->wr_page = page;
    pg->wr_buf = malloc(256);
    memset(pg->wr_buf, -1, 256);
    return pg;
}

wr_page_t *wr_get_page(int page)
{
    wr_page_t *pg;

    for (pg=wr_top; pg; pg=pg->wr_next)
        if (pg->wr_page == page)
	    return pg;
    pg = wr_new_page(page);
    pg->wr_next = wr_top;
    wr_top = pg;
    return pg;
}

int wr_apply_line()
{
    uint8_t *ptr;
    int left;
    int ilen;
    int iaddr;
    int itype;
    int page;
    int offs;
    wr_page_t *pg;

    ptr = wr_lnbuf;
    left = wr_lnoff;

    ilen  = *ptr++; left--;
    iaddr = *ptr++ << 8; left--;
    iaddr |= *ptr++ << 0; left--;
    itype = *ptr++; left--;

    printf("%02X %04X %02X\n", ilen, iaddr, itype);

    switch (itype)
    {
    case 0:
        page = iaddr >> 8;
	offs = iaddr & 0xFF;
	pg = wr_get_page(page);
	memcpy(pg->wr_buf+offs, ptr, ilen);
	break;

    case 1:
        break;
    }
    return 0;
}

int wr_load()
{
    int res;

    while (wr_char != EOF)
    {
        res = wr_load_line();
	if (res < 0)
	{
	    fprintf(stderr, "load error\n");
	    goto bad;
        }
        res = wr_apply_line();
	if (res < 0)
	{
	    fprintf(stderr, "apply error\n");
	    goto bad;
        }
    }
    return 0;

bad:
    return -1;
}

int wr_exec()
{
    int cnt;
    wr_page_t *pg;

    cnt = 0;
    for (pg=wr_top; pg; pg=pg->wr_next)
        cnt++;
    printf("%d page(s) will be written\n", cnt);
    for (pg=wr_top; pg; pg=pg->wr_next)
    {
        dev_page_erase(pg->wr_page);
        dev_page_write(pg->wr_page, pg->wr_buf);
    }
}

int wr_close()
{
    fclose(wr_fp);
    return 0;
}
