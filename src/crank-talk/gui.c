#include "crank-talk/gui.h"
#include "crank-talk/array.h"
#include "crank-talk/bytes.h"
#include "crank-talk/context.h"
#include "crank-talk/errors.h"
#include "crank-talk/io.h"
#include "crank-talk/message.h"
#include "crank-talk/string.h"
#include <stdio.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#undef STB_TRUETYPE_IMPLEMENTATION

CRANK_TALK_API crank_talk_Point_t*
crank_talk_Point_make(crank_talk_VMContext_t *context, crank_talk_oop_t x, crank_talk_oop_t y)
{
    crank_talk_Point_t *point = (crank_talk_Point_t *)CrankAllocateValuesObjectWithClassName(context, Point);
    point->x = x;
    point->y = y;
    return point;
}

static crank_talk_oop_t
crank_talk_Font_loadFromFile(crank_talk_VMContext_t *context, const char *fileName)
{
    crank_talk_ByteArray_t *bytes = crank_talk_io_readWholeFileAsByteArray(context, fileName);
    crank_talk_oop_t fontValue = CrankAllocateValuesObjectWithClassName(context, Font);
    crank_talk_Font_t *font = (crank_talk_Font_t *)fontValue;
    font->rawData = bytes;
    return fontValue;
}

static crank_talk_oop_t
crank_talk_Font_default(crank_talk_VMContext_t *context)
{
    if(!context->roots.defaultFont)
        context->roots.defaultFont = crank_talk_Font_loadFromFile(context, "assets/fonts/DejaVuSans.ttf");
    return context->roots.defaultFont;
}

static crank_talk_oop_t
crank_talk_Font_default_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    return crank_talk_Font_default(context);
}

static crank_talk_oop_t
crank_talk_FontFace_createWithSize(crank_talk_VMContext_t *context, crank_talk_Font_t *font, uint32_t fontFaceSize)
{
    crank_talk_Form_t *image = (crank_talk_Form_t*)CrankAllocateValuesObjectWithClassName(context, Form);
    int imageWidth = 512;
    int imageHeight = 512;

    image->width = crank_talk_oop_encodeSmallInteger(imageWidth);
    image->height = crank_talk_oop_encodeSmallInteger(imageHeight);
    image->depth = crank_talk_oop_encodeSmallInteger(8);
    image->bits = (crank_talk_ByteArray_t*)crank_talk_ByteArray_create(context, imageWidth*imageHeight);

    size_t bakedCharDataSize = 256-31;
    stbtt_bakedchar *bakedCharsData = calloc(bakedCharDataSize, sizeof(stbtt_bakedchar));
    stbtt_BakeFontBitmap(font->rawData->elements, 0, (float)fontFaceSize, image->bits->elements, imageWidth, imageHeight, 31, bakedCharDataSize, bakedCharsData);

    float ascent;
    float descent;
    float linegap;
    stbtt_GetScaledFontVMetrics(font->rawData->elements, 0, (float)fontFaceSize, &ascent, &descent, &linegap);

    printf("Font face ascent %f descent %f linegap %f\n", ascent, descent, linegap);
    crank_talk_FontFace_t *fontFace = (crank_talk_FontFace_t *)CrankAllocateValuesObjectWithClassName(context, FontFace);

    fontFace->image = image;
    fontFace->ascent = crank_talk_oop_fromFloat(context, ascent);
    fontFace->descent = crank_talk_oop_fromFloat(context, descent);
    fontFace->linegap = crank_talk_oop_fromFloat(context, linegap);

    fontFace->bakedChars = (crank_talk_Array_t*)crank_talk_Array_create(context, bakedCharDataSize);
    for(size_t i = 0; i < bakedCharDataSize; ++i)
    {
        stbtt_bakedchar *bakedChar = bakedCharsData + i;
        crank_talk_FontBakedChar_t *fontBakedChar = (crank_talk_FontBakedChar_t *)CrankAllocateValuesObjectWithClassName(context, FontBakedChar);
        fontBakedChar->x0 = crank_talk_oop_encodeSmallInteger(bakedChar->x0);
        fontBakedChar->y0 = crank_talk_oop_encodeSmallInteger(bakedChar->y0);
        fontBakedChar->x1 = crank_talk_oop_encodeSmallInteger(bakedChar->x1);
        fontBakedChar->y1 = crank_talk_oop_encodeSmallInteger(bakedChar->y1);
        fontBakedChar->xoff = crank_talk_oop_fromFloat(context, bakedChar->xoff);
        fontBakedChar->yoff = crank_talk_oop_fromFloat(context, bakedChar->yoff);
        fontBakedChar->advance = crank_talk_oop_fromFloat(context, bakedChar->xadvance);
        fontFace->bakedChars->elements[i] = (crank_talk_oop_t)fontBakedChar;
    }

    free(bakedCharsData);

    return (crank_talk_oop_t)fontFace;
}

