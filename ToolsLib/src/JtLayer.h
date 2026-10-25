#pragma once
/**
 * \file JtLayer.h
 * \date 20.2.2024
 * \author: Juergen Riegel <mail@juergen-riegel.net>
 * \copyright 2024 GPL 2.0
 * \details This file holds the interface to tools createing JSON from JtData_... classes
 */


#include <cstdint>
#include <vector>
#include <string>
#include <map>

struct LayerInfo 
{
	std::string ActiveLayerFilter;
	std::map<std::string, std::vector<uint32_t>> LayerMap;
};

LayerInfo ScanForLayerFilter(const std::vector<char>& stream);

std::vector<uint32_t> ScanForLayer(const std::vector<char>& stream);