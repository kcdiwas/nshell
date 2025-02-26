#include <iostream>

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  
  bool exit = false;

  // Uncomment this block to pass the first stage
  do {
    std::cout << "$ ";

    std::string input;
    std::getline(std::cin, input);

    int limiterIndex = input.find(" "); 
    int cutoffLength = limiterIndex;
    if (limiterIndex == -1) {
      cutoffLength = input.size();
      limiterIndex = input.size();
    } else {
      limiterIndex++;
    }

    std::string cmd = input.substr(0,cutoffLength);
    std::string args = input.substr(limiterIndex, input.size());    
    if (cmd == "exit") {
      if (args == "0") {
        exit = true;
      }
    } else if (cmd == "echo") {
      std::cout << args;
      exit = true;
    } else {
        std::cerr << cmd << ": command not found" << "\n";
    }
    
  } while(exit == false);
}
