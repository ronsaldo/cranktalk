#include "crank-talk/gui.h"
#include "crank-talk/array.h"
#include "crank-talk/context.h"
#include "crank-talk/message.h"
#define SDL_MAIN_HANDLED
#include <SDL.h>

static bool hasInitializedSDL2 = false;
static bool isQuitting = false;

CRANK_TALK_API crank_talk_Point_t*
crank_talk_Point_make(crank_talk_VMContext_t *context, crank_talk_oop_t x, crank_talk_oop_t y)
{
    crank_talk_Point_t *point = (crank_talk_Point_t *)CrankAllocateValuesObjectWithClassName(context, Point);
    point->x = x;
    point->y = y;
    return point;
}

static void
crank_talk_ensureSDL2Initialization()
{
    if(hasInitializedSDL2)
        return;

    SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1");

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE);

    hasInitializedSDL2 = true;
}

static void sdl2_dispatchEvent(crank_talk_VMContext_t *context, uint32_t windowID, crank_talk_oop_t event)
{
    SDL_Window *window = SDL_GetWindowFromID(windowID);
    if(!window)
        return;

    crank_talk_OSWindow_t *osWindow = SDL_GetWindowData(window, "OSWindow");
    if(!osWindow || !osWindow->eventHandler)
        return;

    crank_talk_send1(context, context->roots.processEventSymbol, (crank_talk_oop_t)osWindow->eventHandler, event);
}

static void
sdl2_onMouseButtonDown(crank_talk_VMContext_t *context, SDL_MouseButtonEvent *event)
{
    crank_talk_oop_t buttonEventValue = CrankAllocateValuesObjectWithClassName(context, OSMouseButtonPressEvent);
    crank_talk_OSMouseButtonPressEvent_t *buttonEvent = (crank_talk_OSMouseButtonPressEvent_t *)buttonEventValue;
    buttonEvent->super.super.delta = crank_talk_Point_make(context, crank_talk_oop_encodeSmallInteger(0), crank_talk_oop_encodeSmallInteger(0));
    buttonEvent->super.super.position = crank_talk_Point_make(context, crank_talk_oop_encodeSmallInteger(event->x), crank_talk_oop_encodeSmallInteger(event->y));
    buttonEvent->super.super.modifiers = crank_talk_oop_encodeSmallInteger(SDL_GetModState());
    buttonEvent->super.button = crank_talk_oop_encodeSmallInteger(event->button);
    
    sdl2_dispatchEvent(context, event->windowID, buttonEventValue);
}

static void
sdl2_onMouseButtonUp(crank_talk_VMContext_t *context, SDL_MouseButtonEvent *event)
{
    crank_talk_oop_t buttonEventValue = CrankAllocateValuesObjectWithClassName(context, OSMouseButtonReleaseEvent);
    crank_talk_OSMouseButtonReleaseEvent_t *buttonEvent = (crank_talk_OSMouseButtonReleaseEvent_t *)buttonEventValue;
    buttonEvent->super.super.delta = crank_talk_Point_make(context, crank_talk_oop_encodeSmallInteger(0), crank_talk_oop_encodeSmallInteger(0));
    buttonEvent->super.super.position = crank_talk_Point_make(context, crank_talk_oop_encodeSmallInteger(event->x), crank_talk_oop_encodeSmallInteger(event->y));
    buttonEvent->super.super.modifiers = crank_talk_oop_encodeSmallInteger(SDL_GetModState());
    buttonEvent->super.button = crank_talk_oop_encodeSmallInteger(event->button);
    
    sdl2_dispatchEvent(context, event->windowID, buttonEventValue);
}

static void
sdl2_onMouseMotion(crank_talk_VMContext_t *context, SDL_MouseMotionEvent *event)
{
    crank_talk_oop_t motionEventValue = CrankAllocateValuesObjectWithClassName(context, OSMouseMoveEvent);
    crank_talk_OSMouseMoveEvent_t *moveEvent = (crank_talk_OSMouseMoveEvent_t *)motionEventValue;
    moveEvent->super.delta = crank_talk_Point_make(context, crank_talk_oop_encodeSmallInteger(event->xrel), crank_talk_oop_encodeSmallInteger(event->yrel));
    moveEvent->super.position = crank_talk_Point_make(context, crank_talk_oop_encodeSmallInteger(event->x), crank_talk_oop_encodeSmallInteger(event->y));
    moveEvent->super.modifiers = crank_talk_oop_encodeSmallInteger(SDL_GetModState());
    moveEvent->buttonState = crank_talk_oop_encodeSmallInteger(event->state);
    
    sdl2_dispatchEvent(context, event->windowID, motionEventValue);
}

