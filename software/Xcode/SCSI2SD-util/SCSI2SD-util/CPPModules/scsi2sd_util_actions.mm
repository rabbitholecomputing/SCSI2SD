//
//  scsi2sd_util_actions.cpp
//  SCSI2SD-util
//
//  Created by Gregory Casamento on 12/5/19.
//  Copyright © 2019 RabbitHole Computing, LLC. All rights reserved.
//

#include "scsi2sd_util_actions.hh"

#import <Foundation/NSDistributedNotificationCenter.h>
#import <Foundation/NSString.h>
#import <Foundation/NSData.h>

void LogMessage(char *message)
{
    NSString *str = [NSString stringWithCString:message encoding:NSUTF8StringEncoding];
    NSLog(@"%@",str);
    [[NSDistributedNotificationCenter defaultCenter] postNotificationName:@"SCSI2SDLogNotification" object:str];
}

void SetStatusText(char *message)
{
    NSString *str = [NSString stringWithCString:message encoding:NSUTF8StringEncoding];
    NSLog(@"%@",str);
    [[NSDistributedNotificationCenter defaultCenter] postNotificationName:@"SCSI2SDStatusNotification" object:str];
}

using namespace SCSI2SD;

class ProgressWrapper
{
public:

    void clearProgressDialog()
    {
    }

    void update(unsigned char arrayId, unsigned short rowNum)
    {
        // [[NSDistributedNotificationCenter defaultCenter] postNotificationName:@"SCSI2SDUpdateNotification" object:str];
    }

private:
    size_t myMaxRows;
    size_t myNumRows;
};
static ProgressWrapper TheProgressWrapper;

extern "C"
void ProgressUpdate(unsigned char arrayId, unsigned short rowNum)
{
    TheProgressWrapper.update(arrayId, rowNum);
}

namespace
{

class AppFrame
{
public:
    HID *myHID;
    Bootloader *myBootloader;


    void mmLogStatus(const std::string& msg)
    {
        // We set PassMessages to false on our log window to prevent popups, but
        // this also prevents LogStatus from updating the status bar.
        //SetStatusText(msg.c_str());
        //LogMessage(msg.c_str());
    }

    void onConfigChanged()
    {
        evaluate();
    }



    void OnID_ConfigDefaults()
    {
        myBoardPanel->setConfig(ConfigUtil::DefaultBoardConfig());
        for (size_t i = 0; i < myTargets.size(); ++i)
        {
            myTargets[i]->setConfig(ConfigUtil::Default(i));
        }
    }

