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
#include <iostream>
#include <QDir>
#include <QStack>
#include <QFileInfo>
#pragma warning (pop)

#include <JTData_Node.hxx>
#include <JtNode_Part.hxx>
#include <JtNode_RangeLOD.hxx>
#include <JtNode_Instance.hxx>
#include <JtNode_Partition.hxx>
#include <JtNode_MetaData.hxx>
#include <JtNode_Shape_TriStripSet.hxx>
#include <JtElement_ShapeLOD_TriStripSet.hxx>
#include <JtElement_ProxyMetaData.hxx>
#include <JtAttribute_GeometricTransform.hxx>
#include <JtAttribute_Material.hxx>

#include "JTData_SceneGraph.hxx"

#include "../../ToolsLib/src/JtData2Json.h"


#include <limits>
#include <cmath>

using namespace Eigen;

// =======================================================================
// function : JTData_SceneGraph
// purpose  :
// =======================================================================
JTData_SceneGraph::JTData_SceneGraph()
  : myLoadingThread (myLoadingQueue)
{
}

// =======================================================================
// function : ~JTData_SceneGraph
// purpose  :
// =======================================================================
JTData_SceneGraph::~JTData_SceneGraph()
{
  myLoadingThread.quit();
  myLoadingThread.wait();
}

// =======================================================================
// function : FillGroup
// purpose  :
// =======================================================================
void JTData_SceneGraph::FillGroup (const JTData_GroupNodePtr& theGroupNode,
  const std::shared_ptr<JtNode_Group>& theGroupRecord, const QString& thePrefix)
{
  if (theGroupRecord->Children().IsEmpty())
    return;

  for (int aChildIdx = 0; aChildIdx < (int)theGroupRecord->Children().Count(); ++aChildIdx)
  {
    std::shared_ptr<JtNode_Base> aChildRecord = std::dynamic_pointer_cast<JtNode_Base>(theGroupRecord->Children()[aChildIdx]);

    if (!aChildRecord)
    {
      continue;
    }

    JTData_NodePtr aChildNode = PushNode (aChildRecord, thePrefix);

    Q_ASSERT_X (!aChildNode.isNull(),
      "PushNode", "Error! Failed to extract node from LSG segment");

    if (!aChildNode.isNull())
    {
      theGroupNode->Children.push_back (aChildNode);
    }
  }
}



// =======================================================================
// function : setNodeName
// purpose  : Auxiliary function to set the correct Node name
// =======================================================================
static void setNodeName (const std::shared_ptr<JtNode_Base>& theNodeRecord, 
                         const JTData_NodePtr&      theNode)
{
  QString aName (QString::fromStdWString (theNodeRecord->Name()));

  // Remove unnecessary suffixes from name
  if (aName.contains (".asm"))
  {
    aName.remove (".asm");
  }
  else if (aName.contains (".part"))
  {
    aName.remove (".part");
  }

  theNode->SetName (aName);
}

const std::vector<char> HandleLateLoadsMeta(const JtData_Object::VectorOfLateLoads& aLateLoaded)
{
    //static std::vector<char> EmptyVector;
    std::vector<char> ResultVector;
    int numOfMeta = 0;
    for (int i = 0; i < aLateLoaded.Count(); i++) {
        if (aLateLoaded[i]->getSegmentType() == SegmentType::Meta_Data) {
            if (aLateLoaded[i]->DefferedObject() == nullptr)
                aLateLoaded[i]->Load();

            std::shared_ptr<JtData_Object>  prop = aLateLoaded[i]->DefferedObject();
            if (prop && (dynamic_cast<const JtElement_ProxyMetaData*>(prop.get()) != nullptr)) {
                std::shared_ptr<JtElement_ProxyMetaData> aProxyMetaDataElement =
                    std::dynamic_pointer_cast<JtElement_ProxyMetaData>(prop);

                auto newProps = aProxyMetaDataElement->getKeyValueStream();

                ResultVector.insert(ResultVector.end(), newProps.begin(), newProps.end());
                numOfMeta++;
            }
        }
    }

    if (numOfMeta > 1)
        std::cout << "MoreMeta";

    return ResultVector;
}

