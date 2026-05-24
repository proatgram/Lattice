export module Lattice.Registry;

export import std;

export namespace Lattice {
    template <typename Val, typename Key = std::string>
    class Registry {
        struct Constructable {};
        public:
            Registry<Val, Key>(Constructable) {}

            inline static auto GetInstance() -> std::shared_ptr<Registry<Val, Key>> {
                static std::shared_ptr<Registry<Val, Key>> instance = std::make_shared<Registry<Val, Key>>(Constructable());

                return instance;
            }

            inline auto Register(const Key &key, const Val &value) -> std::expected<void, std::string> {
                if (m_registeryStore.contains(key))
                    return std::unexpected("Key already exists in Registry.");

                m_registeryStore[key] = value;

                return {};
            }

            inline auto Unregister(const Key &key) -> std::expected<void, std::string> {
                if (!m_registeryStore.contains(key))
                    return std::unexpected("Key doesn't exist in Registry,");

                m_registeryStore.erase(key);

                return {};
            }

            inline auto Query(const Key &key) const -> std::optional<Val> {
                typename std::map<Key, Val>::const_iterator it = m_registeryStore.find(key);

                return (it != std::cend(m_registeryStore) ? std::optional<Val>{it->second} : std::optional<Val>{std::nullopt});
            }
            
        private:
            std::map<Key, Val> m_registeryStore;
    };
}  // export namespace Lattice
