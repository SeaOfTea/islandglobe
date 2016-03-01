#pragma once
#include "DxApp.h"

struct VertexIndexBuffer {
	ID3D11Buffer* vbuf_; // vertex buffer
	ID3D11Buffer* ibuf_; // index buffer
};

struct Vertex {
	XMFLOAT3 position;
	XMFLOAT4 color;
	XMFLOAT3 normal;
	XMFLOAT2 tex;
};

struct Material {
	Material() {ZeroMemory(this, sizeof(this));}
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
	XMFLOAT4 Reflect;

};

const XMFLOAT4 BLACK (0.0f, 0.0f, 0.0f, 1.0f);
const XMFLOAT4 WHITE (1.0f, 1.0f, 1.0f, 1.0f);
const XMFLOAT4 RED   (1.0f, 0.0f, 0.0f, 1.0f);
const XMFLOAT4 GREEN (0.0f, 1.0f, 0.0f, 1.0f);
const XMFLOAT4 BLUE  (0.0f ,0.0f, 1.0f, 1.0f);

class Geometry
{
private:
	XMFLOAT3 _position;
	XMFLOAT3 _scale;
	XMFLOAT4 _color;
	XMFLOAT4 _blendFactor;



protected:
	VertexIndexBuffer buffer;
	int totalVerts_;


public:
	Geometry(void);
	~Geometry(void);

	XMFLOAT3 GetPosition()const{return _position ;}
	XMFLOAT4 GetColor()const{ return _color; }
	VertexIndexBuffer GetBuffer()const{ return buffer ;}
	virtual int GetTotalVerts() const{ return totalVerts_; }
	XMFLOAT3 GetScale() const{ return _scale;}

	XMFLOAT4 GetBlendfactor() const{ return _blendFactor;}

	virtual void LoadTexture(ID3D11Device*, LPCTSTR fileName);

	void SetColor(XMFLOAT4);
	void SetBlendFactor(XMFLOAT4);
	void Scale(XMFLOAT3);
	void Translate(XMFLOAT3);

	void ReleaseBuffers();

	virtual HRESULT BuildGeometry(ID3D11Device*);

	ID3D11ShaderResourceView* _textureMap;
	ID3D11SamplerState* _textureSampler;


	ID3D11BlendState* Transparency;
	ID3D11RasterizerState* CCWcullMode;
	ID3D11RasterizerState* CWcullMode;

};

