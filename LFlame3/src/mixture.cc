#include <mixture.hh>

#include <libxml/SAX.h>
#include <libxml/xmlschemas.h>

#include <cstring>

#include <sys/stat.h>

namespace flame {

char const * getViscosityModelName(ViscosityModel m) {
  switch(m) {
  case VISCOSITY_CONSTANT:
    return "constant";
    break;
  case VISCOSITY_SUTHERLAND:
    return "sutherland";
    break;
  case VISCOSITY_NONE:
    return "none";
    break;
  }
  return "";
}

char const * getConductivityModelName(ConductivityModel m) {
  switch(m) {
  case CONDUCTIVITY_CONSTANT:
    return "constant";
    break;
  case CONDUCTIVITY_SUTHERLAND:
    return "sutherland";
    break;
  case CONDUCTIVITY_NONE:
    return "none";
    break;
  }
  return "";
}

char const * getDiffusivityModelName(DiffusivityModel m) {
  switch(m) {
  case DIFFUSIVITY_SCHMIDT:
    return "schmidt";
    break;
  case DIFFUSIVITY_NONE:
    return "none";
    break;
  }
  return "";
}

char const * getThermochemistryModelName(ThermochemistryModel m) {
  switch(m) {
  case THERMOCHEMISTRY_CALORICALLY_PERFECT:
    return "caloricallyPerfect";
    break;
  case THERMOCHEMISTRY_NASA9:
    return "caloricallyNASA9";
    break;
  case THERMOCHEMISTRY_NONE:
    return "none";
    break;
  }
  return "";
}

void Mixture::clear() {
  nSpecies = 0;
  for(int i = 0; i < FLAME_MAX_NSPECIES; ++i) {
    clearSpecies(i);
  }
}

void Mixture::clearSpecies(int idx) {
  for(int i = 0; i < FLAME_NAME_MAX_LENGTH; ++i) {
    speciesName[idx][i] = '\0';
  }

  molecularWeight[idx] = 0.0;

  viscosityModel[idx] = VISCOSITY_NONE;
  
  constantViscosity[idx].value = 0.0;
  
  sutherlandViscosity[idx].refTemperature = 0.0;
  sutherlandViscosity[idx].refViscosity = 0.0;
  sutherlandViscosity[idx].refConstant = 0.0;

  conductivityModel[idx] = CONDUCTIVITY_NONE;
  
  constantConductivity[idx].value = 0.0;
  
  sutherlandConductivity[idx].refTemperature = 0.0;
  sutherlandConductivity[idx].refConductivity = 0.0;
  sutherlandConductivity[idx].refConstant = 0.0;

  diffusivityModel[idx] = DIFFUSIVITY_NONE;

  schmidtNumber[idx].value = 0.0;

  thermochemistryModel[idx] = THERMOCHEMISTRY_NONE;

  caloricallyPerfectThermochemistry[idx].specificHeat = 0.0;

  for(int i = 0; i < 3; ++i)
    nasa9Thermochemistry[idx].tRange[i] = 0.0;

  for(int i = 0; i < 18; ++i) {
    nasa9Thermochemistry[idx].cpCoeff[i] = 0.0;
    nasa9Thermochemistry[idx].hCoeff[i] = 0.0;
    nasa9Thermochemistry[idx].sCoeff[i] = 0.0;
  }
}

std::ostream & operator<<(std::ostream & s, Mixture const & mix) {
  s << "mixture: {" << std::endl;
  for(int i = 0; i < mix.nSpecies; ++i) {
    s << "  species: {" << std::endl;
    s << "    name: '" << mix.speciesName[i] << "'" << std::endl;
    s << "    molecularWeight: " << mix.molecularWeight[i] << std::endl;

    s << "    viscosity: ";
    switch(mix.viscosityModel[i]) {
    case VISCOSITY_CONSTANT:
      s << getViscosityModelName(mix.viscosityModel[i])
        << "(" << mix.constantViscosity[i].value << ")";
      break;
    case VISCOSITY_SUTHERLAND:
      s << getViscosityModelName(mix.viscosityModel[i])
        << "("
        << "refTemperature: " << mix.sutherlandViscosity[i].refTemperature << ", "
        << "refViscosity: " << mix.sutherlandViscosity[i].refViscosity << ", "
        << "refConstant: " << mix.sutherlandViscosity[i].refConstant
        << ")";
      break;
    case VISCOSITY_NONE:
      s << getViscosityModelName(mix.viscosityModel[i]);
      break;
    }
    s << std::endl;

    s << "    conductivity: ";
    switch(mix.conductivityModel[i]) {
    case CONDUCTIVITY_CONSTANT:
      s << getConductivityModelName(mix.conductivityModel[i])
        << "(" << mix.constantConductivity[i].value << ")";
      break;
    case CONDUCTIVITY_SUTHERLAND:
      s << getConductivityModelName(mix.conductivityModel[i])
        << "("
        << "refTemperature: " << mix.sutherlandConductivity[i].refTemperature << ", "
        << "refConductivity: " << mix.sutherlandConductivity[i].refConductivity << ", "
        << "refConstant: " << mix.sutherlandConductivity[i].refConstant
        << ")";
      break;
    case CONDUCTIVITY_NONE:
      s << getConductivityModelName(mix.conductivityModel[i]);
      break;
    }
    s << std::endl;

    s << "    diffusivity: ";
    switch(mix.diffusivityModel[i]) {
    case DIFFUSIVITY_SCHMIDT:
      s << getDiffusivityModelName(mix.diffusivityModel[i])
        << "(" << mix.schmidtNumber[i].value << ")";
      break;
    case DIFFUSIVITY_NONE:
      s << getDiffusivityModelName(mix.diffusivityModel[i]);
      break;
    }

    s << "    thermochemistry: ";
    switch(mix.thermochemistryModel[i]) {
    case THERMOCHEMISTRY_CALORICALLY_PERFECT:
      s << getThermochemistryModelName(mix.thermochemistryModel[i])
        << "("
        << "specificHeat: "
        << mix.caloricallyPerfectThermochemistry[i].specificHeat
        << ")";
      break;
    case THERMOCHEMISTRY_NASA9:
      s << getThermochemistryModelName(mix.thermochemistryModel[i])
        << "(";
      s << "tRange=[";
      for(int j = 0; j < 3; ++j) {
        s << mix.nasa9Thermochemistry[i].tRange[j] << " ";
      }
      s << "], ";
      s << "cpCoeff=[";
      for(int j = 0; j < 18; ++j) {
        s << mix.nasa9Thermochemistry[i].cpCoeff[j] << " ";
      }
      s << "], ";
      s << "hCoeff=[";
      for(int j = 0; j < 18; ++j) {
        s << mix.nasa9Thermochemistry[i].hCoeff[j] << " ";
      }
      s << "], ";
      s << "sCoeff=[";
      for(int j = 0; j < 18; ++j) {
        s << mix.nasa9Thermochemistry[i].sCoeff[j] << " ";
      }
      s << "]";
      s  << ")";
      break;
    case THERMOCHEMISTRY_NONE:
      s << getThermochemistryModelName(mix.thermochemistryModel[i]);
      break;
    }
    s << std::endl;

    s << "  }" << std::endl;
  }
  s << "}" << std::endl;

  return s;
}

// ==============================================================================================

struct Attribute {
  std::string prefix, URI, name, value;
};

struct Element {
  std::string prefix, URI, name;
  int nAttributes;
  std::vector<Attribute> attributes;
};

enum ParserFSM {
  MIXTURE,
  MIXTURE_SPECIES,
  MIXTURE_SPECIES_NAME,
  MIXTURE_SPECIES_MOLECULAR_WEIGHT,
  MIXTURE_SPECIES_VISCOSITY,
  MIXTURE_SPECIES_VISCOSITY_CONSTANT,
  MIXTURE_SPECIES_VISCOSITY_SUTHERLAND,
  MIXTURE_SPECIES_VISCOSITY_SUTHERLAND_REF_TEMPERATURE,
  MIXTURE_SPECIES_VISCOSITY_SUTHERLAND_REF_VISCOSITY,
  MIXTURE_SPECIES_VISCOSITY_SUTHERLAND_REF_CONSTANT,
  MIXTURE_SPECIES_CONDUCTIVITY,
  MIXTURE_SPECIES_CONDUCTIVITY_CONSTANT,
  MIXTURE_SPECIES_CONDUCTIVITY_SUTHERLAND,
  MIXTURE_SPECIES_CONDUCTIVITY_SUTHERLAND_REF_TEMPERATURE,
  MIXTURE_SPECIES_CONDUCTIVITY_SUTHERLAND_REF_CONDUCTIVITY,
  MIXTURE_SPECIES_CONDUCTIVITY_SUTHERLAND_REF_CONSTANT,
  MIXTURE_SPECIES_DIFFUSIVITY,
  MIXTURE_SPECIES_DIFFUSIVITY_SCHMIDT_NUMBER,
  MIXTURE_SPECIES_THERMOCHEMISTRY,
  MIXTURE_SPECIES_THERMOCHEMISTRY_SPECIFIC_HEAT,
  MIXTURE_SPECIES_THERMOCHEMISTRY_NASA9_POLYNOMIAL,
  MIXTURE_SPECIES_THERMOCHEMISTRY_NASA9_POLYNOMIAL_TEMPERATURE_RANGES,
  MIXTURE_SPECIES_THERMOCHEMISTRY_NASA9_POLYNOMIAL_COEFFICIENTS,
  ParserFSM_NONE
};


class MixtureParserData {
  std::vector<Element> elementStack;
  std::stack<ParserFSM> fsm;
  Mixture mixture;

