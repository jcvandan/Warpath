#pragma once
#include "TankDefines.h"
#include "Renderer.h"
#include "Mesh.h"
#include "SoundManager.h"
using namespace std;

namespace malaz{

class CMesh;

class CEntityTemplate
{
public:
	//Constructor / Destructor
	CEntityTemplate( string Name,					//Name of the template
					 string Type,					//Type of the template
					 string Folder,					//FOlder where the model is found
					 string Filename,				//Filename of the file to load for mesh
					 ERenderMethod RenderMethod,	//The render method the mesh will use
					 CRenderer* Renderer,			//Shader manager pointer to set render method
					 CSoundManager* SoundManager,
					 LPDIRECT3DDEVICE9 d3dDevice,	//The d3d Device	
					 const string& Texture = "" );

	virtual ~CEntityTemplate()
	{
		delete m_Mesh;
	}

//----------------------------------------------------------------------------
// Getters
//----------------------------------------------------------------------------

	CMesh* Mesh()
	{
		return m_Mesh;
	}

	string Name()
	{
		return m_Name;
	}

	string Type()
	{
		return m_Type;
	}

	CRenderMethod* RenderMethod()
	{
		return m_RenderMethod;
	}

	CSoundManager* SoundManager()
	{
		return m_SoundManager;
	}

protected:
	//Pointer to the mesh the template uses
	CMesh* m_Mesh;
	
	//The Render Method structure the mesh will use
	CRenderMethod* m_RenderMethod;

	//Strings to hold type and name
	string m_Name;
	string m_Type;

	//The specular power of the entity
	float m_SpecularPower;

	// Pointer to the sound manager
	CSoundManager* m_SoundManager;
};

}// namespace malaz