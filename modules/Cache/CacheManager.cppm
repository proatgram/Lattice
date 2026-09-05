export module Lattice.Cache.Manager;

export import std;

export import Lattice.Cache.Serialization;

export namespace Lattice::Cache {
    /**
     * @brief A singleton class that manages cache operations.
     */
    class CacheManager {
        struct Constructable{};
        public:
            CacheManager(Constructable);

            /**
             * @brief Singleton getter.
             *
             * @return A singleton to the manager.
             */
            static auto GetInstance() -> std::shared_ptr<CacheManager>;

            /**
             * @brief Reads the cache given a specific cache identifier.
             *
             * @param[in] cacheIdentifier The identifier for the cache to read.
             *
             * @return The cache if it exists, or std::nullopt{} if it doesn't exist.
             */
            auto ReadCache(const std::string &cacheIdentifier) -> std::optional<std::shared_ptr<Serialization::Cache>>;
            /**
             * @brief Writes a cache context to disk.
             *
             * @param[in] cache The cache context to write.
             */
            auto WriteCache(const std::shared_ptr<Serialization::Cache> &cache) -> void;
        private:
            static auto ParseRawSection(const std::string &rawSectionHeader, std::vector<std::string> rawSectionLines) -> std::shared_ptr<Serialization::Cache::ISection>;
    };
}
