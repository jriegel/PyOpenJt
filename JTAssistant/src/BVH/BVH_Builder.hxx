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

#ifndef _BVH_Builder_Header
#define _BVH_Builder_Header

#include <BVH_Set.hxx>
#include <BVH_Tree.hxx>

namespace BVH
{
  //! Minimum node size to split.
  const double THE_NODE_MIN_SIZE = 1e-5;
}

//! Performs construction of BVH tree using bounding
//! boxes (AABBs) of abstract objects.
//! \tparam T Numeric data type
//! \tparam N Vector dimension
template<class T, int N>
class BVH_Builder
{
public:

  //! Creates new abstract BVH builder.
  BVH_Builder (const int theLeafNodeSize,
               const int theMaxTreeDepth);

  //! Releases resources of BVH builder.
  virtual ~BVH_Builder();

  //! Builds BVH using specific algorithm.
  virtual void Build (BVH_Set<T, N>*       theSet,
                      BVH_Tree<T, N>*      theBVH,
                      const BVH_Box<T, N>& theBox) = 0;

protected:

  //! Updates depth of constructed BVH tree.
  void UpdateDepth (BVH_Tree<T, N>*        theBVH,
                    const int theLevel)
  {
    if (theLevel > theBVH->myDepth)
    {
      theBVH->myDepth = theLevel;
    }
  }

protected:

  int myMaxTreeDepth; //!< Maximum depth of constructed BVH
  int myLeafNodeSize; //!< Maximum number of objects per leaf

public:

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

};

#include <BVH_Builder.lxx>

#endif // _BVH_Builder_Header
