//  ---------------------------------------------------------------------------
//  Department of Computer Science, University of Hull
//  DirectX 11 framework, Aug 2012
//
//  ---------------------------------------------------------------------------
#include "MyApp.h"

//struct Vertex {
//	XMFLOAT3 position;
//	XMFLOAT4 color;
//	XMFLOAT3 normal;
//};

struct ShaderConstants {
    XMMATRIX worldViewProjection;
};


struct Light
{
	XMFLOAT3 dir;
	float pad;
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
};

//TODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODO

Light light;

struct cbPerFrame
{
	Light  light;
};

cbPerFrame constbuffPerFrame;


const float TURN_SPEED = 0.1f;


MyApp::MyApp() {
	XMMATRIX I = XMMatrixIdentity();
	_worldMx = I;
	_viewMx = I;
	_projMx = I;

	_WorldInverseTransposeMx = NULL;
	_WorldViewProjMx = NULL;
	_LightDirectionVec = NULL;
	_Materials = NULL;

	_depthStencilStateLess = NULL;
	_depthStencilStateEqual = NULL;

	_textureMap = NULL;

	_inputLayout = NULL;
	_vertexBuffer = NULL;
	_indexBuffer = NULL;
	_fX = NULL;
}

MyApp::~MyApp() {
	if (_vertexBuffer) _vertexBuffer->Release();
	if (_indexBuffer) _indexBuffer->Release();
	if (_inputLayout) _inputLayout->Release();
	if (_fX) _fX->Release();
}

bool MyApp::initialise(HINSTANCE hInstance, int cmdShow) {

	if (!DxApp::initialise(hInstance, cmdShow))
		return false;

	//TwInit(TW_DIRECT3D11, _dxDev);
	//TwWindowSize(800, 600);

	// Build FXs
	if (FAILED(buildFX())) {
		MessageBox(hWnd(), L"Cannot create effects", L"Error", MB_OK|MB_ICONERROR);
		return false;
	}

	// Build Geometry
	if (FAILED(buildGeometry())) {
		MessageBox(hWnd(), L"Cannot create geometry", L"Error", MB_OK|MB_ICONERROR);
		return false;
	}

	return true;
}


HRESULT MyApp::buildFX() {
	// Load and create the pre-compiled shader code
	std::vector<char> compiledShader(0);
	if (!loadEffects("fx/Effects.fxo", compiledShader))
		return S_FALSE;

	DXRETURN(D3DX11CreateEffectFromMemory(&compiledShader[0], compiledShader.size(), 0, _dxDev, &_fX));


		// Bind to the global data
	_WorldInverseTransposeMx = _fX->GetVariableByName("worldInverseTranspose")->AsMatrix();
	if (!_WorldInverseTransposeMx)
		return S_FALSE;

	// Bind to the global data
	_WorldViewProjMx = _fX->GetVariableByName("worldViewProj")->AsMatrix();
	if (!_WorldViewProjMx)
		return S_FALSE;

		// Bind to the global data
	_LightDirectionVec = _fX->GetVariableByName("lightDir")->AsVector();
	if (!_LightDirectionVec)
		return S_FALSE;

		// Bind to the global data
	_Materials = _fX->GetVariableByName("gMaterial")->AsClassInstance();
	if (!_LightDirectionVec)
		return S_FALSE;

	
	// Bind to the Shaders
	_technique = _fX->GetTechniqueByName("RenderScene");
	if (!_technique)
		return S_FALSE;



    // Create the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0,  DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(Vertex, color), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0,  DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, normal), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0,  DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(Vertex, tex), D3D11_INPUT_PER_VERTEX_DATA, 0 } // TEXCOORD 12
    };

	D3DX11_PASS_DESC passDesc;
	_technique->GetPassByIndex(0)->GetDesc(&passDesc);


	DXRETURN(_dxDev->CreateInputLayout(layout, 4, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &_inputLayout));

	

	return S_OK;
}

HRESULT MyApp::buildGeometry() 
{
	factory.CreateModel(_dxDev, "island.obj", L"sand.bmp");
	factory.Scale(0, XMFLOAT3 (0.15, 0.15, 0.15));
	factory.CreateModel(_dxDev, "sea.obj", L"seaTexture01.bmp");
	factory.Scale(1, XMFLOAT3 (0.15, 0.15, 0.15));
	factory.CreateModel(_dxDev, "base.obj", L"wood01.bmp");
	factory.Scale(2, XMFLOAT3 (0.15, 0.15, 0.15));
	factory.CreateModel(_dxDev, "palmTree.obj", L"treeBark_diffuse_01.bmp");
	factory.Scale(3, XMFLOAT3 (0.015, 0.015, 0.015));
	factory.Translate(3, XMFLOAT3(0, -2.3, 1));
	factory.CreateModel(_dxDev, "dome.obj");
	factory.Scale(4, XMFLOAT3 (0.15, 0.15, 0.15));
	//factory.SetBlendFactor(4, XMFLOAT4(0.75f, 0.75f, 0.75f, 1.0f));
	//factory.CreateModel(_dxDev, "cup.obj");
	//factory.Scale(2, XMFLOAT3(0.5, 0.5, 0.5));
	//factory.Translate(2, XMFLOAT3(1, 0, 0));
	//factory.CreateModel(_dxDev, "cup.obj");
	//factory.Scale(3, XMFLOAT3(0.25, 0.25, 0.25));


    return S_OK;
}

