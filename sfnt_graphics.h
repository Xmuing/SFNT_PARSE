/********************************************************************************
 * @Author: xu 1424839890@qq.com
 * @Date: 2024-06-25 13:59:30
 * @LastEditors: xu 1424839890@qq.com
 * @LastEditTime: 2024-06-25 14:12:00
 * @FilePath: \ttf_engine\sfnt_graphics.h
 * @Description: 
 * @
 * @Copyright (c) 2024 by xu 1424839890@qq.com, All Rights Reserved. 
 ********************************************************************************/
#ifndef _SNFT_GRAPHICS_H_
#define _SNFT_GRAPHICS_H_

#include <SDL2/SDL.h>
extern SDL_Window      *window;
extern SDL_Surface     *surface;
extern SDL_Renderer    *renderer;

void draw_line(int x0,int y0,int x1,int y1,int color);
void draw_sec_bezier(int x0,int y0,int x1,int y1,int x2,int y2,int color,int step);

#endif
