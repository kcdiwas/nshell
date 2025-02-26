#include <iostream>
#include <string>
#include <filesystem>
#include <unistd.h>
#include <memory>

class Shell
{
private:
  bool exit_requested = false;

  struct Command
  {
    std::string cmd;
    std::string args;
  };

  Command parseCommand(const std::string &input) const
  {
    size_t space_pos = input.find(' ');
    if (space_pos == std::string::npos)
    {
      return {input, ""};
    }
    return {
        input.substr(0, space_pos),
        input.substr(space_pos + 1)};
  }

  std::string findInPath(const std::string &cmd) const
  {
    const char *path_env = getenv("PATH");
    if (!path_env)
      return "";

    std::string path = path_env;
    size_t start = 0;
    size_t pos;

    while ((pos = path.find(':', start)) != std::string::npos)
    {
      std::string dir = path.substr(start, pos - start);
      std::string full_path = dir + "/" + cmd;

      if (access(full_path.c_str(), F_OK) == 0)
      {
        return full_path;
      }
      start = pos + 1;
    }

    // Check last directory
    std::string dir = path.substr(start);
    std::string full_path = dir + "/" + cmd;
    return (access(full_path.c_str(), F_OK) == 0) ? full_path : "";
  }

  void handleBuiltin(const std::string &cmd, const std::string &args)
  {
    if (cmd == "type")
    {
      executeType(args);
    }
    else if (cmd == "cd")
    {
      executeCD(args);
    }
    else if (cmd == "pwd")
    {
      executePWD();
    }
    else if (cmd == "echo")
    {
      std::cout << args << '\n';
    }
    else if (cmd == "exit" && args == "0")
    {
      exit_requested = true;
    }
  }

  void executeType(const std::string &args)
  {
    Command cmd = parseCommand(args);
    if (isBuiltin(cmd.cmd))
    {
      std::cout << cmd.cmd << " is a shell builtin\n";
    }
    else
    {
      std::string path = findInPath(cmd.cmd);
      if (!path.empty())
      {
        std::cout << cmd.cmd << " is " << path << '\n';
      }
      else
      {
        std::cerr << cmd.cmd << ": not found\n";
      }
    }
  }

  void executeCD(const std::string &args)
  {
    if (args.empty() || args == "~")
    {
      const char *home = getenv("HOME");
      if (home)
      {
        chdir(home);
      }
    }
    else if (chdir(args.c_str()) != 0)
    {
      std::cerr << "cd: " << args << ": No such file or directory\n";
    }
  }

  void executePWD() const
  {
    std::cout << std::filesystem::current_path().string() << '\n';
  }

  bool isBuiltin(const std::string &cmd) const
  {
    static const std::string builtins[] = {
        "echo", "cd", "pwd", "type", "exit"};

    for (const auto &builtin : builtins)
    {
      if (cmd == builtin)
        return true;
    }
    return false;
  }

public:
  void run()
  {
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    while (!exit_requested)
    {
      std::cout << "$ ";
      std::string input;
      std::getline(std::cin, input);

      if (input.empty())
        continue;

      Command cmd = parseCommand(input);

      if (isBuiltin(cmd.cmd))
      {
        handleBuiltin(cmd.cmd, cmd.args);
      }
      else if (!findInPath(cmd.cmd).empty())
      {
        system(input.c_str());
      }
      else
      {
        std::cerr << cmd.cmd << ": command not found\n";
      }
    }
  }
};

int main()
{
  Shell shell;
  shell.run();
  return 0;
}
