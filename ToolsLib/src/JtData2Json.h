#pragma once
/**
 * \file JtData2Json.h
 * \date 20.2.2024
 * \author: Juergen Riegel <mail@juergen-riegel.net>
 * \copyright 2024 GPL 2.0
 * \details This file holds the interface to tools createing JSON from JtData_... classes
 */

#include <JtData_Model.hxx>
#include <JtNode_MetaData.hxx>

#include <ostream>
#include <vector>

std::ostream& operator<<(std::ostream& os, const TCollection_ExtendedString& extstr);

enum dumpConfig {
	dump_TOC = 1,
	dump_LOD0 = 2,
	dump_Mesh = 4,
	dump_all  = INT_MAX
};

int writeModel(Handle(JtData_Model) model, std::ostream& out, int indent = 0, int config = 0);

//int writePart(Handle(JtNode_Partition) model, std::ostream& out, int indent = 0, int config = 0);

//int writeMetaData(Handle(JtNode_MetaData) model, std::ostream& out, int indent = 0, int config = 0);


int writeKeyValueStream(const std::vector<char> &stream, std::ostream& out, int indent = 0);


struct indentOp {
	indentOp(int level) : level(level) {}
private:
	friend std::ostream& operator<<(std::ostream& stream, const indentOp& val);
	int level;
};

inline std::ostream& operator<<(std::ostream& stream, const indentOp& val) {
	for (int i = 0; i < val.level; i++)
		stream << '\t';
	return stream;
}

void WriteMeshesWithoutLSG(dumpConfig config, std::ostream& outStream, Handle(JtData_Model) rootModel, const std::string& fileName);

void RecurseDownTheTree(dumpConfig config, int& indention, std::ostream& outStream, const Handle(JtNode_Base)& theNodeRecord, const std::string& thePrefix);

void HandleAttributes(dumpConfig config, int& indention, std::ostream& outStream, const Handle(JtNode_Base)& theNodeRecord);

void HandleAllChildren(dumpConfig config, int& indention, std::ostream& outStream, const Handle(JtNode_Group)& theGroupRecord, const std::string& thePrefix);


