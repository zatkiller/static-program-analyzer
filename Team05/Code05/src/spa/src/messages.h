#pragma once

// Define your error messages here
namespace messages {

namespace qps {

namespace parser {
    // Default messages
    inline constexpr char defaultPqlExceptionMesssage[] = "PqlException is thrown!";
    inline constexpr char defaultPqlSyntaxExceptionMessage[] = "PqlSyntaxException is thrown!";
    inline constexpr char defaultPqlSemanticExceptionMessage[] = "PqlSemanticException is thrown!";

    // Parsing syntax error messages
    inline constexpr char leadingZeroMessage[] = "Leading zero detected!";
    inline constexpr char notExpectingTokenMessage[] = "Not expecting this token type!";
    inline constexpr char unexpectedWhitespaceMessage[] = "Unexpected whitespace";
    inline constexpr char invalidStmtRefMessage[] = "Not a valid StmtRef!";
    inline constexpr char invalidEntRefMessage[] = "Not a valid EntRef!";
    inline constexpr char invalidRelRefMessage[] = "Unrecognized RelRef";

    // Parsing validation error messages
    inline constexpr char synonymNotStatementTypeMessage[] = "synonym not a valid statement";
    inline constexpr char synonymNotEntityTypeMessage[] = "synonym not a valid entity";

    // Declaration error messages
    inline constexpr char noSuchDesignEntityMessage[] = "No such design entity!";

    // Select error messages
    inline constexpr char declarationDoesNotExistMessage[] = "declaration does not exist in declaration!";

    // Such that error messages
    inline constexpr char cannotBeWildcardMessage[] = "cannot be a wildcard!";
    inline constexpr char notVariableSynonymMessage[] = "synonym has to be of variable design entity";
    inline constexpr char notProcedureSynonymMessage[] = "synonym has to be of procedure design entity";
    inline constexpr char notAssignSynonymMessage[] = "synonym has to be of assign design entity";
    inline constexpr char notValidPatternType[] = "Not an valid pattern type";

    // With clause error messages
    inline constexpr char invalidAttrCompRefMessage[] = "Invalid token appeared when parsing AttrCompareRef";
    inline constexpr char invalidAttrNameForDesignEntity[] = "Design Entity does not contain this AttrName";
    inline constexpr char incompatibleComparisonMessage[] = "Incompatible comparison for AttrCopare";

    // Pattern error messages
    inline constexpr char expressionInvalidGrammarMessage[] = "Pattern expr has invalid grammar!";
    inline constexpr char expressionUnexpectedEndMessage[] = "Pattern expr has unexpectedly reached end of expression!";
    inline constexpr char notAnAssignPatternMessage[] = "Not an assign pattern!";

}  //  namespace parser

}  //  namespace qps

}  // namespace messages
