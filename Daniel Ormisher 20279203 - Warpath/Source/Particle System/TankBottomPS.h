#pragma once
#include "TankDefines.h"
#include "ParticleSystem.h"
using namespace std;

namespace malaz{

class CTankBottomPS : public CParticleSystem
{
public:
	CTankBottomPS( CMatrix4x4* TankMatrix,
				   LPDIRECT3DTEXTURE9 Texture,
				   string Owner,
				   LPDIRECT3DDEVICE9 d3dDevice );

	// Update the system
	virtual bool Update( float updateTime, CMessenger* Messenger );

	string m_Owner;

protected:
	// Emit particle function
	virtual void EmitParticle();
};

} // end namespace malaz