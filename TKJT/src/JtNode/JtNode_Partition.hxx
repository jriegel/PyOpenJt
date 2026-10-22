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

#ifndef _JtNode_Partition_HeaderFile
#define _JtNode_Partition_HeaderFile

#include <JtNode_Group.hxx>
#include <JtData_Model.hxx>

#include <string>

//! Leaf node representing the external JT file reference.
class JtNode_Partition : public JtNode_Group
{
public:
  //! Read this entity from a JT file.
  TKJT_EXPORT bool Read (JtData_Reader& theReader) override;

  //! Dump this entity.
  TKJT_EXPORT int Dump (std::ostream& theStream) const override;

  //! Load the referenced JT file and bind its LSG to this node.
  TKJT_EXPORT bool Load();

  //! Unbind children from this node and unload the referenced JT file.
  TKJT_EXPORT void Unload();

  //! Returns file name.
  const std::wstring& FileName() const { return myFileName; }

  DEFINE_OBJECT_CLASS (JtNode_Partition)

protected:
  std::shared_ptr<JtData_Model>       myModel;
  std::wstring myFileName;
};


#endif // _JtNode_Partition_HeaderFile
