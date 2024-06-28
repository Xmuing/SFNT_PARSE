#ifndef _TTF_FS_DRIVER_H_
#define _TTF_FS_DRIVER_H_

#include "stdio.h"
#include "stdint.h"

#define SWAP16(x) ((uint16_t) \
    ((((x) & 0xff00) >> 8) | \
     (((x) & 0x00ff) << 8)))

#define SWAP32(x) \
    ((((x) & 0xff000000) >> 24) | \
     (((x) & 0x00ff0000) >>  8) | \
     (((x) & 0x0000ff00) <<  8) | \
     (((x) & 0x000000ff) << 24))

# define SWAP64(x) \
    ((((x) & 0xff00000000000000ull)>> 56)  | \
     (((x) & 0x00ff000000000000ull) >> 40) | \
     (((x) & 0x0000ff0000000000ull) >> 24) | \
     (((x) & 0x000000ff00000000ull) >> 8)  | \
     (((x) & 0x00000000ff000000ull) << 8)  | \
     (((x) & 0x0000000000ff0000ull) << 24) | \
     (((x) & 0x000000000000ff00ull) << 40) | \
     (((x) & 0x00000000000000ffull) << 56))


typedef FILE file_t;

void ttf_open(file_t **file,const char *path);
void ttf_close(file_t *file);
void ttf_read(file_t *file,void* buff,size_t size);
void ttf_read16(file_t *file,void* buff,size_t size);
void ttf_read32(file_t *file,void* buff,size_t size);
void ttf_read64(file_t *file,void* buff,size_t size);
void ttf_write(file_t *file,void* buff,size_t size);
void ttf_seek(file_t *file,size_t offset);
size_t ttf_tell(file_t *file);

#endif