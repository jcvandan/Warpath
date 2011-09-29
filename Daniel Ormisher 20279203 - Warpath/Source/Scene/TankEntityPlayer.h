#pragma once
#include "TankEntity.h"
#include "Track.h"
#include "GUI.h"

namespace malaz{

class CTankEntityPlayer : public CTankEntity
{
public:
	CTankEntityPlayer( CTankTemplate* Template, string Name,
					   CTrack* track,
					   vector<CEntity*>* TankVector,
					   CMessenger* Messenger,
					   CGUI* GUI,
					   CVector3 pos = CVector3( 0.0f, 0.0f, 0.0f ), 
					   CVector3 rot = CVector3( 0.0f, 0.0f, 0.0f ),  
					   CVector3 scale = CVector3( 1.0f, 1.0f, 1.0f ) );

	virtual bool Update( float updateTime, CMessenger* Messenger, CD3DManager* d3dManager );

	bool Racing()
	{
		return ( (m_State == Normal)||(m_State == Starting)||(m_State == Restarting) );
	}


private:
	void PlayerControl( float updateTime );
};

} // end namespace malaz