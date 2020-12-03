#ifndef GENERATOR_H
#define GENERATOR_H
#include <string>
#include <vector>
#include <unordered_map>
#include "lnode.h"





class Generator
{
public:
    Generator();
    int readFile(std::string);

private:
    int readRotationLine(std::vector<std::string> line);
    void readLine(std::string line);
    void generateProduction(std::string predecessor);
    std::shared_ptr<LRuleLine> processRule(std::string line);
    std::shared_ptr<LFwdRuleLine> processFwdRule(std::string line);
    std::shared_ptr<LRotRuleLine> processRotRule(std::string line);
    std::shared_ptr<LLeafRuleLine> processLeafRule(std::string line);
    std::shared_ptr<LPredRuleLine> processPredRule(std::string line);
    std::unordered_map<std::string, LRule> generateRules(std::vector<std::string> predecessors);
    std::vector<LLayer> generateLayers(std::unordered_map<std::string, LRule> &rules,
                                       glm::vec3 initAngle, glm::vec3 initLoc);
    std::vector<LLayer> generateLayer(std::string rule, std::unordered_map<std::string, LRule> &rules,
                         glm::vec3 curAngle, glm::vec3 curLocation,
                         std::unordered_map<std::string, float> variables, int depth);



};

#endif // GENERATOR_H
