#include "njpt.h"

int main(int argc, char **argv) {
    nj::Pty p;
    p.ConnectShell();
    p.ReadMaster(1);
    return 0;
}
