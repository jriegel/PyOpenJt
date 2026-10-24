// JT format reading and visualization tools
// Copyright (C) 2014-2015 OPEN CASCADE SAS
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

#include <JtDecode_VertexData_Deering.hxx>

//#define NO_JT_MULTITHREADING
#include <JtData_Parallel.hxx>
#include <cmath>

namespace
{
  static const int TABLE_BITS = 13;

  struct SinCos
  {
    double Sin, Cos;
    void Set (double aValue)
    {
      Sin = sin (aValue);
      Cos = cos (aValue);
    }
  };

  static double Phi[16]= {0.904837, 0.818731, 0.740818,	0.670320,	0.606531,	0.548812, 0.449329, 0.367879,
                                 0.22313,  0.135335, 0.090484,	0.163746,	0.222245,	0.268128,	0.303265,	0.329287};
  class LookupTable
  {
    static const size_t BITS_SIZE = 16;
    static const size_t TABLE_SIZE = 1 << TABLE_BITS;
  public:
    SinCos Theta[TABLE_SIZE + BITS_SIZE];
    SinCos Psi  [TABLE_SIZE + BITS_SIZE];
    LookupTable()
    {
      double aMaxPsi     = 0.615479709;
      double aTableSizeF = (double) TABLE_SIZE;
      size_t i = 0;
      for ( ; i <= TABLE_SIZE; ++i)
      {
        Theta[i].Set (std::asin (std::tan (aMaxPsi * (TABLE_SIZE - i) / aTableSizeF)));
        Psi  [i].Set (aMaxPsi * (i / aTableSizeF));
      }
      size_t j = 0;
      for (i=TABLE_SIZE+1 ; i < TABLE_SIZE + BITS_SIZE; i++,j++)
      {
        Theta[i].Set (std::asin (std::tan (Phi[j])));
        Psi  [i].Set (Phi[j]);
      }
    }
  };

  static const LookupTable LOOKUP_TABLE;
}

JtDecode_VertexData_Deering::JtDecode_VertexData_Deering (const Jt_U8      theNbBits,
                                                          JtDecode_Unpack& theUnpacker,
                                                          const size_t theNbPackages)
  : JtDecode_VertexData (theNbPackages, theUnpacker)
  , myNbBits (theNbBits), myNbPkg (theNbPackages) {}

int JtDecode_VertexData_Deering::getOutCompCount (size_t)
{
  return 3;
}

void JtDecode_VertexData_Deering::decode (Decoded::Ref theResults)
{
  Jt_VecU32 aSextantCodes, anOctantCodes, aThetaCodes, aPsiCodes;
  Jt_VecI32 aPacked;

  if (myNbPkg == 1)
  {
    // JT 10.x: one package with sextant, octant, theta and psi packed into a single code
    aPacked = Jt_VecI32 (decodePackage (0));
  }
  else
  {
    JtData_Parallel::TaskGroup aDecodeTasks;
    aDecodeTasks.Run (getDecodingFunctor (0, aSextantCodes));
    aDecodeTasks.Run (getDecodingFunctor (1, anOctantCodes));
    aDecodeTasks.Run (getDecodingFunctor (2, aThetaCodes));
    aDecodeTasks.Run (getDecodingFunctor (3, aPsiCodes));
    aDecodeTasks.Wait();
  }

  const Jt_U32 aMask = (1u << myNbBits) - 1;
  int anOffset = TABLE_BITS - myNbBits;
  for (Decoded::SizeType i = 0; i < theResults.Count(); ++i)
  {
    Jt_U32 aSextant, anOctantCode, aThetaCode, aPsiCode;
    if (myNbPkg == 1)
    {
      const Jt_U32 aCode = static_cast<Jt_U32>(aPacked[i]);
      aPsiCode     = aCode & aMask;
      aThetaCode   = (aCode >> myNbBits) & aMask;
      anOctantCode = (aCode >> (2 * myNbBits)) & 7;
      aSextant     = (aCode >> (2 * myNbBits + 3)) & 7;
    }
    else
    {
      aSextant     = aSextantCodes[i];
      anOctantCode = anOctantCodes[i];
      aThetaCode   = aThetaCodes[i];
      aPsiCode     = aPsiCodes[i];
    }

    const SinCos& aThetaPar = LOOKUP_TABLE.Theta[(aThetaCode + (aSextant & 1)) << anOffset];
    const SinCos& aPsiPar   = LOOKUP_TABLE.Psi  [ aPsiCode                      << anOffset];

    Jt_F32 x = static_cast <Jt_F32> (aPsiPar.Cos * aThetaPar.Cos);
    Jt_F32 y = static_cast <Jt_F32> (aPsiPar.Sin);
    Jt_F32 z = static_cast <Jt_F32> (aPsiPar.Cos * aThetaPar.Sin);

    // Change coordinates based on the sextant
    Jt_DirF32 aResult;
    switch (aSextant)
    {
    default:
    case 0: // No op
      aResult.X = x;
      aResult.Y = y;
      aResult.Z = z;
      break;

    case 1: // Mirror about x=z plane
      aResult.X = z;
      aResult.Y = y;
      aResult.Z = x;
      break;

    case 2: // Rotate CW
      aResult.X = y;
      aResult.Y = z;
      aResult.Z = x;
      break;

    case 3: // Mirror about x=y plane
      aResult.X = y;
      aResult.Y = x;
      aResult.Z = z;
      break;

    case 4: // Rotate CCW
      aResult.X = z;
      aResult.Y = x;
      aResult.Z = y;
      break;

    case 5: // Mirror about y=z plane
      aResult.X = x;
      aResult.Y = z;
      aResult.Z = y;
      break;
    }

    // Change some more based on the octant
    Jt_U32 anOctant = anOctantCode;

    // if first bit is 0, negate x component
    if ((anOctant & 4) == 0)
      aResult.X = -aResult.X;

    // if second bit is 0, negate y component
    if ((anOctant & 2) == 0)
      aResult.Y = -aResult.Y;

    // if third bit is 0, negate z component
    if ((anOctant & 1) == 0)
      aResult.Z = -aResult.Z;

    theResults[i] = aResult;
  }
}
