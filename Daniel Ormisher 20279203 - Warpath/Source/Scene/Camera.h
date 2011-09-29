/*******************************************
	Camera.h

	Camera class declarations
********************************************/

#pragma once

#include "Defines.h"
#include "CVector3.h"
#include "CMatrix4x4.h"
#include "Input.h"
#include "Entity.h"
#include "Messenger.h"
using namespace gen;

namespace malaz{

class CMessenger;

struct Plane
{
	CVector3 Point;
	CVector3 Normal;
};

class CCamera
{
public:

	/////////////////////////////
	// Constructors

	// Constructor, with defaults for all parameters
	CCamera( const CVector3& position = CVector3::kOrigin, 
	         const CVector3& rotation = CVector3( 0.0f, 0.0f, 0.0f ),
			 TFloat32 nearClip = 1.0f, TFloat32 farClip = 100000.0f,
			 TFloat32 fov = kfPi/3.0f, TFloat32 aspect = 1.33f );


	///////////////////////////
	// Getters / Setters

	// Direct access (reference) to position and matrix
	CVector3& Position()
	{
		return m_Matrix.Position();
	}
	CMatrix4x4& Matrix()
	{
		return m_Matrix;
	}

	// Camera internals - Getters
	TFloat32 GetNearClip()
	{
		return m_NearClip;
	}
	TFloat32 GetFarClip()
	{
		return m_FarClip;
	}
	TFloat32 GetFOV()
	{
		return m_FOV;
	}
	TFloat32 GetAspect()
	{
		return m_Aspect;
	}

	// Camera internals - Setters
	void SetNearFarClip( TFloat32 nearClip, TFloat32 farClip )
	{
		m_NearClip = nearClip;
		m_FarClip = farClip;
	}
	void SetFOV( TFloat32 fov )
	{
		m_FOV = fov;
	}
	void SetAspect( TFloat32 aspect )
	{
		m_Aspect = aspect;
	}


	// Camera matrices - Getters
	CMatrix4x4 GetViewMatrix()
	{
		return m_MatView;
	}
	CMatrix4x4 GetProjMatrix()
	{
		return m_MatProj;
	}
	CMatrix4x4 GetViewProjMatrix()
	{
		return m_MatViewProj;
	}

	/////////////////////////////
	// Camera matrix functions

	// Sets up the view and projection transform matrices for the camera
	void CalculateMatrices();

	// Controls the camera - uses the current view matrix for local movement
	void Control( EKeyCode turnUp, EKeyCode turnDown,
	              EKeyCode turnLeft, EKeyCode turnRight,  
	              EKeyCode moveForward, EKeyCode moveBackward,
	              EKeyCode moveLeft, EKeyCode moveRight,
				  TFloat32 MoveSpeed, TFloat32 RotSpeed,
				  float updateTime );

	//Update Function
	virtual void Update( float updateTime, CMessenger* Messenger ) = 0;

	///////////////////////////
	// Camera picking

	// Calculate the X and Y pixel coordinates for the corresponding to given world coordinate
	// using this camera. Pass the viewport width and height. Return false if the world coordinate
	// is behind the camera
	bool PixelFromWorldPt( CVector3& worldPt, TUInt32 ViewportWidth, TUInt32 ViewportHeight,
	                       CVector2* PixelPt );

	// Calculate the world coordinates of a point on the near clip plane corresponding to given 
	// X and Y pixel coordinates using this camera. Pass the viewport width and height
	CVector3 WorldPtFromPixel( TInt32 X, TInt32 Y,
	                           TUInt32 ViewportWidth, TUInt32 ViewportHeight );


	///////////////////////////
	// Frustrum planes

	// Calculate the 6 planes of the camera's viewing frustum. Return each plane as a point (on
	// the plane) and a vector (pointing away from the frustum). Returned in two parameter arrays.
	// Order of planes passed back is near, far, left, right, top, bottom
	//   Four frustum planes:  _________
	//   Near, far, left and   \       /
	//   right. Top and bottom  \     /
	//   not shown               \   /
	//                            \_/
	//                             ^ Camera
	void CalculateFrustrumPlanes(  );

	bool SphereInFrustum( CVector3 point, float Radius );
	bool PointInFrustum( CVector3 point );
	//bool BoxInFrustum( CVector3 point, float Radius );

private:
	// Current positioning matrix
	CMatrix4x4 m_Matrix;

	// Near and far clip plane distances
	TFloat32 m_NearClip;
	TFloat32 m_FarClip;

	// Field of view - the angle covered from the left to the right side of the viewport
	TFloat32 m_FOV;

	// Aspect ratio of the viewport = Width / Height
	TFloat32 m_Aspect;

	// Current view and projection matrices
	CMatrix4x4 m_MatView;
	CMatrix4x4 m_MatProj;
	CMatrix4x4 m_MatViewProj; // Combined view/projection matrix

	Plane m_FrustumPlanes[6];
};


/*******************************************
	CRoamCamera
********************************************/

class CRoamCamera : public CCamera
{
public:
	virtual void Update( float updateTime, CMessenger* Messenger );
};

} // namespace malaz