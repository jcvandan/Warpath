#include "PixelShader.h"

namespace malaz{

CPixelShader::CPixelShader()
{
}

CPixelShader::~CPixelShader()
{
	if( m_Shader != NULL )
		m_Shader->Release();

	if( m_ShaderConsts != NULL )
		m_ShaderConsts->Release();
}

void CPixelShader::Load(const string& fileName, LPDIRECT3DDEVICE9 d3dDevice)
{
	//**********************************
	// Step 1: Compile shader program

	// Temporary variable to hold compiled pixel shader code
    LPD3DXBUFFER pShaderCode;

	// Check for shader support
	D3DCAPS9 d3dCaps;
	ZeroMemory(&d3dCaps, sizeof(D3DCAPS9)); // Can't hurt to zero the struct.

	HRESULT hRes = d3dDevice->GetDeviceCaps( &d3dCaps );

	if (d3dCaps.PixelShaderVersion < D3DPS_VERSION(2, 0)) 
	{
			MessageBox( NULL, "Your graphics card does not support Pixel Shader 2.0", "Pixel Shader Error", MB_OK );
	}

	// Compile external HLSL pixel shader into shader code to submit to the hardware
	D3DXCompileShaderFromFile( fileName.c_str(), // File containing pixel shader (HLSL)
							   NULL, NULL,       // Advanced compilation options - not needed here
							   "main",           // Name of main function in the shader
							   "ps_2_0",
							   //D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION,
							   0,
							   &pShaderCode,     // Ptr to variable to hold compiled shader code
							   NULL,             // Ptr to variable to hold error messages (not needed)
							   &m_ShaderConsts );// Ptr to variable to hold constants for the shader	


	//***************************************************
	// Step 2: Convert compiled program to usable shader

	// Create the pixel shader using the compiled shader code
    HRESULT hr=d3dDevice->CreatePixelShader( (DWORD*)pShaderCode->GetBufferPointer(), &m_Shader );

	// Discard the shader code now the shader has been created 
	pShaderCode->Release();
}

} // namespace malaz