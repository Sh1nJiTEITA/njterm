#include "njpt.h"

#include <fcntl.h>
#include <stdlib.h>

namespace nj {

Pty::Pty() {
    // Open unused master device
    masterDescriptor = posix_openpt(O_RDWR);
    if (masterDescriptor == -1) {
    }

    if (grantpt(masterDescriptor)) {
        // fmt::println(stderr, "Cant grant access to the slave ps");
    }

    if (unlockpt(masterDescriptor)) {
        // fmt::println(stderr, "Cant unlock pt device");
    }
}

std::string Pty::SlaveName() { return ptsname(masterDescriptor); }

}; // namespace nj
