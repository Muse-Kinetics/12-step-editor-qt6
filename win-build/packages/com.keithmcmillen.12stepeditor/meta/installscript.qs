function Component()
{
    // Default constructor
}

Component.prototype.createOperations = function()
{
    console.log("=== Starting createOperations ===");

    var targetDir = installer.value("TargetDir");
    console.log("TargetDir: " + targetDir);

    // Create the basic operations for the component
    component.createOperations();
    console.log("Operations created.");

    var displayName = component.displayName;
    var description = component.description;

    console.log("DisplayName: " + displayName);
    console.log("Description: " + description);

    if (systemInfo.productType === "windows") 
    {
        console.log("Running Windows-specific setup...");

        var systemPath = "C:/Windows/System32/";
        console.log("Checking for required VC runtime DLLs in: " + systemPath);

        var runtimeMissing = !installer.fileExists(systemPath + "vcruntime140.dll") || 
                             !installer.fileExists(systemPath + "msvcp140.dll");

        if (runtimeMissing) 
        {
            console.log("❌ Missing VC runtime. Will install after file copy.");
            
            // Correct path inside the installed folder
            var vcRedistPath = targetDir + "/12 Step Editor/vc_redist.x64.exe";

            // Notify the user before installation
            QMessageBox.information("vc_runtime_info", 
                "Installing Required Runtime",
                "The required Visual C++ Redistributable is missing. It will be installed automatically " +
                "after the package is copied. Please wait...",
                QMessageBox.Ok);

            // Execute the installer after copying files. The exit-code allowlist is required:
            // vc_redist.x64.exe legitimately returns 3010 (installed, reboot required) and
            // 1638/5100 (an equal-or-newer runtime is already present) on success paths that
            // aren't 0 - without allowlisting them, QtIFW treats any non-zero exit as a hard
            // failure and aborts the rest of createOperations (including the shortcut
            // operations below), even though the redistributable installed correctly. See
            // ../SOP-WMS-and-Chunked-Firmware-Update-Migration.md §4.10.
            component.addOperation("Execute", "{0,3010,1638,5100}", vcRedistPath, "/install", "/quiet", "/norestart");
        }
        else
        {
            console.log("✅ Visual C++ runtime is already installed.");
        }

        // Create a shortcut for the application
        component.addOperation("CreateShortcut", 
                                targetDir + "/" + displayName + "/" + displayName + ".exe",
                                installer.value("StartMenuDir") + "/" + displayName + ".lnk",
                                "workingDirectory=" + targetDir + "/" + displayName,
                                "iconPath=" + targetDir + "/12 Step Editor/Content/Icons/appicon.ico",
                                "description=" + description);

        // Create a shortcut to the maintenance tool for uninstallation
        component.addOperation("CreateShortcut",
                                targetDir + "/maintenancetool.exe",
                                targetDir + "/Uninstall " + displayName + ".lnk",
                                "workingDirectory=" + targetDir,
                                "description=Uninstall " + displayName);
    }

    console.log("✅ createOperations completed.");
}
