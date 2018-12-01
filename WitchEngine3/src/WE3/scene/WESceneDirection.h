#ifndef _WESceneDirection_h
#define _WESceneDirection_h

#include "../math/WEVector.h"
#include "math.h"

namespace WE {

	enum SceneDirectionEnum {

		Scene_Right = 0, Scene_Up, Scene_Forward
	};

	enum SceneDirection2DEnum {

		Scene2D_Right = 0, Scene2D_Forward, Scene2D_None
	};

	struct SceneSphericalVector : Vector3 {

		enum Coord { Mag = 0, HorizAngle, VertAngle };

		void initFromCartesian(const Vector3& cartesian) {

			el[Mag] = cartesian.mag();
			el[VertAngle] = asinf(cartesian.el[Scene_Up] / el[Mag]);
			el[HorizAngle] = atan2f(cartesian.el[Scene_Right], cartesian.el[Scene_Forward]);
		}

		inline const float& getMag() { return el[Mag]; }
		inline const float& getHorizAngle() { return el[HorizAngle]; }
		inline const float& getVertAngle() { return el[VertAngle]; }
	};

	struct SceneCylindricalVector : Vector3 {

		enum Coord { HorizMag = 0, HorizAngle, VertMag };

		void initFromCartesian(const Vector3& cartesian) {

			el[HorizMag] = sqrtf(cartesian.el[Scene_Forward] * cartesian.el[Scene_Forward] + cartesian.el[Scene_Right] * cartesian.el[Scene_Right]);
			el[VertMag] = cartesian.el[Scene_Up];
			el[HorizAngle] = atan2f(cartesian.el[Scene_Right], cartesian.el[Scene_Forward]);
		}

		inline const float& getHorizMag() { return el[HorizMag]; }
		inline const float& getHorizAngle() { return el[HorizAngle]; }
		inline const float& getVertMag() { return el[VertMag]; }
	};
}

#endif