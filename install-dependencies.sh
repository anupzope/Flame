#!/bin/bash

# On Warhawk1
#COMPILER_MODULE="gcc/10.2.0"
#MPI_MODULE="openmpi/4.0.4"
#ZLIB_MODULE="zlib/1.2.11"
#BDIR="build-warhawk1"
#IPATH=/apps/contrib/fsi/Flame-Stack/01
#MPATH=/apps/contrib/fsi/Flame-Stack/01/modulefiles
#module load ${COMPILER_MODULE} ${MPI_MODULE} ${ZLIB_MODULE}
#module load cmake texlive python

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

# On Hercules
BUILD_NTHREADS=10
COMPILER_MODULE="intel-oneapi-compilers/2023.1.0"
MPI_MODULE="intel-oneapi-mpi/2021.9.0"
MKL_MODULE="intel-oneapi-mkl/2023.1.0"
HDF5_MODULE="hdf5/1.14.1-2"
IPATH="/apps/contrib/FSI/Flame-Stack/1.0"
MPATH="/apps/contrib/FSI/Flame-Stack/1.0/modulefiles"
module load "${COMPILER_MODULE}"
module load "${MPI_MODULE}"
module load "${MKL_MODULE}"
module load "${HDF5_MODULE}"

module load cmake

echo "Modules:"
module list

export CC=icc
export CXX=icpc
export F90=ifort
export F77=ifort
export FC=ifort

export I_MPI_CC="icc"
export I_MPI_CXX="icpc"
export I_MPI_F90="ifort"

export MKL_INCLUDE_DIR="${MKLROOT}/include"
export MKL_LIBRARY_DIR="${MKLROOT}/lib"
export MKL_LIBRARY_NAMES=("mkl_scalapack_lp64" "mkl_intel_lp64" "mkl_sequential" "mkl_core" "mkl_blacs_intelmpi_lp64" "pthread" "m" "dl")
export MKL_LDFLAGS="-L${MKL_LIBRARY_DIR} -Wl,--no-as-needed ${MKL_LIBRARY_NAMES[@]/#/-l}"
export MKL_CFLAGS="-I${MKL_INCLUDE_DIR}"

module use "${MPATH}"

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

    cd "googletest-${GTEST_VERSION}" && echo "Directory: ${PWD}"
    cmake -S . -B build \
	  -DCMAKE_INSTALL_PREFIX="${GTEST_IPATH}" \
	  -DBUILD_SHARED_LIBS=ON
    tmpval=$(prompt_yn "Is the configuration ok?")
    cmake --build build --parallel "${BUILD_NTHREADS}"
    cmake --install build
    cd .. && echo "Directory: ${PWD}"

    echo "Creating modulefile for Gtest..."
    mkdir -p "${GTEST_MPATH}"
    cat > "${GTEST_MPATH}/${GTEST_VERSION}.lua" <<EOF
family("${GTEST_MODULENAME}")

local package="Gtest"
local version="${GTEST_VERSION}"
local path="${GTEST_IPATH}"

prepend_path("LD_LIBRARY_PATH", pathJoin(path, "lib64"))
prepend_path("PKG_CONFIG_PATH", pathJoin(pathJoin(path, "lib64"), "pkgconfig"))
setenv("GTEST_DIR", path)
setenv("GTest_DIR", path)
setenv("GTEST_ROOT", path)

whatis("Package: "..package)
whatis("Version: "..version)
whatis("Path: "..path)
EOF
fi

module load "${GTEST_MODULENAME}/${GTEST_VERSION}"


