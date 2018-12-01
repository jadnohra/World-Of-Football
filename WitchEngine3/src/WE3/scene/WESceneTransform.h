#ifndef _WESceneTranform_h
#define _WESceneTranform_h

#include "../math/WEMatrix.h"
#include "../math/WEBox.h"
#include "../math/WEQuaternion.h"

#include "WESceneTransformation.h"
#include "WESceneDirection.h"

namespace WE {

	//transforms from local to parent
	struct SceneTransform : RigidMatrix4x3 {

		SceneTransform(bool setIdentity = false);

		//given objects pos/orient rel. to parent
		inline const Vector3& getPosition() const { return row[3]; }
		inline CtVector3& getPositionCt() { return row[3]; }
		inline Vector3& position() { return row[3]; }
		void getPosition(Vector3& pos) const; 
		void setPosition(const Vector3 &pos); 
		void movePosition(const Vector3 &pos); 
		void movePositionByVel(const CtVector3& vel, const float& dt); 

		void setOrientation(CardinalAxis axis, float theta, bool leftHanded = true);
		void setOrientation(const Vector3 &axis, float theta, bool isNormalized = false, bool leftHanded = true);
		void setOrientation(const Vector3 &eye, const Vector3 &lookAt, const Vector3 &unitUp);
		void setOrientation(const Vector3 &dir, const Vector3 &unitUp);
		bool isOrientation(CtVector3 &dir, const Vector3 &unitUp);
		void setOrientation(const Vector3 &eulerAngles, bool leftHanded = true);
		void setOrientation(const Quaternion& orient);
		void setOrientation2D(const float& dirX, const float& dirY, bool leftHanded);
		inline const Vector3& getDirection(const SceneDirectionEnum& dir) const { return row[dir]; }
		void getOrientation(Quaternion& orient);
		void moveOrientation(const Vector3 &axis, float theta, bool isNormalized = false, bool leftHanded = true);
		void moveOrientation(Quaternion &quat, bool isNormalized = false);
		void setTransformation(const SceneTransformation& trans, bool isNormalized = false, bool leftHanded = true);
		
		void syncBox(Box& box, bool syncRotation = true, bool syncTranslation = true) const;


		inline CtVector3& script_getPosition() { return position(); }
		inline CtVector3& script_getDirection(int dir) { return row[dir]; }
		inline void script_transformVector(CtVector3& in, Vector3& out) { return transformVector(in, out); }
		inline void script_transformPoint(CtVector3& in, Vector3& out) { return transformPoint(in, out); }
		inline bool script_isEqualDir(SceneTransform& comp, int dir) { return comp.getDirection((SceneDirectionEnum) dir).equals(getDirection((SceneDirectionEnum) dir)); }
		inline void script_simpleInvTransformVector(CtVector3& in, Vector3& out) { return simpleInvTransformVector(in, out); }
		//inline void script_simpleInvTransformPoint(CtVector3& in, Vector3& out) { return simpleInvTransformPoint(in, out); }
		inline void script_invTransformVector(CtVector3& in, Vector3& out) { return invTransformVector(in, out); }
		inline void script_invTransformPoint(CtVector3& in, Vector3& out) { return invTransformPoint(in, out); }

		static SceneTransform kIdentity;
	};


}

#endif
