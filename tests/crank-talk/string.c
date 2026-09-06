#include "crank-talk/string.h"
#include "crank-talk/context.h"
#include "assertions.h"
#include <stdio.h>

void string_runTests(void)
{
    crank_talk_VMContext_t *context = crank_talk_vm_createContext();

    // Hello world string creation
    {
        crank_talk_oop_t helloWorld = crank_talk_String_createWithCString(context, "Hello World");
        Assert(crank_talk_String_equalsCString(helloWorld, "Hello World"));
    }

    // Symbol internation
    {
        crank_talk_oop_t first = crank_talk_Symbol_internWithCString(context, "testSymbol");
        Assert(crank_talk_Symbol_equalsCString(first, "testSymbol"));

        crank_talk_oop_t second = crank_talk_Symbol_internWithCString(context, "testSymbol");
        Assert(crank_talk_Symbol_equalsCString(second, "testSymbol"));

        AssertEquals(first, second);
    }

    // String stream
    {
        crank_talk_oop_t stringStream = crank_talk_StringStream_create(context);
        crank_talk_StringStream_insertCString(context, stringStream, "Hello World!!!");
        crank_talk_StringStream_insertCString(context, stringStream, "Wuff");
        crank_talk_oop_t string = crank_talk_StringStream_asString(context, stringStream);
        Assert(crank_talk_String_equalsCString(string, "Hello World!!!Wuff"));
    }

    // Hello world as string
    {
        crank_talk_oop_t helloWorldValue = crank_talk_String_createWithCString(context, "Hello World");
        crank_talk_oop_t helloWorldAsString = crank_talk_oop_asString(context, helloWorldValue);
        Assert(crank_talk_String_equalsCString(helloWorldAsString, "Hello World"));

        crank_talk_oop_t helloWorldPrintString = crank_talk_oop_printString(context, helloWorldValue);
        Assert(crank_talk_String_equalsCString(helloWorldPrintString, "'Hello World'"));
    }

    crank_talk_vm_destroyContext(context);
}
