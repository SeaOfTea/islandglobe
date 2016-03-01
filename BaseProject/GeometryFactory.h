#pragma once
#include "Geometry.h"

using namespace std;

class GeometryFactory
{
private:
	vector<Geometry> list;
public:
	GeometryFactory(void);
	~GeometryFactory(void);

	void Add(Geometry pGeometry);
	void ClearAll();
	int GetSize()const{ return list.size();}

	void CreateCube(ID3D11Device *);
	void CreateCylinder();
	void CreateModel(ID3D11Device *_dxDev, char* filename, LPCTSTR textureName);
	void CreateModel(ID3D11Device *_dxDev, char* filename);

	void Scale(int index, XMFLOAT3 pScale);
	void Translate(int index, XMFLOAT3 pTranslate);
	void SetBlendFactor(int index, XMFLOAT4 pBlend);

	void LoadTexture(int index, ID3D11Device*, LPCTSTR fileName);

	void Draw(ID3D11DeviceContext* _dxImmedDC, ID3DX11EffectTechnique* _technique, ID3DX11EffectMatrixVariable *_WorldViewProjMx, const XMMATRIX &_worldMx, const XMMATRIX &_viewMx, const XMMATRIX &_projMx, int pass );
};

