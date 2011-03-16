// LocalVarMap.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef LOCAL_VAR_MAP_H
#define LOCAL_VAR_MAP_H

#include <map>
#include <stack>

#include "Hashing.h"
#include "ObjRef.h"

namespace EScript {
class Object;

/**
 *  A hirarchical map for EScript-Objects.
 *  [LocalVarMap]
 */
class LocalVarMap {

	public:

		// --------------
		// ---- main
		LocalVarMap();
		~LocalVarMap();

		// \note if parent is NULL, use init().
		void init(LocalVarMap *parent);
		void init();
		inline LocalVarMap * getParent()const 		{	return parent;	}

		void reset();

		inline Object * find(const identifierId id)const{
			objMap_t::const_iterator i=getValues()->find(id);
			return (i==getValues()->end() || i->second.empty()) ?
				NULL : i->second.top().get();
		}

		bool findAndUpdate(const identifierId id,Object * val);
		inline void declare(const identifierId varId,Object * val){
			objStack_t & s=(*getValues())[varId];
			s.push(val);
			localVars.push(&s);
		}

		// --------------
		// ---- internals
	private:
		typedef std::stack<ObjRef> objStack_t;
		typedef std::map<identifierId,objStack_t > objMap_t;
		inline objMap_t * getValues()const 		{	return values;	}

		LocalVarMap * parent;
		objMap_t * values;
		objMap_t m;
		std::stack<objStack_t *> localVars;
};


}
#endif // LOCAL_VAR_MAP_H
