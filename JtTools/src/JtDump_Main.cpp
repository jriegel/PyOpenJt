/**
 * \file JtDump_Main.cpp
 * \date 20.2.2024
 * \author: Juergen Riegel <mail@juergen-riegel.net>
 * \copyright 2024 GPL 2.0
 * \details This file holds the executable definition to dump Jt information as JSON
 */

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

#include <JtData2Json.h>
#include <JtLayer.h>
#include <JtUsdMesh.h>
#include <JtUtils.h>

#include <iostream>
#include <iomanip>
#include <assert.h>
#include <fstream>
#include <regex>
#include <string>
#include <vector>

using namespace std;

std::ostream* outStream = &(std::cout);
int indention = 0;
int outFilter = dumpConfig::dump_all;
//int meshData = 1;

int main(int argc, char* argv[])
{
    const std::string version = "0.8";
    std::vector<std::string> files;
    std::string outFileName;
    bool showVersion = false;
    bool showHelp = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--version") {
            showVersion = true;
        }
        else if (arg == "-h" || arg == "--help") {
            showHelp = true;
        }
        else if (arg == "-o") {
            if (i + 1 >= argc) {
                std::cerr << "Missing value for -o" << std::endl;
                return 1;
            }
            outFileName = argv[++i];
        }
        else if (arg.rfind('-', 0) == 0) {
            std::cerr << "Unknown option: " << arg << std::endl;
            return 1;
        }
        else {
            files.push_back(arg);
        }
    }

    if (showVersion) {
        std::cout << version << std::endl;
        return 0;
    }

    if (showHelp) {
        std::cout << "Usage: JtDump [options] <jt-file> [<jt-file> ...]" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  -o <file>   Write JSON output to the specified file" << std::endl;
        std::cout << "  -h, --help  Show this help message" << std::endl;
        std::cout << "  --version   Show the program version" << std::endl;
        return 0;
    }

    if (files.empty()) {
        std::cerr << "No input Jt files given!" << std::endl;
        return 1;
    }

    std::ofstream outFile(outFileName.c_str());
    if (outFile.is_open()) {
        outStream = &outFile;
    }

    if (files.size() > 1) {
        *outStream << '[' << std::endl;
        indention++;
    }

    bool first = true;

    // go through Jt files
    for (const auto& aarg : files) {
        std::string fileName = std::regex_replace(aarg, std::regex("\\\\"), "/");

        if (files.size() > 1) {
            if (first)
                first = false;
            else
                *outStream << ',';
        }

        *outStream << '{' << std::endl;
        indention++;

        TCollection_ExtendedString aFileName(fileName.c_str());

        Handle(JtData_Model) rootModel = new JtData_Model(aFileName);

        if (rootModel.IsNull())
            return 2;

        Handle(JtNode_Partition) PartitionNode = rootModel->Init();

        writeModel(rootModel, *outStream, indention, outFilter);

        if (PartitionNode.IsNull()) {
            WriteMeshesWithoutLSG(dumpConfig::dump_all, *outStream, rootModel, fileName);
        }
        else {
            RecurseDownTheTreeJson(dumpConfig::dump_all, indention, *outStream, PartitionNode, fileName);
        }

        indention--;
        *outStream << '}' << std::endl;
    }

    if (files.size() > 1) {
        indention--;
        *outStream << ']' << std::endl;
    }

    assert(indention == 0);

    return 0;
}


