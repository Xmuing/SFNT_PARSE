/********************************************************************************
 * @Author: xu 1424839890@qq.com
 * @Date: 2024-06-21 11:06:37
 * @LastEditors: xu 1424839890@qq.com
 * @LastEditTime: 2024-06-28 14:38:05
 * @FilePath: \ttf_engine\ttf_table_glyf.c
 * @Description: 
 * @
 * @Copyright (c) 2024 by xu 1424839890@qq.com, All Rights Reserved. 
 ********************************************************************************/
#include "truetype.h"
#include "sfnt_graphics.h"

typedef struct 
{
    int16_t     contours_num;//正数:简单图元,负数:复合图元
    FWord       x_min;
    FWord       y_min;
    FWord       x_max;
    FWord       y_max;
}glyph_head_t;

typedef struct 
{
    uint16_t    *end_points;//[contours_num]
    uint16_t    instruction_length;
    uint8_t     *instructions;//[instruction_length]
    uint8_t     *flags;
    void        *x_coords;//uint8_t or int16
    void        *y_coords;//uint8_t or int16
}simple_glyph_def;

#define FLAG_On_Curve   0X01
#define FLAG_X_SHORT    0X02
#define FLAG_Y_SHORT    0X04
#define FLAG_REPEAT     0X08
#define FLAG_X_SIGN     0X10
#define FLAG_Y_SIGN     0X20
#define FLAG_X          0x12
#define FLAG_Y          0x24

typedef struct 
{
    uint16_t    flag;
    uint16_t    index;
    uint16_t    x_crood;//int16、uint16、int8 或 uint8
    uint16_t    y_crood;//int16、uint16、int8 或 uint8
    F2Dot14     scale_x;
    F2Dot14     scale_y;
    F2Dot14     scale_1;
    F2Dot14     scale_2;
}complex_glyph_def;

#define FLAG_WORD               0X0001
#define FLAG_SIGNED             0X0002
#define FLAG_GIRD               0X0004
#define FLAG_SCALE              0x0008
#define FLAG_CONTINUE           0X0020
#define FLAG_SCALE2             0X0040
#define FLAG_SCALE4             0X0080
#define FLAG_INSTRUCT           0X0100
#define FLAG_METRICS            0X0200
#define FLAG_COMPOUND           0X0400
#define FLAG_SCALED_OFFSET      0X0800
#define FLAG_UNSCALED_OFFSET    0X1000

typedef struct{
    uint16_t    x_crood;//int16、uint16、int8 或 uint8
    uint16_t    y_crood;//int16、uint16、int8 或 uint8
    float       scale_x;
    float       scale_y;
    float       scale_1;
    float       scale_2;
}glyph_from_t;

static void glyf_sample_parse(truetype_font_t* ttf,size_t id);
static size_t glyf_complex_parse(truetype_font_t* ttf,size_t id,size_t x,size_t y,font_style_t style);
static void glyf_draw_simple(truetype_font_t* ttf,int16_t contours_num,size_t x0,size_t y0 ,font_style_t style);
static size_t glyf_draw_complex(truetype_font_t* ttf,complex_glyph_def complex_glyph,size_t x0,size_t y0 ,font_style_t style);

size_t ttf_table_glyf_parse(truetype_font_t* ttf,size_t id,size_t x,size_t y,font_style_t style)
{
    glyph_head_t glyph_head;
    file_t* file = ttf->file;

    F2Dot14 scale = (style.font_size<<14)/ttf->units_per_em;
    if(ttf->loca_fmt){
        uint32_t index[2];
        ttf_seek(ttf->file,ttf->table_loca.offset+4*id);
        ttf_read32(ttf->file,index,2);
        if(index[0]==index[1]) return (style.font_size>>2)+style.font_space;
        ttf_seek(file,ttf->table_glyf.offset+index[0]);
    }else{
        uint16_t index[2];
        ttf_seek(ttf->file,ttf->table_loca.offset+2*id);
        ttf_read16(ttf->file,index,2);
        if(index[0]==index[1]) return (style.font_size>>2)+style.font_space;
        ttf_seek(file,ttf->table_glyf.offset+2*index[0]);
    }

    // SDL_Delay(300);
    // SDL_Rect rect;
    // rect.x = 0;
    // rect.y = 0;
    // rect.w = 800;
    // rect.h = 600;
	// SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    // SDL_RenderFillRect(renderer,&rect);

    ttf_read16(file,&glyph_head,5);
    if(glyph_head.contours_num>0){
        glyf_sample_parse(ttf,id);
        glyf_draw_simple(ttf,glyph_head.contours_num,x,y,style);
        return (((glyph_head.x_max*scale)>>14)+style.font_space);
    }else if(glyph_head.contours_num<0){
        return glyf_complex_parse(ttf,id,x,y,style);  
    }
    return 0;
}



