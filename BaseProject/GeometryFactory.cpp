#include "GeometryFactory.h"
#include "Cube.h"
#include "ObjModel.h"


GeometryFactory::GeometryFactory(void)
{
}


GeometryFactory::~GeometryFactory(void)
{
}

void GeometryFactory::CreateCube(ID3D11Device * _dxDev)
{
	Cube cube;
	cube.BuildGeometry(_dxDev);
	list.push_back(cube);
}

void GeometryFactory::CreateModel(ID3D11Device *_dxDev, char* filename, LPCTSTR textureName)
{
	ObjModel model;
	model.BuildGeometry(_dxDev, filename);
	model.LoadTexture(_dxDev, textureName);
	list.push_back(model);
}
void GeometryFactory::CreateModel(ID3D11Device *_dxDev, char* filename)
{
	ObjModel model;
	model.BuildGeometry(_dxDev, filename);
	model.LoadTexture(_dxDev, L"default.jpg");
	list.push_back(model);
}

void GeometryFactory::ClearAll()
{
	list.clear();
}

void GeometryFactory::Scale(int index, XMFLOAT3 pScale)
{
	list[index].Scale(pScale);
}

void GeometryFactory::Translate(int index, XMFLOAT3 pTranslate)
{
	list[index].Translate(pTranslate);
}

void GeometryFactory::LoadTexture(int index, ID3D11Device* _dxDev, LPCTSTR fileName)
{
	list[index].LoadTexture(_dxDev, fileName);
}

void GeometryFactory::SetBlendFactor(int index, XMFLOAT4 pBlend)
{
	list[index].SetBlendFactor(pBlend);
}

void GeometryFactory::Draw(ID3D11DeviceContext* _dxImmedDC, ID3DX11EffectTechnique* _technique, ID3DX11EffectMatrixVariable *_WorldViewProjMx, const XMMATRIX& _worldMx, const XMMATRIX& _viewMx, const XMMATRIX& _projMx, int pass )
{
	for(int i=0; i < list.size(); i++)
	{
		VertexIndexBuffer v = list[i].GetBuffer();
		if(v.ibuf_ != NULL)
		{
			// Bind vertex buffer
			UINT stride = sizeof(Vertex), offset = 0;

			float blendFactor[4];

			blendFactor[0] = list[i].GetBlendfactor().w;
			blendFactor[1] = list[i].GetBlendfactor().x;
			blendFactor[2] = list[i].GetBlendfactor().y;
			blendFactor[3] = list[i].GetBlendfactor().z;

			//_dxImmedDC->OMSetBlendState(Transparency, blendFactor, 0xffffffff);

		
			_dxImmedDC->IASetVertexBuffers(0, 1, &v.vbuf_, &stride, &offset);

			// Bind index buffer
			_dxImmedDC->IASetIndexBuffer(v.ibuf_, DXGI_FORMAT_R32_UINT, 0);


			// Update the constant buffer
			XMMATRIX worldMatrix;
			XMMATRIX worldMatrixScale;
			XMMATRIX worldMatrixTranslate;
			worldMatrixScale = XMMatrixScaling(list[i].GetScale().x, list[i].GetScale().y, list[i].GetScale().z);
			worldMatrixTranslate = XMMatrixTranslation(list[i].GetPosition().x, list[i].GetPosition().y, list[i].GetPosition().z);

			worldMatrix = worldMatrixScale * worldMatrixTranslate *_worldMx;

			//worldMatrix = XMMatrixTranspose(worldMatrix);

			XMMATRIX mx = worldMatrix*_viewMx*_projMx;
			_WorldViewProjMx->SetMatrix((float*)&mx);

			_dxImmedDC->PSSetShaderResources( 0, 1, &list[i]._textureMap );
			_dxImmedDC->PSSetSamplers( 0, 1, &list[i]._textureSampler );
	

			// Commit the constant buffer to the GPU
			_technique->GetPassByIndex(pass)->Apply(0, _dxImmedDC);




			// Draw
			_dxImmedDC->DrawIndexed(sizeof(&v.ibuf_), (0), 0);
		}
		else
		{
			UINT stride = sizeof( Vertex );
			UINT offset = 0;

			_dxImmedDC->IASetVertexBuffers( 0, 1, &v.vbuf_, &stride, &offset );
			_dxImmedDC->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );


			XMMATRIX worldMatrix;
			XMMATRIX worldMatrixScale;
			XMMATRIX worldMatrixTranslate;
			worldMatrixScale = XMMatrixScaling(list[i].GetScale().x, list[i].GetScale().y, list[i].GetScale().z);
			worldMatrixTranslate = XMMatrixTranslation(list[i].GetPosition().x, list[i].GetPosition().y, list[i].GetPosition().z);

			worldMatrix = worldMatrixScale * worldMatrixTranslate *_worldMx;
			XMMATRIX mx = worldMatrix*_viewMx*_projMx;
			_WorldViewProjMx->SetMatrix((float*)&mx);

			worldMatrix = XMMatrixTranspose(XMMatrixInverse(&XMMatrixDeterminant(worldMatrix), worldMatrix));

	

			_technique->GetPassByIndex(pass)->Apply(0, _dxImmedDC);

			_dxImmedDC->PSSetShaderResources( 0, 1, &list[i]._textureMap );
			_dxImmedDC->PSSetSamplers( 0, 1, &list[i]._textureSampler );

			int j = list[i].GetTotalVerts();
			_dxImmedDC->Draw( j, 0 );
		}
	}
}
