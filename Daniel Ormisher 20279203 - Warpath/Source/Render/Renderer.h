/*******************************************************
	Renderer.h

	Declaration of CRenderer class. This class
	contains an enumerated list of the render methods
	that can be used by models in the game. It also
	contains arrays of pointers to the vertex and pixel
	shader classes to be used in the rendering.
	
******************************************************/
#pragma once

#include "VertexShader.h"
#include "PixelShader.h"
#include "LightManager.h"
#include "TankDefines.h"
#include <vector>
#include "CMatrix4x4.h"
#include "CVector3.h"
using namespace gen;

namespace malaz{

//Enumerated list of the render methods available, this can be added to and changed
//without affecting any of the shader manager functions
enum ERenderMethod
{
	PixelLighting,
	PixelLightingSpec,
	NormalMapping,
	VertexLighting,
	Texture,
	BrightenedTexture,
	Black,
	NumRenderMethods
};



//A structure that contains information needed for rendering, anything that needs to be rendered in the
//game must hold this structure somewhere, it contains the render method and the vertex/pixel shaders needed
//to render
struct CRenderMethod
{
	//Properties
	CVertexShader* m_vs;
	CPixelShader* m_ps;
	ERenderMethod m_Method;

	//Constructors
	CRenderMethod();
	CRenderMethod( CVertexShader* VS, CPixelShader* PS, ERenderMethod method )
		: m_vs( VS ), m_ps( PS ), m_Method( method ) {}

	//Destructor
	~CRenderMethod()
	{
	}
};

class CRenderer
{
private:
	//Constructor / Destructor
	CRenderer( LPDIRECT3DDEVICE9 d3dDevice );
	~CRenderer();

public:
	// Get instance of renderer function
	static CRenderer* GetInstance()
	{
		return m_Instance;
	}

	// Static create and destroy methods to create singleton instance of CRenderer
	static void Create( LPDIRECT3DDEVICE9 d3dDevice );
	static void Destroy();

//Sets the consts to be used in the const table for the shaders of the appropriate render method
	void LoadMethodConsts( CLightManager* lightMan, CMatrix4x4 &viewProjMat, 
						   CVector3 camPos, float SpecularPow );

//This function is called by an entities own render function to set its world matrix for the shaders
//it is going to use to render
	void SetMethodWorldMat(CMatrix4x4* WorldMat, ERenderMethod RenderMethod );

//Obtains 2 pointers to CVertexShader and CPixelShader classes for the method
	CRenderMethod* GetMethod( ERenderMethod RenderMethod );

private:
//Loads and compiles the shaders for the appropriate render method
	void LoadMethods();

// Class is a singleton so must have a static instance of this class
	static CRenderer* m_Instance;

//Define a vector list of CRenderMethod
	typedef vector<CRenderMethod*> TCRenderMethod;
	typedef TCRenderMethod::iterator TSRenderMethIter;

	TCRenderMethod m_Methods;
	// A vector to hold the shaders (simply for memory management)
	vector<CVertexShader*> VSVector;
	vector<CPixelShader*> PSVector;

	// Pointer to the d3dDevice
	LPDIRECT3DDEVICE9 m_d3dDevice;
};

} // namespace malaz{