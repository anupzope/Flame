#!/bin/bash

# On Warhawk1
COMPILER_MODULE="gcc/10.2.0"
MPI_MODULE="openmpi/4.0.4"
ZLIB_MODULE="zlib/1.2.11"
BDIR="build-warhawk1"
IPATH=/apps/contrib/fsi/Flame-Stack/01
MPATH=/apps/contrib/fsi/Flame-Stack/01/modulefiles
module load ${COMPILER_MODULE} ${MPI_MODULE} ${ZLIB_MODULE}
module load cmake texlive python

# On Rifle
#COMPILER_MODULE="gcc/12.2.0"
#MPI_MODULE="openmpi/4.1.4"
#ZLIB_MODULE="zlib/1.2.13"
#BDIR="build-rifle"
#IPATH=/var/tmp/anup/Flame-Stack/01
#MPATH=/var/tmp/anup/Flame-Stack/01/modulefiles
#module load ${COMPILER_MODULE} ${MPI_MODULE} ${ZLIB_MODULE}
#module load cmake texlive python

# On Scout
#COMPILER_MODULE="gcc/10.2.0"
#MPI_MODULE="openmpi/4.0.4"
#ZLIB_MODULE="zlib/1.2.11"
#BDIR="build-scout"
#IPATH=/apps/contrib/fsi/Flame-Stack/01
#MPATH=/apps/contrib/fsi/Flame-Stack/01/modulefiles
#module load ${COMPILER_MODULE} ${MPI_MODULE} ${ZLIB_MODULE}
#module load cmake texlive python

# On Guppy
#COMPILER_MODULE="gcc/12.2.0"
#MPI_MODULE="openmpi/4.1.4"
#ZLIB_MODULE="zlib/1.2.13"
#BDIR="build-guppy"
#IPATH=/local/scratch/adz8/FlameStack/01
#MPATH=/local/scratch/adz8/FlameStack/01/modulefiles
#module load ${COMPILER_MODULE} ${MPI_MODULE} ${ZLIB_MODULE}
#module load texlive python

# On Orion
#COMPILER_MODULE="gcc/11.3.0"
#MPI_MODULE="openmpi/4.0.4"
#ZLIB_MODULE="zlib/1.2.11"
#BDIR="build-orion"
#IPATH=/apps/contrib/FSI/Flame-Stack/01
#MPATH=/apps/contrib/FSI/Flame-Stack/01/modulefiles
#module load ${COMPILER_MODULE} ${MPI_MODULE} ${ZLIB_MODULE}
#module load cmake texlive python

module list

# Get MPI Configuration
MPICC=`/usr/bin/which --tty-only mpicc`
MPI_DIR="${MPICC%*/*/*}"
MPI_CFLAGS="-I${MPI_DIR}/include"
MPI_LDFLAGS="-L${MPI_DIR}/lib -lmpi"

module use "${MPATH}"

mkdir -p "${BDIR}"

function prompt_yn() {
    read -p "$1" -n 1 response
    if [[ "x${response}" == "xy" ]]; then
	echo "1"
    else
	echo "0"
    fi
}

############################################
## Install Google Test and Google Logging ##
############################################

# Obtain source from: https://github.com/google/glog.git

GLOG_VERSION="master"
INSTALL_GLOG=$(prompt_yn "Install Glog/${GLOG_VERSION}?")
GLOG_MODULENAME="FlameGlog"
GLOG_IPATH="${IPATH}/Glog/${GLOG_VERSION}"
GLOG_MPATH="${MPATH}/${GLOG_MODULENAME}"
if [[ ${INSTALL_GLOG} -eq 1 ]]; then
  cd "${BDIR}" && echo "Directory: ${PWD}"
  rm -rf glog
  cp -R ../sources/glog ./
  cd glog && echo "Directory: ${PWD}"
  cmake -S . -B build -DCMAKE_INSTALL_PREFIX="${GLOG_IPATH}"
  cmake --build build
  cmake --build build --target test
  cmake --build build --target install
  cd .. && echo "Directory: ${PWD}"
  cd .. && echo "Directory: ${PWD}"
  
  echo "Creating modulefile for Glog..."
  mkdir -p "${GLOG_MPATH}"
  cat > "${GLOG_MPATH}/${GLOG_VERSION}.lua" <<EOF
