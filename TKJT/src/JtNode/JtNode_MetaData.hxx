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

#ifndef _JtNode_MetaData_HeaderFile
#define _JtNode_MetaData_HeaderFile

#include <JtNode_Group.hxx>

//! Meta Data Node Element is used for storing references to specific meta-data
//! defined in dedicated segment of Jt file.
class JtNode_MetaData : public JtNode_Group
{
public:
  //! Read this entity from a JT file.
  TKJT_EXPORT bool Read (JtData_Reader& theReader) override;

  //! Dump this entity.
  TKJT_EXPORT int Dump (std::ostream& theStream) const override;

  //! Bind a late loaded property to the object.
  TKJT_EXPORT void BindLateLoads(const ListOfLateLoads& theLateLoads) override;

  //! Get the late loaded properties associated with this node.
  const VectorOfLateLoads& LateLoads() const { return myLateLoads; }


  DEFINE_STANDARD_RTTIEXT(JtNode_MetaData,JtNode_Group)
  DEFINE_OBJECT_CLASS (JtNode_MetaData)

protected:
	VectorOfLateLoads myLateLoads;

};

DEFINE_STANDARD_HANDLE(JtNode_MetaData, JtNode_Group)

#endif // _JtNode_MetaData_HeaderFile
