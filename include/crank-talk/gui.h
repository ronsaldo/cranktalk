#ifndef CRANK_TALK_GUI_H
#define CRANK_TALK_GUI_H

#include "object-model.h"

typedef struct crank_talk_Color_s crank_talk_Color_t;
typedef struct crank_talk_Point_s crank_talk_Point_t;
typedef struct crank_talk_Rectangle_s crank_talk_Rectangle_t;
typedef struct crank_talk_Form_s crank_talk_Form_t;
typedef struct crank_talk_FormCanvas_s crank_talk_FormCanvas_t;

typedef struct crank_talk_OSWindow_s crank_talk_OSWindow_t;
typedef struct crank_talk_OSWindowEventHandler_s crank_talk_OSWindowEventHandler_t;

typedef struct crank_talk_OSEvent_s crank_talk_OSEvent_t;
    typedef struct crank_talk_OSKeyboardEvent_s crank_talk_OSKeyboardEvent_t;
        typedef struct crank_talk_OSKeyDownEvent_s crank_talk_OSKeyDownEvent_t;
        typedef struct crank_talk_OSKeyUpEvent_s crank_talk_OSKeyUpEvent_t;
    typedef struct crank_talk_OSTextInputEvent_s crank_talk_OSTextInputEvent_t;
    typedef struct crank_talk_OSMouseEvent_s crank_talk_OSMouseEvent_t;
        typedef struct crank_talk_OSMouseButtonEvent_s crank_talk_OSMouseButtonEvent_t;
            typedef struct crank_talk_OSMouseButtonPressEvent_s crank_talk_OSMouseButtonPressEvent_t;
            typedef struct crank_talk_OSMouseButtonReleaseEvent_s crank_talk_OSMouseButtonReleaseEvent_t;
        typedef struct crank_talk_OSMouseMoveEvent_s crank_talk_OSMouseMoveEvent_t;
        typedef struct crank_talk_OSMouseWheelEvent_s crank_talk_OSMouseWheelEvent_t;
    typedef struct crank_talk_OSWindowEvent_s crank_talk_OSWindowEvent_t;
        typedef struct crank_talk_OSWindowExposeEvent_s crank_talk_OSWindowExposeEvent_t;


typedef struct crank_talk_OSWindowRenderer_s crank_talk_OSWindowRenderer_t;
typedef struct crank_talk_OSWindowFormRenderer_s crank_talk_OSWindowFormRenderer_t;

struct crank_talk_Color_s
{
    crank_talk_Object_t super;
    crank_talk_oop_t r;
    crank_talk_oop_t g;
    crank_talk_oop_t b;
    crank_talk_oop_t a;
};

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
    crank_talk_Form_t *form;
    crank_talk_Point_t *origin;
    crank_talk_Point_t *scaleFactor;
};

struct crank_talk_OSWindow_s
{
    crank_talk_Object_t super;
    crank_talk_ExternalAddress_t *handle;
    crank_talk_OSWindowEventHandler_t *eventHandler;
    crank_talk_Point_t *extent;
    crank_talk_OSWindowRenderer_t *renderer;
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

struct crank_talk_OSKeyboardEvent_s
{
    crank_talk_OSEvent_t super;
    crank_talk_oop_t scancode;
    crank_talk_oop_t symbol;
    crank_talk_oop_t modifiers;
    crank_talk_oop_t repeat;
};

struct crank_talk_OSKeyDownEvent_s
{
    crank_talk_OSKeyboardEvent_t super;
};

struct crank_talk_OSKeyUpEvent_s
{
    crank_talk_OSKeyboardEvent_t super;
};

struct crank_talk_OSTextInputEvent_s
{
    crank_talk_OSEvent_t super;
    crank_talk_oop_t text;
    crank_talk_oop_t modifiers;
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
    crank_talk_oop_t buttonState;
};

struct crank_talk_OSMouseWheelEvent_s
{
    crank_talk_OSMouseEvent_t super;
    crank_talk_oop_t scrollHorizontal;
    crank_talk_oop_t scrollVertical;
};

struct crank_talk_OSWindowEvent_s
{
    crank_talk_OSMouseEvent_t super;
};

struct crank_talk_OSWindowExposeEvent_s
{
    crank_talk_OSMouseEvent_t super;
};

struct crank_talk_OSWindowRenderer_s
{
    crank_talk_Object_t super;
    crank_talk_ExternalAddress_t *rendererHandle; 
};

struct crank_talk_OSWindowFormRenderer_s
{
    crank_talk_OSWindowRenderer_t super;
    crank_talk_Form_t *form;
    crank_talk_ExternalAddress_t *textureHandle; 
};

CRANK_TALK_API crank_talk_Point_t* crank_talk_Point_make(crank_talk_VMContext_t *context, crank_talk_oop_t x, crank_talk_oop_t y);

#endif //CRANK_TALK_GUI_H

