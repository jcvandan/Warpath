//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

// World matrix and combined view / projection matrix for transformation calculations
float4x4 WorldMatrix    : WORLD;
float4x4 ViewProjMatrix : VIEWPROJECTION;

//-----------------------------------------------------------------------------
// Input / output structures
//-----------------------------------------------------------------------------

struct VS_Input
{
	float3 Position  : POSITION;  // The position of the vertex in model-space
	float2 TexCoord0 : TEXCOORD0; // Texture coordinate for the vertex
};

struct VS_Output
{
	float4 Position       : POSITION;
	float2 TexCoord0      : TEXCOORD0;
};

//-----------------------------------------------------------------------------
// Main function
//-----------------------------------------------------------------------------

void main( in VS_Input i, out VS_Output o ) 
{
	 // Convert model vertex position from (x,y,z) to (x,y,z,1) to prepare for matrix multiplication
    float4 ModelPosition = float4(i.Position, 1.0f);

    // Multiply model vertex position by the world matrix to get its 3D world position
    float4 WorldPosition = mul( ModelPosition, WorldMatrix );         

    // Multiply world position by combined view/projection matrix to get 2D viewport vertex
    o.Position = mul( WorldPosition, ViewProjMatrix );
    
	o.TexCoord0 = i.TexCoord0;
}