GLOG_VERSION="0.7.0"
INSTALL_GLOG=$(prompt_yn "Install Glog/${GLOG_VERSION}?")
GLOG_MODULENAME="FlameGlog"
GLOG_IPATH="${IPATH}/Glog/${GLOG_VERSION}"
GLOG_MPATH="${MPATH}/${GLOG_MODULENAME}"
if [[ ${INSTALL_GLOG} -eq 1 ]]; then
    if [ ! -f "glog-${GLOG_VERSION}.tar.gz" ]; then
	wget -O "glog-${GLOG_VERSION}.tar.gz" "https://github.com/google/glog/archive/refs/tags/v${GLOG_VERSION}.tar.gz"
    fi

    if [ ! -d "glog-${GLOG_VERSION}" ]; then
	tar -xf "glog-${GLOG_VERSION}.tar.gz"
    fi

    cd "glog-${GLOG_VERSION}" && echo "Directory: ${PWD}"
    cmake -S . -B build -DCMAKE_INSTALL_PREFIX="${GLOG_IPATH}"
    tempval=$(prompt_yn "Is the configuration ok?")
    cmake --build build --parallel "${BUILD_NTHREADS}"
    cmake --build build --target test
    cmake --build build --target install
    cd .. && echo "Directory: ${PWD}"

    echo "Creating modulefile for Glog..."
    mkdir -p "${GLOG_MPATH}"
    cat > "${GLOG_MPATH}/${GLOG_VERSION}.lua" <<EOF
family("${GLOG_MODULENAME}")

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

module load "${GLOG_MODULENAME}/${GLOG_VERSION}"

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

METIS_VERSION="5.1.0"
INSTALL_METIS=$(prompt_yn "Install METIS/${METIS_VERSION}?")
METIS_MODULENAME="FlameMETIS"
METIS_IPATH="${IPATH}/METIS/${METIS_VERSION}"
METIS_MPATH="${MPATH}/${METIS_MODULENAME}"
if [[ "${INSTALL_METIS}" -eq 1 ]]; then
    if [ ! -f "metis-5.1.0.tar.gz" ]; then
	wget -O "metis-5.1.0.tar.gz" "http://glaros.dtc.umn.edu/gkhome/fetch/sw/metis/metis-5.1.0.tar.gz"
    fi

    if [ ! -d "metis-5.1.0" ]; then
	tar -xf "metis-5.1.0.tar.gz"
    fi

    cd "metis-5.1.0" && echo "Directory: ${PWD}"
    make config prefix="${METIS_IPATH}" shared=1
    tempval=$(prompt_yn "Is the configuration ok?")
    make install
    cd .. && echo "Directory: ${PWD}"

    echo "Creating modulefile for METIS..."
    mkdir -p "${METIS_MPATH}"
    cat > "${METIS_MPATH}/${METIS_VERSION}.lua" <<EOF
family("${METIS_MODULENAME}")

local package="METIS"
local version="${METIS_VERSION}"
local path="${METIS_IPATH}"

prepend_path("PATH", pathJoin(path, "bin"))
prepend_path("LD_LIBRARY_PATH", pathJoin(path,"lib"))
setenv("METIS_DIR", path)
setenv("METIS_ROOT", path)

whatis("Package: "..package)
whatis("Version: "..version)
whatis("Path: "..path)
EOF
fi

module load "${METIS_MODULENAME}/${METIS_VERSION}"


PARMETIS_VERSION="4.0.3"
INSTALL_PARMETIS=$(prompt_yn "Install ParMETIS/${PARMETIS_VERSION}?")
PARMETIS_MODULENAME="FlamePARMETIS"
PARMETIS_IPATH="${METIS_IPATH}"
PARMETIS_MPATH="${MPATH}/${PARMETIS_MODULENAME}"
if [[ "${INSTALL_PARMETIS}" -eq 1 ]]; then
    if [ ! -f "parmetis-${PARMETIS_VERSION}.tar.gz" ]; then
	wget -O "parmetis-${PARMETIS_VERSION}.tar.gz" "http://glaros.dtc.umn.edu/gkhome/fetch/sw/parmetis/parmetis-${PARMETIS_VERSION}.tar.gz"
    fi

    if [ ! -d "parmetis-${PARMETIS_VERSION}" ]; then
	tar -xf "parmetis-${PARMETIS_VERSION}.tar.gz"
    fi

    cd "parmetis-${PARMETIS_VERSION}" && echo "Directory: ${PWD}"
    make config prefix="${METIS_IPATH}" shared=1
    tempval=$(prompt_yn "Is the configuration ok?")
    make install
    cd .. && echo "Directory: ${PWD}"

    echo "Creating modulefile for ParMETIS..."
    mkdir -p "${PARMETIS_MPATH}"
    cat > "${PARMETIS_MPATH}/${PARMETIS_VERSION}.lua" <<EOF
