/*************************************************************
	Implementation of CChaseCamera's update function
**************************************************************/
#include "ChaseCamera.h"

namespace malaz{

const float HEIGHT = 15.5f;
const float MIN_DIST = 15.0f;
const float MAX_DIST = 20.0f;

int FrameCount = 0;
float lastFrameSpeed = 0.0f;

CChaseCamera::CChaseCamera( CTankEntityPlayer* target, CVector3 position, CVector3 rotation ) : CCamera( position, rotation )
{
	//Set the target property to the entity passed
	m_Target = target;

	// Set the camera's position behind the tank
	Matrix().SetPosition( CVector3( target->Matrix().Position() ) );
	Matrix().FaceDirection( target->Matrix().ZAxis() );
	Matrix().MoveLocal( CVector3( 0.0f, 5.0f, -MIN_DIST ) );

	m_UID = CHASE_CAMERA;

	m_Speed = 0.0f;
}

void CChaseCamera::ProcessMessages( CMessenger* Messenger )
{
	// Fetch any messages
	CMsg* msg = Messenger->FetchMessage( m_UID );
	while ( msg )
	{
		// If a picked up message is sent recieve the power up goes inactive
		if( msg->type == Msg_BackOnTrack )
		{
			// Set the camera's position behind the tank
			Matrix().SetPosition( CVector3( m_Target->Matrix().Position() ) );
			Matrix().FaceDirection( m_Target->Matrix().ZAxis() );
			Matrix().MoveLocal( CVector3( 0.0f, 5.0f, -MIN_DIST ) );
		}

		delete msg;
		msg = Messenger->FetchMessage( m_UID );
	}
}

void CChaseCamera::Update( float updateTime, CMessenger* Messenger )
{
	ProcessMessages( Messenger );

	CalculateMatrices();

	if( m_Target->OnTrack() && m_Target->Racing() )
	{
		// Calculate the distance from the camera to the target
		double dX = Matrix().Position().x - m_Target->Matrix().Position().x;
		double dZ = Matrix().Position().z - m_Target->Matrix().Position().z;
		double dist = sqrt( dX*dX + dZ*dZ );

		// If the distance is greater than the max distance juts move the camera at the same speed as the tank
		if( dist >= MAX_DIST || dist <= MIN_DIST )
			m_Speed = m_Target->Speed();
		else
			m_Speed = m_Target->Speed()/2;

		Matrix().MoveLocalZ( m_Speed*updateTime );


		////////////////////////////////////////////////////////
		// Cameras Rotation

		// Calulate the angle between the targets Z axis and the camera Z axis
		// Normalise the Z axes of cam and target
		CVector3 mNormZ = Normalise( Matrix().ZAxis() );
		CVector3 tNormZ = Normalise( m_Target->Matrix().ZAxis() );
		// Calculate the dot of thes axes
		double dot = Dot( mNormZ, tNormZ );
		// The inverse cosine of this calculation is the angle between the 2 axes
		double alpha = ToDegrees( acos( dot ) );

		// Need to calculate which way to turn, this is done by getting the Dot product of
		// the cameras X axis and the target axis
		CVector3 mNormX = Normalise( Matrix().XAxis() );
		double beta = Dot( mNormX, tNormZ );

		// If the angle is small do slow rotation
		if( alpha >= 0.1f && alpha < 5.0f )
		{
			if( beta > 0.0f )
				RotateAroundPoint( m_Target->Matrix().Position(), ( m_Target->TankTemplate()->TurnSpeed()/2 )*updateTime );
			else
				RotateAroundPoint( m_Target->Matrix().Position(), -( m_Target->TankTemplate()->TurnSpeed()/2 )*updateTime );
		}
		// If the angle is above 5.0f, we need to speed up the rotation to catch up with the tank
		else if( alpha >= 5.0f && alpha < 10.0f )
		{
			if( beta > 0.0f )
				RotateAroundPoint( m_Target->Matrix().Position(), m_Target->TankTemplate()->TurnSpeed()*updateTime );
			else
				RotateAroundPoint( m_Target->Matrix().Position(), -m_Target->TankTemplate()->TurnSpeed()*updateTime );
		}
		else if( alpha >= 10.0f )
		{
			if( beta > 0.0f )
				RotateAroundPoint( m_Target->Matrix().Position(), ( m_Target->TankTemplate()->TurnSpeed()*1.5f )*updateTime );
			else
				RotateAroundPoint( m_Target->Matrix().Position(), -( m_Target->TankTemplate()->TurnSpeed()*1.5f )*updateTime );
		}


		////////////////////////////////////////////////////////
		// Cameras Position in Relation to Tanks Z axis

		// Due to the slow rotation of the camera in relation to the tank, we end up with the camera not being exactly behind the tank
		// when the tank is making large turns. I solve this by every frame calculating the distance from the cameraa position to the tanks
		// Zaxis using a dot product. If the distance increases the cmaera moves on its x axis to match up with the tank again

		// Calculate the distance
		CVector3 tPos( m_Target->Matrix().Position().x, 0.0f, m_Target->Matrix().Position().z );
		CVector3 mPos( Matrix().Position().x, 0.0f, Matrix().Position().z );
		CVector3 v( tPos, mPos );
        float distanceZ = Dot( Normalise( m_Target->Matrix().XAxis() ), v );

		if( fabs(distanceZ) > 0.2f )
		{
			Matrix().MoveLocalX( -distanceZ*2*updateTime );
		}

		lastFrameSpeed = m_Target->Speed();
	}
	else
	{	
		RotateAroundPoint( m_Target->Matrix().Position(), -m_Target->TankTemplate()->TurnSpeed()*updateTime, true );
	}

	CalculateFrustrumPlanes();
}

void CChaseCamera::RotateAroundPoint( const CVector3& point, float RotateSpeed, bool FacePoint )
{
	// First, calculate the the offset from the target point position to the world origin
	double dX = -( CVector3::kOrigin.x - point.x );
	double dZ = -( CVector3::kOrigin.z - point.z );

	// Now move the matrix we want to rotate to the world origin but offset using previous values
	CVector3& mPos = Matrix().Position();
	Matrix().SetPosition( CVector3( mPos.x - dX, mPos.y, mPos.z - dZ ) );

	// Rotate matrix around world origin
	Matrix().RotateWorldY( RotateSpeed );

	// Now move back to original position using offsets
	Matrix().SetPosition( CVector3( mPos.x + dX, mPos.y, mPos.z + dZ ) );

	if( FacePoint )
		Matrix().FaceTarget( point );
}

} // namespace malaz