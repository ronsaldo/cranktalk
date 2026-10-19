#include "crank-talk/io.h"
#include "crank-talk/bytes.h"
#include "crank-talk/context.h"
#include "crank-talk/errors.h"
#include "crank-talk/message.h"
#include "crank-talk/string.h"
#include <stdio.h>

static void
crank_talk_Stdio_stdin_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    return crank_talk_Primitive_return(primContext, primContext->context->roots.stdinStream);
}

static void
crank_talk_Stdio_stdout_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    return crank_talk_Primitive_return(primContext, primContext->context->roots.stdoutStream);
}

static void
crank_talk_Stdio_stderr_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    return crank_talk_Primitive_return(primContext, primContext->context->roots.stderrStream);
}

CRANK_TALK_API crank_talk_ByteArray_t*
crank_talk_io_readWholeFileAsByteArray(crank_talk_VMContext_t *context, const char *fileName)
{
    FILE *file = fopen(fileName, "rb");
    if(!file)
        crank_talk_errorPrintf(context, "Failed to open file %s.", fileName);

    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    crank_talk_ByteArray_t *bytes = (crank_talk_ByteArray_t*)crank_talk_ByteArray_create(context, fileSize);
    if(fileSize > 0 && fread(bytes->elements, fileSize, 1, file) != 1)
    {
        fclose(file);
        crank_talk_errorPrintf(context, "Failed to read the content of file %s.", fileName);
    }

    fclose(file);
    return bytes;
}

static void
crank_talk_BinaryFileStream_nextPut(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t character = primContext->arguments[1];
    if(!crank_talk_oop_isCharacter(character) && !crank_talk_oop_isSmallInteger(character))
        crank_talk_error(primContext->context, "Expected a character or a small integer for nextPut:");

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
    
    
    return crank_talk_Primitive_return(primContext, self);
}

static void
crank_talk_BinaryFileStream_nextPutAll(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t bytesObject = primContext->arguments[1];
    
    crank_talk_BinaryFileStream_t *stream = (crank_talk_BinaryFileStream_t *)self;
    FILE *file = ((crank_talk_ExternalAddress_t*)stream->handle)->address;

    size_t bytesSize = crank_talk_object_getSizeInBytes(bytesObject);
    uint8_t *bytesData = crank_talk_oop_getBytesData(bytesObject);
    if(bytesSize > 0 && fwrite(bytesData, bytesSize, 1, file) != 1)
        crank_talk_error(primContext->context, "Failed to write in file.");

    return crank_talk_Primitive_return(primContext, self);
}

static void
crank_talk_BinaryFileStream_print(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t printedObject = primContext->arguments[1];

    crank_talk_oop_t bytesObject = crank_talk_oop_printString(primContext->context, printedObject);
    
    crank_talk_BinaryFileStream_t *stream = (crank_talk_BinaryFileStream_t *)self;
    FILE *file = ((crank_talk_ExternalAddress_t*)stream->handle)->address;

    size_t bytesSize = crank_talk_object_getSizeInBytes(bytesObject);
    uint8_t *bytesData = crank_talk_oop_getBytesData(bytesObject);
    if(bytesSize > 0 && fwrite(bytesData, bytesSize, 1, file) != 1)
        crank_talk_error(primContext->context, "Failed to write in file.");

    return crank_talk_Primitive_return(primContext, self);
}

static void
crank_talk_BinaryFileStream_cr(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t self = primContext->arguments[0];
    
    crank_talk_BinaryFileStream_t *stream = (crank_talk_BinaryFileStream_t *)self;
    FILE *file = ((crank_talk_ExternalAddress_t*)stream->handle)->address;
    fputc('\r', file);

    return crank_talk_Primitive_return(primContext, self);
}

static void
crank_talk_BinaryFileStream_lf(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t self = primContext->arguments[0];
    
    crank_talk_BinaryFileStream_t *stream = (crank_talk_BinaryFileStream_t *)self;
    FILE *file = ((crank_talk_ExternalAddress_t*)stream->handle)->address;
    fputc('\n', file);

    return crank_talk_Primitive_return(primContext, self);
}

static void
crank_talk_BinaryFileStream_space(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t self = primContext->arguments[0];
    
    crank_talk_BinaryFileStream_t *stream = (crank_talk_BinaryFileStream_t *)self;
    FILE *file = ((crank_talk_ExternalAddress_t*)stream->handle)->address;
    fputc(' ', file);

    return crank_talk_Primitive_return(primContext, self);
}

void
crank_talk_registerIOPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetPrimitiveMethod(context, (crank_talk_oop_t)crank_talk_oop_getClass(context, context->roots.StdioClass), "stdin", 1, "Stdio::stdin", crank_talk_Stdio_stdin_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, (crank_talk_oop_t)crank_talk_oop_getClass(context, context->roots.StdioClass), "stdout", 1, "Stdio::stdour", crank_talk_Stdio_stdout_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, (crank_talk_oop_t)crank_talk_oop_getClass(context, context->roots.StdioClass), "stderr", 1, "Stdio::stderr", crank_talk_Stdio_stderr_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BinaryFileStreamClass, "nextPut:", 2, "BinaryFileStream::nextPut:", crank_talk_BinaryFileStream_nextPut);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BinaryFileStreamClass, "nextPutAll:", 2, "BinaryFileStream::nextPutAll:", crank_talk_BinaryFileStream_nextPutAll);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BinaryFileStreamClass, "print:", 2, "BinaryFileStream::print:", crank_talk_BinaryFileStream_print);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BinaryFileStreamClass, "cr", 1, "BinaryFileStream::cr", crank_talk_BinaryFileStream_cr);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BinaryFileStreamClass, "lf", 1, "BinaryFileStream::lf", crank_talk_BinaryFileStream_lf);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BinaryFileStreamClass, "space", 1, "BinaryFileStream::space", crank_talk_BinaryFileStream_space);
}