static void glyf_sample_parse(truetype_font_t* ttf,size_t id)
{
    file_t* file = ttf->file;
    glyph_head_t glyph_head;
    simple_glyph_def simple_glyph;

    if(ttf->loca_fmt){
        uint32_t index[2];
        ttf_seek(ttf->file,ttf->table_loca.offset+4*id);
        ttf_read32(ttf->file,index,2);
        if(index[0]==index[1]) return;
        ttf_seek(file,ttf->table_glyf.offset+index[0]);
    }else{
        uint16_t index[2];
        ttf_seek(ttf->file,ttf->table_loca.offset+2*id);
        ttf_read16(ttf->file,index,2);
        if(index[0]==index[1]) return;
        ttf_seek(file,ttf->table_glyf.offset+2*index[0]);
    }
    ttf_read16(file,&glyph_head,5);
    
    simple_glyph.end_points = ttf->Points_end;
    ttf_read16(file,simple_glyph.end_points,glyph_head.contours_num);
    ttf_read16(file,&simple_glyph.instruction_length,1);
    if(simple_glyph.instruction_length){
        //skip instruction
        size_t temp = ttf_tell(file);
        temp += simple_glyph.instruction_length;
        ttf_seek(file,temp);
    }

    uint16_t point_num = simple_glyph.end_points[glyph_head.contours_num-1]+1;
    simple_glyph.flags = ttf->flags;

    uint16_t cnt_x = 0,cnt_y = 0;
    uint16_t unit_x = 0,unit_y = 0;
    uint8_t* flag_ptr = simple_glyph.flags;
    for(uint16_t i=0;i<point_num;i++)
    {
        ttf_read(file,flag_ptr,1);
        switch(*flag_ptr&FLAG_X)
        {
            case 0:
                unit_x = 2;
            break;
            case FLAG_X_SHORT:
                unit_x = 1;
            break;
            case FLAG_X_SIGN:
                unit_x = 0;
            break;
            case FLAG_X:
                unit_x = 1;
            break;
        }

        switch(*flag_ptr&FLAG_Y)
        {
            case 0:
                unit_y = 2;
            break;
            case FLAG_Y_SHORT:
                unit_y = 1;
            break;
            case FLAG_Y_SIGN:
                unit_y = 0;
            break;
            case FLAG_Y:
                unit_y = 1;
            break;
        }

        cnt_x += unit_x;
        cnt_y += unit_y;

        if(*flag_ptr&FLAG_REPEAT){
            flag_ptr ++;
            ttf_read(file,flag_ptr,1);
            i += (*flag_ptr);
            cnt_x += unit_x*(*flag_ptr);
            cnt_y += unit_y*(*flag_ptr);
        }
        flag_ptr ++;
    }

    simple_glyph.x_coords = ttf->Points_x;
    ttf_read(file,simple_glyph.x_coords,cnt_x);

    simple_glyph.y_coords = ttf->Points_y;
    ttf_read(file,simple_glyph.y_coords,cnt_y);


}

// static void glyf_complex_parse(truetype_font_t* ttf,size_t id,size_t x,size_t y,size_t font_size)
static size_t glyf_complex_parse(truetype_font_t* ttf,size_t id,size_t x,size_t y,font_style_t style)
{
    complex_glyph_def complex_glyph;
    glyph_head_t glyph_head;
    file_t* file = ttf->file;
    size_t addr;
    size_t len,res=0;
    F2Dot14 scale = (style.font_size<<14)/ttf->units_per_em;

    if(ttf->loca_fmt){
        uint32_t index[2];
        ttf_seek(ttf->file,ttf->table_loca.offset+4*id);
        ttf_read32(ttf->file,index,2);
        if(index[0]==index[1]) return (style.font_size>>2)+style.font_space;
        ttf_seek(file,ttf->table_glyf.offset+index[0]);
    }else{
        uint16_t index[2];
        ttf_seek(ttf->file,ttf->table_loca.offset+2*id);
        ttf_read16(ttf->file,index,2);
        if(index[0]==index[1]) return (style.font_size>>2)+style.font_space;
        ttf_seek(file,ttf->table_glyf.offset+2*index[0]);
    }
    
    ttf_read16(file,&glyph_head,5);

    do{
        ttf_read16(file,&complex_glyph,2);
        if(complex_glyph.flag & FLAG_WORD){
            ttf_read16(file,&complex_glyph.x_crood,1);
            ttf_read16(file,&complex_glyph.y_crood,1);
        }else{
            ttf_read(file,&complex_glyph.x_crood,1);
            ttf_read(file,&complex_glyph.y_crood,1);
            complex_glyph.x_crood = (int8_t)complex_glyph.x_crood;
            complex_glyph.y_crood = (int8_t)complex_glyph.y_crood;
        }

        if(complex_glyph.flag & FLAG_SCALE){
            ttf_read16(file,&complex_glyph.scale_x,1);
            complex_glyph.scale_y = complex_glyph.scale_x;
            complex_glyph.scale_1 = complex_glyph.scale_2 = 0;
        }else if(complex_glyph.flag & FLAG_SCALE2){
            ttf_read16(file,&complex_glyph.scale_x,1);
            ttf_read16(file,&complex_glyph.scale_y,1);
            complex_glyph.scale_1 = complex_glyph.scale_2 = 0;
        }else if(complex_glyph.flag & FLAG_SCALE4){
            ttf_read16(file,&complex_glyph.scale_x,1);
            ttf_read16(file,&complex_glyph.scale_1,1);
            ttf_read16(file,&complex_glyph.scale_2,1);
            ttf_read16(file,&complex_glyph.scale_y,1);
        }else{
            complex_glyph.scale_y = complex_glyph.scale_x = 16384;
            complex_glyph.scale_1 = complex_glyph.scale_2 = 0;
        }

        addr = ttf_tell(file);
        if(complex_glyph.flag & FLAG_SIGNED){
            len = glyf_draw_complex(ttf,complex_glyph,x,y,style);
        }else{
            debug_print("error!\n");
        }
        ttf_seek(file,addr);
        if(len > res) res = len;
    }while(complex_glyph.flag & FLAG_CONTINUE);
    // size_t temp;
    // if (complex_glyph.flag & FLAG_INSTRUCT)
    // {
    //     uint16_t length;
    //     ttf_read16(file,&length,1);
    //     temp = ttf_tell(file);
    //     temp += length;
    //     ttf_seek(file,temp);
    // }
    return res;
}