static void
sdl2_onMouseWheel(crank_talk_VMContext_t *context, SDL_MouseWheelEvent *event)
{
    crank_talk_oop_t wheelEventValue = CrankAllocateValuesObjectWithClassName(context, OSMouseWheelEvent);
    crank_talk_OSMouseWheelEvent_t *wheelEvent = (crank_talk_OSMouseWheelEvent_t *)wheelEventValue;
    wheelEvent->super.delta = crank_talk_Point_make(context, crank_talk_oop_encodeSmallInteger(0), crank_talk_oop_encodeSmallInteger(0));
    wheelEvent->super.position = crank_talk_Point_make(context, crank_talk_oop_encodeSmallInteger(event->mouseX), crank_talk_oop_encodeSmallInteger(event->mouseY));
    wheelEvent->super.modifiers = crank_talk_oop_encodeSmallInteger(SDL_GetModState());
    wheelEvent->scrollHorizontal = crank_talk_oop_encodeSmallInteger(event->x);
    wheelEvent->scrollVertical = crank_talk_oop_encodeSmallInteger(event->y);
    
    sdl2_dispatchEvent(context, event->windowID, wheelEventValue);
}

static void
sdl2_onWindowEvent(crank_talk_VMContext_t *context, SDL_WindowEvent *event)
{
    switch (event->event)
    {
    case SDL_WINDOWEVENT_EXPOSED:
    {
        crank_talk_oop_t exposeEventValue = CrankAllocateValuesObjectWithClassName(context, OSWindowExposeEvent);
        sdl2_dispatchEvent(context, event->windowID, exposeEventValue);
    } break;
    default:
        break;
    }
}

static void
processSDL2Event(crank_talk_VMContext_t *context, SDL_Event *event)
{
    switch(event->type)
    {
    case SDL_MOUSEBUTTONDOWN:
        sdl2_onMouseButtonDown(context, &event->button);
        break;
    case SDL_MOUSEBUTTONUP:
        sdl2_onMouseButtonUp(context, &event->button);
        break;
    case SDL_MOUSEMOTION:
        sdl2_onMouseMotion(context, &event->motion);
        break;
    case SDL_MOUSEWHEEL:
        sdl2_onMouseWheel(context, &event->wheel);
        break;
    case SDL_WINDOWEVENT:
        sdl2_onWindowEvent(context, &event->window);
        break;
    case SDL_QUIT:
        isQuitting = true;
        break;
    }
}

static void
processSDL2Events(crank_talk_VMContext_t *context)
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
        processSDL2Event(context, &event);
}

static crank_talk_oop_t
crank_talk_OSWindow_open_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    crank_talk_oop_t self = arguments[0];
    crank_talk_OSWindow_t *osWindow = (crank_talk_OSWindow_t *)self;
    crank_talk_ensureSDL2Initialization();

    intptr_t windowWidth = crank_talk_oop_decodeSmallInteger(osWindow->extent->x);
    intptr_t windowHeight = crank_talk_oop_decodeSmallInteger(osWindow->extent->y);

    SDL_Window *handle = SDL_CreateWindow("CrankTalk Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_SHOWN);
    osWindow->handle = (crank_talk_ExternalAddress_t*)crank_talk_ExternalAddress_make(context, handle);

    SDL_SetWindowData(handle, "OSWindow", osWindow);
    context->roots.openWindowList = crank_talk_Array_concat(context, context->roots.openWindowList, crank_talk_Array_With(context, self));
    return self;
}

static crank_talk_oop_t
crank_talk_OSWindow_mainLoop_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    while(!isQuitting)
    {
        processSDL2Events(context);
    }

    crank_talk_oop_t self = arguments[0];
    return self;
}

void
crank_talk_registerGUIPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetPrimitiveMethod(context, (crank_talk_oop_t)crank_talk_oop_getClass(context, context->roots.OSWindowClass), "mainLoop", 1, crank_talk_OSWindow_mainLoop_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.OSWindowClass, "open", 1, crank_talk_OSWindow_open_primitive);
}
