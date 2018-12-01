#include "CollRecord.h"

#include "CollContact.h"

namespace WOF { namespace match {

void CollRecordTri::render(Renderer& renderer, const RenderColor* pTriColor, bool triWireFrame) {

	renderer.draw(*tri, &Matrix4x3Base::kIdentity, pTriColor, false, triWireFrame);
}


void CollRecordVol::render(Renderer& renderer, bool renderVolumes, const RenderColor* pVolColor) {

	renderer.drawVol(vol, &Matrix4x3Base::kIdentity, pVolColor);
}

} }