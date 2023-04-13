#include "stdafx.h"
#include "Resource.h"
#include "InstallStubDlg.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "unzip.h"
#include <direct.h>
#include <Windows.h>
#include "TExact.h"


int32_t mkdirs(const std::string& directoryPath)
{
    size_t dirPathLen = directoryPath.length();
    if (dirPathLen > MAX_PATH)
    {
        return -1;
    }
    char tmpDirPath[MAX_PATH] = { 0 };
    for (uint32_t i = 0; i < dirPathLen; ++i)
    {
        tmpDirPath[i] = directoryPath[i];
        if (tmpDirPath[i] == '\\' || tmpDirPath[i] == '/')
        {
            if (GetFileAttributesA(tmpDirPath) == INVALID_FILE_ATTRIBUTES)
            {
                int32_t ret = _mkdir(tmpDirPath);
                if (ret != 0)
                {
                    return ret;
                }
            }
        }
    }
    return 0;
}



bool extractZip(const std::string& zipPath, const std::string& extractPath, CInstallStubDlg* win) {
    unzFile zipFile = unzOpen(zipPath.c_str());

    if (!zipFile) {
        std::cerr << "Could not open ZIP file: " << zipPath << '\n';
        return false;
    }
    unz_global_info globalInfo;
    if (unzGetGlobalInfo(zipFile, &globalInfo) != UNZ_OK) {
        std::cerr << "Could not get global ZIP info.\n";
        unzClose(zipFile);
        return false;
    }

    char buffer[1024];

    for (uint32_t i = 0; i < globalInfo.number_entry; i++) {
        

        unz_file_info fileInfo;
        if (unzGetCurrentFileInfo(zipFile, &fileInfo, buffer, sizeof(buffer), NULL, 0, NULL, 0) != UNZ_OK) {
            std::cerr << "Could not get file info for file #" << i << ".\n";
            unzClose(zipFile);
            return false;
        }
        std::string entryName(buffer, fileInfo.size_filename);
        if (unzOpenCurrentFile(zipFile) != UNZ_OK) {
            std::cerr << "Could not open file #" << i << " in ZIP.\n";
            unzClose(zipFile);
            return false;
        }

        std::string extractFilePath = extractPath + "/" + entryName;
        // 如果是目录，则创建目录
        if (mkdirs(extractFilePath.c_str()) != 0) {
            std::cerr << "Could not create directory: " << extractFilePath << '\n';
            unzCloseCurrentFile(zipFile);
            unzClose(zipFile);
            return false;
        }

        // 如果是文件，则创建文件并写入数据
        {
            std::ofstream outFile(extractFilePath, std::ios::binary);
            if (!outFile) {
                std::cerr << "Could not create file: " << extractFilePath << '\n';
                unzCloseCurrentFile(zipFile);
                unzClose(zipFile);
                return false;
            }
            int result = UNZ_OK;
            do {
                result = unzReadCurrentFile(zipFile, buffer, sizeof(buffer));
                if (result < 0) {
                    std::cerr << "Error " << result << " while reading file #" << i << " in ZIP.\n";
                    unzCloseCurrentFile(zipFile);
                    unzClose(zipFile);
                    return false;
                }
                if (result > 0) {
                    outFile.write(buffer, result);
                }
            } while (result > 0);
            outFile.close();
            win->m_ExactProgressInt = (i * 100 / globalInfo.number_entry + 1);
            char buf[1000];
            sprintf(buf, "%u%%, %s", win->m_ExactProgressInt, entryName.c_str());
            win->m_EaxctPercent = buf;
            win->PostMessage(WM_YY_UPDATE, 0, 0);
        }
        unzCloseCurrentFile(zipFile);
        if (unzGoToNextFile(zipFile) != UNZ_OK) {
            break;
        }
    }

    unzClose(zipFile);
    std::cout << "Extract succeeded.\n";
    return true;
}
