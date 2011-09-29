/**********************************************
	NormalMapping.vsh

	Vertex shader that transforms 3D vertices
	to viewport, and passes data for normal-
	mapping / parallax-mapping to the pixel
	shader. Also passes a set of texture
	coordinates (UVs)
***********************************************/

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

// World matrix and combined view / projection matrix for transformation calculations
float4x4 WorldMatrix    : WORLD;
float4x4 ViewProjMatrix : VIEWPROJECTION;

//-----------------------------------------------------------------------------
// Input / output structures
//-----------------------------------------------------------------------------

// Input to vertex shader - usual position, normal and UVs
struct VS_Input
{
	float3 Position  : POSITION;  // The position of the vertex in model-space
	float3 Normal    : NORMAL;    // Vertex normal in model-space
	float3 Tangent   : TANGENT;   // Vertex tangent in model-space
	float2 TexCoord0 : TEXCOORD0; // Texture coordinate for the vertex
};

// Output from vertex shader. This shader sends the world position, model normal and model 
// tangent to the pixel shader to do the lighing calculations. There are no semantics for such
// a use so we use TEXCOORD semantics, this is usual for any non-standard data types passed
// to a pixel shader
struct VS_Output
{
	float4 Position      : POSITION;
	float2 TexCoord0     : TEXCOORD0;
	float3 WorldPosition : TEXCOORD1; // Use TEXCOORD semantic for non-standard data - see above
	float3 ModelNormal   : TEXCOORD2;
	float3 ModelTangent  : TEXCOORD3;
};

//-----------------------------------------------------------------------------
// Main function
//-----------------------------------------------------------------------------

// Main vertex shader function. Transforms model-space position and normal to world space and
// passes on to the pixel shader for it to do per-pixel lighting calculations
void main( in VS_Input i, out VS_Output o ) 
{
     // Transform model vertex position to world space, then to viewport space. Pass both forms on
    // to the pixel shader
    float4 ModelPosition = float4(i.Position, 1.0f);
    float4 WorldPosition = mul( ModelPosition, WorldMatrix );         
    o.WorldPosition = WorldPosition;
    o.Position = mul( WorldPosition, ViewProjMatrix );

	//***| INFO |*****************************************************************************
	// No need to transform normals in the vertex shader, the pixel shader will calculate the
    // world normals now. Just pass model-space normal and tangent directly to pixel shader
	//****************************************************************************************
    o.ModelNormal = i.Normal;
    o.ModelTangent = i.Tangent;

	// Also pass input texture coordinate directly to the pixel shader
    o.TexCoord0 = i.TexCoord0;
}
