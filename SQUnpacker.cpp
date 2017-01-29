// SQUnpacker.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

BOOL Extract(LPCSTR pszFileName) {
	char szFile[MAX_PATH];

	sprintf(szFile, "files\\%s", pszFileName);

	HANDLE hFile = CreateFileA(szFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		_cprintf("Error: Failed opening file %s(%d)\n", pszFileName, GetLastError());
		return FALSE;
	} else {
		// sprintf(szFile, "output\\%s", pszFileName);
		// CreateDirectoryA(szFile, NULL);

		HANDLE hMapping = CreateFileMappingA(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
		LPCSTR pszData = (LPCSTR)MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);
		LPCSTR pszDataEnd = pszData + GetFileSize(hFile, NULL);

		LPCSTR ppszData = pszData + 0x1074;
		LPDWORD pIntFiles = (LPDWORD) (pszData + 0x9a);

		INT64 dwSections = *(PINT64) ppszData;
		ppszData += 8;

		for (int c = 0; c < dwSections; c++) {
			ppszData += 8;
		}

		_cprintf("Files: %d\n", *pIntFiles);

		for (int c = 0; c < *pIntFiles; c++) {
			DWORD dwLen = *(LPDWORD) ppszData;
			ppszData += 4;

			dwLen += (4 - (dwLen%4))%4;
			ppszData += dwLen;
			ppszData += 16;
		}

		if (*ppszData == 0x01) {
			ppszData += 16;
		} else if (*ppszData == 0x02) {
			ppszData += 24;
		} else {
			DebugBreak();
		}

		if (*ppszData == 0) {
			ppszData += 4;
		}

		for (int c = 0; ppszData < pszDataEnd && c < *pIntFiles; c++) {
			DWORD dwLen = *(LPDWORD) ppszData;
			ppszData += 4;

			dwLen += (4 - (dwLen%4))%4;
			ZeroMemory(szFile, MAX_PATH);
			memcpy(szFile, ppszData, dwLen);
			ppszData += dwLen;

			dwLen = *(LPDWORD) ppszData;
			ppszData += 4;

			if (dwLen == 2/* && (*(LPDWORD) ppszData == 0x14 || *(LPDWORD) ppszData == 0xd)*/) {
				// WAV?
				ppszData += 12;
				dwLen = *(LPDWORD) ppszData;
				ppszData += 4;
			} else if (dwLen == 0x00000200 && *(LPDWORD) ppszData == 0x00000080) { // 5167a9ed345a204b9e101a7d0daa35f0608ee884
				ppszData += 48;
				dwLen = *(LPDWORD) ppszData;
				ppszData += 4;
			} else if (dwLen == 0x00000400 && *(LPDWORD) ppszData == 0x00000100) { // c3c2006a523eccab5b443fb874459f91f2e8c34b3
				ppszData += 48;
				dwLen = *(LPDWORD) ppszData;
				ppszData += 4;
			}

			if (*szFile) {
				_cprintf("%s(%d)\n", szFile, dwLen);

				char szFile2[MAX_PATH];
				sprintf(szFile2, "output\\%s_%s", pszFileName, szFile);

				if (dwLen>4 && !strncmp(ppszData + 1, "PNG", 3)) {
					strcat(szFile2, ".png");
				} else if (dwLen>4 && !strncmp(ppszData, "RIFF", 4)) {
					strcat(szFile2, ".wav");
				} else if (dwLen>2 && *(LPWORD)ppszData == 0xfbff) {
					strcat(szFile2, ".mp3");
				}

				HANDLE hFile2 = CreateFileA(szFile2, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
				DWORD dwWritten;
				WriteFile(hFile2, ppszData, dwLen, &dwWritten, NULL);
				CloseHandle(hFile2);
			}
			/*
			dwLen += 4;
			dwLen += (8 - ((ppszData + dwLen - pszData) % 8)) % 8;
			ppszData += dwLen;
			*/
			ppszData += dwLen;
			while (ppszData < pszDataEnd && *ppszData == 0) ppszData++;
		}

		UnmapViewOfFile(pszData);
		CloseHandle(hMapping);
		CloseHandle(hFile);

		_cprintf("\n");
		return TRUE;
	}
	return FALSE;
}

BOOL Extract2(LPCSTR pszFileName) {
	char szFile[MAX_PATH];

	sprintf(szFile, "Data\\%s", pszFileName);

	HANDLE hFile = CreateFileA(szFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		_cprintf("Error: Failed opening file %s(%d)\n", pszFileName, GetLastError());
		return FALSE;
	} else {
		HANDLE hMapping = CreateFileMappingA(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
		LPCSTR pszData = (LPCSTR)MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);
		LPCSTR pszDataEnd = pszData + GetFileSize(hFile, NULL);

		BYTE bType = pszData[3];

		_cprintf("File Type: 0x%02x\n", bType);

		if ( bType == 0x31) {
			// png / mp3
			LPCSTR ppszData = pszData + 0x1000;
			DWORD dwLen = *(LPDWORD) ppszData;
			ppszData += 4;
			ZeroMemory(szFile, MAX_PATH);
			memcpy(szFile, ppszData, dwLen);

			dwLen += (4 - (dwLen % 4)) % 4;
			ppszData += dwLen;

			LPDWORD lpdwLen = (LPDWORD) ppszData;

			if (*lpdwLen == 0x02 && *(LPDWORD)(ppszData + 4) == 0x0d) {
				ppszData += 16;
			
				lpdwLen = (LPDWORD) ppszData;
			}

			ppszData += 4;

			_cprintf("File: %s(%d)\n", szFile, *lpdwLen);

			char szFile2[MAX_PATH];
			sprintf(szFile2, "output2\\%s_%s", pszFileName, szFile);

			if (*lpdwLen>4 && !strncmp(ppszData + 1, "PNG", 3)) {
				strcat(szFile2, ".png");
			} else if (*lpdwLen>4 && !strncmp(ppszData, "RIFF", 4)) {
				strcat(szFile2, ".wav");
			} else if (*lpdwLen>2 && *(LPWORD)ppszData == 0xfbff) {
				strcat(szFile2, ".mp3");
			}

			HANDLE hFile2 = CreateFileA(szFile2, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
			DWORD dwWritten;
			WriteFile(hFile2, ppszData, *lpdwLen, &dwWritten, NULL);
			CloseHandle(hFile2);
		} else {
			// Error
			// DebugBreak();
		}

		UnmapViewOfFile(pszData);
		CloseHandle(hMapping);
		CloseHandle(hFile);

		_cprintf("\n");
		return TRUE;
	}
	return FALSE;
}

int _tmain(int argc, _TCHAR* argv[])
{
	CreateDirectoryA("output", NULL);
	CreateDirectoryA("output2", NULL);

	_cprintf("SisterQuest RPG Android resource extractor v1.0\n");
	_cprintf("Written by Stark Wong@StudioKUMA 2017\n\n");

	WIN32_FIND_DATAA ffd;

	HANDLE hFind = FindFirstFileA("files\\*", &ffd);

	if (hFind == INVALID_HANDLE_VALUE) {
		_cprintf("Warning: Cannot open 'files' directory\n");
	} else {
		int c = 0;
		do {
			if ((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
				_cprintf("%s\n", ffd.cFileName);
				if (!Extract(ffd.cFileName)) break;

			}
		} while (FindNextFileA(hFind, &ffd));

		FindClose(hFind);
	}


	hFind = FindFirstFileA("Data\\*", &ffd);

	if (hFind == INVALID_HANDLE_VALUE) {
		_cprintf("Warning: Cannot open 'Data' directory\n");
	} else {
		int c = 0;
		do {
			if ((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 && !strchr(ffd.cFileName,'.')) {
				_cprintf("%s\n", ffd.cFileName);
				if (!Extract2(ffd.cFileName)) break;

			}
		} while (FindNextFileA(hFind, &ffd));

		FindClose(hFind);
	}

	_getch();

	return 0;
}

