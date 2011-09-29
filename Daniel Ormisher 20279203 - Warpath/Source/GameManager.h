#include "EntityManager.h"
#include "Messenger.h"
#include "ParticleSystemManager.h"
#include "GUI.h"
#include "LightManager.h"
#include "SoundManager.h"
using namespace gen;

namespace malaz{

enum eGameState
{
	FrontEnd,
	Game
};

class CGameManager
{
private:
	CGameManager();
	~CGameManager();

public:
	static void Create();
	static void Destroy();

	static CGameManager* GetInstance()
	{
		return m_Instance;
	}

	////////////////////////////////////////////////////////
	// Setup / Update / Render methods

	// Game setup, initiliase everything needed to begin
	bool Setup( CD3DManager* d3dManager );

	// Update method
	bool Update( CD3DManager* d3dManager, float updateTime );

	// Render method
	void Render( CD3DManager* d3dManager, float updateTime );

	void Shutdown();

private:
	// The singleton instance of this class
	static CGameManager* m_Instance;

	////////////////////////////////////////////////
	// Front End Methods

	bool FrontEndSetup( CD3DManager* d3dManager );
	bool FrontEndUpdate( CD3DManager* d3dManager, float updateTime );
	void FrontEndShutdown();

	////////////////////////////////////////////////
	// Main Game Methods

	bool GameSetup( CD3DManager* d3dManager );
	bool GameUpdate( CD3DManager* d3dManager, float updateTime );
	bool GamePaused( CD3DManager* d3dManager, float updateTime );
	void GameShutdown();

	////////////////////////////////////////////////
	// Pointers to main engine singletons

	// The entity manager
	CEntityManager* m_EntityManager;
	// The messenger
	CMessenger* m_Messenger;
	// The renderer
	CRenderer* m_Renderer;
	// The light manageer
	CLightManager* m_LightManager;
	// The GUI
	CGUI* m_GUI;
	// The particle system manager
	CParticleSystemManager* m_PSManager;
	// The sound manager
	CSoundManager* m_SoundManager;

	// The overall game state
	eGameState m_GameState;

	// Pause state
	bool m_Pause;
};

} // end namespace malaz