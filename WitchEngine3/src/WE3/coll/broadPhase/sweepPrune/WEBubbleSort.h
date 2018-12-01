#ifndef _WEBubbleSort_h
#define _WEBubbleSort_h

namespace WE {

	enum BubbleSortOp {
		Bubble_Forward = 0, Bubble_Back
	};


	template<class T>
	void bubbleSortAddToSortListBeginning(T* pLink, T*& pFirst) {

		pLink->pPrev = NULL;
		pLink->pNext = pFirst;
		pFirst = pLink;
	};


	template<class T>
	void bubbleSortSwap(T* p[4]) {

		if (p[0]) {
			p[0]->pNext = p[2];
		}
		p[2]->pPrev = p[0];

		p[1]->pNext = p[3];
		if (p[3]) {
			p[3]->pPrev = p[1];
		}

		p[2]->pNext = p[1];
		p[1]->pPrev = p[2];
	}


	template<class T>
	void bubbleSortRemove(T* pLink, T*& pFirst) {

		T* p[3];

		if (pLink == pFirst) {
			pFirst = pLink->pNext;
		}

		p[0] = pLink->pPrev;
		p[1] = pLink;
		p[2] = pLink->pNext;

		if (p[0]) {
			p[0]->pNext = p[2];
		}
		if (p[2]) {
			p[2]->pPrev = p[0];
		}
	}



	template<class T>
	void bubbleSortMove(T* pLink, BubbleSortOp op) {

		T* p[4];

		switch (op) {
			case Bubble_Forward:
				p[0] = pLink->pPrev;
				p[1] = pLink;
				p[2] = pLink->pNext;
				p[3] = pLink->pNext->pNext;
				break;
			case Bubble_Back:
				p[0] = pLink->pPrev->pPrev;
				p[1] = pLink->pPrev;
				p[2] = pLink;
				p[3] = pLink->pNext;
				break;
		}

		bubbleSortSwap<T>(p);
	}


	template<class T>
	void bubbleSort(T* pLink, T*& pFirst, bool& moved) {

		moved = false;

		while(pLink->pNext) {

			if (pLink->sortValue > pLink->pNext->sortValue) {

				if (pFirst == pLink) {
					pFirst = pLink->pNext;
				}

				bubbleSortMove<T>(pLink, Bubble_Forward);

				moved = true;
			} else {
				break;
			}
		}

		//we already moved forward so we can stop it here
		if (moved) {
			return;
		}

		while(pLink->pPrev) {

			if (pLink->sortValue < pLink->pPrev->sortValue) {

				if (pFirst == pLink->pPrev) {
					pFirst = pLink;
				}

				bubbleSortMove<T>(pLink, Bubble_Back);

				moved = true;
			} else {
				break;
			}
		}

	};

};

#endif