family("${PARMETIS_MODULENAME}")

local package="ParMETIS"
local version="${PARMETIS_VERSION}"
local path="${PARMETIS_IPATH}"

prepend_path("PATH", pathJoin(path, "bin"))
prepend_path("LD_LIBRARY_PATH", pathJoin(path,"lib"))
setenv("PARMETIS_DIR", path)
setenv("PARMETIS_ROOT", path)

whatis("Package: "..package)
whatis("Version: "..version)
whatis("Path: "..path)
EOF
fi

module load "${PARMETIS_MODULENAME}/${PARMETIS_VERSION}"


##################
## Install HDF5 ##
##################

#HDF5_VERSION="1.14.1"
#INSTALL_HDF5=$(prompt_yn "Install hdf5/${HDF5_VERSION}?")
#HDF5_MODULENAME="FlameHDF5"
#HDF5_IPATH="${IPATH}/HDF5/${HDF5_VERSION}"
#HDF5_MPATH="${MPATH}/${HDF5_MODULENAME}"
#if [[ ${INSTALL_HDF5} -eq 1 ]]; then
#  cd "${BDIR}" && echo "Directory: ${PWD}"
#  rm -rf hdf5-1.14.1-2
#  echo "Extracting source for HDF5..."
#  tar -xf ../sources/hdf5-1.14.1-2.tar.gz
#  cd hdf5-1.14.1-2 && echo "Directory: ${PWD}"
#  mkdir build
#  cd build && echo "Directory: ${PWD}"
#  echo "Configuring HDF5..."
#  cmake .. -DHDF5_ENABLE_PARALLEL=ON -DCMAKE_INSTALL_PREFIX="${HDF5_IPATH}"
#  #CFLAGS="${MPI_CFLAGS}" CXXFLAGS="${MPI_CFLAGS}" LDFLAGS="${MPI_LDFLAGS}" ./configure --enable-parallel --prefix="${HDF5_IPATH}"
#  echo "Building HDF5..."
#  make -j4
#  echo "Installing HDF5..."
#  make install
#  cd .. && echo "Directory: ${PWD}"
#  cd .. && echo "Directory: ${PWD}"
#  cd .. && echo "Directory: ${PWD}"
#  
#  echo "Creating modulefile for HDF5..."
#  mkdir -p "${HDF5_MPATH}"
#  cat > "${HDF5_MPATH}/${HDF5_VERSION}.lua" <<EOF
#family("${HDF5_MODULENAME}")
#
#depends_on("${COMPILER_MODULE}")
#depends_on("${MPI_MODULE}")
#depends_on("${ZLIB_MODULE}")
#
#local package="HDF5"
#local version="${HDF5_VERSION}"
#local path="${HDF5_IPATH}"
#
#prepend_path("PATH", pathJoin(path, "bin"))
#prepend_path("LD_LIBRARY_PATH", pathJoin(path,"lib"))
#setenv("HDF5_DIR", path)
#setenv("HDF5_ROOT", path)
#
#whatis("Package: "..package)
#whatis("Version: "..version)
#whatis("Path: "..path)
#EOF
#fi
#
#module load "${HDF5_MODULENAME}/${HDF5_VERSION}"


##################
## Install CGNS ##
##################

# Obtain source from:
# git clone -b master https://github.com/CGNS/CGNS.git

