#ifndef FLAME_PLOT_HH
#define FLAME_PLOT_HH

#include <string>
#include <vector>

#include <Loci.h>

namespace flame {

struct PlotSettings {
  int frequency;
  std::vector<int> counts;
  std::vector<std::string> variables;
  
  std::string toString() const;
  void fromString(std::string const & str);
  int fromOptionsList(options_list const & ol, std::string & err);
};

std::ostream & operator<<(std::ostream & s, PlotSettings const & rhs);
std::istream & operator>>(std::istream & s, PlotSettings & rhs);

} // end: namespace flame

namespace Loci {

class PlotSettingsSchemaConverter {
  flame::PlotSettings & ref;
  std::string fmt;
  
public:
  explicit PlotSettingsSchemaConverter(flame::PlotSettings & iref) : ref(iref) {
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
struct data_schema_traits<flame::PlotSettings> {
  typedef USER_DEFINED_CONVERTER Schema_Converter;
  typedef char Converter_Base_Type;
  typedef PlotSettingsSchemaConverter Converter_Type;
};

} // end: namespace Loci

namespace flame {

void dumpScalar(
  sequence const & seq,
  const_store<float> & c2n,
  std::string const & caseName,
  std::string const & plotPostfix,
  std::string const & type,
  std::string const & sname
);

class NodalScalarOutput : public Loci::pointwise_rule {
  std::string var_name;
  std::string value_name;
  Loci::const_store<float> c2n;
  Loci::const_param<std::string> plotPostfix;
  Loci::const_param<std::string> caseName;
  Loci::param<bool> OUTPUT;
  
public:
  NodalScalarOutput(const char * vname, const char * valname);
  virtual void compute(const Loci::sequence & seq);
};

} // end: namespace flame

#define OUTPUT_SCALAR(X,Y) class OUT_##Y : public flame::NodalScalarOutput {\
public: \
  OUT_##Y() : flame::NodalScalarOutput(X,#Y){} \
}; \
register_rule<OUT_##Y> register_OUT_##Y;

#endif // #ifndef FLAME_PLOT_HH
