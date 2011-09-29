#include "Entity.h"
#include "Mesh.h"

namespace malaz{

//----------------------------------------------------------------------
// CEntity class implementation
//----------------------------------------------------------------------

CEntity::CEntity( CEntityTemplate* Template,				//Pointer to the template for the entity
				  string Name,								//Name of entity				 
				  CVector3 pos,   //( 0.0f, 0.0f, 0.0f )	//Initial positioning of Entity
				  CVector3 rot,   //( 0.0f, 0.0f, 0.0f )	//Initial rotation
				  CVector3 scale )//( 1.0f, 1.0f, 1.0f )	//Initial scale, these and 2 above will create matrix
{
	m_Template = Template;
	m_Name = Name;

	m_UID = Name;

	// Need to scale the meshes bounding radius depending on the scaling of the entity
	m_Scale = scale;
	float rScale = ( scale.x+scale.y+scale.z )/3.0f;
	m_ScaledBoundRadius = rScale * m_Template->Mesh()->BoundingRadius();

	m_ZAxisRadius = m_Template->Mesh()->MaxBounds().z * m_Scale.z;
	m_XAxisRadius = m_Template->Mesh()->MaxBounds().x * m_Scale.x;

	int numNodes = m_Template->Mesh()->GetNumNodes();
	m_WorldMatrices = new CMatrix4x4[numNodes];
	m_LocalMatrices = new CMatrix4x4[numNodes];
	
	for(int i=1; i<numNodes; ++i)
	{
		m_LocalMatrices[i] = m_Template->Mesh()->GetNode( i ).positionMatrix;
	}

	//Overwrite the root node with desired matrix
	m_LocalMatrices[0] = CMatrix4x4( pos, rot, kZXY, scale );
}				

CEntity::~CEntity()
{
	delete [] m_WorldMatrices;
	delete [] m_LocalMatrices;
}

void CEntity::Render( LPDIRECT3DDEVICE9 d3dDevice, CRenderer* Renderer )
{	
	// Calculate absolute matrices from relative node matrices & node heirarchy
	m_WorldMatrices[0] = m_LocalMatrices[0];
	TUInt32 numNodes = m_Template->Mesh()->GetNumNodes();

	for (TUInt32 node = 1; node < numNodes; ++node)
	{
		m_WorldMatrices[node] = m_LocalMatrices[node] * m_WorldMatrices[m_Template->Mesh()->GetNode( node ).parent];
	}

	m_Template->Mesh()->Render( d3dDevice, m_Template->RenderMethod(), Renderer, m_WorldMatrices );
}

/////////////////////////////////////////////////////////////////
// COLLISION DETECTION METHODS

bool CEntity::CheckPointInOOBB( const CVector3& Position )
{
	float distX, distZ;

	// First calculate the distance from the X axis
	CVector3 v( Matrix().Position(), Position );
    distX = abs( Dot( v, Normalise( Matrix().XAxis() ) ) );

	// Then calculate the distance from the Z axis
	v  = CVector3( Matrix().Position(), Position );
    distZ = abs( Dot( v, Normalise( Matrix().ZAxis() ) ) );

	if( distX <= m_XAxisRadius && distZ <= m_ZAxisRadius )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CEntity::CheckSphereAgainstOOBB( const CVector3& Point, const float& Radius )
{
	float distX, distZ;

	// First calculate the distance from the X axis
	CVector3 v( Matrix().Position(), Point );
    distX = abs( Dot( v, Normalise( Matrix().XAxis() ) ) );

	// Then calculate the distance from the Z axis
	v  = CVector3( Matrix().Position(), Point );
    distZ = abs( Dot( v, Normalise( Matrix().ZAxis() ) ) );

	if( distX <= ( m_XAxisRadius + Radius ) && distZ <= ( m_ZAxisRadius + Radius ) )
	{
		return true;
	}
	else
	{
		return false;
	}
}


bool CEntity::CheckOOBBtoOOBBIntersection( const float& ZAxisRadius, const float& XAxisRadius,
										   const CVector3& Position )
{
	// First calculate the distance from the X axis
	CVector3 v( Matrix().Position(), Position );
    float distX = abs( Dot( v, Normalise( Matrix().XAxis() ) ) );

	// Then calculate the distance from the Z axis
	v  = CVector3( Matrix().Position(), Position );
    float distZ = abs( Dot( v, Normalise( Matrix().ZAxis() ) ) );

	// Now check for an intersection
	if( distX <= ( (m_XAxisRadius*0.8) + (XAxisRadius*0.8) ) &&
		distZ <= ( (m_ZAxisRadius*0.8) + (ZAxisRadius*0.8) ) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

} // namespace malaz