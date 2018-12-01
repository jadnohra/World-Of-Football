#include "WED3D9DeviceSelection.h"


namespace WE {

int extractBackBufferFormatColorDepth(D3DFORMAT& format) {

	switch (format) {
		case D3DFMT_A2R10G10B10:
			return 30;
		case D3DFMT_A8R8G8B8:
			return 24;
		case D3DFMT_X8R8G8B8:
			return 24;
		case D3DFMT_A1R5G5B5:
			return 15;
		case D3DFMT_X1R5G5B5:
			return 15;
		case D3DFMT_R5G6B5:
			return 16;
	}

	assrt(false);
	return 0;
}


bool D3D9DeviceSelection::isDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, 
                                  D3DFORMAT BackBufferFormat, bool bWindowed, VideoDeviceSelectionContext* pContext )
{
    // Typically want to skip backbuffer formats that don't support alpha blending
    IDirect3D9* pD3D = DXUTGetD3DObject(); 

    if (pContext) {

		bool acceptable = true;
		VideoDeviceSelectionResult result;
		memset(&(result), 0, sizeof(VideoDeviceSelectionResult));

		if( pContext->requireAlphaBlending 
			&&
			FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                    AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, 
					D3DRTYPE_TEXTURE, BackBufferFormat ) ) ) {
			
						return false;
		}

		int colDepth = extractBackBufferFormatColorDepth(BackBufferFormat);

		if (pContext->maxColorDepth > 0) {

			if (colDepth > pContext->maxColorDepth) {

				return false;
			}
		}
		

		if (pContext->minStencilBitDepth > 0) {

			//here add better support for depth format
			//the autoDepthBuffer format doesnt work correctly in DXUT it seems

			D3DFORMAT depthFormat;

			//if (colDepth >= 24) {

				depthFormat = D3DFMT_D24S8;

				if (pContext->minStencilBitDepth > 8) {

					return false;
				}

			//} else {

			//	depthFormat = D3DFMT_D15S1;

			//	if (pContext->minStencilBitDepth > 1) {

			//		return false;
			//	}
			//}

			if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal,
										pCaps->DeviceType,  
										AdapterFormat,  
										D3DUSAGE_DEPTHSTENCIL, 
										D3DRTYPE_SURFACE,
										depthFormat ) ) ) {

				depthFormat = D3DFMT_D15S1;

				if (pContext->minStencilBitDepth > 1) {

					return false;
				}

				if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal,
										pCaps->DeviceType,  
										AdapterFormat,  
										D3DUSAGE_DEPTHSTENCIL, 
										D3DRTYPE_SURFACE,
										depthFormat ) ) ) {
					return false;
				} 
			}
		}



		if (pContext->allowRefDevice == false) {
			if (pCaps->DeviceType != D3DDEVTYPE_HAL) {
				acceptable = false;
			}
		}

		if (pContext->requireVertexBlending) {

			result.vertexProcessing = D3DCREATE_HARDWARE_VERTEXPROCESSING;
			if (pCaps->MaxVertexBlendMatrixIndex < 256) {

				if (pContext->allowNoPaletteVertexBlending) {
			
					if (pCaps->MaxVertexBlendMatrices < 4) {

						result.vertexProcessing = D3DCREATE_MIXED_VERTEXPROCESSING;
						if (pContext->allowSoftwareVertexBlending == false) {
							acceptable = false;
						}
					} 

				} else {

					result.vertexProcessing = D3DCREATE_MIXED_VERTEXPROCESSING;
					if (pContext->allowSoftwareVertexBlending == false) {
						acceptable = false;
					}

				}

			} 
		}

		if (pContext->tryAntiAlias && (pContext->antiAliasSamples >= 2) && (pContext->antiAliasSamples <= 16)) {


			D3DMULTISAMPLE_TYPE sampleType = (D3DMULTISAMPLE_TYPE) pContext->antiAliasSamples;

			if( FAILED(pD3D->CheckDeviceMultiSampleType( pCaps->AdapterOrdinal, 
									pCaps->DeviceType, BackBufferFormat, 
									FALSE, sampleType, NULL ) ) ) {

				acceptable = false;
			} else {

				//debugBreak();
			}

			/*
			if (acceptable) {

				if (FAILED(pD3D->CheckDeviceMultiSampleType( pCaps->AdapterOrdinal, 
									pCaps->DeviceType, DepthBufferFormat, 
									FALSE, sampleType, NULL ) ) ) {

					acceptable = false;
				}
			}
			*/
		}

		if (pContext->requireDepthBias) {

			if (!(pCaps->RasterCaps & D3DPRASTERCAPS_DEPTHBIAS))
				acceptable = false;
		}
	
		if (acceptable) {

			memcpy(&pContext->result, &result, sizeof(VideoDeviceSelectionResult));
		}

		return acceptable;
	
	}
		    	
    return true;
}


bool D3D9DeviceSelection::modifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, VideoDeviceSelectionContext* pContext )
{

	if (pContext) {

		pDeviceSettings->pp.PresentationInterval = pContext->vSync ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;
	
		/*
		pDeviceSettings->pp.MultiSampleType = D3DMULTISAMPLE_NONMASKABLE;
		pDeviceSettings->pp.MultiSampleQuality = 2;
		*/

		//improve this for more detailed stencil format requirements
		if (pContext->minStencilBitDepth > 0) {

			pDeviceSettings->pp.EnableAutoDepthStencil = TRUE;

			switch (pDeviceSettings->pp.AutoDepthStencilFormat) {
				case D3DFMT_D16: {

						if (pContext->modifySettingsAlternativeIndex == -1) {

							pContext->hasModifySettingsAlternatives = true;
							pDeviceSettings->pp.AutoDepthStencilFormat = D3DFMT_D24S8;

						} else {

							pContext->hasModifySettingsAlternatives = false;
							pDeviceSettings->pp.AutoDepthStencilFormat = D3DFMT_D15S1;
						}
					}
					break;
				case D3DFMT_D24X8:
					pDeviceSettings->pp.AutoDepthStencilFormat = D3DFMT_D24S8;
					break;
				case D3DFMT_D32:
					pDeviceSettings->pp.AutoDepthStencilFormat = D3DFMT_D24S8;
					break;
			}
		}

		pDeviceSettings->BehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
		pDeviceSettings->BehaviorFlags &= ~D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		pDeviceSettings->BehaviorFlags &= ~D3DCREATE_MIXED_VERTEXPROCESSING;

		pDeviceSettings->BehaviorFlags |= pContext->result.vertexProcessing;
	}

	

    return true;
}

}