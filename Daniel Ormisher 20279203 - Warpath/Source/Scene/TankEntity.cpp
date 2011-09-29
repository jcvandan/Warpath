#include "TankEntity.h"

namespace malaz{

int SourceNum=1;

CTankEntity::CTankEntity( CTankTemplate* Template, string Name,
					      CTrack* Track,
						  vector<CEntity*>* TankVector,
						  CMessenger* Messenger,
						  CGUI* GUI,
					      CVector3 pos,// = CVector3( 0.0f, 0.0f, 0.0f ), 
					      CVector3 rot,// = CVector3( 0.0f, 0.0f, 0.0f ),  
					      CVector3 scale )// = CVector3( 1.0f, 1.0f, 1.0f ) )
					      : CEntity( Template, Name, pos, rot, scale )
{
	// Store pointer to tank template
	m_TankTemplate = Template;

	m_TankList = TankVector;

	// Store pointer to the track for iteration purposes, initialise track iterator
	// property with the first piece of the track
	m_Track = Track;
	m_TrackPiece = Track->Begin();

	// Initialise other proeprties properties
	m_Speed = 0.0f;	
	m_PowerUp = None;
	m_TurretAngle = 0.0f;
	m_NumLaps = 0;
	m_ExplosionHeight = 25.0f;

	m_Timer = 0.0f;
	
	m_GUI = GUI;

	m_Health = m_TankTemplate->MaxHealth();

	m_TurnSpeed = m_TankTemplate->TurnSpeed();
	m_MaxSpeed = m_TankTemplate->MaxSpeed();

	m_State = Starting;

	CMsgParticleSystem* msg = new CMsgParticleSystem( m_Name, Msg_CreateTankBottom, "TankBottom", &Matrix() );
	Messenger->SendMessageA( PS_MANAGER, msg );

	msg = new CMsgParticleSystem( m_Name, Msg_CreateCollision, "MachineGunCollision", &Matrix() );
	Messenger->SendMessageA( PS_MANAGER, msg );

	msg = new CMsgParticleSystem( m_Name, Msg_CreateFire, "Fire", &Matrix() );
	Messenger->SendMessageA( PS_MANAGER, msg );

	// Set up the sound stuff
	m_SourceNum = SourceNum;
	++SourceNum;
	m_SourceNum2 = SourceNum;
	++SourceNum;

	m_EnginePitch = 1.0f;
	TankTemplate()->SoundManager()->Play( m_SourceNum, Matrix().Position(), Sounds_Engine, true, m_EnginePitch, 10.0f );
}

bool CTankEntity::SetPowerUp( ePowerUp type, int Bullets, CMessenger* Messenger )
{
	// If we already have a power up return false because the power up cant be picked up
	if( m_PowerUp != None )
	{		
		return false;
	}
	else
	{
		m_PowerUp = type;

		if( m_PowerUp == MachineGun )
		{
			// Send a message to the particle manager to create a machine gun particle system
			CMsgParticleSystem* msg = new CMsgParticleSystem( m_Name, Msg_CreateMachineGunSystem, "MachineGun", &Matrix() );
			Messenger->SendMessageA( "Particle System Manager", msg );
		}
		if( m_PowerUp == Mortar )
		{
			// Send a message to the particle manager to create a mortar particle system
			CMsgMortar* msg = new CMsgMortar( m_Name, Msg_CreateMortar, "Mortar", &m_TrackPiece, &Matrix() );
			Messenger->SendMessageA( "Particle System Manager", msg );
		}
		if( m_PowerUp == Heal )
		{
			m_Health = m_TankTemplate->MaxHealth();
		}

		return true;
	}
}

void CTankEntity::ProcessMessages( CMessenger* Messenger )
{
	// Fetch any messages
	CMsg* msg = Messenger->FetchMessage( m_UID );
	while ( msg )
	{
		// If a picked up message is sent recieve the power up goes inactive
		if( msg->type == Msg_PickUp )
		{
			// Must cast the msg to the correct derived type
			CMsgPowerUp* m = static_cast<CMsgPowerUp*>( msg );

			// If the power up is set send confirmation message to power up, otherwise do nothing
			if( SetPowerUp( m->powerUp, m->numBullets, Messenger ) )
			{
				CMsg* newMsg = new CMsg( m_Name, Msg_PickedUp );
				Messenger->SendMessageA( msg->from, newMsg );
			}
		}
		else if( msg->type == Msg_WeaponRunOut )
		{
			m_PowerUp = None;
		}
		else if( msg->type == Msg_EmitCollision )
		{
			CMsg* msg = new CMsg( Name(), Msg_EmitCollision );
			Messenger->SendMessageA( PS_MANAGER, msg );

			m_SubState.TakingDamage = true;
			m_SubState.DamageTimer = 0.0f;
		}
		else if( msg->type == Msg_Explosion )
		{
			CMsgParticleSystem* newMsg = static_cast<CMsgParticleSystem*>( msg );
			// Calculate the distance from the blast
			float dist = Distance( *newMsg->position, Matrix().Position() );

			// If its within range set state to ExplosionDamage
			if( dist <= BLAST_RADIUS )
			{
				m_State = ExplosionDamage;
				m_ExplosionVector = CVector3( *newMsg->position, Matrix().Position() );
				m_ExplosionVector.y = m_ExplosionHeight;
			}
		}
		else if( msg->type == Msg_Collision )
		{
			CMsgCollision* newMsg = static_cast<CMsgCollision*>( msg );

			m_SubState.Collided = true;
			m_CollisionVector = CVector3( newMsg->pos, Matrix().Position() );
			m_CollisionMovement = newMsg->moveAmount;
		}

		// Now free up the memory from the message
		delete msg;
		msg = Messenger->FetchMessage( m_UID );
	}
}

void CTankEntity::MachineGunShoot( float updateTime, CMessenger* Messenger )
{
	CMsgParticleSystem* msg = new CMsgParticleSystem( m_Name, Msg_ShootMachineGun, "MachineGun", &Matrix() );
	Messenger->SendMessageA( "Particle System Manager", msg );

	TankTemplate()->SoundManager()->Play( m_SourceNum2, Matrix().Position(), Sounds_Gun, false, 1.0f, 20.0f );
}

void CTankEntity::MortarShoot( float updateTime, malaz::CMessenger *Messenger )
{
	CMsgParticleSystem* msg = new CMsgParticleSystem( m_Name, Msg_ShootMortar, "", NULL );
	Messenger->SendMessageA( "Particle System Manager", msg );

	m_PowerUp = None;

	TankTemplate()->SoundManager()->Play( m_SourceNum2, Matrix().Position(), Sounds_Mortar );
}

void CTankEntity::MortarTurretAction( float updateTime )
{
	if( m_TurretAngle < ToRadians( 35.0f ) )
	{
		float rotateAmount = 1.0f*updateTime;
		Matrix(4).RotateLocalX( -rotateAmount );
		m_TurretAngle += rotateAmount;
	}
}

void CTankEntity::TurretAction( float updateTime )
{
	if( m_TurretAngle > ToRadians( 1.0f ) )
	{
		float rotateAmount = 1.0f*updateTime;
		Matrix(4).RotateLocalX( rotateAmount );
		m_TurretAngle -= rotateAmount;
	}
}

// This function tests where the tank is in relation to the track
bool CTankEntity::TrackTesting()
{
	//////////////////////////////////////////////////
    // TRACK

	/*********Calculate distance from track's origin**********************/

	float x = Matrix().Position().x - (*m_TrackPiece)->Matrix().Position().x;
	float y = Matrix().Position().y - (*m_TrackPiece)->Matrix().Position().y;
	float z = Matrix().Position().z - (*m_TrackPiece)->Matrix().Position().z;
	
	float dist = sqrt( x*x + y*y + z*z );

	// if the track piece is a corner
	if( (*m_TrackPiece)->Template()->Name() == "Track Corner" )
    {
		// First calculate the distance from the corners z axis
        CVector3 v( (*m_TrackPiece)->Matrix().Position(), Matrix().Position() );
        float distanceZ = Dot( v, Normalise( (*m_TrackPiece)->Matrix().ZAxis() ) );

		// Calculate the distance from the corners x axis
		v = CVector3( (*m_TrackPiece)->Matrix().Position(), Matrix().Position() );
        float distanceX = Dot( v, Normalise( (*m_TrackPiece)->Matrix().XAxis() ) );

		// Test for the top right corner of the piece
		if( distanceZ < -11.0f && distanceX > 9.0f )
        {
            return false;
        }

		// Tests for the bottom right corner
		if( distanceZ < -16.0f && distanceX <= 9.0f )
        {
			return false;
        }

		if( distanceX < -11.0f && distanceZ > 9.0f )
		{
           return false;
        }

		// Test for the bottom left corner
		if( distanceX < -16.0f && distanceZ <= 9.0f )
		{
            return false;
        }

		// Test for the top left corner
		if( distanceX > 12.0f && distanceZ > 12.0f )
		{
            return false;
        }

		// Slightly convoluted process to testing whether to move to the next track, because there are two
		// axes to test for and these axes are changed for different corner pieces we must check the distance
		// from both z and x axes, if it is the required distance from either, we then need to decide whether 
		// to increment or decrement the iterator, this is done based on the distance from the next and previous
		// pieces. It is made further complicated due to the fact that we must test if the pieces are the last or
		// first piece of the track because if we decrement either of these the game will crash
		if( distanceX > (*m_TrackPiece)->BoundsZ() && distanceZ < (*m_TrackPiece)->BoundsX() ||
			distanceZ > (*m_TrackPiece)->BoundsZ() && distanceX < (*m_TrackPiece)->BoundsX() )
		{
			DistanceToNextTrack();	
		}
    }
	else
    {
		/*********Calculate distance from track's Z axis**********************/
		//Calculate a vector from the tank to the track's origin
        CVector3 v( (*m_TrackPiece)->Matrix().Position(), Matrix().Position() );

		// The dot product of v and the tracks x axis will give us the distance
        float distance = Dot( v, Normalise( (*m_TrackPiece)->Matrix().XAxis() ) );

		// If the distance is 
		if( fabs(distance) > (*m_TrackPiece)->BoundsX() )
        {
            return false;
        }

		/*********Calculate distance from track's X axis**********************/
        v = CVector3( (*m_TrackPiece)->Matrix().Position(), Matrix().Position() );

        distance = Dot( v, Normalise( (*m_TrackPiece)->Matrix().ZAxis() ) );

		// If the absolute value of distance is greater than the max Z bounds, we must
		// change track piece
		if( fabs(distance) > (*m_TrackPiece)->BoundsZ() )
		{
			DistanceToNextTrack();
		}
    }

	return true;
}


void CTankEntity::DistanceToNextTrack()
{
	// Calculate dist from next and previous pieces
	float x, z, distNext, distPrev;

	// If the current piece is not the last piece
	if( m_TrackPiece != m_Track->Back() )
	{
		x = Matrix().Position().x - (*(m_TrackPiece+1))->Matrix().Position().x;
		z = Matrix().Position().z - (*(m_TrackPiece+1))->Matrix().Position().z;				
		distNext = sqrt( x*x + z*z );
	}
	// If it is the last piece, the distance to the next piece must be the distance to the
	// first piece of track
	else
	{
		x = Matrix().Position().x - (*m_Track->Begin())->Matrix().Position().x;
		z = Matrix().Position().z - (*m_Track->Begin())->Matrix().Position().z;				
		distNext = sqrt( x*x + z*z );
	}

	// If the current piece is not the first piece
	if( m_TrackPiece != m_Track->Begin() )
	{
		x = Matrix().Position().x - (*(m_TrackPiece-1))->Matrix().Position().x;
		z = Matrix().Position().z - (*(m_TrackPiece-1))->Matrix().Position().z;				
		distPrev = sqrt( x*x + z*z );
	}
	// If it is the distance to the previous piece must be the dist to the last piece of the track
	else
	{
		x = Matrix().Position().x - (*m_Track->Begin())->Matrix().Position().x;
		z = Matrix().Position().z - (*m_Track->Begin())->Matrix().Position().z;				
		distPrev = sqrt( x*x + z*z );
	}

	// If distNext is smaller than distPrev, we increment
	if( distNext < distPrev )
	{
		if( m_TrackPiece != m_Track->Back() )
		{
			m_TrackPiece++;
			m_Waypoints = NULL;
		}
		else
		{
			m_TrackPiece = m_Track->Begin();			
			m_Waypoints = NULL;
			++m_NumLaps;
		}
	}
	// If distPrev is smaller than distNext, we decrememt
	else// if( distPrev < distNext )
	{
		if( m_TrackPiece != m_Track->Begin() )
		{
			m_TrackPiece--;			
			m_Waypoints = NULL;
		}
		else
		{
			m_TrackPiece = m_Track->Back();			
			m_Waypoints = NULL;
		}
	}		
}

int CTankEntity::CheckLane()
{
	// First we must calculate the distance from the current track pieces ZAxis
	CVector3 v( (*m_TrackPiece)->Matrix().Position(), Matrix().Position() );
    float distanceZ = Dot( v, Normalise( (*m_TrackPiece)->Matrix().XAxis() ) );

	if( distanceZ > 3.5f )
		return 0;
	else if( distanceZ >= 0.0f && distanceZ <= 3.5f )
		return 1;
	else if( distanceZ < 0.0f && distanceZ >= -3.5f )
		return 2;
	else if( distanceZ < -3.5f )
		return 3;
}

double CTankEntity::TurnTowardsTarget(  TFloat32 updateTime,				
										double turnSpeed,
										CVector3 targetPos,
										CMatrix4x4& matrix,
										CMatrix4x4& relMatrix )
{
	//First Normalise X and Z axis
	CVector3 normX = Normalise( relMatrix.XAxis() );
	CVector3 normZ = Normalise( relMatrix.ZAxis() );

	//Then normalise vector from source to target
	CVector3 normTargVec = Normalise( CVector3( relMatrix.Position(), targetPos ) );

	//Get dot products of angles and vectors
	double b = Dot( normX, normTargVec );
	
	double a = acos( Dot( normZ, normTargVec ) );

	if( a > ToRadians( 2.0f ) )
	{
		if( b > ToRadians(0.0f) )
		{			
			matrix.RotateLocalY( turnSpeed*updateTime );
		}
		else
		{
			matrix.RotateLocalY( -turnSpeed*updateTime );
		}
	}
	return a;
}

void CTankEntity::ExplosionBehaviour( float updateTime )
{
	float height;

	if( TrackTesting() )
		height = 11.0f;
	else
		height = 2.0f;

	if( m_ExplosionHeight >= 25.0f )
	{
		Matrix().RotateLocalX( Random( 0.0f, 10.0f )*updateTime );
		Matrix().RotateLocalZ( Random( 0.0f, 10.0f )*updateTime );
	}

	if( Matrix().Position().y >= height )
	{
		Matrix().Move( CVector3( m_ExplosionVector.x*2*updateTime, 
								 m_ExplosionVector.y*updateTime,
								 m_ExplosionVector.z*2*updateTime ) );			
	}
	else if( m_ExplosionHeight >= 1.0f )
	{
		m_ExplosionHeight *= 0.25;
		Matrix().MoveY( 0.1f );
		m_ExplosionVector.y = m_ExplosionHeight;
	}
	else
	{
		Matrix().Move( CVector3( m_ExplosionVector.x*2*updateTime, 
								 0.0f,
								 m_ExplosionVector.z*2*updateTime ) );

		if( m_ExplosionVector.x > 0.0f )
			m_ExplosionVector.x -= 3.0f*updateTime;
		if( m_ExplosionVector.z > 0.0f )
			m_ExplosionVector.z -= 3.0f*updateTime;

		if( m_ExplosionVector.x <= 0.0f && m_ExplosionVector.z <= 0.0f )
		{
			m_ExplosionHeight = 25.0f;
			m_State = Dead;
		}
	}

	m_ExplosionVector.y -= 35.0f*updateTime;
}

void CTankEntity::TakingDamage( float updateTime )
{
	if( m_SubState.DamageTimer == 0.0f )
	{
		m_TurnSpeed = m_TankTemplate->TurnSpeed()/1.5;
		m_Health -= 2.0f*updateTime;
	}

	// If the timer goes past 1 second restor normal sub state
	if( m_SubState.DamageTimer >= 1.0f )
	{
		m_SubState.TakingDamage = false;
		m_TurnSpeed = m_TankTemplate->TurnSpeed();
		m_MaxSpeed = m_TankTemplate->MaxSpeed();
		m_SubState.DamageTimer = 0.0f;
	}
	else
	{
		m_SubState.DamageTimer += updateTime;
		m_Health -= 2.0f*updateTime;
	}
}

void CTankEntity::BoostBehaviour( float updateTime )
{
	if( m_SubState.BoostTimer == 0.0f )
	{
		m_MaxSpeed = m_TankTemplate->MaxSpeed()*1.5f;
	}

	if( m_SubState.BoostTimer >= 1.0f )
	{
		m_SubState.Boosting = false;
		m_MaxSpeed = m_TankTemplate->MaxSpeed();
		m_SubState.BoostTimer = 0.0f;
	}
	else
	{
		m_SubState.BoostTimer += updateTime;
	}
}

/////////////////////////////////////////////////////
// Tank Collision methods

void CTankEntity::TankCollision( float updateTime, CMessenger* Messenger )
{
	vector<CEntity*>::iterator iter;

	iter = m_TankList->begin();

	while( iter != m_TankList->end() )
	{
		if( (*iter)->Name() != Name() )
		{
			// Calculate the squared distance between the power up and tank and check if it is less than
			// the squared sum of the the two bounding radii
			CVector3 pos ( (*iter)->Matrix().Position(), Matrix().Position() );
			float d1 = pos.x * pos.x + pos.y * pos.y + pos.z * pos.z;
			float d2 = (*iter)->ScaledRadius() + ScaledRadius();

			// If there has been an intersection of bounding spheres...
			if( d1 <= d2 * d2 )
			{
				// Do a more accurate OOBB test
				if( (*iter)->CheckOOBBtoOOBBIntersection( m_ZAxisRadius, m_XAxisRadius, Matrix().Position() ) )
				{
					m_CollisionVector = CVector3( (*iter)->Matrix().Position(), Matrix().Position() );

					m_SubState.Collided = true;
					CMsgCollision* msg = new CMsgCollision( Name(), Msg_Collision, Matrix().Position(), 0.0f );
					Messenger->SendMessageA( (*iter)->Name(), msg );
				}
			}
		}

		++iter;
	}
}

void CTankEntity::DeathBehaviour( float updateTime, CMessenger* Messenger )
{
	m_Timer += updateTime;

	CMsg* msg = new CMsg( Name(), Msg_EmitFire );
	Messenger->SendMessageA( PS_MANAGER, msg );

	if( m_Timer >= 0.5f )
	{
		m_State = Restarting;
		m_Timer = 0.0f;
	}
}

void CTankEntity::OffTrackBehaviour( float updateTime, CMessenger* Messenger )
{
	if( Matrix().Position().y > 2.0f )
	{
		Matrix().MoveY( -15.0f*updateTime );
		Matrix().MoveLocalZ( m_Speed*updateTime );
		Matrix().RotateLocalZ( 5.0f*updateTime );
	}
	else
	{
		m_Timer += updateTime;

		if( m_Timer >= 0.5f )
		{
			m_PowerUp = None;
			m_MaxSpeed = m_TankTemplate->MaxSpeed();
			m_TurnSpeed = m_TankTemplate->TurnSpeed();
			m_State = Dead;
			m_Timer = 0.0f;
		}
	}
}

void CTankEntity::RestartingBehaviour( float updateTime, CMessenger* Messenger )
{
	if( m_Name == PLAYER_TANK )
	{
		CMsg* msg = new CMsg( m_Name, Msg_BackOnTrack );
		Messenger->SendMessageA( CHASE_CAMERA, msg );
	}

	Matrix().SetPosition( (*m_TrackPiece)->Matrix().Position() );
	Matrix().SetY( 11.0f );

	if( m_TrackPiece == m_Track->Back() )
	{
		if( (*m_TrackPiece)->Template()->Name() == "Track Corner" )
		{
			Matrix().FaceDirection( -(*m_Track->Begin())->Matrix().ZAxis() );
		}
		else
		{
			Matrix().FaceDirection( -(*m_TrackPiece)->Matrix().ZAxis() );
		}
	}
	else
	{
		if( (*m_TrackPiece)->Template()->Name() == "Track Corner" )
		{
			++m_TrackPiece;
			Matrix().FaceDirection( -(*m_TrackPiece)->Matrix().ZAxis() );
			--m_TrackPiece;
		}
		else
		{
			Matrix().FaceDirection( -(*m_TrackPiece)->Matrix().ZAxis() );
		}
	}

	m_Speed = 0.0f;
	m_State = Normal;
}

void CTankEntity::CollisionBehaviour( float updateTime )
{	
	Matrix().Move( CVector3( m_CollisionVector.x*2.0f*updateTime,
						     0.0f,
						     m_CollisionVector.z*2.0f*updateTime ) );

	m_SubState.CollisionTimer += updateTime;

	if( m_SubState.CollisionTimer >= 0.1f )
	{
		m_SubState.Collided = false;
		m_SubState.CollisionTimer = 0.0f;
	}
}

void CTankEntity::StartingBehaviour( float updateTime )
{
	m_Timer += updateTime;

	if( m_Timer >= 4.0f )
	{
		m_State = Normal;
		m_Timer = 0.0f;
	}
}

void CTankEntity::CheckWin()
{
	if( m_NumLaps >= WIN_LAPS )
	{
		m_State = Won;

		if( Name() == PLAYER_TANK )
			m_GUI->SetWinState( true );
		else
			m_GUI->SetWinState( false );

		vector<CEntity*>::iterator iter;
		iter = m_TankList->begin();

		while( iter != m_TankList->end() )
		{
			if( (*iter)->Name() != Name() )
			{
				static_cast<CTankEntity*>( *iter )->SetLostState();
			}
			++iter;
		}
			
	}
}

} // namespace malaz