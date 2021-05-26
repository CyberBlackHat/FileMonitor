#if defined(_WIN32) 
#include <stdio.h>
#include <windows.h>
#include <Wincrypt.h>
#include <sstream>
#include "CommonDefs.h"

static const int BUFSIZE = 1024; 
static const int MD5LEN = 16; 
std::string calculate_md5_hash_of_file(const String& fname, unsigned long& status) {
    BOOL bResult = FALSE;
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    HANDLE hFile = NULL;
    BYTE rgbFile[BUFSIZE];
    DWORD cbRead = 0;
    BYTE rgbHash[MD5LEN];
    DWORD cbHash = 0;
    CHAR rgbDigits[] = "0123456789abcdef";
    LPCWSTR filename = fname.c_str();
    std::stringstream strstream; 
    // Logic to check usage goes here.

    hFile = CreateFile(filename,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_SEQUENTIAL_SCAN,
        NULL);

    if (INVALID_HANDLE_VALUE == hFile)
    {
        status = GetLastError();
        printf("Error opening file %s\nError: %d\n", filename,
            status);
        
        return strstream.str();
    }

    // Get handle to the crypto provider
    if (!CryptAcquireContext(&hProv,
        NULL,
        NULL,
        PROV_RSA_FULL,
        CRYPT_VERIFYCONTEXT))
    {
        status = GetLastError();
        printf("CryptAcquireContext failed: %d\n", status);
        CloseHandle(hFile);
 
        return strstream.str();
    }

    if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash))
    {
        status = GetLastError();
        printf("CryptAcquireContext failed: %d\n", status);
        CloseHandle(hFile);
        CryptReleaseContext(hProv, 0);
        return strstream.str();
    }

    while (bResult = ReadFile(hFile, rgbFile, BUFSIZE,
        &cbRead, NULL))
    {
        if (0 == cbRead)
        {
            break;
        }

        if (!CryptHashData(hHash, rgbFile, cbRead, 0))
        {
            status = GetLastError();
            printf("CryptHashData failed: %d\n", status);
            CryptReleaseContext(hProv, 0);
            CryptDestroyHash(hHash);
            CloseHandle(hFile);
            return strstream.str();
        }
    }

    if (!bResult)
    {
        status = GetLastError();
        printf("ReadFile failed: %d\n", status);
        CryptReleaseContext(hProv, 0);
        CryptDestroyHash(hHash);
        CloseHandle(hFile);
        return strstream.str();
    }

    cbHash = MD5LEN;
    if (CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0))
    {
        printf("Calculating MD5 hash of file %s is: ", filename);
        for (DWORD i = 0; i < cbHash; i++)
        {
            strstream << rgbDigits[rgbHash[i] >> 4] << rgbDigits[rgbHash[i] & 0xf]; 
        }
        printf("%s\n", strstream.str().c_str());
    }
    else
    {
        status = GetLastError();
        printf("CryptGetHashParam failed: %d\n", status);
    }

    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);
    CloseHandle(hFile);
    return strstream.str();
}
#else 
//Should provide here other implementation for MAC & LINUX

#endif 