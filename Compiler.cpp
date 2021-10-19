#include "Compiler.h"

Compiler::Compiler(std::string tokensPath, std::string syntaxPath, std::string programPath) {
    this->tokensPath = tokensPath;
    this->syntaxPath = syntaxPath;
    this->programPath = programPath;

    this->codes.push_back("id");
    this->codes.push_back("constant");

    this->readTokens();
    this->readRules();
}

void Compiler::readTokens() {
    // file structure
    // first line: reserved words
    // second line: operators
    // third line: separators
    std::ifstream file(this->tokensPath);
    std::string reservedWordsString = "";
    std::string operatorsString = "";
    std::string separatorsString = "";

    std::getline(file, reservedWordsString);
    std::getline(file, operatorsString);
    std::getline(file, separatorsString);

    this->reservedWords = this->splitString(reservedWordsString, ' ');
    this->operators = this->splitString(operatorsString, ' ');
    this->separators = this->splitString(separatorsString, ' ');

    // addings reserved words, operators and separators to codes vector
    std::cout << "[Codifying reserved words, operators and separators ...]\n";
    for (auto rw : this->reservedWords) {
        this->codes.push_back(rw);
    }
    for (auto op : this->operators) {
        this->codes.push_back(op);
    }
    for (auto sep : this->separators) {
        this->codes.push_back(sep);
    }
    std::cout << "[... done]\n";

    file.close();
}

void Compiler::readRules() {
    // to do - how to interpret rules?
}

std::vector<std::string> Compiler::splitString(std::string str, char delim) {
    std::vector<std::string> result;
    std::istringstream iss(str);
    std::string item;
    while (std::getline(iss, item, delim)) {
        result.push_back(item);
    }
    return result;
}

bool Compiler::findInVector(std::vector<std::string> vec, std::string elem) {
    for (auto v : vec) {
        if (v == elem) {
            return true;
        }
    }
    return false;
}

std::string Compiler::readNextToken(std::ifstream& file, bool& isNewLine, int currentLine) {
    std::string token = "";
    char ch;
    bool isChar = false;
    bool foundEndCharQuote = false;
    bool isString = false;
    bool foundEndStringQuote = false;
    while (file >> std::noskipws >> ch) {
        if (ch == '"') {
            if (isString == false) {
                isString = true;
            }
            else {
                foundEndStringQuote = true;
                token += ch;
                break;
            }
            token += ch;
            continue;
        }
        if (ch == '\'') {
            if (isChar == false) {
                isChar = true;
            }
            else {
                foundEndCharQuote = true;
                token += ch;
                break;
            }
            token += ch;
            continue;
        }
        if (ch != ' ' && ch != '\n') {
            token += ch;
            continue;
        }
        else {
            if (ch == '\n') {
                isNewLine = true;
            }
            if (isString && !foundEndStringQuote) {
                token += " ";
                continue;
            }
            if (isChar && !foundEndCharQuote) {
                this->logError("[LEXICAL ERROR at line " + std::to_string(currentLine) + ": token `" + token + "` is missing a closing '.]\n");
            }
            return token;
        }
    }
    if (isString && !foundEndStringQuote) {
        this->logError("[LEXICAL ERROR at line " + std::to_string(currentLine) + ": token `" + token + "` is missing a closing \".]\n");
        
    }
    if (isChar && !foundEndCharQuote) {
        this->logError("[LEXICAL ERROR at line " + std::to_string(currentLine) + ": token `" + token + "` is missing a closing '.]\n");
    }
    return token;

    // check for strings
}

std::string Compiler::readNextTokenV2(std::ifstream& file, bool& isNewLine) {
    // improvements:
    // - stop building a token once hitting a separator character
    std::string token = "";
    char ch;
    std::string chAsString;
    while (file >> std::noskipws >> ch) {
        chAsString = ch;
        if (this->getIsSeparator(chAsString) || ch == ' ' || ch == '\n') {
            if (ch == '\n') {
                isNewLine = true;
            }
            if (this->getIsSeparator(chAsString)) {
                if (token == "") {
                    return chAsString;
                }
            }
            // to fix the "a)" situation, use look ahead when you are at "a" by keeping two character buffers
            // current and previous
            return token;
        }
        else {
            token += ch;
        }
        if (this->getIsReservedWord(token)) {
            return token;
        }
        if (this->getIsOperator(token)) {
            // to do: lookahead
            return token;
        }
    }
    return token;
}

