#pragma once
#include <string>
#include "GameParams.h"
#include <nlohmann/json.hpp> 

class ConfigManager {
public:
    // ★★★ 修正箇所: filepath 引数を削除 ★★★
    static bool Save(const GameParams& params);

    // ★★★ 修正箇所: filepath 引数を削除 ★★★
    static bool Load(GameParams& params);

private:
    ConfigManager() = delete;

    // ★★★ 修正箇所: パスを内部定数として定義 ★★★
    static const std::string CONFIG_FILEPATH;
};