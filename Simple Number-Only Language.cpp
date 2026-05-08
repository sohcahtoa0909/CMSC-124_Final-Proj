#include <iostream>
#include <string>
#include <map>
#include <cctype>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <cmath>

using namespace std;

// Structure to hold variable information
struct Variable {
    enum Type { INT_TYPE, FLOAT_TYPE };
    Type type;
    union {
        int intValue;
        float floatValue;
    };
};

class SNOLInterpreter {
private:
    map<string, Variable> variables;
    bool running;
    

    // Check if variable name follows SNOL rules
    bool isValidVariableName(const string& name) {
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
    bool parseLiteral(const string& token, Variable& result) {
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
    bool getVariable(const string& name, Variable& result) {
        auto it = variables.find(name);
        if (it != variables.end()) {
            result = it->second;
            return true;
        }
        return false;
    }


    // Set variable value
    void setVariable(const string& name, const Variable& value) {
        variables[name] = value;
    }
    

    // Check if variable exists
    bool variableExists(const string& name) {
        return variables.find(name) != variables.end();
    }


    // Error messages
    void printTypeMismatchError() {
        cout << "SNOL> Error! Operands must be the same type in an arithmetic operation!" << endl;
    }
    void printUndefinedError(const string& varName) {
        cout << "SNOL> Error! Undefined variable [" << varName << "]" << endl;
    }
    void printModuloTypeError() {
        cout << "SNOL> Error! Modulo operation only allowed on integers!" << endl;
    }
    void printDivisionByZeroError() {
        cout << "SNOL> Error! Division by zero!" << endl;
    }


    // Evaluate an arithmetic expression
    bool evaluateExpression(const string& expr, Variable& result) {
        // Trim whitespace
        string trimmedExpr = expr;
        size_t start = trimmedExpr.find_first_not_of(" \t");
        size_t end = trimmedExpr.find_last_not_of(" \t");
        if (start == string::npos) return false;
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
    bool evaluateArithmetic(const string& expr, Variable& result) {
        // Remove all spaces for easier parsing
        string cleanExpr;
        for (char c : expr) {
            if (c != ' ') cleanExpr += c;
        }
        return parseAddSubtract(cleanExpr, result);
    }


    bool parseAddSubtract(const string& expr, Variable& result) {

    }


    bool parseMultiplyDivide(const string& expr, Variable& result) {

    }


    bool parsePrimary(const string& expr, Variable& result) {

    }


    void handlePrint(const string& expr) {

    }
    
    void handleBeg(const string& varName) {

    }


    void handleAssignment(const string& command) {

    }


public:
    SNOLInterpreter() {
        running = true;
    }

    
    // Main interpreter loop
    void run() {
        cout << "The SNOL environment is now active, you may proceed with giving your commands." << endl;
        cout << endl;
        
        string command;
        
        while (running) {
            cout << "Command: ";
            if (!getline(cin, command)) {
                break;
            }
            
            // Skip empty commands
            if (command.empty())
                continue;
            
            // Execute command based on type
            
            // 1. Exit command
            if (command == "EXIT!") {
                cout << "Interpreter is now terminated..." << endl;
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
            else if (command.find('=') != string::npos) {
                handleAssignment(command);
            }
            // 5. Simple expression
            else {
                Variable result;
                if (!evaluateExpression(command, result)) {
                    cout << "SNOL> Unknown command! Does not match any valid command of the language." << endl;
                }
            }
        }
    }
};


// Main entry point
int main() {
    SNOLInterpreter interpreter;
    interpreter.run();
    return 0;
}