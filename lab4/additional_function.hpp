#pragma once

#include <iostream>

template<class T>
void checkTheResultVec(std::vector<T> vec) {
    unsigned int lengthVec = vec.size();
    std::cout<<"\n---\n";
    std::cout<<"Size:"<<lengthVec<<"\n";
    for (unsigned int i = 0; i < lengthVec; ++i) {
        std::cout<<vec[i]<<' ';
    }
    std::cout<<"\n---\n";
}

template<class T>
void checkTheResultText(std::vector<T> text) {
    std::cout<<"\n---\n";
    unsigned int lengthText = text.size();
    std::cout<<"Size:"<<lengthText<<"\n";
    for (unsigned int i = 0; i < text.size(); ++i) {
        std::cout<<text[i].word<<' '<<text[i].lineNumber<<" "<<text[i].wordNumber<<'\n';
    } 
    std::cout<<"\n---\n";
}

void checkRuleBadCharacter(std::unordered_map<unsigned int, std::vector<unsigned int>> map, std::vector<unsigned int> pattern) {
    std::cout<<"\n---\n";
    unsigned int lengthMap = map.size();
    std::cout<<"Size:"<<lengthMap<<'\n';
    for (unsigned int i = 0; i < pattern.size(); ++i) {
        std::cout<<pattern[i]<<":";
        for (unsigned int j = 0; j < map[pattern[i]].size(); ++j) {
            std::cout<<map[pattern[i]][j]<<' '; 
        }
        std::cout<<'\n';
    }
    std::cout<<"\n---\n";
}
