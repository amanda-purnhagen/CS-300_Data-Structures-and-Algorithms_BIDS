#include <iostream>
#include <time.h>

#include "CSVparser.hpp"

using namespace std;

//============================================================================
// Global definitions visible to all methods and classes
//============================================================================

// Forward declarations
double strToDouble(string str, char ch);
struct Bid {
    string bidId; // unique identifier
    string title;
    string fund;
    double amount;
    Bid() {
        amount = 0.0;
    }
};

// Define a structure to hold bid information

class BinarySearchTree {

private:
    
    // Internal structure for tree node
    struct Node {
        Bid bid;
        // Create the key for the given bid
        unsigned key;
        
        Node* left = nullptr;
        Node* right = nullptr;

        unsigned int Hash(int key) {
            unsigned bucketIndex;
            bucketIndex = key % 7;
            return bucketIndex;
        }

        Node(Bid aBid) {
            this->bid = aBid;
            key = Hash(atoi(this->bid.bidId.c_str()));
        }
    };
    Node* root;
    void addNode(Node* parentNode, Node* newNode);
    void inOrder(Node* node);
    void postOrder(Node* node);
    void preOrder(Node* node);
    Node* minValue(Node* node) {
        Node* current = root;
        while (current && current->left != NULL) {
            current = current->left;
        }
        return current;
    };
    void removeNode(Node* node, string bidId) {
        unsigned key = atoi(bidId.c_str()) % 7;
        Node* parentNode = nullptr;
        while (node != nullptr) {
            if (node->key == key) {
                if (node->bid.bidId == bidId) {
                    int c = 0;
                    // If the specified node is a leaf (has two empty subtrees)
                    if (node->left == nullptr && node->right == nullptr) {
                        if (parentNode != nullptr) {
                            parentNode->left = nullptr;
                        }
                        free(node);
                        return;
                    }

                    // If left subtree is not null AND right subtree is null
                    if (node->left != nullptr && node->right == nullptr) {
                        if (parentNode != nullptr) {
                            parentNode->left = node->left;
                        }
                        free(node);
                        return;
                    }

                    // If left subtree is null AND right subtree is not null
                    if (node->left == nullptr && node->right != nullptr) {
                        if (parentNode != nullptr) {
                            parentNode->left = node->right;
                        }
                        free(node);
                        return;
                    }

                    // If left subtree is not null AND right subtree is not null
                    // First, find in-order successor of node
                    // Next, copy content of that successor to "node"
                    // Finally, delete the in-order successor
                    if (node->left != nullptr && node->right != nullptr) {
                        parentNode->right = minValue(node->right);
                        parentNode->right->left = node->left;
                        return;
                    }
                }
                else {
                    // If it's the right key but not the right bid
                    parentNode = node;
                    node = node->left;
                }
            }
            // The key does not match the bid. Move on in-order
            // Bids in the left subtree are smaller than that of current node
            else if (node->key < key) {
                parentNode = node;
                node = node->left;
            }
            // Bids in the right subtree are bigger than that of current node
            else if (node->key > key) {
                parentNode = node;
                node = node->right;
            }
        }
    }

public:
    BinarySearchTree();
    virtual ~BinarySearchTree();
    void InOrder();
    void PostOrder();
    void PreOrder();
    void Insert(Bid bid);
    void Remove(string bidId);
    Bid Search(string bidId);
};

/**
 * Default constructor
 */
BinarySearchTree::BinarySearchTree() {
    // Initialize housekeeping variables: Assign nullptr to root
    this->root = nullptr;
}

/**
 * Destructor
 */
BinarySearchTree::~BinarySearchTree() {
    // recurse from root deleting every node
    Node* node = root;
    if (node != nullptr) {
        inOrder(node->left);
        cout << node->key << ":" << node->bid.bidId << endl;
        inOrder(node->right);
        free(node);
    }
}

/**
 * Traverse the tree in order
 */
void BinarySearchTree::InOrder() {
    inOrder(root);
}

/**
 * Traverse the tree in post-order
 */
void BinarySearchTree::PostOrder() {
    postOrder(root);
}

/**
 * Traverse the tree in pre-order
 */
void BinarySearchTree::PreOrder() {
    preOrder(root);
}

/**
 * Insert a bid
 */
void BinarySearchTree::Insert(Bid bid) {
    Node* newNode = new Node(bid);
    // If the BST is empty, assign root with newNode
    if (root == nullptr) {
        root = newNode;
    }
    // Else, call the addNode() method
    else {
        addNode(root, newNode);
    }
}

/**
 * Remove a bid
 */
void BinarySearchTree::Remove(string bidId) {
    removeNode(root, bidId);
}

/**
 * Search for a bid
 */
Bid BinarySearchTree::Search(string bidId) {
    Node* node = root;
    Bid bid;
    // Hash the bidId
    unsigned key = atoi(bidId.c_str()) % 7;
    while (node != nullptr) {
        // Search for the key
        // If the node's key matches the provided bidId's hashed key
        if (node->key == key) {
            // And the node's bidId matches the provided bidId
            if (node->bid.bidId == bidId) {
                // Then we found our search query
                return node->bid;
            }
        }
        // If the current node's key value is less than our search query key
        if (node->key <= key) {
            // Then move on in-order
            node = node->left;
        }
        else {
            node = node->right;
        }
    }

    // In the case that no existing bids matched the search query
    return bid;
}

/**
 * Add a bid to some node (recursive)
 */
