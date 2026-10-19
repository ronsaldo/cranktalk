#include "crank-talk/message.h"
#include "crank-talk/array.h"
#include "crank-talk/behavior.h"
#include "crank-talk/context.h"
#include "crank-talk/dictionary.h"
#include "crank-talk/errors.h"
#include "crank-talk/function.h"
#include "crank-talk/string.h"
#include <stdlib.h>
#include <stdio.h>

CRANK_TALK_API void
crank_talk_Behavior_withSelectorSymbolSetMethod(crank_talk_VMContext_t *context, crank_talk_oop_t behaviorValue, crank_talk_oop_t selector, crank_talk_oop_t method)
{
    crank_talk_Behavior_t *behavior = (crank_talk_Behavior_t*)behaviorValue;
    if(!behavior->methodDictionary)
        behavior->methodDictionary = (crank_talk_MethodDictionary_t*)crank_talk_MethodDictionary_create(context);
    
    crank_talk_MethodDictionary_atPut(context, (crank_talk_oop_t)behavior->methodDictionary, selector, method);
}

CRANK_TALK_API void
crank_talk_Behavior_withSelectorSetMethod(crank_talk_VMContext_t *context, crank_talk_oop_t behaviorValue, const char *selector, crank_talk_oop_t method)
{
    crank_talk_Behavior_t *behavior = (crank_talk_Behavior_t*)behaviorValue;
    if(!behavior->methodDictionary)
        behavior->methodDictionary = (crank_talk_MethodDictionary_t*)crank_talk_MethodDictionary_create(context);
    
    crank_talk_oop_t selectorSymbol = crank_talk_Symbol_internWithCString(context, selector);
    crank_talk_MethodDictionary_atPut(context, (crank_talk_oop_t)behavior->methodDictionary, selectorSymbol, method);
}

CRANK_TALK_API void
crank_talk_createAndSetNativePrimitiveMethod(crank_talk_VMContext_t *context, crank_talk_oop_t behavior, const char *selector, size_t argumentCount, crank_talk_CompiledFunctionNativeEntryPoint_t entryPoint)
{
    crank_talk_oop_t name = crank_talk_Symbol_internWithCString(context, selector);
    crank_talk_oop_t compiledFunction = (crank_talk_oop_t)crank_talk_CompiledFunction_createForPrimitive(context, name, argumentCount, entryPoint);
    crank_talk_Behavior_withSelectorSetMethod(context, behavior, selector, compiledFunction);
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_sendWithClass(crank_talk_VMContext_t *context, crank_talk_oop_t receiverClass, crank_talk_oop_t selector, size_t argumentCount, crank_talk_oop_t *arguments)
{
    assert(argumentCount >= 1);
    crank_talk_oop_t receiver = arguments[0];
    crank_talk_oop_t foundMethod = crank_talk_Behavior_lookupSelector(receiverClass, selector);

    if(crank_talk_oop_isNil(foundMethod))
    {
        if(selector == context->roots.doesNotUnderstandSymbol)
        {
            assert(argumentCount == 2);
            crank_talk_Message_t *sentMessage = (crank_talk_Message_t *)arguments[1];
            if(crank_talk_oop_isKindOf(context, receiverClass, context->roots.ClassClass))
            {
                crank_talk_Class_t *clazz = (crank_talk_Class_t*)receiverClass;
                if(clazz->name)
                {
                    crank_talk_errorPrintf(context, "Message not understood #" CRANK_TALK_STRING_FORMAT " by " CRANK_TALK_STRING_FORMAT,
                        CRANK_TALK_STRING_FORMAT_ARG((crank_talk_oop_t)sentMessage->selector),
                        CRANK_TALK_STRING_FORMAT_ARG((crank_talk_oop_t)clazz->name)
                    );
                    abort();
                }
            }

            crank_talk_errorPrintf(context, "Message not understood #" CRANK_TALK_STRING_FORMAT, CRANK_TALK_STRING_FORMAT_ARG((crank_talk_oop_t)sentMessage->selector));
            abort();
        }

        // Create the message arguments
        crank_talk_oop_t messageArgumentsValue = crank_talk_Array_create(context, argumentCount - 1);
        crank_talk_oop_t *messageArguments = crank_talk_oop_getSlotsData(messageArgumentsValue);
        for(size_t i = 1; i < argumentCount; ++i)
            messageArguments[i - 1] = arguments[i];

        crank_talk_oop_t messageValue = CrankAllocateValuesObjectWithClassName(context, Message);
        crank_talk_Message_t *message = (crank_talk_Message_t *)messageValue;
        message->selector = (crank_talk_Symbol_t*)selector;
        message->arguments = (crank_talk_Array_t*) messageArgumentsValue;
        message->lookupClass = (crank_talk_Behavior_t*)receiverClass;

        return crank_talk_send1(context, context->roots.doesNotUnderstandSymbol, receiver, messageValue);
    }

    return crank_talk_apply(context, foundMethod, argumentCount, arguments);
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_superSend(crank_talk_VMContext_t *context, crank_talk_oop_t receiverClass, crank_talk_oop_t selector, size_t argumentCount, crank_talk_oop_t *arguments)
{
    if(!receiverClass)
        crank_talk_error(context, "Expected a receiver class.");

    crank_talk_Behavior_t *behavior = (crank_talk_Behavior_t *)receiverClass;
    if(!behavior->superclass)
        crank_talk_error(context, "Expected a behavior with a superclass.");

    return crank_talk_sendWithClass(context, (crank_talk_oop_t)behavior->superclass, selector, argumentCount, arguments);
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_send(crank_talk_VMContext_t *context, crank_talk_oop_t selector, size_t argumentCount, crank_talk_oop_t *arguments)
{
    assert(argumentCount >= 1);
    crank_talk_oop_t receiver = arguments[0];
    crank_talk_oop_t receiverClass = (crank_talk_oop_t)crank_talk_oop_getClass(context, receiver);
    return crank_talk_sendWithClass(context, receiverClass, selector, argumentCount, arguments);
}
