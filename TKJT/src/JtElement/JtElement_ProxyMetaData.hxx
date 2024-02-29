// JT format reading and visualization tools
// Copyright (C) 2024 Juergen Riegel
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

#ifndef _JtElement_ProxyMetaData_HeaderFile
#define _JtElement_ProxyMetaData_HeaderFile

#include <JtData_Object.hxx>

#include <vector>

//! Base Shape LOD Element is the general representation for LODs. It is a 
//! container of the geometric definition data (vertices, polygons, 
//! normals, etc.).
class JtElement_ProxyMetaData : public JtData_Object
{
public:
  //! Read this entity from a JT file.
  Standard_EXPORT Standard_Boolean Read (JtData_Reader& theReader) Standard_OVERRIDE;

  //! Dump this entity.
  Standard_EXPORT Standard_Integer Dump (Standard_OStream& theStream) const Standard_OVERRIDE;

  const std::vector<char>& getKeyValueStream() { return keyValueStream; }

  DEFINE_STANDARD_RTTIEXT(JtElement_ProxyMetaData,JtData_Object)
  DEFINE_OBJECT_CLASS (JtElement_ProxyMetaData)

private:
	std::vector<char> keyValueStream = std::vector<char>(2048,'\0');
};

DEFINE_STANDARD_HANDLE(JtElement_ProxyMetaData, JtData_Object)

#endif // _JtElement_ProxyMetaData_HeaderFile
