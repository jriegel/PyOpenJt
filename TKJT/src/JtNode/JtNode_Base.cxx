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

#include <JtNode_Base.hxx>
#include <JtData_Reader.hxx>
#include <JtData_DeferredObject.hxx>


IMPLEMENT_OBJECT_CLASS(JtNode_Base, "Base node",
                       "10dd1035-2ac8-11d1-9b-6b-00-80-c7-bb-59-97")

//=======================================================================
//function : Read
//purpose  : Read this entity from a translate file
//=======================================================================
bool JtNode_Base::Read (JtData_Reader& theReader)
{
  if (!JtData_Object::Read (theReader))
    return false;

  Jt_I16 aVersion;
  if (theReader.Model()->MajorVersion() > 8
  && !theReader.ReadVersion (aVersion))
    return false;

  Jt_U32 aFlags;
  if (!theReader.ReadU32 (aFlags))
    return false;

  return JtData_DeferredObject::ReadVec (theReader, myAttributes);
}

//=======================================================================
//function : Dump
//purpose  :
//=======================================================================
int JtNode_Base::Dump (std::ostream& theStream) const
{
  theStream << "JtNode_Base ";
  if (!myAttributes.IsEmpty())
    theStream << "{ A:" << myAttributes.Count() << " } ";
  return JtData_Object::Dump (theStream);
}

//=======================================================================
//function : BindObjects
//purpose  : Bind other objects to the object
//=======================================================================
void JtNode_Base::BindObjects (const MapOfObjects& theObjectsMap)
{
  JtData_DeferredObject::ResolveVec (myAttributes, theObjectsMap);
}

//=======================================================================
//function : BindName
//purpose  : Bind a name to the object
//=======================================================================
void JtNode_Base::BindName (const std::wstring& theName)
{
  // keep only the first ';'-separated token
  const size_t aStart = theName.find_first_not_of (L';');
  if (aStart == std::wstring::npos)
  {
    myName.clear();
    return;
  }
  myName = theName.substr (aStart, theName.find (L';', aStart) - aStart);
}
