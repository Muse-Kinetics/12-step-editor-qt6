#-------------------------------------------------
#
# Project created by QtCreator 2014-03-13T12:10:11
#
#-------------------------------------------------

QT       += core gui \
            widgets \
            qml \
            quick


TARGET = "12 Step Editor"
TEMPLATE = app
VERSION = 3.0.1.C
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

#uncomment this and DEPLOY = 1 to build a console version of the app. Do this once before deploying the app.
#BUILD_CONSOLE = 1

# Uncomment this line if you want to deploy the app (codesign, xxxDeployqt, copy content, and create installer/dmg etc
DEPLOY = 1

# Uncomment this line if your project includes QML, this will add the qmldir option to the qt deployment utility command
INCLUDE_QML = 1

# still holding onto support for High Sierra here, separate build

message("Building with Qt $${QT_VERSION}")

# build with Qt 5.11.3 to support El Capitan, Sierra, and High Sierra
lessThan(QT_MAJOR_VERSION, 6){
    macx{
        message("Building legacy MacOS Intel Binary")
        QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.11
    }
}

# build with Qt 6.2.1 to support Mojave and later
versionAtLeast(QT_VERSION, 6.2.1){
    macx{
        message("Building Apple M1/Intel Universal Binary")
        QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.14
        QMAKE_APPLE_DEVICE_ARCHS = x86_64 arm64
    }
}

!isEmpty(BUILD_CONSOLE) {
    CONFIG += console
}

INCLUDEPATH +=  presetimageformatting \
                presetimageformatting/DeviceManager \
                presetimageformatting/TreeView


SOURCES += main.cpp\
    globalVars.c \
    inc/KMI_KMDM/KMI_SysexMessages.c \
    inc/KMI_KMDM/KMI_mdm.cpp \
    inc/KMI_KMDM/cvCal/cvCal.cpp \
    inc/KMI_KMDM/fwupdate/fwupdate.cpp \
    inc/KMI_KMDM/kmiSysEx/kmiSysEx.cpp \
    inc/KMI_KMDM/pedalCal/pedalcal.cpp \
    inc/KMI_KMDM/qt_ui/kmiSpinBoxUpDown.cpp \
    inc/KMI_KMDM/troubleshoot/troubleshoot.cpp \
    inc/KMI_Ports/kmi_ports.cpp \
    inc/KMI_Updates/kmi_updates.cpp \
    inc/rtmidi/RtMidi.cpp \
        mainwindow.cpp \
    presetinterface.cpp \
    modlines.cpp \
    miditab.cpp \
    keytab.cpp \
    keyedit.cpp \
    setlist.cpp \
    copypastehandler.cpp \
    importexporthandler.cpp \
    settings.cpp \
    tooltipeventfilter.cpp \
    presetimageformatting/imageformatter.cpp \
    presetimageformatting/menu.cpp \
    presetimageformatting/menus.cpp \
    presetimageformatting/DeviceManager/devicemanager.cpp \
    presetimageformatting/DeviceManager/standaloneimage.cpp \
    presetimageformatting/DeviceManager/sysex.cpp \
    presetimageformatting/DeviceManager/sysexcmds.cpp \
    presetimageformatting/DeviceManager/sysexdata.cpp \
    presetimageformatting/DeviceManager/download.c \
    presetimageformatting/TreeView/treeitem.cpp \
    presetimageformatting/TreeView/treemodel.cpp \
    globalpresetinterface.cpp \
    selectallspinbox.cpp \
    userdialog.cpp

