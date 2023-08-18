#ifndef FLAME_LFLAME_UTILS_HH
#define FLAME_LFLAME_UTILS_HH

#include <Loci.h>

#include <vector>
#include <string>

namespace Loci {

class STDStringVectorSchemaConverter {
  std::vector<std::string> & ref;
  std::string str;
  
public:
  explicit STDStringVectorSchemaConverter(std::vector<std::string> & ref);
  
  int getSize() const { return str.size()+1; }
  
  void getState(char * buf, int & size) {
    size = str.size()+1;
    for(int i = 0; i < size-1; ++i) {
      buf[i] = str[i];
    }
    buf[size] = '\0';
  }
  
  void setState(char * buf, int size);
};

template<>
struct data_schema_traits<std::vector<std::string> > {
  typedef USER_DEFINED_CONVERTER  Schema_Converter;
  typedef char Converter_Base_Type;
  typedef STDStringVectorSchemaConverter Converter_Type;
};

} // end: namespace Loci

#endif // end: #ifndef FLAME_LFLAME_UTILS_HH
