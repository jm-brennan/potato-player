#include <stdio.h>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <set>

#include "audio_player.h"
#include "Playlist.h"


int main() {
    std::random_device random;
    Playlists playlists = parse_playlists();

    std::vector<std::filesystem::path> tracks = randomize_playlist(playlists[2], random);

    //fs::path playpath = *playlists[2].tracks.begin();
    //std::cout << "play: " << playpath << "\n";

    play(tracks);
}