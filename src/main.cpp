#include "ncurses_display.h"
#include "system.h"

#include <iostream> //used for testing
#include "linux_parser.h"

int main() {
  System system;
  // NCursesDisplay::Display(system);
  auto p = Process(1);
  std::cout << LinuxParser::Command(1) << "\n";
  
}