/********************************************************************************
 * @Author: xu 1424839890@qq.com
 * @Date: 2024-06-21 09:15:29
 * @LastEditors: xu 1424839890@qq.com
 * @LastEditTime: 2024-06-21 09:33:00
 * @FilePath: \print_ttf\ttf_debug.h
 * @Description: 
 * @
 * @Copyright (c) 2024 by xu 1424839890@qq.com, All Rights Reserved. 
 ********************************************************************************/
#ifndef _TTF_DEBUG_H_
#define _TTF_DEBUG_H_

#define TTF_DEBUG     

#ifdef TTF_DEBUG
#define debug_print(...) printf(__VA_ARGS__)
#else 
#define debug_print(...) ;
#endif
#endif