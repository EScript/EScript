// RuntimeHelper.h
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2013-2014 Claudius Jähn <ClaudiusJ@live.de>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#ifndef ES_RUNTIME_HELPER_H_INCLUDED
#define ES_RUNTIME_HELPER_H_INCLUDED

#include "StringId.h"
#include <cstddef>
#include <iosfwd>
#include <string>
#include <utility>
#include <unordered_map>

namespace EScript {
// ------------------------------------------
// Forward declarations.
class CodeFragment;
class Object;
class Runtime;
class StringData;
template<class _T>class ERef;
typedef ERef<Object> ObjRef;

template<class _T>class EPtr;
typedef EPtr<Object> ObjPtr;

template<typename _T>class _ObjArray;
typedef _ObjArray<ObjRef> ParameterValues;

// ------------------------------------------
// Internals
namespace _Internals{
//! (internal) Non-inline part of @a assertParamCount
ESCRIPTAPI void assertParamCount_2(Runtime & runtime, int paramCount, int min, int max);
//! (internal) Non-inline part of @a assertType.
ESCRIPTAPI void assertType_throwError(Runtime & runtime, const ObjPtr & obj,const char * className);
}

// ------------------------------------------

/*!
 * Check if the number of given parameters is in the given range (min <= number <= max).
 * A range value of <0 accepts an arbitrary number of parameters.
 * If too many parameters are given, a warning message is shown.
 * If too few parameter are given, a runtime error is thrown.
 */
inline void assertParamCount(Runtime & runtime, const ParameterValues & params, int min=-1, int max=-1) {
	const int paramCount = static_cast<int>(params.count());
	if((paramCount < min && min >= 0) || ((paramCount > max) && max >= 0))
		_Internals::assertParamCount_2(runtime, paramCount, min, max);
}
inline void assertParamCount(Runtime & runtime, const size_t paramCount, int min=-1, int max=-1) {
	const int paramCountI = static_cast<int>(paramCount);
	if((paramCountI < min && min >= 0) || ((paramCountI > max) && max >= 0))
		_Internals::assertParamCount_2(runtime, paramCountI, min, max);
}

/*!
 * Try to cast the given object to the specified type.
 * If the object is not of the appropriate type, a runtime error is thrown.
 */
template<class T> static T * assertType(Runtime & runtime, const ObjPtr & obj) {
	T * t = dynamic_cast<T *>(obj.get());
	if(t == nullptr)
		_Internals::assertType_throwError(runtime, obj, T::getClassName());
	return t;
}

ESCRIPTAPI ObjRef callMemberFunction(Runtime & rt, ObjPtr obj, StringId fnNameId, const ParameterValues & params);
ESCRIPTAPI ObjRef callFunction(Runtime & rt, Object * function, const ParameterValues & params);

/*! Compile and execute the given code and return the result.
	\note may throw 'std::exception' or 'Object *' on failure. */
ESCRIPTAPI ObjRef _eval(Runtime & runtime, const CodeFragment & code,const std::unordered_map<StringId,ObjRef>& staticVars);

/*! Compile and execute the given code. If the code could be executed without an exception, (true,result) is returned;
	if an exception (of type Object*) occured (false,exception) is returned and the error message is sent to the runtime's logger.
	@param fileId Name used to identify the executed code in exception messages and stack traces; the default is '[inline]'
	@return (success, result)
*/
ESCRIPTAPI std::pair<bool, ObjRef> eval(Runtime & runtime, const StringData & code,const StringId & fileId,const std::unordered_map<StringId,ObjRef>& staticVars);
ESCRIPTAPI std::pair<bool, ObjRef> eval(Runtime & runtime, const StringData & code,const StringId & fileId = StringId());

//! @return (success, result)
ESCRIPTAPI std::pair<bool, ObjRef> executeStream(Runtime & runtime, std::istream & stream);

//! @return result (mmay throw an exception)
ESCRIPTAPI ObjRef _loadAndExecute(Runtime & runtime, const std::string & filename,const std::unordered_map<StringId,ObjRef>& staticVars);

//! @return (success, result)
ESCRIPTAPI std::pair<bool, ObjRef> loadAndExecute(Runtime & runtime, const std::string & filename);
ESCRIPTAPI std::pair<bool, ObjRef> loadAndExecute(Runtime & runtime, const std::string & filename,const std::unordered_map<StringId,ObjRef>& staticVars);

//! creates and throws an Exception objects.
ESCRIPTAPI void throwRuntimeException(const std::string & what);

}

#endif /* ES_RUNTIME_HELPER_H_INCLUDED */
