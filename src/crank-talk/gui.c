#include "crank-talk/gui.h"
#include "crank-talk/context.h"
#include "crank-talk/message.h"
#define SDL_MAIN_HANDLED
#include <SDL.h>

static bool hasInitializedSDL2 = false;
static bool isQuitting = false;

static void
crank_talk_ensureSDL2Initialization()
{
    if(hasInitializedSDL2)
        return;

    SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1");

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE);

    hasInitializedSDL2 = true;
}

static void
processSDL2Event(crank_talk_VMContext_t *context, SDL_Event *event)
{
    switch(event->type)
    {
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
