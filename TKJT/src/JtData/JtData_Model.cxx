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

#include <JtData_Model.hxx>
#include <JtData_FileReader.hxx>
#include <JtData_Inflate.hxx>
#include <JtData_InflateLzma.hxx>

#include <JtData_Message.hxx>

#include <JtNode_Partition.hxx>
#include <JtProperty_LateLoaded.hxx>
#include <JtProperty_String.hxx>

#include <string>
#include <sstream>
#include <charconv>
#include <filesystem>
#include <bit>
#include <JtData_Object.hxx>

using namespace std;

const bool JtData_Model::IsLittleEndianHost = std::endian::native == std::endian::little;

static const Jt_GUID EOEMarkerGUID("ffffffff-ffff-ffff-ff-ff-ff-ff-ff-ff-ff-ff");


//=======================================================================
//function : JtData_Model
//purpose  : Constructor
//=======================================================================
JtData_Model::JtData_Model(const std::wstring &theFileName,
                           const std::shared_ptr<JtData_Model> & theParent)
    : myParent(theParent), myFileName(theFileName), myIsFileLE(false), myMajorVersion(0), myMinorVersion(0) {}

//=======================================================================
//function : Init
//purpose  : Read JT file header, TOC and LSG and return a handle to the root LSG node
//=======================================================================
std::shared_ptr<JtNode_Partition> JtData_Model::Init()
{
  // Open the file
  std::ifstream aFile;
  if (!open(aFile))
  {
    ALARM("Error: Failed to open Jt file");
    return std::shared_ptr<JtNode_Partition>();
  }

  // Read File Header

  // Read version
  char vstr[81] = {};
  aFile.read(vstr, 80);

  std::string aMajorVersionStr(vstr);
  if (aMajorVersionStr.find("Version") == std::string::npos)
  {
    ALARM("Error: Failed to read Jt file version");
    return std::shared_ptr<JtNode_Partition>();
  }

  // the second whitespace-separated token holds "<major>.<minor>"
  {
    std::istringstream aTokens(aMajorVersionStr);
    std::string aFirst, aSecond;
    aTokens >> aFirst >> aSecond;
    aMajorVersionStr = aSecond;
  }

  size_t aDot = aMajorVersionStr.find('.');
  if (aDot == std::string::npos || aDot < 1)
  {
    ALARM("Error: Failed to read Jt file version");
    return std::shared_ptr<JtNode_Partition>();
  }

  std::string aMinorVersionStr = aMajorVersionStr.substr(aDot + 1);
  aMajorVersionStr.resize(aDot);

  auto aParseInt = [](const std::string& theStr, int& theValue)
  {
    const char* aEnd = theStr.data() + theStr.size();
    auto aRes = std::from_chars(theStr.data(), aEnd, theValue);
    return !theStr.empty() && aRes.ec == std::errc() && aRes.ptr == aEnd;
  };
  if (!aParseInt(aMajorVersionStr, myMajorVersion) || !aParseInt(aMinorVersionStr, myMinorVersion))
  {
    ALARM("Error: Failed to read Jt file version");
    return std::shared_ptr<JtNode_Partition>();
  }

  TRACE(std::string("\n\nInfo: File version = ") + std::to_string(myMajorVersion) + "." + std::to_string(myMinorVersion));

  // Read byte order
  char aByteOrder;
  aFile.read(&aByteOrder, 1);
  myIsFileLE = (aByteOrder == 0); 
     
  TRACE(std::string("Info: Byte order = ") + (myIsFileLE ? "LE" : "BE"));

  // Read reserved field, TOC offset, LSG Segment ID
  Jt_I32 aReservedField;
  Jt_U64 aTOCOffset;
  Jt_GUID anLSGSegmentGUID;
  {
    JtData_FileReader aReader(aFile, self());

    if (myMajorVersion < 10) {
        Jt_I32 oldTocOffset;
        if (!aReader.ReadI32(aReservedField) || !aReader.ReadI32(oldTocOffset) || !aReader.ReadGUID(anLSGSegmentGUID))
        {
            ALARM("Error: Failed to read TOC offset and LSG segment ID");
            return std::shared_ptr<JtNode_Partition>();
        }
        aTOCOffset = oldTocOffset;
    }
    else {
        if (!aReader.ReadI32(aReservedField) || !aReader.ReadU64(aTOCOffset) || !aReader.ReadGUID(anLSGSegmentGUID))
        {
            ALARM("Error: Failed to read TOC offset and LSG segment ID");
            return std::shared_ptr<JtNode_Partition>();
        }
    }
  }

  char guidBuf[100]; anLSGSegmentGUID.ToString(guidBuf);
  //cerr << "LSG Guid:" << guidBuf << '\n';

  // Read TOC
  if (!readTOC(aFile, aTOCOffset))
  {
    ALARM("Error: Failed to read TOC");
    return std::shared_ptr<JtNode_Partition>();
  }

  // Find and read LSG segment
  auto aTocIter = myTOC.find(anLSGSegmentGUID);
  if (aTocIter == myTOC.end())
  {
    cerr << "No LSG segment, TODO: Read only Meshes plain...\n";
    return std::shared_ptr<JtNode_Partition>();
  }
  const Jt_U64 anLSGSegmentOffset = aTocIter->second;
  TRACE("Info: LSG segment is found at offset " + anLSGSegmentOffset);

  std::shared_ptr<JtData_Object> aRootNode = readSegment(aFile, anLSGSegmentOffset, true);

  aFile.close();

  return std::dynamic_pointer_cast<JtNode_Partition>(aRootNode);
}

