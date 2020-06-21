#define WIN32_LEAN_AND_MEAN
#define WIN32_NO_STATUS
#include <stdbool.h>
#include <windows.h>
#include <winternl.h>
#include <stdio.h>
#include <stdlib.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>

// Minimal version of run.c that just uses the pipe method.
// The original is part of cygwin.
BOOL configure_startupinfo(STARTUPINFO* psi,
                           HANDLE* hpToChild,
                           HANDLE* hpFromChild,
                           HANDLE* hpToParent,
                           HANDLE* hpFromParent)
{
    SECURITY_ATTRIBUTES handle_attrs;
    HANDLE hpStdInput[2];
    HANDLE hpStdOutput[2];

    ZeroMemory (psi, sizeof (STARTUPINFO));
    psi->cb = sizeof (STARTUPINFO);
    psi->hStdInput   = GetStdHandle(STD_INPUT_HANDLE);
    psi->hStdOutput  = GetStdHandle(STD_OUTPUT_HANDLE);
    psi->hStdError   = GetStdHandle(STD_ERROR_HANDLE);
    psi->dwFlags     = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    psi->wShowWindow = SW_HIDE;

    handle_attrs.nLength = sizeof(SECURITY_ATTRIBUTES);
    handle_attrs.bInheritHandle = TRUE;
    handle_attrs.lpSecurityDescriptor = NULL;

    /* create a pipe for child's stdin.  Don't allow child to */
    /* inherit the write end of the pipe.                     */
    CreatePipe (&hpStdInput[0], &hpStdInput[1], &handle_attrs, 0);
    SetHandleInformation ( hpStdInput[1], HANDLE_FLAG_INHERIT, 0);

    /* create a pipe for child's stdout.  Don't allow child to */
    /* inherit the read end of the pipe.                       */
    CreatePipe (&hpStdOutput[0], &hpStdOutput[1], &handle_attrs, 0);
    SetHandleInformation ( hpStdOutput[0], HANDLE_FLAG_INHERIT, 0);

    psi->hStdInput   = hpStdInput[0];
    psi->hStdOutput  = hpStdOutput[1];
    psi->hStdError   = hpStdOutput[1];

    *hpToChild   = hpStdInput[1];
    *hpFromChild = hpStdOutput[0];
    *hpToParent  = hpStdOutput[1];
    *hpFromParent= hpStdInput[0];

    return TRUE;
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
   STARTUPINFO start;
   PROCESS_INFORMATION child;
   DWORD retval;
   BOOL bFuncReturn;
   HANDLE hToChild, hFromChild;
   HANDLE hToParent, hFromParent;

   configure_startupinfo(&start,
                         &hToChild, &hFromChild,
                         &hToParent, &hFromParent);

   ZeroMemory( &child, sizeof(PROCESS_INFORMATION) );

   bFuncReturn = CreateProcess (NULL,
       lpCmdLine, /* command line                        */
       NULL,      /* process security attributes         */
       NULL,      /* primary thread security attributes  */
       TRUE,      /* handles are inherited,              */
       0,         /* creation flags                      */
       NULL,      /* use parent's environment            */
       NULL,      /* use parent's current directory      */
       &start,    /* STARTUPINFO pointer                 */
       &child);   /* receives PROCESS_INFORMATION        */

    CloseHandle(child.hProcess);
    CloseHandle(child.hThread);
    CloseHandle(hFromParent);
    CloseHandle(hToParent);
    CloseHandle(hFromChild);
    CloseHandle(hToChild);
    return 0;
}
