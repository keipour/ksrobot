/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright (C) 2013  Kourosh <kourosh.sartipi@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef LOOPDETECTOR_H
#define LOOPDETECTOR_H

#include <common/Interface.h>
#include <common/VisualOdometryInterface.h>
#include <common/PCLUtils.h>
#include <tbb/concurrent_queue.h>
#include <boost/unordered_map.hpp>

namespace KSRobot
{
namespace common
{

class LoopDetectorInterface : public Interface
{
public:
    class LoopClosure
    {
    public:
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
        Eigen::Isometry3f               Transform; // Transform from 1 to 2
        size_t                          Cycle1;
        size_t                          Cycle2;
    };
    
    typedef LoopDetectorInterface                           this_type;
    typedef boost::shared_ptr<this_type>                    Ptr;
    typedef boost::shared_ptr<const this_type>              ConstPtr;
    
    LoopDetectorInterface();
    virtual ~LoopDetectorInterface();
    
    virtual void                                            RegisterToVO(VisualOdometryInterface::Ptr vo);
    boost::signals2::connection                             RegisterLoopReceiver(boost::function<void(const LoopClosure& lc)> fn);
    
    virtual bool                                            RunSingleCycle();
    
    virtual void                                            ReadSettings(ProgramOptions::Ptr po);
protected:
    struct KeyframeData
    {
        VisualKeyframe::Ptr             Keyframe;
        KinectPointCloud::ConstPtr      PointCloud;
        KinectGrayImage::ConstPtr       Image;
        KinectRawDepthImage::ConstPtr   RawDepth;
    };

    virtual void                                            ProcessFrame(const KeyframeData& kfdata) = 0;
    virtual void                                            CollectCandidates(size_t curr_cycle, std::vector<size_t>& candidates) = 0;
    //virtual void                                            Find
    
    VisualOdometryInterface::Ptr                            mVO;
private:
    void                                                    OnNewKeyframe();
    void                                                    PreprocessFrame(const KeyframeData& kd);
    bool                                                    CheckTransform(size_t prev_cycle, size_t curr_cycle, Eigen::Isometry3f& trans);
private:
    tbb::concurrent_queue<KeyframeData>                     mQueue;
    boost::signals2::signal<void(const LoopClosure& lc)>    mOnLoopDetected;
    boost::signals2::connection                             mConnection; // connection to VO
    GICP                                                    mGICP;
    
    typedef boost::unordered_map<size_t, pcl::PointCloud<pcl::PointXYZ>::Ptr >  PointCloudMap;
    PointCloudMap                                           mPointCloudMap;
    int                                                     mPointCloudDownsampleRate;
    CLASS_DEF_PYEXPORT;
};

};
};

#endif // LOOPDETECTOR_H
