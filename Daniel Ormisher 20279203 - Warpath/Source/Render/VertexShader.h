#pragma once
#include "TankDefines.h"
using namespace std;

namespace malaz{

class CVertexShader
{
public:
	CVertexShader();
	~CVertexShader();

	void Load(const string& fileName, LPDIRECT3DDEVICE9 d3dDevice);

	//Shader and Shader consts table
	LPDIRECT3DVERTEXSHADER9 m_Shader;
	LPD3DXCONSTANTTABLE     m_ShaderConsts;
};

} // namespace malaz{