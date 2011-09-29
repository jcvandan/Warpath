#pragma once
#include "TankDefines.h"
#include "Entity.h"
#include "TankTemplate.h"
#include "Track.h"
#include "WeaponPS.h"
#include "GUI.h"
#include "SoundManager.h"

namespace malaz{

// The main state of the tank entity
enum eTankState
{
	Normal,
	Starting,
	OffTrack,
	ExplosionDamage,
	Dead,
	Restarting,
	Won,
	Lost
};

struct sTankSubState
{
	sTankSubState()
	{
		TakingDamage = Boosting = Collided = false;
		DamageTimer = BoostTimer = CollisionTimer = 0.0f;
	}

	bool TakingDamage;
	float DamageTimer;

	bool Boosting;
	float BoostTimer;

	bool Collided;
	float CollisionTimer;
};

class CTankEntity : public CEntity
{
	friend class CTankEntityPlayer;
	friend class CTankEntityAI;
public:
	// Constructor
	CTankEntity( CTankTemplate* Template, string Name,
			     CTrack* Track,
				 vector<CEntity*>* TankVector,
				 CMessenger* Messenger,
				 CGUI* GUI,
			     CVector3 pos = CVector3( 0.0f, 0.0f, 0.0f ), 
			     CVector3 rot = CVector3( 0.0f, 0.0f, 0.0f ),  
			     CVector3 scale = CVector3( 1.0f, 1.0f, 1.0f ) );

	// General getters for instance data
	const int& Health()
	{
		return m_Health;
	}

	const float& Speed()
	{
		return m_Speed;
	}

	const bool& OnTrack()
	{
		return m_State != OffTrack;			
	}

	CTankTemplate* TankTemplate()
    {
        return m_TankTemplate;
    }

	// Main Update method 
	virtual bool Update( float updateTime, CMessenger* Messenger, CD3DManager* d3dManager ) = 0;

protected:

	// Check for a win
	void CheckWin();

	// Method to set state to lost
	void SetLostState(){ m_State = Lost; }

	// Message processing method
	virtual void ProcessMessages( CMessenger* Messenger );

	/////////////////////////////////////////////////////
	// State based behaviour methods

	// Explosion method and data
	void ExplosionBehaviour( float updateTime );

	void OffTrackBehaviour( float updateTime, CMessenger* Messenger );
	
	void DeathBehaviour( float updateTime, CMessenger* Messenger );

	void RestartingBehaviour( float updateTime, CMessenger* Messenger );

	void StartingBehaviour( float updateTime );

	CVector3 m_ExplosionVector;
	float m_ExplosionHeight;

	// Sub state methods
	void TakingDamage( float updateTime );
	void BoostBehaviour( float updateTime );

	// The main state and the sub state of the tank
	eTankState m_State;
	sTankSubState m_SubState;

	/////////////////////////////////////////////////////
	// TLane Checking

	int CheckLane();
	// The lane the tank is currently in
	int m_Lane;

	/////////////////////////////////////////////////////
	// Tank Collision methods

	// Check for collisions with all tanks
	void TankCollision( float updateTime, CMessenger* Messenger );

	void CollisionBehaviour( float updateTime );

	vector<CEntity*>* m_TankList;

	CVector3 m_CollisionVector;
	float m_CollisionMovement;


	/////////////////////////////////////////////////////
	// Power Up functions / Data

	// Functions
	bool SetPowerUp( ePowerUp type, int Bullets, CMessenger* Messenger );
	void MachineGunShoot( float updateTime, CMessenger* Messenger );
	void MortarShoot( float updateTime, CMessenger* Messenger );
	void MortarTurretAction( float updateTime );

	void TurretAction( float updateTime );

	// Power up data
	ePowerUp m_PowerUp;

	/////////////////////////////////////////////////////
	// Track Collision

	// Test tank is on track
	bool TrackTesting();

	// Check the distance to the next piece of track
	void DistanceToNextTrack();

	// Track data
    TrackIter m_TrackPiece;
	CTrack* m_Track;
	
	/////////////////////////////////////////////////////
	// Turning methods

	double TurnTowardsTarget( TFloat32 updateTime,	  // Update time					
							  double turnSpeed,		  // Turn speed
							  CVector3 targetPos,	  // Target position
							  CMatrix4x4& matrix,	  // Root matrix
							  CMatrix4x4& relMatrix );// Sub mesh matrix(if not manipulating a sub-mesh simply

	
	/////////////////////////////////////////////////////
	// General Instance Data

	//Current health and speed
	int m_Health;
	float m_Speed;

	// Store the template variables so they can be changed via power ups etc
	float m_TurnSpeed;
	float m_MaxSpeed;

	// Timer - used for all higher level state timing
	float m_Timer;

	// Pointer to tank template
	CTankTemplate* m_TankTemplate;

	// Data
	float m_TurretAngle;
	int m_NumLaps;

	// Vector to hold the waypoints for the tanks direction	
	vector<CVector3>::const_iterator m_WaypointIter;
	const vector<CVector3>* m_Waypoints;

	// Pointer to the GUI
	CGUI* m_GUI;

	// source number for sounds
	int m_SourceNum;
	int m_SourceNum2;

	float m_EnginePitch;
};

} //namespace malaz