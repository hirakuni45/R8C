/* $Id: proto.h,v 1.3 2015/01/26 14:46:33 clare Exp $ */

int dev_sync();
int dev_speed(int);
int dev_status();
int dev_id_zero();
int dev_id_fill();
int dev_poll_flash();
int dev_page_erase(uint16_t);
int dev_page_write(uint16_t, uint8_t *);
int dev_page_read(uint16_t, uint8_t *);

extern uint8_t dev_srd;
#define SRD_FLASH_READY	0x80
#define	SRD_ERASE_ERR	0x20
#define	SRD_WRITE_ERR	0x10

extern uint8_t dev_srd1;
#define	SRD1_ID_MASK	0x0C
#define	SRD1_ID_PENDING	0x00
#define	SRD1_ID_ERR	0x04
#define SRD1_ID_RESERVED 0x08
#define	SRD1_ID_OK	0x0C
