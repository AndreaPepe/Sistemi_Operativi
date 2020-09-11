// this program polls for event-messages on a window

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <malloc.h>

#define READ_BUFFER 4096

#define AUTO_KILLER

char *command1 = "command1";
char *command2 = "command2";
char *term = "term";

UINT command1_type = 0;
UINT command2_type = 0;
UINT term_type = 0;
UINT msg_type = 0;


HWND hWindow;

LRESULT CALLBACK WndProc(HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam) {

	if (message == WM_CREATE){
		printf("window creation ok\n");
		return 0;
	}

	if (message == command1_type){
		printf("requested command 1\n");
		return 1;
	}

	if (message == command2_type){
		printf("requested command 2\n");
		return 2;
	}

	if (message == term_type){
		printf("requested termination\n");
		PostQuitMessage(0);
		return 3;
	}

	printf("going for default  treatment\n");	
	return (DefWindowProc(hWindow, message, wParam, lParam));

}


DWORD WINAPI Killer(void * nothing) {

	int ret;
	UINT msg_type;
	char buff[READ_BUFFER];

	while (1){
		scanf("%s", buff);

		printf("trying to kill with '%s' event-message\n", buff);

		msg_type = RegisterWindowMessage(buff);
		if (!msg_type) {
			printf("Can't create '%s' event-message for error %u\n", buff, GetLastError());
			fflush(stdout);
			ExitProcess(-1);
		}
		else{
			printf("event-message '%s' correctly registered - code is %u\n", buff, msg_type);
		}

		ret = PostMessage(HWND_BROADCAST, msg_type, 0, 0);
		printf("event-message post returned %d\n", ret);
	}

}



void main(int argc, char *argv[])
{
	struct _thread_info * thread_info = NULL;

	WNDCLASS wndclass;
	char nome_applicazione[] = "test";
	int ret;
	MSG msg;

	term_type = RegisterWindowMessage(term);
	if (!term_type) {
		printf("Can't create term message for error %d\n", GetLastError());
		fflush(stdout);
		ExitProcess(-1);
	}
	else{
		printf("term event-message correctly registered - code is %u\n", term_type);
	}

	command1_type = RegisterWindowMessage(command1);
	if (!command1_type) {
		printf("Can't create command1 message for error %d\n", GetLastError());
		fflush(stdout);
		ExitProcess(-1);
	}
	else{
		printf("command1 event-message correctly registered - code is %u\n", command1_type);
	}

	command2_type = RegisterWindowMessage(command2);
	if (!command1_type) {
		printf("Can't create command2 message for error %d\n", GetLastError());
		fflush(stdout);
		ExitProcess(-1);
	}
	else{
		printf("command2 event-message correctly registered - code is %u\n", command2_type);
	}

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = NULL;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadIcon(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = nome_applicazione;

	if (!RegisterClass(&wndclass)) {
		printf("Can't register class"); fflush(stdout);
		ExitProcess(-1);
	}

	hWindow = CreateWindow(nome_applicazione,
		"Test su messaggi-evento",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		NULL,
		NULL);

	if (hWindow == INVALID_HANDLE_VALUE) {
		printf("Can't create window for error %d\n", GetLastError());
		fflush(stdout);
		ExitProcess(-1);
	}

#ifdef AUTO_KILLER
	if ((CreateThread(NULL, 0, Killer, NULL, 0, NULL)) == INVALID_HANDLE_VALUE) {
		printf("Can't start event-message listening thread\n");
		fflush(stdout);
		ExitProcess(-1);
	}
#endif

	printf("start polling on main tread\n");
	while (ret = GetMessage(&msg, NULL, 0, 0)) {
		if (ret == -1){
			printf("event-message poll error\n");
		}
		printf("got event-message while polling - type is %u\n", msg.message);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	ExitProcess(0);


}


