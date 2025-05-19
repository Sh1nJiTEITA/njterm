#pragma once
#ifndef NJ_TERM_HPP
#define NJ_TERM_HPP

#include <string>

namespace nj {

using FileDescriptor = int;

class Pty {

  public: // ASSIGNMENT
    Pty();

  public: // EXPORT
    std::string SlaveName();

  private:
    FileDescriptor masterDescriptor;
};

}; // namespace nj

#endif
