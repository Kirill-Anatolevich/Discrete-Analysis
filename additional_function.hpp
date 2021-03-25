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
