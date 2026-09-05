export module Lattice.Cache.Serialization;

export import std;

export namespace Lattice::Cache::Serialization {
    /**
     * @brief Abstraction over a Cache file.
     *
     * A Cache file for Lattice is similar in format
     * to INI/TOML, but much more simpler. Ultimately
     * you do not need to know the format in order to
     * write and read from it.
     * 
     * Each cache context can have sections, which are
     * either normal sections or array based sections
     * that are iteratable.
     */
    class Cache {
        public:
            virtual ~Cache() = default;

            /**
             * @brief Represents any kind of section
             */
            class ISection {
                public:
                    virtual ~ISection() = default;

                    /**
                     * @brief Returns if this section is an array.
                     *
                     * @return true if it's an array, false otherwise.
                     */
                    virtual auto IsArray() const -> bool = 0;

                    /**
                     * @brief Gets the section name.
                     *
                     * @return The section name.
                     */
                    auto GetName() const -> std::string;
                    /**
                     * @brief Sets the section name.
                     * 
                     * @param[in] sectionName The section name to set.
                     */
                    auto SetName(const std::string &sectionName) -> void;

                private:
                    std::string m_sectionName;
            };
            class Section final : public ISection, public std::enable_shared_from_this<Section> {
                public:
                    /**
                     * @brief Const array operator.
                     */
                    auto operator[](const std::string &key) const -> std::string;
                    /**
                     * @brief Non-const array operator.
                     */
                    auto operator[](const std::string &key) -> std::string&;
                    auto IsArray() const -> bool final;
                    
                    /**
                     * @brief Gets the value at a specific key.
                     *
                     * @param[in] key The key.
                     *
                     * @return The value at key.
                     */
                    auto Get(const std::string &key) const -> std::string;
                    /**
                     * @brief Gets all of the key-value pairs.
                     *
                     * @return A map consisting of the key-value pairs.
                     */
                    auto GetAll() const -> std::map<std::string, std::string>;
                    /**
                     * @brief Sets a key to a specific value.
                     *
                     * @param[in] key The key to set/cahnge.
                     * @param[in] value The value to set/change.
                     *
                     * @return Pointer to the current section for chaining.
                     */
                    auto Set(const std::string &key, const std::string &value) -> std::shared_ptr<Section>;

                private:
                    std::map<std::string, std::string> m_contents;
                    std::string m_sectionName;
            };
            class SectionArray final : public ISection, public std::enable_shared_from_this<SectionArray> {
                public:
                    auto IsArray() const -> bool final;
                    /**
                     * @brief Const array operator.
                     */
                    auto operator[](int index) const -> const std::shared_ptr<Section>;
                    /**
                     * @brief Non-const array operator.
                     */
                    auto operator[](int index) -> std::shared_ptr<Section>;

                    /**
                     * @brief Gets the section at the specified index.
                     */
                    auto Get(int index) const -> const std::shared_ptr<Section>;
                    /**
                     * @brief Gets all of the sections in the section array.
                     *
                     * @return A vector containing all the sections for this section array.
                     */
                    auto GetAll() const -> const std::vector<std::shared_ptr<Section>>&;
                    /**
                     * @brief Gets all of the sections in the section array.
                     *
                     * @return A vector containing all the sections for this section array.
                     */
                    auto GetAll() -> std::vector<std::shared_ptr<Section>>&;
                    /**
                     * @brief Sets the section at the given index.
                     *
                     * @param[in] index The index of the section.
                     * @param[in] section The section to put at the index.
                     *
                     * @return Pointer to the current section array for chaining.
                     */
                    auto Set(int index, const std::shared_ptr<Section> &section) -> std::shared_ptr<SectionArray>;
                    /**
                     * @brief Adds a section to the section array.
                     *
                     * @param[in] section The section to add.
                     *
                     * @return Pointer to the current section array for chaining.
                     */
                    auto Add(const std::shared_ptr<Section> &section) -> std::shared_ptr<SectionArray>;

                    /**
                     * @brief Sets the size for the section array.
                     *
                     * Simply resizes the internal storage vector.
                     *
                     * @param[in] size The size for the section array.
                     *
                     * @return Pointer to the current section array for chaining.
                     */
                    auto SetSize(std::size_t size) -> std::shared_ptr<SectionArray>;
                    /**
                     * @brief Gets the current size for the section array.
                     *
                     * @return The size of the section array.
                     */
                    auto GetSize() const -> std::size_t;

                private:
                    std::vector<std::shared_ptr<Section>> m_arraySections;
            };

            /**
             * @brief Const array operator.
             */
            auto operator[](const std::string &sectionName) const -> const std::shared_ptr<ISection>;
            /**
             * Non-const array operator.
             */
            auto operator[](const std::string &sectionName) -> std::shared_ptr<ISection>;

            /**
             * @brief Checks if this cache contains a section by a given name.
             *
             * @param[in] sectionName The section to check exists.
             *
             * @return true if a section by that name exists, false otherwise.
             */
            auto ContainsSection(const std::string &sectionName) const -> bool;
            /**
             * @brief Gets a section by a given name.
             *
             * If the section doesn't exist, the behaviour is the same
             * as std::map::at with an invalid key.
             *
             * @param[in] sectionName The section to get.
             *
             * @return The section by that name.
             */
            auto GetSection(const std::string &sectionName) const -> const std::shared_ptr<ISection>;
            /**
             * @brief Gets a section by a given name.
             *
             * If the section doesn't exist, it will be created and returned.
             *
             * @param[in] sectionName The section to get.
             *
             * @return The section by that name.
             */
            auto GetSection(const std::string &sectionName) -> std::shared_ptr<ISection>;
            /**
             * @brief Adds a section to the cache.
             *
             * @param[in] section The section to add to the cache.
             *
             * @return A reference to the object for chaining.
             */
            auto AddSection(const std::shared_ptr<ISection> &section) -> Cache&;
            /**
             * @brief Gets all of the sections.
             *
             * @return A type satisfying std::ranges::view
             */
            inline auto GetAllSections() const -> std::ranges::view auto {
                return m_cacheSections | std::views::values;
            }
            
            /**
             * @brief Gets the name of the cache.
             *
             * @return The cache name.
             */
            auto GetCacheName() const -> std::string;
            /**
             * @brief Sets the cache name.
             *
             * @param[in] cacheName The name for the cache to set.
             *
             * @return A reference to the object for chaining.
             */
            auto SetCacheName(const std::string &cacheName) -> Cache&;

            /**
             * @brief Gets the file location for the cache.
             *
             * @return A filesystem::path for the cache file.
             */
            auto GetCacheLocation() const -> std::filesystem::path;

            /**
             * @brief Marks the cache as dirty.
             *
             * A dirty cache signifies that the cache on disk is invalid
             * and needs to be regenerated and written.
             *
             * @return A reference to the object for chaining.
             */
            auto MarkDirty() -> Cache&;
            /**
             * @brief Checks if the cache is dirty.
             *
             * @return true if the cache is dirty, false otherwise.
             */
            auto IsDirty() const -> bool;

        private:
            std::map<std::string, std::shared_ptr<ISection>> m_cacheSections;
            std::filesystem::path m_cachePath;
            std::string m_cacheName;
            bool m_isDirty{};
    };
}  // Lattice::Cache::Serialization
