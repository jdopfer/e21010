# IS507 V3

IS507 is an experimental campaign in two parts.
The first part is focused on 21Mg, and the second part on 20Mg.
Between the two parts of the campaign, the experimental setups were very different.

This project carries out extensive data analyses on 21Mg data from the second part of 
the experimental campaign which was actually focused on 20Mg - 21Mg were "merely" 
calibration runs in this context.

Experiment, with focus on 20Mg, was part of Morten's thesis (v1); Sofie inherited 21Mg (v2).
Erik then continued the analysis of 21Mg (v3).

## Setting up the project

Make a **build directory** from which CMake can interpret the `CMakeLists.txt` contained 
in this **project directory**;

e.g.

```shell
cd /path/to/is507_v3
mkdir build
cd build
cmake ..
```

CMake will complain if you have a piece of software missing and/or if you do not appear 
to be in possession of the relevant data files.
Inspect the CMake files (they are written to be quite understandable) which give any 
errors and follow the instructions of the error messages.

CMake is made to take care of *sorting* the unpacked data files for you.

When that is done, from within the **build directory**, do

```shell
cmake --build .
```

## Order of proceedings

*Alternate title: "The order of logical steps through the data analysis"*

Each step is contained within the following relative paths/files:

1. [matcher_checks/checks.md](matcher_checks/checks.md)
2. Coming soon!

## Note

Where any reference to a file by the name `all.root` (e.g. in a Jupyter Notebook) appears, 
this file has been created by running `hadd -f all.root 21Mg_*` in the relevant data 
output directory.
This command simply concatenates all input files `21Mg_*` into the output file `all.root`.