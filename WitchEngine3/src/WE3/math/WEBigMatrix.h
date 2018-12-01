#ifndef _WEBigMatrix_h
#define _WEBigMatrix_h

#include "../WEMacros.h"
#include "../WEMem.h"
#include "../WEAssert.h"


namespace WE {


	template<class T>
	struct BigMatrix {

		int rows;
		int cols;

		int s;

		T* _el;

		struct TempVector {

			T* el;
			
			inline TempVector(T* _el) : el(_el) {}

			inline T& operator[](const int& _i) {

				return el[_i];
			}
		};

		BigMatrix() {

			_el = 0;
			s = 0;
			rows = 0;
			cols = 0;
		}

		BigMatrix(int _rows, int _cols) {

			_el = 0;
			s = 0;
			rows = 0;
			cols = 0;

			create(_rows, _cols);
		}

		~BigMatrix() {

			destroy();
		}

		void destroy() {
	
			MMemDeleteArray(_el);

			s = 0;
			rows = 0;
			cols = 0;
		}

		void size(int& rows, int& cols) {

			rows = this->rows;
			cols = this->cols;
		}

		void create(int _rows, int _cols) {

			int _s = _rows * _cols;
			
			if (_s < s) {

				rows = _rows;
				cols = _cols;

				return;	
			}

			destroy();

			s = _s;

			rows = _rows;
			cols = _cols;

			if (_s) {
					
				MMemNew(_el, T[s]);	
			}
		}

		inline T& el(int _row, int _col) {

			assrt(_row < rows && _col < cols);

			return _el[_row * cols + _col];
		}

		inline const T& el(int _row, int _col) const {

			assrt(_row < rows && _col < cols);

			return _el[_row * cols + _col];
		}

		inline TempVector operator[](const int& _row) {

			return TempVector(_el + _row * cols);
		}

		void mulVector(T* in, T* out) {

			
			for (int x = 0; x < cols; x++) {

				out[x] = 0.0f;

				for (int y = 0; y < rows; y++) {

					out[x] += in[y] * el(x, y);
				}
			}
		}

	};

	
	template<class T>
	struct BigMatrixOffset {

		int offRow;
		int offCol;

		void setup(int rowOffset, int colOffset) {

			offRow = rowOffset;
			offCol = colOffset;
		}

		inline T& el(int row, int col, BigMatrix<T>& mat) {

			return mat.el(offRow + row, offCol + col);
		}

		void setRow(int setRow, int setCol, BigMatrix<T>& mat, T* rowVector, int size) {

			int row = offRow + setRow;

			for (int off = offCol + setCol, i = 0; i < size; off++, i++) {

				mat.el(row, off) = rowVector[i];
			}
		}

		void setRow(int setRow, int setCol, BigMatrix<T>& mat, T* rowVector, int size, T& mul) {

			int row = offRow + setRow;

			for (int off = offCol + setCol, i = 0; i < size; off++, i++) {

				mat.el(row, off) = rowVector[i] * mul;
			}
		}

		void setRow3(int setRow, int setCol, BigMatrix<T>& mat, T* rowVector) {

			int row = offRow + setRow;
			int off = offCol + setCol;

			mat.el(row, off++) = rowVector[0];
			mat.el(row, off++) = rowVector[1];
			mat.el(row, off++) = rowVector[2];
		}

		void setRow3(int setRow, int setCol, BigMatrix<T>& mat, T* rowVector, T& mul) {

			int row = offRow + setRow;
			int off = offCol + setCol;

			mat.el(row, off++) = rowVector[0] * mul;
			mat.el(row, off++) = rowVector[1] * mul;
			mat.el(row, off++) = rowVector[2] * mul;
		}

		void setCol(int setRow, int setCol, BigMatrix<T>& mat, T* rowVector, int size) {

			int col = offCol + setCol;

			for (int off = offRow + setRow, i = 0; i < size; off++, i++) {

				mat.el(off, col) = colVector[i];
			}
		}

		void setCol(int setRow, int setCol, BigMatrix<T>& mat, T* rowVector, int size, T& mul) {

			int col = offCol + setCol;

			for (int off = offRow + setRow, i = 0; i < size; off++, i++) {

				mat.el(off, col) = colVector[i] * mul;
			}
		}

		void setCol3(int setRow, int setCol, BigMatrix<T>& mat, T* colVector, int size) {

			int off = offRow + setRow;
			int col = offCol + setCol;

			mat.el(off++, col) = colVector[0];
			mat.el(off++, col) = colVector[1];
			mat.el(off++, col) = colVector[2];
		}

		void setCol3(int setRow, int setCol, BigMatrix<T>& mat, T* colVector, int size, T& mul) {

			int off = offRow + setRow;
			int col = offCol + setCol;

			mat.el(off++, col) = colVector[0] * mul;
			mat.el(off++, col) = colVector[1] * mul;
			mat.el(off++, col) = colVector[2] * mul;
		}
	};

}


#endif