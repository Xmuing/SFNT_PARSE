#include "truetype.h"

typedef struct{
    uint16_t platform_id;
    uint16_t platform_spec_id;
    uint32_t offset;
}cmap_subtable_t;

typedef struct{
    uint16_t version;
    uint16_t subtables_num;
    cmap_subtable_t *subtable;
}cmap_index_t;

typedef struct{


}cmap_fmt0_t;

typedef struct{


}cmap_fmt2_t;

typedef struct{
    uint16_t length;
    uint16_t language;
    uint16_t segcount_x2;
    uint16_t search_range;
    uint16_t entry_selector;
    uint16_t range_shift;
    uint16_t *end_code;//end_code[segcount];
    uint16_t reserved;
    uint16_t *start_code;//start_code[segcount];
    int16_t  *id_delta;//id_delta[segcount];
    uint16_t *id_range_offset;//id_range_offset[segcount];
    uint16_t *glyph_index;//glyph_index[variable];
}cmap_fmt4_t;

typedef struct{


}cmap_fmt6_t;

typedef struct{


}cmap_fmt8_t;

typedef struct{


}cmap_fmt10_t;

typedef struct{


}cmap_fmt12_t;

typedef struct{


}cmap_fmt13_t;

typedef struct{


}cmap_fmt14_t;

static uint32_t cmap_get_offset_unicode(cmap_index_t index,cmap_subtable_t* subtable);

static void cmap_fmt4_parse(truetype_font_t* ttf,size_t offset);


bool ttf_table_cmap_parse(truetype_font_t* ttf)
{
    cmap_index_t index;
    uint16_t temp;
    file_t* file = ttf->file;

    ttf->end_code = NULL;
    ttf->start_code = NULL;
    ttf->id_delta = NULL;

    ttf->id_range_offset = NULL;
    // ttf->glyph_index = NULL;

    ttf_seek(file,ttf->table_cmap.offset);
    ttf_read16(file,&index,2);
    index.subtable = (cmap_subtable_t*)malloc(sizeof(cmap_subtable_t)*index.subtables_num);
    if(index.subtable == NULL){
        debug_print("NO ENOUGH MEMORY\n");
        return false;
    }
    for (int i = 0; i < index.subtables_num; i++)
    {
        ttf_read16(file,&index.subtable[i],2);
        ttf_read32(file,&index.subtable[i].offset,1);
    }
    uint32_t offset_unicode = cmap_get_offset_unicode(index,index.subtable);
    if(offset_unicode == 0){
        debug_print("Table cmap:Not suitable for Unicode encodings!\n");
        return false;
    }
    ttf_seek(file,ttf->table_cmap.offset + offset_unicode);
    ttf_read16(file,&ttf->cmap_fmt,1);
    switch (ttf->cmap_fmt)
    {
        case 0:
            /* code */
            break;
        case 2:
            /* code */
            break;
        case 4:
            cmap_fmt4_parse(ttf,ttf_tell(file));
            break;
        case 6:
            /* code */
            break;
        case 8:
            /* code */
            break;
        case 10:
            /* code */
            break;
        case 12:
            /* code */
            break;
        case 13:
            /* code */
            break;
        case 14:
            /* code */
            break;
        default:
            debug_print("Table cmap :unknow format type\n");
            return false;
    }

    free(index.subtable);
    return true;
}

static uint32_t cmap_get_offset_unicode(cmap_index_t index,cmap_subtable_t* subtable)
{
    for(int i=0;i<index.subtables_num;i++)
    {
        if(subtable[i].platform_id != 0) continue;
        switch(subtable[i].platform_spec_id)
        {
            case 0:
                debug_print("Table cmap : Unicode Version 1.0 semantics\n");
            break;
            case 1:
                debug_print("Table cmap : Unicode Version 1.1 semantics\n");
            break;
            case 2:
                debug_print("Table cmap : ISO 10646 1993 semantics (deprecated)\n");
            break;
            case 3:
                debug_print("Table cmap : Unicode 2.0 or later semantics (BMP only)\n");
            break;
            case 4:
                debug_print("Table cmap : Unicode 2.0 or later semantics (non-BMP characters allowed)\n");
            break;
            case 5:
                debug_print("Table cmap : Unicode Variation Sequences\n");
            break;
            case 6:
                debug_print("Table cmap : Last Resort\n");
            break;
            default:
                debug_print("Table cmap : subtable error!\n");
            break;
        }
        debug_print("Unicode Subtable Offset: 0x%x",subtable[i].offset);
        return subtable[i].offset;
    }
    return 0;
}


