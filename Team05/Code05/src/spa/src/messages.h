#pragma once

// Define your error messages here
namespace messages {

namespace qps {

namespace parser {
    inline constexpr char defaultPqlExceptionMesssage[] = "PqlException is thrown!";
    inline constexpr char defaultPqlSyntaxExceptionMessage[] = "PqlSyntaxException is thrown!";
    inline constexpr char defaultPqlSemanticExceptionMessage[] = "PqlSemanticException is thrown!";

    inline constexpr char notExpectingTokenMessage[] = "Not expecting this token type!";
    inline constexpr char noSuchDesignEntityMessage[] = "No such design entity!";
    inline constexpr char declarationDoesNotExistMessage[] = "declaration does not exist in declaration!";

    inline constexpr char invalidStmtRefMessage[] = "Not a valid StmtRef!";
    inline constexpr char invalidEntRefMessage[] = "Not a valid EntRef!";
    inline constexpr char invalidRelRefMessage[] = "Unrecognized RelRef";
    inline constexpr char notAnAssignmentMessage[] = "Not an assignment";

    inline constexpr char cannotBeWildcardMessage[] = "cannot be a wildcard!";
    inline constexpr char notVariableSynonymMessage[] = "synonym has to be of variable design entity";
    inline constexpr char notProcedureSynonymMessage[] = "synonym has to be of procedure design entity";
    inline constexpr char synonymNotStatementTypeMessage[] = "synonym not a valid statement";
    inline constexpr char synonymNotEntityTypeMessage[] = "synonym not a valid entity";

    inline constexpr char expressionInvalidGrammarMessage[] = "Pattern expr has invalid grammar!";
    inline constexpr char expressionUnexpectedEndMessage[] = "Pattern expr has unexpectedly reached end of expression!";
}  //  namespace parser

}  //  namespace qps

}  // namespace messages
