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

#ifndef _JtProperty_LateLoaded_HeaderFile
#define _JtProperty_LateLoaded_HeaderFile

#include <JtProperty_Base.hxx>
#include <JtData_Model.hxx>


enum SegmentType {
	Logical_Scene_Graph = 1,
	JT_BRep = 2,
	PMI_Data = 3,
	Meta_Data = 4,
	Shape = 6,
	Shape_LOD0 = 7,
	Shape_LOD1 = 8,
	Shape_LOD2 = 9,
	Shape_LOD3 = 10,
	Shape_LOD4 = 11,
	Shape_LOD5 = 12,
	Shape_LOD6 = 13,
	Shape_LOD7 = 14,
	Shape_LOD8 = 15,
	Shape_LOD9 = 16
};

class JtProperty_LateLoaded : public JtProperty_Base
{
public:
  //! Read this entity from a JT file.
  Standard_EXPORT Standard_Boolean Read (JtData_Reader& theReader) Standard_OVERRIDE;

  //! Dump this entity.
  Standard_EXPORT Standard_Integer Dump (Standard_OStream& theStream) const Standard_OVERRIDE;

  //! Load object from the referenced JT file segment.
  void Load() { myDefferedObject = mySegModel->ReadSegment(mySegOffset); }
  
  //! Load object from the referenced JT file segment.
  void LoadDump() { myDefferedObject = mySegModel->ReadSegmentDump(mySegOffset); }

  Handle(JtData_Object) DefferedObject() { return myDefferedObject; }

  void Unload() { myDefferedObject.Nullify(); }

  //! returns the type of the late load segment as specified in Segment Type Table in the standard
  Jt_I32 getSegmentType() const { return mySegmentType; }


  DEFINE_STANDARD_RTTIEXT(JtProperty_LateLoaded,JtProperty_Base)
  DEFINE_OBJECT_CLASS (JtProperty_LateLoaded)

protected:
  Handle(JtData_Model) mySegModel;
  Jt_U64               mySegOffset;
  Jt_I32			   mySegmentType;

  Handle(JtData_Object) myDefferedObject;
};

DEFINE_STANDARD_HANDLE(JtProperty_LateLoaded, JtProperty_Base)

#endif // _JtProperty_LateLoaded_HeaderFile
