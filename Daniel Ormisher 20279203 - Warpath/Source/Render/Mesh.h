/*******************************************
	Model.h

	Model class declaration
********************************************/

#pragma once

#include <string>
#include <d3dx9.h>

#include "Defines.h"
#include "CVector3.h"
#include "CMatrix4x4.h"
#include "MeshData.h"
#include "Camera.h"
#include "Renderer.h"
using namespace gen;

namespace malaz
{

class CCamera;

// Mesh class
class CMesh
{
/*-----------------------------------------------------------------------------------------
	Constructors/Destructors
-----------------------------------------------------------------------------------------*/
public:
	// Constructor creates a mesh with no data
	CMesh();
	
	// Constructor that takes load parameters and loads a mesh
	CMesh( const string &Folder, const string &filename, LPDIRECT3DDEVICE9 d3dDevice, const string& Texture = "" );

	~CMesh();

private:
	// Disallow use of copy constructor and assignment operator (private and not defined)
	CMesh( const CMesh& );
	CMesh& operator=( const CMesh& );

public:


/*-----------------------------------------------------------------------------------------
	Public interface
-----------------------------------------------------------------------------------------*/
public:

	/////////////////////////////////////
	// Geometry access / enumeration

	// Get minimum and maximum bounds (axis-aligned)
	const CVector3& MinBounds()
	{
		return m_MinBounds;
	}
	const CVector3& MaxBounds()
	{
		return m_MaxBounds;
	}

	// Get radius of bounding sphere (from (0,0,0) in model space)
	TFloat32 BoundingRadius()
	{
		return m_BoundingRadius;
	}

	// Return total number of triangles in the mesh
	TUInt32 GetNumTriangles();

	// Request an enumeration of the triangles in the mesh. Get the individual triangles with
	// calls to GetTriangle, finish the enumeration with EndEnumTriangles
	void BeginEnumTriangles();

	// Get the next triangle in the mesh, used after BeginEnumTriangles. Fills the supplied
	// CVector3 pointers with the three vertex coordinates of the triangle. Returns true if a
	// triangle was successfully returned, false if there are no more triangles to enumerate
	bool GetTriangle( CVector3* pVertex1, CVector3* pVertex2, CVector3* pVertex3 );


	// Return total number of vertices in the mesh
	TUInt32 GetNumVertices();

	// Request an enumeration of the vertices in the mesh. Get the individual vertices with calls
	// to GetVertex, finish the enumeration with EndEnumVertices
	void BeginEnumVertices();

	// Get the next vertex in the mesh, used after BeginEnumVertices. Fills the supplied CVector3
	// pointer with the next vertex. Returns true if a vertex was successfully returned, false if
	// there are no more vertices to enumerate
	bool GetVertex( CVector3* pVertex );


	/////////////////////////////////////
	// Hierarchy access

	TUInt32 GetNumNodes()
	{
		return m_NumNodes;
	}

	const SMeshNode& GetNode( TUInt32 node )
	{
		return m_Nodes[node];
	}


	/////////////////////////////////////
	// Creation

	// Load the mesh from an X-File
	bool Load( const string &Folder, const string &filename, LPDIRECT3DDEVICE9 d3dDevice );


	/////////////////////////////////////
	// Rendering

	// Render the model using the given matrix list as a hierarchy (must be one matrix per node)
	// and from the given camera
	void Render( LPDIRECT3DDEVICE9 d3dDevice, CRenderMethod* method,
				 CRenderer* Renderer, CMatrix4x4* worldMat );

	// Function to add a texture to a mesh
	void AddTexture( string fileName, LPDIRECT3DDEVICE9 d3dDevice );
	// Function to change the primary texture
	void SetTexture( string fileName, LPDIRECT3DDEVICE9 d3dDevice ); 


/*-----------------------------------------------------------------------------------------
	Private interface
-----------------------------------------------------------------------------------------*/
private:
	
	/////////////////////////////////////
	// Types

	// The DirectX form of a sub-mesh. Stores controlling node and material used. The vertex/face
	// data stored in seperate vertex and index buffers for each mesh. This is sub-optimal - it
	// would be better to share buffers between meshes where possible, but this would make the code
	// much more complex
	struct SSubMeshDX
	{
		TUInt32                 node;     // Node controlling this sub-mesh 
		TUInt32                 material; // Index of material used by this sub-mesh

		// Vertex data for the sub-mesh stored in a vertex buffer and the number / size of
		// the vertices in the buffer
		LPDIRECT3DVERTEXBUFFER9 vertexBuffer;
		TUInt32                 numVertices;
		TUInt32                 vertexSize;

		// Index data for the sub-mesh stored in a index buffer and the number of
		// indices in the buffer, assuming 16-bit integer indices
		LPDIRECT3DINDEXBUFFER9  indexBuffer;
		TUInt32                 numIndices;
	};

	// DirectX form of a material - stores texture pointers instead of filenames
	struct SMeshMaterialDX
	{
		ERenderMethod      renderMethod;

		D3DXCOLOR          diffuseColour;
		D3DXCOLOR          specularColour;
		TFloat32           specularPower;

		TUInt32            numTextures;
		LPDIRECT3DTEXTURE9 textures[kiMaxTextures];
	};


	/////////////////////////////////////
	// Support functions

	// Release all nodes, sub-meshes and materials along with any DirectX data
	void ReleaseResources();

	// Creates a DirectX specific sub-mesh from an imported sub-mesh
	bool CreateSubMeshDX
	(
		const SSubMesh& subMesh,
		SSubMeshDX*     subMeshDX,
		LPDIRECT3DDEVICE9 d3dDevice
	);

	// Creates a DirectX specific material from an imported material
	bool CreateMaterialDX
	(
		const SMeshMaterial& material,
		SMeshMaterialDX*     materialDX,
		string Folder,
		LPDIRECT3DDEVICE9 d3dDevice
	);

	// Pre-processing after loading
	bool PreProcess();


	/*---------------------------------------------------------------------------------------------
		Data
	---------------------------------------------------------------------------------------------*/

	// Does this mesh have any geometry to render
	bool             m_HasGeometry;

	// Hierarchy for mesh - stored as a depth-first list of nodes, see SMeshNode defn in MeshData.h
	TUInt32          m_NumNodes;
	SMeshNode*       m_Nodes;        // Dynamically allocated array

	// Sub-meshes for mesh - each uses a single material
	TUInt32          m_NumSubMeshes;
	SSubMesh*        m_SubMeshes;    // Original sub-mesh data (dynamically allocated array)
	SSubMeshDX*      m_SubMeshesDX;  // DirectX sub-mesh data (vertex / index buffers)

	// Materials used in mesh
	TUInt32          m_NumMaterials;
	SMeshMaterialDX* m_Materials;    // Dynamically allocated array

	// Mesh bounding volume - minimum and maximum x,y & z values stored in two vectors
	CVector3         m_MinBounds;
	CVector3         m_MaxBounds;

	// Bounding sphere radius (from (0,0,0) in model space)
	TFloat32         m_BoundingRadius;

	// Data to support vertex / triangle enumeration
	TUInt32          m_EnumTriMesh;  // Current mesh being enumerated for triangles
	TUInt32          m_EnumTri;      // Current triangle (within above mesh) being enumerated
	TUInt32          m_EnumVertMesh; // Current mesh being enumerated for vertices
	TUInt32          m_EnumVert;     // Current vertices (within above mesh) being enumerated
};


} // namespace gen