#include "generator.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "lib/SupportMethods.h"
#include "l-systems/lnode.h"
#include <memory>
#include <regex>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/quaternion.hpp>
#include <QFile>
#include <QFileInfo>
#include <QCoreApplication>
#include <QRunnable>
#include <QThreadPool>
#include <QMutex>

int GENERATION_DEPTH = 3;
//TODO Allow editing the trunk color
glm::vec4 TREE_TRUNK_BROWN(0.324,0.207,0.039,1.0);
glm::vec4 LEAF_GREEN(0.227,0.372,0.043,1.0);

Generator::Generator()
{
    // Determine best cylinder parameters
    m_cylinder = std::make_shared<Cylinder>(20, 20);

    // Load in leaf
    QString leafFile(":/obj/l-systems/leaf.obj");
    MeshLoader leafCreator = MeshLoader();
    leafCreator.loadMesh(leafFile);

    std::vector<Vertex> vertices = leafCreator.getVertices();

    std::vector<int> indices = leafCreator.getIndices();

    m_leaf = std::make_shared<LoadedMesh>(vertices, indices);


}

// Read in the Lsystem file
std::vector<std::shared_ptr<LShapeNode>> Generator::readFile(std::string fileName, glm::vec3 initAngle, glm::vec3 initLoc, glm::vec3 initSize)
{
    try {
        std::string file = SupportMethods::get_file_contents(fileName.data());
        std::regex predex = std::regex("(->|;)");
        std::vector<std::string> predecessors = SupportMethods::splitRegex(file, predex);

        std::unordered_map<std::string, LRule> rules = generateRules(predecessors);

        generateLayers(rules, initAngle,initLoc, initSize);
    }
    catch(...) {

    }
    return m_shapeNodes;
}


struct Layer : public QRunnable
{
    std::string rule;
    std::unordered_map<std::string, LRule> &rules;
    glm::vec3 curAngle;
    glm::vec3 lastAngle;
    glm::vec3 curLocation;
    glm::vec3 curScale;
    std::unordered_map<std::string, float> variables;
    int depth;
    std::vector<std::shared_ptr<LShapeNode>> &nodes;
    std::shared_ptr<Cylinder> m_cylinder;
    std::shared_ptr<LoadedMesh> m_leaf;
    QMutex *mutex;

