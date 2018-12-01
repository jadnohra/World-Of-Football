#include "WEMeshRenderTreeOptimizer2.h"

#include "../WETL/WETLTree.h"
#include "../WEMacros.h"

namespace WE {

struct rto_RenderStateNode : WETL::TreeNodeT<rto_RenderStateNode> {

	const MeshRenderState* pRS;

	//has 2 or more children
	bool isBranching() {

		return !(pFirstChild == pLastChild);
	}

	bool isNonBranching() {

		return (pFirstChild == pLastChild);
	}

	void swapWith(rto_RenderStateNode* pSwap) {

		swapt<const MeshRenderState*>(pRS, pSwap->pRS);
	}
};

typedef WETL::IndexShort rto_RenderStateNodeIndex;
typedef WETL::CountedArray<rto_RenderStateNode, false, rto_RenderStateNodeIndex::Type> rto_RenderStateNodeArray;

struct rto_RenderStateTree {

	rto_RenderStateNode root;
	rto_RenderStateNodeArray treeArray;


	rto_RenderStateTree() {

		root.pRS = NULL;
	}

	~rto_RenderStateTree() {
	}

	rto_RenderStateNode& addNode() {

		return treeArray.addOne();
	}
};

typedef WETL::CountedArray<bool, false, MeshRSIndex::Type> rto_MarkArray;

struct rto_Context {

	//shared array
	MeshRSIndex markedCount;
	rto_MarkArray markArray;
	
	//shared array
	MeshRenderStateArray RSArray;
};

struct rto_FaceInfo {

	const MeshFaceContext* pFaceContext;
	MeshFace2* pFace;

	rto_Context* pContext;
	
	rto_FaceInfo(MeshFace2* _pFace = NULL, const MeshFaceContext* _pFaceContext = NULL, 
					rto_Context* _pContext = NULL) {

		init(_pFace, _pFaceContext, _pContext);
	}

	void init(MeshFace2* _pFace, const MeshFaceContext* _pFaceContext, rto_Context* _pContext) {

		pContext = _pContext;

		pFaceContext = _pFaceContext;
		pFace = _pFace;

		initRenderStateArray();
		initMarkArray();
	}	

	void initRenderStateArray() {

		pContext->RSArray.count = 0;
		pFace->extractRenderStatesTo(*pFaceContext, pContext->RSArray);
	}

	void initMarkArray() {

		pContext->markedCount = 0;

		MeshRSIndex::Type count = getRenderStateCount();

		pContext->markArray.count = 0;
		pContext->markArray.reserve(count);
		
		for (MeshRSIndex i = 0; i < count; i++) {

			pContext->markArray.addOne(false);
		}
	}

	MeshRSIndex::Type getRenderStateCount() {

		return pContext->RSArray.count;
	}

	inline const MeshRenderState* getRenderState(MeshRSIndex::Type index) {

		return pContext->RSArray.el[index];
	}

	void mark(MeshRSIndex::Type index) {

		pContext->markArray.el[index] = true;
		pContext->markedCount++;
	}

	inline bool getMarked(MeshRSIndex::Type index) {

		return pContext->markArray.el[index];
	}

	bool allMarked() {

		return pContext->markedCount == getRenderStateCount();
	}

	inline MeshRSIndex::Type getMarkedCount() {

		return pContext->markedCount;
	}

	MeshRSIndex::Type getUnmarkedCount() {

		return getRenderStateCount() - pContext->markedCount;
	}
};


struct rto_BranchFaceFitInfo {

	MeshRSIndex equalCount;
	MeshRSIndex equalTypeCount;
	MeshRSIndex differentCount;

	void zero() {

		equalCount = 0;
		equalTypeCount = 0;
		differentCount = 0;
	}

	static int __cdecl qsort_compare(const void* _e1, const void* _e2, bool asc) {
		//< 0 if elem1 less than elem2
		rto_BranchFaceFitInfo& e1 = *((rto_BranchFaceFitInfo*) _e1);
		rto_BranchFaceFitInfo& e2 = *((rto_BranchFaceFitInfo*) _e2);
		
		if (e1.equalCount < e2.equalCount) { 
			return asc ? -1 : 1;
		} else if (e1.equalCount > e2.equalCount) { 
			return asc ? 1 : -1;
		} 

		if (e1.equalTypeCount < e2.equalTypeCount) { 
			return asc ? -1 : 1;
		} else if (e1.equalTypeCount > e2.equalTypeCount) { 
			return asc ? 1 : -1;
		} 

		if (e1.differentCount < e2.differentCount) { 
			return asc ? 1 : -1;
		} else if (e1.differentCount > e2.differentCount) { 
			return asc ? -1 : 1;
		} 

		return 0;
	}

