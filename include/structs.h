#pragma once
#include "libraries.h"
#include "macros.h"

// structs.h

//------------------------------------------------------------------------------------------------------------------ Value Struct

/**
 * @notice pBackwardFunc is a pointer to a function that computes the derivative of the operation 
 * that produced a given Value struct based on it's ancestor values.
 * @param Value* The value struct to compute the derivative for based on it's ancestors.
*/
typedef struct _value Value; // <--- forward declaration for self-reference in pBackwardFunc
typedef void (*pBackwardFunc)(Value*);

/**
 * @notice Value is the central struct in the autoGrad.c implementation. It represents a single value in the
 * computational graph as it passes through the network. The graph is constructed as operations are performed.
 * @param value The value of the node
 * @param grad The gradient of the node
 * @param Backward ptr to a d/dx function for the operation that produced the value
 * @param ancestors arr of ancestor nodes (dynamically allocated)
 * @param op Str of operation that produced the value (debugging)
 * @param ancestorArrLen length of the ancestors array
*/
typedef struct _value {
    double value;             
    double grad;              
    pBackwardFunc Backward; 
    Value **ancestors;       
    char* opStr; 
    int ancestorArrLen;        
} Value;

//------------------------------------------------------------------------------------------------------------------ Value Tracker Struct

/**
 * @notice GraphNode is node in a stack of Value structs. It is used to store all newly created Value structs. 
 * The stack's function is to divorce the deallocation of the computational graph from the need to traverse the 
 * computational graph. This mechanism is being used to avoid recursively deallocating the graph, which is less 
 * obvious as to how it works and more error prone.
 * @param value A pointer to a Value struct
 * @param next A pointer to the next ValueTracker struct in the list
 * 
*/
typedef struct _graphNode {
    Value* value;
    struct _graphNode* next;
} GraphNode;


/**
 * @notice Graph Stack is a stack of ValueTracker structs. It is used to store all newly created Value structs.
 * @param head A pointer to the head of the stack
 * @param len The length of the stack
*/
typedef struct {
    GraphNode* head;
    int len;
} GraphStack;

//------------------------------------------------------------------------------------------------------------------ Hash Table Struct

/**
 * @notice VisitedNode is a struct that represents a node in the hash table
 * @param key The value of the node
 * @param next A pointer to the next node in the list
*/
typedef struct VisitedNode {
    Value* key;
    struct VisitedNode* next;
} VisitedNode;

/**
 * @notice HashTable is a struct that represents a hash table
 * @param buckets An array of pointers to VisitedNode structs
 * @param size The size of the hash table
*/
typedef struct {
    VisitedNode** buckets;
    int size;
} HashTable;

//------------------------------------------------------------------------------------------------------------------ Neural Network Struct

/**
 * @notice Layer is a struct that represents a single layer in a multilayer perceptron.
 * @param inputSize The layer's input vector size (alsoe the number of neurons in the previous layer)
 * @param outputSize The layer's output vector size (also the number of neurons in the current layer)
 * @param weights A matrix of weights connecting the previous layer to the current layer. Weights are 
 * stored as a contiguous array of Value structs in row-major order. len == (inputSize x outputSize)
 * @param biases A vector of biases for the current layer. Biases are stored as a contiguous array of
 * Value structs. len == outputSize
 * @param next A pointer to the next layer in the network
 * @param prev A pointer to the previous layer in the network
*/
typedef struct _layer {
    int inputSize;
    int outputSize;
    Value** weights;
    Value** biases;
    Value** outputVector;
    struct _layer* next;
    struct _layer* prev;
} Layer;

/**
 * @notice mlp is a struct that represents a multi-layer perceptron. It houses the start and end of the
 * network as well as the number of layers in the network.
 * @param inputLayer A pointer to the first layer in the network
 * @param outputLayer A pointer to the last layer in the network
 * @param numLayers The number of layers in the network
 * @param graphStack A stack of all unique Value ptrs within the computational graph. Not including 
 * the weights, biases, and output vectors of the mlp itself.
*/
typedef struct {
    Layer* inputLayer;
    Layer* outputLayer;
    int numLayers;
    GraphStack* graphStack;
} MLP;

//------------------------------------------------------------------------------------------------------------------ Function Prototypes

// Auto Grad Related Prototypes
Value* newValue(double _value, Value* _ancestors[], int _ancestorArrLen, char _opStr[]);
void addBackward(Value* v);
Value* Add(Value* a, Value* b, GraphStack *graphStack);
void mulBackward(Value* v);
Value* Mul(Value* a, Value* b, GraphStack *graphStack);
void reluBackward(Value* v);
Value* ReLU(Value* a, GraphStack *graphStack);
void dfs(Value* v, HashTable* visited, Value*** stack, int* index);
void reverseArray(Value** arr, int start, int end);
void reverseTopologicalSort(Value* start, Value*** sorted, int* count);
void Backward(Value* v);
void freeValue(Value* v);
void releaseGraph(GraphStack* graphStack);

// Hash Table Related Prototypes
HashTable* createHashTable(int size);
unsigned int hashValuePtr(void* ptr, int size);
void insertVisited(HashTable* table, Value* value);
bool isVisited(HashTable* table, Value* value);
void freeHashTable(HashTable* table);

// Load Data Related Prototypes
void loadData(Value* features[][IRIS_FEATURES], Value* targets[][IRIS_CLASSES]);
void freeDataFeatures(Value* dataArr[][IRIS_FEATURES], int numRows);
void freeDataTargets(Value* dataArr[][IRIS_CLASSES], int numRows);

// Neural Network Related Prototypes
Value** initWeights(int inputSize, int outputSize);
Value** initBiases(int outputSize);
void freeWeights(Value** weights, int inputSize, int outputSize);
void freeBiases(Value** biases, int outputSize);
MLP* createMLP(int inputSize, int layerSizes[], int numLayers);
void freeMLP(MLP* mlp);
Value** initOutputVector(int outputSize);
void zeroGrad(MLP** mlp, int inputSize, int layerSizes[], int numLayers);

// Forward Pass Related Prototypes
void MultiplyWeights(Layer* layer, Value** input, GraphStack* graphStack);
void AddBias(Layer* layer, Value** input, GraphStack* graphStack);
Value** copyInput(Value** input, int inputSize);
void Forward(MLP* mlp, Value** input);

// SGD related prototypes
void Step(MLP* mlp, int lr);

// Graph Tracker Related Prototypes
GraphStack* newGraphStack(void);
void pushGraphStack(GraphStack* stack, Value* value);
void popGraphStack(GraphStack* stack);