// 01B ==> OFF
// 10B ==> ON
// 11B ==> MID
// XX|XX|XX|XX
#define ON              0X80
#define ON_ON           0XA0    //--> ON
#define ON_OFF          0X90
#define ON_OFF_ON       0X98       //--> ON
#define ON_OFF_OFF      0X94       //--> MID_OFF
#define ON_MID_OFF      0XB4

#define OFF             0X40
#define OFF_OFF         0X50 
#define OFF_OFF_OFF     0X54       //--> OFF_MID_OFF
#define OFF_OFF_ON      0X58       //--> OFF_ON
#define OFF_ON          0X60
#define OFF_ON_ON       0X68       //--> OFF_ON
#define OFF_ON_OFF      0X64 
#define OFF_ON_OFF_OFF  0x65   //--> OFF_MID_OFF
#define OFF_ON_OFF_ON   0x66   //--> OFF_ON
#define OFF_MID_OFF     0x74

static void glyf_draw_simple(truetype_font_t* ttf,int16_t contours_num,size_t x0,size_t y0 ,font_style_t style)
{
    uint16_t *end_points = ttf->Points_end;
    uint8_t* x_croods = (uint8_t*)ttf->Points_x;
    uint8_t* y_croods = (uint8_t*)ttf->Points_y;
    uint8_t* flag = ttf->flags;
    int16_t x[5]={0,0,0,0,0};
    int16_t y[5]={0,0,0,0,0};
    uint8_t step = 0;
    uint8_t state = 0;
    F2Dot14 scale = (style.font_size<<14)/ttf->units_per_em;

    uint8_t flag_bak;
    int circle = 0;
    for(int i=0; i<contours_num; i++)
    {
        step = 0;
        state = 0;
        for(int j=(i>0)?end_points[i-1]+1:0;j<end_points[i]+1;j++)
        {
            if(circle<=0){
                flag_bak = *flag;
                if(*flag&FLAG_REPEAT){
                    flag ++;
                    circle = *flag +1;
                }else{
                    circle = 1;
                }
            }
            if(circle){
                switch(flag_bak&FLAG_X)
                {
                    case 0:
                        x[step] += (int16_t)SWAP16(*(uint16_t*)x_croods);
                        x_croods += 2;
                    break;
                    case FLAG_X_SHORT:
                        x[step] -= ((uint8_t)*x_croods);
                        x_croods ++;
                    break;
                    case FLAG_X_SIGN:
                        // x[step] = x[step-1];
                    break;
                    case FLAG_X:
                        x[step] += ((uint8_t)*x_croods);
                        x_croods ++;
                    break;
                }

                switch(flag_bak&FLAG_Y)
                {
                    case 0:
                        y[step] += (int16_t)SWAP16(*(uint16_t*)y_croods);
                        y_croods += 2;
                    break;
                    case FLAG_Y_SHORT:
                        y[step] -= ((uint8_t)*y_croods);
                        y_croods ++;
                    break;
                    case FLAG_Y_SIGN:
                        // y[step] = y[step-1];
                    break;
                    case FLAG_Y:
                        y[step] += ((uint8_t)*y_croods);
                        y_croods ++;
                    break;
                }

                switch(state){
                    case ON:  
                        if(flag_bak&FLAG_On_Curve){
                            state = ON;
                            step = 1;
                            draw_line(  x0+((x[0]*scale)>>14),
                                        y0+((y[0]*scale)>>14),
                                        x0+((x[1]*scale)>>14),
                                        y0+((y[1]*scale)>>14),
                                        style.font_color);
                            x[0] = x[1];
                            y[0] = y[1];
                        }
                        else{
                            state = ON_OFF;
                            step = 2;
                            x[2] = x[1];
                            y[2] = y[1];
                        }
                        break;
                    // case ON_ON:
                    case ON_MID_OFF:    
                    case ON_OFF:
                        if(flag_bak&FLAG_On_Curve){
                            state = ON;
                            step = 1;
                            draw_sec_bezier(    x0+((x[0]*scale)>>14),
                                                y0+((y[0]*scale)>>14),
                                                x0+((x[1]*scale)>>14),
                                                y0+((y[1]*scale)>>14),
                                                x0+((x[2]*scale)>>14),
                                                y0+((y[2]*scale)>>14),
                                                style.font_color,
                                                style.font_size);
                            x[0] = x[2];
                            y[0] = y[2];
                            x[1] = x[2];
                            y[1] = y[2];
                        }
                        else{
                            state = ON_MID_OFF;
                            step = 2;
                            draw_sec_bezier(    x0+((x[0]*scale)>>14),
                                                y0+((y[0]*scale)>>14),
                                                x0+((x[1]*scale)>>14),
                                                y0+((y[1]*scale)>>14),
                                                x0+(((x[1]+x[2])*scale)>>15),
                                                y0+(((y[1]+y[2])*scale)>>15),
                                                style.font_color,
                                                style.font_size);
                            x[0] = (x[1]+x[2])/2;
                            y[0] = (y[1]+y[2])/2;
                            x[1] = x[2];
                            y[1] = y[2];
                        }
                        break;
                    // case ON_OFF_ON:
                    // case ON_OFF_OFF:
                    case OFF:
                        if(flag_bak&FLAG_On_Curve){
                            state = OFF_ON;
                            x[0] = x[1];
                            y[0] = y[1];
                        }
                        else{
                            state = OFF_OFF;
                        }
                        step = 2;
                        x[2] = x[1];
                        y[2] = y[1];
                        break;
                    case OFF_OFF:
                        if(flag_bak&FLAG_On_Curve){
                            state = OFF_ON;
                            step = 2;
                            draw_sec_bezier(    x0+(((x[0]+x[1])*scale)>>15),
                                                y0+(((y[0]+y[1])*scale)>>15),
                                                x0+((x[1]*scale)>>14),
                                                y0+((y[1]*scale)>>14),
                                                x0+((x[2]*scale)>>14),
                                                y0+((y[2]*scale)>>14),
                                                style.font_color,
                                                style.font_size);
                            x[0] = (x[0]+x[1])/2;
                            y[0] = (y[0]+y[1])/2;
                            x[1] = x[2];
                            y[1] = y[2];
                        }
                        else{
                            state = OFF_MID_OFF;
                            step = 3;
                            draw_sec_bezier(    x0+(((x[0]+x[1])*scale)>>15),
                                                y0+(((y[0]+y[1])*scale)>>15),
                                                x0+((x[1]*scale)>>14),
                                                y0+((y[1]*scale)>>14),
                                                x0+(((x[1]+x[2])*scale)>>15),
                                                y0+(((y[1]+y[2])*scale)>>15),
                                                style.font_color,
                                                style.font_size);
                            x[0] = (x[0]+x[1])/2;
                            y[0] = (y[0]+y[1])/2;
                            x[1] = (x[1]+x[2])/2;
                            y[1] = (y[1]+y[2])/2;
                        }
                        break;
                    // case OFF_OFF_OFF:
                    // case OFF_OFF_ON:
                    case OFF_ON:
                        if(flag_bak&FLAG_On_Curve){
                            state = OFF_ON;
                            step = 2;
                            draw_line(  x0+((x[1]*scale)>>14),
                                        y0+((y[1]*scale)>>14),
                                        x0+((x[2]*scale)>>14),
                                        y0+((y[2]*scale)>>14),
                                        style.font_color);
                            x[1] = x[2];
                            y[1] = y[2];
                        }
                        else{
                            state = OFF_ON_OFF;
                            step = 3;
                            x[3] = x[2];
                            y[3] = y[2];
                        }
                        break;
                    // case OFF_ON_ON:
                    case OFF_MID_OFF:
                    case OFF_ON_OFF:
                        if(flag_bak&FLAG_On_Curve){
                            state = OFF_ON;
                            step = 2;
                            draw_sec_bezier(    x0+((x[1]*scale)>>14),
                                                y0+((y[1]*scale)>>14),
                                                x0+((x[2]*scale)>>14),
                                                y0+((y[2]*scale)>>14),
                                                x0+((x[3]*scale)>>14),
                                                y0+((y[3]*scale)>>14),
                                                style.font_color,
                                                style.font_size);
                            x[1] = x[3];
                            y[1] = y[3];
                            x[2] = x[3];
                            y[2] = y[3];
                        }else{
                            state = OFF_MID_OFF;
                            step = 3;
                            draw_sec_bezier(    x0+((x[1]*scale)>>14),
                                                y0+((y[1]*scale)>>14),
                                                x0+((x[2]*scale)>>14),
                                                y0+((y[2]*scale)>>14),
                                                x0+(((x[2]+x[3])*scale)>>15),
                                                y0+(((y[2]+y[3])*scale)>>15),
                                                style.font_color,
                                                style.font_size);
                            x[1] = (x[2]+x[3])/2;
                            y[1] = (y[2]+y[3])/2;
                            x[2] = x[3];
                            y[2] = y[3];
                        }
                        break;
                    // case OFF_ON_OFF_OFF:
                    // case OFF_ON_OFF_ON:
                    default:
                        step = 1;
                        x[1] = x[0];
                        y[1] = y[0];
                        //end
                        x[4] = x[0];
                        y[4] = y[0];
                        if(flag_bak&FLAG_On_Curve) state = ON;
                        else state = OFF;
                        break;
                }
                circle--;
            }
            if(circle == 0) flag ++;
        }
        switch (state)
        {
            case ON:
                draw_line(  x0+((x[0]*scale)>>14),
                            y0+((y[0]*scale)>>14),
                            x0+((x[4]*scale)>>14),
                            y0+((y[4]*scale)>>14),
                            style.font_color);
            break;
            case ON_MID_OFF:    
            case ON_OFF:
                draw_sec_bezier(    x0+((x[0]*scale)>>14),
                                    y0+((y[0]*scale)>>14),
                                    x0+((x[1]*scale)>>14),
                                    y0+((y[1]*scale)>>14),
                                    x0+((x[3]*scale)>>14),
                                    y0+((y[3]*scale)>>14),
                                    style.font_color,
                                    style.font_size);
                x[0] = x[1];
                y[0] = y[1];
            break;
            case OFF_ON:
                draw_sec_bezier(    x0+((x[1]*scale)>>14),
                                    y0+((y[1]*scale)>>14),
                                    x0+((x[4]*scale)>>14),
                                    y0+((y[4]*scale)>>14),
                                    x0+((x[0]*scale)>>14),
                                    y0+((y[0]*scale)>>14),
                                    style.font_color,
                                    style.font_size);
                x[0] = x[1];
                y[0] = y[1];
            break;
            case OFF_MID_OFF:
            case OFF_ON_OFF:
                draw_sec_bezier(    x0+((x[1]*scale)>>14),
                                    y0+((y[1]*scale)>>14),
                                    x0+((x[2]*scale)>>14),
                                    y0+((y[2]*scale)>>14),
                                    x0+(((x[2]+x[4])*scale)>>15),
                                    y0+(((y[2]+y[4])*scale)>>15),
                                    style.font_color,
                                    style.font_size);
                draw_sec_bezier(    x0+(((x[2]+x[4])*scale)>>15),
                                    y0+(((y[2]+y[4])*scale)>>15),
                                    x0+((x[4]*scale)>>14),
                                    y0+((y[4]*scale)>>14),
                                    x0+((x[0]*scale)>>14),
                                    y0+((y[0]*scale)>>14),
                                    style.font_color,
                                    style.font_size);
                x[0] = x[2];
                y[0] = y[2];
            break;
        default:
            break;
        }
    }
}

