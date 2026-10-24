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

#include <JtDecode_Int32CDP.hxx>

#include <JtDecode_BitReader.hxx>
#include <JtDecode_ProbContext.hxx>

#include <JtDecode_Int32CDP_Null.hxx>
#include <JtDecode_Int32CDP_Bitlength.hxx>
#include <JtDecode_Int32CDP_Bitlength2.hxx>
#include <JtDecode_Int32CDP_Huffman.hxx>
#include <JtDecode_Int32CDP_Arithmetic.hxx>
#include <JtDecode_Int32CDP_Chopper.hxx>

#include <JtData_Message.hxx>

JtDecode_Int32CDP::EncodedDataPtr JtDecode_Int32CDP::loadCDP1 (JtData_Reader& theReader)
{
  // Read codec type
  int aCodec;
  if (!theReader.ReadU8 (aCodec))
    return EncodedDataPtr();

  // Null codec, just read uncoded values
  if (aCodec == 0)
  {
    Jt_VecU32 aCodeText;
    if (!theReader.ReadVec (aCodeText))
      return EncodedDataPtr();

    return EncodedDataPtr (new JtDecode_Int32CDP_Null (aCodeText.Move()));
  }

  // Bits based codec
  std::unique_ptr<JtDecode_Int32CDP_Bits> aData;
  switch (aCodec)
  {
  case 1:
    aData = std::make_unique<JtDecode_Int32CDP_Bitlength>(theReader);
    break;
  case 2:
    aData = std::make_unique<JtDecode_Int32CDP_Huffman>(theReader);
    break;
  case 3:
    aData = std::make_unique<JtDecode_Int32CDP_Arithmetic>(theReader);
    break;
  default:
    TRACE (std::string("Unknown Codec (") + std::to_string(aCodec) + ")!");
    return EncodedDataPtr();
  }

  // Huffman or arithmetic codec: read probability contexts and load OOB data
  int aProbContextTableCount = 0;
  if (aCodec >= 2)
  {
    // read probability contexts
    if (!theReader.ReadU8 (aProbContextTableCount))
      return EncodedDataPtr();

    JtData_Vector<JtDecode_ProbContextI32> aProbContexts (aProbContextTableCount);
    {
      JtDecode_BitReader::Raw aContextReader (theReader);

      if (!aProbContexts[0].ReadFirst (aContextReader))
        return EncodedDataPtr();

      for (int i = 1; i < aProbContextTableCount; i++)
        if (!aProbContexts[i].ReadNext (aContextReader, aProbContexts[0]))
          return EncodedDataPtr();
    }
    aData->SetProbContexts (aProbContexts.Move());

    // load out-of-band data
    Jt_I32 anOutOfBandValueCount;
    theReader.ReadI32 (anOutOfBandValueCount);
    if (anOutOfBandValueCount > 0)
    {
      JtDecode_Int32CDP anOOBData;
      if (!anOOBData.Load1 (theReader))
        return EncodedDataPtr();
      aData->SetOOBData (std::move (anOOBData));
    }
  }

  // Read CodeText length and number of output values
  Jt_I32 aCodeTextLength;
  Jt_I32 aValueCount;
  if (!theReader.ReadI32 (aCodeTextLength)
   || !theReader.ReadI32 (aValueCount))
  {
    return EncodedDataPtr();
  }
  aData->SetOutValCount (aValueCount);

  // Read symbol count if present
  if (aProbContextTableCount > 1)
  {
    Jt_I32 aSymbolCount;
    if (!theReader.ReadI32 (aSymbolCount))
      return EncodedDataPtr();
  }

  // Read CodeText data
  if (!aData->LoadVec())
    return EncodedDataPtr();

  return aData;
}

