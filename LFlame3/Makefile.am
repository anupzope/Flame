bin_PROGRAMS = LFlame3 LFlame3UTests

LPP = @LPP@

SUFFIXES = .loci

.loci.cc:
	$(LPP) $(LOCI_CXXFLAGS) -I$(srcdir)/include $< -o $@

LFlame3_SOURCES=src/main.cc \
  src/flame.cc \
  src/boundary_checker.cc \
  src/grid_post_processor.cc \
  src/bcImpermeable.cc \
  src/bcViscousWall.cc \
  src/plot.cc \
  src/totalVolume.cc \
  src/mixture.cc \
  src/initialConditions.cc \
  src/solverTimestepping.cc \
  src/solverRungeKutta.cc \
  src/solverPlotting.cc \
  src/solverMultispecies.cc \
  src/solverInviscid.cc \
  src/solverViscous.cc \
  src/solverParameters.cc \
  src/constants.cc \
  src/solverEoS.cc \
  src/utils.cc \
  src/flux.cc \
  src/solverPeriodic.cc \
  src/bcSetup.cc \
  src/solverVolumeIntegrated.cc \
  src/solverCFL.cc \
  src/solverThermodynamics.cc \
  src/solverTransport.cc \
  src/solverFlowRegime.cc \
  src/solverFaceValues.cc \
  src/restart.cc \
  src/solverRestart.cc \
  src/bcInflowOutflow.cc \
  src/bcInputs.cc \
  src/bcRefs.cc \
  src/boundaryRefs.cc \
  src/signal_handler.cc \
  src/gridMetrics.cc \
  src/solverDiffusive.cc \
  src/solverTimeAveraging.cc

LFlame3_LDFLAGS = $(LDFLAGS)
LFlame3_LDADD = 
LFlame3_CXXFLAGS = $(CXXFLAGS) -I$(srcdir)/include
LFlame3_CPPFLAGS = $(CPPFLAGS) -DFLAME_DATA_DIR=\"$(pkgdatadir)\" \
  -DFLAME_NAME_MAX_LENGTH=256 \
  -DFLAME_MAX_NSPECIES=256

if HAVE_GLOG
  LFlame3_CPPFLAGS += -DFLAME_HAVE_GLOG
  LFlame3_CXXFLAGS += $(GLOG_CXXFLAGS)
  LFlame3_LDFLAGS += $(GLOG_LDFLAGS)
  LFlame3_LDADD += $(GLOG_LIBS)
endif
if HAVE_XML2
  LFlame3_CPPFLAGS += $(XML2_CPPFLAGS)
  LFlame3_CXXFLAGS += $(XML2_CFLAGS)
  LFlame3_LDFLAGS += $(XML2_LDFLAGS)
  LFlame3_LDADD += $(XML2_LIBS)
endif
if HAVE_MPI
  LFlame3_CPPFLAGS += $(MPI_CPPFLAGS)
  LFlame3_CXXFLAGS += $(MPI_CFLAGS)
  LFlame3_LDFLAGS += $(MPI_LDFLAGS)
  LFlame3_LDADD += $(MPI_LIBS)
endif
if HAVE_HDF5
  LFlame3_CPPFLAGS += $(HDF5_CPPFLAGS)
  LFlame3_CXXFLAGS += $(HDF5_CFLAGS)
  LFlame3_LDFLAGS += $(HDF5_LDFLAGS)
  LFlame3_LDADD += $(HDF5_LIBS)
endif
if HAVE_METIS
  LFlame3_CPPFLAGS += $(METIS_CPPFLAGS)
  LFlame3_CXXFLAGS += $(METIS_CFLAGS)
  LFlame3_LDFLAGS += $(METIS_LDFLAGS)
  LFlame3_LDADD += $(METIS_LIBS)
endif
if HAVE_PARMETIS
  LFlame3_CPPFLAGS += $(PARMETIS_CPPFLAGS)
  LFlame3_CXXFLAGS += $(PARMETIS_CFLAGS)
  LFlame3_LDFLAGS += $(PARMETIS_LDFLAGS)
  LFlame3_LDADD += $(PARMETIS_LIBS)
endif
if HAVE_CGNS
  LFlame3_CPPFLAGS += $(CGNS_CPPFLAGS)
  LFlame3_CXXFLAGS += $(CGNS_CFLAGS)
  LFlame3_LDFLAGS += $(CGNS_LDFLAGS)
  LFlame3_LDADD += $(CGNS_LIBS)
endif
if HAVE_PETSC
  LFlame3_CPPFLAGS += $(PETSC_CPPFLAGS)
  LFlame3_CXXFLAGS += $(PETSC_CFLAGS)
  LFlame3_LDFLAGS += $(PETSC_LDFLAGS)
  LFlame3_LDADD += $(PETSC_LIBS)
