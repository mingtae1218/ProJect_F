#include "SuperLoop.h"
#include <Arduino.h>

SuperLoop::SuperLoop(){
      previousMillis =0;
      cycle =0;
   }
   void SuperLoop::begin(unsigned long cycle){
      this->cycle =cycle;
      previousMillis = millis();
   }

void SuperLoop::loop(){
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= cycle && !Stop){
	 previousMillis = currentMillis;
	 job();

  }
}

   SuperLoop:: ~ SuperLoop(){

   }

   void SuperLoop::setCycle(unsigned long cycle) {
	   this->cycle = cycle;
   }
   unsigned long SuperLoop::getCycle() {
	   return cycle;
   }
   void SuperLoop::setStop(bool Stop) {
	   this->Stop =Stop;
   }
   bool SuperLoop::isStop() {
	   return Stop;
   }

