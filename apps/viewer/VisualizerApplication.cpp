#include "VisualizerApplication.h"
#include <gui/Utils.h>

VisualizerApplication::VisualizerApplication(int& argc, char** argv, int oa) :
    QApplication(argc, argv, oa), mPO(new KSRobot::common::ProgramOptions())
{
    KSRobot::gui::Utils::RegisterDefaultQtTypes();
}

VisualizerApplication::~VisualizerApplication()
{
    if( mSettingsFileName != "" )
        mPO->SaveToFile(mSettingsFileName.c_str());
}

void VisualizerApplication::SetSettingsFileName(const std::string& fn)
{
    mSettingsFileName = fn;
    mPO->LoadFromFile(mSettingsFileName.c_str());
}


