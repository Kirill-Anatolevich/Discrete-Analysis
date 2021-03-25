#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include "additional_function.hpp"

struct TWord {
    unsigned int word;
    unsigned int lineNumber;
    unsigned int wordNumber;
};

class TApostolicoGiancarlo {
private:
    std::vector<unsigned int> pattern;
    std::vector<TWord> text;
    unsigned int tmpNumber;
    std::string tmpStrNumber;
    char tmpCharacter;
    std::string str;
    TWord word;
    std::unordered_map<unsigned int, std::vector<unsigned int>> characterOccurrence;
    unsigned int patternSize;
    std::vector<unsigned int> zFunctionRevPat;
public:
    TApostolicoGiancarlo() {
        ReadingPattern();
        ReadingText();
        RuleBadCharacter();
        RuleGoodSuffix();
    }

    void addWordToPattern(std::string &tmpS) {
        tmpNumber = std::stoi(tmpS);
        pattern.push_back(tmpNumber);
        tmpS.clear();
    }
    
    void addWordToText(std::string &tmpS, unsigned int &lineNum, unsigned int &wordNum) {
        tmpNumber = std::stoi(tmpS);
        word.word = tmpNumber;
        word.lineNumber = lineNum;
        word.wordNumber = wordNum;
        text.push_back(word);
        tmpS.clear();
    }

    void ReadingPattern() {
        std::getline(std::cin, str);
        unsigned int strSize = str.size();
        if (strSize == 0) {
            return;
        }
        for (unsigned int i = 0; i < strSize; ++i) {
            if (str[i] == ' ') {
                if (tmpStrNumber.empty()) {
                    continue;
                }
                addWordToPattern(tmpStrNumber);
            }
            else {
                tmpStrNumber.push_back(str[i]);
            }
        } 
        if (!tmpStrNumber.empty()) {
            addWordToPattern(tmpStrNumber);
        }
        //checkTheResultVec(pattern);
        str.clear();
        patternSize = pattern.size();
    }
    
    void ReadingText() {
        char c;
        unsigned int lineNumber = 0;
        unsigned int wordNumber = 0;
        c = getchar();
        while (c != EOF) {
            if (c == '\n') {
                ++lineNumber;
                wordNumber = 0;
                unsigned strSize = str.size();
                if (strSize != 0) {
                for (unsigned int i = 0; i < strSize; ++i) {
                    if (str[i] == ' ') {
                        if (tmpStrNumber.empty()) {
                            continue;
                        }
                        ++wordNumber;
                        addWordToText(tmpStrNumber, lineNumber, wordNumber);
                    }
                    else {
                        tmpStrNumber.push_back(str[i]);
                    }
                } 
                if (!tmpStrNumber.empty()) {
                    ++wordNumber;
                    addWordToText(tmpStrNumber, lineNumber, wordNumber);
                }
                str.clear();
                }
            }
            else {
                str.push_back(c);
            }
            c = getchar();
        }
        //checkTheResultText(text);
    }

    void RuleBadCharacter() {
        for (unsigned int i = 0; i < patternSize; ++i) {
            characterOccurrence[pattern[i]].push_back(i);
        }  
        //checkRuleBadCharacter(characterOccurrence, pattern);
    }
    
    void ZFunction(std::vector<unsigned int> vec) {
        for (unsigned int l = 0, r = 0, i = 1; i < patternSize; ++i) {
            if (i <= r) {
                zFunctionRevPat[i] = std::min(z[i - l], r - i + 1);
            }
            while (i + zFunctionRevPat[i] < patternSize && vec[zFunctionRevPat[i]] == vec[i + zFunctionRevPat[i]]) {
                ++zFunctionRevPat[i];
            }
            if (i + zFunctionRevPat[i] - 1 > r) {
                l = i;
                r = i + zFunctionRevPat[i] - 1;
            }
       } 
    }

    void RuleGoodSuffix() {
        std::vector<unsigned int> reversedPattern = pattern;
        std::reverse(reversedPattern.begin(), reversedPattern.end());
        checkTheResultVec(reversedPattern);
    }
};

int main() {
    TApostolicoGiancarlo tmp;
}
