/**********************************************
	D3DManager.h

	Declaration of d3d manager class:-
	Class looks sets up the LPDIRECT3DDEVICE9
	device and looks after the device itself.
	Purpose of the class is to 
***********************************************/

#pragma once
#include "TankDefines.h"
using namespace std;

namespace malaz{

class CD3DManager
{
private:
	//Constructor / Destructor
	CD3DManager( bool fullscreen );
	~CD3DManager();

public:
	// Singleton creation methods
	static void Create( bool fullscreen = false );
	static void Destroy();

	static CD3DManager* GetInstance()
	{
		return m_Instance;
	}

	//D3D Setup function, initialises the d3d Device
	bool D3DSetup( HWND hWnd, bool Reset = false );
	bool ResetD3D();
	
	//The directx d3d device to be used for rendering
	LPDIRECT3DDEVICE9 m_d3dDevice;

	// Toggle full screen on / off
	void ToggleFullscreen( HWND hWnd );

	// Draw text function
	void DisplayText( D3DXCOLOR fontColour, int X, int Y, string text, bool centre = false );

	// Functions to return the back buffer height and width
	const int& BackBufferHeight(){ return m_BackBufferHeight; }
	const int& BackBufferWidth(){ return m_BackBufferWidth; }

private:
	// Singleton instance of class
	static CD3DManager* m_Instance;

	/////////////////////////////////////
	// Properties

	// d3d font interface
	ID3DXFont* m_Font;

	// Backbuffer width and height
	int m_BackBufferHeight;
	int m_BackBufferWidth;

	bool m_Fullscreen;
};

} // namespace malaz