    Layer(std::string rule, std::unordered_map<std::string, LRule> &rules,
          glm::vec3 curAngle, glm::vec3 lastAngle, glm::vec3 curLocation, glm::vec3 curScale,
          std::unordered_map<std::string, float> &variables, int depth, std::vector<std::shared_ptr<LShapeNode>> &nodes, std::shared_ptr<Cylinder> m_cylinder,
          std::shared_ptr<LoadedMesh> m_leaf, QMutex *mutex) : rule(rule), rules(rules), curAngle(curAngle),
            lastAngle(lastAngle), curLocation(curLocation), curScale(curScale), variables(variables), depth(depth),
              nodes(nodes), m_cylinder(m_cylinder), m_leaf(m_leaf), mutex(mutex){}
    std::vector<LLayer> finalLayer;
    void run() override final
    {
        auto autoRule = rules.find(rule);
        LRule lrule = autoRule->second;
        LLayer newLayer;
        newLayer.angle = curAngle;
        newLayer.lastAngle = lastAngle;
        newLayer.location = curLocation;
        newLayer.scale = curScale;
        newLayer.variables = variables;
        newLayer.operation = lrule.operation;

        for(int i = 0; i < (int)lrule.successorNodes.size(); i++) {
            std::shared_ptr<LRuleLine> lruleline = lrule.successorNodes[i];
            switch(lruleline.get()->ruleType) {

                case lineType::FWD: {

                    // Adjust the current radius and length by the provided values
                    std::shared_ptr<LFwdRuleLine> line(std::dynamic_pointer_cast<LFwdRuleLine>(lruleline));
                    float length = SupportMethods::parseIntoFloat(line.get()->length, variables);
                    float radius = SupportMethods::parseIntoFloat(line.get()->radius, variables);
                    glm::vec3 scale = {radius, length, radius};
                    newLayer.scale *= scale;

                    // Paint the shape by using the current scale, applying the current rotation, and then translating
                    std::shared_ptr<LShapeNode> newNode = std::make_shared<LShapeNode>();
                    glm::mat4x4 transform = glm::mat4x4(1.0);
                    transform = glm::translate(transform, newLayer.location);
    //                std::cout << transform[3].y << std::endl;
    //                std::cout << newLayer.location.y<< std::endl;
                    glm::vec3 targetVec = newLayer.angle;
                    glm::quat rotQuat = glm::rotation({0.0,1.0,0.0}, targetVec);
                    glm::mat4 rotationMatrix = glm::toMat4(rotQuat);
                    transform = transform * rotationMatrix;
                    newLayer.lastAngle = newLayer.angle;
    //                transform = glm::rotate(transform, -newLayer.angle.x, glm::vec3(1.0, 0.0, 0.0));
    //                transform = glm::rotate(transform, newLayer.angle.y, glm::vec3(0.0, 1.0, 0.0));
    //                transform = glm::rotate(transform, newLayer.angle.z, glm::vec3(0.0, 0.0, 1.0));
                    transform = glm::scale(transform, newLayer.scale);
                    // Shift the cylinder up so the base starts at y=0;
                    transform = glm::translate(transform, {0.0,.5,0.0});
    //                std::cout << transform[3].y << std::endl;
                    newNode.get()->transform = transform;
                    newNode.get()->shape = m_cylinder;
                    newNode.get()->color = TREE_TRUNK_BROWN;
                    mutex->lock();
                    nodes.push_back(newNode);
                    mutex->unlock();
                    // Then, move the current location in the direction of current facing by the current length
                    // Modified a bit so the new branch is inside the old.
                    glm::vec3 newDistance = newLayer.angle * newLayer.scale.y * .9f;
                    newLayer.location += newDistance;
                    break;
                }

                case lineType::VEC: {
                    std::shared_ptr<LVecRuleLine> line(std::dynamic_pointer_cast<LVecRuleLine>(lruleline));
                    float vecX = glm::radians(SupportMethods::parseIntoFloat(line.get()->vector[0], variables));
                    float vecY = glm::radians(SupportMethods::parseIntoFloat(line.get()->vector[1], variables));
                    float vecZ = glm::radians(SupportMethods::parseIntoFloat(line.get()->vector[2], variables));
                    float percentage = SupportMethods::parseIntoFloat(line.get()->percent, variables);

                    // Obtain a vector in between the current and the destination vector, according to the percentage value
                    glm::vec3 destVec = glm::normalize(glm::vec3(vecX, vecY, vecZ));
                    glm::vec3 mergedVec = percentage * destVec + (newLayer.angle * (1.0f - percentage));

                    newLayer.angle = glm::normalize(mergedVec);
                    break;


                }

                case lineType::ROT: {

                    // Rotate the angle facing by the angle provided
                    std::shared_ptr<LRotRuleLine> line(std::dynamic_pointer_cast<LRotRuleLine>(lruleline));
                    float angleX = glm::radians(SupportMethods::parseIntoFloat(line.get()->rot[0], variables));
                    float angleY = glm::radians(SupportMethods::parseIntoFloat(line.get()->rot[1], variables));
                    float angleZ = glm::radians(SupportMethods::parseIntoFloat(line.get()->rot[2], variables));
                    newLayer.angle = glm::rotateX(newLayer.angle, angleX);
                    newLayer.angle = glm::rotateY(newLayer.angle, angleY);
                    newLayer.angle = glm::rotateZ(newLayer.angle, angleZ);
                    newLayer.angle = glm::normalize(newLayer.angle);
                    break;
                }

                case lineType::LEAF: {


                    // Adjust the current radius and length by the provided values
                    std::shared_ptr<LLeafRuleLine> line(std::dynamic_pointer_cast<LLeafRuleLine>(lruleline));
                    float r = SupportMethods::parseIntoFloat(line.get()->color[0], variables);
                    float g = SupportMethods::parseIntoFloat(line.get()->color[1], variables);
                    float b = SupportMethods::parseIntoFloat(line.get()->color[2], variables);
                    float a = SupportMethods::parseIntoFloat(line.get()->color[3], variables);
                    float length = SupportMethods::parseIntoFloat(line.get()->length, variables);
                    float thickness = SupportMethods::parseIntoFloat(line.get()->thickness, variables);
                    float width = SupportMethods::parseIntoFloat(line.get()->width, variables);
                    glm::vec3 scale = {length, thickness, width};

                    // Paint the shape by using the current scale, applying the current rotation, and then translating
                    std::shared_ptr<LShapeNode> newNode = std::make_shared<LShapeNode>();
                    glm::mat4x4 transform = glm::mat4x4(1.0);
                    transform = glm::translate(transform, newLayer.location);
                    glm::vec3 targetVec = newLayer.angle;
                    glm::quat rotQuat = glm::rotation({0.0,1.0,0.0}, targetVec);
                    glm::mat4 rotationMatrix = glm::toMat4(rotQuat);
                    transform = transform * rotationMatrix;
                    newLayer.lastAngle = newLayer.angle;
                    transform = glm::scale(transform, scale);

                    newNode.get()->transform = transform;
                    newNode.get()->shape = m_leaf;
                    newNode.get()->color = {r, g, b, a};
                    mutex->lock();
                    nodes.push_back(newNode);
                    mutex->unlock();
                    break;
                }
                case lineType::PRED: {
                std::shared_ptr<LPredRuleLine> pred(std::dynamic_pointer_cast<LPredRuleLine>(lruleline));
                float newVariableValue = SupportMethods::parseIntoFloat(pred.get()->rule, variables);
                    if (newVariableValue < GENERATION_DEPTH) {
                        // TODO Support multiple variables by splitting by commas and doing this multiple times

                        // Parse the new value for the variable used, and create a child branch
                        float newVariableValue = SupportMethods::parseIntoFloat(pred.get()->rule, variables);

                        std::unordered_map<std::string, float> newVariables = variables;
                        // TODO Support multiple variables by saving a list of variables used and updating these
                        newVariables[std::string(1,pred.get()->rule[0])] = newVariableValue;

                        Layer *root = new Layer(pred.get()->pred, rules, newLayer.angle, newLayer.lastAngle, newLayer.location,
                                                newLayer.scale, newVariables, newVariableValue, nodes, m_cylinder, m_leaf, mutex);
                        QThreadPool::globalInstance()->start(root);
                        //generateLayer(pred.get()->pred, rules, newLayer.angle, newLayer.lastAngle, newLayer.location, newLayer.scale, newVariables, newVariableValue);
                    }
                    break;
                }
            }
        }
    }
};

