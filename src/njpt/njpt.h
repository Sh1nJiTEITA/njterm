#pragma once
#ifndef NJ_TERM_HPP
#define NJ_TERM_HPP

#include <string>

namespace nj {

// GLOBAL CONSTS

inline constexpr size_t SingleMasterReadBufferSize = 256;

// SOME USINGS ------------------------
using FileDescriptor = int;
using ProcessID = pid_t;

auto DefaultShell() -> const char *;

/**
 * @class Pty
 * @brief Provides an interface to a pseudoterminal (pty).
 * Terminal emulators work by creating a pseudoterminal pair:
 * - The master side is used by the emulator (this process).
 * - The slave side is used by the shell or subprocess (child).
 * The emulator communicates with the shell through the master.
 */
class Pty {

  public: // ASSIGNMENT
    /**
     * @brief Constructor: initializes the master and slave pty file
     * descriptors. Opens a new pseudoterminal pair (master/slave).
     */
    Pty();
    ~Pty();

  public: // EXPORT
    /**
     * @brief Returns the system path to the slave pty device (e.g. /dev/pts/3).
     */
    auto SlaveName() -> std::string;

    /**
     * @brief Forks the current process and starts a shell in the child process.
     * The child process:
     * - Creates a new session.
     * - Sets the slave as its controlling terminal.
     * - Redirects stdin/stdout/stderr to the slave.
     * - Executes the default system shell.
     */
    auto ConnectShell() -> void;

    /**
     * @brief Reads data from shell while timeout is not null
     *
     * @param timeout_sec Amount of time to work until process
     * idle for this amount of time
     */
    auto ReadMaster(int timeout_sec = 1) -> std::string;

  private:
    FileDescriptor deviceDescriptor;
    FileDescriptor slaveDescriptor;
    ProcessID shellProcessID;
};

}; // namespace nj

#endif
