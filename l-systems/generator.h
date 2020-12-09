#ifndef GENERATOR_H
#define GENERATOR_H
#include <string>
#include <vector>
#include <unordered_map>
#include "lnode.h"
#include "shapes/Cylinder.h"
#include "shapes/MeshLoader.h"
#include "shapes/LoadedMesh.h"



class Generator
{
public:
    Generator();
    std::vector<std::shared_ptr<LShapeNode>> readFile(std::string fileName, glm::vec3 initAngle, glm::vec3 initLoc, glm::vec3 initSize);
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
    std::shared_ptr<LVecRuleLine> processVecRule(std::string line, LRule *lineRule);
    std::unordered_map<std::string, LRule> generateRules(std::vector<std::string> &predecessors);
    void generateLayers(std::unordered_map<std::string, LRule> &rules,
                                       glm::vec3 initAngle, glm::vec3 initLoc, glm::vec3 initSize);
    void generateLayer(std::string rule, std::unordered_map<std::string, LRule> &rules,
                         glm::vec3 curAngle, glm::vec3 lastAngle, glm::vec3 curLocation, glm::vec3 curScale,
                         std::unordered_map<std::string, float> &variables, int depth);
    std::shared_ptr<Cylinder> m_cylinder;
    std::shared_ptr<LoadedMesh> m_leaf;



};

#endif // GENERATOR_H
