#ifndef PARSER_H
#define PARSER_H

#include <cstdlib>
#include <cmath>
#include <cctype>
#include <iostream>
#include <map>
#include <string>

#include "variable.h"

class SNOLParser {
private:
    std::map<std::string, Variable>& variables;

    static std::string trim(const std::string& s) {
        size_t start = s.find_first_not_of(" \t");
        if (start == std::string::npos) return "";
        size_t end = s.find_last_not_of(" \t");
        return s.substr(start, end - start + 1);
    }

    bool isValidVariableName(const std::string& name) {
        if (name.empty() || !std::isalpha(static_cast<unsigned char>(name[0]))) return false;
        for (size_t i = 1; i < name.length(); ++i) {
            if (!std::isalnum(static_cast<unsigned char>(name[i]))) return false;
        }
        if (name == "BEG" || name == "PRINT" || name == "EXIT!") return false;
        return true;
    }

    bool parseLiteral(const std::string& token, Variable& result) {
        char* endPtr;
        long intValue = std::strtol(token.c_str(), &endPtr, 10);
        if (*endPtr == '\0') {
            result.type = Variable::INT_TYPE;
            result.intValue = static_cast<int>(intValue);
            return true;
        }

        float floatValue = std::strtof(token.c_str(), &endPtr);
        if (*endPtr == '\0') {
            result.type = Variable::FLOAT_TYPE;
            result.floatValue = floatValue;
            return true;
        }

        return false;
    }

    bool getVariable(const std::string& name, Variable& result) {
        auto it = variables.find(name);
        if (it == variables.end()) return false;
        result = it->second;
        return true;
    }

    void printTypeMismatchError() {
        std::cout << "SNOL> Error! Operands must be the same type in an arithmetic operation!" << std::endl;
    }

    void printUndefinedError(const std::string& varName) {
        std::cout << "SNOL> Error! Undefined variable [" << varName << "]" << std::endl;
    }

    void printModuloTypeError() {
        std::cout << "SNOL> Error! Modulo operation only allowed on integers!" << std::endl;
    }

    void printDivisionByZeroError() {
        std::cout << "SNOL> Error! Division by zero!" << std::endl;
    }

    bool applyBinary(char op, const Variable& left, const Variable& right, Variable& out) {
        if (left.type != right.type) {
            printTypeMismatchError();
            return false;
        }

        if (left.type == Variable::INT_TYPE) {
            out.type = Variable::INT_TYPE;
            int a = left.intValue;
            int b = right.intValue;
            switch (op) {
                case '+': out.intValue = a + b; return true;
                case '-': out.intValue = a - b; return true;
                case '*': out.intValue = a * b; return true;
                case '/':
                    if (b == 0) {
                        printDivisionByZeroError();
                        return false;
                    }
                    out.intValue = a / b;
                    return true;
                case '%':
                    if (b == 0) {
                        printDivisionByZeroError();
                        return false;
                    }
                    out.intValue = a % b;
                    return true;
                default:
                    return false;
            }
        }

        if (op == '%') {
            printModuloTypeError();
            return false;
        }

        out.type = Variable::FLOAT_TYPE;
        float a = left.floatValue;
        float b = right.floatValue;
        switch (op) {
            case '+': out.floatValue = a + b; return true;
            case '-': out.floatValue = a - b; return true;
            case '*': out.floatValue = a * b; return true;
            case '/':
                if (std::fabs(b) < 1e-9f) {
                    printDivisionByZeroError();
                    return false;
                }
                out.floatValue = a / b;
                return true;
            default:
                return false;
        }
    }

    bool parsePrimary(const std::string& expr, Variable& result) {
        std::string t = trim(expr);
        if (t.empty()) return false;

        if (t[0] == '+' || t[0] == '-') {
            Variable inner;
            if (!parsePrimary(t.substr(1), inner)) return false;
            if (t[0] == '-') {
                if (inner.type == Variable::INT_TYPE) inner.intValue = -inner.intValue;
                else inner.floatValue = -inner.floatValue;
            }
            result = inner;
            return true;
        }

        if (t.front() == '(' && t.back() == ')') {
            int depth = 0;
            bool fullyWrapped = true;
            for (size_t i = 0; i < t.size(); ++i) {
                if (t[i] == '(') depth++;
                else if (t[i] == ')') {
                    depth--;
                    if (depth == 0 && i != t.size() - 1) {
                        fullyWrapped = false;
                        break;
                    }
                }
                if (depth < 0) return false;
            }
            if (depth != 0) return false;
            if (fullyWrapped) return parseAddSubtract(t.substr(1, t.size() - 2), result);
        }

        if (isValidVariableName(t)) {
            if (!getVariable(t, result)) {
                printUndefinedError(t);
                return false;
            }
            return true;
        }

        return parseLiteral(t, result);
    }

    bool parseMultiplyDivide(const std::string& expr, Variable& result) {
        int depth = 0;
        for (int i = static_cast<int>(expr.size()) - 1; i >= 0; --i) {
            char c = expr[i];
            if (c == ')') depth++;
            else if (c == '(') depth--;
            else if (depth == 0 && (c == '*' || c == '/' || c == '%')) {
                Variable left, right;
                if (!parseMultiplyDivide(expr.substr(0, i), left)) return false;
                if (!parsePrimary(expr.substr(i + 1), right)) return false;
                return applyBinary(c, left, right, result);
            }
        }
        return parsePrimary(expr, result);
    }

    bool parseAddSubtract(const std::string& expr, Variable& result) {
        int depth = 0;
        for (int i = static_cast<int>(expr.size()) - 1; i >= 0; --i) {
            char c = expr[i];
            if (c == ')') depth++;
            else if (c == '(') depth--;
            else if (depth == 0 && (c == '+' || c == '-')) {
                if (i == 0) continue;
                Variable left, right;
                if (!parseAddSubtract(expr.substr(0, i), left)) return false;
                if (!parseMultiplyDivide(expr.substr(i + 1), right)) return false;
                return applyBinary(c, left, right, result);
            }
        }
        return parseMultiplyDivide(expr, result);
    }

public:
    explicit SNOLParser(std::map<std::string, Variable>& vars) : variables(vars) {}

    bool evaluateExpression(const std::string& expr, Variable& result) {
        std::string trimmedExpr = trim(expr);
        if (trimmedExpr.empty()) return false;

        if (isValidVariableName(trimmedExpr)) {
            if (!getVariable(trimmedExpr, result)) {
                printUndefinedError(trimmedExpr);
                return false;
            }
            return true;
        }

        if (parseLiteral(trimmedExpr, result)) return true;

        std::string cleanExpr;
        for (char c : trimmedExpr) if (c != ' ') cleanExpr += c;
        if (cleanExpr.empty()) return false;
        return parseAddSubtract(cleanExpr, result);
    }
};

#endif
