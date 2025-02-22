#include <iostream>
using namespace std;

(*function)(double) derivate((*function)(double)){
      stepsize = 1e-4;
      double derivate(double x) {
        return (function(x+h) - function(x))/h;
      }
      return (*derivate);
}
