#pragma once
#include "Entity.h"

namespace malaz{

class CTrackEntity : public CEntity
{
public:
	//Constructor
	CTrackEntity( CEntityTemplate* Template,						//Pointer to the template for the entity
				  string Name,										//Name of entity				  
				  CVector2 bounds,									//Max Bounds of the track
				  vector<CVector3>* Lanes,
				  CVector3 pos = CVector3( 0.0f, 0.0f, 0.0f ),		//Initial positioning of Entity
				  CVector3 rot = CVector3( 0.0f, 0.0f, 0.0f ),		//Initial rotation
				  CVector3 scale = CVector3( 1.0f, 1.0f, 1.0f ) )	//Initial scale, these and 2 above will create matrix
				  : CEntity( Template, Name, pos, rot, scale )
	{
		if( Template->Name() == "Track Corner" )
		{
			m_Lanes = Lanes;
		}
		else
			m_Lanes = NULL;
		
		m_BoundsX = bounds.x;
		m_BoundsZ = bounds.y;		
	}

	~CTrackEntity()
	{
		delete [] m_Lanes;
	}

	// Getters for max boundary values
	const float& BoundsX(){ return m_BoundsX; }
	const float& BoundsZ(){ return m_BoundsZ; }

	const vector<CVector3>* Waypoints( int Lane )
	{
		return &m_Lanes[Lane];
	}

private:
	// Vector of waypoints (only used for corner pieces)
	vector<CVector3>* m_Lanes;

	// Properties
	float m_BoundsX;
	float m_BoundsZ;
};

}// namespace malaz