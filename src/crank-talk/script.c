#include "crank-talk/eval.h"
#include "crank-talk/context.h"
#include "crank-talk/environment.h"
#include "crank-talk/errors.h"
#include "crank-talk/message.h"
#include "crank-talk/string.h"
#include <stdlib.h>
#include <stdio.h>

static void
crank_talk_ScriptContext_loadFile_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_ScriptContext_t *scriptContext = (crank_talk_ScriptContext_t *)primContext->arguments[0];
    crank_talk_oop_t path = primContext->arguments[1];

    if(!crank_talk_oop_isKindOf(primContext->context, path, primContext->context->roots.StringClass))
        crank_talk_error(primContext->context, "Expected a string with path for loading the script.");

    // Find the script directory.
    crank_talk_oop_t directoryBinding = crank_talk_Environment_lookupSymbolRecursively(primContext->context, scriptContext->lexicalEnvironment, crank_talk_Symbol_internWithCString(primContext->context, "__FileDir__"));
    crank_talk_oop_t fullpath = path;
    if(directoryBinding)
    {
        assert(crank_talk_oop_isKindOf(primContext->context, directoryBinding, primContext->context->roots.LiteralVariableClass));
        crank_talk_oop_t directoryPath = ((crank_talk_LiteralVariable_t *)directoryBinding)->value;
        fullpath = crank_talk_String_joinPath(primContext->context, directoryPath, path);
    }

    crank_talk_oop_t result = crank_talk_analyzeAndEvaluateFileNamed(primContext->context, fullpath);
    return crank_talk_Primitive_return(primContext, result);
}

void
crank_talk_registerScriptPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ScriptContextClass, "loadFile:", 2, "ScriptContext::loadFile:", crank_talk_ScriptContext_loadFile_primitive);
}
