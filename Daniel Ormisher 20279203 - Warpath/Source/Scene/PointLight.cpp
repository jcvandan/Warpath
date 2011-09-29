#include "PointLight.h"

namespace malaz{

CPointLight::CPointLight(D3DXVECTOR3 pos, D3DXCOLOR colour, double brightness) 
						: m_Pos(pos), m_Colour(colour), m_Brightness(brightness)
{
}

} // namespace malaz{