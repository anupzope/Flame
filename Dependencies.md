# Installation of Flame Dependencies

## Guppy
Load following modules. These are the dependencies required to build and run Flame
```
module load mpich intel-oneapi-mkl
```
Additional dependencies are required for building Flame dependencies
```
module load cmake miniconda3 texlive/20210325
```

## Set the install prefix

FLAME_DEP_INSTALL_PREFIX=/var/tmp/anup/flame-deps/installed

### GKlib
```
git clone https://github.com/KarypisLab/GKlib.git
cd GKlib
make config prefix=${FLAME_DEP_INSTALL_PREFIX}/metis
make -j4
make install
```

### METIS
```
git clone https://github.com/KarypisLab/METIS.git
cd METIS
make config prefix=${FLAME_DEP_INSTALL_PREFIX}/metis shared=1
make -j4
make install
```

### ParMETIS
```
git clone https://github.com/KarypisLab/ParMETIS.git
cd ParMETIS
make config prefix=${FLAME_DEP_INSTALL_PREFIX}/metis shared=1
make -j4
make install
```
### Module file for METIS
```
family("fmetis")
  
help([[
sets environment for GKlib, METIS, and ParMETIS (4/12/2023)
]])

whatis("Name: GKlib,METIS, ParMETIS")
whatis("Version: master (4/12/2023)")
whatis("Category: mesh partitioning library")

local base = "/var/tmp/anup/flame-deps/installed/metis"

prepend_path("PATH", pathJoin(base,"bin"))
prepend_path("LD_LIBRARY_PATH", pathJoin(base,"lib"))
setenv("METIS_DIR", base)
setenv("PARMETIS_DIR", base)
```

### HDF5
Use ccmake to configure. Enable parallel HDF5 and set installation path. No other changes in the configuration are required. Then, build as usual using `make` and `make install`.

### Modulefile for HDF5
```
family("fhdf5")
  
help([[
sets environment for HDF5/1.10.9 (compiled on 4/12/2023)
]])

whatis("Name: HDF5")
whatis("Version: 1.10.9 (compiled on 4/12/2023)")
whatis("Category: File storage format")

local base = "/var/tmp/anup/flame-deps/installed/hdf5/1.10.9"

prepend_path("PATH", pathJoin(base,"bin"))
prepend_path("LD_LIBRARY_PATH", pathJoin(base,"lib"))
prepend_path("PKG_CONFIG_PATH", pathJoin(base,"lib/pkgconfig"))
setenv("HDF5_DIR", base)
```

### CGNS
Obtain CGNS using `git clone https://github.com/CGNS/CGNS.git`. Then,  use `ccmake` and set following options.
```
CGNS_BUILD_CGNSTOOLS:ON
CGNS_BUILD_SHARED:ON
CGNS_BUILD_TESTING:OFF
CGNS_ENABLE_64BIT:ON
CGNS_ENABLE_BASE_SCOPE:OFF
CGNS_ENABLE_FORTRAN:OFF
CGNS_ENABLE_HDF5:ON
CGNS_ENABLE_MEM_DEBUG:OFF
CGNS_ENABLE_PARALLEL:ON
CGNS_ENABLE_SCOPING:OFF
CGNS_ENABLE_TESTS:OFF
CGNS_USE_SHARED:ON
CMAKE_BUILD_TYPE:Release
CMAKE_INSTALL_PREFIX:${FLAME_DEP_INSTALL_PREFIX}/cgns
HDF5_DIR:${FLAME_DEP_INSTALL_PREFIX}/hdf5/1.10.9/cmake
HDF5_NEED_MPI:ON
HDF5_NEED_SZIP:OFF
HDF5_NEED_ZLIB:OFF
MPI_C_COMPILER:/apps/spack-managed/gcc-8.5.0/mpich-4.0.2-trjeszvguiers7id4hpnzdbngw5i7cfr/bin/mpicc
```
Then proceed to `make` and `make install`

