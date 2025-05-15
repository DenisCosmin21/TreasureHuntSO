//
// Created by debian on 5/13/25.
//

#include "AvlTree.h"
#include <stdio.h>

avlTree createTree() {
    return NULL;
}

static avlTree initNode(node2 data) {
    avlTree newNode = malloc(sizeof(struct avlNode));
    newNode->data = data;
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->height = 1;
    return newNode;
}

static ssize_t max(ssize_t a, ssize_t b) {
    return a > b ? a : b;
}

static ssize_t getNodeHeight(struct avlNode *node) {
    if (node != NULL) {
        return node->height;
    }
    return 0;
}

static ssize_t getBalanceFactor(avlTree tree) {
    if (tree == NULL) {
        return 0;
    }

    return getNodeHeight(tree->left) - getNodeHeight(tree->right);
}

static struct avlNode *rotateLeft(struct avlNode *node) {
    struct avlNode *y = node->right;
    struct avlNode *subTreeY = y->left;
    y->left = node;
    node->right = subTreeY;

    y->height = 1 + max(getNodeHeight(y->left), getNodeHeight(y->right));
    node->height = 1 + max(getNodeHeight(node->left), getNodeHeight(node->right));
    return y;
}

static struct avlNode *rotateRight(struct avlNode *node) {
    struct avlNode *y = node->left;
    struct avlNode *subTreeY = y->right;
    y->right = node;
    node->left = subTreeY;

    y->height = 1 + max(getNodeHeight(y->left), getNodeHeight(y->right));
    node->height = 1 + max(getNodeHeight(node->left), getNodeHeight(node->right));
    return y;
}

avlTree addNode(avlTree tree, node2 data) {
    if (tree == NULL) {
        return initNode(data);
    }

    if (data.key < tree->data.key) {
        tree->left = addNode(tree->left, data);
    }
    else if (data.key >  tree->data.key) {
        tree->right = addNode(tree->right, data);
    }
    else
        return tree;

    tree->height = max(getNodeHeight(tree->left) , getNodeHeight(tree->right)) + 1;

    ssize_t balanceFactor = getBalanceFactor(tree);

    if (balanceFactor > 1) { //left left or left right rotation needed
        if (data.key < tree->left->data.key) {  //Left left scenario, so right rotate
            return rotateRight(tree);
        }
        else if (data.key > tree->left->data.key) { //left right rotation
            tree->left = rotateLeft(tree);
            return rotateRight(tree);
        }
    }
    else if (balanceFactor < -1) { //right right or right left rotation needed
        if (data.key > tree->right->data.key) {  //Right right scenario, so left rotate
            return rotateLeft(tree);
        }
        else if (data.key > tree->right->data.key) { //right left scenario
            tree->right = rotateRight(tree);
            return rotateLeft(tree);
        }
    }

    return tree;
}

int existsNode(avlTree tree, size_t key) {
    if (getNode(tree, key) != NULL) {
        return 0;
    }

    return 1;
}

node *getNode(avlTree tree, size_t key) {
    if (tree == NULL) {
        return NULL;
    }
    if (key < tree->data.key) {
        return getNode(tree->left, key);
    }
    else if (key > tree->data.key) {
        return getNode(tree->right, key);
    }
    else {
        if (key == tree->data.key) {
            return &tree->data.data;
        }
    }
}

void printNode(node node) {
    printf("{value : %lld, name : %s}", node.value, node.userName);
}

void printNode2(node2 node) {
    printf("{key : %lld => ", node.key);
    printNode(node.data);
    printf("}");
}

void preorder(avlTree tree, void (*callback)(avlTree tree)) {
    if (tree == NULL)
        return;

    if (tree->left != NULL)
        preorder(tree->left, callback);
    callback(tree);
    if (tree->right != NULL)
        preorder(tree->right, callback);
}

void postorder(avlTree tree, void (*callback)(avlTree tree)) {
    if (tree == NULL)
        return;

    if (tree->left != NULL)
        postorder(tree->left, callback);
    if (tree->right != NULL)
        postorder(tree->right, callback);
    callback(tree);
}

static void freeNode(avlTree tree) {
    free(tree);
}

avlTree destructTree(avlTree tree) {
    if (tree != NULL)
        postorder(tree, freeNode);

    return NULL;
}

static void printTreeNode(avlTree tree) {
    printNode2(tree->data);
}

void printTree(avlTree tree) {
    preorder(tree, printTreeNode);
}