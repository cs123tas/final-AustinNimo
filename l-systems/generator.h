#ifndef GENERATOR_H
#define GENERATOR_H
#include <string>
#include <vector>
#include <unordered_map>
#include "lnode.h"
#include "shapes/Cylinder.h"




class Generator
{
public:
    Generator();
    int readFile(std::string);
    std::vector<std::shared_ptr<LShapeNode>> m_shapeNodes;

private:
//    int readRotationLine(std::vector<std::string> line);
    void readLine(std::string line);
//    void generateProduction(std::string predecessor);
    std::shared_ptr<LRuleLine> processRule(std::string line, LRule *lineRule);
    std::shared_ptr<LFwdRuleLine> processFwdRule(std::string line, LRule *lineRule);
    std::shared_ptr<LRotRuleLine> processRotRule(std::string line, LRule *lineRule);
    std::shared_ptr<LLeafRuleLine> processLeafRule(std::string line, LRule *lineRule);
    std::shared_ptr<LPredRuleLine> processPredRule(std::string line, LRule *lineRule);
    std::unordered_map<std::string, LRule> generateRules(std::vector<std::string> predecessors);
    std::vector<LLayer> generateLayers(std::unordered_map<std::string, LRule> &rules,
                                       glm::vec3 initAngle, glm::vec3 initLoc, glm::vec3 initSize);
    std::vector<LLayer> generateLayer(std::string rule, std::unordered_map<std::string, LRule> &rules,
                         glm::vec3 curAngle, glm::vec3 lastAngle, glm::vec3 curLocation, glm::vec3 curScale,
                         std::unordered_map<std::string, float> variables, int depth);
    std::shared_ptr<Cylinder> m_cylinder;



};

#endif // GENERATOR_H