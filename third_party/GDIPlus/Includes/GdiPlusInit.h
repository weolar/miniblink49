/**************************************************************************
*
* Copyright (c) 2000 Microsoft Corporation
*
* Module Name:
*
*   Gdiplus init
*
* Abstract:
*
*   GDI+ startup/shutdown API's
*
* Created:
*
*   09/02/2000 agodfrey
*      Created it.
*
**************************************************************************/

#ifndef _GDIPLUSINIT_H
#define _GDIPLUSINIT_H

// Used for debug event notification (debug builds only)

enum DebugEventLevel
{
    DebugEventLevelFatal,
    DebugEventLevelWarning
};

// Callback function that GDI+ can call, on debug builds, for assertions
// and warnings.

typedef VOID (WINAPI *DebugEventProc)(DebugEventLevel level, CHAR *message);

// Notification functions which the user must call appropriately if
// "SuppressBackgroundThread" (below) is set.

typedef Status (WINAPI *NotificationHookProc)(OUT ULONG_PTR *token);
typedef VOID (WINAPI *NotificationUnhookProc)(ULONG_PTR token);

// Input structure for GdiplusStartup()

struct GdiplusStartupInput
{
    UINT32 GdiplusVersion;             // Must be 1
    DebugEventProc DebugEventCallback; // Ignored on free builds
    BOOL SuppressBackgroundThread;     // FALSE unless you're prepared to call 
                                       // the hook/unhook functions properly
    BOOL SuppressExternalCodecs;       // FALSE unless you want GDI+ only to use
                                       // its internal image codecs.
    
    GdiplusStartupInput(
        DebugEventProc debugEventCallback = NULL,
        BOOL suppressBackgroundThread = FALSE,
        BOOL suppressExternalCodecs = FALSE)
    {
        GdiplusVersion = 1;
        DebugEventCallback = debugEventCallback;
        SuppressBackgroundThread = suppressBackgroundThread;
        SuppressExternalCodecs = suppressExternalCodecs;
    }
};

// Output structure for GdiplusStartup()

struct GdiplusStartupOutput
{
    // The following 2 fields are NULL if SuppressBackgroundThread is FALSE.
    // Otherwise, they are functions which must be called appropriately to
    // replace the background thread.
    //
    // These should be called on the application's main message loop - i.e.
    // a message loop which is active for the lifetime of GDI+.
    // "NotificationHook" should be called before starting the loop,
    // and "NotificationUnhook" should be called after the loop ends.
    
    NotificationHookProc NotificationHook;
    NotificationUnhookProc NotificationUnhook;
};

// GDI+ initialization. Must be called before GDI+ API's are used.
//
// token  - may not be NULL - accepts a token to be passed in the corresponding
//          GdiplusShutdown call.
// input  - may not be NULL
// output - may be NULL only if input->SuppressBackgroundThread is FALSE.

extern "C" Status WINAPI GdiplusStartup(
    OUT ULONG_PTR *token,
    const GdiplusStartupInput *input,
    OUT GdiplusStartupOutput *output);

// GDI+ termination. Must be called before GDI+ is unloaded. GDI+ API's may not
// be called after this.

extern "C" VOID WINAPI GdiplusShutdown(ULONG_PTR token);

#endif
