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

#include "JTData_Node.hxx"

// =======================================================================
// function : ~JTData_Node
// purpose  :
// =======================================================================
JTData_Node::~JTData_Node()
{
  //
}

// =======================================================================
// function : EstimateMemoryUsed
// purpose  :
// =======================================================================
int JTData_GroupNode::EstimateMemoryUsed (JTData_InstanceMap& theMap) const
{
  int aSize = sizeof (JTData_GroupNode);

  for (size_t anIdx = 0; anIdx < Children.size(); ++anIdx)
  {
    aSize += Children.at (anIdx)->EstimateMemoryUsed (theMap) + sizeof (JTData_NodePtr);
  }

  return aSize;
}

void JTData_GroupNode::applayLayerFilter(const std::vector<uint32_t>& LayerInfo)
{
    for (auto child : Children)
        child->applayLayerFilter(LayerInfo);
}

// =======================================================================
// function : EstimateMemoryUsed
// purpose  :
// =======================================================================
int JTData_InstanceNode::EstimateMemoryUsed (JTData_InstanceMap& /*theMap*/) const
{
  return sizeof (JTData_InstanceNode) + sizeof (JTData_NodePtr);
}

void JTData_InstanceNode::applayLayerFilter(const std::vector<uint32_t>& LayerInfo)
{
    Reference->applayLayerFilter(LayerInfo);
}

// =======================================================================
// function : JTData_PartitionNode
// purpose  :
// =======================================================================
JTData_PartitionNode::JTData_PartitionNode (const QString& theFileName)
  : JTData_GroupNode(),
    myFileName (theFileName)
{
  //
}



// =======================================================================
// function : EstimateMemoryUsed
// purpose  :
// =======================================================================
int JTData_PartitionNode::EstimateMemoryUsed (JTData_InstanceMap& theMap) const
{
  return JTData_GroupNode::EstimateMemoryUsed (theMap) + myFileName.toUtf8().length();
}


void JTData_PartNode::applayLayerFilter(const std::vector<uint32_t>& LayerInfo)
{
    SetVisible(false);
    for (int layerInfor : LayerInfo) {
        for (int layerNum : LayerNumbers) {
            if (layerInfor == layerNum) {
                SetVisible(true);
                goto end;
            }
        }
    }
end:
    return;
}

// =======================================================================
// function : ~JTData_ShapeNode
// purpose  :
// =======================================================================
JTData_ShapeNode::~JTData_ShapeNode()
{
  //
}

// =======================================================================
// function : EstimateMemoryUsed
// purpose  :
// =======================================================================
int JTData_ShapeNode::EstimateMemoryUsed (JTData_InstanceMap& /*theMap*/) const
{
  return sizeof (JTData_ShapeNode) + sizeof (JTCommon_AABB);
}

// =======================================================================
// function : JTData_MeshNode
// purpose  :
// =======================================================================
JTData_MeshNode::JTData_MeshNode (const JTCommon_AABB& theBox, const JTData_MeshNodeSourcePtr& theSource)
  : JTData_ShapeNode (theBox),
    mySource (theSource)
{
  //
}

// =======================================================================
// function : ~JTData_MeshNode
// purpose  :
// =======================================================================
JTData_MeshNode::~JTData_MeshNode()
{
  //
}

// =======================================================================
// function : EstimateMemoryUsed
// purpose  :
// =======================================================================
int JTData_MeshNode::EstimateMemoryUsed (JTData_InstanceMap& theMap) const
{
  int aSize = JTData_ShapeNode::EstimateMemoryUsed (theMap);

  if (mySource.isNull() || theMap.contains (mySource.data()))
    return aSize;

  JTCommon_TriangleDataPtr aData = mySource->Triangulation();

  if (!aData.isNull())
  {
    aSize += aData->Data->Vertices().Count() * 24 + aData->Data->Indices().Count() * 4;
  }

  theMap.insert (mySource.data());

  return aSize;
}

// =======================================================================
// function : JTData_MeshNodeSource
// purpose  :
// =======================================================================
JTData_MeshNodeSource::JTData_MeshNodeSource (JTData_LoadingQueue& theQueue, const std::shared_ptr<JtNode_Shape_TriStripSet>& theShape)
  : myQueue (theQueue),
    myShape (theShape)
{
  myTriangleCount = 0;
}

// =======================================================================
// function : RequestTriangulation
// purpose  :
// =======================================================================
JTCommon_TriangleDataPtr JTData_MeshNodeSource::RequestTriangulation (const int theIndex, QObject* theFeedback)
{
  if (myData.isNull())
  {
    const JtData_Object::VectorOfLateLoads& aLateLoaded = myShape->LateLoads();

    if (aLateLoaded.IsEmpty())
    {
      return myData;
    }

    std::shared_ptr<JtData_Object> anObject = aLateLoaded[theIndex]->DefferedObject();

    if (anObject)
    {
      std::shared_ptr<JtElement_ShapeLOD_TriStripSet> aLOD =
        std::dynamic_pointer_cast<JtElement_ShapeLOD_TriStripSet>(anObject);

      if (aLOD)
      {
        BuildTriangulation (aLOD);
      }

      aLateLoaded[theIndex]->Unload();
    }
    else
    {
      if (myQueue.Size() < 1000) // fix overflow of the loading queue
      {
        JTData_WorkItem anItem (aLateLoaded[theIndex], theFeedback);

        if (!myQueue.Enqueued (anItem))
        {
          myQueue.Enqueue (anItem);
        }
      }
    }
  }

  return myData;
}

// =======================================================================
// function : BuildTriangulation
// purpose  :
// =======================================================================
void JTData_MeshNodeSource::BuildTriangulation (const std::shared_ptr<JtElement_ShapeLOD_TriStripSet>& theShapeLOD)
{
  if (!theShapeLOD)
  {
    return;
  }

  myData.reset (new JTCommon_TriangleData);

  myData->Data = theShapeLOD;

  myTriangleCount = theShapeLOD->Indices().Count() / 3;
}
