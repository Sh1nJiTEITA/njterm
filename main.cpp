#include "njpt.h"

int main(int argc, char **argv) {
    nj::Pty p;
    p.ConnectShell();
    p.ReadMaster(0, 50);
    p.WriteMaster("ls\n");
    p.ReadMaster(0, 100);
    return 0;
}