static void glyf_draw_from(truetype_font_t* ttf,int16_t contours_num,size_t x0,size_t y0,glyph_from_t from,font_style_t style)
{
    uint16_t *end_points = ttf->Points_end;
    uint8_t* x_croods = (uint8_t*)ttf->Points_x;
    uint8_t* y_croods = (uint8_t*)ttf->Points_y;
    uint8_t* flag = ttf->flags;
    int16_t x[5]={from.x_crood,from.x_crood,from.x_crood,from.x_crood,from.x_crood};
    int16_t y[5]={from.y_crood,from.y_crood,from.y_crood,from.y_crood,from.y_crood};
    uint8_t step = 0;
    uint8_t state = 0;

    
    uint8_t flag_bak;
    int circle = 0;
    for(int i=0; i<contours_num; i++)
    {
        step = 0;
        state = 0;
        for(int j=(i>0)?end_points[i-1]+1:0;j<end_points[i]+1;j++)
        {
            if(circle<=0){
                flag_bak = *flag;
                if(*flag&FLAG_REPEAT){
                    flag ++;
                    circle = *flag +1;
                }else{
                    circle = 1;
                }
            }
            if(circle){
                switch(flag_bak&FLAG_X)
                {
                    case 0:
                        x[step] += (int16_t)SWAP16(*(uint16_t*)x_croods);
                        x_croods += 2;
                    break;
                    case FLAG_X_SHORT:
                        x[step] -= ((uint8_t)*x_croods);
                        x_croods ++;
                    break;
                    case FLAG_X_SIGN:
                        // x[step] = x[step-1];
                    break;
                    case FLAG_X:
                        x[step] += ((uint8_t)*x_croods);
                        x_croods ++;
                    break;
                }

                switch(flag_bak&FLAG_Y)
                {
                    case 0:
                        y[step] += (int16_t)SWAP16(*(uint16_t*)y_croods);
                        y_croods += 2;
                    break;
                    case FLAG_Y_SHORT:
                        y[step] -= ((uint8_t)*y_croods);
                        y_croods ++;
                    break;
                    case FLAG_Y_SIGN:
                        // y[step] = y[step-1];
                    break;
                    case FLAG_Y:
                        y[step] += ((uint8_t)*y_croods);
                        y_croods ++;
                    break;
                }

                switch(state){
                    case ON:  
                        if(flag_bak&FLAG_On_Curve){
                            state = ON;
                            step = 1;
                            draw_line(  x0+x[0]*from.scale_x+y[0]*from.scale_2,
                                        y0+x[0]*from.scale_1+y[0]*from.scale_y,
                                        x0+x[1]*from.scale_x+y[1]*from.scale_2,
                                        y0+x[1]*from.scale_1+y[1]*from.scale_y,
                                        style.font_color);
                            x[0] = x[1];
                            y[0] = y[1];
                        }
                        else{
                            state = ON_OFF;
                            step = 2;
                            x[2] = x[1];
                            y[2] = y[1];
                        }
                        break;
                    // case ON_ON:
                    case ON_MID_OFF:    
                    case ON_OFF:
                        if(flag_bak&FLAG_On_Curve){
                            state = ON;
                            step = 1;
                            draw_sec_bezier(    x0+x[0]*from.scale_x+y[0]*from.scale_2,
                                                y0+x[0]*from.scale_1+y[0]*from.scale_y,
                                                x0+x[1]*from.scale_x+y[1]*from.scale_2,
                                                y0+x[1]*from.scale_1+y[1]*from.scale_y,
                                                x0+x[2]*from.scale_x+y[2]*from.scale_2,
                                                y0+x[2]*from.scale_1+y[2]*from.scale_y,
                                                style.font_color,
                                                style.font_size);
                            x[0] = x[2];
                            y[0] = y[2];
                            x[1] = x[2];
                            y[1] = y[2];
                        }
                        else{
                            state = ON_MID_OFF;
                            step = 2;
                            draw_sec_bezier(    x0+x[0]*from.scale_x+y[0]*from.scale_2,
                                                y0+x[0]*from.scale_1+y[0]*from.scale_y,
                                                x0+x[1]*from.scale_x+y[1]*from.scale_2,
                                                y0+x[1]*from.scale_1+y[1]*from.scale_y,
                                                x0+(x[1]+x[2])/2*from.scale_x+(y[1]+y[2])/2*from.scale_2,
                                                y0+(x[1]+x[2])/2*from.scale_1+(y[1]+y[2])/2*from.scale_y,
                                                style.font_color,
                                                style.font_size);
                            x[0] = (x[1]+x[2])/2;
                            y[0] = (y[1]+y[2])/2;
                            x[1] = x[2];
                            y[1] = y[2];
                        }
                        break;
                    // case ON_OFF_ON:
                    // case ON_OFF_OFF:
                    case OFF:
                        if(flag_bak&FLAG_On_Curve){
                            state = OFF_ON;
                            x[0] = x[1];
                            y[0] = y[1];
                        }
                        else{
                            state = OFF_OFF;
                        }
                        step = 2;
                        x[2] = x[1];
                        y[2] = y[1];
                        break;
                    case OFF_OFF:
                        if(flag_bak&FLAG_On_Curve){
                            state = OFF_ON;
                            step = 2;
                            draw_sec_bezier(    x0+(x[0]+x[1])/2*from.scale_x+(y[0]+y[1])/2*from.scale_2,
                                                y0+(x[0]+x[1])/2*from.scale_1+(y[0]+y[1])/2*from.scale_y,
                                                x0+x[1]*from.scale_x+y[1]*from.scale_2,
                                                y0+x[1]*from.scale_1+y[1]*from.scale_y,
                                                x0+x[2]*from.scale_x+y[2]*from.scale_2,
                                                y0+x[2]*from.scale_1+y[2]*from.scale_y,
                                                style.font_color,
                                                style.font_size);
                            x[0] = (x[0]+x[1])/2;
                            y[0] = (y[0]+y[1])/2;
                            x[1] = x[2];
                            y[1] = y[2];
                        }
                        else{
                            state = OFF_MID_OFF;
                            step = 3;
                            draw_sec_bezier(    x0+(x[0]+x[1])/2*from.scale_x+(y[0]+y[1])/2*from.scale_2,
                                                y0+(x[0]+x[1])/2*from.scale_1+(y[0]+y[1])/2*from.scale_y,
                                                x0+x[1]*from.scale_x+y[1]*from.scale_2,
                                                y0+x[1]*from.scale_1+y[1]*from.scale_y,
                                                x0+(x[1]+x[2])/2*from.scale_x+(y[1]+y[2])/2*from.scale_2,
                                                y0+(x[1]+x[2])/2*from.scale_1+(y[1]+y[2])/2*from.scale_y,
                                                style.font_color,
                                                style.font_size);
                            x[0] = (x[0]+x[1])/2;
                            y[0] = (y[0]+y[1])/2;
                            x[1] = (x[1]+x[2])/2;
                            y[1] = (y[1]+y[2])/2;
                        }
                        break;
                    // case OFF_OFF_OFF:
                    // case OFF_OFF_ON:
                    case OFF_ON:
                        if(flag_bak&FLAG_On_Curve){
                            state = OFF_ON;
                            step = 2;
                            draw_line(  x0+x[1]*from.scale_x+y[1]*from.scale_2,
                                        y0+x[1]*from.scale_1+y[1]*from.scale_y,
                                        x0+x[2]*from.scale_x+y[2]*from.scale_2,
                                        y0+x[2]*from.scale_1+y[2]*from.scale_y,
                                        style.font_color);
                            x[1] = x[2];
                            y[1] = y[2];
                        }
                        else{
                            state = OFF_ON_OFF;
                            step = 3;
                            x[3] = x[2];
                            y[3] = y[2];
                        }
                        break;
                    // case OFF_ON_ON:
                    case OFF_MID_OFF:
                    case OFF_ON_OFF:
                        if(flag_bak&FLAG_On_Curve){
                            state = OFF_ON;
                            step = 2;
                            draw_sec_bezier(    x0+x[1]*from.scale_x+y[1]*from.scale_2,
                                                y0+x[1]*from.scale_1+y[1]*from.scale_y,
                                                x0+x[2]*from.scale_x+y[2]*from.scale_2,
                                                y0+x[2]*from.scale_1+y[2]*from.scale_y,
                                                x0+x[3]*from.scale_x+y[3]*from.scale_2,
                                                y0+x[3]*from.scale_1+y[3]*from.scale_y,
                                                style.font_color,
                                                style.font_size);
                            x[1] = x[3];
                            y[1] = y[3];
                            x[2] = x[3];
                            y[2] = y[3];
                        }else{
                            state = OFF_MID_OFF;
                            step = 3;
                            draw_sec_bezier(    x0+x[1]*from.scale_x+y[1]*from.scale_2,
                                                y0+x[1]*from.scale_1+y[1]*from.scale_y,
                                                x0+x[2]*from.scale_x+y[2]*from.scale_2,
                                                y0+x[2]*from.scale_1+y[2]*from.scale_y,
                                                x0+(x[2]+x[3])/2*from.scale_x+(y[2]+y[3])/2*from.scale_2,
                                                y0+(x[2]+x[3])/2*from.scale_1+(y[2]+y[3])/2*from.scale_y,
                                                style.font_color,
                                                style.font_size);
                            x[1] = (x[2]+x[3])/2;
                            y[1] = (y[2]+y[3])/2;
                            x[2] = x[3];
                            y[2] = y[3];
                        }
                        break;
                    // case OFF_ON_OFF_OFF:
                    // case OFF_ON_OFF_ON:
                    default:
                        step = 1;
                        x[1] = x[0];
                        y[1] = y[0];
                        //end
                        x[4] = x[0];
                        y[4] = y[0];
                        if(flag_bak&FLAG_On_Curve) state = ON;
                        else state = OFF;
                        break;
                }
                circle--;
            }
            if(circle == 0) flag ++;
        }
        switch (state)
        {
            case ON:
                draw_line(  x0+x[0]*from.scale_x+y[0]*from.scale_2,
                            y0+x[0]*from.scale_1+y[0]*from.scale_y,
                            x0+x[3]*from.scale_x+y[3]*from.scale_2,
                            y0+x[3]*from.scale_1+y[3]*from.scale_y,
                            style.font_color);
            break;
            case ON_MID_OFF:    
            case ON_OFF:
                draw_sec_bezier(    x0+x[0]*from.scale_x+y[0]*from.scale_2,
                                    y0+x[0]*from.scale_1+y[0]*from.scale_y,
                                    x0+x[1]*from.scale_x+y[1]*from.scale_2,
                                    y0+x[1]*from.scale_1+y[1]*from.scale_y,
                                    x0+x[3]*from.scale_x+y[3]*from.scale_2,
                                    y0+x[3]*from.scale_1+y[3]*from.scale_y,
                                    style.font_color,
                                    style.font_size);
                x[0] = x[1];
                y[0] = y[1];
            break;
            case OFF_ON:
                draw_sec_bezier(    x0+x[1]*from.scale_x+y[1]*from.scale_2,
                                    y0+x[1]*from.scale_1+y[1]*from.scale_y,
                                    x0+x[4]*from.scale_x+y[4]*from.scale_2,
                                    y0+x[4]*from.scale_1+y[4]*from.scale_y,
                                    x0+x[0]*from.scale_x+y[0]*from.scale_2,
                                    y0+x[0]*from.scale_1+y[0]*from.scale_y,
                                    style.font_color,
                                    style.font_size);
                x[0] = x[1];
                y[0] = y[1];
            break;
            case OFF_MID_OFF:
            case OFF_ON_OFF:
                draw_sec_bezier(    x0+x[1]*from.scale_x+y[1]*from.scale_2,
                                    y0+x[1]*from.scale_1+y[1]*from.scale_y,
                                    x0+x[2]*from.scale_x+y[2]*from.scale_2,
                                    y0+x[2]*from.scale_1+y[2]*from.scale_y,
                                    x0+(x[2]+x[4])/2*from.scale_x+(y[2]+y[4])/2*from.scale_2,
                                    y0+(x[2]+x[4])/2*from.scale_1+(y[2]+y[4])/2*from.scale_y,
                                    style.font_color,
                                    style.font_size);
                draw_sec_bezier(    x0+(x[2]+x[4])/2*from.scale_x+(y[2]+y[4])/2*from.scale_2,
                                    y0+(x[2]+x[4])/2*from.scale_1+(y[2]+y[4])/2*from.scale_y,
                                    x0+x[4]*from.scale_x+y[4]*from.scale_2,
                                    y0+x[4]*from.scale_1+y[4]*from.scale_y,
                                    x0+x[0]*from.scale_x+y[0]*from.scale_2,
                                    y0+x[0]*from.scale_1+y[0]*from.scale_y,
                                    style.font_color,
                                    style.font_size);
                x[0] = x[2];
                y[0] = y[2];
            break;
        default:
            break;
        }
    }
}

