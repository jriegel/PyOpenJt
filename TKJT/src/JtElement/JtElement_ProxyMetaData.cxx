// JT format reading and visualization tools
// Copyright (C) 2024 Juergen Riegel
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

#include <JtElement_ProxyMetaData.hxx>
#include <JtData_Reader.hxx>

#include <assert.h>
#include <stdio.h>
#include <codecvt>
#include <string>

IMPLEMENT_STANDARD_RTTIEXT(JtElement_ProxyMetaData, JtData_Object)

IMPLEMENT_OBJECT_CLASS(JtElement_ProxyMetaData, "Base Shape LOD Object",
    "ce357247-38fb-11d1-a5-06-00-60-97-bd-c6-e1")

const int strBufSize = 1024 * 64;
char strBuf[1024 * 64 + 2] ;


//=======================================================================
//function : Read
//purpose  : Read this entity from a translate file
//=======================================================================
Standard_Boolean JtElement_ProxyMetaData::Read (JtData_Reader& theReader)
{
#if 0
    bool ok = true;
    uint8_t theValue;
    std::cout << "\n Raw PMI dump:\n";
    while (ok) {
        ok = theReader.ReadPrimitiveValue(theValue);
        std::cout << (char)theValue;
    }
    std::cout << "\n END Raw PMI dump\n";

    return true;
#endif

  std::wstring_convert<std::codecvt_utf8<wchar_t>> toUTF8;
  std::wstring Key, Value;
  Key.reserve(100);
  Value.reserve(2048);
  int i;
  keyValueStream.clear();

  Jt_U16 aVersion;
  theReader.ReadU16(aVersion);

  theReader.ReadMbString(Key);

  while (Key.size() > 0) {

      for(const char& Char: toUTF8.to_bytes(Key))
          keyValueStream.push_back(Char);
      keyValueStream.push_back('\0');

      /*assert(PropertyKey.LengthOfCString() < strBufSize);
      Standard_PCharacter mchastr = strBuf;
      int length = PropertyKey.ToUTF8CString(mchastr);

      size_t i = 0;
      while (strBuf[i] != '\0')
          keyValueStream.push_back(strBuf[i++]);
      keyValueStream.push_back('\0');*/

      Jt_U8 ValueType;
      theReader.ReadU8(ValueType);

      switch (ValueType) {
      case 1:
          theReader.ReadMbString(Value);
          for (const char& Char : toUTF8.to_bytes(Value))
              keyValueStream.push_back(Char);
          keyValueStream.push_back('\0');

 /*         assert(Value.LengthOfCString() < strBufSize);
          length = Value.ToUTF8CString(mchastr);

          i = 0;
          while (strBuf[i] != '\0')
              keyValueStream.push_back(strBuf[i++]);
          keyValueStream.push_back('\0');*/

          break;
      case 2:
          Jt_I32 aIValue;
          theReader.Read(aIValue);
          snprintf(strBuf, strBufSize, "%d", aIValue);

          i = 0;
          while (strBuf[i] != '\0')
              keyValueStream.push_back(strBuf[i++]);
          keyValueStream.push_back('\0');

          break;
      case 3:
          Jt_F32 aFValue;
          theReader.Read(aFValue);
          snprintf(strBuf, strBufSize, "%f", aFValue);

          i = 0;
          while (strBuf[i] != '\0')
              keyValueStream.push_back(strBuf[i++]);
          keyValueStream.push_back('\0');

          break;
      case 4:

          Jt_I16 Year, Month, Day, Hour, Minute, Second;
          theReader.Read(Year);
          theReader.Read(Month);
          theReader.Read(Day);
          theReader.Read(Hour);
          theReader.Read(Minute);
          theReader.Read(Second);
          snprintf(strBuf, strBufSize, "%d-%d-%dT%d:%d:%d", Year, Month, Day, Hour, Minute, Second);

          i = 0;
          while (strBuf[i] != '\0')
              keyValueStream.push_back(strBuf[i++]);
          keyValueStream.push_back('\0');

          break;
      default: return false;
      }

      // read next potential Property key or NULL if end of table
      theReader.ReadMbString(Key);
  }

  return true;
}

//=======================================================================
//function : Dump
//purpose  :
//=======================================================================
Standard_Integer JtElement_ProxyMetaData::Dump (Standard_OStream& theStream) const
{
  theStream << "JtElement_ProxyMetaData ";
  return JtData_Object::Dump (theStream);
}
