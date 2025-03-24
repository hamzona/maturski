#pragma once
#include <string>
#include <vector>
struct question
{
    std::string question;
    std::string answers[4];
    int rightAnswer;
};
extern std::vector<question> data;
