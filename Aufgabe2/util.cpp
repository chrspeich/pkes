
#include "util.h"

double mean_push_get(double* mean, size_t size, double next) {
  double result = 0.0;
  double n = 0;
  
  for (size_t i = 0; i < size; i++) {
    result += mean[i];
    n += 1;
  }
  
  for (size_t i = 0; i < size - 1; i++) {
    mean[i] = mean[i+1];
  }
  
  mean[size - 1] = next;
  
  return result / n;
}
