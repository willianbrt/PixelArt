#include "SymmetryContext.h"


int SymmetryContext::pointMirrored(float point, float comprimento){
    return comprimento - point - 1; 
}
int SymmetryContext::pointMirrored(int point, int center, int comprimento){
    return comprimento - center - point - 1; 
}