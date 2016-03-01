#pragma once
#include "Geometry.h"
class Cube : public Geometry
{
public:
	Cube(void);
	~Cube(void);
	
	virtual HRESULT BuildGeometry(ID3D11Device*);
};

