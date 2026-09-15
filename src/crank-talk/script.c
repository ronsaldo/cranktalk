#include "crank-talk/eval.h"
#include "crank-talk/context.h"
#include "crank-talk/environment.h"
#include "crank-talk/errors.h"
#include "crank-talk/message.h"
#include "crank-talk/string.h"
#include <stdlib.h>
#include <stdio.h>

static crank_talk_oop_t
crank_talk_ScriptContext_loadFile_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);

    crank_talk_ScriptContext_t *scriptContext = (crank_talk_ScriptContext_t *)arguments[0];
    crank_talk_oop_t path = arguments[1];

    if(!crank_talk_oop_isKindOf(context, path, context->roots.StringClass))
        crank_talk_error("Expected a string with path for loading the script.");

    // Find the script directory.
    crank_talk_oop_t directoryBinding = crank_talk_Environment_lookupSymbolRecursively(context, scriptContext->lexicalEnvironment, crank_talk_Symbol_internWithCString(context, "__FileDir__"));
    crank_talk_oop_t fullpath = path;
    if(directoryBinding)
    {
        assert(crank_talk_oop_isKindOf(context, directoryBinding, context->roots.LiteralVariableClass));
        crank_talk_oop_t directoryPath = ((crank_talk_LiteralVariable_t *)directoryBinding)->value;
        fullpath = crank_talk_String_joinPath(context, directoryPath, path);
    }

    return crank_talk_analyzeAndEvaluateFileNamed(context, fullpath);
}

void
crank_talk_registerScriptPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ScriptContextClass, "loadFile:", 1, crank_talk_ScriptContext_loadFile_primitive);
}
