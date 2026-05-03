/* AVIS-ARTIFACT: SCANNER OPS HEADER */
/* FILENAME: scanner_ops.h */

#ifndef SCANNER_OPS_H
#define SCANNER_OPS_H

#include <windows.h>

extern char CURRENT_PATH[MAX_PATH];

void InitCurrentPath(const char *root);

void FillDrives(HWND hList);
void FillDirs(HWND hList, const char *root);
void FillFiles(HWND hList, const char *root);

void AddSelectedFile(HWND hWnd);
void CompileSources(HWND hWnd);
void CopyToClipboard(HWND hWnd);

void SnapshotDirectoryTree(HWND hWnd);
void SnapshotFilesList(HWND hWnd);
void ClearSourceList(HWND hWnd);

#endif