HEADERS  += mainwindow.h \
    globalVars.h \
    inc/KMI_KMDM/KMI_DevData.h \
    inc/KMI_KMDM/KMI_FwVersions.h \
    inc/KMI_KMDM/KMI_SysexMessages.h \
    inc/KMI_KMDM/KMI_mdm.h \
    inc/KMI_KMDM/cvCal/cvCal.h \
    inc/KMI_KMDM/cvCal/cvCalData.h \
    inc/KMI_KMDM/fwupdate/fwupdate.h \
    inc/KMI_KMDM/kmiSysEx/kmiSysEx.h \
    inc/KMI_KMDM/midi.h \
    inc/KMI_KMDM/pedalCal/pedalcal.h \
    inc/KMI_KMDM/pedalCal/readOnlySlider.h \
    inc/KMI_KMDM/qt_ui/kmiSpinBoxUpDown.h \
    inc/KMI_KMDM/troubleshoot/troubleshoot.h \
    inc/KMI_Ports/kmi_ports.h \
    inc/KMI_Updates/kmi_updates.h \
    inc/rtmidi/RtMidi.h \
    presetinterface.h \
    modlines.h \
    miditab.h \
    definitions.h \
    keytab.h \
    keyedit.h \
    setlist.h \
    copypastehandler.h \
    importexporthandler.h \
    settings.h \
    tooltipeventfilter.h \
    presetimageformatting/imageformatter.h \
    presetimageformatting/menu.h \
    presetimageformatting/menus.h \
    presetimageformatting/midicommon.h \
    presetimageformatting/StandalonePreset.h \
    presetimageformatting/DeviceManager/device.h \
    presetimageformatting/DeviceManager/devicemanager.h \
    presetimageformatting/DeviceManager/download.h \
    presetimageformatting/DeviceManager/standaloneimage.h \
    presetimageformatting/DeviceManager/sysex.h \
    presetimageformatting/DeviceManager/sysexcmds.h \
    presetimageformatting/DeviceManager/sysexdata.h \
    presetimageformatting/TreeView/treeitem.h \
    presetimageformatting/TreeView/treemodel.h \
    presetimageformatting/12step.h \
    globalpresetinterface.h \
    selectallspinbox.h \
    userdialog.h

FORMS    += forms/mainwindow.ui \
    forms/modlineForm.ui \
    forms/saveAsForm.ui \
    forms/midiTabForm.ui \
    forms/keyTabForm.ui \
    forms/keyEditForm.ui \
    forms/setlistForm.ui \
    forms/settingsForm.ui \
    forms/aboutForm.ui \
    forms/deleteForm.ui \
    forms/importOldFoundForm.ui \
    forms/importOldNotFoundForm.ui \
#    forms/fwoodform.ui \
#    forms/fwprogressform.ui \
#    forms/fwupdatecompleteform.ui \
    forms/aboutFormWin.ui \
    forms/deleteFormWin.ui \
#    forms/fwoodformWin.ui \
#    forms/fwprogressformWin.ui \
#    forms/fwupdatecompleteformWin.ui \
    forms/importOldFoundFormWin.ui \
    forms/importOldNotFoundFormWin.ui \
    forms/keyEditFormWin.ui \
    forms/keyTabFormWin.ui \
    forms/mainwindowWin.ui \
    forms/midiTabFormWin.ui \
    forms/modlineFormWin.ui \
    forms/saveAsFormWin.ui \
    forms/setlistFormWin.ui \
    forms/settingsFormWin.ui \
    inc/KMI_KMDM/cvCal/cvCal.ui \
    inc/KMI_KMDM/fwupdate/fwupdate.ui \
    inc/KMI_KMDM/pedalCal/pedalcal.ui \
    inc/KMI_KMDM/troubleshoot/troubleshoot.ui

#-------------------MIDI--------------------#
#-------------------------------------------#

INCLUDEPATH += \
    inc/rtmidi \
    inc/KMI_Ports \
    inc/KMI_KMDM \
    inc/KMI_KMDM/kmiSysEx \
    inc/KMI_KMDM/cvCal \
    inc/KMI_KMDM/pedalCal \
    inc/KMI_KMDM/fwupdate \
    inc/KMI_KMDM/troubleshoot \
    inc/KMI_KMDM/qt_ui \
    inc/KMI_Updates

#DEFINES += \
#    MDM_DEBUG_ENABLED # enable deeper deebugging for KMI Midi Device Manager

# These defines set RtMidi to the correct OS API
macx{
    DEFINES += __MACOSX_CORE__=1
    LIBS += -framework CoreMidi
    LIBS += -framework CoreAudio
    LIBS += -framework CoreFoundation
}

