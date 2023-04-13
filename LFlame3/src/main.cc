#include <Loci.h>

#include <iostream>
#include <iomanip>

//#include <cstdlib>
#include <argp.h>
#include <unistd.h>

struct arguments {
  std::string meshFileName;
  std::string inputFileName;
  std::string caseName;
};

static char doc[] = "Multi-Species Navier-Stokes Solver or Turbulent Combustion";

static char args_doc[] = "None";

static struct argp_option options[] = {
  {"mesh", 'm', "PATH-STRING", 0, "VOG mesh file name"},
  {"input", 'i', "PATH-STRING", 0, "VARS file name"},
  {"case", 'c' , "STRING", 0, "Name of this case (default=case)"},
  {0}
};

static error_t parse_opt(int key, char * arg, argp_state * state) {
  arguments * input = (arguments *)state->input;
  
  switch(key) {
  case 'm':
    input->meshFileName = std::string(arg);
    break;
  case 'i':
    input->inputFileName = std::string(arg);
    break;
  case 'c':
    input->caseName = std::string(arg);
    break;
  default:
    return ARGP_ERR_UNKNOWN;
  }
  
  return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc };

int main(int argc, char * argv[]) {
  Loci::Init(&argc, &argv);
  
  arguments arg;
  arg.meshFileName = "";
  arg.inputFileName = "";
  arg.caseName = "case";
  
  argp_parse(&argp, argc, argv, 0, 0, &arg);
  
  std::cout << "Program Arguments:" << std::endl;
  std::cout << "  " << std::setw(20) << "mesh file" << " = " << arg.meshFileName << std::endl;
  std::cout << "  " << std::setw(20) << "input file" << " = " << arg.inputFileName << std::endl;
  std::cout << "  " << std::setw(20) << "case name" << " = " << arg.caseName << std::endl;
  
  //rule_db rdb;
  //fact_db fdb;
  //
  //rdb.add_rules(global_rule_list);
  //
  //Loci::load_module("fvm", rdb);
  
  Loci::Finalize();
  
  return 0;
}
