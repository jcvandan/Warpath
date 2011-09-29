#include "TankDefines.h"
#include "CVector3.h"
#include "CMatrix4x4.h"
#include "Entity.h"

namespace malaz{

class CEntity;

////////////////////////////////////////
// Enumerated Message Types

enum eMessageType
{
	Msg_PickUp,
	Msg_PickedUp,
	Msg_CreateFireSystem,
	Msg_CreateMachineGunSystem,
	Msg_ShootMachineGun,
	Msg_CreateMortar,
	Msg_ShootMortar,
	Msg_CreateTankBottom,
	Msg_CreateCollision,
	Msg_EmitCollision,
	Msg_CreateExplosion,
	Msg_Explosion,
	Msg_Collision,
	Msg_CreateFire,
	Msg_EmitFire,
	Msg_WeaponRunOut,
	Msg_GetTankList,
	Msg_BackOnTrack,
	Msg_Winner
};

/////////////////////////////////////////////////////////
// CMsg - the message base class

class CMsg
{
public:
	CMsg(){};
	CMsg( EntityNameUID From, eMessageType Type )
	{
		from = From;
		type = Type;
	}

	eMessageType	type;
	EntityNameUID	from;
};


/////////////////////////////////////////////////////////
// Message class for collisions

class CMsgCollision : public CMsg
{
public:
	CMsgCollision( EntityNameUID From, eMessageType Type, CVector3 Position, float MoveAmount )
				   : CMsg( From, Type )
	{
		pos = Position;
		moveAmount = MoveAmount;
	}

	CVector3 pos;
	float moveAmount;
};


/////////////////////////////////////////////////////////
// Message class for retrieving the tank vector

class CMsgGetTankList : public CMsg
{
public:
	CMsgGetTankList( EntityNameUID From, eMessageType Type, const vector<CEntity*>* TankList )
					: CMsg( From, Type )
	{
		tankList = TankList;
	}

	const vector<CEntity*>* tankList;
};


/////////////////////////////////////////////////////////
// Message class for power ups

class CMsgPowerUp : public CMsg
{
public:
	CMsgPowerUp( EntityNameUID From, eMessageType Type, ePowerUp PowerUp, int Bullets ) 
				 : CMsg( From, Type )
	{
		powerUp = PowerUp;
	}

	ePowerUp		powerUp;
	int				numBullets;
	bool			recieved;
};


/////////////////////////////////////////////////////////
// Message class for all messages to do with collisions

class CMsgParticleSystem : public CMsg
{
public:
	CMsgParticleSystem( EntityNameUID From, eMessageType Type, string texture, CMatrix4x4* Matrix = NULL, CVector3* Pos = NULL ) 
						: CMsg( From, Type )
	{
		textureName = texture;
		matrix = Matrix;
		position = Pos;
	}

	string textureName;
	CMatrix4x4* matrix;
	CVector3*	position;
};

class CMsgMortar : public CMsgParticleSystem
{
public:
	CMsgMortar( EntityNameUID From, eMessageType Type, string texture, void* Iter, CMatrix4x4* Matrix = NULL, CVector3* Pos = NULL ) 
				: CMsgParticleSystem( From, Type, texture, Matrix, Pos )
	{
		iter = Iter;
	}

	void* iter;
};

} // end namespace malaz