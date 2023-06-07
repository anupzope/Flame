#ifndef BOUNDARY_CHECKER_HH
#define BOUNDARY_CHECKER_HH

#include <Loci.h>

namespace flame {

bool check_boundary_conditions(fact_db & facts);

class BoundaryChecker : public Loci::CPTR_type {
public:
  /**
   * Returns string containing comma separated list of boundary
   * condition tags that are checked by this checker.
   */
  virtual std::string boundaryConditions() = 0;
  
  /**
   * Returns string containing variables that are tested by this
   * checker.
   */
  virtual std::string variablesChecked() = 0;
  
  /**
   * Check the boundary condition.
   */
  virtual bool checkOptions(const options_list & bc_options) = 0;
  
  /**
   * Emit error message.
   */
  virtual std::ostream & errorMessage(std::ostream & s) = 0;
};

typedef Loci::CPTR<BoundaryChecker> BoundaryCheckerP;

class BoundaryCheckerList {
public:
  struct Item {
    Loci::CPTR<BoundaryChecker> entry;
    Item * next;
  };
  
  static Item * items;
  
  static void insert(Loci::CPTR<BoundaryChecker> cp) {
    Item * p = new Item;
    p->next = items;
    p->entry = cp;
    items = p;
  }
};

template<typename T>
class registerBoundaryChecker {
public:
  registerBoundaryChecker() {
    Loci::CPTR<BoundaryChecker> p = new T();
    BoundaryCheckerList::insert(p);
  }
};

} // end: namespace flame

#endif // #ifndef BOUNDARY_CHECKER_HH
