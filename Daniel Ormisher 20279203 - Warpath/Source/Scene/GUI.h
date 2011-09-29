#pragma once
#include "Sprite.h"
#include "TankDefines.h"
#include "D3DManager.h"
#include <list>
using namespace std;

namespace malaz{

// States for the GUI
enum eGUIState
{
	GUIFrontEnd,
	GUIPaused,
	GUILoading,
	GUIRaceStarting,
	GUIRacing,
	GUIWon,
	GUILost
};

class CGUI
{
private:
	// Private ctor / dtor due to singleton
	CGUI( CD3DManager* d3dManager );
	~CGUI();

public:
	// Singleton create and destroy methods
	static void Create( CD3DManager* d3dManager );
	static void Destroy();

	// Returns static instance of singelton class
	static CGUI* GetInstance()
	{
		return m_Instance;
	}

	// Sets the current power up to be displayed
	void PowerUp( ePowerUp PowerUp );

	// Draws the GUI
	void Draw( float updateTime, bool MouseHit = false );

	// Update the GUI
	int Update( float updateTime, bool MouseHit = false );

	// Set the Win/Lose image to be displayed
	void SetWinState( bool PlayerWon )
	{
		if( PlayerWon )
			m_State = GUIWon;
		else
			m_State = GUILost;
	}

	// Sets the state of the GUI
	void SetState( eGUIState state )
	{
		m_State = state;
	}

	const eGUIState& GetState()
	{
		return m_State;
	}

private:
	// Static singleton instance of class
	static CGUI* m_Instance;

	// The sprite to be drawn
	LPD3DXSPRITE m_Sprite;

	// Main GUI sprite
	CGUISprite* m_mainGUI;

	// Loading sprite
	CGUISprite* m_Loading;

	// Pointers to power up sprites
	CGUISprite* m_MainPowerUp;
	CGUISprite* m_SpeedBoost;
	CGUISprite* m_MachineGun;
	CGUISprite* m_Mortar;

	// Pointers to sprites for title screen
	CGUISprite* m_Title;
	CGUISprite* m_TitleStart[2];
	CGUISprite* m_TitleEnd[2];

	// Array of pointers to race start sprites
	CGUISprite* RaceStart[4];

	// Win / Lose Sprites
	CGUISprite* m_Won;
	CGUISprite* m_Lost;

	// Pause Sprites
	CGUISprite* m_Billboard;
	CGUISprite* m_Paused;

	// GUI Data
	float m_Timer; // GUI Timer

	eGUIState m_State; // GUI State

	float m_ScaleX; // X Scale for correct positioning
	float m_ScaleY; // Y Scale for correct positioning

	CD3DManager* m_d3dManager; // Pointer to the d3dManager	
};

} // namespace malaz