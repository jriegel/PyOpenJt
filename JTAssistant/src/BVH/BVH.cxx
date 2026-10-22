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

#include <BVH_Geometry.hxx>
#include <BVH_Triangulation.hxx>
#include <BVH_LinearBuilder.hxx>
#include <BVH_BinnedBuilder.hxx>
#include <BVH_SweepPlaneBuilder.hxx>
#include <BVH_SpatialMedianBuilder.hxx>

// Specific instantiations of struct templates to avoid compilation warnings

template class BVH_Box<double, 2>;
template class BVH_Box<double, 3>;
template class BVH_Box<double, 4>;

template class BVH_Box<float, 2>;
template class BVH_Box<float, 3>;
template class BVH_Box<float, 4>;

template class BVH_Set<double, 2>;
template class BVH_Set<double, 3>;
template class BVH_Set<double, 4>;

template class BVH_Set<float, 2>;
template class BVH_Set<float, 3>;
template class BVH_Set<float, 4>;

template class BVH_Object<double, 2>;
template class BVH_Object<double, 3>;
template class BVH_Object<double, 4>;

template class BVH_Object<float, 2>;
template class BVH_Object<float, 3>;
template class BVH_Object<float, 4>;

template class BVH_ObjectSet<double, 2>;
template class BVH_ObjectSet<double, 3>;
template class BVH_ObjectSet<double, 4>;

template class BVH_ObjectSet<float, 2>;
template class BVH_ObjectSet<float, 3>;
template class BVH_ObjectSet<float, 4>;

template class BVH_Geometry<double, 2>;
template class BVH_Geometry<double, 3>;
template class BVH_Geometry<double, 4>;

template class BVH_Geometry<float, 2>;
template class BVH_Geometry<float, 3>;
template class BVH_Geometry<float, 4>;

template class BVH_Tree<double, 2>;
template class BVH_Tree<double, 3>;
template class BVH_Tree<double, 4>;

template class BVH_Tree<float, 2>;
template class BVH_Tree<float, 3>;
template class BVH_Tree<float, 4>;

template class BVH_Builder<double, 2>;
template class BVH_Builder<double, 3>;
template class BVH_Builder<double, 4>;

template class BVH_Builder<float, 2>;
template class BVH_Builder<float, 3>;
template class BVH_Builder<float, 4>;

template class BVH_BinnedBuilder<double, 2>;
template class BVH_BinnedBuilder<double, 3>;
template class BVH_BinnedBuilder<double, 4>;

template class BVH_BinnedBuilder<float, 2>;
template class BVH_BinnedBuilder<float, 3>;
template class BVH_BinnedBuilder<float, 4>;

template class BVH_LinearBuilder<double, 3>;
template class BVH_LinearBuilder<double, 4>;

template class BVH_LinearBuilder<float, 3>;
template class BVH_LinearBuilder<float, 4>;

template class BVH_SweepPlaneBuilder<double, 2>;
template class BVH_SweepPlaneBuilder<double, 3>;
template class BVH_SweepPlaneBuilder<double, 4>;

template class BVH_SweepPlaneBuilder<float, 2>;
template class BVH_SweepPlaneBuilder<float, 3>;
template class BVH_SweepPlaneBuilder<float, 4>;

template class BVH_SpatialMedianBuilder<double, 2>;
template class BVH_SpatialMedianBuilder<double, 3>;
template class BVH_SpatialMedianBuilder<double, 4>;

template class BVH_SpatialMedianBuilder<float, 2>;
template class BVH_SpatialMedianBuilder<float, 3>;
template class BVH_SpatialMedianBuilder<float, 4>;

template class BVH_PrimitiveSet<double, 2>;
template class BVH_PrimitiveSet<double, 3>;
template class BVH_PrimitiveSet<double, 4>;

template class BVH_PrimitiveSet<float, 2>;
template class BVH_PrimitiveSet<float, 3>;
template class BVH_PrimitiveSet<float, 4>;

template class BVH_Triangulation<double, 2>;
template class BVH_Triangulation<double, 3>;
template class BVH_Triangulation<double, 4>;

template class BVH_Triangulation<float, 2>;
template class BVH_Triangulation<float, 3>;
template class BVH_Triangulation<float, 4>;

template class BVH_Transform<double, 4>;
template class BVH_Transform<float, 4>;
