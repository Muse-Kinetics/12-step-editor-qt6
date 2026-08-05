function Controller()
{
}

Controller.prototype.TargetDirectoryPageCallback = function()
{
    var targetDir = installer.value("TargetDir");
    var maintenanceTool = targetDir + "/maintenancetool.exe";

    if (installer.fileExists(maintenanceTool))
    {
        var reply = QMessageBox.question(
            "existingInstallationFound",
            "Existing Installation Found",
            "An existing installation of 12 Step Editor was found at:\n\n" + targetDir +
            "\n\nIt must be removed before installing this version. Remove it now?",
            QMessageBox.Yes | QMessageBox.No
        );

        if (reply === QMessageBox.Yes)
        {
            console.log("Removing existing installation at " + targetDir);
            installer.execute(maintenanceTool, ["purge", "-c", "--am"]);

            // maintenancetool.exe can't delete its own running binary; final cleanup
            // of the last few files completes in a short-lived helper process after
            // purge's own process exits, so give it a moment before checking.
            installer.execute("cmd.exe", ["/c", "ping", "-n", "3", "127.0.0.1"]);

            if (installer.fileExists(maintenanceTool))
            {
                QMessageBox.critical(
                    "existingInstallationRemoveFailed",
                    "Removal Failed",
                    "The existing installation could not be fully removed. Please close 12 Step Editor " +
                    "if it is running, then uninstall it manually before continuing.",
                    QMessageBox.Ok
                );
            }
        }
    }
}
