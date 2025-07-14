#include "builder.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

// Return file type.
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

// Obj path from ordinary,
// Like: main.c -> main.o
std::string to_obj_path(const std::string &src_path) {
  std::filesystem::path p(src_path);
  std::filesystem::path out = "build";
  out /= p.stem();
  out += ".o";
  return out.string();
}

// Obsolute path frim relative
std::string make_absolute(const std::string &rel) {
  return std::filesystem::absolute(rel).string();
}

bool log_enabled = true;
void log(std::string input) {
  if (log_enabled)
    std::cout << input << std::endl;
}

// Execute command
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

// C compiler command
std::string cc_cmd(Consts &data, const std::string &input,
                   const std::string &output) {
  const auto cc = data.get_string("CC");
  const auto cflags = data.get_vector("CFLAGS");
  const auto includes = data.get_vector("INCLUDE");
  const auto defines = data.get_vector("DEFINES");

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

// C++ compile command
std::string cxx_cmd(Consts &data, const std::string &input,
                    const std::string &output) {
  const auto &cxx = data.get_string("CXX");
  const auto &flags = data.get_vector("CXXFLAGS");
  const auto &includes = data.get_vector("INCLUDE");
  const auto &defines = data.get_vector("DEFINES");

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

// Assembler command
std::string as_cmd(Consts &data, const std::string &input,
                   const std::string &output) {
  const auto &as = data.get_string("AS");
  const auto &flags = data.get_vector("ASFLAGS");

  std::string cmd = *as + " -o " + output + " " + input;

  for (const auto &flag : *flags)
    cmd += " " + flag;

  return cmd;
}

// Linker command
std::string ld_cmd(Consts &data, const std::vector<std::string> &objs) {
  const auto &dir = data.get_string("BUILD_DIR");
  const auto &target = data.get_string("TARGET");

  const auto &ld = data.get_string("LD");
  const auto &flags = data.get_vector("LDFLAGS");
  const auto &libs = data.get_vector("LIBS");

  std::string cmd = *ld + " -o " + *dir + "/" + *target;

  for (const auto &obj : objs)
    cmd += " " + obj;

  for (const auto &flag : *flags)
    cmd += " " + flag;

  for (const auto &lib : *libs)
    cmd += " -l" + lib;

  return cmd;
}

// Generating compile_commands.json for `clangd` LSP
void write_compile_commands(
    std::string dir,
    const std::vector<std::tuple<std::string, std::string, std::string>>
        &entries) {
  std::ofstream out(dir + "compile_commands.json");
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

// Main build process
void Builder::build(Consts data) {
  log("SBS TOOL");
  log(*data.get_string("PROJECT") + " BUILDING");

  // Executing $PRE_BUILD
  if (auto opt = data.get_vector("PRE_BUILD"))
    for (auto &cmd : *opt)
      log(exec(cmd));

  // Creating build directory ($BUILD_DIR)
  exec("rm -rf " + *data.get_string("BUILD_DIR") + "/");
  if (auto opt = data.get_string("BUILD_DIR")) {
    exec("mkdir " + *opt);
  }
  std::vector<std::tuple<std::string, std::string, std::string>>
      compile_entries;
  std::string cwd = std::filesystem::current_path().string();

  // Compilation output, for linking.
  std::vector<std::string> objs;
  if (auto opt = data.get_vector("SOURCES")) {
    for (auto &input : *opt) {
      // File type, eg. `.c`, `.cpp` or `.as`
      SourceType type = get_source_type(std::string(input));
      // Obj file from ordinary file
      std::string output = to_obj_path(std::string(input));

      std::string cmd;
      switch (type) {
      case SourceType::C:
        cmd = cc_cmd(data, input, output);
        break;
      case SourceType::CPP:
        cmd = cxx_cmd(data, input, output);
        break;
      case SourceType::ASM:
        exec(as_cmd(data, input, output));
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

  // Linking
  exec(ld_cmd(data, objs));

  // Generating compile_commands
  const auto &dir = data.get_string("BUILD_DIR");
  write_compile_commands(*dir + "/", compile_entries);

  // Executing $POST_BUILD
  if (auto opt = data.get_vector("POST_BUILD"))
    for (const auto &cmd : *opt)
      log(exec(cmd));
}
