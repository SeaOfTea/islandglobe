#include "Geometry.h"
#include <D3DX11tex.h>


Geometry::Geometry(void)
{
	_scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	_position = XMFLOAT3(0.0, 0.0, 0.0);
	_color = XMFLOAT4(0.5, 0.5, 0.5, 1.0);
	_blendFactor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
}


Geometry::~Geometry(void)
{
}

void Geometry::Scale(XMFLOAT3 pScale)
{
	_scale.x *= pScale.x;
	_scale.y *= pScale.y;
	_scale.z *= pScale.z;
}

void Geometry::Translate(XMFLOAT3 pTransform)
{
	XMVECTOR pos = XMLoadFloat3(&_position);
	XMVECTOR trans = XMLoadFloat3(&pTransform);
	pos += trans;

	XMStoreFloat3(&_position, pos);
}

void Geometry::ReleaseBuffers()
{
	buffer.ibuf_->Release();
	buffer.ibuf_->Release();

	Transparency->Release();
	CCWcullMode->Release();
	CWcullMode->Release();

	_textureMap->Release();
	_textureSampler->Release();

}

HRESULT Geometry::BuildGeometry(ID3D11Device*)
{
	return true;
}

void Geometry::LoadTexture(ID3D11Device* _dxDev, LPCTSTR fileName)
{
	D3DX11CreateShaderResourceViewFromFile( _dxDev, fileName,
		NULL, NULL, &_textureMap, NULL );

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory( &sampDesc, sizeof(sampDesc) );
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	_dxDev->CreateSamplerState(&sampDesc, &_textureSampler);

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory( &blendDesc, sizeof(blendDesc) );

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory( &rtbd, sizeof(rtbd) );

	rtbd.BlendEnable			 = true;
	rtbd.SrcBlend				 = D3D11_BLEND_SRC_COLOR;
	rtbd.DestBlend				 = D3D11_BLEND_BLEND_FACTOR;
	rtbd.BlendOp				 = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha			 = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha			 = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha			 = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask	 = D3D10_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = rtbd;

	_dxDev->CreateBlendState(&blendDesc, &Transparency);


}

void Geometry::SetBlendFactor(XMFLOAT4 pBlend)
{
	_blendFactor = pBlend;
}