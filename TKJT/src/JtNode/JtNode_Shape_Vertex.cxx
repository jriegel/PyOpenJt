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

#include <JtNode_Shape_Vertex.hxx>
#include <JtData_Reader.hxx>

#include <JtElement_ShapeLOD_Vertex.hxx>


IMPLEMENT_OBJECT_CLASS(JtNode_Shape_Vertex, "Vertex Shape Object",
                       "10dd107f-2ac8-11d1-6b-9b-00-80-c7-bb-59-97")

//=======================================================================
//function : Read
//purpose  : Read this entity from a translate file
//=======================================================================
bool JtNode_Shape_Vertex::Read (JtData_Reader &theReader)
{
  if (!JtNode_Shape_Base::Read (theReader))
    return false;

  Jt_I16 aVersion = 1;

  if (theReader.Model()->MajorVersion() >= 10)
  {
    // JT 10.x: version and vertex binding only, no quantization parameters
    JtElement_ShapeLOD_Vertex::VertexBinding2 aVertexBinding;
    return theReader.ReadVersion (aVersion)
        && aVertexBinding.Read (theReader);
  }

  if (theReader.Model()->MajorVersion() < 9)
  {
    Jt_I32 aNormalBinding;
    Jt_I32 aTextCoordBinding;
    Jt_I32 aColorBinding;
    if (!theReader.ReadI32 (aNormalBinding)
     || !theReader.ReadI32 (aTextCoordBinding)
     || !theReader.ReadI32 (aColorBinding))
      return false;
  }
  else
  {
    JtElement_ShapeLOD_Vertex::VertexBinding2 aVertexBinding;
    if (!theReader.ReadI16 (aVersion)
     || !aVertexBinding.Read (theReader))
      return false;
  }

  JtElement_ShapeLOD_Vertex::QuantizationParams aQuantizationParams;
  if (!aQuantizationParams.Read (theReader))
    return false;

  if (aVersion != 1)
  {
    JtElement_ShapeLOD_Vertex::VertexBinding2 aVertexBinding;
    if (!aVertexBinding.Read (theReader))
      return false;
  }

  return true;
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
int JtNode_Shape_Vertex::Dump (std::ostream &theStream) const
{
  theStream << "JtNode_Shape_Vertex ";
  return JtNode_Shape_Base::Dump (theStream);
}
