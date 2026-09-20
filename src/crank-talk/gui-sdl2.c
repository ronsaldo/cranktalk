#include "crank-talk/gui.h"
#include "crank-talk/array.h"
#include "crank-talk/bytes.h"
#include "crank-talk/context.h"
#include "crank-talk/errors.h"
#include "crank-talk/message.h"
#include "crank-talk/string.h"
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

static void sdl2_dispatchEvent(crank_talk_VMContext_t *context, uint32_t windowID, crank_talk_oop_t event)
{
    SDL_Window *window = SDL_GetWindowFromID(windowID);
    if(!window)
        return;

    crank_talk_OSWindow_t *osWindow = SDL_GetWindowData(window, "OSWindow");
    if(!osWindow || !osWindow->eventHandler)
        return;

    crank_talk_OSEvent_t *osEvent = (crank_talk_OSEvent_t *)event;
    osEvent->window = osWindow;
    crank_talk_send1(context, context->roots.processEventSymbol, (crank_talk_oop_t)osWindow->eventHandler, event);
}

static void
sdl2_onTextInput(crank_talk_VMContext_t *context, SDL_TextInputEvent *event)
{
    crank_talk_oop_t textInputEventValue = CrankAllocateValuesObjectWithClassName(context, OSTextInputEvent);
    crank_talk_OSTextInputEvent_t *textEvent = (crank_talk_OSTextInputEvent_t *)textInputEventValue;
    textEvent->text = crank_talk_String_createWithCString(context, event->text);
    textEvent->modifiers = crank_talk_oop_encodeSmallInteger(SDL_GetModState());
    sdl2_dispatchEvent(context, event->windowID, textInputEventValue);
}

static void
sdl2_onKeyDown(crank_talk_VMContext_t *context, SDL_KeyboardEvent *event)
{
    crank_talk_oop_t keyEventValue = CrankAllocateValuesObjectWithClassName(context, OSKeyDownEvent);
    crank_talk_OSKeyDownEvent_t *keyEvent = (crank_talk_OSKeyDownEvent_t *)keyEventValue;
    keyEvent->super.scancode = crank_talk_oop_encodeSmallInteger(event->keysym.scancode);
    keyEvent->super.symbol = crank_talk_oop_encodeSmallInteger(event->keysym.sym);
    keyEvent->super.modifiers = crank_talk_oop_encodeSmallInteger(event->keysym.mod);
    keyEvent->super.repeat = crank_talk_oop_encodeSmallInteger(event->repeat);
    sdl2_dispatchEvent(context, event->windowID, keyEventValue);
}

static void
sdl2_onKeyUp(crank_talk_VMContext_t *context, SDL_KeyboardEvent *event)
{
    crank_talk_oop_t keyEventValue = CrankAllocateValuesObjectWithClassName(context, OSKeyUpEvent);
    crank_talk_OSKeyUpEvent_t *keyEvent = (crank_talk_OSKeyUpEvent_t *)keyEventValue;
    keyEvent->super.scancode = crank_talk_oop_encodeSmallInteger(event->keysym.scancode);
    keyEvent->super.symbol = crank_talk_oop_encodeSmallInteger(event->keysym.sym);
    keyEvent->super.modifiers = crank_talk_oop_encodeSmallInteger(event->keysym.mod);
    keyEvent->super.repeat = crank_talk_oop_encodeSmallInteger(event->repeat);
    sdl2_dispatchEvent(context, event->windowID, keyEventValue);
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
    case SDL_KEYDOWN:
        sdl2_onKeyDown(context, &event->key);
        break;
    case SDL_KEYUP:
        sdl2_onKeyUp(context, &event->key);
        break;
    case SDL_TEXTINPUT:
        sdl2_onTextInput(context, &event->text);
        break;
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

    SDL_Window *handle = SDL_CreateWindow("CrankTalk Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
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

static crank_talk_oop_t
crank_talk_OSWindow_newFormRenderer_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    crank_talk_oop_t self = arguments[0];
    crank_talk_OSWindow_t *osWindow = (crank_talk_OSWindow_t *)self;
    if(!osWindow->handle || !osWindow->handle->address)
        crank_talk_error(context, "Expected an opened window for creating a form renderer.");
    if(osWindow->renderer)
        crank_talk_error(context, "Window already has an associated renderer.");

    SDL_Window *windowHandle = osWindow->handle->address;
    SDL_Renderer *renderer = SDL_CreateRenderer(windowHandle, -1, 0);

    crank_talk_OSWindowFormRenderer_t *formRenderer = (crank_talk_OSWindowFormRenderer_t*)CrankAllocateValuesObjectWithClassName(context, OSWindowFormRenderer);
    formRenderer->super.rendererHandle = (crank_talk_ExternalAddress_t*)crank_talk_ExternalAddress_make(context, renderer);
    osWindow->renderer = &formRenderer->super;

    return (crank_talk_oop_t)formRenderer;
}

static crank_talk_oop_t
crank_talk_OSWindowRenderer_beginFrame_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    crank_talk_oop_t self = arguments[0];
    crank_talk_OSWindowRenderer_t *osWindowRenderer = (crank_talk_OSWindowRenderer_t *)self;
    if(!osWindowRenderer->rendererHandle)
        crank_talk_error(context, "Expected a valid window renderer.");

    SDL_Renderer *renderer = osWindowRenderer->rendererHandle->address;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    return self;
}

