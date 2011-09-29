#include "TankBottomPS.h"
#include "MathDX.h"

namespace malaz{

CTankBottomPS::CTankBottomPS( CMatrix4x4* TankMatrix,
							  LPDIRECT3DTEXTURE9 Texture,
						      string Owner,
						      LPDIRECT3DDEVICE9 d3dDevice )
							   : CParticleSystem( 100, Texture, 0.005f, 0.1f, 2.0f,
							     TankMatrix, CVector3( 1.0f, 0.0f, 2.0f ), d3dDevice ){}

bool CTankBottomPS::Update( float updateTime, CMessenger* Messenger )
{
	m_ParticlesIter = m_AliveParticles.begin();

	// Update particle data
	while( m_ParticlesIter != m_AliveParticles.end() )
	{
		// Do the update on each particle
		(*m_ParticlesIter)->m_Matrix.MoveLocalY( -5.0f*updateTime );
		(*m_ParticlesIter)->m_Life -= updateTime;
		(*m_ParticlesIter)->m_Alpha = ( 1/m_ParticleLife )*(*m_ParticlesIter)->m_Life;

		if( (*m_ParticlesIter)->m_Life <= 0.0f )
		{
			m_DeadParticles.push_back( *m_ParticlesIter );
			m_ParticlesIter = m_AliveParticles.erase( m_ParticlesIter );
			continue;
		}

		++m_ParticlesIter;
	}

	// Emit a new particle
	if( m_Time >= m_Frequency )
	{
		EmitParticle();
		m_Time = 0.0f;
	}

	m_Time += updateTime;
	return true;
	
	return true;
}

void CTankBottomPS::EmitParticle()
{
	for( int i=0; i<10; i++ )
	{
	// Only emit a particle if the number of particles counter is less than the maximum
	if( !m_DeadParticles.empty() )
	{
		SParticleUpdate* p = m_DeadParticles.back();

		p->m_Matrix = *m_Emitter;
		p->m_Alpha = 1.0f;
		p->m_Life = m_ParticleLife;
		//p->m_Matrix.MoveLocalY( -0.25f );

		p->m_Matrix.MoveLocal( CVector3( Random( m_EmitterRange.x, -m_EmitterRange.x ),
										 Random( m_EmitterRange.y, -m_EmitterRange.y ),
										 Random( m_EmitterRange.z, -m_EmitterRange.z ) ) );

		m_DeadParticles.pop_back();
		m_AliveParticles.push_front( p );
	}
	}
}

} // end namespace malaz