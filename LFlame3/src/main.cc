//#include <easylogging++.h>

#include <boundary_checker.hh>
#include <logger.hh>
#include <plot.hh>

#include <Loci.h>

#include <iostream>
#include <iomanip>
#include <string>

#include <argp.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

struct arguments {
  std::string caseName;
  bool fpe;
  std::string icDirectory;
  std::string query;
};

static char doc[] = "Multi-Species Navier-Stokes Solver for Turbulent Combustion";

static char args_doc[] = "None";

static struct argp_option options[] = {
  {"case", 'c', "STRING", 0, "Case name, expected files: <case_name.vars> and <case_name.vog>"},
  {"fpe", 1000, 0, 0, "Enable floating point exception trapping"},
  {"ic", 'r', "DIR-PATH", 0, "Initial conditions directory"},
  {"query", 'q', "STRING", 0, "Query variable (default: solution)"},
  {0}
};

static error_t parse_opt(int key, char * arg, argp_state * state) {
  arguments * input = (arguments *)state->input;
  
  switch(key) {
  case 'c':
    input->caseName = std::string(arg);
    break;
  case 1000:
    input->fpe = true;
    break;
  case 'r':
    input->icDirectory = std::string(arg);
    break;
  case 'q':
    input->query = std::string(arg);
    break;
  default:
    return ARGP_ERR_UNKNOWN;
  }
  
  return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc };

//INITIALIZE_EASYLOGGINGPP