linux{
    DEFINES += define __LINUX_ALSA__=1
    LIBS += -lasound
}

win32{
    DEFINES += __WINDOWS_MM__=1
        LIBS += -lwinmm
}
# end rtmidi defines

OTHER_FILES +=

RESOURCES += \
    inc/KMI_KMDM/fwupdate/fw_stylesheets.qrc \
    resources.qrc \
    resources/fonts/fonts.qrc

#--------------------Icons--------------------#
#---------------------------------------------#
win32{
RC_FILE =       resources/appicon.rc.txt
}

macx{
ICON = resources/appicon.icns
}

DISTFILES += \
    inc/KMI_KMDM/README.md \
    inc/KMI_KMDM/fwupdate/stylesheets/GrayButtonStyleSheet.qss \
    inc/KMI_KMDM/fwupdate/stylesheets/RedButtonStyleSheet.qss \
    inc/KMI_KMDM/fwupdate/stylesheets/fwUpdateStyles_lightBlue.qss \
    inc/KMI_KMDM/fwupdate/stylesheets/fwUpdateStyles_red.qss \
    inc/KMI_Ports/README.md \
    inc/KMI_Updates/README.md \
    resources/firmware/12Step-29.syx \
    resources/firmware/12step.syx

#--------------- contents/resources --------

# copy SSL DLLs for checking kmi.com for updates
win32 {
    LIBCRYPTO_SRC = $$PWD/inc/KMI_Updates/ssl/libcrypto-1_1-x64.dll
    LIBSSL_SRC = $$PWD/inc/KMI_Updates/ssl/libssl-1_1-x64.dll

    LIBCRYPTO_DST = $$replace(LIBCRYPTO_SRC, '/', '\\')
    LIBSSL_DST = $$replace(LIBSSL_SRC, '/', '\\')

    CONFIG(debug, debug|release) {
        QMAKE_POST_LINK += copy /y \"$$LIBCRYPTO_DST\" \"$$OUT_PWD\\debug\\\" && \
                           copy /y \"$$LIBSSL_DST\" \"$$OUT_PWD\\debug\\\" && \
                           echo "Copied OpenSSL DLLs to debug directory."
    } else {
        QMAKE_POST_LINK += copy /y \"$$LIBCRYPTO_DST\" \"$$OUT_PWD\\release\\\" && \
                           copy /y \"$$LIBSSL_DST\" \"$$OUT_PWD\\release\\\" && \
                           echo "Copied OpenSSL DLLs to release directory."
    }
}



