#include "GameSetup.h"
#include "Input.h"
#include <fstream>

namespace malaz{

extern int MouseX;
extern int MouseY;

bool AddWaypointList( CCamera* Camera, int MouseX, int MouseY, CD3DManager* d3dManager,
					  vector<CVector3>* waypointList, float WaypointHeight )
{
	if( KeyHit( Key_Space ) )
	{
		// Vector we will place on the vector list
		CVector3 vector;

		// Get the world point from the mouse coords
		CVector3 worldPoint = Camera->WorldPtFromPixel( MouseX, MouseY, d3dManager->BackBufferWidth(), d3dManager->BackBufferHeight() );

		// Work out the ray to cast into the scene
		CVector3 ray = worldPoint - Camera->Matrix().Position();

		// Create a matrix at the position of the camera
		CMatrix4x4 point( Camera->Matrix().Position(), 
						  CVector3( 0.0f, 0.0f, 0.0f ), 
						  kZXY,
						  CVector3( 1.0f, 1.0f, 1.0f ) );

		CVector3* pos = &point.Position();

		// Make it face in the z direction of the camera
		point.FaceDirection( ray );

		// If the rays y is < 0 it is facing down
		if( ray.y < 0 )
		{
			// Move the matrix along its Z until it hits 0
			while( point.Position().y > 0 )
			{
				point.MoveLocalZ( 0.01f );
			}
		}
		else
		{
			return false;
		}

		// Now get the coords of the waypoint
		vector.x = point.Position().x;
		vector.y = WaypointHeight;
		vector.z = point.Position().z;

		waypointList->push_back( vector );
	}
	else if( KeyHit( Key_Back ) )
	{
		waypointList->pop_back();
	}
	else if( KeyHit( Key_Return ) )
	{		
		ofstream outFile;
		outFile.open("Waypoints.txt"); 
		
		if (outFile.is_open())
		{
			vector<CVector3>::iterator i;
			
			for( i = waypointList->begin(); i < waypointList->end(); i++ )
			{
				outFile << "<Waypoint Lane="" X=\"" << (*i).x << " \" Y=\"10.5\" " << "Z=\"" << (*i).z << "\" />" << endl;
			}
			outFile.close();
		}
	}

	return true;
}

}// namespace malaz