static crank_talk_oop_t
crank_talk_OSWindowRenderer_endFrame_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    crank_talk_oop_t self = arguments[0];
    crank_talk_OSWindowRenderer_t *osWindowRenderer = (crank_talk_OSWindowRenderer_t *)self;
    if(!osWindowRenderer->rendererHandle)
        crank_talk_error(context, "Expected a valid window renderer.");

    SDL_Renderer *renderer = osWindowRenderer->rendererHandle->address;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderPresent(renderer);
    return self;
}

static crank_talk_oop_t
crank_talk_OSWindowFormRenderer_beginFrame_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    crank_talk_oop_t self = arguments[0];
    crank_talk_OSWindowFormRenderer_t *osWindowRenderer = (crank_talk_OSWindowFormRenderer_t *)self;
    if(!osWindowRenderer->super.rendererHandle)
        crank_talk_error(context, "Expected a valid window renderer.");

    SDL_Renderer *renderer = osWindowRenderer->super.rendererHandle->address;
    int rendererWidth = 0;
    int rendererHeight = 0;
    SDL_GetRendererOutputSize(renderer, &rendererWidth, &rendererHeight);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    if(!osWindowRenderer->form
        || crank_talk_oop_decodeSmallInteger(osWindowRenderer->form->width) != rendererWidth
        || crank_talk_oop_decodeSmallInteger(osWindowRenderer->form->height) != rendererHeight)
    {
        crank_talk_Form_t *newForm = (crank_talk_Form_t *)CrankAllocateValuesObjectWithClassName(context, Form);
        newForm->width = crank_talk_oop_encodeSmallInteger(rendererWidth);
        newForm->height = crank_talk_oop_encodeSmallInteger(rendererHeight);
        newForm->depth = crank_talk_oop_encodeSmallInteger(32);
        newForm->bits = (crank_talk_ByteArray_t*)crank_talk_ByteArray_create(context, rendererWidth*4*rendererHeight);
        osWindowRenderer->form = newForm;

        // Destroy the old texture
        if(osWindowRenderer->textureHandle)
        {
            SDL_DestroyTexture(osWindowRenderer->textureHandle->address);
            osWindowRenderer->textureHandle = NULL;
        }

        // Create a new texture
        SDL_Texture *newTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, rendererWidth, rendererHeight);
        osWindowRenderer->textureHandle = (crank_talk_ExternalAddress_t*)crank_talk_ExternalAddress_make(context, newTexture);
    }
    return self;
}

static crank_talk_oop_t
crank_talk_OSWindowFormRenderer_blitForm_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    crank_talk_oop_t self = arguments[0];
    crank_talk_OSWindowFormRenderer_t *osWindowRenderer = (crank_talk_OSWindowFormRenderer_t *)self;
    if(!osWindowRenderer->super.rendererHandle || !osWindowRenderer->textureHandle)
        crank_talk_error(context, "Expected a valid window renderer.");

    crank_talk_Form_t *form = osWindowRenderer->form;
    SDL_Renderer *renderer = osWindowRenderer->super.rendererHandle->address;
    SDL_Texture *texture = osWindowRenderer->textureHandle->address;
    SDL_UpdateTexture(texture, NULL, crank_talk_oop_getBytesData((crank_talk_oop_t)form->bits), crank_talk_oop_decodeSmallInteger(form->width)*4);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    return self;
}

void
crank_talk_registerGUIBackendPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetPrimitiveMethod(context, (crank_talk_oop_t)crank_talk_oop_getClass(context, context->roots.OSWindowClass), "mainLoop", 1, crank_talk_OSWindow_mainLoop_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.OSWindowClass, "open", 1, crank_talk_OSWindow_open_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.OSWindowClass, "newFormRenderer", 1, crank_talk_OSWindow_newFormRenderer_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.OSWindowRendererClass, "beginFrame", 1, crank_talk_OSWindowRenderer_beginFrame_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.OSWindowRendererClass, "endFrame", 1, crank_talk_OSWindowRenderer_endFrame_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.OSWindowFormRendererClass, "beginFrame", 1, crank_talk_OSWindowFormRenderer_beginFrame_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.OSWindowFormRendererClass, "blitForm", 1, crank_talk_OSWindowFormRenderer_blitForm_primitive);
}
