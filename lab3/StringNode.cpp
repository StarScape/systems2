#include "StringNode.h"

StringNode::StringNode() {
  key = "";
  val = -1;
  next = NULL;
}

StringNode::StringNode(std::string s, int v, StringNode *n) {
  key = s;
  val = v;
  next = n;
}

StringNode::~StringNode() {}
