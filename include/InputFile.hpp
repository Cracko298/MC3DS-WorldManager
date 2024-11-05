#pragma once
#include "FileBase.hpp"
#include <string>

namespace FsLib
{
    class InputFile : public FsLib::FileBase
    {
        public:
            InputFile(void) = default;
            // Constructor that calls open.
            InputFile(const std::u16string &FilePath);
            // Opens file for reading. IsOpen can be used to see if this was successful.
            void Open(const std::u16string &FilePath);
            // Attempts to read ReadSize into Buffer. Returns number of bytes read on success, 0 on failure.
            size_t Read(void *Buffer, size_t ReadSize);
            // Attempts to read a line from the file. Returns true on success, false on failure. Line is written to LineOut.
            bool ReadLine(std::string &LineOut);
            // Reads a character from file. Returns -1 on failure.
            char GetCharacter(void);
    };
} // namespace FsLib
