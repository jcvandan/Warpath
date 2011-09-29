/**********************************************
	D3DManager.h

	Implementation of CD3DManager class
***********************************************/

#include "D3DManager.h"

namespace malaz{

CD3DManager* CD3DManager::m_Instance = NULL;

void CD3DManager::Create( bool fullscreen )
{
	if( m_Instance == NULL )
	{
		m_Instance = new CD3DManager( fullscreen );
	}
}

void CD3DManager::Destroy()
{
	delete m_Instance;
}

CD3DManager::CD3DManager( bool fullscreen )
{
	m_d3dDevice = NULL;
	m_Fullscreen = fullscreen;
}

CD3DManager::~CD3DManager()
{
	if( m_d3dDevice != NULL )
		m_d3dDevice->Release();
	if( m_Font )
		m_Font->Release();
}

bool CD3DManager::D3DSetup( HWND hWnd, bool Reset )
{
	// Create the d3dDevice
	LPDIRECT3D9 D3D = NULL;
    D3D = Direct3DCreate9( D3D_SDK_VERSION );
	if (!D3D)
	{
        return false;
	}

	// Set up the structure used to create the D3DDevice
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;  // Wait for vertical sync
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;               // Use desktop pixel format (windowed mode)
	d3dpp.BackBufferCount = 1;
	d3dpp.EnableAutoDepthStencil = TRUE;         // Create depth buffer (Z)
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8; // Allow 24 bits of depth buffer, 8 bits of stencil

	// Check for antialiasing support
	D3DMULTISAMPLE_TYPE mst = D3DMULTISAMPLE_2_SAMPLES;
	DWORD quality = 0;

	// Check for hardware multisampling support on back buffer
	if( SUCCEEDED( D3D->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 
													D3DFMT_X8R8G8B8, true, mst, &quality) ) )
	{
		// Check for hardware multisampling support on depth buffer
		if( SUCCEEDED( D3D->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 
														D3DFMT_D24S8, true, mst, &quality) ) )
		{
			d3dpp.MultiSampleType = mst;
			d3dpp.MultiSampleQuality = quality-1;
		}
	}

	if( !m_Fullscreen )
	{		
		d3dpp.Windowed = TRUE;
		d3dpp.BackBufferWidth = 1000;
		d3dpp.BackBufferHeight = 800;
	}
	else
	{
		MONITORINFO monitorInfo;
		monitorInfo.cbSize = sizeof(MONITORINFO);
		if (GetMonitorInfo( D3D->GetAdapterMonitor( D3DADAPTER_DEFAULT ), &monitorInfo ))
		{
			d3dpp.BackBufferWidth = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
			d3dpp.BackBufferHeight = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
		}
		else
		{
			d3dpp.BackBufferWidth = 1280;
			d3dpp.BackBufferHeight = 1024;
		}
		// Set other fullscreen parameters
		d3dpp.Windowed = FALSE;
	}

	if( !Reset )
	{
		// Create the D3DDevice
		if (FAILED(D3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
									  D3DCREATE_HARDWARE_VERTEXPROCESSING,
									  &d3dpp, &m_d3dDevice )))
		{
			return false;
		}
	}
	else
	{
		if( m_Font != NULL )
			m_Font->Release();

		if( FAILED(m_d3dDevice->Reset( &d3dpp ) ) )
			return false;
	}

	// Create a font and the font interface
	D3DXCreateFont( m_d3dDevice, 40, 0, FW_BOLD, 0, FALSE, 
					DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
					DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
					TEXT("STENCIL STD"), &m_Font );

	m_BackBufferHeight = d3dpp.BackBufferHeight;
	m_BackBufferWidth = d3dpp.BackBufferWidth;

    // Turn on tri-linear filtering (for up to three simultaneous textures)
    m_d3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    m_d3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    m_d3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

	m_d3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    m_d3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    m_d3dDevice->SetSamplerState( 1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

	m_d3dDevice->SetSamplerState( 2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    m_d3dDevice->SetSamplerState( 2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    m_d3dDevice->SetSamplerState( 2, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

	//Enable point sprites for the engine
	m_d3dDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE );

	//Allow Alpha blending
	m_d3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_d3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
    m_d3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	// Let d3d know the vertex format
	m_d3dDevice->SetFVF(  D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 );

	// Check for shader support
	D3DCAPS9 d3dCaps;
	ZeroMemory(&d3dCaps, sizeof(D3DCAPS9)); // Can't hurt to zero the struct.

	HRESULT hRes = m_d3dDevice->GetDeviceCaps( &d3dCaps );

	if (d3dCaps.PixelShaderVersion < D3DPS_VERSION(2, 0)) 
	{
			MessageBox( NULL, "Your graphics card does not support Pixel Shader 2.0", "Pixel Shader Error", MB_OK );
	}

	if (d3dCaps.PixelShaderVersion < D3DVS_VERSION(2, 0))
	{
			MessageBox( NULL, "Your graphics card does not support Vertex Shader 2.0", "Vertex Shader Error", MB_OK );
	}

	D3D->Release();
 
	return true;
}

void CD3DManager::ToggleFullscreen( HWND hWnd )
{
	m_Fullscreen = !m_Fullscreen;
	D3DSetup( hWnd, m_Fullscreen );
}

void CD3DManager::DisplayText( D3DXCOLOR fontColour, int X, int Y, string text, bool centre )
{
	RECT rect;
	if (!centre)
	{
		SetRect( &rect, X, Y, 0, 0 );
		m_Font->DrawText( NULL, text.c_str(), -1, &rect, DT_NOCLIP, fontColour );
	}
	else
	{
		SetRect( &rect, X - 100, Y, X + 100, 0 );
		m_Font->DrawText( NULL, text.c_str(), -1, &rect, DT_CENTER | DT_NOCLIP, fontColour );
	}
}

} // namespace malaz