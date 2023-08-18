#include <utils.hh>

#include <iomanip>
#include <sstream>

namespace Loci {

STDStringVectorSchemaConverter::STDStringVectorSchemaConverter(
  std::vector<std::string> & ref
) : ref(ref) {
  std::ostringstream ss;
  for(size_t i = 0; i < ref.size(); ++i) {
    ss << std::quoted(ref[i]) << " ";
  }
  str = ss.str();
}

void STDStringVectorSchemaConverter::setState(char * buf, int size) {
  ref.resize(size);
  std::string str(buf);
  std::istringstream ss(str);
  for(int i = 0; i < size; ++i) {
    ss >> std::quoted(ref[i]);
  }
}

} // end: namespace Loci
