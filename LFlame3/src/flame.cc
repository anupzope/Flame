#include <flame.hh>

namespace flame {

RegisterUserVariable::RegisterUserVariable(
  std::string const & userName, std::string const & internalName
) {
  UserVariableRegistry::get().addVariable(userName, internalName);
}

UserVariableRegistry::UserVariableRegistry() {
}

void UserVariableRegistry::addVariable(
  std::string const & userName, std::string const & internalName
) {
  std::map<std::string, std::string>::const_iterator iter;
  iter = variables.find(userName);
  if(iter == variables.end()) {
    variables.insert(std::make_pair(userName, internalName));
  } else {
    throw std::runtime_error(
      std::string("user variable ")+userName+std::string(" exists in registry")
    );
  }
}

UserVariableRegistry & UserVariableRegistry::get() {
  static UserVariableRegistry obj;
  return obj;
}

bool UserVariableRegistry::containsUserVariable(std::string const & userName) const {
  std::map<std::string, std::string>::const_iterator iter;
  iter = variables.find(userName);
  return iter != variables.end();
}

bool UserVariableRegistry::containsInternalVariable(std::string const & internalName) const {
  bool found = false;
  std::map<std::string, std::string>::const_iterator iter;
  iter = variables.begin();
  while(iter != variables.end()) {
    if(iter->second == internalName) {
      found = true;
      break;
    }
    ++iter;
  }
  return true;
}

std::string const & UserVariableRegistry::getInternalName(std::string const & userName) const {
  std::map<std::string, std::string>::const_iterator iter;
  iter = variables.find(userName);
  if(iter != variables.end()) {
    return iter->second;
  } else {
    throw std::runtime_error(
      std::string("user variable ") +
      userName +
      std::string(" does not exists in registry")
    );
  }
}

RegisterUserVariable pg_soundSpeed("pg", "gagePressure");
RegisterUserVariable t_temperature("t", "temperature");
RegisterUserVariable r_density("r", "density");
RegisterUserVariable v_velocity("v", "velocity");
RegisterUserVariable a_soundSpeed("a", "soundSpeed");

} // end: namespace flame
