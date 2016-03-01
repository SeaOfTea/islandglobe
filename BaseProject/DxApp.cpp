//  ---------------------------------------------------------------------------
//  Department of Computer Science, University of Hull
//  DirectX 11 framework, Aug 2012
//
//  ---------------------------------------------------------------------------

#include "DxApp.h"
#include <fstream>

DxApp* DxApp::_instance = NULL;

DxApp::DxApp() {
	_hApp = 0;
	_hWnd = 0;
	_instance = this;

	_dxDev = NULL;
	_dxImmedDC = NULL;
	_swapChain = NULL;
	_renderTargetView = NULL;
	_depthStencilView = NULL;
}

DxApp::~DxApp() {
	if (_dxImmedDC) _dxImmedDC->ClearState();
	if (_renderTargetView) _renderTargetView->Release();
	if (_depthStencilView) _depthStencilView->Release();
	
	if (_swapChainDesc.Windowed) _swapChain->Release();
	if (_dxImmedDC) _dxImmedDC->Release();
	if (_dxDev) _dxDev->Release();

	//TwTerminate();

	_instance = NULL;
}

bool DxApp::initialise(HINSTANCE hInstance, int cmdShow) {
	_hApp = hInstance;

	// Register our window class
	WNDCLASSEX wcex = { sizeof(WNDCLASSEX), CS_HREDRAW|CS_VREDRAW, MessageProc,
		0L, 0L, _hApp, NULL, NULL, NULL, NULL, L"Tutorial 1", NULL };
	RegisterClassEx(&wcex);

	// Create a window
	RECT rc = { 0, 0, 1024, 768 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	_hWnd = CreateWindow(L"Tutorial 1", L"Tutorial 1 using DirectX11", 
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 
		rc.right-rc.left, rc.bottom-rc.top, NULL, NULL, _hApp, NULL);
	if (!_hWnd) {
		MessageBox(NULL, L"Cannot create window", L"Error", MB_OK|MB_ICONERROR);
		return false;
	}

	// Initialize D3D11
	if (FAILED(initialiseDxDevice())) {
		MessageBox(_hWnd, L"Cannot create D3D11 device", L"Error", MB_OK|MB_ICONERROR);
		return false;
	}

	ShowWindow(_hWnd, cmdShow);
	UpdateWindow(_hWnd);
	
	return true;
}


// Main message loop
int DxApp::run() {

	MSG msg = {0};
	while (WM_QUIT != msg.message) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			updateScene();
			renderScene();
		}
	}
	return (int)msg.wParam;
}

void DxApp::onKey(WPARAM wParam)
{
	if(wParam == VK_ESCAPE)
		PostQuitMessage(0);
}

// Called every time the application receives a message
LRESULT CALLBACK DxApp::MessageProc(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam) {
	assert (_instance);

	//if(TwEventWin(wnd, message, wParam, lParam))
	//	return 0;

	switch (message) {

	case WM_SIZE: 
		_instance->onResize(LOWORD(lParam), HIWORD(lParam));
		return 0;

	case WM_CHAR:
		_instance->onKey(wParam);
		/*if (wParam == VK_ESCAPE)
			PostQuitMessage(0);*/
		return 0;

	case WM_KEYDOWN:
		_instance->onKey(wParam);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProc(wnd, message, wParam, lParam);
	}
}

// Create Direct3D device and swap chain
HRESULT DxApp::initialiseDxDevice() {
	// Get window size
	RECT rc;
	GetClientRect(_hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	// Create D3D11 device and swap chain
	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	ZeroMemory(&_swapChainDesc, sizeof(_swapChainDesc));
	_swapChainDesc.BufferDesc.Width = width;
	_swapChainDesc.BufferDesc.Height = height;
	_swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	_swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	_swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	_swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	_swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

	// No multisampling
	_swapChainDesc.SampleDesc.Count = 1;
	_swapChainDesc.SampleDesc.Quality = 0;

	_swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	_swapChainDesc.BufferCount = 1;
	_swapChainDesc.OutputWindow = _hWnd;
	_swapChainDesc.Windowed = true;
	_swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	_swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// Try to create a hardware accelerated device
	const D3D_FEATURE_LEVEL featureLevel[] = {D3D_FEATURE_LEVEL_10_0};
	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		NULL, // Primary graphics adapter
		D3D_DRIVER_TYPE_HARDWARE, // Use graphics hardware
		NULL, // Use hardware rasteriser
		createDeviceFlags, // Debug flags
		featureLevel, // Feature level
		1, // Elements in the feature level
		D3D11_SDK_VERSION, // SDK version
		&_swapChainDesc, // Description of the swap chain
		&_swapChain, // Returns the created swap chain
		&_dxDev, // Returns the created device
		NULL, // Returns feature level
		&_dxImmedDC // Returns the Device Context 
		);

	if (FAILED(hr)) {
		// If failed, try to create a reference device
		DXRETURN(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_REFERENCE, NULL, createDeviceFlags, 
			NULL, 0, D3D11_SDK_VERSION, &_swapChainDesc, &_swapChain, &_dxDev, NULL, &_dxImmedDC));
		
		MessageBox(_hWnd, L"No DX10 hardware acceleration found.\nSwitching to REFERENCE driver (very slow).", L"Warning", MB_OK|MB_ICONWARNING);
	}

	// Create a depth - stencil view
	_depthStencilDesc.Width = width;
	_depthStencilDesc.Height = height;
	_depthStencilDesc.MipLevels = 1;
	_depthStencilDesc.ArraySize = 1;
	_depthStencilDesc.Format = DXGI_FORMAT_D16_UNORM;
	_depthStencilDesc.SampleDesc = _swapChainDesc.SampleDesc;
	_depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	_depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	_depthStencilDesc.CPUAccessFlags = 0;
	_depthStencilDesc.MiscFlags = 0;
	
	DXRETURN( createDepthStencilStates() );		// NEW

	DXRETURN( createBlendState() );				// NEW

	DXRETURN( createDepthStencilView() );

	// Create a render target view
	DXRETURN( createRenderTargetView() );

	// Bind the views to the output
	_dxImmedDC->OMSetRenderTargets(1, &_renderTargetView, _depthStencilView);

	// Setup viewport
	setupViewport();

	return S_OK;
}

