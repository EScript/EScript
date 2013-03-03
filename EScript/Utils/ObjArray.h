// ObjArray.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_OBJARRAY_H_
#define ES_OBJARRAY_H_

#include "ObjRef.h"
#include <algorithm>

namespace EScript {

class Object;
/**
 * Array of fixed size for EScript::Objects (via ObjRef or ObjPtr).
 * \note This array is especially optimized for sizes of < 3 (typical number of parameters). For those sizes,
 *	the benchmark indicates that it is a good deal faster than a std::vector.
 */
template<typename _T>
class	_ObjArray{
	// typedefs
public:
	typedef _T*			iterator;
	typedef const _T*		const_iterator;
	typedef std::size_t	size_type;

	// data
private:
	size_type paramCount;
	_T internalParams[2];
	_T * params;

	_ObjArray & operator=(const _ObjArray & other); //unimplemented
public:
	typedef _ObjArray<_T> thisObj_t;

	_ObjArray() : paramCount(0),params(nullptr){
	}
	_ObjArray(ObjPtr p1) : paramCount(1),params(internalParams){
		params[0]=p1;
	}
	_ObjArray(ObjPtr p1,ObjPtr p2) : paramCount(2),params(internalParams){
		params[0]=p1,	params[1]=p2;
	}
	_ObjArray(ObjPtr p1,ObjPtr p2,ObjPtr p3) : paramCount(3),params(new _T[3]){
		params[0]=p1,	params[1]=p2,	params[2]=p3;
	}
	_ObjArray(ObjPtr p1,ObjPtr p2,ObjPtr p3,ObjPtr p4) : paramCount(4),params(new _T[4]){
		params[0]=p1,	params[1]=p2,	params[2]=p3,	params[3]=p4;
	}
	explicit _ObjArray(size_type _paramCount) : paramCount(_paramCount),params(paramCount>2 ? new _T[paramCount] : internalParams){
	}
	explicit _ObjArray(const _ObjArray & other) : paramCount(other.paramCount),params(paramCount>2 ? new _T[paramCount] : internalParams){
		if(paramCount>0)
			std::copy(other.begin(),other.end(),begin());
	}

	~_ObjArray()								{	if(paramCount > 2) delete [] params; }
	//! \note no range check is performed.
	inline void set(size_type i,ObjPtr v)		{	params[i]=v; }
	inline void emplace(size_type i,ObjRef && v){	params[i] = std::move(v); }

	inline size_type count()const				{	return paramCount;	}
	inline size_type size()const				{	return paramCount;	}
	inline bool empty()const					{	return paramCount==0;	}

	inline ObjPtr operator[](size_type i)const	{	return i<paramCount ?	params[i] : nullptr;	}
	inline ObjPtr get(size_type i)const			{	return i<paramCount ?	params[i] : nullptr;	}
	inline iterator			begin()				{	return params; }
	inline const_iterator	begin()const		{	return params; }
	inline iterator			end()				{	return params+size(); }
	inline const_iterator	end()const			{	return params+size(); }
};

typedef _ObjArray<ObjRef> ParameterValues;

}
#endif // ES_OBJARRAY_H_
