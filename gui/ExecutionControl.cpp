#include <gui/ExecutionControl.h>
#include <gui/Utils.h>

#include <QVBoxLayout>
#include <QRect>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>

#include <boost/graph/graph_concepts.hpp>

using namespace KSRobot;
using namespace gui;
using namespace common;

ExecutionControl::ExecutionControl(QWidget* parent, Qt::WindowFlags f) : KWidgetBase(parent, f)
{
    qRegisterMetaType<common::ExecCtrlData>("ExecCtrlData");
}

ExecutionControl::~ExecutionControl()
{
}

void ExecutionControl::InitControl(ProgramOptions::Ptr po)
{
    KWidgetBase::InitControl(po);

    mUI = new Ui::ExecutionControl();
    
    mUI->setupUi(this);
    mUI->retranslateUi(this);
    
    if( mGuiPO->GetString("DefaultSource", std::string("file")) == "kinect" )
        mUI->mRdBtnKinectDevice->setChecked(true);
    else
        mUI->mRdBtnKinectFile->setChecked(true);
    
    mBinderFns.SetPO(mGuiPO);
    mBinderVars.SetPO(mGuiPO);
    
    // attack paths to controls
    BindValueToName("Kinect.GetFromDevice", mUI->mRdBtnKinectDevice);
    BindValueToName("Fovis.Enable", mUI->mChkEnableFovis);
    BindValueToName("iSAM.Enable", mUI->mChkEnableSAM);
    BindValueToName("OctoMap.Enable", mUI->mChkEnableOctomap);
    BindValueToName("OMPL.Enable", mUI->mChkEnableOMPL);
    BindValueToName("Comm.Enable", mUI->mChkEnableComm);
    BindValueToName("GUI.ViewFovis", mUI->mChkViewFovisResults);
    BindValueToName("GUI.ViewOctoMap", mUI->mChkViewOctomap);
    BindValueToName("GUI.ViewOMPL", mUI->mChkViewOMPL);
    BindValueToName("GUI.ViewPCL", mUI->mChkViewPointCloud);
    BindValueToName("GUI.ViewRGBD", mUI->mChkViewRGBD);
    
    mBinderVars.AddVariableSetting("Kinect.GetFromDevice", &(mData.Kinect.GetFromDevice), false);
    mBinderVars.AddVariableSetting("Kinect.SourceDevice", &(mData.Kinect.SourceDevice), std::string(""));
    mBinderVars.AddVariableSetting("Kinect.SourceDir", &(mData.Kinect.SourceDir), 
                                   std::string("/windows/E/Datasets/rgbd_dataset_freiburg2_pioneer_slam/"));
    mBinderVars.AddVariableSetting("Fovis.Enable", &(mData.Fovis.Enable), false);
    mBinderVars.AddVariableSetting("iSAM.Enable", &(mData.iSAM.Enable), false);
    mBinderVars.AddVariableSetting("OctoMap.Enable", &(mData.OctoMap.Enable), false);
    mBinderVars.AddVariableSetting("OMPL.Enable", &(mData.OMPL.Enable), false);
    mBinderVars.AddVariableSetting("Comm.Enable", &(mData.Comm.Enable), false);
    mBinderVars.AddVariableSetting("Comm.Address", &(mData.Comm.Address), std::string("127.0.0.1"));
    mBinderVars.AddVariableSetting("Comm.Port", &(mData.Comm.Port), 0);
    mBinderVars.AddVariableSetting("GUI.ViewFovis", &(mData.GUI.ViewFovis), false);
    mBinderVars.AddVariableSetting("GUI.ViewOctoMap", &(mData.GUI.ViewOctoMap), false);
    mBinderVars.AddVariableSetting("GUI.ViewOMPL", &(mData.GUI.ViewOMPL), false);
    mBinderVars.AddVariableSetting("GUI.ViewPCL", &(mData.GUI.ViewPCL), false);
    mBinderVars.AddVariableSetting("GUI.ViewRGBD", &(mData.GUI.ViewRGBD), false);
}

void ExecutionControl::ReadSettings()
{
    mBinderVars.ReadSettings();
}

void ExecutionControl::SaveSettings()
{
    mBinderFns.WriteSettings();
}

void ExecutionControl::BindSettings()
{
    
}

