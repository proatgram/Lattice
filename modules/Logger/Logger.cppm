export module Lattice.Logger;

export import std;

export namespace Lattice::Logger {
    class Logger {
        struct Constructable{};
        public:
            Logger(Constructable);
            static auto GetInstance() -> std::shared_ptr<Logger>;

        private:
    };
}
