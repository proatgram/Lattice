module Lattice.Logger;

using namespace Lattice::Logger;

Logger::Logger(Constructable) {}

auto Logger::GetInstance() -> std::shared_ptr<Logger> {
    static std::shared_ptr<Logger> instance = std::make_shared<Logger>(Constructable{});

    return instance;
}
