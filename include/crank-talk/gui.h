#ifndef CRANK_TALK_GUI_H
#define CRANK_TALK_GUI_H

#include "object-model.h"

typedef struct crank_talk_Point_s crank_talk_Point_t;
typedef struct crank_talk_Rectangle_s crank_talk_Rectangle_t;
typedef struct crank_talk_Form_s crank_talk_Form_t;
typedef struct crank_talk_FormCanvas_s crank_talk_FormCanvas_t;

typedef struct crank_talk_OSWindow_s crank_talk_OSWindow_t;
typedef struct crank_talk_OSWindowEventHandler_s crank_talk_OSWindowEventHandler_t;

typedef struct crank_talk_OSEvent_s crank_talk_OSEvent_t;
    typedef struct crank_talk_OSMouseEvent_s crank_talk_OSMouseEvent_t;
        typedef struct crank_talk_OSMouseButtonEvent_s crank_talk_OSMouseButtonEvent_t;
            typedef struct crank_talk_OSMouseButtonPressEvent_s crank_talk_OSMouseButtonPressEvent_t;
            typedef struct crank_talk_OSMouseButtonReleaseEvent_s crank_talk_OSMouseButtonReleaseEvent_t;
        typedef struct crank_talk_OSMouseMoveEvent_s crank_talk_OSMouseMoveEvent_t;

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
    crank_talk_OSWindowEventHandler_t *eventHandler;
    crank_talk_Point_t *extent;
};

struct crank_talk_OSWindowEventHandler_s
{
    crank_talk_Object_t super;
};

struct crank_talk_OSEvent_s
{
    crank_talk_Object_t super;
    crank_talk_OSWindow_t *window;
};

struct crank_talk_OSMouseEvent_s
{
    crank_talk_OSEvent_t super;
    crank_talk_Point_t *position;
    crank_talk_Point_t *delta;
    crank_talk_oop_t modifiers;
};

struct crank_talk_OSMouseButtonEvent_s
{
    crank_talk_OSMouseEvent_t super;
    crank_talk_oop_t button;
};

struct crank_talk_OSMouseButtonPressEvent_s
{
    crank_talk_OSMouseButtonEvent_t super;
};

struct crank_talk_OSMouseButtonReleaseEvent_s
{
    crank_talk_OSMouseButtonEvent_t super;
};

struct crank_talk_OSMouseMoveEvent_s
{
    crank_talk_OSMouseEvent_t super;
};

#endif //CRANK_TALK_GUI_H

