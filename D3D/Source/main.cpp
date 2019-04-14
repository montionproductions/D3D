#include "Framework.h"

int main() {
  Framework* framwork = new Framework();
   
  if(framwork->Initilize()) {
    framwork->Run();
  }
  
  delete framwork;

  return 0;
}