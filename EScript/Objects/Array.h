// Array.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ARRAY_H
#define ARRAY_H

#include "Collection.h"
#include "Iterator.h"
#include "../Utils/ObjArray.h"
#include <vector>
#include <stack>

namespace EScript {

/*! [Array]  ---|> [Collection] ---|> [Object]  */
class Array : public Collection {
		ES_PROVIDES_TYPE_NAME(Array)
	private:
		static std::stack<Array *> pool;
	public:
		static Type* typeObject;
		static void init(EScript::Namespace & globals);

		static Array * create(Type * type=NULL);
		static Array * create(const ParameterValues & p,Type * type=NULL);
		static Array * create(size_t num,Object ** objs,Type * type=NULL);
		static Array * create(size_t num,char ** strings,Type * type=NULL);
		static void release(Array * b);

		// ---

		Array(Type * type=NULL);
		virtual ~Array();

		void pushBack(ObjPtr obj);
		void popBack();

		void pushFront(ObjPtr obj);
		void popFront();
		void reverse();

		Object * back()const;
		Object * front()const;
		void rt_sort(Runtime & runtime,Object * function=NULL,bool reverseOrder=false);
		void rt_filter(Runtime & runtime,ObjPtr function, const ParameterValues & additionalValues);
		Object * get(unsigned int index) {
			return index<vec.size() ?  vec.at(index).get():NULL;
		}
		/// returns -1 if not found
		int rt_indexOf(Runtime & runtime,ObjPtr search,size_t begin=0);
		void removeIndex(size_t index);

		std::string implode(const std::string & delimiter=";");
		void append(Collection * c);
		void resize(size_t newSize);
		void reserve(size_t capacity);
		void swap(Array * other);

		/// ---|> [Collection]
		virtual Object * getValue(ObjPtr key);
		virtual void setValue(ObjPtr key,ObjPtr value);
		virtual size_t count()const;
		virtual Iterator * getIterator();
		virtual void clear();


		/// ---|> [Object]
		virtual Object * clone()const;
		virtual internalTypeId_t _getInternalTypeId()const 	{	return _TypeIds::TYPE_ARRAY;	}

		/**
		 * [ArrayIterator] ---|> [Iterator] ---|> [Object]
		 */
		class ArrayIterator : public Iterator {
				ES_PROVIDES_TYPE_NAME(ArrayIterator)
			public:
				ArrayIterator(Array * ar);
				virtual ~ArrayIterator();

				/// ---|> [Object]
				virtual Object * key();
				virtual Object * value();
				virtual void reset();
				virtual void next();
				virtual bool end();

				/// ---|> [Object]
				// todo: clone!
			private:
				ERef<Array> arrayRef;
				unsigned int index;
		};

	private:
		void init(const ParameterValues & p);
		void init(size_t num,Object ** objs);
		void init(size_t num,char ** strings);

		std::vector<ObjRef> vec;
};

}

#endif // COLLECTION_H
