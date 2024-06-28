/********************************************************************************
 * @Author: xu 1424839890@qq.com
 * @Date: 2024-06-21 08:39:02
 * @LastEditors: xu 1424839890@qq.com
 * @LastEditTime: 2024-06-28 15:24:55
 * @FilePath: \ttf_engine\main.c
 * @Description: 
 * @
 * @Copyright (c) 2024 by xu 1424839890@qq.com, All Rights Reserved. 
 ********************************************************************************/
#include "stdio.h"
#include "truetype.h"
#include <SDL2/SDL.h>

#include "sfnt_graphics.h"

int main(int argc,char* argv[])
{
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("字体测试", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 600, SDL_WINDOW_SHOWN);
    if (!window)
        return -1;
    // surface = SDL_GetWindowSurface(window);
    // if (!surface){
    //     SDL_DestroyWindow(window);
    //     return -2;
    // }
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer){
        SDL_FreeSurface(surface);
        SDL_DestroyWindow(window);
        return -3;
    }

    truetype_font_t* roboto;
    font_style_t style = {.font_size = 32,.font_color=0xffffff,.bg_color=0x000000,.font_space = 4};
    roboto = ttf_create("D:\\Desktop\\ttf\\ttf_engine\\test1.ttf");
    ttf_print_utf8(roboto,0,600,"The quick brown fox jumps over the lazy dog",style);
    ttf_print_utf8(roboto,0,600-32,"敏捷的棕色狐狸跳过了懒惰的狗",style);

    SDL_RenderPresent(renderer);

    ttf_destory(roboto);

//destory renderer
    SDL_DestroyRenderer(renderer);
	// SDL_FreeSurface(surface);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}