#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

char* regionOptions[] = {"USA", "EUR", "JPN"};
char* regionCodes[] = {"00040000001B8700", "000400000017CA00", "000400000017FD00"};
char selectedRegionCode[17];

void selectRegion() {
    consoleClear();
    printf("Select Region:\n");
    int choice = 0;
    int lastChoice = -1;
    while (1) {
        hidScanInput();
        u32 kDown = hidKeysDown();

        if (kDown & KEY_DOWN) {
            choice = (choice + 1) % 3;
        } else if (kDown & KEY_UP) {
            choice = (choice + 2) % 3;
        } else if (kDown & KEY_A) {
            strcpy(selectedRegionCode, regionCodes[choice]);
            break;
        }

        if (choice != lastChoice) {
            consoleClear();
            printf("Select Your Game Region:\n");
            for (int i = 0; i < 3; i++) {
                if (i == choice) {
                    printf("> %s\n", regionOptions[i]);
                } else {
                    printf("  %s\n", regionOptions[i]);
                }
            }
            gfxFlushBuffers();
            gfxSwapBuffers();
            lastChoice = choice;
        }
        gspWaitForVBlank();
    }
    consoleClear();
}

int listFolders(const char* path, char folderNames[20][256]) {
    DIR* dir;
    struct dirent* ent;
    int folderCount = 0;

    if ((dir = opendir(path)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_DIR && strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                strcpy(folderNames[folderCount], ent->d_name);
                folderCount++;
            }
        }
        closedir(dir);
    } else {
        perror("");
    }

    return folderCount;
}

void selectFolder(char folderNames[20][256], int folderCount, char selectedFolder[256]) {
    consoleClear();
    printf("Select Folder:\n");
    int choice = 0;
    int lastChoice = -1;
    while (1) {
        hidScanInput();
        u32 kDown = hidKeysDown();

        if (kDown & KEY_DOWN) {
            choice = (choice + 1) % folderCount;
        } else if (kDown & KEY_UP) {
            choice = (choice + folderCount - 1) % folderCount;
        } else if (kDown & KEY_A) {
            strcpy(selectedFolder, folderNames[choice]);
            break;
        }

        if (choice != lastChoice) {
            consoleClear();
            printf("Select a World:\n");
            for (int i = 0; i < folderCount; i++) {
                if (i == choice) {
                    printf("> %s\n", folderNames[i]);
                } else {
                    printf("  %s\n", folderNames[i]);
                }
            }
            gfxFlushBuffers();
            gfxSwapBuffers();
            lastChoice = choice;
        }
        gspWaitForVBlank();
    }
    consoleClear();
}

void copyDirectory(const char* src, const char* dest) {
    DIR* dir;
    struct dirent* ent;
    
    if ((dir = opendir(src)) != NULL) {
        if (mkdir(dest, 0777) && errno != EEXIST) {
            printf("Error creating destination directory: %s\n", dest);
            perror("mkdir");
            closedir(dir);
            return;
        }

        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_DIR) {
                if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                    char srcPath[512];
                    char destPath[512];
                    snprintf(srcPath, sizeof(srcPath), "%s/%s", src, ent->d_name);
                    snprintf(destPath, sizeof(destPath), "%s/%s", dest, ent->d_name);

                    copyDirectory(srcPath, destPath);
                }
            } else {
                char srcPath[512];
                char destPath[512];
                snprintf(srcPath, sizeof(srcPath), "%s/%s", src, ent->d_name);
                snprintf(destPath, sizeof(destPath), "%s/%s", dest, ent->d_name);

                FILE* srcFile = fopen(srcPath, "rb");
                if (srcFile == NULL) {
                    printf("Error opening source file: %s\n", srcPath);
                    perror("fopen");
                    continue;
                }

                FILE* destFile = fopen(destPath, "wb");
                if (destFile == NULL) {
                    printf("Error opening destination file: %s\n", destPath);
                    perror("fopen");
                    fclose(srcFile);
                    continue;
                }

                char buffer[1024];
                size_t bytes;
                while ((bytes = fread(buffer, 1, sizeof(buffer), srcFile)) > 0) {
                    if (fwrite(buffer, 1, bytes, destFile) != bytes) {
                        printf("Error writing to destination file: %s\n", destPath);
                        perror("fwrite");
                        break;
                    }
                }

                fclose(srcFile);
                fclose(destFile);
            }
        }
        closedir(dir);
    } else {
        printf("Error opening source directory: %s\n", src);
        perror("opendir");
    }
}

void deleteAndRenameFile(const char* path, const char* oldName, const char* newName) {
    char oldPath[512];
    char newPath[512];
    snprintf(oldPath, sizeof(oldPath), "%s/%s", path, oldName);
    snprintf(newPath, sizeof(newPath), "%s/%s", path, newName);
    remove(oldPath);
    rename(newPath, oldPath);
}

void createDirectoryRecursive(const char* path) {
    char temp[512];
    char* p = NULL;
    size_t len;

    snprintf(temp, sizeof(temp), "%s", path);
    len = strlen(temp);
    
    if (temp[len - 1] == '/') {
        temp[len - 1] = 0;
    }

    for (p = temp + 1; *p; p++) {
        if (*p == '/') {
            *p = 0;
            mkdir(temp, 0777);
            *p = '/';
        }
    }

    mkdir(temp, 0777);
}

int citraCheck() {
    struct stat st;
    if (stat("sdmc:/Nintendo 3DS/00000000000000000000000000000000", &st) == 0 && S_ISDIR(st.st_mode)) {
        consoleClear();
        svcSleepThread(1000000);
        return 1;
    }
    return 0;
}

