bin_PROGRAMS = LFlame3

LPP = @LPP@

SUFFIXES = .loci

.loci.cc:
	$(LPP) $(LFlame3_CXXFLAGS) $(LFlame3_CPPFLAGS) $< -o $@

LFlame3_SOURCES=src/main.cc \
  src/boundary_checker.cc \
  src/grid_post_processor.cc \
  src/bcImpermeable.cc \
  src/logger.cc \
  src/plot.cc \
  src/totalVolume.cc \
  src/species.cc \
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
  src/solverTransport.cc
LFlame3_LDFLAGS = 
LFlame3_LDADD = 
LFlame3_CXXFLAGS = -I$(srcdir)/include
LFlame3_CPPFLAGS = 
if HAVE_MPI
  LFlame3_LDFLAGS += $(MPI_LDFLAGS)
  LFlame3_LDADD += $(MPI_LIBS)
  LFlame3_CXXFLAGS += $(MPI_CXXFLAGS)
  LFlame3_CPPFLAGS += $(MPI_CPPFLAGS)
endif
if HAVE_LOCI
  LFlame3_LDFLAGS += $(LOCI_LDFLAGS) $(LOCI_DEP_LDFLAGS)
  LFlame3_LDADD += $(LOCI_LIBS) $(LOCI_DEP_LIBS)
  LFlame3_CXXFLAGS += $(LOCI_CXXFLAGS) $(LOCI_DEP_CXXFLAGS)
  LFlame3_CPPFLAGS += $(LOCI_CPPFLAGS) $(LOCI_DEP_CPPFLAGS)
endif
if HAVE_GLOG
  LFlame3_LDFLAGS += $(GLOG_LDFLAGS)
  LFlame3_LDADD += $(GLOG_LIBS)
  LFlame3_CXXFLAGS += $(GLOG_CFLAGS)
  LFlame3_CPPFLAGS += -DFLAME_HAVE_GLOG
endif
