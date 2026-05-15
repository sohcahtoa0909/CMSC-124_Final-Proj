#ifndef SNOL_H
#define SNOL_H

#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <cctype>

#include "parser.h"
#include "variable.h"

class SNOLInterpreter {
private:
    std::map<std::string, Variable> variables;
    bool running;
    SNOLParser parser;

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

    void handlePrint(const std::string& expr) {
        Variable result;
        if (!parser.evaluateExpression(expr, result)) {
            std::cout << "SNOL> Error! Invalid PRINT expression." << std::endl;
            return;
        }
        std::cout << "SNOL> ";
        if (result.type == Variable::INT_TYPE) std::cout << result.intValue;
        else std::cout << std::fixed << std::setprecision(4) << result.floatValue;
        std::cout << std::endl;
    }

    void handleBeg(const std::string& varName) {
        std::string name = trim(varName);

        if (!isValidVariableName(name)) {
            std::cout << "SNOL> Error! Invalid variable name." << std::endl;
            return;
        }
        std::cout << "SNOL> Please enter value for [" << name << "]: " << std::endl;
        std::cout << "Input: ";
        std::string input;
        std::getline(std::cin, input);

        Variable value;

        if (!parser.evaluateExpression(input, value)) {
            std::cout << "SNOL> Error! Invalid number format." << std::endl;
            return;
        }
        variables[name] = value;
    }

    void handleAssignment(const std::string& command) {
        size_t eqPos = command.find('=');
        if (eqPos == std::string::npos) {
            std::cout << "SNOL> Error! Invalid assignment syntax." << std::endl;
            return;
        }

        std::string left = trim(command.substr(0, eqPos));
        std::string right = trim(command.substr(eqPos + 1));
        if (!isValidVariableName(left)) {
            std::cout << "SNOL> Error! Invalid variable name." << std::endl;
            return;
        }

        Variable result;
        if (!parser.evaluateExpression(right, result)) {
            std::cout << "SNOL> Error! Invalid assignment expression." << std::endl;
            return;
        }
        variables[left] = result;
    }

public:
    SNOLInterpreter() : running(true), parser(variables) {}

    void run() {
        std::cout << "The SNOL environment is now active, you may proceed with giving your commands." << std::endl;
        std::cout << std::endl;

        std::string command;
        while (running) {
            std::cout << "Command: ";
            if (!std::getline(std::cin, command)) break;
            if (command.empty()) continue;

            if (command == "EXIT!") {
                std::cout << "Interpreter is now terminated..." << std::endl;
                running = false;
            } else if (command.substr(0, 6) == "PRINT ") {
                handlePrint(command.substr(6));
            } else if (command.substr(0, 4) == "BEG ") {
                handleBeg(command.substr(4));
            } else if (command.find('=') != std::string::npos) {
                handleAssignment(command);
            } else {
                Variable result;
                if (!parser.evaluateExpression(command, result)) {
                    std::cout << "SNOL> Unknown command! Does not match any valid command of the language." << std::endl;
                }
            }
        }
    }
};

#endif
