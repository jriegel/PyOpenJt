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

#ifndef _JtElement_ShapeLOD_Vertex_HeaderFile
#define _JtElement_ShapeLOD_Vertex_HeaderFile

#include <JtElement_ShapeLOD_Base.hxx>

#include <JtData_Types.hxx>
#include <JtData_CompVector.hxx>

//! Vertex Shape LOD Element represents LODs defined by collections of vertices.
class JtElement_ShapeLOD_Vertex : public JtElement_ShapeLOD_Base
{
public:
  typedef JtData_CompVector <float  , int32_t> VertexData;
  typedef JtData_Vector     <int32_t, int32_t> IndicesVec;

  class QuantizationParams
  {
    Jt_U8 myBitsPerVertex;
    Jt_U8 myNormalBitsFactor;
    Jt_U8 myBitsPerTextureCoord;
    Jt_U8 myBitsPerColor;

  public:
    bool Read (JtData_Reader& theReader);

    Jt_U8 BitsPerVertex()       { return myBitsPerVertex; }
    Jt_U8 NormalBitsFactor()    { return myNormalBitsFactor; }
    Jt_U8 BitsPerTextureCoord() { return myBitsPerTextureCoord; }
    Jt_U8 BitsPerColor()        { return myBitsPerColor; }

    int BitsPerNormal() { return 6 + 2 * (int) myNormalBitsFactor; }
  };

  class VertexBinding1
  {
    uint8_t myNormalBinding;
    uint8_t myTextCoordBinding;
    uint8_t myColorBinding;

  public:
    enum BindingType
    {
      None         = 0,
      PerVertex    = 1,
      PerFacet     = 2,
      PerPrimitive = 3
    };

    bool Read (JtData_Reader& theReader);

    BindingType NormalBinding()    { return (BindingType) myNormalBinding; }
    BindingType TextCoordBinding() { return (BindingType) myTextCoordBinding; }
    BindingType ColorBinding()     { return (BindingType) myColorBinding; }
  };

  class VertexBinding2
  {
    uint32_t                                              myTextFlags;
    struct {uint8_t Vertex:3, Normal:1, Color:2, Flag:1;} myLSB;
    struct {uint8_t Reserved:7, Auxiliary:1;}             myMSB;

  public:
    bool Read (JtData_Reader& theReader);

    bool IsNormalBinding();
    bool IsFlagBinding();
    bool IsAuxFieldBinding();
    size_t    NbVertexCoordComponents();
    size_t    NbColorComponents();
    size_t    NbTextCoordComponents (size_t theTexture);
  };

public:
  //! Read this entity from a JT file.
  TKJT_EXPORT bool Read (JtData_Reader &theReader) override;

  //! Dump this entity.
  TKJT_EXPORT int Dump (std::ostream& S) const override;

  //! Indices into the vertex parameters arrays.
  const IndicesVec& Indices()  const { return myIndices; }

  //! Vertex coordinates.
  const VertexData& Vertices() const { return myVertices; }

  //! Normals; can be empty if there is no normals data.
  const VertexData& Normals()  const { return myNormals; }

  DEFINE_OBJECT_CLASS (JtElement_ShapeLOD_Vertex)

protected:
  class VertexDataDecodeTask;
  class MeshDecodeTask;

  bool readVertexShapeLODData (
    JtData_Reader&   theReader,
    bool theIsTriStripSet = false);

  bool readVertexBasedShapeCompressedRepData (JtData_Reader& theReader);
  bool readTopologicallyCompressedData       (JtData_Reader& theReader);

protected:
  IndicesVec myIndices;  //!< Indices into the vertex parameters arrays
  VertexData myVertices; //!< vertex coordinates
  VertexData myNormals;  //!< normals; can be empty if there is no normals data
};


#endif // _JtElement_ShapeLOD_Vertex_HeaderFile
