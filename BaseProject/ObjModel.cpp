#include<fstream>
#include<vector>
#include<string>
#include"ObjModel.h"
#include"TokenStream.h"


ObjModel::ObjModel( )
{
    vertices_ = 0;
    normals_ = 0;
    texCoords_ = 0;
    totalVerts_ = 0;
}

      
ObjModel::~ObjModel( )
{
    Release( );
}


void ObjModel::Release( )
{
    totalVerts_ = 0;

    if( vertices_ != 0 ) delete[] vertices_;
    if( normals_ != 0 ) delete[] normals_;
    if( texCoords_ != 0 ) delete[] texCoords_;

    vertices_ = 0;
    normals_ = 0;
    texCoords_ = 0;
}


bool ObjModel::LoadOBJ( char *fileName )
{
    std::ifstream fileStream;
    int fileSize = 0;

    fileStream.open( fileName, std::ifstream::in );
   
    if( fileStream.is_open( ) == false )
        return false;

    fileStream.seekg( 0, std::ios::end );
    fileSize = ( int )fileStream.tellg( );
    fileStream.seekg( 0, std::ios::beg );

    if( fileSize <= 0 )
        return false;

    char *buffer = new char[fileSize];

    if( buffer == 0 )
        return false;

    memset( buffer, '\0', fileSize );

    TokenStream tokenStream, lineStream, faceStream;
    std::string tempLine, token;

    fileStream.read( buffer, fileSize );
    tokenStream.SetTokenStream( buffer );

    delete[] buffer;

    tokenStream.ResetStream( );

    std::vector<float> verts, norms, texC;
    std::vector<int> faces;

    char lineDelimiters[2] = { '\n', ' ' };


    while( tokenStream.MoveToNextLine( &tempLine ))
    {
        lineStream.SetTokenStream( ( char* )tempLine.c_str( ) );
        tokenStream.GetNextToken( 0, 0, 0 );

        if( !lineStream.GetNextToken( &token, lineDelimiters, 2 ) )
            continue;

        if( strcmp( token.c_str( ), "v" ) == 0 )
        {
            lineStream.GetNextToken( &token, lineDelimiters, 2 );
            verts.push_back( ( float )atof( token.c_str( ) ) );

            lineStream.GetNextToken( &token, lineDelimiters, 2 );
            verts.push_back( ( float )atof( token.c_str( ) ) );

            lineStream.GetNextToken( &token, lineDelimiters, 2 );
            verts.push_back( ( float )atof( token.c_str( ) ) );
        }
        else if( strcmp( token.c_str( ), "vn" ) == 0 )
        {
            lineStream.GetNextToken( &token, lineDelimiters, 2 );
            norms.push_back( ( float )atof( token.c_str( ) ) );

            lineStream.GetNextToken( &token, lineDelimiters, 2 );
            norms.push_back( ( float )atof( token.c_str( ) ) );

            lineStream.GetNextToken( &token, lineDelimiters, 2 );
            norms.push_back( ( float )atof( token.c_str( ) ) );
        }
        else if( strcmp( token.c_str( ), "vt" ) == 0 )
        {
            lineStream.GetNextToken( &token, lineDelimiters, 2 );
            texC.push_back( ( float )atof( token.c_str( ) ) );

            lineStream.GetNextToken( &token, lineDelimiters, 2 );
            texC.push_back( ( float )atof( token.c_str( ) ) );
        }
        else if( strcmp( token.c_str( ), "f" ) == 0 )
        {
            char faceTokens[3] = { '\n', ' ', '/' };
            std::string faceIndex;

            faceStream.SetTokenStream( ( char* )tempLine.c_str( ) );
            faceStream.GetNextToken( 0, 0, 0 );

            for( int i = 0; i < 3; i++ )
            {
                faceStream.GetNextToken( &faceIndex, faceTokens, 3 );
                faces.push_back( ( int )atoi( faceIndex.c_str( ) ) );

                faceStream.GetNextToken( &faceIndex, faceTokens, 3 );
                faces.push_back( ( int )atoi( faceIndex.c_str( ) ) );

                faceStream.GetNextToken( &faceIndex, faceTokens, 3 );
                faces.push_back( ( int )atoi( faceIndex.c_str( ) ) );
            }
        }
        else if( strcmp( token.c_str( ), "#" ) == 0 )
        {
            int a = 0;
            int b = a;
        }

        token[0] = '\0';
    }

    // "Unroll" the loaded obj information into a list of triangles.

    int vIndex = 0, nIndex = 0, tIndex = 0;
    int numFaces = ( int )faces.size( ) / 9;

    totalVerts_ = numFaces * 3;

    vertices_ = new float[totalVerts_ * 3];

    if( ( int )norms.size( ) != 0 )
    {
        normals_ = new float[totalVerts_ * 3];
    }

    if( ( int )texC.size( ) != 0 )
    {
        texCoords_ = new float[totalVerts_ * 2];
    }

    for( int f = 0; f < ( int )faces.size( ); f+=3 )
    {
        vertices_[vIndex + 0] = verts[( faces[f + 0] - 1 ) * 3 + 0];
        vertices_[vIndex + 1] = verts[( faces[f + 0] - 1 ) * 3 + 1];
        vertices_[vIndex + 2] = verts[( faces[f + 0] - 1 ) * 3 + 2];




        vIndex += 3;

        if(texCoords_)
        {
            texCoords_[tIndex + 0] = texC[( faces[f + 1] - 1 ) * 2 + 0];
            texCoords_[tIndex + 1] = texC[( faces[f + 1] - 1 ) * 2 + 1];
            tIndex += 2;
        }

        if(normals_)
        {
            normals_[nIndex + 0] = norms[( faces[f + 2] - 1 ) * 3 + 0];
            normals_[nIndex + 1] = norms[( faces[f + 2] - 1 ) * 3 + 1];
            normals_[nIndex + 2] = norms[( faces[f + 2] - 1 ) * 3 + 2];
            nIndex += 3;
        }
    }

    verts.clear( );
    norms.clear( );
    texC.clear( );
    faces.clear( );

    return true;
}

