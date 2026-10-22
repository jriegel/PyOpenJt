// JT format reading and visualization tools
// Copyright (C) 2014-2015 OPEN CASCADE SAS
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2 of the License, or any later
// version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// Copy of the GNU General Public License is in LICENSE.txt and  
// on <http://www.gnu.org/licenses/>.

#ifndef _JtData_Message_HeaderFile
#define _JtData_Message_HeaderFile

#include <iostream>

#ifdef TKJT_CERRTRACING
    #define TRACE(msg)   /*std::cerr << "TRACE: " << msg << '\n'*/;
    #define INFO(msg)    /*std::cerr << "INFO: " << msg << '\n'*/;
    #define WARNING(msg) std::cerr << "WARNING: " << msg << '\n';
    #define ALARM(msg)   std::cerr << "ALARM: " << msg << '\n';
    #define FAIL(msg)    std::cerr << "FAIL: " << msg << '\n';

#else
    #define TRACE(msg)
    #define INFO(msg)
    #define WARNING(msg)
    #define ALARM(msg)
    #define FAIL(msg)
#endif
#endif // _JtData_Message_HeaderFile
