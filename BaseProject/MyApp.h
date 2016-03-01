//  ---------------------------------------------------------------------------
//  Department of Computer Science, University of Hull
//  DirectX 11 framework, Aug 2012
//
//  ---------------------------------------------------------------------------

#pragma once
#include "dxapp.h"
#include <xnamath.h>
#include "Camera.h"
#include "Geometry.h"
#include "Cube.h"
#include "ObjModel.h"
#include "GeometryFactory.h"
#include <D3DX11tex.h>

struct object_as_vertex_index_buffer {
	ID3D11Buffer* vbuf_; // vertex buffer
	ID3D11Buffer* ibuf_; // index buffer
};

class MyApp : public DxApp {
private:
	XMMATRIX _worldMx;
	XMMATRIX _viewMx;
	XMMATRIX _projMx;

	ObjModel objModel;
	GeometryFactory factory;
	Camera mCamera;
	Cube cube1;
	Cube cube2;
	
	ID3DX11EffectMatrixVariable *_WorldViewProjMx;
	ID3DX11EffectMatrixVariable *_WorldInverseTransposeMx;
	ID3DX11EffectVectorVariable *_LightDirectionVec;
	ID3DX11EffectClassInstanceVariable *_Materials;

	ID3D11InputLayout *_inputLayout;
	ID3D11InputLayout *_lightLayout;
	ID3D11Buffer *_vertexBuffer;
	ID3D11Buffer *_indexBuffer;

	int totalVerts_;
	
	ID3DX11Effect *_fX;
	ID3DX11EffectTechnique *_technique;

	ID3D11ShaderResourceView* _textureMap;
	ID3D11SamplerState* _textureSampler;




public:
	MyApp();
	~MyApp();

	bool initialise(HINSTANCE hInstance, int cmdShow);

	HRESULT buildFX();
	HRESULT buildGeometry();

	HRESULT loadTexture();


	void onKey(WPARAM);
	void renderScene();
	void updateScene();
	void render(int);

	HRESULT onResize(int width, int height);
};

