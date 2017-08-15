# tilengine cmake project-config input for ./configure scripts

set(TILENGINE_LIBDIR "<libpath>")
set(TILENGINE_INCLUDE_DIRS "<incpath>")
set(TILENGINE_LIBRARIES "<libpath>/<library> ")
string(STRIP "${TILENGINE_LIBRARIES}" TILENGINE_LIBRARIES)
