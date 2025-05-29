#pragma once
#ifndef NJ_CON_H
#define NJ_CON_H

#include "njluastate.h"
#include <string>

namespace nj::con {

class Config {
  public:
    Config();
    Config(const std::string &config_data);
    Config(std::string &&config_data) noexcept;

  private:
    lua::State state;
};

} // namespace nj::con

#endif
