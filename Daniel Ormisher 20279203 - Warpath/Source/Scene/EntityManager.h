//////////////////////////////////////////////////////////////////
//  CEntityManager.h
//
//  Declaration of CEntityManager class. Class looks after creation/
//  update/rendering/deletion of entities.
//////////////////////////////////////////////////////////////////

#pragma once
#include "TankDefines.h"
#include "Renderer.h"
#include "TankEntityAI.h"
#include "TankEntityPlayer.h"
#include "TrackEntity.h"
#include "Camera.h"
#include "Messenger.h"
#include "PowerUpEntity.h"
#include "SoundManager.h"
#include "GUI.h"
#include <vector>
using namespace gen;

namespace malaz{

class CEntity;
class CEntityTemplate;

class CEntityManager
{
	friend class CParticleSystemManager;

private:
	// Private constructors and destructors
	CEntityManager(){}
	CEntityManager( CD3DManager* d3d3Manager, CRenderer* manager, CMessenger* Messenger, 
					CGUI* GUI, CSoundManager* SoundManager );
	~CEntityManager();

public:
	// Get the singletone instance of the class
	static CEntityManager* GetInstance()
	{
		return m_Instance;
	}

	// Singleton create and destroy methods
	static void Create( CD3DManager* d3d3Manager, CRenderer* manager, CMessenger* Messenger, 
						CGUI* GUI, CSoundManager* SoundManager  );
	static void Destroy();

	////////////////////////////////////////////////////////////////
	// Entity Creation Methods

	// Create a general entity
	CEntity* CreateEntity( string templateName,
						   string Name,
						   CVector3 pos = CVector3( 0.0f, 0.0f, 0.0f ),		//Initial positioning of Entity
						   CVector3 rot = CVector3( 0.0f, 0.0f, 0.0f ),		//Initial rotation
						   CVector3 scale = CVector3( 1.0f, 1.0f, 1.0f ) );	//Initial scale, these and 2 above will create matrix

	// Create an AI tank entity
	CTankEntityAI* CreateAITank( string templateName, 
							     string Name,
								 CTrack* track,
								 int Lane,
							     CVector3 pos = CVector3( 0.0f, 0.0f, 0.0f ),		//Initial positioning of Entity
							     CVector3 rot = CVector3( 0.0f, 0.0f, 0.0f ),		//Initial rotation
							     CVector3 scale = CVector3( 1.0f, 1.0f, 1.0f ) );	//Initial scale, these and 2 above will create matrix

	// Create a player tank entity
	CTankEntityPlayer* CreatePlayerTank( string templateName, 
									     string Name,
										 CTrack* track,
									     CVector3 pos = CVector3( 0.0f, 0.0f, 0.0f ),		//Initial positioning of Entity
									     CVector3 rot = CVector3( 0.0f, 0.0f, 0.0f ),		//Initial rotation
									     CVector3 scale = CVector3( 1.0f, 1.0f, 1.0f ) );	//Initial scale, these and 2 above will create matrix

	// Create a track entity
	CTrackEntity* CreateTrackEntity( string templateName,							 //Pointer to the template for the entity
									 string Name,									 //Name of entity
									 CVector2 bounds,								 //Max Bounds of the track
									 vector<CVector3>* Lanes,
									 CVector3 pos = CVector3( 0.0f, 0.0f, 0.0f ),	 //Initial positioning of Entity
									 CVector3 rot = CVector3( 0.0f, 0.0f, 0.0f ),	 //Initial rotation
									 CVector3 scale = CVector3( 1.0f, 1.0f, 1.0f ) );//Initial scale

	// Create a power up entity
	CPowerUpEntity* CreatePowerUpEntity( string templateName,
										 string Name,
										 CVector3 pos = CVector3( 0.0f, 0.0f, 0.0f ),
										 CVector3 rot = CVector3( 0.0f, 0.0f, 0.0f ),
										 CVector3 scale = CVector3( 1.0f, 1.0f, 1.0f ) );

	////////////////////////////////////////////////////////////////
	// Entity Template Creation Methods

	//Create basic entity and entity template
	CEntityTemplate* CreateEntityTemplate( string Name, string Type, string Folder, string Filename, ERenderMethod RenderMethod, string Texture = "" );

	//Create tank entity template and entity
	CTankTemplate* CreateTankTemplate( string Name,
									   string Type,
									   string Folder,
									   string Filename,
									   ERenderMethod RenderMethod,
									   string Texture = "",
									   float MaxSpeed = 10.0f, 
									   float Acceleration = 1.1f, 
									   float TurnSpeed = 0.5f,
									   int Weight = 100,
									   int MaxHealth = 100 );

	// Create template for a power up entity
	CPowerUpTemplate* CreatePowerUpTemplate( string Name,
										     string Type,
										     string Folder,
										     string Filename,
										     ERenderMethod RenderMethod,
										     ePowerUp PowerUpType,
											 int NumBullets,
										     const string& Texture = "" );

	////////////////////////////////////////////////////////////////////
	// EntityManager update and Entities update / rendering

	// EntityManager Update Method
	void Update( float updateTime );

	// Render and update all entities
	void RenderAllEntities( CCamera* Camera );
	void UpdateAllEntities( float UpdateTime );

	// Deletion methods
	void DeleteTemplate( string Name, string Type );
	void DeleteEntity( string Name, string Type );

	////////////////////////////////////////////////////////////////////
	// Get Entity and Template methods

	//Function to get pointer to a template
	CEntityTemplate* GetTemplate( string name );

	// GetEntity function
	CEntity* GetEntity( string Name );

	
private:
	// static instance of the entity manager
	static CEntityManager* m_Instance;

	// UID of class
	EntityNameUID m_UID;

	//Delete all entities / templates - called on destructor
	void DeleteAllEntities();
	void DeleteAllTemplates();

	//////////////////////////////////////////////////////////////////
	// Entity and Template data structures

	//Type defs for the vector and iterator for the templates
	typedef vector<CEntityTemplate*> EntityTemplates;
	typedef EntityTemplates::iterator EntityTemplatesIter;
	//Type defs for the vector and iterator for the entities
	typedef vector<CEntity*> Entities;
	typedef Entities::iterator EntitiesIter;

	//The lists to hold the templates and entites
	EntityTemplates m_EntityTemplates;
	Entities m_Entities;
	Entities m_TankEntities;

	//////////////////////////////////////////////////////////////////////
	// Pointers to other engine singleton classes

	//Pointer to the shader manager
	CRenderer* m_Renderer;
	//Pointer to d3ddevice
	CD3DManager* m_d3dManager;
	// Pointer to the messenger
	CMessenger* m_Messenger;	
	// Pointer to the GUI
	CGUI* m_GUI;
	// Pointer to the sound manager
	CSoundManager* m_SoundManager;
};

} // namespace malaz{