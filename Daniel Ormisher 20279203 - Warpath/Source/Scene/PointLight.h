#pragma once
#include "TankDefines.h"
using namespace std;

namespace malaz{

class CPointLight
{
public:
	CPointLight(D3DXVECTOR3 pos, D3DXCOLOR colour, double brightness);

	D3DXVECTOR3 m_Pos;
	D3DXCOLOR m_Colour;
	double m_Brightness;
};

} // namespace malaz