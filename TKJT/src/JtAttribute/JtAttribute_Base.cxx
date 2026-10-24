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

#include <JtAttribute_Base.hxx>
#include <JtData_Reader.hxx>


//=======================================================================
//function : JtAttribute_Base
//purpose  :
//=======================================================================
JtAttribute_Base::JtAttribute_Base()
: myVersion           (0)
, myStateFlags        (0)
, myFieldInhibitFlags (0) {}

//=======================================================================
//function : Read
//purpose  : Read this entity from a translate file
//=======================================================================
bool JtAttribute_Base::Read (JtData_Reader& theReader)
{
  if (!JtData_Object::Read (theReader))
    return false;

  if (theReader.Model()->MajorVersion() > 8
  && !theReader.ReadVersion (myVersion))
    return false;

  if (!theReader.ReadU8  (myStateFlags)
   || !theReader.ReadU32 (myFieldInhibitFlags))
    return false;

  if (theReader.Model()->MajorVersion() >= 10)
  {
    Jt_U32 aFieldFinalFlags;
    return theReader.ReadU32 (aFieldFinalFlags);
  }
  return true;
}

//=======================================================================
//function : Dump
//purpose  :
//=======================================================================
int JtAttribute_Base::Dump (std::ostream& theStream) const
{
  theStream << "JtAttribute_Base ";
  return JtData_Object::Dump (theStream);
}
