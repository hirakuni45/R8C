/* $Id: writer.h,v 1.2 2015/01/26 14:46:33 clare Exp $ */

typedef struct wr_page wr_page_t;
struct wr_page
{
    struct wr_page *wr_next;
    uint8_t *wr_buf;
    int wr_page;
};

void wr_set_fname(uint8_t const *);
int wr_open();
int wr_close();
int wr_load();
int wr_exec();