// =======================================================================
// function : PushNode
// purpose  :
// =======================================================================
JTData_NodePtr JTData_SceneGraph::PushNode (const std::shared_ptr<JtNode_Base>& theNodeRecord, const QString& thePrefix)
{
  JTData_NodePtr aResult;

  // Extract LSG node

  if ((dynamic_cast<const JtNode_Partition*>(theNodeRecord.get()) != nullptr))
  {
    std::shared_ptr<JtNode_Partition> aPartitionRecord =
      std::dynamic_pointer_cast<JtNode_Partition>(theNodeRecord);

    QFileInfo anInfo (QString::fromStdWString (aPartitionRecord->FileName()));

    if (!aPartitionRecord->FileName().empty())
    {
      aResult = JTData_PartitionNodePtr (new JTData_PartitionNode (
        QDir::cleanPath (QDir (thePrefix).absoluteFilePath (anInfo.filePath()))));
    }

    Q_ASSERT_X (!aResult.isNull(),
      "PushNode", "Error! Failed to create partition node");

    FillGroup (aResult.dynamicCast<JTData_GroupNode>(), aPartitionRecord, thePrefix);
  }
  else if ((dynamic_cast<const JtNode_Part*>(theNodeRecord.get()) != nullptr))
  {
    std::shared_ptr<JtNode_Part> aPartRecord = std::dynamic_pointer_cast<JtNode_Part>(theNodeRecord);
    JTData_PartNodePtr aPartNode = JTData_PartNodePtr(new JTData_PartNode);
    aResult = JTData_PartNodePtr (aPartNode);
    const JtData_Object::VectorOfLateLoads& aLateLoaded = aPartRecord->LateLoads();

    // handle late loaded properties
    auto metaData = HandleLateLoadsMeta(aLateLoaded);
    std::stringstream tempOut;
    writeKeyValueStream(metaData, tempOut, 0);
    aResult->Properties = tempOut.str().c_str();

    // set the layer in part node
    
    aPartNode->setLayerNumbers(ScanForLayer(metaData));

    FillGroup (aResult.dynamicCast<JTData_GroupNode>(),
      std::dynamic_pointer_cast<JtNode_Group>(theNodeRecord), thePrefix);
  }
  else if ((dynamic_cast<const JtNode_MetaData*>(theNodeRecord.get()) != nullptr))
  {
      std::shared_ptr<JtNode_MetaData> aPartRecord = std::dynamic_pointer_cast<JtNode_MetaData>(theNodeRecord);
      const JtData_Object::VectorOfLateLoads& aLateLoaded = aPartRecord->LateLoads();
      aResult = JTData_GroupNodePtr(new JTData_GroupNode);

      // handle late loaded properties
      auto metaData = HandleLateLoadsMeta(aLateLoaded);
      std::stringstream tempOut;
      writeKeyValueStream(metaData, tempOut);
      aResult->Properties = tempOut.str().c_str();

      // handling layer in top meta data
      auto layerInfoTemp = ScanForLayerFilter(metaData);
      // set only the first
      if (layerInfo.LayerMap.size() == 0)
          layerInfo = layerInfoTemp;

      FillGroup(aResult.dynamicCast<JTData_GroupNode>(),
      std::dynamic_pointer_cast<JtNode_Group>(theNodeRecord), thePrefix);
  }
  else if ((dynamic_cast<const JtNode_RangeLOD*>(theNodeRecord.get()) != nullptr))
  {
    JTData_RangeLODNodePtr aRangeLOD = JTData_RangeLODNodePtr (new JTData_RangeLODNode);

    std::shared_ptr<JtNode_RangeLOD> aRangeLODRecord =
      std::dynamic_pointer_cast<JtNode_RangeLOD>(theNodeRecord);

    FillGroup (aRangeLOD, aRangeLODRecord, thePrefix);

    aRangeLOD->Center() = Eigen::Vector4f (
      static_cast<float> (aRangeLODRecord->Center().X),
      static_cast<float> (aRangeLODRecord->Center().Y),
      static_cast<float> (aRangeLODRecord->Center().Z),
      1.0);

    if (!aRangeLODRecord->RangeLimits().IsEmpty())
    {
      for (int anIdx = 1; anIdx <= aRangeLODRecord->RangeLimits().Count(); ++anIdx)
      {
        aRangeLOD->Ranges().push_back (aRangeLODRecord->RangeLimits()[anIdx]);
      }
    }
    else
    {
      aRangeLOD->Ranges().push_back (std::numeric_limits<float>::max());

      for (int anIdx = 1; anIdx < (int)aRangeLODRecord->Children().Count(); ++anIdx)
      {
        aRangeLOD->Ranges().push_back (0.0);
      }
    }

    aResult = aRangeLOD;
  }
  else if ((dynamic_cast<const JtNode_LOD*>(theNodeRecord.get()) != nullptr))
  {
    aResult = JTData_LODNodePtr (new JTData_LODNode);

    FillGroup (aResult.dynamicCast<JTData_GroupNode>(),
      std::dynamic_pointer_cast<JtNode_Group>(theNodeRecord), thePrefix);
  }
  else if ((dynamic_cast<const JtNode_Group*>(theNodeRecord.get()) != nullptr))
  {
    aResult = JTData_GroupNodePtr (new JTData_GroupNode);

    FillGroup (aResult.dynamicCast<JTData_GroupNode>(),
      std::dynamic_pointer_cast<JtNode_Group>(theNodeRecord), thePrefix);
  }
  else if ((dynamic_cast<const JtNode_Instance*>(theNodeRecord.get()) != nullptr))
  {
    std::shared_ptr<JtNode_Instance> anInstance = std::dynamic_pointer_cast<JtNode_Instance>(theNodeRecord);

    // Note: To support JT Viewer operations (such as object hiding) it is convenient
    // to eliminate instance nodes from the scene graph. In result, using of sub-tree
    // references becomes impossible. But the actual geometric data is not duplicated
    // by decomposing the node on the description part and data source part.

    std::shared_ptr<JtNode_Base> aNode = std::dynamic_pointer_cast<JtNode_Base>(anInstance->Object());

    Q_ASSERT_X (aNode,
      "PushNode", "Error! Invalid object in LSG segment");

    aResult = PushNode (aNode, thePrefix);
  }
  else if ((dynamic_cast<const JtNode_Shape_Vertex*>(theNodeRecord.get()) != nullptr))
  {
    std::shared_ptr<JtNode_Shape_Vertex> aShapeRecord = std::dynamic_pointer_cast<JtNode_Shape_Vertex>(theNodeRecord);

    if ((dynamic_cast<const JtNode_Shape_TriStripSet*>(aShapeRecord.get()) != nullptr))
    {
      std::shared_ptr<JtNode_Shape_TriStripSet> aMeshRecord = std::dynamic_pointer_cast<JtNode_Shape_TriStripSet>(aShapeRecord);

      JTCommon_AABB aBox (
        Eigen::Vector4f (aMeshRecord->Bounds().MinCorner.X,
                         aMeshRecord->Bounds().MinCorner.Y,
                         aMeshRecord->Bounds().MinCorner.Z,
                         1.f),
        Eigen::Vector4f (aMeshRecord->Bounds().MaxCorner.X,
                         aMeshRecord->Bounds().MaxCorner.Y,
                         aMeshRecord->Bounds().MaxCorner.Z,
                         1.f));

      if (mySources.find (aMeshRecord) == mySources.end())
      {
        JTData_MeshNodeSourcePtr aMeshSource (
          new JTData_MeshNodeSource (myLoadingQueue, aMeshRecord));

        // Add to new mesh source to the map (can be reused)
        mySources.emplace (aMeshRecord, aMeshSource);
      }

      aResult = JTData_MeshNodePtr (new JTData_MeshNode (
        aBox, mySources.at (aMeshRecord)));
    }
  }

  Q_ASSERT_X (!aResult.isNull(),
    "PushNode", "Error! Unknown type of LSG node");

  setNodeName (theNodeRecord, aResult);

  // Extract attributes

  if (theNodeRecord->Attributes().IsEmpty())
    return aResult;

  for (int anIdx = 0; anIdx < (int)theNodeRecord->Attributes().Count(); ++anIdx)
  {
    const std::shared_ptr<JtData_Object>& anObject = theNodeRecord->Attributes()[anIdx];

    if (!anObject)
      continue;

    std::shared_ptr<JtAttribute_Base> anAttrib = std::dynamic_pointer_cast<JtAttribute_Base>(anObject);

    Q_ASSERT_X (anAttrib,
      "PushNode", "Error! Invalid node attribute");

    if ((dynamic_cast<const JtAttribute_GeometricTransform*>(anAttrib.get()) != nullptr))
    {
      std::shared_ptr<JtAttribute_GeometricTransform> aTransform =
        std::dynamic_pointer_cast<JtAttribute_GeometricTransform>(anAttrib);

      Eigen::Matrix4f aMatrix;

      for (int aX = 0; aX < 4; ++aX)
      {
        for (int aY = 0; aY < 4; ++aY)
        {
          aMatrix (aX, aY) = static_cast<float> (aTransform->GetTrsf()[aY * 4 + aX]);
        }
      }

      if (aMatrix (3, 3) == 0.f)
      {
        aMatrix (3, 3) = 1.f; // fix problem with homogeneous coordinates
      }

      aResult->Attributes.push_back (JTData_TransformAttributePtr (new JTData_TransformAttribute (aMatrix)));
    }
    else if ((dynamic_cast<const JtAttribute_Material*>(anAttrib.get()) != nullptr))
    {
      std::shared_ptr<JtAttribute_Material> aMaterial =
        std::dynamic_pointer_cast<JtAttribute_Material>(anAttrib);

      Eigen::Vector4f aAmbientColor (
        static_cast<float> (aMaterial->AmbientColor()[0]),
        static_cast<float> (aMaterial->AmbientColor()[1]),
        static_cast<float> (aMaterial->AmbientColor()[2]),
        static_cast<float> (aMaterial->AmbientColor()[3]));

      Eigen::Vector4f aDiffuseColor (
        static_cast<float> (aMaterial->DiffuseColor()[0]),
        static_cast<float> (aMaterial->DiffuseColor()[1]),
        static_cast<float> (aMaterial->DiffuseColor()[2]),
        static_cast<float> (aMaterial->DiffuseColor()[3]));

      Eigen::Vector4f aSpecularColor (
        static_cast<float> (aMaterial->SpecularColor()[0]),
        static_cast<float> (aMaterial->SpecularColor()[1]),
        static_cast<float> (aMaterial->SpecularColor()[2]),
        static_cast<float> (aMaterial->SpecularColor()[3]));

      Eigen::Vector4f aEmissionColor (
        static_cast<float> (aMaterial->EmissionColor()[0]),
        static_cast<float> (aMaterial->EmissionColor()[1]),
        static_cast<float> (aMaterial->EmissionColor()[2]),
        static_cast<float> (aMaterial->EmissionColor()[3]));

      float aShininess =
        static_cast<float> (aMaterial->Shininess());

      JTData_MaterialAttributePtr aMaterialAttrib (new JTData_MaterialAttribute (
        aAmbientColor, aDiffuseColor, aSpecularColor, aEmissionColor, aShininess));

      aResult->Attributes.push_back (aMaterialAttrib);
    }
  }

  return aResult;
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
bool JTData_SceneGraph::Init (const std::shared_ptr<JtNode_Partition>& thePartitionRecord, JTData_PartitionNode* thePartition)
{
  if (!thePartitionRecord)
  {
    return false;
  }

  JTData_PartitionNodePtr aRoot = PushNode (thePartitionRecord,
    QFileInfo (thePartition->FileName()).path()).dynamicCast<JTData_PartitionNode>();

  if (aRoot.isNull())
  {
    return false;
  }

  for (size_t anIdx = 0; anIdx < aRoot->Children.size(); ++anIdx)
  {
    thePartition->Children.push_back (aRoot->Children.at (anIdx));
  }

  return true;
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
bool JTData_SceneGraph::Init (const std::shared_ptr<JtData_Model>& theModel, const QString& theFileName)
{
  if (!myRoot.isNull())
    myRoot.reset();

  std::shared_ptr<JtNode_Partition> aNode = theModel->Init();

  if (aNode)
  {
    
    myRoot = PushNode (aNode, QFileInfo (theFileName).path());
  }

  Q_ASSERT_X (!myRoot.isNull(),
    "Init", "Error! failed to create root node for LSG segment");

  return true;
}

// =======================================================================
// function : GenerateRanges
// purpose  :
// =======================================================================
void GenerateRanges (const JTData_RangeLODNodePtr& theRangeLOD,
  const JTCommon_AABB& theGlobalBox, const float theScale)
{
  const float aLength = theGlobalBox.Size().norm();

  theRangeLOD->Ranges().clear();

  for (size_t anIdx = 0; anIdx < theRangeLOD->Children.size() - 1; ++anIdx)
  {
    theRangeLOD->Ranges().push_back (aLength / (theRangeLOD->Children.size() - anIdx) * theScale);
  }
}

// =======================================================================
// function : GenerateRanges
// purpose  :
// =======================================================================
void JTData_SceneGraph::GenerateRanges (
  const JTCommon_AABB& theGlobalBox, const float theScale)
{
  QStack<JTData_NodePtr> aStack;

  if (myRoot.isNull())
  {
    return;
  }

  aStack.push (myRoot);

  for (;;)
  {
    if (aStack.isEmpty())
    {
      break;
    }

    JTData_NodePtr aNode = aStack.pop();

    if (!aNode.dynamicCast<JTData_RangeLODNode>().isNull())
    {
      JTData_RangeLODNodePtr aRangeLOD = aNode.dynamicCast<JTData_RangeLODNode>();

      ::GenerateRanges (aRangeLOD, theGlobalBox, theScale);
    }
    else if (!aNode.dynamicCast<JTData_InstanceNode>().isNull())
    {
      JTData_InstanceNodePtr anInstance = aNode.dynamicCast<JTData_InstanceNode>();

      aStack.push (anInstance->Reference);
    }

    if (!aNode.dynamicCast<JTData_GroupNode>().isNull())
    {
      JTData_GroupNodePtr aGroup = aNode.dynamicCast<JTData_GroupNode>();

      for (size_t anIdx = 0; anIdx < aGroup->Children.size(); ++anIdx)
      {
        aStack.push (aGroup->Children.at (anIdx));
      }
    }
  }
}

// =======================================================================
// function : Clear
// purpose  :
// =======================================================================
void JTData_SceneGraph::Clear()
{
  myRoot.reset();
}

// =======================================================================
// function : EstimateMemoryUsed
// purpose  :
// =======================================================================
int JTData_SceneGraph::EstimateMemoryUsed() const
{
  JTData_InstanceMap aMap;

  if (!myRoot.isNull())
  {
    return sizeof (JTData_SceneGraph) + myRoot->EstimateMemoryUsed (aMap);
  }

  return sizeof (JTData_SceneGraph);
}
