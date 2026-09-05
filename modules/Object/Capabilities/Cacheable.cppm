export module Lattice.Object.Capabilities.Cacheable;

export import std;

import Lattice.Object.Capabilities.ICapability;

export import Lattice.Cache.Serialization;

export namespace Lattice::Object::Capabilities {
    class Cacheable : public ICapability {
        public:
            virtual ~Cacheable();

            auto GetCache() const -> std::optional<std::shared_ptr<Cache::Serialization::Cache>>;

        protected:
            virtual auto SerializeCache() const -> std::shared_ptr<Cache::Serialization::Cache> = 0;
            virtual auto DeserializeCache(const std::shared_ptr<Cache::Serialization::Cache> &cacheInstance) -> void = 0;

        private:
            std::optional<std::shared_ptr<Cache::Serialization::Cache>> m_cacheInstance; 
    };
}  // export namespace Lattice::Object::Capabilities
