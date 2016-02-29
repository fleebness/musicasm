# musicasm
A flexible C++ framework for assembling music

Requires PortAudio from http://www.portaudio.com, and a modern C++ compiler.


Windows:

* Create an SDKS folder, and an SDKS environment variable pointing to that folder.
* Download and build PortAudio within this SDKS folder.  You want to build from the
  portaudio\build\msvc folder's project file.
* Copy the PortAudio DLLs to your own folders as appropriate.
* I may find myself using boost libraries in the future.  If I do, I will build to
  $(sdks)\boost\include for headers, $(sdks)\boost\lib32 for 32-bit, and
  $(sdks)\boost\lib64 for 64-bit libraries.  I am unlikely to use any other libraries.

Linux:

 --- coming

