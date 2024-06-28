/********************************************************************************
 * @Author: xu 1424839890@qq.com
 * @Date: 2024-06-20 15:00:48
 * @LastEditors: xu 1424839890@qq.com
 * @LastEditTime: 2024-06-27 13:58:31
 * @FilePath: \ttf_engine\truetype.c
 * @Description: 
 * @
 * @Copyright (c) 2024 by xu 1424839890@qq.com, All Rights Reserved. 
 ********************************************************************************/
#include "truetype.h"

static bool snft_check(sfnt_info_t snft);

truetype_font_t* ttf_create(const char* name)
{
    truetype_font_t* new_font = (truetype_font_t*)malloc(sizeof(truetype_font_t));

    file_t *ttf_file;
    ttf_open(&ttf_file,name);
    if(ttf_file == NULL){
        debug_print("Can't open file:%s",name);
        return NULL;
    }
    new_font->file = ttf_file;

    ttf_read32(ttf_file,&new_font->snft_head.version,1);
    ttf_read16(ttf_file,&new_font->snft_head.tables_num,4);

    if(snft_check(new_font->snft_head) == false)
    {
        ttf_destory(new_font);
        ttf_close(ttf_file);
        return NULL;
    }

    if(ttf_load_table(new_font,"maxp",&new_font->table_maxp)==false) goto faile_to_create;
    if(ttf_load_table(new_font,"head",&new_font->table_head)==false) goto faile_to_create;
    if(ttf_load_table(new_font,"loca",&new_font->table_loca)==false) goto faile_to_create;
    if(ttf_load_table(new_font,"cmap",&new_font->table_cmap)==false) goto faile_to_create;
    if(ttf_load_table(new_font,"glyf",&new_font->table_glyf)==false) goto faile_to_create;

    if(ttf_table_head_parse(new_font)==false) goto faile_to_create;
    if(ttf_table_maxp_parse(new_font)==false) goto faile_to_create;
    if(ttf_table_loca_parse(new_font)==false) goto faile_to_create;
    if(ttf_table_cmap_parse(new_font)==false) goto faile_to_create;
    
    // font_style_t style;
    // style.font_color = 0xffffff;
    // style.font_size = 24;
    // for(int i=0;i<1037;i++){
    //     ttf_table_glyf_parse(new_font,i,0,240,style);
    // }
    return new_font;

faile_to_create:
    ttf_destory(new_font);
    return NULL;
    
}

void ttf_destory(truetype_font_t* ttf)
{
    if(ttf == NULL) return;
    //cmap
    if(ttf->end_code != NULL) free(ttf->end_code);
    if(ttf->start_code != NULL) free(ttf->start_code);
    if(ttf->id_delta != NULL) free(ttf->id_delta);
    if(ttf->id_range_offset != NULL) free(ttf->id_range_offset);
    // if(ttf->glyph_index != NULL) free(ttf->glyph_index);

    // glyf
    if(ttf->Points_x != NULL) free(ttf->Points_x);
    if(ttf->Points_y != NULL) free(ttf->Points_y);
    if(ttf->Points_end != NULL) free(ttf->Points_end);
    // if(ttf->Glyphs_index != NULL) free(ttf->Glyphs_index);
    ttf_close(ttf->file);
    free(ttf);
}

bool ttf_load_table(truetype_font_t* ttf,const char* tag,sfnt_index_t* table)
{
    uint32_t table_entry_addr = sizeof(ttf->snft_head);
    file_t* file = ttf->file;
    //跳转至table_entry起始位置
    ttf_seek(file,table_entry_addr);
    for(int i=0; i<ttf->snft_head.tables_num;i++)
    {
        ttf_read(file,table,sizeof(sfnt_index_t));
        if(memcmp(table->tag,tag,4) == 0)
        {
            table->check_sum = SWAP32(table->check_sum);
            table->offset = SWAP32(table->offset);
            table->length = SWAP32(table->length);
            debug_print("Table : %.4s\n",table->tag);
            debug_print("Check_Sum :0x%x\n", table->check_sum);
            debug_print("Offset : 0x%x\n", table->offset);
            debug_print("Length : 0x%x\n", table->length);
            return true;
        }
        table_entry_addr += sizeof(sfnt_index_t);
        ttf_seek(file,table_entry_addr);
    }
    if(strcmp(table->tag,tag) == 0) return true;
    else memset(table,0,sizeof(sfnt_index_t));
    return false;
}

void ttf_print()
{}

static bool snft_check(sfnt_info_t snft)
{
    switch(snft.version){
        case 0x00010000:
            debug_print("scaler type :WINDOWS\n");
        break;
        case 0x74727565:
            debug_print("scaler type :IOS\n");
        break;
        default:
            debug_print("scaler type :unkonw\n");
        return false;
    }

    return true;
}