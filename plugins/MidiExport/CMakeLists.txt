# check for drumstick-file

IF(LMMS_HAVE_FLUIDSYNTH)
	PKG_CHECK_MODULES(DRUMSTICK_FILE REQUIRED drumstick-file>=0.5.0)
	IF(NOT DRUMSTICK_FILE_FOUND)
		MESSAGE(FATAL_ERROR "LMMS requires libdrumstick-file and drumstick-dev >= 0.5.0 - please install, remove CMakeCache.txt and try again!")
	ENDIF(NOT DRUMSTICK_FILE_FOUND)
	INCLUDE(BuildPlugin)
	LINK_DIRECTORIES(${DRUMSTICK_FILE_LIBDIR})
	INCLUDE_DIRECTORIES(${DRUMSTICK_FILE_INCLUDEDIR})
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")
	BUILD_PLUGIN(midiexport MidiExport.cpp midiWriter.cpp
		MOCFILES MidiExport.h midiWriter.h)
	TARGET_LINK_LIBRARIES(midiexport ${DRUMSTICK_FILE_LIBRARIES})
ENDIF(LMMS_HAVE_FLUIDSYNTH)
