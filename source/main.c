#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <citro3d.h>
#include <signal.h>

void copyDirectory(const char *src, const char *dst);
void cleanup() {
    socExit();
    gfxExit();
    cfguExit();
    aptExit();
    srvExit();
    gspExit();
}

void deleteFile(const char *path) {
    remove(path);
}

void renameFile(const char *oldPath, const char *newPath) {
    rename(oldPath, newPath);
}

int main() {
    atexit(cleanup);
    if (srvInit() != 0) {
        printf("Failed to initialize srv.\n");
        return 1;
    }
    if (aptInit() != 0) {
        printf("Failed to initialize apt.\n");
        srvExit();
        return 1;
    }
    gfxInitDefault();
    if (cfguInit() != 0) {
        printf("Failed to initialize cfgu.\n");
        gfxExit();
        aptExit();
        srvExit();
        return 1;
    }

    consoleInit(GFX_TOP, NULL);
    consoleClear();

    gfxFlushBuffers();
    gfxSwapBuffers();
    gspWaitForVBlank();

    printf("Select Region:\nA. USA\nB. EUR\nX. JPN\n");

    // Region selection
    int region = 0;
    while (region == 0) {
        hidScanInput();
        u32 kDown = hidKeysDown();
        if (kDown & KEY_A) region = 1;
        else if (kDown & KEY_B) region = 2;
        else if (kDown & KEY_X) region = 3;

        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    const char *regionCode;
    switch (region) {
        case 1: regionCode = "00040000001B8700"; break;
        case 2: regionCode = "000400000017CA00"; break;
        case 3: regionCode = "000400000017FD00"; break;
    }

    printf("Selected region code: %s\n", regionCode);

    DIR *dir = opendir("/Minecraft 3DS/worlds");
    if (!dir) {
        printf("Failed to open directory '/Minecraft 3DS/worlds'.\n");
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
        gfxExit();
        atexit(cleanup);
        aptExit();
    }

    struct dirent *entry;
    char folders[256][256];
    int folderCount = 0;

    // Reading directories
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            strcpy(folders[folderCount++], entry->d_name);
        }
    }
    closedir(dir);

    printf("Select Folder:\n");
    for (int i = 0; i < folderCount; i++) {
        printf("%d. %s\n", i + 1, folders[i]);
    }

    // Folder selection
    int folderIndex = -1;
    while (folderIndex < 0 || folderIndex >= folderCount) {
        hidScanInput();
        u32 kDown = hidKeysDown();
        if (kDown & KEY_A) folderIndex = 0; // Placeholder: update this logic based on actual input scheme

        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    printf("Selected folder: %s\n", folders[folderIndex]);

    // Set up source and destination paths
    char srcPath[256], dstPath[256];
    snprintf(srcPath, sizeof(srcPath), "/Minecraft 3DS/worlds/%s/db/cdb", folders[folderIndex]);
    snprintf(dstPath, sizeof(dstPath), "/luma/titles/%s/romfs/templates/mario", regionCode);

    printf("Copying directory...\n");
    copyDirectory(srcPath, dstPath);

    // Handle file operations
    char indexPath[256], newIndexPath[256];
    snprintf(indexPath, sizeof(indexPath), "/luma/titles/%s/romfs/templates/mario/index.cdb", regionCode);
    snprintf(newIndexPath, sizeof(newIndexPath), "/luma/titles/%s/romfs/templates/mario/newindex.cdb", regionCode);

    deleteFile(indexPath);
    renameFile(newIndexPath, indexPath);

    printf("Operation completed.\n");

    gfxFlushBuffers();
    gfxSwapBuffers();
    gspWaitForVBlank();
    gfxExit();
    return 0;
}

void copyDirectory(const char *src, const char *dst) {
    srvInit();
    DIR *dir = opendir(src);
    if (!dir) return;

    mkdir(dst, 0777);

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        char srcPath[512], dstPath[512]; // Increased buffer size
        snprintf(srcPath, sizeof(srcPath), "%s/%s", src, entry->d_name);
        snprintf(dstPath, sizeof(dstPath), "%s/%s", dst, entry->d_name);

        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
            copyDirectory(srcPath, dstPath);
        } else {
            FILE *srcFile = fopen(srcPath, "rb");
            FILE *dstFile = fopen(dstPath, "wb");
            if (srcFile && dstFile) {
                char buffer[1024];
                size_t bytes;
                while ((bytes = fread(buffer, 1, sizeof(buffer), srcFile)) > 0) {
                    fwrite(buffer, 1, bytes, dstFile);
                }
                fclose(srcFile);
                fclose(dstFile);
            }
        }
    }
    closedir(dir);
}
