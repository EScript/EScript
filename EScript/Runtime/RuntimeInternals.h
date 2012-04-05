// RuntimeInternals.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_RUNTIME_INTERNALS_H
#define ES_RUNTIME_INTERNALS_H

#include "Runtime.h"

namespace EScript {

/*! [RuntimeInternals] */
class RuntimeInternals  {
		Runtime &runtime;

	/// @name Main
	// 	@{
		RuntimeInternals(RuntimeInternals & other); // = delete
	public:
		RuntimeInternals(Runtime & rt);
	// @}

	// --------------------
	
	/// @name System calls
	// 	@{
	//! (interna) Used by the Runtime.
	private:
		std::vector<ERef<Function> > systemFunctions;
		void initSystemFunctions();
	public:
		Object * sysCall(uint32_t sysFnId,ParameterValues & params);
	//	@}
};
}

#endif // ES_RUNTIME_INTERNALS_H
