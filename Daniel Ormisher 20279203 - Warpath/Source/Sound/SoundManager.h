#pragma once
//Include OpenAL headers
#include <al.h>      // Main OpenAL functions
#include <alc.h>     // OpenAL "context" functions (also part of main OpenAL API)
#include <AL/alut.h> // OpenAL utility library - helper functions

#include "TankDefines.h"
#include "CMatrix4x4.h"

#include <vector>

using namespace gen;

namespace malaz{

const int MAX_SOURCES = 30;

// enum type - must be updated to add sounds
enum Sounds
{
	Sounds_Music,
	Sounds_Engine,
	Sounds_Gun,
	Sounds_Mortar,
	Sounds_Explosion,
	Sounds_Max,
};

// holds filenames for sounds, must match enum list above
const string SoundFiles[] = 
{
	"Music.wav",
	"Engine.wav",
	"Gun.wav",
	"Mortar.wav",
	"Explosion.wav",
};

// enum type used for sources
//enum Sources
//{
//	Source_Music,
//	Source_PlayerTank,
//	Source

class CSoundManager
{
private:
	// Constructors and destructors - private as class is a singleton
	CSoundManager(){}
	CSoundManager( CMatrix4x4* Listener );
	~CSoundManager();

public:

///////////////////////////////////////
// Singleton Creation methods

	static CSoundManager* GetInstance()
	{
		return m_Instance;
	}

	static void Create( CMatrix4x4* Listener = NULL );
	static void Destroy();

	void Update( float updateTime );

	void Play( int Source, const CVector3& SourcePos, Sounds Sound, bool Loop = false,
			   float Pitch = 1.0f, float Gain = 10.0f );

	void UpdateSound( int Source, const CVector3& SourcePos, Sounds Sound, float Pitch = 1.0f, float Gain = 5.0f );

	void StopSound( int Source );

	void SetListener( CMatrix4x4* Listener )
	{
		m_Listener = Listener;
	}

private:
	static CSoundManager* m_Instance;

	// Buffers to hold loaded sounds
	ALuint m_Buffers[Sounds_Max];

	// Number of sources
	ALuint m_Sources[MAX_SOURCES];

	// The number of sources
	int m_NumSources;

	// Pointer to camera
	CMatrix4x4* m_Listener;
};

} // end namespace malaz