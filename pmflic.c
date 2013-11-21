//////////////////////////////////////////////////////////////////////////////
// This is the source file for the PMFLIC demo program.
//
// Copyright 1991 - Voyager Technologies, Inc.
//////////////////////////////////////////////////////////////////////////////

#define INCL_PM 			/* Selectively include		*/
#define INCL_DOS
#define INCL_GPI
#include <os2.h>

#include <string.h>

//
// Exported functions from the animation class library
//
#include <fli_ex.h>

//
// Header file used in the example
//
#include <pmflic.h>

//
// Global variables
//
    HWND    hwndFlic;
    HAB     hab;
    HMQ     hmq;

VOID cdecl main(int argc,char *argv[]);
VOID WindowInitialization(VOID);
MRESULT EXPENTRY MyWindowProc (HWND hwnd,USHORT msg,MPARAM mp1,MPARAM mp2);
MRESULT EXPENTRY AboutDlgProc(HWND hwnd,USHORT msg,MPARAM mp1,MPARAM mp2);

VOID cdecl main(int argc,char *argv[])
{
    ULONG flCreate = FCF_TITLEBAR | FCF_SYSMENU | FCF_MENU | FCF_BORDER |
		     FCF_MINBUTTON | FCF_ICON | FCF_ACCELTABLE;
    QMSG    qmsg;
    RECTL   rectParent;
    SHORT   x,y;
    SIZEL   szBmp;
    CHAR    szFlic[200];
    HWND    hwndFrame;
    HWND    hwndClient;

    //
    // Ensure the correct number of arguments were entered
    //
    if (argc > 1)
      strcpy(szFlic,argv[1]);
    else
      //
      // Display the default animation
      //
      strcpy(szFlic,ID_VTI_FLICS);

    //
    // Append an extension if one wasn't found
    //
    if (!strchr(szFlic,'.'))
      strcat(szFlic,".FLI");

    //
    // Initialize PM
    //
    WindowInitialization();

    //
    // The message box has to be displayed after initialization
    //
    if (argc < 2)
      //
      // Display some help if no parameters were entered
      //
      WinMessageBox(HWND_DESKTOP,HWND_DESKTOP,
		    "Usage Is:\n    PMFLIC filename.fli\n","PMFLIC - 1.00",1,
		    MB_ENTER | MB_NOICON | MB_SYSTEMMODAL);

    hwndFrame = WinCreateStdWindow(HWND_DESKTOP,
				   WS_VISIBLE,
				   &flCreate,
				   "VTI-Sample",
				   "VTI - Animation Player",
				   0L,
				   (HMODULE)NULL,
				   ID_FLIC,
				   &hwndClient);

    if (hwndFrame == (HWND)NULL ||
        hwndClient == (HWND)NULL)
	return;

    //
    // Find the size of the desktop
    //
    WinQueryWindowRect(HWND_DESKTOP,(PRECTL)&rectParent);

    //
    // Initialize the size of the client area
    //
    szBmp.cx = 320;
    szBmp.cy = 200;

    //
    // Find the starting point of the client area
    //
    x = (SHORT)(rectParent.xRight - rectParent.xLeft) - (SHORT)szBmp.cx;
    x /= 2;

    y = (SHORT)(rectParent.yTop - rectParent.yBottom) - (SHORT)szBmp.cy;
    y /= 2;
    rectParent.yBottom = y;
    rectParent.xLeft   = x;
    rectParent.yTop    = y + szBmp.cy;
    rectParent.xRight  = x + szBmp.cx;

    //
    // Get the location of a frame that would contain that client area
    //
    WinCalcFrameRect(hwndFrame,(PRECTL)&rectParent,FALSE);

    //
    // Move, Size and Show the window
    //
    WinSetWindowPos(hwndFrame,(HWND)NULL,
		    (SHORT)rectParent.xLeft,(SHORT)rectParent.yBottom,
		    (SHORT)(rectParent.xRight - rectParent.xLeft),
		    (SHORT)(rectParent.yTop - rectParent.yBottom),
		    SWP_MOVE | SWP_SIZE | SWP_SHOW);

    //
    // Create the animation control
    //
    hwndFlic = WinCreateWindow(hwndClient,FLICBOX_CLASS,szFlic,
			       WS_VISIBLE,
			       0,0,320,200,hwndClient,HWND_TOP,
			       0,(PVOID)NULL,
			       (PVOID)NULL);

    WinSetFocus(HWND_DESKTOP,hwndFrame);

    while (WinGetMsg(hab,&qmsg,(HWND)NULL,0,0))
        WinDispatchMsg(hab,&qmsg);

    WinDestroyWindow(hwndFrame);        /* Tidy up...                   */
    WinDestroyMsgQueue(hmq);            /* and                          */
    WinTerminate(hab);			/* terminate the application	*/

    DosExit(EXIT_PROCESS,0);
}

VOID WindowInitialization()
{
    SIZEL   size;

    hab = WinInitialize((USHORT)NULL);
    hmq = WinCreateMsgQueue(hab, 0);

    //
    // Class used in the sample program
    //
    WinRegisterClass(hab,"VTI-Sample",MyWindowProc,
		     CS_SIZEREDRAW | CS_CLIPCHILDREN,0);

    //
    // This call to the animation class library registers the
    // animation class
    //
    FlicInit(hab);
}

MRESULT EXPENTRY MyWindowProc (HWND hwnd,USHORT msg,MPARAM mp1,MPARAM mp2)
{
    HPS         hps;
    RECTL	rc;
    CHAR	Text[81];

    switch (msg)
    {
      case WM_COMMAND:
	switch (SHORT1FROMMP(mp1))
	{
	  case IDM_ABOUT:

	    //
	    // This message tells the animation control to stop playing
	    //
	    WinSendMsg(hwndFlic,FLI_STOP,0l,0l);

	    //
	    // Display the about dialog box
	    //
	    WinDlgBox(HWND_DESKTOP, hwnd, (PFNWP)AboutDlgProc,
		      (HMODULE)NULL, IDM_ABOUT, NULL);

	    //
	    // Tell the animation control to resume animating
	    //
	    WinSendMsg(hwndFlic,FLI_START,0l,0l);

	    break;

	  default:
	    return WinDefWindowProc(hwnd,msg,mp1,mp2);
	}
	break;

      //
      // Cause termination
      //
      case WM_CLOSE:
	WinPostMsg(hwnd,WM_QUIT,0L,0L);
	break;

      default:
	return WinDefWindowProc( hwnd, msg, mp1, mp2 );
    }

    return FALSE;
}

//
// Simple dialog procedure, nothing special
//
MRESULT EXPENTRY AboutDlgProc(HWND hwnd,USHORT msg,MPARAM mp1,MPARAM mp2)
{
    switch (msg)
    {
      case WM_COMMAND :
        switch (COMMANDMSG(&msg)->cmd)
        {
            case DID_OK :
            case DID_CANCEL :
                 WinDismissDlg (hwnd, TRUE);
                 return FALSE;
        }

        break;
     }

     return WinDefDlgProc (hwnd,msg,mp1,mp2);
}
