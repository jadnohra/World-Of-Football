#ifndef _WECollDataTypes_h
#define _WECollDataTypes_h

#include "../render/WERenderDataTypes.h"
#include "../WEDataTypes.h"
#include "../math/WEBigMatrix.h"

namespace WE {

	typedef RenderUINT CollPrimIndex;
	typedef USHORT CollMaterial;
	typedef IndexT<unsigned short> CollIndex;
	
	/*
	struct CollMask {

		union {
			struct {
				unsigned colliderMask : 15;
				unsigned selfCollide : 1;
			};
			struct {
				UINT16 bitField;
			};
			struct {

				unsigned _mask1 : 1;
				unsigned _mask2 : 1;
				unsigned _mask3 : 1;
				unsigned _mask4 : 1;
				unsigned _mask5 : 1;
				unsigned _mask6 : 1;
				unsigned _mask7 : 1;
				unsigned _mask8 : 1;
				unsigned _mask9 : 1;
				unsigned _mask10 : 1;
				unsigned _mask11 : 1;
				unsigned _mask12 : 1;
				unsigned _mask13 : 1;
				unsigned _mask14 : 1;
				unsigned _mask15 : 1;
				unsigned _maskSelf : 1;
			};
		};

		CollMask() {
		}

		CollMask(const UINT16& val) {

			bitField = val;
		}


		void setFull() {

			bitField = 0xFFFF;
		}

		//uses the whole collierMask to check if its the same object type
		inline bool mask(const CollMask& comp) const {

			return (colliderMask == comp.colliderMask) ? 
					(bool) selfCollide : 
					((colliderMask & comp.colliderMask) != 0);
		}

		//uses objectType to check if its the same object type
		template<class TypeT>
		inline bool mask(const CollMask& comp, const TypeT& type, const TypeT& compType) const {

			return (type == compType) ? 
					(bool) selfCollide : 
					((colliderMask & comp.colliderMask) != 0);
		}

		inline bool maskNoSelfCheck(const CollMask& comp) const {

			return ((colliderMask & comp.colliderMask) != 0);
		}
	};
	*/

	typedef UINT32 CollMaskID;

	struct CollMaskMatrix {

		BigMatrix<bool> matrix;

		//uses the whole collierMask to check if its the same object type
		inline bool mask(const CollMaskID& id1, const CollMaskID& id2) const {

			return matrix.el(id1, id2);
		}
	};

	
	struct CollMask {

		CollMaskID id;

		CollMask() {}
		CollMask(const CollMaskID& _id) : id(_id) {}

		//uses the whole collierMask to check if its the same object type
		inline bool mask(const CollMask& comp, const CollMaskMatrix& matrix) const {

			return matrix.matrix.el(id, comp.id);
		}
	};
	
}

#endif