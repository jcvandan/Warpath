#pragma once
#include "TankDefines.h"
#include "Renderer.h"
#include "Track.h"
#include "EntityTemplate.h"
using namespace std;

namespace malaz{

class CTankTemplate : public CEntityTemplate
{
public:
	CTankTemplate( string Name,
				   string Type,
				   string Folder,
				   string Filename,
			       ERenderMethod RenderMethod,
				   CRenderer* Renderer,
				   CSoundManager* SoundManager,
				   LPDIRECT3DDEVICE9 d3dDevice,
				   const string& Texture = "",
			       float MaxSpeed = 20.0f,
				   float Acceleration = 1.1f,
				   float TurnSpeed = 0.5f,
			       int Weight = 50, 
				   int MaxHealth = 100 );

	//Getter functions
	const float& MaxSpeed()
	{
		return m_MaxSpeed;
	}
	const float& Acceleration()
	{
		return m_Acceleration;
	}
	const float& TurnSpeed()
	{
		return m_TurnSpeed;
	}
	const int& Weight()
	{
		return m_Weight;
	}
	const int& MaxHealth()
	{
		return m_MaxHealth;
	}
	
private:
	//********DATA********//
	float m_MaxSpeed;
	float m_Acceleration;
	float m_TurnSpeed;
	int m_Weight;
	int m_MaxHealth;	
};

}