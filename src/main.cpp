#include <iostream>

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // Uncomment this block to pass the first stage
  do {
    std::cout << "$ ";

    std::string input;
    std::getline(std::cin, input);

    std::cerr << input << ": command not found" << "\n";
    
  } while(true);
}
