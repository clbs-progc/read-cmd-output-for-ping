#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include <stdbool.h> //For bool run = true
                     //Can be replaced with int run = 1 if you prefer not using the library


int main() {
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hStdOut, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
    HWND hwnd = GetConsoleWindow();
    LONG oldstyle = GetWindowLong(hwnd, GWL_STYLE);
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    style &= ~(WS_BORDER|WS_CAPTION|WS_THICKFRAME);
    COORD here = {0,0};
    DWORD written = 0;

    RECT rect;
    GetWindowRect(hwnd, &rect);
    int newWidth = (rect.right - rect.left);
    int newHeight = (rect.bottom - rect.top);

    // Ping command for Windows: -n 4 sends 4 packets
    // char command[] = "ping -n 4 8.8.8.8 -l 1";
    char command[] = "ping 8.8.8.8 -l 1 -t"; // -t sets an infinite loop
    char psBuffer[256];
    char *result =  NULL;
    int numberfin = 0;
    char screen[256];

    FILE *pPipe;
    if ((pPipe = _popen(command, "r")) == NULL) {
        return 1;
    }

    bool run = true;
    while(run){
        while (fgets(psBuffer, sizeof(psBuffer), pPipe) != NULL && !_kbhit()) {
            //printf("%s", psBuffer); // Print output from command
            if(strstr(psBuffer, "time=") != NULL)
            {
                result = strstr(psBuffer, "time=");
                sscanf(result, "time=%d", &numberfin);
                sprintf(screen, "PING: %ims", numberfin);
                WriteConsoleOutputCharacter(hStdOut, screen, 10, here, &written);
            }
        }
        int ch = _getch();
        switch(ch)
        {
            case 68:// F10
                SetWindowLong(hwnd, GWL_STYLE, style);
                SetWindowPos(hwnd, NULL, 50, 50, newWidth, newHeight-30, SWP_NOMOVE | SWP_NOZORDER);
                break;

            case 67: // F9
                SetWindowLong(hwnd, GWL_STYLE, oldstyle);
                SetWindowPos(hwnd, NULL, 50, 50, newWidth, newHeight+30, SWP_NOMOVE | SWP_NOZORDER);
                break;

            case 27: // ESC
                memset(screen, 0, sizeof(screen)); //Clears screen buffer
                run = false;
                sprintf(screen, "EXITING");
                system("taskkill -im PING.exe -f"); // Kills ping.exe to stop it from running once the program closes.
                WriteConsoleOutputCharacter(hStdOut, screen, 10, here, &written);
                break;

        }
    }

    sleep(2);
    FreeConsole();
    _pclose(pPipe);
    return 0;

}
