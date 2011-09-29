#pragma once
#include "TankDefines.h"
#include "TankEntity.h"
#include "PowerUpTemplate.h"

namespace malaz{

enum ePowerUpState
{
	Active,
	Inactive,
};

class CPowerUpEntity : public CEntity
{
public:
	CPowerUpEntity();

	CPowerUpEntity( CPowerUpTemplate* Template, string Name,
					const vector<CEntity*>* Entities,
					CVector3 pos = CVector3( 0.0f, 0.0f, 0.0f ), 
					CVector3 rot = CVector3( 0.0f, 0.0f, 0.0f ),  
					CVector3 scale = CVector3( 1.0f, 1.0f, 1.0f ) );

	~CPowerUpEntity();

	CPowerUpTemplate* PowerUpTemplate()
	{
		return m_PowerUpTemplate;
	}

	virtual bool Update( float updateTime, CMessenger* Messenger, CD3DManager* d3dManager );

	virtual void ProcessMessages( CMessenger* Messenger );

	virtual void Render( LPDIRECT3DDEVICE9 d3dDevice, CRenderer* Renderer );

private:
	// PowerUpTemplate
	CPowerUpTemplate* m_PowerUpTemplate;

	// Vector containing list of tanks
	const vector<CEntity*>* m_TankList;

	// State variable
	ePowerUpState m_State;

	// Timer for when in dead state
	float m_InactiveTimer;

	// Bounce movement variables
	float m_Speed;
	float m_Bounce;
};

} // namespace malaz