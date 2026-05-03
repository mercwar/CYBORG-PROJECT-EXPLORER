/* ---------------------------------------------------------------------------
   AIFVS-ARTIFACT — FILE: BOX_DEBUG.h
   ROLE: Sovereign Debug Window + Log Interface
   LAW:  AVIS-2026 — BOX Debug Layer
   --------------------------------------------------------------------------- */

#ifndef BOX_DEBUG_H
#define BOX_DEBUG_H

#include <windows.h>
#include <wchar.h>

/* ---------------------------------------------------------------------------
   PUBLIC API
   --------------------------------------------------------------------------- */

/* Open the debug window (creates it if not already open) */
void BOX_Debug_Open(void);

/* Append text to the debug window and log file */
void BOX_Debug_Log(const wchar_t* msg);

/* Internal helper used by BOX_Debug_Log */
void BOX_Debug_COPY(const wchar_t* msg);

/* Initialize root log directory */
void BOX_SetRoot(void);

/* ---------------------------------------------------------------------------
   MACRO: BOX_DEBUG
   A convenience wrapper so you can call:
       BOX_DEBUG(L"message");
   or:
       BOX_DEBUG(L"Value: %d", x);
   --------------------------------------------------------------------------- */

#define BOX_DEBUG(fmt, ...) do {                     \
    wchar_t _buf[1024];                              \
    _buf[0] = L'\0';                                 \
    _snwprintf(_buf, 1024, fmt, __VA_ARGS__);        \
    BOX_Debug_Log(_buf);                             \
} while(0)

#endif /* BOX_DEBUG_H */
