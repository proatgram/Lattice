export module Lattice.Tooling.Configuration;

export import std;

export namespace Lattice::Tooling {
    /**
     * @brief A class that can be used to store configurations.
     *
     * Other classes can inherit from this and add their own helper
     * methods that abstract away the raw map, making something
     * more straight forward to configure.
     */
    class Configuration {
        protected:
            struct Constructable {};
        public:
            Configuration(Constructable);

            /**
             * @brief Gets the underlying configuration storage map.
             *
             * It is discouraged to use this directly. Instead, inherit from
             * the class and add helper methods.
             *
             * @return A `std::map` of std::string keys and std::any values.
             */
            auto GetConfigStore() const -> const std::map<std::string, std::any>&;

            /**
             * @brief Gets a specific value for a key.
             *
             * It is discouraged to use this directly. Instead, inherit from
             * the class and add helper methods.
             *
             * Since the configuration store can have any value as it's value
             * through the use of `std::any`, you have to know what value is
             * stored, or it will return nothing.
             *
             * @param[in] key The key to get.
             *
             * @return A `std::optional` either containing the value if found
             * and the type is correct, or nothing if not found or there is a
             * type mismatch.
             */
            template <typename T>
            inline auto Get(const std::string &key) const -> std::optional<T> {
                std::type_index type = typeid(T);

                if (auto it = m_configStore.find(key); it != std::end(m_configStore)) {
                    try {
                        return std::any_cast<T>(it->second);
                    } catch ([[maybe_unused]] const std::bad_any_cast& ex) {
                        return {};
                    }
                }

                return {};
            }

            /**
             * @brief Sets a specific value for a key.
             *
             * It is discouraged to use this directly. Instead, inherit from
             * the class and add helper methods.
             *
             * @tparam `T` The type of the value.
             *
             * @param[in] key The key to set.
             * @param[in] value The value to set for the key.
             */
            template <typename T>
            inline auto Set(const std::string &key, const T &value) -> void {
                m_configStore[key] = value;
            }

        protected:
            std::map<std::string, std::any> m_configStore;
    };
}  // export namespace Lattice::Tooling
