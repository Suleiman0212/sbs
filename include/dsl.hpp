#pragma once

#include <optional>
#include <string>
#include <variant>
#include <vector>

class Consts;

struct Const {
  std::string name;
  std::variant<std::string, std::vector<std::string>> value;

  std::string tos();
};

class Consts {
public:
  Consts(std::vector<Const> init_data) : data(std::move(init_data)) {}
  std::optional<Const> get(std::string name);
  void set(Const value);

  std::optional<std::string> get_string(std::string name);
  std::optional<std::vector<std::string>> get_vector(std::string name);

private:
  std::vector<Const> data;
};

// DSL default data
const Consts DSL_DEFAULT_CONSTS({
    Const{"PROJECT", "unnamed"},                    //
    Const{"BUILD_DIR", "build"},                    //
    Const{"TARGET", "unnamed"},                     //
    Const{"CC", "cc"},                              //
    Const{"CXX", "c++"},                            //
    Const{"AS", "as"},                              //
    Const{"LD", "ld"},                              //
    Const{"CFLAGS", std::vector<std::string>()},    //
    Const{"CXXFLAGS", std::vector<std::string>()},  //
    Const{"ASFLAGS", std::vector<std::string>()},   //
    Const{"LDFLAGS", std::vector<std::string>()},   //
    Const{"SOURCES", std::vector<std::string>()},   //
    Const{"INCLUDE", std::vector<std::string>()},   //
    Const{"LIBS", std::vector<std::string>()},      //
    Const{"DEFINES", std::vector<std::string>()},   //
    Const{"PRE_BUILD", std::vector<std::string>()}, //
    Const{"POST_BUILD", std::vector<std::string>()} //
});
