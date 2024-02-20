#include "autoGrad.h"
#include "lib.h"

//---------------------------------------------------------------------------------------------------------------------- Value Constructor

/**
 * @note newValue() allocates memory for a Value struct and initializes its fields given passed arguments
 * @param value double to set the value ofthe Value struct to. 
 * @param ancestors optional array of ptrs to ancestor Value structs that created this Value
 * @param ancestorsArrLen The integer number of ancestors in the array
 * @param opString A string identifying the operation that created this node
 * @return A ptr to the newly created Value struct.
*/
Value* newValue(double value, Value* ancestors[], int ancestorArrLen, char opString[]){

    // allocate mem 
    Value* v = (Value*)malloc(sizeof(Value));
    assert(v != NULL);

    // init value fields
    v->value = value;
    v->grad = 0;
    v->ancestorArrLen = ancestorArrLen;

    // New Node (not created from an operation)
    if (ancestorArrLen == NO_ANCESTORS && ancestors == NULL){
        v->ancestors == NULL;
    }
    // New Node (derived from existing nodes via an operation)
    else if (ancestorArrLen > 0 && ancestors != NULL){  

        // allocate mem for ptrs to ancestors
        v->ancestors = (Value**)malloc(ancestorArrLen * sizeof(Value*));
        assert(v->ancestors != NULL);

        // link to ancestor nodes 
        for (int i = 0; i < ancestorArrLen; i++){

            assert(ancestors[i] != NULL);
            v->ancestors[i] = ancestors[i];
        }
    }else{
        printf("Unexpected Behavior in newValue related to graph ancestors");
        exit(0);
    }

    // Backward ptr set to NULL
    v->Backward = NULL;

    // allocate mem for operation str, then copy 
    v->opString = (char*)malloc(strlen(opString) + 1); // +1 for \n 
    assert(v->opString != NULL);
    strcpy(v->opString, opString);

    return v;
}

//---------------------------------------------------------------------------------------------------------------------- Value Destructor

/**
 * @note freeValue is used to free the memory within a value struct
 * @dev all ptrs are set to NULL after releasing
 * @param v ptr to a ptr to a Value to free
*/
void freeValue(Value** v){
   
    assert((*v) != NULL);

    // free dynamically allocated members first
    if ((*v)->ancestors != NULL){

        free((*v)->ancestors); 
        (*v)->ancestors = NULL;
    }
    if ((*v)->opString != NULL){

        free((*v)->opString);
        (*v)->ancestors = NULL;
    }
       
    // free Value struct itself and set to NULL
    free((*v));
    (*v) = NULL;
}


//---------------------------------------------------------------------------------------------------------------------- Add Operation

/**
 * @note addBackward() computes the derivitive operation for two variable addition wrt to Value structs w/ two ancestors 
 * that are the result of calling the Add() function.
 * @dev z = x + y --> dz/dx = 1 and dz/dx = 1
 * @param v ptr to a Value struct to compute the grad of
 */
void addBackward(Value* v){

    assert(v != NULL);
    assert(v->ancestors != NULL);
    assert(v->ancestorArrLen == 2);
    
    // Propagate the gradient to both ancestors. Since the grad is 0 in both cases, applying 
    // the chain rule backwards just means adding the gradient if v to its ancestors
    for(int i = 0; i<2; i++){
        if (v->ancestors[i] != NULL){
            v->ancestors[i]->grad += v->grad;
        }
    }
}

/**
 * @note Add() is used to add two Value structs together. It returns a new Value struct whose ancestors are the inputs 
 * @dev the Backward function ptr of the resulting Value is also set to addBackward()
 * @dev any Value() structs that are created from Add() are considered to be part of the computational graph and are 
 * therefore pushed to a graphStack for later deallocation.
 * @param a A pointer to a Value Struct
 * @param b A pointer to a Value Struct
 * @param graphStack A pointer to a GraphStack struct 
*/
Value* Add(Value* a, Value* b, GraphStack* graphStack){

    assert(a != NULL && b != NULL);
    assert(graphStack != NULL);

    // Create new Value for the sum
    Value* sumValue = newValue(a->value + b->value, (Value*[]){a, b}, 2, "add");

    // push value to the stack. 
    pushGraphStack(graphStack, sumValue);

    // Set the backward function ptr for addition
    sumValue->Backward = addBackward;

    return sumValue;
}