# Conditionally include deployment steps based on the DEPLOY flag
!isEmpty(DEPLOY) {


    win32 {

        #-------------- sign the application
        package_dir = $$shell_path($$absolute_path("..\\win-build\\packages\\com.keithmcmillen.12stepeditor\\data\\$${TARGET}", $$PWD))
        content_dir = $$shell_path($$absolute_path("..\\win-build\\packages\\com.keithmcmillen.12stepeditor\\data\\Content", $$PWD))
        repo_root_dir = $$shell_path($$absolute_path("..", $$PWD))

        path_to_signtool = "C:\\Program Files (x86)\\Windows Kits\\10\\bin\\10.0.22000.0\\x64\\signtool.exe"


        build_subdir = release  # Default to release
        CONFIG(debug, debug|release): build_subdir = debug

        temp_out_pwd = $$replace(OUT_PWD, "/", "\\")
        binary_src = "$${temp_out_pwd}\\$${build_subdir}\\$${TARGET}.exe"
        binary_dest = "$${package_dir}\\$${TARGET}.exe"
        app_name = $${TARGET}

        !isEmpty(BUILD_CONSOLE) {
            binary_dest = "$${package_dir}\\$${TARGET} (debug console).exe"
            app_name = $${TARGET} (debug console)
        }

        # newline
        QMAKE_POST_LINK += echo "" $$escape_expand(\\n\\t)

        QMAKE_POST_LINK += echo "Copying executable to package_dir" $$escape_expand(\\n\\t)

        QMAKE_POST_LINK += copy /y \"$$binary_src \" \"$$binary_dest \" $$escape_expand(\\n\\t)


        QMAKE_POST_LINK += echo "Signing App Executable" $$escape_expand(\\n\\t)
        QMAKE_POST_LINK += $$path_to_signtool sign /v /debug /a /tr http://timestamp.globalsign.com/tsa/advanced /td SHA256 /fd certHash \"$$binary_dest\" $$escape_expand(\\n\\t)

        # only process the rest if this is not the console debug app
        isEmpty(BUILD_CONSOLE) {
            #-------------- run qt deploy utility (copies dlls and frameworks to deployment dir)

            path_to_qtwindeploy = $$[QT_INSTALL_BINS]\\windeployqt.exe


            deploy_opts = "--compiler-runtime"
            !isEmpty(INCLUDE_QML) {
                deploy_opts += " --qmldir \"$$PWD\""
            }


            QMAKE_POST_LINK += echo "Running qtwindeploy: $$package_dir" $$escape_expand(\\n\\t)
            QMAKE_POST_LINK += $$path_to_qtwindeploy $$deploy_opts --dir \"$$package_dir\" \"$$binary_dest\" $$escape_expand(\\n\\t)

            #----------------- copy SSL dlls ----------------------------

            QMAKE_POST_LINK += echo "Copying SSL dlls to package_dir" $$escape_expand(\\n\\t)
            QMAKE_POST_LINK += copy /y \"$$LIBCRYPTO_DST\" \"$$package_dir\" $$escape_expand(\\n\\t)
            QMAKE_POST_LINK += copy /y \"$$LIBSSL_DST\" \"$$package_dir\" $$escape_expand(\\n\\t)

            #----------------- copy changelog ----------------------------

            changelog_src = "$${repo_root_dir}\\CHANGELOG.md"

            QMAKE_POST_LINK += echo "Copying changelog" $$escape_expand(\\n\\t)
            QMAKE_POST_LINK += copy /y \"$$changelog_src\" \"$$content_dir\" $$escape_expand(\\n\\t)
            #QMAKE_POST_LINK += xcopy /Y /E /I /H /K \"$$content_src\" \"$$package_dir\\_Extras\" $$escape_expand(\\n\\t)

            #----------------- create installer ----------------------------

            path_to_bincreate = "C:\\Qt6\QtIFW-4.6.0\\bin\\binarycreator.exe"
            path_to_installerbase = "C:\\Qt6\QtIFW-4.6.0\\bin\\installerbase.exe"
            path_to_install = $$shell_path($$absolute_path("..\\win-build", $$PWD))
            installer_name = \"$$app_name Installer v$$VERSION\"
            installer_file = \"$$app_name Installer v$${VERSION}.exe\"

            #message("path_to_bincreate is: " $$quote($$path_to_bincreate))
            #message("installer_file is: " $$quote($$installer_file))

            #QMAKE_POST_LINK += echo "Signing installerbase.exe" $$escape_expand(\\n\\t)
            #QMAKE_POST_LINK += $$path_to_signtool sign /v /debug /a /tr http://timestamp.globalsign.com/tsa/advanced /td SHA256 /fd certHash \"$$path_to_installerbase\" $$escape_expand(\\n\\t)

            #QMAKE_POST_LINK += $$path_to_signtool remove /s \"$$path_to_installerbase\" $$escape_expand(\\n\\t)


            QMAKE_POST_LINK += echo "Creating Installer" $$escape_expand(\\n\\t)
            QMAKE_POST_LINK += cd \"$$path_to_install\" $$escape_expand(\\n\\t)
            QMAKE_POST_LINK += \"$$path_to_bincreate\" --offline-only -c config/config.xml -p packages $$installer_name $$escape_expand(\\n\\t)

            QMAKE_POST_LINK += echo "Signing Installer" $$escape_expand(\\n\\t)
            QMAKE_POST_LINK += $$path_to_signtool sign /v /debug /a /tr http://timestamp.globalsign.com/tsa/advanced /td SHA256 /fd certHash $$installer_file $$escape_expand(\\n\\t)
        }
    }

}






