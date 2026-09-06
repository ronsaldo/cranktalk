#ifndef CRANK_TALK_SCANNER_H
#define CRANK_TALK_SCANNER_H

#include "object-model.h"

typedef enum crank_talk_TokenKind_e {
#define TokenKind(name) CrankTalkTokenKind##name,
#include "token-kind.inc"
#undef TokenKind
} crank_talk_TokenKind_t;

CRANK_TALK_API const char *crank_talk_TokenKind_toString(crank_talk_TokenKind_t tokenKind);

CRANK_TALK_API crank_talk_oop_t crank_talk_CrankTalk_scanSourceCode(crank_talk_VMContext_t *context, crank_talk_oop_t sourceCodeValue);


#endif //CRANK_TALK_SCANNER_H
