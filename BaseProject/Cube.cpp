#include "Cube.h"


Cube::Cube(void)
{
}


Cube::~Cube(void)
{
}

HRESULT Cube::BuildGeometry(ID3D11Device* _dxDev)
{
	Vertex vertices[] =
	{
		{XMFLOAT3( 0.5f,  0.5f,  0.0f), GREEN, XMFLOAT3( 0.0f,  1.0f,  0.0f) },
		{XMFLOAT3(-0.5f,  0.5f,  0.0f), GREEN, XMFLOAT3( 0.0f,  1.0f,  0.0f)},
		{XMFLOAT3( 0.5f,  -0.5f,  0.0f), GREEN, XMFLOAT3( 0.0f,  1.0f,  0.0f)},
		{XMFLOAT3(-0.5f, -0.5f,  0.0f), GREEN, XMFLOAT3( 0.0f,  1.0f,  0.0f)},
	};

    D3D11_BUFFER_DESC vbd;
    vbd.ByteWidth = sizeof(vertices);
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    vbd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = vertices;
    vinitData.SysMemPitch = 0;
    vinitData.SysMemSlicePitch = 0;

	
	_dxDev->CreateBuffer(&vbd, &vinitData, &buffer.vbuf_);

   // Create index buffer
	DWORD indices[] = {
		0, 1, 2, 3,
	};

    D3D11_BUFFER_DESC ibd;
    ibd.ByteWidth = sizeof(indices);
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    ibd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = indices;
    iinitData.SysMemPitch = 0;
    iinitData.SysMemSlicePitch = 0;

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory( &sampDesc, sizeof(sampDesc) );
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//_dxDev->CreateSamplerState( &sampDesc, &);




    _dxDev->CreateBuffer(&ibd, &iinitData, &buffer.ibuf_);

	return true;
}