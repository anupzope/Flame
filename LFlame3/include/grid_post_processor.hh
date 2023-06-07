#ifndef GRID_POST_PROCESSOR_HH
#define GRID_POST_PROCESSOR_HH

#include <Loci.h>

namespace flame {

class GridPostProcessor : public Loci::CPTR_type {
public:
  virtual void processData(fact_db & facts, rule_db & rdb) const = 0;
};

class GridPostProcessorList {
public:
  struct Item {
    Loci::CPTR<GridPostProcessor> entry;
    Item * next;
  };
  
  static Item * items;
  
  static void insert(Loci::CPTR<GridPostProcessor> cp) {
    Item * p = new Item;
    p->next = items;
    p->entry = cp;
    items = p;
  }
};

template<typename T, typename ... Args>
class registerGridPostProcessor {
public:
  registerGridPostProcessor(Args... args) {
    Loci::CPTR<GridPostProcessor> p = new T(args...);
    GridPostProcessorList::insert(p);
  }
};

} // end: namespace flame

#endif // #ifndef GRID_POST_PROCESSOR_HH
