//----------------------------------------------------------------------
// CTankTemplate class implementation
//----------------------------------------------------------------------
#include "TankTemplate.h"

namespace malaz{

CTankTemplate::CTankTemplate( string Name,
							  string Type,
							  string Folder,
							  string Filename,
							  ERenderMethod RenderMethod, 
							  CRenderer* Renderer,
							  CSoundManager* SoundManager,
							  LPDIRECT3DDEVICE9 d3dDevice,	
							  const string& Texture,
							  float MaxSpeed, //= 20.0f,
							  float Acceleration, //= 1.1f,
							  float TurnSpeed, //= 0.5f,
							  int Weight, //= 50,
							  int MaxHealth ) //= 100
							  : CEntityTemplate( Name, Type, Folder, Filename, RenderMethod,
												 Renderer, SoundManager, d3dDevice, Texture )
{
	m_MaxSpeed = MaxSpeed;
	m_Acceleration = Acceleration;
	m_TurnSpeed = TurnSpeed;
	m_Weight = Weight;
	m_MaxHealth = MaxHealth;	
}

} // namespace malaz