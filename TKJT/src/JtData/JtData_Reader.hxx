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

#ifndef _JtData_Reader_HeaderFile
#define _JtData_Reader_HeaderFile

#include <JtData_ReaderInterface.hxx>
#include <JtData_ByteSwap.hxx>

//! Reader provides low level reading operation.
class JtData_Reader : public JtData_ReaderInterface<JtData_Reader>
{
public:
  //! Reader class incapsulating a pointer to JtData_Reader.
  //! This is a static virtual class.
  //! It should be used as a parent class in the following way:
  //! class Derived : public JtData_Reader::Wrapper<Derived>.
  template <class Derived>
  class Wrapper : public JtData_ReaderInterface<Derived>
  {
  public:
    Wrapper (JtData_Reader& theReader) : myReader (&theReader) {}

    operator       JtData_Reader&()       { return *myReader; }
    operator const JtData_Reader&() const { return *myReader; }

  private:
    JtData_Reader* myReader;
  };

public:
  //! Constructor.
  TKJT_EXPORT JtData_Reader (const std::shared_ptr<JtData_Model>& theModel);

  //! Destructor.
  TKJT_EXPORT virtual ~JtData_Reader();

  //! Read raw bytes from the stream.
  TKJT_EXPORT virtual bool ReadBytes (void* theBuffer, size_t theLength) = 0;

  //! Skip some bytes.
  TKJT_EXPORT virtual bool SkipBytes (size_t theLength) = 0;

  //! Get absolute reading position.
  TKJT_EXPORT virtual size_t GetPosition() const = 0;

  //! Get the associated model.
  const std::shared_ptr<JtData_Model>& Model() const { return myModel; }

  //! Read a primitive value from the stream.
  template <class Type>
  bool ReadPrimitiveValue (Type& theValue)
  {
    if (!ReadBytes (&theValue, sizeof (Type)))
      return false;

    if (myNeedSwap)
      ByteSwap (theValue);

    return true;
  }

  //! Specialization for signed byte.
  bool ReadPrimitiveValue (int8_t& theValue)
    { return ReadBytes (&theValue, sizeof (int8_t)); }

  //! Specialization for unsigned byte.
  bool ReadPrimitiveValue (uint8_t& theValue)
    { return ReadBytes (&theValue, sizeof (uint8_t)); }

  //! Read given number of primitive values all having the same type.
  template <class Type, class SizeT>
  bool ReadPrimitiveArray (Type* theArray, const SizeT theLength)
  {
    if (!ReadBytes (theArray, theLength * sizeof (Type)))
      return false;

    if (myNeedSwap)
      for (SizeT i = 0; i < theLength; i++)
        ByteSwap (theArray[i]);

    return true;
  }

  //! Specialization for array of signed bytes.
  template <class SizeT>
  bool ReadPrimitiveArray (int8_t* theArray, const SizeT theLength)
    { return ReadBytes (theArray, theLength); }

  //! Specialization for array of unsigned bytes.
  template <class SizeT>
  bool ReadPrimitiveArray (uint8_t* theArray, const SizeT theLength)
    { return ReadBytes (theArray, theLength); }

  //! Load/map raw bytes from the stream to memory
  //! and return a pointer to access the data.
  const void* LoadBytes (const size_t theLength);

  //! Free/unmap memory allocated/mapped by the Load method.
  void UnloadBytes (const void* theAddress);

  //! Read a 1-byte character string.
  bool ReadSbString (TCollection_AsciiString& theString);

  //! Read a 2-byte character string.
  bool ReadMbString (TCollection_ExtendedString& theString);

  //! Read a 2-byte character string into a std::wstring
  bool ReadMbString(std::wstring& theString);

  bool ReadObject(char16_t &theObject)
  {
    Jt_I16 theValue;
    if (ReadI16(theValue)) {
      theObject = theValue;
      return true;
    }
    return false;
  }

protected:
  std::shared_ptr<JtData_Model> myModel;
  bool     myNeedSwap;
};

#endif // _JtData_Reader_HeaderFile
