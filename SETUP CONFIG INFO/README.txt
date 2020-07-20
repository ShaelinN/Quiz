*************************************
*	SETTING UP THIS PROJECT	    *
*	FOR THE FIRST TIME	    *
*	IN VISUAL STUDIO	    *
*************************************
The .vs folder houses the project dependencies,
but to submit with a viable file size, it had to be deleted
to run this project in Visual Studio, do the following series of actions

1. Configure SFML dependencies before running the project.

	go to project > properties. Switch configuration to All Configurations

	A) Go to:
	project > properties > C/C++ > General> add additional include dependencies:
	add the following statement, typing the install directory of SFML in place of the 
	place holder SFML_Inst_Dir:

	SFML_Inst_Dir/include



	B) Go to:
	project > properties > Linker > General > add additional library dependencies:
	add the following statement, typing the install directory of SFML in place of the 		
	placeholder SFML_Inst_Dir:

	SFML_Inst_Dir/lib



	C)
		(I) switch configuration to Debug
		go to:
		project > properties > Linker > Input > additional dependencies
		add the following:

		sfml-graphics-d.lib
		sfml-window-d.lib
		sfml-system-d.lib
		sfml-audio-d.lib

		(II) switch configuration to Release
		go to:
		project > properties > Linker > Input > additional dependencies
		add the following:

		sfml-graphics.lib
		sfml-window.lib
		sfml-system.lib
		sfml-audio.lib
2. DLL files 
	In the event that Visual Studio claims that any sfml dll file is corrupted, 
	it is safe to overwrite them with a new copy of the dll files.
	These are found in SFML_Inst_Dir/bin.

	The project was written using SFML 2.5.1, so if your sfml version is different, 
	it may also be necessary to overwrite the sfml dlls with the dlls from the 
	version you are linking as a dependency