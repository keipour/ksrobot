#include "ExecutionControl.h"

#include <QVBoxLayout>
#include <QRect>
#include "Utils.h"
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>

using namespace KSRobot;
using namespace gui;
using namespace utils;

void ExecControlData::EnableComm(bool enable)
{
//     if( !OMPL.Enable )
//         throw std::runtime_error("Cannot enable communications while OMPL is disabled.");
    Comm.Enable = enable; // for now no dependency
}

void ExecControlData::EnableFovis(bool enable)
{
//     if( !enable && (OctoMap.Enable || OMPL.Enable || iSAM.Enable) )
//         throw std::runtime_error("Cannot disable Fovis. Cause : OctoMap|OMPL|iSAM");
    Fovis.Enable = enable;
}

void ExecControlData::EnableSAM(bool enable)
{
    //NOTE: For now SAM is independent
//     if( enable && !Fovis.Enable )
//         throw std::runtime_error("Cannot enable iSAM while Fovis is disabled.");
    iSAM.Enable = enable;
}

void ExecControlData::EnableOctoMap(bool enable)
{
//     if( enable && !Fovis.Enable )
//         throw std::runtime_error("Cannot enable OctoMap while Fovis is disabled");
//     
//     if( !enable && OMPL.Enable )
//         throw std::runtime_error("Cannot disable OctoMap while OMPL is set.");
//     
    OctoMap.Enable = enable;
}

void ExecControlData::EnableOMPL(bool enable)
{
//     if( !Fovis.Enable || !OctoMap.Enable )
//         throw std::runtime_error("Cannot enable OMPLE. Cause: Fovis|OctoMap");
//     
    OMPL.Enable = enable;
}

void ExecControlData::CheckConsistancy()
{
    if( !Fovis.Enable )
    {
        if( OctoMap.Enable )
            throw std::runtime_error("Cannot enable OctoMap while Fovis is disabled.");
        if( iSAM.Enable )
            throw std::runtime_error("Cannot enable iSAM while Fovis is disabled.");
        if( Comm.Enable )
            throw std::runtime_error("Cannot enable Communications while Fovis is disabled.");
    }
    
    if( !OctoMap.Enable )
    {
        if( OMPL.Enable )
            throw std::runtime_error("Cannot enable OMPL while OctoMap is disabled.");
    }

    if( !iSAM.Enable )
    {
        // For now iSAM is independent.
    }
    
    if( !OMPL.Enable )
    {
        if( Comm.Enable )
            throw std::runtime_error("Cannot enable Communications while OMPL is disabled.");
    }
}


ExecutionControl::ExecutionControl(QWidget* parent, Qt::WindowFlags f) : KWidgetBase(parent, f)
{
    qRegisterMetaType<ExecControlData>("ExecControlData");
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
    
    //TODO: Change this, should be inside kinect
    if( mGuiPO->GetString("DefaultSource", std::string("file")) == "kinect" )
        mUI->mRdBtnKinectDevice->setChecked(true);
    else
        mUI->mRdBtnKinectFile->setChecked(true);
    
//    mBinder.SetPO(mGuiPO);
}

void ExecutionControl::ReadSettings()
{
    //Kinect
    mData.Kinect.GetFromDevice = mGuiPO->GetBool("Kinect.GetFromDevice", false);
    mData.Kinect.SourceDevice = mGuiPO->GetString("Kinect.SourceDevice", std::string(""));
    mData.Kinect.SourceDir = mGuiPO->GetString("Kinect.SourceDir", 
                                               std::string("/windows/E/Datasets/rgbd_dataset_freiburg2_pioneer_slam/"));
    //FOVIS
    mData.Fovis.Enable = mGuiPO->GetBool("Fovis.Enable", false);
    //iSAM
    mData.iSAM.Enable = mGuiPO->GetBool("iSAM.Enable", false);
    //OctoMap
    mData.OctoMap.Enable = mGuiPO->GetBool("OctoMap.Enable", false);
    //OMPL
    mData.OMPL.Enable = mGuiPO->GetBool("OMPL.Enable", false);
    //Comm
    mData.Comm.Enable = mGuiPO->GetBool("Comm.Enable", false);
    mData.Comm.Port = mGuiPO->GetInt("Comm.Port", 0); //TODO: change default
    mData.Comm.Address = mGuiPO->GetString("Comm.Address", std::string("127.0.0.1"));
    //GUI
    mData.GUI.ViewFovis = mGuiPO->GetBool("GUI.ViewFovis", false);
    mData.GUI.ViewOctoMap = mGuiPO->GetBool("GUI.ViewOctoMap", false);
    mData.GUI.ViewOMPL = mGuiPO->GetBool("GUI.ViewOMPL", false);
    mData.GUI.ViewPCL = mGuiPO->GetBool("GUI.ViewPCL", false);
    mData.GUI.ViewRGBD = mGuiPO->GetBool("GUI.ViewRGBD", false);
}