static crank_talk_oop_t
crank_talk_FontFace_default(crank_talk_VMContext_t *context)
{
    if(!context->roots.defaultFontFace)
        context->roots.defaultFontFace = crank_talk_FontFace_createWithSize(context, (crank_talk_Font_t*)crank_talk_Font_default(context), 16);
    return context->roots.defaultFontFace;
}

static crank_talk_oop_t
crank_talk_FontFace_default_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    return crank_talk_FontFace_default(context);
}

static uint32_t
crank_talk_Color_encodeBGRA32(crank_talk_Color_t *color)
{
    float r = (float)crank_talk_oop_numberAsFloat(color->r);
    float g = (float)crank_talk_oop_numberAsFloat(color->g);
    float b = (float)crank_talk_oop_numberAsFloat(color->b);
    float a = (float)crank_talk_oop_numberAsFloat(color->a);
    return (uint32_t)(b * 255) | ((uint32_t)(g * 255) << 8) | ((uint32_t)(r * 255) << 16) | ((uint32_t)(a * 255) << 24);
}

static crank_talk_oop_t
crank_talk_FormCanvas_fillWithColor_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);

    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t colorValue = arguments[1];
    crank_talk_FormCanvas_t *formCanvas = (crank_talk_FormCanvas_t *)self;
    if(!crank_talk_oop_isKindOf(context, colorValue, context->roots.ColorClass))
        crank_talk_error(context, "Expected a color.");

    crank_talk_Color_t *color = (crank_talk_Color_t*)colorValue;

    crank_talk_Form_t *form = formCanvas->form;
    if(crank_talk_oop_decodeSmallInteger(form->depth) != 32)
        crank_talk_error(context, "Only 32 bit forms are supported.");
    
    size_t formWidth = (size_t)crank_talk_oop_decodeSmallInteger(form->width);
    size_t formHeight = (size_t)crank_talk_oop_decodeSmallInteger(form->height);
    uint8_t *formBits = form->bits->elements;

    size_t pixelCount = formWidth*formHeight;
    uint32_t *pixel = (uint32_t *)formBits;
    uint32_t fillColor = crank_talk_Color_encodeBGRA32(color);
    for(size_t i = 0; i < pixelCount; ++i)
        pixel[i] = fillColor;
    
    return self;
}

static void
crank_talk_FormCanvas_fillRectangleWithColor(crank_talk_VMContext_t *context, crank_talk_Form_t *form,
    float fMinX, float fMinY, float fMaxX, float fMaxY,
    uint32_t fillColor )
{
    // Fetch the form parameters
    int formWidth = (int)crank_talk_oop_decodeSmallInteger(form->width);
    int formHeight = (int)crank_talk_oop_decodeSmallInteger(form->height);
    int formPitch = formWidth*4;
    uint8_t *formBits = form->bits->elements;

    // Clamp the rectangle.
    int minX = (int)fMinX;
    int minY = (int)fMinY;
    int maxX = (int)fMaxX;
    int maxY = (int)fMaxY;
    if(minX < 0) minX = 0;
    if(minY < 0) minY = 0;
    if(maxX > formWidth) maxX = formWidth;
    if(maxY > formHeight) maxY = formHeight;

    // Fill the rectangle
    for(int y = minY; y < maxY; ++y)
    {
        uint32_t *row = (uint32_t*)(formBits + formPitch*y);
        for(int x = minX; x < maxX; ++x)
            row[x] = fillColor;
    }
}

static crank_talk_oop_t
crank_talk_FormCanvas_fillRectangleWithColor_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 3);

    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t rectangleValue = arguments[1];
    crank_talk_oop_t colorValue = arguments[2];
    crank_talk_FormCanvas_t *formCanvas = (crank_talk_FormCanvas_t *)self;
    if(!crank_talk_oop_isKindOf(context, rectangleValue, context->roots.RectangleClass))
        crank_talk_error(context, "Expected a rectangle.");
    if(!crank_talk_oop_isKindOf(context, colorValue, context->roots.ColorClass))
        crank_talk_error(context, "Expected a color.");

    crank_talk_Rectangle_t *rectangle = (crank_talk_Rectangle_t*)rectangleValue;
    crank_talk_Color_t *color = (crank_talk_Color_t*)colorValue;

    crank_talk_Form_t *form = formCanvas->form;
    if(crank_talk_oop_decodeSmallInteger(form->depth) != 32)
        crank_talk_error(context, "Only 32 bit forms are supported.");
    
    // Fetch the rectangle corners.
    float originX = (float)crank_talk_oop_numberAsFloat(formCanvas->origin->x);
    float originY = (float)crank_talk_oop_numberAsFloat(formCanvas->origin->y);
    float scaleX = (float)crank_talk_oop_numberAsFloat(formCanvas->scaleFactor->x);
    float scaleY = (float)crank_talk_oop_numberAsFloat(formCanvas->scaleFactor->y);

    float fMinX = ((float)crank_talk_oop_numberAsFloat(rectangle->origin->x) + originX) * scaleX;
    float fMinY = ((float)crank_talk_oop_numberAsFloat(rectangle->origin->y) + originY) * scaleY;
    float fMaxX = ((float)crank_talk_oop_numberAsFloat(rectangle->corner->x) + originX) * scaleX;
    float fMaxY = ((float)crank_talk_oop_numberAsFloat(rectangle->corner->y) + originY) * scaleY;

    uint32_t fillColor = crank_talk_Color_encodeBGRA32(color);
    crank_talk_FormCanvas_fillRectangleWithColor(context, form, fMinX, fMinY, fMaxX, fMaxY, fillColor);
    return self;
}

