/********************************************************************************
 * @Author: xu 1424839890@qq.com
 * @Date: 2024-06-20 09:02:21
 * @LastEditors: xu 1424839890@qq.com
 * @LastEditTime: 2024-06-26 15:55:43
 * @FilePath: \ttf_engine\draw_bezier.c
 * @Description: 
 * @
 * @Copyright (c) 2024 by xu 1424839890@qq.com, All Rights Reserved. 
 ********************************************************************************/
#include "sfnt_graphics.h"

SDL_Window      *window;
SDL_Surface     *surface;
SDL_Renderer    *renderer;

void draw_line(int x0,int y0,int x1,int y1,int color)
{
    SDL_Point points[2];

    points[0].x = x0;
    points[0].y = 600+32-y0;
    points[1].x = x1;
    points[1].y = 600+32-y1;

	SDL_SetRenderDrawColor(renderer, color&0X00FF0000>>16, color&0X0000FF00>>8, color&0X000000FF, 255);
    // SDL_RenderDrawLine(renderer,points[0].x,points[0].y,points[1].x,points[1].y);
	SDL_RenderDrawLines(renderer, points, 2);
    SDL_RenderPresent(renderer);
}

void draw_sec_bezier(int x0,int y0,int x1,int y1,int x2,int y2,int color,int step)
{
    float x=x0,y=y0;
    float x_cur,y_cur;
    float k_x1,k_y1;
    float k_x2,k_x3,k_y2,k_y3;
    float k_step = 1.0f/step;

    k_x1 = k_step*k_step*(x0-x1*2+x2);
    k_x2 = k_step*(x1-x0)*2 + k_x1;
    k_x3 = k_x1+k_x1;
    k_y1 = k_step*k_step*(y0-y1*2+y2);
    k_y2 = k_step*(y1-y0)*2 + k_y1;
    k_y3 = k_y1+k_y1;

    float kx_step=0;
    float ky_step=0;
    for(int step = 1;step<step;step++)
    {
        x_cur = x+k_x2+kx_step;
        y_cur = y+k_y2+ky_step;
        kx_step += k_x3;
        ky_step += k_y3;
        draw_line(x,y,x_cur,y_cur,color);
        x = x_cur;
        y = y_cur;
    }
    draw_line(x,y,x2,y2,color);
}