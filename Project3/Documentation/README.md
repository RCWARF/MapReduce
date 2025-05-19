Overview
--------

CSE687_Project3.sln VS solution has the following projects.

1) MapReduceInterfaces - Contains MapBase and ReduceBase abstract classes for map and reduce. Workflow would invoke map and reduce methods through these base classes
2) MapLibrary - Contains DLL library for Map class implementation. This project generates MapLibrary.dll.
3) ReduceLibrary - Contains DLL library for Reduce class implementation. This project generates ReduceLibrary.dll.
4) MapReduce - Contains the implementation for Executive, Workflow and Sort classes. This project generates 

   Threading
   ---------
   The Workflow creates threads to invoke map and reduce functions.
   
   TODO -
   1. Create multiple mapper and multiple reducer processes.
   2. Create a partition function that splits keys into R buckets (where R is the number of reducer processes). Each mapper will generate R output files
      corresponding to the number of reducers.
   3. The reducers can be created once all the mappers are complete.
   4. The directories and file naming convention can be configurable.
   5. Your command line executable (controller) will:
      a. Assign the files in a reasonable way amongst the map
         processes.
      b. Wait for all map processes to complete successfully.
      c. Create reducer processes. (This can be done before all the
         map processes have completed.)
      d. Execute the sorting.
      e. Kick-off the reducers.
      f. Wait for all reducer processes to complete successfully.
      g. Perform a final reduction of all results.
      h. Write the SUCCESS file indicating a successful run.
   7. Notice that the sorting and grouping should happen as part of the
      reducer process.
   8. The mapper process and reducer process are separate entities from
      the DLLs. The mapper process will need to run the mapper
      function from the DLL, same goes for the reducer.
   9. You may either have a single executable that can do both mapper
      and reducer OR you may choose to split these into separate
      executable projects. Either is fine.
   10. No network communication is required for this project. The
      controller can get the exit codes from the mappers & reducers to
      measure success. 
 
   
Build
-----
Launch CSE687_Project3.sln to make changes and build

Installing boost library
------------------------

Reference - http://www.boost.org/build/doc/html/bbv2/overview/invocation.html 
Download the latest boot version (1_85_0) from http://www.boost.org/doc/libs/ and extract under C:\boost_1_85_0.
Build all packages with the following steps.
1) Run the bootstrap script (bootstrap.bat) prior to building anything.
2) Release build step -  b2 variant=release address-model=64 link=static,shared. 64-bit static and dynamic binaries in Release configuration for all available Boost components.
3) Debug build step - b2 variant=debug address-model=64 link=static,shared


