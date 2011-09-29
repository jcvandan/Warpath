////////////////////////////////////////////////////////////
// Implementation of the weapon particle systems used
// in the game
////////////////////////////////////////////////////////////

#include "WeaponPS.h"
#include "MathDX.h"

namespace malaz{

////////////////////////////////////////////////////////////
// CMachinGunPS implementation
////////////////////////////////////////////////////////////

CMachineGunPS::CMachineGunPS(  CMatrix4x4* TankMatrix,
							   LPDIRECT3DTEXTURE9 Texture,
							   const vector<CEntity*>* Entities,
							   string Owner,
							   LPDIRECT3DDEVICE9 d3dDevice )
							   : CParticleSystem( 76, Texture, 0.05f, 2.0f, 2.0f,
								 TankMatrix, CVector3( 1.0f, 0.0f, 0.0f), d3dDevice )
{
	m_Owner = Owner;
	m_TankList = Entities;
}

bool CMachineGunPS::Update( float updateTime, CMessenger* Messenger )
{
	m_ParticlesIter = m_AliveParticles.begin();

	int i=0;
	// Update particle data
	while( m_ParticlesIter != m_AliveParticles.end() )
	{
		(*m_ParticlesIter)->m_Matrix.MoveLocalZ( 100.0f*updateTime );
		(*m_ParticlesIter)->m_Life -= updateTime;
		(*m_ParticlesIter)->m_Alpha = ( 1/m_ParticleLife )*(*m_ParticlesIter)->m_Life;

		//////////////////////////////////////////////
		// Check for collision with all 4 tanks

		for( m_TankIter = m_TankList->begin(); m_TankIter != m_TankList->end(); ++m_TankIter )
		{
			if( (*m_TankIter)->Name() != m_Owner )
			{
				if( m_ParticlesIter !=  m_AliveParticles.end() )
				{
					// Calculate the squared distance between the power up and tank and check if it is less than
					// the squared sum of the the two bounding radii
					CVector3 pos = (*m_TankIter)->Matrix().Position() - (*m_ParticlesIter)->m_Matrix.Position();
					float d1 = pos.x * pos.x + pos.y * pos.y + pos.z * pos.z;
					float d2 = ( (*m_TankIter)->ScaledRadius() );

					// If it is in within the bounding sphere...
					if( d1 <= d2 * d2 )
					{
						// We then do a check on the OOBB
						if( (*m_TankIter)->CheckPointInOOBB( (*m_ParticlesIter)->m_Matrix.Position() ) )
						{
							// Send message to tell the tank of the collision
							CMsg* msg = new CMsg( "", Msg_EmitCollision );
							Messenger->SendMessageA( (*m_TankIter)->Name(), msg );

							// Make the bullet invisible
							(*m_ParticlesIter)->m_Alpha = 0.0f;
							(*m_ParticlesIter)->m_Life = 0.0f;

							// Erase the particle from the alive particle list
							m_ParticlesIter = m_AliveParticles.erase( m_ParticlesIter );

							if( m_ParticlesIter != m_AliveParticles.begin() )
							{
								--m_ParticlesIter;
							}
						}// ...end OOBB check
					}// ...end bounding sphere check
				}
			}
		}// end of main for loop
		
		++i;

		if( m_ParticlesIter !=  m_AliveParticles.end() )				
			++m_ParticlesIter;
	}

	if( !m_AliveParticles.empty() )
	{
		if( (*m_AliveParticles.front()).m_Life <= 0.0f && m_DeadParticles.empty() )
    		return false;
	}

	if( m_DeadParticles.empty() )
	{
		// First send message to owner
		CMsg* m = new CMsg( "", Msg_WeaponRunOut );
		Messenger->SendMessageA( m_Owner, m );
	}

	if( m_DeadParticles.empty() && m_AliveParticles.empty() )
	{
		return false;
	}
	
	return true;
}

void CMachineGunPS::Shoot( float updateTime )
{
	// Emit a new particle
	if( m_Time >= m_Frequency )
	{
 		EmitParticle();
		m_Time = 0.0f;
	}

	m_Time += updateTime;
}

void CMachineGunPS::EmitParticle()
{
	// Only emit a particle if the number of particles counter is less than the maximum
	if( !m_DeadParticles.empty() )
	{
		SParticleUpdate* p[2];
		for( int i=0; i<2; i++ )
		{
			p[i] = m_DeadParticles.back();
			p[i]->m_Matrix = *m_Emitter;
			p[i]->m_Matrix.MoveLocalY( 1.0f );

			m_DeadParticles.pop_back();
			m_AliveParticles.push_front( p[i] );
		}

		p[0]->m_Matrix.MoveLocal( CVector3( -1.0f, 0.5f, 0.5f ) );
		p[1]->m_Matrix.MoveLocal( CVector3( 1.0f, 0.5f, 0.5f ) );
	}
}


////////////////////////////////////////////////////////////
// CCollisionPS implementation
////////////////////////////////////////////////////////////

CCollisionPS::CCollisionPS( CMatrix4x4* Matrix, LPDIRECT3DTEXTURE9 Texture, LPDIRECT3DDEVICE9 d3dDevice, string Owner )
												: CParticleSystem( 20, Texture, 0.00f, 0.5f, 3.0f, Matrix,
												CVector3( 0.0f, 0.0f, 0.0f), d3dDevice )
{ 
	m_Owner = Owner; 
}

bool CCollisionPS::Update( float updateTime, CMessenger* Messenger )
{
	m_ParticlesIter = m_AliveParticles.begin();

	// Update particle data
	while( m_ParticlesIter != m_AliveParticles.end() )
	{
		// Do the update on each particle
		(*m_ParticlesIter)->m_Matrix.MoveLocalZ( -10.0f*updateTime );
		(*m_ParticlesIter)->m_Life -= updateTime;
		(*m_ParticlesIter)->m_Alpha = ( 1/m_ParticleLife )*(*m_ParticlesIter)->m_Life;

		if( (*m_ParticlesIter)->m_Life <= 0.0f )
		{
			m_DeadParticles.push_back( *m_ParticlesIter );
			m_ParticlesIter = m_AliveParticles.erase( m_ParticlesIter );
			continue;
		}

		++m_ParticlesIter;
	}
	
	return true;
}

void CCollisionPS::Emit( float updateTime )
{
	// Emit a new particle
	if( m_Time >= m_Frequency )
	{
		EmitParticle();
		m_Time = 0.0f;
	}
}

void CCollisionPS::EmitParticle()
{
	// Only emit a particle if the number of particles counter is less than the maximum
	if( !m_DeadParticles.empty() )
	{
		while( !m_DeadParticles.empty() )
		{
			SParticleUpdate* p;

			p = m_DeadParticles.back();
			p->m_Matrix = *m_Emitter;
			p->m_Life = m_ParticleLife;
			p->m_Alpha = 1.0f;

			float min = 0.0f;
			float max = ToRadians( 360.0f );
			p->m_Matrix.RotateLocalX( Random( min, max ) );
			p->m_Matrix.RotateLocalY( Random( min, max ) );
			p->m_Matrix.RotateLocalZ( Random( min, max ) );

			m_DeadParticles.pop_back();
			m_AliveParticles.push_front( p );
		}
	}
}


////////////////////////////////////////////////////////////
// CMortar implementation
////////////////////////////////////////////////////////////

CMortarPS::CMortarPS( CMatrix4x4* TankMatrix,
					   LPDIRECT3DTEXTURE9 Texture,
					   string Owner,
					   TrackIter& Iter,
					   CSoundManager* SoundManager,
					   LPDIRECT3DDEVICE9 d3dDevice )
					   : CParticleSystem( 20, Texture, 0.05f, 0.5f, 20.0f,
						 TankMatrix, CVector3( 1.0f, 0.0f, 0.0f), d3dDevice, SoundManager )
{
	m_TrackPiece = Iter;
	m_Mortar = NULL;
	m_Owner = Owner;
}

bool CMortarPS::Update( float updateTime, CMessenger* Messenger )
{
	// First update the mortar particle data
	if( m_Mortar != NULL )
	{
		m_Mortar->m_Matrix.MoveLocalZ( m_MortarSpeed*updateTime );

		m_Mortar->m_Matrix.MoveLocal( CVector3( 0.0f, m_MortarHeight*updateTime, 0.0f ) );

		bool trackTest = CheckAgainstTrack( &m_Mortar->m_Matrix, m_TrackPiece );

		float height;
		if( trackTest )
			height = 10.5f;
		else
			height = 1.0f;

		if( m_Mortar->m_Matrix.Position().y <= 10.5f )
		{
			m_Mortar->m_Life = 0.0f;
			m_Mortar->m_Alpha = 0.0f;

			// Then send message to create the explosion system
			CVector3* pos = new CVector3( m_Mortar->m_Matrix.Position() );
			CMsgParticleSystem* msg = new CMsgParticleSystem( "", Msg_CreateExplosion, "Explosion", NULL, pos );
			Messenger->SendMessageA( PS_MANAGER, msg );

			// Then send messages to tanks to tell of explosion
			msg = new CMsgParticleSystem( "", Msg_Explosion, "", NULL, pos );
			Messenger->SendMessageA( PLAYER_TANK, msg );

			msg = new CMsgParticleSystem( "", Msg_Explosion, "", NULL, pos );
			Messenger->SendMessageA( AI_TANK1, msg );

			msg = new CMsgParticleSystem( "", Msg_Explosion, "", NULL, pos );
			Messenger->SendMessageA( AI_TANK2, msg );

			msg = new CMsgParticleSystem( "", Msg_Explosion, "", NULL, pos );
			Messenger->SendMessageA( AI_TANK3, msg );

			SoundManager()->Play( 15, m_Mortar->m_Matrix.Position(), Sounds_Mortar );

			m_Mortar = NULL;
		}

		// Emit a new particle
		if( m_Time >= m_Frequency )
		{
			if( !m_AliveParticles.empty() )
			{
				EmitParticle();
				m_Time = 0.0f;
			}
		}
		
		m_MortarHeight -= 100.0f*updateTime;
		m_MortarSpeed -= 20.0f*updateTime;
	}

	// Update particle data
	m_ParticlesIter = m_AliveParticles.begin();

	while( m_ParticlesIter != m_AliveParticles.end() )
	{
		if( *m_ParticlesIter != m_Mortar )
		{
			(*m_ParticlesIter)->m_Matrix.MoveY( -1.0f*updateTime );
			(*m_ParticlesIter)->m_Life -= updateTime;
			(*m_ParticlesIter)->m_Alpha = ( 1/m_ParticleLife )*(*m_ParticlesIter)->m_Life;
		}
		
		++m_ParticlesIter;
	}	

	m_Time += updateTime;

	// If the last particle on the alive list is dead delete the system
	if( !m_AliveParticles.empty() )
	{
		if( (*m_AliveParticles.front()).m_Life <= 0.0f )
    		return false;
	}
	
	return true;
}

void CMortarPS::Shoot( float updateTime )
{
	// Shoot the mortar only if the list is empty
 	if( m_AliveParticles.empty() )
	{
 		m_Mortar = m_DeadParticles.back();
		m_AliveParticles.push_front( m_Mortar );
		m_Mortar->m_Matrix = *m_Emitter;
		m_Mortar->m_Matrix.MoveLocalY( 1.0f );
		m_Mortar->m_Size = m_ParticleSize;

		m_MortarHeight = 50.0f;
		m_MortarSpeed = 60.0f;
	}
}

void CMortarPS::EmitParticle()
{
	// Only emit a particle if the number of particles counter is less than the maximum
	if( !m_DeadParticles.empty() && m_Mortar != NULL )
	{
		// Initialise the particles matrix
		SParticleUpdate* p = m_DeadParticles.back();
		m_AliveParticles.push_front( p );

		p->m_Matrix = m_Mortar->m_Matrix;
		p->m_Alpha = 1.0f;
		p->m_Life = m_ParticleLife;
		p->m_Size = m_ParticleSize/4;

		m_DeadParticles.pop_back();
	}
}


////////////////////////////////////////////////////////////
// CMortarExplosionPS implementation
////////////////////////////////////////////////////////////

CMortarExplosionPS::CMortarExplosionPS( CVector3* Pos, LPDIRECT3DTEXTURE9 Texture,
										const vector<CEntity*> *Entities, LPDIRECT3DDEVICE9 d3dDevice )
									    : CParticleSystem( 100, Texture, 0.0f, 1.0f, 6.0f, NULL,
														   CVector3(), d3dDevice )
{
	m_Position = Pos;
}

CMortarExplosionPS::~CMortarExplosionPS()
{
	//CParticleSystem::~CParticleSystem();
	delete m_Position;
}

bool CMortarExplosionPS::Update( float updateTime, CMessenger* Messenger )
{
	while( !m_DeadParticles.empty() )
	{
		SParticleUpdate* p;

		p = m_DeadParticles.back();
		p->m_Matrix = CMatrix4x4( *m_Position );
		p->m_Life = m_ParticleLife;
		p->m_Alpha = 1.0f;

		float min = ToRadians( -90.0f );
		float max = ToRadians( 90.0f );
		p->m_Matrix.RotateLocalX( Random( min, max ) );
		p->m_Matrix.RotateLocalZ( Random( min, max ) );

		m_DeadParticles.pop_back();
		m_AliveParticles.push_front( p );
	}

	m_ParticlesIter = m_AliveParticles.begin();

	// Update particle data
	while( m_ParticlesIter != m_AliveParticles.end() )
	{
		// Do the update on each particle
		(*m_ParticlesIter)->m_Matrix.MoveLocalY( 15.0f*updateTime );
		(*m_ParticlesIter)->m_Life -= updateTime;
		(*m_ParticlesIter)->m_Alpha = ( 1/m_ParticleLife )*(*m_ParticlesIter)->m_Life;

		++m_ParticlesIter;
	}

	// If the last particle on the alive list is dead delete the system
	if( !m_AliveParticles.empty() )
	{
		if( (*m_AliveParticles.front()).m_Life <= 0.0f )
    		return false;
	}
	
	return true;
}

} // end namespace malaz