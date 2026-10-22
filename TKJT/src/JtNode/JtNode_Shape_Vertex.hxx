// JT format reading and visualization tools
// Copyright (C) 2013-2015 OPEN CASCADE SAS
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

#ifndef _JtNode_Shape_Vertex_HeaderFile
#define _JtNode_Shape_Vertex_HeaderFile

#include <JtNode_Shape_Base.hxx>

//! Vertex Shape Node Element defines shapes defined by collections of vertices.
class JtNode_Shape_Vertex : public JtNode_Shape_Base
{
public:
  //! Read this entity from a JT file.
  TKJT_EXPORT bool Read (JtData_Reader &theReader) override;

  //! Dump this entity.
  TKJT_EXPORT int Dump (std::ostream& S) const override;

  DEFINE_OBJECT_CLASS (JtNode_Shape_Vertex)
};


#endif // _JtNode_Shape_Vertex_HeaderFile
