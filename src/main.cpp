#include <argparse/argparse.hpp>

import std;
import Lattice.Logger.ILogger;
import Lattice.Logger.TextLogger;

import Lattice;

auto main(int argc, char *argv[]) -> int {
    // build subcommand arguments
    argparse::ArgumentParser buildParser("build");
    buildParser.add_description("Build an object, project, or set of objects.");
    buildParser.add_argument("objects")
                .help("Objects to be built.")
                .remaining();
    buildParser.add_argument("-j", "--jobs")
                .help("Allow N jobs at once. No options for no concurrency.")
                .scan<'u', std::size_t>();

    // Main program arguments
    argparse::ArgumentParser program("lattice", "0.0.1", argparse::default_arguments::help, true);
    if (!std::filesystem::exists(std::filesystem::current_path()  / "project.yaml")) {
        program.add_argument("-p", "--project")
               .required();
    } else {
        program.add_argument("-p", "--project")
            .default_value(std::filesystem::current_path() / "project.yaml");
    }
    program.add_subparser(buildParser);

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception &err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program << std::endl;
        return -1;
    }


    try {
        std::shared_ptr<Lattice::Lattice> lattice = Lattice::Lattice::GetInstance();

        lattice->LoadConfig(program.get<std::string>("--project"));

        if (program.is_subcommand_used(buildParser)) {
                lattice->StartBuild(
                    (buildParser.is_used("objects") ? std::optional<std::list<std::string>>{buildParser.get<std::list<std::string>>("objects")} : std::nullopt),
                    (buildParser.is_used("--jobs") ? std::optional<std::size_t>(buildParser.get<std::size_t>("--jobs")) : 1));
        }
    } catch (const std::runtime_error &err) {
        std::shared_ptr<Lattice::Logger::TextLogger> textLogger = std::dynamic_pointer_cast<Lattice::Logger::TextLogger>(Lattice::Logger::ILogger::GetDefault());
        if (textLogger) {
            textLogger->Error(std::format("ERROR: {}", err.what()));
        } else {
            std::cerr << "ERROR: " << err.what() << std::endl;
        }

        return -1;
    }

    return 0;
}