family("${GLOG_MODULENAME}")

depends_on("${COMPILER_MODULE}")

local package="Glog"
local version="${GLOG_VERSION}"
local path="${GLOG_IPATH}"

prepend_path("LD_LIBRARY_PATH", pathJoin(path, "lib64"))
prepend_path("PKG_CONFIG_PATH", pathJoin(pathJoin(path, "lib64"), "pkgconfig"))
setenv("GLOG_DIR", path)
setenv("GLOG_ROOT", path)

whatis("Package: "..package)
whatis("Version: "..version)
whatis("Path: "..path)
EOF
fi

GTEST_VERSION="1.14.0"
INSTALL_GTEST=$(prompt_yn "Install Gtest/${GTEST_VERSION}?")
GTEST_MODULENAME="FlameGtest"
GTEST_IPATH="${IPATH}/Gtest/${GTEST_VERSION}"
GTEST_MPATH="${MPATH}/${GTEST_MODULENAME}"
if [[ "${INSTALL_GTEST}" -eq 1 ]]; then
    if [ ! -f "googletest-${GTEST_VERSION}.tar.gz" ]; then
	wget -O "googletest-${GTEST_VERSION}.tar.gz" "https://github.com/google/googletest/archive/refs/tags/v${GTEST_VERSION}.tar.gz"
    fi

    if [ ! -d "googletest-${GTEST_VERSION}" ]; then
	tar -xf "googletest-${GTEST_VERSION}.tar.gz"
    fi

    cd "googletest-${GTEST_VERSION}"
    cmake -S . -B build \
	  -DCMAKE_INSTALL_PREFIX="${GTEST_IPATH}" \
	  -DBUILD_SHARED_LIBS=ON
    tmpval=$(prompt_yn "Is the configuration ok?")
    cmake --build build
    cmake --install build
    cd ..

    echo "Creating modulefile for Gtest..."
    mkdir -p "${GTEST_MPATH}"
    cat > "${GTEST_MPATH}/${GTEST_VERSION}.lua" <<EOF
family("${GTEST_MODULENAME}")

depends_on("${COMPILER_MODULE}")

local package="Gtest"
local version="${GTEST_VERSION}"
local path="${GTEST_IPATH}"

prepend_path("LD_LIBRARY_PATH", pathJoin(path, "lib64"))
prepend_path("PKG_CONFIG_PATH", pathJoin(pathJoin(path, "lib64"), "pkgconfig"))
setenv("GTEST_DIR", path)
setenv("GTEST_ROOT", path)

whatis("Package: "..package)
whatis("Version: "..version)
whatis("Path: "..path)
EOF
fi


########################################
## Install GKlib, METIS, and ParMETIS ##
########################################

# Obtain source from:
# git clone https://github.com/KarypisLab/GKlib.git.
# git clone https://github.com/KarypisLab/ParMETIS.git
# git clone https://github.com/KarypisLab/METIS.git
# 
# Edit
# the `CMakeLists.txt` file in GKlib and turn `ON` the
# `BUILD_SHARED_LIBS` options after cloning the repository.
# 
# Add Compiler flag "-D_POSIX_C_SOURCE=199309L" in GKlib,
# METIS, and ParMETIS (as follows) to avoid error related
# to siginfo_t.
# 
# add_compile_definitions(_POSIX_C_SOURCE=199309L)

