#include "parser.h"

FunctionDefinitionNode *makeFunctionDefinitionNode(const Vector *vectorList, int *index) {
    FunctionDefinitionNode *functionDefinitionNode = calloc(1, sizeof(FunctionDefinitionNode));
    functionDefinitionNode->declarationSpecifierNodes = buildVectorList();

    while (isDeclarationSpecifier(vectorList, *index)) {
        DeclarationSpecifierNode *declarationSpecifierNode = makeDeclarationSpecifierNode(vectorList, index);

        if (declarationSpecifierNode == NULL) {
            printf("error: could not create declaration-specifier-type node.\n");
            return NULL;
        }
        
        pushVector(functionDefinitionNode->declarationSpecifierNodes, declarationSpecifierNode);
    }

    functionDefinitionNode->declaratorNode = makeDeclaratorNode(vectorList, index);

    if (functionDefinitionNode->declaratorNode == NULL) {
        printf("error: could not create declarator-type node.\n");
        return NULL;
    }

    functionDefinitionNode->compoundStatementNode = makeCompoundStatementNode(vectorList, index);

    if (functionDefinitionNode->compoundStatementNode == NULL) {
        printf("error: could not create compound-statement-type node\n");
        return NULL;
    }

    return functionDefinitionNode;
}

bool isFunctionDefinition(const Vector *vectorList, int index) {
    const Token *token = vectorList->contents[index];

    while (!(token->type == TK_IDENTIFIER && !stringMapContains(classMap, token->string))) {
        token = vectorList->contents[++index];
    }

    token = vectorList->contents[++index];

    if (token->type != TK_LEFT_PARENTHESIS) {
        return false;
    }

    while (token->type != TK_RIGHT_PARENTHESIS) {
        token = vectorList->contents[++index];
    }

    token = vectorList->contents[++index];
    return (token->type == TK_LEFT_PARENTHESIS);
}

bool isFunctionDeclaration(const Vector *vectorList, int index) {
    const Token *token = vectorList->contents[index];

    while (!(token->type == TK_IDENTIFIER && !stringMapContains(classMap, token->string))) {
        token = vectorList->contents[++index];
    }

    token = vectorList->contents[++index];

    if (token->type != TK_LEFT_PARENTHESIS) {
        return false;
    }

    while (token->type != TK_RIGHT_PARENTHESIS) {
        token = vectorList->contents[++index];
    }

    token = vectorList->contents[++index];
    return (token->type == TK_NEWLINE);
}

bool isTypeSpecifier(const Vector *vectorList, int index) {
    const Token *token   = vectorList->contents[index];
    const Token *token2  = vectorList->contents[index + 1];
    const int type       = token->type;
    const int type2      = token2->type;

    return(type == TK_DIM || type2 == TK_AS);
}

bool isDeclarationSpecifier(const Vector* vectorList, int index) {
    const Token* token = vectorList->contents[index];
    const int type = token->type;

    return(isTypeSpecifier(vectorList, index) || type == TK_CONST);
}

DeclarationNode *makeDeclarationNode(const Vector *vectorList, int *index) {
    DeclarationNode *declarationNode = malloc(sizeof(DeclarationNode));
    declarationNode->declarationSpecifierNodes = buildVectorList();
    declarationNode->initializeDeclaratorNodes = buildVectorList();

    const Token* token = vectorList->contents[*index];

    while (isDeclarationSpecifier(vectorList, *index) && token->type != TK_NEWLINE) {
        DeclarationSpecifierNode* declarationSpecifierNode = makeDeclarationSpecifierNode(vectorList, index);

        if (declarationSpecifierNode == NULL) {
            printf("error: could not create declaration-specifier-type node.\n");
            return NULL;
        }

        pushVector(declarationNode->declarationSpecifierNodes, declarationSpecifierNode);
    }

    token = vectorList->contents[*index];

    while (token->type != TK_NEWLINE) {
        InitializeDeclaratorNode* initializeDeclaratorNode = makeInitializingDeclaratorNode(vectorList, index);

        if (initializeDeclaratorNode == NULL) {
            printf("error: could not create initialize-declarator-type node.\n");
            return NULL;
        }

        pushVector(declarationNode->initializeDeclaratorNodes, initializeDeclaratorNode);

        token = vectorList->contents[*index];

        if (token->type == TK_COMMA) {
            token = vectorList->contents[++*index];
        } 
    }

    (*index)++;
    return declarationNode;
}

ExternalDeclarationNode *makeExternalDeclarationNode(const Vector *vectorList, int *index) {
    ExternalDeclarationNode *externalDeclarationNode = calloc(1, sizeof(ExternalDeclarationNode));
    const Token *token = vectorList->contents[*index];

    if (isFunctionDefinition(vectorList, *index)) {
        externalDeclarationNode->functionDefinitionNode = makeFunctionDefinitionNode(vectorList, index);

        if (externalDeclarationNode->functionDefinitionNode == NULL) {
            printf("error: could not create function-definition-type node.\n");
            return NULL;
        }
    }
    else if (isFunctionDeclaration(vectorList, *index)) {
        token = vectorList->contents[*index];
        printf("%s\n", token->string);

        while (token->type != TK_NEWLINE) {
            token = vectorList->contents[++*index];
        }

        (*index)++;
    }
    else {
        externalDeclarationNode->declarationNode = makeDeclarationNode(vectorList, index);

        if (externalDeclarationNode->declarationNode == NULL) {
            printf("error: could not create declaration-definition-type node.\n");
            return NULL;
        }
    } 

    return externalDeclarationNode;
}

TransUnitNode *makeTransUnitNode() {
    TransUnitNode *transUnitNode = malloc(sizeof(TransUnitNode));
    transUnitNode->externalDeclarationNodes = buildVectorList();
    return transUnitNode;
}

TransUnitNode *parse(const Vector *vectorList) {
    classMap = buildStringMap(1024);
    int index = 0;

    TransUnitNode *transUnitNode = makeTransUnitNode();

    while (index < vectorList->size) {
        ExternalDeclarationNode *externalDeclarationNode = makeExternalDeclarationNode(vectorList, &index);

        if (externalDeclarationNode == NULL) {
            printf("error: could not create external-declaration-type node.\n");
            return NULL;
        }

        pushVector(transUnitNode->externalDeclarationNodes, externalDeclarationNode);
    }

    return transUnitNode;
}