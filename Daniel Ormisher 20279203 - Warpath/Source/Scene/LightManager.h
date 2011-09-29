#pragma once
#include "TankDefines.h"
#include "PointLight.h"
#include "CVector3.h"
#include <vector>
using namespace gen;

namespace malaz{

class CLightManager
{
private:
	CLightManager();
	~CLightManager();

public:
	static void Create();
	static void Destroy();

	static CLightManager* GetInstance()
	{
		return m_Instance;
	}

	void CreateLight( CVector3 Position,
					  D3DXCOLOR Colour,
					  float Brightness );

	void SetAmbientLight( float r, float g, float b );

	const D3DXCOLOR& AmbientLight()
	{
		return m_AmbientLight;
	}

	//vector of Light objects
	vector<CPointLight> m_Lights;

private:
	// Ambient Colour
	D3DXCOLOR m_AmbientLight;

	// Singleton static instance of this class
	static CLightManager* m_Instance;
	//The number of lights that has been created
	int m_NumLights;
};

} // namespace malaz
