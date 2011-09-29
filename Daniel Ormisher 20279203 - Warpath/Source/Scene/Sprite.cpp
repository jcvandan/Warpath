#include "Sprite.h"
#include "MathDX.h"

namespace malaz{

CGUISprite::CGUISprite( const std::string& Filename,
						LPDIRECT3DDEVICE9 d3dDevice,
						CVector2 Position,
						float Rotation,
						CVector2 Scale )
{
	Load( Filename, d3dDevice, Position, Rotation, Scale );
}

CGUISprite::~CGUISprite()
{
	if( m_Texture != NULL )
		m_Texture->Release();

	delete m_Matrix;
}

void CGUISprite::Load( const string& Filename,
					   LPDIRECT3DDEVICE9 d3dDevice,
					   CVector2 Position,
					   float Rotation,
					   CVector2 Scale )
{
	// Create a texture to map to the sprite
	D3DXCreateTextureFromFile( d3dDevice, Filename.c_str(), &m_Texture );
	
	// Store copy of position
	m_Position = Position;

	// Convert the CVector2's to d3d types
	D3DXVECTOR2 pos = ToD3DXVECTOR( Position );
	D3DXVECTOR2 scale = ToD3DXVECTOR( Scale );

	m_Matrix = new D3DXMATRIX();

	D3DXMatrixTransformation2D( m_Matrix, NULL, 0.0f, &scale, &pos, Rotation, &pos );
}

} // end namespace malaz