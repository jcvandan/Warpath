#include "ParticleSystemManager.h"
#include "MathDX.h"

namespace malaz{

// The full file path for texture loading
string FilePath = "Source\\Particle System\\";

string sCollision = "Collision";
string sMachineGun = "MachineGun";
string sMortar = "Mortar";
string sFire = "Fire";

///////////////////////////////////////
// Singleton Creation methods

// Make the pointer to the single instance of the manager NULL
CParticleSystemManager* CParticleSystemManager::m_Instance = NULL;

// Create method that is used instead of consturctor to create single instance of manager
void CParticleSystemManager::Create( CD3DManager* d3d3Manager, CRenderer* Renderer,  CTrack* Track,
									 CMessenger* Messenger, CEntityManager* entityManager, CSoundManager* SoundManager )
{
	if( !m_Instance )
	{
		m_Instance = new CParticleSystemManager( d3d3Manager, Renderer, Track, Messenger, entityManager, SoundManager );
	}
}

// Destroy method that is called to destroy the single instance of the manager
void CParticleSystemManager::Destroy()
{
	delete m_Instance;
}

// Conctructor - called by the create method as it is private
CParticleSystemManager::CParticleSystemManager( CD3DManager* d3dManager, CRenderer* Renderer,  CTrack* Track,
											    CMessenger* Messenger, CEntityManager* entityManager, CSoundManager* SoundManager )
{
	// Store pointers to other manager classes
	m_d3dManager = d3dManager;
	m_Renderer = Renderer;
	m_Messenger = Messenger;
	m_entityManager = entityManager;
	m_SoundManager = SoundManager;
	m_Track = Track;

	// Set the classes own UID
	m_UID = PS_MANAGER;

	////////////////////////////////////////////////////////
	// Initialise / Setup Rendering data

	// Create the vertex declaration
	D3DVERTEXELEMENT9 ParticleElements[] = 
	{
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		{0, 16, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
		D3DDECL_END()
	};

	d3dManager->m_d3dDevice->CreateVertexDeclaration( ParticleElements, &m_VDecl );

	// Load the shaders
	m_VS = new CVertexShader();
	m_VS->Load( "Source\\Render\\Shaders\\Particle.vsh", d3dManager->m_d3dDevice );

	m_PS = new CPixelShader();
	m_PS->Load( "Source\\Render\\Shaders\\Particle.psh", d3dManager->m_d3dDevice );

	// Send message to the entity manager to get tank vector
	CMsg* msg = new CMsg( m_UID, Msg_GetTankList );
	m_Messenger->SendMessageA( ENTITY_MANAGER, msg );
}

// Destructor is also private, making it only accessible through the Destroy() method
CParticleSystemManager::~CParticleSystemManager()
{
	// Delete all still active systems
	DestroyAllSystems();

	// Delete the shader classes
	delete m_PS;
	delete m_VS;

	// Release memory used by the vertex decleration
	m_VDecl->Release();
}


///////////////////////////////////////
// Particle System management methods

// Main Update method for the manager, everything is done from in this function
void CParticleSystemManager::RenderSystems( CCamera* MainCamera, float updateTime )
{
	// Set the appropriate render states
	m_d3dManager->m_d3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );
	m_d3dManager->m_d3dDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  TRUE );
	m_d3dManager->m_d3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,  FALSE );

	m_d3dManager->m_d3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
    m_d3dManager->m_d3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

	// Get the texture method then set the values needed
	CMatrix4x4 mat( CVector3(0.0f, 0.0f, 0.0f) );

	m_d3dManager->m_d3dDevice->SetVertexShader( m_VS->m_Shader );
	m_d3dManager->m_d3dDevice->SetPixelShader( m_PS->m_Shader );

	m_VS->m_ShaderConsts->SetMatrix( m_d3dManager->m_d3dDevice, "WorldMatrix", &ToD3DXMATRIX( mat ) );
	m_VS->m_ShaderConsts->SetFloat( m_d3dManager->m_d3dDevice, "ViewportHeight", (float)m_d3dManager->BackBufferHeight() );
	m_VS->m_ShaderConsts->SetFloatArray( m_d3dManager->m_d3dDevice, "CamPos", (float*)&MainCamera->Matrix().Position(), 3 );

	// Set the vertex declaration
	m_d3dManager->m_d3dDevice->SetVertexDeclaration( m_VDecl );

	// Render all the general systems
	RenderAllSystems( MainCamera );

	m_d3dManager->m_d3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,  TRUE );
}

// Function that deletes all remaining systems, only called when destructor is called
bool CParticleSystemManager::DestroyAllSystems()
{
	m_ParticleSystemsIter = m_ParticleSystems.begin();

	// Iterate the systems on the general system list
	while( m_ParticleSystemsIter != m_ParticleSystems.end() )
	{
		// Delete current system
		if( (*m_ParticleSystemsIter) )
		{
			delete (*m_ParticleSystemsIter);
		}

		++m_ParticleSystemsIter;
	}

	return true;
}

