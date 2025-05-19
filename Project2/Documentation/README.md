Overview
--------

CSE687_Project2.sln contains four VS projects.

1) MapReduceInterfaces - Contains MapBase and ReduceBase abstract classes for map and reduce. Workflow would invoke map and reduce methods through these base classes
2) MapLibrary - Contains DLL library for Map class implementation. This project generates MapLibrary.dll.
3) ReduceLibrary - Contains DLL library for Reduce class implementation. This project generates ReduceLibrary.dll.
4) MapReduce - Contains the implementation for Executive, Workflow and Sort classes. This project generates 

CSE687_Project2.sln VS solution that has all the above projects to make changes and build.

Installing boost library
------------------------

Reference - http://www.boost.org/build/doc/html/bbv2/overview/invocation.html 
Download the latest boot version (1_85_0) from http://www.boost.org/doc/libs/ and extract under C:\boost_1_85_0.
Build all packages with the following steps.
1) Run the bootstrap script (bootstrap.bat) prior to building anything.
2) Release build step -  b2 variant=release address-model=64 link=static,shared. 64-bit static and dynamic binaries in Release configuration for all available Boost components.
3) Debug build step - b2 variant=debug address-model=64 link=static,shared


