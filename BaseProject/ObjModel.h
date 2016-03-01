#pragma once
#include <string>
#include "Geometry.h"

class ObjModel : public Geometry
{
   public:
      ObjModel( );      
      ~ObjModel( );

      void Release( );
      bool LoadOBJ( char *fileName );
	  HRESULT BuildGeometry(ID3D11Device *_dxDev, char* filename);

      float *GetVertices()   const{ return vertices_; }
      float *GetNormals()    const{ return normals_; }
      float *GetTexCoords()  const{ return texCoords_; }
      int GetTotalVerts() const{ return totalVerts_; }

   private:
      float *vertices_;
      float *normals_;
      float *texCoords_;
};