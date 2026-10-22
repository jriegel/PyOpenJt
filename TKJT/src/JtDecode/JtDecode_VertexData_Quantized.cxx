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

#include <JtDecode_VertexData_Quantized.hxx>
#include <JtData_VectorRef.hxx>

bool JtDecode_VertexData_Quantized::UniformQuantizerData::Read (JtData_Reader& theReader)
{
  return theReader.ReadF32 (min) && theReader.ReadF32 (max) && theReader.ReadU8 (bits);
}

bool JtDecode_VertexData_Quantized::PointQuantizerData::Read (JtData_Reader& theReader)
{
  return X.Read (theReader) && Y.Read (theReader) && Z.Read (theReader);
}

JtDecode_VertexData_Quantized::JtDecode_VertexData_Quantized (
                              const PointQuantizerData& theQuantizerData,
                              const size_t       theNbComponents,
                                    JtDecode_Unpack&    theUnpacker)
  : JtDecode_VertexData (theNbComponents, theUnpacker)
  , myQuantizerData     (theQuantizerData) {}

int JtDecode_VertexData_Quantized::getOutCompCount (size_t thePackageCount)
{
  return static_cast<int>(thePackageCount);
}

void JtDecode_VertexData_Quantized::decode (Decoded::Ref theResults)
{
  for (Decoded::CompCountType j = 0; j < theResults.CompCount(); j++)
  {
    Jt_VecI32 aCodes (decodePackage (j));

    const UniformQuantizerData& aCurQuantizerData =
      JtData_VectorRef<const UniformQuantizerData> (myQuantizerData)[j];

    double    aMin  = aCurQuantizerData.min;
    double    aMax  = aCurQuantizerData.max;
    int aBits = aCurQuantizerData.bits;

    double aMaxCode = aBits < 32 ? (1 << aBits) : 0xFFFFFFFF;
    double aDecodeMultiplier = (aMax - aMin) / aMaxCode;

    for (Decoded::SizeType i = 0; i < theResults.Count(); i++)
    {
      double aValue = aMin + (static_cast<Jt_U32>(aCodes[i]) - 0.5) * aDecodeMultiplier;
      theResults[i][j] = static_cast <float> (aValue);
    }
  }
}