METIS_VERSION="master"
INSTALL_METIS=$(prompt_yn "Install metis/${METIS_VERSION}?")
METIS_MODULENAME="FlameMETIS"
METIS_IPATH="${IPATH}/METIS/${METIS_VERSION}"
METIS_MPATH="${MPATH}/${METIS_MODULENAME}"
if [[ ${INSTALL_METIS} -eq 1 ]]; then
  cd "${BDIR}" && echo "Directory: ${PWD}"
  rm -rf GKlib
  cp -R ../sources/GKlib ./
  cd GKlib && echo "Directory: ${PWD}"
  make config prefix="${METIS_IPATH}"
  make -j4
  make install
  cd .. && echo "Directory: ${PWD}"
  cd .. && echo "Directory: ${PWD}"
  
  cd "${BDIR}" && echo "Directory: ${PWD}"
  rm -rf METIS
  cp -R ../sources/METIS ./
  cd METIS && echo "Directory: ${PWD}"
  make config prefix="${METIS_IPATH}" gklib_path="${METIS_IPATH}" shared=1
  make -j4
  make install
  cd .. && echo "Directory: ${PWD}"
  cd .. && echo "Directory: ${PWD}"
  
  cd "${BDIR}" && echo "Directory: ${PWD}"
  rm -rf ParMETIS
  cp -R ../sources/ParMETIS ./
  cd ParMETIS && echo "Directory: ${PWD}"
  make config prefix="${METIS_IPATH}" gklib_path="${METIS_IPATH}" shared=1
  make -j4
  make install
  cd .. && echo "Directory: ${PWD}"
  cd .. && echo "Directory: ${PWD}"
  
  echo "Creating modulefile for METIS..."
  mkdir -p "${METIS_MPATH}"
  cat > "${METIS_MPATH}/${METIS_VERSION}.lua" <<EOF
family("${METIS_MODULENAME}")

depends_on("${COMPILER_MODULE}")
depends_on("${MPI_MODULE}")

local package="GKlib, METIS, and ParMETIS"
local version="${METIS_VERSION}"
local path="${METIS_IPATH}"

prepend_path("PATH", pathJoin(path, "bin"))
prepend_path("LD_LIBRARY_PATH", pathJoin(path,"lib"))
setenv("METIS_DIR", path)
setenv("METIS_ROOT", path)
setenv("PARMETIS_DIR", path)
setenv("PARMETIS_ROOT", path)

whatis("Package: "..package)
whatis("Version: "..version)
whatis("Path: "..path)
EOF
fi

module load "${METIS_MODULENAME}/${METIS_VERSION}"


##################
## Install HDF5 ##
##################

HDF5_VERSION="1.14.1"
INSTALL_HDF5=$(prompt_yn "Install hdf5/${HDF5_VERSION}?")
HDF5_MODULENAME="FlameHDF5"
HDF5_IPATH="${IPATH}/HDF5/${HDF5_VERSION}"
HDF5_MPATH="${MPATH}/${HDF5_MODULENAME}"
if [[ ${INSTALL_HDF5} -eq 1 ]]; then
  cd "${BDIR}" && echo "Directory: ${PWD}"
  rm -rf hdf5-1.14.1-2
  echo "Extracting source for HDF5..."
  tar -xf ../sources/hdf5-1.14.1-2.tar.gz
  cd hdf5-1.14.1-2 && echo "Directory: ${PWD}"
  mkdir build
  cd build && echo "Directory: ${PWD}"
  echo "Configuring HDF5..."
  cmake .. -DHDF5_ENABLE_PARALLEL=ON -DCMAKE_INSTALL_PREFIX="${HDF5_IPATH}"
  #CFLAGS="${MPI_CFLAGS}" CXXFLAGS="${MPI_CFLAGS}" LDFLAGS="${MPI_LDFLAGS}" ./configure --enable-parallel --prefix="${HDF5_IPATH}"
  echo "Building HDF5..."
  make -j4
  echo "Installing HDF5..."
  make install
  cd .. && echo "Directory: ${PWD}"
  cd .. && echo "Directory: ${PWD}"
  cd .. && echo "Directory: ${PWD}"
  
  echo "Creating modulefile for HDF5..."
  mkdir -p "${HDF5_MPATH}"
  cat > "${HDF5_MPATH}/${HDF5_VERSION}.lua" <<EOF
