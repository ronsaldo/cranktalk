#include "crank-talk/context.h"
#include "crank-talk/analysis-evaluation.h"
#include "crank-talk/errors.h"
#include "crank-talk/eval.h"
#include "crank-talk/parser.h"
#include "crank-talk/scanner.h"
#include "crank-talk/string.h"
#include <stdio.h>
#include <string.h>

static void
printVersion(void)
{
    printf("sysmel-vm version 0.1\n");
}

static void
printHelp(void)
{
    printf("sysmel-vm <options>\n");
}

static void
analyzeAndEvaluateAndPrintString(crank_talk_VMContext_t *context, const char *sourceString)
{
    crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, sourceString);
    crank_talk_oop_t printedValue = crank_talk_oop_printString(context, value);
    printf(CRANK_TALK_STRING_FORMAT"\n", CRANK_TALK_STRING_FORMAT_ARG(printedValue));
}


int main(int argc, const char *argv[])
{
    crank_talk_VMContext_t *context = crank_talk_vm_createContext();
    for(int i = 1; i < argc; ++i) {
        const char *arg = argv[i];
        if(*arg == '-')
        {
            if(!strcmp(arg, "-version"))
            {
                crank_talk_vm_destroyContext(context);
                printVersion();
                return 0;
            }
            else if(!strcmp(arg, "-help"))
            {
                crank_talk_vm_destroyContext(context);
                printHelp();
                return 0;
            }
            else if(!strcmp(arg, "-print-eval"))
            {
                arg = argv[++i];
                analyzeAndEvaluateAndPrintString(context, arg);
            }
        }
        else
        {
            crank_talk_analyzeAndEvaluateFileNamed(context,  crank_talk_String_createWithCString(context, arg));
        }
    }
    crank_talk_vm_destroyContext(context);
    return 0;
}