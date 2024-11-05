#pragma once
#include <3ds.h>
#include <string>
#include <vector>

namespace FsLib
{
    class Directory
    {
        public:
            Directory(void) = default;
            // Just calls the open function for you.
            Directory(const std::u16string &DirectoryPath);
            // Opens directory and reads contents. Whether operation was successful or not can be checked with IsOpen()
            void Open(const std::u16string &DirectoryPath);
            // Returns whether directory was successfully opened and read or not.
            bool IsOpen(void) const;
            // Returns number of entries in directory.
            uint32_t GetEntryCount(void) const;
            // Returns whether or not the entry at index is a directory.
            bool EntryAtIsDirectory(int index) const;
            // Returns entry path as a standard C++/UTF8 string. This is for display purposes.
            std::string GetEntryPathAtAsUTF8(int index) const;
            // Same as above, but as UTF16. For filesystem operations.
            std::u16string GetEntryPathAtAsUTF16(int index) const;
            // Returns entry name as a standard C++/UTF8 string. This should be used for display purposes.
            std::string GetEntryNameAtAsUTF8(int index) const;
            // Returns Entry name as u16string. This should be used for building and reading file paths.
            std::u16string GetEntryNameAtAsUTF16(int index) const;

        private:
            // Directory handle.
            Handle m_DirectoryHande;
            // Directory path
            std::u16string m_DirectoryPath;
            // Whether Open was successful
            bool m_WasOpened = false;
            // Directory entry vector.
            std::vector<FS_DirectoryEntry> m_DirectoryList;
            // Shortcut to close directory handle. Automatically called after read and not needed outside of here.
            bool Close(void);
    };
} // namespace FsLib
