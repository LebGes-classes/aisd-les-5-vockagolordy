#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cmath>

using namespace std;

struct Node {
    int val;
    int kids;
    bool mark;
    Node* parent;
    Node* child;
    Node* left;
    Node* right;

    Node(int v) {
        val = v;
        kids = 0;
        mark = false;
        parent = NULL;
        child = NULL;
        left = this;
        right = this;
    }
};

class FibHeap {
    Node* max;
    int count;

public:
    FibHeap() {
        max = NULL;
        count = 0;
    }

    ~FibHeap() {
        if (max == NULL) return;

        vector<Node*> toDelete;
        Node* current = max;

        do {
            toDelete.push_back(current);
            if (current->child != NULL) {
                Node* kid = current->child;
                do {
                    toDelete.push_back(kid);
                    kid = kid->right;
                } while (kid != current->child);
            }
            current = current->right;
        } while (current != max);

        for (Node* node : toDelete) {
            delete node;
        }
    }

    Node* insert(int x) {
        Node* newNode = new Node(x);

        if (max == NULL) {
            max = newNode;
        }
        else {
            newNode->left = max->left;
            newNode->right = max;
            max->left->right = newNode;
            max->left = newNode;

            if (newNode->val > max->val) {
                max = newNode;
            }
        }
        count++;
        return newNode;
    }

    int getMax() {
        if (max == NULL) {
            return -1;
        }
        return max->val;
    }

    int extractMax() {
        if (max == NULL) {
            return -1;
        }

        Node* oldMax = max;
        int result = oldMax->val;

        if (oldMax->child != NULL) {
            Node* kid = oldMax->child;
            do {
                Node* next = kid->right;
                kid->parent = NULL;
                kid->left = max->left;
                kid->right = max;
                max->left->right = kid;
                max->left = kid;
                kid = next;
            } while (kid != oldMax->child);
        }

        oldMax->left->right = oldMax->right;
        oldMax->right->left = oldMax->left;

        if (oldMax == oldMax->right) {
            max = NULL;
        }
        else {
            max = oldMax->right;
            makeGood();
        }

        delete oldMax;
        count--;
        return result;
    }

    void increaseKey(Node* node, int newVal) {
        if (newVal < node->val) {
            return;
        }

        node->val = newVal;
        Node* parent = node->parent;

        if (parent != NULL && node->val > parent->val) {
            cut(node, parent);
            cascadingCut(parent);
        }

        if (node->val > max->val) {
            max = node;
        }
    }

    void merge(FibHeap& other) {
        if (other.max == NULL) {
            return;
        }

        if (max == NULL) {
            max = other.max;
        }
        else {
            Node* ourLeft = max->left;
            Node* theirLeft = other.max->left;

            ourLeft->right = other.max;
            other.max->left = ourLeft;
            max->left = theirLeft;
            theirLeft->right = max;

            if (other.max->val > max->val) {
                max = other.max;
            }
        }
        count += other.count;
        other.max = NULL;
        other.count = 0;
    }

    bool empty() {
        return count == 0;
    }

private:
    void makeGood() {
        int maxDegree = (int)(log(count) / log(2)) + 1;
        vector<Node*> degreeTable(maxDegree, NULL);

        vector<Node*> roots;
        Node* current = max;
        do {
            roots.push_back(current);
            current = current->right;
        } while (current != max);

        for (Node* root : roots) {
            int deg = root->kids;
            while (degreeTable[deg] != NULL) {
                Node* other = degreeTable[deg];
                if (root->val < other->val) {
                    swap(root, other);
                }
                link(other, root);
                degreeTable[deg] = NULL;
                deg++;
            }
            degreeTable[deg] = root;
        }

        max = NULL;
        for (Node* node : degreeTable) {
            if (node != NULL) {
                if (max == NULL) {
                    max = node;
                    node->left = node;
                    node->right = node;
                }
                else {
                    node->left = max->left;
                    node->right = max;
                    max->left->right = node;
                    max->left = node;

                    if (node->val > max->val) {
                        max = node;
                    }
                }
            }
        }
    }

    void link(Node* child, Node* parent) {
        child->left->right = child->right;
        child->right->left = child->left;

        child->parent = parent;
        if (parent->child == NULL) {
            parent->child = child;
            child->left = child;
            child->right = child;
        }
        else {
            child->left = parent->child->left;
            child->right = parent->child;
            parent->child->left->right = child;
            parent->child->left = child;
        }
        parent->kids++;
        child->mark = false;
    }

    void cut(Node* node, Node* parent) {
        node->left->right = node->right;
        node->right->left = node->left;

        if (parent->child == node) {
            if (node->right == node) {
                parent->child = NULL;
            }
            else {
                parent->child = node->right;
            }
        }
        parent->kids--;

        node->parent = NULL;
        node->left = max->left;
        node->right = max;
        max->left->right = node;
        max->left = node;
        node->mark = false;
    }

    void cascadingCut(Node* node) {
        Node* parent = node->parent;
        if (parent != NULL) {
            if (!node->mark) {
                node->mark = true;
            }
            else {
                cut(node, parent);
                cascadingCut(parent);
            }
        }
    }
};

class PriorityQueue {
    FibHeap heap;

public:
    void enqueue(int value) {
        heap.insert(value);
    }

    int dequeue() {
        return heap.extractMax();
    }

    int peek() {
        return heap.getMax();
    }

    void increasePriority(Node* node, int newValue) {
        heap.increaseKey(node, newValue);
    }

    void merge(PriorityQueue& other) {
        heap.merge(other.heap);
    }

    bool isEmpty() {
        return heap.empty();
    }

    Node* getNode(int value) {
        return heap.insert(value);
    }
};

int main() {
    srand(time(0));

    PriorityQueue q;
    vector<Node*> nodes;

    clock_t start = clock();
    for (int i = 0; i < 1000; i++) {
        int val = rand() % 1000;
        nodes.push_back(q.getNode(val));
    }
    double addTime = (clock() - start) * 1000.0 / CLOCKS_PER_SEC;

    start = clock();
    for (int i = 0; i < 100; i++) {
        q.peek();
    }
    double peekTime = (clock() - start) * 1000.0 / CLOCKS_PER_SEC;

    start = clock();
    for (int i = 0; i < 100; i++) {
        int idx = rand() % nodes.size();
        q.increasePriority(nodes[idx], nodes[idx]->val + 500);
    }
    double incTime = (clock() - start) * 1000.0 / CLOCKS_PER_SEC;

    start = clock();
    while (!q.isEmpty()) {
        q.dequeue();
    }
    double deqTime = (clock() - start) * 1000.0 / CLOCKS_PER_SEC;

    cout << "Operation\tTime (ms)\n";
    cout << "Add\t\t" << addTime << "\n";
    cout << "Peek\t\t" << peekTime << "\n";
    cout << "Increase\t" << incTime << "\n";
    cout << "Dequeue\t\t" << deqTime << "\n";

    return 0;
}