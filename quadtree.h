#ifndef QUADTREE_H
#define QUADTREE_H

#include "place.h"

// typedef struct QuadTreeNode;

typedef struct QuadTreeNode{
    uint32_t x, y, width, height;
    Inst* instance;
    struct QuadTreeNode* nw; // Northwest child
    struct QuadTreeNode* ne; // Northeast child
    struct QuadTreeNode* sw; // Southwest child
    struct QuadTreeNode* se; // Southeast child
} QuadTreeNode;

QuadTreeNode* create_quadtree_node(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
int insert_quadtree(QuadTreeNode* node, Inst* instance);

#endif // QUADTREE_H