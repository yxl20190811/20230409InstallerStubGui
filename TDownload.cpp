#include "stdafx.h"
#include "Resource.h"
#include "InstallStubDlg.h"
#include "TDownload.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <curl/curl.h>

struct write_ptr {
    FILE* m_fp;
    CInstallStubDlg* m_win;
};
// Callback function for curl to write downloaded data to file
static size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream) {
    struct write_ptr* data = (struct write_ptr*)stream;
    size_t written = fwrite(ptr, size, nmemb, data->m_fp);
    CInstallStubDlg* win = data->m_win;
    {
        char buf[1000];
        win->m_DownloadProgressInt += written;
        sprintf(buf, "downloaded: %u(total:%u)", win->m_DownloadProgressInt, 0);
        win->m_DownloadPercent = buf;
        
        win->PostMessage(WM_YY_UPDATE, 0, 0);
    }
    return written;
}

// Callback function for curl to display download progress
static int progress_func(void* ptr, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
    
    //std::cout << "Downloaded " << dlnow << " of " << dltotal << " bytes.\n";
    return 0;
}

// Function to download file from given url to specified path
bool downloadFile(const std::string& url, const std::string& path, CInstallStubDlg* win) {
    CURL* curl;
    FILE* fp = NULL;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        FILE* fp = NULL;
        fopen_s(&fp, path.c_str(), "wb");
        if (!fp) {
            std::cerr << "Could not open file: " << path << '\n';
            curl_easy_cleanup(curl);
            return false;
        }
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        struct write_ptr tmp;
        tmp.m_fp = fp;
        tmp.m_win = win;
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &tmp);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
        //curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, win);
        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_func);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36");
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // Disable certificate verification
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); // Disable hostname verification
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "Error downloading file: " << curl_easy_strerror(res) << '\n';
            fclose(fp);
            curl_easy_cleanup(curl);
            return false;
        }
        fclose(fp);
        curl_easy_cleanup(curl);
        return true;
    }
    return false;
}
