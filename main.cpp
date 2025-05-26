#include "njpt.h"
#include "njwin.h"

int main(int argc, char **argv) {
    auto win = nj::win::CreateWindow();
    auto keyctrl = nj::win::CreateKeyControl();
    // nj::Pty p;
    // p.ConnectShell();
    // p.ReadMaster(1, 50);
    // p.WriteMaster("ls\n");
    // p.ReadMaster(1, 100);
    return 0;
}
