# Installation of Flame Dependencies

## Rifle and Guppy
### Required System Modules
Load following system modules on Rifle.
```
module load zlib/1.2.13 gcc/12.2.0 mpich/4.0.2
```
Local following system modules on Guppy.
```
module load zlib/1.2.12 gcc/12.1.0 mpich/4.0.2
```
Additional dependencies required for building Flame dependencies:
```
module load cmake texlive
```

### Set Install Prefixes
```
FLAMEDEP_PREFIX=/var/tmp/anup/flame-deps/installed
FLAMEDEP_MPREFIX=/var/tmp/anup/flame-deps/modulefiles
```

### Install GKlib, METIS, and ParMETIS
#### Install GKlib
Edit the `CMakeLists.txt` file and turn `ON` the `BUILD_SHARED_LIBS` options after cloning the repository.
Add Compiler flag "-D_POSIX_C_SOURCE=199309L" to avoid error related to siginfo_t
```
git clone https://github.com/KarypisLab/GKlib.git
cd GKlib
make config prefix=${FLAMEDEP_PREFIX}/metis/master
make -j4
make install
```
### Install METIS
```
git clone https://github.com/KarypisLab/METIS.git
cd METIS
make config prefix=${FLAMEDEP_PREFIX}/metis/master shared=1
make -j4
make install
```

### Install ParMETIS
```
git clone https://github.com/KarypisLab/ParMETIS.git
cd ParMETIS
make config prefix=${FLAMEDEP_PREFIX}/metis/master shared=1
make -j4
make install
```

#### Modulefile for GKlib, METIS and ParMATIS
```
mkdir -p ${FLAMEDEP_MPREFIX}/flamemetis
cat > ${FLAMEDEP_MPREFIX}/flamemetis/master.lua << EOF
family("flamemetis")
  
help([[
sets environment for GKlib, METIS, and ParMETIS
]])

local base="${FLAMEDEP_PREFIX}"
local package="metis"
local version="master"
local prefix=pathJoin(pathJoin(base, package), version)

prepend_path("PATH", pathJoin(prefix,"bin"))
prepend_path("LD_LIBRARY_PATH", pathJoin(prefix,"lib"))
setenv("METIS_DIR", prefix)
setenv("PARMETIS_DIR", prefix)

whatis("Name: GKlib, METIS, ParMETIS")
whatis("Version: "..version)
whatis("Category: mesh partitioning library")
whatis("Path: "..prefix)
EOF
```

### Install HDF5/1.14.1
```
tar -xf ../sources/hdf5-1.14.1-2.tar.gz
cd hdf5-1.14.1-2
CFLAGS="-I${MPICH_ROOT}/include" CXXFLAGS="-I${MPICH_ROOT}/include" LDFLAGS="-L${MPICH_ROOT}/lib -lmpi" ./configure --enable-parallel --prefix=${FLAMEDEP_PREFIX}/hdf5/1.14.1
make -j4
make install
```

#### Modulefile for HDF5/1.14.1
```
mkdir -p ${FLAMEDEP_MPREFIX}/flamehdf5
cat > ${FLAMEDEP_MPREFIX}/flamehdf5/1.14.1.lua << EOF
family("flamehdf5")

help([[
sets environment for HDF5
]])

local base="${FLAMEDEP_PREFIX}"
local package="hdf5"
local version="1.14.1"
local prefix=pathJoin(pathJoin(base, package), version)

prepend_path("PATH", pathJoin(prefix, "bin"))
prepend_path("LD_LIBRARY_PATH", pathJoin(prefix, "lib"))
setenv("HDF5_ROOT", prefix)
setenv("HDF5_DIR", prefix)

whatis("Name: "..package)
whatis("Version: "..version)
whatis("Category: File format")
whatis("Path: "..prefix)
EOF
```

### Install CGNS/4.3.0
```
git clone https://github.com/CGNS/CGNS.git
cd CGNS
git checkout v4.3.0
mkdir build
cd build
cmake -DCGNS_BUILD_SHARED=ON -DCMAKE_INSTALL_PREFIX=${FLAMEDEP_PREFIX}/cgns/4.3.0 \
-DHDF5_DIR=${HDF5_DIR} -DHDF5_NEED_MPI=ON -DHDF5_NEED_ZLIB=ON  -DCGNS_ENABLE_PARALLEL=ON ..
make -j4
make install
```

#### Modulefile for CGNS/4.3.0
```
mkdir -p ${FLAMEDEP_MPREFIX}/flamecgns
cat > ${FLAMEDEP_MPREFIX}/flamecgns/4.3.0.lua << EOF
family("flamecgns")

help([[
sets environment for CGNS
]])

local base="${FLAMEDEP_PREFIX}"
local package="cgns"
local version="4.3.0"
local prefix=pathJoin(pathJoin(base, package), version)

prepend_path("PATH", pathJoin(prefix, "bin"))
prepend_path("LD_LIBRARY_PATH", pathJoin(prefix, "lib"))
setenv("CGNS_ROOT", prefix)
setenv("CGNS_DIR", prefix)

whatis("Name: "..package)
whatis("Version: "..version)
whatis("Category: Mesh format")
whatis("Path: "..prefix)
EOF
```

