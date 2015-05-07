/* $Id: reader.c,v 1.1 2015/01/25 12:49:24 clare Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

#include "proto.h"

uint8_t rd_buf[256];
uint8_t *rd_fname;
int rd_fd;
int rd_addr;
int rd_size;

void rd_set_fname(uint8_t *fname)
{
    rd_fname = fname;
}

void rd_set_addr(int addr)
{
    rd_addr = addr;
}

void rd_set_size(int size)
{
    rd_size = size;
}

int rd_open()
{
    rd_fd = open(rd_fname, O_WRONLY|O_CREAT, 0777);
    if (rd_fd == -1)
        goto bad;
    return 0;
bad:
    return -1;
}

int rd_close()
{
    if (rd_fd >= 0)
        close(rd_fd);
}

int rd_exec()
{
    int pg, pg0, pg1;

    pg0 = rd_addr >> 8;
    pg1 = (rd_addr+rd_size-1) >> 8;
    printf("pages from 0x%04X to 0x%04X will be read\n", pg0, pg1);
    for (pg=pg0; pg<=pg1; pg++)
    {
        printf("page 0x%04X\n", pg);
	dev_page_read(pg, rd_buf);
	write(rd_fd, rd_buf, 256);
    }
}