### Modulefile for CGNS
```
family("fcgns")
  
help([[
sets environment for CGNS (compiled on 4/12/2023)
]])

whatis("Name: CGNS")
whatis("Version: master (compiled on 4/12/2023)")
whatis("Category: CFD General Notation System")

local base = "/var/tmp/anup/flame-deps/installed/cgns"

prepend_path("PATH", pathJoin(base,"bin"))
prepend_path("LD_LIBRARY_PATH", pathJoin(base,"lib"))
prepend_path("CMAKE_MODULE_PATH", pathJoin(base,"lib/cmake/cgns"))
setenv("CGNS_DIR", base)
```

### PETSc 3.18.5

```
wget https://ftp.mcs.anl.gov/pub/petsc/release-snapshots/petsc-3.18.5.tar.gz
tar -xf petsc-3.18.5.tar.gz
cd petsc-3.18.5

./configure \
--prefix=${FLAME_DEP_INSTALL_PREFIX}/petsc/3.18.5 \
--with-metis-include=${FLAME_DEP_INSTALL_PREFIX}/metis/include \
--with-metis-lib="-Wl,-rpath,${FLAME_DEP_INSTALL_PREFIX}/metis/lib -L${FLAME_DEP_INSTALL_PREFIX}/metis/lib -lmetis ${FLAME_DEP_INSTALL_PREFIX}/metis/lib/libGKlib.a" \
--with-parmetis-include=${FLAME_DEP_INSTALL_PREFIX}/metis/include \
--with-parmetis-lib="-Wl,-rpath,${FLAME_DEP_INSTALL_PREFIX}/metis/lib -L${FLAME_DEP_INSTALL_PREFIX}/metis/lib -lparmetis ${FLAME_DEP_INSTALL_PREFIX}/metis/lib/libGKlib.a" \
--with-hdf5-dir=${FLAME_DEP_INSTALL_PREFIX}/hdf5/1.10.9 \
--with-cgns-dir=${FLAME_DEP_INSTALL_PREFIX}/cgns \
--with-blas-lapack-dir=$MKLROOT

make -j4
make install
```

### Modulefile for PETSc
```
family("fpetsc")
  
help([[
sets environment for PETSc/3.18.5 (compiled on 4/12/2023)
]])

whatis("Name: PETSc")
whatis("Version: 3.18.5 (compiled on 4/12/2023)")
whatis("Category: Portable, Extensible Toolkit for Scientific Computation")

local base = "/var/tmp/anup/flame-deps/installed/petsc/3.18.5"

prepend_path("LD_LIBRARY_PATH", pathJoin(base,"lib"))
prepend_path("PKG_CONFIG_PATH", pathJoin(base,"lib/pkgconfig"))
setenv("PETSC_DIR", base)
setenv("PETSC_ARCH", "")
```

### Loci

```
cvs -d /cavs/simcenter/data4/loci/cvsroot checkout -r rel-4-0-patches 2dgv configure FVMMod FVMtools lpp install.txt lpp Makefile ParMetis-4.0 README sprng src tmpcopy Tutorial

./configure --prefix ${FLAME_DEP_INSTALL_PREFIX}/Loci --with-metis ${FLAME_DEP_INSTALL_PREFIX}/metis

make -j4
make install
```
### Modulefile for Loci
```
family("floci")
  
help([[
sets environment for Loci/4.0 (compiled on 4/12/2023)
]])

whatis("Name: Loci")
whatis("Version: 4.0 (compiled on 4/12/2023)")
whatis("Category: Auto-parallelization framework for scientific computations")

local base = "/var/tmp/anup/flame-deps/installed/Loci/Loci-Linux-x86_64-mpic++-rel-4-0-patches"

prepend_path("PATH", pathJoin(base,"bin"))
prepend_path("LD_LIBRARY_PATH", pathJoin(base,"lib"))
setenv("LOCI_BASE", base)
```
