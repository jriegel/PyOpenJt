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

#include <JtData_InflateLzma.hxx>

#define min(a, b) ((a) < (b) ? (a) : (b))

//=======================================================================
//function : JtData_Inflate
//purpose  : Constructor
//=======================================================================

JtData_InflateLzma::JtData_InflateLzma(JtData_Reader& theReader, const size_t theLength)
  : JtData_Reader (theReader.Model())
  , myReader      (&theReader)
  , myInBuffer    (0L)
  , myInputRest   (theLength)
  , myOutBufPos   (myOutBuffer)
  , myOutBufRest  (0)
{
  myZStream = LZMA_STREAM_INIT;

  
  lzma_ret ret = lzma_stream_decoder(
      &myZStream, UINT64_MAX, LZMA_CONCATENATED);

  if (ret == LZMA_OK)
      return;

  const char* msg;
  switch (ret) {
  case LZMA_MEM_ERROR:
      msg = "Memory allocation failed";
      break;

  case LZMA_OPTIONS_ERROR:
      msg = "Unsupported decompressor flags";
      break;

  default:
      // This is most likely LZMA_PROG_ERROR indicating a bug in
      // this program or in liblzma. It is inconvenient to have a
      // separate error message for errors that should be impossible
      // to occur, but knowing the error code is important for
      // debugging. That's why it is good to print the error code
      // at least when there is no good error message to show.
      msg = "Unknown error, possibly a bug";
      break;
  }

}

//=======================================================================
//function : ~JtData_Inflate
//purpose  : Destructor
//=======================================================================

JtData_InflateLzma::~JtData_InflateLzma()
{
    lzma_end(&myZStream);
    myReader->Unload (myInBuffer);
}

//=======================================================================
//function : ReadBytes
//purpose  : Buffered inflate raw bytes
//=======================================================================

bool JtData_InflateLzma::ReadBytes (void* theBuffer, size_t theLength)
{
  // Do special actions if more data is needed than remains in the internal output buffer
  if (theLength > myOutBufRest)
  {
    // first, pick up the data remained in the buffer if any
    memcpy (theBuffer, myOutBufPos, myOutBufRest);

    // then, read the rest of the needed data
    theBuffer = reinterpret_cast <char*>(theBuffer) + myOutBufRest;
    theLength =           theLength  - myOutBufRest;

    // if the data amount to read is large enough,
    // leave the internal buffer empty and read the data directly to the external buffer
    if (theLength > sizeof (myOutBuffer) / 2)
    {
      myOutBufRest = 0;
      return read (static_cast <uint8_t*> (theBuffer), theLength) == theLength;
    }

    // load the internal buffer
    myOutBufPos  = myOutBuffer;
    myOutBufRest = read (myOutBuffer, sizeof (myOutBuffer));
    if (!myOutBufRest)
      return false;

    // go down and copy the needed data remainder from the internal output buffer
  }

  // Copy the needed data (or its remainder) from the internal output buffer
  memcpy (theBuffer, myOutBufPos, theLength);
  myOutBufPos  += theLength;
  myOutBufRest -= theLength;
  return true;
}

//=======================================================================
//function : SkipBytes
//purpose  : Skip some bytes
//=======================================================================

bool JtData_InflateLzma::SkipBytes (size_t theLength)
{
  while (theLength > myOutBufRest)
  {
    theLength   -= myOutBufRest;
    myOutBufPos  = myOutBuffer;
    myOutBufRest = read (myOutBuffer, sizeof (myOutBuffer));
    if (!myOutBufRest)
      return false;
  }

  myOutBufPos  += theLength;
  myOutBufRest -= theLength;
  return true;
}

//=======================================================================
//function : GetPosition
//purpose  : Get absolute reading position in the inflated data
//=======================================================================

size_t JtData_InflateLzma::GetPosition() const
{
  return myZStream.total_out - myOutBufRest;
}

//=======================================================================
//function : read
//purpose  : unbuffered inflate
//=======================================================================

size_t JtData_InflateLzma::read (uint8_t* theBuffer, size_t theLength)
{
  myZStream.next_out  = (uint8_t*)theBuffer;
  myZStream.avail_out = static_cast<unsigned>(theLength);

  // Run inflate() on input until the input exceeds or the output buffer is full
  while (myZStream.avail_out > 0)
  {
    // load next piece of input data if needed
    if (!myInBuffer)
    {
      if (!myInputRest)
        break;

      size_t aBufSize = min (CHUNK, myInputRest);
      myInBuffer = static_cast <const uint8_t*> (myReader->Load (aBufSize));
      if (!myInBuffer)
        return 0;

      myZStream.next_in  = const_cast <uint8_t*> (myInBuffer);
      myZStream.avail_in = static_cast<unsigned>(aBufSize);
      myInputRest       -= aBufSize;
    }

    // decompress the loaded data
    int ret = lzma_code(&myZStream, LZMA_RUN);

    // free the input buffer if it is exhausted
    if (myZStream.avail_in == 0)
    {
      myReader->Unload (myInBuffer);
      myInBuffer = 0L;
    }

	if (ret != LZMA_OK) {
		// Once everything has been decoded successfully, the
		// return value of lzma_code() will be LZMA_STREAM_END.
		//
		// It is important to check for LZMA_STREAM_END. Do not
		// assume that getting ret != LZMA_OK would mean that
		// everything has gone well or that when you aren't
		// getting more output it must have successfully
		// decoded everything.
		if (ret == LZMA_STREAM_END)
			return true;

		// It's not LZMA_OK nor LZMA_STREAM_END,
		// so it must be an error code. See lzma/base.h
		// (src/liblzma/api/lzma/base.h in the source package
		// or e.g. /usr/include/lzma/base.h depending on the
		// install prefix) for the list and documentation of
		// possible values. Many values listen in lzma_ret
		// enumeration aren't possible in this example, but
		// can be made possible by enabling memory usage limit
		// or adding flags to the decoder initialization.
		const char* msg;
		switch (ret) {
		case LZMA_MEM_ERROR:
			msg = "Memory allocation failed";
			break;

		case LZMA_FORMAT_ERROR:
			// .xz magic bytes weren't found.
			msg = "The input is not in the .xz format";
			break;

		case LZMA_OPTIONS_ERROR:
			// For example, the headers specify a filter
			// that isn't supported by this liblzma
			// version (or it hasn't been enabled when
			// building liblzma, but no-one sane does
			// that unless building liblzma for an
			// embedded system). Upgrading to a newer
			// liblzma might help.
			//
			// Note that it is unlikely that the file has
			// accidentally became corrupt if you get this
			// error. The integrity of the .xz headers is
			// always verified with a CRC32, so
			// unintentionally corrupt files can be
			// distinguished from unsupported files.
			msg = "Unsupported compression options";
			break;

		case LZMA_DATA_ERROR:
			msg = "Compressed file is corrupt";
			break;

		case LZMA_BUF_ERROR:
			// Typically this error means that a valid
			// file has got truncated, but it might also
			// be a damaged part in the file that makes
			// the decoder think the file is truncated.
			// If you prefer, you can use the same error
			// message for this as for LZMA_DATA_ERROR.
			msg = "Compressed file is truncated or "
				"otherwise corrupt";
			break;

		default:
			// This is most likely LZMA_PROG_ERROR.
			msg = "Unknown error, possibly a bug";
			break;
		}

		fprintf(stderr, "Decoder error : "
			"%s (error code %u)\n",
			msg, ret);
		return 0;
	}
  }

  return theLength - myZStream.avail_out;
}
