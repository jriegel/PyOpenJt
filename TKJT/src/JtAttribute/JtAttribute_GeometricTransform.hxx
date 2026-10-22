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

#ifndef _JtAttribute_GeometricTransform_HeaderFile
#define _JtAttribute_GeometricTransform_HeaderFile

#include <JtAttribute_Base.hxx>

#include <Eigen/Core>

//! Class representing Jt transformation attribute.
class JtAttribute_GeometricTransform : public JtAttribute_Base
{
public:
  //! Read this entity from a JT file.
  TKJT_EXPORT bool Read (JtData_Reader& theReader) override;

  //! Dump this entity.
  TKJT_EXPORT int Dump (std::ostream& theStream) const override;

  //! Return transformation as 4x4 matrix (translation in the last column).
  Eigen::Matrix4d GetMatrix() const { return Eigen::Map<const Eigen::Matrix4d> (myTransform); }

  //! Return transformationy.
  const double* GetTrsf() const { return myTransform; }

  DEFINE_OBJECT_CLASS (JtAttribute_GeometricTransform)

protected:
  double myTransform[16];
};


#endif // _JtAttribute_GeometricTransform_HeaderFile