//=======================================================================
//function : open
//purpose  : Open the file
//=======================================================================
bool JtData_Model::open(ifstream &aFile) const
{
  aFile.open(std::filesystem::path(myFileName), ios::binary | ios::in);

  return aFile.is_open();
}

//=======================================================================
//function : readTOC
//purpose  : Read TOC from the JT file
//=======================================================================
bool JtData_Model::readTOC(std::ifstream &theFile, const Jt_U64 theOffset)
{
  JtData_FileReader aReader(theFile, self(), theOffset);

  // Read entry count
  Jt_I32 aCount;
  if (!aReader.ReadI32(aCount) || aCount < 1)
    return false;
  TRACE("Info: TOC entries count = " + aCount);

  // Read TOC entries
  std::unordered_map<int, int> aTypeMap;
  
  while (aCount--)
  {
    // Read Segment ID, offset, length, attributes
    Jt_GUID aGUID;
    Jt_U64 aOffset;
    Jt_U32 aLength;
    Jt_U32 aAttrib;

    if (myMajorVersion > 9) {

        if (!aReader.ReadGUID(aGUID) || !aReader.ReadU64(aOffset) || !aReader.ReadU32(aLength) || !aReader.ReadU32(aAttrib))
        {
            return false;
        }
    }
    else {
        Jt_I32 oldOffset, oldLength;
        if (!aReader.ReadGUID(aGUID) || !aReader.ReadI32(oldOffset) || !aReader.ReadI32(oldLength) || !aReader.ReadU32(aAttrib))
        {
            return false;
        }
        aOffset = oldOffset;
        aLength = oldLength;
    }

    char guidBuf[100]; aGUID.ToString(guidBuf);


    const int aType = (aAttrib >> 24) & 0xFF;

    //int lowerBits = aAttrib & 0xFF; 
    //cerr << "TOC Entry: GUID:" << guidBuf << " Type:"<< aType << " LowerBits:" << lowerBits << " Offsett:" << aOffset << " Length : " << aLength << " Attribute : " << aAttrib << '\n';

    ++aTypeMap[aType];

    myTOC[aGUID] = aOffset;
    //stdToc.insert({ aGUID, aOffset });
    //TocEntry tocItem(aGUID, int(aOffset), int(aLength), int(aType));

    stdToc.emplace_back(aGUID, aOffset, aLength, int(aType));
    //stdToc.push_back(tocItem);
  }

  TRACE("Info: Type statistics");
  for (const auto& aTypeCount : aTypeMap)
  {
    //cerr << (std::string("Type ") + std::to_string(aTypeCount.first) + " count " + std::to_string(aTypeCount.second)) << '\n';
  }

  return true;
}

