#include "njpt.h"
#include "njlog.h"

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

namespace nj {

Pty::Pty() {
    // clang-format off
    log::Info("Creating pt");

    deviceDescriptor = posix_openpt(O_RDWR);
    log::CheckCall(deviceDescriptor, "Cant create device descriptor");

    log::CheckCall(grantpt(deviceDescriptor), "Cant grant access to the slave ps");
    log::CheckCall(unlockpt(deviceDescriptor), "Cant unlock pt device");

    slaveDescriptor = open(SlaveName().c_str(), O_RDWR);
    log::CheckCall(slaveDescriptor, "Cant open slave file: {}");

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
    shellProcessID = fork();
    log::CheckCall(shellProcessID, "Cant fork new process for shell");
}

}; // namespace nj
