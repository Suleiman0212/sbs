#include "interpreter.hpp"
#include <cstdlib>
#include <iostream>
#include <optional>

std::string Const::tos() {
  std::string buffer;
  buffer.append("\tName: ");
  buffer.append(name);
  buffer.append("\n\tValue: ");

  std::visit(
      [&buffer](const auto &val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::string>) {
          buffer.append(val);
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
          for (const auto &v : val) {
            buffer.append(v);
            buffer.append(" ");
          }
        }
      },
      value);

  return buffer;
}

Consts CONSTS({
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

std::optional<Const> get_const(std::string name) { return CONSTS.get(name); }

std::optional<Const> Consts::get(std::string name) {
  for (const auto &con : CONSTS.data) {
    if (name == con.name)
      return con;
  }

  return std::nullopt;
}

void Consts::set(Const ncon) {
  for (auto &con : CONSTS.data) {
    if (ncon.name == con.name) {
      con = ncon;
      return;
    }
  }

  CONSTS.data.push_back(ncon);
}

Interpreter::Interpreter() {}

Interpreter::~Interpreter() {}

void set_const(std::vector<Token>::iterator &tok,
               std::vector<Token>::iterator end) {
  Token prev = *tok;
  ++tok;

  if (tok == end) {
    std::cerr << "ERROR!\n" << std::endl;
    std::cerr << "Expected `Value` or `Vector` after `Const`" << std::endl;
    exit(1);
  }

  if (tok->type != TokenType::Value && tok->type != TokenType::Vector) {
  }

  switch (tok->type) {
  case TokenType::Value:
  case TokenType::Vector:
    CONSTS.set(Const{std::get<std::string>(prev.data), tok->data});
    break;
  default:
    std::cerr << "ERROR!\n" << std::endl;
    std::cerr << "Unexpected token after `Const` {\n"
              << tok->tos() << "\n}\n"
              << std::endl;
    exit(1);
  }

  ++tok;
}

std::optional<Const> Interpreter::get_const(std::string name) {
  return CONSTS.get(name);
}

// std::optional<std::string> Interpreter::get_value(std::string name) {
//   return std::get<std::string>(get_const(name)->value);
// }

std::optional<std::string> Interpreter::get_value(std::string name) {
  auto c = get_const(name);
  if (!c) {
    std::cerr << "DSL ERROR: $" << name << " is not defined" << std::endl;
    return std::nullopt;
  }

  if (!std::holds_alternative<std::string>(c->value)) {
    std::cerr << "DSL ERROR: $" << name << " is not a string" << std::endl;
    return std::nullopt;
  }

  return std::get<std::string>(c->value);
}

// std::optional<std::vector<std::string>>
// Interpreter::get_vector(std::string name) {
//   return std::get<std::vector<std::string>>(get_const(name)->value);
// }

std::optional<std::vector<std::string>>
Interpreter::get_vector(std::string name) {
  auto c = get_const(name);
  if (!c) {
    std::cerr << "DSL ERROR: $" << name << " is not defined" << std::endl;
    return std::nullopt;
  }

  if (!std::holds_alternative<std::vector<std::string>>(c->value)) {
    std::cerr << "DSL ERROR: $" << name << " is not a list/vector" << std::endl;
    return std::nullopt;
  }

  return std::get<std::vector<std::string>>(c->value);
}

Consts Interpreter::interpret(std::vector<Token> input) {
  Consts buff = CONSTS;
  auto tok = input.begin();
  auto end = input.end();

  while (tok != end) {
    switch (tok->type) {
    case TokenType::Unknown:
      std::cerr << "ERROR:\n" << std::endl;
      std::cerr << "Unknown token {\n" << tok->tos() << "\n}\n" << std::endl;
      exit(1);
    case TokenType::Const:
      set_const(tok, end);
      break;
    default:
      std::cerr << "ERROR:\n" << std::endl;
      std::cerr << "Expected `Const` found {\n"
                << tok->tos() << "\n}\n"
                << std::endl;
      exit(1);
    }
  }

  return buff;
}
