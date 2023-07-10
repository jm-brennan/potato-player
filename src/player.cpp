#include <stdio.h>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <set>

#include "audio_player.h"

namespace fs = std::filesystem;
using namespace nlohmann;

struct Playlist {
    uint32_t id = 0;
    std::string name;
    std::set<fs::path> tracks;
};

using Playlists = std::unordered_map<uint32_t, Playlist>;

void add_all_tracks_in_dir(fs::path dir, std::set<fs::path>& tracks) {
    for (const auto& path : fs::directory_iterator(dir)) {
        if (!fs::exists(path)) {
            continue;
        }

        if (fs::is_directory(path)) {
            std::cout << "adding dir " << path << "\n";
            add_all_tracks_in_dir(path, tracks);
        } else {
            std::cout << "adding track " << path << "\n";
            tracks.insert(path);
        }
    }
}

Playlists parse_playlists() {
    const std::string PLAYLISTS_DIR = "../playlists";
    const std::string TRACKS_DIR = "../tracks";
    const std::string ID_KEY = "id";
    const std::string NAME_KEY = "name";
    const std::string TRACKS_KEY = "tracks";

    fs::path playlistsDir(PLAYLISTS_DIR);
    fs::path tracksDir(TRACKS_DIR);
    std::cout << "playlist root is " << playlistsDir << "\n";


    Playlists playlists;

    for (const auto& playlistFile : fs::directory_iterator(playlistsDir)) {
        if (fs::is_directory(playlistFile.path())) {
            continue;
        }

        std::cout << "parsing playlist file " << playlistFile.path() << std::endl;

        std::ifstream ifs(playlistFile.path());

        json jf;
        try {
             jf = json::parse(ifs);
            std::cout << "parsed json: " << jf << "\n";
        } catch (json::exception& e) {
            std::cout << e.what() << "\n";
            continue;
        }

        if (jf.contains(ID_KEY) && jf.contains(NAME_KEY) && jf.contains(TRACKS_KEY)) {
            uint32_t id = jf[ID_KEY];

            if (playlists.find(id) != playlists.end()) {
                std::cout << "Warning, overwriting playlist id " << id << "\n";
            }

            Playlist playlist;
            playlist.id = id;
            playlist.name = jf[NAME_KEY];

            for (const auto& track : jf[TRACKS_KEY]) {
                fs::path trackPath = tracksDir / fs::path(track);
                std::cout << "\ntrack listing: " << trackPath << "\n";

                if (!fs::exists(trackPath)) {
                    std::cout << "track at " << trackPath << " does not exist\n";
                    continue;
                }

                if (fs::is_directory(trackPath)) {
                    std::cout << "playlist " << id << " contains all tracks in directory " << trackPath << "\n";
                    add_all_tracks_in_dir(trackPath, playlist.tracks);
                } else {
                    std::cout << "found track " << track << "\n";
                    playlist.tracks.insert(trackPath);
                }
            }

            playlists[id] = playlist;
        }
        else
        {
            std::cout << "Playlist file " << playlistFile.path() << " is not formatted correctly";
        }
    }
    return playlists;
}

int main() {
    Playlists playlists = parse_playlists();

    fs::path playpath = *playlists[1].tracks.begin();
    std::cout << "play: " << playpath << "\n";

    play(playpath);
}