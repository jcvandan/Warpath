#include "TankFirePS.h"
#include "MathDX.h"

namespace malaz{

CTankFirePS::CTankFirePS( CMatrix4x4* TankMatrix, LPDIRECT3DTEXTURE9 Texture, string Owner, LPDIRECT3DDEVICE9 d3dDevice )
						  : CParticleSystem( 20, Texture, 0.05f, 0.6f, 4.0f,
						  TankMatrix, CVector3( 0.8f, 0.0f, 2.0f ), d3dDevice ){ m_Owner = Owner; }

bool CTankFirePS::Update( float updateTime, CMessenger* Messenger )
{
	m_ParticlesIter = m_AliveParticles.begin();

	// Update particle data
	while( m_ParticlesIter != m_AliveParticles.end() )
	{
		// Do the update on each particle
		(*m_ParticlesIter)->m_Matrix.MoveY( 5.0f*updateTime );
		(*m_ParticlesIter)->m_Life -= updateTime;
		(*m_ParticlesIter)->m_Alpha = ( 1/m_ParticleLife )*(*m_ParticlesIter)->m_Life;

		if( (*m_ParticlesIter)->m_Life <= 0.0f )
		{
			m_DeadParticles.push_back( *m_ParticlesIter );
			m_ParticlesIter = m_AliveParticles.erase( m_ParticlesIter );
			continue;
		}
		
		if( m_ParticlesIter != m_AliveParticles.end() )
			++m_ParticlesIter;
	}

	m_Time += updateTime;
	return true;
}

void CTankFirePS::EmitParticle()
{
	// Emit a new particle
	if( m_Time >= m_Frequency )
	{
		// Only emit a particle if the number of particles counter is less than the maximum
		if( !m_DeadParticles.empty() )
		{
			// Initialise the particles matrix
			SParticleUpdate* p = m_DeadParticles.back();
			m_AliveParticles.push_front( p );

			p->m_Matrix = *m_Emitter;
			p->m_Matrix.MoveLocalY( 1.0f );
			p->m_Alpha = 1.0f;
			p->m_Life = m_ParticleLife;

			// Now randomise the position depending on the range of the emitter
			p->m_Matrix.MoveLocal( CVector3( Random( m_EmitterRange.x, -m_EmitterRange.x ),
											 Random( m_EmitterRange.y, -m_EmitterRange.y ),
											 Random( m_EmitterRange.z, -m_EmitterRange.z ) ) );

			m_DeadParticles.pop_back();
		}

		m_Time = 0.0f;
	}
}

} // end namespace malaz