#include "apiMeshShapeTranslator.h"
#include <maya/MFnDagNode.h>
#include <maya/MShaderManager.h>
#include "apiMeshShape.h"

AtNode* CApiMeshShapeTranslator::CreateArnoldNodes()
{
	return AddArnoldNode("polymesh");
}

void CApiMeshShapeTranslator::Export(AtNode* polymesh)
{
	Export(polymesh, false);
}

void CApiMeshShapeTranslator::ExportMeshShaders(AtNode* polymesh)
{		
	//Get dag node from dagPath
	MFnDagNode fnDagNode(m_dagPath);
	
	//Get shader node from the dagNode and set it to shader attribute.
	MPlugArray connections;
	auto shadingGroup = GetNodeShadingGroup(fnDagNode.object(), 0);
	auto shader = ExportConnectedNode(shadingGroup);

	AiNodeSetPtr(polymesh, "shader", shader);
}


void CApiMeshShapeTranslator::Export(AtNode* polymesh, bool isUpdate)
{
	//Export transforms
	ExportMatrix(polymesh);

	//Export rendering parameters
	ProcessRenderFlags(polymesh);
	
	//Export light linking
	ExportLightLinking(polymesh);	

	// We could enable/disable motion deform here, but I'll ignore it and make it never deforming.
	m_motionDeform = false;

	//Export shaders
	ExportMeshShaders(polymesh);
	
	//If it is an update call, we won't need to export geometry again.
	if(!isUpdate)
	{
		ExportMeshData(polymesh);
	}
}

void CApiMeshShapeTranslator::ExportMeshData(AtNode* polymesh)
{
	// Usually this kind of method should be with an extra parameter, step for motion blur. 
	// I'll ignore it in here to make the sample as simple as possible.
	MStatus status;
	m_dagPath.extendToShape();
	MFnDagNode fnDag(m_dagPath);	

	// Get apiMeshShape node first
	auto apiMeshNode = dynamic_cast<apiMesh*>(fnDag.userNode());
	if(NULL == apiMeshNode)
	{
		MGlobal::displayError("Failed to convert apiMeshShape");
		return;
	}

	
	auto geom = apiMeshNode->meshGeom();
	auto verticesArray = geom->vertices;
	unsigned int numVertices = verticesArray.length();
	unsigned int numNormals = geom->normals.length();
	unsigned int numPolygons = geom->faceCount;
	auto vertices = new float[verticesArray.length() * 3];
	auto normals = new float[numNormals * 3];
	
	// Get raw vertices
	for (int i = 0; i < numVertices; ++i)
	{
		vertices[i*3] = verticesArray[i].x;	
		vertices[i*3 + 1] = verticesArray[i].y;
		vertices[i*3 + 2] = verticesArray[i].z;		
	}

	// Get raw normals
	// We need to export normals if it is smoothShading and doesn't have arnold subdivision properties. For apiMeshShape, export always.
	for (int i = 0; i < numNormals; ++i)
	{
		normals[i*3] = geom->normals[i].x;	
		normals[i*3 + 1] =  geom->normals[i].y;
		normals[i*3 + 2] =  geom->normals[i].z;		
	}

			
	// We always export the first frame.
	// Please check XGen's sample for how to export shapes in motion.
	// We are going just export basic geometry infos(vertices, normals), 
	// there could be other info like references or color sets etc... I'll skip it here.
	AtArray* uvs = nullptr;
	AtArray* nsides = nullptr;
	AtArray* vertexIndices = nullptr;
	AtArray* normalIndices = nullptr;


	// Component IDs
	// Vertices per polygon count
	nsides = AiArrayAllocate(numPolygons,1,AI_TYPE_UINT);
	unsigned int polygonVertexCount = 0;
	for(unsigned int i = 0; i < numPolygons; ++i)
	{
		auto numPolygonVertexCount = geom->face_counts[i];
		polygonVertexCount += numPolygonVertexCount;
		AiArraySetUInt(nsides, i, numPolygonVertexCount);
	}

	// Vertex indices and normal indices
	vertexIndices = AiArrayAllocate(polygonVertexCount, 1, AI_TYPE_UINT);
	normalIndices = AiArrayAllocate(polygonVertexCount, 1, AI_TYPE_UINT);

	MIntArray polygonVertexIndices;
	unsigned int vertexIndex = 0;
	for(unsigned int polygon_id =0; polygon_id < geom->face_connects.length(); ++polygon_id)
	{
		AiArraySetUInt(vertexIndices, polygon_id, geom->face_connects[polygon_id]);		
		AiArraySetUInt(normalIndices, polygon_id, geom->face_connects[polygon_id]);

	}

	// Set array parameters
	AiNodeSetArray(polymesh, "vlist", AiArrayConvert(numVertices * 3, 1, AI_TYPE_FLOAT, vertices));
	AiNodeSetArray(polymesh, "nlist", AiArrayConvert(numNormals * 3, 1, AI_TYPE_FLOAT, normals));

	AiNodeSetArray(polymesh, "nsides", nsides);
	AiNodeSetArray(polymesh, "vidxs", vertexIndices);
	AiNodeSetArray(polymesh,"nidxs", normalIndices);	
}

void CApiMeshShapeTranslator::Update(AtNode* polymesh)
{
	Export(polymesh, true);
}

