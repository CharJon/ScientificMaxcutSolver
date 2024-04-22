# sms - Scip MaxCut Solver

## Cite

If you make use of this code, please cite the corresponding publication:

```latex
@inproceedings{DBLP:conf/acda/CharfreitagMM23,
  author       = {Jonas Charfreitag and
                  Sven Mallach and
                  Petra Mutzel},
  editor       = {Jonathan W. Berry and
                  David B. Shmoys and
                  Lenore Cowen and
                  Uwe Naumann},
  title        = {Integer Programming for the Maximum Cut Problem: {A} Refined Model
                  and Implications for Branching},
  booktitle    = {{SIAM} Conference on Applied and Computational Discrete Algorithms,
                  {ACDA} 2023, Seattle, WA, USA, May 31 - June 2, 2023},
  pages        = {63--74},
  publisher    = {{SIAM}},
  year         = {2023},
  url          = {https://doi.org/10.1137/1.9781611977714.6},
  doi          = {10.1137/1.9781611977714.6},
  timestamp    = {Sat, 28 Oct 2023 13:59:35 +0200},
  biburl       = {https://dblp.org/rec/conf/acda/CharfreitagMM23.bib},
  bibsource    = {dblp computer science bibliography, https://dblp.org}
}
```

## External dependencies

All but one dependency of this project are manged by git (via submodules).
Only SCIP needs to be "installed". The easiest way to make SCIP available on your machine is using one of the "
installers" ZIB offers for download here: https://scipopt.org/index.php#download.
For linux e.g. download the "SCIPOptSuite-8.0.0-Linux-ubuntu.sh" or "SCIPOptSuite-8.0.0-Linux-debian.sh" run it and it
will ask you where to extract all files to. When building cmake will find SCIP now.

## Building

1) clone project
2) make sure SCIP is installed (see step above)
3) run ```git submodule update --init --recursive``` (see further below for bugfixes)
4) now cmake building should work:

```bash
mkdir build && cd build
cmake ..
make -j 10 # -j 10 makes make use 10 threads
```

If SCIP is not on your path, or you want to use a different version of SCIP as the one on your path:

```bash
mkdir build && cd build
cmake .. -DSCIP_DIR:PATH="{my/path/to/scip}"
make -j 10 # -j 10 makes make use 10 threads
```

## Testing

1) build the project as described above (all CMake targets including tests get build by default)
2) stay in the build directory
3) execute the test binary

```bash
./test/your_test --gtest_filter=*
```

`--gtest_filter` can be used to select the test to run. For example `--gtest_filter=IoTest*` will run all test in the
`IoTest` testsuite, ` --gtest_filter=IoTest.ParseEdgelist` will only run the `ParseEdgelist` test.

The `gtest_filter` can be changed to only execute test matching the expression.

## Benchmarking

Example for how to use the simexpal config:
1) ```cd benchmark```
2) ```simex b make```
3) ```simex e launch --instset rest_test_set```


## git submodules

git submodules can be as awesome as annoying. How to hardreset all
submodules: ```rm -fr .git/config .git/modules && git submodule deinit -f . && git submodule update --init --recursive```

## cli arguments

- GNU suggets: https://www.gnu.org/prep/standards/html_node/Option-Table.html#Option-Table

## SCIP

- SCIP Parameter: https://www.scipopt.org/doc/html/PARAMETERS.php
- SCIP basic
  concepts: http://co-at-work.zib.de/berlin2009/downloads/2009-09-22/2009-09-22-1100-SH-Basic-Concepts-SCIP.pdf
- SCIP 8.0 release notes: https://arxiv.org/pdf/2112.08872.pdf

#### Cutting Planes in SCIP

- Information on how to implement constraint handlers can be found here: https://www.scipopt.org/doc/html/CONS.php
- Useful slides are here: https://www.scipopt.org/download/slides/SCIP-cuttingPlanes.pdf
- Information for the TSP cutting plane example: https://www.scipopt.org/doc/html/TSP_MAIN.php
- CH slides 2020: https://co-at-work.zib.de/slides/Donnerstag_17.9/SCIP.pdf
- CH slides 2015: http://co-at-work.zib.de/files/20150930-Hendel-slides.pdf

### Event Handler in SCIP

- How to add: https://www.scipopt.org/doc/html/EVENT.php
- Example: https://www.scipopt.org/doc/html/EVENTHDLR_MAIN.php
- Events: https://www.scipopt.org/doc/html/type__event_8h.php

### Primal Heuristics in SCIP

- How to add: https://www.scipopt.org/doc/html/HEUR.php

### Visualize B&B-Tree

- https://www.scipopt.org/doc/html/FAQ.php#visualizebranchandbound

#### Memory Management of and in SCIP

- Reference counter for SCIP objects (like variables, constraints, etc.): https://www.scipopt.org/doc/html/OBJ.php
- Allocating memory, the SCIP way: https://www.scipopt.org/doc/html/MEMORY.php

### Compiling SCIP and linking against different LP-solver

Download link: https://scipopt.org/download/release/scipoptsuite-8.0.2.tgz

### Linking against CPLEX

- Download SCIP
- Replace X.X.X by scip version, make sure all varialbes ($) are set and possibly add cplex version in the following command

```bash
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/opt/scip-X.X.X-cplex -DLPS=cpx -DCPLEX_DIR=$CPLEX_DIR -DIPOPT=off
```

### Advanced

Not needed by default for this project

#### Ipopt

To allow "-DIPOPT_DIR=$IPOPT_DIR" flag:

```bash
git clone https://github.com/coin-or/Ipopt.git
cd Ipopt
mkdir build && cd build
../configure --prefix=/path/to/install/dir
make -j
make install
```

## Compiling SCIP

Download link: https://scipopt.org/download/release/scipoptsuite-8.0.2.tgz

### With CPLEX

```bash
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/opt/scip-X.X.X-cplex -DLPS=cpx -DCPLEX_DIR=$CPLEX_DIR -DIPOPT=off
```

### Advanced

#### Ipopt

```bash
git clone https://github.com/coin-or/Ipopt.git
cd Ipopt
mkdir build && cd build
../configure --prefix=/path/to/install/dir
make -j
make install
```

#### Linking against CPLEX

```bash
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/opt/scip-X.X.X-cplex -DLPS=cpx -DCPLEX_DIR=$CPLEX_DIR -DIPOPT_DIR=$IPOPT_DIR
```
