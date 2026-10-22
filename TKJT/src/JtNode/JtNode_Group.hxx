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

#ifndef _JtNode_Group_HeaderFile
#define _JtNode_Group_HeaderFile

#include <JtNode_Base.hxx>

//! Group node contains the ordered list of children nodes (can be empty).
class JtNode_Group : public JtNode_Base
{
public:
  //! Read this entity from a JT file.
  TKJT_EXPORT bool Read (JtData_Reader& theReader) override;

  //! Dump this entity.
  TKJT_EXPORT int Dump (std::ostream& theStream) const override;

  //! Bind other objects to the object.
  TKJT_EXPORT void BindObjects (const MapOfObjects& theObjectsMap) override;

  //! Return list of children.
  const VectorOfObjects& Children() const { return myChildren; }

  DEFINE_OBJECT_CLASS (JtNode_Group)

protected:
  VectorOfObjects myChildren;
};


#endif // _JtNode_Group_HeaderFile
