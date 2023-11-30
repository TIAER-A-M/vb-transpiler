#include "lexer.h"

bool determineTokenType(const char *str, const char *pos, int stringLength) {
    if (strncmp(str, pos, stringLength) == 0) {
        return true;
    }
    else {
        return false;
    }
}

Token *readString(const char *p, int *pos) {
    (*pos)++;
    int len = 0;
    bool escape = false;

    while (true) {
        if (!escape && p[*pos + len] == '"') {
            break;
        }

        if (p[*pos + len] == '\\') {
            escape = true;
            len++;
        }
        else {
            escape = false;
            len++;
        }
    }

    Token *token  = calloc(1, sizeof(Token));
    token->type   = TK_STRING;
    token->strlen = len;
    token->string = malloc(sizeof(char) * token->strlen + 1);
    strncpy(token->string, &p[*pos], token->strlen);
    token->string[token->strlen] = '\0';

    *pos += (len + 1);
    return token;
}

Token *readNumber(const char *p, int *pos) {
    Token *token = calloc(1, sizeof(Token));
    token->type = TK_NUMBER;

    int len = 0;

    while (isdigit(p[*pos + len])) {
        len++;
    }

    char buf[32];
    strncpy(buf, &p[*pos], 32);
    buf[len] = '\0';

    token->num = atoi(buf);
    *pos += len;
    return token;
}

int getTokenType(const Vector *vec, int index) {
    Token *token = vec->contents[index];
    return token->type;
}

Vector *lex(char *addr) {
    Vector *vector = buildVectorList();

    int pos = 0;
    const char *p = addr;
    Token *token = NULL;

    while (p[pos]) {
        if (isspace(p[pos])) {
            pos++;
        }
        else if (p[pos] == '"') {
            token = readString(p, &pos);

            if (token == NULL) {
                printf("Failed to read string/char.\n");
                return NULL;
            }

            pushVector(vector, token);
        }
        else if (isSymbol(p[pos])) {
            token = readSymbol(p, &pos);

            if (token == NULL) {
                printf("Failed to read symbol.\n");
                return NULL;
            }

            pushVector(vector, token);
        }
        else if (isalpha(p[pos]) || p[pos] == '_') {
            token = readKeyword(p, &pos);

            if (token == NULL) {
                printf("Failed to read identifier.\n");
                return NULL;
            }

            pushVector(vector, token);
        }
        else if (isdigit(p[pos])) {
            token = readNumber(p, &pos);

            if (token == NULL) {
                printf("Failed to read number.\n");
                return NULL;
            }

            pushVector(vector, token);
        }
        else {
            printf("Invalid character [token %d] = '%c'\n", pos, p[pos]);
            return NULL;
        }
    }

    return vector;
}

