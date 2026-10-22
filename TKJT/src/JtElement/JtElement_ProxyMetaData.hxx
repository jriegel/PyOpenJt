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
  TKJT_EXPORT bool Read (JtData_Reader& theReader) override;

  //! Dump this entity.
  TKJT_EXPORT int Dump (std::ostream& theStream) const override;

  const std::vector<char>& getKeyValueStream() { return keyValueStream; }

  DEFINE_OBJECT_CLASS (JtElement_ProxyMetaData)

private:
	std::vector<char> keyValueStream = std::vector<char>(2048,'\0');
};


#endif // _JtElement_ProxyMetaData_HeaderFile
