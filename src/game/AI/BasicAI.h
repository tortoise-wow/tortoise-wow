#ifndef MANGOS_BASIC_AI_COMPATIBILITY_H
#define MANGOS_BASIC_AI_COMPATIBILITY_H

#include "AggressorAI.h"

// The maintained VMaNGOS Eluna adapter derives its Lua creature AI from
// BasicAI. Turtle's equivalent default hostile AI is AggressorAI.
class BasicAI : public AggressorAI
{
    public:
        explicit BasicAI(Creature* creature) : AggressorAI(creature) {}
};

#endif
