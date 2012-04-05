// RuntimeInternals.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "RuntimeInternals.h"
#include "FunctionCallContext.h"
#include "../Utils/Macros.h"
#include "../Utils/Helper.h"
#include "../Objects/Collections/Array.h"
#include "../Objects/Collections/Map.h"
#include "../Objects/Values/Void.h"
#include "../Objects/YieldIterator.h"

#include <sstream>

namespace EScript{

//! (ctor)
RuntimeInternals::RuntimeInternals(Runtime & rt) : runtime(rt){
	initSystemFunctions();
}


// -------------------------------------------------------------
// System calls

//! (internal)
void RuntimeInternals::initSystemFunctions(){
	systemFunctions.resize(7);

	//! init system calls \note the order of the functions MUST correspond to their funcitonId as defined in Consts.h
	{	// SYS_CALL_CREATE_ARRAY = 0;
		struct _{
			ESF( sysCall,0,-1,Array::create(parameter) )
		};
		systemFunctions[Consts::SYS_CALL_CREATE_ARRAY] = new Function(_::sysCall);
	}	
	{	//! [ESF] Map SYS_CALL_CREATE_MAP( key0,value0, key1,value1, ... )
		struct _{
			ES_FUNCTION( sysCall) {
					if ( (parameter.count()%2)==1 ) runtime.warn("Map: Last parameter ignored!");
					Map * a=Map::create();
					for (ParameterValues::size_type i=0;i<parameter.count();i+=2)
						a->setValue(parameter[i],parameter[i+1]);
				return a;			
			}
		};
		systemFunctions[Consts::SYS_CALL_CREATE_MAP] = new Function(_::sysCall);
	}
	{	//! [ESMF] Void SYS_CALL_THROW_TYPE_EXCEPTION( expectedType, receivedValue )
		struct _{
			ES_FUNCTION( sysCall) {
				assertParamCount(runtime,2,-1);
				std::ostringstream os;
				os << "Wrong parameter type: Expected ";
				for(size_t i = 0;i<parameter.size()-1;++i ){
					if(i>0) os <<", ";
					os<<(parameter[i].isNotNull() ? parameter[i]->toDbgString() : "???");
				}
				os << " but got " << parameter[parameter.size()-1]->toDbgString()<<".";
				runtime.setException(os.str());
				return static_cast<Object*>(NULL);
			}
		};
		systemFunctions[Consts::SYS_CALL_THROW_TYPE_EXCEPTION] = new Function(_::sysCall);
	}
	{	//! [ESMF] Void SYS_CALL_THROW( [value] )
		struct _{
			ESF( sysCall,0,1,(runtime.setExceptionState( parameter.count()>0 ? parameter[0] : Void::get() ),static_cast<Object*>(NULL)))
		};
		systemFunctions[Consts::SYS_CALL_THROW] = new Function(_::sysCall);
	}	
	{	//! [ESMF] Void SYS_CALL_EXIT( [value] )
		struct _{
			ESF( sysCall,0,1,(runtime.setExitState( parameter.count()>0 ? parameter[0] : Void::get() ),static_cast<Object*>(NULL)))
		};
		systemFunctions[Consts::SYS_CALL_EXIT] = new Function(_::sysCall);
	}
	{	//! [ESMF] Iterator SYS_CALL_GET_ITERATOR( object );
		struct _{
			ES_FUNCTION( sysCall) {
				assertParamCount(runtime,1,1);
				ObjRef it;
				if(	Collection * c=parameter[0].toType<Collection>()){
					it = c->getIterator();
				}else if(parameter[0].toType<YieldIterator>()){
					it = parameter[0].get();
				}else {
					it = callMemberFunction(runtime,parameter[0] ,Consts::IDENTIFIER_fn_getIterator,ParameterValues());
				}
				if(it==NULL){
					runtime.setException("Could not get iterator from '" + parameter[0]->toDbgString() + '\'');
					return NULL;
				}
				return it.detachAndDecrease();
			}
		};
		systemFunctions[Consts::SYS_CALL_GET_ITERATOR] = new Function(_::sysCall);
	}
	{	//! [ESMF] Void SYS_CALL_TEST_ARRAY_PARAMETER_CONSTRAINTS( expectedTypes*, Array receivedValue )
		struct _{
			ES_FUNCTION( sysCall) {
				assertParamCount(runtime,2,-1);
				const size_t constraintEnd = parameter.size()-1;

				Array * values = assertType<Array>(runtime,parameter[constraintEnd]);
				
				for(Array::iterator it = values->begin();it!=values->end();++it){
					bool success = false;
					for(size_t i = 0; i<constraintEnd; ++i){
						if(Runtime::checkParameterConstraint(runtime,*it,parameter[i])){
							success = true;
							break;
						}
					}
					if(!success){
						std::ostringstream os;
						os << "Wrong parameter type: Expected ";
						for(size_t i = 0;i<constraintEnd;++i ){
							if(i>0) os <<", ";
							os<<(parameter[i].isNotNull() ? parameter[i]->toDbgString() : "???");
						}
						os << " but got " << (*it)->toDbgString()<<".";
						runtime.setException(os.str());
						return static_cast<Object*>(NULL);
					}
				}
				return static_cast<Object*>(NULL);
			}
		};
		systemFunctions[Consts::SYS_CALL_TEST_ARRAY_PARAMETER_CONSTRAINTS] = new Function(_::sysCall);
	}
}

//! (static)
Object * RuntimeInternals::sysCall(uint32_t sysFnId,ParameterValues & params){
	Function * fn = NULL;
	if(sysFnId<systemFunctions.size()){
		fn = systemFunctions.at(sysFnId).get();
	}
	if(!fn){
		runtime.setException("Unknown systemCall."); // \todo improve message
		return NULL;
	}
	return fn->getFnPtr()(runtime,NULL,params);
}

// -------------------------------------------------------------

}