// Consts.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Consts.h"

namespace EScript{

const identifierId Consts::IDENTIFIER_this=stringToIdentifierId("this");
//const identifierId Indentifier::IDENTIFIER_parent=stringToIdentifierId("_parent");
const identifierId Consts::IDENTIFIER_thisFn=stringToIdentifierId("thisFn");
const identifierId Consts::IDENTIFIER_fn_constructor=stringToIdentifierId("_constructor");
const identifierId Consts::IDENTIFIER_fn_less=stringToIdentifierId("<");
const identifierId Consts::IDENTIFIER_fn_greater=stringToIdentifierId(">");
const identifierId Consts::IDENTIFIER_fn_equal=stringToIdentifierId("==");
const identifierId Consts::IDENTIFIER_fn_identical=stringToIdentifierId("===");
const identifierId Consts::IDENTIFIER_fn_getIterator=stringToIdentifierId("getIterator");
const identifierId Consts::IDENTIFIER_fn_get=stringToIdentifierId("_get");
const identifierId Consts::IDENTIFIER_fn_set=stringToIdentifierId("_set");

const identifierId Consts::IDENTIFIER_true=stringToIdentifierId("true");
const identifierId Consts::IDENTIFIER_false=stringToIdentifierId("false");
const identifierId Consts::IDENTIFIER_void=stringToIdentifierId("void");
const identifierId Consts::IDENTIFIER_null=stringToIdentifierId("null");

const identifierId Consts::IDENTIFIER_as=stringToIdentifierId("as");
const identifierId Consts::IDENTIFIER_break=stringToIdentifierId("break");
const identifierId Consts::IDENTIFIER_catch=stringToIdentifierId("catch");
const identifierId Consts::IDENTIFIER_continue=stringToIdentifierId("continue");
const identifierId Consts::IDENTIFIER_do=stringToIdentifierId("do");
const identifierId Consts::IDENTIFIER_else=stringToIdentifierId("else");
const identifierId Consts::IDENTIFIER_exit=stringToIdentifierId("exit");
const identifierId Consts::IDENTIFIER_for=stringToIdentifierId("for");
const identifierId Consts::IDENTIFIER_foreach=stringToIdentifierId("foreach");
const identifierId Consts::IDENTIFIER_if=stringToIdentifierId("if");
const identifierId Consts::IDENTIFIER_namespace=stringToIdentifierId("namespace");
const identifierId Consts::IDENTIFIER_return=stringToIdentifierId("return");
const identifierId Consts::IDENTIFIER_throw=stringToIdentifierId("throw");
const identifierId Consts::IDENTIFIER_try=stringToIdentifierId("try");
const identifierId Consts::IDENTIFIER_var=stringToIdentifierId("var");
const identifierId Consts::IDENTIFIER_while=stringToIdentifierId("while");

const identifierId Consts::IDENTIFIER_originalId=stringToIdentifierId("originalId");

const identifierId Consts::IDENTIFIER_LINE=stringToIdentifierId("__LINE__");

}
