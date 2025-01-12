#include "JtGltfMesh.h"

#include <JtData_Model.hxx>
#include <JtNode_Base.hxx>
#include <JtNode_Partition.hxx>
#include <JtNode_Part.hxx>
#include <JtNode_RangeLOD.hxx>
#include <JtNode_LOD.hxx>
#include <JtNode_Instance.hxx>
#include <JtNode_Shape_Vertex.hxx>
#include <JtNode_Shape_TriStripSet.hxx>
#include <JtNode_MetaData.hxx>
#include <JtNode_Shape_Base.hxx>
#include <JtNode_Shape_TriStripSet.hxx>

#include <JtProperty_LateLoaded.hxx>

#include <JtAttribute_Material.hxx>
#include <JtAttribute_GeometricTransform.hxx>

#include <JtElement_ProxyMetaData.hxx>
#include <JtElement_ShapeLOD_Vertex.hxx>

#include "JtUtils.h"
#include "JtLayer.h"

#include "vmmlib/matrix.hpp"
#include "vmmlib/vector.hpp"

#include <assert.h>
#include <limits>



using namespace std;



#ifdef WIN32
#include <Windows.h>
#include <stringapiset.h>
#define CP_UTF8 65001
#endif 

// Define these only in *one* .cc file.
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

// #define TINYGLTF_NOEXCEPTION // optional. disable exception handling.
#include "tiny_gltf.h"



#pragma warning( push )
#pragma warning( disable: 4267 4127)
struct gltfConfig {
    bool useLayer = false;
    std::vector<uint32_t> actFilter;
    tinygltf::Model model;
    tinygltf::Buffer buffer;
    tinygltf::Mesh mesh;
    bool isInRangeLOD = false;
    uint32_t LOD = 0;
    vmml::Matrix4d actTrsf;
    bool TrsfSet = false;
    float actDiffuseColor[4];
    float actAmbientColor[4];
    float actSpecularColor[4];
    float actEmissionColor[4];
    bool ColorSet = false;
};
#pragma warning( pop )

void addFloatToBuffer(float f, std::vector<unsigned char> &data)
{
    unsigned char b[4] = { 0 };
    memcpy(b, &f, 4);
    data.push_back(b[0]);
    data.push_back(b[1]);
    data.push_back(b[2]);
    data.push_back(b[3]);
}

void addUIntToBuffer(uint32_t value, std::vector<unsigned char>& data)
{
    data.push_back(value & 0xFF);
    data.push_back((value >> 8) & 0xFF);
    data.push_back((value >> 16) & 0xFF);
    data.push_back((value >> 24) & 0xFF);
}

void RecurseDownTheTreeGlTf(gltfConfig& config, const Handle(JtNode_Base)& theNodeRecord);
void HandleAllChildrenGlTf(gltfConfig& config, const Handle(JtNode_Group)& theGroupRecord);
void HandleAttributesGlTf(gltfConfig& config, const Handle(JtNode_Base)& theNodeRecord);


