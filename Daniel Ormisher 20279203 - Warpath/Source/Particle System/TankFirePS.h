#pragma once
#include "TankDefines.h"
#include "ParticleSystem.h"
using namespace std;

namespace malaz{

class CTankFirePS : public CParticleSystem
{
public:
	CTankFirePS( CMatrix4x4* TankMatrix, LPDIRECT3DTEXTURE9 Texture, string Owner, LPDIRECT3DDEVICE9 d3dDevice );

	// Update the system
	virtual bool Update( float updateTime, CMessenger* Messenger );

	// Emit particle function
	virtual void EmitParticle();

	string m_Owner;
};

} // end namespace malaz