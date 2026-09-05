module Lattice.Object.Capabilities.Cacheable;

import Lattice.Cache.Manager;

using namespace Lattice::Object::Capabilities;

Cacheable::~Cacheable() {
    if (m_cacheInstance.has_value() && m_cacheInstance.value() && m_cacheInstance.value()->IsDirty()) {
        Cache::CacheManager::GetInstance()->WriteCache(m_cacheInstance.value());
    }
}

auto Cacheable::GetCache() const -> std::optional<std::shared_ptr<Cache::Serialization::Cache>> {
    return m_cacheInstance;
}
