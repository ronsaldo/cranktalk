#ifndef CRANK_TALK_GUI_H
#define CRANK_TALK_GUI_H

#include "object-model.h"

typedef struct crank_talk_Point_s crank_talk_Point_t;
typedef struct crank_talk_Rectangle_s crank_talk_Rectangle_t;
typedef struct crank_talk_Form_s crank_talk_Form_t;
typedef struct crank_talk_FormCanvas_s crank_talk_FormCanvas_t;
typedef struct crank_talk_OSWindow_s crank_talk_OSWindow_t;

struct crank_talk_Point_s
{
    crank_talk_Object_t super;
    crank_talk_oop_t x;
    crank_talk_oop_t y;
};

struct crank_talk_Rectangle_s
{
    crank_talk_Object_t super;
    crank_talk_Point_t *origin;
    crank_talk_Point_t *corner;
};

struct crank_talk_Form_s
{
    crank_talk_Object_t super;
    crank_talk_ByteArray_t *bits;
    crank_talk_oop_t width;
    crank_talk_oop_t height;
    crank_talk_oop_t depth;
};

struct crank_talk_FormCanvas_s
{
    crank_talk_Object_t super;
    crank_talk_Form_t form;
};

struct crank_talk_OSWindow_s
{
    crank_talk_Object_t super;
    crank_talk_ExternalAddress_t *handle;
};

#endif //CRANK_TALK_GUI_H

