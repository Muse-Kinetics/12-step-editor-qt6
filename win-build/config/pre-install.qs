function preInstall() 
{
    console.log("running pre-install");
    var targetDir = installer.value("TargetDir");

    // Check if the directory already exists and prevent installation if needed
    if (installer.fileExists(targetDir)) 
    {
        installer.messageBox("The directory you selected already exists and contains an installation. " +
                             "Please uninstall the existing installation first or choose a different directory.",
                             "Error", 0x10 /*QMessageBox::Critical*/, 0x04 /*QMessageBox::Ok*/);
        installer.setDefaultPageVisible(QInstaller.TargetDirectory, false);
        installer.setDefaultPageVisible(QInstaller.ComponentSelection, false);
        installer.setDefaultPageVisible(QInstaller.Introduction, false);
        installer.setDefaultPageVisible(QInstaller.ReadyForInstallation, false);
        installer.setDefaultPageVisible(QInstaller.PerformInstallation, false);
        installer.setDefaultPageVisible(QInstaller.LicenseCheck, false);
        installer.setDefaultPageVisible(QInstaller.StartMenuSelection, false);
        gui.rejectWithoutPrompt();
        return;
    }

    // Check for required Visual C++ runtime DLLs
    var systemPath = "C:/Windows/System32/";
    var runtimeMissing = !installer.fileExists(systemPath + "vcruntime140.dll") || 
                         !installer.fileExists(systemPath + "msvcp140.dll");

    if (runtimeMissing) 
    {
        console.log("Required Visual C++ runtime is missing. Running installer...");
        var vcRedistPath = targetDir + "/vc_redist_x64.exe";

        if (installer.fileExists(vcRedistPath)) 
        {
            var result = installer.execute(vcRedistPath, ["/install", "/quiet", "/norestart"]);
            if (result !== 0) 
            {
                installer.messageBox("Failed to install the required Visual C++ Redistributable. " +
                                     "Please install it manually before proceeding.", 
                                     "Error", 0x10 /*QMessageBox::Critical*/, 0x04 /*QMessageBox::Ok*/);
                gui.rejectWithoutPrompt();
                return;
            }
        } 
        else 
        {
            installer.messageBox("The required Visual C++ Redistributable installer was not found. " +
                                 "Ensure vc_redist_x64.exe is included in the installer package.", 
                                 "Error", 0x10 /*QMessageBox::Critical*/, 0x04 /*QMessageBox::Ok*/);
            gui.rejectWithoutPrompt();
            return;
        }
    }
}