//=======================================================================
//function : readSegment
//purpose  : Read objects from a JT file segment
//=======================================================================
std::shared_ptr<JtData_Object> JtData_Model::readSegment(std::ifstream &theFile,
                                                const Jt_U64 theOffset,
                                                const bool theIsLSG) const
{
  JtData_FileReader aReader(theFile, self(), theOffset);

  size_t aSegStart = aReader.GetPosition();

  // Read the segment header
  Jt_GUID aGUID;
  Jt_I32 aType;
  Jt_I32 aSize;
  if (!aReader.ReadGUID(aGUID) || !aReader.ReadI32(aType) || !aReader.ReadI32(aSize))
  {
    ALARM("Error: Failed to read header of segment with offset " + theOffset);
    return std::shared_ptr<JtData_Object>();
  }

  // Select appropriate reader for reading the segment data:
  // - use the file reader if the segment is not compressed;
  // - use JtData_Inflate is the segment is compressed.
  JtData_Reader *aDataReaderPtr = &aReader;
  switch (aType)
  {
      case 1:
      case 2:
      case 3:
      case 4:
      case 17:
      case 18:
      case 20:
      case 24:
      {
        // read Compression Flag, Data Length, Algorithm
        Jt_U32 aFlag;
        Jt_I32 aDataLength;
        Jt_U8 aAlgorithm;
        if (!aReader.ReadU32(aFlag) || !aReader.ReadI32(aDataLength) || !aReader.ReadU8(aAlgorithm))
        {
          ALARM("Error: Failed to read compression flags of segment with offset " + theOffset);
          return std::shared_ptr<JtData_Object>();
        }

        // if compressed, replace the reader by a JtData_Inflate instance
        if (aFlag == 2 && aAlgorithm == 2)
          aDataReaderPtr = new JtData_Inflate(aReader, aDataLength - sizeof(Jt_U8));
        else if (aFlag == 3 && aAlgorithm == 3)
            aDataReaderPtr = new JtData_InflateLzma(aReader, aDataLength - sizeof(Jt_U8));
      }
  }

  // Read the segment data
  std::shared_ptr<JtData_Object> aFirstObject;
  bool aResult;

  if (theIsLSG)
    aResult = readLSGData(*aDataReaderPtr, aFirstObject);
  else
    aResult = readElement(*aDataReaderPtr, aFirstObject);

  // Free the JtData_Inflate reader if it was used
  if (aDataReaderPtr != &aReader)
    delete aDataReaderPtr;

  // Check the reading result
  if (!aResult)
  {
    ALARM("Error: Segment data reading failed");
    return std::shared_ptr<JtData_Object>();
  }

  // Check the loaded data length
  size_t aBytesSpec = aSegStart + aSize;
  size_t aReaderPos = aReader.GetPosition();
  if (aBytesSpec < aReaderPos)
  {
    size_t aBytesRest = aBytesSpec - aReaderPos;
    ALARM("Error: " + std::to_string(aBytesRest) + " extra bytes were read after end of the segment");
    return std::shared_ptr<JtData_Object>();
  }
  if (aBytesSpec > aReaderPos)
  {
    size_t aBytesRest = aBytesSpec - aReaderPos;
    WARNING("Warning: " + std::to_string(aBytesRest) + " segment bytes remained after reading, ignored");
  }

  // Return the first object read from the segment
  return aFirstObject;
}

