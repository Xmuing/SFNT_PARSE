/********************************************************************************
 * @Author: xu 1424839890@qq.com
 * @Date: 2024-06-20 15:01:00
 * @LastEditors: xu 1424839890@qq.com
 * @LastEditTime: 2024-06-28 15:16:55
 * @FilePath: \ttf_engine\truetype.h
 * @Description: 
 * @
 * @Copyright (c) 2024 by xu 1424839890@qq.com, All Rights Reserved. 
 ********************************************************************************/
#ifndef _TTF_LOAD_H_
#define _TTF_LOAD_H_

#include "ttf_fs_driver.h"
#include "ttf_type.h"

#include "malloc.h"
#include "string.h"
#include "ttf_debug.h"

typedef struct{
	char name[20];
	file_t* file;
	sfnt_info_t snft_head;
	sfnt_index_t table_cmap;
	sfnt_index_t table_glyf;
	sfnt_index_t table_head;
	// sfnt_index_t table_hhea;
	// sfnt_index_t table_hmtx;
	sfnt_index_t table_loca;
	sfnt_index_t table_maxp;
	// sfnt_index_t table_name;
	// sfnt_index_t table_post;

	uint16_t num_Glyphs;
	uint16_t max_Points;
	uint16_t max_Contours;

	//head info
	int16_t     loca_fmt;
	uint16_t    units_per_em;
	FWord	x_min;
    FWord	y_min;
    FWord	x_max;
    FWord	y_max;

	//cmap info
	uint16_t cmap_fmt;
	//fmt4
	uint16_t segcount;
	uint16_t *end_code;				//end_code[segcount];
    uint16_t *start_code;			//start_code[segcount];
    int16_t  *id_delta;				//id_delta[segcount];
    uint16_t *id_range_offset;		//id_range_offset[segcount];
	size_t	id_range_offset_addr;
    // uint16_t *glyph_index;			//glyph_index[variable];

	//glyphs info
	// uint32_t* Glyphs_index;
	uint16_t* Points_x;
	uint16_t* Points_y;
	uint16_t* Points_end;
	uint8_t* flags;

}truetype_font_t;

typedef struct 
{
	uint32_t font_color;
	uint32_t bg_color;
	size_t font_size;
	size_t font_space;
	size_t line_space;
}font_style_t;

truetype_font_t* ttf_create(const char* name);
void ttf_destory(truetype_font_t* ttf);
bool ttf_load_table(truetype_font_t* ttf,const char* tag,sfnt_index_t* table);
bool ttf_table_head_parse(truetype_font_t* ttf);
bool ttf_table_maxp_parse(truetype_font_t* ttf);
bool ttf_table_loca_parse(truetype_font_t* ttf);
bool ttf_table_cmap_parse(truetype_font_t* ttf);
size_t ttf_get_id(truetype_font_t* ttf,size_t id);
size_t ttf_table_glyf_parse(truetype_font_t* ttf,size_t id,size_t x,size_t y,font_style_t style);
void ttf_print_utf8(truetype_font_t* ttf,size_t x0,size_t y0,const char* str,font_style_t style);

#endif