void Generator::generateLayers(std::unordered_map<std::string, LRule> &rules,
                                              glm::vec3 initAngle, glm::vec3 initLoc, glm::vec3 initSize) {
    auto autoRule = rules.find("L");
    LRule initRule = autoRule->second;
    std::unordered_map<std::string, float> initVariables;

    for (auto const& element : rules)
    {
        for(int i = 0; i < (int)element.second.variableNames.size(); i++) {
            initVariables.insert({element.second.variableNames[i], 0.0});
        }
    }

    //generateLayer("L", rules, initAngle, initAngle, initLoc, initSize, initVariables, 0);
    QMutex mutex;
    Layer *root = new Layer("L", rules, initAngle, initAngle, initLoc, initSize, initVariables, 0, m_shapeNodes, m_cylinder, m_leaf, &mutex);
    QThreadPool::globalInstance()->start(root);
    QThreadPool::globalInstance()->waitForDone();
}



// TODO account for the various modifications to angle loc etc.
void Generator::generateLayer(std::string rule, std::unordered_map<std::string, LRule> &rules,
                                glm::vec3 curAngle, glm::vec3 lastAngle, glm::vec3 curLocation, glm::vec3 curScale,
                                std::unordered_map<std::string, float> &variables, int depth) {

    auto autoRule = rules.find(rule);
    LRule lrule = autoRule->second;
    LLayer newLayer;
    newLayer.angle = curAngle;
    newLayer.lastAngle = lastAngle;
    newLayer.location = curLocation;
    newLayer.scale = curScale;
    newLayer.variables = variables;
    newLayer.operation = lrule.operation;

    for(int i = 0; i < (int)lrule.successorNodes.size(); i++) {
        std::shared_ptr<LRuleLine> lruleline = lrule.successorNodes[i];
        switch(lruleline.get()->ruleType) {

            case lineType::FWD: {

                // Adjust the current radius and length by the provided values
                std::shared_ptr<LFwdRuleLine> line(std::dynamic_pointer_cast<LFwdRuleLine>(lruleline));
                float length = SupportMethods::parseIntoFloat(line.get()->length, variables);
                float radius = SupportMethods::parseIntoFloat(line.get()->radius, variables);
                glm::vec3 scale = {radius, length, radius};
                newLayer.scale *= scale;

                // Paint the shape by using the current scale, applying the current rotation, and then translating
                std::shared_ptr<LShapeNode> newNode = std::make_shared<LShapeNode>();
                glm::mat4x4 transform = glm::mat4x4(1.0);
                transform = glm::translate(transform, newLayer.location);
//                std::cout << transform[3].y << std::endl;
//                std::cout << newLayer.location.y<< std::endl;
                glm::vec3 targetVec = newLayer.angle;
                glm::quat rotQuat = glm::rotation({0.0,1.0,0.0}, targetVec);
                glm::mat4 rotationMatrix = glm::toMat4(rotQuat);
                transform = transform * rotationMatrix;
                newLayer.lastAngle = newLayer.angle;
//                transform = glm::rotate(transform, -newLayer.angle.x, glm::vec3(1.0, 0.0, 0.0));
//                transform = glm::rotate(transform, newLayer.angle.y, glm::vec3(0.0, 1.0, 0.0));
//                transform = glm::rotate(transform, newLayer.angle.z, glm::vec3(0.0, 0.0, 1.0));
                transform = glm::scale(transform, newLayer.scale);
                // Shift the cylinder up so the base starts at y=0;
                transform = glm::translate(transform, {0.0,.5,0.0});
//                std::cout << transform[3].y << std::endl;
                newNode.get()->transform = transform;
                newNode.get()->shape = m_cylinder;
                newNode.get()->color = TREE_TRUNK_BROWN;
                m_shapeNodes.push_back(newNode);

                // Then, move the current location in the direction of current facing by the current length
                // Modified a bit so the new branch is inside the old.
                glm::vec3 newDistance = newLayer.angle * newLayer.scale.y * .9f;
                newLayer.location += newDistance;
                break;
            }

            case lineType::VEC: {
                std::shared_ptr<LVecRuleLine> line(std::dynamic_pointer_cast<LVecRuleLine>(lruleline));
                float vecX = glm::radians(SupportMethods::parseIntoFloat(line.get()->vector[0], variables));
                float vecY = glm::radians(SupportMethods::parseIntoFloat(line.get()->vector[1], variables));
                float vecZ = glm::radians(SupportMethods::parseIntoFloat(line.get()->vector[2], variables));
                float percentage = SupportMethods::parseIntoFloat(line.get()->percent, variables);

                // Obtain a vector in between the current and the destination vector, according to the percentage value
                glm::vec3 destVec = glm::normalize(glm::vec3(vecX, vecY, vecZ));
                glm::vec3 mergedVec = percentage * destVec + (newLayer.angle * (1.0f - percentage));

                newLayer.angle = glm::normalize(mergedVec);
                break;


            }

            case lineType::ROT: {

                // Rotate the angle facing by the angle provided
                std::shared_ptr<LRotRuleLine> line(std::dynamic_pointer_cast<LRotRuleLine>(lruleline));
                float angleX = glm::radians(SupportMethods::parseIntoFloat(line.get()->rot[0], variables));
                float angleY = glm::radians(SupportMethods::parseIntoFloat(line.get()->rot[1], variables));
                float angleZ = glm::radians(SupportMethods::parseIntoFloat(line.get()->rot[2], variables));
                newLayer.angle = glm::rotateX(newLayer.angle, angleX);
                newLayer.angle = glm::rotateY(newLayer.angle, angleY);
                newLayer.angle = glm::rotateZ(newLayer.angle, angleZ);
                newLayer.angle = glm::normalize(newLayer.angle);
                break;
            }

            case lineType::LEAF: {


                // Adjust the current radius and length by the provided values
                std::shared_ptr<LLeafRuleLine> line(std::dynamic_pointer_cast<LLeafRuleLine>(lruleline));
                float r = SupportMethods::parseIntoFloat(line.get()->color[0], variables);
                float g = SupportMethods::parseIntoFloat(line.get()->color[1], variables);
                float b = SupportMethods::parseIntoFloat(line.get()->color[2], variables);
                float a = SupportMethods::parseIntoFloat(line.get()->color[3], variables);
                float length = SupportMethods::parseIntoFloat(line.get()->length, variables);
                float thickness = SupportMethods::parseIntoFloat(line.get()->thickness, variables);
                float width = SupportMethods::parseIntoFloat(line.get()->width, variables);
                glm::vec3 scale = {length, thickness, width};

                // Paint the shape by using the current scale, applying the current rotation, and then translating
                std::shared_ptr<LShapeNode> newNode = std::make_shared<LShapeNode>();
                glm::mat4x4 transform = glm::mat4x4(1.0);
                transform = glm::translate(transform, newLayer.location);
                glm::vec3 targetVec = newLayer.angle;
                glm::quat rotQuat = glm::rotation({0.0,1.0,0.0}, targetVec);
                glm::mat4 rotationMatrix = glm::toMat4(rotQuat);
                transform = transform * rotationMatrix;
                newLayer.lastAngle = newLayer.angle;
                transform = glm::scale(transform, scale);

                newNode.get()->transform = transform;
                newNode.get()->shape = m_leaf;
                newNode.get()->color = {r, g, b, a};
                m_shapeNodes.push_back(newNode);
                break;
            }
            case lineType::PRED: {
            std::shared_ptr<LPredRuleLine> pred(std::dynamic_pointer_cast<LPredRuleLine>(lruleline));
            float newVariableValue = SupportMethods::parseIntoFloat(pred.get()->rule, variables);
                if (newVariableValue < GENERATION_DEPTH) {
                    // TODO Support multiple variables by splitting by commas and doing this multiple times

                    // Parse the new value for the variable used, and create a child branch
                    float newVariableValue = SupportMethods::parseIntoFloat(pred.get()->rule, variables);

                    std::unordered_map<std::string, float> newVariables = variables;
                    // TODO Support multiple variables by saving a list of variables used and updating these
                    newVariables[std::string(1,pred.get()->rule[0])] = newVariableValue;
                    generateLayer(pred.get()->pred, rules, newLayer.angle, newLayer.lastAngle, newLayer.location, newLayer.scale, newVariables, newVariableValue);
                }
                break;
            }
        }
    }
}

