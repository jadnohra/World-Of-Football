#ifndef _WEContactNormalSolver_h
#define _WEContactNormalSolver_h

#include "WEVector.h"
#include "WEBigMatrix.h"
#include "WEBigVector.h"
#include "../WETL/WETLArray.h"

namespace WE {

	/*
		Tries to find a unique contact resolution normal from 
		multiple normals
		
		also very useful in finding configurations where
		a solution is not possible because of conflicting normals, 
		in this case solve() returns NULL.
		trying to iteratively solve such configurations would 
		never find a valid soltion

		reuse this object to save memory and improve performance
	*/
	class ContactNormalSolver {
	public:

		typedef unsigned int Index;
		typedef float Type;

		struct Contact {

			Vector3 normal;
		};

	public:

		ContactNormalSolver();

		void reset();
		void reserveSize(Index size);

		inline Contact& add() { return mContacts.addOne(); }
		inline void remove() { --mContacts.count; }
		inline const Index& getCount() { return mContacts.count; }

		inline Contact& getContact(const Index& index) { return mContacts[index]; }

		const Vector3* solve();

	protected:

		int initHelperConstraints(bool settestdim[3], Type testdim[3], int padddim[3], int p);
		void addHelperConstraints(bool settestdim[3], Type testdim[3], int padddim[3], int p, int padd);

	protected:

		static const Type kEps;
		static const bool kAddHelperConstraints;
		static const Type kZero;
		static const Type kOne;

		typedef WETL::CountedArray<Contact, false, Index, WETL::ResizeExact> Vectors;

		Vectors mContacts;
		Vector mSolution;

		BigMatrix<Type> G;
		BigVector<Type> g0;
		BigMatrix<Type> CE;
		BigVector<Type> ce0;
		BigMatrix<Type> CI;
		BigVector<Type> ci0;
		BigVector<Type> x;

		int n, m;
	};
}

#endif