// Method to delete a single system
bool CParticleSystemManager::DeleteSystem()
{

	return true;
}

void CParticleSystemManager::UpdateSystems( float updateTime )
{
	///////////////////////////////////////
	// Message Processing

	CMsg* msg = static_cast<CMsgParticleSystem*>( m_Messenger->FetchMessage( m_UID ) );
	while ( msg )
	{
		if( msg->type == Msg_CreateMachineGunSystem )
		{
			CMsgParticleSystem* newMsg = static_cast<CMsgParticleSystem*>( msg );
			CreateMachineGunSystem( newMsg->matrix, newMsg->textureName, newMsg->from );
		}
		else if( msg->type == Msg_ShootMachineGun )
		{
			m_WeaponSystemsIter =  m_WeaponSystems.find( msg->from+sMachineGun );

			if( m_WeaponSystemsIter != m_WeaponSystems.end() )
			{
				CMachineGunPS* ps2 = static_cast<CMachineGunPS*>( m_WeaponSystemsIter->second );				
				ps2->Shoot( updateTime );
			}
		}
		else if( msg->type == Msg_GetTankList )
		{
			CMsgGetTankList* newMsg = static_cast<CMsgGetTankList*>( msg );
			m_TankList = newMsg->tankList;
		}
		else if( msg->type == Msg_CreateCollision )
		{
			CMsgParticleSystem* newMsg = static_cast<CMsgParticleSystem*>( msg );
			CreateCollisionSystem( newMsg->matrix, newMsg->textureName, newMsg->from );
		}
		else if( msg->type == Msg_CreateMortar )
		{
			CMsgMortar* newMsg = static_cast<CMsgMortar*>( msg );

			TrackIter* p = reinterpret_cast<TrackIter*>( newMsg->iter );
			TrackIter i = *p;

			CreateMortar( newMsg->matrix, newMsg->textureName, newMsg->from, i );
		}
		else if( msg->type == Msg_ShootMortar )
		{
			m_WeaponSystemsIter =  m_WeaponSystems.find( msg->from+sMortar );

			if( m_WeaponSystemsIter != m_WeaponSystems.end() )
			{
				CMortarPS* ps2 = static_cast<CMortarPS*>( m_WeaponSystemsIter->second );				
				ps2->Shoot( updateTime );
			}
		}
		else if( msg->type == Msg_CreateTankBottom )
		{
			CMsgParticleSystem* newMsg = static_cast<CMsgParticleSystem*>( msg );
			CreateTankBottomSystem( newMsg->matrix, newMsg->textureName, newMsg->from );
		}
		else if( msg->type == Msg_EmitCollision )
		{
			m_WeaponSystemsIter =  m_WeaponSystems.find( msg->from+sCollision );

			if( m_WeaponSystemsIter != m_WeaponSystems.end() )
			{
				CCollisionPS* ps2 = static_cast<CCollisionPS*>( m_WeaponSystemsIter->second );				
				ps2->Emit( updateTime );
			}
		}
		else if( msg->type == Msg_CreateExplosion )
		{
			CMsgParticleSystem* newMsg = static_cast<CMsgParticleSystem*>( msg );
			CreateExplosion( newMsg->textureName, newMsg->position );
		}
		else if( msg->type == Msg_CreateFire )
		{
			CMsgParticleSystem* newMsg = static_cast<CMsgParticleSystem*>( msg );
			CreateTankFireSystem( newMsg->matrix, newMsg->from, newMsg->textureName );
		}
		else if( msg->type == Msg_EmitFire )
		{
			m_WeaponSystemsIter =  m_WeaponSystems.find( msg->from+sFire );

			if( m_WeaponSystemsIter != m_WeaponSystems.end() )
			{
				CTankFirePS* ps2 = static_cast<CTankFirePS*>( m_WeaponSystemsIter->second );				
				ps2->EmitParticle();
			}
		}
		
		delete msg;
		msg = m_Messenger->FetchMessage( m_UID );
	}

	UpdateAllSystems( updateTime );
}

// Update all the active general particle systems (non weapon systems)
void CParticleSystemManager::UpdateAllSystems( float updateTime )
{
	// Iterate the systems on the general system list
	m_ParticleSystemsIter = m_ParticleSystems.begin();

	while( m_ParticleSystemsIter != m_ParticleSystems.end() )
	{
		// Update current system
		if( !(*m_ParticleSystemsIter)->Update( updateTime, m_Messenger ) )
		{
			CParticleSystem* temp = *m_ParticleSystemsIter;
			m_ParticleSystemsIter = m_ParticleSystems.erase( m_ParticleSystemsIter );
			
			delete temp;
			continue;
		}

		++m_ParticleSystemsIter;
	}

	// Iterate the systems on the weapon system list
	m_WeaponSystemsIter = m_WeaponSystems.begin();

	while( m_WeaponSystemsIter != m_WeaponSystems.end() )
	{
		// Update current system, if the system returns false delet it then send a message back
		// to its owner to let them know it is used up
		if( !m_WeaponSystemsIter->second->Update( updateTime, m_Messenger ) )
		{ 	
			// First send message to owner
			CMachineGunPS* ps = static_cast<CMachineGunPS*>( m_WeaponSystemsIter->second );

			// Then delete the memory and remove it from the list
    		CParticleSystem* temp = m_WeaponSystemsIter->second;
			m_WeaponSystemsIter = m_WeaponSystems.erase( m_WeaponSystemsIter );
			
			delete temp;
			continue;
		}
		
		++m_WeaponSystemsIter;
	}
}

