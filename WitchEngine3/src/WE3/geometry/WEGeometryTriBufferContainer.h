#ifndef _WEGeometryTriBufferContainer_h
#define _WEGeometryTriBufferContainer_h

#include "WEGeometryTriBuffer.h"

namespace WE {

	class GeometryTriBufferContainer {
	public:

		typedef GeometryTriBuffer TriBuffer;
		typedef TriBuffer::Index Index;

	public:

		void render(Renderer& renderer, const RenderColor* pColor = NULL);
		void simplify(const TriBuffer::Simplification* pSettings);

		void addToVol(AAB& vol);

	protected:

		typedef WETL::PtrArrayEx<TriBuffer, Index> TriBuffers;
		
		TriBuffers mTriBuffers;
		Index mTriCount;


	public:

		GeometryTriBufferContainer();

		inline void destroy() { mTriBuffers.destroy(); }

		inline void reserve(const Index& count) { }
		inline TriBuffer& addOne() { return dref(mTriBuffers.addNewOne()); }

		inline const Index& getCount() { return mTriBuffers.size; }
		inline TriBuffer& at(const Index& i) { return dref(mTriBuffers.el[i]); }

		Index getTriCount();
	};

}


#endif