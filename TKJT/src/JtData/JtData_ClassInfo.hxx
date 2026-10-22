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

#ifndef _JtData_ClassInfo_HeaderFile
#define _JtData_ClassInfo_HeaderFile

#include <JtData_Types.hxx>

#include <memory>
#include <unordered_map>

template <class RootClass>
class JtData_ClassInfo
{
public:
  static const JtData_ClassInfo* Find (const Jt_GUID& theGUID)
  {
    auto anIter = JtData_ClassInfo<RootClass>::DerivedClasses().find (theGUID);
    if (anIter != JtData_ClassInfo<RootClass>::DerivedClasses().end())
      return anIter->second;
    else
      return nullptr;
  }

  virtual std::shared_ptr<RootClass> Create() const = 0;
  virtual const char* Name()   const = 0;

  // D-tor to prevent gcc warnings
  virtual ~JtData_ClassInfo() {};

protected:
  typedef std::unordered_map<Jt_GUID, const JtData_ClassInfo*> MapOfClasses;
  static MapOfClasses& DerivedClasses()
  {
    static MapOfClasses aMap;
    return aMap;
  }
};

template <class Class, class RootClass>
class JtData_ClassInfoT : public JtData_ClassInfo<RootClass>
{
  const char* myName;

public:
  JtData_ClassInfoT (const Jt_GUID& theGUID, const char* theName,
                     bool theRegister = false)
  : myName (theName)
  {
    if (theRegister)
      this->DerivedClasses()[theGUID] = this;
  }

  virtual std::shared_ptr<RootClass> Create() const { return std::make_shared<Class>(); }
  virtual const char* Name()   const { return myName; }
};

#define DEFINE_CLASS_INFO(theClass, theRootClass) \
  static JtData_ClassInfoT<theClass, theRootClass> ClassInfo;

#define IMPLEMENT_CLASS_INFO(theClass, theRootClass, theName, theGUID) \
  JtData_ClassInfoT<theClass, theRootClass> theClass::ClassInfo (theGUID, theName, true);

#endif // _JtData_ClassInfo_HeaderFile
