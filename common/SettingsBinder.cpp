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

#include <common/SettingsBinder.h>

namespace KSRobot
{
namespace common
{

SettingsBinder::SettingsBinder()
{

}

SettingsBinder::~SettingsBinder()
{
    for(DataArray::iterator iter = mData.begin(); iter != mData.end(); iter++)
        delete *iter;
}

void SettingsBinder::ReadSettings()
{
    for(DataArray::iterator iter = mData.begin(); iter != mData.end(); iter++)
    {
        (*iter)->ReadFromFile(mPO);
    }
}

void SettingsBinder::WriteSettings()
{
    for(DataArray::iterator iter = mData.begin(); iter != mData.end(); iter++)
    {
        (*iter)->WriteToFile(mPO);
    }
}

void SettingsBinder::SetPO(ProgramOptions::Ptr po)
{
    mPO = po;
}

} // end namespace utils
} // end namespace KSRobot
