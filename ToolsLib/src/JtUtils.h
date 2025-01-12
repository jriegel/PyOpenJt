#pragma once
/**
 * \file JtUtils.h
 * \date 31.12.2024
 * \author: Juergen Riegel <mail@juergen-riegel.net>
 * \copyright 2024 GPL 2.0
 * \details This file holds common tool classes 
 */


#include <vector>

#include <JtData_Object.hxx>

const std::vector<char> HandleLateLoadsMeta(const JtData_Object::VectorOfLateLoads& aLateLoaded);
