//////////////////////////////////////////////////////////////////
//  TankEntity.h
//
//  Declaration of CTankEntityAI. Inherits common tank behaviour
//  from CTankEntity but implements own scripted behaviour via
//  Update method.
//////////////////////////////////////////////////////////////////

#pragma once
#include "TankDefines.h"
#include "TankEntity.h"
#include "GUI.h"

namespace malaz{

// The available lanes for the AI tank to drive in
enum eLanes
{
	Lane1,
	Lane2,
	Lane3,
	Lane4
};

class CTankEntityAI : public CTankEntity
{	
public:
	CTankEntityAI( CTankTemplate* Template, string Name,
				   CTrack* Track,
				   CTankEntity* Player,
				   vector<CEntity*>* TankVector,
				   CMessenger* Messenger,
				   CGUI* GUI,
				   int Lane,
				   CVector3 pos = CVector3( 0.0f, 0.0f, 0.0f ), 
				   CVector3 rot = CVector3( 0.0f, 0.0f, 0.0f ),  
				   CVector3 scale = CVector3( 1.0f, 1.0f, 1.0f ) );

	~CTankEntityAI();

	virtual bool Update( float updateTime, CMessenger* Messenger, CD3DManager* d3dManager );

private:
	int m_Frames;

//////////////////////////////////////////
// Tank AI decision making methods

	// PowerUp decision making
	void MortarDecision( float updateTime, CMessenger* Messenger );
	void MachineGunDecision( float updateTime, CMessenger* Messenger );

	void SpeedboostDecision( float updateTime );

	void LaneSwitchDecision( float updateTime );

	void SpeedChanges( float updateTime );

//////////////////////////////////////////
// Tank AI movement methods

	void CheckWaypointCollision();

	float TurnTowardsVector( float updateTime,				
							 float turnSpeed,
							 CVector3& targetVec,
							 CMatrix4x4& matrix );

	// Pointer to player tank
	CTankEntity* m_Player;
};
} // namespace malaz