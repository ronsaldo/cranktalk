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
crank_talk_Font_defaultMonospaced(crank_talk_VMContext_t *context)
{
    if(!context->roots.defaultMonospacedFont)
        context->roots.defaultMonospacedFont = crank_talk_Font_loadFromFile(context, "assets/fonts/DejaVuSansMono.ttf");
    return context->roots.defaultMonospacedFont;
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
crank_talk_Font_defaultMonospaced_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    return crank_talk_Font_defaultMonospaced(context);
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

static crank_talk_oop_t
crank_talk_FontFace_defaultMonospaced(crank_talk_VMContext_t *context)
{
    if(!context->roots.defaultMonospacedFontFace)
        context->roots.defaultMonospacedFontFace = crank_talk_FontFace_createWithSize(context, (crank_talk_Font_t*)crank_talk_Font_defaultMonospaced(context), 16);
    return context->roots.defaultMonospacedFontFace;
}

static crank_talk_oop_t
crank_talk_FontFace_defaultMonospaced_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    return crank_talk_FontFace_defaultMonospaced(context);
}

static stbtt_bakedchar
crank_talk_FontFace_readBakedChar(crank_talk_FontBakedChar_t *fontBakedChar)
{
    stbtt_bakedchar baked = {0};
    baked.x0 = crank_talk_oop_decodeSmallInteger(fontBakedChar->x0);
    baked.y0 = crank_talk_oop_decodeSmallInteger(fontBakedChar->y0);
    baked.x1 = crank_talk_oop_decodeSmallInteger(fontBakedChar->x1);
    baked.y1 = crank_talk_oop_decodeSmallInteger(fontBakedChar->y1);
    baked.xoff = crank_talk_oop_numberAsFloat(fontBakedChar->xoff);
    baked.yoff = crank_talk_oop_numberAsFloat(fontBakedChar->yoff);
    baked.xadvance = crank_talk_oop_numberAsFloat(fontBakedChar->advance);
    return baked;
}

static crank_talk_oop_t
crank_talk_FontFace_measureTextExtent_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);

    crank_talk_FontFace_t *fontFace = (crank_talk_FontFace_t *)arguments[0];
    crank_talk_String_t *text =  (crank_talk_String_t *)arguments[1];

    int imageWidth = (int)crank_talk_oop_decodeSmallInteger(fontFace->image->width);
    int imageHeight = (int)crank_talk_oop_decodeSmallInteger(fontFace->image->width);

    float extentX = 0;
    float extentY = 0;
    float currentBaselineX = 0;
    float currentBaselineY = crank_talk_oop_numberAsFloat(fontFace->ascent);
    size_t textSize = crank_talk_object_getSizeInBytes((crank_talk_oop_t)text);
    for(size_t i = 0; i < textSize; ++i)
    {
        // TODO: Decode UTF-8.
        char c = text->elements[i];
        if(c < ' ')
            continue;

        crank_talk_FontBakedChar_t *fontBakedChar = (crank_talk_FontBakedChar_t *)fontFace->bakedChars->elements[c - 31];
        stbtt_bakedchar bakedChar = crank_talk_FontFace_readBakedChar(fontBakedChar);

        stbtt_aligned_quad quadToDraw = {};
        stbtt_GetBakedQuad(&bakedChar, imageWidth, imageHeight, 0, &currentBaselineX, &currentBaselineY, &quadToDraw, true);

        if(quadToDraw.x0 > extentX) extentX = quadToDraw.x0;
        if(quadToDraw.x1 > extentX) extentX = quadToDraw.x1;
        if(quadToDraw.y0 > extentY) extentY = quadToDraw.y0;
        if(quadToDraw.y1 > extentY) extentY = quadToDraw.y1;
    }

    return (crank_talk_oop_t)crank_talk_Point_make(context, crank_talk_oop_encodeSmallInteger(ceil(extentX)), crank_talk_oop_encodeSmallInteger(ceil(extentY)));
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