### Install OpenBLAS/0.3.23
```
tar -xf ../sources/OpenBLAS-0.3.23.tar.gz
cd OpenBLAS-0.3.23
make -j4
make install PREFIX=${FLAMEDEP_PREFIX}/openblas/0.3.23
```

#### Modulefile for OpenBLAS/0.3.23
```
mkdir -p ${FLAMEDEP_MPREFIX}/flameopenblas
cat > ${FLAMEDEP_MPREFIX}/flameopenblas/0.3.23.lua << EOF
family("flameopenblas")

help([[
sets environment for OpenBLAS
]])

local base="${FLAMEDEP_PREFIX}"
local package="openblas"
local version="0.3.23"
local prefix=pathJoin(pathJoin(base, package), version)

prepend_path("PATH", pathJoin(prefix, "bin"))
prepend_path("LD_LIBRARY_PATH", pathJoin(prefix, "lib"))
prepend_path("PKG_CONFIG_PATH", pathJoin(base,"lib/pkgconfig"))
setenv("OPENBLAS_ROOT", prefix)
setenv("OPENBLAS_DIR", prefix)

whatis("Name: "..package)
whatis("Version: "..version)
whatis("Category: BLAS/LAPACK library")
whatis("Path: "..prefix)
EOF
```

### Install PETSc/3.19.2
```
tar -xf ../sources/petsc-3.19.2.tar.gz
cd petsc-3.19.2

./configure CC=mpicc CXX=mpicxx FC=mpifort \
--prefix=${FLAMEDEP_PREFIX}/petsc/3.19.2 \
--with-hdf5-dir=${HDF5_DIR} \
--with-cgns-dir=${CGNS_DIR} \
--with-openblas-dir=${OPENBLAS_DIR} \
--with-metis-include=${FLAMEDEP_PREFIX}/metis/master/include \
--with-metis-lib="-Wl,-rpath,${FLAMEDEP_PREFIX}/metis/master/lib -L${FLAMEDEP_PREFIX}/metis/master/lib -lmetis -lGKlib" \
--with-parmetis-include=${FLAMEDEP_PREFIX}/metis/master/include \
--with-parmetis-lib="-Wl,-rpath,${FLAMEDEP_PREFIX}/metis/master/lib -L${FLAMEDEP_PREFIX}/metis/master/lib -lparmetis -lGKlib"

make -j4
make install
```

#### Modulefile for PETSc/3.19.2
```
mkdir -p ${FLAMEDEP_MPREFIX}/flamepetsc
cat > ${FLAMEDEP_MPREFIX}/flamepetsc/3.19.2.lua << EOF
family("flamepetsc")

help([[
sets environment for PETSc
]])

local base="${FLAMEDEP_PREFIX}"
local package="petsc"
local version="3.19.2"
local prefix=pathJoin(pathJoin(base, package), version)

prepend_path("PATH", pathJoin(prefix, "bin"))
prepend_path("LD_LIBRARY_PATH", pathJoin(prefix, "lib"))
prepend_path("PKG_CONFIG_PATH", pathJoin(base,"lib/pkgconfig"))
setenv("PETSC_ROOT", prefix)
setenv("PETSC_DIR", prefix)
setenv("PETSC_ARCH", "")

whatis("Name: "..package)
whatis("Version: "..version)
whatis("Category: Sparse library of linear and non-linear solvers, and more...")
whatis("Path: "..prefix)
EOF
```

### Install Loci/4.0
```
cp -R ../sources/Loci ./
cd Loci
cvs -d /cavs/simcenter/data4/loci/cvsroot checkout -r rel-4-0-patches 2dgv configure FVMMod FVMtools lpp install.txt lpp Makefile ParMetis-4.0 README sprng src tmpcopy Tutorial

./configure --prefix ${FLAMEDEP_PREFIX}/Loci/4.0 --with-metis ${FLAMEDEP_PREFIX}/metis/master

make -j4
make install
```

#### Modulefile for Loci/4.0
```
mkdir -p ${FLAMEDEP_MPREFIX}/flameloci
cat > ${FLAMEDEP_MPREFIX}/flameloci/4.0.lua << EOF
family("flameloci")

help([[
sets environment for Loci
]])

local base="${FLAMEDEP_PREFIX}"
local package="Loci"
local version="4.0"
local arch="Loci-Linux-x86_64-mpic++-rel-4-0-patches"
local prefix=pathJoin(pathJoin(pathJoin(base, package), version), arch)

prepend_path("PATH", pathJoin(prefix,"bin"))
prepend_path("LD_LIBRARY_PATH", pathJoin(prefix,"lib"))
setenv("LOCI_BASE", prefix)

whatis("Name: "..package)
whatis("Version: "..version)
whatis("Category: Auto-parallelization framework for scientific computations")
whatis("path: "..prefix)
EOF
```

### Prepare to Build Flame
After all the installation of dependencies is complete use following command to prepare the dependencies for use.
#### Rifle
```
module load zlib/1.2.13 gcc/12.2.0 mpich/4.0.2
module load flamemetis flamehdf5 flamecgns flameopenblas flamepetsc flameloci
```
#### Guppy
```
module load zlib/1.2.13 gcc/12.1.0 mpich/4.0.2
module load flamemetis flamehdf5 flamecgns flameopenblas flamepetsc flameloci
```