void createWorldFolderName(const char* path) {
    FILE* file = fopen(path, "wb");
    if (file != NULL) {
        fclose(file);
        printf("File '%s' created successfully.\n", path);
    } else {
        printf("Error creating file '%s'.\n", path);
        perror("fopen");
    }
}

int extractSlotNumber(const char* filename) {
    int slotNumber;
    if (sscanf(filename, "slt%d.cdb", &slotNumber) == 1) {
        return slotNumber;
    }
    return -1;
}

int compareSlotNumbers(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

void generateTemplateJson(const char* destPath) {
    DIR* dir;
    struct dirent* ent;
    int slotNumbers[1024];
    int slotCount = 0;

    if ((dir = opendir(destPath)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG && strstr(ent->d_name, "slt") != NULL && strstr(ent->d_name, ".cdb") != NULL) {
                int slotNumber = extractSlotNumber(ent->d_name);
                if (slotNumber != -1) {
                    slotNumbers[slotCount++] = slotNumber;
                }
            }
        }
        closedir(dir);
    } else {
        perror("Could not open directory");
        return;
    }

    qsort(slotNumbers, slotCount, sizeof(int), compareSlotNumbers);

    char jsonPath[512];
    snprintf(jsonPath, sizeof(jsonPath), "%s/template.json", destPath);
    FILE* jsonFile = fopen(jsonPath, "w");
    if (jsonFile == NULL) {
        perror("Could not create template.json");
        return;
    }

    fprintf(jsonFile, "{\n    \"slot_files\": [\n");
    for (int i = 0; i < slotCount; i++) {
        fprintf(jsonFile, "        %d", slotNumbers[i]);
        if (i < slotCount - 1) {
            fprintf(jsonFile, ",");
        }
        fprintf(jsonFile, "\n");
    }
    fprintf(jsonFile, "    ]\n}\n");

    fclose(jsonFile);
}

void clearDirectory(const char* path) {
    DIR* dir;
    struct dirent* ent;
    
    if ((dir = opendir(path)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG) {
                char filePath[1024];
                snprintf(filePath, sizeof(filePath), "%s/%s", path, ent->d_name);
                remove(filePath);
            } else if (ent->d_type == DT_DIR) {
                if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                    char dirPath[1024];
                    snprintf(dirPath, sizeof(dirPath), "%s/%s", path, ent->d_name);
                    clearDirectory(dirPath);
                    rmdir(dirPath);
                }
            }
        }
        closedir(dir);
    } else {
        perror("opendir");
    }
}


int main() {
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);
    srvInit();
    fsInit();
    int citraVarCheck = 0;

    createDirectoryRecursive("sdmc:/Minecraft 3DS/worlds");

    char folderNames[20][256];
    int folderCount = listFolders("sdmc:/Minecraft 3DS/worlds", folderNames);

    if (folderCount > 0 && citraVarCheck == 0) {
        selectRegion();
        char selectedFolder[512];
        selectFolder(folderNames, folderCount, selectedFolder);

        char srcPath[1024];
        char destPath[1024];
        char worldName[1024];
        snprintf(srcPath, sizeof(srcPath), "sdmc:/Minecraft 3DS/worlds/%s/db/cdb", selectedFolder);
        snprintf(destPath, sizeof(destPath), "sdmc:/luma/titles/%s/romfs/templates/mario", selectedRegionCode);
        int totalWorldSize = sizeof(destPath)+sizeof(selectedFolder);
        snprintf(worldName, totalWorldSize, "%s/%s.name", destPath, selectedFolder);

        printf("Creating Required Folders...\n");
        createDirectoryRecursive(destPath);
        svcSleepThread(1500000000);
        consoleClear();

        printf("Removing Any Former Worlds...\n");
        svcSleepThread(1500000000);
        clearDirectory(destPath);
        consoleClear();

        printf("Copying directory...\n");
        copyDirectory(srcPath, destPath);
        consoleClear();

        printf("Deleting Unnessisary Files...\n");
        deleteAndRenameFile(destPath, "index.cdb", "newindex.cdb");
        svcSleepThread(1500000000);
        consoleClear();

        printf("Retrieving World Name and Applying it...\n");
        createWorldFolderName(worldName);
        svcSleepThread(2500000000);
        consoleClear();

        printf("Creating Slot Numbering File...\n");
        generateTemplateJson(destPath);
        svcSleepThread(5000000000);
        consoleClear();

        printf("Conversion Completed...\nStart Minecraft 3DS Edition\nAnd make a new 'Mario' World.\n\n");
        printf("\nPress 'start' to Exit.");
        gspWaitForVBlank();
        gfxFlushBuffers();
        gfxSwapBuffers();
        svcSleepThread(150000000);
        while (1) {
            hidScanInput();
            u32 kDown = hidKeysDown();

            if (kDown & KEY_START) {
                break;
            }
        }
    }   else if (folderCount == 0 && citraVarCheck == 0) {
        printf("Such Emptiness...\nHead on over to the UniStore to download some \nreally cool maps!\n\n");
        printf("\nPress 'start' to Exit.");
        gspWaitForVBlank();
        gfxFlushBuffers();
        gfxSwapBuffers();
        while (1) {
            hidScanInput();
            u32 kDown = hidKeysDown();

            if (kDown & KEY_START) {
                break;
            }
        }
    }
    else {
        printf("\nI'm not supposed to run in here.\nI was made for a REAL 3DS Console.\nYou're welcome to use my PC-Release!\n\n\n\n\n\n\n\n\n\n");
        printf("\nPress 'start' to Exit.");
        gspWaitForVBlank();
        gfxFlushBuffers();
        gfxSwapBuffers();
        while (1) {
            hidScanInput();
            u32 kDown = hidKeysDown();

            if (kDown & KEY_START) {
                break;
            }
        }
    }

    fsExit();
    gfxExit();
    aptExit();
    return 0;
}
