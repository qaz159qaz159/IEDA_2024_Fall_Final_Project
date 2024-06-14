#include "quadtree.h"

QuadTreeNode* create_quadtree_node(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    QuadTreeNode* node = (QuadTreeNode*)malloc(sizeof(QuadTreeNode));
    node->x = x;
    node->y = y;
    node->width = width;
    node->height = height;
    node->instance = NULL;
    node->nw = node->ne = node->sw = node->se = NULL;
    return node;
}

int insert_quadtree(QuadTreeNode* node, Inst* instance) {
    if (!node) return FALSE;

    if (instance->x < node->x || instance->x >= node->x + node->width ||
        instance->y < node->y || instance->y >= node->y + node->height) {
        return FALSE;
    }

    if (!node->nw && !node->instance) {
        node->instance = instance;
        return TRUE;
    }

    if (!node->nw) {
        uint32_t half_width = 0;
        uint32_t half_height = 0;
        if (node->width % 2 == 1) {
            uint32_t half_width = (node->width + 1) / 2;
            uint32_t half_height = (node->height + 1) / 2;
        } else {
            uint32_t half_width = node->width / 2;
            uint32_t half_height = node->height / 2;
        }
        node->nw = create_quadtree_node(node->x, node->y, half_width, half_height);
        node->ne = create_quadtree_node(node->x + half_width, node->y, node->width - half_width, half_height);
        node->sw = create_quadtree_node(node->x, node->y + half_height, half_width, node->height - half_height);
        node->se = create_quadtree_node(node->x + half_width, node->y + half_height, node->width - half_width, node->height - half_height);
    }

    if (insert_quadtree(node->nw, instance)) return TRUE;
    if (insert_quadtree(node->ne, instance)) return TRUE;
    if (insert_quadtree(node->sw, instance)) return TRUE;
    if (insert_quadtree(node->se, instance)) return TRUE;

    return FALSE;
}