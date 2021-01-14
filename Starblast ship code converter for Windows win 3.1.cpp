#include <windows.h>
#include <wtypes.h>
#include <winuser.h>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <locale>
#include <Commctrl.h>
#include <uxtheme.h>
#include <shellapi.h>

#define FILE_MENU_RESET 1
#define FILE_MENU_INPUT 2
#define FILE_MENU_OUTPUT 3
#define FILE_MENU_EXIT 4
#define HELP 5
#define ABOUT 6
#define COPY 7
#define PASTE 8
#define CONVERT 9
#define WEB 10
#define MAP 11
#define EDITOR 12
#define MODDING 13
#define DOCUMENT 14
#define DOCEDIT 15
#define STARWEB 16
#define CLIENT 17
#define SETTINGS_MENU_DARKMODE 18
#define SETTINGS_MENU_STRING_SINGLE 19
#define SETTINGS_MENU_STRING_DOUBLE 20
#define CHECK 21
#define LOG 22
#define BUG 23
#define SETTINGS_MENU_RESET 24
#define SETTINGS_MENU_VAL_NULL 25
#define SETTINGS_MENU_VAL_ZERO 26
#define SETTINGS_MENU_VAL_NONE 27
#define SETTINGS_MENU_BOOL_FALSE 28
#define SETTINGS_MENU_BOOL_ZERO 29
#define SETTINGS_MENU_BOOL_NONE 30

using namespace std;

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

void AddMenus(HWND);
void AddControls(HWND);
int horizontal;
int vertical;
wchar_t* text1=L"Starblast ship code converter for Windows";
wstring in=L"\0",out=L"\0";
int indent=39,bval=0,nval=0;

