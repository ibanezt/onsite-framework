How to compile the GameClient (should work for Linux and Windows):
1) 	Install the SDK and NDK wherever you like.
2) 	Install the ADT Plug-in for Eclipse to install the 
3) 	Open local.properties (in the root of the project)
4) 	Include/change 
		sdk.dir=C\:\\Users\\Sirius\\AppData\\Local\\Android\\sdk 
	to your android sdk path
5)	Include/change
		ndk.dir=C\:\\android-ndk-r10d
	to your android ndk path
6)	Open your OS's command line and cd to the root folder of the GameClient project (../GameClient/GameClient/).
	Then execute the following:
		>(path to ndk)\ndk-build
7)	Open the project in Eclipse and run the project as an android project. (which it should already be)