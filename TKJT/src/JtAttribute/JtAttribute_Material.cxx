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

#include <JtAttribute_Material.hxx>
#include <JtData_Reader.hxx>

IMPLEMENT_STANDARD_RTTIEXT(JtAttribute_Material, JtAttribute_Base)

IMPLEMENT_OBJECT_CLASS(JtAttribute_Material, "Material Object",
                       "10dd1030-2ac8-11d1-9b-6b-00-80-c7-bb-59-97")

namespace
{
  //! Auxiliary function
  static bool readColor (JtData_Reader&   theReader,
                                     Jt_F32         (&theVector) [4],
                                     bool theIsSingleF32 = false)
  {
    if (!theIsSingleF32)
      return theReader.ReadArray (theVector);

    Jt_F32 aVal;
    if (!theReader.ReadF32 (aVal))
      return false;

    theVector[0] = aVal;
    theVector[1] = aVal;
    theVector[2] = aVal;
    theVector[3] = 1.0;

    return true;
  }
}

//=======================================================================
//function : JtAttribute_Material
//purpose  :
//=======================================================================
JtAttribute_Material::JtAttribute_Material()
  : myVersion (0) {}

//=======================================================================
//function : Read
//purpose  : Read this entity from a translate file
//=======================================================================
bool JtAttribute_Material::Read (JtData_Reader& theReader)
{
  if (!JtAttribute_Base::Read (theReader))
    return false;

  if (theReader.Model()->MajorVersion() > 8
  && !theReader.ReadI16 (myVersion))
    return false;

  if (!theReader.ReadU16 (myDataFlags)
   || !readColor (theReader, myAmbientColor , (myDataFlags & 3) == 3)
   || !readColor (theReader, myDiffuseColor)
   || !readColor (theReader, mySpecularColor, (myDataFlags & 5) == 5)
   || !readColor (theReader, myEmissionColor, (myDataFlags & 9) == 9)
   || !theReader.ReadF32 (myShininess))
    return false;

  if (myVersion == 2
  && !theReader.ReadF32 (myReflectivity))
    return false;

  return true;
}

//=======================================================================
//function : Dump
//purpose  :
//=======================================================================
int JtAttribute_Material::Dump (std::ostream& theStream) const
{
  theStream << "JtAttribute_Material ";
  return JtAttribute_Base::Dump (theStream);
}
