#include "EntityTemplate.h"
#include "Mesh.h"

namespace malaz{

//----------------------------------------------------------------------
// CEntityTemplate class implementation
//----------------------------------------------------------------------

CEntityTemplate::CEntityTemplate( string Name,
								  string Type,
								  string Folder,
								  string Filename,
								  ERenderMethod RenderMethod,
								  CRenderer* Renderer,
								  CSoundManager* SoundManager,
								  LPDIRECT3DDEVICE9 d3dDevice,
								  const string& Texture )
{
	m_Name = Name;
	m_Type = Type;

	m_Mesh = new CMesh( Folder, Filename, d3dDevice, Texture );

	m_SpecularPower = 256.0f;

	m_RenderMethod = Renderer->GetMethod( RenderMethod );

	m_SoundManager = SoundManager;
}

} // namespace malaz