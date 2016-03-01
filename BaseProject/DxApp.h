//  ---------------------------------------------------------------------------
//  Department of Computer Science, University of Hull
//  DirectX 11 framework, Aug 2012
//
//  ---------------------------------------------------------------------------

#pragma once
#include <d3d11.h>
#include <d3dx11Effect.h>
#include <dxerr.h>
#include <xnamath.h>
#include <cassert>
#include <vector>

class DxApp {
private:
	static DxApp* _instance;
	HINSTANCE _hApp;
	HWND _hWnd;

protected:
	ID3D11Device *_dxDev;
	ID3D11DeviceContext *_dxImmedDC;
	IDXGISwapChain *_swapChain;
	DXGI_SWAP_CHAIN_DESC _swapChainDesc;
	
	ID3D11RenderTargetView *_renderTargetView;
	ID3D11DepthStencilView *_depthStencilView;
	ID3D11DepthStencilState *_depthStencilStateLess;
	ID3D11DepthStencilState *_depthStencilStateEqual;
	D3D11_TEXTURE2D_DESC _depthStencilDesc;

	float blendFactor[4];								// NEW
	ID3D11BlendState *blendState;						// NEW
	ID3D11DepthStencilState* m_depthStencilStateLess;	// NEW
	ID3D11DepthStencilState* m_depthStencilStateEqual;	// NEW

private:
	static LRESULT CALLBACK MessageProc(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam);
	
	HRESULT initialiseDxDevice();
	HRESULT createDepthStencilView();
	HRESULT createRenderTargetView();

	HRESULT createBlendState();							// NEW
	HRESULT createDepthStencilStates();					// NEW
	void setupViewport();

	void setViewportValues();

protected:
	bool loadEffects(const char* filename, std::vector<char> &compiledShader);

public:
	DxApp();
	virtual ~DxApp();

	virtual bool initialise(HINSTANCE instance, int cmdShow);
	int run();

	virtual void onKey(WPARAM);

	virtual void renderScene() = 0;
	virtual void updateScene() = 0;



	virtual HRESULT onResize(int width, int height);

	static HINSTANCE hApp() { assert(_instance); return _instance->_hApp; }
	static HWND hWnd() { assert(_instance); return _instance->_hWnd; }
};

// Debug trace macro for DirectX
#ifdef _DEBUG
	#ifndef HR
		#define DXCHECK(x)										\
		{														\
			HRESULT hr = (x);									\
			if (FAILED(hr)) {									\
				DXTrace(__FILE__,__LINE__, hr, L#x, true);		\
			}													\
		}
	#endif
#else
	#ifndef HR
		#define DXCHECK(x) (x)
	#endif
#endif

// Debug trace macro for DirectX
#ifdef _DEBUG
	#ifndef HRRETURN
		#define DXRETURN(x)										\
		{														\
			HRESULT hr = (x);									\
			if (FAILED(hr)) {									\
				DXTrace(__FILE__,__LINE__, hr, L#x, true);		\
				return hr;										\
			}													\
		}
	#endif
#else
	#ifndef HR
		#define DXRETURN(x)										\
		{														\
			HRESULT hr = (x);									\
			if (FAILED(hr)) {									\
				return hr;										\
			}													\
		}
	#endif
#endif