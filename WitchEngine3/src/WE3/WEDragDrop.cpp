#include "WEDragDrop.h"

#include "WEMacros.h"
#include "WEMem.h"

using namespace WE;

DragDrop::DragDrop() {
	mFct = NULL;
}

void DragDrop::Register(HWND hWnd, OnDragDropFile fct) {
	OleInitialize(NULL);
	RegisterDragDrop(hWnd, this);

	mFct = fct;
}

STDMETHODIMP DragDrop::QueryInterface(REFIID riid, LPVOID* ppv)
{
	HRESULT hr = E_NOINTERFACE;
	*ppv = NULL;

	if (IID_IUnknown == riid)
	{
		*ppv = this;
	}
	
	else if (IID_IDropTarget == riid)
	{
		*ppv = this;
	}
	
	if (NULL != *ppv)
	{
		// We've handed out a pointer to an interface so obey the COM rules
		//   and AddRef its reference count.
		((LPUNKNOWN)*ppv)->AddRef();
		hr = NOERROR;
	}
	
	return (hr);
}

STDMETHODIMP_(ULONG) DragDrop::AddRef(VOID)
{
	return (0);
}

STDMETHODIMP_(ULONG) DragDrop::Release(VOID)
{
	return (0);
}


STDMETHODIMP DragDrop::DragEnter( IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
	return (0);
}

STDMETHODIMP DragDrop::DragOver( DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
	return (0);
}

STDMETHODIMP DragDrop::DragLeave( VOID)
{
	return (0);
}

STDMETHODIMP DragDrop::Drop( IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
	if (mFct == NULL) {
		return S_OK;
	}
	
	FORMATETC Format;
    STGMEDIUM StorageMedium;

	Format.cfFormat = CF_HDROP;
	Format.ptd = NULL;
	Format.dwAspect = DVASPECT_CONTENT;
	Format.lindex = -1;
	Format.tymed = TYMED_HGLOBAL;

	if( pDataObj->GetData(&Format ,&StorageMedium) != S_OK) {
		return(E_INVALIDARG);
	}
	
	HDROP DropFiles = (HDROP) GlobalLock(StorageMedium.hGlobal);

	if (DropFiles != NULL) {
		UINT FileCount = DragQueryFile(DropFiles,  0xFFFFFFFF, NULL, 0);

		if (FileCount == 1) {
			UINT BufferSize = MAX_PATH + 2;
			WCHAR* Buffer;
			MMemNew(Buffer, WCHAR[BufferSize]);

			BufferSize = DragQueryFile(DropFiles, 0, Buffer, BufferSize);

			if (BufferSize > MAX_PATH + 2) {
				MMemDeleteArray(Buffer);
				WCHAR* Buffer;
				MMemNew(Buffer, WCHAR[BufferSize]);

				BufferSize = DragQueryFile(DropFiles, 0, Buffer, BufferSize);
			}

			mFct(Buffer);

		} 
	}
	
	GlobalUnlock(StorageMedium.hGlobal);
	ReleaseStgMedium(&StorageMedium);

	
	return (S_OK);
}
