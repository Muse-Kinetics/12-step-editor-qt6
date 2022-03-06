#!/bin/bash

# This script packages the K-Mix mac application and handles the Apple signing/notarization process.
# - You must use the entitlements included in this directory.

# app_name is the name of the dmg volume
this_year=2022
app_name="12 Step Editor"
app_name_fp=12\ Step\ Editor
app_source="../Build/5.14.2/$app_name.app"

version=2.1.0
bundle_id="com.keithmcmillen.$app_name"
dmg_path=./dmg
subfolder_path="./dmg/$app_name"
app_path="$subfolder_path/$app_name.app"
dmg_bg="$dmg_path/background.tiff"
dmg_icon="./$app_name.icns"
path_to_dqt=~/Qt/5.14.2/clang_64/bin/macdeployqt

qml_dir=../QT/
developer_id="Developer ID Application: Kesumo, LLC (${APPLE_TEAM_ID})"

final_dmg_name="./$app_name Mac v$version.dmg"


# set current directory to where the script was called from
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd "$DIR"

clear

echo "$app_name Builder v$version"
echo 
echo "Press any key to continue"
echo
read -n 1 -s -r -p ""

if [ -d "$app_path" ] 
then
	rm -rf "$app_path"
	echo ""
	echo "Cleaning out old app..."
	echo ""
fi


# copy the .app and resources
\cp -R "$app_source" "$subfolder_path/"


echo ""
echo "### - Updating info.plist"
echo ""

# update info.plist
plutil -insert "App Category" -string "Music" "$app_path/Contents/Info.plist"
plutil -replace "Get Info string" -string "$version, Copyright 2021 Keith McMillen Instruments" "$app_path/Contents/Info.plist"
plutil -replace "CFBundleLongVersionString" -string "$app_name $version, Copyright $this_year Keith McMillen Instruments" "$app_path/Contents/Info.plist"
plutil -replace "Bundle versions string, short" -string "$version" "$app_path/Contents/Info.plist"
plutil -replace "Bundle version" -string "$version" "$app_path/Contents/Info.plist"


echo ""
echo "### - Running macdeployqt"
echo ""

#run macdeployqt
$path_to_dqt "$app_path" -verbose=2 -codesign="$developer_id" -qmldir="$qml_dir" -executable="$app_path/Contents/MacOS/$app_name"
codesign -s "$developer_id" --options runtime --timestamp --force --deep --entitlements ./entitlements.mac.plist -f "$app_path/Contents/Frameworks/QtPrintSupport.framework/Versions/5/Resources/QtPrintSupport.prl"
#codesign -s "$developer_id" --options runtime --timestamp --force --deep --entitlements ./entitlements.mac.plist -f "$app_path/Contents/Frameworks/QtQuickWidgets.framework/Versions/5/Resources/QtQuickWidgets.prl"

echo ""
echo "### - Cleaning attributes"
echo ""

xattr -cr "$app_path"

echo ""
echo "### - Signing application"
echo ""

# sign the code
# QT frameworks plists copied. See: http://barenka.blogspot.com/2014/04/signing-qt5-520-applications-in-mac-osx.html
codesign -s "$developer_id" --options runtime --timestamp --force --deep --entitlements ./entitlements.mac.plist -f "$app_path"


#set folder icon (only needed if there is a folder in the dmg)
#fileicon set "$dmg_path/" "$icon_file"

# create dmg (brew install create-dmg)

echo ""
echo "### - Creating dmg..."
echo ""

# delete dmg if it already exists
test -f "$final_dmg_name" && rm "$final_dmg_name"

create-dmg \
  --volname "$app_name" \
  --volicon "$dmg_icon" \
  --background "$dmg_bg" \
  --window-pos 200 120 \
  --window-size 530 380 \
  --icon-size 100 \
  --icon "$app_name" 160 220 \
  --app-drop-link 375 220 \
  --hdiutil-verbose \
  "$final_dmg_name" \
  "$dmg_path/"

# # change folder icon and inspect
# open "$final_dmg_name"
# sleep 4 # wait for image to mount
# open "/Volumes/$app_name"

# echo ""
# echo "### - Inspect volume, eject and press any key to continue"
# echo
# read -n 1 -s -r -p ""

# clean dmg attributes
xattr -cr "$final_dmg_name"

echo ""
echo "### - Signing dmg...."
echo

# sign the dmg
codesign -s "Developer ID Application: Kesumo, LLC (${APPLE_TEAM_ID})" --options runtime --timestamp --force --deep --entitlements ./entitlements.mac.plist -f  "$final_dmg_name"

echo ""
echo "### - Notorizing..."
echo
# submit to notarize
xcrun altool --notarize-app -f "$final_dmg_name" -t osx -u ${APPLE_ID} -p ${APPLE_APP_PASSWORD} -primary-bundle-id $bundle_id


