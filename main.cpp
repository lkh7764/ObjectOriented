#include <iostream>
#include <cstdlib> // include rand()
#include <Windows.h>
#include <conio.h>
#include "GameObject.h"
#include "Utils.h"


// 1: 실행 안 되는 지 확인
// 2: 왜 안 되는지 이유 찾기

HANDLE hStdin;
DWORD fdwSaveOldMode;
int nDebugLine = 0;

VOID ErrorExit(LPCSTR);
VOID KeyEventProc(KEY_EVENT_RECORD);
VOID MouseEventProc(MOUSE_EVENT_RECORD);
VOID ResizeEventProc(WINDOW_BUFFER_SIZE_RECORD);

void log(const char* fmt, ...);

int main()
{
	GameObject::Initialize();
	bool exit_flag = false;

    DWORD cNumRead, fdwMode, i;
    INPUT_RECORD irInBuf[128];
    int counter = 0;


    // Get the standard input handle.

    hStdin = GetStdHandle(STD_INPUT_HANDLE);
    if (hStdin == INVALID_HANDLE_VALUE)
        ErrorExit("GetStdHandle");

    // Save the current input mode, to be restored on exit.

    if (!GetConsoleMode(hStdin, &fdwSaveOldMode))
        ErrorExit("GetConsoleMode");

    // Enable the window and mouse input events.

    fdwMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
    if (!SetConsoleMode(hStdin, fdwMode))
        ErrorExit("SetConsoleMode");

    // Loop to read and handle the next 100 input events.


	while (exit_flag == false)
	{
		GameObject::ClearFrame();
        cNumRead = 0;

        // Wait for the events.

        if (GetNumberOfConsoleInputEvents(hStdin, &cNumRead))
            ErrorExit("GetNumberOfConsoleInputEvents");

        if (cNumRead != 0) {
            if (!ReadConsoleInput(
                hStdin,      // input buffer handle
                irInBuf,     // buffer to read into
                128,         // size of read buffer
                &cNumRead)) // number of records read
                ErrorExit("ReadConsoleInput");
        }

        // Dispatch the events to the appropriate handler.

        for (i = 0; i < cNumRead; i++)
        {
            switch (irInBuf[i].EventType)
            {
            case KEY_EVENT: // keyboard input
                KeyEventProc(irInBuf[i].Event.KeyEvent);
                break;

            case MOUSE_EVENT: // mouse input
                MouseEventProc(irInBuf[i].Event.MouseEvent);
                break;

            case WINDOW_BUFFER_SIZE_EVENT: // scrn buf. resizing
                ResizeEventProc(irInBuf[i].Event.WindowBufferSizeEvent);
                break;

            case FOCUS_EVENT:  // disregard focus events

            case MENU_EVENT:   // disregard menu events
                break;

            default:
                ErrorExit("Unknown event type");
                break;
            }
        }

		//if (_kbhit()) GameObject::ProcessInput(exit_flag);

		GameObject::ExamineCollision();

		GameObject::UpdateAll();
		
		GameObject::DrawAll();

		GameObject::RenderFrame();
	}


    SetConsoleMode(hStdin, fdwSaveOldMode);
	GameObject::Deinitialize();

	return 0;
}


void log(const char* fmt, ...) {
    char dest[1024];
    va_list argPtr;

    va_start(argPtr, fmt);

    Borland::GotoXY(0, nDebugLine++ % 10 + 20);
    vsprintf (dest, fmt, argPtr);
    va_end(argPtr);
    //printf("[%5d: %5d] %s", nDebugLine, clock() - startTime, dest);
}

VOID ErrorExit(LPCSTR lpszMessage)
{
    log("%s", lpszMessage);

    // Restore input mode on exit.

    SetConsoleMode(hStdin, fdwSaveOldMode);

    ExitProcess(0);
}

VOID KeyEventProc(KEY_EVENT_RECORD ker)
{
    log("Key event: bkeyDown: %d, wRepeatCount: %d, keyCode: %d,  wScanCode: %d, uChar: %d, KeyState: %d",
        ker.bKeyDown, ker.wRepeatCount, ker.wVirtualKeyCode, ker.wVirtualScanCode, ker.uChar.AsciiChar, ker.dwControlKeyState);

    if (ker.bKeyDown)
        printf(" => key pressed\n");
    else printf(" => key released\n");
    printf("%100s", " ");
}

VOID MouseEventProc(MOUSE_EVENT_RECORD mer)
{
#ifndef MOUSE_HWHEELED
#define MOUSE_HWHEELED 0x0008
#endif
    log("Mouse event: (%d, %d), Button: %d, KeyState: %d, eventFlag: %d",
        mer.dwMousePosition.X, mer.dwMousePosition.Y, mer.dwButtonState, mer.dwControlKeyState, mer.dwEventFlags);

    switch (mer.dwEventFlags)
    {
    case 0:
        if (mer.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
        {
            printf(" => left button press");
        }
        else if (mer.dwButtonState == RIGHTMOST_BUTTON_PRESSED)
        {
            printf(" => right button press");
        }
        else
        {
            printf(" => button press");
        }
        break;
    case DOUBLE_CLICK:
        printf(" => double click");
        break;
    case MOUSE_HWHEELED:
        printf(" => horizontal mouse wheel");
        break;
    case MOUSE_MOVED:
        printf(" => mouse moved");
        break;
    case MOUSE_WHEELED:
        printf(" => vertical mouse wheel");
        break;
    default:
        printf(" => unknown");
        break;
    }
    printf("%100s", " ");
}

VOID ResizeEventProc(WINDOW_BUFFER_SIZE_RECORD wbsr)
{
    log("Resize event: Console screen buffer is %d columns by %d rows.", wbsr.dwSize.X, wbsr.dwSize.Y);
    printf("%100s", " ");
}

