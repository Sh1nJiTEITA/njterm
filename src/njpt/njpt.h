#pragma once
#ifndef NJ_TERM_HPP
#define NJ_TERM_HPP

#include <string>

namespace nj {

using FileDescriptor = int;
using ProcessID = pid_t;

class Pty {

  public: // ASSIGNMENT
    Pty();
    ~Pty();

  public: // EXPORT
    auto SlaveName() -> std::string;
    auto ConnectShell() -> void;

  private:
    FileDescriptor deviceDescriptor;
    FileDescriptor slaveDescriptor;
    ProcessID shellProcessID;
};

}; // namespace nj

#endif
