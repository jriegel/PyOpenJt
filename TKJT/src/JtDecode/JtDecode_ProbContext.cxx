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

#include <JtDecode_ProbContext.hxx>
#include <unordered_map>

bool JtDecode_ProbContextI32::ReadFirst (JtDecode_BitReader::Raw& theReader)
{
  return read (theReader, 32, 6, 6, 6, 6, 32);
}

bool JtDecode_ProbContextI32::ReadNext (JtDecode_BitReader::Raw& theReader,
                                                    const JtDecode_ProbContextI32& theFirstContext)
{
  if (!read (theReader, 32, 6, 6, 0, 6, 0))
    return false;

  std::unordered_map<int, int> aValuesMap;
  size_t i;
  for (i = 0; i < theFirstContext.Size(); i++)
    aValuesMap[theFirstContext[i].symbol] = theFirstContext[i].associatedValue;
  for (i = 0; i < Size(); i++)
  {
    auto anIter = aValuesMap.find (myEntries[i].symbol);
    if (anIter != aValuesMap.end())
      myEntries[i].associatedValue = anIter->second;
  }

  return true;
}

bool JtDecode_ProbContextI32::ReadMk2 (JtDecode_BitReader::Raw& theReader)
{
  return read (theReader, 16, 6, 6, 6, 0, 32);
}

bool JtDecode_ProbContextI32::ReadMk3 (JtDecode_BitReader::Raw& theReader)
{
  // JT 10.x layout: no symbol field, entries start with an escape flag bit
  if (!theReader.Load (16 + 6 + 7 + 32))
    return false;

  unsigned anEntryCount = theReader.ReadU32 (16);
  unsigned aOccBits     = theReader.ReadU32 (6);
  unsigned aValBits     = theReader.ReadU32 (7);
  int32_t  aMinValue    = theReader.ReadI32 (32);

  if (aOccBits > 32 || aValBits > 32
   || !theReader.Load (anEntryCount * (1 + aOccBits + aValBits)))
    return false;

  myEntries.Allocate (anEntryCount);
  myTotalCount = 0;

  for (size_t i = 0; i < anEntryCount; i++)
  {
    Entry& anEntry = myEntries[i];

    anEntry.symbol          = theReader.ReadU32 (1) ? -2 : 0;
    anEntry.occCount        = theReader.ReadU32Or0 (aOccBits);
    anEntry.associatedValue = theReader.ReadU32Or0 (aValBits) + aMinValue;
    anEntry.nextContext     = 0;

    myTotalCount += anEntry.occCount;
  }

  return true;
}

bool JtDecode_ProbContextI32::read (JtDecode_BitReader::Raw& theReader,
                                                const unsigned theEntryCountWidth,
                                                const unsigned theSymBitsNumWidth,
                                                const unsigned theOccBitsNumWidth,
                                                const unsigned theValBitsNumWidth,
                                                const unsigned theNxtBitsNumWidth,
                                                const unsigned theMinValueWidth)
{
  if (!theReader.Load (theEntryCountWidth + theSymBitsNumWidth + theOccBitsNumWidth
                     + theValBitsNumWidth + theNxtBitsNumWidth + theMinValueWidth))
    return false;

  unsigned      anEntryCount = theReader.ReadU32Or0 (theEntryCountWidth);
  unsigned      aSymBits     = theReader.ReadU32Or0 (theSymBitsNumWidth);
  unsigned      aOccBits     = theReader.ReadU32Or0 (theOccBitsNumWidth);
  unsigned      aValBits     = theReader.ReadU32Or0 (theValBitsNumWidth);
  unsigned      aNxtBits     = theReader.ReadU32Or0 (theNxtBitsNumWidth);
  int32_t       aMinValue    = theReader.ReadI32Or0 (theMinValueWidth);

  if (!theReader.Load (anEntryCount * (aSymBits + aOccBits + aValBits + aNxtBits)))
    return false;

  myEntries.Allocate (anEntryCount);
  myTotalCount = 0;

  for (size_t i = 0; i < anEntryCount; i++)
  {
    Entry& anEntry = myEntries[i];

    anEntry.symbol          = theReader.ReadU32Or0 (aSymBits) - 2;
    anEntry.occCount        = theReader.ReadU32Or0 (aOccBits);
    anEntry.associatedValue = theReader.ReadU32Or0 (aValBits) + aMinValue;
    anEntry.nextContext     = theReader.ReadU32Or0 (aNxtBits);

    myTotalCount += anEntry.occCount;
  }

  return true;
}
