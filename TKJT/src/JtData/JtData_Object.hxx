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

#ifndef _JtData_Object_HeaderFile
#define _JtData_Object_HeaderFile

#include <memory>
#include <unordered_map>
#include <vector>
#include <string>

#include <JtData_Vector.hxx>
#include <JtData_ClassInfo.hxx>

class JtData_Reader;
class JtProperty_LateLoaded;


//! The base class representing Jt entity.
class JtData_Object
{
public:
  virtual ~JtData_Object() = default;

  typedef JtData_ClassInfo <JtData_Object> ClassInfo;

  //! Return class by Jt Object type ID.
  //! @param theGUID - Jt object's type GUID.
  static const ClassInfo* FindClass (const Jt_GUID& theGUID) { return ClassInfo::Find (theGUID); }

  typedef std::unordered_map <int, std::shared_ptr<JtData_Object>>               MapOfObjects;
  typedef JtData_Vector       <std::shared_ptr<JtData_Object>>                   VectorOfObjects;
  typedef std::vector         <std::shared_ptr<JtProperty_LateLoaded>>           ListOfLateLoads;
  typedef JtData_Vector       <std::shared_ptr<JtProperty_LateLoaded>>           VectorOfLateLoads;

public:
  //! Read this entity from a JT file.
  //! @param theReader - current Jt file reader.
  TKJT_EXPORT virtual bool Read (JtData_Reader& theReader);

  //! Dump this entity.
  TKJT_EXPORT virtual int Dump (std::ostream& S) const;

  //! Bind other objects to the object.
  TKJT_EXPORT virtual void BindObjects (const MapOfObjects& theObjectsMap);

  //! Bind a late loaded property to the object.
  TKJT_EXPORT virtual void BindLateLoads (const ListOfLateLoads& theLateLoads);

  //! Bind a name to the object.
  TKJT_EXPORT virtual void BindName (const std::wstring& theName);

};

#define DEFINE_OBJECT_CLASS(Class) \
  DEFINE_CLASS_INFO(Class, JtData_Object)

#define IMPLEMENT_OBJECT_CLASS(Class, Name, GUID) \
  IMPLEMENT_CLASS_INFO(Class, JtData_Object, Name, GUID)

#endif // _JtData_Object_HeaderFile
