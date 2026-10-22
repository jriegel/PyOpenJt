// JT format reading and visualization tools
// Copyright (C) 2001-2015 OPEN CASCADE SAS
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

#ifndef _JtElement_XTBRep_HeaderFile
#define _JtElement_XTBRep_HeaderFile

#include <JtData_Object.hxx>

class JtElement_XTBRep : public JtData_Object
{
public:
  //! Default constructor.
  JtElement_XTBRep() : myVersion (0), myPKMajorVersion (0), myPKMinorVersion (0), myPKBuild (0) {}

  //! Read this entity from a JT file.
  TKJT_EXPORT bool Read (JtData_Reader &theReader) override;

  //! Dump this entity.
  TKJT_EXPORT int Dump (std::ostream& S) const override;

  const Jt_String& Data() const { return myData; }

  void ClearData() { myData.Free(); }

  DEFINE_OBJECT_CLASS (JtElement_XTBRep)

protected:
  Jt_I32 myVersion;
  Jt_I32 myPKMajorVersion; //!< Parasolid Kernel Major Version
  Jt_I32 myPKMinorVersion; //!< Parasolid Kernel Minor Version
  Jt_I32 myPKBuild;        //!< Parasolid Kernel Build Number
  Jt_String myData;        //!< Parasolid Binary Data
};


#endif // _JtElement_XTBRep_HeaderFile
