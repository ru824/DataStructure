#include "Edge.h"

// ¹¹Ôìº¯Êı
Edge::Edge(int s, int d, int w) : src(s), dest(d), weight(w) {}

int Edge::getSrc() const { return src; }
int Edge::getDest() const { return dest; }
int Edge::getWeight() const { return weight; }
void Edge::setWeight(int w) { weight = w; }