HRESULT ObjModel::BuildGeometry(ID3D11Device *_dxDev, char* filename)
{
	HRESULT d3dResult;
	if( LoadOBJ(filename ) == false)
	{
		return false;
	}
    totalVerts_ = GetTotalVerts( );

    Vertex* vertices = new Vertex[totalVerts_];
    float* vertsPtr = GetVertices( );
    float* texCPtr = GetTexCoords( );
	float* normPtr = GetNormals();

    for( int i = 0; i < totalVerts_; i++ )
    {
		vertices[i].color = XMFLOAT4(1.0f,0.0f,0.0f,1.0f);
        vertices[i].position = XMFLOAT3( *(vertsPtr + 0), *(vertsPtr + 1), *(vertsPtr + 2) );
        vertsPtr += 3;

		vertices[i].normal = XMFLOAT3( *(normPtr + 0), *(normPtr + 1), *(normPtr + 2) );
		normPtr+=3;

        vertices[i].tex = XMFLOAT2( *(texCPtr + 0), *(texCPtr + 1) );
        texCPtr += 2;
    }

	D3D11_BUFFER_DESC vertexDesc1;
    ZeroMemory( &vertexDesc1, sizeof( vertexDesc1 ) );
    vertexDesc1.Usage = D3D11_USAGE_DEFAULT;
    vertexDesc1.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexDesc1.ByteWidth = sizeof( Vertex ) * totalVerts_;

    D3D11_SUBRESOURCE_DATA resourceData1;
    ZeroMemory( &resourceData1, sizeof( resourceData1 ) );
    resourceData1.pSysMem = vertices;

	
	d3dResult = _dxDev->CreateBuffer( &vertexDesc1, &resourceData1, &buffer.vbuf_);
	d3dResult = _dxDev->CreateBuffer(NULL, NULL, &buffer.ibuf_);

	if(FAILED (d3dResult))
	{
		return false;
	}

	delete[] vertices;
    Release( );

	return true;
}