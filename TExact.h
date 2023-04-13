#pragma once
#include <string>
class CInstallStubDlg;

bool extractZip(const std::string& zipPath, const std::string& extractPath, CInstallStubDlg* win);
