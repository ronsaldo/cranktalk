#ifndef CRANK_TALK_STRING_H
#define CRANK_TALK_STRING_H

#include "object-model.h"
#include <stddef.h>

#define CRANK_TALK_STRING_FORMAT "%.*s"
#define CRANK_TALK_STRING_FORMAT_ARG(x) (int)crank_talk_Value_getSizeInBytes(x), (const char*)crank_talk_Value_getBytesData(x)

CRANK_TALK_API size_t crank_talk_String_computeHashWithBytes(size_t size, const uint8_t *bytes);

CRANK_TALK_API crank_talk_oop_t crank_talk_String_allocateWithSize(crank_talk_VMContext_t *context, size_t stringSize);

CRANK_TALK_API crank_talk_oop_t crank_talk_String_createWithString(crank_talk_VMContext_t *context, size_t stringSize, const char *string);
CRANK_TALK_API crank_talk_oop_t crank_talk_String_createWithCString(crank_talk_VMContext_t *context, const char *cstring);
CRANK_TALK_API bool crank_talk_String_equalsCString(crank_talk_oop_t left, const char *right);

CRANK_TALK_API crank_talk_oop_t crank_talk_String_concat(crank_talk_VMContext_t *context, crank_talk_oop_t leftString,  crank_talk_oop_t rightString);
CRANK_TALK_API crank_talk_oop_t crank_talk_String_substring(crank_talk_VMContext_t *context, crank_talk_oop_t string, size_t startIndex, size_t count);
CRANK_TALK_API crank_talk_oop_t crank_talk_String_dirname(crank_talk_VMContext_t *context, crank_talk_oop_t path);
CRANK_TALK_API crank_talk_oop_t crank_talk_String_basename(crank_talk_VMContext_t *context, crank_talk_oop_t path);
CRANK_TALK_API crank_talk_oop_t crank_talk_String_joinPath(crank_talk_VMContext_t *context, crank_talk_oop_t leftPath, crank_talk_oop_t rightPath);

CRANK_TALK_API crank_talk_oop_t crank_talk_Symbol_internWithString(crank_talk_VMContext_t *context, size_t stringSize, const char *string);
CRANK_TALK_API crank_talk_oop_t crank_talk_Symbol_internWithCString(crank_talk_VMContext_t *context, const char *cstring);
CRANK_TALK_API bool crank_talk_Symbol_equalsCString(crank_talk_oop_t left, const char *right);

// String stream
CRANK_TALK_API crank_talk_oop_t crank_talk_StringStream_create(crank_talk_VMContext_t *context);
CRANK_TALK_API void crank_talk_StringStream_insertCharacter(crank_talk_VMContext_t *context, crank_talk_oop_t stream, char character);
CRANK_TALK_API void crank_talk_StringStream_insertString(crank_talk_VMContext_t *context, crank_talk_oop_t stream, size_t stringSize, const char *stringData);
CRANK_TALK_API void crank_talk_StringStream_insertCString(crank_talk_VMContext_t *context, crank_talk_oop_t stream, const char *string);
CRANK_TALK_API void crank_talk_StringStream_insertBytesObject(crank_talk_VMContext_t *context, crank_talk_oop_t stream, crank_talk_oop_t object);

CRANK_TALK_API void crank_talk_StringStream_print(crank_talk_VMContext_t *context, crank_talk_oop_t stream, crank_talk_oop_t value);
CRANK_TALK_API void crank_talk_StringStream_fullPrint(crank_talk_VMContext_t *context, crank_talk_oop_t stream, crank_talk_oop_t value);

CRANK_TALK_API crank_talk_oop_t crank_talk_StringStream_asString(crank_talk_VMContext_t *context, crank_talk_oop_t stream);
CRANK_TALK_API crank_talk_oop_t crank_talk_StringStream_asSymbol(crank_talk_VMContext_t *context, crank_talk_oop_t stream);

// String messages
CRANK_TALK_API crank_talk_oop_t crank_talk_oop_asString(crank_talk_VMContext_t *context, crank_talk_oop_t value);
CRANK_TALK_API crank_talk_oop_t crank_talk_oop_printString(crank_talk_VMContext_t *context, crank_talk_oop_t value);

#endif //CRANK_TALK_STRING_H
