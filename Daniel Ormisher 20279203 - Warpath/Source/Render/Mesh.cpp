/*******************************************
	Mesh.cpp

	Mesh class implementation
********************************************/

#include "Mesh.h"
#include "CImportXFile.h"

namespace malaz
{

// Folder for all texture and mesh files
static const string MediaFolder = "Media\\";

//-----------------------------------------------------------------------------
// Constructor / destructor
//-----------------------------------------------------------------------------

// Model constructor
CMesh::CMesh()
{
	// Initialise member variables
	m_HasGeometry = false;

	m_NumNodes = 0;
	m_Nodes = 0;

	m_NumSubMeshes = 0;
	m_SubMeshes = 0;
	m_SubMeshesDX = 0;

	m_NumMaterials = 0;
	m_Materials = 0;
}

CMesh::CMesh( const string &Folder, const string &filename, LPDIRECT3DDEVICE9 d3dDevice, const string& Texture )
{
	// Initialise member variables
	m_HasGeometry = false;

	m_NumNodes = 0;
	m_Nodes = 0;

	m_NumSubMeshes = 0;
	m_SubMeshes = 0;
	m_SubMeshesDX = 0;

	m_NumMaterials = 0;
	m_Materials = 0;

	Load( Folder, filename, d3dDevice );

	// If a texture has been specified
	if( Texture != "" )
	{
		string full = MediaFolder + Folder + "\\" + Texture;
		SetTexture( full, d3dDevice );
	}
}

CMesh::~CMesh()
{
	ReleaseResources();
}

// Release all nodes, sub-meshes and materials along with any DirectX data
void CMesh::ReleaseResources()
{
	for (TUInt32 material = 0; material < m_NumMaterials; ++material)
	{
		for (TUInt32 texture = 0; texture < m_Materials[material].numTextures; ++texture)
		{
			if (m_Materials[material].textures[texture])
			{
				m_Materials[material].textures[texture]->Release();
			}
		}
	}
	delete[] m_Materials;
	m_Materials = 0;
	m_NumMaterials = 0;

	for (TUInt32 subMesh = 0; subMesh < m_NumSubMeshes; ++subMesh)
	{
		if (m_SubMeshesDX[subMesh].indexBuffer)
		{
			m_SubMeshesDX[subMesh].indexBuffer->Release();
		}
		if (m_SubMeshesDX[subMesh].vertexBuffer)
		{
			m_SubMeshesDX[subMesh].vertexBuffer->Release();
		}
	}
	delete[] m_SubMeshesDX;
	delete[] m_SubMeshes;
	m_SubMeshesDX = 0;
	m_SubMeshes = 0;
	m_NumSubMeshes = 0;

	delete[] m_Nodes;
	m_Nodes = 0;
	m_NumNodes = 0;

	m_HasGeometry = false;
}


//-----------------------------------------------------------------------------
// Geometry access / enumeration
//-----------------------------------------------------------------------------

// Return total number of triangles in the mesh
TUInt32 CMesh::GetNumTriangles()
{
	TUInt32 numTriangles = 0;

	// Go through all submeshes ...
	for (TUInt32 subMesh = 0; subMesh < m_NumSubMeshes; ++subMesh)
	{
		numTriangles += m_SubMeshes[subMesh].numFaces;
	}

	return numTriangles;
}

// Request an enumeration of the triangles in the mesh. Get the individual triangles with
// calls to GetTriangle, finish the enumeration with EndEnumTriangles
void CMesh::BeginEnumTriangles()
{
	m_EnumTriMesh = 0;
	m_EnumTri = 0;
}

// Get the next triangle in the mesh, used after BeginEnumTriangles. Fills the supplied
// CVector3 pointers with the three vertex coordinates of the triangle. Returns true if a
// triangle was successfully returned, false if there are no more triangles to enumerate
bool CMesh::GetTriangle( CVector3* pVertex1, CVector3* pVertex2, CVector3* pVertex3 )
{
	// If enumerated all meshes then finished
	if (m_EnumTriMesh >= m_NumSubMeshes)
	{
		return false;
	}

	// If enumerated all triangles in current mesh...
	if (m_EnumTri >= m_SubMeshes[m_EnumTriMesh].numFaces)
	{
		// Move to next mesh - finished if no more meshes
		++m_EnumTriMesh;
		if (m_EnumTriMesh >= m_NumSubMeshes)
		{
			return false;
		}
		m_EnumTri = 0; // Start at first triangle of next mesh
	}

	// Get current face from submesh
	SMeshFace face = m_SubMeshes[m_EnumTriMesh].faces[m_EnumTri];

	// Get pointer to vertex refered to by first face index - deal with flexible vertex size
	TUInt8* pVertexData = m_SubMeshes[m_EnumTriMesh].vertices + 
	                      face.aiVertex[0] * m_SubMeshes[m_EnumTriMesh].vertexSize;

	// Copy vertex coordinate to output pointer
	// Assuming first three floats are the vertex coord x,y & z. See comment in CMesh::PreProcess
	TFloat32* pVertexCoord = reinterpret_cast<TFloat32*>(pVertexData);
	pVertex1->x = *pVertexCoord++;
	pVertex1->y = *pVertexCoord++;
	pVertex1->z = *pVertexCoord;

	// Get second vertex coordinate
	pVertexData = m_SubMeshes[m_EnumTriMesh].vertices +
	              face.aiVertex[1] * m_SubMeshes[m_EnumTriMesh].vertexSize;
	pVertexCoord = reinterpret_cast<TFloat32*>(pVertexData);
	pVertex2->x = *pVertexCoord++;
	pVertex2->y = *pVertexCoord++;
	pVertex2->z = *pVertexCoord;

	// Get third vertex coordinate
	pVertexData = m_SubMeshes[m_EnumTriMesh].vertices +
	              face.aiVertex[2] * m_SubMeshes[m_EnumTriMesh].vertexSize;
	pVertexCoord = reinterpret_cast<TFloat32*>(pVertexData);
	pVertex2->x = *pVertexCoord++;
	pVertex2->y = *pVertexCoord++;
	pVertex2->z = *pVertexCoord;

	return true;
}


// Return total number of vertices in the mesh
TUInt32 CMesh::GetNumVertices()
{
	TUInt32 numVertices = 0;

	// Go through all submeshes ...
	for (TUInt32 subMesh = 0; subMesh < m_NumSubMeshes; ++subMesh)
	{
		numVertices += m_SubMeshes[subMesh].numVertices;
	}

	return numVertices;
}

// Request an enumeration of the vertices in the mesh. Get the individual vertices with calls
// to GetVertex, finish the enumeration with EndEnumVertices
void CMesh::BeginEnumVertices()
{
	m_EnumVertMesh = 0;
	m_EnumVert = 0;
}

// Get the next vertex in the mesh, used after BeginEnumVertices. Fills the supplied CVector3
// pointer with the next vertex. Returns true if a vertex was successfully returned, false if
// there are no more vertices to enumerate
bool CMesh::GetVertex( CVector3* pVertex )
{
	// If enumerated all meshes then finished
	if (m_EnumVertMesh >= m_NumSubMeshes)
	{
		return false;
	}

	// If enumerated all vertices in current mesh...
	if (m_EnumVert >= m_SubMeshes[m_EnumVertMesh].numVertices)
	{
		// Move to next mesh - finished if no more meshes
		++m_EnumVertMesh;
		if (m_EnumVertMesh >= m_NumSubMeshes)
		{
			return false;
		}
		m_EnumVert = 0; // Start at first vertex of next mesh
	}

	// Get pointer to current vertex in current mesh - deal with flexible vertex size
	TUInt8* pVertexData = m_SubMeshes[m_EnumVertMesh].vertices + 
	                      m_EnumVert * m_SubMeshes[m_EnumVertMesh].vertexSize;

	// Copy vertex coordinate to output pointer
	// Assuming first three floats are the vertex coord x,y & z. See comment in CMesh::PreProcess
	TFloat32* pVertexCoord = reinterpret_cast<TFloat32*>(pVertexData);
	pVertex->x = *pVertexCoord++;
	pVertex->y = *pVertexCoord++;
	pVertex->z = *pVertexCoord;

	return true;
}


//-----------------------------------------------------------------------------
// Creation
//-----------------------------------------------------------------------------

// Create the model from an X-File, returns true on success
bool CMesh::Load( const string &Folder, const string &filename, LPDIRECT3DDEVICE9 d3dDevice )
{
	// Create a X-File import helper class
	CImportXFile importFile;

	// Add media folder path
	string fullFileName = MediaFolder + Folder + "\\" + filename;

	// Check that the given file is an X-file
	if (!importFile.IsXFile( fullFileName ))
	{
		return false;
	}

	// Import the file, return on failure
	EImportError error = importFile.ImportFile( fullFileName );
	if (error != kSuccess)
	{
		if (error == kFileError)
		{
			string errorMsg = "Error loading mesh " + fullFileName;
			SystemMessageBox( errorMsg.c_str(), "Mesh Error" );
		}
		return false;
	}

	// Release any existing geometry
	if (m_HasGeometry)
	{
		ReleaseResources();
	}

	// Get node data from import class
	m_NumNodes = importFile.GetNumNodes();
	m_Nodes = new SMeshNode[m_NumNodes];
	if (!m_Nodes)
	{
		return false;
	}
	for (TUInt32 node = 0; node < m_NumNodes; ++node)
	{
		importFile.GetNode( node, &m_Nodes[node] );
	}

	// Get submesh data from import class - convert to DirectX data for rendering
	// but retain original data for easy access to vertices / faces
	TUInt32 requiredSubMeshes = importFile.GetNumSubMeshes();
	m_SubMeshes = new SSubMesh[requiredSubMeshes];
	m_SubMeshesDX = new SSubMeshDX[requiredSubMeshes];
	if (!m_SubMeshes || !m_SubMeshesDX)
	{
		ReleaseResources();
		return false;
	}
	for (m_NumSubMeshes = 0; m_NumSubMeshes < requiredSubMeshes; ++m_NumSubMeshes)
	{
		// Determine if the render method for this mesh needs tangents
		//ERenderMethod meshMethod = importFile.GetSubMeshRenderMethod( m_NumSubMeshes );
		bool tangents = false;

		importFile.GetSubMesh( m_NumSubMeshes, &m_SubMeshes[m_NumSubMeshes] );
		if (!CreateSubMeshDX( m_SubMeshes[m_NumSubMeshes], &m_SubMeshesDX[m_NumSubMeshes], d3dDevice ))
		{
			ReleaseResources();
			return false;
		}
	}

	// Get material data from import class, also load textures
	TUInt32 requiredMaterials = importFile.GetNumMaterials();
	m_Materials = new SMeshMaterialDX[requiredMaterials];
	if (!m_Materials)
	{
		ReleaseResources();
		return false;
	}
	for (m_NumMaterials = 0; m_NumMaterials < requiredMaterials; ++m_NumMaterials)
	{
		SMeshMaterial importMaterial; 
		importFile.GetMaterial( m_NumMaterials, &importMaterial );
		if (!CreateMaterialDX( importMaterial, &m_Materials[m_NumMaterials], Folder, d3dDevice ))
		{
			ReleaseResources();
			return false;
		}
	}

	// Geometry pre-processing - just calculating bounding box in this example
	if (!PreProcess())
	{
		ReleaseResources();
		return false;
	}

	m_HasGeometry = true;
	return true;
}

// Creates a DirectX specific sub-mesh from an imported sub-mesh
bool CMesh::CreateSubMeshDX
(
	const SSubMesh& subMesh,
	SSubMeshDX*     subMeshDX,
	LPDIRECT3DDEVICE9 d3dDevice
)
{
	// Copy node and material
	subMeshDX->node = subMesh.node;
	subMeshDX->material = subMesh.material;

	// Create the vertex buffer
	unsigned int bufferSize = subMesh.numVertices * subMesh.vertexSize;
    if (FAILED(d3dDevice->CreateVertexBuffer( bufferSize, D3DUSAGE_WRITEONLY, 0,
                                                 D3DPOOL_MANAGED, &subMeshDX->vertexBuffer, NULL )))
    {
        return false;
    }
	subMeshDX->numVertices = subMesh.numVertices;
	subMeshDX->vertexSize = subMesh.vertexSize;

    // "Lock" the vertex buffer so we can write to it
    void* bufferData;
    if (FAILED(subMeshDX->vertexBuffer->Lock( 0, bufferSize, (void**)&bufferData, 0 )))
	{
        return false;
	}

	// Copy the vertex data
    memcpy( bufferData, subMesh.vertices, bufferSize );

	// Unlock the vertex buffer again so it can be used for rendering
    subMeshDX->vertexBuffer->Unlock();


    // Create the index buffer - assuming 16-bit (WORD) index data
	bufferSize = subMesh.numFaces * 3 * sizeof(WORD);
    if (FAILED(d3dDevice->CreateIndexBuffer( bufferSize, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,
                                                D3DPOOL_MANAGED, &subMeshDX->indexBuffer, NULL )))
    {
        return false;
    }
	subMeshDX->numIndices = subMesh.numFaces * 3;

    // "Lock" the index buffer so we can write to it
    if (FAILED(subMeshDX->indexBuffer->Lock( 0, bufferSize, (void**)&bufferData, 0 )))
	{
        return false;
	}

	// Copy the index data
    memcpy( bufferData, subMesh.faces, bufferSize );

	// Unlock the index buffer again so it can be used for rendering
    subMeshDX->indexBuffer->Unlock();

	return true;
}

// Creates a DirectX specific material from an imported material
bool CMesh::CreateMaterialDX
(
	const SMeshMaterial& material,
	SMeshMaterialDX*     materialDX,
	string Folder,
	LPDIRECT3DDEVICE9 d3dDevice
)
{
	// Copy colours and shininess from material
	materialDX->diffuseColour = D3DXCOLOR( material.diffuseColour.r, material.diffuseColour.g, 
	                                       material.diffuseColour.b, material.diffuseColour.a );
	materialDX->specularColour = D3DXCOLOR( material.specularColour.r, material.specularColour.g, 
	                                        material.specularColour.b, material.specularColour.a );
	materialDX->specularPower = material.specularPower;

	// Load material textures
	materialDX->numTextures = material.numTextures;
	for (TUInt32 texture = 0; texture < material.numTextures; ++texture)
	{
		string fullFileName = MediaFolder + Folder + "\\" + material.textureFileNames[texture];
		if (FAILED(D3DXCreateTextureFromFile( d3dDevice, fullFileName.c_str(),
											  &materialDX->textures[texture] )))
		{
			string errorMsg = "Error loading texture " + fullFileName;
			SystemMessageBox( errorMsg.c_str(), "CMesh Error" );
			return false;
		}
	}
	return true;
}


// Pre-processing after loading, returns true on success - just calculates bounding box here
// Rejects mesh if no sub-meshes or any empty sub-meshes
bool CMesh::PreProcess()
{
	// Ensure at least one non-empty sub-mesh
	if (m_NumSubMeshes == 0 || m_SubMeshes[0].numVertices == 0)
	{
		return false;
	}

	// Set initial bounds from first vertex
	// Assuming first three floats are the vertex coord x,y & z. Would be better to support
	// a flexible data type system like DirectX vertex declarations (D3DVERTEXELEMENT9)
	TFloat32* pVertexCoord = reinterpret_cast<TFloat32*>(m_SubMeshes[0].vertices);
	m_MinBounds.x = m_MaxBounds.x = *pVertexCoord++;
	m_MinBounds.y = m_MaxBounds.y = *pVertexCoord++;
	m_MinBounds.z = m_MaxBounds.z = *pVertexCoord;
	m_BoundingRadius = m_MinBounds.Length();

	// Go through all submeshes ...
	for (TUInt32 subMesh = 0; subMesh < m_NumSubMeshes; ++subMesh)
	{
		// Reject mesh if it contains empty sub-meshes
		if (m_SubMeshes[subMesh].numVertices == 0)
		{
			return false;
		}

		// Go through all vertices
		TUInt8* pVertex = m_SubMeshes[subMesh].vertices;
		for (TUInt32 vert = 0; vert < m_SubMeshes[subMesh].numVertices; ++vert)
		{
			// Get vertex coord as vector
			pVertexCoord = reinterpret_cast<TFloat32*>(pVertex); // Assume float x,y,z coord again
			CVector3 vertex;
			vertex.x = *pVertexCoord++;
			vertex.y = *pVertexCoord++;
			vertex.z = *pVertexCoord;
			
			// Compare vertex against current bounds, updating bounds where necessary
			if (vertex.x < m_MinBounds.x)
			{
				m_MinBounds.x = vertex.x;
			}
			if (vertex.x > m_MaxBounds.x)
			{
				m_MaxBounds.x = vertex.x;
			}
			++pVertexCoord;

			if (vertex.y < m_MinBounds.y)
			{
				m_MinBounds.y = vertex.y;
			}
			if (vertex.y > m_MaxBounds.y)
			{
				m_MaxBounds.y = vertex.y;
			}
			++pVertexCoord;

			if (vertex.z < m_MinBounds.z)
			{
				m_MinBounds.z = vertex.z;
			}
			if (vertex.z > m_MaxBounds.z)
			{
				m_MaxBounds.z = vertex.z;
			}

			TFloat32 length = vertex.Length();
			if (length > m_BoundingRadius)
			{
				m_BoundingRadius = length;
			}

			// Step to next vertex (flexible vertex size)
			pVertex += m_SubMeshes[subMesh].vertexSize;
		}
	}

	return true;
}


//-----------------------------------------------------------------------------
// Rendering
//-----------------------------------------------------------------------------

// Render the model using the given matrix list as a hierarchy (must be one matrix per node)
// and from the given camera
void CMesh::Render( LPDIRECT3DDEVICE9 d3dDevice, CRenderMethod* method,
					CRenderer* Renderer, CMatrix4x4* worldMat )
{
	if (m_HasGeometry)
	{
		//Allow Alpha blending
		d3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		d3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		d3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

		// Let d3d know the vertex format
		d3dDevice->SetFVF(  D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 );

		d3dDevice->SetVertexShader( method->m_vs->m_Shader );
		d3dDevice->SetPixelShader( method->m_ps->m_Shader );

		int a = m_NumSubMeshes;

		for (TUInt32 subMesh = 0; subMesh < m_NumSubMeshes; ++subMesh)
		{
			// Get a reference to the submesh and its material to reduce code clutter
			SSubMeshDX& sub = m_SubMeshesDX[subMesh];
			SMeshMaterialDX& material = m_Materials[sub.material];

			//Set textures from the material
			for (TUInt32 texture = 0; texture < material.numTextures; ++texture)
			{			
				d3dDevice->SetTexture( texture, material.textures[texture] );
			}
			
			Renderer->SetMethodWorldMat( &worldMat[sub.node], method->m_Method );

			// Tell DirectX the vertex and index buffers to use
			d3dDevice->SetStreamSource( 0, sub.vertexBuffer, 0, sub.vertexSize );
			d3dDevice->SetIndices( sub.indexBuffer );

			// Draw the primitives from the buffer - a triangle list
			d3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, sub.numVertices, 0,
			                                 sub.numIndices / 3 );
		}
	}
}

void CMesh::AddTexture(std::string fileName, LPDIRECT3DDEVICE9 d3dDevice)
{
	LPDIRECT3DTEXTURE9 tex;
	D3DXCreateTextureFromFile( d3dDevice, fileName.c_str(), &tex );

	m_Materials->textures[m_Materials->numTextures] = tex;
	m_Materials->numTextures += 1;
}

void CMesh::SetTexture(std::string fileName, LPDIRECT3DDEVICE9 d3dDevice)
{
	if( m_Materials->textures[0] )
		m_Materials->textures[0]->Release();

	m_Materials->numTextures--;

	LPDIRECT3DTEXTURE9 tex;
	D3DXCreateTextureFromFile( d3dDevice, fileName.c_str(), &tex );

	m_Materials->textures[m_Materials->numTextures] = tex;
	m_Materials->numTextures += 1;
}

} // namespace gen