/**
 * \file JtDump_Main.cpp
 * \date 20.2.2024
 * \author: Juergen Riegel <mail@juergen-riegel.net>
 * \copyright 2024 GPL 2.0
 * \details This file holds the executable definition to dump Jt information as JSON
 */

#include <CLI/CLI.hpp>

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
#include <regex>

using namespace std;

std::ostream* outStream = &(std::cout);
int indention = 0;
int outFilter = dumpConfig::dump_all;
//int meshData = 1;

int main(int argc, char* argv[])
{
	CLI::App app("JtDump - dumps information of a Jt file as JSON");

	// add version output
	app.set_version_flag("--version", std::string("0.8"));

	std::vector<std::string> files;
	app.add_option("jt-files", files, "Jt file(s) to dump")->check(CLI::ExistingFile);
    std::string outFileName;
    app.add_option("-o", outFileName, "Out File Name");
    //app.add_option("-m", meshData, "Mesh Data mode (0=none)");

	CLI11_PARSE(app, argc, argv);

	if (files.size() == 0) {

		cerr << "No input Jt files given!" << endl;
		return 1;
	}

    std::ofstream outFile(outFileName.c_str());
    if (outFile.is_open()) {
        outStream = &outFile;
    }

    if (files.size() > 1) {
        *outStream << '[' << endl;
        indention++;
    }

    bool first = true;

	// go through Jt files
	for (const auto& aarg : files) {

        string fileName = regex_replace(aarg, std::regex("\\\\"), "/"); // replace 'def' -> 'klm'

        if (files.size() > 1) {
            if (first)
                first = false;
            else
                *outStream << ',';
        }

        *outStream << '{' << endl;
        indention++;


		TCollection_ExtendedString aFileName(fileName.c_str(), Standard_True);


		Handle(JtData_Model) rootModel =   new JtData_Model(aFileName);


		if (rootModel.IsNull())
			return 2;

		Handle(JtNode_Partition) PartitionNode = rootModel->Init();

    
        writeModel(rootModel, *outStream, indention, outFilter);

        if (PartitionNode.IsNull()) {
            WriteMeshesWithoutLSG(dumpConfig::dump_all ,*outStream, rootModel, fileName);
        }
        else {
            RecurseDownTheTree(dumpConfig::dump_all, indention, *outStream, PartitionNode, fileName);
        }

        indention--;
        *outStream << '}' << endl;
		
	}

    if (files.size() > 1) {
        indention--;
        *outStream << ']' << endl;
    }

    assert(indention == 0);

	return 0;
}