//=======================================================================
//function : readSegment
//purpose  : Read objects from a JT file segment
//=======================================================================
std::shared_ptr<JtData_Object> JtData_Model::readSegmentDump(std::ifstream& theFile,
    const Jt_U64 theOffset,
    const bool ) const
{
    JtData_FileReader aReader(theFile, self(), theOffset);

    size_t aSegStart = aReader.GetPosition();

    // Read the segment header
    Jt_GUID aGUID;
    Jt_I32 aType;
    Jt_I32 aSize;
    if (!aReader.ReadGUID(aGUID) || !aReader.ReadI32(aType) || !aReader.ReadI32(aSize))
    {
        ALARM("Error: Failed to read header of segment with offset " + theOffset);
        return std::shared_ptr<JtData_Object>();
    }

    // Select appropriate reader for reading the segment data:
    // - use the file reader if the segment is not compressed;
    // - use JtData_Inflate is the segment is compressed.
    JtData_Reader* aDataReaderPtr = &aReader;
    switch (aType)
    {
    case 1:
    case 2:
    case 3:
    case 4:
    case 17:
    case 18:
    case 20:
    case 24:
    {
        // read Compression Flag, Data Length, Algorithm
        Jt_I32 aFlag;
        Jt_I32 aDataLength;
        Jt_U8 aAlgorithm;
        if (!aReader.ReadI32(aFlag) || !aReader.ReadI32(aDataLength) || !aReader.ReadU8(aAlgorithm))
        {
            ALARM("Error: Failed to read compression flags of segment with offset " + theOffset);
            return std::shared_ptr<JtData_Object>();
        }

        // if compressed, replace the reader by a JtData_Inflate instance
        if (aFlag == 2 && aAlgorithm == 2)
            aDataReaderPtr = new JtData_Inflate(aReader, aDataLength - sizeof(Jt_U8));
        else if (aFlag == 3 && aAlgorithm == 3)
            aDataReaderPtr = new JtData_InflateLzma(aReader, aDataLength - sizeof(Jt_U8));
    }
    }

    //// Read the segment data
    std::shared_ptr<JtData_Object> aFirstObject;
    //bool aResult;

    //if (theIsLSG)
    //    aResult = readLSGData(*aDataReaderPtr, aFirstObject);
    //else
    //    aResult = readElement(*aDataReaderPtr, aFirstObject);


    //// Check the reading result
    //if (!aResult)
    //{
    //    ALARM("Error: Segment data reading failed");
    //    return std::shared_ptr<JtData_Object>();
    //}

    uint8_t theValue;
    std::cout << "\n Raw PMI dump:\n";
    for (int i = 0; i < aSize; i++) {
        aDataReaderPtr->ReadPrimitiveValue(theValue);
        if(__isascii(theValue) != 0)
            std::cout << '.';
        else
            std::cout << (char)theValue;
    }
    std::cout << "\n END Raw PMI dump\n";

    // Free the JtData_Inflate reader if it was used
    if (aDataReaderPtr != &aReader)
        delete aDataReaderPtr;

    // Check the loaded data length
    size_t aBytesSpec = aSegStart + aSize;
    size_t aReaderPos = aReader.GetPosition();
    if (aBytesSpec < aReaderPos)
    {
        size_t aBytesRest = aBytesSpec - aReaderPos;
        ALARM("Error: " + std::to_string(aBytesRest) + " extra bytes were read after end of the segment");
        return std::shared_ptr<JtData_Object>();
    }
    if (aBytesSpec > aReaderPos)
    {
        size_t aBytesRest = aBytesSpec - aReaderPos;
        WARNING("Warning: " + std::to_string(aBytesRest) + " segment bytes remained after reading, ignored");
    }

    // Return the first object read from the segment
    return aFirstObject;
}

