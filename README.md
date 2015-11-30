[![Build Status](https://travis-ci.org/ValeevGroup/tiledarray.svg?branch=master)](https://travis-ci.org/ValeevGroup/tiledarray)

### Synopsis
TiledArray is a scalable, block-sparse tensor library that is designed to aid in rapid composition of high-performance tensor expressions, appearing for example in many-body quantum mechanics. It allows users to compose tensor expressions of arbitrary complexity in native C++ code that closely resembles the standard mathematical notation. The library is designed to scale from a single multicore computer to a massive cluster.

TiledArray is built on top of MADNESS parallel runtime (MADWorld), part of [MADNESS numerical calculus framework](https://github.com/m-a-d-n-e-s-s/madness).

TiledArray is a work in progress. Its development has been possible thanks to generous support from the U.S. National Science Foundation, the Alfred P. Sloan Foundation, and the Camille and Henry Dreyfus Foundation.

### Design Goals
* General-purpose arithmetic on dense and block-sparse tensors;
* High-level (math-like) composition as well as full access to low-level data and algorithms, both from C++
* Massive shared- and distributed-memory parallelism
* Deeply-reusable framework: everything can be customized, from tile types (e.g. to support on-disk or compute-on-the-fly tensors) to how the structure of sparse tensors is described.

### Example Code

The following example expressions are written in C++ with TiledArray. TiledArray use the [Einstein summation convention](http://en.wikipedia.org/wiki/Einstein_notation) when evaluating tensor expressions.

* Matrix-matrix multiplication

    C("m,n") = 2.0 * A("m,k") * B("k,n");

* Matrix-vector multiplication

    C("n") = A("k") * B("k,n");

* A more complex tensor expression
 
    E("m,n") = 2.0 * A("m,k") * B("k,n") + C("k,n") * D("k,m");

The following application is a minimual example of a distributed-memory matrix multiplcation.

```c++
#include <tiledarray.h>

using TA = TiledArray;

int main(int argc, char** argv) {
  // Initialize the parallel runtime
  TA::World& world = TA::initialize(argc, argv);
  
  // Construct a 2D tiled range structure that defines
  // the tiling of an array. Each dimension contains
  // 10 tiles.
  TA::TiledRange trange = 
      { { 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 },
        { 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 } };
  
  // Construct and fill the argument arrays with data
  TA::TArrayD A(world, trange);
  TA::TArrayD B(world, trange);
  A.fill_local(3.0);
  B.fill_local(2.0);
  
  // Construct the (empty) result array.
  TA::TArrayD C;
  
  // Perform a distributed matrix multiplication
  C("i,j") = A("i,k") * B("k,j");
  
  // Tear down the parallel runtime. 
  TA::finalize();
  return 0;
}
```

### Installing TiledArray

#### Dependencies

* **C++ compiler with C++11 support** - Compilers that have been tested include:
  * GCC 4.7.2 and later, 
  * Clang 3.4 and later, 
  * Apple Clang 5.0 and later, and
  * Intel C/C++ Compiler 15 and later.
* **Cmake** 2.8.8 or later
* **Eigen** - Version 3.0 and later. http://eigen.tuxfamily.org
* **MADNESS** - While it is possible to compile MADNESS separately, we recommend compiling MADNESS automatically as part of TiledArray. Compilation of MADNESS requires the following additional prerequisites (see the [MADNESS GitHub page](https://github.com/m-a-d-n-e-s-s/madness) for details):
  * **autoconf** 2.59 or later
  * **automake** 1.11 or later
  * **libtool** 2.x or later
  * **Pthreads**
  * **MPI-2 or MPI-3 library** - [MPICH](http://www.mpich.org), [MVAPICH](http://mvapich.cse.ohio-state.edu), and [Intel MPI](https://software.intel.com/en-us/intel-mpi-library) have been tested. Intel MPI is recommended on x86/Infiniband hardware, but any of these libraries will do. We do not recommend OpenMPI as `MPI_THREAD_MULTIPLE` support is [broken](https://github.com/open-mpi/ompi/issues/157).
  * **LAPACK** and **BLAS** - Serial (sequential, or 1-thread) versions of these libraries is recommended. If you have to use threaded version of these libraries, to avoid poor performance (or even errors) due to non-interoperable threading runtimes it is recommended to configure these libraries to use single thread at runtime before entering the block of TiledArray code.
  * **Intel Threading Building Blocks** (optional) Version 4.3 Update 4 or later. 
* **Doxygen** (optional) - Used to generate for documentation only
* **Boost** (optional) - Version 1.33.0 or later for unit tests only
 
#### Build

TiledArray includes several tool chain files for common platforms. These files contain system specific settings that have been tested on the various platforms. We recommend using one of these 

```
$ git clone https://github.com/ValeevGroup/TiledArray.git tiledarray
$ cd tiledarray
$ mkdir build
$ cd build
$ cmake \
    -D CMAKE_INSTALL_PREFIX=/path/to/tiledarray/install \
    -D CMAKE_TOOLCHAIN_FILE=../cmake/toolchains/osx-clang-mpi-accelerate.cmake \
    ..
```

Common CMake cache variables that you may want to define include:

##### Compiler Variables
```
-D CMAKE_C_COMPILER=/path/to/bin/cc
-D CMAKE_CXX_COMPILER=/path/to/bin/c++
-D MPI_C_COMPILER=/path/to/bin/mpicc
-D MPI_CXX_COMPILER=/path/to/bin/mpicxx
```

##### Option Variables
```
-D CMAKE_BUILD_TYPE=(Release|Debug|RelWithDebInfo)
-D BUILD_SHARED_LIBS=(TRUE|FALSE)
-D TA_ERROR=(none|throw|assert)
```

##### Library Variables
```
-D Madness_ROOT_DIR=/path/to/madness/root/dir
-D TBB_ROOT_DIR=/path/to/tbb/root/dir
-D LAPACK_LIBRARIES=(semicolon seperated list of LAPACK libraries)
-D BLAS_LIBRARIES=(semicolon seperated list of BLAS libraries)
-D BLA_STATIC=(TRUE|FALSE)
-D INTEGER4=(TRUE|FLASE)
-D EIGEN_INCLUDE_DIR=/path/to/eigen3/include
```

`BLA_STATIC` indicates static LAPACK and BLAS libraries will be perferred. `INTEGER4` indicated the Fortran integer width used by BLAS and LAPACK; if `TRUE` (the default), the integer size is `integer*4`, otherwise `integer*8` is used.

Note, when configuring TiledArray, CMake will download and build MADNESS, Eigen, and Boost if they are not found on the system. Boost will only be installed if unit testing is enabled. This behavior can be disable with `-D TA_EXPERT=TRUE`.

### Developers
TiledArray is developed by the [Valeev Group](http://research.valeyev.net) at [Virginia Tech](http://www.vt.edu).

### How to Cite

Cite TiledArray as
> "TiledArray: A general-purpose scalable block-sparse tensor framework", Justus A. Calvin and Edward F. Valeev, https://github.com/valeevgroup/tiledarray .

Inner workings of TiledArray are partially described in the following publications:
* Justus A. Calvin, Cannada A. Lewis, and Edward F. Valeev, "Scalable Task-Based Algorithm for Multiplication of Block-Rank-Sparse Matrices.", Proceedings of the 5th Workshop on Irregular Applications: Architectures and Algorithms, http://dx.doi.org/10.1145/2833179.2833186.
* Justus A. Calvin and Edward F. Valeev, "Task-Based Algorithm for Matrix Multiplication: A Step Towards Block-Sparse Tensor Computing." http://arxiv.org/abs/1504.05046 .

The MADNESS parallel runtime is described in the following publication:
* Robert J. Harrison, Gregory Beylkin, Florian A. Bischoff, Justus A. Calvin, George I. Fann, Jacob Fosso-Tande, Diego Galindo, Jeff R. Hammond, Rebecca Hartman-Baker, Judith C. Hill, Jun Jia, Jakob S. Kottmann, M-J. Yvonne Ou, Junchen Pei, Laura E. Ratcliff, Matthew G. Reuter, Adam C. Richie-Halford, Nichols A. Romero, Hideo Sekino, William A. Shelton, Bryan E. Sundahl, W. Scott Thornton, Edward F. Valeev, Álvaro Vázquez-Mayagoitia, Nicholas Vence and Yukina Yokoi, "madness: A Multiresolution, Adaptive Numerical Environment for Scientific Simulation.", http://arxiv.org/abs/1507.01888 .

### Acknowledgements
Development of TiledArray is made possible by past and present contributions from the National Science Foundation (awards CHE-0847295, CHE-0741927, OCI-1047696, CHE-1362655, ACI-1047696), the Alfred P. Sloan Foundation, and the Camille and Henry Dreyfus Foundation.
