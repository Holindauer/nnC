#pragma once

// lib.h

// standard libs
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <math.h>

// header files
#include "value.h"
#include "autoGrad.h"
#include "graphStack.h"
#include "hashTable.h"
#include "backward.h"
#include "mlp.h"
#include "forward.h"
#include "gradientDescent.h"
#include "loss.h"

// macros
#define NO_ANCESTORS 0
#define BINARY 1
#define UNARY 0
#define HASHTABLE_SIZE 150
#define EPSILON 1e-10 
#define NON_TRAINING_CALL 0