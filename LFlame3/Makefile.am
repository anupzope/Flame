bin_PROGRAMS = LFlame3 LFlame3UTests

LPP = @LPP@

SUFFIXES = .loci

.loci.cc:
	$(LPP) $(LOCI_CXXFLAGS) -I$(srcdir)/include $< -o $@

LFlame3_SOURCES=src/main.cc \
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
  src/solverDiffusive.cc

LFlame3_LDFLAGS = $(LDFLAGS)
LFlame3_LDADD = 
LFlame3_CXXFLAGS = $(CXXFLAGS) -I$(srcdir)/include
LFlame3_CPPFLAGS = $(CPPFLAGS) -DFLAME_DATA_DIR=\"$(pkgdatadir)\" \
  -DFLAME_NAME_MAX_LENGTH=256 \
  -DFLAME_MAX_NSPECIES=256

if HAVE_XML2
  LFlame3_CXXFLAGS += $(XML2_CFLAGS)
  LFlame3_LDFLAGS += $(XML2_LIBS)
endif
if HAVE_LOCI
  LFlame3_LDFLAGS += $(LOCI_LDFLAGS)
  LFlame3_LDADD += $(LOCI_LIBS)
  LFlame3_CXXFLAGS += $(LOCI_CXXFLAGS)
  LFlame3_CPPFLAGS += $(LOCI_CPPFLAGS)
endif
if HAVE_GLOG
  LFlame3_LDFLAGS += $(GLOG_LDFLAGS)
  LFlame3_LDADD += $(GLOG_LIBS)
  LFlame3_CXXFLAGS += $(GLOG_CFLAGS)
  LFlame3_CPPFLAGS += -DFLAME_HAVE_GLOG
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

if HAVE_XML2
  LFlame3UTests_CXXFLAGS += $(XML2_CFLAGS)
  LFlame3UTests_LDFLAGS += $(XML2_LIBS)
endif
if HAVE_LOCI
  LFlame3UTests_LDFLAGS += $(LOCI_LDFLAGS)
  LFlame3UTests_LDADD += $(LOCI_LIBS)
  LFlame3UTests_CXXFLAGS += $(LOCI_CXXFLAGS)
  LFlame3UTests_CPPFLAGS += $(LOCI_CPPFLAGS)
endif
if HAVE_GLOG
  LFlame3UTests_LDFLAGS += $(GLOG_LDFLAGS)
  LFlame3UTests_LDADD += $(GLOG_LIBS)
  LFlame3UTests_CXXFLAGS += $(GLOG_CFLAGS)
  LFlame3UTests_CPPFLAGS += -DFLAME_HAVE_GLOG
endif
if HAVE_GTEST
  LFlame3UTests_LDFLAGS += $(GTEST_LDFLAGS)
  LFlame3UTests_LDADD += $(GTEST_LIBS)
  LFlame3UTests_CXXFLAGS += $(GTEST_CFLAGS)
  LFlame3UTests_CPPFLAGS += -DFLAME_HAVE_GTEST
endif
