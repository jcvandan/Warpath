/**************************************************
	LightTex.vsh

	Vertex Shader that displays a pixel and lighting
***************************************************/

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

// World matrix and combined view / projection matrix for transformation calculations
float4x4 WorldMatrix    : WORLD;
float4x4 ViewProjMatrix : VIEWPROJECTION;

// Current lighting information - ambient + two point lights (no attenuation)
float3 AmbientColour;
float3 Light1Pos; // Point light 1 - position
float3 Light1Colour;   // Point light 1 - colour
float3 Light2Pos; // Point light 2...
float3 Light2Colour;

// Shininess of material and camera position needed for specular calculation
float SpecularPower;
float3 CameraPosition;
float Wiggle;


//-----------------------------------------------------------------------------
// Input / output structures
//-----------------------------------------------------------------------------

// Input to vertex shader - usual position, normal and UVs
struct VS_Input
{
	float3 Position  : POSITION;  // The position of the vertex in model-space
	float3 Normal    : NORMAL;    // Vertex normal in model-space
	float2 TexCoord0 : TEXCOORD0; // Texture coordinate for the vertex
};

// Output from vertex shader. This shader outputs the diffuse & specular colours and texture
// coordinate for use in the pixel shader
struct VS_Output
{
	float4 Position       : POSITION;
	float3 DiffuseColour  : COLOR0;  // Diffuse colour calcuated at this vertex (ambient colour added in)
	float3 SpecularColour : COLOR1;  // Specular colour calcuated at this vertex 
	float2 TexCoord0      : TEXCOORD0;
};


//-----------------------------------------------------------------------------
// Main function
//-----------------------------------------------------------------------------

// Main vertex shader function. Transforms model-space position and normal to world space then
// calculate diffuse/specular light at the vertex - pass light colours and the texture coordinates
// to pixel shader
void main( in VS_Input i, out VS_Output o ) 
{
	//************************
	// Matrix transformations
	//************************

    // Convert model vertex position from (x,y,z) to (x,y,z,1) to prepare for matrix multiplication
    float4 ModelPosition = float4(i.Position, 1.0f);

    // Multiply model vertex position by the world matrix to get its 3D world position
    float4 WorldPosition = mul( ModelPosition, WorldMatrix );         

    // Multiply world position by combined view/projection matrix to get 2D viewport vertex
    o.Position = mul( WorldPosition, ViewProjMatrix );

	// Similar process to transform model normals to world space
    float4 ModelNormal = float4(i.Normal, 0.0f);
    float4 WorldNormal = mul( ModelNormal, WorldMatrix );

	// Lighting preparation

	float3 CameraDir = normalize( CameraPosition - WorldPosition );

	float3 TotalDiffuseColour = AmbientColour;
	float3 TotalSpecularColour = 0;

	// First light

	// Calculate diffuse lighting from the 1st light. Standard equation: Diffuse = max(0, N.L)
	float3 LightDir = normalize( Light1Pos - WorldPosition );
	float3 DiffuseColour = Light1Colour * max( 0, dot( WorldNormal, LightDir ) );
	TotalDiffuseColour += DiffuseColour;

	// Calculate specular lighting from the 1st light. Standard equation: Specular = max(0, (N.H)^p)
	// Slight tweak here: multiply by diffuse colour - a better effect for using pixel lighting
	float3 Halfway = normalize( CameraDir + LightDir );
	TotalSpecularColour += max( 0, pow( dot( WorldNormal, Halfway ), SpecularPower ) );

	// Second light
	
	LightDir = normalize( Light2Pos - WorldPosition );
	DiffuseColour = Light2Colour * max( 0, dot( WorldNormal, LightDir ) );
	TotalDiffuseColour += DiffuseColour;
	
	Halfway = normalize( CameraDir + LightDir );
	TotalSpecularColour += DiffuseColour * max( 0, pow( dot( WorldNormal, Halfway ), SpecularPower ) );	

	// Output data to Pixel Shader

	// Output calculated diffuse and specular colours - i.e. send to pixel shader
    o.DiffuseColour = TotalDiffuseColour;
    o.SpecularColour = TotalSpecularColour;

	// Send input texture coordinate directly to the pixel shader
    o.TexCoord0 = i.TexCoord0;
}
