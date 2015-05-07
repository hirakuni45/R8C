# $Id: Makefile,v 1.6 2015/01/28 14:16:04 clare Exp $

BINDIR=/usr/local/bin
PROG=r8cprog
SRCS=main.c serial.c proto.c reader.c writer.c
NOMAN=1
# LDADD=-ledit

read:
	./${PROG} -d /dev/dtyU0 -a 0x8000 -s 0x8000 -r test.out

write:
	./${PROG} -d /dev/dtyU0 -w leddrv.hex

.include <bsd.prog.mk>
