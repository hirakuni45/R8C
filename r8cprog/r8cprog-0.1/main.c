/* $Id: main.c,v 1.7 2015/01/28 14:16:04 clare Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "serial.h"
#include "proto.h"
#include "reader.h"
#include "writer.h"

int verbose;

void usage()
{
    printf("r8cprog -d tty [-b bps] [-r outfile -a addr -s size] [-w infile] [-v level]\n");
}

int main(int argc, char **argv)
{
    int ch, val;
    int speed = 38400;
    char do_read = 0;
    char do_write = 0;

    while (ch=getopt(argc, argv, "b:d:r:w:a:s:v:"), ch!=-1)
    {
        switch (ch)
	{
	case 'b':
	    val = strtol(optarg, NULL, 0);
	    speed = val;
	    break;
	case 'd':
	    ser_fname(optarg);
	    break;
        case 'r':
	    do_read = 1;
	    rd_set_fname(optarg);
	    break;
        case 'w':
	    do_write = 1;
	    wr_set_fname(optarg);
	    break;
        case 'a':
	    val = strtol(optarg, NULL, 0);
	    rd_set_addr(val);
	    break;
        case 's':
	    val = strtol(optarg, NULL, 0);
	    rd_set_size(val);
	    break;
        case 'v':
	    val = strtol(optarg, NULL, 0);
	    verbose |= val;
	}
    }
    if (!do_read && !do_write)
    {
        usage();
	return 0;
    }
    if (ser_open())
        goto bad;
    if (dev_sync())
        goto bad0;
    if (dev_speed(speed))
        goto bad0;
    dev_status();
    if ((dev_srd1 & SRD1_ID_MASK) != SRD1_ID_OK)
    {
        printf("device is not blank\n");
        dev_id_zero();
        dev_status();
        if ((dev_srd1 & SRD1_ID_MASK) != SRD1_ID_OK)
	{
            printf("device ID is not 00:00:00:00:00:00:00\n");
            dev_id_fill();
            dev_status();
            if ((dev_srd1 & SRD1_ID_MASK) != SRD1_ID_OK)
	    {
                printf("device ID is not FF:FF:FF:FF:FF:FF:FF\n");
	        goto bad1;
            }
	}
    }
    if (do_read)
    {
        if (rd_open())
	{
	    perror(rd_fname);
	    goto bad1;
	}
	rd_exec();
	rd_close();
    }
    if (do_write)
    {
        if (wr_open())
	{
	    perror(rd_fname);
	    goto bad1;
	}
	wr_load();
	wr_exec();
	wr_close();
    }
    dev_speed(9600);
    ser_close();
    return 0;

bad1:
    dev_speed(9600);
bad0:
    ser_close();
bad:
    return -1;
}