  std::string charData;
  int speciesIndex;

public:
  MixtureParserData() {
    init();
  }

  Mixture const & getMixture() const {
    return mixture;
  }

  void init() {
    elementStack.clear();
    fsm = std::stack<ParserFSM>();
    mixture.clear();
    charData.clear();
    speciesIndex = -1;
  }

  void pushElement(Element const & elem) {
    elementStack.push_back(elem);

    std::stringstream ss;
    for(auto const & e : elementStack) {
      ss << '/' << e.name;
    }
    std::string path = ss.str();

    if(path == "/mixture") {
      fsm.push(MIXTURE);
    } else if(path == "/mixture/species") {
      fsm.push(MIXTURE_SPECIES);
      speciesIndex++;
      mixture.clearSpecies(speciesIndex);
    } else if(path == "/mixture/species/name") {
      fsm.push(MIXTURE_SPECIES_NAME);
      charData.clear();
    } else if(path == "/mixture/species/molecularWeight") {
      fsm.push(MIXTURE_SPECIES_MOLECULAR_WEIGHT);
      charData.clear();
    } else if(path == "/mixture/species/viscosity") {
      fsm.push(MIXTURE_SPECIES_VISCOSITY);
    } else if(path == "/mixture/species/viscosity/constant") {
      fsm.push(MIXTURE_SPECIES_VISCOSITY_CONSTANT);
      charData.clear();
    } else if(path == "/mixture/species/viscosity/sutherland") {
      fsm.push(MIXTURE_SPECIES_VISCOSITY_SUTHERLAND);
    } else if(path == "/mixture/species/viscosity/sutherland/refTemperature") {
      fsm.push(MIXTURE_SPECIES_VISCOSITY_SUTHERLAND_REF_TEMPERATURE);
      charData.clear();
    } else if(path == "/mixture/species/viscosity/sutherland/refViscosity") {
      fsm.push(MIXTURE_SPECIES_VISCOSITY_SUTHERLAND_REF_VISCOSITY);
      charData.clear();
    } else if(path == "/mixture/species/viscosity/sutherland/refConstant") {
      fsm.push(MIXTURE_SPECIES_VISCOSITY_SUTHERLAND_REF_CONSTANT);
      charData.clear();
    } else if(path == "/mixture/species/conductivity") {
      fsm.push(MIXTURE_SPECIES_CONDUCTIVITY);
    } else if(path == "/mixture/species/conductivity/constant") {
      fsm.push(MIXTURE_SPECIES_CONDUCTIVITY_CONSTANT);
      charData.clear();
    } else if(path == "/mixture/species/conductivity/sutherland") {
      fsm.push(MIXTURE_SPECIES_CONDUCTIVITY_SUTHERLAND);
    } else if(path == "/mixture/species/conductivity/sutherland/refTemperature") {
      fsm.push(MIXTURE_SPECIES_CONDUCTIVITY_SUTHERLAND_REF_TEMPERATURE);
      charData.clear();
    } else if(path == "/mixture/species/conductivity/sutherland/refConductivity") {
      fsm.push(MIXTURE_SPECIES_CONDUCTIVITY_SUTHERLAND_REF_CONDUCTIVITY);
      charData.clear();
    } else if(path == "/mixture/species/conductivity/sutherland/refConstant") {
      fsm.push(MIXTURE_SPECIES_CONDUCTIVITY_SUTHERLAND_REF_CONSTANT);
      charData.clear();
    } else if(path == "/mixture/species/diffusivity") {
      fsm.push(MIXTURE_SPECIES_DIFFUSIVITY);
    } else if(path == "/mixture/species/diffusivity/schmidtNumber") {
      fsm.push(MIXTURE_SPECIES_DIFFUSIVITY_SCHMIDT_NUMBER);
      charData.clear();
    } else if(path == "/mixture/species/thermochemistry") {
      fsm.push(MIXTURE_SPECIES_THERMOCHEMISTRY);
    } else if(path == "/mixture/species/thermochemistry/specificHeat") {
      fsm.push(MIXTURE_SPECIES_THERMOCHEMISTRY_SPECIFIC_HEAT);
      charData.clear();
    } else if(path == "/mixture/species/thermochemistry/NASA9Polynomial") {
      fsm.push(MIXTURE_SPECIES_THERMOCHEMISTRY_NASA9_POLYNOMIAL);
    } else if(path == "/mixture/species/thermochemistry/NASA9Polynomial/temperatureRanges") {
      fsm.push(MIXTURE_SPECIES_THERMOCHEMISTRY_NASA9_POLYNOMIAL_TEMPERATURE_RANGES);
      charData.clear();
    } else if(path == "/mixture/species/thermochemistry/NASA9Polynomial/coefficients") {
      fsm.push(MIXTURE_SPECIES_THERMOCHEMISTRY_NASA9_POLYNOMIAL_COEFFICIENTS);
      charData.clear();
    } else {
      std::cerr << "Unprocessed path: " << path << std::endl;
    }
  }

