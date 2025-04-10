#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <cctype>
using namespace std;

struct Instruction {
    string op, arg1, arg2, result;
};
vector<Instruction> icg;

// Token types
enum TokenType { KEYWORD, IDENTIFIER, NUMBER, SYMBOL };

struct Token {
    TokenType type;
    string value;
};

map<string, TokenType> keywords = {
    {"int", KEYWORD},
    {"if", KEYWORD},
    {"else", KEYWORD},
    {"while", KEYWORD}
};

vector<Token> tokenize(const string& line) {
    vector<Token> tokens;
    string token = "";
    for (char ch : line) {
        if (isalnum(ch) || ch == '_') {
            token += ch;
        } else {
            if (!token.empty()) {
                if (keywords.count(token)) tokens.push_back({KEYWORD, token});
                else if (isdigit(token[0])) tokens.push_back({NUMBER, token});
                else tokens.push_back({IDENTIFIER, token});
                token = "";
            }
            if (!isspace(ch)) tokens.push_back({SYMBOL, string(1, ch)});
        }
    }
    if (!token.empty()) {
        if (keywords.count(token)) tokens.push_back({KEYWORD, token});
        else if (isdigit(token[0])) tokens.push_back({NUMBER, token});
        else tokens.push_back({IDENTIFIER, token});
    }
    return tokens;
}

bool isIdentifier(const Token& t) { return t.type == IDENTIFIER; }
bool isNumber(const Token& t) { return t.type == NUMBER; }
bool isSymbol(const Token& t, const string& val) { return t.type == SYMBOL && t.value == val; }

int tempCount = 0;
int labelCount = 0;
string newTemp() { return "t" + to_string(tempCount++); }
string newLabel() { return "L" + to_string(labelCount++); }

void parseBlock(const vector<Token>& tokens, size_t& i, map<string, string>& symbolTable);

void parse(const vector<Token>& tokens, map<string, string>& symbolTable) {
    size_t i = 0;
    parseBlock(tokens, i, symbolTable);
}

void parseBlock(const vector<Token>& tokens, size_t& i, map<string, string>& symbolTable) {
    while (i < tokens.size()) {
        if (tokens[i].type == KEYWORD && tokens[i].value == "int") {
            string var = tokens[i + 1].value;
            if (i + 2 < tokens.size() && isSymbol(tokens[i + 2], ";")) {
                symbolTable[var] = "int";
                cout << "Declared variable: " << var << " of type int\n";
                i += 3;
            } else if (i + 6 < tokens.size() && isSymbol(tokens[i + 2], "=") && (isNumber(tokens[i + 3]) || isIdentifier(tokens[i + 3])) && isSymbol(tokens[i + 4], "+") && (isNumber(tokens[i + 5]) || isIdentifier(tokens[i + 5])) && isSymbol(tokens[i + 6], ";")) {
                string left = tokens[i + 3].value;
                string right = tokens[i + 5].value;
                string temp = newTemp();
                icg.push_back({"ADD", left, right, temp});
                icg.push_back({"MOV", temp, "", var});
                symbolTable[var] = "int";
                cout << "Declared variable: " << var << " of type int\n";
                cout << temp << " = " << left << " + " << right << endl;
                cout << var << " = " << temp << endl;
                i += 7;
            } else if (i + 4 < tokens.size() && isSymbol(tokens[i + 2], "=") && (isNumber(tokens[i + 3]) || isIdentifier(tokens[i + 3])) && isSymbol(tokens[i + 4], ";")) {
                symbolTable[var] = "int";
                icg.push_back({"MOV", tokens[i + 3].value, "", var});
                cout << "Declared variable: " << var << " of type int\n";
                cout << var << " = " << tokens[i + 3].value << endl;
                i += 5;
            } else {
                cerr << "Syntax error in declaration\n";
                return;
            }
        } else if (tokens[i].type == KEYWORD && tokens[i].value == "if") {
            string falseLabel = newLabel();
            if (i + 6 < tokens.size() && isSymbol(tokens[i + 1], "(") && isIdentifier(tokens[i + 2]) && isSymbol(tokens[i + 3], ">") && isNumber(tokens[i + 4]) && isSymbol(tokens[i + 5], ")") && isSymbol(tokens[i + 6], "{")) {
                string condVar = tokens[i + 2].value;
                string condVal = tokens[i + 4].value;
                icg.push_back({"JLE", condVar, condVal, falseLabel});
                i += 7;
                parseBlock(tokens, i, symbolTable);
                icg.push_back({"LABEL", "", "", falseLabel});
            }
        } else if (tokens[i].type == KEYWORD && tokens[i].value == "while") {
            string startLabel = newLabel();
            string falseLabel = newLabel();
            if (i + 6 < tokens.size() && isSymbol(tokens[i + 1], "(") && isIdentifier(tokens[i + 2]) && isSymbol(tokens[i + 3], "<") && isNumber(tokens[i + 4]) && isSymbol(tokens[i + 5], ")") && isSymbol(tokens[i + 6], "{")) {
                string condVar = tokens[i + 2].value;
                string condVal = tokens[i + 4].value;
                icg.push_back({"LABEL", "", "", startLabel});
                icg.push_back({"JGE", condVar, condVal, falseLabel});
                i += 7;
                parseBlock(tokens, i, symbolTable);
                icg.push_back({"JMP", "", "", startLabel});
            }
        } else if (isIdentifier(tokens[i])) {
            string var = tokens[i].value;
            if (i + 5 < tokens.size() && isSymbol(tokens[i + 1], "=") && (isNumber(tokens[i + 2]) || isIdentifier(tokens[i + 2])) && isSymbol(tokens[i + 3], "+") && (isNumber(tokens[i + 4]) || isIdentifier(tokens[i + 4])) && isSymbol(tokens[i + 5], ";")) {
                string left = tokens[i + 2].value;
                string right = tokens[i + 4].value;
                string temp = newTemp();
                icg.push_back({"ADD", left, right, temp});
                icg.push_back({"MOV", temp, "", var});
                cout << temp << " = " << left << " + " << right << endl;
                cout << var << " = " << temp << endl;
                i += 6;
            } else if (i + 3 < tokens.size() && isSymbol(tokens[i + 1], "=") && (isNumber(tokens[i + 2]) || isIdentifier(tokens[i + 2])) && isSymbol(tokens[i + 3], ";")) {
                icg.push_back({"MOV", tokens[i + 2].value, "", var});
                cout << var << " = " << tokens[i + 2].value << endl;
                i += 4;
            } else {
                cerr << "Syntax error in assignment\n";
                return;
            }
        } else if (isSymbol(tokens[i], "}")) {
            i++;
            return;
        } else {
            cerr << "Unknown statement\n";
            return;
        }
    }
}

