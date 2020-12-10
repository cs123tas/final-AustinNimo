#include "SupportMethods.h"
#include "math.h"
#include <sstream>
#include <fstream>
#include <cerrno>
#include "shunting-yard.cpp"
#include <QString>
#include <QFile>
#include <QTextStream>
SupportMethods::SupportMethods()
{

}

// Blend two RGBA values together using a mask for the second one
RGBA SupportMethods::blendRGBA(RGBA canvas, RGBA pixel, float mask) {
    // Need some 255 math here
    int red = round(canvas.r * (1.0 - (mask * pixel.a / 255.0)) + (mask * pixel.r * pixel.a / 255.0));
    int green = round(canvas.g * (1.0 - (mask * pixel.a / 255.0)) + (mask * pixel.g * pixel.a / 255.0));
    int blue = round(canvas.b * (1.0 - (mask * pixel.a / 255.0)) + (mask * pixel.b * pixel.a / 255.0));
    RGBA returnRGBA = RGBA(red, green, blue, 255);
    return returnRGBA;
}

bool SupportMethods::fEquals(float x, float y, float difference) {
    return abs(x - y) < difference;
}

bool SupportMethods::fLT(float x, float y) {
    return x - .00001 < y;
}

bool SupportMethods::fGT(float x, float y) {
    return y < x + .00001;
}

float SupportMethods::clip(float n, float lower, float upper) {
  return std::max(lower, std::min(n, upper));
}

// Split a string into components
std::vector<std::string> SupportMethods::split (std::string s, std::string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find (delimiter, pos_start)) != std::string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}

std::string SupportMethods::get_file_contents(const char *filename)
{
  std::ifstream in(filename, std::ios::in | std::ios::binary);
  if (in)
  {
    std::ostringstream contents;
    contents << in.rdbuf();
    in.close();
    std::string returnString = (contents.str());
    returnString.erase(remove(returnString.begin(), returnString.end(), ' '), returnString.end());
    return returnString;
  }
  throw(errno);
}

std::string SupportMethods::get_file_contents(QString filename)
{

    QFile inputFile(filename);
    if (inputFile.open(QIODevice::ReadOnly)) {
        QTextStream in(&inputFile);
        QString qLine = in.readAll();
        std::string returnString = qLine.toUtf8().constData();
        returnString.erase(remove(returnString.begin(), returnString.end(), ' '), returnString.end());
        return returnString;
    }
    throw(errno);
}


// Split a string into sections with any characters seperated.
std::vector<std::string> splitWords (std::string s) {

    std::regex split = std::regex("([a-zA-Z]+|[^a-zA-Z]+)");
    std::vector<std::string> matches = SupportMethods::splitMatchesRegex(s, split);
    return matches;
}

// Remove variables from a string and replace them with their value
std::string removeVariables(std::string s, std::unordered_map<std::string, float> variables) {
    std::vector<std::string> words = splitWords(s);
    std::string returnString = "";
    for (int i = 0; i < (int)words.size(); i++) {
        auto search = variables.find(words[i]);
        if(search != variables.end()) {
            words[i] = std::to_string(search->second);
        }
        returnString.append(words[i]);
    }
    return returnString;
}

float SupportMethods::shuntingYard(std::string s) {
    packToken result = calculator::calculate(s.data());
    return result.asDouble();
}

float SupportMethods::random(float a, float b) {
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

// First, replace any variables with the string of their value. Then, parse the rest.
float SupportMethods::parseIntoFloat(std::string s, std::unordered_map<std::string, float> variables) {
    std::string varsRemovedString = removeVariables(s, variables);

    std::size_t found = varsRemovedString.find_first_of("(");
    std::string ruleType = varsRemovedString.substr(0, found);
    std::string rest = varsRemovedString.substr(found + 1);
    if(ruleType == "random") {
        std::regex splitColon = std::regex("(:)");
        std::vector<std::string> minMax = SupportMethods::splitRegex(rest, splitColon);
        float min = parseIntoFloat(minMax[0], variables);
        float max = parseIntoFloat(minMax[1].substr(0, minMax[1].size() - 1), variables);
        float evaluation = random(min, max);
        return evaluation;
    }
    // TODO add randomness
    float evaluation = shuntingYard(varsRemovedString);

    return evaluation;
}

std::vector<std::string> SupportMethods::splitRegex(std::string s, std::regex r)
{
  std::vector<std::string> splits;
  std::smatch m; // <-- need a match object

  while (regex_search(s, m, r))  // <-- use it here to get the match
  {
    int split_on = m.position(); // <-- use the match position
    splits.push_back(s.substr(0, split_on));
    s = s.substr(split_on + m.length()); // <-- also, skip the whole match
  }

  if(!s.empty()) {
    splits.push_back(s); // and there may be one last token at the end
  }

  return splits;
}

std::vector<std::string> SupportMethods::splitMatchesRegex(std::string s, std::regex r)
{
  std::vector<std::string> splits;
  std::smatch m; // <-- need a match object

  while (regex_search(s, m, r))  // <-- use it here to get the match
  {
    int split_on = m.position(); // <-- use the match position
    int length = m.length();
    splits.push_back(s.substr(0, split_on + m.length()));
    s = s.substr(split_on + m.length());
  }

  if(!s.empty()) {
    splits.push_back(s); // and there may be one last token at the end
  }

  return splits;
}
