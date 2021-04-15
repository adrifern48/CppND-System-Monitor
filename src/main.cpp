#include "ncurses_display.h"
#include "system.h"

#include <iostream> //used for testing
#include "process.h"

int main() {
  System system;
  NCursesDisplay::Display(system);
  // std::vector<Process> test = system.Processes();
  
}