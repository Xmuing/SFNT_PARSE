/********************************************************************************
 * @Author: xu 1424839890@qq.com
 * @Date: 2024-06-21 08:48:26
 * @LastEditors: xu 1424839890@qq.com
 * @LastEditTime: 2024-06-25 11:13:32
 * @FilePath: \ttf_engine\ttf_fs_driver.c
 * @Description: 
 * @
 * @Copyright (c) 2024 by xu 1424839890@qq.com, All Rights Reserved. 
 ********************************************************************************/
#include "ttf_fs_driver.h"

void ttf_open(file_t **file,const char *name)
{
    *file = fopen(name,"rb");
}

void ttf_close(file_t *file)
{
    if(file == NULL) return;
    fclose(file);
}

void ttf_read(file_t *file,void* buff,size_t size)
{
    fread(buff,1,size,file);
}

void ttf_read16(file_t *file,void* buff,size_t size)
{
    fread(buff,2,size,file);
    uint16_t* p = (uint16_t*)buff;
    for(int i=0;i<size;i++){
        *p = SWAP16(*p);
        p++;
    }
}

void ttf_read32(file_t *file,void* buff,size_t size)
{
    fread(buff,4,size,file);
    uint32_t* p = (uint32_t*)buff;
    for(int i=0;i<size;i++){
        *p = SWAP32(*p);
        p++;
    }
}

void ttf_read64(file_t *file,void* buff,size_t size)
{
    fread(buff,8,size,file);
    uint64_t* p = (uint64_t*)buff;
    for(int i=0;i<size;i++){
        *p = SWAP64(*p);
        p++;
    }
}

void ttf_write(file_t *file,void* buff,size_t size)
{
    fwrite(buff,1,size,file);
}

void ttf_seek(file_t *file,size_t offset)
{
    fseek(file,offset,SEEK_SET);
}

size_t ttf_tell(file_t *file)
{
    return ftell(file);
}