void writeTestGltfMesh( std::string name)
{
    // Create a model with a single mesh and save it as a gltf file
    tinygltf::Model m;
    tinygltf::Scene scene;
    tinygltf::Mesh mesh;
    tinygltf::Primitive primitive;
    tinygltf::Node node;
    tinygltf::Buffer buffer;
    tinygltf::BufferView bufferView1;
    tinygltf::BufferView bufferView2;
    tinygltf::Accessor accessor1;
    tinygltf::Accessor accessor2;
    tinygltf::Asset asset;

    // This is the raw data buffer. 
    buffer.data = {
        // 6 bytes of indices and two bytes of padding
        0x00,0x00,0x01,0x00,0x02,0x00,0x00,0x00,
        // 36 bytes of floating point numbers
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x3f,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x3f,
        0x00,0x00,0x00,0x00 };

    // "The indices of the vertices (ELEMENT_ARRAY_BUFFER) take up 6 bytes in the
    // start of the buffer.
    bufferView1.buffer = 0;
    bufferView1.byteOffset = 0;
    bufferView1.byteLength = 6;
    bufferView1.target = TINYGLTF_TARGET_ELEMENT_ARRAY_BUFFER;

    // The vertices take up 36 bytes (3 vertices * 3 floating points * 4 bytes)
    // at position 8 in the buffer and are of type ARRAY_BUFFER
    bufferView2.buffer = 0;
    bufferView2.byteOffset = 8;
    bufferView2.byteLength = 36;
    bufferView2.target = TINYGLTF_TARGET_ARRAY_BUFFER;

    // Describe the layout of bufferView1, the indices of the vertices
    accessor1.bufferView = 0;
    accessor1.byteOffset = 0;
    accessor1.componentType = TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT;
    accessor1.count = 3;
    accessor1.type = TINYGLTF_TYPE_SCALAR;
    accessor1.maxValues.push_back(2);
    accessor1.minValues.push_back(0);

    // Describe the layout of bufferView2, the vertices themself
    accessor2.bufferView = 1;
    accessor2.byteOffset = 0;
    accessor2.componentType = TINYGLTF_COMPONENT_TYPE_FLOAT;
    accessor2.count = 3;
    accessor2.type = TINYGLTF_TYPE_VEC3;
    accessor2.maxValues = { 1.0, 1.0, 0.0 };
    accessor2.minValues = { 0.0, 0.0, 0.0 };

    // Build the mesh primitive and add it to the mesh
    primitive.indices = 0;                 // The index of the accessor for the vertex indices
    primitive.attributes["POSITION"] = 1;  // The index of the accessor for positions
    primitive.material = 0;
    primitive.mode = TINYGLTF_MODE_TRIANGLES;
    mesh.primitives.push_back(primitive);

    // Other tie ups
    node.mesh = 0;
    scene.nodes.push_back(0); // Default scene

    // Define the asset. The version is required
    asset.version = "2.0";
    asset.generator = "tinygltf";

    // Now all that remains is to tie back all the loose objects into the
    // our single model.
    m.scenes.push_back(scene);
    m.meshes.push_back(mesh);
    m.nodes.push_back(node);
    m.buffers.push_back(buffer);
    m.bufferViews.push_back(bufferView1);
    m.bufferViews.push_back(bufferView2);
    m.accessors.push_back(accessor1);
    m.accessors.push_back(accessor2);
    m.asset = asset;

    // Create a simple material
    tinygltf::Material mat;
    mat.pbrMetallicRoughness.baseColorFactor = { 1.0f, 0.9f, 0.9f, 1.0f };
    mat.pbrMetallicRoughness.metallicFactor = 0.2;
    mat.pbrMetallicRoughness.roughnessFactor = 0.8;
    mat.doubleSided = true;
    m.materials.push_back(mat);

    // Save it to a file
    tinygltf::TinyGLTF gltf;
    gltf.WriteGltfSceneToFile(&m, name +".gltf",
        true, // embedImages
        true, // embedBuffers
        true, // pretty print
        false); // write binary

}

static Handle(Standard_Type) TypeOf_JtNode_Partition = STANDARD_TYPE(JtNode_Partition);
static Handle(Standard_Type) TypeOf_JtNode_Part = STANDARD_TYPE(JtNode_Part);
static Handle(Standard_Type) TypeOf_JtNode_RangeLOD = STANDARD_TYPE(JtNode_RangeLOD);
static Handle(Standard_Type) TypeOf_JtNode_LOD = STANDARD_TYPE(JtNode_LOD);
static Handle(Standard_Type) TypeOf_JtNode_Group = STANDARD_TYPE(JtNode_Group);
static Handle(Standard_Type) TypeOf_JtNode_Instance = STANDARD_TYPE(JtNode_Instance);
static Handle(Standard_Type) TypeOf_JtNode_Shape_Vertex = STANDARD_TYPE(JtNode_Shape_Vertex);
static Handle(Standard_Type) TypeOf_JtNode_MetaData = STANDARD_TYPE(JtNode_MetaData);
static Handle(Standard_Type) TypeOf_JtNode_Shape_TriStripSet = STANDARD_TYPE(JtNode_Shape_TriStripSet);
static Handle(Standard_Type) TypeOf_JtAttribute_Material = STANDARD_TYPE(JtAttribute_Material);
static Handle(Standard_Type) TypeOf_JtAttribute_GeometricTransform = STANDARD_TYPE(JtAttribute_GeometricTransform);
static Handle(Standard_Type) TypeOf_JtElement_ProxyMetaData = STANDARD_TYPE(JtElement_ProxyMetaData);
static Handle(Standard_Type) TypeOf_JtElement_ShapeLOD_Vertex = STANDARD_TYPE(JtElement_ShapeLOD_Vertex);



