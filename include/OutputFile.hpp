#pragma once
#include "FileBase.hpp"
#include <string>

namespace FsLib
{
    class OutputFile : public FsLib::FileBase
    {
        public:
            OutputFile(void) = default;
            // This constructor calls Open for you.
            OutputFile(const std::u16string &FilePath, bool Append);
            /*
                Opens and file for writing. Append is whether the file is being appended to.
                If false is passed to Append, the file is deleted before being created and opened.
                IsOpen can be used to tell if the operation was successful.
            */
            void Open(const std::u16string &FilePath, bool Append);
            // Attempts to write Buffer of WriteSize bytes to file. Returns number of bytes written on success, zero on failure.
            size_t Write(const void *Buffer, size_t WriteSize);
            // Attempts to write a formatted string to file. Returns true on success.
            bool Writef(const char *Format, ...);
            // Operator for quick, lazy string writing
            void operator<<(const char *String);
            // Writes a single byte to file. Returns true on success.
            bool PutCharacter(char C);
            // Flushes file. Returns true on success.
            bool Flush(void);

        private:
            // These functions attempt to open file for the respective. Return false on failure.
            bool OpenForWriting(FS_Archive Archive, const char16_t *FilePath);
            bool OpenForAppending(FS_Archive Archive, const char16_t *FilePath);
            // This function will resize the file if necessary to fit the incoming buffer.
            bool ResizeIfNeeded(size_t BufferSize);
    };
} // namespace FsLib
