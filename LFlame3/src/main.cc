#include <flame.hh>
#include <plot.hh>
#include <boundary_checker.hh>

#include <iostream>
#include <iomanip>
#include <string>

#include <argp.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <glog/logging.h>

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

void logPrefix(std::ostream& s, const google::LogMessageInfo& l, void* data) {
  s << l.severity;
}

int main(int argc, char * argv[]) {
  Loci::Init(&argc, &argv);
  
  google::InitGoogleLogging(argv[0], logPrefix);
  google::InstallFailureSignalHandler();
  FLAGS_logtostdout=1;
  //FLAGS_logtostderr = 1;
  //FLAGS_max_log_size=1024; // in MB
  //FLAGS_log_prefix=0;
  //FLAGS_log_dir=".";
  
  arguments arg;
  arg.caseName = "";
  arg.fpe = false;
  arg.icDirectory = "";
  arg.query = "solution";
  
  argp_parse(&argp, argc, argv, 0, 0, &arg);
  
  //{
  //  // Initialize logging system
  //  flame::logger.initialize("debug/log", Loci::MPI_rank);
  //}
  
  if(Loci::MPI_rank == 0) {
    LOG(INFO) << "Program Arguments:";
    LOG(INFO) << "  case = " << arg.caseName;
    LOG(INFO) << "  fpe enabled = " << arg.fpe;
    LOG(INFO) << "  IC directory = " << arg.icDirectory;
    LOG(INFO) << "  query = " << arg.query;
  }
  
  // Create debug directory if it does not exist.
  {
    struct stat statbuf;
    int fid = open("debug", O_RDONLY);
    if(fid < 0) {
      mkdir("debug", 0755);
    } else {
      fstat(fid, &statbuf);
      if(!S_ISDIR(statbuf.st_mode)) {
        LOG(ERROR) << "file \"debug\" should be a directory!," <<
          " rename \"debug\" and start again.";
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
        LOG(ERROR) << "file \"output\" should be a directory!," <<
          " rename \"output\" and start again.";
        Loci::Abort();
      }
      close(fid);
    }
  }
  
  if(arg.fpe) {
    if(Loci::MPI_rank == 0) {
      LOG(INFO) << "Enabling floating point exception trapping";
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
    std::ostringstream ss;
    ss << arg.caseName << ".vars";
    
    if(Loci::MPI_rank == 0) {
      LOG(INFO) << "Reading vars file \"" << ss.str() << "\"";
    }
    
    try {
      facts.read_vars(ss.str(), rdb);
    } catch(Loci::BasicException & err) {
      if(Loci::MPI_rank == 0) {
        std::ostringstream ss1;
        err.Print(ss1);
        LOG(ERROR) << ss1.str() << ": Could not read \"" << ss.str() << "\"";
      }
      Loci::Abort();
    }
    
    if(Loci::MPI_rank == 0) {
      LOG(INFO) << "Reading vars file complete";
    }
  }
  
  // Read grid file
  {
    std::stringstream ss;
    ss << arg.caseName << ".vog";
    
    if(Loci::MPI_rank == 0) {
      LOG(INFO) << "Reading grid file \"" << ss.str() << "\"";
    }
    
    if(!Loci::setupFVMGrid(facts, ss.str())) {
      if(Loci::MPI_rank == 0) {
        LOG(ERROR) << "Unable to read grid file \"" << ss.str() << "\"";
      }
      Loci::Abort();
    }
    
    if(Loci::MPI_rank == 0) {
      LOG(INFO) << "Reading grid file complete";
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
      LOG(ERROR) << "Boundary condition errors detected";
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
        LOG(ERROR) << "Unable to open initial conditions directory: " << icDirectory;
        Loci::Abort();
      }
    } else {
      icDirectory = standardICDirectory;
    }
    
    if(!S_ISDIR(statbuf.st_mode)) {
      LOG(ERROR) << "file \"" << icDirectory << "\" should be a directory!";
      Loci::Abort();
    }
    
    icDirectory += "/";
    
    param<bool> hasICDirectory; *hasICDirectory = true;
    facts.create_fact("hasICDirectory", hasICDirectory);
    
    constraint withICDirectory;
    withICDirectory = ~EMPTY;
    facts.create_fact("withICDirectory", withICDirectory);
    
    param<std::string> icdir; *icdir = icDirectory;
    facts.create_fact("icDirectory", icdir);
  } else {
    param<bool> hasICDirectory; *hasICDirectory = false;
    facts.create_fact("hasICDirectory", hasICDirectory);
    
    constraint withoutICDirectory;
    withoutICDirectory = ~EMPTY;
    facts.create_fact("withoutICDirectory", withoutICDirectory);
  }
  
  //// Setup constraints for plot variables.
  //{
  //  param<std::string> plotInfo;
  //  std::stringstream plotInfoValue;
  //  param<options_list> plotOptions;
  //  plotOptions = facts.get_variable("plotOptions");
  //  
  //  flame::PlotSettings settings;
  //  std::string err;
  //  if(settings.fromOptionsList(*plotOptions, err)) {
  //    LOG(ERROR) << err;
  //    Loci::Abort();
  //  }
  //  
  //  int const nNodalVariables = settings.nodalVariables.size();
  //  int const nBoundaryVariables = settings.boundaryVariables.size();
  //  int const nTotalVariables = nNodalVariables + nBoundaryVariables;
  //  
  //  for(int i = 0; i < nNodalVariables; ++i) {
  //    constraint x;
  //    x = ~EMPTY;
  //    std::string constraintName = std::string("plotNodal_") + settings.nodalVariables[i];
  //    facts.create_fact(constraintName, x);
  //    plotInfoValue << constraintName << ":";
  //  }
  //  
  //  for(int i = 0; i < nBoundaryVariables; ++i) {
  //    constraint x;
  //    x = ~EMPTY;
  //    std::string constraintName = std::string("plotBoundary_") + settings.boundaryVariables[i];
  //    facts.create_fact(constraintName, x);
  //    plotInfoValue << constraintName << ":";
  //  }
  //  
  //  // Pambient is a special param that always needs to be written out
  //  {
  //    std::string constraintName = "plotParam_Pambient";
  //    constraint plotParam_Pambient;
  //    plotParam_Pambient = ~EMPTY;
  //    facts.create_fact(constraintName, plotParam_Pambient);
  //    plotInfoValue << constraintName;
  //  }
  //  
  //  *plotInfo = plotInfoValue.str();
  //  facts.create_fact("plotInfo", plotInfo);
  //}
  
  // Process printOptions
  {
    Loci::storeRepP posp = facts.get_variable("printOptions");
    if(posp != 0 && posp->RepType() == Loci::PARAMETER) {
      param<options_list> printOptions;
      printOptions.setRep(posp);

      // Parse printOptions.
      flame::PrintSettings settings;
      std::string err;
      if(settings.fromOptionsList(*printOptions, err)) {
        LOG(ERROR) << err;
        Loci::Abort();
      }

      // Create fact for printSettings.
      param<flame::PrintSettings> printSettings;
      *printSettings = settings;
      facts.create_fact("printSettings", printSettings);

      // Create constraints for individual parameters to print.
      int const nParams = settings.parameters.size();
      for(int i = 0; i < nParams; ++i) {
        std::string constraintName = std::string("printParam_") + settings.parameters[i];
        constraint x;
        x = ~EMPTY;
        facts.create_fact(constraintName, x);
      }
    } else {
      if(Loci::MPI_rank == 0) {
        LOG(INFO) << "printOptions not specified";
      }
    }
  }

  // Process plotOptions.
  {
    Loci::storeRepP posp = facts.get_variable("plotOptions");
    if(posp != 0 && posp->RepType() == Loci::PARAMETER) {
      param<std::string> plotInfo;
      std::stringstream plotInfoValue;
      param<options_list> plotOptions;
      plotOptions.setRep(posp);

      // Parse plotOptions.
      flame::PlotSettings settings;
      std::string err;
      if(settings.fromOptionsList(*plotOptions, err)) {
        LOG(ERROR) << err;
        Loci::Abort();
      }

      // Create fact for plotSettings.
      param<flame::PlotSettings> plotSettings;
      *plotSettings = settings;
      facts.create_fact("plotSettings", plotSettings);

      // Create constraints for individual plotting variables.
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
        constraint x;
        x = ~EMPTY;
        facts.create_fact(constraintName, x);
        plotInfoValue << constraintName;
      }

      *plotInfo = plotInfoValue.str();
      facts.create_fact("plotInfo", plotInfo);
    } else {
      if(Loci::MPI_rank == 0) {
        LOG(INFO) << "plotOptions not specified";
      }
    }
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
    LOG(ERROR) << "Query failed!";
    Loci::Abort();
  }
  
  Loci::Finalize();
  
  return 0;
}
