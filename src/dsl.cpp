#include "dsl.hpp"

#include <iostream>

// Const To String
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

std::optional<Const> Consts::get(std::string name) {
  for (const auto &con : data) {
    if (name == con.name)
      return con;
  }

  return std::nullopt;
}

void Consts::set(Const ncon) {
  for (auto &con : data) {
    if (ncon.name == con.name) {
      con = ncon;
      return;
    }
  }

  data.push_back(ncon);
}

std::optional<std::string> Consts::get_string(std::string name) {
  auto c = get(name);
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

std::optional<std::vector<std::string>> Consts::get_vector(std::string name) {
  auto c = get(name);
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