void ExecutionControl::SaveSettings()
{
    //Kinect
    mGuiPO->PutBool("Kinect.GetFromDevice", mData.Kinect.GetFromDevice);
    if( mData.Kinect.GetFromDevice )
        mGuiPO->PutString("Kinect.SourceDevice", mUI->mKinectDevPath->text().toStdString());
    else
        mGuiPO->PutString("Kinect.SourceDir", mUI->mKinectDevPath->text().toStdString());
    //FOVIS
    mGuiPO->PutBool("Fovis.Enable", mUI->mChkEnableFovis->isChecked());
    //iSAM
    mGuiPO->PutBool("iSAM.Enable", mUI->mChkEnableSAM->isChecked());
    //OctoMap
    mGuiPO->PutBool("OctoMap.Enable", mUI->mChkEnableOctomap->isChecked());
    //OMPL
    mGuiPO->PutBool("OMPL.Enable", mUI->mChkEnableOMPL->isChecked());
    //Comm
    mGuiPO->PutBool("Comm.Enable", mUI->mChkEnableComm->isChecked());
    //GUI
    mGuiPO->PutBool("GUI.ViewFovis", mUI->mChkViewFovisResults->isChecked());
    mGuiPO->PutBool("GUI.ViewOctoMap", mUI->mChkViewOctomap->isChecked());
    mGuiPO->PutBool("GUI.ViewOMPL", mUI->mChkViewOMPL->isChecked());
    mGuiPO->PutBool("GUI.ViewPCL", mUI->mChkViewPointCloud->isChecked());
    mGuiPO->PutBool("GUI.ViewRGBD", mUI->mChkViewRGBD->isChecked());
}

void ExecutionControl::BindSettings()
{
    
}

void ExecutionControl::BindValueToName(const std::string& name, QAbstractButton* btn)
{
    mBinder.AddFunctionSetting<bool>(name, 
                            boost::bind(&QAbstractButton::isChecked, btn), 
                            boost::bind(&QAbstractButton::setChecked, btn, _1));
}

bool ExecutionControl::UpdateUIFromData()
{
    //TODO: First move these to another function, which is executed at initialization.
    //      Second make sure that the variables will be also updated; but this is necessary
    //             only for starting a run.
    BindValueToName("Kinect.GetFromDevice", mUI->mRdBtnKinectDevice);
    BindValueToName("Fovis.Enabled", mUI->mChkEnableFovis);
    BindValueToName("iSAM.Enabled", mUI->mChkEnableSAM);
    BindValueToName("OctoMap.Enabled", mUI->mChkEnableOctomap);
    BindValueToName("OMPL.Enabled", mUI->mChkEnableOMPL);
    BindValueToName("Comm.Enable", mUI->mChkEnableComm);
    
    
    
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
        
        //mUI->mKinectDevPath->setText(mData.Kinect.Source.c_str());
        // Update FOVIS
        mUI->mChkEnableFovis->setChecked(mData.Fovis.Enable);
        // Update iSAM
        mUI->mChkEnableSAM->setChecked(mData.iSAM.Enable);
        // Update OctoMap
        mUI->mChkEnableOctomap->setChecked(mData.OctoMap.Enable);
        // Update OMPL
        mUI->mChkEnableOMPL->setChecked(mData.OMPL.Enable);
        // Update Comm
        mUI->mChkEnableComm->setChecked(mData.Comm.Enable);
        
        mUI->mChkViewFovisResults->setChecked(mData.GUI.ViewFovis);
        mUI->mChkViewOctomap->setChecked(mData.GUI.ViewOctoMap);
        mUI->mChkViewOMPL->setChecked(mData.GUI.ViewOMPL);
        mUI->mChkViewPointCloud->setChecked(mData.GUI.ViewPCL);
        mUI->mChkViewRGBD->setChecked(mData.GUI.ViewRGBD);
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
    //TODO: Implement this.
    try
    {
        // Update Kinect
        mData.Kinect.GetFromDevice = mUI->mRdBtnKinectDevice->isChecked();
        if( mData.Kinect.GetFromDevice )
            mData.Kinect.SourceDevice = mUI->mKinectDevPath->text().toStdString();
        else
            mData.Kinect.SourceDir = mUI->mKinectDevPath->text().toStdString();
        // Update Fovis
        mData.EnableFovis(mUI->mChkEnableFovis->isChecked());
        // Update iSAM
        mData.EnableSAM(mUI->mChkEnableSAM->isChecked());
        // Update OctoMap
        mData.EnableOctoMap(mUI->mChkEnableOctomap->isChecked());
        // Update OMPL
        mData.EnableOMPL(mUI->mChkEnableOMPL->isChecked());
        // Update Commserver
        mData.EnableComm(mUI->mChkEnableComm->isChecked());
        
        // Update GUI settings
        mData.GUI.ViewFovis = mUI->mChkViewFovisResults->isChecked();
        mData.GUI.ViewOctoMap = mUI->mChkViewOctomap->isChecked();
        mData.GUI.ViewOMPL = mUI->mChkViewOMPL->isChecked();
        mData.GUI.ViewPCL = mUI->mChkViewPointCloud->isChecked();
        mData.GUI.ViewRGBD = mUI->mChkViewRGBD->isChecked();
        
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
