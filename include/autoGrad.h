#pragma once
#include "lib.h"

// autoGrad.h


/**
 * @notice pBackwardFunc is a pointer to a function that computes the derivative of the operation 
 * that produced a given Value struct based on it's ancestor values.
 * @param Value* The value struct to compute the derivative for based on it's ancestors.
*/
typedef struct _value Value; // <--- forward declaration for self-reference in pBackwardFunc
typedef void (*pBackwardFunc)(Value*);

/**
 * @notice Value represents a single node in the computational graph as it passes through the network. 
 * The graph is constructed as operations are performed.
 * @dev operations are 
 * @param value The double value of the node
 * @param grad The partial derivative value for the node wrt the final output of the graph
 * @param Backward ptr to a d/dx function for the operation that produced the value [Add(), Mull(), ReLU()...]
 * @param ancestors arr of ancestor nodes (dynamically allocated) 
 * @param op String indicating the operation that produced the value (debugging)
 * @param ancestorArrLen length of the ancestors array
*/
typedef struct _value {
    double value;             
    double grad;              
    pBackwardFunc Backward; 
    Value** ancestors;
    char* opString; 
    int ancestorArrLen;        
} Value;

/**
 * @notice GraphNode is used to track each node within a stack of Value structs (GraphStack). 
 * @dev Each time a Value is created during the forward pass, it is stored in a GraphNode and 
 * pushed to the GraphStack. This allows for sequential deallocation of memory for complicated
 * computation graphs without risk of double frees. 
 * @param pValStruct A pointer to the Value struct at this node
 * @param next A pointer to the next ValueTracker struct in the stack
*/
typedef struct _graphNode {
    Value* pValStruct;
    struct _graphNode* next;
} GraphNode;

/**
 * @notice GraphStack is a stack of GraphNode for storing all Value structs created during the 
 * forward pass.
 * @dev This allows for sequential deallocation of the computational graph.
 * @param head A pointer to the head of the stack
 * @param len The length of the stack
*/
typedef struct {
    GraphNode* head;
    int len;
} GraphStack;



// Value Constructor/Destructor
Value* newValue(double value, Value* ancestors[], int ancestorArrLen, char opString[]);
void freeValue(Value** v);

// GraphStack related functions
GraphStack* newGraphStack(void);
void pushGraphStack(GraphStack* stack, Value* value);
void popGraphStack(GraphStack* stack);
void releaseGraph(GraphStack* graphStack);

// Value Operations
void addBackward(Value* v);
Value* Add(Value* a, Value* b, GraphStack* graphStack);
void mulBackward(Value* v);
Value* Mul(Value* a, Value* b, GraphStack* graphStack);
void reluBackward(Value* v);
Value* ReLU(Value* a, GraphStack* graphStack);