Token *readSymbol(const char *p, int *pos) {
    Token *token = calloc(1, sizeof(Token));

    const char first = p[*pos];
    (*pos)++;
    const char second = p[*pos];
    
    switch (first) {
        case '[': {
            token->type = TK_LEFT_SQUARE;
            return token;
        }
        case ']': {
            token->type = TK_RIGHT_SQUARE;
            return token;
        }
        case '(': {
            token->type = TK_LEFT_PARENTHESIS;
            return token;
        }
        case ')': {
            token->type = TK_RIGHT_PARENTHESIS;
            return token;
        }
        case '{': {
            token->type = TK_LEFT_BRACKET;
            return token;
        }
        case '}': {
            token->type = TK_RIGHT_BRACKET;
            return token;
        }
        case '*': {
            if (second == '=') {
                token->type = TK_MULTIPLY_EQUAL;
                (*pos)++;
            }
            else {
                token->type = TK_ASTERISK;
            }

            return token;
        }
        case '/': {
            if (second == '=') {
                token->type = TK_DIVIDE_EQUAL;
                (*pos)++;
            }
            else {
                token->type = TK_SLASH;
            }

            return token;
        }
        case '+': {
            if (second == '=') {
                token->type = TK_ADD_EQUAL;
                (*pos)++;
            }
            else {
                token->type = TK_PLUS;
            }

            return token;
        }
        case '^': {
            if (second == '=') {
                token->type = TK_HAT_EQUAL;
                (*pos)++;
            }
            else {
                token->type = TK_HAT;
            }

            return token;
        }
        case '-': {
            if (second == '=') {
                token->type = TK_MINUS_EQUAL;
                (*pos)++;
            }
            else {
                token->type = TK_MINUS;
            }

            return token;
        }
        case '=': {
            token->type = TK_ASSIGNMENT;
            return token;
        }
        case '!': {
            token->type = TK_EXCLAMATION;
            return token;
        }
        case '&': {
            if (second == '=') {
                token->type = TK_AND_EQUAL;
                (*pos)++;
            }
            else {
                token->type = TK_CONCAT;
            }

            return token;
        }
        case ':': {
            token->type = TK_COLON;
            return token;
        }
        case ';': {
            token->type = TK_SEMI_COLON;
            return token;
        }
        case '<': {
            if (second == '<') {
                token->type = TK_SHIFT_LEFT;
                (*pos)++;
            }
            if (second == '>') {
                token->type = TK_NOT_EQUAL;
                (*pos)++;
            }
            else if (second == '=') {
                token->type = TK_LESS_OR_EQUAL;
                (*pos)++;
            }
            else {
                token->type = TK_LEFT_ANGLE;
            }
            
            return token;
        }
        case '>': {
            if (second == '>') {
                token->type = TK_SHIFT_RIGHT;
                (*pos)++;
            }
            else if (second == '=') {
                token->type = TK_GREATER_OR_EQUAL;
                (*pos)++;
            }
            else {
                token->type = TK_RIGHT_ANGLE;
            }

            return token;
        }
        case ',': {
            token->type = TK_COMMA;
            return token;
        }
        case '.': {
            token->type = TK_DOT;
            return token;
        }
        case '\'': {
            token->type = TK_COMMENT;
            return token;
        }
        case '\\': {
            token->type = TK_BACKSLASH;
            return token;
        }
        default: {
            return NULL;
        }
    }
}

bool isSymbol(char p) {
    switch (p) {
        case '[':
        case ']':
        case '(':
        case ')':
        case '{':
        case '}':
        case '*':
        case '/':
        case '+':
        case '-':
        case '=':
        case '!':
        case '&':
        case ';':
        case ':':
        case '<':
        case '>':
        case ',':
        case '.':
        case '\'':
        case '\\': {
            return true;
        }
        default: {
            return false;
        }
    }
}

