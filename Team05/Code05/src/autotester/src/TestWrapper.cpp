#include "TestWrapper.h"
#include "AbstractWrapper.h"
#include <fstream>
#include <sstream>

// implementation code of WrapperFactory - do NOT modify the next 5 lines
AbstractWrapper* WrapperFactory::wrapper = 0;
AbstractWrapper* WrapperFactory::createWrapper() {
    if (wrapper == 0) wrapper = new TestWrapper;
    return wrapper;
}
// Do not modify the following line
volatile bool AbstractWrapper::GlobalStop = false;
AbstractWrapper::~AbstractWrapper() = default;

// a default constructor
TestWrapper::TestWrapper() {
    // create any objects here as instance variables of this class
    // as well as any initialization required for your spa program
}

TestWrapper::~TestWrapper() = default;

// method for parsing the SIMPLE source
void TestWrapper::parse(std::string filename) {
    // call your parser to do the parsing
    // ...rest of your code...
    std::ifstream file;
    std::stringstream buffer;
    file.open(filename);
    buffer << file.rdbuf();

    if (!sp.processSimple(buffer.str(), &pkb)) {
        std::cout << "Failed to parse SIMPLE source file." << std::endl;
        std::exit(1);
    };
}

// method to evaluating a query
void TestWrapper::evaluate(std::string query, std::list<std::string>& results) {
    qps.evaluate(query, results, &pkb);
}
