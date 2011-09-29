#pragma once
#include "TankDefines.h"
#include "TrackEntity.h"
#include <vector>

namespace malaz{

// Vector and iterator to hold track pieces
typedef vector<CTrackEntity*> TrackVector;
typedef TrackVector::iterator TrackIter;

typedef vector<CVector3> WaypointVector;
typedef TrackVector::iterator WaypointIter;

// Track testing functions
bool CheckAgainstTrack( CMatrix4x4* Mat, TrackIter& tIter );
void TrackIncrement( CMatrix4x4* Mat, TrackIter& tIter );

class CTrack
{
private:
	CTrack(){}
	~CTrack(){}

public:
	static void Create();
	static void Destroy();

	static CTrack* GetInstance()
	{
		return m_Instance;
	}	

    void Add( CTrackEntity* piece ){ m_Track.push_back( piece ); }

	TrackIter Begin(){ return m_Track.begin(); }
	TrackIter Back(){ return m_Track.end()-1; }

private:
	// Singleton instance
	static CTrack* m_Instance;

	// Vector containin pointers to the track pieces
    TrackVector m_Track;

	// Vectors containing lists of waypoints for the AI tanks to follow
	WaypointVector m_Waypoints[3];
};

} // end namespace malaz