static void
crank_talk_FormCanvas_drawCharacterRectangleWithColor(crank_talk_VMContext_t *context,
    crank_talk_Form_t *form, float fMinX, float fMinY, float fMaxX, float fMaxY,
    crank_talk_Form_t *sourceForm, float fSourceMinX, float fSourceMinY, float fSourceMaxX, float fSourceMaxY,
    uint32_t fillColor)
{
    // Fetch the form parameters
    int formWidth = (int)crank_talk_oop_decodeSmallInteger(form->width);
    int formHeight = (int)crank_talk_oop_decodeSmallInteger(form->height);
    int formPitch = formWidth*4;
    uint8_t *formBits = form->bits->elements;

    // Fetch the source form parameters
    int sourceFormWidth = (int)crank_talk_oop_decodeSmallInteger(sourceForm->width);
    int sourceFormHeight = (int)crank_talk_oop_decodeSmallInteger(sourceForm->height);
    int sourceFormPitch = sourceFormWidth;
    uint8_t *sourceFormBits = sourceForm->bits->elements;

    // Clamp the rectangle.
    int minX = (int)fMinX;
    int minY = (int)fMinY;
    int maxX = (int)fMaxX;
    int maxY = (int)fMaxY;
    if(minX < 0) minX = 0;
    if(minY < 0) minY = 0;
    if(maxX > formWidth) maxX = formWidth;
    if(maxY > formHeight) maxY = formHeight;

    uint8_t fillColorB = fillColor & 0xFF;
    uint8_t fillColorG = (fillColor >> 8) & 0xFF;
    uint8_t fillColorR = (fillColor >> 16) & 0xFF;
    //uint8_t fillColorA = (fillColor >> 24) & 0xFF;

    // Blit the rectangle
    for(int y = minY; y < maxY; ++y)
    {
        float alphaY = (y - fMinY) / (fMaxY - fMinY);
        float t = fSourceMinY * (1 - alphaY) + fSourceMaxY*alphaY;
        //printf("alphaY %f: %f\n", alphaY, t);
        
        uint32_t *row = (uint32_t*)(formBits + formPitch*y);
        for(int x = minX; x < maxX; ++x)
        {
            float alphaX = (x - fMinX) / (fMaxX - fMinX);
            float s = fSourceMinX * (1 - alphaX) + fSourceMaxX*alphaX;

            float sourceS = sourceFormWidth*s;
            float sourceT = sourceFormHeight*t;
            int sourceLeft = (int)sourceS;
            int sourceTop = (int)sourceT;
            int sourceRight = sourceLeft + 1;
            int sourceBottom = sourceTop + 1;
            float fractS = sourceS - sourceLeft;
            float fractT = sourceT - sourceTop;

            // Compute the alpha corners
            float sourceTopLeftAlpha = *(sourceFormBits + sourceFormPitch*sourceTop + sourceLeft) / 255.0f;
            float sourceTopRightAlpha = *(sourceFormBits + sourceFormPitch*sourceTop + sourceRight) / 255.0f;
            float sourceBottomLeftAlpha = *(sourceFormBits + sourceFormPitch*sourceBottom + sourceLeft) / 255.0f;
            float sourceBottomRightAlpha = *(sourceFormBits + sourceFormPitch*sourceBottom + sourceRight) / 255.0f;

            // Compute the interpolated alpha.
            float sourceTopAlpha = (1.0f - fractS)*sourceTopLeftAlpha + fractS*sourceTopRightAlpha;
            float sourceBottomAlpha = (1.0f - fractS)*sourceBottomLeftAlpha + fractS*sourceBottomRightAlpha;
            float sourceAlpha = (1.0f - fractT)*sourceTopAlpha + fractT*sourceBottomAlpha;

            uint32_t destColor = row[x];
            uint8_t destColorB = destColor & 0xFF;
            uint8_t destColorG = (destColor >> 8) & 0xFF;
            uint8_t destColorR = (destColor >> 16) & 0xFF;
            //uint8_t destColorA = (destColor >> 24) & 0xFF;

            float blendColorB = fillColorB*sourceAlpha + destColorB*(1.0f - sourceAlpha);
            float blendColorG = fillColorG*sourceAlpha + destColorG*(1.0f - sourceAlpha);
            float blendColorR = fillColorR*sourceAlpha + destColorR*(1.0f - sourceAlpha);

            row[x] = ((uint32_t)blendColorB) | ((uint32_t)blendColorG << 8) | ((uint32_t)blendColorR << 16);
        }
            
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

static crank_talk_oop_t
crank_talk_FormCanvas_drawStringFromToAtWithFontAndColor_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 7);

    crank_talk_oop_t self = arguments[0];
    crank_talk_String_t *string = (crank_talk_String_t *)arguments[1];
    intptr_t stringStartIndex = crank_talk_oop_decodeSmallInteger(arguments[2]);
    intptr_t stringStopIndex = crank_talk_oop_decodeSmallInteger(arguments[3]);
    crank_talk_Point_t *baselineStart = (crank_talk_Point_t *)arguments[4];
    crank_talk_FontFace_t *fontFace = (crank_talk_FontFace_t *)arguments[5];
    crank_talk_Color_t *color = (crank_talk_Color_t *)arguments[6];
    uint32_t fillColor = crank_talk_Color_encodeBGRA32(color);
    
    crank_talk_FormCanvas_t *formCanvas = (crank_talk_FormCanvas_t *)self;
    crank_talk_Form_t *form = formCanvas->form;
    if(crank_talk_oop_decodeSmallInteger(form->depth) != 32)
        crank_talk_error(context, "Only 32 bit forms are supported.");
        
    crank_talk_Form_t *fontForm = fontFace->image;
    int fontImageWidth  = (int)crank_talk_oop_decodeSmallInteger(fontForm->width);
    int fontImageHeight = (int)crank_talk_oop_decodeSmallInteger(fontForm->height);
    float currentBaselineX = (float)crank_talk_oop_numberAsFloat(baselineStart->x) + crank_talk_oop_numberAsFloat(formCanvas->origin->x);
    float currentBaselineY = (float)crank_talk_oop_numberAsFloat(baselineStart->y) + crank_talk_oop_numberAsFloat(formCanvas->origin->y);

    // TODO: Decode UTF-8
    for(size_t i = stringStartIndex - 1; i < stringStopIndex; ++i)
    {
        char c = string->elements[i];
        if(c < ' ') continue;

        crank_talk_FontBakedChar_t *fontBakedChar = (crank_talk_FontBakedChar_t *)fontFace->bakedChars->elements[c - 31];
        stbtt_bakedchar bakedChar = crank_talk_FontFace_readBakedChar(fontBakedChar);

        stbtt_aligned_quad quadToDraw = {};
        stbtt_GetBakedQuad(&bakedChar, fontImageWidth, fontImageHeight, 0, &currentBaselineX, &currentBaselineY, &quadToDraw, true);

        //crank_talk_FormCanvas_fillRectangleWithColor(context, form, quadToDraw.x0, quadToDraw.y0, quadToDraw.x1, quadToDraw.y1, fillColor);
        crank_talk_FormCanvas_drawCharacterRectangleWithColor(context,
            form, quadToDraw.x0, quadToDraw.y0, quadToDraw.x1, quadToDraw.y1,
            fontForm, quadToDraw.s0, quadToDraw.t0, quadToDraw.s1, quadToDraw.t1,
            fillColor);
    }

    return self;
}