    void OnID_SaveFile()
    {
        TimerLock lock(myTimer);



        wxFileDialog dlg(
            this,
            "Save config settings",
            "",
            "",
            "XML files (*.xml)|*.xml",
            wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
        if (dlg.ShowModal() == wxID_CANCEL) return;

        wxFileOutputStream file(dlg.GetPath());
        if (!file.IsOk())
        {
            LogError("Cannot save settings to file '%s'.", dlg.GetPath());
            return;
        }

        wxTextOutputStream s(file);

        s << "<SCSI2SD>\n";

        s << ConfigUtil::toXML(myBoardPanel->getConfig());
        for (size_t i = 0; i < myTargets.size(); ++i)
        {
            s << ConfigUtil::toXML(myTargets[i]->getConfig());
        }

        s << "</SCSI2SD>\n";
    }

    void OnID_OpenFile(wxCommandEvent& event)
    {
        TimerLock lock(myTimer);

        wxFileDialog dlg(
            this,
            "Load config settings",
            "",
            "",
            "XML files (*.xml)|*.xml",
            wxFD_OPEN | wxFD_FILE_MUST_EXIST);
        if (dlg.ShowModal() == wxID_CANCEL) return;

        try
        {
            std::pair<BoardConfig, std::vector<TargetConfig>> configs(
                ConfigUtil::fromXML(std::string(dlg.GetPath())));

            myBoardPanel->setConfig(configs.first);

            size_t i;
            for (i = 0; i < configs.second.size() && i < myTargets.size(); ++i)
            {
                myTargets[i]->setConfig(configs.second[i]);
            }

            for (; i < myTargets.size(); ++i)
            {
                myTargets[i]->setConfig(ConfigUtil::Default(i));
            }
        }
        catch (std::exception& e)
        {
            LogError(
                "Cannot load settings from file '%s'.\n%s",
                dlg.GetPath(),
                e.what());

            wxMessageBox(
                e.what(),
                "Load error",
                wxOK | wxICON_ERROR);
        }
    }

    void OnID_Firmware(wxCommandEvent& event)
    {
        TimerLock lock(myTimer);
        doFirmwareUpdate();
    }

    void OnID_Bootloader()
    {
        TimerLock lock(myTimer);
        doBootloaderUpdate();
    }

    void OnID_LogWindow()
    {
        // myLogWindow->Show();
    }

    void doFirmwareUpdate()
    {
        while (true)
        {
            try
            {
                if (!myHID) myHID.reset(HID::Open());
                if (myHID)
                {
                    mmLogStatus("Resetting SCSI2SD into bootloader");

                    myHID->enterBootloader();
                    myHID.reset();
                }


                if (!myBootloader)
                {
                    myBootloader.reset(Bootloader::Open());
                    if (myBootloader)
                    {
                        mmLogStatus("Bootloader found");
                        break;
                    }
                }

                else if (myBootloader)
                {
                    // Verify the USB HID connection is valid
                    if (!myBootloader->ping())
                    {
                        mmLogStatus("Bootloader ping failed");
                        myBootloader.reset();
                    }
                    else
                    {
                        mmLogStatus("Bootloader found");
                        break;
                    }
                }
            }
            catch (std::exception& e)
            {
                mmLogStatus(e.what());
                myHID.reset();
                myBootloader.reset();
            }
            wxMilliSleep(100);
            if (!progress->Pulse())
            {
                return; // user cancelled.
            }
        }

        int totalFlashRows = 0;
        std::string tmpFile;
        try
        {
            zipper::ReaderPtr reader(new zipper::FileReader(filename));
            zipper::Decompressor decomp(reader);
            std::vector<zipper::CompressedFilePtr> files(decomp.getEntries());
            for (auto it(files.begin()); it != files.end(); it++)
            {
                if (myBootloader && myBootloader->isCorrectFirmware((*it)->getPath()))
                {
                    std::stringstream msg;
                    msg << "Found firmware entry " << (*it)->getPath() <<
                        " within archive " << filename;
                    mmLogStatus(msg.str());
                    tmpFile =
                        wxFileName::CreateTempFileName(
                            _("SCSI2SD_Firmware"), static_cast<wxFile*>(NULL)
                            );
                    zipper::FileWriter out(tmpFile);
                    (*it)->decompress(out);
                    msg.clear();
                    msg << "Firmware extracted to " << tmpFile;
                    mmLogStatus(msg.str());
                    break;
                }
            }

            if (tmpFile.empty())
            {
                // TODO allow "force" option
                wxMessageBox(
                    "Wrong filename",
                    "Wrong filename",
                    wxOK | wxICON_ERROR);
                return;
            }

            Firmware firmware(tmpFile);
            totalFlashRows = firmware.totalFlashRows();
        }
        catch (std::exception& e)
        {
            mmLogStatus(e.what());
            std::stringstream msg;
            msg << "Could not open firmware file: " << e.what();
            wxMessageBox(
                msg.str(),
                "Bad file",
                wxOK | wxICON_ERROR);
            wxRemoveFile(tmpFile);
            return;
        }

        {
            wxWindowPtr<wxGenericProgressDialog> progress(
                new wxGenericProgressDialog(
                    "Loading firmware",
                    "Loading firmware",
                    totalFlashRows,
                    this,
                    wxPD_AUTO_HIDE | wxPD_REMAINING_TIME)
                    );
            TheProgressWrapper.setProgressDialog(progress, totalFlashRows);
        }

        std::stringstream msg;
        msg << "Upgrading firmware from file: " << tmpFile;
        mmLogStatus(msg.str());

        try
        {
            myBootloader->load(tmpFile, &ProgressUpdate);
            TheProgressWrapper.clearProgressDialog();

            wxMessageBox(
                "Firmware update successful",
                "Firmware OK",
                wxOK);
            mmLogStatus("Firmware update successful");


            myHID.reset();
            myBootloader.reset();
        }
        catch (std::exception& e)
        {
            TheProgressWrapper.clearProgressDialog();
            mmLogStatus(e.what());
            myHID.reset();
            myBootloader.reset();

            wxMessageBox(
                "Firmware Update Failed",
                e.what(),
                wxOK | wxICON_ERROR);

            wxRemoveFile(tmpFile);
        }
    }
    
    void doBootloaderUpdate(char *path)
    {
        NSString *filename = [NSString stringWithCString: path
                                                encoding: NSUTF8StringEncoding];

        NSData *fileData = [NSData dataWithContentsOfFile:filename];
        NSUInteger len = [fileData length];
        if (len != 0x2400)
        {
            NSLog(@"Incorrect size, invalid boodloader");
            return;
        }
        
        uint8_t *data = (uint8_t *)[fileData bytes];
        static char magic[] = {
            'P', 0, 'S', 0, 'o', 0, 'C', 0, '3', 0, ' ', 0,
            'B', 0, 'o', 0, 'o', 0, 't', 0, 'l', 0, 'o', 0, 'a', 0, 'd', 0, 'e', 0, 'r', 0};

        uint8_t* dataEnd = data + sizeof(data);
        if (std::search(data, dataEnd, magic, magic + sizeof(magic)) >= dataEnd)
        {
            NSLog(@"Not a valid boot loader file");
            return;
        }

        NSLog(@"Upgrading bootloader from file: %@", filename);

        int currentProgress = 0;
        int totalProgress = 36;

        for (size_t flashRow = 0; flashRow < 36; ++flashRow)
        {
            // std::stringstream ss;
            // ss << "Programming flash array 0 row " << (flashRow);
            // mmLogStatus(ss.str());
            currentProgress += 1;

            if (currentProgress == totalProgress)
            {
                // ss.str("Save Complete.");
                // mmLogStatus("Save Complete.");
            }
            if (!progress->Update(
                    (100 * currentProgress) / totalProgress,
                    ss.str()
                    )
                )
            {
                goto abort;
            }

            uint8_t rowData = data + (flashRow * 256);
            std::vector<uint8_t> flashData(rowData, rowData + 256);
            try
            {
                myHID->writeFlashRow(0, flashRow, flashData);
            }
            catch (std::runtime_error& e)
            {
                mmLogStatus(e.what());
                goto err;
            }
        }

        goto out;

    err:
        mmLogStatus("Bootloader update failed");
        progress->Update(100, "Bootloader update failed");
        goto out;

    abort:
        mmLogStatus("Bootloader update aborted");

    out:
        return;


    }

    void dumpSCSICommand(std::vector<uint8_t> buf)
        {
        std::stringstream msg;
        msg << std::hex;
        for (size_t i = 0; i < 32 && i < buf.size(); ++i)
        {
            msg << std::setfill('0') << std::setw(2) <<
            static_cast<int>(buf[i]) << ' ';
        }
        LogMessage(this, msg.str().c_str());
        }

    void logSCSI()
    {
        if (!mySCSILogChk->IsChecked() ||
            !myHID)
        {
            return;
        }
        try
        {
            std::vector<uint8_t> info(HID::HID_PACKET_SIZE);
            if (myHID->readSCSIDebugInfo(info))
            {
                dumpSCSICommand(info);
            }
        }
        catch (std::exception& e)
        {
            LogWarning(this, e.what());
            myHID.reset();
        }
    }

    void OnID_Timer()
    {
        logSCSI();
        time_t now = time(NULL);
        if (now == myLastPollTime) return;
        myLastPollTime = now;

        // Check if we are connected to the HID device.
        // AND/or bootloader device.
        try
        {
            if (myBootloader)
            {
                // Verify the USB HID connection is valid
                if (!myBootloader->ping())
                {
                    myBootloader.reset();
                }
            }

            if (!myBootloader)
            {
                myBootloader.reset(Bootloader::Open());

                if (myBootloader)
                {
                    mmLogStatus("SCSI2SD Bootloader Ready");
                }
            }

            int supressLog = 0;
            if (myHID && myHID->getFirmwareVersion() < MIN_FIRMWARE_VERSION)
            {
                // No method to check connection is still valid.
                // So assume it isn't.
                myHID.reset();
                supressLog = 1;
            }
            else if (myHID && !myHID->ping())
            {
                // Verify the USB HID connection is valid
                myHID.reset();
            }

            if (!myHID)
            {
                myHID.reset(HID::Open());
                if (myHID)
                {
                    if (myHID->getFirmwareVersion() < MIN_FIRMWARE_VERSION)
                    {
                        if (!supressLog)
                        {
                            // Oh dear, old firmware
                            std::stringstream msg;
                            msg << "Firmware update required. Version " <<
                                myHID->getFirmwareVersionStr();
                            mmLogStatus(msg.str());
                        }
                    }
                    else
                    {
                        std::stringstream msg;
                        msg << "SCSI2SD Ready, firmware version " <<
                            myHID->getFirmwareVersionStr();
                        mmLogStatus(msg.str());

                        std::vector<uint8_t> csd(myHID->getSD_CSD());
                        std::vector<uint8_t> cid(myHID->getSD_CID());
                        std::stringstream sdinfo;
                        sdinfo << "SD Capacity (512-byte sectors): " <<
                            myHID->getSDCapacity() << std::endl;

                        sdinfo << "SD CSD Register: ";
                        if (sdCrc7(&csd[0], 15, 0) != (csd[15] >> 1))
                        {
                            sdinfo << "BADCRC ";
                        }
                        for (size_t i = 0; i < csd.size(); ++i)
                        {
                            sdinfo <<
                                std::hex << std::setfill('0') << std::setw(2) <<
                                static_cast<int>(csd[i]);
                        }
                        sdinfo << std::endl;
                        sdinfo << "SD CID Register: ";
                        if (sdCrc7(&cid[0], 15, 0) != (cid[15] >> 1))
                        {
                            sdinfo << "BADCRC ";
                        }
                        for (size_t i = 0; i < cid.size(); ++i)
                        {
                            sdinfo <<
                                std::hex << std::setfill('0') << std::setw(2) <<
                                static_cast<int>(cid[i]);
                        }

                        LogMessage(this, "%s", sdinfo.str());

                        if (mySelfTestChk->IsChecked())
                        {
                            std::stringstream scsiInfo;
                            scsiInfo << "SCSI Self-Test: " <<
                                (myHID->scsiSelfTest() ? "Passed" : "FAIL");
                            LogMessage(this, "%s", scsiInfo.str());
                        }

                        if (!myInitialConfig)
                        {
/* This doesn't work properly, and causes crashes.
                            wxCommandEvent loadEvent(wxEVT_NULL, ID_BtnLoad);
                            GetEventHandler()->AddPendingEvent(loadEvent);
*/
                        }

                    }
                }
                else
                {
                    char ticks[] = {'/', '-', '\\', '|'};
                    std::stringstream ss;
                    ss << "Searching for SCSI2SD device " << ticks[myTickCounter % sizeof(ticks)];
                    myTickCounter++;
                    SetStatusText(ss.str());
                }
            }
        }
        catch (std::runtime_error& e)
        {
            std::cerr << e.what() << std::endl;
            mmLogStatus(e.what());
        }

        evaluate();
    }

    void doLoad()
    {
        TimerLock lock(myTimer);
        if (!myHID) return;

        mmLogStatus("Loading configuration");

        wxWindowPtr<wxGenericProgressDialog> progress(
            new wxGenericProgressDialog(
                "Load config settings",
                "Loading config settings",
                100,
                this,
                wxPD_CAN_ABORT | wxPD_REMAINING_TIME)
                );

        int currentProgress = 0;
        int totalProgress = myTargets.size() * SCSI_CONFIG_ROWS + 1;

        // Read board config first.
        std::vector<uint8_t> boardCfgFlashData;
        int flashRow = SCSI_CONFIG_BOARD_ROW;
        {
            std::stringstream ss;
            ss << "Reading flash array " << SCSI_CONFIG_ARRAY <<
                " row " << flashRow;
            mmLogStatus(ss.str());
            currentProgress += 1;

            if (!progress->Update(
                    (100 * currentProgress) / totalProgress,
                    ss.str()
                    )
                )
            {
                goto abort;
            }

            try
            {
                myHID->readFlashRow(
                    SCSI_CONFIG_ARRAY, flashRow, boardCfgFlashData);
                myBoardPanel->setConfig(
                    ConfigUtil::boardConfigFromBytes(&boardCfgFlashData[0]));
            }
            catch (std::runtime_error& e)
            {
                mmLogStatus(e.what());
                goto err;
            }
        }

        for (size_t i = 0;
            i < myTargets.size();
            ++i)
        {
            flashRow = (i <= 3)
                ? SCSI_CONFIG_0_ROW + (i*SCSI_CONFIG_ROWS)
                : SCSI_CONFIG_4_ROW + ((i-4)*SCSI_CONFIG_ROWS);
            std::vector<uint8_t> raw(sizeof(TargetConfig));

            for (size_t j = 0; j < SCSI_CONFIG_ROWS; ++j)
            {
                std::stringstream ss;
                ss << "Reading flash array " << SCSI_CONFIG_ARRAY <<
                    " row " << (flashRow + j);
                mmLogStatus(ss.str());
                currentProgress += 1;
                if (currentProgress == totalProgress)
                {
                    ss.str("Load Complete.");
                    mmLogStatus("Load Complete.");
                }

                if (!progress->Update(
                        (100 * currentProgress) / totalProgress,
                        ss.str()
                        )
                    )
                {
                    goto abort;
                }

                std::vector<uint8_t> flashData;

                try
                {
                    myHID->readFlashRow(
                        SCSI_CONFIG_ARRAY, flashRow + j, flashData);

                }
                catch (std::runtime_error& e)
                {
                    mmLogStatus(e.what());
                    goto err;
                }

                std::copy(
                    flashData.begin(),
                    flashData.end(),
                    &raw[j * SCSI_CONFIG_ROW_SIZE]);
            }
            myTargets[i]->setConfig(ConfigUtil::fromBytes(&raw[0]));
        }

        // Support old boards without board config set
        if (memcmp(&boardCfgFlashData[0], "BCFG", 4)) {
            BoardConfig defCfg = ConfigUtil::DefaultBoardConfig();
            defCfg.flags = myTargets[0]->getConfig().flagsDEPRECATED;
            myBoardPanel->setConfig(defCfg);
        }

        myInitialConfig = true;
        goto out;

    err:
        mmLogStatus("Load failed");
        progress->Update(100, "Load failed");
        goto out;

    abort:
        mmLogStatus("Load Aborted");

    out:
        return;
    }

    void doSave()
    {
        TimerLock lock(myTimer);
        if (!myHID) return;

        mmLogStatus("Saving configuration");

        wxWindowPtr<wxGenericProgressDialog> progress(
            new wxGenericProgressDialog(
                "Save config settings",
                "Saving config settings",
                100,
                this,
                wxPD_CAN_ABORT | wxPD_REMAINING_TIME)
                );


        int currentProgress = 0;
        int totalProgress = myTargets.size() * SCSI_CONFIG_ROWS + 1;

        // Write board config first.
        int flashRow = SCSI_CONFIG_BOARD_ROW;
        {
            std::stringstream ss;
            ss << "Programming flash array " << SCSI_CONFIG_ARRAY <<
                " row " << flashRow;
            mmLogStatus(ss.str());
            currentProgress += 1;

            if (!progress->Update(
                    (100 * currentProgress) / totalProgress,
                    ss.str()
                    )
                )
            {
                goto abort;
            }

            std::vector<uint8_t> flashData =
                ConfigUtil::boardConfigToBytes(myBoardPanel->getConfig());
            try
            {
                myHID->writeFlashRow(
                    SCSI_CONFIG_ARRAY, flashRow, flashData);
            }
            catch (std::runtime_error& e)
            {
                mmLogStatus(e.what());
                goto err;
            }
        }

        flashRow = SCSI_CONFIG_0_ROW;
        for (size_t i = 0;
            i < myTargets.size();
            ++i)
        {
            flashRow = (i <= 3)
                ? SCSI_CONFIG_0_ROW + (i*SCSI_CONFIG_ROWS)
                : SCSI_CONFIG_4_ROW + ((i-4)*SCSI_CONFIG_ROWS);

            TargetConfig config(myTargets[i]->getConfig());
            std::vector<uint8_t> raw(ConfigUtil::toBytes(config));

            for (size_t j = 0; j < SCSI_CONFIG_ROWS; ++j)
            {
                std::stringstream ss;
                ss << "Programming flash array " << SCSI_CONFIG_ARRAY <<
                    " row " << (flashRow + j);
                mmLogStatus(ss.str());
                currentProgress += 1;

                if (currentProgress == totalProgress)
                {
                    ss.str("Save Complete.");
                    mmLogStatus("Save Complete.");
                }
                if (!progress->Update(
                        (100 * currentProgress) / totalProgress,
                        ss.str()
                        )
                    )
                {
                    goto abort;
                }

                std::vector<uint8_t> flashData(SCSI_CONFIG_ROW_SIZE, 0);
                std::copy(
                    &raw[j * SCSI_CONFIG_ROW_SIZE],
                    &raw[(1+j) * SCSI_CONFIG_ROW_SIZE],
                    flashData.begin());
                try
                {
                    myHID->writeFlashRow(
                        SCSI_CONFIG_ARRAY, flashRow + j, flashData);
                }
                catch (std::runtime_error& e)
                {
                    mmLogStatus(e.what());
                    goto err;
                }
            }
        }

        myHID.reset();

        goto out;

    err:
        mmLogStatus("Save failed");
        progress->Update(100, "Save failed");
        goto out;

    abort:
        mmLogStatus("Save Aborted");

    out:
        return;
    }

    // Note: Don't confuse this with the wxApp::OnExit virtual method
    void OnExitEvt();

    void OnCloseEvt();

    void OnAbout()
    {
    }
};

/*
wxBEGIN_EVENT_TABLE(AppFrame, wxFrame)
    EVT_MENU(AppFrame::ID_ConfigDefaults, AppFrame::OnID_ConfigDefaults)
    EVT_MENU(AppFrame::ID_Firmware, AppFrame::OnID_Firmware)
    EVT_MENU(AppFrame::ID_Bootloader, AppFrame::OnID_Bootloader)
    EVT_MENU(AppFrame::ID_LogWindow, AppFrame::OnID_LogWindow)
    EVT_MENU(AppFrame::ID_SaveFile, AppFrame::OnID_SaveFile)
    EVT_MENU(AppFrame::ID_OpenFile, AppFrame::OnID_OpenFile)
    EVT_MENU(wxID_EXIT, AppFrame::OnExitEvt)
    EVT_MENU(wxID_ABOUT, AppFrame::OnAbout)

    EVT_TIMER(AppFrame::ID_Timer, AppFrame::OnID_Timer)

    EVT_COMMAND(wxID_ANY, ConfigChangedEvent, AppFrame::onConfigChanged)

    EVT_MENU(ID_BtnSave, AppFrame::doSave)
    EVT_MENU(ID_BtnLoad, AppFrame::doLoad)

    EVT_CLOSE(AppFrame::OnCloseEvt)

wxEND_EVENT_TABLE()
*/



