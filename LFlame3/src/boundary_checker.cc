#include <boundary_checker.hh>

namespace flame {

BoundaryCheckerList::Item * BoundaryCheckerList::items = nullptr;

struct BoundaryCheckerInfo {
  Loci::variableSet bcNames;
  Loci::variableSet bcOptions;
  Loci::CPTR<BoundaryChecker> checker;
};

bool check_boundary_conditions(fact_db & facts) {
  bool error = false;
  
  // Count the number of boundary checkers.
  int nBCChecker = 0;
  {
    BoundaryCheckerList::Item * bcChecker = BoundaryCheckerList::items;
    while(bcChecker != nullptr) {
      ++nBCChecker;
      bcChecker = bcChecker->next;
    }
  }
  
  // Collect information of all the boundary checkers.
  BoundaryCheckerInfo * bcCheckerInfo = new BoundaryCheckerInfo[nBCChecker];
  {
    BoundaryCheckerList::Item * bcChecker = BoundaryCheckerList::items;
    int i = 0;
    while(bcChecker != nullptr) {
      Loci::variableSet bcNames;
      std::cerr << bcChecker->entry->boundaryConditions() << std::endl;
      if(bcChecker->entry->boundaryConditions() == "*") {
        bcNames = ~EMPTY;
      } else {
        Loci::exprP expr = Loci::expression::create(bcChecker->entry->boundaryConditions());
        bcNames = Loci::variableSet(expr) ;
      }
      
      Loci::variableSet bcOptions;
      if(bcChecker->entry->variablesChecked().size() == 0) {
        bcOptions = EMPTY;
      } else {
        Loci::exprP expr = Loci::expression::create(bcChecker->entry->variablesChecked());
        bcOptions = Loci::variableSet(expr);
      }
      
      bcCheckerInfo[i].bcNames = bcNames;
      bcCheckerInfo[i].bcOptions = bcOptions;
      bcCheckerInfo[i].checker = bcChecker->entry;
      
      bcChecker = bcChecker->next;
      ++i;
    }
  }
  
  // Process the boundaryConditions variable in the fact database for
  // sanity.
  param<options_list> bcInfo;
  bcInfo = facts.get_variable("boundary_conditions");
  
  options_list::option_namelist nl = bcInfo->getOptionNameList();
  options_list::option_namelist::iterator li;
  
  // Print boundary names.
  //std::cout << "Boundary names:" << std::endl;
  //for(li = nl.begin(); li != nl.end(); ++li) {
  //  std::string bName = *li;
  //  std::cout << "  " << bName << std::endl;
  //}
  
  // Validate boundary types and boundary options.
  for(li = nl.begin(); li != nl.end(); ++li) {
    std::string bName = *li;
    //std::cout << "Boundary Name = " << bName << std::endl;
    Loci::option_value_type valueType = bcInfo->getOptionValueType(bName);
    Loci::option_values values = bcInfo->getOption(bName);
    
    options_list::arg_list valueList;
    std::string bcName;
    
    switch(valueType) {
    case Loci::NAME:
      values.get_value(bcName);
      break;
    case Loci::FUNCTION:
      values.get_value(bcName);
      values.get_value(valueList);
      break;
    default:
      if(Loci::MPI_rank == 0) {
        std::cerr << "cannot interprete value assigned to boundary \""
          << bName << "\"" << std::endl;
      }
      error = true;
      delete[] bcCheckerInfo;
      return error;
    }
    
    options_list bcOptions;
    bcOptions.Input(valueList);
    
    // Create a set of all the variables that are specified as options
    // for this boundary condition.
    Loci::variableSet bcOptionVars;
    {
      options_list::option_namelist onl = bcOptions.getOptionNameList();
      options_list::option_namelist::iterator it;
      for(it = onl.begin(); it != onl.end(); ++it) {
        bcOptionVars += Loci::variable(*it);
      }
    }
    
    Loci::variable bcVar(bcName);
    
    bool foundBCMatch = false;
    bool foundEmptyMatch = false;
    Loci::variableSet uncheckedBCOptions = bcOptionVars;
    
    for(int i = 0; i < nBCChecker; ++i) {
      if(bcCheckerInfo[i].bcNames.inSet(bcVar)) {
        if(bcCheckerInfo[i].bcNames != ~EMPTY) {
          foundBCMatch = true;
          if(bcCheckerInfo[i].bcOptions == EMPTY) {
            foundEmptyMatch = true;
          }
        }
        
        if((bcCheckerInfo[i].bcOptions & bcOptionVars) != EMPTY) {
          try {
            if(bcCheckerInfo[i].checker->checkOptions(bcOptions)) {
              uncheckedBCOptions -= bcCheckerInfo[i].bcOptions;
            }
          } catch(Loci::BasicException & err) {
            if(Loci::MPI_rank == 0) {
              std::cerr << "ERROR: boundary type " << bcName << " for boundary id \""
                << bName << "\":" << std::endl;
              err.Print(std::cerr);
            }
          }
        }
      }
    }
    
    if(!foundBCMatch) {
      if(Loci::MPI_rank == 0) {
        std::cerr << "Boundary type \"" << bcName << "\" is unknown for boundary id \""
          << bName << "\"" << std::endl;
      }
      error = true;
    } else if(bcOptionVars == EMPTY && !foundEmptyMatch) {
      if(Loci::MPI_rank == 0) {
        std::cerr << "Boundary type \"" << bcName
          << "\" requires argument(s) for boundary id \"" << bName << "\"" << std::endl;
      }
      error = true;
    } else if(uncheckedBCOptions != EMPTY) {
      if(Loci::MPI_rank == 0) {
        bool errorPrinted = false;
        for(int i = 0; i < nBCChecker; ++i) {
          if(bcCheckerInfo[i].bcNames.inSet(bcVar)) {
            if((bcCheckerInfo[i].bcOptions & uncheckedBCOptions) != EMPTY) {
              std::cerr << "check failed for boundary condition "
                << bcName << " on boundary id \"" << bName << "\"" << std::endl;
              bcCheckerInfo[i].checker->errorMessage(std::cerr);
              errorPrinted = true;
            }
          }
        }
        if(!errorPrinted) {
          std::cerr << "options " << uncheckedBCOptions
            << " not compatible with boundary condition "
            << bcName << " for boundary id \"" << bName << "\"" << std::endl;
        }
      }
      error = true;
    }
  }
  
  delete[] bcCheckerInfo;
  
  return error;
}

}
