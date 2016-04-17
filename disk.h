#ifndef _DISK_H_
#define _DISK_H_

#define DISK_BLOCKS  10752
#define BLOCK_SIZE   2048

int isDiskCreated(char *name);
int make_disk(char *name);
int open_disk(char *name);
int close_disk();

int block_write(int block, char *buf);
int block_read(int block, char *buf);

#endif
