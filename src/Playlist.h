#pragma once

#include <stdio.h>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <set>
#include <random>
#include <algorithm>


struct Playlist {
    uint32_t id = 0;
    std::string name;
    std::set<std::filesystem::path> tracks;
};

using Playlists = std::unordered_map<uint32_t, Playlist>;

Playlists parse_playlists();

std::vector<std::filesystem::path> randomize_playlist(const Playlist& playlist, std::random_device random);