std::unordered_map<std::string, LRule> Generator::generateRules(std::vector<std::string> &predecessors) {
    std::unordered_map<std::string, LRule> rules;

    for(int i = 0; i < (int)predecessors.size(); i++) {
        if (predecessors[i] == "\r" || predecessors[i] == "\n" || predecessors[i] =="\r\n") {
            continue;
        }
        LRule newRule;

        // Remove \r and \n from the predecessor
        predecessors[i].erase( std::remove(predecessors[i].begin(), predecessors[i].end(), '\r'), predecessors[i].end() );
        predecessors[i].erase( std::remove(predecessors[i].begin(), predecessors[i].end(), '\n'), predecessors[i].end() );

        std::vector<std::string> vars = SupportMethods::split(predecessors[i], "(");
        std::string ruleName = vars[0];

        vars = SupportMethods::split(vars[1], ")");

        //  Now vars contains a list of variable names split by commas
        if(vars.size() > 0) {
            vars = SupportMethods::split(vars[0], ",");
            // Now it's just a list of variable names, so add it to the new rule
            for(int j = 0; j < (int)vars.size(); j++) {
                newRule.variableNames.push_back(vars[j]);
                newRule.variableValues.push_back(0.0);
            }
        }
        i = i + 1;

        std::string rule = predecessors[i];

        std::stringstream ruleStream(rule);
        std::string line;
        while (std::getline(ruleStream, line)) {
            // ignore newline only lines
            if (line == "\r" || line == "\n" || line =="\r\n") {
                continue;
            }
            std::shared_ptr<LRuleLine> ruleLine = processRule(line, &newRule);
            newRule.successorNodes.push_back(ruleLine);
        }


        newRule.operation = rule;
        rules.insert({ruleName, newRule});
    }
    return rules;
}

