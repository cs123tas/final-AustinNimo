#include "generator.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "lib/SupportMethods.h"
#include "l-systems/lnode.h"
#include <memory>
#include <regex>
#include <glm/gtx/rotate_vector.hpp>

int GENERATION_DEPTH = 5;

Generator::Generator()
{
    // Determine best cylinder parameters
    m_cylinder = std::make_shared<Cylinder>(50, 50);

}

// Read in the Lsystem file
int Generator::readFile(std::string fileName)
{
    std::string file = SupportMethods::get_file_contents(fileName.data());
    std::regex predex = std::regex("(->|;)");
    std::vector<std::string> predecessors = SupportMethods::splitRegex(file, predex);

    std::unordered_map<std::string, LRule> rules = generateRules(predecessors);

    std::vector<LLayer> layers = generateLayers(rules, {0.0,1.0,0.0},{0.0,0.0,0.0}, {0.1,0.1,0.1});

    return 1;
}

std::vector<LLayer> Generator::generateLayers(std::unordered_map<std::string, LRule> &rules,
                                              glm::vec3 initAngle, glm::vec3 initLoc, glm::vec3 initSize) {
    auto autoRule = rules.find("L");
    LRule initRule = autoRule->second;
    std::unordered_map<std::string, float> initVariables;

    for(int i = 0; i < (int)rules.size(); i++) {
        initVariables.insert({initRule.variableNames[i], 0.0});
    }

    std::vector<LLayer> root = generateLayer("L", rules, initAngle, initLoc, initSize, initVariables, 0);
    return root;
}

// TODO account for the various modifications to angle loc etc.
std::vector<LLayer> Generator::generateLayer(std::string rule, std::unordered_map<std::string, LRule> &rules,
                                glm::vec3 curAngle, glm::vec3 curLocation, glm::vec3 curScale,
                                std::unordered_map<std::string, float> variables, int depth) {

    auto autoRule = rules.find(rule);
    LRule lrule = autoRule->second;
    LLayer newLayer;
    newLayer.angle = curAngle;
    newLayer.location = curLocation;
    newLayer.scale = curScale;
    newLayer.variables = variables;
    newLayer.operation = lrule.operation;

    std::vector<LLayer> layers;

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
                glm::mat4x4 transform = glm::mat4x4(1);
                transform = glm::translate(transform, newLayer.location);
                transform = glm::rotate(transform, newLayer.angle.x, glm::vec3(1.0, 0.0, 0.0));
                transform = glm::rotate(transform, newLayer.angle.y, glm::vec3(0.0, 1.0, 0.0));
                transform = glm::rotate(transform, newLayer.angle.z, glm::vec3(0.0, 0.0, 1.0));
//                std::cout << transform[3].y << std::endl;
//                std::cout << newLayer.location.y<< std::endl;
                transform = glm::scale(transform, newLayer.scale);
//                std::cout << transform[3].y << std::endl;
                newNode.get()->transform = transform;
                newNode.get()->shape = m_cylinder;
                m_shapeNodes.push_back(newNode);

                // Then, move the current location in the direction of current facing by the current length
                // Modified a bit so the new branch is inside the old.
                glm::vec3 newDistance = glm::normalize(newLayer.angle) * newLayer.scale.y;
                newLayer.location += newDistance;
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
                // TODO make a new leaf here
                break;
            }
            case lineType::PRED: {
                if (depth < GENERATION_DEPTH) {
                    std::shared_ptr<LPredRuleLine> pred(std::dynamic_pointer_cast<LPredRuleLine>(lruleline));
                    // TODO Support multiple variables by splitting by commas and doing this multiple times

                    // Parse the new value for the variable used, and create a child branch
                    float newVariableValue = SupportMethods::parseIntoFloat(pred.get()->rule, variables);

                    std::unordered_map<std::string, float> newVariables = variables;
                    newVariables[pred.get()->pred] = newVariableValue;
                    std::vector<LLayer> children = generateLayer("L", rules, newLayer.angle, newLayer.location, newLayer.scale, newVariables, depth + 1);
                    layers.insert( layers.end(), children.begin(), children.end() );
                }
                break;
            }
        }
    }
    layers.push_back(newLayer);
    return layers;
}

std::unordered_map<std::string, LRule> Generator::generateRules(std::vector<std::string> predecessors) {
    std::unordered_map<std::string, LRule> rules;

    for(int i = 0; i < (int)predecessors.size(); i++) {

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
            if (line == "\r" || line == "\n") {
                continue;
            }
            std::shared_ptr<LRuleLine> ruleLine = processRule(line, &newRule);
            // TODO REMOVE TEST CODE
            if(ruleLine.get()->ruleType == lineType::ROT) {
                std::shared_ptr<LRotRuleLine> test(std::dynamic_pointer_cast<LRotRuleLine>(ruleLine));
                if (test) {
                    std::cout << test.get()->rule << std::endl;
                }
            }
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
        case lineType::PRED:
            std::shared_ptr<LPredRuleLine> returnRule = processPredRule(rest, lineRule);
            // TODO support more than one variable
            returnRule.get()->pred = lineRule->variableNames[0];
            return returnRule;
    }
    return nullptr;
}


// TODO Finish
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


// TODO Finish
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


// TODO Finish
std::shared_ptr<LLeafRuleLine> Generator::processLeafRule(std::string line, LRule *lineRule) {
    std::shared_ptr<LLeafRuleLine> returnRule = std::make_shared<LLeafRuleLine>();
    returnRule.get()->ruleType = lineType::LEAF;

    // Remove \r and \n from the line
    line.erase( std::remove(line.begin(), line.end(), '\r'), line.end() );
    line.erase( std::remove(line.begin(), line.end(), '\n'), line.end() );

    returnRule.get()->rule = line;

    std::regex removeParenComma(",");
    std::vector<std::string> vals = SupportMethods::splitRegex(line, removeParenComma);

    returnRule.get()->size = vals[0];

    returnRule.get()->color[0] = vals[1];
    returnRule.get()->color[1] = vals[2];
    returnRule.get()->color[2] = vals[3];
    returnRule.get()->color[3] = vals[4].substr(0, vals[4].size() - 1);

    return returnRule;

}

// TODO Finish
std::shared_ptr<LPredRuleLine> Generator::processPredRule(std::string line, LRule *lineRule) {
    std::shared_ptr<LPredRuleLine> returnRule = std::make_shared<LPredRuleLine>();
    returnRule.get()->ruleType = lineType::PRED;

    // Remove \r and \n from the line
    line.erase( std::remove(line.begin(), line.end(), '\r'), line.end() );
    line.erase( std::remove(line.begin(), line.end(), '\n'), line.end() );

    returnRule.get()->rule = line.substr(0, line.size() - 1);
    return returnRule;

}

// Read each line in and forward it to the appropriate method
void Generator::readLine(std::string line) {
    std::vector<std::string> pieces = SupportMethods::split(line, ":");
    lineType lineRule = lineMap[pieces[0]];
    switch(lineRule) {
        case lineType::FWD:
            break;

        case lineType::ROT:
            break;

        case lineType::LEAF:
            break;

        case lineType::PRED:
            break;
    }

    std::cout << pieces[0] << std::endl;
}

//void Generator::generateProduction(std::string predecessor) {

//}

////int Generator::readRotationLine(std::vector<std::string> line) {

////}

