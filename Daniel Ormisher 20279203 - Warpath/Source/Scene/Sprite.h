#pragma once
#include "TankDefines.h"
#include "CVector2.h"
#include "CMatrix4x4.h"
using namespace gen;

namespace malaz{

class CGUISprite
{
public:
	//Constructor / Destructor
	CGUISprite(){}
	CGUISprite( const string& Filename,
			   LPDIRECT3DDEVICE9 d3dDevice,
			   CVector2 Position = CVector2( 0.0f, 0.0f ),
			   float Rotation = 0.0f,
			   CVector2 Scale = CVector2( 1.0f, 1.0f ) );

	~CGUISprite();

	void Load( const string& Filename,
			   LPDIRECT3DDEVICE9 d3dDevice,
			   CVector2 Position = CVector2( 0.0f, 0.0f ),
			   float Rotation = 0.0f,
			   CVector2 Scale = CVector2( 1.0f, 1.0f ) );

	LPDIRECT3DTEXTURE9 Texture()
	{
		return m_Texture;
	}

	const CVector2& Position()
	{
		return m_Position;
	}

	D3DXMATRIX* Matrix()
	{
		return m_Matrix;
	}

private:
	//////////////////////////////////////
	// DATA

	//The teture to map to the sprite
	LPDIRECT3DTEXTURE9 m_Texture;
	
	// The matrix to transform the sprite
    D3DXMATRIX* m_Matrix;

	// Position
	CVector2 m_Position;
};

} // namespace malaz