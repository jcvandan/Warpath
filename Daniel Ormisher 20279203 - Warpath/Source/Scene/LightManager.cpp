#include "LightManager.h"
#include "MathDX.h"

namespace malaz{

CLightManager* CLightManager::m_Instance = NULL;

void CLightManager::Create()
{
	m_Instance = new CLightManager();
}

void CLightManager::Destroy()
{
	delete m_Instance;
}

CLightManager::CLightManager()
{
	m_NumLights=0;
}

CLightManager::~CLightManager()
{
}

void CLightManager::SetAmbientLight( float r, float g, float b )
{
	m_AmbientLight.r = r;
	m_AmbientLight.b = b;
	m_AmbientLight.g = g;
	m_AmbientLight.a = 1.0f;
}

void CLightManager::CreateLight( CVector3 Position,
								 D3DXCOLOR Colour,
								 float Brightness )
{
	CPointLight pointLight( ToD3DXVECTOR( Position ), Colour, Brightness );
	m_Lights.push_back( pointLight );

	m_NumLights++;
}

} // namespace malaz{