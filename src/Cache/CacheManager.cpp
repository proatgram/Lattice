module;
#include <filesystem>
module Lattice.Cache.Manager;

import std;

using namespace Lattice::Cache;

CacheManager::CacheManager(Constructable) {}

auto CacheManager::GetInstance() -> std::shared_ptr<CacheManager> {
    static std::shared_ptr<CacheManager> instance = std::make_shared<CacheManager>(Constructable());

    return instance;
}

auto CacheManager::ReadCache(const std::string &cacheIdentifier) -> std::optional<std::shared_ptr<Serialization::Cache>> {
    if (!std::filesystem::exists(std::filesystem::current_path() / ".cache/" / (cacheIdentifier + ".cache")))
        throw std::runtime_error(std::format("Failed to read cache for {}: File does not exist.", cacheIdentifier));

    std::fstream file(std::filesystem::current_path() / ".cache/" / (cacheIdentifier + ".cache"), std::fstream::in);

    std::shared_ptr<Serialization::Cache> cache = std::make_shared<Serialization::Cache>();

    std::string line;

    std::string currentSectionHeader;
    std::vector<std::string> currentSectionLines;
    while (std::getline(file, line)) {
        if (line.starts_with('#') || line.empty())
            continue;
        if (line.starts_with('[') && line.ends_with(']')) {
            if (!currentSectionHeader.empty()) {
                try {
                    cache->AddSection(ParseRawSection(currentSectionHeader, currentSectionLines));
                } catch (const std::exception &err) {
                    throw std::runtime_error(std::format("Failed to read CacheID {}: Failed to parse cache file ({}).", cacheIdentifier, (std::filesystem::current_path() / ".cache/" / (cacheIdentifier + ".cache")).string()));
                }
                currentSectionLines.clear();
            }

            currentSectionHeader = line;
            continue;
        }

        currentSectionLines.push_back(line);
    }


    if (!currentSectionHeader.empty()) {
        try {
            cache->AddSection(ParseRawSection(currentSectionHeader, currentSectionLines));
        } catch (const std::exception &err) {
            throw std::runtime_error(std::format("Failed to read CacheID {}: Failed to parse cache file ({}).", cacheIdentifier, (std::filesystem::current_path() / ".cache/" / (cacheIdentifier + ".cache")).string()));
        }
    }

    return cache;
}

auto CacheManager::WriteCache(const std::shared_ptr<Serialization::Cache> &cache) -> void {
    if (!std::filesystem::exists(std::filesystem::current_path() / ".cache/"))
        std::filesystem::create_directory(std::filesystem::current_path() / ".cache/");

    std::fstream file(std::filesystem::current_path() / ".cache/" / (cache->GetCacheName() + ".cache"), std::fstream::out | std::fstream::trunc);
    for (const std::shared_ptr<Serialization::Cache::ISection> &isection : cache->GetAllSections()) {
        if (isection->IsArray()) {
            const std::shared_ptr<Serialization::Cache::SectionArray> sectionArray = std::dynamic_pointer_cast<Serialization::Cache::SectionArray>(isection);
            // Prep section name
            file << "[" << isection->GetName() << ":" << sectionArray->GetSize() << "]" << std::endl;
            for (int i = 0; i < sectionArray->GetSize(); i++) {
                for (const auto &[key, value] : sectionArray.get()->Get(i)->GetAll()) {
                    file << i << ":" << key << " = " << value << std::endl;
                }

                file << std::endl << std::flush;
            }
        } else {
            const std::shared_ptr<Serialization::Cache::Section> section = std::dynamic_pointer_cast<Serialization::Cache::Section>(isection);
            // Prep section name
            file << "[" << isection->GetName() << "]" << std::endl;
            for (const auto &[key, value] : section->GetAll()) {
                file << key << " = " << value << std::endl;
            }
        }
    }

    file << std::flush;
    file.close();
}

auto CacheManager::ParseRawSection(const std::string &rawSectionHeader, std::vector<std::string> rawSectionLines) -> std::shared_ptr<Serialization::Cache::ISection> {
    std::shared_ptr<Serialization::Cache::ISection> isection;
    std::string sectionName;
    std::size_t arraySize{0};
    if (rawSectionHeader.contains(':')) {
        arraySize = std::stoi(rawSectionHeader.substr(rawSectionHeader.find(':') + 1, rawSectionHeader.find(']') - rawSectionHeader.find(":") - 1));
        sectionName = rawSectionHeader.substr(rawSectionHeader.find('[') + 1, rawSectionHeader.find(':') - rawSectionHeader.find('[') - 1);
        isection = std::make_shared<Serialization::Cache::SectionArray>();
        std::static_pointer_cast<Serialization::Cache::SectionArray>(isection)->SetSize(arraySize);
    } else {
        sectionName = rawSectionHeader.substr(rawSectionHeader.find('[') + 1, rawSectionHeader.find(']') - rawSectionHeader.find('[') - 1);
        isection = std::make_shared<Serialization::Cache::Section>();
    }

    isection->SetName(sectionName);

    if (arraySize > 0) {
        std::shared_ptr<Serialization::Cache::SectionArray> sectionArray = std::dynamic_pointer_cast<Serialization::Cache::SectionArray>(isection);
        for (const std::string &rawSectionLine : rawSectionLines) {
            std::size_t index = std::stoi(rawSectionLine.substr(rawSectionLine.find_first_not_of(' '), rawSectionLine.find_first_of(':')));

            std::string key = rawSectionLine.substr(rawSectionLine.find_first_of(':') + 1, rawSectionLine.find('=') - rawSectionLine.find_first_of(':') - 1);
            key = key.substr(key.find_first_not_of(' '), key.find_last_not_of(' ') + 1);

            std::string value = rawSectionLine.substr(rawSectionLine.find('=') + 1);
            value = value.substr(value.find_first_not_of(' '), value.find_last_not_of(' '));

            if (index >= sectionArray->GetSize()) {
                std::shared_ptr<Serialization::Cache::Section> section = std::make_shared<Serialization::Cache::Section>();
                section->Set(key, value);
                sectionArray->Set(index, section);
            } else {
                sectionArray->Get(index)->Set(key, value);
            }
        }
    } else {
        std::shared_ptr<Serialization::Cache::Section> section = std::dynamic_pointer_cast<Serialization::Cache::Section>(isection);

        for (const std::string &rawSectionLine : rawSectionLines) {
            std::string key = rawSectionLine.substr(0, rawSectionLine.find('='));
            key = key.substr(key.find_first_not_of(' '), key.find_last_not_of(' ') + 1);

            std::string value = rawSectionLine.substr(rawSectionLine.find('=') + 1);
            value = value.substr(value.find_first_not_of(' '), value.find_last_not_of(' '));

            section->Set(key, value);
        }
    }

    return isection;
}