family("${HDF5_MODULENAME}")

depends_on("${COMPILER_MODULE}")
depends_on("${MPI_MODULE}")
depends_on("${ZLIB_MODULE}")

local package="HDF5"
local version="${HDF5_VERSION}"
local path="${HDF5_IPATH}"

prepend_path("PATH", pathJoin(path, "bin"))
prepend_path("LD_LIBRARY_PATH", pathJoin(path,"lib"))
setenv("HDF5_DIR", path)
setenv("HDF5_ROOT", path)

whatis("Package: "..package)
whatis("Version: "..version)
whatis("Path: "..path)
EOF
fi

module load "${HDF5_MODULENAME}/${HDF5_VERSION}"


##################
## Install CGNS ##
##################

# Obtain source from:
# git clone -b master https://github.com/CGNS/CGNS.git

CGNS_VERSION="4.3.0"
INSTALL_CGNS=$(prompt_yn "Install cgns/${CGNS_VERSION}?")
CGNS_MODULENAME="FlameCGNS"
CGNS_IPATH="${IPATH}/CGNS/${CGNS_VERSION}"
CGNS_MPATH="${MPATH}/${CGNS_MODULENAME}"

if [[ ${INSTALL_CGNS} -eq 1 ]]; then
  cd "${BDIR}" && echo "Directory: ${PWD}"
  rm -rf CGNS
  echo "Copying CGNS source..."
  cp -R ../sources/CGNS ./
  cd CGNS && echo "Directory: ${PWD}"
  mkdir build
  cd build && echo "Directory: ${PWD}"
  echo "Configuring CGNS..."
  cmake -DCGNS_BUILD_SHARED=ON -DCMAKE_INSTALL_PREFIX="${CGNS_IPATH}" \
    -DHDF5_NEED_MPI=ON -DHDF5_NEED_ZLIB=ON  -DCGNS_ENABLE_PARALLEL=ON ..
  echo "Building CGNS..."
  make -j4
  echo "Installing CGNS..."
  make install
  cd .. && echo "Directory: ${PWD}"
  cd .. && echo "Directory: ${PWD}"
  cd .. && echo "Directory: ${PWD}"
  
  echo "Creating modulefile for CGNS..."
  mkdir -p "${CGNS_MPATH}"
  cat > "${CGNS_MPATH}/${CGNS_VERSION}.lua" <<EOF
family("${CGNS_MODULENAME}")

depends_on("${COMPILER_MODULE}")
depends_on("${MPI_MODULE}")
depends_on("${ZLIB_MODULE}")
depends_on("${HDF5_MODULENAME}/${HDF5_VERSION}")

local package="CGNS"
local version="${CGNS_VERSION}"
local path="${CGNS_IPATH}"

prepend_path("PATH", pathJoin(path, "bin"))
prepend_path("LD_LIBRARY_PATH", pathJoin(path,"lib"))
setenv("CGNS_DIR", path)
setenv("CGNS_ROOT", path)

whatis("Package: "..package)
whatis("Version: "..version)
whatis("Path: "..path)
EOF
fi

module load "${CGNS_MODULENAME}/${CGNS_VERSION}"


######################
## Install OpenBLAS ##
######################

OpenBLAS_VERSION="0.3.23"
INSTALL_OpenBLAS=$(prompt_yn "Install OpenBLAS/${OpenBLAS_VERSION}?")
OpenBLAS_MODULENAME="FlameOpenBLAS"
OpenBLAS_IPATH="${IPATH}/OpenBLAS/${OpenBLAS_VERSION}"
OpenBLAS_MPATH="${MPATH}/${OpenBLAS_MODULENAME}"

