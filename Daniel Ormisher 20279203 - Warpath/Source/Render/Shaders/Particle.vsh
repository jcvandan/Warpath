//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

// World matrix and combined view / projection matrix for transformation calculations
float4x4 WorldMatrix    : WORLD;
float4x4 ViewProjMatrix : VIEWPROJECTION;
float3	 CamPos;
float	 ViewportHeight;

//-----------------------------------------------------------------------------
// Input / output structures
//-----------------------------------------------------------------------------

struct VS_Input
{
	float3	Position		: POSITION;  // The position of the vertex in model-space
	float	Size			: TEXCOORD0;
	float	Colour			: TEXCOORD1;
};

struct VS_Output
{
	float4	Position	: POSITION;
	float2	TexCoord0	: TEXCOORD0;
	float	Colour		: COLOR0;
	float	Size		: PSIZE;
};

//-----------------------------------------------------------------------------
// Main function
//-----------------------------------------------------------------------------

void main( in VS_Input i, out VS_Output o ) 
{
    float4 ModelPosition = float4(i.Position, 1.0f);
    float4 WorldPosition = mul( ModelPosition, WorldMatrix );
    o.Position = mul( WorldPosition, ViewProjMatrix );
    
    // Calculate the scaled size of the sprite
    float3 world = WorldPosition.xyz;
    float fDist = distance( world, CamPos );
	o.Size = ViewportHeight * i.Size * sqrt( 1/( fDist*fDist ) );
	o.Colour = i.Colour;
}