#ifndef _TTF_TYPE_H_
#define _TTF_TYPE_H_

#include "stdint.h"
#include "stdbool.h"
#include "sfnt_type.h"

typedef struct{
	uint32_t	version;/* 缩放器类型，Windows必须为0x00010000，其它允许的值有'true' (0x74727565)，'typ1' (0x74797031)，'OTTO' (0x4F54544F)，其它值不支持 */
	uint16_t	tables_num;
	uint16_t	search_range;/* (maximum power of 2 <= numTables)*16 */
	uint16_t	entry_selector;/* log2(maximum power of 2 <= numTables) */
	uint16_t	range_shift;/* numTables*16-searchRange */
}sfnt_info_t;

typedef struct{
	char tag[4];
	uint32_t check_sum;
	uint32_t offset;
	uint32_t length;
}sfnt_index_t;

#endif
