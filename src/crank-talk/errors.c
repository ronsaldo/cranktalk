#include "crank-talk/errors.h"
#include "crank-talk/context.h"
#include "crank-talk/string.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

CRANK_TALK_API void
crank_talk_error(const char *message)
{
    fprintf(stderr, "Error: %s\n", message);
    abort();
}

/*
SYSMEL_VM_API void
sysmel_vm_errorPrintf(sysmel_VMContext_t *context, const char *format, ...)
{
    (void)context;
    char buffer[256];

    va_list valist;
    va_start(valist, format);
    vsnprintf(buffer, sizeof(buffer), format, valist);
    va_end(valist);

    sysmel_vm_error(buffer);
}

SYSMEL_VM_API void
sysmel_vm_errorAtSourcePosition(sysmel_VMContext_t *context, sysmel_vm_Value_t abstractSourcePosition, const char *message)
{
    if(sysmel_vm_Value_isKindOf(context, abstractSourcePosition, context->roots.SourcePositionClass))
    {
        sysmel_vm_SourcePosition_t *sourcePosition = (sysmel_vm_SourcePosition_t*)abstractSourcePosition.value.gc;
        sysmel_vm_SourceCode_t *sourceCode = (sysmel_vm_SourceCode_t *)sourcePosition->sourceCode.value.gc;
        fprintf(stderr, SYSMEL_VM_STRING_FORMAT "/" SYSMEL_VM_STRING_FORMAT ":%d.%d-%d.%d: ",
            SYSMEL_VM_STRING_FORMAT_ARG(sourceCode->directory),
            SYSMEL_VM_STRING_FORMAT_ARG(sourceCode->name),
            (int)sysmel_vm_Value_getSize(sourcePosition->startLine), (int)sysmel_vm_Value_getSize(sourcePosition->startColumn),
            (int)sysmel_vm_Value_getSize(sourcePosition->endLine), (int)sysmel_vm_Value_getSize(sourcePosition->endColumn));

    }

    fprintf(stderr, "Error: %s\n", message);
    abort();
}

SYSMEL_VM_API void
sysmel_vm_errorPrintfAtSourcePosition(sysmel_VMContext_t *context, sysmel_vm_Value_t sourcePosition, const char *format, ...)
{
    char buffer[256];

    va_list valist;
    va_start(valist, format);
    vsnprintf(buffer, sizeof(buffer), format, valist);
    va_end(valist);

    sysmel_vm_errorAtSourcePosition(context, sourcePosition, buffer);
}

*/