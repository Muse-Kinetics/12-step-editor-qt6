clear

appName="12 Step Editor"

appLocation1="/Applications/$appName/$appName.app/Contents/MacOS/$appName"
appLocation2="/Applications/$appName.app/Contents/MacOS/$appName"

if test -f "$appLocation1"; then
    "$appLocation1"
else
	if test -f "$appLocation2"; then
    	"$appLocation2"
	else
		echo "$appName.app not detected in /Applications"
		echo ""
		echo "Please make sure that the $appName application executable is located in the /Applications directory."
		echo ""
		echo "You can manually run the editor in console mode with the following command:"
		echo ""
		echo "[path to \"$appName.app\"]/Contents/MacOS/$appName"
		echo ""
		echo "Press any key to quit"
		read -n 1 -s -r -p ""
		exit
	fi
fi

echo "The $appName has quit."
echo ""
echo "Press any key to continue"
echo
read -n 1 -s -r -p ""