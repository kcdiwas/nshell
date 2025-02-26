#include <iostream>
#include <stdlib.h>
#include <unistd.h>

std::string printCmd(std::string input)
{
  int limiterIndex = input.find(" ");
  int cutoffLength = limiterIndex;
  if (limiterIndex == -1)
  {
    cutoffLength = input.size();
    limiterIndex = input.size();
  }
  else
  {
    limiterIndex++;
  }

  return input.substr(0, cutoffLength);
}

std::string printArgs(std::string input)
{
  int limiterIndex = input.find(" ");
  if (limiterIndex == -1)
  {
    limiterIndex = input.size();
  }
  else
  {
    limiterIndex++;
  }

  return input.substr(limiterIndex, input.size());
}

std::string checkPath(std::string cmd)
{
  std::string path = getenv("PATH");

  size_t start = 0;
  size_t pos = path.find(":");

  while (start < path.size())
  {
    std::string dir;
    if (pos == std::string::npos)
    {
      dir = path.substr(start);
      start = path.size();
    }
    else
    {
      dir = path.substr(start, pos - start);
      start = pos + 1;
      pos = path.find(":", start);
    }

    std::string fullPath = dir + "/" + cmd;
    if (access(fullPath.c_str(), F_OK) == 0)
    {
      return fullPath;
    }
  }
  return "";
}

bool runType(std::string args)
{
  std::string cmd = printCmd(args);
  if (cmd == "echo")
  {
    std::cout << "echo is a shell builtin" << "\n";
    return false;
  }
  else if (cmd == "exit")
  {
    std::cout << "exit is a shell builtin" << "\n";
    return false;
  }
  else if (cmd == "type")
  {
    std::cout << "type is a shell builtin" << "\n";
    return false;
  }
  else if (!checkPath(cmd).empty())
  {
    std::cout << cmd << " is " << checkPath(cmd) << "\n";
    return false;
  }
  else
  {
    std::cerr << cmd << ": not found" << "\n";
    return false;
  }
}

int main()
{
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  bool exit = false;

  // Uncomment this block to pass the first stage
  do
  {
    std::cout << "$ ";

    std::string input;
    std::getline(std::cin, input);

    std::string cmd = printCmd(input);
    std::string args = printArgs(input);
    if (cmd == "exit")
    {
      if (args == "0")
      {
        exit = true;
      }
    }
    else if (cmd == "echo")
    {
      std::cout << args << "\n";
    }
    else if (cmd == "type")
    {
      runType(args);
    }
    else if (!checkPath(cmd).empty())
    {
      system(input.c_str());
    }
    else
    {
      std::cerr << cmd << ": command not found" << "\n";
    }

  } while (exit == false);
}
