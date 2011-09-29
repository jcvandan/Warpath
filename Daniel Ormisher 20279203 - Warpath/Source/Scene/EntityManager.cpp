#include "EntityManager.h"
#include "Entity.h"
#include "TankEntityAI.h"
#include "TankEntityPlayer.h"

namespace malaz{

///////////////////////////////////////
// Singleton Creation methods

// Make the pointer to the single instance of the manager NULL
CEntityManager* CEntityManager::m_Instance = NULL;

// Create method that is used instead of consturctor to create single instance of manager
void CEntityManager::Create( CD3DManager* d3d3Manager, CRenderer* manager, CMessenger* Messenger, 
							 CGUI* GUI, CSoundManager* SoundManager )
{
	if( !m_Instance )
	{
		m_Instance = new CEntityManager( d3d3Manager, manager, Messenger, GUI, SoundManager );
	}
}

// Destroy method that is called to destroy the single instance of the manager
void CEntityManager::Destroy()
{
	delete m_Instance;
}

// Conctructor - called by the create method as it is private
CEntityManager::CEntityManager( CD3DManager* d3d3Manager, CRenderer* manager, CMessenger* Messenger, 
								CGUI* GUI, CSoundManager* SoundManager  )
{
	m_Entities.reserve( 1024 );
	m_EntityTemplates.reserve( 1024 );

	m_d3dManager = d3d3Manager;
	m_Renderer = manager;
	m_Messenger = Messenger;
	m_GUI = GUI;

	m_SoundManager = SoundManager;

	m_UID = ENTITY_MANAGER;
}

// Destructor is also private, making it only accessible through the Destroy() method
CEntityManager::~CEntityManager()
{
	DeleteAllEntities();
	DeleteAllTemplates();
}

//------------------------------------------------------------------------------------------
// Entity Creation Methods
//------------------------------------------------------------------------------------------

// Create a basic entity type
CEntity* CEntityManager::CreateEntity( string templateName, 
									   string Name,
				   					   CVector3 pos,		//Initial positioning of Entity
									   CVector3 rot,		//Initial rotation
									   CVector3 scale )	//Initial scale, these and 2 above will create matrix
{
	CEntityTemplate* temp = GetTemplate( templateName );
	CEntity* ent = new CEntity( temp, Name, pos, rot, scale );
	m_Entities.push_back( ent );

	//m_EntityUIDs++;
	return ent;
}

// Create an AI tank
CTankEntityAI* CEntityManager::CreateAITank( string templateName, string Name,
											 CTrack* Track,
											 int Lane,
										     CVector3 pos,	//= CVector3( 0.0f, 0.0f, 0.0f )
										     CVector3 rot,	//= CVector3( 0.0f, 0.0f, 0.0f )
										     CVector3 scale ) //= CVector3( 1.0f, 1.0f, 1.0f )
{
	//Must get the template then cast it to a tank template
	CTankTemplate* temp = static_cast<CTankTemplate*>( GetTemplate( templateName ) );

	CTankEntity* player = static_cast<CTankEntity*>( GetEntity( PLAYER_TANK ) );

	CTankEntityAI* ent = new CTankEntityAI( temp, Name, Track, player, &m_TankEntities, m_Messenger, m_GUI, Lane, pos, rot, scale );
	m_TankEntities.push_back( ent );

	return ent;
}

// Create a player tank
CTankEntityPlayer* CEntityManager::CreatePlayerTank( string templateName, string Name,													
													 CTrack* Track,
												     CVector3 pos,	//= CVector3( 0.0f, 0.0f, 0.0f )
												     CVector3 rot,	//= CVector3( 0.0f, 0.0f, 0.0f )
												     CVector3 scale ) //= CVector3( 1.0f, 1.0f, 1.0f )
{
	//Must get the template then cast it to a tank template
	CTankTemplate* temp = static_cast<CTankTemplate*>( GetTemplate( templateName ) );

	CTankEntityPlayer* ent = new CTankEntityPlayer( temp, Name, Track, &m_TankEntities, m_Messenger, m_GUI, pos, rot, scale );
	m_TankEntities.push_back( ent );

	return ent;
}

// Create a track entity
CTrackEntity* CEntityManager::CreateTrackEntity( string templateName,		//Pointer to the template for the entity
												 string Name,				//Name of entity
												 CVector2 bounds,			// Max bounds of the track piece
												 vector<CVector3>* Lanes,
											     CVector3 pos,				//= CVector3( 0.0f, 0.0f, 0.0f )
											     CVector3 rot,				//= CVector3( 0.0f, 0.0f, 0.0f )
											     CVector3 scale )			//= CVector3( 1.0f, 1.0f, 1.0f )
{
	CEntityTemplate* temp = GetTemplate( templateName );
	CTrackEntity* ent = new CTrackEntity( temp, Name, bounds, Lanes, pos, rot, scale );
	m_Entities.push_back( ent );

	return ent;
}

// Create a power up entity
CPowerUpEntity* CEntityManager::CreatePowerUpEntity( string templateName, 
												     string Name,
			   									     CVector3 pos,		//Initial positioning of Entity
												     CVector3 rot,		//Initial rotation
												     CVector3 scale )	//Initial scale, these and 2 above will create matrix
{
	CPowerUpTemplate* temp = static_cast<CPowerUpTemplate*>( GetTemplate( templateName ) );
	CPowerUpEntity* ent = new CPowerUpEntity( temp, Name, &m_TankEntities, pos, rot, scale );
	m_Entities.push_back( ent );

	return ent;
}

//------------------------------------------------------------------------------------------
// Entity Template Creation Methods
//------------------------------------------------------------------------------------------

//Basic Entities / Entity Templates
CEntityTemplate* CEntityManager::CreateEntityTemplate( string Name, string Type, string Folder, string Filename, ERenderMethod RenderMethod, string Texture )
{
	CEntityTemplate* newTemplate = new CEntityTemplate( Name, Type, Folder, Filename, RenderMethod, m_Renderer, 
														m_SoundManager, m_d3dManager->m_d3dDevice, Texture );
	m_EntityTemplates.push_back( newTemplate );

	return newTemplate;
}

//Tank Entities / Tank Entity Templates
CTankTemplate* CEntityManager::CreateTankTemplate( string Name, 
												   string Type,
												   string Folder,
												   string Filename,
												   ERenderMethod RenderMethod,
												   string Texture,
												   float MaxSpeed,// = 10.0f,
												   float Acceleration,// = 1.1f,
												   float TurnSpeed,// = 0.5f
												   int Weight,// = 100
												   int MaxHealth )// = 100
{
	CTankTemplate* newTemplate = new CTankTemplate( Name, Type, Folder, Filename, RenderMethod, m_Renderer, m_SoundManager, m_d3dManager->m_d3dDevice, 
													Texture, MaxSpeed, Acceleration, TurnSpeed, Weight, MaxHealth );
	m_EntityTemplates.push_back( newTemplate );

	return newTemplate;
}

// Create a template for power up entities
CPowerUpTemplate* CEntityManager::CreatePowerUpTemplate( string Name,
													    string Type,
													    string Folder,
													    string Filename,
													    ERenderMethod RenderMethod,
													    ePowerUp PowerUpType,														
														int NumBullets,
													    const string& Texture )
{
	CPowerUpTemplate* newTemplate = new CPowerUpTemplate( Name, Type, Folder, Filename, RenderMethod, m_Renderer, m_SoundManager,
														  PowerUpType, NumBullets, m_d3dManager->m_d3dDevice, Texture );
	m_EntityTemplates.push_back( newTemplate );

	return newTemplate;
}

//------------------------------------------------------------------------------------------
// Entity Management methods
//------------------------------------------------------------------------------------------

// Returns a pointer to specified template
CEntityTemplate* CEntityManager::GetTemplate( string name )
{
	EntityTemplatesIter iter;

	for(iter = m_EntityTemplates.begin(); iter != m_EntityTemplates.end(); iter++ )
	{
		if( (*iter)->Name() == name )
			return (*iter);
	}

	return NULL;
}

// Returns a pointer to specified entity
CEntity* CEntityManager::GetEntity(std::string Name)
{
	EntitiesIter iter;

	for(iter = m_Entities.begin(); iter != m_Entities.end(); iter++ )
	{
		if( (*iter)->Name() == Name )
			return (*iter);
	}

	for(iter = m_TankEntities.begin(); iter != m_TankEntities.end(); iter++ )
	{
		if( (*iter)->Name() == Name )
			return (*iter);
	}

	return NULL;
}

//------------------------------------------------------------------------------------------
// Render / Update / Destroy Entities
// Destroy Templates
//------------------------------------------------------------------------------------------

// Render all the entities
void CEntityManager::RenderAllEntities( CCamera* Camera )
{
	EntitiesIter iter;

	for(iter = m_Entities.begin(); iter != m_Entities.end(); iter++ )
	{
		(*iter)->Render( m_d3dManager->m_d3dDevice, m_Renderer );
	}

	for(iter = m_TankEntities.begin(); iter != m_TankEntities.end(); iter++ )
	{
		(*iter)->Render( m_d3dManager->m_d3dDevice, m_Renderer );
	}
}

// Update all the entities
void CEntityManager::UpdateAllEntities( float updateTime )
{
	EntitiesIter iter;

	int i=0;
	for(iter = m_Entities.begin(); iter != m_Entities.end(); iter++ )
	{
		// if the entities update function returns false delete the entity
		if( !(*iter)->Update( updateTime, m_Messenger, m_d3dManager ) )
		{
			CEntity* ptr = (*iter);			

			if( iter == m_Entities.end()-1 )
			{				
				m_Entities.pop_back();
				delete ptr;
				return;
			}
			else
			{
				m_Entities[i] = m_Entities.back();
				m_Entities.pop_back();
				delete ptr;
			}
		}
		i++;
	}

	i=0;
	for(iter = m_TankEntities.begin(); iter != m_TankEntities.end(); iter++ )
	{
		// if the entities update function returns false delete the entity
		if( !(*iter)->Update( updateTime, m_Messenger, m_d3dManager ) )
		{
			CEntity* ptr = (*iter);			

			if( iter == m_TankEntities.end()-1 )
			{				
				m_TankEntities.pop_back();
				delete ptr;
				return;
			}
			else
			{
				m_TankEntities[i] = m_TankEntities.back();
				m_TankEntities.pop_back();
				delete ptr;
			}
		}
		i++;
	}
}

void CEntityManager::DeleteAllEntities()
{
	EntitiesIter iter;

	for(iter = m_Entities.begin(); iter != m_Entities.end(); iter++ )
	{
		delete(*iter);
	}

	for(iter = m_TankEntities.begin(); iter != m_TankEntities.end(); iter++ )
	{
		delete(*iter);
	}
}

void CEntityManager::DeleteAllTemplates()
{
	EntityTemplatesIter iter;

	for(iter = m_EntityTemplates.begin(); iter != m_EntityTemplates.end(); iter++ )
	{
		delete(*iter);
	}
}


//------------------------------------------------------------------------------------------
// Update the entity Manager
//------------------------------------------------------------------------------------------

void CEntityManager::Update( float updateTime )
{
	// Fetch any messages
	CMsg* msg = m_Messenger->FetchMessage( m_UID );
	while ( msg )
	{
		if( msg->type == Msg_GetTankList )
		{
			CMsgGetTankList* newMsg = new CMsgGetTankList( m_UID, Msg_GetTankList, &m_TankEntities );
			m_Messenger->SendMessageA( msg->from, newMsg );
			delete msg;
		}

		msg = m_Messenger->FetchMessage( m_UID );
	}

	UpdateAllEntities( updateTime );
}

} // namespace malaz