	static int __cdecl qsort_compare_asc(const void* e1, const void* e2) {
		return qsort_compare(e1, e2, true);
	}

	static int __cdecl qsort_compare_desc(const void* e1, const void* e2) {
		return qsort_compare(e1, e2, false);
	}
};

//a branch is either:
// 1. a part of tree with zero or more nodes that all have only one child
//      all nodes are movebale
// 2. one node with more than 2 children, unmovable
struct rto_Branch {

	rto_RenderStateNode* pNode;
	rto_RenderStateNodeIndex nodeCount;
	
	rto_BranchFaceFitInfo fitInfo;

	rto_RenderStateNodeIndex::Type getNodeCount() {

		return nodeCount;
	}

	void init(rto_RenderStateNode* _pNode) {

		pNode = _pNode;

		nodeCount = 0;

		rto_RenderStateNode* pBrowseNode = pNode;

		while (pBrowseNode && pBrowseNode->isNonBranching()) {

			nodeCount++;

			pBrowseNode = pBrowseNode->pFirstChild;
		}

		if (nodeCount > 1) {
			
			//if last node is branching remove it

			if (pBrowseNode && pBrowseNode->isBranching()) {

				nodeCount--;
			}
		}
	}

	bool toNextNode(rto_RenderStateNode*& pCurr, rto_RenderStateNodeIndex::Type& index) {

		if (index >= nodeCount) {

			pCurr = NULL;

		} else {

			index++;

			if (pCurr == NULL) {
				
				pCurr = pNode;

			} else {
			
				pCurr = pCurr->pFirstChild;
			}
		}

		return (pCurr != NULL);
	}

	rto_RenderStateNode* browseToIndex(rto_RenderStateNodeIndex::Type index) {

		rto_RenderStateNodeIndex::Type i = 0;
		rto_RenderStateNode* pNode = NULL;

		while (toNextNode(pNode, i)) {

			if (i == index + 1) {

				return pNode;
			}
		}

		return NULL;
	}


	static int __cdecl qsort_fitCompare_asc(const void* e1, const void* e2) {
		//< 0 if elem1 less than elem2
		return rto_BranchFaceFitInfo::qsort_compare_asc(
			&(((rto_Branch*) e1)->fitInfo), &(((rto_Branch*) e2)->fitInfo));
	}

