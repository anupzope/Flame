#include <species.hh>

#include <sstream>

namespace flame {

std::ostream & operator<<(std::ostream & s, SpeciesProperties const & obj) {
  s << obj.molecularWeight << ' ';
  
  s << obj.constantSpecificHeat << ' ';
  
  s << obj.constantGamma << ' ';
  
  s << obj.constantViscosity << ' ';
  
  for(int i = 0; i < 3; ++i) {
    s << obj.sutherlandViscosityParameters[i] << ' ';
  }
  
  s << obj.constantConductivity << ' ';
  
  for(int i = 0; i < 3; ++i) {
    s << obj.sutherlandConductivityParameters[i] << ' ';
  }
  
  return s;
}

std::istream & operator>>(std::istream & s, SpeciesProperties & obj) {
  s >> obj.molecularWeight;
  
  s >> obj.constantSpecificHeat;
  
  s >> obj.constantGamma;
  
  s >> obj.constantViscosity;
  
  for(int i = 0; i < 3; ++i) {
    s >> obj.sutherlandViscosityParameters[i];
  }
  
  s >> obj.constantConductivity;
  
  for(int i = 0; i < 3; ++i) {
    s >> obj.sutherlandConductivityParameters[i];
  }
  
  return s;
}

std::ostream & serialize(std::ostream & s, SpeciesProperties const & obj) {
  s.write((char const *)(&obj.molecularWeight), sizeof(double));
  s.write((char const *)(&obj.constantSpecificHeat), sizeof(double));
  s.write((char const *)(&obj.constantGamma), sizeof(double));
  s.write((char const *)(&obj.constantViscosity), sizeof(double));
  s.write((char const *)(obj.sutherlandViscosityParameters), sizeof(double)*3);
  s.write((char const *)(&obj.constantConductivity), sizeof(double));
  s.write((char const *)(obj.sutherlandConductivityParameters), sizeof(double)*3);
  return s;
}

std::istream & deserialize(std::istream & s, SpeciesProperties & obj) {
  s.read((char*)(&obj.molecularWeight), sizeof(double));
  s.read((char*)(&obj.constantSpecificHeat), sizeof(double));
  s.read((char*)(&obj.constantGamma), sizeof(double));
  s.read((char*)(&obj.constantViscosity), sizeof(double));
  s.read((char*)(obj.sutherlandViscosityParameters), sizeof(double)*3);
  s.read((char*)(&obj.constantConductivity), sizeof(double));
  s.read((char*)(obj.sutherlandConductivityParameters), sizeof(double)*3);
  return s;
}

} // end: namespace flame

namespace Loci {
} // end: namespace Loci
