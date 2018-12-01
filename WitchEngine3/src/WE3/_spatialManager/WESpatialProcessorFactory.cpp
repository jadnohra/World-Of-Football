#include "WESpatialProcessorFactory.h"

#include "../WEDebugBypass.h"

namespace WE {

typedef AbstractContainer::ElementAddress ElementAddress;
typedef AbstractContainer::AddressBatchContext AddressBatchContext;
typedef AbstractContainer::Index ProcessingIndex;


void fct_impl_addToVol_Tri1_AAB(const ProcessingIndex& processingIndex, void* pArg, void* context, const ElementAddress* pAddress, AddressBatchContext* pCtx) {

	SpatialManagerTypeBase::Tri1& arg = dref((SpatialManagerTypeBase::Tri1*) pArg);
	AAB& vol = dref((AAB*) context);

	arg.addTo(vol);
}

void fct_impl_classifyCount_Tri1_Plane(const ProcessingIndex& processingIndex, void* pArg, void* context, const ElementAddress* pAddress, AddressBatchContext* pCtx) {

	SpatialManagerTypeBase::Tri1& arg = dref((SpatialManagerTypeBase::Tri1*) pArg);
	SpatialProcessorFactory::PlaneTriClassifyCount& classif = dref((SpatialProcessorFactory::PlaneTriClassifyCount*) context);

	for (int i = 0; i < classif.planeCount; ++i) {

		classif.counts[i].count[classif.planes[i].classify(arg)]++;
	}
}

void fct_impl_classifyIndexes_Tri1_Plane(const ProcessingIndex& processingIndex, void* pArg, void* context, const ElementAddress* pAddress, AddressBatchContext* pCtx) {

	SpatialManagerTypeBase::Tri1& arg = dref((SpatialManagerTypeBase::Tri1*) pArg);
	SpatialProcessorFactory::PlaneTriClassifyIndexes& classif = dref((SpatialProcessorFactory::PlaneTriClassifyIndexes*) context);

	((PlanePartitionResult*) classif._currResults)[processingIndex] = classif.pPlane->classify(arg);
	classif._currResults = ((BYTE*) classif._currResults) + classif.stride;
}

void fct_impl_split_Tri1_Plane(const ProcessingIndex& processingIndex, void* pArg, void* context, const ElementAddress* pAddress, AddressBatchContext* pCtx) {

	SpatialManagerTypeBase::Tri1& arg = dref((SpatialManagerTypeBase::Tri1*) pArg);
	SpatialProcessorFactory::PlaneTriSplit& split = dref((SpatialProcessorFactory::PlaneTriSplit*) context);

	PlanePartitionResult nodes[2] = {PP_None, PP_None }; 

	switch (split.pPlane->classify(arg)) {

		case PP_Back: {

			nodes[0] = PP_Back;
			++split.resultSummary.count[PP_Back];

		} break;

		case PP_Front: {

			nodes[0] = PP_Front;
			++split.resultSummary.count[PP_Front];

		} break;

		case PP_BackAndFront: {

			nodes[0] = PP_Back;
			nodes[1] = PP_Front;

			++split.resultSummary.count[PP_BackAndFront];

		} break;

		default: {

			assrt(false);

		} break;
	}

	for (int i = 0; i < 2; ++i) {

		if (nodes[i] != PP_None) {

			SpatialProcessorFactory::PlaneTriSplit::SplitNode& node = split.nodes[nodes[i]];

			if (!node.batch.isValid()) {

				node.batch = split.container->createElementAddressBatch();
			}

			node.batch->add(dref(pAddress), pCtx);
		}
	}
}

void fct_impl_Tri1_render(const ProcessingIndex& processingIndex, void* pArg, void* context, const ElementAddress* pAddress, AddressBatchContext* pCtx) {

	SpatialManagerTypeBase::Tri1& arg = dref((SpatialManagerTypeBase::Tri1*) pArg);
	SpatialProcessorFactory::Render& render = dref((SpatialProcessorFactory::Render*) context);

	//DebugBypass* pBypass = getDebugBypass();

	//if (pBypass->currTri == pBypass->renderTri) {

		//render.renderer->draw((const Triangle&) arg, &Matrix4x3Base::kIdentity, &RenderColor::kRed, false, false);

	//} else {

		render.renderer->draw((const Triangle&) arg, &Matrix4x3Base::kIdentity, render.pColor, false, render.wireFrame);
	//}

	//++pBypass->currTri;
}


SpatialProcessorFactory::ElementProcessorInfo* SpatialProcessorFactory::create_addToVol_Tri1(AAB& volume, const SpatialManagerTypeBase& typeBase, ElementProcessorInfo& result) {

	result.init(&fct_impl_addToVol_Tri1_AAB, &volume, typeBase.getType_Tri1());

	return &result;
}

SpatialProcessorFactory::ElementProcessorInfo* SpatialProcessorFactory::create_classify_Tri1(PlaneTriClassifyCount& planeTriClassify, const SpatialManagerTypeBase& typeBase, ElementProcessorInfo& result) {

	result.init(&fct_impl_classifyCount_Tri1_Plane, &planeTriClassify, typeBase.getType_Tri1());

	return &result;
}

SpatialProcessorFactory::ElementProcessorInfo* SpatialProcessorFactory::create_classify_Tri1(PlaneTriClassifyIndexes& planeTriClassify, const SpatialManagerTypeBase& typeBase, ElementProcessorInfo& result) {

	result.init(&fct_impl_classifyIndexes_Tri1_Plane, &planeTriClassify, typeBase.getType_Tri1());

	return &result;
}

SpatialProcessorFactory::ElementProcessorInfo* SpatialProcessorFactory::create_split_Tri1(PlaneTriSplit& planeTriSplit, const SpatialManagerTypeBase& typeBase, ElementProcessorInfo& result) {

	result.init(&fct_impl_split_Tri1_Plane, &planeTriSplit, typeBase.getType_Tri1());

	return &result;
}

SpatialProcessorFactory::ElementProcessorInfo* SpatialProcessorFactory::create_render_Tri1(Render& render, const SpatialManagerTypeBase& typeBase, ElementProcessorInfo& result) {

	result.init(&fct_impl_Tri1_render, &render, typeBase.getType_Tri1());

	return &result;
}

}