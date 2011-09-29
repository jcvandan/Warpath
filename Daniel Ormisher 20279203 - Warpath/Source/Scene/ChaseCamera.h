/*******************************************
	CChaseCamera
********************************************/
#pragma once
#include "Camera.h"
#include "TankDefines.h"
#include "TankEntityPlayer.h"

namespace malaz{

class CChaseCamera : public CCamera
{
public:
	CChaseCamera( CTankEntityPlayer* target, CVector3 position = CVector3( 0.0f, 0.0f, 0.0f ),
				  CVector3 rotation = CVector3( 0.0f, 0.0f, 0.0f ) );

	virtual void Update( float updateTime, CMessenger* Messenger );

	void RotateAroundPoint( const CVector3& point, float RotateSpeed, bool FacePoint = false );

	void ProcessMessages( CMessenger* Messenger );

private:
	string m_UID;
	CTankEntityPlayer* m_Target;
	float m_Speed;
};


} // namespace malaz