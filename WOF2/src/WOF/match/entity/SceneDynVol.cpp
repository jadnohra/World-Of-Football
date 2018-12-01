#include "SceneDynVol.h"

#include "../Match.h"

namespace WOF { namespace match {

SceneDynVol::SceneDynVol() {
}

SceneDynVol::~SceneDynVol() {

	if (mNodeMatch.isValid()) {

		mNodeMatch->getCollEngine().destroy(mNodeMatch, *this, mVolColl);
	}
}

bool SceneDynVol::create(Match& match, Object* pRefObject, const TCHAR* name, const WorldMaterialID& collMaterial, const Vector3& pos, VolumeStruct& refVolume, ObjectType objType) {

	bool initSuccess = true;

	initSuccess = initSuccess && nodeInit(match, objType, match.genNodeId(), name);

	if (initSuccess == false) {

		return false;
	}

	mRefObject = pRefObject;

	{
		CollEngine& collEngine = mNodeMatch->getCollEngine();

		mRefVolume.init(refVolume);

		collEngine.init(mNodeMatch, *this, mVolColl, collMaterial);

		translate(mRefVolume, pos, mVolColl.volume);
		collEngine.add(mNodeMatch, *this, mVolColl, mVolColl.volume);
	}

	return true;
}


void SceneDynVol::update(const Vector3& pos) {

	CollEngine& collEngine = mNodeMatch->getCollEngine();

	Volume& newVol = collEngine.updateStart(mNodeMatch, *this, mVolColl);
	translate(mRefVolume, pos, newVol);				
	collEngine.updateEnd(mNodeMatch, *this, mVolColl, newVol);
}

void SceneDynVol::render(Renderer& renderer, const bool& cleanIfNeeded, const bool& flagExtraRenders, RenderPassEnum pass) {

	if (flagExtraRenders && (pass == RP_Normal)) {

		renderer.drawVol(mVolColl.volume, &Matrix4x3Base::kIdentity, &RenderColor::kBlue);
	}
}

void SceneDynVol::addSweptBallContacts(WorldMaterialManager& matManager, 
											const WorldMaterialID& sphereMat, const float& radius, 
											const Vector3& pos, const Vector3& sweepVector, 
											SweptIntersectionArray& intersections, bool executeMaterials) {

	mVolColl.addSweptBallContacts(this,matManager, sphereMat, radius, pos, sweepVector, intersections, executeMaterials);

}
	


SceneStaticVol::SceneStaticVol() {
}

SceneStaticVol::~SceneStaticVol() {

	if (mNodeMatch.isValid()) {

		mNodeMatch->getCollEngine().destroy(mNodeMatch, *this, mVolColl);
	}
}

bool SceneStaticVol::create(Match& match, Object* pRefObject, const TCHAR* name, const WorldMaterialID& collMaterial, VolumeStruct& volume, const bool& registerWithCollEngine, ObjectType objType) {

	bool initSuccess = true;

	initSuccess = initSuccess && nodeInit(match, objType, match.genNodeId(), name);

	if (initSuccess == false) {

		return false;
	}

	mRefObject = pRefObject;

	{
		CollEngine& collEngine = mNodeMatch->getCollEngine();

		collEngine.init(mNodeMatch, *this, mVolColl, collMaterial);

		mVolColl.volume.init(volume);

		if (registerWithCollEngine)
			collEngine.add(mNodeMatch, *this, mVolColl, mVolColl.volume);
	}

	return true;
}


void SceneStaticVol::render(Renderer& renderer, const bool& cleanIfNeeded, const bool& flagExtraRenders, RenderPassEnum pass) {

	if (flagExtraRenders && (pass == RP_Normal)) {

		renderer.drawVol(mVolColl.volume, &Matrix4x3Base::kIdentity, &RenderColor::kBlue);
	}
}

void SceneStaticVol::addSweptBallContacts(WorldMaterialManager& matManager, 
											const WorldMaterialID& sphereMat, const float& radius, 
											const Vector3& pos, const Vector3& sweepVector, 
											SweptIntersectionArray& intersections, bool executeMaterials) {

	mVolColl.addSweptBallContacts(this,matManager, sphereMat, radius, pos, sweepVector, intersections, executeMaterials);

}



} }