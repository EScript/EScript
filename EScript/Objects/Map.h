#ifndef MAP_H
#define MAP_H

#include "Collection.h"
#include "Iterator.h"
#include <map>

namespace EScript {

/*! [Map] ---|> [Collection] ---|> [Object] */
class Map : public Collection {
        ES_PROVIDES_TYPE_NAME(Map)
        /*!	[MapEntry]	*/
        struct MapEntry {
			MapEntry() {	}
			MapEntry(ObjPtr _key,ObjPtr _value) : key(_key),value(_value) {	}
			MapEntry(const MapEntry &other) : key(other.key),value(other.value) {}
			MapEntry & operator=(const MapEntry &other){
				if(this!=&other){
					key=other.key;
					value=other.value;
				}
				return *this;
			}
			ObjRef key;
			ObjRef value;
        };
        typedef std::map<std::string,MapEntry>  objectMap_t;

    public:

        /*!	[MapIterator] ---|> [Iterator]	*/
        class MapIterator : public Iterator {
                ES_PROVIDES_TYPE_NAME(MapIterator)
            public:
                MapIterator(Map * ar);
                virtual ~MapIterator();

                /// ---|> [Iterator]
                virtual Object * key();
                /// ---|> [Iterator]
                virtual Object * value();
                /// ---|> [Iterator]
                virtual void reset();
                /// ---|> [Iterator]
                virtual void next();
                /// ---|> [Iterator]
                virtual bool end();

            private:
                ERef<Map> mapRef;
                objectMap_t::iterator it;
        };
        // -----------------------------------

        static Type* typeObject;
        static void init(EScript::Namespace & globals);

		static Map * create();
		static Map * create(const std::map<identifierId,Object *> &);

        // ---
        Map(Type * type=NULL);
        virtual ~Map();

        void unset(ObjPtr key);
        Object * getValue(const std::string & key);
        void merge(Collection * c,bool overwrite=true);

        /// ---|> [Collection]
        virtual Object * getValue(ObjPtr key);
        /// ---|> [Collection]
        virtual void setValue(ObjPtr key,ObjPtr value);
        /// ---|> [Collection]
        virtual size_t count()const;
        /// ---|> [Collection]
        virtual Iterator * getIterator();
        /// ---|> [Collection]
        virtual void clear();

        /// ---|> [Object]
        Object * clone()const;

    private:
        objectMap_t m;
};

}

#endif // MAP_H
