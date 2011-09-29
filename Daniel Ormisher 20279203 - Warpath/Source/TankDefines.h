/*******************************************
	Defines.h

	General definitions for the project
********************************************/

#pragma once // Prevent file being included more than once (would cause errors)

#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include "Input.h"
#include <math.h>

namespace malaz{

const int LANES = 4;

typedef std::string EntityNameUID;

enum ePowerUp
{
	MachineGun,
	Mortar,
	HomingMissile,
	SpeedBoost,
	Heal,
	Invincible,
	None
};

//-----------------------------------------------------------------------------
// Movement / rotation definitions
//-----------------------------------------------------------------------------

//Number of Shaders Allowed to be used

const float CORNER_WIDTH = 13.0f;
const float BLAST_RADIUS = 10.0f;

//Number of Lights in the Game
const int LIGHT_NUM = 4;

const std::string PS_MANAGER = "Particle System Manager";
const std::string ENTITY_MANAGER = "Entity Manager";
const std::string PLAYER_TANK = "Player";
const std::string AI_TANK1 = "AI-1";
const std::string AI_TANK2 = "AI-2";
const std::string AI_TANK3 = "AI-3";
const std::string CHASE_CAMERA = "Chase Camera";

const int WIN_LAPS = 2;

} // namespace malaz