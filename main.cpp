#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include "additional_function.hpp"

struct TWord {
    unsigned int word;
    unsigned int lineNumer;
    unsigned int wordNumber;
};

class TApostolicoGiancarlo {
    TApostolicoGiancarlo() {
        ReadingPattern();
        ReadingText();
        RuleBadCharacter();
        RuleGoodSuffix();
    }
};

int main() {
    std::vector<unsigned int> a;
    a = {1, 2, 3};
    checkTheResultVec(a);
}