CGNS_VERSION="4.4.0"
INSTALL_CGNS=$(prompt_yn "Install cgns/${CGNS_VERSION}?")
CGNS_MODULENAME="FlameCGNS"
CGNS_IPATH="${IPATH}/CGNS/${CGNS_VERSION}"
CGNS_MPATH="${MPATH}/${CGNS_MODULENAME}"
if [[ ${INSTALL_CGNS} -eq 1 ]]; then
    if [ ! -f "" ]; then
	wget -O "CGNS-${CGNS_VERSION}.tar.gz" "https://github.com/CGNS/CGNS/archive/refs/tags/v${CGNS_VERSION}.tar.gz"
    fi

    if [ ! -d "CGNS-${CGNS_VERSION}" ]; then
	tar -xf "CGNS-${CGNS_VERSION}.tar.gz"
    fi

    cd "CGNS-${CGNS_VERSION}" && echo "Directory: ${PWD}"
    echo "Configuring CGNS..."
    cmake -S . -B build \
          -DCGNS_BUILD_SHARED=ON \
          -DCMAKE_INSTALL_PREFIX="${CGNS_IPATH}" \
	  -DCMAKE_BUILD_TYPE=Release \
	  -DHDF5_NEED_MPI=ON \
	  -DHDF5_NEED_ZLIB=ON \
	  -DHDF5_NEED_ZSIP=ON \
	  -DHDF5_PREFER_PARALLEL=ON \
	  -DCGNS_ENABLE_PARALLEL=ON
    tempval=$(prompt_yn "Is the configuration ok?")
    cmake --build build --parallel "${BUILD_NTHREADS}"
    cmake --install build
    cd .. && echo "Directory: ${PWD}"

    echo "Creating modulefile for CGNS..."
    mkdir -p "${CGNS_MPATH}"
    cat > "${CGNS_MPATH}/${CGNS_VERSION}.lua" <<EOF
family("${CGNS_MODULENAME}")

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

#OpenBLAS_VERSION="0.3.23"
#INSTALL_OpenBLAS=$(prompt_yn "Install OpenBLAS/${OpenBLAS_VERSION}?")
#OpenBLAS_MODULENAME="FlameOpenBLAS"
#OpenBLAS_IPATH="${IPATH}/OpenBLAS/${OpenBLAS_VERSION}"
#OpenBLAS_MPATH="${MPATH}/${OpenBLAS_MODULENAME}"
#
#if [[ ${INSTALL_OpenBLAS} -eq 1 ]]; then
#  cd "${BDIR}" && echo "Directory: ${PWD}"
#  rm -rf OpenBLAS-0.3.23
#  echo "Extracting OpenBLAS..."
#  tar -xf ../sources/OpenBLAS-0.3.23.tar.gz
#  cd OpenBLAS-0.3.23 && echo "Directory: ${PWD}"
#  echo "Building OpenBLAS..."
#  make -j4
#  echo "Installing OpenBLAS..."
#  make install PREFIX="${OpenBLAS_IPATH}"
#  cd .. && echo "Directory: ${PWD}"
#  cd .. && echo "Directory: ${PWD}"
#  
#  echo "Creating modulefile for OpenBLAS..."
#  mkdir -p "${OpenBLAS_MPATH}"
#  cat > "${OpenBLAS_MPATH}/${OpenBLAS_VERSION}.lua" <<EOF
#family("${OpenBLAS_MODULENAME}")
#
#depends_on("${COMPILER_MODULE}")
#
#local package="OpenBLAS"
#local version="${OpenBLAS_VERSION}"
#local path="${OpenBLAS_IPATH}"
#
#prepend_path("PATH", pathJoin(path, "bin"))
#prepend_path("LD_LIBRARY_PATH", pathJoin(path,"lib"))
#prepend_path("PKG_CONFIG_PATH", pathJoin(pathJoin(path, "lib"), "pkgconfig"))
#setenv("OpenBLAS_DIR", path)
#setenv("OpenBLAS_ROOT", path)
#
#whatis("Package: "..package)
#whatis("Version: "..version)
#whatis("Path: "..path)
#EOF
#fi
#
#module load "${OpenBLAS_MODULENAME}/${OpenBLAS_VERSION}"