void BinarySearchTree::addNode(Node* parentNode, Node* newNode) {
    // If the newNode's key is larger than the current node's key
    // AND the left subtree is empty
    // Then the newNode is placed as the left subtree
    if (parentNode->key <= newNode->key) {
        if (parentNode->left == nullptr) {
            parentNode->left = newNode;
        }
        // If the left subtree is NOT empty
        else {
            // Then recursively locate the next empty left subtree
            addNode(parentNode->left, newNode);
        }
    }

    // If the newNode's key is smaller than the current node's key
    // AND the right subtree is empty
    // Then the newNode is placed as the right subtree
    if (parentNode->key > newNode->key) {
        if (parentNode->right == nullptr) {
            parentNode->right = newNode;
        }
        // If the right subtree is NOT empty
        else {
            // Then recursively locate the next empty right subtree
            addNode(parentNode->right, newNode);
        }
    }
}

void BinarySearchTree::inOrder(Node* node) {
    // If the BST is not empty
    if (node != nullptr) {
        // Node bidIds in the left subtree are smaller than root
        inOrder(node->left);
        // Recursively print the left-most node before the right node
        cout << node->bid.bidId << ": " << node->bid.title << " " <<
            node->bid.amount << " " << node->bid.fund << endl;
        inOrder(node->right);
    }
}

void BinarySearchTree::postOrder(Node* node) {
    if (node != nullptr) {
        postOrder(node->left);
        postOrder(node->right);
        // Recursively print the right node before the left node for
        // reverse order
        cout << node->bid.bidId << ": " << node->bid.title << " " << 
            node->bid.amount << " " << node->bid.fund << endl;
    }
}

void BinarySearchTree::preOrder(Node* node) {
    // Prints the the root, then the left subtree, then the right subtree
    if (node != nullptr) {
        cout << node->bid.bidId << ": " << node->bid.title << " " <<
            node->bid.amount << " " << node->bid.fund << endl;
        preOrder(node->left);
        preOrder(node->right);
    }
}

//============================================================================
// Static methods used for testing
//============================================================================

/**
 * Display the bid information to the console (std::out)
 *
 * @param bid struct containing the bid info
 */
void displayBid(Bid bid) {
    cout << bid.bidId << ": " << bid.title << " | " << bid.amount << " | "
            << bid.fund << endl;
    return;
}

/**
 * Load a CSV file containing bids into a container
 *
 * @param csvPath the path to the CSV file to load
 * @return a container holding all the bids read
 */
void loadBids(string csvPath, BinarySearchTree* bst) {
    cout << "Loading CSV file " << csvPath << endl;

    // initialize the CSV Parser using the given path
    csv::Parser file = csv::Parser(csvPath);

    // read and display header row (optional)
    vector<string> header = file.getHeader();
    for (auto const& c : header) {
        cout << c << " | ";
    }
    cout << "" << endl;

    try {
        // Loop to read rows of the CSV file
        for (unsigned int i = 0; i < file.rowCount(); i++) {

            // Create a data structure and add to the collection of bids
            Bid bid;
            bid.bidId = file[i][1];
            bid.title = file[i][0];
            bid.fund = file[i][8];
            bid.amount = strToDouble(file[i][4], '$');

            cout << "Item: " << bid.title << ", Fund: " << bid.fund << ", Amount: " << bid.amount << endl;

            // push this bid to the end
            bst->Insert(bid);
        }
    } catch (csv::Error &e) {
        std::cerr << e.what() << std::endl;
    }
}

/**
 * Simple C function to convert a string to a double
 * after stripping out unwanted char
 *
 * credit: http://stackoverflow.com/a/24875936
 *
 * @param ch The character to strip out
 */
double strToDouble(string str, char ch) {
    str.erase(remove(str.begin(), str.end(), ch), str.end());
    return atof(str.c_str());
}

/**
 * The main() method
 */
int main(int argc, char* argv[]) {

    // process command line arguments
    string csvPath, bidKey;
    switch (argc) {
    case 2:
        csvPath = argv[1];
        bidKey = "98109";
        break;
    case 3:
        csvPath = argv[1];
        bidKey = argv[2];
        break;
    default:
        csvPath = "res/eBid_Monthly_Sales_Dec_2016.csv";
        bidKey = "98109";
    }

    // Define a timer variable
    clock_t ticks;

    // Define a binary search tree to hold all bids
    BinarySearchTree* bst = nullptr;

    Bid bid;

    int choice = 0;
    while (choice != 9) {
        cout << "Menu:" << endl;
        cout << "  1. Load Bids" << endl;
        cout << "  2. Display All Bids" << endl;
        cout << "  3. Find Bid" << endl;
        cout << "  4. Remove Bid" << endl;
        cout << "  9. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {

        case 1:
            bst = new BinarySearchTree();

            // Initialize a timer variable before loading bids
            ticks = clock();

            // Complete the method call to load the bids
            loadBids(csvPath, bst);

            //cout << bst->Size() << " bids read" << endl;

            // Calculate elapsed time and display result
            // Current clock ticks minus starting clock ticks
            ticks = clock() - ticks; 
            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;
            break;

        case 2:
            bst->InOrder();
            break;

        case 3:
            ticks = clock();

            bid = bst->Search(bidKey);

            // Current clock ticks minus starting clock ticks
            ticks = clock() - ticks;

            if (!bid.bidId.empty()) {
                displayBid(bid);
            } else {
            	cout << "Bid ID " << bidKey << " not found." << endl;
            }

            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;

            break;

        case 4:
            bst->Remove(bidKey);
            break;

        case 9:
            bst->~BinarySearchTree();
            break;
        }
    }

    cout << "Good bye." << endl;

	return 0;
}
