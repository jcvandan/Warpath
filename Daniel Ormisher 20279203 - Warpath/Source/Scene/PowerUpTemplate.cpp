#include "PowerUpTemplate.h"

namespace malaz{

CPowerUpTemplate::CPowerUpTemplate( string Name,
								    string Type,
								    string Folder,
								    string Filename,
								    ERenderMethod RenderMethod, 
								    CRenderer* Renderer,
									CSoundManager* SoundManager,
								    ePowerUp PowerUpType,
									int NumBullets,
								    LPDIRECT3DDEVICE9 d3dDevice,	
								    const string& Texture )
									: CEntityTemplate( Name, Type, Folder, Filename, RenderMethod,
													   Renderer, SoundManager, d3dDevice, Texture )
{
	m_Type = PowerUpType;
	m_numBullets = NumBullets;
}


} // namespace malaz