// Main header for class
#include "GameManager.h"

// Gameplay related includes
#include "Track.h"
#include "CParseLevel.h"
#include "Camera.h"
#include "ChaseCamera.h"
#include "Input.h"
#include "CMatrix4x4.h"
#include "CVector3.h"
#include "TankEntityPlayer.h"
#include <sstream>

namespace malaz{

//////////////////////////////////////////
// Gameplay related pointers

// Camera pointers
CCamera* MainCamera;
CChaseCamera* chaseCam;
CRoamCamera* roamCam;

// Track
CTrack* Track;
// Level Parser
CParseLevel* LevelParser;
// Pointer to the player tank
CTankEntityPlayer* player;

//////////////////////////////////////////
// Initialise the game manager instance to NULL
CGameManager* CGameManager::m_Instance = NULL;

// String stream for displaying update time
stringstream textStream;

extern int MouseX;
extern int MouseY;

////////////////////////////////////////////////
// The singletone create and destroy methods

void CGameManager::Create()
{
	if( !m_Instance )
	{
		m_Instance = new CGameManager();
	}
}

void CGameManager::Destroy()
{
	delete m_Instance;
}

////////////////////////////////////////////////
// Constructor and destructor / Load engine

CGameManager::CGameManager()
{
	m_GameState = FrontEnd;
	m_Pause = false;

	// Create the sound manager
	CSoundManager::Create();
	m_SoundManager = CSoundManager::GetInstance();	
}

CGameManager::~CGameManager()
{
}

////////////////////////////////////////////////////////
// Setup / Update / Render methods

// Game setup, initiliase everything needed to begin
bool CGameManager::Setup( CD3DManager* d3dManager )
{
	return FrontEndSetup( d3dManager );
}

// Update method
bool CGameManager::Update( CD3DManager* d3dManager, float updateTime )
{
	if( m_GameState == FrontEnd )
	{
		return FrontEndUpdate( d3dManager, updateTime );
	}
	else if( m_GameState == Game )
	{
		if( KeyHit( Key_Escape ) )
		{
			if( m_GUI->GetState() != GUIRaceStarting )
			{				
				m_Pause = !m_Pause;

				ShowCursor( m_Pause );

				if( m_GUI->GetState() != GUIPaused )
					m_GUI->SetState( GUIPaused );		
				else
					m_GUI->SetState( GUIRacing );
			}
		}

		if( !m_Pause )
			return GameUpdate( d3dManager, updateTime );
		else
			return GamePaused( d3dManager, updateTime  );
	}

	return true;
}

// Render method
void CGameManager::Render( CD3DManager* d3dManager, float updateTime )
{
    // Clear the backbuffer and the zbuffer
    d3dManager->m_d3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                         D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );

    // Begin the scene
    if( SUCCEEDED( d3dManager->m_d3dDevice->BeginScene() ) )
    {
		if( m_GameState == Game )
		{
			// Get combined view-projection matrix from camera
			CMatrix4x4 viewProjMatrix = MainCamera->GetViewProjMatrix();
			CVector3 CamPos = MainCamera->Matrix().Position();

			// Load the constants for all the shaders
			m_Renderer->LoadMethodConsts( m_LightManager, viewProjMatrix, CamPos, 10000.0f );

			// Render all the entities
			m_EntityManager->RenderAllEntities( MainCamera );

			// Update and draw all the particle systems
			m_PSManager->RenderSystems( MainCamera, updateTime );			

			#ifdef _DEBUG
				double a = 1.0f / updateTime;
				textStream << "Update Time: " << a;
				d3dManager->DisplayText( D3DXCOLOR( 255, 0, 0, 100 ), 10, 50, textStream.str() );
				textStream.str("");
			#endif
		}

		// Draw the GUI
		m_GUI->Draw( updateTime );

		// End the scene
        d3dManager->m_d3dDevice->EndScene();
	}

    // Present the backbuffer contents to the display
    d3dManager->m_d3dDevice->Present( NULL, NULL, NULL, NULL );
}

void CGameManager::Shutdown()
{
	if( m_GameState == FrontEnd )
		FrontEndShutdown();
	else if( m_GameState == Game )
		GameShutdown();
}

////////////////////////////////////////////////
// Front End Methods

bool CGameManager::FrontEndSetup( CD3DManager* d3dManager )
{
	//****************Load the GUI********************//

	CGUI::Create( d3dManager );
	m_GUI = CGUI::GetInstance();

	return true;
}

