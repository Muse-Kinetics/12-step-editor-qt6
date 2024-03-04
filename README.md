# 12 Step QT6 #

Deploying on MacOS:
* the "Signing and Notarization" directory contains a shell script to deploy the MacOS binary and content into a dmg file, and then sign/notarize it

Deploying on Windows:
* Deployment is now built into the Qt PRO file, which creates a Windows installer. In the "win-build" directory there are two directories, "packages" and "config", that contain the qtinstaller build scripts. 