std::shared_ptr<LRuleLine> Generator::processRule(std::string line, LRule *lineRule) {
    std::size_t found = line.find_first_of("(");
    std::string ruleType = line.substr(0, found);
    std::string rest = line.substr(found + 1);
    lineType ruleEnum;
    auto search = lineMap.find(ruleType);
    if(search == lineMap.end()) {
        ruleEnum = lineType::PRED;
    } else {
        ruleEnum = search->second;
    }
    switch(ruleEnum) {
        case lineType::FWD:
            return processFwdRule(rest, lineRule);
        case lineType::ROT:
            return processRotRule(rest, lineRule);
        case lineType::LEAF:
            return processLeafRule(rest, lineRule);
        case lineType::VEC:
            return processVecRule(rest, lineRule);
        case lineType::PRED:
            std::shared_ptr<LPredRuleLine> returnRule = processPredRule(rest, lineRule);
            // TODO support more than one variable
            returnRule.get()->pred = ruleType;
            return returnRule;
    }
    return nullptr;
}


std::shared_ptr<LFwdRuleLine> Generator::processFwdRule(std::string line, LRule *lineRule) {
    std::shared_ptr<LFwdRuleLine> returnRule = std::make_shared<LFwdRuleLine>();

    returnRule.get()->ruleType = lineType::FWD;

    // Remove \r and \n from the line
    line.erase( std::remove(line.begin(), line.end(), '\r'), line.end() );
    line.erase( std::remove(line.begin(), line.end(), '\n'), line.end() );

    returnRule.get()->rule = line;

    std::regex removeParenComma(",");
    std::vector<std::string> vals = SupportMethods::splitRegex(line, removeParenComma);
    //std::string value = SupportMethods::parseParen(vals[0], lineRule);

    returnRule.get()->distance = vals[0];
    returnRule.get()->radius = vals[1];
    returnRule.get()->length = vals[2].substr(0, vals[2].size() - 1);

    return returnRule;
}