Token *readKeyword(const char *p, int *pos) {
    Token *token = malloc(sizeof(Token));

    int len = 0;
    
    while (isalpha(p[*pos + len]) || isdigit(p[*pos + len]) || p[*pos + len] == '_') {
        len++;
    }

    // keywords
    if (determineTokenType("AddHandler", &p[*pos], 10)) {
        token->type = TK_ADD_HANDLER;
    }
    else if (determineTokenType("AddressOf", &p[*pos], 9)) {
        token->type = TK_ADDRESS_OF;
    }
    else if (determineTokenType("Alias", &p[*pos], 5)) {
        token->type = TK_ALIAS;
    }
    else if (determineTokenType("And", &p[*pos], 3)) {
        token->type = TK_AND;
    }
    else if (determineTokenType("AndAlso", &p[*pos], 7)) {
        token->type = TK_AND_ALSO;
    }
    else if (determineTokenType("As", &p[*pos], 2)) {
        token->type = TK_AS;
    }
    else if (determineTokenType("Boolean", &p[*pos], 7)) { 
        token->type = TK_BOOLEAN;
    }
    else if (determineTokenType("ByRef", &p[*pos], 5)) {
        token->type = TK_BY_REF;
    }
    else if (determineTokenType("Byte", &p[*pos], 4)) {
        token->type = TK_BYTE;
    }
    else if (determineTokenType("ByVal", &p[*pos], 5)) {
        token->type = TK_BY_VAL;
    }
    else if (determineTokenType("Call", &p[*pos], 4)) {
        token->type = TK_CALL;
    }
    else if (determineTokenType("Case", &p[*pos], 4)) {
        token->type = TK_CASE;
    }
    else if (determineTokenType("Catch", &p[*pos], 5)) {
        token->type = TK_CATCH;
    }
    else if (determineTokenType("CBool", &p[*pos], 5)) {
        token->type = TK_CBOOL;
    }
    else if (determineTokenType("CByte", &p[*pos], 5)) { 
        token->type = TK_CBYTE; 
    }
    else if (determineTokenType("CChar", &p[*pos], 5)) {
        token->type = TK_CCHAR;
    }
    else if (determineTokenType("CDate", &p[*pos], 5)) {
        token->type = TK_CDATE;
    }
    else if (determineTokenType("CDbl", &p[*pos], 4)) {
        token->type = TK_CDBL;   
    }
    else if (determineTokenType("CDec", &p[*pos], 4)) {
        token->type = TK_CDEC;
    }
    else if (determineTokenType("Char", &p[*pos], 4)) {
        token->type = TK_CHAR;
    }
    else if (determineTokenType("CInt", &p[*pos], 4)) {
        token->type = TK_CINT;
    }
    else if (determineTokenType("Class", &p[*pos], 5)) {
        token->type = TK_CLASS;
    }
    else if (determineTokenType("CLng", &p[*pos], 4)) {
        token->type = TK_CLNG;
    }
    else if (determineTokenType("CObj", &p[*pos], 4)) { 
        token->type = TK_COBJ;
    }
    else if (determineTokenType("Const", &p[*pos], 5)) {
        token->type = TK_CONST;
    }
    else if (determineTokenType("Continue", &p[*pos], 8)) {
        token->type = TK_CONTINUE;
    }
    else if (determineTokenType("CSByte", &p[*pos], 6)) {
        token->type = TK_CSBYTE;
    }
    else if (determineTokenType("CShort", &p[*pos], 6)) {
        token->type = TK_CSHORT;
    }
    else if (determineTokenType("CSng", &p[*pos], 4)) {
        token->type = TK_CSNG;
    }
    else if (determineTokenType("CStr", &p[*pos], 4)) {
        token->type = TK_CSTR;
    }
    else if (determineTokenType("CType", &p[*pos], 5)) {
        token->type = TK_CTYPE;
    }
    else if (determineTokenType("CUInt", &p[*pos], 5)) {
        token->type = TK_CUINT;
    }
    else if (determineTokenType("CULng", &p[*pos], 5)) {
        token->type = TK_CULONG;
    }
    else if (determineTokenType("CUShort", &p[*pos], 7)) {
        token->type = TK_CUSHORT;
    }
    else if (determineTokenType("Date", &p[*pos], 4)) {
        token->type = TK_DATE;
    }
    else if (determineTokenType("Decimal", &p[*pos], 7)) {
        token->type = TK_DECIMAL;
    }
    else if (determineTokenType("Declare", &p[*pos], 7)) {
        token->type = TK_DECLARE;
    }
    else if (determineTokenType("Default", &p[*pos], 7)) {
        token->type = TK_DEFAULT;
    }
    else if (determineTokenType("Delegate", &p[*pos], 8)) {
        token->type = TK_DELEGATE;
    }
    else if (determineTokenType("Dim", &p[*pos], 3)) {
        token->type = TK_DIM;
    }
    else if (determineTokenType("DirectCast", &p[*pos], 10)) {
        token->type = TK_DIRECT_CAST;
    }
    else if (determineTokenType("Do", &p[*pos], 2)) {
        token->type = TK_DO;
    }
    else if (determineTokenType("Double", &p[*pos], 6)) {
        token->type = TK_DOUBLE;
    }
    else if (determineTokenType("Each", &p[*pos], 4)) {
        token->type = TK_EACH;
    }
    else if (determineTokenType("Else", &p[*pos], 4)) {
        token->type = TK_ELSE;
    }
    else if (determineTokenType("ElseIf", &p[*pos], 6)) {
        token->type = TK_ELSE_IF;
    }
    else if (determineTokenType("End", &p[*pos], 3)) {
        token->type = TK_END;
    }
    else if (determineTokenType("EndIf", &p[*pos], 5)) {
        token->type = TK_END_IF;
    }
    else if (determineTokenType("Enum", &p[*pos], 4)) {
        token->type = TK_ENUM;
    }
    else if (determineTokenType("Erase", &p[*pos], 5)) {
        token->type = TK_ERASE;
    }
    else if (determineTokenType("Error", &p[*pos], 5)) {
        token->type = TK_ERROR;
    }
    else if (determineTokenType("Event", &p[*pos], 5)) {
        token->type = TK_EVENT;
    }
    else if (determineTokenType("Exit", &p[*pos], 4)) {
        token->type = TK_EXIT;
    }
    else if (determineTokenType("False", &p[*pos], 5)) {
        token->type = TK_FALSE;
    }
    else if (determineTokenType("Finally", &p[*pos], 7)) {
        token->type = TK_FINALLY;
    }
    else if (determineTokenType("For", &p[*pos], 3)) {
        token->type = TK_FOR;
    }
    else if (determineTokenType("For Each...Next", &p[*pos], 15)) {
        token->type = TK_FOR_EACH_NEXT;
    }
    else if (determineTokenType("Friend", &p[*pos], 6)) {
        token->type = TK_FRIEND;
    }
    else if (determineTokenType("Function", &p[*pos], 8)) {
        token->type = TK_FUNCTION;
    }
    else if (determineTokenType("Get", &p[*pos], 3)) {
        token->type = TK_GET;
    }
    else if (determineTokenType("GetType", &p[*pos], 8)) {
        token->type = TK_GET_TYPE;
    }
    else if (determineTokenType("GetXMLNamespace", &p[*pos], 15)) {
        token->type = TK_GET_XML_NAMESPACE;
    }
    else if (determineTokenType("Global", &p[*pos], 6)) {
        token->type = TK_GLOBAL;
    }
    else if (determineTokenType("GoSub", &p[*pos], 5)) {
        token->type = TK_GO_SUB;
    }
    else if (determineTokenType("GoTo", &p[*pos], 4)) {
        token->type = TK_GO_TO;
    }
    else if (determineTokenType("Handles", &p[*pos], 7)) {
        token->type = TK_HANDLES;
    }
    else if (determineTokenType("If", &p[*pos], 2)) {
        token->type = TK_IF;
    }
    else if (determineTokenType("Implements", &p[*pos], 10)) {
        token->type = TK_IMPLEMENTS;
    }
    else if (determineTokenType("Imports", &p[*pos], 7)) {
        token->type = TK_IMPORTS;
    }
    else if (determineTokenType("In", &p[*pos], 2)) {
        token->type = TK_IN;
    }
    else if (determineTokenType("Inherits", &p[*pos], 8)) {
        token->type = TK_INHERITS;
    }
    else if (determineTokenType("Integer", &p[*pos], 7)) {
        token->type = TK_INTEGER;
    }
    else if (determineTokenType("Interface", &p[*pos], 9)) {
        token->type = TK_INTERFACE;
    }
    else if (determineTokenType("Is", &p[*pos], 2)) {
        token->type = TK_IS;
    }
    else if (determineTokenType("IsNot", &p[*pos], 5)) {
        token->type = TK_IS_NOT;
    }
    else if (determineTokenType("Let", &p[*pos], 3)) {
        token->type = TK_LET;
    }
    else if (determineTokenType("Lib", &p[*pos], 3)) {
        token->type = TK_LIB;
    }
    else if (determineTokenType("Like", &p[*pos], 4)) {
        token->type = TK_LIKE;
    }
    else if (determineTokenType("Long", &p[*pos], 4)) {
        token->type = TK_LONG;
    }
    else if (determineTokenType("Loop", &p[*pos], 4)) {
        token->type = TK_LOOP;
    }
    else if (determineTokenType("Me", &p[*pos], 2)) {
        token->type = TK_ME;
    }
    else if (determineTokenType("Mod", &p[*pos], 3)) {
        token->type = TK_MOD;
    }
    else if (determineTokenType("Module", &p[*pos], 6)) {
        token->type = TK_MODULE;
    }
    else if (determineTokenType("MustInherit", &p[*pos], 11)) {
        token->type = TK_MUST_INHERIT;
    }
    else if (determineTokenType("MustOverride", &p[*pos], 12)) {
        token->type = TK_MUST_OVERRIDE;
    }
    else if (determineTokenType("MyBase", &p[*pos], 6)) {
        token->type = TK_MY_BASE;
    }
    else if (determineTokenType("MyClass", &p[*pos], 7)) {
        token->type = TK_MY_CLASS;
    }
    else if (determineTokenType("Namespace", &p[*pos], 9)) {
        token->type = TK_NAMESPACE;
    }
    else if (determineTokenType("Narrowing", &p[*pos], 9)) {
        token->type = TK_NARROWING;
    }
    else if (determineTokenType("New", &p[*pos], 3)) {
        token->type = TK_NEW;
    }
    else if (determineTokenType("Next", &p[*pos], 4)) {
        token->type = TK_NEXT;
    }
    else if (determineTokenType("Not", &p[*pos], 3)) {
        token->type = TK_NOT;
    }
    else if (determineTokenType("Nothing", &p[*pos], 7)) {
        token->type = TK_NOTHING;
    }
    else if (determineTokenType("NotInheritable", &p[*pos], 14)) {
        token->type = TK_NOT_INHERITABLE;
    }
    else if (determineTokenType("NotOverridable", &p[*pos], 14)) {
        token->type = TK_NOT_OVERRIDABLE;
    }
    else if (determineTokenType("Object", &p[*pos], 6)) {
        token->type = TK_OBJECT;
    }
    else if (determineTokenType("Of", &p[*pos], 2)) {
        token->type = TK_OF;
    }
    else if (determineTokenType("On", &p[*pos], 2)) {
        token->type = TK_ON;
    }
    else if (determineTokenType("Operator", &p[*pos], 8)) {
        token->type = TK_OPERATOR;
    }
    else if (determineTokenType("Option", &p[*pos], 6)) {
        token->type = TK_OPTION;
    }
    else if (determineTokenType("Optional", &p[*pos], 8)) {
        token->type = TK_OPTIONAL;
    }
    else if (determineTokenType("Or", &p[*pos], 2)) {
        token->type = TK_OR;
    }
    else if (determineTokenType("OrElse", &p[*pos], 6)) {
        token->type = TK_OR_ELSE;
    }
    else if (determineTokenType("Out", &p[*pos], 3)) {
        token->type = TK_OUT;
    }
    else if (determineTokenType("Overloads", &p[*pos], 9)) {
        token->type = TK_OVERLOADS;
    }
    else if (determineTokenType("Overridable", &p[*pos], 11)) {
        token->type = TK_OVERRIDABLE;
    }
    else if (determineTokenType("Overrides", &p[*pos], 9)) {
        token->type = TK_OVERRIDES;
    }
    else if (determineTokenType("ParamArray", &p[*pos], 10)) {
        token->type = TK_PARAM_ARRAY;
    }
    else if (determineTokenType("Partial", &p[*pos], 7)) {
        token->type = TK_PARTIAL;
    }
    else if (determineTokenType("Private", &p[*pos], 7)) {
        token->type = TK_PRIVATE;
    }
    else if (determineTokenType("Property", &p[*pos], 8)) {
        token->type = TK_PROPERTY;
    }
    else if (determineTokenType("Protected", &p[*pos], 9)) {
        token->type = TK_PROTECTED;
    }
    else if (determineTokenType("Public", &p[*pos], 6)) {
        token->type = TK_PUBLIC;
    }
    else if (determineTokenType("RaiseEvent", &p[*pos], 10)) {
        token->type = TK_RAISE_EVENT;
    }
    else if (determineTokenType("ReadOnly", &p[*pos], 8)) {
        token->type = TK_READ_ONLY;
    }
    else if (determineTokenType("ReDim", &p[*pos], 5)) {
        token->type = TK_REDIM;
    }
    else if (determineTokenType("REM", &p[*pos], 3)) {
        token->type = TK_REM;
    }
    else if (determineTokenType("RemoveHandler", &p[*pos], 13)) {
        token->type = TK_REMOVE_HANDLER;
    }
    else if (determineTokenType("Resume", &p[*pos], 6)) {
        token->type = TK_RESUME;
    }
    else if (determineTokenType("Return", &p[*pos], 6)) {
        token->type = TK_RETURN;
    }
    else if (determineTokenType("SByte", &p[*pos], 5)) {
        token->type = TK_SBYTE;
    }
    else if (determineTokenType("Select", &p[*pos], 6)) {
        token->type = TK_SELECT;
    }
    else if (determineTokenType("Set", &p[*pos], 3)) {
        token->type = TK_SET;
    }
    else if (determineTokenType("Shadows", &p[*pos], 7)) {
        token->type = TK_SHADOWS;
    }
    else if (determineTokenType("Shared", &p[*pos], 6)) {
        token->type = TK_SHARED;
    }
    else if (determineTokenType("Short", &p[*pos], 5)) {
        token->type = TK_SHORT;
    }
    else if (determineTokenType("Single", &p[*pos], 6)) {
        token->type = TK_SINGLE;
    }
    else if (determineTokenType("Static", &p[*pos], 6)) {
        token->type = TK_STATIC;
    }
    else if (determineTokenType("Step", &p[*pos], 4)) {
        token->type = TK_STEP;
    }
    else if (determineTokenType("Stop", &p[*pos], 4)) {
        token->type = TK_STOP;
    }
    else if (determineTokenType("String", &p[*pos], 6)) {
        token->type = TK_STRING;
    }
    else if (determineTokenType("Structure", &p[*pos], 9)) {
        token->type = TK_STURCTURE;
    }
    else if (determineTokenType("Sub", &p[*pos], 3)) {
        token->type = TK_SUB;
    }
    else if (determineTokenType("SyncLock", &p[*pos], 8)) {
        token->type = TK_SYNCLOCK;
    }
    else if (determineTokenType("Then", &p[*pos], 4)) {
        token->type = TK_THEN;
    }
    else if (determineTokenType("Throw", &p[*pos], 5)) {
        token->type = TK_THROW;
    }
    else if (determineTokenType("To", &p[*pos], 2)) {
        token->type = TK_TO;
    }
    else if (determineTokenType("True", &p[*pos], 4)) {
        token->type = TK_TRUE;
    }
    else if (determineTokenType("Try", &p[*pos], 3)) {
        token->type = TK_TRY;
    }
    else if (determineTokenType("TryCast", &p[*pos], 8)) {
        token->type = TK_TRY_CAST;
    }
    else if (determineTokenType("TypeOf...Is", &p[*pos], 11)) {
        token->type = TK_TYPE_OF_IS;
    }
    else if (determineTokenType("UInteger", &p[*pos], 8)) {
        token->type = TK_UINTEGER;
    }
    else if (determineTokenType("ULong", &p[*pos], 5)) {
        token->type = TK_ULONG;
    }
    else if (determineTokenType("UShort", &p[*pos], 6)) {
        token->type = TK_USHORT;
    }
    else if (determineTokenType("Using", &p[*pos], 5)) {
        token->type = TK_USING;
    }
    else if (determineTokenType("Variant", &p[*pos], 7)) {
        token->type = TK_VARIANT;
    }
    else if (determineTokenType("Wend", &p[*pos], 4)) {
        token->type = TK_WEND;
    }
    else if (determineTokenType("When", &p[*pos], 4)) {
        token->type = TK_WHEN;
    }
    else if (determineTokenType("While", &p[*pos], 5)) {
        token->type = TK_WHILE;
    }
    else if (determineTokenType("Widening", &p[*pos], 8)) {
        token->type = TK_WIDENING;
    }
    else if (determineTokenType("With", &p[*pos], 4)) {
        token->type = TK_WITH;
    }
    else if (determineTokenType("WithEvents", &p[*pos], 10)) {
        token->type = TK_WITH_EVENTS;
    }
    else if (determineTokenType("WriteOnly", &p[*pos], 9)) {
        token->type = TK_WRITE_ONLY;
    }
    else if (determineTokenType("Xor", &p[*pos], 3)) {
        token->type = TK_XOR;
    }
    else if (determineTokenType("#Else", &p[*pos], 5)) {
        token->type = TK_POUND_ELSE;
    }

    // form attributes
    else if (determineTokenType("Appearance", &p[*pos], 10)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("AutoRedraw", &p[*pos], 10)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("BackColor", &p[*pos], 9)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("BorderStyle", &p[*pos], 11)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("Caption", &p[*pos], 7)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("ClipControls", &p[*pos], 12)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("ControlBox", &p[*pos], 10)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("DrawMode", &p[*pos], 8)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("DrawStyle", &p[*pos], 9)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("DrawWidth", &p[*pos], 9)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("Enabled", &p[*pos], 8)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("FillColor", &p[*pos], 9)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("FillStyle", &p[*pos], 9)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("Font", &p[*pos], 4)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("FontTransparent", &p[*pos], 15)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("ForeColor", &p[*pos], 9)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("HasDC", &p[*pos], 5)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("Height", &p[*pos], 6)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("HelpContextID", &p[*pos], 13)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("KeyPreview", &p[*pos], 10)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("Left", &p[*pos], 4)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("LinkMode", &p[*pos], 8)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("LinkTopic", &p[*pos], 9)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("MaxButton", &p[*pos], 9)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("MDIChild", &p[*pos], 8)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("MinButton", &p[*pos], 9)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("MouseIcon", &p[*pos], 9)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("MousePointer", &p[*pos], 12)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("Moveable", &p[*pos], 8)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("NegotiateMenu", &p[*pos], 13)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("OLEDropMode", &p[*pos], 11)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("Palette", &p[*pos], 8)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("PaletteMode", &p[*pos], 11)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("Picture", &p[*pos], 8)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("RightToLeft", &p[*pos], 11)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("ScaleHeight", &p[*pos], 11)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("ScaleLeft", &p[*pos], 9)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("ScaleMode", &p[*pos], 9)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("ScaleTop", &p[*pos], 8)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("ScaleWidth", &p[*pos], 10)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("ShowInTaskbar", &p[*pos], 13)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("StartUpPosition", &p[*pos], 5)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("Tag", &p[*pos], 3)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("Top", &p[*pos], 3)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("Visible", &p[*pos], 7)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("WhatsThisButton", &p[*pos], 15)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("Width", &p[*pos], 5)) {
        token->type = TK_POUND_ELSE;
    }
    else if (determineTokenType("WindowState", &p[*pos], 11)) {
        token->type = TK_POUND_ELSE;
    }
    else {
        token->type = TK_IDENTIFIER;
    }

    if (token->type == TK_IDENTIFIER) {
        token->string = malloc(sizeof(char) * len + 1);
        token->strlen = len + 1;
        strncpy(token->string, &p[*pos], len);
        token->string[len] = '\0';
    }

    *(pos) += len;

    return token;
}