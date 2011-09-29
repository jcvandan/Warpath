#pragma once
#include "TankDefines.h"
#include "Renderer.h"
#include "Camera.h"
#include "Messenger.h"
#include "SoundManager.h"
#include <deque>
#include <vector>
using namespace std;

namespace malaz{

////////////////////////////////////////////////////////////////////
//  SParticle
//
//  This structure matches the vertex format used for rendering of
//  the pointlist for each particle system
////////////////////////////////////////////////////////////////////

struct SParticle
{
	D3DXVECTOR3 m_Position;	
	float m_Size;
	float m_Alpha;
};


////////////////////////////////////////////////////////////////////
//  SParticleUpdate
//
//  Structure is the particle values that are updated on the CPU
//  before being copied to the vertex buffer to be passed to the GPU
////////////////////////////////////////////////////////////////////

struct SParticleUpdate
{
	SParticleUpdate(){ m_Alpha=0.0f; }

	CMatrix4x4	m_Matrix;
	D3DXVECTOR3	m_Speed;
	float		m_Size;
	float		m_Alpha;
	float		m_Life;
};


////////////////////////////////////////////////////////////////////
//  CParticleSystem
//
//  Base particle system class. Class is abstract and contains
//  implementation of methods for drawing and emitting particles.
//  Emit method is virtual, update function is abstract.
////////////////////////////////////////////////////////////////////

class CParticleSystem
{
public:
	// Constructor
	CParticleSystem( int MaxParticles,				// The maximum number of particles allowed to be active in the system
					 LPDIRECT3DTEXTURE9 Texture,	// The texture to be mapped to the point sprites
					 float Frequency,				// The frequency of particles being released
					 float ParticleLife,			// How long the particles shold live
					 float ParticleSize,			// The initial size of the particles
					 CMatrix4x4* Emitter,			// Tne position of the emitter
					 CVector3 EmitterRange,			// The range around the emitter (if we want a box emitting field)					 
					 LPDIRECT3DDEVICE9 d3dDevice,
					 CSoundManager* SoundManager = NULL );		

	// Destructor
	virtual ~CParticleSystem();

	// Update the system
	virtual bool Update( float updateTime, CMessenger* Messenger ) = 0;

	// Render the particle system
	void Render( int ViewportHeight, CCamera* cam, LPDIRECT3DDEVICE9 d3dDevice );

	CSoundManager* SoundManager()
	{
		return m_SoundManager;
	}

protected:
	// Emit particle function
	virtual void EmitParticle();

////////////////////////////////////////////////////////
// Rendering data

	// Vertex buffer containing point sprites
	LPDIRECT3DVERTEXBUFFER9 m_VertexBuffer;
	// The texture to be mapped to the sprites
	LPDIRECT3DTEXTURE9 m_Texture;

////////////////////////////////////////////////////////
// Update data

	// Position of the emitter and range around the emitter
	CMatrix4x4* m_Emitter;
	CVector3	m_EmitterRange;

	// The frequency of emitting and the life of particles
	float m_Frequency;
	float m_ParticleLife;

	// Initial size of particles
	float m_ParticleSize;

	// Particle System timer
	float m_Time;

	// The maximum number of particles in the system
	int m_MaxParticles;
	int m_NumParticles;

	// Array of SParticle and SParticleUpdate structs
	SParticleUpdate*	m_ParticleUpdates;

	// Vectors for storing dead and alive particles
	typedef deque<SParticleUpdate*> AliveParticles;
	AliveParticles m_AliveParticles;
	
	typedef vector<SParticleUpdate*> DeadParticles;
	DeadParticles m_DeadParticles;	

	AliveParticles::iterator m_ParticlesIter;

	CSoundManager* m_SoundManager;
};

} // end namespace malaz