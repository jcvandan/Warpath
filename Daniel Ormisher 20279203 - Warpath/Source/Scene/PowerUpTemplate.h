#pragma once
#include "TankDefines.h"
#include "Renderer.h"
#include "EntityTemplate.h"
#include <vector>
using namespace std;

namespace malaz{

class CPowerUpTemplate : public CEntityTemplate
{
public:
	CPowerUpTemplate( string Name,
				      string Type,
				      string Folder,
				      string Filename,
				      ERenderMethod RenderMethod,
				      CRenderer* Renderer,					  
					  CSoundManager* SoundManager,
					  ePowerUp PowerUpType,
					  int NumBullets,
				      LPDIRECT3DDEVICE9 d3dDevice,
				      const string& Texture = "" );

	const ePowerUp& PowerUpType(){ return m_Type; }
	int Bullets(){ return m_numBullets; }

private:
	ePowerUp m_Type;
	// Number of bullets the power up has
	int m_numBullets;
};

} // namespace malaz