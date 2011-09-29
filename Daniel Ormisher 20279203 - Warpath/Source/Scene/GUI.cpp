#include "GUI.h"

namespace malaz{

// File path constant
const string filePath = "Media\\GUI\\";

// Centre global holds centre of screen
CVector2 center;

// Mouse coordinates
extern int MouseX;
extern int MouseY;

// Enum for the race start
enum RaceStart
{
	Three,
	Two,
	One,
	Go
};

// Singletone instance NULL initialisation
CGUI* CGUI::m_Instance = NULL;

void CGUI::Create( CD3DManager* d3dManager )
{
	m_Instance = new CGUI( d3dManager );
}

void CGUI::Destroy()
{
	delete m_Instance;
}

CGUI::CGUI( CD3DManager* d3dManager )
{
	// Create the sprite
	if( FAILED( D3DXCreateSprite( d3dManager->m_d3dDevice, &m_Sprite ) ) )
		int i=0;

	// Store pointer to the d3dManager
	m_d3dManager = d3dManager;

	// Calculate the scaling factor to set correct positions for GUI images
	m_ScaleX = (float)m_d3dManager->BackBufferWidth()/1680.0f;
	m_ScaleY = (float)m_d3dManager->BackBufferHeight()/1050.0f;

	// Set the GUI data
	m_State = GUIFrontEnd;	// Initialise the state
	m_MainPowerUp = NULL;	// Set to NULL
	m_Timer = 0.0f;			// Initialise timer to 0	

	// Caluclate the center of the screen
	center = CVector2( (float)m_d3dManager->BackBufferWidth()/2, (float)m_d3dManager->BackBufferHeight()/2 );

	// Load the main GUI sprite
	m_mainGUI = new CGUISprite( filePath+"gui.PNG", d3dManager->m_d3dDevice, CVector2( 1250*m_ScaleX, 850*m_ScaleY ), 
								0.0f, CVector2( 0.75f*m_ScaleX, 0.75f*m_ScaleY ) );


	////////////////////////////////////////////////////////////////
	// Load the Main Game GUI sprites

	const string PowerUpSprites[3] = { "mgun.PNG", "mortar.PNG", "speed.PNG" };
	CVector2 PowerUpPos( 1263*m_ScaleX, 860*m_ScaleY );

	m_SpeedBoost = new CGUISprite( filePath+PowerUpSprites[2], d3dManager->m_d3dDevice, PowerUpPos, 
								   0.0f, CVector2( 0.65f*m_ScaleX, 0.65f*m_ScaleY ) );
	m_MachineGun = new CGUISprite( filePath+PowerUpSprites[0], d3dManager->m_d3dDevice, PowerUpPos, 
								   0.0f, CVector2( 0.65f*m_ScaleX, 0.65f*m_ScaleY ) );
	m_Mortar = new CGUISprite( filePath+PowerUpSprites[1], d3dManager->m_d3dDevice, PowerUpPos, 
							   0.0f, CVector2( 0.65f*m_ScaleX, 0.65f*m_ScaleY ) );


	////////////////////////////////////////////////////////////////
	// Load the 1,2,3,Go! Starting sprites

	const string StartSprites[4] = { "3.PNG", "2.PNG", "1.PNG", "go.PNG" };

	CVector2 startPos( center.x-250*m_ScaleX, center.y-250*m_ScaleY );
	for( int i = 0; i<4; ++i )
	{		
		RaceStart[i] = new CGUISprite( filePath+StartSprites[i], d3dManager->m_d3dDevice, startPos, 0.0f, CVector2( m_ScaleX, m_ScaleY ) );
	}


	////////////////////////////////////////////////////////////////
	// Load the Win/Lose Sprites

	m_Won = new CGUISprite( filePath+"won.PNG", d3dManager->m_d3dDevice, CVector2( 300*m_ScaleX, 0.0f ), 0.0f, CVector2( 2.0f*m_ScaleX, 2.0f*m_ScaleY ) );
	m_Lost = new CGUISprite( filePath+"lost.PNG", d3dManager->m_d3dDevice, CVector2( 300*m_ScaleX, 0.0f ), 0.0f, CVector2( 2.0f*m_ScaleX, 2.0f*m_ScaleY ) );


	////////////////////////////////////////////////////////////////
	// Load the Title Sprites

	m_Title = new CGUISprite( filePath+"Title.PNG", d3dManager->m_d3dDevice, CVector2( 0.0f, 0.0f ),
							  0.0f, CVector2( 0.83f*m_ScaleX, 0.55f*m_ScaleY ) );

	m_TitleStart[0] = new CGUISprite( filePath+"start.PNG", d3dManager->m_d3dDevice, CVector2( center.x-155*m_ScaleX, center.y+20*m_ScaleY ), 
									  0.0f, CVector2( 0.63f*m_ScaleX, 0.35f*m_ScaleY ) );
	m_TitleStart[1] = new CGUISprite( filePath+"start.PNG", d3dManager->m_d3dDevice, CVector2( center.x-200*m_ScaleX, center.y ), 
									  0.0f, CVector2( 0.83f*m_ScaleX, 0.55f*m_ScaleY ) );

	m_TitleEnd[0] = new CGUISprite( filePath+"exit.PNG", d3dManager->m_d3dDevice, CVector2( center.x-155*m_ScaleX, center.y+50*m_ScaleY ), 
									0.0f, CVector2( 0.63f*m_ScaleX, 0.35f*m_ScaleY ) );
	m_TitleEnd[1] = new CGUISprite( filePath+"exit.PNG", d3dManager->m_d3dDevice, CVector2( center.x-200*m_ScaleX, center.y ), 
									0.0f, CVector2( 0.83f*m_ScaleX, 0.55f*m_ScaleY ) );

	// Loading sprite
	m_Loading = new CGUISprite( filePath+"loading.PNG", d3dManager->m_d3dDevice, CVector2( center.x-250*m_ScaleX, center.y*m_ScaleY ), 
								 0.0f, CVector2( m_ScaleX*1.0, m_ScaleY*1.0 ) );

	///////////////////////////////////////////////////////////////
	// Load the Pause menu sprites

	m_Billboard = new CGUISprite( filePath+"billboard.png", d3dManager->m_d3dDevice, CVector2( center.x-250*m_ScaleX, center.y-250*m_ScaleY ),
								  0.0f, CVector2( m_ScaleX, m_ScaleY ) );
	m_Paused = new CGUISprite( filePath+"game_paused.png", d3dManager->m_d3dDevice, CVector2( center.x-250*m_ScaleX, center.y-265*m_ScaleY ),
							   0.0f, CVector2( m_ScaleX, m_ScaleY ) );
}

CGUI::~CGUI()
{
	delete m_mainGUI;
	delete m_SpeedBoost;
	delete m_MachineGun;
	delete m_Mortar;
	delete m_Title;
	delete m_Won;
	delete m_Lost;
	delete m_Billboard;
	delete m_Paused;

	for( int i=0; i<4; ++i )
		delete RaceStart[i];

	for( int i=0; i<2; ++i )
	{
		delete m_TitleEnd[i];
		delete m_TitleStart[i];
	}
}

void CGUI::PowerUp( ePowerUp PowerUp )
{
	// Set the main power up pointer (the one drawn)
	if( PowerUp == SpeedBoost )
		m_MainPowerUp = m_SpeedBoost;
	else if( PowerUp == MachineGun )
		m_MainPowerUp = m_MachineGun;
	else if( PowerUp == Mortar )
		m_MainPowerUp = m_Mortar;
	else if( PowerUp == None )
		m_MainPowerUp = NULL;
}

void CGUI::Draw( float updateTime, bool MouseHit )
{
	// Begin the sprite draw sequence
	m_Sprite->Begin( D3DXSPRITE_ALPHABLEND );

	if( m_State == GUIFrontEnd || m_State == GUILoading )
	{
		m_Sprite->SetTransform( m_Title->Matrix() );
		m_Sprite->Draw( m_Title->Texture(), NULL, NULL, &D3DXVECTOR3( 0.0f, 0.0f, 1.0f ), 0xFFFFFFFF );		

		if( ( MouseX > m_TitleStart[0]->Position().x && MouseX < m_TitleStart[0]->Position().x+300*m_ScaleX ) &&
			( MouseY > m_TitleStart[0]->Position().y-30*m_ScaleY && MouseY < m_TitleStart[0]->Position().y+20*m_ScaleY ) )
		{
			m_Sprite->SetTransform( m_TitleStart[1]->Matrix() );
			m_Sprite->Draw( m_TitleStart[1]->Texture(), NULL, NULL, &D3DXVECTOR3( 0.0f, 0.0f, 1.0f ), 0xFFFFFFFF );
		}
		else
		{
			m_Sprite->SetTransform( m_TitleStart[0]->Matrix() );
			m_Sprite->Draw( m_TitleStart[0]->Texture(), NULL, NULL, &D3DXVECTOR3( 0.0f, 0.0f, 1.0f ), 0xFFFFFFFF );
		}	

		if( ( MouseX > m_TitleEnd[0]->Position().x && MouseX < m_TitleEnd[0]->Position().x+300*m_ScaleX ) &&
			( MouseY > m_TitleEnd[0]->Position().y-10*m_ScaleY && MouseY < m_TitleEnd[0]->Position().y+50*m_ScaleY ) )
		{
			m_Sprite->SetTransform( m_TitleEnd[1]->Matrix() );
			m_Sprite->Draw( m_TitleEnd[1]->Texture(), NULL, NULL, &D3DXVECTOR3( 0.0f, 0.0f, 1.0f ), 0xFFFFFFFF );
		}
		else
		{
			m_Sprite->SetTransform( m_TitleEnd[0]->Matrix() );
			m_Sprite->Draw( m_TitleEnd[0]->Texture(), NULL, NULL, &D3DXVECTOR3( 0.0f, 0.0f, 1.0f ), 0xFFFFFFFF );
		}

		if( m_State == GUILoading )
		{
			m_Sprite->SetTransform( m_Loading->Matrix() );
			m_Sprite->Draw( m_Loading->Texture(), NULL, NULL, &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );

			m_Sprite->SetTransform( m_Billboard->Matrix() );
			m_Sprite->Draw( m_Billboard->Texture(), NULL, NULL, &D3DXVECTOR3( 0.0f, 0.0f, 0.2f ), 0xFFFFFFFF );
		}
	}
	else if( m_State == GUIRaceStarting || m_State == GUIRacing )
	{
		// Draw the main GUI for during racing
		m_Sprite->SetTransform( m_mainGUI->Matrix() );
		m_Sprite->Draw( m_mainGUI->Texture(), NULL, NULL, NULL, 0xFFFFFFFF );

		// If the player has a power up then draw it
		if( m_MainPowerUp != NULL )
		{
			m_Sprite->SetTransform( m_MainPowerUp->Matrix() );
			m_Sprite->Draw( m_MainPowerUp->Texture(), NULL, NULL, NULL, 0xFFFFFFFF );
		}

		// If the race is starting then draw 1,2,3,Go! images
		if( m_State == GUIRaceStarting )
		{
			if( m_Timer < 1.0f )
			{
				m_Sprite->SetTransform( RaceStart[Three]->Matrix() );
				m_Sprite->Draw( RaceStart[Three]->Texture(), NULL, NULL, NULL, 0xFFFFFFFF );
			}
			else if( m_Timer < 2.0f )
			{
				m_Sprite->SetTransform( RaceStart[Two]->Matrix() );
				m_Sprite->Draw( RaceStart[Two]->Texture(), NULL, NULL, NULL, 0xFFFFFFFF );
			}
			else if( m_Timer < 3.0f )
			{
				m_Sprite->SetTransform( RaceStart[One]->Matrix() );
				m_Sprite->Draw( RaceStart[One]->Texture(), NULL, NULL, NULL, 0xFFFFFFFF );
			}
			else if( m_Timer <= 4.0f )
			{
				m_Sprite->SetTransform( RaceStart[Go]->Matrix() );
				m_Sprite->Draw( RaceStart[Go]->Texture(), NULL, NULL, NULL, 0xFFFFFFFF );
			}
			else
			{
				m_State = GUIRacing;
				m_Timer = 0.0f;
			}

			m_Timer += updateTime;
		}
	}
	else if( m_State == GUIWon )
	{
		m_Sprite->SetTransform( m_Won->Matrix() );
		m_Sprite->Draw( m_Won->Texture(), NULL, NULL, NULL, 0xFFFFFFFF );
	}
	else if( m_State == GUILost )
	{
		m_Sprite->SetTransform( m_Lost->Matrix() );
		m_Sprite->Draw( m_Lost->Texture(), NULL, NULL, NULL, 0xFFFFFFFF );
	}
	else if( m_State == GUIPaused )
	{
		m_Sprite->SetTransform( m_Billboard->Matrix() );
		m_Sprite->Draw( m_Billboard->Texture(), NULL, NULL, NULL, 0xFFFFFFFF );

		m_Sprite->SetTransform( m_Paused->Matrix() );
		m_Sprite->Draw( m_Paused->Texture(), NULL, NULL, NULL, 0xFFFFFFFF );		
			
		// Check for whether to draw enlarged or normal exit sprite
		if( ( MouseX > m_TitleEnd[0]->Position().x && MouseX < m_TitleEnd[0]->Position().x+300*m_ScaleX ) &&
			( MouseY > m_TitleEnd[0]->Position().y-10*m_ScaleY && MouseY < m_TitleEnd[0]->Position().y+50*m_ScaleY ) )
		{
			m_Sprite->SetTransform( m_TitleEnd[1]->Matrix() );
			m_Sprite->Draw( m_TitleEnd[1]->Texture(), NULL, NULL, NULL, 0xFFFFFFFF );
		}
		else
		{
			m_Sprite->SetTransform( m_TitleEnd[0]->Matrix() );
			m_Sprite->Draw( m_TitleEnd[0]->Texture(), NULL, NULL, NULL, 0xFFFFFFFF );
		}
	}

	m_Sprite->End();
}

int CGUI::Update( float updateTime, bool MouseHit )
{
	if( MouseHit )
	{
		if( ( MouseX > m_TitleStart[0]->Position().x && MouseX < m_TitleStart[0]->Position().x+300*m_ScaleX ) &&
			( MouseY > m_TitleStart[0]->Position().y-30*m_ScaleY && MouseY < m_TitleStart[0]->Position().y+20*m_ScaleY ) )
		{
			return 1;
		}
		else
		{
			m_Sprite->SetTransform( m_TitleStart[0]->Matrix() );
			m_Sprite->Draw( m_TitleStart[0]->Texture(), NULL, NULL, &D3DXVECTOR3( 0.0f, 0.0f, 1.0f ), 0xFFFFFFFF );
		}	

		if( ( MouseX > m_TitleEnd[0]->Position().x && MouseX < m_TitleEnd[0]->Position().x+300*m_ScaleX ) &&
			( MouseY > m_TitleEnd[0]->Position().y-10*m_ScaleY && MouseY < m_TitleEnd[0]->Position().y+50*m_ScaleY ) )
		{
			return 2;
		}
	}

	return 0;
}

} //namespace malaz