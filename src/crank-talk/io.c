#include "crank-talk/io.h"
#include "crank-talk/context.h"
#include "crank-talk/errors.h"
#include "crank-talk/message.h"
#include "crank-talk/string.h"
#include <stdio.h>

static crank_talk_oop_t
crank_talk_Stdio_stdin_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    (void)arguments;
    assert(argumentCount > 0);
    return context->roots.stdinStream;
}

static crank_talk_oop_t
crank_talk_Stdio_stdout_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    (void)arguments;
    assert(argumentCount > 0);
    return context->roots.stdoutStream;
}

static crank_talk_oop_t
crank_talk_Stdio_stderr_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    (void)arguments;
    assert(argumentCount > 0);
    return context->roots.stderrStream;
}

static crank_talk_oop_t
crank_talk_BinaryFileStream_nextPut(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    (void)arguments;
    assert(argumentCount >= 2);
    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t character = arguments[1];
    if(!crank_talk_oop_isCharacter(character) && !crank_talk_oop_isSmallInteger(character))
        crank_talk_error(context, "Expected a character or a small integer for nextPut:");

    crank_talk_BinaryFileStream_t *stream = (crank_talk_BinaryFileStream_t *)self;
    FILE *file = ((crank_talk_ExternalAddress_t*)stream->handle)->address;

    if (crank_talk_oop_isCharacter(character))
    {
        // TODO: UTF-8 encode the character.
        fputc(crank_talk_oop_decodeCharacter(character)& 0xff, file);
    }
    else
    {
        fputc(crank_talk_oop_decodeSmallInteger(character)& 0xff, file);
    }
    
    
    return self;
}

static crank_talk_oop_t
crank_talk_BinaryFileStream_nextPutAll(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    (void)arguments;
    assert(argumentCount >= 2);
    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t bytesObject = arguments[1];
    
    crank_talk_BinaryFileStream_t *stream = (crank_talk_BinaryFileStream_t *)self;
    FILE *file = ((crank_talk_ExternalAddress_t*)stream->handle)->address;

    size_t bytesSize = crank_talk_object_getSizeInBytes(bytesObject);
    uint8_t *bytesData = crank_talk_oop_getBytesData(bytesObject);
    if(fwrite(bytesData, bytesSize, 1, file) != 1)
        crank_talk_error(context, "Failed to write in file.");

    return self;
}

static crank_talk_oop_t
crank_talk_BinaryFileStream_print(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    (void)arguments;
    assert(argumentCount >= 2);
    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t printedObject = arguments[1];

    crank_talk_oop_t bytesObject = crank_talk_oop_printString(context, printedObject);
    
    crank_talk_BinaryFileStream_t *stream = (crank_talk_BinaryFileStream_t *)self;
    FILE *file = ((crank_talk_ExternalAddress_t*)stream->handle)->address;

    size_t bytesSize = crank_talk_object_getSizeInBytes(bytesObject);
    uint8_t *bytesData = crank_talk_oop_getBytesData(bytesObject);
    if(fwrite(bytesData, bytesSize, 1, file) != 1)
        crank_talk_error(context, "Failed to write in file.");

    return self;
}

static crank_talk_oop_t
crank_talk_BinaryFileStream_cr(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    (void)arguments;
    assert(argumentCount >= 1);
    crank_talk_oop_t self = arguments[0];
    
    crank_talk_BinaryFileStream_t *stream = (crank_talk_BinaryFileStream_t *)self;
    FILE *file = ((crank_talk_ExternalAddress_t*)stream->handle)->address;
    fputc('\r', file);

    return self;
}

static crank_talk_oop_t
crank_talk_BinaryFileStream_lf(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    (void)arguments;
    assert(argumentCount >= 1);
    crank_talk_oop_t self = arguments[0];
    
    crank_talk_BinaryFileStream_t *stream = (crank_talk_BinaryFileStream_t *)self;
    FILE *file = ((crank_talk_ExternalAddress_t*)stream->handle)->address;
    fputc('\n', file);

    return self;
}

void
crank_talk_registerIOPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetPrimitiveMethod(context, (crank_talk_oop_t)crank_talk_oop_getClass(context, context->roots.StdioClass), "stdin", 1, crank_talk_Stdio_stdin_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, (crank_talk_oop_t)crank_talk_oop_getClass(context, context->roots.StdioClass), "stdout", 1, crank_talk_Stdio_stdout_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, (crank_talk_oop_t)crank_talk_oop_getClass(context, context->roots.StdioClass), "stderr", 1, crank_talk_Stdio_stderr_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BinaryFileStreamClass, "nextPut:", 2, crank_talk_BinaryFileStream_nextPut);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BinaryFileStreamClass, "nextPutAll:", 2, crank_talk_BinaryFileStream_nextPutAll);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BinaryFileStreamClass, "print:", 2, crank_talk_BinaryFileStream_print);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BinaryFileStreamClass, "cr", 1, crank_talk_BinaryFileStream_cr);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BinaryFileStreamClass, "lf", 1, crank_talk_BinaryFileStream_lf);
}