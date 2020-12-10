#ifndef LNODE_H
#define LNODE_H
#include <vector>
#include "shapes/Shape.h"
#include <unordered_map>
#include "lib/RGBA.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "lib/CS123SceneData.h"

enum nodeType{SHAPE, OPERATION, PREDECESSOR};

// Types of rules
enum lineType{ROT, FWD, LEAF, PRED, VEC};

// Map from the string in a file to the enum
static std::unordered_map<std::string,lineType> lineMap = {
    {"rot",lineType::ROT},
    {"fwd",lineType::FWD},
    {"leaf",lineType::LEAF},
    {"vec",lineType::VEC},
    {"leaf",lineType::PRED}};

struct LRuleLine {
    lineType ruleType;
    virtual ~LRuleLine() = default;
};

struct LFwdRuleLine : public LRuleLine{
    std::string rule;
    std::string distance;
    std::string radius;
    std::string length;
};

struct LRotRuleLine : public LRuleLine{
    std::string rule;
    std::string rot[3];
};

struct LLeafRuleLine : public LRuleLine{
    std::string rule;
    std::string length;
    std::string width;
    std::string thickness;
    std::string color[4];
};

struct LPredRuleLine : public LRuleLine{
    std::string pred;
    std::string rule;
};

struct LVecRuleLine : public LRuleLine{
    std::string vector[3];
    std::string percent;
    std::string rule;
};

struct LRule {
    std::vector<std::string> variableNames;
    std::vector<float> variableValues;
    std::string operation;
    std::vector<std::shared_ptr<LRuleLine>> successorNodes;
};

struct LNode {
    nodeType type;
    virtual ~LNode(){}
};

struct LShapeNode : public LNode{
    std::shared_ptr<Shape> shape;
    glm::mat4x4 transform;
    glm::vec4 color;
    LShapeNode() {
    }
    LShapeNode(const LShapeNode &node) {
        shape = node.shape;
        transform = node.transform;
    }

    void rotate(float angle, glm::vec3 rotation) {
        transform = glm::rotate(transform, angle, rotation) * transform;
    }
    void translate(glm::vec3 translation) {
        transform = glm::translate(transform, translation);
    }
    void scale(glm::vec3 scale) {
        transform = glm::scale(transform, scale);
    }
    void matrix(glm::mat4x4 matrix) {
        transform = matrix * transform;
    }

};

struct LOperationNode : public LNode{
    std::string operation;
};

struct LPredecessorNode : public LNode{
    LRule rule;
};

struct LLayer {
    std::unordered_map<std::string, float> variables;
    glm::vec3 location;
    glm::vec3 angle;
    glm::vec3 lastAngle;
    glm::vec3 scale;
    glm::mat4x4 transform;
    std::string operation;
    std::vector<LShapeNode> shapeNodes;
    LLayer(){};
    LLayer(const LLayer &layer) {
        variables = layer.variables;
        location = layer.location;
        angle = layer.angle;
        operation = layer.operation;
        shapeNodes = layer.shapeNodes;
    }
};

struct ParseNode {
    std::string op;
    ParseNode *left;
    ParseNode *right;
    ParseNode *parentNode;
};

struct TreeDistribution {
    std::vector<std::shared_ptr<LShapeNode>> treeNodes;
    std::vector<glm::vec3> treeLocations;
    std::vector<glm::vec3> treeAngles;
};

#endif // LNODE_H


/* What is a node?
 * First there are text nodes, then actual nodes.
 * A text node can be an predecessor. In this case, it is a node that contains inside of it multiple successors.
 * A successor can itself be a predecessor although then it won't be realized until another iteration.
 * A rule is formed of a predecessor node and all its children. It is also associated with a list of variables. I'm going to say all these variables must be floats.
 * So first, let's have code that generates a list of rule nodes. In text at first.
 * The first rule node is assumed to be the axiom.
 * Every text node needs to be able to convert to an actual node
 *
 * A node is either an operation or a recursive call to a rule. At a given levels of the tree, variables have a given value.
 * Nodes in real space have a starting location and starting angle. This determines their starting orientation.
 * After this all operations take place according to their variable list.
 *
 */
