#define _XOPEN_SOURCE 500 // For realpath

#include "crank-talk/eval.h"
//#include "crank-talk/analysis-evaluation.h"
#include "crank-talk/context.h"
#include "crank-talk/errors.h"
#include "crank-talk/parser.h"
#include "crank-talk/scanner.h"
#include "crank-talk/string.h"

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

CRANK_TALK_API crank_talk_oop_t
crank_talk_analyzeAndEvaluateSourceCode(crank_talk_VMContext_t *context, crank_talk_oop_t sourceCodeValue)
{
    crank_talk_oop_t tokensValue = crank_talk_CrankTalk_scanSourceCode(context, sourceCodeValue);
    crank_talk_oop_t ast = crank_talk_parseScannedTokens(context, tokensValue);
    if(!crank_talk_ParseTree_checkAndPrintErrors(context, (crank_talk_ParseTreeNode_t*)ast))
        crank_talk_error("AST has parse errors.");
//
    //crank_talk_oop_t evaluationContext = crank_talk_createTopLevelAnalysisAndEvaluationContext(context, sourceCodeValue);
    //return crank_talk_ParseTreeAnalyzeAndEvaluateDecayedNode(context, ast, evaluationContext);
    abort();
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_analyzeAndEvaluateString(crank_talk_VMContext_t *context, const char *sourceString)
{
    crank_talk_oop_t sourceCodeValue = CrankAllocateValuesObjectWithClassName(context, SourceCode);
    crank_talk_SourceCode_t *sourceCode = (crank_talk_SourceCode_t *)sourceCodeValue;
    sourceCode->text = (crank_talk_String_t*)crank_talk_String_createWithCString(context, sourceString);
    sourceCode->name = (crank_talk_String_t*)crank_talk_String_createWithCString(context, "<cli>");

    return crank_talk_analyzeAndEvaluateSourceCode(context, sourceCodeValue);
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_analyzeAndEvaluateFileNamed(crank_talk_VMContext_t *context, crank_talk_oop_t fileNameString)
{
    size_t fileNameStringSize = crank_talk_object_getSizeInBytes(fileNameString);
    char* fileName = malloc(fileNameStringSize + 1);
    memcpy(fileName, crank_talk_oop_getBytesData(fileNameString), fileNameStringSize);
    fileName[fileNameStringSize] = 0;

    // Compute the full path.
    // TODO: Implement this for Windows.
    char *absolutePath = realpath(fileName, NULL);

    // Open the file
    crank_talk_oop_t absolutePathString = crank_talk_String_createWithCString(context, absolutePath);
    FILE *file = fopen(absolutePath, "rb");
    free(absolutePath);
    free(fileName);
    if(!file)
        crank_talk_errorPrintf(context, "Failed to open file "CRANK_TALK_STRING_FORMAT"\n", CRANK_TALK_STRING_FORMAT_ARG(absolutePathString));

    // Fetch the file size
    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Read the file.
    crank_talk_oop_t sourceCodeText = crank_talk_String_allocateWithSize(context, fileSize);
    if(fread(crank_talk_oop_getBytesData(sourceCodeText), fileSize, 1, file) != 1)
    {
        fclose(file);
        crank_talk_errorPrintf(context, "Failed to read the content offile "CRANK_TALK_STRING_FORMAT"\n", CRANK_TALK_STRING_FORMAT_ARG(absolutePathString));
    }
    fclose(file);
    
    crank_talk_oop_t dirname = crank_talk_String_dirname(context, absolutePathString);
    crank_talk_oop_t basename = crank_talk_String_basename(context, absolutePathString);
    //printf("absolute path "CRANK_TALK_STRING_FORMAT"\n", CRANK_TALK_STRING_FORMAT_ARG(absolutePathString));
    //printf("dirname "CRANK_TALK_STRING_FORMAT"\n", CRANK_TALK_STRING_FORMAT_ARG(dirname));
    //printf("basename "CRANK_TALK_STRING_FORMAT"\n", CRANK_TALK_STRING_FORMAT_ARG(basename));
    
    crank_talk_oop_t sourceCodeValue = CrankAllocateValuesObjectWithClassName(context, SourceCode);
    crank_talk_SourceCode_t *sourceCode = (crank_talk_SourceCode_t *)sourceCodeValue;
    sourceCode->text = (crank_talk_String_t*)sourceCodeText;
    sourceCode->directory = (crank_talk_String_t*)dirname;
    sourceCode->name = (crank_talk_String_t*)basename;

    return crank_talk_analyzeAndEvaluateSourceCode(context, sourceCodeValue);
}