#ifndef SUPPORTMETHODS_H
#define SUPPORTMETHODS_H
#include <tuple>
#include "RGBA.h"
#include <iostream>
#include <vector>
#include <math.h>
#include <algorithm>
#include <assert.h>
#include "iostream"
#include <memory>
#include <regex>
#include "l-systems/lnode.h"


// TODO make this work
const int MAX_SUPER_DEPTH = 20;

class SupportMethods
{
public:
    SupportMethods();
    static RGBA blendRGBA(RGBA canvas, RGBA pixel, float mask);
    static std::vector<std::string> split (std::string s, std::string delimiter);
    static bool fEquals(float x, float y, float difference=.00001);
    static bool fLT(float x, float y);
    static bool fGT(float x, float y);
    static float clip(float n, float lower, float upper);
    static std::string get_file_contents(const char *filename);
    static std::vector<std::string> splitRegex(std::string s, std::regex r);
    static std::vector<std::string> splitMatchesRegex(std::string s, std::regex r);
//    static std::string parseParen(std::string s, LRule *lineRule);
    static float parseIntoFloat(std::string s, std::unordered_map<std::string, float> variables);
    static float shuntingYard(std::string s);




};



#endif // SUPPORTMETHODS_H
