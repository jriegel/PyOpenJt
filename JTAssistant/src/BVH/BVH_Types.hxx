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

#ifndef _BVH_Types_Header
#define _BVH_Types_Header

#include <Eigen/Core>
#include <Eigen/StdVector>
#include <Eigen/LU>


#include <vector>

namespace BVH
{
  //! Tool class for selecting appropriate vector type (Eigen).
  //! \tparam T Numeric data type
  //! \tparam N Component number
  template<class T, int N> struct VectorType
  {
    // Not implemented
  };

  template<class T> struct VectorType<T, 1>
  {
    typedef T Type;
  };

  template<class T> struct VectorType<T, 2>
  {
    typedef Eigen::Matrix<T, 2, 1> Type;
  };

  template<class T> struct VectorType<T, 3>
  {
    typedef Eigen::Matrix<T, 3, 1> Type;
  };

  template<class T> struct VectorType<T, 4>
  {
    typedef Eigen::Matrix<T, 4, 1> Type;
  };

  //! Tool class for selecting appropriate matrix type (Eigen).
  //! \tparam T Numeric data type
  //! \tparam N Matrix dimension
  template<class T, int N> struct MatrixType
  {
    // Not implemented
  };

  template<class T> struct MatrixType<T, 4>
  {
    typedef Eigen::Matrix<T, 4, 4> Type;
  };

  //! Tool class for selecting type of array of vectors (STD vector).
  //! \tparam T Numeric data type
  //! \tparam N Component number
  template<class T, int N = 1> struct ArrayType
  {
    typedef std::vector<typename VectorType<T, N>::Type, Eigen::aligned_allocator<typename VectorType<T, N>::Type> > Type;
  };
}

//! 2D vector of integers.
typedef BVH::VectorType<int, 2>::Type BVH_Vec2i;
//! 3D vector of integers.
typedef BVH::VectorType<int, 3>::Type BVH_Vec3i;
//! 4D vector of integers.
typedef BVH::VectorType<int, 4>::Type BVH_Vec4i;

//! Array of 2D vectors of integers.
typedef BVH::ArrayType<int, 2>::Type BVH_Array2i;
//! Array of 3D vectors of integers.
typedef BVH::ArrayType<int, 3>::Type BVH_Array3i;
//! Array of 4D vectors of integers.
typedef BVH::ArrayType<int, 4>::Type BVH_Array4i;

//! 2D vector of single precision reals.
typedef BVH::VectorType<float, 2>::Type BVH_Vec2f;
//! 3D vector of single precision reals.
typedef BVH::VectorType<float, 3>::Type BVH_Vec3f;
//! 4D vector of single precision reals.
typedef BVH::VectorType<float, 4>::Type BVH_Vec4f;

//! Array of 2D vectors of single precision reals.
typedef BVH::ArrayType<float, 2>::Type BVH_Array2f;
//! Array of 3D vectors of single precision reals.
typedef BVH::ArrayType<float, 3>::Type BVH_Array3f;
//! Array of 4D vectors of single precision reals.
typedef BVH::ArrayType<float, 4>::Type BVH_Array4f;

//! 2D vector of double precision reals.
typedef BVH::VectorType<double, 2>::Type BVH_Vec2d;
//! 3D vector of double precision reals.
typedef BVH::VectorType<double, 3>::Type BVH_Vec3d;
//! 4D vector of double precision reals.
typedef BVH::VectorType<double, 4>::Type BVH_Vec4d;

//! Array of 2D vectors of double precision reals.
typedef BVH::ArrayType<double, 2>::Type BVH_Array2d;
//! Array of 3D vectors of double precision reals.
typedef BVH::ArrayType<double, 3>::Type BVH_Array3d;
//! Array of 4D vectors of double precision reals.
typedef BVH::ArrayType<double, 4>::Type BVH_Array4d;

//! 4x4 matrix of single precision reals.
typedef BVH::MatrixType<float, 4>::Type BVH_Mat4f;

//! 4x4 matrix of double precision reals.
typedef BVH::MatrixType<double, 4>::Type BVH_Mat4d;

namespace BVH
{
  template<class T, int N> struct MatrixOp
  {
    // Not implemented
  };

  template<class T> struct MatrixOp<T, 4>
  {
    typedef typename BVH::MatrixType<T, 4>::Type BVH_Mat4t;

    static void Inverse (const BVH_Mat4t& theIn,
                         BVH_Mat4t&       theOut)
    {
      theOut = theIn.inverse();
    }

    typedef typename BVH::VectorType<T, 4>::Type BVH_Vec4t;

    static BVH_Vec4t Multiply (const BVH_Mat4t& theMat,
                               const BVH_Vec4t& theVec)
    {
      BVH_Vec4t aOut = theMat * theVec;
      return aOut * static_cast<T> (1.0 / aOut.w());
    }
  };

  //! Tool class for accessing specific vector component (by index).
  //! \tparam T Numeric data type
  //! \tparam N Component number
  template<class T, int N> struct VecComp
  {
    // Not implemented
  };

  template<class T> struct VecComp<T, 2>
  {
    typedef typename BVH::VectorType<T, 2>::Type BVH_Vec2t;

    static T Get (const BVH_Vec2t& theVec, const int theAxis)
    {
      return theAxis == 0 ? theVec.x() : theVec.y();
    }
  };

  template<class T> struct VecComp<T, 3>
  {
    typedef typename BVH::VectorType<T, 3>::Type BVH_Vec3t;

    static T Get (const BVH_Vec3t& theVec, const int theAxis)
    {
      return theAxis == 0 ? theVec.x() : ( theAxis == 1 ? theVec.y() : theVec.z() );
    }
  };

  template<class T> struct VecComp<T, 4>
  {
    typedef typename BVH::VectorType<T, 4>::Type BVH_Vec4t;

    static T Get (const BVH_Vec4t& theVec, const int theAxis)
    {
      return theAxis == 0 ? theVec.x() :
        (theAxis == 1 ? theVec.y() : ( theAxis == 2 ? theVec.z() : theVec.w() ));
    }
  };

  //! Tool class providing typical operations on the array. It allows
  //! for interoperability between STD vector and NCollection vector.
  //! \tparam T Numeric data type
  //! \tparam N Component number
  template<class T, int N = 1> struct Array
  {
    typedef typename BVH::ArrayType<T, N>::Type BVH_ArrayNt;

    static inline const typename BVH::VectorType<T, N>::Type& Value (
        const BVH_ArrayNt& theArray, const int theIndex)
    {
      return theArray[theIndex];
    }

    static inline typename BVH::VectorType<T, N>::Type& ChangeValue (
      BVH_ArrayNt& theArray, const int theIndex)
    {
      return theArray[theIndex];
    }

    static inline void Append (BVH_ArrayNt& theArray,
      const typename BVH::VectorType<T, N>::Type& theElement)
    {
      theArray.push_back (theElement);
    }

    static inline int Size (const BVH_ArrayNt& theArray)
    {
      return static_cast<int> (theArray.size());
    }

    static inline void Clear (BVH_ArrayNt& theArray)
    {
      theArray.clear();
    }
  };

  template<class T>
  static inline int IntFloor (const T theValue)
  {
    const int aRes = static_cast<int> (theValue);

    return aRes - static_cast<int> (aRes > theValue);
  }
}

#endif // _BVH_Types_Header
