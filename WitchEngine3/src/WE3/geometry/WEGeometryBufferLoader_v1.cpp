#include "WEGeometryBufferLoader_v1.h"

namespace WE {

/*
bool GeometryBufferLoader_v1::determinBufferType(DataSourceChunk& chunk, RenderBufferEnum& type) {

	bool found = false;

	if (chunk.hasAttribute(L"facecount") == true) {

		type = RB_IndexBuffer;
		found = true;

	} else if (chunk.hasAttribute(L"vertexcount") == true) {

		type = RB_VertexBuffer;
		found = true;
	}

	return found;
}

bool GeometryBufferLoader_v1::load(GeometryBuffer& buffer, DataSourceChunk& chunk, RenderBufferEnum* pCheckType) {

	RenderBufferEnum type;

	if (determinBufferType(chunk, type) == false) {

		return false;
	}

	if (pCheckType && type != *pCheckType) {

		return false;
	}

	if (type == RB_IndexBuffer) {

		return loadIndexBuffer(buffer, chunk);
	} else {

		return loadVertexBuffer(buffer, chunk);
	}
}
*/

}