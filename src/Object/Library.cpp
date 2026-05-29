module Lattice.Object.Library;

import Lattice.System.Process;
import Lattice.Tooling.Compiler;
import Lattice.Registry;
import Lattice.Object.Toolchain;

using namespace Lattice;

Library::Library(Constructable, const std::string &identifier) : Object(Object::Constructable(), identifier) {}

auto Library::GetProperties() const -> std::bitset<Object::TOTAL_PROPERTIES> {
    return std::bitset<TOTAL_PROPERTIES>()
        .set(std::to_underlying(Properties::Parsable))
        .set(std::to_underlying(Properties::Buildable));
}

auto Library::Build() -> void {

    // Obtains a toolchain if it can.
    std::optional<std::shared_ptr<Toolchain>> toolchain = Registry<std::shared_ptr<Toolchain>>::GetInstance()->Query(m_toolchainId.value_or("default"));
    if (!toolchain)
        throw std::runtime_error(std::format("Failed to query toolchain {} for library object \"{}\".", m_toolchainId.value_or("default"), m_identifier));

    // Obtains a compiler from said toolchain if it can.
    std::optional<std::shared_ptr<Tooling::ICompiler>> compiler = toolchain.value()->GetCompiler();
    if (!compiler)
        throw std::runtime_error(std::format("Failed to obtain a compiler for library object \"{}\".", m_identifier));

    // Creates a configuration using this object.
    Tooling::CompilerConfiguration compilerConfig = compiler.value()->CreateConfiguration(shared_from_this());

    System::Command compilerCommand = compiler.value()->CreateCommand(compilerConfig);

    std::expected<std::shared_ptr<System::Process>, std::string> compilerProcess = System::Process::Spawn(compilerCommand);

    if (!compilerProcess)
        throw std::runtime_error(std::format("Failed to build library \"{}\": Failed to spawn compiler process: {}", m_identifier, compilerProcess.error()));

    compilerProcess.value()->Wait();
}

auto Library::Parse(const std::string &parsableString) -> void {
    
}

auto LibraryFactory::Create(const std::string &identifier) -> std::shared_ptr<Object> {
    return std::make_shared<Library>(Library::Constructable(), identifier);
}
