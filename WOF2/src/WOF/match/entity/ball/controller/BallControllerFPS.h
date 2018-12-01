#ifndef h_WOF_match_BallControllerFPS
#define h_WOF_match_BallControllerFPS

#include "WE3/math/WEVector.h"
using namespace WE;

#include "BallController.h"

namespace WOF { namespace match { 

	class SceneNode;
	
	class BallControllerFPS : public BallController {
	public:

		BallControllerFPS();

		void setVibration(float vibration);
		void setRotation(float rotation);
		float getVibration();
		float getRotation();

		void setNodeAttachOffset(const Vector& offset);
		void attachPositionToNode(SceneNode* pNode);
		bool isAttachedPositionToNode();
		bool isAttachedPositionToNode(SceneNode* pNode);

		void startLoadFiring(const Clock& time);
		void stopLoadFiring(Vector3& vel);
		
		virtual int getControllerType();
		virtual void setPosition(const Point& pos);
		//virtual void attach(Match& match, Ball* pBall);

		virtual bool handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		virtual void frameMove(Match& match, const Clock& time, Time& outEndTime);

	protected:

		SoftPtr<SceneNode> mPosAttachNode;

		bool mIsLoadFiring;
		Time mLoadFireStartTime;
		float mLoadFireRotation;

		float mVibration;
		float mRotation;
		
		Vector3 mAttachOffset;
		Point mPos;
	};

} } 

#endif