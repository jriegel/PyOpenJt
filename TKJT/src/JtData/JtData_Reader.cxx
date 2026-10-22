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

#include <JtData_Reader.hxx>
#include <string>

//=======================================================================
//function : JtData_Reader
//purpose  : Constructor
//=======================================================================

JtData_Reader::JtData_Reader (const std::shared_ptr<JtData_Model>& theModel)
  : myModel    (theModel)
  , myNeedSwap (theModel->IsFileLE() != JtData_Model::IsLittleEndianHost) {}

//=======================================================================
//function : ~JtData_Reader
//purpose  : Destructor
//=======================================================================

JtData_Reader::~JtData_Reader() {}

//=======================================================================
//function : LoadBytes
//purpose  : Load/map raw bytes from the stream to memory
//=======================================================================

const void* JtData_Reader::LoadBytes (const size_t theLength)
{
  void* aBuffer = Standard::Allocate (theLength);
  if (!aBuffer)
    return 0L;

  if (!Read (aBuffer, theLength))
  {
    Standard::Free (aBuffer);
    return 0L;
  }

  return aBuffer;
}

//=======================================================================
//function : UnloadBytes
//purpose  : Free/unmap memory allocated/mapped by the Load method
//=======================================================================

void JtData_Reader::UnloadBytes (const void* theAddress)
{
  if (theAddress)
    Standard::Free (const_cast <void*&> (theAddress));
}

//=======================================================================
//function : ReadSbString
//purpose  : Read a 1-byte character string
//=======================================================================

bool JtData_Reader::ReadSbString (std::string& theString)
{
  Jt_I32 aCount;
  if (!ReadI32 (aCount))
    return false;

  std::vector<char> aStrBuffer (static_cast<size_t> (aCount) + 1, '\0');
  if (!Read (aStrBuffer.data(), aCount))
    return false;
  theString = aStrBuffer.data();

  return true;
}

//=======================================================================
//function : ReadMbString
//purpose  : Read a 2-byte character string
//=======================================================================

bool JtData_Reader::ReadMbString(std::wstring& theString)
{
    Jt_I32 aCount;
    if (!ReadI32(aCount))
        return false;

    std::vector<char16_t> aStrBuffer (static_cast<size_t> (aCount) + 1, u'\0');
    if (!ReadArray(aStrBuffer.data(), aCount))
        return false;

    theString.clear();
    for (const char16_t* aChar = aStrBuffer.data(); *aChar != u'\0'; ++aChar)
        theString.push_back(static_cast<wchar_t>(*aChar));

    return true;
}
