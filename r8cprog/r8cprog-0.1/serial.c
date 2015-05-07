/* $Id: serial.c,v 1.6 2015/01/27 12:33:04 clare Exp $ */
/*
 * serial port communication
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#include "serial.h"

uint8_t const *serfn;
int serfd;
uint8_t serlogph;
struct termios iossave;
struct termios iosrun;
extern int verbose;

void ser_fname(uint8_t const *fn)
{
    serfn = fn;
}

int ser_open()
{
    int fd;
    struct termios ios;
    uint8_t cmd;

    fd = open(serfn, O_RDWR);
    if (fd == -1)
    {
        perror("open");
        goto bad;
    }
    serfd = fd;
    if (tcgetattr(fd, &iossave))
    {
        perror("tcgetattr");
	close(fd);
	goto bad;
    }
    memset(&ios, 0, sizeof(ios));
    cfsetspeed(&ios, B9600);
    ios.c_cflag |= CS8|CLOCAL|CREAD;
    ios.c_cc[VMIN] = 1;
    ios.c_cc[VTIME] = 0;
    tcsetattr(serfd, TCSADRAIN, &ios);
    iosrun = ios;
    return 0;

bad:
    return -1;
}

int ser_speed(int speed)
{
    cfsetspeed(&iosrun, speed);
    tcsetattr(serfd, TCSADRAIN, &iosrun);
    return 0;
}

int ser_close()
{
    int fd = serfd;

    tcsetattr(serfd, TCSADRAIN, &iossave);
    close(fd);
    serfd = -1;
    return 0;
}

uint8_t ser_getch()
{
    uint8_t ch;
    ssize_t rlen;

    rlen = read(serfd, &ch, sizeof(ch));
    if (rlen == -1)
        return -1;
    ser_read_log(ch);
    return ch;
}

void ser_putch(uint8_t ch)
{
    ssize_t wlen;

    wlen = write(serfd, &ch, sizeof(ch));
    if (wlen == -1)
        return;
    ser_write_log(ch);
    return;
}

void ser_drain()
{
    fd_set rfds;
    int nfds;
    int rv;
    struct timeval tv;

    for (;;)
    {
        FD_ZERO(&rfds);
	FD_SET(serfd, &rfds);
	nfds = serfd+1;
	tv.tv_sec = 0;
	tv.tv_usec = 40*1000;
	rv = select(nfds, &rfds, NULL, NULL, &tv);
	switch (rv)
	{
	case -1:
	    break;
        case 0:
	    return;
        default:
	    ser_getch();
	}
    }
    ser_flush_log();
}

int ser_pollch()
{
    fd_set rfds;
    int nfds;
    int rv;
    struct timeval tv;

    FD_ZERO(&rfds);
    FD_SET(serfd, &rfds);
    nfds = serfd+1;
    tv.tv_sec = 0;
    tv.tv_usec = 40*1000;
    rv = select(nfds, &rfds, NULL, NULL, &tv);
    switch (rv)
    {
    case -1:
        return -2;
    case 0:
        return -1;
    default:
        return ser_getch();
    }
}

void ser_read_log(uint8_t ch)
{
    if ((verbose & 1) == 0)
        return;

    switch (serlogph)
    {
    default:
        printf("\n");
    case 0:
        printf("--->");
    case 1:
        printf(" %02X", ch);
	fflush(stdout);
	serlogph = 1;
    }
}

void ser_write_log(uint8_t ch)
{
    if ((verbose & 1) == 0)
        return;

    switch (serlogph)
    {
    default:
        printf("\n");
    case 0:
        printf("<---");
    case 2:
        printf(" %02X", ch);
	fflush(stdout);
	serlogph = 2;
    }
}

void ser_flush_log()
{
    if ((verbose & 1) == 0)
        return;

    switch (serlogph)
    {
    default:
        printf("\n");
    case 0:
        serlogph = 0;
        break;
    }
}