if [[ ${INSTALL_OpenBLAS} -eq 1 ]]; then
  cd "${BDIR}" && echo "Directory: ${PWD}"
  rm -rf OpenBLAS-0.3.23
  echo "Extracting OpenBLAS..."
  tar -xf ../sources/OpenBLAS-0.3.23.tar.gz
  cd OpenBLAS-0.3.23 && echo "Directory: ${PWD}"
  echo "Building OpenBLAS..."
  make -j4
  echo "Installing OpenBLAS..."
  make install PREFIX="${OpenBLAS_IPATH}"
  cd .. && echo "Directory: ${PWD}"
  cd .. && echo "Directory: ${PWD}"
  
  echo "Creating modulefile for OpenBLAS..."
  mkdir -p "${OpenBLAS_MPATH}"
  cat > "${OpenBLAS_MPATH}/${OpenBLAS_VERSION}.lua" <<EOF
family("${OpenBLAS_MODULENAME}")

depends_on("${COMPILER_MODULE}")

local package="OpenBLAS"
local version="${OpenBLAS_VERSION}"
local path="${OpenBLAS_IPATH}"

prepend_path("PATH", pathJoin(path, "bin"))
prepend_path("LD_LIBRARY_PATH", pathJoin(path,"lib"))
prepend_path("PKG_CONFIG_PATH", pathJoin(pathJoin(path, "lib"), "pkgconfig"))
setenv("OpenBLAS_DIR", path)
setenv("OpenBLAS_ROOT", path)

whatis("Package: "..package)
whatis("Version: "..version)
whatis("Path: "..path)
EOF
fi

module load "${OpenBLAS_MODULENAME}/${OpenBLAS_VERSION}"


###################
## Install PETSc ##
###################

PETSc_VERSION="3.19.2"
INSTALL_PETSc=$(prompt_yn "Install petsc/${PETSc_VERSION}?")
PETSc_MODULENAME="FlamePETSc"
PETSc_IPATH="${IPATH}/PETSc/${PETSc_VERSION}"
PETSc_MPATH="${MPATH}/${PETSc_MODULENAME}"

if [[ ${INSTALL_PETSc} -eq 1 ]]; then
  cd "${BDIR}" && echo "Directory: ${PWD}"
  rm -rf petsc-3.19.2
  echo "Extracting source for PETSc..."
  tar -xf ../sources/petsc-3.19.2.tar.gz
  cd petsc-3.19.2 && echo "Directory: ${PWD}"
  echo "Configuring PETSc..."
  ./configure CC=mpicc CXX=mpicxx FC=mpifort \
    --prefix="${PETSc_IPATH}" \
    --with-hdf5-dir="${HDF5_IPATH}" \
    --with-cgns-dir="${CGNS_IPATH}" \
    --with-openblas-dir="${OpenBLAS_IPATH}" \
    --with-metis-include="${METIS_IPATH}/include" \
    --with-metis-lib="-Wl,-rpath,${METIS_IPATH}/lib -L${METIS_IPATH}/lib -lmetis -lGKlib" \
    --with-parmetis-include="${METIS_IPATH}/include" \
    --with-parmetis-lib="-Wl,-rpath,${METIS_IPATH}/lib -L${METIS_IPATH}/lib -lparmetis -lGKlib"
  echo "Building PETSc..."
  make -j4
  echo "Installing PETSc..."
  make install
  cd .. && echo "Directory: ${PWD}"
  cd .. && echo "Directory: ${PWD}"
  
  echo "Creating modulefile for PETSc..."
  mkdir -p "${PETSc_MPATH}"
  cat > "${PETSc_MPATH}/${PETSc_VERSION}.lua" <<EOF
family("${PETSc_MODULENAME}")

