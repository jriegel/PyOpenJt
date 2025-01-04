#pragma once
/**
 * \file JtLayer.h
 * \date 20.2.2024
 * \author: Juergen Riegel <mail@juergen-riegel.net>
 * \copyright 2024 GPL 2.0
 * \details This file holds the interface to tools createing JSON from JtData_... classes
 */

#include <JtElement_ShapeLOD_Vertex.hxx>
#include <JtData_Model.hxx>
#include <JtNode_MetaData.hxx>

#include <string>


void writeTestGltfMesh(std::string name);


void RecurseDownTheTreeGlTf(const Handle(JtNode_Base)& theNodeRecord, const std::string& thePrefix);

