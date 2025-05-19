Overview
--------

CSE687_Project4.sln VS solution has the following projects.

1) MapReduceInterfaces - Contains MapBase and ReduceBase abstract classes for map and reduce. Workflow would invoke map and reduce methods through these base classes
2) MapLibrary - Contains DLL library for Map class implementation. This project generates MapLibrary.dll.
3) ReduceLibrary - Contains DLL library for Reduce class implementation. This project generates ReduceLibrary.dll.
4) MapReduce - Contains the implementation for Executive, Workflow and Sort classes. This project generates 
5) SocketsComm - Contains the code from https://ecs.syr.edu/faculty/fawcett/handouts/CSE687-OnLine/Repository/Comm/.
6) Stub - Contains the code for stubs that runs map and reduce processes. It includes SocketComm code as well.

   Threading
   ---------
   The Workflow creates threads to invoke map and reduce functions.
   
Build
-----
Launch CSE687_Project4.sln to make changes and build

Execute
-------

Open two different command terminals and run the following from the CSE687MapReduce\Project4\Source\x64\Debug folder

Note: You must run Stubs.exe BEFORE running MapReduce.exe.


.\Stubs.exe --map-dll ".\MapLibrary.dll" --reduce-dll ".\ReduceLibrary.dll" --stubs localhost:9091 localhost:9092

.\MapReduce.exe --input "C:\CSE687MapReduce\InputFiles\shakespeare" --temp ".\temp" --output ".\output" --stubs localhost:9091 localhost:9092 --mappers 2 --reducers 3

If above applications completed with no errors, the results of the map reduce operation will be located in the specified output directory. 

reduce.txt - file containing the map reduce output

SUCCESS - file indicating the operation completed successfully

Installing boost library
------------------------

Reference - http://www.boost.org/build/doc/html/bbv2/overview/invocation.html 
Download the latest boot version (1_85_0) from http://www.boost.org/doc/libs/ and extract under C:\boost_1_85_0.
Build all packages with the following steps.
1) Run the bootstrap script (bootstrap.bat) prior to building anything.
2) Release build step -  b2 variant=release address-model=64 link=static,shared. 64-bit static and dynamic binaries in Release configuration for all available Boost components.
3) Debug build step - b2 variant=debug address-model=64 link=static,shared


