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

#ifndef _JtDecode_VertexData_Quantized_HeaderFile
#define _JtDecode_VertexData_Quantized_HeaderFile

#include <JtDecode_VertexData.hxx>

//! Class decoding Uniform Quantizer data coded in collections containing
//! information in form of range divided into levels of equal spacing.
class JtDecode_VertexData_Quantized : public JtDecode_VertexData
{
public:
  //! Uniform Quantizer data.
  struct UniformQuantizerData
  {
    Jt_F32 min, max;
    Jt_U8  bits;

    bool Read (JtData_Reader& theReader);
  };

  //! Point data (one quantizer per coordinate).
  struct PointQuantizerData
  {
    UniformQuantizerData X, Y, Z;

    bool Read (JtData_Reader& theReader);
  };

  //! Constructor.
  TKJT_EXPORT JtDecode_VertexData_Quantized (
    const PointQuantizerData& theQuantizerData,
    const size_t theNbComponents,
    JtDecode_Unpack& theUnpacker = JtDecode_Unpack_Null);

protected:
  //! Get expected count of output components.
  TKJT_EXPORT int getOutCompCount (size_t thePackageCount) override;

  //! Decoding method.
  TKJT_EXPORT void decode (Decoded::Ref theResults) override;

  PointQuantizerData myQuantizerData;
};

#endif
