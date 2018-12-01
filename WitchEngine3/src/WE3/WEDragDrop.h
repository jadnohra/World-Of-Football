#ifndef _WEDragDrop_h
#define _WEDragDrop_h

#include "oleidl.h"

namespace WE {
	

	typedef void (*OnDragDropFile)(WCHAR* path);

	class DragDrop : public IDropTarget {
		
		public:

		OnDragDropFile mFct;

		DragDrop();

		void Register(HWND hWnd, OnDragDropFile fct);

		//IDropTarget Fctions
		STDMETHODIMP QueryInterface(REFIID, LPVOID*);
		STDMETHODIMP_(ULONG) AddRef(VOID);
		STDMETHODIMP_(ULONG) Release(VOID);

		STDMETHODIMP DragEnter( IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
		STDMETHODIMP DragOver( DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
		STDMETHODIMP DragLeave( VOID);
		STDMETHODIMP Drop( IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);

	};

};

#endif