JtDecode_Int32CDP::EncodedDataPtr JtDecode_Int32CDP::loadCDP2 (JtData_Reader& theReader)
{
  // Read number of output values
  Jt_I32 aValueCount;
  if (!theReader.ReadI32 (aValueCount))
    return EncodedDataPtr();

  if (aValueCount == 0)
    return EncodedDataPtr (new JtDecode_Int32CDP_Null (Jt_VecU32().Move()));

  // Read codec type
  int aCodec;
  if (!theReader.ReadU8 (aCodec))
    return EncodedDataPtr();

  // Chopper pseudo-codec
  if (aCodec == 4)
  {
    Jt_U8 aChop;
    if (!theReader.ReadU8 (aChop))
      return EncodedDataPtr();

    if (aChop == 0)
      return loadCDP2 (theReader);

    Jt_I32 aBias;
    Jt_U8  aSpan;
    if (!theReader.ReadI32 (aBias)
     || !theReader.ReadU8  (aSpan))
      return EncodedDataPtr();

    // load chopped MSB and LSB data
    JtDecode_Int32CDP aMSBData;
    JtDecode_Int32CDP aLSBData;
    if (!aMSBData.Load2 (theReader)
     || !aLSBData.Load2 (theReader))
      return EncodedDataPtr();

    if (aMSBData.GetOutValCount() != aValueCount
     || aLSBData.GetOutValCount() != aValueCount)
    {
      ALARM ("Error: Chopped data size mismatch");
      return EncodedDataPtr();
    }

    return EncodedDataPtr (
      new JtDecode_Int32CDP_Chopper (std::move (aMSBData), std::move (aLSBData), aChop, aSpan, aBias));
  }

  // Null, bitlength or arithmetic codec: read CodeText length
  Jt_I32 aCodeTextLength;
  if (!theReader.ReadI32 (aCodeTextLength) || aCodeTextLength <= 0)
    return EncodedDataPtr();

  // Null codec
  if (aCodec == 0)
  {
    Jt_VecU32 aCodeText;
    if (!theReader.ReadVec (aCodeText, (aCodeTextLength + 31) / 32))
      return EncodedDataPtr();

    return EncodedDataPtr (new JtDecode_Int32CDP_Null (aCodeText.Move()));
  }

  // Bits based codec
  std::unique_ptr<JtDecode_Int32CDP_Bits> aData;
  switch (aCodec)
  {
  case 1: // Bitlength codec
    aData = std::make_unique<JtDecode_Int32CDP_Bitlength2>(theReader, theReader.Model()->MajorVersion() >= 10);
    break;

  case 3: // Arithmetic codec
    aData = std::make_unique<JtDecode_Int32CDP_Arithmetic>(theReader);
    break;

  default: // Unknown codec
    TRACE (std::string("Unknown Codec (") + std::to_string(aCodec) + ")!");
    return EncodedDataPtr();
  }

  // Bitlength or arithmetic codec: set expected output values count
  aData->SetOutValCount (aValueCount);

  // Bitlength or arithmetic codec: read CodeText data
  if (!aData->Load ((aCodeTextLength + 31) / 32))
    return EncodedDataPtr();

  // Arithmetic codec: read probability context and load OOB data
  if (aCodec == 3)
  {
    // read probability context
    const bool aIsV10 = theReader.Model()->MajorVersion() >= 10;
    JtData_Vector<JtDecode_ProbContextI32> aProbContexts (1);
    {
      JtDecode_BitReader::Raw aContextReader (theReader);
      if (aIsV10 ? !aProbContexts[0].ReadMk3 (aContextReader)
                 : !aProbContexts[0].ReadMk2 (aContextReader))
        return EncodedDataPtr();
    }
    const bool aHasEscape = !aIsV10 || aProbContexts[0].HasEscape();
    aData->SetProbContexts (aProbContexts.Move());

    // load out-of-band data (in 10.x only present if the context has an escape symbol)
    if (aHasEscape)
    {
      JtDecode_Int32CDP anOOBData;
      if (!anOOBData.Load2 (theReader))
        return EncodedDataPtr();

      aData->SetOOBData (std::move (anOOBData));
    }
  }

  return aData;
}
