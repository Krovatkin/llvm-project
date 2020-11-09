* Select a field of type int:
m fieldDecl(hasType(asString("int")))

* A greater operator in an if statement
m ifStmt(hasCondition(binaryOperator(hasOperatorName(">"))))

* A greater operator with an integral literal RHS in an if statement
m ifStmt(hasCondition(binaryOperator(allOf(hasOperatorName(">"), hasRHS(integerLiteral().bind("int"))))))

* The right had side equals to 3
m ifStmt(hasCondition(binaryOperator(allOf(hasOperatorName(">"), hasRHS(integerLiteral(equals(3)).bind("int"))))))

m functionDecl()
m methodDecl()
m cxxConstructExpr()