static crank_talk_oop_t
crank_talk_FormCanvas_frameRectangleWithBorderWithColor_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 3);

    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t rectangleValue = arguments[1];
    crank_talk_oop_t borderWidthValue = arguments[2];
    crank_talk_oop_t colorValue = arguments[3];
    crank_talk_FormCanvas_t *formCanvas = (crank_talk_FormCanvas_t *)self;
    if(!crank_talk_oop_isKindOf(context, rectangleValue, context->roots.RectangleClass))
        crank_talk_error(context, "Expected a rectangle.");
    if(!crank_talk_oop_isKindOf(context, colorValue, context->roots.ColorClass))
        crank_talk_error(context, "Expected a color.");

    crank_talk_Rectangle_t *rectangle = (crank_talk_Rectangle_t*)rectangleValue;
    crank_talk_Color_t *color = (crank_talk_Color_t*)colorValue;

    crank_talk_Form_t *form = formCanvas->form;
    if(crank_talk_oop_decodeSmallInteger(form->depth) != 32)
        crank_talk_error(context, "Only 32 bit forms are supported.");
    

    float borderWidth = (float)crank_talk_oop_numberAsFloat(borderWidthValue);

    // Fetch the rectangle corners.
    float originX = (float)crank_talk_oop_numberAsFloat(formCanvas->origin->x);
    float originY = (float)crank_talk_oop_numberAsFloat(formCanvas->origin->y);
    float scaleX = (float)crank_talk_oop_numberAsFloat(formCanvas->scaleFactor->x);
    float scaleY = (float)crank_talk_oop_numberAsFloat(formCanvas->scaleFactor->y);

    float fMinX = ((float)crank_talk_oop_numberAsFloat(rectangle->origin->x) + originX) * scaleX;
    float fMinY = ((float)crank_talk_oop_numberAsFloat(rectangle->origin->y) + originY) * scaleY;
    float fMaxX = ((float)crank_talk_oop_numberAsFloat(rectangle->corner->x) + originX) * scaleX;
    float fMaxY = ((float)crank_talk_oop_numberAsFloat(rectangle->corner->y) + originY) * scaleY;

    uint32_t fillColor = crank_talk_Color_encodeBGRA32(color);

    // Left
    crank_talk_FormCanvas_fillRectangleWithColor(context, form, fMinX, fMinY, fMinX + borderWidth, fMaxY, fillColor);
    // Right
    crank_talk_FormCanvas_fillRectangleWithColor(context, form, fMaxX - borderWidth, fMinY, fMaxX, fMaxY, fillColor);

    // Top
    crank_talk_FormCanvas_fillRectangleWithColor(context, form, fMinX, fMinY, fMaxX, fMinY + borderWidth, fillColor);
    // Bottom
    crank_talk_FormCanvas_fillRectangleWithColor(context, form, fMinX, fMaxY - borderWidth, fMaxX, fMaxY, fillColor);

    return self;
}

void
crank_talk_registerGUIPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetPrimitiveMethod(context, (crank_talk_oop_t)crank_talk_oop_getClass(context, context->roots.FontClass), "default", 3, crank_talk_Font_default_primitive);
    
    crank_talk_createAndSetPrimitiveMethod(context, (crank_talk_oop_t)crank_talk_oop_getClass(context, context->roots.FontFaceClass), "default", 3, crank_talk_FontFace_default_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FormCanvasClass, "fillWithColor:", 3, crank_talk_FormCanvas_fillWithColor_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FormCanvasClass, "fillRectangle:color:", 3, crank_talk_FormCanvas_fillRectangleWithColor_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FormCanvasClass, "frameRectangle:borderWidth:color:", 4, crank_talk_FormCanvas_frameRectangleWithBorderWithColor_primitive);
}

