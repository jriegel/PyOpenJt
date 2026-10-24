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

#include <JtNode_Shape_Base.hxx>
#include <JtData_Reader.hxx>


IMPLEMENT_OBJECT_CLASS(JtNode_Shape_Base, "Base Shape Object",
                       "10dd1059-2ac8-11d1-9b-6b-00-80-c7-bb-59-97")

//=======================================================================
//function : Read
//purpose  : Read this entity from a translate file
//=======================================================================
bool JtNode_Shape_Base::Read (JtData_Reader& theReader)
{
  if (!JtNode_Base::Read (theReader))
    return false;

  Jt_I16 aVersion;
  if (theReader.Model()->MajorVersion() > 8
  && !theReader.ReadVersion (aVersion))
    return false;

  if (theReader.Model()->MajorVersion() >= 10)
  {
    // JT 10.x stores a single bounding box
    return theReader.ReadUniformStruct<Jt_F32> (myReservedBnd)
        && theReader.ReadF32   (myArea)
        && theReader.ReadArray (myVertexRange)
        && theReader.ReadArray (myNodeRange)
        && theReader.ReadArray (myPolyRange)
        && theReader.ReadI32   (mySize)
        && theReader.ReadF32   (myCompression);
  }

  return theReader.ReadUniformStruct<Jt_F32> (myReservedBnd)
      && theReader.ReadUniformStruct<Jt_F32> (myUntransBnd)
      && theReader.ReadF32   (myArea)
      && theReader.ReadArray (myVertexRange)
      && theReader.ReadArray (myNodeRange)
      && theReader.ReadArray (myPolyRange)
      && theReader.ReadI32   (mySize)
      && theReader.ReadF32   (myCompression);
}

//=======================================================================
//function : Dump
//purpose  :
//=======================================================================
int JtNode_Shape_Base::Dump (std::ostream& theStream) const
{
  theStream << "JtNode_Shape_Base ";
  return JtNode_Base::Dump (theStream);
}