	static int __cdecl qsort_fitCompare_desc(const void* e1, const void* e2) {
		//< 0 if elem1 less than elem2
		return rto_BranchFaceFitInfo::qsort_compare_desc(
			&(((rto_Branch*) e1)->fitInfo), &(((rto_Branch*) e2)->fitInfo));
	}
};

typedef WETL::IndexByte rto_BranchIndex;
typedef WETL::CountedArray<rto_Branch, true, rto_BranchIndex::Type> rto_BranchArray;

void rto_getBranches(rto_RenderStateNode& root, rto_BranchArray& branches, bool resetCount) {

	if (resetCount) {

		branches.count = 0;
	}

	rto_RenderStateNode* pNode = root.pFirstChild;

	while (pNode) {

		rto_Branch& branch = branches.addOne();
		branch.init(pNode);

		pNode = pNode->pNextSibling;
	}
}

void rto_getBranchRSFit(rto_Branch& branch, const MeshRenderState* pState, rto_BranchFaceFitInfo& fitInfo) {

	rto_RenderStateNode* pNode = NULL;
	rto_RenderStateNodeIndex::Type index = 0;

	while (branch.toNextNode(pNode, index)) {
	
		switch (pNode->pRS->compare(*pState)) {
			case RSC_Equal:
				fitInfo.equalCount++;
				break;
			case RSC_EqualType:
				fitInfo.equalTypeCount++;
				break;
			default:
				fitInfo.differentCount++;
				break;
		}
	}
}

void rto_getBranchFaceFit(rto_Branch& branch, rto_FaceInfo& faceInfo, rto_BranchFaceFitInfo& fitInfo, bool resetFitInfo) {

	if (resetFitInfo) {
		
		fitInfo.zero();
	}
	
	for (MeshRSIndex i = 0; i < faceInfo.getMarkedCount(); i++) {

		if (faceInfo.getMarked(i) == false) {
			
			rto_getBranchRSFit(branch, faceInfo.getRenderState(i), fitInfo);
		}
	}
}

void rto_getBranchArrayFaceFit(rto_BranchArray& branches, rto_FaceInfo& faceInfo) {

	for (rto_BranchIndex b = 0; b < branches.count; b++) {

		rto_getBranchFaceFit(branches.el[b], faceInfo, branches.el[b].fitInfo, true);
	}
}

void rto_sortBranchArrayByFit(rto_BranchArray& branches, bool asc) {

	if (asc) {

		qsort(branches.el, branches.count, sizeof(rto_Branch), rto_Branch::qsort_fitCompare_asc);
	} else {

		qsort(branches.el, branches.count, sizeof(rto_Branch), rto_Branch::qsort_fitCompare_desc);
	}
}

void rto_appendFaceToNode(rto_RenderStateTree& tree, rto_RenderStateNode& node, rto_FaceInfo& faceInfo, bool mark) {

	for (MeshRSIndex i = 0; i < faceInfo.getMarkedCount(); i++) {

		if (faceInfo.getMarked(i) == false) {
			
			rto_RenderStateNode& newNode = tree.addNode();	
			newNode.pRS = faceInfo.getRenderState(i);

			node.addChild(&newNode);

			if (mark) {

				faceInfo.mark(i);
			}
		}
	}
}

bool rto_NodeFindFaceCompType(rto_RenderStateNode* pNode, rto_FaceInfo& faceInfo, MeshRenderStateCompareEnum compType, MeshRSIndex::Type& indexInMesh) {

	for (MeshRSIndex i = 0; i < faceInfo.getMarkedCount(); i++) {

		if (faceInfo.getMarked(i) == false) {
			
			if (faceInfo.getRenderState(i)->compare(*(pNode->pRS)) == compType) {

				indexInMesh = i;
				return true;
			}
		}
	}

	return false;
}


void rto_swapBranchNodes(rto_Branch& branch, rto_RenderStateNodeIndex::Type fromIndex, rto_RenderStateNodeIndex::Type toIndex) {

	if (fromIndex == toIndex) {

		return;
	}

	rto_RenderStateNode* pFromNode = NULL;
	rto_RenderStateNode* pToNode = NULL;

	rto_RenderStateNode* pNode = NULL;
	rto_RenderStateNodeIndex::Type index = 0;

	while (branch.toNextNode(pNode, index)) {

		if (index == fromIndex + 1) {
		
			pFromNode = pNode;

		} else if (index == toIndex + 1) {

			pToNode = pNode;
		} 
	}

	dref<rto_RenderStateNode>(pFromNode).swapWith(pToNode);
}

void rto_swapBranchNodes(rto_Branch& branch, rto_RenderStateNodeIndex::Type fromIndex, rto_RenderStateNode* pFromNode, rto_RenderStateNodeIndex::Type toIndex) {

	if (fromIndex == toIndex) {

		return;
	}
	
	rto_RenderStateNode* pToNode = NULL;

	rto_RenderStateNode* pNode = NULL;
	rto_RenderStateNodeIndex::Type index = 0;

	while (branch.toNextNode(pNode, index)) {

		if (index == toIndex + 1) {

			pToNode = pNode;
		} 
	}

	
	assert(pFromNode != NULL);
	dref<rto_RenderStateNode>(pFromNode).swapWith(pToNode);
}

void rto_moveCompTypesToTop(rto_Branch& branch, rto_FaceInfo& faceInfo, MeshRenderStateCompareEnum compType, 
							bool mark, bool useMaxLimit, rto_RenderStateNodeIndex::Type maxMoveCount, 
							rto_RenderStateNodeIndex::Type& movedCount) {

	rto_RenderStateNode* pNode = NULL;
	rto_RenderStateNodeIndex::Type index = 0;

	movedCount = 0;
	MeshRSIndex::Type indexInMesh;

	while (branch.toNextNode(pNode, index)) {
	
		if (useMaxLimit && (movedCount == maxMoveCount)) {

			return;
		}

		while (rto_NodeFindFaceCompType(pNode, faceInfo, compType, indexInMesh)) {

			rto_swapBranchNodes(branch, index, pNode, movedCount);

			if (mark) {

				faceInfo.mark(indexInMesh);
			}

			movedCount++;
		}
	}
}

void rto_mergeFaceToBranch(rto_RenderStateTree& tree, rto_RenderStateNode& node, rto_Branch& branch, 
						   rto_FaceInfo& faceInfo, bool selectSameTypeBranchOffNode, rto_RenderStateNodeIndex::Type& mergedCount) {

	mergedCount = 0;

	if ((branch.fitInfo.equalCount == 0) && (branch.fitInfo.equalTypeCount == 0)) {

		//rto_appendFaceToNode(tree, node, faceInfo, true);

	} else {

		if (faceInfo.getUnmarkedCount() == branch.fitInfo.equalCount) {

			//fits completely
			return;

		} else {

			rto_RenderStateNodeIndex::Type equalMovedCount;
			rto_RenderStateNodeIndex::Type equalTypeMovedCount;

			if (branch.fitInfo.equalCount) {

				rto_moveCompTypesToTop(branch, faceInfo, RSC_Equal, true, false, 0, equalMovedCount);
				//optionally sort by expense ?
			}

			mergedCount += equalMovedCount;

			if (selectSameTypeBranchOffNode) {

				if (branch.fitInfo.equalTypeCount) {

					//optimize by choosing best equalType bu expense?
					rto_moveCompTypesToTop(branch, faceInfo, RSC_Equal, false, true, 1, equalTypeMovedCount);
					//optionally sort by expense ?
				}
			}
		}
	}
}

void rto_recurseMerge(rto_RenderStateTree& tree, rto_RenderStateNode& node, rto_FaceInfo& faceInfo) {

	if (faceInfo.allMarked()) {

		return;
	}

	rto_BranchArray branches;
	
	rto_getBranches(node, branches, true);

	if (branches.count) {

		rto_getBranchArrayFaceFit(branches, faceInfo);
		rto_sortBranchArrayByFit(branches, false);
	} 

	bool branchFound = true;

	if (branches.count || 
		((branches.el[0].fitInfo.equalCount == 0) && (branches.el[0].fitInfo.equalTypeCount == 0))
		) {

		branchFound = false;
	}

	
	if (branches.count) {

		rto_Branch& branch = branches.el[0];
		rto_RenderStateNodeIndex::Type mergedCount;

		rto_mergeFaceToBranch(tree, node, branch, faceInfo, true, mergedCount);

		rto_RenderStateNodeIndex::Type branchOffIndex = mergedCount;

		if (branchOffIndex == 0) {

			//strange, no equals, shoudlnt have gotten here
			rto_appendFaceToNode(tree, node, faceInfo, true);

		} else {

			branchOffIndex--;

			rto_RenderStateNode* pBranchOffNode = branch.browseToIndex(branchOffIndex);

			//recurse to next branches
			rto_recurseMerge(tree, dref<rto_RenderStateNode>(pBranchOffNode), faceInfo);
		}
		
	} else {

		rto_appendFaceToNode(tree, node, faceInfo, true);
	}
}

void rto_merge(rto_RenderStateTree& tree, rto_RenderStateNode& node, rto_FaceInfo& faceInfo) {

	rto_recurseMerge(tree, node, faceInfo);
}

void rto_createRenderStateTree(const MeshFaceContext& faceContext, StaticMeshFace2Array& faceArray) {

	//for now we select the best looking branch in the 1st level,
	//without considering deeper levels

	rto_Context context;
	rto_FaceInfo faceInfo;
	rto_RenderStateTree tree;
	

	for (MeshFaceIndex::Type f = 0; f < faceArray.size; f++) {
	
		faceInfo.init(&faceArray.el[f], &faceContext, &context);

		rto_merge(tree, tree.root, faceInfo);
	}

}

}

