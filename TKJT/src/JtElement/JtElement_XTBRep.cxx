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

#include <JtElement_XTBRep.hxx>
#include <JtData_Reader.hxx>



IMPLEMENT_OBJECT_CLASS(JtElement_XTBRep, "XT B-Rep Object",
                       "873a70e0-2ac9-11d1-9b-6b-00-80-c7-bb-59-97")

//=======================================================================
//function : Read
//purpose  : Read this entity from a translate file
//=======================================================================
bool JtElement_XTBRep::Read (JtData_Reader& theReader)
{
  if (!JtData_Object::Read (theReader)
   || !theReader.ReadI32 (myVersion)
   || !theReader.ReadI32 (myPKMajorVersion)
   || !theReader.ReadI32 (myPKMinorVersion))
  {
    return false;
  }

  if (theReader.Model()->MajorVersion() > 8
  && !theReader.ReadI32 (myPKBuild))
  {
    return false;
  }

  return theReader.ReadVec (myData);
}

//=======================================================================
//function : Dump
//purpose  :
//=======================================================================
int JtElement_XTBRep::Dump (std::ostream& theStream) const
{
  theStream << "JtElement_XTBRep { " << myPKMajorVersion << "." << myPKMinorVersion << "." << myPKBuild << " } ";
  return JtData_Object::Dump (theStream);
}
