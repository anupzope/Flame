#ifndef FLAME_LFLAME3_RESTART_HH
#define FLAME_LFLAME3_RESTART_HH

#include <Loci.h>

#include <vector>
#include <string>
#include <ostream>
#include <istream>

namespace flame {

struct RestartSettings {
  std::vector<int> frequencies;
  std::vector<int> counts;
  
  std::string toString() const;
  void fromString(std::string const & str);
  int fromOptionsList(options_list const & ol, std::string & err);
};

std::ostream & operator<<(std::ostream & s, RestartSettings const & rhs);
std::istream & operator>>(std::istream & s, RestartSettings & rhs);

} // end: namespace flame

namespace Loci {

class RestartSettingsSchemaConverter {
  flame::RestartSettings & ref;
  std::string fmt;
  
public:
  explicit RestartSettingsSchemaConverter(flame::RestartSettings & ref) : ref(ref) {
    fmt = ref.toString();
  }
  
  int getSize() const {
    return fmt.size();
  }
  
  void getState(char * buf, int & size) {
    size = getSize();
    for(int i = 0; i < size; ++i) {
      buf[i] = fmt[i];
    }
  }
  
  void setState(char * buf, int size) {
    ref.fromString(std::string(buf));
  }
};

template<>
struct data_schema_traits<flame::RestartSettings> {
  typedef USER_DEFINED_CONVERTER Schema_Converter;
  typedef char Converter_Base_Type;
  typedef RestartSettingsSchemaConverter Converter_Type;
};

} // end: namespace Loci

#endif // #ifndef FLAME_LFLAME3_RESTART_HH