//=======================================================================
//function : readLSGData
//purpose  : Read LSG segment data
//=======================================================================
bool JtData_Model::readLSGData(JtData_Reader &theReader,
                                           std::shared_ptr<JtData_Object> & theFirstObject) const
{
  const std::wstring anObjectNameKey = L"JT_PROP_NAME";

  // Read graph elements
  JtData_Object::MapOfObjects aMapObjects;
  if (!readElements(theReader, aMapObjects, theFirstObject))
  {
    ALARM("Error: Failed to read LSG elements");
    return false;
  }

  // Bind the objects together
  for (auto& anEntry : aMapObjects)
    anEntry.second->BindObjects(aMapObjects);

  // Read property atom elements
  JtData_Object::MapOfObjects aMapProperties;
  if (!readElements(theReader, aMapProperties, theFirstObject))
  {
    ALARM("Error: Failed to read property atom elements");
    return false;
  }

  // Read Property Table
  Jt_I16 aTableVersion;
  Jt_I32 aTableLength;
  if (!theReader.ReadI16(aTableVersion) || !theReader.ReadI32(aTableLength))
  {
    ALARM("Error: Failed to read property table");
    return false;
  }

  while (aTableLength-- > 0)
  {
    // get an object
    Jt_I32 anObjectId;
    if (!theReader.ReadI32(anObjectId))
    {
      ALARM("Error: Failed to read LSG object ID");
      return false;
    }

    std::shared_ptr<JtData_Object> anObject;
    if (auto anObjIter = aMapObjects.find(anObjectId); anObjIter != aMapObjects.end())
      anObject = anObjIter->second;

    // read (key,value) pairs
    JtData_Object::ListOfLateLoads aLateLoads;
    Jt_I32 aKey, aValue;
    for (;;)
    {
      // read key
      if (!theReader.ReadI32(aKey))
      {
        ALARM("Error: Failed to read property key");
        return false;
      }

      if (aKey == 0)
        break;

      // read value
      if (!theReader.ReadI32(aValue))
      {
        ALARM("Error: Failed to read property value");
        return false;
      }

      // analyse the value
      std::shared_ptr<JtData_Object> aProperty;
      if (auto aPropIter = aMapProperties.find(aValue); aPropIter != aMapProperties.end())
      {
        aProperty = aPropIter->second;
        // if this is late loaded property, add it to a list
        std::shared_ptr<JtProperty_LateLoaded> aLateProp =
            std::dynamic_pointer_cast<JtProperty_LateLoaded>(aProperty);
        if (aLateProp)
          aLateLoads.push_back(aLateProp);
        // if this is a string property with key = "JT_PROP_NAME", bind it to the object
        else
        {
          std::shared_ptr<JtProperty_String> aStringProp =
              std::dynamic_pointer_cast<JtProperty_String>(aProperty);
          if (aStringProp)
          {
            std::shared_ptr<JtData_Object> aKeyProperty;
            if (auto aKeyIter = aMapProperties.find(aKey); aKeyIter != aMapProperties.end())
            {
              aKeyProperty = aKeyIter->second;
              std::shared_ptr<JtProperty_String> aKeyStringProp =
                  std::dynamic_pointer_cast<JtProperty_String>(aKeyProperty);
              if (aKeyStringProp && aKeyStringProp->Value() == anObjectNameKey)
                anObject->BindName(aStringProp->Value());
            }
          }//else
           //   cerr << "Unknown Property\n";
        }
      

      }
      //else
        //  cerr << "did not find a Property\n";
    }

    // bind the late loaded properties to the object
    if (anObject && !aLateLoads.empty())
      anObject->BindLateLoads(aLateLoads);
  }

  return true;
}

//=======================================================================
//function : readElements
//purpose  : Read a sequence of elements from a JT file segment
//=======================================================================
bool JtData_Model::readElements(JtData_Reader &theReader,
                                            JtData_Object::MapOfObjects &theMap,
                                            std::shared_ptr<JtData_Object> & theFirstObject) const
{
  std::shared_ptr<JtData_Object> anObject;
  Jt_I32 anObjectID;

  // Read elements while possible until End-Of-Elements marker is reached
  while (readElement(theReader, anObject, &anObjectID))
  {
    // if End-Of-Elements marker is reached, finish the reading
    if (!anObject)
      return true;

    // if an object of a known type is read, store it
    if (typeid(*anObject) != typeid(JtData_Object))
    {
      theMap[anObjectID] = anObject;
      if (!theFirstObject)
        theFirstObject = anObject;
    }
  }

  // Fail if the reading was failed
  return false;
}

