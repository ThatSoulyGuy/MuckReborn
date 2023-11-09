#ifndef PAIR_HPP
#define PAIR_HPP

#include <iostream>
#include <unordered_map>
#include <string>

template<typename T, typename A>
struct Pair
{
	std::unordered_map<std::string, T> dynamicVariables1;
    std::unordered_map<std::string, A> dynamicVariables2;

    void SetVariable(const std::string& name, T value) 
    {
        dynamicVariables1[name] = value;
    }

    void SetVariable(const std::string& name, A value)
    {
        dynamicVariables2[name] = value;
    }

    T GetVariable1(const std::string& name) 
    {
        if (dynamicVariables1.find(name) != dynamicVariables1.end()) 
            return dynamicVariables1[name];
        else 
        {
            std::cerr << "Variable not found!" << std::endl;
            return -1;
        }
    }

    A GetVariable2(const std::string& name)
    {
        if (dynamicVariables2.find(name) != dynamicVariables2.end())
            return dynamicVariables2[name];
        else
        {
            std::cerr << "Variable not found!" << std::endl;
            return -1;
        }
    }
};

#endif // !PAIR_HPP