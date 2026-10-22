#include "JtUsdMesh.h"

#include <assert.h>
#include  <cctype>

#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

void writeUsdMesh(std::shared_ptr<JtElement_ShapeLOD_Vertex> vertexElement, std::string name)
{
    static int counter = 0;

    ofstream outfile(name + to_string(counter++) +".usda");


    // write header
    outfile <<
        "#usda 1.0\n"
        "(\n"
        "    defaultPrim = \"" << name << "\" \n"
        "    metersPerUnit = 0.001\n"
        "    upAxis = \"Z\" \n"
        ") \n\n";


    const auto &indices = vertexElement->Indices();
    const auto &vertices = vertexElement->Vertices();
    const auto &normals = vertexElement->Normals();

    assert(indices.Count() % 3 == 0);

    outfile <<
        "def Xform  \"" << name << "\" \n"
        "{\n";
    // write Mesh
    outfile <<
        "def Mesh  \"" << name << "_mesh" << "\" \n"
        "{\n";

    size_t triangleCnt = indices.Count() / 3;
    
    // get all unique colors
    //ColorMap colorMap;
    //mapColor(colorMap);

    // Writing 'faceVertexCounts'
    outfile << "    int[] faceVertexCounts = [";
    bool First = true;
    for (size_t tria = 0; tria < triangleCnt; tria++) {
        if (First) {
            First = false;
            outfile << '3';
        }
        else {
            outfile << ", 3";
        }
    }
    outfile << "]\n";

    // Writing Indexes
    First = true;
    outfile << "    int[] faceVertexIndices = [";
    for (size_t idx = 0; idx < indices.Count() ; idx++) {
        if (First) 
            First = false;           
        else 
            outfile << ',';
        
        outfile << indices.Data()[idx];

    }
    outfile << "]\n";

    // Writing Vertexes
    outfile << "    point3f[] points = [";
    First = true;
    for (size_t tria = 0; tria < vertices.Count(); tria++) {
        if (First) {
            First = false;
        }
        else {
            outfile << ", ";
        }
        outfile << "(" << vertices.Data()[tria * 3 + 0] << ", " << vertices.Data()[tria * 3 + 1] << ", " << vertices.Data()[tria * 3 + 2] << ")";
    }
    outfile << "]\n";

    // Writing Normals
    outfile << "    normal3f[] normals = [";
    First = true;
    for (size_t tria = 0; tria < normals.Count(); tria++) {
        if (First) {
            First = false;
        }
        else {
            outfile << ", ";
        }
        outfile << "(" << normals.Data()[tria * 3 + 0] << ", " << normals.Data()[tria * 3 + 1] << ", " << normals.Data()[tria * 3 + 2] << ")";
    }
    outfile <<
        "]\n"
        "    uniform token subdivisionScheme = \"none\"\n"
        "    uniform token subsetFamily:materialBind:familyType = \"partition\"\n";
    /*
    // write out the Section definitions for the material
    for (auto it = colorMap.begin(); it != colorMap.end(); it++) {

        // add color to return set
        colorSet.insert(it->first);

        // get the RGBA colors
        RGBA color(it->first);
        // write section definition
        outfile_layer <<
            "    def GeomSubset \"Subset_" << color.toString() << "\"\n"
            "    {\n"
            "         uniform token elementType = \"face\"\n"
            "         uniform token familyName = \"materialBind\"\n"
            "         int[] indices = [";
        // write out the indexes of triangles which has that material
        First = true;
        for (size_t idx = 0; idx < it->second.size(); idx++) {
            if (First) {
                First = false;
                outfile_layer << it->second[idx];
            }
            else {
                outfile_layer << ',' << it->second[idx];
            }
        }
        outfile_layer <<
            "]\n"
            "         rel material:binding = </CAR/General_Materials/Material_" << color.toString() << ">\n"
            "    }\n";

        part.addGeomSubset(color.toString(), color.toString());

    }
    */
    // end Mesh def
    outfile << " }\n";
    outfile << "}\n";



}
