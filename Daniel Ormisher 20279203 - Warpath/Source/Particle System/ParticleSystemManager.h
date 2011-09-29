//////////////////////////////////////////////////////////////////
//  ParticleSystemManager.h
//
//  Declaration of CParticleSystemManager class. Class looks after
//  creation/update/rendering of all active particle systems.
//  Class is a singleton.
//////////////////////////////////////////////////////////////////

#include "TankDefines.h"
#include "Messenger.h"
#include "ParticleSystem.h"
#include "TankBottomPS.h"
#include "TankFirePS.h"
#include "Camera.h"
#include "EntityManager.h"
#include "Track.h"
#include "SoundManager.h"
#include "Track.h"
#include <list>
#include <map>

using namespace gen;

namespace malaz{

class CParticleSystemManager
{
private:
	// Constructors and destructors - private as class is a singleton
	CParticleSystemManager(){}
	CParticleSystemManager( CD3DManager* d3dManager, CRenderer* manager, CTrack* Track,
							CMessenger* Messenger, CEntityManager* entityManager, CSoundManager* SoundManager );
	~CParticleSystemManager();

public:

///////////////////////////////////////
// Singleton Creation methods

	static CParticleSystemManager* GetInstance()
	{
		return m_Instance;
	}

	static void Create( CD3DManager* d3d3Manager, CRenderer* manager,  CTrack* Track,
						CMessenger* Messenger, CEntityManager* entityManager, CSoundManager* SoundManager );
	static void Destroy();

///////////////////////////////////////
// Particle System creation methods

	// Create a system for the bottom of the tanks
	void CreateTankBottomSystem( CMatrix4x4* TankMatrix, string TexName, string Owner );

	// Create a tank fire system
	void CreateTankFireSystem( CMatrix4x4* TankMatrix, string Owner, string TexName ); // The tank to be set on fire

	void CreateMachineGunSystem( CMatrix4x4* TankMatrix, string TexName, string Owner );
	void CreateCollisionSystem( CMatrix4x4* Matrix, string TexName, string Owner );

	void CreateMortar( CMatrix4x4* TankMatrix, string TexName, string Owner, TrackIter Iter );
	void CreateExplosion( string TexName, CVector3* Position );

///////////////////////////////////////
// Particle System texture methods

	void LoadTexture( string Texture, string Name );
	LPDIRECT3DTEXTURE9 GetTexture( string Name );

///////////////////////////////////////
// Particle System management methods

	void RenderSystems( CCamera* MainCamera, float updateTime );
	void UpdateSystems( float updateTime );
	bool DeleteSystem();

private:
	// Method destroys all active systems
	bool DestroyAllSystems();	

	// Particle Systems update and rendering
	void UpdateAllSystems( float updateTime );
	void RenderAllSystems( CCamera* MainCamera );

///////////////////////////////////////
// DATA
private:

	// static instance of the particle system manager
	static CParticleSystemManager* m_Instance;

	// The UID, or name, of the manager
	string m_UID;
		
////////////////////////////////////////////////////////
// Rendering data

	// Shaders
	CPixelShader* m_PS;
	CVertexShader* m_VS;

	// The vertex declaration
	IDirect3DVertexDeclaration9* m_VDecl;

	// Texture data
	multimap<string, LPDIRECT3DTEXTURE9> m_TextureMap;
	multimap<string, LPDIRECT3DTEXTURE9>::iterator m_TextureMapIter;
	typedef pair<string, LPDIRECT3DTEXTURE9> TexturePair;

////////////////////////////////////////////////////////
// System Management data

	// typedefs of lists for particle systems
	typedef vector<CParticleSystem*>		ParticleSystems;
	typedef ParticleSystems::iterator	ParticleSystemsIter;

	ParticleSystems		m_ParticleSystems;
	ParticleSystemsIter	m_ParticleSystemsIter;

	// typedefs of lists for weapons systems
	typedef multimap<string, CParticleSystem*> WeaponSystems;
	typedef multimap<string, CParticleSystem*>::iterator WeaponSystemsIter;
	typedef pair<string, CParticleSystem*> WeaponPair;

	WeaponSystems		m_WeaponSystems;
	WeaponSystemsIter	m_WeaponSystemsIter;

	// Vector containing list of tanks
	const vector<CEntity*>* m_TankList;

////////////////////////////////////////////////////////
// Pointers to other singleton managers in the engine

	//Pointer to the shader manager
	CRenderer* m_Renderer;
	//Pointer to d3ddevice
	CD3DManager* m_d3dManager;
	// Pointer to the messenger
	CMessenger* m_Messenger;
	// Entity Manager
	CEntityManager* m_entityManager;
	// Pointer to the sound manager
	CSoundManager* m_SoundManager;
	// Pointer to the track
	CTrack* m_Track;
};

}// end namespace malaz
