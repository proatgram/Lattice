#include <argparse/argparse.hpp>

import Lattice;

auto main(int argc, char *argv[]) -> int {
    argparse::ArgumentParser program("lattice");

    program.parse_args(argc, argv);

    std::shared_ptr<Lattice::Lattice> lattice = Lattice::Lattice::GetInstance();

    lattice->LoadConfig("examples/project.yaml");

    return 0;
}
