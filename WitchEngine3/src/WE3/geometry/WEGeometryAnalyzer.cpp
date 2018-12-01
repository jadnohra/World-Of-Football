#include "WEGeometryAnalyzer.h"


namespace WE {

void GeometryAnalyzer::addToVolume(GeometryVertexBuffer& VB, GeometryIndexBuffer* pIB, AAB& volume) {

	bool byFace = (pIB != NULL);

	if (byFace) {

		if (pIB->getElementCount() == 0) {

			byFace = false;
		}
	} 

	if (VB.getElementCount() == 0) {

		return;
	}

	const RenderElementProfile& vprofile = VB.getProfile();
	RenderUINT vertexCount = VB.getElementCount();
	void* vb = VB.bufferPtr();

	RenderSubElIndex el_position = vprofile.findSub(D3DDECLUSAGE_POSITION);

	if (el_position < 0) {

		return;
	}

	float* pos;
	
	if (byFace) {

		UINT32 faceIndices[3];

		RenderUINT faceCount = pIB->getElementCount() / 3;
		const RenderElementProfile& iprofile = pIB->getProfile();
		void* ib = pIB->bufferPtr();

		for (RenderUINT f = 0, i = 0; f < faceCount; f++, i += 3) {

			iprofile.face_get(ib, i, faceIndices);
			
			for (short j = 0; j < 3; j++) {

				pos = vprofile.getF(vb, faceIndices[j], el_position);
				volume.merge(pos);
			}
		}

	} else {

		for (RenderUINT i = 0; i < vertexCount; i ++) {

			pos = vprofile.getF(vb, i, el_position);
			volume.merge(pos);
		}
	}
}


void GeometryAnalyzer::toCollFaceArray(GeometryVertexBuffer& VB, GeometryIndexBuffer* pIB, StaticCollFaceArray& faceArray, StaticCollVertexArray* pVertexArray, const Matrix4x3Base* pTransf) {

	bool byFace = true;

	if (pIB == NULL) {

		assrt(false);
		return;
	}

	GeometryIndexBuffer& IB = *pIB;

	/*
	bool byFace = (pIB != NULL);

	if (byFace) {

		if (pIB->getElementCount() == 0) {

			byFace = false;
		}
	} 
	
	if (VB.getElementCount() == 0) {

		return;
	}
	*/

	if ((VB.getElementCount() == 0) || (IB.getElementCount() == 0)) {

		faceArray.destroy();
		return;
	}

	const RenderElementProfile& vprofile = VB.getProfile();
	RenderUINT vertexCount = VB.getElementCount();
	void* vb = VB.bufferPtr();

	RenderSubElIndex el_position = vprofile.findSub(D3DDECLUSAGE_POSITION);

	if (el_position < 0) {

		faceArray.destroy();
		return;
	}

	float* pos;
	Point points[3];
	
	if (byFace) {

		RenderUINT faceCount = IB.getElementCount() / 3;
		const RenderElementProfile& iprofile = IB.getProfile();
		void* ib = IB.bufferPtr();

		faceArray.resize(faceCount);
		
		if (pVertexArray) {

			pVertexArray->resize(vertexCount);
			CollVertex* el = pVertexArray->el;

			for (RenderUINT i = 0; i < vertexCount; i ++) {

				pos = vprofile.getF(vb, i, el_position);
				el[i].set(pos);

				if (pTransf) {
					collTransform(*pTransf, el[i]);
				}
			}

		}

		CollFace* el = faceArray.el;

		for (RenderUINT f = 0, i = 0; f < faceCount; f++, i += 3) {

			iprofile.face_get(ib, i, el[f].indices);
			
			for (short j = 0; j < 3; j++) {

				pos = vprofile.getF(vb, el[f].indices[j], el_position);
				points[j].set(MExpand3(pos));
			}

			el[f].initNormal(points[0], points[1], points[2]);

			if (pTransf) {
				collTransform(*pTransf, el[f]);
			}
		}
	} 

}

}