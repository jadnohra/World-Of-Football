#ifndef _WETLTree_h
#define _WETLTree_h

#include "malloc.h"
#include "stdlib.h"
#include "../WEMacros.h"


namespace WETL {

	
	template <class T>
	class TreeNodeT {
	public:

		T* pParent;

		T* pFirstChild;
		T* pLastChild;

		T* pNextSibling;
		T* pPrevSibling;

	public:

		TreeNodeT() {

			pParent = NULL;

			pFirstChild = NULL;
			pLastChild = NULL;

			pNextSibling = NULL;
			pPrevSibling = NULL;
		}

		~TreeNodeT() {
		}

		void resetTreeNode() {

			pParent = NULL;

			pFirstChild = NULL;
			pLastChild = NULL;

			pNextSibling = NULL;
			pPrevSibling = NULL;
		}

		void addChild(T* pChild) {

			if (pChild->pParent != NULL) {
				pChild->pParent->removeChild(pChild);
			}

			if (pFirstChild == NULL) {
				pFirstChild = pChild;
				pLastChild = pChild;
			} else {
				pLastChild->pNextSibling = pChild;
				pChild->pPrevSibling = pLastChild;
				pLastChild = pChild;
			} 

			pChild->pParent = (T*) this;
		}

		void removeChild(T* pChild) {

			if (pChild->pParent != (T*) this) {
				return;
			}

			if (pChild == pFirstChild) {
				pFirstChild = pChild->pNextSibling;
			}

			if (pChild == pLastChild) {
				pLastChild = pChild->pPrevSibling;
			}

			if (pChild->pPrevSibling != NULL) {
				pChild->pPrevSibling->pNextSibling = pChild->pNextSibling;
				pChild->pPrevSibling = NULL;
			}

			if (pChild->pNextSibling != NULL) {
				pChild->pNextSibling->pPrevSibling = pChild->pPrevSibling;
				pChild->pNextSibling = NULL;
			}

			pChild->pPrevSibling = NULL;
			pChild->pNextSibling = NULL;

			pChild->pParent = NULL;

		}

		void addSibling(T* pSibling) {

			if ((pSibling->pParent != NULL) && (pSibling->pParent != pParent)) {
				pSibling->pParent->removeChild(pSibling);
			}

			pSibling->pParent = pParent;


			if (pNextSibling != NULL) {

				pSibling->pPrevSibling = (T*) this;
				pSibling->pNextSibling = pNextSibling;

				pNextSibling = pSibling;
				pNextSibling->pPrevSibling = pSibling;

			} else {

				pSibling->pPrevSibling = (T*) this;
				pSibling->pNextSibling = NULL;

				pNextSibling = pSibling;
			}

		}

		void removeFromSiblings() {

			if (pParent != NULL) {
				pParent->removeChild((T*) this);
				return;
			}

			if (pNextSibling != NULL) {
				pNextSibling->pPrevSibling = pPrevSibling;
			}

			if (pPrevSibling != NULL) {
				pPrevSibling->pNextSibling = pNextSibling;
			}

			pPrevSibling = NULL;
			pNextSibling = NULL;

		}

		int debug(int id = 0, int ind = 0) {

			static char buff[16];

			itoa(id, buff, 10);
			
			int _ind = ind;

			while(_ind) {
				OutputDebugStringA(" ");
				_ind--;
			}

			OutputDebugStringA(buff);
			OutputDebugStringA("\n");

			if (pFirstChild) {
				id = pFirstChild->debug(id + 1, ind + 1);
			}

			if (pNextSibling) {
				id = pNextSibling->debug(id + 1, ind);
			}

			return id;
		}

	};

	class TreeNode : TreeNodeT<TreeNode> {
	};

}

#endif