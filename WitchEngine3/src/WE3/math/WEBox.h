#ifndef _WEBox_h
#define _WEBox_h

#include "WEOBB.h"

namespace WE {


	class Box {
	public:

		OBB obb;

	public:

		Vector boxPosition;

		Vector centerOffset;
		bool isDirty;

	public:

		Box();
		Box(const Box& ref);

		void init(WE::AAB3& ref, bool identityBasis = false);
		void init(WE::AAB3& ref, const float& scale, bool identityBasis = false);
		void init(WE::Vector& extents, bool identityBasis = false);
		void setCoordFrame(const Matrix4x3Base& coordFrame);

		void transform(const Matrix4x3Base& matrix, OBB& result) const;
		
		void empty();
		bool isEmpty();	

		void mergeTo(WE::AAB3& aab);

		Vector& position(bool readOnly = false);
		Matrix3x3Base& basis(bool readOnly = false);
		const OBB& toOBB();
		
		void toPoints(Vector points[8]);

		void update(bool force = false);

		static void transform(const AAB3& aab, const Matrix4x3Base& matrix, OBB& result);

	protected:

		void _update();
	};
	
	
}


#endif