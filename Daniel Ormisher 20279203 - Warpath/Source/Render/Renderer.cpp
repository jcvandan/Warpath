/*******************************************************
	Renderer.cpp

	Implementation of the shader manager class
	
******************************************************/

#include "Renderer.h"
#include "Defines.h"
#include "MathDX.h"

namespace malaz{

const string ShaderFolder = "Source\\Render\\Shaders\\";

// Global Pointers to shaders
CVertexShader* vsTexture;
CVertexShader* vsVertexLighting;
CVertexShader* vsPixelLighting;
CVertexShader* vsNormalMapping;

CPixelShader* psTexture;
CPixelShader* psBrightenedTexture;
CPixelShader* psVertexLighting;
CPixelShader* psPixelLighting;
CPixelShader* psPixelLightingSpec;
CPixelShader* psNormalMapping;
CPixelShader* psBlack;

CRenderer* CRenderer::m_Instance = NULL;

void CRenderer::Create( LPDIRECT3DDEVICE9 d3dDevice )
{
	if( !m_Instance )
	{
		m_Instance = new CRenderer( d3dDevice );
	}
}

void CRenderer::Destroy()
{
	delete m_Instance;
}

CRenderer::CRenderer( LPDIRECT3DDEVICE9 d3dDevice )
{
	m_d3dDevice = d3dDevice;
	LoadMethods();
}

CRenderer::~CRenderer()
{
	//Need to iterate through the vector list here and delete the CRenderMethods
	TSRenderMethIter iter;
	for( iter = m_Methods.begin(); iter != m_Methods.end(); iter++ )
	{
		delete (*iter);
	}

	// Free up the memory from all the vertex shaders
	vector<CVertexShader*>::iterator VIter;
	for( VIter = VSVector.begin(); VIter != VSVector.end(); VIter++ )
	{
		delete (*VIter);
	}

	// Free up the memory from all the pixel shaders
	vector<CPixelShader*>::iterator PIter;
	for( PIter = PSVector.begin(); PIter != PSVector.end(); PIter++ )
	{
		delete (*PIter);
	}

	m_Methods.clear();
}

void CRenderer::LoadMethods()
{
	// Load Vertex Shaders
	// For each shader we must allocate memory, load the correct shader then push it onto the appropriate vector
	vsTexture = new CVertexShader();
	vsTexture->Load( ShaderFolder + "Texture.vsh", m_d3dDevice );
	VSVector.push_back( vsTexture );

	vsVertexLighting = new CVertexShader();
	vsVertexLighting->Load( ShaderFolder + "VertexLighting.vsh", m_d3dDevice );
	VSVector.push_back( vsVertexLighting );

	vsPixelLighting = new CVertexShader();
	vsPixelLighting->Load( ShaderFolder + "PixelLighting.vsh", m_d3dDevice );
	VSVector.push_back( vsPixelLighting );

	vsNormalMapping = new CVertexShader();
	vsNormalMapping->Load( ShaderFolder + "NormalMapping.vsh", m_d3dDevice );
	VSVector.push_back( vsNormalMapping );

	// Now do the same for pixel shaders
	psTexture = new CPixelShader();
	psTexture->Load( ShaderFolder + "Texture.psh", m_d3dDevice );
	PSVector.push_back( psTexture );

	psBrightenedTexture = new CPixelShader();
	psBrightenedTexture->Load( ShaderFolder + "BrightenedTexture.psh", m_d3dDevice );
	PSVector.push_back( psBrightenedTexture );

	psVertexLighting = new CPixelShader();
	psVertexLighting->Load( ShaderFolder + "VertexLighting.psh", m_d3dDevice );
	PSVector.push_back( psVertexLighting );
	
	psPixelLightingSpec = new CPixelShader();
	psPixelLightingSpec->Load( ShaderFolder + "PixelLightingSpecular.psh", m_d3dDevice );
	PSVector.push_back( psPixelLightingSpec );

	psPixelLighting = new CPixelShader();
	psPixelLighting->Load( ShaderFolder + "PixelLightingDiffuse.psh", m_d3dDevice );
	PSVector.push_back( psPixelLighting );

	psNormalMapping = new CPixelShader();
	psNormalMapping->Load( ShaderFolder + "NormalMapping.psh", m_d3dDevice );
	PSVector.push_back( psNormalMapping );

	psBlack = new CPixelShader();
	psBlack->Load( ShaderFolder + "Black.psh", m_d3dDevice );
	PSVector.push_back( psBlack );

// Load the methods used for rendering		
	CRenderMethod* method;

	// Load Pixel Lighting method
	method = new CRenderMethod( vsPixelLighting, psPixelLighting, PixelLighting );
	m_Methods.push_back( method );

	// Load Normal Mapping method with only diffuse
	method = new CRenderMethod( vsNormalMapping, psNormalMapping, NormalMapping );
	m_Methods.push_back( method );

	// Load Normal Mapping method with diffuse and specular
	method = new CRenderMethod( vsPixelLighting, psPixelLightingSpec, PixelLightingSpec );
	m_Methods.push_back( method );

	// Load Normal Mapping method
	method = new CRenderMethod( vsVertexLighting, psVertexLighting, VertexLighting );
	m_Methods.push_back( method );

	// Load Plain Texture method
	method = new CRenderMethod( vsTexture, psTexture, Texture );
	m_Methods.push_back( method );

	// Load Brightened Texture method
	method = new CRenderMethod( vsTexture, psBrightenedTexture, BrightenedTexture );
	m_Methods.push_back( method );

	// Load Black method
	method = new CRenderMethod( vsTexture, psBlack, Black );
	m_Methods.push_back( method );
}

//In the LoadShaderConsts method, the user specifies the method using its string name
//the appropriate consts are then set for the shaders
void CRenderer::LoadMethodConsts( CLightManager* lightMan, CMatrix4x4 &viewProjMat, 
								  CVector3 camPos, float SpecularPow )
{
	TSRenderMethIter iter;
	D3DXMATRIXA16 viewProj = ToD3DXMATRIX( viewProjMat );

// Load the constants for each render method
	
/*********** Vertex Shader for the Texture, Black and BrightenedTexture methods ****/
	vsTexture->m_ShaderConsts->SetMatrix( m_d3dDevice, "ViewProjMatrix", &viewProj );

/*********** Constants for the shaders used in PixelLighting method  ****************/
	vsPixelLighting->m_ShaderConsts->SetMatrix( m_d3dDevice, "ViewProjMatrix", &viewProj );
	psPixelLighting->m_ShaderConsts->SetFloatArray( m_d3dDevice, "AmbientColour",
															(float*)&lightMan->AmbientLight(), 3 );
	//Set up the consts for light 1
	psPixelLighting->m_ShaderConsts->SetFloatArray( m_d3dDevice, "Light1Pos",
															(float*)&lightMan->m_Lights[0].m_Pos, 3 );
	psPixelLighting->m_ShaderConsts->SetFloatArray( m_d3dDevice, "Light1Colour", 
															(float*)&lightMan->m_Lights[0].m_Colour, 4 );

	// Set up consts for pix lighting with specular highlights
	psPixelLightingSpec->m_ShaderConsts->SetFloatArray( m_d3dDevice, "Light1Pos", 
															(float*)&lightMan->m_Lights[0].m_Pos, 3 );
	psPixelLightingSpec->m_ShaderConsts->SetFloatArray( m_d3dDevice, "Light1Colour", 
															(float*)&lightMan->m_Lights[0].m_Colour, 4 );
	vsVertexLighting->m_ShaderConsts->SetFloatArray( m_d3dDevice, "CameraPosition", 
															(float*)&camPos, 3 );
	psPixelLightingSpec->m_ShaderConsts->SetFloat( m_d3dDevice, "SpecularPower", 500.0 );

/*********** Constants for the shaders used in VertexLighting method  ****************/
	vsVertexLighting->m_ShaderConsts->SetMatrix( m_d3dDevice, "ViewProjMatrix", &viewProj );
	vsVertexLighting->m_ShaderConsts->SetFloatArray( m_d3dDevice, "AmbientColour", 
															(float*)&lightMan->AmbientLight(), 3 );
	vsVertexLighting->m_ShaderConsts->SetFloatArray( m_d3dDevice, "CameraPosition", 
															(float*)&camPos, 3 );
	//light 1
	vsVertexLighting->m_ShaderConsts->SetFloatArray( m_d3dDevice, "Light1Pos", 
															(float*)&lightMan->m_Lights[0].m_Pos, 3 );
	vsVertexLighting->m_ShaderConsts->SetFloatArray( m_d3dDevice, "Light1Colour", 
															(float*)&lightMan->m_Lights[0].m_Colour, 4 );
	//Specular Power
	vsVertexLighting->m_ShaderConsts->SetFloat( m_d3dDevice, "SpecularPower", SpecularPow );

/*********** Constants for the shaders used in NormalMapping method  ****************/
	vsNormalMapping->m_ShaderConsts->SetMatrix( m_d3dDevice, "ViewProjMatrix", &viewProj );

	psNormalMapping->m_ShaderConsts->SetFloatArray( m_d3dDevice, "AmbientColour", 
															(float*)&lightMan->AmbientLight(), 3 );
	psNormalMapping->m_ShaderConsts->SetFloatArray( m_d3dDevice, "CameraPosition", 
															(float*)&camPos, 3 );
	//light 1
	psNormalMapping->m_ShaderConsts->SetFloatArray( m_d3dDevice, "Light1Pos", 
															(float*)&lightMan->m_Lights[0].m_Pos, 3 );
	psNormalMapping->m_ShaderConsts->SetFloatArray( m_d3dDevice, "Light1Colour", 
															(float*)&lightMan->m_Lights[0].m_Colour, 4 );
	psNormalMapping->m_ShaderConsts->SetFloatArray( m_d3dDevice, "Light1Brightness",
												    (float*)&lightMan->m_Lights[0].m_Brightness, 3 );
	psNormalMapping->m_ShaderConsts->SetFloat( m_d3dDevice, "SpecularPower", 256.0 );
}

CRenderMethod* CRenderer::GetMethod( ERenderMethod RenderMethod )
{
	//Need to iterate through the vector list and choose a method
	TSRenderMethIter iter;

	for( iter = m_Methods.begin(); iter != m_Methods.end(); iter++ )
	{
		if( (*iter)->m_Method == RenderMethod )
		{
			return (*iter);
		}
	}

	return NULL;
}

void CRenderer::SetMethodWorldMat( CMatrix4x4* WorldMat, ERenderMethod RenderMethod )
{
	TSRenderMethIter iter;

	D3DXMATRIXA16 worldMat = ToD3DXMATRIX( *WorldMat );

	for( iter = m_Methods.begin(); iter != m_Methods.end(); iter++ )
	{
		if( (*iter)->m_Method == RenderMethod )
		{
			(*iter)->m_vs->m_ShaderConsts->SetMatrix( m_d3dDevice, "WorldMatrix", &worldMat );
		}
	}
}

} // namespace malaz{
