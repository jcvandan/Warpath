//////////////////////////////////////////////////////////////////
//  TankEntityAI.cpp
//
//  Contains implementation of CTankEntityAI methods.
//////////////////////////////////////////////////////////////////

#include "TankEntityAI.h"

namespace malaz{

//----------------------------------------------------------------------
// Constants
//----------------------------------------------------------------------
const float POINT_RADIUS = 0.25f;

const float LANE1_DIST = 3.0f;
const float LANE2_DIST = -3.0f;

const float TANK_DIST = 35.0f;

//----------------------------------------------------------------------
// Function to help with Tank positioning / movement
//----------------------------------------------------------------------

float DistanceFromAxis( CVector3& Pos, CVector3& TargPos, CVector3& TargAxis )
{
	// First calculate the distance from the corners z axis
    CVector3 v( TargPos, Pos );
    return Dot( v, Normalise( TargAxis ) );
}

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------

CTankEntityAI::CTankEntityAI( CTankTemplate* Template, string Name,
						      CTrack* Track,
							  CTankEntity* Player,							  
							  vector<CEntity*>* TankVector,
							  CMessenger* Messenger,
							  CGUI* GUI,
							  int Lane,
						      CVector3 pos,// = CVector3( 0.0f, 0.0f, 0.0f ), 
						      CVector3 rot,// = CVector3( 0.0f, 0.0f, 0.0f ),  
						      CVector3 scale )// = CVector3( 1.0f, 1.0f, 1.0f ) )
						      : CTankEntity( Template, Name, Track, TankVector, Messenger, GUI, pos, rot, scale )
{
	m_Lane = Lane;
	m_Waypoints = NULL;
	m_Frames = 0;
	m_Player = Player;
}

CTankEntityAI::~CTankEntityAI()
{
	CEntity::~CEntity();
}

//----------------------------------------------------------------------
// Method gradually turns tank towards a vector - used to keep enemy tanks
// facing the correct direction of track.
//----------------------------------------------------------------------

float CTankEntityAI::TurnTowardsVector( float updateTime,				
										float turnSpeed,
										CVector3& targetVec,
										CMatrix4x4& matrix )
{
	//First Normalise X and Z axis
	CVector3 normX = Normalise( matrix.XAxis() );
	CVector3 normZ = Normalise( matrix.ZAxis() );

	//Then normalise vector from source to target
	CVector3 normTargVec = Normalise( targetVec );

	//Get dot products of angles and vectors
	double b = Dot( normX, normTargVec );
	
	double a = acos( Dot( normZ, normTargVec ) );

	if( a > ToRadians( 5.0f ) )
	{
		if( b > ToRadians(0.0f) )
		{			
			matrix.RotateLocalY( -turnSpeed*updateTime );
		}
		else
		{
			matrix.RotateLocalY( turnSpeed*updateTime );
		}
	}
	return a;
}

//----------------------------------------------------------------------
// Contains all scripted behaviour for enemy tanks
//----------------------------------------------------------------------

bool CTankEntityAI::Update( float updateTime, CMessenger* Messenger, CD3DManager* d3dManager )
{
	// Fetch any messages
	ProcessMessages( Messenger );

	m_EnginePitch = m_Speed*2/m_MaxSpeed+1.0f;
	TankTemplate()->SoundManager()->UpdateSound( m_SourceNum, Matrix().Position(), Sounds_Engine, m_EnginePitch );
	TankTemplate()->SoundManager()->UpdateSound( m_SourceNum2, Matrix().Position(), Sounds_Engine, 1.0f );

	if( m_State == Normal )
	{
		///////////////////////////////////////////
		// Sub State / Collision behaviour

		SpeedChanges( updateTime );

		if( m_SubState.TakingDamage )
		{
			TakingDamage( updateTime );
		}

		if( m_SubState.Boosting )
		{
			BoostBehaviour( updateTime );
		}

		// Methods that deal with any collision behaviour
		if( m_SubState.Collided )
		{
			CollisionBehaviour( updateTime );
		}
		else
		{
			TankCollision( updateTime, Messenger );
		}


		///////////////////////////////////////////////
		// AI Tank Movement

		// Always have the tank moving on it's Z by it's speed property,
		// then we can simply manipulate the speed
		Matrix().MoveLocalZ( m_Speed*updateTime );

		if( m_Speed == 0.0f )
			m_Speed = 0.01;

		if( m_Speed < m_MaxSpeed )
			m_Speed *= m_TankTemplate->Acceleration();
		

		//////////////////////////////////////////////
		// Weapon decision making

		if( m_PowerUp == MachineGun )
		{
			MachineGunDecision( updateTime, Messenger );
		}
		else if( m_PowerUp == Mortar )
		{				
			MortarTurretAction( updateTime );
			MortarDecision( updateTime, Messenger );
		}
		else if( m_PowerUp == SpeedBoost )
		{
			SpeedboostDecision( updateTime );
		}
		else
		{
			TurretAction( updateTime );
		}

		//////////////////////////////////////////////
		// AI Lane movement

		// Every 10000 frames make decision whether to switch lanes
		if( m_Frames % 10000 )
		{
			LaneSwitchDecision( updateTime );
			m_Frames = 0;
		}

		if( (*m_TrackPiece)->Template()->Name() != "Track Corner" )
		{
			int iLane = CheckLane();

			if( iLane < m_Lane )
			{
				Matrix().RotateLocalY( m_TurnSpeed/2*updateTime );
			}
			else if( iLane > m_Lane )
			{
				Matrix().RotateLocalY( -m_TurnSpeed/2*updateTime );
			}
			else
			{
				TurnTowardsVector( updateTime, m_TurnSpeed, (*m_TrackPiece)->Matrix().ZAxis(),
								   Matrix() );
			}
		}
		else
		{
			if( m_Waypoints == NULL )
			{
				m_Waypoints = (*m_TrackPiece)->Waypoints( m_Lane );
				m_WaypointIter = m_Waypoints->begin();
			}

			TurnTowardsTarget( updateTime, m_TurnSpeed, (*m_WaypointIter),
							   Matrix(), Matrix() );

			CheckWaypointCollision();
		}

		// Chsgeck whether the tank is on the track
		if( !TrackTesting() )
		{
			m_State = OffTrack;
		}
	}
	else if( m_State == OffTrack )
	{
		OffTrackBehaviour( updateTime, Messenger );
	}
	else if( m_State == ExplosionDamage )
	{
		ExplosionBehaviour( updateTime );
	}
	else if( m_State == Dead )
	{
		DeathBehaviour( updateTime, Messenger );
	}
	else if( m_State == Restarting )
	{
		RestartingBehaviour( updateTime, Messenger );
	}
	else if( m_State == Starting )
	{
		StartingBehaviour( updateTime );
	}
	else if( m_State == Won || m_State == Lost )
	{
		if( m_Speed > 0.0f )
			m_Speed = -10.0f;
		if( m_Speed < 0.0f )
			m_Speed = 0.0f;

		Matrix().MoveLocalZ( m_Speed*updateTime );
	}

	CheckWin();

	++m_Frames;

	return true;
}

void CTankEntityAI::CheckWaypointCollision()
{
	if( fabs( Matrix().Position().x  - (*m_WaypointIter).x ) < POINT_RADIUS ||
		fabs( Matrix().Position().z  - (*m_WaypointIter).z ) < POINT_RADIUS )
	{
		// Only increment the iterator if it is not the last one (stops crashes)
		if( *m_WaypointIter != m_Waypoints->back() )
			++m_WaypointIter;
	}
}

//////////////////////////////////////////
// Tank AI decision making methods

// Make decision whether to use speedboost - decision based on section of track
void CTankEntityAI::SpeedboostDecision( float updateTime )
{
	TrackIter i;

	if( m_TrackPiece == m_Track->Back() || m_TrackPiece == m_Track->Back()-1 )
	{
		i = m_Track->Begin()+1;
	}
	else
	{
		i = m_TrackPiece+2;
	}		

	if( (*m_TrackPiece)->Template()->Name() == "Track Corner" && 
		( (*i)->Template()->Name() == "Track Walkway" || (*i)->Template()->Name() == "Track Straight" ) )
    {
		float dist = 0.0f;

		if( m_TrackPiece != m_Track->Back() )
		{
			++m_TrackPiece;
			dist = Distance( Matrix().Position(), (*m_TrackPiece)->Matrix().Position() );	
			--m_TrackPiece;
		}
		else
		{
			dist = Distance( Matrix().Position(), (*m_Track->Begin())->Matrix().Position() );	
		}

		if( abs(dist) <= 25.0f )
		{
			m_SubState.Boosting = true;
			m_PowerUp = None;
		}
	}
}

// Make decision on whether to used mortar - decision based on positioning of enemy tanks
void CTankEntityAI::MortarDecision( float updateTime, CMessenger* Messenger )
{
	vector<CEntity*>::iterator iter;

	iter = m_TankList->begin();

	while( iter != m_TankList->end() )
	{
		CTankEntity* tank = static_cast<CTankEntity*>( (*iter) );
		if( tank->Name() != Name() )
		{
			float dist = DistanceFromAxis( Matrix().Position(), tank->Matrix().Position(), tank->Matrix().ZAxis() );
			if( dist > -TANK_DIST && ( tank->m_TrackPiece >= m_TrackPiece ) )
			{
				MortarShoot( updateTime, Messenger );
				return;
			}
		}

		++iter;
	}
}

// Make decision on whether to fire machine gun - decision based on position of enemy tanks
void CTankEntityAI::MachineGunDecision( float updateTime, CMessenger* Messenger )
{
	vector<CEntity*>::iterator iter;

	iter = m_TankList->begin();

	while( iter != m_TankList->end() )
	{
		CTankEntity* tank = static_cast<CTankEntity*>( (*iter) );
		if( tank->Name() != Name() )
		{
			float dist = DistanceFromAxis( Matrix().Position(), tank->Matrix().Position(), tank->Matrix().ZAxis() );
			if( abs( m_Lane - tank->m_Lane ) <= 1 && dist < 0.0f )
			{
				MachineGunShoot( updateTime, Messenger );
			}
			else
			{
				TankTemplate()->SoundManager()->StopSound( m_SourceNum2 );
			}
		}

		++iter;
	}
}

// Make random decision on whether to change lanes
void CTankEntityAI::LaneSwitchDecision( float updateTime )
{
	int a = 0;
	int b = 50000;

	if( Random( a, b ) == 2500 )
	{
		int i = Random( 0, 1 );

		if( i == 0 )
		{
			if( m_Lane == Lane1 )
				++m_Lane;
			else
				--m_Lane;
		}
		else
		{
			if( m_Lane == Lane4 )
				--m_Lane;
			else
				++m_Lane;
		}
	}
}

// Function slows down / speeds up AI tank based on position of player.
// Aim is to keep the tanks in a group. Distance is calculated by working
// out differences between iterators.
void CTankEntityAI::SpeedChanges( float updateTime )
{
	if( (m_TrackPiece - m_Player->m_TrackPiece) < 1 )
	{
		if( (m_TrackPiece - m_Player->m_TrackPiece) < 1 )
		{
			m_MaxSpeed = m_TankTemplate->MaxSpeed()*1.2f;
		}
		else if( (m_TrackPiece - m_Player->m_TrackPiece) < 2 )
		{
			m_MaxSpeed = m_TankTemplate->MaxSpeed()*1.3f;
		}
		else if( (m_TrackPiece - m_Player->m_TrackPiece) < 3 )
		{
			m_MaxSpeed = m_TankTemplate->MaxSpeed()*1.5f;
		}
	}
	else if( (m_TrackPiece - m_Player->m_TrackPiece) > 2 )
	{
		if( (m_TrackPiece - m_Player->m_TrackPiece) > 2 )
		{
			m_MaxSpeed = m_TankTemplate->MaxSpeed()*0.8f;
		}
		else if( (m_TrackPiece - m_Player->m_TrackPiece) > 3 )
		{
			m_MaxSpeed = m_TankTemplate->MaxSpeed()*0.7f;
		}
		else if( (m_TrackPiece - m_Player->m_TrackPiece) > 4 )
		{
			m_MaxSpeed = m_TankTemplate->MaxSpeed()*0.6f;
		}
	}
	else
	{
		m_MaxSpeed = m_TankTemplate->MaxSpeed();
	}

	if( m_Speed > m_MaxSpeed )
	m_Speed = m_MaxSpeed;
}

} // namespace malaz