std::shared_ptr<LRotRuleLine> Generator::processRotRule(std::string line, LRule *lineRule) {
    std::shared_ptr<LRotRuleLine> returnRule = std::make_shared<LRotRuleLine>();
    returnRule.get()->ruleType = lineType::ROT;

    // Remove \r and \n from the line
    line.erase( std::remove(line.begin(), line.end(), '\r'), line.end() );
    line.erase( std::remove(line.begin(), line.end(), '\n'), line.end() );

    returnRule.get()->rule = line;

//    std::regex removeParenComma("(,|\\(|\\))");
    std::regex removeParenComma(",");
    std::vector<std::string> vals = SupportMethods::splitRegex(line, removeParenComma);

    returnRule.get()->rot[0] = vals[0];
    returnRule.get()->rot[1] = vals[1];
    returnRule.get()->rot[2] = vals[2].substr(0, vals[2].size() - 1);

    return returnRule;
}


std::shared_ptr<LLeafRuleLine> Generator::processLeafRule(std::string line, LRule *lineRule) {
    std::shared_ptr<LLeafRuleLine> returnRule = std::make_shared<LLeafRuleLine>();
    returnRule.get()->ruleType = lineType::LEAF;

    // Remove \r and \n from the line
    line.erase( std::remove(line.begin(), line.end(), '\r'), line.end() );
    line.erase( std::remove(line.begin(), line.end(), '\n'), line.end() );

    returnRule.get()->rule = line;

    std::regex removeParenComma(",");
    std::vector<std::string> vals = SupportMethods::splitRegex(line, removeParenComma);

    returnRule.get()->length = vals[0];
    returnRule.get()->width = vals[1];
    returnRule.get()->thickness = vals[2];

    returnRule.get()->color[0] = vals[3];
    returnRule.get()->color[1] = vals[4];
    returnRule.get()->color[2] = vals[5];
    returnRule.get()->color[3] = vals[6].substr(0, vals[6].size() - 1);

    return returnRule;

}

std::shared_ptr<LVecRuleLine> Generator::processVecRule(std::string line, LRule *lineRule) {
    std::shared_ptr<LVecRuleLine> returnRule = std::make_shared<LVecRuleLine>();
    returnRule.get()->ruleType = lineType::VEC;

    // Remove \r and \n from the line
    line.erase( std::remove(line.begin(), line.end(), '\r'), line.end() );
    line.erase( std::remove(line.begin(), line.end(), '\n'), line.end() );

    returnRule.get()->rule = line;

    std::regex removeParenComma(",");
    std::vector<std::string> vals = SupportMethods::splitRegex(line, removeParenComma);

    returnRule.get()->vector[0] = vals[0];
    returnRule.get()->vector[1] = vals[1];
    returnRule.get()->vector[2] = vals[2];

    returnRule.get()->percent = vals[3].substr(0, vals[3].size() - 1);

    return returnRule;
}

std::shared_ptr<LPredRuleLine> Generator::processPredRule(std::string line, LRule *lineRule) {
    std::shared_ptr<LPredRuleLine> returnRule = std::make_shared<LPredRuleLine>();
    returnRule.get()->ruleType = lineType::PRED;

    // Remove \r and \n from the line
    line.erase( std::remove(line.begin(), line.end(), '\r'), line.end() );
    line.erase( std::remove(line.begin(), line.end(), '\n'), line.end() );

    returnRule.get()->rule = line.substr(0, line.size() - 1);
    return returnRule;

}

//void Generator::generateProduction(std::string predecessor) {

//}

////int Generator::readRotationLine(std::vector<std::string> line) {

////}

