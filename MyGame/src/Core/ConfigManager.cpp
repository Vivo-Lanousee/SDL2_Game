#include "ConfigManager.h"
#include <fstream>
#include <iostream>


const std::string ConfigManager::CONFIG_FILEPATH = "assets/data/config.json";

// 設定をファイルに保存する
bool ConfigManager::Save(const GameParams& params) {
    const std::string& filepath = CONFIG_FILEPATH;

    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << filepath << " (Directory must exist)" << std::endl;
        return false;
    }

    try {
        json j;
        to_json(j, params);
        file << j.dump(4);
        std::cout << "Config saved successfully to: " << filepath << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error saving config: " << e.what() << std::endl;
        return false;
    }
}

// 設定をファイルからロードする
bool ConfigManager::Load(GameParams& params) {
    // ★★★ 修正箇所 3: 内部定数を使用 ★★★
    const std::string& filepath = CONFIG_FILEPATH;

    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Warning: Config file not found, using default parameters: " << filepath << std::endl;
        return false;
    }

    try {
        json j;
        file >> j;
        from_json(j, params);
        std::cout << "Config loaded successfully from: " << filepath << std::endl;
        return true;
    }
    catch (const json::exception& e) {
        std::cerr << "Error: JSON parsing failed while loading config: " << e.what() << std::endl;
        return false;
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading config: " << e.what() << std::endl;
        return false;
    }
}