###################
## Install PETSc ##
###################

PETSC_VERSION="3.21.3"
INSTALL_PETSC=$(prompt_yn "Install PETSc/${PETSC_VERSION}?")
PETSC_MODULENAME="FlamePETSC"
PETSC_IPATH="${IPATH}/PETSC/${PETSC_VERSION}"
PETSC_MPATH="${MPATH}/${PETSC_MODULENAME}"
if [[ ${INSTALL_PETSC} -eq 1 ]]; then
    if [[ ! -f "petsc-${PETSC_VERSION}.tar.gz" ]]; then
	wget -O "petsc-${PETSC_VERSION}.tar.gz" "https://web.cels.anl.gov/projects/petsc/download/release-snapshots/petsc-${PETSC_VERSION}.tar.gz"
    fi

    if [ ! -d "petsc-${PETSC_VERSION}" ]; then
	tar -xf "petsc-${PETSC_VERSION}.tar.gz"
    fi

    cd "petsc-${PETSC_VERSION}" && echo "Directory: ${PWD}"
    echo "Configuring PETSc..."
    ./configure \
        --prefix="${PETSC_IPATH}" \
        --with-metis=1 \
        --with-metis-dir="${METIS_DIR}" \
        --with-parmetis=1 \
	--with-parmetis-dir="${PARMETIS_DIR}" \
        --with-blaslapack-dir="${MKLROOT}" \
        --with-scalapack-lib="${MKL_LDFLAGS}" \
        --with-scalapack-include="${MKL_INCLUDE_DIR}" \
        --with-mkl_cpardiso=1 \
        --with-mkl_pardiso=1 \
        --with-mkl_sparse=1 \
        --with-mkl_sparse_optimize=1 \
	--with-cgns=1 \
        --with-cgns-dir="${CGNS_DIR}" \
	--with-hdf5=1 \
        --with-hdf5-dir="${HDF5_ROOT}"
    tempval=$(prompt_yn "Is the configuration ok?")
    make -j${BUILD_NTHREADS}
    make install
    cd .. && echo "Directory: ${PWD}"

    echo "Creating modulefile for PETSc..."
    mkdir -p "${PETSC_MPATH}"
    cat > "${PETSC_MPATH}/${PETSC_VERSION}.lua" <<EOF
family("${PETSC_MODULENAME}")

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

module load "${PETSC_MODULENAME}/${PETSC_VERSION}"


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
    cd Loci && echo "Directory: ${PWD}"
    echo "Configuring Loci..."
    ./configure --prefix "${LOCI_IPATH}" --with-metis "${PARMETIS_DIR}"
    echo "Building Loci..."
    cd OBJ && echo "Directory: ${PWD}"
    make -j4
    echo "Installing Loci..."
    make install
    cd .. && echo "Directory: ${PWD}"
    cd .. && echo "Directory: ${PWD}"

    echo "Creating modulefile for Loci..."
    mkdir -p "${LOCI_MPATH}"
    cat > "${LOCI_MPATH}/${LOCI_VERSION}.lua" <<EOF
family("${LOCI_MODULENAME}")

depends_on("${COMPILER_MODULE}")
depends_on("${MPI_MODULE}")
depends_on("${MKL_MODULE}")
depends_on("${HDF5_MODULE}")
depends_on("${GLOG_MODULENAME}/${GLOG_VERSION}")
depends_on("${GTEST_MODULENAME}/${GTEST_VERSION}")
depends_on("${METIS_MODULENAME}/${METIS_VERSION}")
depends_on("${PARMETIS_MODULENAME}/${PARMETIS_VERSION}")
depends_on("${CGNS_MODULENAME}/${CGNS_VERSION}")
depends_on("${PETSC_MODULENAME}/${PETSC_VERSION}")

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

