#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>

const unsigned int UNDEFINED = -1;

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
    std::vector<unsigned int> L;
    std::vector<unsigned int> l;
    std::vector<unsigned int> N;
public:
    TApostolicoGiancarlo() {
        ReadingPattern();
        if (patternSize == 0)
            return;
        ReadingText();
        RuleBadCharacter();
        RuleGoodSuffix();
        Search();
    }

    void addWordToPattern(std::string& tmpS) {
        tmpNumber = std::stoull(tmpS);
        pattern.push_back(tmpNumber);
        tmpS.clear();
    }

    void addWordToText(std::string& tmpS, unsigned int& lineNum, unsigned int& wordNum) {
        tmpNumber = std::stoull(tmpS);
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
            patternSize = 0;
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
        str.clear();
        patternSize = pattern.size();
    }

    void ReadingText() {
        char c;
        unsigned int lineNumber = 0;
        unsigned int wordNumber = 0;
        c = getchar();
        if (patternSize == 0) {
            return;
        }
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
    }

    void RuleBadCharacter() {
        for (unsigned int i = 0; i < patternSize; ++i) {
            characterOccurrence[pattern[i]].push_back(i);
        }
    }

    void ZFunction(std::vector<unsigned int> vec) {
        zFunctionRevPat = std::vector<unsigned int>(patternSize);
        for (unsigned int l = 0, r = 0, i = 1; i < patternSize; ++i) {
            if (i <= r) {
                zFunctionRevPat[i] = std::min(zFunctionRevPat[i - l], r - i + 1);
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
        N = std::vector<unsigned int>(patternSize);
        L = std::vector<unsigned int>(patternSize, 0);
        l = std::vector<unsigned int>(patternSize + 1, 0);
        ZFunction(reversedPattern);
        for (unsigned int i = 0; i < patternSize; ++i) {
            N[i] = zFunctionRevPat[patternSize - i - 1];
        }
        unsigned int j;
        for (unsigned int i = 0; i < patternSize - 1; ++i) {
            j = patternSize - N[i];
            if (j == patternSize) {
                continue;
            }
            L[j] = i;
        }
        for (unsigned int i = patternSize, j = 0; i > 0; --i) {
            j = patternSize - i + 1;
            if (N[j - 1] == j) {
                l[i - 1] = j;
            }
            else {
                l[i - 1] = l[i];
            }
        }
    }

    unsigned int MaxShift(unsigned int a, unsigned int b) {
        return (a > b) ? a : b;
    }

    unsigned int ShiftRuleBadCharacter(unsigned int word, unsigned int index) {
        if (characterOccurrence[word].size() == 0 || characterOccurrence[word][0] > index) {
            return (index + 1);
        }
        unsigned int rightmost = characterOccurrence[word][0];
        for (unsigned int i = 1; i < characterOccurrence[word].size(); ++i) {
            if (characterOccurrence[word][i] > index) {
                return index - rightmost;
            }
            rightmost = characterOccurrence[word][i];
        }
        return index - rightmost;
    }

    unsigned int ShiftRuleGoodSuffix(unsigned int j) {
        if (j == patternSize - 1) {
            return 1;
        }
        if (L[j + 1] == 0) {
            if (l[j + 1] == 0) {
                return 1;
            }
            return patternSize - l[j + 1];
        }
        return patternSize - L[j + 1] - 1;
    }

    void Search() {
        unsigned int textSize = text.size();
        if (textSize == 0) {
            return;
        }
        std::vector<unsigned int> M(textSize, UNDEFINED);
        unsigned int i = patternSize - 1;
        unsigned int j = patternSize - 1;
        unsigned int startComparison = i;
        while (i <= textSize - 1) {
            if (M[i] == UNDEFINED || (M[i] == N[j] && M[i] == 0)) {
                if (text[i].word == pattern[j]) {
                    if (j == 0) {
                        std::cout << text[i].lineNumber << ", " << text[i].wordNumber << "\n";
                        M[startComparison] = patternSize;
                        i = startComparison + (patternSize - l[j + 1]);
                        startComparison = i;
                        j = patternSize - 1;
                        continue;
                    }
                    --i;
                    --j;
                }
                else {
                    M[startComparison] = startComparison - i;
                    i = (startComparison + MaxShift(ShiftRuleBadCharacter(text[i].word, j), ShiftRuleGoodSuffix(j)));
                    j = patternSize - 1;
                    startComparison = i;
                }
            }
            else if (M[i] < N[j]) {
                j -= M[i];
                i -= M[i];
            } 
            else if (M[i] >= N[j] && N[j] == j + 1) {
                M[startComparison] = startComparison - j;
                i = startComparison + 1 - patternSize;
                std::cout << text[i].lineNumber << ", " << text[i].wordNumber << "\n";
                i = startComparison + (patternSize - l[1]);
                startComparison = i;
                j = patternSize - 1;
            }
            else if (M[i] > N[j] && N[j] < j + 1) {
                M[startComparison] = startComparison - i;
                i = (startComparison + MaxShift(ShiftRuleBadCharacter(text[i].word, j), ShiftRuleGoodSuffix(j - N[j])));
                startComparison = i;
                j = patternSize - 1;
            }
            else if (M[i] == N[j] && (N[j] > 0 && N[j] < j + 1)) {
                j -= M[i];
                i -= M[i];
            }
        }
    }
};

int main() {
    TApostolicoGiancarlo tmp;
}
