#include "generator.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "lib/SupportMethods.h"
#include "l-systems/lnode.h"
#include <memory>
#include <regex>

int GENERATION_DEPTH = 4;

Generator::Generator()
{

}

// Read in the Lsystem file
int Generator::readFile(std::string fileName)
{
    std::string file = SupportMethods::get_file_contents(fileName.data());
    std::regex predex = std::regex("(->|;)");
    std::vector<std::string> predecessors = SupportMethods::splitRegex(file, predex);

    std::unordered_map<std::string, LRule> rules = generateRules(predecessors);

    std::vector<LLayer> layers = generateLayers(rules, {0.0,0.0,0.0},{0.0,0.0,0.0});

    return 1;
}

std::vector<LLayer> Generator::generateLayers(std::unordered_map<std::string, LRule> &rules,
                                              glm::vec3 initAngle, glm::vec3 initLoc) {
    auto autoRule = rules.find("L");
    LRule initRule = autoRule->second;
    std::unordered_map<std::string, float> initVariables;

    for(int i = 0; i < (int)rules.size(); i++) {
        initVariables.insert({initRule.variableNames[i], 0.0});
    }
    std::vector<LLayer> root = generateLayer("L", rules, initAngle, initLoc, initVariables, 0);
    return root;
}

// TODO account for the various modifications to angle loc etc.
std::vector<LLayer> Generator::generateLayer(std::string rule, std::unordered_map<std::string, LRule> &rules,
                                glm::vec3 curAngle, glm::vec3 curLocation,
                                std::unordered_map<std::string, float> variables, int depth) {

    auto autoRule = rules.find(rule);
    LRule lrule = autoRule->second;
    LLayer newLayer;
    newLayer.angle = curAngle;
    newLayer.location = curLocation;
    newLayer.variables = variables;
    newLayer.operation = lrule.operation;

    std::vector<LLayer> layers;

    for(int i = 0; i < (int)lrule.successorNodes.size(); i++) {
        std::shared_ptr<LRuleLine> lruleline = lrule.successorNodes[i];
        switch(lruleline.get()->ruleType) {

            case lineType::FWD: {
                std::shared_ptr<LFwdRuleLine> line(std::dynamic_pointer_cast<LFwdRuleLine>(lruleline));
                // TODO Make a new cylinder here
                newLayer.location += newLayer.angle * line.get()->length;
                break;
            }

            case lineType::ROT: {
                std::shared_ptr<LRotRuleLine> line(std::dynamic_pointer_cast<LRotRuleLine>(lruleline));
                // TODO Clean up radians?
                newLayer.angle += line.get()->rot;
                break;
            }

            case lineType::LEAF: {
                // TODO make a new leaf here
                break;
            }
            case lineType::PRED: {
                if (depth < GENERATION_DEPTH) {
                    std::vector<LLayer> children = generateLayer("L", rules, newLayer.angle, newLayer.location, newLayer.variables, depth + 1);
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
            std::shared_ptr<LRuleLine> ruleLine = processRule(line);
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

std::shared_ptr<LRuleLine> Generator::processRule(std::string line) {
    std::size_t found = line.find_first_of("(");
    std::string ruleType = line.substr(0, found);
    std::string rest = line.substr(found);
    lineType ruleEnum;
    auto search = lineMap.find(ruleType);
    if(search == lineMap.end()) {
        ruleEnum = lineType::PRED;
    } else {
        ruleEnum = search->second;
    }
    switch(ruleEnum) {
        case lineType::FWD:
            return processFwdRule(rest);
        case lineType::ROT:
            return processRotRule(rest);
        case lineType::LEAF:
            return processLeafRule(rest);
        case lineType::PRED:
            std::shared_ptr<LPredRuleLine> returnRule = processPredRule(rest);
            returnRule.get()->pred = ruleType;
            return returnRule;
    }
}


// TODO Finish
std::shared_ptr<LFwdRuleLine> Generator::processFwdRule(std::string line) {
    std::shared_ptr<LFwdRuleLine> returnRule = std::make_shared<LFwdRuleLine>();

    returnRule.get()->ruleType = lineType::FWD;

    // Remove \r and \n from the line
    line.erase( std::remove(line.begin(), line.end(), '\r'), line.end() );
    line.erase( std::remove(line.begin(), line.end(), '\n'), line.end() );

    returnRule.get()->rule = line;

    std::regex removeParenComma("(,|\\(|\\))");
    std::vector<std::string> vals = SupportMethods::splitRegex(line, removeParenComma);

    returnRule.get()->distance = stof(vals[1]);
    returnRule.get()->radius = stof(vals[2]);
    returnRule.get()->length = stof(vals[3]);

    return returnRule;
}


// TODO Finish
std::shared_ptr<LRotRuleLine> Generator::processRotRule(std::string line) {
    std::shared_ptr<LRotRuleLine> returnRule = std::make_shared<LRotRuleLine>();
    returnRule.get()->ruleType = lineType::ROT;

    // Remove \r and \n from the line
    line.erase( std::remove(line.begin(), line.end(), '\r'), line.end() );
    line.erase( std::remove(line.begin(), line.end(), '\n'), line.end() );

    returnRule.get()->rule = line;

    std::regex removeParenComma("(,|\\(|\\))");
    std::vector<std::string> vals = SupportMethods::splitRegex(line, removeParenComma);

    returnRule.get()->rot = {stof(vals[1]), stof(vals[2]), stof(vals[3])};

    return returnRule;
}


// TODO Finish
std::shared_ptr<LLeafRuleLine> Generator::processLeafRule(std::string line) {
    std::shared_ptr<LLeafRuleLine> returnRule = std::make_shared<LLeafRuleLine>();
    returnRule.get()->ruleType = lineType::LEAF;

    // Remove \r and \n from the line
    line.erase( std::remove(line.begin(), line.end(), '\r'), line.end() );
    line.erase( std::remove(line.begin(), line.end(), '\n'), line.end() );

    returnRule.get()->rule = line;

    std::regex removeParenComma("(,|\\(|\\))");
    std::vector<std::string> vals = SupportMethods::splitRegex(line, removeParenComma);

    returnRule.get()->size = stof(vals[1]);
    returnRule.get()->color = RGBA(stof(vals[2]), stof(vals[3]), stof(vals[4]), stof(vals[5]));

    return returnRule;

}

// TODO Finish
std::shared_ptr<LPredRuleLine> Generator::processPredRule(std::string line) {
    std::shared_ptr<LPredRuleLine> returnRule = std::make_shared<LPredRuleLine>();
    returnRule.get()->ruleType = lineType::PRED;

    // Remove \r and \n from the line
    line.erase( std::remove(line.begin(), line.end(), '\r'), line.end() );
    line.erase( std::remove(line.begin(), line.end(), '\n'), line.end() );

    returnRule.get()->rule = line;
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

void Generator::generateProduction(std::string predecessor) {

}

int Generator::readRotationLine(std::vector<std::string> line) {

}

