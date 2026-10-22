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

#ifndef JTCOMMON_UTILS_H
#define JTCOMMON_UTILS_H

#include <vector>
#include <numeric>

#pragma warning (push, 0)
#include <QString>
#include <QSharedPointer>
#include <QElapsedTimer>
#include <QMap>
#pragma warning (pop)

#include <BVH/BVH_Box.hxx>

#include <JtElement_ShapeLOD_TriStripSet.hxx>

#include <Eigen/Core>

typedef BVH_Box<float, 4> JTCommon_AABB;

typedef std::vector<Eigen::Vector2f, Eigen::aligned_allocator<Eigen::Vector2f> > Array2f;
typedef std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f> > Array3f;
typedef std::vector<Eigen::Vector4f, Eigen::aligned_allocator<Eigen::Vector4f> > Array4f;

class JTCommon_TriangleData
{
public:

  std::shared_ptr<JtElement_ShapeLOD_TriStripSet> Data;
};

typedef QSharedPointer<JTCommon_TriangleData> JTCommon_TriangleDataPtr;

template <typename T>
class JTCommon_Callback
{
public:

  virtual ~JTCommon_Callback() {}

  virtual void Execute (T theParam) = 0;
};

struct JTCommon_CmdArgs
{
  QString FileName;

  bool DoBenchmarking;
};

class JTCommon_Profiler
{
private:

  JTCommon_Profiler()
  {
    //
  }

public:

  static JTCommon_Profiler& GetProfiler()
  {
    static JTCommon_Profiler* aProfiler = new JTCommon_Profiler();

    return *aProfiler;
  }

  void Start()
  {
    myTimer.start();
  }

  void WriteElapsed (QString theTag)
  {
    myValues.insert (theTag, myTimer.elapsed());
  }

  const QMap<QString, qint64>& Values() const
  {
    return myValues;
  }

protected:

  QElapsedTimer myTimer;
  QMap<QString, qint64> myValues;

};

#endif // JTCOMMON_UTILS_H
