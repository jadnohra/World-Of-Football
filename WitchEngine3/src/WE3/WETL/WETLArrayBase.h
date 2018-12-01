#ifndef _WETLArrayBase_h
#define _WETLArrayBase_h


namespace WETL {


	template <class T, class IdxT> 
	struct ArrayBase {

		typedef IdxT Index;
		//Xstatic const IdxT IndexNull = ((IdxT) -1);

		T* el;
		IdxT size;
		
		ArrayBase() {

			el = NULL;
			size = 0;
		}

		inline bool inline_searchFor(const T& search, const IdxT& from, const IdxT& count, IdxT& index) {

			for (index = from; index < count; index++) {

				if (el[index] == search) {
					
					return true;
				}
			}

			return false;
		}

		bool searchFor(const T& search, const IdxT& from, const IdxT& count, IdxT& index) {

			return inline_searchFor(search, from, count, index);
		}

		template<typename SearchT>
		inline bool inline_castSearchFor(const SearchT& search, const IdxT& from, const IdxT& count, IdxT& index) {

			for (index = from; index < count; index++) {

				if ((SearchT) el[index] == search) {
					
					return true;
				}
			}

			return false;
		}

		template<typename SearchT>
		bool castSearchFor(const SearchT& search, const IdxT& from, const IdxT& count, IdxT& index) {

			return inline_castSearchFor(search, from, count, index);
		}

		inline T& operator[] (const IdxT& i) {
			return el[i];
		}
	};

	
	template <class T, class IdxT> 
	struct CountedArrayBase : ArrayBase<T, IdxT> {

		IdxT count;

		CountedArrayBase() {

			count = 0;
		}
	};


	template <class T>
	struct _LinkT {

		T previous;
		T next;
	};


	template <class T>
	struct LinkT : _LinkT<T> {
		
		LinkT() {

			previous = 0;
			next = 0;
		};
	};

	template <class IdxT>
	struct Link : _LinkT<IdxT> {

		IdxT freeLink;
	};

	

	template <class IdxT> 
	struct LinkArray {

		typedef IdxT Index;
		static const Index IdxTNull = ((Index) -1);

		Link<IdxT>* pLinks;
		
		IdxT _first;
		IdxT _last;

		
		LinkArray() {

			pLinks = NULL;
			_first = IdxTNull;
			_last = IdxTNull;
		}

		static inline IdxT NullIndex() {

			return WE::NullIndex<IdxT>();
		}

		inline const IdxT& first() {

			return _first;
		}

		inline const IdxT& last() {

			return _last;
		}

		inline const IdxT& next(const IdxT& i) {

			return pLinks[i].next;
		}

		inline const IdxT& prev(const IdxT& i) {

			return pLinks[i].prev;
		}
	};

}

#endif