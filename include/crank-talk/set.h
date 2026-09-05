#ifndef CRANK_TALK_SET_H
#define CRANK_TALK_SET_H

#include "object-model.h"

typedef size_t (*crank_talk_identitySet_explicitHashFunction_t)(crank_talk_VMContext_t *context, void *element);
typedef bool (*crank_talk_identitySet_explicitEqualsFunction_t)(void *element, crank_talk_oop_t setElement);

/**
 * Creates a set.
 */
CRANK_TALK_API crank_talk_oop_t crank_talk_IdentitySet_create(crank_talk_VMContext_t *context);

/**
 * Finds an element in the set
 */
CRANK_TALK_API bool crank_talk_IdentitySet_findWithExplicitHash(crank_talk_VMContext_t *context, crank_talk_oop_t setValue, void *element, crank_talk_identitySet_explicitHashFunction_t hashFunction, crank_talk_identitySet_explicitEqualsFunction_t equalsFunction, crank_talk_oop_t *outFoundElement);

/**
 * Inserts an element in the set
 */
CRANK_TALK_API void crank_talk_IdentitySet_insert(crank_talk_VMContext_t *context, crank_talk_oop_t setValue, crank_talk_oop_t element);

#endif //SYSMEL_VM_SET_H
