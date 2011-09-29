#include "PowerUpEntity.h"

namespace malaz{

int frames=0;

CPowerUpEntity::CPowerUpEntity( CPowerUpTemplate* Template, string Name,							   
								const vector<CEntity*>* Entities,
								CVector3 pos, CVector3 rot, CVector3 scale )
								: CEntity( Template, Name, pos, rot, scale )
{
	// Allocate memory for the vector of tanks
	m_TankList = Entities;

	// Copy the pointer to the power up template
	m_PowerUpTemplate = Template;

	// Set the initial state to Active
	m_State = Active;

	// Now iterate the entity managers entity vector and push any pointers to tanks
	// onto our own entity vector
	vector<CEntity*>::const_iterator i;

	// Initiliase numeric properties
	m_InactiveTimer = 0.0f;
	m_Speed = 0.0f;
	m_Bounce = 0.0f;
}

CPowerUpEntity::~CPowerUpEntity()
{
}

void CPowerUpEntity::ProcessMessages( CMessenger* Messenger )
{
	// Fetch any messages
	CMsg* msg = Messenger->FetchMessage( m_UID );
	while ( msg )
	{
		// If a picked up message is sent recieve the power up goes inactive
		if( msg->type == Msg_PickedUp )
		{
			m_State = Inactive;
			m_InactiveTimer = 0.0f;			
			delete msg;
		}

		msg = Messenger->FetchMessage( m_UID );
	}
}

bool CPowerUpEntity::Update( float updateTime, CMessenger* Messenger, CD3DManager* d3dManager )
{
	// Fetch any messages
	ProcessMessages( Messenger );

	// If the current power up state is active
	if( m_State == Active )
	{
		// Make the sphere bounce up and down slightly using a sin wave
		m_Speed = 15.0f * Sin( m_Bounce * 5.0f );
		m_Bounce += updateTime;

		Matrix().MoveLocalY( m_Speed*updateTime );

		// Now iterate through the list of tanks and check for a collision every couple of frames
		vector<CEntity*>::const_iterator i;

		for(i = m_TankList->begin(); i<m_TankList->end(); i++)
		{
			// Calculate the squared distance between the power up and tank and check if it is less than
			// the squared sum of the the two bounding radii
			CVector3 pos = (*i)->Matrix().Position() - Matrix().Position();
			float d1 = pos.x * pos.x + pos.y * pos.y + pos.z * pos.z;
			float d2 = (*i)->ScaledRadius() + ScaledRadius();

			if( d1 <= d2 * d2 )
			{
				if( (*i)->CheckSphereAgainstOOBB( Matrix().Position(), ScaledRadius() ) )
				{
					// If true then collision has occurred so send message to the tank telling it it has
					// picked up a power up
					CMsgPowerUp* msg = new CMsgPowerUp( Name(), Msg_PickUp, PowerUpTemplate()->PowerUpType(),
														PowerUpTemplate()->Bullets() );

					Messenger->SendMessageA( (*i)->Name(), msg );
				}
			}				
		}
	}// end if m_State==Active
	
	// If the current state is inactive
	else if( m_State == Inactive )
	{
		// If the power up has been Inactive for 5 seconds make it active again
		if( m_InactiveTimer >= 10.0f )
		{
			m_State = Active;
		}
		
		m_InactiveTimer += updateTime;
	}// end if state == Inactive

	frames++;

	return true;
}


void CPowerUpEntity::Render( LPDIRECT3DDEVICE9 d3dDevice, CRenderer* Renderer )
{
	if( m_State != Inactive )
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
}

} // namespace malaz