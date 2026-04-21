#include <argparse/argparse.hpp>

import Lattice;

auto main(int argc, char *argv[]) -> int {
    argparse::ArgumentParser program("lattice");

    program.parse_args(argc, argv);

    Lattice::Lattice &lattice = Lattice::Lattice::GetInstance();

    return 0;
}
