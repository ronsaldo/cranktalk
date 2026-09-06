#include "crank-talk/message.h"
#include "crank-talk/behavior.h"
#include "crank-talk/errors.h"

CRANK_TALK_API crank_talk_oop_t
sysmel_vm_send(crank_talk_VMContext_t *context, crank_talk_oop_t selector, size_t argumentCount, crank_talk_oop_t *arguments)
{
    assert(argumentCount >= 1);
    crank_talk_oop_t receiver = arguments[0];
    crank_talk_oop_t receiverClass = (crank_talk_oop_t)crank_talk_oop_getClass(context, receiver);
    crank_talk_oop_t foundMethod = crank_talk_Behavior_lookupSelector(receiverClass, selector);

    if(crank_talk_oop_isNil(foundMethod))
    {
        /*if(sysmel_vm_Value_equals(selector, context->roots.doesNotUnderstandSymbol))
        {
            assert(argumentCount == 2);
            sysmel_vm_Message_t *sentMessage = (sysmel_vm_Message_t *)arguments[1].value.gc;
            if(sysmel_vm_Value_isKindOf(context, receiverType, context->roots.NominalTypeClass))
            {
                sysmel_vm_NominalType_t *nominalType = (sysmel_vm_NominalType_t*)receiverType.value.gc;
                if(!sysmel_vm_Value_isNil(nominalType->name))
                {
                    fprintf(stderr, "Message not understood #" SYSMEL_VM_STRING_FORMAT " by " SYSMEL_VM_STRING_FORMAT "\n",
                        SYSMEL_VM_STRING_FORMAT_ARG(sentMessage->selector),
                        SYSMEL_VM_STRING_FORMAT_ARG(nominalType->name)
                    );
                    abort();
                }
            }

            fprintf(stderr, "Message not understood #" SYSMEL_VM_STRING_FORMAT "\n", SYSMEL_VM_STRING_FORMAT_ARG(sentMessage->selector));
            abort();
        }

        // Create the message arguments
        crank_talk_oop_t messageArgumentsValue = sysmel_vm_Array_create(context, argumentCount - 1);
        crank_talk_oop_t *messageArguments = sysmel_vm_Value_getValuesData(messageArgumentsValue);
        for(size_t i = 1; i < argumentCount; ++i)
            messageArguments[i - 1] = arguments[i];

        crank_talk_oop_t messageValue = SysmelAllocateValuesObjectWithClassName(context, Message);
        sysmel_vm_Message_t *message = (sysmel_vm_Message_t *)messageValue.value.gc;
        message->selector = selector;
        message->arguments = messageArgumentsValue;
        message->lookupClass = receiverType;

        return sysmel_vm_send1(context, context->roots.doesNotUnderstandSymbol, receiver, messageValue);
        */
        crank_talk_error("TODO: nil found method.");
    }

    return crank_talk_apply(context, foundMethod, argumentCount, arguments);
}