HMENU hMenu;
HWND hEdit,hDone,hConvert,hSave,hOpen,hCopy,hPaste,hReset,hDarkMode,hInput,hOutput;
string t,s;
void HideConsole()
{
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);
}
void RunCommand(string s)
{
	ShellExecute(0, 0, s.c_str(), 0, 0 , SW_HIDE );
}
void Export(char* path,int as)
{
	FILE *file;
	string test=path;
	if (as==1) {
		if (test.find(".txt",test.length()-4)!=test.length()-4) test=test+".txt";
	}
	file=fopen(test.c_str(),"w");
	char *data=new char[GetWindowTextLength(hDone)+1];
	GetWindowText(hDone,data,GetWindowTextLength(hDone)+1);
	fwrite(data,GetWindowTextLength(hDone)+1,1,file);
	fclose(file);
}
void Savefile(HWND hWnd)
{
	char filename[100];
	OPENFILENAME ofn;
	ZeroMemory(&ofn,sizeof(OPENFILENAME));
	ofn.lStructSize=sizeof(OPENFILENAME);
	ofn.hwndOwner=hWnd;
	ofn.lpstrFile=filename;
	ofn.lpstrFile[0]='\0';
	ofn.nMaxFile=100;
	ofn.lpstrFilter="Text Document (*.txt)\0*.TXT\0All files (*.*)\0*.*\0";
	ofn.nFilterIndex=1;
	GetSaveFileName(&ofn);


	Export(ofn.lpstrFile,ofn.nFilterIndex);
}
void DisplayFile(char* path,HWND hWnd)
{
	FILE * pFile;
	long lSize;
	char * buffer;
	size_t result;

	pFile = fopen ( path , "rb" );
	if (pFile==NULL)
	{
		MessageBox(hWnd,"File not exist","Error",MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	fseek (pFile , 0 , SEEK_END);
	lSize = ftell (pFile);
	rewind (pFile);

	// allocate memory to contain the whole file:
	buffer = (char*) malloc (sizeof(char)*lSize);
	if (buffer == NULL)
	{
		MessageBox(hWnd,"Memory Error","Error",MB_OK|MB_ICONEXCLAMATION);
		return;
	}

	// copy the file into the buffer:
	result = fread (buffer,1,lSize,pFile);
	if (result != lSize)
	{
		MessageBox(hWnd,"Cannot Read Input File","Error",MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	int i=0;
	string h=buffer;
	while (i<=lSize-1)
	{
		if (h[i]==char(10))
		{
			h.insert(i,"\r");
			i+=2;
			lSize++;
		}
		else i++;
	}
	for (i=0; i<=lSize-1; i++)
	{
		if (h[i]==char(10)) break;
	}
	SetWindowText(hEdit,h.c_str());
	if (i<lSize-1) MessageBox(hWnd,"File contains more than 1 line","File Input Warning",MB_OK|MB_ICONEXCLAMATION);
	// terminate
	fclose (pFile);
	free (buffer);
}
void convert(string t, string &s)
{
	s=t;
	int i;
	//Remove the var <name>_code string + typespec{} --- Completed
	for (i=0; i<=s.length()-1; i++)
	{
		if (s[i]==char(39) && s[i+1]=='{' && s[i+2]=='"') s.erase(0,i+2);
		if ((int(s[i])==116) && (int(s[i+1])==121) && (int(s[i+2])==112) && (int(s[i+3])==101) && (int(s[i+4])==115) && (int(s[i+5])==112)) s.erase(i-2,s.length()-1);
	}
	//Remove last symbols --- Completed
	bool b=0;
	i=s.length();
	while (!b)
	{
		i--;
		if (s[i]!='}') b=1;
	}
	s.erase(i+1,s.length()-1);
	//Set " and '
	for (i=0; i<=s.length()-1; i++)
	{
		if (s[i]=='"')
		{
			if (s[i+1]!='}' && s[i+1]!=',' && s[i-1]!=':') s.erase(i,1);
			else s[i]=char(indent);
		}
	}
	//Adjust Enter --- Completed
	i=1;
	while (i<=s.length()-1)
	{
		b=0;
		if (s[i]=='{')
		{
			if (s[i-1]==':')
			{
				s.insert(i,"\r\n");
				i+=3;
				b=1;
			}
		}
		if (s[i]=='}')
		{
			if (s[i-1]!='}')
			{
				s.insert(i,"\r\n");
				i+=3;
				b=1;
			}
		}
		if (s[i]==',')
		{
			switch(toupper(s[i+1]))
			{
				case 48:
				case 49:
				case 50:
				case 51:
				case 52:
				case 53:
				case 54:
				case 55:
				case 56:
				case 57:
				case 45:
					break;
				case 78:
					if (toupper(s[i+2])=='U' && toupper(s[i+3])=='L' && toupper(s[i+4])=='L') break;
					else {
						s.insert(i,"\r\n");
						i+=3;
						b=1;
					}
					break;
				default:
					if (s[i-1]!='}')
					{
						s.insert(i,"\r\n");
						i+=3;
						b=1;
					}
					else {
						s.erase(i,1);
						b=1;
					}
			}
		}
		if (s[i]==char(indent))
		{
			i++;
			while (s[i]!=char(indent)) i++;
		}
		if (!b) i++;
	}
	//Fixed model --- Completed
	s.insert(0,"  ");
	//Adjust space --- Completed
	int sp=2;
	i=0;
	while (i<=s.length()-1)
	{
		b=0;
		if (s[i]=='{')
		{
			b=1;
			sp+=2;
			s.erase(i,1);
			for (int j=1; j<=sp; j++)
			{
				s.insert(i," ");
				i++;
			}
		}
		if (s[i]=='}')
		{
			b=1;
			int j=0;
			while (s[i]=='}')
			{
				s.erase(i,1);
				j++;
			}
			sp-=j*2;
			for (int k=1; k<=sp; k++)
			{
				s.insert(i," ");
				i++;
			}
		}
		if (s[i]==',')
		{
			if (s[i-1]==char(10))
			{
				b=1;
				s.erase(i,1);
				for (int j=1; j<=sp; j++)
				{
					s.insert(i," ");
					i++;
				}
			}
			else i++;
		}
		if (s[i]==':')
		{
			if (s[i+1]!=char(10))
			{
				s.insert(i+1," ");
				i+=2;
			}
		}
		if (s[i]==char(indent))
		{
			i++;
			while (s[i]!=char(indent)) i++;
		}
		if (!b) i++;
	}
	//Finalize the code --- Completed
	s.insert(0,"return model =\r\n");
	//Some fucking Settings --- Completed
	switch (bval)
	{
		case 1:
			i=0;
			while (i<=s.length()-1) {
				if (toupper(s[i])=='T' && toupper(s[i+1])=='R' && toupper(s[i+2])=='U' && toupper(s[i+3])=='E') {
					s.erase(i+1,3);
					s[i]='1';
				}
				if (toupper(s[i])=='F' && toupper(s[i+1])=='A' && toupper(s[i+2])=='L' && toupper(s[i+3])=='S' && toupper(s[i+4])=='E') {
					s.erase(i+1,4);
					s[i]='0';
				}
				if (s[i]==char(indent))
				{
					i++;
					while (s[i]!=char(indent)) i++;
				}
				i++;
			}
			break;
		case 2:
			i=0;
			while (i<=s.length()-1) {
				if (s[i-1]==' ' && (s[i]=='0' || s[i]=='1')) {
					if (s[i]=='0') {
						s.erase(i,1);
						s.insert(i,"false");
						i+=4;
					}
					if (s[i]=='1') {
						s.erase(i,1);
						s.insert(i,"true");
						i+=3;
					}
				}
				if (s[i]==char(indent))
				{
					i++;
					while (s[i]!=char(indent)) i++;
				}
				i++;
			}
			break;
		default:
			break;
	}
	switch (nval)
	{
		case 1:
			i=0;
			while (i<=s.length()-1) {
				if (toupper(s[i])=='N' && toupper(s[i+1])=='U' && toupper(s[i+2])=='L' && toupper(s[i+3])=='L') {
					s.erase(i+1,3);
					s[i]='0';
				}
				if (s[i]==char(indent))
				{
					i++;
					while (s[i]!=char(indent)) i++;
				}
				i++;
			}
			break;
		case 2:
			i=0;
			while (i<=s.length()-1) {
				if (s[i-1]!=' ' && s[i]=='0') {
					if ((int(s[i+1])<48 || int(s[i+1])>57) && (int(s[i-1])<48 || int(s[i-1])>57)) {
						s.erase(i,1);
						s.insert(i,"null");
						i+=3;
					}
				}
				if (s[i]==char(indent))
				{
					i++;
					while (s[i]!=char(indent)) i++;
				}
				i++;
			}
		default:
			break;
	}
}
void toClipboard(string &s)
{
	OpenClipboard(0);
	EmptyClipboard();
	HGLOBAL hg=GlobalAlloc(GMEM_FIXED,s.size()+1);
	if (!hg)
	{
		CloseClipboard();
		return;
	}
	memcpy(GlobalLock(hg),s.c_str(),s.size());
	GlobalUnlock(hg);
	SetClipboardData(CF_TEXT,hg);
	CloseClipboard();
	GlobalFree(hg);
}
void getTaskBarSize(int &hori, int &verti)
{
	RECT rect;
	HWND taskBar = FindWindow("Shell_traywnd", NULL);
	if(taskBar && GetWindowRect(taskBar, &rect))
	{
		verti = rect.bottom - rect.top;
		hori = rect.right - rect.left;
	}
}
void GetDesktopResolution(int& horizontal, int& vertical)
{
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	horizontal = desktop.right;
	vertical = desktop.bottom;
	int hori,verti;
	getTaskBarSize(hori,verti);
	if (horizontal!=hori) horizontal-=hori;
	if (vertical!=verti) vertical-=verti;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPreVInst, LPSTR args, int ncmdshow)
{
	HWND hWnd;
	GetDesktopResolution(horizontal, vertical);
	WNDCLASSW wc = {0};
	wc.hbrBackground=CreateSolidBrush(RGB(240,240,240));
	wc.hCursor=LoadCursor(NULL, IDC_ARROW);
	wc.hInstance=hInst;
	wc.lpszClassName=L"MyClass";
	wc.lpfnWndProc=WindowProcedure;
	if(!RegisterClassW(&wc)) return -1;
	CreateWindowW(L"MyClass",text1,WS_OVERLAPPEDWINDOW|WS_VISIBLE|WS_MAXIMIZE, 0,0,horizontal,vertical,NULL,NULL,NULL,NULL);
	ShowWindow(hWnd,ncmdshow);
	MSG msg= {0};
	while(GetMessage(&msg,NULL,NULL,NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

void open_file(HWND hWnd)
{
	char filename[100];
	OPENFILENAME ofn;
	ZeroMemory(&ofn,sizeof(OPENFILENAME));
	ofn.lStructSize=sizeof(OPENFILENAME);
	ofn.hwndOwner=hWnd;
	ofn.lpstrFile=filename;
	ofn.lpstrFile[0]='\0';
	ofn.nMaxFile=100;
	ofn.lpstrFilter="Text Document\0*.txt\0All files\0*.*\0";
	ofn.nFilterIndex=1;
	if (!GetOpenFileName(&ofn)) return;
	else DisplayFile(ofn.lpstrFile,hWnd);
}
LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	int val;
	static COLORREF bkcolor = RGB(240,240,240);
	static HBRUSH hBrush = CreateSolidBrush(RGB(240,240,240));
	switch(msg)
	{		
		case WM_HOTKEY:
		case WM_COMMAND:
			if (GetActiveWindow() == hWnd)
			switch(wp)
			{
				case FILE_MENU_EXIT:
					val=MessageBoxW(hWnd,L"Are you sure?",L"Exit",MB_YESNO|MB_ICONEXCLAMATION);
					if (val==IDYES)
					{
						DestroyWindow(hWnd);
						::ShowWindow(::GetConsoleWindow(), SW_SHOW);
						exit(0);
					}
					break;
				case FILE_MENU_INPUT:
					open_file(hWnd);
					break;
				case FILE_MENU_OUTPUT:
					Savefile(hWnd);
					break;
				case FILE_MENU_RESET:
					SetWindowTextW(hEdit,L"Insert Modexport code from here...");
					in=L"\0",out=L"\0";
					SetWindowTextW(hDone,L"Ship Editor code will appear here...");
					break;
				case ABOUT:
					MessageBox(hWnd,"STARBLAST SHIP EDITOR CODE CONVERTER FOR WINDOWS\n                 Version 3.0.1 - by Bhpsngum", "About",MB_OK);
					break;
				case COPY:
				{
					char *data=new char[GetWindowTextLength(hDone)+1];
					if (GetWindowText(hDone,data,GetWindowTextLength(hDone)+1)!=0)
					{
						string f(data);
						toClipboard(f);
						SetWindowTextW(hCopy,L"Copied!");
						Sleep(500);
						SetWindowTextW(hCopy,L"Copy");
					}
					else MessageBox(hWnd,"Nothing to copy", "Information",MB_OK|MB_ICONINFORMATION);
				}
				break;
				case PASTE:
				{
					if(!IsClipboardFormatAvailable(CF_TEXT))
					{
						MessageBox(hWnd,"Unsupported data format in Clipboard", "Clipboard Warning",MB_OK|MB_ICONEXCLAMATION);
					}
					else
					{
						if (OpenClipboard(NULL))
						{
							HANDLE clip;
							clip = GetClipboardData(CF_TEXT); //Lock and Copy
							t = (LPSTR)GlobalLock(clip);   //Unlock
							GlobalUnlock(clip);
							CloseClipboard();
						}
						char a[t.length()];
						for (int i=0; i<=t.length()-1; i++) a[i]=t[i];
						SetWindowText(hEdit,a);
					}
				}
				break;
				case CONVERT:
				{
					char a[GetWindowTextLength(hEdit)+1];
					if (GetWindowTextA(hEdit,a,GetWindowTextLength(hEdit)+1)!=0)
					{
						convert(a,s);
						out = wstring(s.begin(), s.end());
						SetWindowTextW(hDone,out.c_str());
					}
					else MessageBox(hWnd,"Nothing to convert", "Information",MB_OK|MB_ICONINFORMATION);
				}
				break;
				case WEB:
					RunCommand("https://htmlpreview.github.io/?https://github.com/GatoCreador887/StarblastModding/blob/master/Starblast%20Ship%20Mod%20Export%20to%20Ship%20Script%20Converter.html");
					break;
				case MAP:
					RunCommand("https://htmlpreview.github.io/?https://github.com/GatoCreador887/StarblastModding/blob/master/Starblast%20Custom%20Map%20Editor.html");
					break;
				case EDITOR:
					RunCommand("https://starblast.io/shipeditor/");
					break;
				case MODDING:
					RunCommand("https://starblast.io/modding.html");
					break;
				case DOCUMENT:
					RunCommand("https://github.com/pmgl/starblast-modding");
					break;
				case DOCEDIT:
					RunCommand("https://starblastio.gamepedia.com/Ship_Editor_Tutorial");
					break;
				case STARWEB:
					RunCommand("https://starblast.io/#");
					break;
				case CLIENT:
					RunCommand("https://dankdmitron.github.io/");
					break;
				case LOG:
					RunCommand("https://raw.githubusercontent.com/Bhpsngum/StarblastConverter/master/Changelogs.txt");
					break;
				case SETTINGS_MENU_STRING_SINGLE:
					CheckMenuItem(hMenu,SETTINGS_MENU_STRING_SINGLE,MF_CHECKED);
					CheckMenuItem(hMenu,SETTINGS_MENU_STRING_DOUBLE,MF_UNCHECKED);
					indent=39;
					break;
				case SETTINGS_MENU_STRING_DOUBLE:
					CheckMenuItem(hMenu,SETTINGS_MENU_STRING_DOUBLE,MF_CHECKED);
					CheckMenuItem(hMenu,SETTINGS_MENU_STRING_SINGLE,MF_UNCHECKED);
					indent=34;
					break;
				case SETTINGS_MENU_DARKMODE:
				{
					DWORD check = CheckMenuItem(hMenu,SETTINGS_MENU_DARKMODE,NULL);
					if (check==MF_CHECKED)
					{
						CheckMenuItem(hMenu,SETTINGS_MENU_DARKMODE,MF_UNCHECKED);
						SetWindowTextW(hDarkMode,L"Darkmode: OFF");
						bkcolor = RGB(240, 240, 240);
        				InvalidateRect(hWnd, NULL, TRUE);
					}
					else
					{
						CheckMenuItem(hMenu,SETTINGS_MENU_DARKMODE,MF_CHECKED);
						SetWindowTextW(hDarkMode,L"Darkmode: ON");
						bkcolor = RGB(15, 15, 15);
        				InvalidateRect(hWnd, NULL, TRUE);
					}
				}
				break;
				case CHECK:
					RunCommand("https://github.com/Bhpsngum/StarblastConverter/");
					break;
				case HELP:
					//RunCommand("https://unknown");
					break;
				case BUG:
					RunCommand("https://github.com/Bhpsngum/StarblastConverter/issues");
				case SETTINGS_MENU_VAL_ZERO:
				{
					nval=1;
					CheckMenuItem(hMenu,SETTINGS_MENU_VAL_ZERO,MF_CHECKED);
					CheckMenuItem(hMenu,SETTINGS_MENU_VAL_NULL,MF_UNCHECKED);
					CheckMenuItem(hMenu,SETTINGS_MENU_VAL_NONE,MF_UNCHECKED);
				}
				break;
				case SETTINGS_MENU_VAL_NULL:
				{
					nval=2;
					CheckMenuItem(hMenu,SETTINGS_MENU_VAL_NULL,MF_CHECKED);
					CheckMenuItem(hMenu,SETTINGS_MENU_VAL_ZERO,MF_UNCHECKED);
					CheckMenuItem(hMenu,SETTINGS_MENU_VAL_NONE,MF_UNCHECKED);
				}
				break;
				case SETTINGS_MENU_VAL_NONE:
				{
					nval=0;
					CheckMenuItem(hMenu,SETTINGS_MENU_VAL_NONE,MF_CHECKED);
					CheckMenuItem(hMenu,SETTINGS_MENU_VAL_NULL,MF_UNCHECKED);
					CheckMenuItem(hMenu,SETTINGS_MENU_VAL_ZERO,MF_UNCHECKED);
				}
				break;
				case SETTINGS_MENU_BOOL_ZERO:
				{
					bval=1;
					CheckMenuItem(hMenu,SETTINGS_MENU_BOOL_ZERO,MF_CHECKED);
					CheckMenuItem(hMenu,SETTINGS_MENU_BOOL_FALSE,MF_UNCHECKED);
					CheckMenuItem(hMenu,SETTINGS_MENU_BOOL_NONE,MF_UNCHECKED);
				}
				break;
				case SETTINGS_MENU_BOOL_FALSE:
				{
					bval=2;
					CheckMenuItem(hMenu,SETTINGS_MENU_BOOL_FALSE,MF_CHECKED);
					CheckMenuItem(hMenu,SETTINGS_MENU_BOOL_ZERO,MF_UNCHECKED);
					CheckMenuItem(hMenu,SETTINGS_MENU_BOOL_NONE,MF_UNCHECKED);
				}
				break;
				case SETTINGS_MENU_BOOL_NONE:
				{
					bval=1;
					CheckMenuItem(hMenu,SETTINGS_MENU_BOOL_NONE,MF_CHECKED);
					CheckMenuItem(hMenu,SETTINGS_MENU_BOOL_FALSE,MF_UNCHECKED);
					CheckMenuItem(hMenu,SETTINGS_MENU_BOOL_ZERO,MF_UNCHECKED);
				}
				break;
				case SETTINGS_MENU_RESET:
				{
					val=MessageBoxW(hWnd,L"Reset to default settings?\nNote: Settings will be reset to default after you close the program",L"Default",MB_YESNO|MB_ICONEXCLAMATION);
					if (val==IDYES)
					{
						bval=0;
						nval=0;
						indent=39;
						CheckMenuItem(hMenu,SETTINGS_MENU_BOOL_NONE,MF_CHECKED);
						CheckMenuItem(hMenu,SETTINGS_MENU_BOOL_FALSE,MF_UNCHECKED);
						CheckMenuItem(hMenu,SETTINGS_MENU_BOOL_ZERO,MF_UNCHECKED);
						CheckMenuItem(hMenu,SETTINGS_MENU_VAL_NONE,MF_CHECKED);
						CheckMenuItem(hMenu,SETTINGS_MENU_VAL_NULL,MF_UNCHECKED);
						CheckMenuItem(hMenu,SETTINGS_MENU_VAL_ZERO,MF_UNCHECKED);
						CheckMenuItem(hMenu,SETTINGS_MENU_STRING_SINGLE,MF_CHECKED);
						CheckMenuItem(hMenu,SETTINGS_MENU_STRING_DOUBLE,MF_UNCHECKED);
					}
				}
				break;
			}
			break;
		case WM_DESTROY:
			DestroyWindow(hWnd);
			::ShowWindow(::GetConsoleWindow(), SW_SHOW);
			exit(0);
			break;
		case WM_CREATE:
			AddMenus(hWnd);
			AddControls(hWnd);
			break;
		case WM_PAINT:
		{
    		PAINTSTRUCT ps;
    		RECT rc;
    		HDC hdc = BeginPaint(hWnd, &ps);
    		GetClientRect(hWnd, &rc);
    		SetDCBrushColor(hdc, bkcolor);
    		FillRect(hdc, &rc, (HBRUSH)GetStockObject(DC_BRUSH));
		
    		//or use ps.rcPaint to repaint only the section which requires update
   			//FillRect(hdc, &ps.rcPaint, (HBRUSH)GetStockObject(DC_BRUSH));

    		EndPaint(hWnd, &ps);
    		return 0;
		}
		break;
		case WM_ERASEBKGND:
			return 0;
		case WM_CTLCOLORSTATIC:
		{
			DWORD CtrlID;
    		if (hInput == (HWND)lp)
			CtrlID = GetDlgCtrlID((HWND)lp); //Window Control ID
    		if (CtrlID == SETTINGS_MENU_DARKMODE) //If desired control
    		{
    			int color;
    			DWORD inst=CheckMenuItem(hMenu,SETTINGS_MENU_DARKMODE,NULL);
    			if (inst==MF_CHECKED) color=240;
    			else color=0;
       			HDC hdcStatic = (HDC) wp;
       			SetTextColor(hdcStatic, RGB(255-color,255-color,255-color));
       			SetBkColor(hdcStatic, RGB(color,color,color));
       			return (INT_PTR)hBrush;
    		}
		}
		break;
		default:
			return DefWindowProcW(hWnd,msg,wp,lp);
	}
}
void AddMenus(HWND hWnd)
{
	HMENU hHelpMenu=CreateMenu();
	HMENU hFileMenu=CreateMenu();
	HMENU hEditMenu=CreateMenu();
	HMENU hMoreMenu=CreateMenu();
	HMENU hSettingsMenu=CreateMenu();
	HMENU hString=CreateMenu();
	HMENU hFormat=CreateMenu();
	HMENU hNonVal=CreateMenu();
	HMENU hBoolVal=CreateMenu();
	hMenu=CreateMenu();
	AppendMenu(hFileMenu,MF_STRING,FILE_MENU_INPUT,"&Import file...\tCtrl+O");
	AppendMenu(hFileMenu,MF_STRING,FILE_MENU_OUTPUT,"&Export file...\tCtrl+S");
	AppendMenu(hFileMenu,MF_SEPARATOR,NULL,NULL);
	AppendMenu(hFileMenu,MF_STRING,FILE_MENU_EXIT,"&Quit\tAlt+F4");
	AppendMenu(hMenu,MF_POPUP,(UINT_PTR)hFileMenu,"&File");
	AppendMenu(hMenu,MF_POPUP,(UINT_PTR)hEditMenu,"&Edit");
	AppendMenu(hEditMenu,MF_STRING,FILE_MENU_RESET,"&Reset\tCtrl+R");
	AppendMenu(hEditMenu,MF_SEPARATOR,NULL,NULL);
	AppendMenu(hEditMenu,MF_STRING,PASTE,"&Insert from Clipboard\tAlt+V");
	AppendMenu(hEditMenu,MF_STRING,COPY,"&Copy Ship Editor code\tAlt+C");
	AppendMenu(hEditMenu,MF_SEPARATOR,NULL,NULL);
	AppendMenu(hEditMenu,MF_STRING,CONVERT,"&Convert\tCtrl+E");
	AppendMenu(hMenu,MF_POPUP,(UINT_PTR)hSettingsMenu,"&Settings");
	AppendMenu(hSettingsMenu,MF_POPUP,SETTINGS_MENU_DARKMODE,"&Dark Mode (Coming Soon)\tAlt+D");
	AppendMenu(hSettingsMenu,MF_SEPARATOR,NULL,NULL);
	AppendMenu(hSettingsMenu,MF_POPUP,(UINT_PTR)hFormat,"Convert Options");
	AppendMenu(hFormat,MF_POPUP,(UINT_PTR)hString,"String Separator");
	AppendMenu(hString,MF_STRING,SETTINGS_MENU_STRING_SINGLE,"Single Quote (') (Default)");
	CheckMenuItem(hMenu,SETTINGS_MENU_STRING_SINGLE,MF_CHECKED);
	AppendMenu(hString,MF_STRING,SETTINGS_MENU_STRING_DOUBLE,"Double Quote (\")");
	AppendMenu(hFormat,MF_POPUP,(UINT_PTR)hNonVal,"Non-value Format");
	AppendMenu(hNonVal,MF_STRING,SETTINGS_MENU_VAL_ZERO,"0");
	AppendMenu(hNonVal,MF_STRING,SETTINGS_MENU_VAL_NULL,"null");
	AppendMenu(hNonVal,MF_STRING,SETTINGS_MENU_VAL_NONE,"Keep Source Formatting");
	CheckMenuItem(hMenu,SETTINGS_MENU_VAL_NONE,MF_CHECKED);
	AppendMenu(hFormat,MF_POPUP,(UINT_PTR)hBoolVal,"Boolean Format");
	AppendMenu(hBoolVal,MF_STRING,SETTINGS_MENU_BOOL_ZERO,"1 - 0");
	AppendMenu(hBoolVal,MF_STRING,SETTINGS_MENU_BOOL_FALSE,"true - false");
	AppendMenu(hBoolVal,MF_STRING,SETTINGS_MENU_BOOL_NONE,"Keep Source Formatting");
	CheckMenuItem(hMenu,SETTINGS_MENU_BOOL_NONE,MF_CHECKED);
	AppendMenu(hFormat,MF_STRING,SETTINGS_MENU_RESET,"&Reset to Default Settings\tAlt+R");
	AppendMenu(hMenu,MF_POPUP,(UINT_PTR)hHelpMenu,"&Help");
	AppendMenu(hHelpMenu,MF_STRING,HELP,"&Tutorial\tF1");
	AppendMenu(hHelpMenu,MF_SEPARATOR,NULL,NULL);
	AppendMenu(hHelpMenu,MF_STRING,ABOUT,"About the Creator");
	AppendMenu(hHelpMenu,MF_SEPARATOR,NULL,NULL);
	AppendMenu(hHelpMenu,MF_STRING,CHECK,"Check for Updates");
	AppendMenu(hHelpMenu,MF_STRING,LOG,"&Changelogs\tCtrl+H");
	AppendMenu(hHelpMenu,MF_SEPARATOR,NULL,NULL);
	AppendMenu(hHelpMenu,MF_STRING,BUG,"Report Bug Issues");
	AppendMenu(hMenu,MF_POPUP,(UINT_PTR)hMoreMenu,"&More...");
	AppendMenu(hMoreMenu,MF_STRING,WEB,"Web version (by Money)");
	AppendMenu(hMoreMenu,MF_STRING,MAP,"Map Creator (by Money)");
	AppendMenu(hMoreMenu,MF_SEPARATOR,NULL,NULL);
	AppendMenu(hMoreMenu,MF_STRING,EDITOR,"Ship Editor");
	AppendMenu(hMoreMenu,MF_STRING,MODDING,"Starblast Modding");
	AppendMenu(hMoreMenu,MF_SEPARATOR,NULL,NULL);
	AppendMenu(hMoreMenu,MF_STRING,DOCUMENT,"Modding Documentation");
	AppendMenu(hMoreMenu,MF_STRING,DOCEDIT,"Ship Editor Tutorial");
	AppendMenu(hMoreMenu,MF_SEPARATOR,NULL,NULL);
	AppendMenu(hMoreMenu,MF_STRING,STARWEB,"Starblast Website");
	AppendMenu(hMoreMenu,MF_STRING,CLIENT,"Starblast Client (Web version)");
	SetMenu(hWnd, hMenu);
	RegisterHotKey(hWnd,SETTINGS_MENU_RESET,MOD_ALT,'R');
	RegisterHotKey(hWnd,FILE_MENU_RESET,MOD_CONTROL,'R');
	RegisterHotKey(hWnd,FILE_MENU_INPUT,MOD_CONTROL,'O');
	RegisterHotKey(hWnd,FILE_MENU_OUTPUT,MOD_CONTROL,'S');
	RegisterHotKey(hWnd,FILE_MENU_EXIT,MOD_ALT,0x73);
	RegisterHotKey(hWnd,PASTE,MOD_ALT,'V');
	RegisterHotKey(hWnd,COPY,MOD_ALT,'C');
	RegisterHotKey(hWnd,CONVERT,MOD_CONTROL,'E');
	RegisterHotKey(hWnd,SETTINGS_MENU_DARKMODE,MOD_ALT,'D');
	RegisterHotKey(hWnd,HELP,NULL,0x70);
	RegisterHotKey(hWnd,LOG,MOD_CONTROL,'H');
}

void AddControls(HWND hWnd)
{
	HideConsole();
	GetDesktopResolution(horizontal, vertical);
	vertical-=140;
	//SetWindowTheme(hWnd,L"",L"");
	hInput = CreateWindowW(L"static", L"INPUT", WS_VISIBLE|WS_CHILD|WS_EX_TRANSPARENT,0,0,horizontal,20,hWnd,NULL,NULL,NULL);
	hEdit = CreateWindowW(L"Edit", L"Insert Modexport code from here...", WS_VISIBLE|WS_CHILD|WS_VSCROLL|WS_HSCROLL|ES_MULTILINE|WS_BORDER,0,41,horizontal/2-1,vertical-41,hWnd,NULL,NULL,NULL);
	hReset = CreateWindowW(L"Button",L"Reset",WS_VISIBLE|WS_CHILD,71,0,70,20,hWnd,(HMENU)FILE_MENU_RESET,NULL,NULL);
	hOutput = CreateWindowW(L"static", L"OUTPUT", WS_VISIBLE|WS_CHILD,horizontal/2,0,horizontal,20,hWnd,NULL,NULL,NULL);
	hDone = CreateWindowW(L"Edit", L"Ship Editor code will appear here...",WS_VISIBLE|WS_CHILD|WS_VSCROLL|WS_HSCROLL|ES_MULTILINE|WS_BORDER,horizontal/2,41,horizontal/2-2,vertical-41,hWnd,NULL,NULL,NULL);
	hPaste = CreateWindowW(L"Button",L"Insert from Clipboard",WS_VISIBLE|WS_CHILD,1,20,horizontal/4-1,20,hWnd,(HMENU)PASTE,NULL,NULL);
	hOpen = CreateWindowW(L"Button",L"Browse File",WS_VISIBLE|WS_CHILD,(horizontal/4)+1,20,horizontal/4-1,20,hWnd,(HMENU)FILE_MENU_INPUT,NULL,NULL);
	hSave = CreateWindowW(L"Button",L"Export to...",WS_VISIBLE|WS_CHILD,(horizontal/2)+1,20,horizontal/4-1,20,hWnd,(HMENU)FILE_MENU_OUTPUT,NULL,NULL);
	hCopy = CreateWindowW(L"Button",L"Copy",WS_VISIBLE|WS_CHILD,(horizontal*3/4)+1,20,horizontal/4-1,20,hWnd,(HMENU)COPY,NULL,NULL);
	hConvert = CreateWindowW(L"Button",L"Convert",WS_VISIBLE|WS_CHILD,142,0,(horizontal/2-142),20,hWnd,(HMENU)CONVERT,NULL,NULL);
	hDarkMode = CreateWindowW(L"Button",L"Darkmode: OFF",WS_VISIBLE|WS_CHILD,horizontal-100,0,100,20,hWnd,(HMENU)SETTINGS_MENU_DARKMODE,NULL,NULL);
	char d[100];
	GetWindowText(hEdit,d,100);
	while (true) {
		if (d==NULL) MessageBox(hWnd,"Fuck","Nani??",MB_OK);
	}
	HFONT hButtonFont = CreateFont( 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Segoe UI");
	HFONT hEditFont = CreateFont( 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Consolas");
	SendMessage(hEdit, WM_SETFONT, (WPARAM)hEditFont, TRUE);
	SendMessage(hDone, WM_SETFONT, (WPARAM)hEditFont, TRUE);
	SendMessage(hReset, WM_SETFONT, (WPARAM)hButtonFont, TRUE);
	SendMessage(hCopy, WM_SETFONT, (WPARAM)hButtonFont, TRUE);
	SendMessage(hPaste, WM_SETFONT, (WPARAM)hButtonFont, TRUE);
	SendMessage(hConvert, WM_SETFONT, (WPARAM)hButtonFont, TRUE);
	SendMessage(hOpen, WM_SETFONT, (WPARAM)hButtonFont, TRUE);
	SendMessage(hSave, WM_SETFONT, (WPARAM)hButtonFont, TRUE);
	SendMessage(hDarkMode, WM_SETFONT, (WPARAM)hButtonFont, TRUE);
}
