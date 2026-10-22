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

#include <JtNode_Part.hxx>
#include <JtData_Reader.hxx>

IMPLEMENT_STANDARD_RTTIEXT(JtNode_Part, JtNode_MetaData)

IMPLEMENT_OBJECT_CLASS(JtNode_Part, "Part Object",
                       "ce357244-38fb-11d1-a5-06-00-60-97-bd-c6-e1")

//=======================================================================
//function : Read
//purpose  : Read this entity from a translate file
//=======================================================================
bool JtNode_Part::Read (JtData_Reader& theReader)
{
  Jt_I16 aVersion;
  Jt_I32 aReserved;
  return JtNode_MetaData::Read (theReader)
      && theReader.ReadI16 (aVersion)
      && theReader.ReadI32 (aReserved);
}

//=======================================================================
//function : Dump
//purpose  :
//=======================================================================
int JtNode_Part::Dump (std::ostream& theStream) const
{
  theStream << "JtNode_Part ";
  return JtNode_MetaData::Dump (theStream);
}