static void cmap_fmt0_parse()
{

}

static void cmap_fmt2_parse()
{

}

static void cmap_fmt4_parse(truetype_font_t* ttf,size_t offset)
{
    cmap_fmt4_t fmt;
    file_t* file = ttf->file;

    ttf_read16(file,&fmt,6);
    ttf->segcount = fmt.segcount_x2/2;

    ttf->end_code = (uint16_t*)malloc(ttf->segcount*sizeof(uint16_t));
    if(ttf->end_code == NULL){
        debug_print("NO ENOUGH MEMORY\n");
        return;
    }

    ttf->start_code = (uint16_t*)malloc(ttf->segcount*sizeof(uint16_t));
    if(ttf->start_code == NULL){
        debug_print("NO ENOUGH MEMORY\n");
        return;
    }

    ttf->id_delta = (int16_t*)malloc(ttf->segcount*sizeof(int16_t));
    if(ttf->id_delta == NULL){
        debug_print("NO ENOUGH MEMORY\n");
        return;
    }

    ttf->id_range_offset = (uint16_t*)malloc(ttf->segcount*sizeof(uint16_t));
    if(ttf->id_range_offset == NULL){
        debug_print("NO ENOUGH MEMORY\n");
        return;
    }

    ttf_read16(file,ttf->end_code,ttf->segcount);
    ttf_read16(file,&fmt.reserved,1);//can skip
    ttf_read16(file,ttf->start_code,ttf->segcount);
    ttf_read16(file,ttf->id_delta,ttf->segcount);

    ttf->id_range_offset_addr = ttf_tell(file);
    ttf_read16(file,ttf->id_range_offset,ttf->segcount);

    // uint16_t glyf_index_length = fmt.length-16-4*fmt.segcount_x2;
    // if(glyf_index_length){
    //     ttf->glyph_index = (uint16_t*)malloc(glyf_index_length);
    //     if(ttf->glyph_index == NULL){
    //         debug_print("NO ENOUGH MEMORY\n");
    //         return;
    //     }
    //     ttf_read16(file,ttf->glyph_index,glyf_index_length/2);
    // }
}

static void cmap_fmt6_parse()
{

}
static void cmap_fmt8_parse()
{

}

static void cmap_fmt10_parse()
{

}

static void cmap_fmt12_parse()
{

}

static void cmap_fmt13_parse()
{

}

static void cmap_fmt14_parse()
{

}

size_t ttf_get_id(truetype_font_t* ttf,size_t id)
{
    switch (ttf->cmap_fmt)
    {
        case 0:
            /* code */
            break;
        case 2:
            /* code */
            break;
        case 4:
            for(uint16_t i=0;i<ttf->segcount;i++){
                if(ttf->end_code[i]<id) continue;
                if(ttf->start_code[i]>id) return 0;
                if(ttf->id_range_offset[i]==0)
                    return id+ttf->id_delta[i];
                else{
                    size_t addr = ttf->id_range_offset_addr+sizeof(uint16_t)*i + ttf->id_range_offset[i] + 2*(id - ttf->start_code[i]);
                    ttf_seek(ttf->file,addr);
                    ttf_read16(ttf->file,&id,1);
                    return id;  
                }
            }
            return 0;
        case 6:
            /* code */
            break;
        case 8:
            /* code */
            break;
        case 10:
            /* code */
            break;
        case 12:
            /* code */
            break;
        case 13:
            /* code */
            break;
        case 14:
            /* code */
            break;
        default:
            debug_print("Table cmap :unknow format type\n");
            return 0;
    }
}
