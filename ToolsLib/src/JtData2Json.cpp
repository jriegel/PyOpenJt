#include "JtData2Json.h"

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

#include <assert.h>

using namespace std;


const char* SegmentTypeString(int typeNum) {

	switch (typeNum) {
	case 1: return "Logical Scene Graph";
	case 2: return "JT B-Rep";
	case 3: return "PMI Data";
	case 4: return "Meta Data";
	case 6: return "Shape";
	case 7: return "Shape LOD0";
	case 8: return "Shape LOD1";
	case 9: return "Shape LOD2";
	case 10: return "Shape LOD3";
	case 11: return "Shape LOD4";
	case 12: return "Shape LOD5";
	case 13: return "Shape LOD6";
	case 14: return "Shape LOD7";
	case 15: return "Shape LOD8";
	case 16: return "Shape LOD9";
	case 17: return "XT B-Rep";
	case 18: return "Wireframe Representation";
	case 20: return "ULP";
	case 23: return "STT";
	case 24: return "LWPA";
	case 30: return "MultiXT B-Rep";
	case 31: return "InfoSegment";
	case 32: return "AEC Shape";
	case 33: return "STEP B-rep";
	default: return "Unknown";
	}
}

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

std::ostream& operator<<(std::ostream& os, const TCollection_ExtendedString& extstr)
{
	static char str[2048];
	assert(extstr.LengthOfCString() < 2048);
	Standard_PCharacter mchastr = str;
	extstr.ToUTF8CString(mchastr);
    string temp = mchastr;
    auto res = ReplaceAll(temp, "\\", "\\\\");

	return (os << res);
}

std::ostream& operator<<(std::ostream& os, const Jt_GUID& extstr)
{
	char str[128];
	extstr.ToString(str);
	return (os << str);
}


int writeModel(Handle(JtData_Model) model, std::ostream& out, int indent /* = 0 */ , int config /* = 0 */)
{
	out << indentOp(indent) << "\"FileName\":\"" << model->FileName() << "\",\n";
	out << indentOp(indent) << "\"MajorVersion\":\"" << model->MajorVersion() << "\",\n";
	out << indentOp(indent) << "\"MinorVersion\":\"" << model->MinorVersion() << "\",\n";
	if (config & dump_TOC) {
		auto toc = model->getTOC();

		if (toc.size() > 0) {
			out << indentOp(indent) << "\"TocTable\": [\n";
			indent++;
			bool first = true;
			for (auto it : toc){
				out << indentOp(indent);
				if (first)
					first = false;
				else
					out << ',';
				out << "[\"" << it.GUID << "\"," << it.Type << ",\"" << SegmentTypeString(it.Type) << "\"]\n";
					
			}
			indent--;
			out << indentOp(indent) << "\n],\n";
		}
	}
	return 0;
}

