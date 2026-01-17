#pragma once
#include <string>
#include "GameParams.h"
#include <nlohmann/json.hpp> 

class ConfigManager {
public:
    static bool Save(const GameParams& params);

    static bool Load(GameParams& params);

private:
    ConfigManager() = delete;

    static const std::string CONFIG_FILEPATH;
};