static size_t glyf_draw_complex(truetype_font_t* ttf,complex_glyph_def complex_glyph,size_t x0,size_t y0 ,font_style_t style)
{
    glyph_from_t from;
    glyph_head_t glyph_head;
    file_t* file = ttf->file;
    F2Dot14 scale = (style.font_size<<14)/ttf->units_per_em;

    if(ttf->loca_fmt){
        uint32_t index[2];
        ttf_seek(ttf->file,ttf->table_loca.offset+4*complex_glyph.index);
        ttf_read32(ttf->file,index,2);
        if(index[0]==index[1]) return (style.font_size>>2)+style.font_space;
        ttf_seek(file,ttf->table_glyf.offset+index[0]);
    }else{
        uint16_t index[2];
        ttf_seek(ttf->file,ttf->table_loca.offset+2*complex_glyph.index);
        ttf_read16(ttf->file,index,2);
        if(index[0]==index[1]) return (style.font_size>>2)+style.font_space;
        ttf_seek(file,ttf->table_glyf.offset+2*index[0]);
    }

    ttf_read16(file,&glyph_head,5);
    glyf_sample_parse(ttf,complex_glyph.index);

    from.scale_x = complex_glyph.scale_x/16384.0f*style.font_size/ttf->units_per_em;
    from.scale_y = complex_glyph.scale_y/16384.0f*style.font_size/ttf->units_per_em;
    from.scale_1 = complex_glyph.scale_1/16384.0f*style.font_size/ttf->units_per_em;
    from.scale_2 = complex_glyph.scale_2/16384.0f*style.font_size/ttf->units_per_em;

    from.x_crood = complex_glyph.x_crood;
    from.y_crood = complex_glyph.y_crood;

    if(from.scale_x<0)x0+=((from.x_crood+from.x_crood)*scale)>>14;
    if(from.scale_y<0)y0+=((from.y_crood+from.y_crood)*scale)>>14;
    glyf_draw_from(ttf,glyph_head.contours_num,x0,y0,from,style);

    return ((glyph_head.x_max*scale)>>14)+(style.font_space);
}