int writeKeyValueStream(const std::vector<char>& stream, std::ostream& out, int indent )
{
	out << indentOp(indent) << "{\n";

	bool tick = true;

	out << indentOp(indent) << "\t\"";
	for (auto it = stream.begin(); it != stream.end(); ++it) {
		if (*it == '\0') {
			if (tick) {
				out << "\":\"";
				tick = false;
			}
			else {
				out << "\"";
				if ((it + 1) == stream.end())
					out << "\n";
				else
					out << ",\n" << indentOp(indent) << "\t\"";
				tick = true;
			}
		}
		else {
			if (*it == '\n')
				out << "\\\\n";
			else if (*it == '\"')
				out << "\\\\\"";
			else if (*it == '\r')
				out << "\\\\r";
			else if (*it == '\b')
				out << "\\\\b";
			else if (*it == '\f')
				out << "\\\\f";
			else if (*it == '\t')
				out << "\\\\t";
			else if (*it == '\\')
				out << "\\\\";
			else
				out << *it;
		}

	}

	out << indentOp(indent) << "}\n";

	return 0;
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

void WriteMeshesWithoutLSG(dumpConfig , std::ostream& outStream, Handle(JtData_Model) rootModel, const std::string& )
{
    auto toc = rootModel->getTOC();

    int indent = 1;

    if (toc.size() > 0) {
        outStream << indentOp(indent) << "\"MeshTable\": [\n";
        indent++;
        bool first = true;
        for (auto it : toc) {
            if (it.Type == 7) {
                outStream << indentOp(indent);
                if (first)
                    first = false;
                else
                    outStream << ',';

                char guidBuf[100]; it.GUID.ToString(guidBuf);
                outStream << "[\"" << guidBuf << "\"," << it.Type << "]\n";
            }
        }
        indent--;
        outStream << indentOp(indent) << "\n]\n";
    }

}

void RecurseDownTheTreeJson(dumpConfig config, int & indention, std::ostream& outStream, const Handle(JtNode_Base)& theNodeRecord, const std::string& thePrefix)
{

    // Handle all different types of LSG Nodes
    if (theNodeRecord->IsKind(TypeOf_JtNode_Partition))
    {
        Handle(JtNode_Partition) aPartitionRecord = Handle(JtNode_Partition)::DownCast(theNodeRecord);

        outStream << indentOp(indention) << "\"LSG\":\n";
        outStream << indentOp(indention) << "{\n";
        indention++;
        outStream << indentOp(indention) << "\"jt_type\":\"Partition\",\n";
        outStream << indentOp(indention) << "\"name\":\"" << theNodeRecord->Name() << "\",\n";

        HandleAttributesJson(config, indention, outStream, theNodeRecord);
        HandleAllChildrenJson(config, indention, outStream, aPartitionRecord, thePrefix);

        indention--;
        outStream << indentOp(indention) << "}\n";
    }
    else if (theNodeRecord->IsKind(TypeOf_JtNode_Part))
    {
        // cast to actuall type
        Handle(JtNode_Part) aPartRecord = Handle(JtNode_Part)::DownCast(theNodeRecord);
        const JtData_Object::VectorOfLateLoads& aLateLoaded = aPartRecord->LateLoads();

        outStream << indentOp(indention) << "{\n";
        indention++;
        outStream << indentOp(indention) << "\"jt_type\":\"Part\",\n";
        outStream << indentOp(indention) << "\"name\":\"" << theNodeRecord->Name() << "\",\n";

        // handle late loaded properties
        auto metaData = HandleLateLoadsMeta(aLateLoaded);
        if (metaData.size() > 0) {
            outStream << indentOp(indention) << "\"properties\":\n";
            writeKeyValueStream(metaData, outStream, indention);

            // handling layer in Part
            std::vector<uint32_t> layer = ScanForLayer(metaData);
            outStream << indentOp(indention) << ",\"layer\":[";
            bool first = true;
            for (const auto& layerNum : layer) {
                if (first)
                    first = false;
                else
                    outStream << ',';
                outStream << layerNum;
            }
            outStream << "],\n";
        }

        HandleAttributesJson(config, indention, outStream, theNodeRecord);
        HandleAllChildrenJson(config, indention, outStream, Handle(JtNode_Group)::DownCast(theNodeRecord), thePrefix);

        indention--;
        outStream << indentOp(indention) << "}\n";

    }
    else if (theNodeRecord->IsKind(TypeOf_JtNode_MetaData))
    {
        Handle(JtNode_MetaData) aPartRecord = Handle(JtNode_MetaData)::DownCast(theNodeRecord);
        const JtData_Object::VectorOfLateLoads& aLateLoaded = aPartRecord->LateLoads();

        outStream << indentOp(indention) << "{\n";
        indention++;
        outStream << indentOp(indention) << "\"jt_type\":\"MetaData\",\n";
        outStream << indentOp(indention) << "\"name\":\"" << theNodeRecord->Name() << "\",\n";

        // handle late loaded properties
        auto metaData = HandleLateLoadsMeta(aLateLoaded);
        if (metaData.size() > 0) {
            outStream << indentOp(indention) << "\"properties\":\n";
            writeKeyValueStream(metaData, outStream, indention);

            // handling layer in top meta data
            LayerInfo layerInfo = ScanForLayerFilter(metaData);
            outStream << indentOp(indention) << ",\"layer-filter\":[";
            bool first = true;
            for (const auto& layerFilter : layerInfo.LayerMap) {
                if (first)
                    first = false;
                else
                    outStream << ',';
                outStream << "{ \"name\":\"" << layerFilter.first << "\", \"layer\":[";
                bool first2 = true;
                for (const auto layerNum : layerFilter.second) {
                    if (first2)
                        first2 = false;
                    else
                        outStream << ',';
                    outStream << layerNum;
                }
                outStream << "]}";
            }
            outStream << "],\n";
            outStream << indentOp(indention) << "\"active-layer-filter\":\"" << layerInfo.ActiveLayerFilter << "\",\n";
        }

        HandleAttributesJson(config, indention, outStream, theNodeRecord);
        HandleAllChildrenJson(config, indention, outStream, Handle(JtNode_Group)::DownCast(theNodeRecord), thePrefix);

        indention--;
        outStream << indentOp(indention) << "}\n";

    }
    else if (theNodeRecord->IsKind(TypeOf_JtNode_RangeLOD))
    {
        Handle(JtNode_RangeLOD) aRangeLODRecord =
            Handle(JtNode_RangeLOD)::DownCast(theNodeRecord);

        outStream << indentOp(indention) << "{\n";
        indention++;
        outStream << indentOp(indention) << "\"jt_type\":\"RangeLOD\",\n";
        outStream << indentOp(indention) << "\"name\":\"" << theNodeRecord->Name() << "\",\n";

        outStream << indentOp(indention) << "\"center\":["
            << (aRangeLODRecord->Center().X) << ','
            << (aRangeLODRecord->Center().Y) << ','
            << (aRangeLODRecord->Center().Z) << "],\n";

        outStream << indentOp(indention) << "\"limits\":[";
        bool first = true;

        for (Standard_Integer anIdx = 1; anIdx <= aRangeLODRecord->RangeLimits().Count(); ++anIdx) {
            if (first)
                first = false;
            else
                outStream << ',';
            outStream << aRangeLODRecord->RangeLimits()[anIdx];
        }
        outStream << "],\n";

        HandleAttributesJson(config, indention, outStream, theNodeRecord);
        HandleAllChildrenJson(config, indention, outStream, aRangeLODRecord, thePrefix);

        indention--;
        outStream << indentOp(indention) << "}\n";

    }
    else if (theNodeRecord->IsKind(TypeOf_JtNode_LOD))
    {
        outStream << indentOp(indention) << "{\n";
        indention++;
        outStream << indentOp(indention) << "\"jt_type\":\"LOD\",\n";
        outStream << indentOp(indention) << "\"name\":\"" << theNodeRecord->Name() << "\",\n";

        HandleAttributesJson(config, indention, outStream, theNodeRecord);
        HandleAllChildrenJson(config, indention, outStream, Handle(JtNode_Group)::DownCast(theNodeRecord), thePrefix);

        indention--;
        outStream << indentOp(indention) << "}\n";

    }
    else if (theNodeRecord->IsKind(TypeOf_JtNode_Group))
    {
        outStream << indentOp(indention) << "{\n";
        indention++;
        outStream << indentOp(indention) << "\"jt_type\":\"Group\",\n";
        outStream << indentOp(indention) << "\"name\":\"" << theNodeRecord->Name() << "\",\n";

        HandleAttributesJson(config, indention, outStream, theNodeRecord);
        HandleAllChildrenJson(config, indention, outStream, Handle(JtNode_Group)::DownCast(theNodeRecord), thePrefix);

        indention--;
        outStream << indentOp(indention) << "}\n";
    }
    else if (theNodeRecord->IsKind(TypeOf_JtNode_Instance))
    {
        Handle(JtNode_Instance) anInstance = Handle(JtNode_Instance)::DownCast(theNodeRecord);

        outStream << indentOp(indention) << "{\n";
        indention++;
        outStream << indentOp(indention) << "\"jt_type\":\"Instance\",\n";
        outStream << indentOp(indention) << "\"name\":\"" << theNodeRecord->Name() << "\",\n";


        HandleAttributesJson(config, indention, outStream, theNodeRecord);

        Handle(JtNode_Base) aNode = Handle(JtNode_Base)::DownCast(anInstance->Object());
        outStream << indentOp(indention) << "\"InstancedObject\":\n";
        RecurseDownTheTreeJson(config, indention, outStream, aNode, thePrefix);

        indention--;
        outStream << indentOp(indention) << "}\n";
    }
    else if (theNodeRecord->IsKind(TypeOf_JtNode_Shape_Vertex))
    {
        Handle(JtNode_Shape_Vertex) aShapeRecord = Handle(JtNode_Shape_Vertex)::DownCast(theNodeRecord);

        if (aShapeRecord->IsKind(TypeOf_JtNode_Shape_TriStripSet))
        {
            Handle(JtNode_Shape_TriStripSet) aMeshRecord = Handle(JtNode_Shape_TriStripSet)::DownCast(aShapeRecord);

            outStream << indentOp(indention) << "{\n";
            indention++;
            outStream << indentOp(indention) << "\"jt_type\":\"TriStripSet\",\n";
            outStream << indentOp(indention) << "\"name\":\"" << theNodeRecord->Name() << "\",\n";


            aMeshRecord->Bounds();
            const JtData_Object::VectorOfLateLoads& aLateLoaded = aMeshRecord->LateLoads();

            // get probably material
            HandleAttributesJson(config, indention, outStream, theNodeRecord);

            for (int i = 0; i < aLateLoaded.Count(); i++) {
                //cerr << "TriStripSet Late load SegType: " << aLateLoaded[i]->getSegmentType() << '\n';
                if (aLateLoaded[i]->DefferedObject().IsNull())
                    aLateLoaded[i]->Load();

                Handle(JtData_Object) prop = aLateLoaded[i]->DefferedObject();
                if (!prop.IsNull() && prop->IsKind(TypeOf_JtElement_ShapeLOD_Vertex)) {
                    Handle(JtElement_ShapeLOD_Vertex) aProxyMetaDataElement = Handle(JtElement_ShapeLOD_Vertex)::DownCast(prop);

                    //writeUsdMesh(aProxyMetaDataElement, "TestMesh");

                    auto indices = aProxyMetaDataElement->Indices();
                    auto vertices = aProxyMetaDataElement->Vertices();
                    auto normals = aProxyMetaDataElement->Normals();

                    outStream << indentOp(indention) << "\"NumIndices\":" << indices.Count() << ",\n";
                    outStream << indentOp(indention) << "\"NumVertices\":" << vertices.Count() << ",\n";
                    outStream << indentOp(indention) << "\"NumNormals\":" << normals.Count() << ",\n";

                    bool first = true;
                    outStream << indentOp(indention) << "\"Indices\":[";
                    if (config && dump_Mesh) {
                        first = true;
                        for (int l = 0; l < indices.Count(); l++) {
                            if (first)
                                first = false;
                            else
                                outStream << ',';
                            outStream << indices.Data()[l];

                        }
                    }
                    outStream << "],\n";

                    outStream << indentOp(indention) << "\"Vertices\":[";
                    if (config && dump_Mesh) {
                        first = true;
                        for (int l = 0; l < vertices.Count() * 3; l++) {
                            if (first)
                                first = false;
                            else
                                outStream << ',';
                            outStream << vertices.Data()[l];

                        }
                    }
                    outStream << "],\n";




                    outStream << indentOp(indention) << "\"Normals\":[";
                    if (config && dump_Mesh) {
                        first = true;
                        for (int l = 0; l < normals.Count() * 3; l++) {
                            if (first)
                                first = false;
                            else
                                outStream << ',';
                            outStream << normals.Data()[l];

                        }
                    }
                    outStream << "]\n";
                }

            }

            indention--;
            outStream << indentOp(indention) << "}\n";

            /*
            JTCommon_AABB aBox(
                Eigen::Vector4f(aMeshRecord->Bounds().MinCorner.X,
                    aMeshRecord->Bounds().MinCorner.Y,
                    aMeshRecord->Bounds().MinCorner.Z,
                    1.f),
                Eigen::Vector4f(aMeshRecord->Bounds().MaxCorner.X,
                    aMeshRecord->Bounds().MaxCorner.Y,
                    aMeshRecord->Bounds().MaxCorner.Z,
                    1.f));

            if (!mySources.Contains(aMeshRecord))
            {
                JTData_MeshNodeSourcePtr aMeshSource(
                    new JTData_MeshNodeSource(myLoadingQueue, aMeshRecord));

                // Add to new mesh source to the map (can be reused)
                mySources.Add(aMeshRecord, aMeshSource);
            }
            */

        }
        else
            cerr << "!!! '" << theNodeRecord->Name() << "' " << "TypeOf_JtNode_Shape_Vertex->Unknown \n";
    }
    else
    {
        cerr << "!!! '" << theNodeRecord->Name() << "' " << "Unknown node type \n";
    }

}


void HandleAttributesJson(dumpConfig , int& indention, std::ostream& outStream, const Handle(JtNode_Base)& theNodeRecord)
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

            outStream << indentOp(indention) << "\"Attribute_GeometricTransform\": [";
            bool first = true;
            for (int i = 0; i < 16; i++) {
                if (first)
                    first = false;
                else
                    outStream << ',';
                outStream << aTransform->GetTrsf()[i];
            }
            outStream << "],\n";

        }
        else if (anAttrib->IsKind(TypeOf_JtAttribute_Material))
        {
            Handle(JtAttribute_Material) aMaterial =
                Handle(JtAttribute_Material)::DownCast(anAttrib);

            outStream << indentOp(indention) << "\"Attribute_MaterialDiffuseColor\": [";
            bool first = true;
            for (int i = 0; i < 4; i++) {
                if (first)
                    first = false;
                else
                    outStream << ',';
                outStream << aMaterial->DiffuseColor()[i];
            }
            outStream << "],\n";

            outStream << indentOp(indention) << "\"Attribute_AmbientColor\": [";
            first = true;
            for (int i = 0; i < 4; i++) {
                if (first)
                    first = false;
                else
                    outStream << ',';
                outStream << aMaterial->AmbientColor()[i];
            }
            outStream << "],\n";

            outStream << indentOp(indention) << "\"Attribute_SpecularColor\": [";
            first = true;
            for (int i = 0; i < 4; i++) {
                if (first)
                    first = false;
                else
                    outStream << ',';
                outStream << aMaterial->SpecularColor()[i];
            }
            outStream << "],\n";

            outStream << indentOp(indention) << "\"Attribute_EmissionColor\": [";
            first = true;
            for (int i = 0; i < 4; i++) {
                if (first)
                    first = false;
                else
                    outStream << ',';
                outStream << aMaterial->EmissionColor()[i];
            }
            outStream << "],\n";

        }
        else {
            outStream << indentOp(indention) << "Unknown Attribute type \n";
        }

    }


}

void HandleAllChildrenJson(dumpConfig config, int& indention, std::ostream& outStream, const Handle(JtNode_Group)& theGroupRecord, const std::string& thePrefix)
{
    if (theGroupRecord->Children().IsEmpty())
        return;

    outStream << indentOp(indention) << "\"children\":\n";
    outStream << indentOp(indention) << "[\n";
    indention++;

    bool first = true;

    for (Standard_Integer aChildIdx = 0; aChildIdx < (Standard_Integer)theGroupRecord->Children().Count(); ++aChildIdx)
    {
        if (first)
            first = false;
        else
            outStream << indentOp(indention) << ",\n";

        Handle(JtNode_Base) aChildRecord = Handle(JtNode_Base)::DownCast(theGroupRecord->Children()[aChildIdx]);

        if (aChildRecord.IsNull())
        {
            outStream << indentOp(indention) << "{}";
            continue;
        }

        RecurseDownTheTreeJson(config, indention, outStream, aChildRecord, thePrefix);
    }

    indention--;
    outStream << indentOp(indention) << "]\n";
}