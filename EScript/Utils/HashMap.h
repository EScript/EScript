#ifndef ESCRIPT_HASHMAP_H_INCLUDED
#define ESCRIPT_HASHMAP_H_INCLUDED
#include <vector>
#include <cstddef>

namespace EScript {

//! Simple HashMap using single linked lists for collision resolution
template<typename value_t,typename key_t = uint32_t>
class HashMap {
		struct Entry {
			Entry * next;
			key_t key;
			value_t value;
			Entry(key_t _key, value_t _value) : next(NULL), key(_key), value(_value) {}
			~Entry() {
				if(next)
					delete next;
			}
		};
		std::vector<Entry *> data;

		size_t elementCount;

		void reallocate(size_t newDataSize) {
			std::vector<Entry *> newData(newDataSize);
			for(typename std::vector<Entry *>::iterator it = data.begin(); it != data.end(); ++it) {
				Entry * e = *it;
				while(e != NULL) {
					Entry * next = e->next;
					e->next = NULL;

					//insert entry
					Entry  ** entryPtr = &newData[e->key % newDataSize];
					while(true) {
						if( *entryPtr == NULL ) {
							*entryPtr = e;
							break;
						} else {
							entryPtr =  & (*entryPtr)->next;
						}
					}


					e = next;
				}
			}
			data.swap(newData);
		}

	public:
		
		// ----------------------------------------------------------------
		// iterators
		
		template <class self_type>
		struct iteratorBase : public std::iterator<std::forward_iterator_tag, value_t> {
				iteratorBase(HashMap & hashmap, size_t _bucket, Entry * e) :
					myHashmap(hashmap), bucket(_bucket), entry(e) { }
				iteratorBase(const self_type & other) :
					myHashmap(other.myHashmap), bucket(other.bucket), entry(other.entry) {}

				const value_t & operator*()const { return entry->value; }
				const value_t operator->()const { return &entry->value; }
				bool operator==(const self_type& rhs) { return entry == rhs.entry; }
				bool operator!=(const self_type& rhs) { return entry != rhs.entry; }

				self_type operator++() {
					entry = entry->next;
					while(entry == NULL) {
						++bucket;
						if(bucket >= myHashmap._numBuckets()) { // end()
							entry = NULL;
							break;
						}
						entry = myHashmap._bucket(bucket);
					}
					return *static_cast<self_type*>(this);
				}
				self_type operator++(int junk) {
					self_type i = *static_cast<self_type*>(this);
					++*static_cast<self_type*>(this);
					return i;
				}
				const key_t key()const				{	return entry->key;	}

				Entry * getEntry()const 			{	return entry;	}
				size_t getBucket()const				{	return bucket;	}
				HashMap & getHashMap()const			{	return myHashmap;	}

			private:
				HashMap & myHashmap;
				size_t bucket;
				Entry * entry;				
				
		};
		struct iterator : public iteratorBase<iterator> {
			iterator(HashMap & hashmap, size_t _bucket, Entry * e) :
				iteratorBase<iterator>(hashmap, _bucket, e) { }
			iterator(const iterator & other) : iteratorBase<iterator>(other) {}
			using iteratorBase<iterator>::getEntry;

			value_t & operator*() { return getEntry()->value; }
			value_t operator->() { return &getEntry()->value; }
		};
		struct const_iterator : public iteratorBase<const_iterator> {
			const_iterator(HashMap & hashmap, size_t _bucket, Entry * e) :
				iteratorBase<const_iterator>(hashmap, _bucket, e) { }
			const_iterator(const const_iterator & other) : iteratorBase<const_iterator>(other) {}
			const_iterator(const iterator & other) : iteratorBase<const_iterator>(other.getHashMap(), other.getBucket(), other.getEntry()) {}

			using iteratorBase<const_iterator>::getEntry;

			value_t & operator*() { return getEntry()->value; }
			value_t operator->() { return &getEntry()->value; }
		};
		// ---------------------------------------------------------------------------