void writeGltfMesh(const Handle(JtNode_Base)& theNodeRecord, const std::string& name)
{
    gltfConfig config;

    tinygltf::Scene scene;
    tinygltf::Node node;
    tinygltf::Asset asset;

    // preallocate
    config.buffer.data.reserve(1024 * 8);

    // Create a simple default material
    tinygltf::Material mat;
    mat.pbrMetallicRoughness.baseColorFactor = { 0.6f, 0.6f, 0.6f, 1.0f };
    mat.pbrMetallicRoughness.metallicFactor = 0.2;
    mat.pbrMetallicRoughness.roughnessFactor = 0.8;
    mat.doubleSided = true;
    config.model.materials.push_back(mat);


    RecurseDownTheTreeGlTf(config, theNodeRecord);

    // finalise

    // Define the asset. The version is required
    asset.version = "2.0";
    asset.generator = "PyOpenJt";
    config.model.asset = asset;

    // Other tie ups
    node.mesh = 0;
    scene.nodes.push_back(0); // Default scene

    config.model.scenes.push_back(scene);
    config.model.meshes.push_back(config.mesh);
    config.model.nodes.push_back(node);
    config.model.buffers.push_back(config.buffer);

    // Save it to a file
    tinygltf::TinyGLTF gltf;
    gltf.WriteGltfSceneToFile(&config.model, name + ".gltf",
        true, // embedImages
        true, // embedBuffers
        true, // pretty print
        false); // write binary
}


