// ObjArray.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef OBJARRAY_H_INCLUDED
#define OBJARRAY_H_INCLUDED

#include "ObjRef.h"
#include <algorithm>
#include <vector>
#include <stack>

namespace EScript {

class Object;
/**
 * Array of fixed size for EScript::Objects (via ObjRef or ObjPtr).
 * \todo Add functions from Util\Array???
 */
template<typename _T>
class  _ObjArray{
	// typedefs
public:
	typedef _T*            iterator;
	typedef const _T*      const_iterator;
	typedef std::size_t    size_type;

	// data
private:
	_T * params;
	size_type paramCount;
	
	static std::vector<std::stack<_T*> > & getPool(){
		static std::vector<std::stack<_T*> > pool;
		return pool;
	}
	static _T * createArray(size_type s){
		if(s==0)
			return NULL;
		if(s<8 && s < getPool().size()){
			std::stack<_T*> & p = getPool()[s];
			if(!p.empty()){
				_T * a = p.top();
				p.pop();
				return a;
			}
		}
		return new _T[s];
	}
	static void releaseArray(_T* a,size_type s){
		if(s>0){
			if(s<8){
				if(s>=getPool().size()){
					getPool().resize(s+1);
				}
				getPool()[s].push(a);
			}else{
				delete []a;
			}
		}
	}
	
	_ObjArray & operator=(const _ObjArray & other); //unimplemented
public:
	typedef _ObjArray<_T> self_t;

	_ObjArray():params(NULL),paramCount(0){
	}
	_ObjArray(ObjPtr p1):params(createArray(1)),paramCount(1){
		params[0]=p1;
	}
	_ObjArray(ObjPtr p1,ObjPtr p2):params(createArray(2)),paramCount(2){
		params[0]=p1,   params[1]=p2;
	}
	_ObjArray(ObjPtr p1,ObjPtr p2,ObjPtr p3):params(createArray(3)),paramCount(3){
		params[0]=p1,   params[1]=p2,   params[2]=p3;
	}
	_ObjArray(ObjPtr p1,ObjPtr p2,ObjPtr p3,ObjPtr p4):params(createArray(4)),paramCount(4){
		params[0]=p1,   params[1]=p2,   params[2]=p3,   params[3]=p4;
	}
	_ObjArray(size_type _paramCount):params(createArray(_paramCount)),paramCount(_paramCount){
	}
	_ObjArray(const _ObjArray & other) : params(createArray(other.paramCount)),paramCount(other.paramCount){
		for(size_type i=0;i<paramCount;++i)
			params[i] = other.params[i];
	}

	~_ObjArray()                            {   releaseArray(params,paramCount); }
	inline void set(size_type i,ObjPtr v)   {  	params[i]=v; }

	inline void clear(){   
		releaseArray(params,paramCount);
		paramCount = 0;
		params = NULL;
	}
	inline size_type count()const           {   return paramCount;  }
	inline size_type size()const            {   return paramCount;  }
	inline bool empty()const            	{   return paramCount==0;  }
	inline void swap(self_t &other){
		std::swap(paramCount,other.paramCount);
		std::swap(params,other.params);
	}

	inline ObjPtr operator[](size_type i)const      {   return i<paramCount ?  params[i] : NULL;    }
	inline ObjPtr get(size_type i)const             {   return i<paramCount ?  params[i] : NULL;    }
	inline iterator                begin()          {   return params; }
	inline const_iterator          begin()  const   {   return params; }
	inline iterator                end()            {   return params+size(); }
	inline const_iterator          end()    const   {   return params+size(); }
};

typedef _ObjArray<ObjPtr> ParameterValues;

}
#endif // OBJARRAY_H_INCLUDED
