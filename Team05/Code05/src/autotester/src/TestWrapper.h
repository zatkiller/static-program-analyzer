#ifndef TESTWRAPPER_H
#define TESTWRAPPER_H

#include <string>
#include <iostream>
#include <list>

// include your other headers here
#include "AbstractWrapper.h"

#include "PKB.h"
#include "pql/qps.h"

class TestWrapper : public AbstractWrapper {
public:
  // default constructor
  TestWrapper();
  
  // destructor
  ~TestWrapper();
  
  // method for parsing the SIMPLE source
  virtual void parse(std::string filename);
  
  // method for evaluating a query
  virtual void evaluate(std::string query, std::list<std::string>& results);

private:
    // Need PKB team to check if this is
    PKB pkb;
    QPS qps;
};

#endif
