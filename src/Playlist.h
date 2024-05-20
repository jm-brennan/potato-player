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
    std::vector<std::filesystem::path> tracks;
    std::vector<uint> playOrder;
};

using Playlists = std::unordered_map<uint32_t, Playlist>;

Playlists parse_playlists();

void randomize_playlist_play_order(Playlist& playlist, std::random_device& random);