// Render all the active systems
void CParticleSystemManager::RenderAllSystems( CCamera* MainCamera )
{
	m_ParticleSystemsIter = m_ParticleSystems.begin();

	// Iterate the systems on the general system list
	while( m_ParticleSystemsIter != m_ParticleSystems.end() )
	{
		// Render current system
		(*m_ParticleSystemsIter)->Render( m_d3dManager->BackBufferHeight(), MainCamera, m_d3dManager->m_d3dDevice );

		++m_ParticleSystemsIter;
	}

	// Iterate the systems on the weapon system list
	m_WeaponSystemsIter = m_WeaponSystems.begin();

	while( m_WeaponSystemsIter != m_WeaponSystems.end() )
	{
		// Update current system 
		m_WeaponSystemsIter->second->Render( m_d3dManager->BackBufferHeight(), MainCamera, m_d3dManager->m_d3dDevice );

		++m_WeaponSystemsIter;
	}
}

///////////////////////////////////////
// Particle System texture methods

void CParticleSystemManager::LoadTexture( string Texture, string Name )
{
	string fullPath = FilePath+Texture;
	LPDIRECT3DTEXTURE9 temp;
	D3DXCreateTextureFromFile( m_d3dManager->m_d3dDevice, fullPath.c_str(), &temp );
	m_TextureMap.insert( TexturePair( Name, temp ) );
}

LPDIRECT3DTEXTURE9 CParticleSystemManager::GetTexture( string Name )
{
	m_TextureMapIter = m_TextureMap.find( Name );
	return m_TextureMapIter->second;
}


///////////////////////////////////////
// Particle System creation methods

void CParticleSystemManager::CreateTankBottomSystem( CMatrix4x4* TankMatrix, string TexName, string Owner )
{
	LPDIRECT3DTEXTURE9 tex = GetTexture( TexName );
	CTankBottomPS* temp = new CTankBottomPS( TankMatrix, tex, Owner, m_d3dManager->m_d3dDevice );
	m_ParticleSystems.push_back( temp );
}

// Create a tank fire system
void CParticleSystemManager::CreateTankFireSystem( CMatrix4x4* TankMatrix, string Owner, string TexName )
{
	LPDIRECT3DTEXTURE9 tex = GetTexture( TexName );
	CTankFirePS* temp = new CTankFirePS( TankMatrix, tex, Owner, m_d3dManager->m_d3dDevice );
	m_WeaponSystems.insert( WeaponPair( Owner+sFire, temp ) );
}

void CParticleSystemManager::CreateMachineGunSystem( CMatrix4x4* TankMatrix, string TexName, string Owner )
{
	LPDIRECT3DTEXTURE9 tex = GetTexture( TexName );	
	CMachineGunPS* temp = new CMachineGunPS( TankMatrix, tex, m_TankList, 
											 Owner, m_d3dManager->m_d3dDevice );
	m_WeaponSystems.insert( WeaponPair( Owner+sMachineGun, temp ) );
}

void CParticleSystemManager::CreateCollisionSystem( CMatrix4x4* Matrix, string TexName, string Owner )
{
	LPDIRECT3DTEXTURE9 tex = GetTexture( TexName );
	CCollisionPS* p = new CCollisionPS( Matrix, tex, m_d3dManager->m_d3dDevice, Owner );
	m_WeaponSystems.insert( WeaponPair( Owner+sCollision, p ) );
}

void CParticleSystemManager::CreateMortar( CMatrix4x4* TankMatrix, string TexName, string Owner, TrackIter Iter )
{
	LPDIRECT3DTEXTURE9 tex = GetTexture( TexName );

	TrackIter i = m_Track->Begin();
	TrackIter ii;
	while( i <= m_Track->Back() )
	{
		if( (*i)->Name() == (*Iter)->Name() )
			ii = i;

		i++;
	}

	CMortarPS* temp = new CMortarPS( TankMatrix, tex, Owner, ii, m_SoundManager, m_d3dManager->m_d3dDevice );
	m_WeaponSystems.insert( WeaponPair( Owner+sMortar, temp ) );
}

void CParticleSystemManager::CreateExplosion( string TexName, CVector3* Position )
{
	LPDIRECT3DTEXTURE9 tex = GetTexture( TexName );
	CMortarExplosionPS* p = new CMortarExplosionPS( Position, tex, m_TankList, m_d3dManager->m_d3dDevice );
	m_ParticleSystems.push_back( p );
}

} // end namespace malaz