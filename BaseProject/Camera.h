#pragma once
#include "dxapp.h"
#include <xnamath.h>

class Camera
{
public:
	Camera(void);
	~Camera(void);

	XMFLOAT3 getPosition()const;
	XMFLOAT3& getLookAt();
	XMFLOAT3 getUp()const;

	//XMVECTOR Camera::getViewDirection()const;

	//XMMATRIX getViewMatrix()const;

	void Forward();
	void BackPedal();
	void StrafeLeft();
	void StrafeRight();

	void SetDistance( float distance, float minDistance, float maxDistance );
    void SetRotation( float x, float y, float minY, float maxY );
    void SetTarget( XMFLOAT3& target );

    void ApplyZoom( float zoomDelta );
    void ApplyRotation( float yawDelta, float pitchDelta );

	void SetPosition(XMFLOAT3 p);

	void Zoom();


	void SetViewMatrix(const XMMATRIX &viewMatrix);

	XMMATRIX GetViewMatrix();
	
private:
	XMFLOAT3 _cameraPosition;
	XMFLOAT3 _lookAt;
	XMFLOAT3 _up;

    float distance_, minDistance_, maxDistance_;
    float xRotation_, yRotation_, yMin_, yMax_;

	float _moveSpeed;
	XMMATRIX _viewMatrix;
};

