#pragma once
// JT format reading and visualization tools
// Copyright (C) 2024 Jürgen Riegel
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

#include <lzma.h>

//! Class performing unpacking of data using zlib.
class JtData_InflateLzma : public JtData_Reader
{
public:
  //! Constructor.
  TKJT_EXPORT JtData_InflateLzma(JtData_Reader& theReader, const size_t theLength);

  //! Destructor.
  TKJT_EXPORT virtual ~JtData_InflateLzma();

  //! Buffered inflate raw bytes.
  TKJT_EXPORT bool ReadBytes (void* theBuffer, size_t theLength) override;

  //! Skip some bytes.
  TKJT_EXPORT bool SkipBytes (size_t theLength) override;

  //! Get absolute reading position in the inflated data.
  TKJT_EXPORT size_t GetPosition() const override;

protected:
  size_t read (uint8_t* theBuffer, size_t theLength);

protected:
  static const size_t CHUNK = 32768;

  JtData_Reader* myReader;
  lzma_stream    myZStream;

  const uint8_t*    myInBuffer;
  size_t  myInputRest;

  uint8_t           myOutBuffer[CHUNK];
  const uint8_t*    myOutBufPos;
  size_t  myOutBufRest;
};


