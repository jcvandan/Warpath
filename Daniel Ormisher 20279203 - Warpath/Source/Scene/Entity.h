//////////////////////////////////////////////////////////////////
//  Entity.h
//
//  Declaration of CEntity class. Base class for Entity inheritance
//  hierarchy. Can be instantiated but has empty update method,
//  only useful for static entities.
//////////////////////////////////////////////////////////////////

#pragma once
#include "TankDefines.h"
#include "Renderer.h"
#include "CMatrix4x4.h"
#include "CVector3.h"
#include "CVector4.h"
#include "EntityTemplate.h"
#include "Messenger.h"
#include "D3DManager.h"
#include "Mesh.h"
using namespace gen;

namespace malaz{

class CEntityTemplate;
class CMessenger;

class CEntity
{
public:
	//Constructor / Destructor
	CEntity( CEntityTemplate* Template,							//Pointer to the template for the entity
			 string Name,										//Name of entity
			 CVector3 pos = CVector3( 0.0f, 0.0f, 0.0f ),		//Initial positioning of Entity
			 CVector3 rot = CVector3( 0.0f, 0.0f, 0.0f ),		//Initial rotation
			 CVector3 scale = CVector3( 1.0f, 1.0f, 1.0f ) );	//Initial scale, these and 2 above will create matrix

	~CEntity();

	//The entities own render function
	virtual void Render( LPDIRECT3DDEVICE9 d3dDevice, CRenderer* Renderer );

	//The entities own update function
	virtual bool Update( float updateTime, CMessenger* Messenger, CD3DManager* d3dManager ){ return true; }

	virtual void ProcessMessages( CMessenger* Messenger ){}

//----------------------------------------------------------------------------
// Getters
//----------------------------------------------------------------------------

	string Name()
	{
		return m_Name;
	}

	CMatrix4x4& Matrix( int node=0 )
	{
		return m_LocalMatrices[node];
	}

    CEntityTemplate* const Template()
    {
        return m_Template;
    }

	const float& ScaledRadius()
	{
		return m_ScaledBoundRadius;
	}

	const EntityNameUID UID()
	{
		return m_UID;
	}

	// Collision Detection methods
	bool CheckPointInOOBB( const CVector3& Position );

	bool CheckOOBBtoOOBBIntersection( const float& ZAxisRadius, const float& XAxisRadius,
									  const CVector3& Position );

	bool CheckSphereAgainstOOBB( const CVector3& Point, const float& Radius );

protected:
//----------------------------------------------------------------------------
//Properties
//----------------------------------------------------------------------------

	//Name
	string m_Name;

	//The world matrix of the entity
	CMatrix4x4* m_WorldMatrices;
	CMatrix4x4* m_LocalMatrices;

	//Pointer to the mesh class
	CEntityTemplate* m_Template;

	// Store a scaled bounding radius value
	float m_ScaledBoundRadius;

	// Unique ID
	EntityNameUID m_UID;

	// Scaling factor used for working out bounding volumes
	CVector3 m_Scale;

	// Collision Detection radii
	float m_ZAxisRadius;
	float m_XAxisRadius;
};


} // namespace malaz