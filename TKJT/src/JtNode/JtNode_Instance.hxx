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

#ifndef _JtNode_Instance_HeaderFile
#define _JtNode_Instance_HeaderFile

#include <JtNode_Base.hxx>

//! Instance Node Element stores a single reference to another node. It is 
//! intended to provide sharing of nodes and assignment of instance-specific
//! attributes. Instances can not contain references to themselves.
class JtNode_Instance : public JtNode_Base
{
public:
  //! Read this entity from a JT file.
  Standard_EXPORT Standard_Boolean Read (JtData_Reader& theReader) Standard_OVERRIDE;

  //! Dump this entity.
  Standard_EXPORT Standard_Integer Dump (Standard_OStream& theStream) const Standard_OVERRIDE;

  //! Bind other objects to the object.
  Standard_EXPORT void BindObjects (const MapOfObjects& theObjectsMap) Standard_OVERRIDE;

  //! Return referenced object.
  const Handle(JtData_Object)& Object() const { return myObject; }

  DEFINE_STANDARD_RTTIEXT(JtNode_Instance,JtNode_Base)
  DEFINE_OBJECT_CLASS (JtNode_Instance)

protected:
  Handle(JtData_Object) myObject;
};

DEFINE_STANDARD_HANDLE(JtNode_Instance, JtNode_Base)

#endif
