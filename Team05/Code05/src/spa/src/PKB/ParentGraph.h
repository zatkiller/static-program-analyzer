//#pragma once
//
//#include <map>
//#include <vector>
//#include <set>
//#include <unordered_set>
//
//#include "PKBField.h"
//
//using Result = std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>;
//using EdgeSet = std::set<STMT_LO>;
//
//class ParentGraph {
//public:
//    void addEdge(STMT_LO u, STMT_LO v);
//    Result getParentT(PKBField entity1, PKBField entity2);
//
//private:
//    std::map<STMT_LO, EdgeSet> edges;
//    std::map<STMT_LO, STMT_LO> reversedEdges;
//    Result traverseStart(PKBField entity1, PKBField entity2);
//    Result traverseEnd(PKBField entity1, PKBField entity2);
//    // TODO(pattan): May need additional helper functions
//};