void
crank_talk_registerGUIPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetPrimitiveMethod(context, (crank_talk_oop_t)crank_talk_oop_getClass(context, context->roots.FontClass), "default", 1, crank_talk_Font_default_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, (crank_talk_oop_t)crank_talk_oop_getClass(context, context->roots.FontClass), "defaultMonospaced", 1, crank_talk_Font_defaultMonospaced_primitive);
    
    crank_talk_createAndSetPrimitiveMethod(context, (crank_talk_oop_t)crank_talk_oop_getClass(context, context->roots.FontFaceClass), "default", 1, crank_talk_FontFace_default_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, (crank_talk_oop_t)crank_talk_oop_getClass(context, context->roots.FontFaceClass), "defaultMonospaced", 1, crank_talk_FontFace_defaultMonospaced_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FontFaceClass, "measureTextExtent:", 2, crank_talk_FontFace_measureTextExtent_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FormCanvasClass, "fillWithColor:", 3, crank_talk_FormCanvas_fillWithColor_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FormCanvasClass, "fillRectangle:color:", 3, crank_talk_FormCanvas_fillRectangleWithColor_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FormCanvasClass, "frameRectangle:borderWidth:color:", 4, crank_talk_FormCanvas_frameRectangleWithBorderWithColor_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FormCanvasClass, "drawString:from:to:at:font:color:", 7, crank_talk_FormCanvas_drawStringFromToAtWithFontAndColor_primitive);
}

