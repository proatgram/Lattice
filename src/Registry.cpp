module Lattice.Registry;

using namespace Lattice;

Registry::Registry(Constructable) {}

auto Registry::GetInstance() -> std::shared_ptr<Registry> {
    static std::shared_ptr<Registry> instance = std::make_shared<Registry>(Constructable());

    return instance;
}
