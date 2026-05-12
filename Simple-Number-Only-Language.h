#include <iostream>
#include <string>
#include <map>
#include <cctype>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <cmath>

class SNOLInterpreter {
    // Structure to hold variable information
    struct Variable {
        enum Type { INT_TYPE, FLOAT_TYPE };
        Type type;
        union {
            int intValue;
            float floatValue;
        };
    };
    
    private:
    std::map<std::string, Variable> variables;
    bool running;
    

    // Check if variable name follows SNOL rules
    bool isValidVariableName(const std::string& name) {
        // First character must be a letter
        if (name.empty() || !isalpha(name[0]))
            return false;
    
        // Remaining characters must be letters or digits
        for (size_t i = 1; i < name.length(); i++) {
            if (!isalnum(name[i]))
                return false;}
    
        // Cannot be keywords
        if (name == "BEG" || name == "PRINT" || name == "EXIT!")
            return false;
        
        return true;
    }


    // Parse integer or float literal
    bool parseLiteral(const std::string& token, Variable& result) {
        // Try parsing as integer
        char* endPtr;
        long intValue = strtol(token.c_str(), &endPtr, 10);
        if (*endPtr == '\0') {
            result.type = Variable::INT_TYPE;
            result.intValue = static_cast<int>(intValue);
            return true;
        }
        
        // Try parsing as float
        float floatValue = strtof(token.c_str(), &endPtr);
        if (*endPtr == '\0') {
            result.type = Variable::FLOAT_TYPE;
            result.floatValue = floatValue;
            return true;
        }
        
        return false;
    }


    // Get variable value
    bool getVariable(const std::string& name, Variable& result) {
        auto it = variables.find(name);
        if (it != variables.end()) {
            result = it->second;
            return true;
        }
        return false;
    }


    // Set variable value
    void setVariable(const std::string& name, const Variable& value) {
        variables[name] = value;
    }
    

    // Check if variable exists
    bool variableExists(const std::string& name) {
        return variables.find(name) != variables.end();
    }


    // Error messages
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


    // Evaluate an arithmetic expression
    bool evaluateExpression(const std::string& expr, Variable& result) {
        // Trim whitespace
        std::string trimmedExpr = expr;
        size_t start = trimmedExpr.find_first_not_of(" \t");
        size_t end = trimmedExpr.find_last_not_of(" \t");
        if (start == std::string::npos) return false;
        trimmedExpr = trimmedExpr.substr(start, end - start + 1);
        
        // Check if it's just a variable
        if (isValidVariableName(trimmedExpr)) {
            if (variableExists(trimmedExpr)) {
                result = variables[trimmedExpr];
                return true;
            } else {
                printUndefinedError(trimmedExpr);
                return false;
            }
        }
        
        // Check if it's just a literal
        if (parseLiteral(trimmedExpr, result)) {
            return true;
        }
        
        // Otherwise, evaluate as arithmetic
        return evaluateArithmetic(trimmedExpr, result);
    }
    
    // Evaluate arithmetic with precedence
    bool evaluateArithmetic(const std::string& expr, Variable& result) {
        // Remove all spaces for easier parsing
        std::string cleanExpr;
        for (char c : expr) {
            if (c != ' ') cleanExpr += c;
        }
        return parseAddSubtract(cleanExpr, result);
    }


    bool parseAddSubtract(const std::string& expr, Variable& result) {
        
    }


    bool parseMultiplyDivide(const std::string& expr, Variable& result) {

    }


    bool parsePrimary(const std::string& expr, Variable& result) {

    }


    void handlePrint(const std::string& expr) {

    }
    
    void handleBeg(const std::string& varName) {

    }


    void handleAssignment(const std::string& command) {

    }


public:
    SNOLInterpreter() {
        running = true;
    }

    
    // Main interpreter loop
    void run() {
        std::cout << "The SNOL environment is now active, you may proceed with giving your commands." << std::endl;
        std::cout << std::endl;
        
        std::string command;
        
        while (running) {
            std::cout << "Command: ";
            if (!std::getline(std::cin, command)) {
                break;
            }
            
            // Skip empty commands
            if (command.empty())
                continue;
            
            // Execute command based on type
            
            // 1. Exit command
            if (command == "EXIT!") {
                std::cout << "Interpreter is now terminated..." << std::endl;
                running = false;
            }
            // 2. PRINT command
            else if (command.substr(0, 6) == "PRINT ") {
                handlePrint(command.substr(6));
            }
            // 3. BEG command
            else if (command.substr(0, 4) == "BEG ") {
                handleBeg(command.substr(4));
            }
            // 4. Assignment command (contains =)
            else if (command.find('=') != std::string::npos) {
                handleAssignment(command);
            }
            // 5. Simple expression
            else {
                Variable result;
                if (!evaluateExpression(command, result)) {
                    std::cout << "SNOL> Unknown command! Does not match any valid command of the language." << std::endl;
                }
            }
        }
    }
};