void RecurseDownTheTreeGlTf(gltfConfig& config, const Handle(JtNode_Base)& theNodeRecord)
{

    // Handle all different types of LSG Nodes
    if (theNodeRecord->IsKind(TypeOf_JtNode_Partition))
    {
        Handle(JtNode_Partition) aPartitionRecord = Handle(JtNode_Partition)::DownCast(theNodeRecord);


        HandleAttributesGlTf(config,  theNodeRecord);
        HandleAllChildrenGlTf(config, aPartitionRecord);

    }
    else if (theNodeRecord->IsKind(TypeOf_JtNode_Part))
    {
        // cast to actuall type
        Handle(JtNode_Part) aPartRecord = Handle(JtNode_Part)::DownCast(theNodeRecord);
        const JtData_Object::VectorOfLateLoads& aLateLoaded = aPartRecord->LateLoads();
        auto name = theNodeRecord->Name();
        bool useThisPart = true;
        if (config.useLayer == true) {
            // handle late loaded properties
            auto metaData = HandleLateLoadsMeta(aLateLoaded);
            if (metaData.size() > 0) {
                useThisPart = false;
                // handling layer in Part
                std::vector<uint32_t> layer = ScanForLayer(metaData);
                for (auto lp : layer) {
                    for (auto l : config.actFilter) {
                        if (lp == l) {
                            useThisPart = true;
                            goto end;
                        }
                    }
                }
            }
        }
end:

        if (useThisPart) {
            HandleAttributesGlTf(config, theNodeRecord);
            HandleAllChildrenGlTf(config, Handle(JtNode_Group)::DownCast(theNodeRecord));
        }
    }
    else if (theNodeRecord->IsKind(TypeOf_JtNode_MetaData))
    {
        Handle(JtNode_MetaData) aPartRecord = Handle(JtNode_MetaData)::DownCast(theNodeRecord);
        const JtData_Object::VectorOfLateLoads& aLateLoaded = aPartRecord->LateLoads();

        
        // handle late loaded properties
        auto metaData = HandleLateLoadsMeta(aLateLoaded);
        if (metaData.size() > 0) {
            
            // handling layer in top meta data
            auto layerInfo = ScanForLayerFilter(metaData);
            if (layerInfo.ActiveLayerFilter.size() != 0) {
                config.useLayer = true;
                config.actFilter = layerInfo.LayerMap[layerInfo.ActiveLayerFilter];
            }
        }

        HandleAttributesGlTf(config, theNodeRecord);
        HandleAllChildrenGlTf(config, Handle(JtNode_Group)::DownCast(theNodeRecord));

        
    }
    else if (theNodeRecord->IsKind(TypeOf_JtNode_RangeLOD))
    {
        Handle(JtNode_RangeLOD) aRangeLODRecord =
            Handle(JtNode_RangeLOD)::DownCast(theNodeRecord);

        config.isInRangeLOD = true;
        config.LOD = 0;
        HandleAttributesGlTf(config, theNodeRecord);
        HandleAllChildrenGlTf(config, aRangeLODRecord);
        config.isInRangeLOD = false;
       
    }
    else if (theNodeRecord->IsKind(TypeOf_JtNode_LOD))
    {
   
        HandleAttributesGlTf(config, theNodeRecord);
        HandleAllChildrenGlTf(config, Handle(JtNode_Group)::DownCast(theNodeRecord));

        
    }
    else if (theNodeRecord->IsKind(TypeOf_JtNode_Group))
    {
        
        HandleAttributesGlTf(config, theNodeRecord);
        HandleAllChildrenGlTf(config, Handle(JtNode_Group)::DownCast(theNodeRecord));

        if (config.isInRangeLOD)
            config.LOD++;

    }
    else if (theNodeRecord->IsKind(TypeOf_JtNode_Instance))
    {
        Handle(JtNode_Instance) anInstance = Handle(JtNode_Instance)::DownCast(theNodeRecord);        

        HandleAttributesGlTf(config, theNodeRecord);

        Handle(JtNode_Base) aNode = Handle(JtNode_Base)::DownCast(anInstance->Object());
        
        RecurseDownTheTreeGlTf(config, aNode);

        // reset the transformation
        config.TrsfSet = false;

    }
    else if (theNodeRecord->IsKind(TypeOf_JtNode_Shape_Vertex))
    {
        if (config.isInRangeLOD && config.LOD !=0)
            return;

        Handle(JtNode_Shape_Vertex) aShapeRecord = Handle(JtNode_Shape_Vertex)::DownCast(theNodeRecord);

        if (aShapeRecord->IsKind(TypeOf_JtNode_Shape_TriStripSet))
        {
            Handle(JtNode_Shape_TriStripSet) aMeshRecord = Handle(JtNode_Shape_TriStripSet)::DownCast(aShapeRecord);

            theNodeRecord->Name();


            auto boundBox = aMeshRecord->Bounds();
            const JtData_Object::VectorOfLateLoads& aLateLoaded = aMeshRecord->LateLoads();

            // get probably material
            HandleAttributesGlTf(config, theNodeRecord);

            for (int i = 0; i < aLateLoaded.Count(); i++) {
                //cerr << "TriStripSet Late load SegType: " << aLateLoaded[i]->getSegmentType() << '\n';
                if (aLateLoaded[i]->DefferedObject().IsNull())
                    aLateLoaded[i]->Load();

                Handle(JtData_Object) prop = aLateLoaded[i]->DefferedObject();
                if (!prop.IsNull() && prop->IsKind(TypeOf_JtElement_ShapeLOD_Vertex)) {
                    Handle(JtElement_ShapeLOD_Vertex) aProxyMetaDataElement = Handle(JtElement_ShapeLOD_Vertex)::DownCast(prop);

                    auto startIndex = config.buffer.data.size();

                    auto indices = aProxyMetaDataElement->Indices();
                    auto vertices = aProxyMetaDataElement->Vertices();
                    auto normals = aProxyMetaDataElement->Normals();

                    uint32_t maxIdx = 0, minIdx = UINT32_MAX;

                    for (int l = 0; l < indices.Count(); l++) {
                        const uint32_t value = indices.Data()[l];
                        
                        if (value > maxIdx) maxIdx = value;
                        if (value < minIdx) minIdx = value;

                        addUIntToBuffer(value, config.buffer.data);
                    }
                    auto startVertex = config.buffer.data.size();
                    
                    double minVrtxX = numeric_limits<float>::max(), maxVrtxX = -numeric_limits<float>::max();
                    double minVrtxY = numeric_limits<float>::max(), maxVrtxY = -numeric_limits<float>::max();
                    double minVrtxZ = numeric_limits<float>::max(), maxVrtxZ = -numeric_limits<float>::max();
                    for (int l = 0; l < vertices.Count(); l++) {
                        
                        vmml::vec4d vrtx(
                            vertices.Data()[l * 3 + 0],
                            vertices.Data()[l * 3 + 1],
                            vertices.Data()[l * 3 + 2],
                            1.0
                        );

                        if (config.TrsfSet) {
                            vrtx = config.actTrsf * vrtx;
                        }

                        // track the bounds for the gltf accessors
                        if (vrtx.x() > maxVrtxX) maxVrtxX = vrtx.x();
                        if (vrtx.x() < minVrtxX) minVrtxX = vrtx.x();
                        if (vrtx.y() > maxVrtxY) maxVrtxY = vrtx.y();
                        if (vrtx.y() < minVrtxY) minVrtxY = vrtx.y();
                        if (vrtx.z() > maxVrtxZ) maxVrtxZ = vrtx.z();
                        if (vrtx.z() < minVrtxZ) minVrtxZ = vrtx.z();

                        
                        addFloatToBuffer((float)vrtx.x(), config.buffer.data);
                        addFloatToBuffer((float)vrtx.y(), config.buffer.data);
                        addFloatToBuffer((float)vrtx.z(), config.buffer.data);
                        
                    }
                    auto startNormals = config.buffer.data.size();
                    
                    double minNrmX = numeric_limits<float>::max(), maxNrmX = -numeric_limits<float>::max();
                    double minNrmY = numeric_limits<float>::max(), maxNrmY = -numeric_limits<float>::max();
                    double minNrmZ = numeric_limits<float>::max(), maxNrmZ = -numeric_limits<float>::max();

                    for (int l = 0; l < normals.Count(); l++) {

                        vmml::vec4d normal(
                            normals.Data()[l * 3 + 0],
                            normals.Data()[l * 3 + 1],
                            normals.Data()[l * 3 + 2],
                            1.0
                        );

                        if (config.TrsfSet) {
                            auto tmpTrsf = config.actTrsf;
                            // only use the rotation
                            tmpTrsf(0, 3) = 0.0;
                            tmpTrsf(1, 3) = 0.0;
                            tmpTrsf(2, 3) = 0.0;
                            normal = tmpTrsf * normal;
                        }

                        // track the bounds for the gltf accessors
                        if (normal.x() > maxNrmX) maxNrmX = normal.x();
                        if (normal.x() < minNrmX) minNrmX = normal.x();
                        if (normal.y() > maxNrmY) maxNrmY = normal.y();
                        if (normal.y() < minNrmY) minNrmY = normal.y();
                        if (normal.z() > maxNrmZ) maxNrmZ = normal.z();
                        if (normal.z() < minNrmZ) minNrmZ = normal.z();

                        addFloatToBuffer((float)normal.x(), config.buffer.data);
                        addFloatToBuffer((float)normal.y(), config.buffer.data);
                        addFloatToBuffer((float)normal.z(), config.buffer.data);

                    }
                    auto endNormals = config.buffer.data.size();

                    tinygltf::BufferView bufferViewIdx, bufferViewVrtx, bufferViewNrm;
                    tinygltf::Accessor accessorIdx, accessorVrtx, accessorNrm;
                    
                    // "The indices of the vertices (ELEMENT_ARRAY_BUFFER)
                    bufferViewIdx.buffer = 0;
                    bufferViewIdx.byteOffset = startIndex;
                    bufferViewIdx.byteLength = startVertex - startIndex;
                    bufferViewIdx.target = TINYGLTF_TARGET_ELEMENT_ARRAY_BUFFER;
                    config.model.bufferViews.push_back(bufferViewIdx);

                    // Describe the layout of bufferViewIdx, the indices of the vertices
                    accessorIdx.bufferView = (int) config.model.bufferViews.size() -1 ;
                    accessorIdx.byteOffset = 0;
                    accessorIdx.componentType = TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT;
                    accessorIdx.count = (startVertex - startIndex) / 4;
                    accessorIdx.type = TINYGLTF_TYPE_SCALAR;
                    accessorIdx.maxValues.push_back(maxIdx);
                    accessorIdx.minValues.push_back(minIdx);
                    config.model.accessors.push_back(accessorIdx);

                    // The vertices 
                    bufferViewVrtx.buffer = 0;
                    bufferViewVrtx.byteOffset = startVertex;
                    bufferViewVrtx.byteLength = startNormals - startVertex;
                    bufferViewVrtx.target = TINYGLTF_TARGET_ARRAY_BUFFER;
                    config.model.bufferViews.push_back(bufferViewVrtx);

                    // Describe the layout of bufferViewVrtx, the vertices themself
                    accessorVrtx.bufferView = (int) config.model.bufferViews.size() - 1;
                    accessorVrtx.byteOffset = 0;
                    accessorVrtx.componentType = TINYGLTF_COMPONENT_TYPE_FLOAT;
                    accessorVrtx.count = (startNormals - startVertex) / 12;
                    accessorVrtx.type = TINYGLTF_TYPE_VEC3;
                    accessorVrtx.maxValues = { maxVrtxX, maxVrtxY, maxVrtxZ };
                    accessorVrtx.minValues = { minVrtxX, minVrtxY, minVrtxZ };
                    config.model.accessors.push_back(accessorVrtx);

                    // The normals
                    bufferViewNrm.buffer = 0;
                    bufferViewNrm.byteOffset = startNormals;
                    bufferViewNrm.byteLength = endNormals - startNormals;
                    bufferViewNrm.target = TINYGLTF_TARGET_ARRAY_BUFFER;
                    config.model.bufferViews.push_back(bufferViewNrm);

                    // Describe the layout of bufferViewNrm, the vertices themself
                    accessorNrm.bufferView = (int) config.model.bufferViews.size() - 1;
                    accessorNrm.byteOffset = 0;
                    accessorNrm.componentType = TINYGLTF_COMPONENT_TYPE_FLOAT;
                    accessorNrm.count = (endNormals - startNormals) / 12;
                    accessorNrm.type = TINYGLTF_TYPE_VEC3;
                    accessorNrm.maxValues = { maxNrmX, maxNrmY, maxNrmZ };
                    accessorNrm.minValues = { minNrmX, minNrmY, minNrmZ };
                    config.model.accessors.push_back(accessorNrm);

                    int material = 0;
                    if (config.ColorSet) {
                        // Create a material
                        tinygltf::Material mat;
                        mat.pbrMetallicRoughness.baseColorFactor[0] = config.actDiffuseColor[0];
                        mat.pbrMetallicRoughness.baseColorFactor[1] = config.actDiffuseColor[1];
                        mat.pbrMetallicRoughness.baseColorFactor[2] = config.actDiffuseColor[2];
                        mat.pbrMetallicRoughness.baseColorFactor[3] = config.actDiffuseColor[3];
                        mat.pbrMetallicRoughness.metallicFactor = 0.2;
                        mat.pbrMetallicRoughness.roughnessFactor = 0.8;
                        mat.doubleSided = true;

                        material = (int)config.model.materials.size();
                        config.model.materials.push_back(mat);

                    }

                    // Build the mesh primitive and add it to the mesh
                    tinygltf::Primitive primitive;
                    primitive.indices = (int) config.model.accessors.size() - 3;                 // The index of the accessor for the vertex indices
                    primitive.attributes["POSITION"] = (int) config.model.accessors.size() - 2;  // The index of the accessor for positions
                    primitive.attributes["NORMAL"] = (int) config.model.accessors.size() - 1;  // The index of the accessor for normals
                    primitive.material = material;
                    primitive.mode = TINYGLTF_MODE_TRIANGLES;
                    config.mesh.primitives.push_back(primitive);
                }

            }
            
            // reset the color
            config.ColorSet = false;
        }
        else
            cerr << "!!! '" << theNodeRecord->Name() << "' " << "TypeOf_JtNode_Shape_Vertex->Unknown \n";
    }
    else
    {
        cerr << "!!! '" << theNodeRecord->Name() << "' " << "Unknown node type \n";
    }

    

}


