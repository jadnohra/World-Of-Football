#ifndef _WESceneTransformation_h
#define _WESceneTransformation_h

#include "../math/WEVector.h"
#include "../math/WEMatrix.h"
#include "../coordSys/WECoordSysConvPool.h"

namespace WE {


	struct SceneTransformation {

		Point trans;
		Vector3 rotaxis;
		float rot;

		void applyConverter(CoordSysConv& conv);
		void transform(const Matrix4x3Base& transf);

		bool validate();
	};

}

#endif
