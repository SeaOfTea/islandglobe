#include "Camera.h"


Camera::Camera(void)
{
	_cameraPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	_lookAt = XMFLOAT3(0.0f, 0.0f, 0.0f);
	_up = XMFLOAT3(0.0f, 1.0f, 0.0f);

	SetDistance( 2.0f, 1.0f, 100.0f );
    SetRotation( 0.0f, 0.0f, -XM_PIDIV2, XM_PIDIV2 );


	_moveSpeed = 0.1;
}

XMFLOAT3 Camera::getPosition()const{return _cameraPosition;}
XMFLOAT3& Camera::getLookAt() {return _lookAt;}
XMFLOAT3 Camera::getUp()const{return _up;}

//XMMATRIX Camera::getViewMatrix()const{return _viewMatrix;}

void Camera::SetDistance( float distance, float minDistance, float maxDistance )
{
    distance_ = distance;
    minDistance_ = minDistance;
    maxDistance_ = maxDistance;

    if( distance_ < minDistance_ ) distance_ = minDistance_;
    if( distance_ > maxDistance_ ) distance_ = maxDistance_;
}


void Camera::SetRotation( float x, float y, float minY, float maxY )
{
    xRotation_ = x;
    yRotation_ = y;
    yMin_ = minY;
    yMax_ = maxY;

    if( yRotation_ < yMin_ ) yRotation_ = yMin_;
    if( yRotation_ > yMax_ ) yRotation_ = yMax_;
}


void Camera::SetTarget( XMFLOAT3& target )
{
    _lookAt = target;
}


void Camera::ApplyZoom( float zoomDelta )
{
    distance_ += zoomDelta;

    if( distance_ < minDistance_ ) distance_ = minDistance_;
    if( distance_ > maxDistance_ ) distance_ = maxDistance_;
}


void Camera::ApplyRotation( float yawDelta, float pitchDelta )
{
    xRotation_ += yawDelta;
    yRotation_ += pitchDelta;

    if( xRotation_ < yMin_ ) xRotation_ = yMin_;
    if( xRotation_ > yMax_ ) xRotation_ = yMax_;
}


void Camera::SetViewMatrix(const XMMATRIX &viewMatrix)
{
	_viewMatrix = viewMatrix;
}

void Camera::Forward()
{
	//TODO: move in direction of lookAt vector
	XMVECTOR viewDirection;

	viewDirection = XMLoadFloat3(&_lookAt);
	//viewDirection = XMVector3Normalize(viewDirection);

	_cameraPosition.x += (XMVectorGetX(viewDirection));
	_cameraPosition.z += (XMVectorGetZ(viewDirection));
}

void Camera::BackPedal()
{
	//TODO: move in direction of lookAt vector
	XMVECTOR viewDirection;

	viewDirection = XMLoadFloat3(&_lookAt);
	//viewDirection = XMVector3Normalize(viewDirection);

	_cameraPosition.x -= (XMVectorGetX(viewDirection) * 0.1);
	_cameraPosition.z -= (XMVectorGetZ(viewDirection)* 0.1);
}

void Camera::StrafeLeft()
{
	//TODO: move in direction of lookAt vector
	_cameraPosition.z += _moveSpeed;
}

void Camera::StrafeRight()
{
	//TODO: move in direction of lookAt vector
	_cameraPosition.z -= _moveSpeed;
}

void Camera::SetPosition(XMFLOAT3 pos)
{
	_cameraPosition = pos;
}

Camera::~Camera(void)
{
}

XMMATRIX Camera::GetViewMatrix()
{
	XMVECTOR zoom = XMVectorSet( 0.0f, 0.0f, distance_, 1.0f );
    XMMATRIX rotation = XMMatrixRotationRollPitchYaw( xRotation_, -yRotation_, 0.0f );

	zoom = XMVector3Transform( zoom, rotation );

	XMVECTOR pos = XMLoadFloat3( &_cameraPosition );
    XMVECTOR lookAt = XMLoadFloat3( &_lookAt );

	pos = lookAt + zoom;
    XMStoreFloat3( &_cameraPosition, pos );

	XMVECTOR up = XMVectorSet( 0.0f, 1.0f, 0.0f, 1.0f );
    up = XMVector3Transform( up, rotation );

	XMMATRIX viewMat = XMMatrixLookAtRH( pos, lookAt, up );

	_viewMatrix = viewMat;

	return _viewMatrix;
}
