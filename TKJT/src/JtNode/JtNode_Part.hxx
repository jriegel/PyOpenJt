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

#ifndef _JtNode_Part_HeaderFile
#define _JtNode_Part_HeaderFile

#include <JtNode_MetaData.hxx>
#include <JtProperty_LateLoaded.hxx>

//! Part Node Element represent the particular part in LSG structure.
class JtNode_Part : public JtNode_MetaData
{
public:
  //! Read this entity from a JT file.
  Standard_EXPORT Standard_Boolean Read (JtData_Reader& theReader) Standard_OVERRIDE;

  //! Dump this entity.
  Standard_EXPORT Standard_Integer Dump (Standard_OStream& theStream) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(JtNode_Part,JtNode_MetaData)
  DEFINE_OBJECT_CLASS (JtNode_Part)


};

DEFINE_STANDARD_HANDLE(JtNode_Part, JtNode_MetaData)

#endif // _JtNode_Part_HeaderFile