void ExecutionControl::BindValueToName(const std::string& name, QAbstractButton* btn)
{
    mBinderFns.AddFunctionSetting<bool>(name, 
                            boost::bind(&QAbstractButton::isChecked, btn), 
                            boost::bind(&QAbstractButton::setChecked, btn, _1));
}

bool ExecutionControl::UpdateUIFromData()
{    
    //TODO: Implemet loading UIData from settings, possibly inside another function
    try
    {
        mData.CheckConsistancy(); // First check to see if the data is correct
        
        if( mData.Kinect.GetFromDevice )
        {
            mUI->mRdBtnKinectDevice->setChecked(true);
            mUI->mKinectDevPath->setText(mData.Kinect.SourceDevice.c_str());
        }
        else
        {
            mUI->mRdBtnKinectFile->setChecked(true);
            mUI->mKinectDevPath->setText(mData.Kinect.SourceDir.c_str());
        }
        
        mBinderFns.ReadSettings();
        
    }
    catch(std::runtime_error& re)
    {
        QMessageBox::critical(this, "ExecutionControl::UpdateUIFromData", QString("Invalid settings: ") + re.what());
        return false;
    }
    
    return false;
}

bool ExecutionControl::UpdateDataFromUI()
{
    try
    {
        // Update Kinect
        mData.Kinect.GetFromDevice = mUI->mRdBtnKinectDevice->isChecked();
        if( mData.Kinect.GetFromDevice )
            mData.Kinect.SourceDevice = mUI->mKinectDevPath->text().toStdString();
        else
            mData.Kinect.SourceDir = mUI->mKinectDevPath->text().toStdString();

        mBinderFns.WriteSettings(); // write to po
        mBinderVars.ReadSettings(); // read data from po
        mData.CheckConsistancy();
    }
    catch(std::runtime_error& re)
    {
        QMessageBox::critical(this, "ExecutionControl::UpdateDataFromUI", QString("Invalid settings: ") + re.what());
        return false;
    }
    return true;
}

void ExecutionControl::on_mBtnStart_clicked()
{
    if( !UpdateDataFromUI() )
        return;
    
    mUI->mTabSettings->setEnabled(false);
    mUI->mBtnStart->setEnabled(false);
    mUI->mBtnStop->setEnabled(true);
    
    emit OnStart(mData);
}

void ExecutionControl::on_mBtnStop_clicked()
{
    emit OnStop();
    GuiStop();
}

void ExecutionControl::GuiStop()
{
    mUI->mTabSettings->setEnabled(true);
    mUI->mBtnStart->setEnabled(true);
    mUI->mBtnStop->setEnabled(false);
}

void ExecutionControl::on_mRdBtnKinectDevice_clicked()
{
}

void ExecutionControl::on_mRdBtnKinectFile_clicked()
{
}

void ExecutionControl::on_mBtnBrowseKinect_clicked()
{
    QString dir;
    if( mUI->mRdBtnKinectFile->isChecked() )
    {
//         std::string defDir = mGuiPO->GetString("DefaultDirPath",
//                                 std::string("/windows/E/Datasets/rgbd_dataset_freiburg2_pioneer_slam/"));
        
        dir = QFileDialog::getExistingDirectory(this, tr("Select the path to dataset"),
                                                mUI->mKinectDevPath->text());
        
        mData.Kinect.SourceDir = dir.toStdString();
        mUI->mKinectDevPath->setText(dir);
    }
    else
    {
        qDebug() << "(ExecutionControl::on_mBtnBrowseKinect_clicked) Not implemented for kinect device.";
        mData.Kinect.SourceDevice = "";
        mUI->mKinectDevPath->setText("");
    }
}

void ExecutionControl::on_mChkEnableFovis_clicked()
{
}

void ExecutionControl::on_mChkEnableSAM_clicked()
{
}

void ExecutionControl::on_mChkEnableOctomap_clicked()
{
}

void ExecutionControl::on_mChkEnableOMPL_clicked()
{
}

void ExecutionControl::on_mChkEnableComm_clicked()
{
}

void ExecutionControl::on_mChkViewRGBD_clicked()
{
}

void ExecutionControl::on_mChkViewPointCloud_clicked()
{
}

void ExecutionControl::on_mChkViewFovisResults_clicked()
{
}

void ExecutionControl::on_mChkViewOctomap_clicked()
{
}

void ExecutionControl::on_mChkViewOMPL_clicked()
{
}