		explicit HashMap(size_t numBuckets = 1) : data(numBuckets, NULL), elementCount(0) {}

		~HashMap() {
			for(typename std::vector<Entry *>::iterator it = data.begin(); it != data.end(); ++it) {
				delete *it;
			}
		}
		size_t _numBuckets()const			{	return data.size();	}
		Entry * _bucket(size_t bucket)const	{	return data[bucket];	}

		const_iterator begin()const	{
			if(!empty()) {
				for(size_t i = 0; i < data.size(); ++i) {
					Entry * e = _bucket(i);
					if(e != NULL)
						return const_iterator(*const_cast<HashMap*>(this), i, e);
				}
			}
			return end();
		}
		iterator begin() {
			if(!empty()) {
				for(size_t i = 0; i < data.size(); ++i) {
					Entry * e = _bucket(i);
					if(e != NULL)
						return iterator(*this, i, e);
				}
			}
			return end();
		}

		void clear() {
			data.clear();
			elementCount = 0;
		}
		bool isSet(const key_t key)const	{	return findPtr(key)!=NULL;	}
		bool empty()const					{	return elementCount == 0;		}
		const_iterator end()const 			{	return const_iterator(*const_cast<HashMap*>(this), data.size(), NULL);	}
		iterator end() 						{	return iterator(*this, data.size(), NULL);	}

		value_t * findPtr(const key_t key) {
			Entry * entry = data[key % data.size()];
			while(entry != NULL) {
				if(entry->key == key)
					return &(entry->value);
				entry = entry->next;
			}
			return NULL;
		}
		const value_t * findPtr(const key_t key)const {
			Entry * entry = data[key % data.size()];
			while(entry != NULL) {
				if(entry->key == key)
					return &(entry->value);
				entry = entry->next;
			}
			return NULL;
		}
//		const iterator find(const key_t key)const{
//			const size_t bucket = key % data.size();
//			Entry * entry = data[bucket];
//			while(entry != NULL) {
//				if(entry->key == key){
//					return iterator(*this,bucket,entry);
//				}
//				entry = entry->next;
//			}
//			return end();
//		}

		void insert(const key_t key, const value_t & value) {
			if( elementCount > data.size() && elementCount > 7)
				reallocate( elementCount * 2 );

			Entry  ** entryPtr = &data[key % data.size()];
			while(true) {
				Entry * entry = *entryPtr;
				if( entry == NULL ) {
					*entryPtr = new Entry(key, value);
					break;
				} else if( entry->key == key) {
					entry->value = value;
					return;
				} else {
					entryPtr =  & (*entryPtr)->next;
				}
			}
			++elementCount;

		}
		value_t & get(const key_t key) {
			if( elementCount > data.size() && elementCount > 7)
				reallocate( elementCount * 2 );

			Entry  ** entryPtr = &data[key % data.size()];
			while(true) {
				Entry * entry = *entryPtr;
				if( entry == NULL ) {
					*entryPtr = new Entry(key, value_t());
					++elementCount;

					break;
				} else if( entry->key == key) {
					break;
				} else {
					entryPtr =  & (*entryPtr)->next;
				}
			}
			return (*entryPtr)->value;
		}
		bool unset(const key_t key) {
			for(Entry  ** entryPtr = &data[key % data.size()]; *entryPtr != NULL; entryPtr = &(*entryPtr)->next) {
				Entry * entry = *entryPtr;
				if( entry->key == key) {
					--elementCount;
					entryPtr = entry->next;
					entry->next = NULL;
					delete entry;
					return true;
				}
			}
			return false;
		}

		size_t size()const					{	return elementCount;	}

		void swap(HashMap & other) {
			data.swap(other.data);
			std::swap(elementCount, other.elementCount);
		}
};
}

#endif // ESCRIPT_HASHMAP_H_INCLUDED
