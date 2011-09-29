#pragma once
#include "TankDefines.h"
using namespace std;

namespace malaz{

class CPixelShader
{
public:
	CPixelShader();
	~CPixelShader();

	void Load(const string& fileName, LPDIRECT3DDEVICE9 d3dDevice);

	//Shader and Shader consts table
	LPDIRECT3DPIXELSHADER9	m_Shader;
	LPD3DXCONSTANTTABLE		m_ShaderConsts;
};

} // namespace malaz{