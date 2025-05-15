//
// Created by debian on 5/13/25.
//

#ifndef AVLTREE_H
#define AVLTREE_H
#include <stdlib.h>

typedef struct {
    char userName[20];
    size_t value;
}node;

typedef struct {
    node data;
    size_t key;
}node2;

typedef struct avlNode{
    node2 data;
    struct avlNode *left;
    struct avlNode *right;
    ssize_t height;
}avlNode_T;

typedef avlNode_T *avlTree;

void printNode(node node);

void printNode2(node2 node);

avlTree createTree(void);

void preorderView(avlTree tree);

avlTree addNode(avlTree tree, node2 data);

void printTree(avlTree tree);

avlTree destructTree(avlTree tree);

int existsNode(avlTree tree, size_t data);

node *getNode(avlTree tree, size_t key);

void preorder(avlTree tree, void (*callback)(avlTree tree));
#endif //AVLTREE_H
