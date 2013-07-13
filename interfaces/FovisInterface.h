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

#ifndef FOVISINTERFACE_H
#define FOVISINTERFACE_H

#include <common/VisualOdometryInterface.h>
#include <common/Timer.h>
#include <boost/shared_array.hpp>
#include <Eigen/Geometry>

namespace KSRobot
{
namespace interfaces
{

class FovisInterface : public common::VisualOdometryInterface
{
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
    FovisInterface();
    virtual ~FovisInterface();
    
    virtual void        RegisterToKinect(common::KinectInterface::Ptr ki);

    virtual bool        Converged();
    virtual float       GetConvergenceError();
    
    virtual bool        RunSingleCycle();
protected:
    virtual bool        CheckForKeyframe();
    virtual void        PublishKeyframeFeatures(common::VisualKeyframe::Ptr kf);
protected:
    
    class FovisImpl;
    boost::shared_ptr<FovisImpl>                mImpl;

    common::Timer::Ptr                          mDataCopyTimer;
};

#endif // FOVISINTERFACE_H

} // end namespace utils
} // end namespace KSRobot