void ttf_print_utf8(truetype_font_t* ttf,size_t x0,size_t y0,const char* str,font_style_t style)
{
    char* str_ptr = str;
    size_t id;

    while(*str_ptr != 0)
    {
        switch (*str_ptr & 0xF0)
        {
        case 0xF0:
            id = (*str_ptr&0x00000007)<<18;
            str_ptr ++;
            id |= (*str_ptr&0x0000003F)<<12;
            str_ptr ++;
            id |= (*str_ptr&0x0000003F)<<6;
            str_ptr ++;
            id |= (*str_ptr&0x0000003F);
            str_ptr ++;
            break;
        case 0xE0:
            id = (*str_ptr&0x00000F)<<12;
            str_ptr ++;
            id |= (*str_ptr&0x00003F)<<6;
            str_ptr ++;
            id |= (*str_ptr&0x00003F);
            str_ptr ++;
            break;
        case 0xC0:
            id = (*str_ptr&0x001F)<<6;
            str_ptr ++;
            id |= (*str_ptr&0x003F);
            str_ptr ++;
            break;
        default:
            id = *str_ptr;
            str_ptr ++;
            break;
        }
        id = ttf_get_id(ttf,id);
        x0 += ttf_table_glyf_parse(ttf,id,x0,y0,style);
    }
}
