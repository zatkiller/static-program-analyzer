#include "resultprojector.h"

namespace qps::evaluator {
    SelectElemInfo SelectElemInfo::ofDeclaration(int columnNo) {
        SelectElemInfo e;
        e.columnNo = columnNo;
        e.isAttr = false;
        return e;
    }

    SelectElemInfo SelectElemInfo::ofAttr(int columnNo, query::AttrName attrName) {
        SelectElemInfo e;
        e.columnNo = columnNo;
        e.attrName = std::move(attrName);
        e.isAttr = true;
        return e;
    }

    std::string ResultProjector::PKBFieldToString(PKBField pkbField) {
        std::string res;
        if (pkbField.entityType == PKBEntityType::STATEMENT) {
            int lineNo = std::get<STMT_LO>(pkbField.content).statementNum;
            res = std::to_string(lineNo);
        } else if (pkbField.entityType == PKBEntityType::CONST) {
            int c = std::get<CONST>(pkbField.content);
            res = std::to_string(c);
        } else if (pkbField.entityType == PKBEntityType::VARIABLE) {
            res = std::get<VAR_NAME>(pkbField.content).name;
        } else if (pkbField.entityType == PKBEntityType::PROCEDURE) {
            res = std::get<PROC_NAME>(pkbField.content).name;
        }
        return res;
    }

    std::string ResultProjector::PKBFieldAttrToString(PKBField pkbField, query::AttrName attrName) {
        std::string res;
        if (attrName == query::AttrName::PROCNAME || attrName == query::AttrName::VARNAME) {
            res = ClauseHandler::getPKBFieldAttr<std::string>(pkbField);
        } else {
            res = std::to_string(ClauseHandler::getPKBFieldAttr<int>(pkbField));
        }
        return res;
    }

    std::list<std::string> ResultProjector::projectResult(ResultTable &table, query::ResultCl resultCl) {
        std::list<std::string> listResult{};
        if (table.hasResult()) {
            return listResult;
        }
        std::vector<query::Elem> tuple = resultCl.getTuple();
        std::vector<SelectElemInfo> elem{};
        for (auto e : tuple) {
            SelectElemInfo elemInfo;
            if (e.isDeclaration())
                elemInfo = SelectElemInfo::ofDeclaration(table.getSynLocation(e.getDeclaration().getSynonym()));
            else
                elemInfo = SelectElemInfo::ofAttr(table.getSynLocation(e.getAttrRef().getDeclarationSynonym()),
                                                  e.getAttrRef().getAttrName());
            elem.push_back(elemInfo);
        }
        std::unordered_set<std::string> resultSet;
        for (auto record : table.getTable()) {
            std::string result;
            for (auto e : elem) {
                PKBField fld = record[e.columnNo];
                result += (e.isAttr ? PKBFieldAttrToString(fld, e.attrName)
                                    : PKBFieldToString(fld)) + " ";
            }
            result.erase(std::find_if(result.rbegin(), result.rend(), [](unsigned char ch) {
                return !std::isspace(ch);}).base(), result.end());
            if (resultSet.find(result) == resultSet.end()) {
                listResult.push_back(result);
                resultSet.insert(result);
            }
        }

        return listResult;
    }
}  // namespace qps::evaluator
