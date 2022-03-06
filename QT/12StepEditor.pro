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

INCLUDEPATH +=  presetimageformatting \
                presetimageformatting/DeviceManager \
                presetimageformatting/TreeView


SOURCES += main.cpp\
    inc/KMI_KMDM/KMI_mdm.cpp \
    inc/KMI_KMDM/fwupdate/fwupdate.cpp \
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

HEADERS  += mainwindow.h \
    inc/KMI_KMDM/KMI_DevData.h \
    inc/KMI_KMDM/KMI_FwVersions.h \
    inc/KMI_KMDM/KMI_SysexMessages.h \
    inc/KMI_KMDM/KMI_mdm.h \
    inc/KMI_KMDM/fwupdate/fwupdate.h \
    inc/KMI_KMDM/midi.h \
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
    forms/fwoodform.ui \
    forms/fwprogressform.ui \
    forms/fwupdatecompleteform.ui \
    forms/aboutFormWin.ui \
    forms/deleteFormWin.ui \
    forms/fwoodformWin.ui \
    forms/fwprogressformWin.ui \
    forms/fwupdatecompleteformWin.ui \
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
    inc/KMI_KMDM/fwupdate/fwupdate.ui

#-------------------MIDI--------------------#
#-------------------------------------------#

INCLUDEPATH += \
    inc/rtmidi \
    inc/KMI_Ports \
    inc/KMI_KMDM \
    inc/KMI_KMDM/fwupdate \
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

#macx{

##INCLUDEPATH += ./source/midiio/mac

## source/midiio/mac/mididevice.h \
##HEADERS +=  source/midiio/mac/mididevicemanager.h
##SOURCES +=  source/midiio/mac/mididevicemanager.cpp

#LIBS += -framework Accelerate
#LIBS += -framework AudioToolbox
#LIBS += -framework Carbon
#LIBS += -framework Cocoa
#LIBS += -framework CoreAudio
#LIBS += -framework CoreMIDI
#LIBS += -framework DiscRecording
#LIBS += -framework IOKit
#LIBS += -framework OpenGL
#LIBS += -framework QTKit
#LIBS += -framework QuartzCore
##LIBS += -framework QuickTime
#LIBS += -framework WebKit

#OBJECTIVE_SOURCES += \

#}

#win32{

##INCLUDEPATH += ./source/midiio/windows
##HEADERS += ./source/midiio/windows/mididevicemanager.h
##SOURCES  += ./source/midiio/windows/mididevicemanager.cpp


#LIBS += \
#  -lcomdlg32 \
#  -limm32 \
#  -lole32 \
#  -loleaut32 \
#  -lrpcrt4 \
#  -lshlwapi \
#  -luuid \
#  -lversion \
#  -lwininet \
#  -lwinmm \
#  -lws2_32 \
#  -lwsock32 \
#  -lshell32
#}

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

macx{

    twelveStepPresets.files = $$PWD/presets
    twelveStepPresets.path = Contents/Resources
    QMAKE_BUNDLE_DATA += twelveStepPresets
}

win32{

    presets.commands = $(COPY_DIR) $$shell_path(\"$$PWD/presets\") $$shell_path(\"$$OUT_PWD/release/presets\")
    export(presets.commands)

    first.depends += $(first) presets
    export(first.depends)

    QMAKE_EXTRA_TARGETS += first presets
}

linux{
    presets.commands = $(COPY_DIR) $$shell_path(\"$$PWD/presets\") $$shell_path(\"$$OUT_PWD/presets\")
    export(presets.commands)

    first.depends += $(first) presets
    export(first.depends)

    QMAKE_EXTRA_TARGETS += first presets
}