endif
if HAVE_LOCI
  LFlame3_CPPFLAGS += $(LOCI_CPPFLAGS)
  LFlame3_CXXFLAGS += $(LOCI_CXXFLAGS)
  LFlame3_LDFLAGS += $(LOCI_LDFLAGS)
  LFlame3_LDADD += $(LOCI_LIBS)
endif

LFlame3UTests_SOURCES=src/mixture.cc \
  tests/unit_tests_main.cc \
  tests/test_mixture_specification.cc

LFlame3UTests_LDFLAGS = $(LDFLAGS)
LFlame3UTests_LDADD = 
LFlame3UTests_CXXFLAGS = $(CXXFLAGS) -I$(srcdir)/include
LFlame3UTests_CPPFLAGS = $(CPPFLAGS) -DFLAME_DATA_DIR=\"$(pkgdatadir)\" \
  -DFLAME_NAME_MAX_LENGTH=256 \
  -DFLAME_MAX_NSPECIES=256

if HAVE_GTEST
  LFlame3UTests_CPPFLAGS += -DFLAME_HAVE_GTEST
  LFlame3UTests_CXXFLAGS += $(GTEST_CXXFLAGS)
  LFlame3UTests_LDFLAGS += $(GTEST_LDFLAGS)
  LFlame3UTests_LDADD += $(GTEST_LIBS)
endif
if HAVE_GLOG
  LFlame3UTests_CPPFLAGS += -DFLAME_HAVE_GLOG
  LFlame3UTests_CXXFLAGS += $(GLOG_CXXFLAGS)
  LFlame3UTests_LDFLAGS += $(GLOG_LDFLAGS)
  LFlame3UTests_LDADD += $(GLOG_LIBS)
endif
if HAVE_XML2
  LFlame3UTests_CPPFLAGS += $(XML2_CPPFLAGS)
  LFlame3UTests_CXXFLAGS += $(XML2_CFLAGS)
  LFlame3UTests_LDFLAGS += $(XML2_LDFLAGS)
  LFlame3UTests_LDADD += $(XML2_LIBS)
endif
if HAVE_MPI
  LFlame3UTests_CPPFLAGS += $(MPI_CPPFLAGS)
  LFlame3UTests_CXXFLAGS += $(MPI_CFLAGS)
  LFlame3UTests_LDFLAGS += $(MPI_LDFLAGS)
  LFlame3UTests_LDADD += $(MPI_LIBS)
endif
if HAVE_HDF5
  LFlame3UTests_CPPFLAGS += $(HDF5_CPPFLAGS)
  LFlame3UTests_CXXFLAGS += $(HDF5_CFLAGS)
  LFlame3UTests_LDFLAGS += $(HDF5_LDFLAGS)
  LFlame3UTests_LDADD += $(HDF5_LIBS)
endif
if HAVE_METIS
  LFlame3UTests_CPPFLAGS += $(METIS_CPPFLAGS)
  LFlame3UTests_CXXFLAGS += $(METIS_CFLAGS)
  LFlame3UTests_LDFLAGS += $(METIS_LDFLAGS)
  LFlame3UTests_LDADD += $(METIS_LIBS)
endif
if HAVE_PARMETIS
  LFlame3UTests_CPPFLAGS += $(PARMETIS_CPPFLAGS)
  LFlame3UTests_CXXFLAGS += $(PARMETIS_CFLAGS)
  LFlame3UTests_LDFLAGS += $(PARMETIS_LDFLAGS)
  LFlame3UTests_LDADD += $(PARMETIS_LIBS)
endif
if HAVE_CGNS
  LFlame3UTests_CPPFLAGS += $(CGNS_CPPFLAGS)
  LFlame3UTests_CXXFLAGS += $(CGNS_CFLAGS)
  LFlame3UTests_LDFLAGS += $(CGNS_LDFLAGS)
  LFlame3UTests_LDADD += $(CGNS_LIBS)
endif
if HAVE_PETSC
  LFlame3UTests_CPPFLAGS += $(PETSC_CPPFLAGS)
  LFlame3UTests_CXXFLAGS += $(PETSC_CFLAGS)
  LFlame3UTests_LDFLAGS += $(PETSC_LDFLAGS)
  LFlame3UTests_LDADD += $(PETSC_LIBS)
endif
if HAVE_LOCI
  LFlame3UTests_CPPFLAGS += $(LOCI_CPPFLAGS)
  LFlame3UTests_CXXFLAGS += $(LOCI_CXXFLAGS)
  LFlame3UTests_LDFLAGS += $(LOCI_LDFLAGS)
  LFlame3UTests_LDADD += $(LOCI_LIBS)
endif
