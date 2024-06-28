/********************************************************************************
 * @Author: xu 1424839890@qq.com
 * @Date: 2024-06-21 11:11:08
 * @LastEditors: xu 1424839890@qq.com
 * @LastEditTime: 2024-06-25 08:44:49
 * @FilePath: \ttf_engine\sfnt_type.h
 * @Description: 
 * @
 * @Copyright (c) 2024 by xu 1424839890@qq.com, All Rights Reserved. 
 ********************************************************************************/
#ifndef _SFNT_TYPE_H_
#define _SFNT_TYPE_H_
#include "stdint.h"

typedef uint8_t uint8;
typedef uint8_t int8;
typedef uint16_t uint16;
typedef uint16_t int16;
typedef struct{uint8 arg[3];}uint24;
typedef uint32_t uint32;
typedef uint32_t int32;

typedef uint16_t shortFrac;     //16 位有符号分数
typedef struct{uint16_t integer; uint16_t decimal;}Fixed;//16.16位有符号定点数
typedef int16_t FWord;          //16 位有符号整数，用于描述 FUnits 中的量，即 em 空间中最小的可测量距离。
typedef uint16_t uFWord;        //16 位无符号整数，用于描述 FUnits 中的量，即 em 空间中最小的可测量距离。
typedef int16_t F2Dot14;        //16 位有符号固定数，低位 14 位代表分数。
typedef int64_t longDataTime;   //自 1904 年 1 月 1 日午夜 12：00 以来日期的长内部格式（以秒为单位）。它表示为有符号的 64 位整数。
typedef struct{uint16_t major; uint16_t minor;}Version16Dot16;//16.16位有符号定点数

typedef uint8 Offset8;
typedef uint16 Offset16;
typedef uint24 Offset24;
typedef uint32 Offset32;

#endif
