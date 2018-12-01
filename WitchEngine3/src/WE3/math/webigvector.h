#ifndef _WEBigVector_h
#define _WEBigVector_h

#include "../WEMacros.h"
#include "../WEMem.h"
#include "../WEAssert.h"


namespace WE {


	template<class T>
	struct BigVector {

		int i;
		int s;

		T* _el;

		BigVector() {

			_el = 0;
			s = 0;
			i = 0;
		}

		BigVector(int _i) {

			_el = 0;
			s = 0;
		
			create(_i);
		}

		~BigVector() {

			destroy();
		}

		int size() {

			return i;
		}

		void destroy() {
	
			MMemDelete(_el);

			i = 0;
			s = 0;
		}

		void create(int _i) {

			if (_i < s) {

				return;
			}

			destroy();

			s = _i;
			i = _i;

			if (i) {
					
				MMemNew(_el, T[s]);	
			}
		}

		inline T& el(const int& _i) {

			assrt(_i < i);

			return _el[_i];
		}

		inline T& operator[](const int& _i) {

			return _el[_i];
		}

		inline operator T* () {

			return _el;
		}
	};

}


#endif