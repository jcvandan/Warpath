#pragma once
#include "TankDefines.h"
#include "CVector3.h"
#include "Camera.h"
#include "D3DManager.h"

namespace malaz{

bool AddWaypointList( CCamera* Camera, int MouseX, int MouseY, CD3DManager* d3dManager,
					  vector<CVector3>* waypointList, float WaypointHeight = 10.5f );

}// namespace malaz