vector<Instruction> optimizeICG(const vector<Instruction>& original) {
    vector<Instruction> optimized;
    map<string, string> valueMap;
    map<string, string> lastAssigned;
    for (const auto& inst : original) {
        if (inst.op == "ADD" && isdigit(inst.arg1[0]) && isdigit(inst.arg2[0])) {
            int result = stoi(inst.arg1) + stoi(inst.arg2);
            valueMap[inst.result] = to_string(result);
            optimized.push_back({"MOV", to_string(result), "", inst.result});
            continue;
        }
        if (inst.op == "MOV" && valueMap.count(inst.arg1)) {
            optimized.push_back({"MOV", valueMap[inst.arg1], "", inst.result});
            valueMap[inst.result] = valueMap[inst.arg1];
            continue;
        }
        string exprKey = inst.op + ":" + inst.arg1 + ":" + inst.arg2;
        if (lastAssigned[inst.result] == exprKey) {
            continue;
        }
        optimized.push_back(inst);
        if (inst.op == "MOV" && !isdigit(inst.arg1[0])) {
            valueMap[inst.result] = inst.arg1;
        } else {
            valueMap.erase(inst.result);
        }
        lastAssigned[inst.result] = exprKey;
    }
    return optimized;
}

void generateAssembly(const vector<Instruction>& icg) {
    cout << "\nGenerated Assembly:\n";
    int regCount = 1;
    for (auto& inst : icg) {
        if (inst.op == "ADD") {
            cout << "MOV R" << regCount << ", " << inst.arg1 << endl;
            cout << "ADD R" << regCount << ", " << inst.arg2 << endl;
            cout << "MOV " << inst.result << ", R" << regCount << endl;
            regCount++;
        } else if (inst.op == "MOV") {
            cout << "MOV " << inst.result << ", " << inst.arg1 << endl;
        } else if (inst.op == "JLE" || inst.op == "JGE") {
            cout << inst.op << " " << inst.arg1 << ", " << inst.arg2 << ", " << inst.result << endl;
        } else if (inst.op == "LABEL") {
            cout << inst.result << ":" << endl;
        } else if (inst.op == "JMP") {
            cout << "JMP " << inst.result << endl;
        }
    }
}

int main() {
    ifstream file("test.c");
    if (!file) {
        cerr << "Cannot open test.c\n";
        return 1;
    }

    string line;
    map<string, string> symbolTable;
    while (getline(file, line)) {
        vector<Token> tokens = tokenize(line);
        parse(tokens, symbolTable);
    }

    vector<Instruction> optimizedICG = optimizeICG(icg);

    cout << "\nOptimized ICG:\n";
    for (const auto& inst : optimizedICG) {
        if (inst.op == "ADD")
            cout << inst.result << " = " << inst.arg1 << " + " << inst.arg2 << endl;
        else if (inst.op == "MOV")
            cout << inst.result << " = " << inst.arg1 << endl;
        else if (inst.op == "JLE" || inst.op == "JGE")
            cout << inst.op << " " << inst.arg1 << ", " << inst.arg2 << " -> " << inst.result << endl;
        else if (inst.op == "LABEL")
            cout << inst.result << ":" << endl;
        else if (inst.op == "JMP")
            cout << "JMP " << inst.result << endl;
    }

    generateAssembly(optimizedICG);
    return 0;
}
