/********************************************************************************
 * @Author: xu 1424839890@qq.com
 * @Date: 2024-06-20 16:07:27
 * @LastEditors: xu 1424839890@qq.com
 * @LastEditTime: 2024-06-26 16:37:02
 * @FilePath: \ttf_engine\ttf_table_head.c
 * @Description: 
 * @
 * @Copyright (c) 2024 by xu 1424839890@qq.com, All Rights Reserved. 
 ********************************************************************************/
#include "truetype.h"

typedef struct
{
    Fixed       version;
    Fixed       font_revision;
    uint32_t    check_sum_adjustment;
    uint32_t    magic_number;           //0x5f0f3cf5
    uint16_t    flags;
    uint16_t    units_per_em;
    longDataTime   created;
    longDataTime   modified;
    FWord       x_min;
    FWord       y_min;
    FWord       x_max;
    FWord       y_max;
    uint16_t    mac_style;
    uint16_t    lowest_rec_ppem;
    int16_t     font_dir_hint;
    int16_t     loca_fmt;
    int16_t     glyph_fmt;
}head_table_t;

bool ttf_table_head_parse(truetype_font_t* ttf)
{
    head_table_t head;
    file_t* file = ttf->file;

    //read head
    ttf_seek(file,ttf->table_head.offset);
    ttf_read32(file,&head.version,4);
    float ver = *(uint32_t*)&head.version/65535.0f;
    debug_print("version : V%.4f\n",ver);
    ver = *(uint32_t*)&head.font_revision/65535.0f;
    debug_print("font_revision : V%.4f\n",ver);
    if(head.magic_number != 0x5f0f3cf5){
        debug_print("magic : %x\n",head.magic_number);
        debug_print("magic number error\n",head.magic_number);
        return false;
    }
    ttf_read16(file,&head.flags,2);
    ttf_read64(file,&head.created,2);
    ttf_read16(file,&head.x_min,11);
    ttf->loca_fmt = head.loca_fmt;
    ttf->units_per_em = head.units_per_em;
    ttf->x_max = head.x_max;
    ttf->x_min = head.x_min;
    ttf->y_max = head.y_max;
    ttf->y_min = head.y_min;
    return true;
}