void MyApp::onKey(WPARAM wParam)
{
	switch(wParam)
	{
	case VK_UP:
		{
			mCamera.ApplyRotation(-0.5,0.0);
			break;
		}
	case VK_DOWN:
		{
			mCamera.ApplyRotation(0.5,0.0);
			break;
		}
	case VK_LEFT:
		{
			//mCamera.ApplyRotation(0.5,0.0);
			break;
		}
	case VK_RIGHT:
		{
			mCamera.StrafeRight();
			break;
		}
	case VK_PRIOR:
		{
			mCamera.ApplyZoom(-1.0f);
			break;
		}
	case VK_NEXT:
		{
			mCamera.ApplyZoom(1.0f);
			break;
		}
	case VK_END:
		{
			mCamera.ApplyRotation(0.0, 0.1);
			break;
		}
	case VK_DELETE:
		{
			mCamera.ApplyRotation(0.0, -0.1);
			break;
		}
	case VK_ESCAPE:
		{
			PostQuitMessage(0);
			break;
		}

	}
}


void MyApp::updateScene() {




}

void MyApp::renderScene() {

	//TwBar *myBar;
	//myBar = TwNewBar("Tweak Bar");


	//TwAddVarRW(myBar, "RotSpeed", TW_TYPE_FLOAT, &rotSpeed, " min=0 max=10 step=0.01 group=Engine label='Rotation speed' ");
	//TwAddVarRW(myBar, "Vector", TW_TYPE_DIR3F, &mCamera.getLookAt(), " group=Engine opened=true axisz=-z showval=true");

	_viewMx = mCamera.GetViewMatrix();

	RECT rect;
	int width_;
	int height_;
	if(GetClientRect(hWnd(), &rect))
	{
		width_ = rect.right - rect.left;
		height_ = rect.bottom - rect.top;
	}
	_projMx = XMMatrixPerspectiveFovRH(XM_PIDIV4, 1.33333333333, 0.1, 1000.0);
	_dxImmedDC->OMSetRenderTargets(1, &_renderTargetView, _depthStencilView);

    // Clear the back buffer 
    float clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    _dxImmedDC->ClearRenderTargetView( _renderTargetView, clearColor );
    _dxImmedDC->ClearDepthStencilView( _depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );


	XMMATRIX mx = _worldMx*_viewMx*_projMx;
	_WorldViewProjMx->SetMatrix((float*)&mx);

		// We need World Inverse Transpose for normal calulation in the shader
	mx = XMMatrixTranspose(XMMatrixInverse(&XMMatrixDeterminant(_worldMx), _worldMx));
	_WorldInverseTransposeMx->SetMatrix((float*)&mx);



    // Bind the input layout
    _dxImmedDC->IASetInputLayout(_inputLayout);
	_dxImmedDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);




	// Update the constant buffer
	

	// Commit the constant buffer to the GPU
	
	// Draw

//	TwDraw();

	// Render ambient light
	_dxImmedDC->OMSetBlendState(0, 0, 0xffffffff);
	_dxImmedDC->OMSetDepthStencilState(m_depthStencilStateLess, 1);
	render(0);


	int numOfDirectionLights = 2;
	float directionalLights[] = {-1.0f, 0.0f, 0.0f,		// from the right
		1.0f, 0.0f, 0.0f,};	// from the left

	_dxImmedDC->OMSetBlendState(blendState, blendFactor, 0xffffffff);
	_dxImmedDC->OMSetDepthStencilState(m_depthStencilStateEqual, 1);
	for(int pass = 0; pass < numOfDirectionLights; ++pass) {
		_LightDirectionVec->SetFloatVector(&directionalLights[pass*3]);
		render(1);
	}

	
	// Swap Buffer
	_swapChain->Present(1, 0);
}

void MyApp::render(int pass)
{
	factory.Draw(_dxImmedDC, _technique, _WorldViewProjMx, _worldMx, _viewMx, _projMx, pass);
}

HRESULT MyApp::onResize(int width, int height)
{
	return 0;
}
