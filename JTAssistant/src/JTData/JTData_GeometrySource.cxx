// JT format reading and visualization tools
// Copyright (C) 2015 OPEN CASCADE SAS
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

#pragma warning (push, 0)
#include <QStack>
#include <QFileInfo>
#pragma warning (pop)

#include "JTData_GeometrySource.hxx"

//=======================================================================
// function : JTData_GeometrySource
// purpose  :
//=======================================================================
JTData_GeometrySource::JTData_GeometrySource()
  : mySceneGraph (NULL)
{
  //
}

//=======================================================================
// function : ~JTData_GeometrySource
// purpose  :
//=======================================================================
JTData_GeometrySource::~JTData_GeometrySource()
{
  delete mySceneGraph;
}

//=======================================================================
// function : LoadPartition
// purpose  :
//=======================================================================
bool JTData_GeometrySource::LoadPartition (JTData_PartitionNode* thePartition)
{
  if (thePartition == NULL)
  {
    return false;
  }

  QMap<QString, std::shared_ptr<JtNode_Partition>>::iterator anIter = myMap.find (thePartition->FileName());

  if (anIter == myMap.end())
  {
    std::wstring aFileName = thePartition->FileName().toStdWString();

    std::shared_ptr<JtData_Model> aModel = std::make_shared<JtData_Model> (aFileName);

    std::shared_ptr<JtNode_Partition> aPartitionRecord = aModel->Init();

    if (!aPartitionRecord)
    {
      return false;
    }

    myMap.insert (thePartition->FileName(), aPartitionRecord);
  }

  const std::shared_ptr<JtNode_Partition>& aPartitionRecord = myMap.find (thePartition->FileName()).value();

  if (!aPartitionRecord)
  {
    return false;
  }

  return mySceneGraph->Init (aPartitionRecord, thePartition);
}

//=======================================================================
// function : LoadExternalPartitions
// purpose  :
//=======================================================================
bool JTData_GeometrySource::LoadExternalPartitions()
{
  if (mySceneGraph == NULL)
  {
    return true;
  }

  JTData_PartitionNode* aRoot =
    dynamic_cast<JTData_PartitionNode*> (mySceneGraph->Tree().data());

  if (aRoot == NULL)
  {
    return false;
  }

  QStack<JTData_Node*> aStack;

  for (size_t anIdx = 0; anIdx < aRoot->Children.size(); ++anIdx)
  {
    aStack.push (aRoot->Children.at (anIdx).data());
  }

  for (;;)
  {
    if (aStack.isEmpty())
      break;

    JTData_GroupNode* aGroup = dynamic_cast<JTData_GroupNode*> (aStack.pop());

    if (aGroup != NULL)
    {
      JTData_PartitionNode* aPartition = dynamic_cast<JTData_PartitionNode*> (aGroup);

      if (aPartition != NULL)
      {
        if (!LoadPartition (aPartition))
        {
          return false;
        }
      }
      else
      {
        for (size_t anIdx = 0; anIdx < aGroup->Children.size(); ++anIdx)
        {
          aStack.push (aGroup->Children.at (anIdx).data());
        }
      }
    }
  }

  return true;
}

//=======================================================================
// function : Init
// purpose  :
//=======================================================================
bool JTData_GeometrySource::Init (const QString& theFileName)
{
  myMap.clear();

  if (mySceneGraph != NULL)
  {
    delete mySceneGraph;
  }

#ifdef _WIN32
  std::ifstream aFile (theFileName.toStdWString().c_str(), std::ios::binary | std::ios::in);
#else
  ifstream aFile (theFileName.toUtf8().data(), ios::binary | ios::in);
#endif

  if (!aFile.is_open())
  {
    return false;
  }

#ifdef _WIN32
  std::wstring aFileName = theFileName.toStdWString();
#else
  std::wstring aFileName = theFileName.toStdWString();
#endif

  std::shared_ptr<JtData_Model> aModel = std::make_shared<JtData_Model> (aFileName);

  if (!aModel)
  {
    return false;
  }

  mySceneGraph = new JTData_SceneGraph;

  if (!mySceneGraph->Init (aModel, theFileName))
  {
    return false;
  }

  return LoadExternalPartitions();
}