  void popElement(std::string const & localName) {
    switch(fsm.top()) {
    case MIXTURE:
      break;
    case MIXTURE_SPECIES:
      mixture.nSpecies = speciesIndex+1;
      break;
    case MIXTURE_SPECIES_NAME:
      std::strncpy(mixture.speciesName[speciesIndex], charData.c_str(), FLAME_NAME_MAX_LENGTH-1);
      mixture.speciesName[speciesIndex][FLAME_NAME_MAX_LENGTH-1] = '\0';
      break;
    case MIXTURE_SPECIES_MOLECULAR_WEIGHT:
    {
      std::stringstream ss(charData);
      ss >> mixture.molecularWeight[speciesIndex];
    }
      break;
    case MIXTURE_SPECIES_VISCOSITY:
      break;
    case MIXTURE_SPECIES_VISCOSITY_CONSTANT:
    {
      mixture.viscosityModel[speciesIndex] = VISCOSITY_CONSTANT;
      std::stringstream ss(charData);
      ss >> mixture.constantViscosity[speciesIndex].value;
    }
      break;
    case MIXTURE_SPECIES_VISCOSITY_SUTHERLAND:
      mixture.viscosityModel[speciesIndex] = VISCOSITY_SUTHERLAND;
      break;
    case MIXTURE_SPECIES_VISCOSITY_SUTHERLAND_REF_TEMPERATURE:
    {
      std::stringstream ss(charData);
      ss >> mixture.sutherlandViscosity[speciesIndex].refTemperature;
    }
      break;
    case MIXTURE_SPECIES_VISCOSITY_SUTHERLAND_REF_VISCOSITY:
    {
      std::stringstream ss(charData);
      ss >> mixture.sutherlandViscosity[speciesIndex].refViscosity;
    }
      break;
    case MIXTURE_SPECIES_VISCOSITY_SUTHERLAND_REF_CONSTANT:
    {
      std::stringstream ss(charData);
      ss >> mixture.sutherlandViscosity[speciesIndex].refConstant;
    }
      break;
    case MIXTURE_SPECIES_CONDUCTIVITY:
      break;
    case MIXTURE_SPECIES_CONDUCTIVITY_CONSTANT:
    {
      mixture.conductivityModel[speciesIndex] = CONDUCTIVITY_CONSTANT;
      std::stringstream ss(charData);
      ss >> mixture.constantConductivity[speciesIndex].value;
    }
      break;
    case MIXTURE_SPECIES_CONDUCTIVITY_SUTHERLAND:
      mixture.conductivityModel[speciesIndex] = CONDUCTIVITY_SUTHERLAND;
      break;
    case MIXTURE_SPECIES_CONDUCTIVITY_SUTHERLAND_REF_TEMPERATURE:
    {
      std::stringstream ss(charData);
      ss >> mixture.sutherlandConductivity[speciesIndex].refTemperature;
    }
      break;
    case MIXTURE_SPECIES_CONDUCTIVITY_SUTHERLAND_REF_CONDUCTIVITY:
    {
      std::stringstream ss(charData);
      ss >> mixture.sutherlandConductivity[speciesIndex].refConductivity;
    }
      break;
    case MIXTURE_SPECIES_CONDUCTIVITY_SUTHERLAND_REF_CONSTANT:
    {
      mixture.diffusivityModel[speciesIndex] = DIFFUSIVITY_SCHMIDT;
      std::stringstream ss(charData);
      ss >> mixture.sutherlandConductivity[speciesIndex].refConstant;
    }
      break;
    case MIXTURE_SPECIES_DIFFUSIVITY:
      break;
    case MIXTURE_SPECIES_DIFFUSIVITY_SCHMIDT_NUMBER:
    {
      std::stringstream ss(charData);
      ss >> mixture.schmidtNumber[speciesIndex].value;
    }
      break;
    case MIXTURE_SPECIES_THERMOCHEMISTRY:
      break;
    case MIXTURE_SPECIES_THERMOCHEMISTRY_SPECIFIC_HEAT:
    {
      mixture.thermochemistryModel[speciesIndex] = THERMOCHEMISTRY_CALORICALLY_PERFECT;
      std::stringstream ss(charData);
      ss >> mixture.caloricallyPerfectThermochemistry[speciesIndex].specificHeat;
    }
      break;
    case MIXTURE_SPECIES_THERMOCHEMISTRY_NASA9_POLYNOMIAL:
      mixture.thermochemistryModel[speciesIndex] = THERMOCHEMISTRY_NASA9;
      break;
    case MIXTURE_SPECIES_THERMOCHEMISTRY_NASA9_POLYNOMIAL_TEMPERATURE_RANGES:
    {
      std::stringstream ss(charData);
      for(int i = 0; i < 3; ++i) {
        ss >> mixture.nasa9Thermochemistry[speciesIndex].tRange[i];
      }
    }
      break;
    case MIXTURE_SPECIES_THERMOCHEMISTRY_NASA9_POLYNOMIAL_COEFFICIENTS:
    {
      std::stringstream ss;
      for(int i = 0; i < 18; ++i) {
        ss >> mixture.nasa9Thermochemistry[speciesIndex].cpCoeff[i];
      }
      for(int i = 0; i < 2; ++i) {
        int j = i*9;
        
        mixture.nasa9Thermochemistry[speciesIndex].hCoeff[j] = -mixture.nasa9Thermochemistry[speciesIndex].cpCoeff[j];
        mixture.nasa9Thermochemistry[speciesIndex].hCoeff[j+1] = mixture.nasa9Thermochemistry[speciesIndex].cpCoeff[j+1];
        mixture.nasa9Thermochemistry[speciesIndex].hCoeff[j+2] = mixture.nasa9Thermochemistry[speciesIndex].cpCoeff[j+2];
        mixture.nasa9Thermochemistry[speciesIndex].hCoeff[j+3] = mixture.nasa9Thermochemistry[speciesIndex].cpCoeff[j+3]/2.0;
        mixture.nasa9Thermochemistry[speciesIndex].hCoeff[j+4] = mixture.nasa9Thermochemistry[speciesIndex].cpCoeff[j+4]/3.0;
        mixture.nasa9Thermochemistry[speciesIndex].hCoeff[j+5] = mixture.nasa9Thermochemistry[speciesIndex].cpCoeff[j+5]/4.0;
        mixture.nasa9Thermochemistry[speciesIndex].hCoeff[j+6] = mixture.nasa9Thermochemistry[speciesIndex].cpCoeff[j+6]/5.0;
        mixture.nasa9Thermochemistry[speciesIndex].hCoeff[j+7] = mixture.nasa9Thermochemistry[speciesIndex].cpCoeff[j+7];
        mixture.nasa9Thermochemistry[speciesIndex].hCoeff[j+8] = mixture.nasa9Thermochemistry[speciesIndex].cpCoeff[j+8];

        mixture.nasa9Thermochemistry[speciesIndex].sCoeff[j] = -mixture.nasa9Thermochemistry[speciesIndex].cpCoeff[j]/2.0;
        mixture.nasa9Thermochemistry[speciesIndex].sCoeff[j+1] = -mixture.nasa9Thermochemistry[speciesIndex].cpCoeff[j+1];
        mixture.nasa9Thermochemistry[speciesIndex].sCoeff[j+2] = mixture.nasa9Thermochemistry[speciesIndex].cpCoeff[j+2];
        mixture.nasa9Thermochemistry[speciesIndex].sCoeff[j+3] = mixture.nasa9Thermochemistry[speciesIndex].cpCoeff[j+3];
        mixture.nasa9Thermochemistry[speciesIndex].sCoeff[j+4] = mixture.nasa9Thermochemistry[speciesIndex].cpCoeff[j+4]/2.0;
        mixture.nasa9Thermochemistry[speciesIndex].sCoeff[j+5] = mixture.nasa9Thermochemistry[speciesIndex].cpCoeff[j+5]/3.0;
        mixture.nasa9Thermochemistry[speciesIndex].sCoeff[j+6] = mixture.nasa9Thermochemistry[speciesIndex].cpCoeff[j+6]/4.0;
        mixture.nasa9Thermochemistry[speciesIndex].sCoeff[j+7] = mixture.nasa9Thermochemistry[speciesIndex].cpCoeff[j+7];
        mixture.nasa9Thermochemistry[speciesIndex].sCoeff[j+8] = mixture.nasa9Thermochemistry[speciesIndex].cpCoeff[j+8];
      }
    }
      break;
    }

    elementStack.pop_back();
    fsm.pop();
  }

