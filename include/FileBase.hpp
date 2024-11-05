#pragma once
#include <3ds.h>
#include <cstdint>

namespace FsLib
{
    enum class SeekOrigin
    {
        Beginning,
        Current,
        End
    };

    class FileBase
    {
        public:
            FileBase(void) = default;
            // This calls close.
            virtual ~FileBase();
            // Closes file Handle.
            void Close(void);
            // Returns if file was successfully opened.
            bool IsOpen(void) const;
            // Returns the current offset in file.
            uint64_t Tell(void) const;
            // Returns the size of the file.
            uint64_t GetSize(void) const;
            // Returns whether end of file has been reached.
            bool EndOfFile(void) const;
            /*
                Seeks to position relative to Origin. Origin can be the following:
                    FsLib::SeekOrigin::Beginning
                    FsLib::SeekOrigin::Current
                    FsLib::SeekOrigin::End
                Offset will be checked and corrected if out of bounds.
            */
            void Seek(int64_t Offset, FsLib::SeekOrigin Origin);

        protected:
            // File handle.
            Handle m_FileHandle;
            // Whether or not file is open
            bool m_IsOpen = false;
            // Offset and size
            int64_t m_Offset, m_FileSize;
            // This can be called to check and correct the offset to a certain degree.
            void EnsureOffsetIsValid(void);
    };
} // namespace FsLib