bool CGameManager::FrontEndUpdate( CD3DManager* d3dManager, float updateTime )
{
	bool mouse = KeyHit( Mouse_LButton );
	int result = m_GUI->Update( updateTime, mouse );

	if( result == 1 )
	{		
		m_GUI->SetState( GUILoading );
		Render( d3dManager, updateTime );
		ShowCursor( false );

		m_GameState = Game;
		GameSetup( d3dManager );
	}
	else if( result == 2 )
		return false;

	return true;
}

void CGameManager::FrontEndShutdown()
{
}

////////////////////////////////////////////////////////////////////////
// Main Game Methods

// Main Game scene setup method
bool CGameManager::GameSetup( CD3DManager* d3dManager )
{
	//****************Load Messenger*******************//
	m_Messenger = new CMessenger();

	//****************Load Renderer********************//
	CRenderer::Create( d3dManager->m_d3dDevice );
	m_Renderer = CRenderer::GetInstance();

	//****************Load Lights********************//
	CLightManager::Create();
	m_LightManager = CLightManager::GetInstance();

	//****************Load Entity Manager********************//
	CEntityManager::Create( d3dManager, m_Renderer, m_Messenger, m_GUI, m_SoundManager );
	m_EntityManager = CEntityManager::GetInstance();

	//****************Load Level / Entities********************//
	CTrack::Create();
	Track = CTrack::GetInstance();

	//*************Load Particle System Manager*****************//
	CParticleSystemManager::Create( d3dManager, m_Renderer, Track, m_Messenger, m_EntityManager, m_SoundManager );
	m_PSManager = CParticleSystemManager::GetInstance();
	m_PSManager->LoadTexture( "TankBottom.tga", "TankBottom" );
	m_PSManager->LoadTexture( "Torch.dds", "Fire" );
	m_PSManager->LoadTexture( "MachineGun.tga", "MachineGun" );
	m_PSManager->LoadTexture( "MachineGunCollision.tga", "MachineGunCollision" );
	m_PSManager->LoadTexture( "Mortar.tga", "Mortar" );
	m_PSManager->LoadTexture( "Explosion.tga", "Explosion" );


	LevelParser = new CParseLevel( m_EntityManager, Track, m_LightManager );
	if(	!LevelParser->ParseFile( "Level1.xml" ) )
	{
		return false;
	}

	//****************Load Cameras********************//

	player = static_cast<CTankEntityPlayer*>( m_EntityManager->GetEntity( PLAYER_TANK ) );
	chaseCam = new CChaseCamera( player );
	roamCam = new CRoamCamera();

	MainCamera = chaseCam;

	m_GUI->SetState( GUIRaceStarting );

	m_SoundManager->Play( 0, CVector3(0.0f, 0.0f, 0.0f), Sounds_Music, true, 1.0f, 0.5f );

	m_SoundManager->SetListener( &chaseCam->Matrix() );

	return true;
}

bool CGameManager::GameUpdate( CD3DManager* d3dManager, float updateTime )
{
	// Update the entity manager and all entities
	m_EntityManager->Update( updateTime );

	// Update all systems
	m_PSManager->UpdateSystems( updateTime );

	// Update the sound manager
	m_SoundManager->Update( updateTime );

	// Controls for switching between chase cam and roam cam
//#ifdef _DEBUG
	if( KeyHit( Key_1 ) )
	{
		if( MainCamera == chaseCam )
		{
			MainCamera = roamCam;
			MainCamera->Matrix().SetPosition( chaseCam->Matrix().Position() );
			MainCamera->Matrix().FaceDirection( chaseCam->Matrix().ZAxis() );
		}
		else if( MainCamera == roamCam )
		{
			MainCamera = chaseCam;
		}
	}
//#endif

	// Update the main camera
	MainCamera->Update( updateTime, m_Messenger );

	return true;
}

bool CGameManager::GamePaused( CD3DManager* d3dManager, float updateTime )
{	
	if( KeyHit( Mouse_LButton ) )
	{
		if( m_GUI->Update( updateTime, true ) == 2 )
		{
			return false;
		}
	}

	return true;
}

void CGameManager::GameShutdown()
{
	// Delete dynamically allocated objects
	delete roamCam;
	delete chaseCam;
	delete LevelParser;
	CLightManager::Destroy();
	CRenderer::Destroy();
	CParticleSystemManager::Destroy();
	CEntityManager::Destroy();
	CGUI::Destroy();
	CTrack::Destroy();
}

} // end namespace malaz