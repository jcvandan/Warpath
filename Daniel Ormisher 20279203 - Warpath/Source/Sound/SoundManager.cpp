#include "SoundManager.h"

namespace malaz{

// The folder path for the sounds
const string filePath = "Source\\Sound\\";

CSoundManager* CSoundManager::m_Instance = NULL;

void CSoundManager::Create( CMatrix4x4* Listener )
{
	if( m_Instance == NULL )
	{
		m_Instance = new CSoundManager( Listener );
	}
}

void CSoundManager::Destroy()
{
	if( m_Instance != NULL )
		delete m_Instance;
}

CSoundManager::CSoundManager( CMatrix4x4* Listener )
{
	// Initialise OpenAL
	alutInit( 0, 0 );

	m_NumSources = 0;

	// Load the sounds needed
	for( int i=0; i<Sounds_Max; ++i )
	{
		string full = filePath+SoundFiles[i];
		m_Buffers[i] = alutCreateBufferFromFile( full.c_str() );
	}

	// Create the sources
	alGenSources( MAX_SOURCES, m_Sources );

	////////////////////////////////////////////////////////
	// Prepare default listener

	ALfloat listenerPos[3] = { 0.0, 0.0, 0.0 };
	ALfloat listenerOri[6] = { 0.0, 0.0, 1.0,
							   0.0, 1.0, 0.0 };

	alListenerfv( AL_POSITION,    listenerPos );
	alListenerfv( AL_ORIENTATION, listenerOri ); 
	alListenerf ( AL_GAIN,        1.0f );

}

CSoundManager::~CSoundManager()
{
	alutExit();
}

void CSoundManager::Play( int Source, const CVector3& SourcePos, Sounds Sound, bool Loop,
						  float Pitch, float Gain )
{
	ALenum state;
    
    alGetSourcei(Source, AL_SOURCE_STATE, &state);
    
    if( state != AL_PLAYING )
	{
		// Set the buffer to be played
		alSourcei ( m_Sources[Source], AL_BUFFER,   m_Buffers[Sound] );
		alSourcef ( m_Sources[Source], AL_PITCH,    Pitch );
		alSourcef ( m_Sources[Source], AL_GAIN,     Gain );

		// Set the source position
		ALfloat sourcePos[3] = { SourcePos.x, SourcePos.y, -SourcePos.z };
		alSourcefv( m_Sources[Source], AL_POSITION, sourcePos );

		// Loop or not loop
		alSourcei ( m_Sources[Source], AL_LOOPING,  Loop );

		// Play the sound
		alSourcePlay( m_Sources[Source] );
	}
}

void CSoundManager::UpdateSound( int Source, const CVector3& SourcePos, Sounds Sound, float Pitch, float Gain )
{
	// Set the buffer to be played
	alSourcei ( m_Sources[Source], AL_BUFFER,   m_Buffers[Sound] );
	alSourcef ( m_Sources[Source], AL_PITCH,    Pitch );
    alSourcef ( m_Sources[Source], AL_GAIN,     Gain );

	// Set the source position
	ALfloat sourcePos[3] = { SourcePos.x, SourcePos.y, -SourcePos.z };
    alSourcefv( m_Sources[Source], AL_POSITION, sourcePos );

	// Loop or not loop
	//alSourcei ( m_Sources[Source], AL_LOOPING,  Loop );
}

void CSoundManager::StopSound( int Source )
{
	alSourceStop( Source );
}

void CSoundManager::Update( float updateTime )
{
	ALfloat listenerPos[3] = { m_Listener->Position().x, m_Listener->Position().y, -m_Listener->Position().z };//{ m_Listener->Position().x, m_Listener->Position().y, -m_Listener->Position().z };
	
	m_Listener->ZAxis().Normalise();
	m_Listener->XAxis().Normalise();
	ALfloat listenerOri[6] = { m_Listener->ZAxis().x, m_Listener->ZAxis().y, -m_Listener->ZAxis().z,
							   m_Listener->YAxis().x, m_Listener->YAxis().y, -m_Listener->ZAxis().z };

	alListenerfv( AL_POSITION,    listenerPos );
	alListenerfv( AL_ORIENTATION, listenerOri );
	alListenerf ( AL_GAIN,        1.0f );
}

} // end namespace malaz

/*
bool isPlaying()
{
    ALenum state;
    
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    
    return (state == AL_PLAYING);
}
*/