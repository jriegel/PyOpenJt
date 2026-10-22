# PyOpenJt

Library and some tools to read and convert OpenJT files. 

## Build

Build the whole project in Debug:

    cmake --build .\WinBuild\

## Data

For smoke testing we have two small files:
* data/ExampleFiles/example_block_jt8.1.jt 
* data/ExampleFiles/example_block_jt9.5.jt

## Testing

One way to test the lib is the JtDump.exe. It gets a JT file as parameter and prints out the content as JSON. 
The executable is in the cMake build directory:
    
    WinBuild/Debug/JtDump.exe

example call:

   F:\Workspaces\PyOpenJt\WinBuild\Debug\JtDump.exe F:\Workspaces\PyOpenJt\Data\ExampleFiles\example_block_jt9.5.jt

This dumps a JSON on STDOUT. STDERR shows some warnings.
