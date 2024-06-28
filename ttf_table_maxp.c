#include "truetype.h"

typedef struct
{
    Fixed Version;                  //0x00010000 for version 1.0.
    uint16_t num_Glyphs;              //Number of glyphs in the font .
    uint16_t max_Points;              //Max points in noncomposite glyph .
    uint16_t max_Contours;            //Max contours in noncomposite glyph.
    uint16_t max_CompositePoints;     //Max points in a composite glyph.
    uint16_t max_CompositeContours;   //Max contours in a composite glyph.
    uint16_t max_Zones;               // 1 if not use the twilight zone [Z0],//or 2 if so use Z0;2 in most cases.
    uint16_t max_TwilightPoints;      // Maximum points used in Z0.
    uint16_t max_Storage;             //Number of storage area locations.
    uint16_t max_FunctionDefs;        //Number of FDEFs.
    uint16_t max_InstructionDefs;
    uint16_t max_StackElements;       //Number of depth.
    uint16_t max_SizeOfInstructions;  //Max byte count for glyph inst.
    uint16_t max_ComponentElements;   //Max number top components refernced.
    uint16_t max_ComponentDepth;      //Max levels of recursion.
}maxp_table_t;

bool ttf_table_maxp_parse(truetype_font_t* ttf)
{
    maxp_table_t maxp;
    file_t* file = ttf->file;
    ttf_seek(file,ttf->table_maxp.offset);

    ttf_read16(file,&maxp,15);
    ttf->num_Glyphs = maxp.num_Glyphs;
    ttf->max_Points = maxp.max_Points;
    ttf->max_Contours = maxp.max_Contours;

    // ttf->Glyphs_index = NULL;
    ttf->Points_x = NULL;
    ttf->Points_y = NULL;
    ttf->Points_end = NULL;
    ttf->flags = NULL;

    // ttf->Glyphs_index = (uint32_t*)malloc((maxp.num_Glyphs+1)*sizeof(uint32_t));
    // if(ttf->Glyphs_index == NULL){
    //     debug_print("NO ENOUGH MEMORY\n");
    //     return false;
    // }
    ttf->Points_x = (uint16_t*)malloc(maxp.max_Points*sizeof(uint16_t));
    if(ttf->Points_x == NULL){
        debug_print("NO ENOUGH MEMORY\n");
        return false;
    }
    ttf->Points_y = (uint16_t*)malloc(maxp.max_Points*sizeof(uint16_t));
    if(ttf->Points_y == NULL){
        debug_print("NO ENOUGH MEMORY\n");
        return false;
    }
    ttf->Points_end = (uint16_t*)malloc(maxp.max_Contours*sizeof(uint16_t));
    if(ttf->Points_end == NULL){
        debug_print("NO ENOUGH MEMORY\n");
        return false;
    }
    ttf->flags = (uint8_t*)malloc(maxp.max_Points*sizeof(uint8_t));
    if(ttf->flags == NULL){
        debug_print("NO ENOUGH MEMORY\n");
        return false;
    }
    return true;
}