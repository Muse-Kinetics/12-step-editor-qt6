#!/bin/bash

# This script packages the K-Mix mac application and handles the Apple signing/notarization process.
# - You must use the entitlements included in this directory.

# app_name is the name of the dmg volume
this_year=2024
app_name="12 Step Editor"
app_name_fp=12\ Step\ Editor
version=3.0.5

bundle_name=12StepEditor
bundle_id="com.keithmcmillen.$bundle_name"
app_source="../build-12StepEditor-Qt_6_3_2_for_macOS-Release/$app_name.app"
app_qml_dir="../QT/"
dmg_path=./dmg
dmg_bg="$dmg_path/background2.tiff"
dmg_icon="./$app_name.icns"
subfolder_path="./dmg/$app_name"
changelog_source="../CHANGELOG.md"
changelog_dest="$subfolder_path/CHANGELOG.md"
content_source="../Content"
content_dest="$subfolder_path/Content"
app_path="$subfolder_path/$app_name.app"
path_to_dqt=~/Qt/6.3.2/macos/bin/macdeployqt
developer_id="Developer ID Application: Kesumo, LLC (${APPLE_TEAM_ID})"
final_dmg_name="./$app_name Mac v$version.dmg"
readme_name="README.pdf"


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
	echo "Cleaning out old editor..."
	echo ""
fi

if [ -f "$changelog_dest" ] 
then
	rm -rf "$changelog_dest"
	echo ""
	echo "Cleaning out old changelog..."
	echo ""
fi

if [ -f "$content_dest" ] 
then
  rm -rf "$content_dest"
  echo ""
  echo "Cleaning out old changelog..."
  echo ""
fi

# echo "Press any key to continue"
# echo
# read -n 1 -s -r -p ""

#echo "Copying $app_source -> $subfolder_path/"
# copy the .app and resources
\cp -R "$app_source" "$subfolder_path/"
\cp -R "$changelog_source" "$subfolder_path/"
\cp -R "$content_source" "$subfolder_path/"

echo ""
echo "### - Updating info.plist"
echo ""
# echo "Press any key to continue"
# echo
# read -n 1 -s -r -p ""

# update info.plist
plutil -insert "App Category" -string "Music" "$app_path/Contents/Info.plist"
plutil -replace "Get Info string" -string "$version, Copyright $this_year Keith McMillen Instruments" "$app_path/Contents/Info.plist"
plutil -replace "CFBundleLongVersionString" -string "$app $version, Copyright $this_year Keith McMillen Instruments" "$app_path/Contents/Info.plist"
plutil -replace "Bundle versions string, short" -string "$version" "$app_path/Contents/Info.plist"
plutil -replace "Bundle version" -string "$version" "$app_path/Contents/Info.plist"

echo ""
echo "### - Cleaning DMG directory attributes"
echo ""
# echo "Press any key to continue"
# echo
# read -n 1 -s -r -p ""

# clean dmg attributes
xattr -cr "$dmg_path/"

echo ""
echo "### - Running macdeployqt"
echo ""
# echo "Press any key to continue"
# echo
# read -n 1 -s -r -p ""

#run macdeployqt
$path_to_dqt "$app_path" -verbose=2 -codesign="$developer_id" -qmldir="$app_qml_dir" -executable="$app_path/Contents/MacOS/$app_name"

echo ""
echo "### - Signing application"
echo ""
# echo "Press any key to continue"
# echo
# read -n 1 -s -r -p ""

# sign the code
codesign -s "$developer_id" --options runtime --timestamp --force --deep --entitlements ./entitlements.mac.plist -f "$app_path"

echo ""
echo "### - Setting subfolder icon - fileicon set $subfolder_path/ $dmg_icon"
echo ""
# echo "Press any key to continue"
# echo
# read -n 1 -s -r -p ""

fileicon set "$subfolder_path/" "$dmg_icon"

echo ""
echo "### - Creating dmg..."
echo ""
# echo "Press any key to continue"
# echo
# read -n 1 -s -r -p ""

# delete dmg if it already exists
test -f "$final_dmg_name" && rm "$final_dmg_name"

create-dmg \
  --volname "$app_name" \
  --volicon "$dmg_icon" \
  --background "$dmg_bg" \
  --window-pos 200 120 \
  --window-size 530 380 \
  --icon-size 90 \
  --icon "$readme_name" 265 100 \
  --icon "$app_name" 155 220 \
  --app-drop-link 370 220 \
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

# remove Icon? file
rm -rf "$subfolder_path/Icon^M"

echo ""
echo "### - Clean dmg attributes..."
echo ""
# echo "Press any key to continue"
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

# submit and notarize

# note - to use the new notarytool you must install a profile/keychain using the following steps in terminal:
# (see https://scriptingosx.com/2021/07/notarize-a-command-line-tool-with-notarytool/)

# 1) Find the profile name by entering: security find-identity -p basic -v
# 2) The profile name is the digits in parenthesis at the end of: "Developer ID Application: Kesumo, LLC (${APPLE_TEAM_ID})"
# 3) Store the credentials by entering: xcrun notarytool store-credentials --apple-id "${APPLE_ID}" --team-id "${APPLE_TEAM_ID}"
# 4) Enter the profile name when prompted
# 5) Enter the app specific password (signing / notarization) for the apple id
# 6) Use the profile id from step #4 in the command below

xcrun notarytool submit "$final_dmg_name" --keychain-profile "${APPLE_KEYCHAIN_PROFILE}" --wait

# deprecated command
#xcrun altool --notarize-app -f "$final_dmg_name" -t osx -u ${APPLE_ID} -p ${APPLE_APP_PASSWORD} -primary-bundle-id $bundle_id

rm -rf ./dmg/12\ Step\ Editor/Icon$'\r