depends_on("${COMPILER_MODULE}")
depends_on("${MPI_MODULE}")
depends_on("${ZLIB_MODULE}")
depends_on("${METIS_MODULENAME}/${METIS_VERSION}")
depends_on("${HDF5_MODULENAME}/${HDF5_VERSION}")
depends_on("${CGNS_MODULENAME}/${CGNS_VERSION}")
depends_on("${OpenBLAS_MODULENAME}/${OpenBLAS_VERSION}")

local package="PETSc"
local version="${PETSc_VERSION}"
local path="${PETSc_IPATH}"

prepend_path("PATH", pathJoin(path, "bin"))
prepend_path("LD_LIBRARY_PATH", pathJoin(path,"lib"))
prepend_path("PKG_CONFIG_PATH", pathJoin(pathJoin(path, "lib"), "pkgconfig"))
setenv("PETSC_DIR", path)
setenv("PETSC_ROOT", path)
setenv("PETSC_ARCH", "")

whatis("Package: "..package)
whatis("Version: "..version)
whatis("Path: "..path)
EOF
fi

module load "${PETSc_MODULENAME}/${PETSc_VERSION}"


##################
## Install Loci ##
##################

# Obtain source from 
# 
# cvs -d /cavs/simcenter/data4/loci/cvsroot checkout -r rel-4-0-patches 2dgv \
# configure FVMMod FVMtools lpp install.txt lpp Makefile ParMetis-4.0 README \
# sprng src tmpcopy Tutorial

LOCI_VERSION="4.0"
INSTALL_LOCI=$(prompt_yn "Install Loci/${LOCI_VERSION}?")
LOCI_MODULENAME="FlameLoci"
LOCI_IPATH="${IPATH}/Loci/${LOCI_VERSION}"
LOCI_MPATH="${MPATH}/${LOCI_MODULENAME}"

if [[ ${INSTALL_LOCI} -eq 1 ]]; then
  cd "${BDIR}" && echo "Directory: ${PWD}"
  rm -rf Loci
  echo "Copying source for Loci..."
  cp -R ../sources/Loci ./
  cd Loci && echo "Directory: ${PWD}"
  echo "Configuring Loci..."
  ./configure --prefix "${LOCI_IPATH}" --with-metis "${METIS_IPATH}" --with-mpi "${MPI_DIR}"
  echo "Building Loci..."
  cd OBJ && echo "Directory: ${PWD}"
  make -j4
  echo "Installing Loci..."
  make install
  cd .. && echo "Directory: ${PWD}"
  cd .. && echo "Directory: ${PWD}"
  cd .. && echo "Directory: ${PWD}"
  
  echo "Creating modulefile for Loci..."
  mkdir -p "${LOCI_MPATH}"
  cat > "${LOCI_MPATH}/${LOCI_VERSION}.lua" <<EOF
family("${LOCI_MODULENAME}")

depends_on("${COMPILER_MODULE}")
depends_on("${MPI_MODULE}")
depends_on("${ZLIB_MODULE}")
depends_on("${GLOG_MODULENAME}/${GLOG_VERSION}")
depends_on("${GTEST_MODULENAME}/${GTEST_VERSION}")
depends_on("${METIS_MODULENAME}/${METIS_VERSION}")
depends_on("${HDF5_MODULENAME}/${HDF5_VERSION}")
depends_on("${CGNS_MODULENAME}/${CGNS_VERSION}")
depends_on("${PETSc_MODULENAME}/${PETSc_VERSION}")

local package="Loci"
local version="${LOCI_VERSION}"
local path="${LOCI_IPATH}/Loci-Linux-x86_64-mpic++-rel-4-0-patches"

prepend_path("PATH", pathJoin(path, "bin"))
prepend_path("LD_LIBRARY_PATH", pathJoin(path,"lib"))
setenv("LOCI_BASE", path)

whatis("Package: "..package)
whatis("Version: "..version)
whatis("Path: "..path)
EOF
fi

module load "${LOCI_MODULENAME}/${LOCI_VERSION}"

