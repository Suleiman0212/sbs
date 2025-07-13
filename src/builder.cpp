#include "builder.hpp"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

SourceType get_source_type(const std::string &path) {
  std::filesystem::path p(path);
  std::string ext = p.extension().string();

  if (ext == ".c")
    return SourceType::C;
  if (ext == ".cpp" || ext == ".cc")
    return SourceType::CPP;
  if (ext == ".s" || ext == ".asm")
    return SourceType::ASM;

  return SourceType::Unknown;
}

std::string to_obj_path(const std::string &src_path) {
  std::filesystem::path p(src_path);
  std::filesystem::path out = "build";
  out /= p.stem();
  out += ".o";
  return out.string();
}

std::string make_absolute(const std::string &rel) {
  return std::filesystem::absolute(rel).string();
}

Builder::Builder() {}

Builder::~Builder() {}

bool log_enabled = true;
void log(std::string input) {
  if (log_enabled)
    std::cout << input << std::endl;
}

std::string exec(const std::string &cmd) {
  log(cmd);
  FILE *pipe = popen(cmd.c_str(), "r");
  if (!pipe) {
    std::cerr << "ERROR: popen() failed" << std::endl;
  }

  std::string result;
  char buffer[256];

  while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
    result += buffer;
  }

  int return_code = pclose(pipe);
  if (return_code != 0) {
    throw std::runtime_error("Command failed with code: " +
                             std::to_string(return_code));
  }

  return result;
}

std::string cc_cmd(Interpreter &it, const std::string &input,
                   const std::string &output) {
  const auto cc = it.get_value("CC");
  const auto cflags = it.get_vector("CFLAGS");
  const auto includes = it.get_vector("INCLUDE");
  const auto defines = it.get_vector("DEFINES");

  std::string cmd =
      *cc + " -c " + make_absolute(input) + " -o " + make_absolute(output);

  for (const auto &def : *defines)
    cmd += " -D" + def;

  for (const auto &inc : *includes)
    cmd += " -I" + make_absolute(inc);

  for (const auto &flag : *cflags)
    cmd += " " + flag;

  return cmd;
}

std::string cxx_cmd(Interpreter &it, const std::string &input,
                    const std::string &output) {
  const auto &cxx = it.get_value("CXX");
  const auto &flags = it.get_vector("CXXFLAGS");
  const auto &includes = it.get_vector("INCLUDE");
  const auto &defines = it.get_vector("DEFINES");

  std::string cmd =
      *cxx + " -c " + make_absolute(input) + " -o " + make_absolute(output);

  for (const auto &def : *defines)
    cmd += " -D" + def;

  for (const auto &inc : *includes)
    cmd += " -I" + make_absolute(inc);

  for (const auto &flag : *flags)
    cmd += " " + flag;

  return cmd;
}

std::string as_cmd(Interpreter &it, const std::string &input,
                   const std::string &output) {
  const auto &as = it.get_value("AS");
  const auto &flags = it.get_vector("ASFLAGS");

  std::string cmd = *as + " -o " + output + " " + input;

  for (const auto &flag : *flags)
    cmd += " " + flag;

  return cmd;
}

std::string ld_cmd(Interpreter &it, const std::vector<std::string> &objs) {
  const auto &dir = it.get_value("BUILD_DIR");
  const auto &target = it.get_value("TARGET");

  const auto &ld = it.get_value("LD");
  const auto &flags = it.get_vector("LDFLAGS");
  const auto &libs = it.get_vector("LIBS");

  std::string cmd = *ld + " -o " + *dir + "/" + *target;

  for (const auto &obj : objs)
    cmd += " " + obj;

  for (const auto &flag : *flags)
    cmd += " " + flag;

  for (const auto &lib : *libs)
    cmd += " -l" + lib;

  return cmd;
}

void write_compile_commands(
    const std::vector<std::tuple<std::string, std::string, std::string>>
        &entries) {
  std::ofstream out("compile_commands.json");
  out << "[\n";
  for (size_t i = 0; i < entries.size(); ++i) {
    const auto &[file, dir, command] = entries[i];
    out << "  {\n";
    out << "    \"file\": \"" << file << "\",\n";
    out << "    \"directory\": \"" << dir << "\",\n";
    out << "    \"command\": \"" << command << "\"\n";
    out << "  }";
    if (i != entries.size() - 1)
      out << ",";
    out << "\n";
  }
  out << "]\n";
}

void Builder::build(Interpreter it) {
  log("SBS TOOL");
  log(*it.get_value("PROJECT") + " BUILDING");

  if (auto opt = it.get_vector("PRE_BUILD")) {
    for (auto &cmd : *opt) {
      log(exec(cmd));
    }
  }

  if (auto opt = it.get_value("BUILD_DIR"))
    exec("mkdir " + *opt);

  std::vector<std::tuple<std::string, std::string, std::string>>
      compile_entries;
  std::string cwd = std::filesystem::current_path().string();

  std::vector<std::string> objs;
  if (auto opt = it.get_vector("SOURCES")) {
    for (auto &input : *opt) {
      SourceType type = get_source_type(std::string(input));
      std::string output = to_obj_path(std::string(input));

      std::string cmd;
      switch (type) {
      case SourceType::C:
        cmd = cc_cmd(it, input, output);
        break;
      case SourceType::CPP:
        cmd = cxx_cmd(it, input, output);
        break;
      case SourceType::ASM:
        exec(as_cmd(it, input, output));
        break;
      case SourceType::Unknown:
        std::cerr << "ERROR!: Unknown source file type, " << input << std::endl;
        exit(1);
        break;
      }

      compile_entries.push_back({input, cwd, cmd});
      exec(cmd);

      objs.push_back(output);
    }
  }

  exec(ld_cmd(it, objs));

  write_compile_commands(compile_entries);

  if (auto opt = it.get_vector("POST_BUILD")) {
    for (const auto &cmd : *opt) {
      log(exec(cmd));
    }
  }
}
