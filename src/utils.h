#pragma once

#include <fstream>
#include <string>
#include <cerrno>
#include <iostream>

inline std::string read_entire_file(const char *filename)
{
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (in) {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return(contents);
    }
    throw(errno);
}
inline uint8_t read_first_byte_of_file(std::string filename) {
    FILE *fileptr;
    char buffer;
    long filelen;

    fileptr = fopen(filename.c_str(), "rb");  // Open the file in binary mode
    fseek(fileptr, 0, SEEK_END);          // Jump to the end of the file
    filelen = ftell(fileptr);             // Get the current byte offset in the file

    if (filelen == 0)
    {
        return 0;
    }

    rewind(fileptr);                      // Jump back to the beginning of the file

    fread(&buffer, 1, 1, fileptr); // Read in the entire file
    fclose(fileptr); // Close the file

    return buffer;
}

inline void write_to_file (std::string filename, std::string data) {
  std::ofstream file;
  file.open (filename);
  file << data;
  file.close();
}