#include "Track.h"

namespace malaz{


//=================================================================================================================
// CTrack Implementation
//=================================================================================================================

// Make the pointer to the single instance of the manager NULL
CTrack* CTrack::m_Instance = NULL;

// Create method that is used instead of consturctor to create single instance of manager
void CTrack::Create()
{
	if( !m_Instance )
	{
		m_Instance = new CTrack();
	}
}

// Destroy method that is called to destroy the single instance of the manager
void CTrack::Destroy()
{
	delete m_Instance;
}


//=================================================================================================================
// Track Testing Functions
//=================================================================================================================

// This function tests where the tank is in relation to the track
bool CheckAgainstTrack( CMatrix4x4* Mat, TrackIter& tIter )
{
	//////////////////////////////////////////////////
    // TRACK

	/*********Calculate distance from track's origin**********************/

	float x = Mat->Position().x - (*tIter)->Matrix().Position().x;
	float y = Mat->Position().y - (*tIter)->Matrix().Position().y;
	float z = Mat->Position().z - (*tIter)->Matrix().Position().z;
	
	float dist = sqrt( x*x + y*y + z*z );

	// if the track piece is a corner
	if( (*tIter)->Template()->Name() == "Track Corner" )
    {
		// First calculate the distance from the corners z axis
        CVector3 v( (*tIter)->Matrix().Position(), Mat->Position() );
        float distanceZ = Dot( v, Normalise( (*tIter)->Matrix().ZAxis() ) );

		// Calculate the distance from the corners x axis
		v = CVector3( (*tIter)->Matrix().Position(), Mat->Position() );
        float distanceX = Dot( v, Normalise( (*tIter)->Matrix().XAxis() ) );

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
		if( distanceX > (*tIter)->BoundsZ() && distanceZ < (*tIter)->BoundsX() ||
			distanceZ > (*tIter)->BoundsZ() && distanceX < (*tIter)->BoundsX() )
		{
			TrackIncrement( Mat, tIter );	
		}
    }
	else
    {
		/*********Calculate distance from track's Z axis**********************/
		//Calculate a vector from the tank to the track's origin
        CVector3 v( (*tIter)->Matrix().Position(), Mat->Position() );

		// The dot product of v and the tracks x axis will give us the distance
        float distance = Dot( v, Normalise( (*tIter)->Matrix().XAxis() ) );

		// If the distance is 
		if( fabs(distance) > (*tIter)->BoundsX() )
        {
            return false;
        }

		/*********Calculate distance from track's X axis**********************/
        v = CVector3( (*tIter)->Matrix().Position(), Mat->Position() );

        distance = Dot( v, Normalise( (*tIter)->Matrix().ZAxis() ) );

		// If the absolute value of distance is greater than the max Z bounds, we must
		// change track piece
		if( fabs(distance) > (*tIter)->BoundsZ() )
		{
			TrackIncrement( Mat, tIter );
		}
    }

	return true;
}

void TrackIncrement( CMatrix4x4* Mat, TrackIter& tIter )
{
	CTrack* track = CTrack::GetInstance();

	// Calculate dist from next and previous pieces
	float x, z, distNext, distPrev;

	// If the current piece is not the last piece
	if( tIter != track->Back() )
	{
		x = Mat->Position().x - (*(tIter+1))->Matrix().Position().x;
		z = Mat->Position().z - (*(tIter+1))->Matrix().Position().z;				
		distNext = sqrt( x*x + z*z );
	}
	// If it is the last piece, the distance to the next piece must be the distance to the
	// first piece of track
	else
	{
		x = Mat->Position().x - (*track->Begin())->Matrix().Position().x;
		z = Mat->Position().z - (*track->Begin())->Matrix().Position().z;				
		distNext = sqrt( x*x + z*z );
	}

	// If the current piece is not the first piece
	if( tIter != track->Begin() )
	{
		x = Mat->Position().x - (*(tIter-1))->Matrix().Position().x;
		z = Mat->Position().z - (*(tIter-1))->Matrix().Position().z;				
		distPrev = sqrt( x*x + z*z );
	}
	// If it is the distance to the previous piece must be the dist to the last piece of the track
	else
	{
		x = Mat->Position().x - (*track->Begin())->Matrix().Position().x;
		z = Mat->Position().z - (*track->Begin())->Matrix().Position().z;				
		distPrev = sqrt( x*x + z*z );
	}

	// If distNext is smaller than distPrev, we increment
	if( distNext < distPrev )
	{
		if( tIter != track->Back() )
		{
			tIter++;
		}
		else
		{
			tIter = track->Begin();
		}
	}
	// If distPrev is smaller than distNext, we decrememt
	else// if( distPrev < distNext )
	{
		if( tIter != track->Begin() )
		{
			tIter--;
		}
		else
		{
			tIter = track->Back();
		}
	}		
}

} // end namespace malaz