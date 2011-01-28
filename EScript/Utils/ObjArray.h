#ifndef OBJARRAY_H_INCLUDED
#define OBJARRAY_H_INCLUDED

#include "ObjRef.h"
#include <algorithm>

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
public:

    _ObjArray():params(NULL),paramCount(0){
    }
    _ObjArray(ObjPtr p1):params(new _T[1]),paramCount(1){
        params[0]=p1;
    }
    _ObjArray(ObjPtr p1,ObjPtr p2):params(new _T[2]),paramCount(2){
        params[0]=p1,   params[1]=p2;
    }
    _ObjArray(ObjPtr p1,ObjPtr p2,ObjPtr p3):params(new _T[3]),paramCount(3){
        params[0]=p1,   params[1]=p2,   params[2]=p3;
    }
    _ObjArray(ObjPtr p1,ObjPtr p2,ObjPtr p3,ObjPtr p4):params(new _T[4]),paramCount(4){
        params[0]=p1,   params[1]=p2,   params[2]=p3,   params[3]=p4;
    }
    _ObjArray(size_type _paramCount):params(new _T[_paramCount]),paramCount(_paramCount){
    }
    _ObjArray(const _ObjArray & other):params(new _T[other.size()]),paramCount(other.size()){
        std::copy(other.begin(),other.end(),begin());
    }

    ~_ObjArray()                            {   delete[]params; }
    inline void set(size_type i,ObjPtr v)   {  	params[i]=v; }

    inline size_type count()const           {   return paramCount;  }
    inline size_type size()const            {   return paramCount;  }

    inline _ObjArray & operator=(const _ObjArray & other) {
    	if(&other != this){
    		paramCount=other.paramCount;
    		_T * newParams=new _T[paramCount];
    		std::copy(other.begin(),other.end(),newParams);
			delete[]params;
			params=newParams;
//			std::cout<<" ##"<<size();
    	}

    	return *this;
	}

    inline ObjPtr operator[](size_type i)const      {   return i<paramCount ?  params[i] : NULL;    }
    inline ObjPtr get(size_type i)const             {   return i<paramCount ?  params[i] : NULL;    }
    inline iterator                begin()          {   return params; }
    inline const_iterator          begin()  const   {   return params; }
    inline iterator                end()            {   return params+size(); }
    inline const_iterator          end()    const   {   return params+size(); }
};

typedef _ObjArray<ObjPtr> ParameterValues;
//typedef _ObjArray<ObjRef> ObjArray;

}
#endif // OBJARRAY_H_INCLUDED
