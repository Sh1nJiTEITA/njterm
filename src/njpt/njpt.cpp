#include "njpt.h"
#include "njlog.h"

#include <bits/types/struct_timeval.h>
#include <cstdlib>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

namespace nj {

auto DefaultShell() -> const char * {
    const char *shell = getenv("SHELL");
    if (!shell)
        shell = "/bin/sh";
    return shell;
}

Pty::Pty() {
    // clang-format off
    log::Info("Creating pt");

    deviceDescriptor = posix_openpt(O_RDWR);
    log::CheckUnixCall(deviceDescriptor, "Cant create device descriptor");

    log::CheckUnixCall(grantpt(deviceDescriptor), "Cant grant access to the slave ps");
    log::CheckUnixCall(unlockpt(deviceDescriptor), "Cant unlock pt device");

    slaveDescriptor = open(SlaveName().c_str(), O_RDWR);
    log::CheckUnixCall(slaveDescriptor, "Cant open slave file: {}");

    // clang-format on
}

Pty::~Pty() {
    log::Info("Descructing pt");

    if (deviceDescriptor != -1)
        close(deviceDescriptor);
    if (slaveDescriptor != -1)
        close(slaveDescriptor);
}

auto Pty::SlaveName() -> std::string { return ptsname(deviceDescriptor); }
auto Pty::ConnectShell() -> void {
    // clang-format off
    
    // Create new child process
    shellProcessID = fork();
    log::CheckUnixCall(shellProcessID, "Cant fork new process for shell");
    // New child process spawns with same content as parent. Here == 0 check
    // is needed to make ONLY child process run shell
    if (shellProcessID == 0) { 
        log::CheckUnixCall(setsid(), "Cant create session for child process");
        log::CheckUnixCall(ioctl(slaveDescriptor, TIOCSCTTY, 0), "ioctl TIOCSTTY failed");
        
        // Make child process (current) ref all std<...> to slave descriptor
        log::CheckUnixCall(dup2(slaveDescriptor, STDIN_FILENO), "Cant dup to stdin");
        log::CheckUnixCall(dup2(slaveDescriptor, STDOUT_FILENO), "Cant dup to stdout");
        log::CheckUnixCall(dup2(slaveDescriptor, STDERR_FILENO), "Cant dup tp stderr");

        const char* shell_path = DefaultShell();
        log::CheckUnixCall(execlp(shell_path, shell_path, nullptr), 
                       "Cant exec default shell={}"/* , std::string(shell_path) */);
    }
    // clang-format on
}

auto Pty::ReadMaster(int timeout_sec, int timeout_usec) -> std::string {
    std::string content;
    content.reserve(SingleMasterReadBufferSize);

    char buffer[SingleMasterReadBufferSize];

    // fd_set a structure for kipping track of multiple file
    // descriptors. As set for watching changes of selected
    // descriptors
    fd_set readfds;
    // Timeout to read after breaking read process
    timeval timeout{.tv_sec = timeout_sec, .tv_usec = 0};

    // Full clear descriptor set
    FD_ZERO(&readfds);
    // Adding descriptors to track
    // Which are connected to master descriptor
    FD_SET(deviceDescriptor, &readfds);

    // clang-format off
    while (true) {
        const int ret = select(deviceDescriptor + 1, &readfds, nullptr, nullptr, &timeout);
        const char* ret_msg = "Cant select (wait) untile master "
                              "descriptor is ready for reading";
        log::CheckUnixCall(ret, ret_msg);
        if (ret == 0) { 
            break;
        }
        ssize_t num = read(deviceDescriptor, buffer, SingleMasterReadBufferSize - 1);
        
        if (num) { 
            buffer[num] = '\0';
            content += buffer;
            timeout.tv_sec = timeout_sec;
            timeout.tv_usec = 0;
            FD_ZERO(&readfds);
            FD_SET(deviceDescriptor, &readfds);
        } else { 
            break;
        }
    }
    // clang-format on
    log::Debug("{}", content);
    return content;
}

auto Pty::WriteMaster(const std::string &msg) -> void {
    ssize_t totalWritten = 0;
    ssize_t dataLen = static_cast<ssize_t>(msg.size());

    while (totalWritten < dataLen) {
        ssize_t written = write(deviceDescriptor, msg.data() + totalWritten,
                                dataLen - totalWritten);
        log::CheckUnixCall(written, "Cant write to master descriptor");
        totalWritten += written;
    }
}

}; // namespace nj
