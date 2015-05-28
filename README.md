# KDB+ Feed Handler Tutorial

This project demonstrates how to build a feedhandler that interfaces with kdb+.

Additional PDF documentation on the C API can be found [here][gitpdfdoc] 

Installation & Setup
--------------------

### Extra Resources

This project uses CMake 2.6+ to build across multiple platforms. It has been tested on Linux and
Windows. Execute the following commands on all platforms to create platform appropriate build
files within the `build` directory.

```sh
mkdir build; cd build; cmake ..
```

### <img src="icons/linux.png" height="16px"> Building on Linux

On Linux, you just need to run make install to complete the build process
and find the binary output in the `../bin` directory.

```sh
make install && cd ../bin
```

### <img src="icons/windows.png" height="16px"> Building on Windows

On Windows platforms you will need to have the msbuild.exe available on your path. CMake creates
two Visual Studio projects that need to be built. The `INSTALL` project will not modify any of the
code and will just move the binaries from the `../build` directory to the `../bin` directory. An
extra `libqtoc.lib` file will be produced on Windows, which can be ignored after the build process.

```bat
msbuild ./ALL_BUILD.vcxproj /p:Configuration=Release
msbuild ./INSTALL.vcxproj /p:Configuration=Release
cd ../bin
```

Running the Examples
--------------------

The resulting directory after running a build should look like this:

    bin/                    -- contains the fakefeed.[dll/so] and feedhandler.[dll/so] libraries and run.q
    build/                  -- contains the makefiles/visual studio projects
    src/                    -- contains the source code
    CMakeLists.txt

Once the build is complete, navigate to the `bin` directory and execute:

    q run.q

on Windows plaforms or on Linux:

    ./run.sh

The `run.sh` script just sets the `LD_LIBRARY_PATH` to look in the current directory before running
the run.q script.

This will load the C shared objects and bind the functions to names (*init* and *halt*). Instructions
and example commands should be displayed as soon as the the run.q script loads.

```apl
##
# run.q
#
# Example q script that loads in two functions from the feedhandler library
# and defines two tables (quote and trade). Calling the init function will
# start the feed handler and push quotes and trades into their respective
# tables. The halt function will stop the feed handler and release any
# resources it held.
#
# AquaQ Analytics
# kdb+ consultancy, training and support
#
# For questions, comments, requests or bug reports, please contact us
# w: www.aquaq.co.uk
# e: support@aquaq.co.uk
#
# examples:
#       init[] to start the feed handler
#       halt[] to stop the feed handler
#

q) init[]
q) count trade
75376
q) count quote
848361
q) halt[]
q) -10#quote
time                           sym  exg   asksize bidsize askprice bidprice sequence cond
----------------------------------------------------------------------------------------------------
2015.04.02D13:00:33.806996000 `YHOO `LSE  463i    303i    20.2     18.45    131364i  0x434040204f220200f88d
2015.04.02D13:00:33.806996000 `MSFT `LSE  118i    45i     84.6     83.06    74369i   0x40204949867f00000080
2015.04.02D13:00:33.806996000 `APPL `LSE  191i    110i    81.9     81.852   120771i  0x49464349621013404d62
2015.04.02D13:00:33.806996000 `IBM  `LSE  180i    409i    86.7     85.364   98308i   0x4343435a000000001806
2015.04.02D13:00:33.806996000 `YHOO `LSE  331i    138i    87.4     85.584   110350i  0x46405a46fb000000e502
...
```

Other Resources
---------------

This resource is intended to suppliment the existing Kx Wiki sections on interfacting with C
and provide some concrete examples. Readers should look at the following pages on the Kx Wiki:

* [Interfacing With C][kxwikiinterface]
* [Extending With C][kxwikiextend]

[aquaqwebsite]: http://www.aquaq.co.uk  "AquaQ Analytics Website"
[aquaqresources]: http://www.aquaq.co.uk/resources "AquaQ Analytics Website Resources"
[gitpdfdoc]: https://github.com/markrooney/kdb-c-interface/blob/master/docs/InterfacingWithC.pdf
[kxwikiinterface]: http://code.kx.com/wiki/Cookbook/InterfacingWithC "Kx Wiki Interfacing with C"
[kxwikiextend]: http://code.kx.com/wiki/Cookbook/ExtendingWithC "Kx Wiki Extending with C"
