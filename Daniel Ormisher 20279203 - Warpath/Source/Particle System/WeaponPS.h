//////////////////////////////////////////////////////////////////
//  WeaponPS.h
//
//  Contains declaration of all particle system weapon and related
//  classes. All classes inherit from CParticlSystem base class.
//////////////////////////////////////////////////////////////////

#pragma once
#include "TankDefines.h"
#include "ParticleSystem.h"
#include "Track.h"
using namespace std;

namespace malaz{

//////////////////////////////////////////////////////////////////
//  CMachineGunPS
//////////////////////////////////////////////////////////////////

class CMachineGunPS : public CParticleSystem
{
public:
	CMachineGunPS( CMatrix4x4* TankMatrix, 
				   LPDIRECT3DTEXTURE9 Texture, 
				   const vector<CEntity*>* Entities, 
				   string Owner, 
				   LPDIRECT3DDEVICE9 d3dDevice );

	// Update the system
	virtual bool Update( float updateTime, CMessenger* Messenger );

	// Shoot function
	void Shoot( float updateTime );

	// Emit particle function
	virtual void EmitParticle();

	string m_Owner;

private:
	// Vector containing list of tanks
	const vector<CEntity*>* m_TankList;
	vector<CEntity*>::const_iterator m_TankIter;
};


//////////////////////////////////////////////////////////////////
//  CCollisionPS
//////////////////////////////////////////////////////////////////

class CCollisionPS : public CParticleSystem
{
public:
	CCollisionPS( CMatrix4x4* Matrix, LPDIRECT3DTEXTURE9 Texture, LPDIRECT3DDEVICE9 d3dDevice, string Owner );

	// Update the system
	virtual bool Update( float updateTime, CMessenger* Messenger );

	void Emit( float updateTime );

protected:
	// Emit particle function
	virtual void EmitParticle();
	string m_Owner;
};


//////////////////////////////////////////////////////////////////
//  CMortarPS
//////////////////////////////////////////////////////////////////

class CMortarPS : public CParticleSystem
{
public:
	CMortarPS( CMatrix4x4* TankMatrix,
			   LPDIRECT3DTEXTURE9 Texture,
			   string Owner,
			   TrackIter& Iter,
			   CSoundManager* SoundManager,
			   LPDIRECT3DDEVICE9 d3dDevice );

	// Update the system
	virtual bool Update( float updateTime, CMessenger* Messenger );

	// Shoot function
	void Shoot( float updateTime );

	// Emit particle function
	virtual void EmitParticle();

	string m_Owner;

	TrackIter m_TrackPiece;

private:
	// The main mortar particle
	SParticleUpdate* m_Mortar;
	float m_MortarHeight;
	float m_MortarSpeed;

	CEntity* ent;
};


//////////////////////////////////////////////////////////////////
//  CMortarExplosionPS
//////////////////////////////////////////////////////////////////

class CMortarExplosionPS : public CParticleSystem
{
public:
	CMortarExplosionPS( CVector3* Pos, 
					    LPDIRECT3DTEXTURE9 Texture,
					    const vector<CEntity*>* Entities,
					    LPDIRECT3DDEVICE9 d3dDevice );

	virtual ~CMortarExplosionPS();

	// Update the system
	virtual bool Update( float updateTime, CMessenger* Messenger );

private:
	CVector3* m_Position;
};

} // end namespace malaz