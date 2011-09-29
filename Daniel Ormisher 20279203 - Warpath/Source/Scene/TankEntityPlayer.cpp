#include "TankEntityPlayer.h"

namespace malaz{

CMatrix4x4 rotMat;
CMatrix4x4 moveMat;

CTankEntityPlayer::CTankEntityPlayer( CTankTemplate* Template, string Name,
									  CTrack* Track,
									  vector<CEntity*>* TankVector,
									  CMessenger* Messenger,
									  CGUI* GUI,
				                      CVector3 pos,
				                      CVector3 rot,
				                      CVector3 scale )
                                      : CTankEntity( Template, Name, Track, TankVector, Messenger, GUI, pos, rot, scale )
{
    m_Speed = 0.0f;
	rotMat = CMatrix4x4( CVector3( 0.0f, 0.0f, 0.0f ) );
	rotMat = CMatrix4x4( CVector3( 0.0f, 0.0f, 0.0f ) );
}

bool CTankEntityPlayer::Update( float updateTime, CMessenger* Messenger, CD3DManager* d3dManager )
{
	// Fetch any messages
	ProcessMessages( Messenger );

	// Set the correct power up to be displayed by the GUI
	if( m_GUI != NULL )
		m_GUI->PowerUp( m_PowerUp );

	// Check what lane the tank is (only used for AI making shooting decisions based on lane position)
	m_Lane = CheckLane();

	// Increase the pitch of the engine noise
	m_EnginePitch = m_Speed*2/m_MaxSpeed+1.0f;
	TankTemplate()->SoundManager()->UpdateSound( m_SourceNum, Matrix().Position(), Sounds_Engine, m_EnginePitch );
	TankTemplate()->SoundManager()->UpdateSound( m_SourceNum2, Matrix().Position(), Sounds_Engine, 1.0f );

	if( m_State == Normal )
	{
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

		// Always have the tank moving on it's Z by it's speed property,
		// then we can simply manipulate the speed
		Matrix().MoveLocalZ( m_Speed*updateTime );

		// Check for user input controls
		PlayerControl( updateTime );

		/////////////////////////////////////////
		// Power Up Functions
		if( m_PowerUp == MachineGun )
		{
			if( KeyHeld( Key_Space ) )
				MachineGunShoot( updateTime, Messenger );
			else
				TankTemplate()->SoundManager()->StopSound( m_SourceNum2 );
		}
		else if( m_PowerUp == Mortar )
		{				
			MortarTurretAction( updateTime );

			if( KeyHit( Key_Space ) )
				MortarShoot( updateTime, Messenger );
		}
		else if( m_PowerUp == SpeedBoost )
		{
			if( KeyHit( Key_Space ) )
			{
				m_SubState.Boosting = true;
				m_PowerUp = None;
			}
		}
		else
		{
			TurretAction( updateTime );
		}

		// Check the player is on the track
		if( !TrackTesting() )
		{
			m_State = OffTrack;
		}

	} // end if m_onTrack
	else if( m_State == OffTrack )
	{
		OffTrackBehaviour( updateTime, Messenger );
	}
	else if( m_State == ExplosionDamage )
	{
		ExplosionBehaviour( updateTime );
	}
	else if( m_State == Restarting )
	{
		RestartingBehaviour( updateTime, Messenger );
	}
	else if( m_State == Dead )
	{
		DeathBehaviour( updateTime, Messenger );
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

	return true;
}

void CTankEntityPlayer::PlayerControl( float updateTime )
{
	// If forwards key is pressed
	if( KeyHeld( Key_W ) )
	{
		if( m_Speed == 0.0f )
			m_Speed = 0.01;

		if( m_Speed < m_MaxSpeed )
			m_Speed *= m_TankTemplate->Acceleration();
		else if( m_Speed > m_MaxSpeed )
			m_Speed -= ( m_TankTemplate->Acceleration() - 1.0f )/8;
	}
	else
	{
		if( m_Speed > 0.0f )
			m_Speed -= ( m_TankTemplate->Acceleration() - 1.0f )/15;
		else if( m_Speed <= 0.0f )
			m_Speed = 0.0f;
	}

	// If the reverse / brake key is pressed
	if( KeyHeld( Key_S ) )
	{
		if( m_Speed <= 0.0f )
			m_Speed = -10.0f;
		else
			m_Speed -= 0.05f;		
	}

	if( KeyHeld( Key_A ) )
	{
		if( m_Speed > 0.1f && m_Speed < 0.4 )
		{
			Matrix().RotateLocalY( -( 0.1*updateTime ) );
		}
		else if( m_Speed > 0.4 || m_Speed < 0.0f )
		{
			Matrix().RotateLocalY( -( m_TurnSpeed*updateTime ) );
		}
	}
	else if( KeyHeld( Key_D ) )
	{
		if( m_Speed > 0.1f && m_Speed < 0.4 )
		{
			Matrix().RotateLocalY( 0.1*updateTime );
		}
		else if( m_Speed > 0.4 || m_Speed < 0.0f )
		{
			Matrix().RotateLocalY( m_TurnSpeed*updateTime );
		}
	}
}

} // end namespace malaz