void HandleAttributesGlTf(gltfConfig& config, const Handle(JtNode_Base)& theNodeRecord)
{
    // Extract attributes

    if (theNodeRecord->Attributes().IsEmpty())
        return;

    for (Standard_Integer anIdx = 0; anIdx < (Standard_Integer)theNodeRecord->Attributes().Count(); ++anIdx)
    {
        const Handle(JtData_Object)& anObject = theNodeRecord->Attributes()[anIdx];

        if (anObject.IsNull())
            continue;

        Handle(JtAttribute_Base) anAttrib = Handle(JtAttribute_Base)::DownCast(anObject);

        if (anAttrib.IsNull())
            cerr << "Error! Invalid node attribute\n";


        if (anAttrib->IsKind(TypeOf_JtAttribute_GeometricTransform))
        {
            Handle(JtAttribute_GeometricTransform) aTransform =
                Handle(JtAttribute_GeometricTransform)::DownCast(anAttrib);

            config.actTrsf(0, 0) = aTransform->GetTrsf()[0];
            config.actTrsf(1, 0) = aTransform->GetTrsf()[1];
            config.actTrsf(2, 0) = aTransform->GetTrsf()[2];
            config.actTrsf(3, 0) = aTransform->GetTrsf()[3];
            config.actTrsf(0, 1) = aTransform->GetTrsf()[4];
            config.actTrsf(1, 1) = aTransform->GetTrsf()[5];
            config.actTrsf(2, 1) = aTransform->GetTrsf()[6];
            config.actTrsf(3, 1) = aTransform->GetTrsf()[7];
            config.actTrsf(0, 2) = aTransform->GetTrsf()[8];
            config.actTrsf(1, 2) = aTransform->GetTrsf()[9];
            config.actTrsf(2, 2) = aTransform->GetTrsf()[10];
            config.actTrsf(3, 2) = aTransform->GetTrsf()[11];
            config.actTrsf(0, 3) = aTransform->GetTrsf()[12];
            config.actTrsf(1, 3) = aTransform->GetTrsf()[13];
            config.actTrsf(2, 3) = aTransform->GetTrsf()[14];
            config.actTrsf(3, 3) = aTransform->GetTrsf()[15];

            config.TrsfSet = true;

        }
        else if (anAttrib->IsKind(TypeOf_JtAttribute_Material))
        {
            Handle(JtAttribute_Material) aMaterial =
                Handle(JtAttribute_Material)::DownCast(anAttrib);
            
            memcpy(config.actDiffuseColor, aMaterial->DiffuseColor(), 16);
            memcpy(config.actAmbientColor, aMaterial->AmbientColor(), 16);
            memcpy(config.actSpecularColor, aMaterial->SpecularColor(), 16);
            memcpy(config.actEmissionColor, aMaterial->EmissionColor(), 16);
            config.ColorSet = true;
        }
        else {
            cerr << "Unknown Attribute type \n";
        }

    }


}

void HandleAllChildrenGlTf(gltfConfig& config, const Handle(JtNode_Group)& theGroupRecord)
{
    if (theGroupRecord->Children().IsEmpty())
        return;

    for (Standard_Integer aChildIdx = 0; aChildIdx < (Standard_Integer)theGroupRecord->Children().Count(); ++aChildIdx)
    {

        Handle(JtNode_Base) aChildRecord = Handle(JtNode_Base)::DownCast(theGroupRecord->Children()[aChildIdx]);

        if (aChildRecord.IsNull())
        {
            
            continue;
        }

        RecurseDownTheTreeGlTf(config, aChildRecord);
    }

    
}