int main(int argc, char * argv[]) {
  Loci::Init(&argc, &argv);
  
  arguments arg;
  arg.caseName = "";
  arg.fpe = false;
  arg.icDirectory = "";
  arg.query = "solution";
  
  argp_parse(&argp, argc, argv, 0, 0, &arg);
  
  {
    // Initialize logging system
    flame::logger.initialize("debug/log", Loci::MPI_rank);
  }
  
  flame::logger.info("Program Arguments:");
  flame::logger.info("  ", std::setw(20), "case ", " = ", arg.caseName);
  flame::logger.info("  ", std::setw(20), "fpe enabled", " = ", std::boolalpha, arg.fpe, std::noboolalpha);
  flame::logger.info("  ", std::setw(20), "IC directory", " = \"", arg.icDirectory, "\"");
  flame::logger.info("  ", std::setw(20), "query", " = ", arg.query);
  
  // Create debug directory if it does not exist.
  {
    struct stat statbuf;
    int fid = open("debug", O_RDONLY);
    if(fid < 0) {
      mkdir("debug", 0755);
    } else {
      fstat(fid, &statbuf);
      if(!S_ISDIR(statbuf.st_mode)) {
        flame::logger.severe("file \"debug\" should be a directory!,"
          " rename \"debug\" and start again.");
        Loci::Abort();
      }
      close(fid);
    }
  }
  
  // Create output directory if it does not exist.
  {
    struct stat statbuf;
    int fid = open("output", O_RDONLY);
    if(fid < 0) {
      mkdir("output", 0755);
    } else {
      fstat(fid, &statbuf);
      if(!S_ISDIR(statbuf.st_mode)) {
        flame::logger.severe("file \"output\" should be a directory!,",
          " rename \"output\" and start again.");
        Loci::Abort();
      }
      close(fid);
    }
  }
  
  if(arg.fpe) {
    if(Loci::MPI_rank == 0) {
      flame::logger.info("Enabling floating point exception trapping");
    }
    set_fpe_abort();
  }
  
  rule_db rdb;
  fact_db facts;
  
  rdb.add_rules(global_rule_list);
  Loci::load_module("fvm", rdb);
  Loci::load_module("fvmoverset", rdb);
  
  // Load vars file.
  {
    std::stringstream ss;
    ss << arg.caseName << ".vars";
    
    if(Loci::MPI_rank == 0) {
      flame::logger.info("Reading vars file \"", ss.str(), "\"");
    }
    
    try {
      facts.read_vars(ss.str(), rdb);
    } catch(Loci::BasicException & err) {
      if(Loci::MPI_rank == 0) {
        std::stringstream ss1;
        err.Print(ss1);
        flame::logger.severe(ss1.str());
        flame::logger.severe("Could not read \"", ss.str(), "\"");
      }
      Loci::Abort();
    }
    
    if(Loci::MPI_rank == 0) {
      flame::logger.info("Reading vars file complete");
    }
  }
  
  // Read grid file
  {
    std::stringstream ss;
    ss << arg.caseName << ".vog";
    
    if(Loci::MPI_rank == 0) {
      flame::logger.info("Reading grid file \"", ss.str(), "\"");
    }
    
    if(!Loci::setupFVMGrid(facts, ss.str())) {
      if(Loci::MPI_rank == 0) {
        flame::logger.severe("Unable to read grid file \"", ss.str(), "\"");
      }
      Loci::Abort();
    }
    
    if(Loci::MPI_rank == 0) {
      flame::logger.info("Reading grid file complete");
    }
  }
  
  {
    param<std::string> caseName;
    *caseName = string(arg.caseName);
    facts.create_fact("caseName", caseName);
  }
  
  // Check boundary condition specification for errors.
  if(flame::check_boundary_conditions(facts)) {
    if(Loci::MPI_rank == 0) {
      flame::logger.severe("Boundary condition errors detected");
    }
    Loci::Abort();
  }
  
  Loci::setupBoundaryConditions(facts);
  Loci::createLowerUpper(facts);
  
  // Check for initial conditions directory and create necessary facts.
  if(arg.icDirectory.size() > 0) {
    std::string icDirectory = arg.icDirectory;
    std::string standardICDirectory = std::string("restart/") + icDirectory;
    
    struct stat statbuf;
    if(stat(standardICDirectory.c_str(), &statbuf)) {
      if(stat(icDirectory.c_str(), &statbuf)) {
        flame::logger.severe("Unable to open initial conditions directory: ", icDirectory);
        Loci::Abort();
      }
    } else {
      icDirectory = standardICDirectory;
    }
    
    if(!S_ISDIR(statbuf.st_mode)) {
      flame::logger.severe("file \"", icDirectory, "\" should be a directory!");
      Loci::Abort();
    }
    
    icDirectory += "/";
    
    param<bool> hasICDirectory; *hasICDirectory = true;
    facts.create_fact("hasICDirectory", hasICDirectory);
    
    param<std::string> icdir; *icdir = icDirectory;
    facts.create_fact("icDirectory", icdir);
  } else {
    param<bool> hasICDirectory; *hasICDirectory = false;
    facts.create_fact("hasICDirectory", hasICDirectory);
  }
  
  // Setup constraints for plot variables.
  {
    param<std::string> plotInfo;
    std::stringstream plotInfoValue;
    param<options_list> plotOptions;
    plotOptions = facts.get_variable("plotOptions");
    
    flame::PlotSettings settings;
    std::string err;
    if(settings.fromOptionsList(*plotOptions, err)) {
      flame::logger.severe(err);
      Loci::Abort();
    }
    
    int const nNodalVariables = settings.nodalVariables.size();
    int const nBoundaryVariables = settings.boundaryVariables.size();
    int const nTotalVariables = nNodalVariables + nBoundaryVariables;
    
    for(int i = 0; i < nNodalVariables; ++i) {
      constraint x;
      x = ~EMPTY;
      std::string constraintName = std::string("plotNodal_") + settings.nodalVariables[i];
      facts.create_fact(constraintName, x);
      plotInfoValue << constraintName << ":";
    }
    
    for(int i = 0; i < nBoundaryVariables; ++i) {
      constraint x;
      x = ~EMPTY;
      std::string constraintName = std::string("plotBoundary_") + settings.boundaryVariables[i];
      facts.create_fact(constraintName, x);
      plotInfoValue << constraintName << ":";
    }
    
    // Pambient is a special param that always needs to be written out
    {
      std::string constraintName = "plotParam_Pambient";
      constraint plotParam_Pambient;
      plotParam_Pambient = ~EMPTY;
      facts.create_fact(constraintName, plotParam_Pambient);
      plotInfoValue << constraintName;
    }
    
    *plotInfo = plotInfoValue.str();
    facts.create_fact("plotInfo", plotInfo);
  }
  
  // Dump parameters from the fact database.
  if(Loci::MPI_rank == 0) {
    std::stringstream ss;
    ss << "output/run_info." << arg.caseName;
    
    std::ofstream db_file(ss.str());
    if(!db_file.fail()) {
      using namespace Loci;
      db_file << "case = " << arg.caseName << endl;
      db_file << "Query = \"" << arg.query << '"' << endl;
      db_file << "facts = {" << endl;
      variableSet ext_facts = facts.get_extensional_facts() ;
      for(variableSet::const_iterator vi = ext_facts.begin(); vi != ext_facts.end(); ++vi) {
        storeRepP sp = facts.get_variable(*vi);
        if(sp != 0) {
          if(sp->RepType() == PARAMETER) {
            db_file << *vi << ": ";
            sp->Print(db_file);
          }
        }
      }
      db_file << "}" << endl;
    }
  }
  
  // Create execution schedule that derives the variable named in
  // arg.query from the database of facts using the rule database rdb.
  if(!Loci::makeQuery(rdb, facts, arg.query)) {
    flame::logger.severe("Query failed!");
    Loci::Abort();
  }
  
  Loci::Finalize();
  
  return 0;
}
