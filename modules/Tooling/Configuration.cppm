export module Lattice.Tooling.Configuration;

export import std;

export namespace Lattice::Tooling {
    class Configuration {
        protected:
            struct Constructable {};
        public:
            Configuration(Constructable);

            auto GetConfigStore() const -> std::map<std::string, std::any>;

            template <typename T>
            inline auto Get(const std::string &key) -> std::optional<T> {
                std::type_index type = typeid(T);
                for (const auto &[storeKey, val] : m_configStore) {
                    if (storeKey == key && val.type() == type)
                        return std::any_cast<T>(val);
                }

                return {};
            }

        protected:
            std::map<std::string, std::any> m_configStore;
    };
}  // export namespace Lattice::Tooling
