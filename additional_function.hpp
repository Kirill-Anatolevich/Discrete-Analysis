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
