#include "ParticleSystem.h"
#include "MathDX.h"

namespace malaz{

// CParticleSystem constructor
CParticleSystem::CParticleSystem( int MaxParticles,
								  LPDIRECT3DTEXTURE9 Texture,
								  float Frequency,
								  float ParticleLife,
								  float ParticleSize,
								  CMatrix4x4* Emitter,
								  CVector3 EmitterRange,								  
								  LPDIRECT3DDEVICE9 d3dDevice,
								  CSoundManager* SoundManager )
{
////////////////////////////////////////////////////////
// Initialise / Setup Update data

	// Allocate memory for the particles
	m_ParticleUpdates = new SParticleUpdate[MaxParticles];

	// Initialise the numerical partical data
	m_MaxParticles = MaxParticles;
	m_NumParticles = 0;
	m_ParticleSize = ParticleSize;
	m_ParticleLife = ParticleLife;
	m_Frequency = Frequency;
	m_Time = 0.0f;

////////////////////////////////////////////////////////
// Initialise / Setup Rendering data

	// Copy the texture	
	m_Texture = Texture;

	// Now load create the vertex buffer
	d3dDevice->CreateVertexBuffer( m_MaxParticles * sizeof(SParticle),
								   D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY | D3DUSAGE_POINTS,
								   D3DFVF_TEX1 | D3DFVF_PSIZE, D3DPOOL_DEFAULT,
								   &m_VertexBuffer, NULL );

	// Store the position and range of the emitter
	m_Emitter = Emitter;
	m_EmitterRange = EmitterRange;

	// Set the initial values of the particles
	for( int i = 0; i < MaxParticles; i++ )
	{
		// Do the update on each particle
		m_ParticleUpdates[i].m_Size = m_ParticleSize;
		m_ParticleUpdates[i].m_Alpha = 1.0f;
		m_ParticleUpdates[i].m_Life = m_ParticleLife;

		// Push all the particle structures onto the dead list
		m_DeadParticles.push_back( &m_ParticleUpdates[i] );
	}

	m_SoundManager = SoundManager;
}

CParticleSystem::~CParticleSystem()
{
	delete m_ParticleUpdates;

	if( m_VertexBuffer )
		m_VertexBuffer->Release();
}

void CParticleSystem::Render( int ViewportHeight, CCamera* cam, LPDIRECT3DDEVICE9 d3dDevice )
{
	// Unlock the vertex buffer to get a pointer to it then copy the values over needed
	SParticle *PointVertices;

	m_VertexBuffer->Lock( 0, m_MaxParticles * sizeof(SParticle),
						 (void**)&PointVertices, D3DLOCK_DISCARD );

	for( int i = 0; i < m_MaxParticles; ++i )
	{
		PointVertices[i].m_Position = ToD3DXVECTOR( m_ParticleUpdates[i].m_Matrix.Position() );//D3DXVECTOR3(0.0f,20.0f,90.0f);
		PointVertices[i].m_Alpha = m_ParticleUpdates[i].m_Alpha;
		PointVertices[i].m_Size = m_ParticleUpdates[i].m_Size;
	}

	m_VertexBuffer->Unlock();

	// Set the texture for the sprites
	d3dDevice->SetTexture( 0, m_Texture );

	d3dDevice->SetStreamSource( 0, m_VertexBuffer, 0, sizeof(SParticle) );
	HRESULT hr = d3dDevice->DrawPrimitive( D3DPT_POINTLIST, 0, m_MaxParticles );
}

void CParticleSystem::EmitParticle()
{
	// Only emit a particle if the number of particles counter is less than the maximum
	if( !m_DeadParticles.empty() )
	{
		SParticleUpdate* p = m_DeadParticles.back();

		p->m_Matrix = *m_Emitter;
		p->m_Alpha = 1.0f;
		p->m_Life = m_ParticleLife;

		p->m_Matrix.MoveLocal( CVector3( Random( m_EmitterRange.x, -m_EmitterRange.x ),
										 Random( m_EmitterRange.y, -m_EmitterRange.y ),
										 Random( m_EmitterRange.z, -m_EmitterRange.z ) ) );

		m_DeadParticles.pop_back();
		m_AliveParticles.push_front( p );
	}
}

} // namespace malaz