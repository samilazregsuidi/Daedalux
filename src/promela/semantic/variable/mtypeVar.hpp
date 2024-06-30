#ifndef MTYPE_VARIABLE_H
#define MTYPE_VARIABLE_H

#include "enumVar.hpp"

typedef enumVar<unsigned char, variable::V_MTYPE> mtypeVar;

typedef enumVar<const unsigned char, variable::V_CMTYPE> cmtypeVar;

typedef enumDef<const unsigned char> mtypeDef;

#endif