HRESULT DxApp::onResize(int width, int height) {
	if (!_dxDev)
		return S_FALSE;

	// Release render target and depth-stencil view
	ID3D11RenderTargetView *nullRTV = NULL;
	_dxImmedDC->OMSetRenderTargets(1, &nullRTV, NULL);
	
	if (_depthStencilView) {
		_depthStencilView->Release();
		_depthStencilView = NULL;
	}

	if (_renderTargetView) {
		_renderTargetView->Release();
		_renderTargetView = NULL;
	}

	// Resize swap chain
	_swapChainDesc.BufferDesc.Width = width;
	_swapChainDesc.BufferDesc.Height = height;
	if (_swapChain) 
		_swapChain->ResizeBuffers(_swapChainDesc.BufferCount, _swapChainDesc.BufferDesc.Width, 
			_swapChainDesc.BufferDesc.Height, _swapChainDesc.BufferDesc.Format, _swapChainDesc.Flags);

	// Re-create a depth-stencil view
	_depthStencilDesc.Width = _swapChainDesc.BufferDesc.Width;
	_depthStencilDesc.Height = _swapChainDesc.BufferDesc.Height;

	DXRETURN( createDepthStencilView() );

	// Re-create a render target 
	DXRETURN( createRenderTargetView() );

	// Bind the views to the output
	_dxImmedDC->OMSetRenderTargets(1, &_renderTargetView, _depthStencilView);

	// Setup viewport
	setupViewport();

	return S_OK;
}

bool DxApp::loadEffects(const char* filename, std::vector<char> &compiledShader) {
	std::ifstream fin(filename, std::ios::binary);
	if (!fin)
		return false;
	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();

	compiledShader.resize(size);
	fin.seekg(0, std::ios_base::beg);
	fin.read(&compiledShader[0], size);
	if (!fin)
		return false;
	return true;
}

void DxApp::setupViewport() {
	D3D11_VIEWPORT vp;
	vp.Width = (float)_swapChainDesc.BufferDesc.Width;
	vp.Height = (float)_swapChainDesc.BufferDesc.Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	_dxImmedDC->RSSetViewports(1, &vp);
}

HRESULT DxApp::createBlendState() {
	D3D11_BLEND_DESC blendStateDesc;
	// Clear the blend state description.
	ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));
	blendStateDesc.RenderTarget[0].BlendEnable = TRUE;
	blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	// Setup the blend factor.
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	return (_dxDev->CreateBlendState(&blendStateDesc, &blendState));
}

HRESULT DxApp::createDepthStencilStates() {
	D3D11_DEPTH_STENCIL_DESC depthstencil_desc;
	depthstencil_desc.DepthEnable = TRUE;
	depthstencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthstencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
	depthstencil_desc.StencilEnable = TRUE;
	depthstencil_desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthstencil_desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	depthstencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthstencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INVERT;
	depthstencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthstencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthstencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthstencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_INVERT;
	depthstencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthstencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	HRESULT result = _dxDev->CreateDepthStencilState(&depthstencil_desc, &m_depthStencilStateLess);
	if(FAILED(result))
	{
		return result;
	}

	depthstencil_desc.DepthFunc = D3D11_COMPARISON_EQUAL;
	result = _dxDev->CreateDepthStencilState(&depthstencil_desc, &m_depthStencilStateEqual);

	return result;
}

HRESULT DxApp::createDepthStencilView() {
	ID3D11Texture2D *dsBuffer = NULL;
	DXRETURN( _dxDev->CreateTexture2D(&_depthStencilDesc, NULL, &dsBuffer) );
	HRESULT hr = _dxDev->CreateDepthStencilView(dsBuffer, NULL, &_depthStencilView);
	dsBuffer->Release();
	
	return hr;
}

HRESULT DxApp::createRenderTargetView() {
	ID3D11Texture2D *backBuffer = NULL;
	DXRETURN( _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer) );
	HRESULT hr = _dxDev->CreateRenderTargetView(backBuffer, NULL, &_renderTargetView);
	backBuffer->Release();
	
	return hr;
}