void Compiler::scan() {
    std::cout << "[Scanning ...]\n";
    std::ifstream file(this->programPath);
    std::string token;
    bool isNewLine = false;
    int currentLine = 1;
    while (true) {
        if (this->getHasError()) {
            break;
        }
        token = this->readNextToken(file, isNewLine, currentLine);
        if (file.eof()) {
            break;
        }
        if (token == "") {
            continue;
        }
        std::cout << token << " on line " << currentLine <<  '\n';

        if (this->getIsReservedWord(token) || this->getIsSeparator(token) || this->getIsOperator(token)) {
            PifPair pair;
            pair.code = this->getCode(token);
            pair.value = std::pair<int,int>(-1, -1);

            if (pair.code == -1) {
                this->logError("[INNER ERROR at line " + std::to_string(currentLine) + ": Token `" + token + "` is a reserved word, separator or operator and it could not have been found in the codes table.");
            }
            
            this->pif.push_back(pair);
        }
        else {
            // check if token is id or constant
            if (this->getIsIdentifier(token) || this->getIsConstant(token)) {
                // check if token is inside symbolTable
                std::pair<int, int> pos;
                if (this->symbolTable->exists(token)) {
                    // find its position
                    pos = this->symbolTable->getPosition(token);
                }
                else {
                    // insert it
                    pos = this->symbolTable->insert(token);
                }
                // add to pif
                PifPair pifPair;
                if (this->getIsConstant(token)) {
                    pifPair.code = 1;
                }
                else if (this->getIsIdentifier(token)) {
                    pifPair.code = 0;
                }
                else {
                    this->logError("[LEXICAL ERROR at line " + std::to_string(currentLine) + ": token `" + token + "` is not an identifier, nor a constant.]\n");
                }
                
                pifPair.value = pos;
                this->pif.push_back(pifPair);
            }
            else {
                this->logError("[LEXICAL ERROR at line " + std::to_string(currentLine) + ": token `" + token + "` could not be classified.]\n");
            }
        }
        if (isNewLine) {
            currentLine++;
            isNewLine = false;
        }
    }
    file.close();
    std::cout << "[... done]\n";
}

void Compiler::displayCodes() {
    std::cout << "[Codes table ...]\n";
    for (int i = 0; i < this->codes.size(); i++) {
        std::cout << i << "   ";
        if (i < 10) {
            std::cout << ' ';
        }
        std::cout << this->codes[i] << '\n';
    }
    std::cout << "[... done]\n";
}


void Compiler::displayPif() {
    std::cout << "[Pif table ...]\n";
    std::cout << "Code - (bucket, position in bucket)\n";
    for (int i = 0; i < this->pif.size(); i++) {
        std::cout << this->pif[i].code << "   ";
        if (this->pif[i].code < 10) {
            std::cout << ' ';
        }
        std::cout << '(' << this->pif[i].value.first << ',' << this->pif[i].value.second << ')' << '\n';
    }
    std::cout << "[... done]\n";
}

void Compiler::displaySymbolTable() {
    this->symbolTable->print();
}

void Compiler::logError(std::string error) {
    std::cout << error;
    this->encounteredError = error;
}

void Compiler::writeToFiles(std::string pifFileName, std::string stFileName, std::string correctnessFileName) {
    this->symbolTable->toFile(stFileName);

    std::ofstream file(pifFileName);
    file << "Code - (bucket, position in bucket)\n";
    for (int i = 0; i < this->pif.size(); i++) {
        file << this->pif[i].code << "   ";
        if (this->pif[i].code < 10) {
            file << ' ';
        }
        file << '(' << this->pif[i].value.first << ',' << this->pif[i].value.second << ')' << '\n';
    }
    
    std::ofstream file1(correctnessFileName);
    if (this->getHasError()) {
        file1 << this->encounteredError;
    }
    else {
        file1 << "Lexically correct";
    }

    file1.close();
    file.close();
}

// getters
int Compiler::getCode(std::string token) {
    for (int i = 0; i < this->codes.size(); i++) {
        if (this->codes.at(i) == token) {
            return i;
        }
    }
    return -1;
}

std::vector<std::string> Compiler::getReservedWords(){
    return this->reservedWords;
}

std::vector<std::string> Compiler::getSeparators() {
    return this->separators;
}

std::vector<std::string> Compiler::getOperators() {
    return this->operators;
}

bool Compiler::getIsReservedWord(std::string token) {
    return this->findInVector(this->reservedWords, token);
}

bool Compiler::getIsSeparator(std::string token) {
    return this->findInVector(this->separators, token);
}

bool Compiler::getIsOperator(std::string token) {
    return this->findInVector(this->operators, token);
}

bool Compiler::getIsIdentifier(std::string token) {
    // to do
    // 2A should not be an identifier
    // or unknown chars $
    return true;
}

bool Compiler::getIsConstant(std::string token) {
    if (token.empty()) {
        return false;
    }
    // check for 'aa'
    if (token[0] == '\'' || token[0] == '\"' || canBeNumber(token))
    {
        return true;
    }
    return false;
}

bool Compiler::canBeNumber(std::string str) {
    // shouldn't start with 0
    // make -12 work
    for (auto ch : str) {
        if (ch < '0' || ch > '9') {
            return false;
        }
    }
    return true;
}

bool Compiler::getHasError() {
    return this->encounteredError != "";
}