//=======================================================================
//function : readElement
//purpose  : Read an element from a JT file segment
//=======================================================================
bool JtData_Model::readElement(JtData_Reader &theReader,
                                           std::shared_ptr<JtData_Object> & theObject,
                                           Jt_I32 *theObjectIDPtr) const
{
  // Read Element Length
    Jt_I32 anElemLength;
  if (!theReader.ReadI32(anElemLength))
  {
    ALARM("Error: Failed to read element length");
    return false;
  }

  // Start reading the element data
  size_t anElemStart = theReader.GetPosition();

  // Read Object Type ID
  Jt_GUID anObjectTypeID;
  if (!theReader.ReadGUID(anObjectTypeID))
  {
    ALARM("Error: Failed to read element GUID");
    return false;
  }
  else
  {
    char aGuidString[128];
    anObjectTypeID.ToString(aGuidString);
    TRACE(std::string("Info: Reading element with GUID ") + aGuidString);
  }

  // Classify and create the object
  const JtData_Object::ClassInfo *aClassInfo = JtData_Object::FindClass(anObjectTypeID);
  if (aClassInfo != NULL)
  {
    // known object
    TRACE(aClassInfo->Name());
    theObject = aClassInfo->Create();
  }
  else if (anObjectTypeID == EOEMarkerGUID)
  {
    // End-Of-Elements marker
    theObject = std::shared_ptr<JtData_Object>();
    return true;
  }
  else
  {
    // unknown object
    char aGuidString[128];
    anObjectTypeID.ToString(aGuidString);
    //cerr << std::string("Unknown Element with GUID ") + aGuidString << '\n';
    theObject = std::make_shared<JtData_Object>();
    return theReader.Skip(anElemStart + anElemLength - theReader.GetPosition());
  }

  // Read the object data
  Jt_U8 anObjectBaseType;
  if (!theReader.ReadU8(anObjectBaseType))
    return false;

  if (theObjectIDPtr)
  {
    // for LSG segment, always read Object ID for elements
    if (!theReader.ReadI32(*theObjectIDPtr))
      return false;
  }
  else if (MajorVersion() > 8)
  {
    // for other segments, read Object ID only if JT version is 9.x or higher
    Jt_I32 anObjectID;
    if (!theReader.ReadI32(anObjectID))
      return false;
  }

  if (!theObject->Read(theReader))
    return false;

  // Check the loaded data length
  size_t aBytesSpec = anElemStart + anElemLength;
  size_t aReaderPos = theReader.GetPosition();
  if (aBytesSpec < aReaderPos)
  {
#ifdef _DEBUG
    size_t aBytesRest = aReaderPos - aBytesSpec;
    cerr << "Error: " << (aBytesRest) << " extra bytes were read after end of the element";
#endif
    return false;
  }
  if (aBytesSpec > aReaderPos)
  {
    size_t aBytesRest = aBytesSpec - aReaderPos;
    WARNING(std::string("Warning: ") + std::to_string(aBytesRest) + " element bytes remained after reading, corrected");
    if (!theReader.Skip(aBytesRest))
      return false;
  }

  return true;
}

//=======================================================================
//function : FindSegment
//purpose  : Lookup offset of a segment in TOCs of this model and its ancestor models
//=======================================================================
std::shared_ptr<JtData_Model> JtData_Model::FindSegment(const Jt_GUID &theGUID,
                                               Jt_U64 &theOffset) const
{
  if (auto aTocIter = myTOC.find(theGUID); aTocIter != myTOC.end())
  {
    theOffset = aTocIter->second;
    return self();
  }
  else if (myParent)
    return myParent->FindSegment(theGUID, theOffset);
  else
  {
    char aGuidString[128];
    theGUID.ToString(aGuidString);
    ALARM(std::string("Error: Failed to find segment with GUID: ") + aGuidString);
    return std::shared_ptr<JtData_Model>();
  }
}

//=======================================================================
//function : ReadSegment
//purpose  : Read object from a late loaded segment
//=======================================================================
std::shared_ptr<JtData_Object> JtData_Model::ReadSegment(const Jt_U64 theOffset) const
{
    ifstream aFile;
    if (!open(aFile))
    {
        ALARM("Error: Failed to open Jt file of late-loaded segment");
        return std::shared_ptr<JtNode_Partition>();
    }

    return readSegment(aFile, theOffset, false);
}

//=======================================================================
//function : ReadSegment
//purpose  : Read object from a late loaded segment
//=======================================================================
std::shared_ptr<JtData_Object> JtData_Model::ReadSegmentDump(const Jt_U64 theOffset) const
{
    ifstream aFile;
    if (!open(aFile))
    {
        ALARM("Error: Failed to open Jt file of late-loaded segment");
        return std::shared_ptr<JtNode_Partition>();
    }

    return readSegmentDump(aFile, theOffset, false);
}

//=======================================================================
//function : Dump
//purpose  :
//=======================================================================
int JtData_Model::Dump(std::ostream & /*theStream*/) const
{
  return 0;
}
