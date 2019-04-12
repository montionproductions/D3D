#include "Framework.h"

void main() {
  Framework* framwork = new Framework();
   
  if(framwork->Initilize()) {
    framwork->Run();
  }
  
  delete framwork;
  return;
}