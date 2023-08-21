# E21010

FRIB experiment 21010 studies the beta decays of 22Al and 26P. 
The experiment was carried out in June-July 2023 at FRIB, MSU, East Lansing, Michigan, USA.

## Dependencies

### For data analyses

* ROOT
* GSL
* [AUSAlib](https://gitlab.au.dk/ausa/ausalib) -- rough guide on its installation [here](https://gitlab.au.dk/ausa/erik/ausa-install) -- and a project wiki [here](https://gitlab.au.dk/ausa/ausalib/-/wikis/home)
* [telescope](https://gitlab.au.dk/ausa/erik/telescope) -- an AUSAlib extension library
* [libconfig++](https://hyperrealm.github.io/libconfig/) -- available in most package managers!

### For simulations

* ROOT
* Geant4
* AUSAlib
* [G4Sim](https://gitlab.au.dk/ausa/g4sim) -- rough guide on its installation [here](https://gitlab.au.dk/au479664/G4Sim/-/wikis/Installation)

If you are only here for the data analyses, comment out the line that says

```cmake
add_subdirectory(sim)
```

in the file `CMakeLists.txt`.

## Setting up the project

### Path to the data

From the root of the project, create an absolute link by the name of `data` to a directory which contains the data from the experiment

```shell
ln -s /absolute/path/to/data/directory data
```

The contents of the data directory are expected to be similar to the following

```shell
user@hostname:somewhere> tree -d /absolute/path/to/data/directory
/absolute/path/to/data/directory
├── raw
├── sorted
└── unpacked
```

#### A few notes on the data

Only the data contained in `data/sorted` are necessary to carry out the various analyses.
Data files in `data/sorted` have signature `XXX_YYYm.root` where `XXX` is a zero-padded run number and `YYY` is a zero-padded sub-run number.
These files contain calibrated (taking geometry, energy losses of the alpha particles from the calibration source, etc. into account) and front-back-matched (in the case of DSSDs) energy spectra for all detectors of the setup.
The prerequisites for creating these ***sorted files*** is the `Sorter` tool (located [here](https://gitlab.au.dk/ausa/sorter)) which takes a setup file, `setup/setup.json`, a matcher file, `matcher.json`, and the ***unpacked files*** contained in `data/unpacked`.
The setup file, apart from specifying the geometry of the setup, points to files containing calibration coefficients `setup/*.cal`.
Data files in `data/unpacked` have signature `XXX_YYY.root`.
These files are created by running the ***raw files*** contained in `data/raw` through our unpacker. Redoing the unpacking will hopefully not be necessary for you, the reader (but sorting the unpacked files might, if you are unhappy with the calibrations/geometry)!


### Building the data analyses and/or simulations

From the root of the project, run

```shell
mkdir build
cd build
cmake ..
cmake --build .
```

## Running the data analyses

From within the `build` directory, locate the executable `bp` and run it, like so

```shell
./bp <path/to/libconfig++/configuration/file> <run number>
```

There are example configuration files in `analysis/*.cfg` -- at least one configuration file per type of analysis.

## Running the simulations

TODO.

## Elog

Elog for the experiment is [here](https://elog.kern.phys.au.dk/e21010/).