  void setValue(std::string const & value) {
    switch(fsm.top()) {
    case MIXTURE:
      break;
    case MIXTURE_SPECIES:
      break;
    case MIXTURE_SPECIES_NAME:
      charData += value;
      break;
    case MIXTURE_SPECIES_MOLECULAR_WEIGHT:
      charData += value;
      break;
    case MIXTURE_SPECIES_VISCOSITY:
      break;
    case MIXTURE_SPECIES_VISCOSITY_CONSTANT:
      charData += value;
      break;
    case MIXTURE_SPECIES_VISCOSITY_SUTHERLAND:
      break;
    case MIXTURE_SPECIES_VISCOSITY_SUTHERLAND_REF_TEMPERATURE:
      charData += value;
      break;
    case MIXTURE_SPECIES_VISCOSITY_SUTHERLAND_REF_VISCOSITY:
      charData += value;
      break;
    case MIXTURE_SPECIES_VISCOSITY_SUTHERLAND_REF_CONSTANT:
      charData += value;
      break;
    case MIXTURE_SPECIES_CONDUCTIVITY:
      break;
    case MIXTURE_SPECIES_CONDUCTIVITY_CONSTANT:
      charData += value;
      break;
    case MIXTURE_SPECIES_CONDUCTIVITY_SUTHERLAND:
      break;
    case MIXTURE_SPECIES_CONDUCTIVITY_SUTHERLAND_REF_TEMPERATURE:
      charData += value;
      break;
    case MIXTURE_SPECIES_CONDUCTIVITY_SUTHERLAND_REF_CONDUCTIVITY:
      charData += value;
      break;
    case MIXTURE_SPECIES_CONDUCTIVITY_SUTHERLAND_REF_CONSTANT:
      charData += value;
      break;
    case MIXTURE_SPECIES_DIFFUSIVITY:
      break;
    case MIXTURE_SPECIES_DIFFUSIVITY_SCHMIDT_NUMBER:
      charData += value;
      break;
    case MIXTURE_SPECIES_THERMOCHEMISTRY:
      break;
    case MIXTURE_SPECIES_THERMOCHEMISTRY_SPECIFIC_HEAT:
      charData += value;
      break;
    case MIXTURE_SPECIES_THERMOCHEMISTRY_NASA9_POLYNOMIAL:
      break;
    case MIXTURE_SPECIES_THERMOCHEMISTRY_NASA9_POLYNOMIAL_TEMPERATURE_RANGES:
      charData += value;
      break;
    case MIXTURE_SPECIES_THERMOCHEMISTRY_NASA9_POLYNOMIAL_COEFFICIENTS:
      charData += value;
      break;
    }
  }
};

void libxmlGetAttributes(
  int const nbAttributes,
  xmlChar const ** attributes,
  std::vector<Attribute> & att
) {
  att.resize(nbAttributes);

  if(attributes != nullptr) {
    for(int i = 0; i < nbAttributes; ++i) {
      int const j = i*5;
      if(attributes[j+1] != nullptr) {
        att[i].prefix = std::string(
          (char const *)attributes[j+1],
          xmlStrlen(attributes[j+1])
        );
      } else {
        att[i].prefix.clear();
      }

      if(attributes[j+2] != nullptr) {
        att[i].URI = std::string(
          (char const *)attributes[j+2],
          xmlStrlen(attributes[j+2])
        );
      } else {
        att[i].URI.clear();
      }

      att[i].name = std::string(
        (char const *)attributes[j],
        xmlStrlen(attributes[j])
      );

      att[i].value = std::string(
        (char const *)attributes[j+3],
        attributes[j+4]-attributes[j+3]
      );
    }
  }
}

void onStartElementNs(
  void * ctx,
  xmlChar const * localName,
  xmlChar const * prefix,
  xmlChar const * URI,
  int nbNamespaces,
  xmlChar const ** namespaces,
  int nbAttributes,
  int nbDefaulted,
  xmlChar const ** attributes
) {
  MixtureParserData * data = (MixtureParserData *)ctx;

  if(xmlStrEqual((xmlChar const *)"http://flame-mixture", URI)) {
    Element elem;
    elem.name = std::string(
      reinterpret_cast<char const *>(localName),
      xmlStrlen(localName)
    );
    elem.nAttributes = nbAttributes;
    libxmlGetAttributes(nbAttributes, attributes, elem.attributes);

    data->pushElement(elem);
  } else {
    std::cerr << "Unknown URI: " << URI << std::endl;
  }
}

void onEndElementNs(
  void * ctx,
  xmlChar const * localName,
  xmlChar const * prefix,
  xmlChar const * URI
) {
  MixtureParserData * data = (MixtureParserData *)ctx;
  
  if(xmlStrEqual((xmlChar const *)"http://flame-mixture", URI)) {
    std::string sLocalName(
      reinterpret_cast<char const *>(localName),
      xmlStrlen(localName)
    );
    data->popElement(sLocalName);
  } else {
    std::cerr << "Unknown URI: " << URI << std::endl;
  }
}

void onCharacters(void * ctx, xmlChar const * ch, int len) {
  MixtureParserData * data = (MixtureParserData *)ctx;

  if(len > 0) {
    std::string value(reinterpret_cast<char const *>(ch), len);
    data->setValue(value);
  }
}

void schemaErrorHandler(void * ctx, char const * msg, ...) {
  char buffer[4196];
  va_list args;
  va_start(args, msg);
  vsprintf(buffer, msg, args);
  va_end(args);
  std::cerr << "Validation error: " << buffer << std::endl;
  std::exit(1);
}

void schemaWarningHandler(void * ctx, char const * msg, ...) {
  char buffer[4196];
  va_list args;
  va_start(args, msg);
  vsprintf(buffer, msg, args);
  va_end(args);
  std::cerr << "Validation warning: " << buffer << std::endl;
  std::exit(1);
}

int parseFromXML(
  std::string const & mixtureFile, Mixture & mixture, std::ostream & msg,
  std::string const & xsdDir
) {
  int error = 0;

  xmlSchemaParserCtxtPtr schemaCtxt = nullptr;
  xmlSchemaPtr schema = nullptr;
  xmlSchemaValidCtxtPtr validSchema = nullptr;
  MixtureParserData * parserData = nullptr;
  xmlParserInputBufferPtr buffer = nullptr;

  try {
    if(!xmlHasFeature(XML_WITH_SCHEMAS)) {
      msg << "XML library is not compiled with schema support";
      throw 1;
    }

    std::string schemaFileName = xsdDir + std::string("/flame-mixture.xsd");

    struct stat statbuf;
    if(stat(mixtureFile.c_str(), &statbuf) == 0) {
      if(!S_ISREG(statbuf.st_mode)) {
        msg << "File '" << mixtureFile << "' is not a regular file";
        throw 2;
      }
    } else {
      msg << "File '" << mixtureFile << "' does not exist";
      throw 3;
    }

    if(stat(schemaFileName.c_str(), &statbuf) == 0) {
      if(!S_ISREG(statbuf.st_mode)) {
        msg << "File '" << schemaFileName << "' is not a regular file";
        throw 4;
      }
    } else {
      msg << "File '" << schemaFileName << "' does not exist";
      throw 5;
    }

    schemaCtxt = xmlSchemaNewParserCtxt(schemaFileName.c_str());
    if(schemaCtxt == nullptr) {
      msg << "Could not create schema context";
      throw 6;
    }

    schema = xmlSchemaParse(schemaCtxt);
    if(schema == nullptr) {
      msg << "Could not parse schema";
      throw 7;
    }

    validSchema = xmlSchemaNewValidCtxt(schema);
    if(validSchema == nullptr) {
      msg << "Could not create valid schema context";
      throw 8;
    }

    parserData = new MixtureParserData;

    xmlSAXHandler handler;
    std::memset(&handler, 0, sizeof(handler));

    handler.initialized = XML_SAX2_MAGIC;
    handler.startElementNs = onStartElementNs;
    handler.endElementNs = onEndElementNs;
    handler.characters = onCharacters;

    buffer = xmlParserInputBufferCreateFilename(
      mixtureFile.c_str(), XML_CHAR_ENCODING_NONE
    );
    if(buffer == nullptr) {
      msg << "Could not create input buffer for file name: '"
          << mixtureFile << ";";
      throw 9;
    }

    xmlSchemaSetValidErrors(validSchema, schemaErrorHandler, schemaWarningHandler, NULL);
    xmlSchemaValidateSetFilename(validSchema, mixtureFile.c_str());
    int ret = xmlSchemaValidateStream(validSchema, buffer, XML_CHAR_ENCODING_NONE, &handler, (void *)parserData);
    if(ret == 0) {
      mixture = parserData->getMixture();
    } else if (ret > 0) {
      msg << mixtureFile << " fails to validate";
      throw 10;
    } else {
      msg << mixtureFile << " validation generated an internal error";
      throw 11;
    }
  } catch(int & err) {
    error = err;
  }

  if(schemaCtxt) xmlSchemaFreeParserCtxt(schemaCtxt);
  if(schema) xmlSchemaFree(schema);
  if(validSchema) xmlSchemaFreeValidCtxt(validSchema);
  //if(buffer) xmlFreeParserInputBuffer(buffer); //segfaults for some reason
  if(parserData) delete parserData;

  return error;
}

} // end: namespace flame

