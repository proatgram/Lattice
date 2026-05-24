export module Lattice.Registry;

export import std;

export namespace Lattice {
    /**
     * @brief A simple registry.
     *
     * A registry is a singleton defined by it's key and value types,
     * obtained by calling `GetInstance()` on the registry with the templated values.
     *
     * A Registry element is unique and forceful. If you try to register something that
     * already exists, it will not overwrite it, and will give an error.
     *
     * @tparam Val Value type
     * @tparam Key Key type
     */
    template <typename Val, typename Key = std::string>
    class Registry {
        struct Constructable {};
        public:
            Registry<Val, Key>(Constructable) {}

            /**
             * @brief Gets the instance for the registry.
             *
             * @return A shared_ptr to the Registry instance.
             */
            inline static auto GetInstance() -> std::shared_ptr<Registry<Val, Key>> {
                static std::shared_ptr<Registry<Val, Key>> instance = std::make_shared<Registry<Val, Key>>(Constructable());

                return instance;
            }

            /**
             * @brief Registers a value with a specific key.
             *
             * This method will fail if the given key already exists and
             * return an unexpected value of an error string.
             *
             * @param[in] key The key for the addition.
             * @param[in] value The value to add.
             *
             * @return A `std::expected` containing an expected value of `void`, or unexpected value of `std::string` telling what happened.
             */
            inline auto Register(const Key &key, const Val &value) -> std::expected<void, std::string> {
                if (m_registeryStore.contains(key))
                    return std::unexpected("Key already exists in Registry.");

                m_registeryStore[key] = value;

                return {};
            }

            /**
             * @brief Unregisters a specific key.
             *
             * This method will fail if the given key doesn't exist
             * in the registry and will return an unexpected value
             * giving an error string.
             *
             * @param[in] key The key to remove.
             *
             * @return A `std::expected` containing an expected value of `void`, or unexpected value of `std::string` telling what happened.
             */
            inline auto Unregister(const Key &key) -> std::expected<void, std::string> {
                if (!m_registeryStore.contains(key))
                    return std::unexpected("Key doesn't exist in Registry,");

                m_registeryStore.erase(key);

                return {};
            }

            /**
             * @brief Queries for a specific key.
             *
             * @param[in] key The key to query for.
             *
             * @return A `std::optional` containing the value for the key, or nothing if it wasn't found.
             */
            inline auto Query(const Key &key) const -> std::optional<Val> {
                typename std::map<Key, Val>::const_iterator it = m_registeryStore.find(key);

                return (it != std::cend(m_registeryStore) ? std::optional<Val>{it->second} : std::optional<Val>{std::nullopt});
            }

            /**
             * @brief Checks if a key exists.
             *
             * @param[in] key The key to check for.
             *
             * @return `true` if found, `false` if not.
             */
            inline auto Contains(const Key &key) const -> bool {
                return m_registeryStore.contains(key);
            }
            
        private:
            std::map<Key, Val> m_registeryStore;
    };
}  // export namespace Lattice
