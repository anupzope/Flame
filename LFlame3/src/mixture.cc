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

char const * getThermochemistryModelName(ThermochemistryModel m) {
  switch(m) {
  case THERMOCHEMISTRY_CALORICALLY_PERFECT:
    return "caloricallyPerfect";
    break;
  case THERMOCHEMISTRY_NONE:
    return "none";
    break;
  }
  return "";
}

void Mixture::clear() {
  nSpecies = 0;
}

std::ostream & operator<<(std::ostream & s, Mixture const & mix) {
  s << "mixture: {" << std::endl;
  s << "  species: {" << std::endl;
  for(int i = 0; i < mix.nSpecies; ++i) {
    s << "    component: {" << std::endl;
    s << "      name: '" << mix.speciesName[i] << "'" << std::endl;
    s << "      molecularWeight: " << mix.molecularWeight[i] << std::endl;

    s << "      viscosity: ";
    switch(mix.viscosityModel[i]) {
    case VISCOSITY_CONSTANT:
      s << getViscosityModelName(mix.viscosityModel[i])
        << "(" << mix.constantViscosity[i].viscosity << ")";
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

    s << "      conductivity: ";
    switch(mix.conductivityModel[i]) {
    case CONDUCTIVITY_CONSTANT:
      s << getConductivityModelName(mix.conductivityModel[i])
        << "(" << mix.constantConductivity[i].conductivity << ")";
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

    s << "      thermochemistry: ";
    switch(mix.thermochemistryModel[i]) {
    case THERMOCHEMISTRY_CALORICALLY_PERFECT:
      s << getThermochemistryModelName(mix.thermochemistryModel[i])
        << "("
        << "specificHeat: "
        << mix.caloricallyPerfectThermochemistry[i].specificHeat
        << ")";
      break;
    case THERMOCHEMISTRY_NONE:
      s << getThermochemistryModelName(mix.thermochemistryModel[i]);
      break;
    }
    s << std::endl;

    s << "    }" << std::endl;
  }
  s << "  }" << std::endl;
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
  MIXTURE_SPECIES_MOLECULARWEIGHT,
  MIXTURE_SPECIES_CONSTANTVISCOSITY,
  MIXTURE_SPECIES_SUTHERLANDVISCOSITY,
  MIXTURE_SPECIES_SUTHERLANDVISCOSITY_REFTEMPERATURE,
  MIXTURE_SPECIES_SUTHERLANDVISCOSITY_REFVISCOSITY,
  MIXTURE_SPECIES_SUTHERLANDVISCOSITY_REFCONSTANT,
  MIXTURE_SPECIES_CONSTANTCONDUCTIVITY,
  MIXTURE_SPECIES_SUTHERLANDCONDUCTIVITY,
  MIXTURE_SPECIES_SUTHERLANDCONDUCTIVITY_REFTEMPERATURE,
  MIXTURE_SPECIES_SUTHERLANDCONDUCTIVITY_REFCONDUCTIVITY,
  MIXTURE_SPECIES_SUTHERLANDCONDUCTIVITY_REFCONSTANT,
  MIXTURE_SPECIES_CALORICALLYPERFECTGAS,
  MIXTURE_SPECIES_CALORICALLYPERFECTGAS_SPECIFICHEAT,
  ParserFSM_NONE
};


class MixtureParserData {
  std::vector<Element> elementStack;
  std::stack<ParserFSM> fsm;
  Mixture mixture;

  std::string charData;
  int speciesIndex;
  double molecularWeight;
  ViscosityModel viscosityModel;
  ConductivityModel conductivityModel;
  ThermochemistryModel thermochemistryModel;
  ConstantViscosityModel constantViscosity;
  SutherlandViscosityModel sutherlandViscosity;
  ConstantConductivityModel constantConductivity;
  SutherlandConductivityModel sutherlandConductivity;
  CaloricallyPerfectThermochemistryModel caloricallyPerfectThermochemistryModel;

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
    } else if(path == "/mixture/species/name") {
      fsm.push(MIXTURE_SPECIES_NAME);
      charData.clear();
    } else if(path == "/mixture/species/molecularWeight") {
      fsm.push(MIXTURE_SPECIES_MOLECULARWEIGHT);
      charData.clear();
    } else if(path == "/mixture/species/constantViscosity") {
      fsm.push(MIXTURE_SPECIES_CONSTANTVISCOSITY);
      charData.clear();
    } else if(path == "/mixture/species/sutherlandViscosity") {
      fsm.push(MIXTURE_SPECIES_SUTHERLANDVISCOSITY);
    } else if(path == "/mixture/species/sutherlandViscosity/refTemperature") {
      fsm.push(MIXTURE_SPECIES_SUTHERLANDVISCOSITY_REFTEMPERATURE);
      charData.clear();
    } else if(path == "/mixture/species/sutherlandViscosity/refViscosity") {
      fsm.push(MIXTURE_SPECIES_SUTHERLANDVISCOSITY_REFVISCOSITY);
      charData.clear();
    } else if(path == "/mixture/species/sutherlandViscosity/refConstant") {
      fsm.push(MIXTURE_SPECIES_SUTHERLANDVISCOSITY_REFCONSTANT);
      charData.clear();
    } else if(path == "/mixture/species/constantConductivity") {
      fsm.push(MIXTURE_SPECIES_CONSTANTCONDUCTIVITY);
      charData.clear();
    } else if(path == "/mixture/species/sutherlandConductivity") {
      fsm.push(MIXTURE_SPECIES_SUTHERLANDCONDUCTIVITY);
    } else if(path == "/mixture/species/sutherlandConductivity/refTemperature") {
      fsm.push(MIXTURE_SPECIES_SUTHERLANDCONDUCTIVITY_REFTEMPERATURE);
      charData.clear();
    } else if(path == "/mixture/species/sutherlandConductivity/refConductivity") {
      fsm.push(MIXTURE_SPECIES_SUTHERLANDCONDUCTIVITY_REFCONDUCTIVITY);
      charData.clear();
    } else if(path == "/mixture/species/sutherlandConductivity/refConstant") {
      fsm.push(MIXTURE_SPECIES_SUTHERLANDCONDUCTIVITY_REFCONSTANT);
      charData.clear();
    } else if(path == "/mixture/species/caloricallyPerfectGas") {
      fsm.push(MIXTURE_SPECIES_CALORICALLYPERFECTGAS);
    } else if(path == "/mixture/species/caloricallyPerfectGas/specificHeat") {
      fsm.push(MIXTURE_SPECIES_CALORICALLYPERFECTGAS_SPECIFICHEAT);
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
    case MIXTURE_SPECIES_MOLECULARWEIGHT:
    {
      std::stringstream ss(charData);
      ss >> mixture.molecularWeight[speciesIndex];
    }
      break;
    case MIXTURE_SPECIES_CONSTANTVISCOSITY:
    {
      mixture.viscosityModel[speciesIndex] = VISCOSITY_CONSTANT;
      std::stringstream ss(charData);
      ss >> mixture.constantViscosity[speciesIndex].viscosity;
    }
      break;
    case MIXTURE_SPECIES_SUTHERLANDVISCOSITY:
      mixture.viscosityModel[speciesIndex] = VISCOSITY_SUTHERLAND;
      break;
    case MIXTURE_SPECIES_SUTHERLANDVISCOSITY_REFTEMPERATURE:
    {
      std::stringstream ss(charData);
      ss >> mixture.sutherlandViscosity[speciesIndex].refTemperature;
    }
      break;
    case MIXTURE_SPECIES_SUTHERLANDVISCOSITY_REFVISCOSITY:
    {
      std::stringstream ss(charData);
      ss >> mixture.sutherlandViscosity[speciesIndex].refViscosity;
    }
      break;
    case MIXTURE_SPECIES_SUTHERLANDVISCOSITY_REFCONSTANT:
    {
      std::stringstream ss(charData);
      ss >> mixture.sutherlandViscosity[speciesIndex].refConstant;
    }
      break;
    case MIXTURE_SPECIES_CONSTANTCONDUCTIVITY:
    {
      mixture.conductivityModel[speciesIndex] = CONDUCTIVITY_CONSTANT;
      std::stringstream ss(charData);
      ss >> mixture.constantConductivity[speciesIndex].conductivity;
    }
      break;
    case MIXTURE_SPECIES_SUTHERLANDCONDUCTIVITY:
      mixture.conductivityModel[speciesIndex] = CONDUCTIVITY_SUTHERLAND;
      break;
    case MIXTURE_SPECIES_SUTHERLANDCONDUCTIVITY_REFTEMPERATURE:
    {
      std::stringstream ss(charData);
      ss >> mixture.sutherlandConductivity[speciesIndex].refTemperature;
    }
      break;
    case MIXTURE_SPECIES_SUTHERLANDCONDUCTIVITY_REFCONDUCTIVITY:
    {
      std::stringstream ss(charData);
      ss >> mixture.sutherlandConductivity[speciesIndex].refConductivity;
    }
      break;
    case MIXTURE_SPECIES_SUTHERLANDCONDUCTIVITY_REFCONSTANT:
    {
      std::stringstream ss(charData);
      ss >> mixture.sutherlandConductivity[speciesIndex].refConstant;
    }
      break;
    case MIXTURE_SPECIES_CALORICALLYPERFECTGAS:
      mixture.thermochemistryModel[speciesIndex] = THERMOCHEMISTRY_CALORICALLY_PERFECT;
      break;
    case MIXTURE_SPECIES_CALORICALLYPERFECTGAS_SPECIFICHEAT:
    {
      std::stringstream ss(charData);
      ss >> mixture.caloricallyPerfectThermochemistry[speciesIndex].specificHeat;
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
    case MIXTURE_SPECIES_MOLECULARWEIGHT:
      charData += value;
      break;
    case MIXTURE_SPECIES_CONSTANTVISCOSITY:
      charData += value;
      break;
    case MIXTURE_SPECIES_SUTHERLANDVISCOSITY:
      break;
    case MIXTURE_SPECIES_SUTHERLANDVISCOSITY_REFTEMPERATURE:
      charData += value;
      break;
    case MIXTURE_SPECIES_SUTHERLANDVISCOSITY_REFVISCOSITY:
      charData += value;
      break;
    case MIXTURE_SPECIES_SUTHERLANDVISCOSITY_REFCONSTANT:
      charData += value;
      break;
    case MIXTURE_SPECIES_CONSTANTCONDUCTIVITY:
      charData += value;
      break;
    case MIXTURE_SPECIES_SUTHERLANDCONDUCTIVITY:
      break;
    case MIXTURE_SPECIES_SUTHERLANDCONDUCTIVITY_REFTEMPERATURE:
      charData += value;
      break;
    case MIXTURE_SPECIES_SUTHERLANDCONDUCTIVITY_REFCONDUCTIVITY:
      charData += value;
      break;
    case MIXTURE_SPECIES_SUTHERLANDCONDUCTIVITY_REFCONSTANT:
      charData += value;
      break;
    case MIXTURE_SPECIES_CALORICALLYPERFECTGAS:
      break;
    case MIXTURE_SPECIES_CALORICALLYPERFECTGAS_SPECIFICHEAT:
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
  std::string const & mixtureFile, Mixture & mixture, std::ostream & msg
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

    std::string schemaFileName = std::string(FLAME_DATA_DIR)
      + std::string("/flame-mixture.xsd");

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
  } catch(...) {
    error = 1;
  }

  if(schemaCtxt) xmlSchemaFreeParserCtxt(schemaCtxt);
  if(schema) xmlSchemaFree(schema);
  if(validSchema) xmlSchemaFreeValidCtxt(validSchema);
  //if(buffer) xmlFreeParserInputBuffer(buffer); //segfaults for some reason
  if(parserData) delete parserData;

  return error;
}

} // end: namespace flame

