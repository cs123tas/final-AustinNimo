#ifndef LNODE_H
#define LNODE_H
#include <vector>
#include "shapes/Shape.h"
#include <unordered_map>
#include "lib/RGBA.h"

enum nodeType{SHAPE, OPERATION, PREDECESSOR};

// Types of rules
enum lineType{ROT, FWD, LEAF, PRED};

// Map from the string in a file to the enum
static std::unordered_map<std::string,lineType> lineMap = {
    {"rot",lineType::ROT},
    {"fwd",lineType::FWD},
    {"leaf",lineType::LEAF},
    {"leaf",lineType::PRED}};

struct LRuleLine {
    lineType ruleType;
    virtual ~LRuleLine() = default;
};

struct LFwdRuleLine : public LRuleLine{
    std::string rule;
    float distance;
    float radius;
    float length;
};

struct LRotRuleLine : public LRuleLine{
    std::string rule;
    glm::vec3 rot;
};

struct LLeafRuleLine : public LRuleLine{
    std::string rule;
    float size;
    RGBA color;
};

struct LPredRuleLine : public LRuleLine{
    std::string pred;
    std::string rule;
};

struct LRule {
    std::vector<std::string> variableNames;
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
    LShapeNode(const LShapeNode &node) {
        shape = node.shape;
        transform = node.transform;
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
