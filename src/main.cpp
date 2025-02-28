#include <iostream>
#include <string>
#include <filesystem>
#include <unistd.h>
#include <memory>
#include <vector>

class Shell
{
private:
  bool exit_requested = false;

  struct Command
  {
    std::string cmd;
    std::vector<std::string> args;
  };

  Command parseCommand(const std::string &input) const
  {
    size_t space_pos = input.find(' ');
    if (space_pos == std::string::npos)
    {
      return {input, {""}};
    }
    std::string args = input.substr(space_pos + 1);
    std::vector<std::string> args_vector;
    parseQuotedArgs(args, args_vector);
    return {input.substr(0, space_pos), args_vector};
  }

  void parseQuotedArgs(const std::string &args, std::vector<std::string> &args_vector) const
  {

    std::string new_args = args;
    std::string space_str = "";
    while (new_args[0] == ' ')
    {
      space_str += " ";
      new_args = new_args.substr(1);
    }
    args_vector.push_back(space_str);
    if (!new_args.empty() && (new_args[0] == '\'' || new_args[0] == '"'))
    {
      char quote = new_args[0];
      size_t end_quote = new_args.find(quote, 1);
      if (end_quote != std::string::npos)
      {
        args_vector.push_back(new_args.substr(1, end_quote - 1));
        parseQuotedArgs(new_args.substr(end_quote + 1), args_vector);
      }
    }
    else if (!new_args.empty())
    {
      bool isLastSpace = false;
      std::string new_str;
      for (char c : new_args)
      {
        if (c == ' ')
        {
          if (!isLastSpace)
          {
            new_str += c;
          }
          isLastSpace = true;
        }
        else
        {
          new_str += c;
          isLastSpace = false;
        }
      }
      args_vector.push_back(new_str);
    }
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

  void handleBuiltin(const std::string &cmd, const std::vector<std::string> &args)
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
      for (const auto &arg : args)
      {
        std::cout << arg;
      }
      std::cout << '\n';
    }
    else if (cmd == "exit")
    {
      std::string new_args;
      for (const auto &arg : args)
      {
        if (!arg.empty() && arg.find_first_not_of(' ') != std::string::npos)
        {
          new_args = arg;
          break;
        }
      }
      if (new_args == "0")
      {
        exit_requested = true;
      }
    }
  }

  void executeType(const std::vector<std::string> &args)
  {
    std::string new_args;
    for (const auto &arg : args)
    {
      if (!arg.empty() && arg.find_first_not_of(' ') != std::string::npos)
      {
        new_args = arg;
        break;
      }
    }
    if (isBuiltin(new_args))
    {
      std::cout << new_args << " is a shell builtin\n";
    }
    else
    {
      std::string path = findInPath(new_args);
      if (!path.empty())
      {
        std::cout << new_args << " is " << path << '\n';
      }
      else
      {
        std::cerr << new_args << ": not found\n";
      }
    }
  }

  void executeCD(const std::vector<std::string> &args)
  {
    std::string new_args;
    for (const auto &arg : args)
    {
      if (!arg.empty() && arg.find_first_not_of(' ') != std::string::npos)
      {
        new_args = arg;
        break;
      }
    }
    if (new_args.empty() || new_args == "~")
    {
      const char *home = getenv("HOME");
      if (home)
      {
        chdir(home);
      }
    }
    else if (chdir(new_args.c_str()) != 0)
    {
      std::cerr << "cd: " << new_args << ": No such file or directory\n";
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
