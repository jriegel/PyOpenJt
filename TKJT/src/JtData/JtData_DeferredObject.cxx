// JT format reading and visualization tools
// Copyright (C) 2014-2015 OPEN CASCADE SAS
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

#include <JtData_DeferredObject.hxx>
#include <JtData_Reader.hxx>


//=======================================================================
//function : Read
//purpose  : Read a deferred object from a JT file
//=======================================================================
bool JtData_DeferredObject::Read (JtData_Reader&         theReader,
                                              std::shared_ptr<JtData_Object>& theResult)
{
  Jt_I32 anObjectID;
  if (!theReader.ReadI32 (anObjectID))
    return false;

  theResult = std::make_shared<JtData_DeferredObject> (anObjectID);
  return true;
}

//=======================================================================
//function : Resolve
//purpose  : Replace the deferred object by the corresponding object from the given map
//=======================================================================
void JtData_DeferredObject::Resolve (std::shared_ptr<JtData_Object>& theObject,
                                     const MapOfObjects&    theObjectsMap)
{
  std::shared_ptr<JtData_DeferredObject> aDeferred = std::dynamic_pointer_cast<JtData_DeferredObject>(theObject);
  if (aDeferred)
  {
    auto anIter = theObjectsMap.find (aDeferred->myObjectID);
    if (anIter != theObjectsMap.end())
      theObject = anIter->second;
  }
}

//=======================================================================
//function : ReadVec
//purpose  : Read an array of deferred objects from a JT file
//=======================================================================
bool JtData_DeferredObject::ReadVec (JtData_Reader&   theReader,
                                                 VectorOfObjects& theResult)
{
  Jt_I32 aCount;
  if (!theReader.ReadI32 (aCount))
    return false;

  theResult.Allocate (aCount);
  for (Jt_I32 i = 0; i < aCount; i++)
  {
    if (!Read (theReader, theResult[i]))
    {
      theResult.Free();
      return false;
    }
  }

  return true;
}

//=======================================================================
//function : ResolveVec
//purpose  : Put objects from the given map instead of corresponding deferred objects
//=======================================================================
void JtData_DeferredObject::ResolveVec (VectorOfObjects&    theVector,
                                        const MapOfObjects& theObjectsMap)
{
  for (VectorOfObjects::SizeType i = 0; i < theVector.Count(); i++)
    Resolve (theVector[i], theObjectsMap);
}
