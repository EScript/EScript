// Compiler.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Compiler.h"
#include "CompilerContext.h"
#include "../Objects/typeIds.h"
#include "../Objects/Values/Bool.h"
#include "../Objects/Values/Number.h"
#include "../Objects/Values/String.h"
#include "../Objects/Values/Void.h"
#include <stdexcept>
#include <map>

#if !defined(_MSC_VER) and !defined(UNUSED_ATTRIBUTE)
#define UNUSED_ATTRIBUTE __attribute__ ((unused))
#else
#define UNUSED_ATTRIBUTE
#endif

namespace EScript{

// init handlerRegistry \todo Can't this be done more efficiently using c++11 functionals???
struct handler_t{ virtual void operator()(CompilerContext & ctxt,Object * obj)=0; };
typedef std::map<internalTypeId_t,handler_t *> handlerRegistry_t;
static bool initHandler(handlerRegistry_t &);
static handlerRegistry_t handlerRegistry;
static bool _handlerInitialized UNUSED_ATTRIBUTE = initHandler(handlerRegistry);

void Compiler::compileExpression(CompilerContext & ctxt,Object * expression)const{
	const internalTypeId_t typeId = expression ? expression->_getInternalTypeId() : 0;
	
	handlerRegistry_t::iterator it = handlerRegistry.find(typeId);
	if(it==handlerRegistry.end()){
		throw std::invalid_argument("Expression can't be compiled.");
	}
	(*it->second)(ctxt,expression);
}


// ------------------------------------------------------------------


//! (static)
bool initHandler(handlerRegistry_t & m){
	// \note  the redundant assignment to 'id2' is a workaround to a strange linker error ("undefined reference EScript::_TypeIds::TYPE_NUMBER")
	#define ADD_HANDLER( _id, _type, _block) \
	{ \
		struct _handler : public handler_t{ \
			~_handler(){} \
			virtual void operator()(CompilerContext & ctxt,Object * obj){ \
				_type * self = dynamic_cast<_type*>(obj); \
				if(!self) throw std::invalid_argument("Wrong type!"); \
				do _block while(false); \
			} \
		}; \
		const internalTypeId_t id2 = _id; \
		m[id2] = new _handler(); \
	}
	// ------------------------


	// Number
	ADD_HANDLER( _TypeIds::TYPE_NUMBER, Number, {
		ctxt.addInstruction(Instruction::createPushNumber(self->toDouble()));
	})
	// Bool
	ADD_HANDLER( _TypeIds::TYPE_BOOL, Bool, {
		ctxt.addInstruction(Instruction::createPushNumber(self->toBool()));
	})	
	// String
	ADD_HANDLER( _TypeIds::TYPE_STRING, String, {
		ctxt.addInstruction(Instruction::createPushString(ctxt.declareString(self->toString())));
	})	
	// Void
	ADD_HANDLER( _TypeIds::TYPE_VOID, Void, {
		ctxt.addInstruction(Instruction::createPushVoid());
	})


	// ------------------------
	#undef ADD_HANDLER
	return true;
}



}