//---------------------------------------------------------------------------------------------------------------------- Mul Operation


/**
 * @note mulBackward() computes the derivitive operation for two variable multiplication wrt to Value structs w/ two ancestors 
 * that are the result of calling the Mul() function.
 * @dev z = x * y is dz/dx = y and dz/dy = x
 * @param v ptr to a Value struct to compute the grad of
 */
void mulBackward(Value* v) {

    assert(v != NULL);
    assert(v->ancestors != NULL);
    assert(v->ancestorArrLen == 2);
   
    assert(v != NULL && v->ancestors != NULL);
    assert(v->ancestors[0] != NULL && v->ancestors[1] != NULL);

    // Apply the chain rule for multivariate multiplication: dz/dx = y * dz and dz/dy = x * dz
    Value* x = v->ancestors[0];
    Value* y = v->ancestors[1];
    x->grad += y->value * v->grad; // dz/dx = y
    y->grad += x->value * v->grad; // dz/dy = x
}

/**
 * @note Mul() is used to multiply two Value structs together. It returns a new Value struct whose ancestors are the inputs 
 * @dev the Backward function ptr of the resulting Value is also set to mulBackward()
 * @dev any Value() structs that are created from Mul() are considered to be part of the computational graph and are 
 * therefore pushed to a graphStack for later deallocation.
 * @param a A pointer to a Value Struct
 * @param b A pointer to a Value Struct
 * @param graphStack A pointer to a GraphStack struct 
*/
Value* Mul(Value* a, Value* b, GraphStack* graphStack) {
    assert(a != NULL && b != NULL);
    assert(graphStack != NULL);

    // Create a new Value for the product
    Value* productValue = newValue(a->value * b->value, (Value*[]){a, b}, 2, "mul");

    // push the new value onto the graph stack
    pushGraphStack(graphStack, productValue);

    // Set the backward function pointer to mulBackward
    productValue->Backward = mulBackward;

    return productValue;
}

//---------------------------------------------------------------------------------------------------------------------- ReLU Operation

/**
 * @note addBackward() computes the derivitive operation for ReLU wrt to Value structs w/ two ancestors 
 * that are the result of calling the ReLU() function.
 * @dev z = x if x > 0 else 0 --> dz/dx = 1 if x > 0 else 0
 * @param v ptr to a Value struct to compute the grad of
 */
void reluBackward(Value* v){

    assert(v != NULL);
    assert(v->ancestors != NULL);
    assert(v->ancestorArrLen == 1);
    assert(v->ancestors[0] != NULL);

    if (v->ancestors[0]->value > 0){
            v->ancestors[0]->grad += v->grad; // dz/dx = 1 case
    }
    // @note dz/dx = 0 case is not handled here because the grad is already 0
}

/**
 * @note ReLU() applies ReLU to a Value struct. It returns a new Value struct whose ancestors are the inputs 
 * @dev the Backward function ptr of the resulting Value is also set to reluBackward()
 * @dev any Value() structs that are created from ReLU() are considered to be part of the computational graph and are 
 * therefore pushed to a graphStack for later deallocation.
 * @param a A pointer to a Value Struct
 * @param graphStack A pointer to a GraphStack struct 
*/
Value* ReLU(Value* a, GraphStack* graphStack) {

    assert(a != NULL);
    assert(graphStack != NULL);

    // Create a new Value for the ReLU activation
    double reluResult = a->value > 0 ? a->value : 0; // f(x) = max(0, x)
    Value* reluValue = newValue(reluResult, (Value*[]){a}, 1, "relu");

    // push the new value onto the graph stack
    pushGraphStack(graphStack, reluValue);

    // Set the backward function pointer to reluBackward
    reluValue->Backward = reluBackward;

    return reluValue;
}