#include <windows.h>
#include <commctrl.h>
#include <Commdlg.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <string.h>
#include <locale.h>
#define _WIN32_WINNT  0x0501
#define  WINVER       0x0501
#define _WIN32_IE     0x0600

#define CALL_EXPDLG   2000
#define BTN_GETFFile  2001
#define ITEMVIEW      2002
#define IDC_BUTTON1   2003
#define BTN_GETINFO   2004
#define NAMETREE      2005
#define IDC_GROUP1    2006
#define IDC_GROUP0    2007


#define USBSTOR "SYSTEM\\CurrentControlSet\\Enum\\USBSTOR"
#define DEVINTERFACE_DISK "SYSTEM\\CurrentControlSet\\Control\\DeviceClasses\\{53f56307-b6bf-11d0-94f2-00a0c91efb8b}"
#define DEVINTERFACE_VOLUME "SYSTEM\\CurrentControlSet\\Control\\DeviceClasses\\{53F5630D-B6BF-11D0-94F2-00A0C91EFB8B}"
//#define MAX_KEY_LENGTH 256
//#define MAX_VALUE      256
//#define MAX_VALUE_NAME 256

#define MAX_VALUE_NAME 1638
#define MAX_KEY_LENGTH 255
#define MAX_VALUE 16383

using namespace std;
struct instanceInfo;
struct usbInfo { /* usb structure info */
	TCHAR		cKeyName[MAX_KEY_LENGTH] ;
	TCHAR		cDeviceID[MAX_KEY_LENGTH] ;
	SYSTEMTIME	stStamp ; // should be creation time of first use
	struct instanceInfo * instance;
} ;

struct instanceInfo { /* each instance record from registry */
	TCHAR		cInstanceID[MAX_KEY_LENGTH] ;
	TCHAR		cFriendlyName[MAX_VALUE] ;
	TCHAR		cDriver[MAX_VALUE] ; // Diver Key - this maches
	TCHAR		cParentIdPrefix[MAX_VALUE] ;
	TCHAR		cHardwareID[MAX_VALUE] ;
	TCHAR		cLastDriveLetter[MAX_VALUE_NAME] ;
	SYSTEMTIME	stDiskStamp ; // Last write time of instance entry
	SYSTEMTIME  stVolumeStamp ; // in Device Class Key {53f56307-b6bf-11d0-94f2-00a0c91efb8b}
	struct instanceInfo * next ; // pointer to next instanceInfo if there is more than one
} ;

struct CurrIndex
{
 int index;
};




// global data.
HINSTANCE minstance;
HFONT h_font;
HWND hLV,hTree;
HTREEITEM Parent;
/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
char szClassName[ ] = "CodeBlocksWindowsApp";

int countSubKeys( const TCHAR hMainKey[] ) ;
void getDeviceList( struct usbInfo *, unsigned int ) ;
void cleanDeviceList( struct usbInfo *);
void ExportToCSV(struct usbInfo *, const char fPath[]);


void CreateWindowContent(HWND parent)
{
    setlocale(LC_ALL,"Ru");
	HWND wnd;
	LV_COLUMN col;
	col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	col.fmt = LVCFMT_LEFT;
	col.iSubItem = 0;
	wnd = CreateWindowEx(0x00000000, "Button", "Устройства:", 0x50020007, 0, 0, 915, 328, parent, (HMENU) IDC_GROUP0, minstance, NULL);
	SendMessage(wnd, WM_SETFONT, (WPARAM) h_font, TRUE);
	hLV = CreateWindowEx(WS_EX_STATICEDGE, "SysListView32", "", WS_CHILD | WS_VISIBLE   | LVS_REPORT   | WS_BORDER | WS_VSCROLL | WS_EX_RIGHTSCROLLBAR | WS_TABSTOP,230, 16, 680, 304, parent, (HMENU) ITEMVIEW, minstance, NULL); //192, 16, 680, 304
	SendMessage(hLV, WM_SETFONT, (WPARAM) h_font, TRUE);
	col.cx = 210;
	col.pszText ="ID экземпляра";
	SendMessage(hLV, LVM_INSERTCOLUMN, 0, (LPARAM) &col);
	col.cx = 95;
	col.pszText = "ID родителя";
	SendMessage(hLV, LVM_INSERTCOLUMN, 1, (LPARAM) &col);
	col.cx = 150;
	col.pszText = "Драйвер";
	SendMessage(hLV, LVM_INSERTCOLUMN, 2, (LPARAM) &col);
	col.cx = 110;
	col.pszText = "Дата диска"; //Disk Stamp
	SendMessage(hLV, LVM_INSERTCOLUMN, 3, (LPARAM) &col);
	col.cx = 110;
	col.pszText = "Дата раздела"; //Volume Stamp
	SendMessage(hLV, LVM_INSERTCOLUMN, 4, (LPARAM) &col);                                // width of column
	wnd = CreateWindowEx(0x00000000, "Button", "Управление:", 0x50020007, 0, 320, 209, 56, parent, (HMENU) IDC_GROUP1, minstance, NULL);
	SendMessage(wnd, WM_SETFONT, (WPARAM) h_font, TRUE);
	wnd = CreateWindowEx(WS_EX_STATICEDGE, "Button", "Получить", WS_CHILD | WS_VISIBLE, 8, 336, 96, 32, parent, (HMENU) BTN_GETINFO, minstance, NULL);
	SendMessage(wnd, WM_SETFONT, (WPARAM) h_font, TRUE);
	//wnd = CreateWindowEx(WS_EX_STATICEDGE, "Button", "Get info by LAN", WS_CHILD | WS_VISIBLE,104, 336, 96, 32 , parent, (HMENU) IDC_BUTTON1, minstance, NULL);
	//SendMessage(wnd, WM_SETFONT, (WPARAM) h_font, TRUE);
	wnd = CreateWindowEx(WS_EX_STATICEDGE, "Button", "Экспорт", WS_CHILD | WS_VISIBLE, 104, 336, 96, 32 , parent, (HMENU) CALL_EXPDLG, minstance, NULL);
	SendMessage(wnd, WM_SETFONT, (WPARAM) h_font, TRUE); //8, 16, 184, 304
	hTree = CreateWindowEx(WS_EX_STATICEDGE, "SysTreeView32", "", 0x50010000, 8, 16, 220, 304, parent, (HMENU) NAMETREE, minstance, NULL);
	SendMessage(hTree, WM_SETFONT, (WPARAM) h_font, TRUE);
/*  хотел добавить надпись, но не разобрался
	wnd = CreateWindowEx(0x00000000, "Button", "Управление2", 0x50020007, 320, 320, 209, 56, parent, (HMENU) IDC_GROUP1, minstance, NULL);
	SendMessage(wnd, WM_SETFONT, (WPARAM) h_font, TRUE);
	wnd = CreateWindowEx(WS_EX_STATICEDGE, "Button", "Get info by LAN", WS_CHILD | WS_VISIBLE,330, 340, 96, 32 , parent, (HMENU) IDC_BUTTON1, minstance, NULL);
	SendMessage(wnd, WM_SETFONT, (WPARAM) h_font, TRUE);*/
}

int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */
    InitCommonControls();
    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_PARENTDC |CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;
    minstance = hThisInstance;
    h_font = CreateFont(-13, 0, 0, 0, FW_NORMAL, 0,
				0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Times New Roman"); // ANSI_CHARSET

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           WS_EX_APPWINDOW,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           "Open Usb History Viewer от КонтинентСвободы.рф",       /* Title Text */
           WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           925, //885
           405,  //405
           NULL,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );
    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}

void newGradebookDialog( HWND hwnd, TCHAR file[] )
{
   OPENFILENAME ofn;

  TCHAR saveFileName[MAX_PATH] ="";

   ZeroMemory( &ofn, sizeof( ofn ) );

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = "CSV File (*.csv)\0*.csv\0";
    ofn.lpstrFile = saveFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrDefExt = "csv";
    ofn.Flags  = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
    ofn.lpstrTitle = "Экспорт в CSV";

 if(GetSaveFileName(&ofn))
  strcpy(file,saveFileName);
}

/*  This function is called by the Windows function DispatchMessage()  if(GetSaveFileName(&ofn)) */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
        case WM_COMMAND:
          switch(LOWORD(wParam))
           {
           case BTN_GETINFO:
             {
                int iSubKeyCount = countSubKeys( USBSTOR ) ;
               // struct usbInfo usb[iSubKeyCount];
                struct usbInfo deviceList[256] ;
              	struct instanceInfo * current ;
              	TVINSERTSTRUCT tvinsert;
                int TreeCount=TreeView_GetCount(GetDlgItem(hwnd,NAMETREE));

                if (TreeCount!=0)
                  {
                      for(int i=0;i<=TreeCount;i++)
                        TreeView_DeleteAllItems(GetDlgItem(hwnd,NAMETREE));
                  }

	            getDeviceList(deviceList,256) ;
	            Sleep(100);
                  for (int i = 1; i < iSubKeyCount; i++) {
	              current = deviceList[i].instance ;
	              tvinsert.hParent=NULL;
                  tvinsert.hInsertAfter=TVI_LAST;
                  tvinsert.item.mask=TVIF_TEXT|TVIF_PARAM;
                  tvinsert.item.lParam=i;
                  tvinsert.item.pszText=current->cFriendlyName;
                  tvinsert.item.iImage=0;
                  tvinsert.item.iSelectedImage=1;
                  Parent=(HTREEITEM)SendDlgItemMessage(hwnd,NAMETREE,
                            TVM_INSERTITEM,0,(LPARAM)&tvinsert);

	                 }
                cleanDeviceList(deviceList);
                }
               break;
            case CALL_EXPDLG:
			//MessageBox(hwnd, "the button IDC_BUTTON1 is clicked.", "Event", MB_OK|MB_ICONINFORMATION);
			{ char filename[] = "";
                     newGradebookDialog( hwnd, filename );
			if (strlen(filename)==0)
						return 0;
                     struct usbInfo recList[256];
                     getDeviceList(recList,256);
                     ExportToCSV(recList,filename);
                     cleanDeviceList(recList);
                     }
			break;
           }
            break;
        case WM_CREATE:
		   CreateWindowContent(hwnd);
		break;
        case WM_NOTIFY:
        case NAMETREE:
			if(((LPNMHDR)lParam)->code ==  NM_DBLCLK) // if code == NM_CLICK - Single click on an item
			{
			    int iItem;
			    int iSubKeyCount = countSubKeys( USBSTOR ) ;
			  //  HTREEITEM hTreeItem;
               // struct usbInfo usb[iSubKeyCount];
                struct usbInfo recList[256] ;
				TVITEM tvit;

                HTREEITEM hTreeItem = TreeView_GetSelection(hTree);


                tvit.hItem = hTreeItem;
                tvit.mask = TVIF_PARAM;
                TreeView_GetItem(hTree, &tvit);
               getDeviceList(recList,256);
               int j =tvit.lParam;
               struct instanceInfo * current = recList[j].instance;
              SendMessage(hLV,LVM_DELETEALLITEMS,0,0);
              	while (current !=NULL)
              	{
                     LVITEM LvItem;
                     memset(&LvItem,0,sizeof(LvItem));
                     char buff[256];
                     iItem=SendMessage(hLV,LVM_GETITEMCOUNT,0,0);
              	     LvItem.mask=LVIF_TEXT;   // Text Style
                     LvItem.iItem=iItem;          // choose item
                     sprintf(buff,TEXT("%s\n"), current->cInstanceID) ;
                     LvItem.pszText = buff;
                     ListView_InsertItem(hLV, &LvItem);


                    sprintf(LvItem.pszText,"%s\n", current->cParentIdPrefix) ;
                      // Text to display
                     LvItem.iSubItem=1;            // Put in coluom at index j
                     ListView_SetItem(hLV, &LvItem);

                    sprintf(LvItem.pszText,"%s\n", current->cDriver) ;
                    LvItem.iSubItem=2;
                    ListView_SetItem(hLV, &LvItem);
//sprintf(LvItem.pszText,TEXT("%02i/%02i/%04i %02i:%02i\n"), current->stDiskStamp.wMonth, current->stDiskStamp.wDay
                    sprintf(LvItem.pszText,TEXT("%02i/%02i/%04i %02i:%02i\n"), current->stDiskStamp.wDay, current->stDiskStamp.wMonth,
				    current->stDiskStamp.wYear, current->stDiskStamp.wHour, current->stDiskStamp.wMinute) ;

                    LvItem.iSubItem=3;
                    ListView_SetItem(hLV, &LvItem);

                    sprintf(LvItem.pszText,TEXT("%02i/%02i/%04i %02i:%02i\n"), current->stVolumeStamp.wDay, current->stVolumeStamp.wMonth,
					current->stVolumeStamp.wYear, current->stVolumeStamp.wHour, current->stVolumeStamp.wMinute) ;
                    LvItem.iSubItem=4;

                    ListView_SetItem(hLV, &LvItem);

              	    current = current->next;
              	}
              	cleanDeviceList(recList);
			}
			break;
		break;
        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

int countSubKeys( const TCHAR hMainKey[] )
{
	/*
	   This function counts the total subkeys of a given registry key in HKLM
	   we open a Registry Key, Query the Key for the number of subkeys and
	   then we close the key.
	*/

	HKEY 		hKey ;
	DWORD		dwSubKeys = 0 ;

	if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE, hMainKey, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS) {

		RegQueryInfoKey ( hKey, NULL, NULL, NULL, &dwSubKeys, NULL,
				NULL, NULL, NULL, NULL, NULL, NULL ) ;

		return (dwSubKeys) ;
	}

	RegCloseKey (hKey) ;

	return 0 ;
}

void getDeviceList( struct usbInfo * usbList, unsigned int iSubKeySize )
{

	HKEY 		hKey, hInstanceKey, hControlKey ;
	DWORD		dwName = MAX_KEY_LENGTH ;
	TCHAR		cSubKeyName[MAX_PATH +1] ;			 	// buffer for subkey name
	DWORD		dwSubKeys = 0 ;							// number of subkeys
	DWORD		dwValues = 0 ;							// number of Values
	DWORD		dwMaxValueNameLen = MAX_VALUE_NAME;		// Max Value Name Length
	DWORD		dwMaxValueLen = MAX_VALUE;				// Max Value Length
	FILETIME	ft ;									// last write time (file time structure)
	SYSTEMTIME	stGMT, stLocal ; // system time for readability
	DWORD 		ValType, ValNameLen, ValLen;
	LPTSTR 		ValName;
	LPBYTE 		Val;
	 unsigned int iSubKeyCount = 0, i, n, j;


	struct instanceInfo * prev, * current ;
	/*
	   Open USBSTOR, get info and SubKeys for each device
    */
  if( RegOpenKeyEx( HKEY_LOCAL_MACHINE, USBSTOR, 0, KEY_READ, &hKey) == ERROR_SUCCESS )
  {
    RegQueryInfoKey( hKey, NULL, NULL, NULL, &dwSubKeys, NULL, NULL, NULL, NULL, NULL, NULL, NULL );
    if( dwSubKeys > 0 )
    {
      for( i = 0; i < dwSubKeys; i++)
      {
        dwName = MAX_KEY_LENGTH;
        if( RegEnumKeyEx( hKey, i, cSubKeyName, &dwName, NULL, NULL, NULL, &ft ) == ERROR_SUCCESS )
        {
          TCHAR deviceKeyName[MAX_KEY_LENGTH] = USBSTOR;
          strcat(deviceKeyName, "\\");
          strcat(deviceKeyName, cSubKeyName);
          strcpy( usbList[i].cDeviceID, cSubKeyName);
          strncpy( usbList[i].cKeyName, deviceKeyName, MAX_KEY_LENGTH-1);
          FileTimeToSystemTime( &ft, &stGMT);
          SystemTimeToTzSpecificLocalTime( NULL, &stGMT, &stLocal);
          usbList[iSubKeyCount].stStamp.wMonth = stLocal.wMonth;
          usbList[iSubKeyCount].stStamp.wDay = stLocal.wDay;
          usbList[iSubKeyCount].stStamp.wYear = stLocal.wYear;
          usbList[iSubKeyCount].stStamp.wHour = stLocal.wHour;
          usbList[iSubKeyCount].stStamp.wMinute = stLocal.wMinute;
          usbList[iSubKeyCount].instance = NULL;
          if( ++iSubKeyCount >= iSubKeySize ) break;
        }
      }
    }
    RegCloseKey( hKey );
  }

  // now we move on to getting the instance info from registry
  for( i = 0; i < iSubKeyCount; i++)
  {
    TCHAR instanceIdKeyName[MAX_KEY_LENGTH];
    if( RegOpenKeyEx( HKEY_LOCAL_MACHINE, usbList[i].cKeyName, 0, KEY_READ, &hKey) == ERROR_SUCCESS )
    {
      RegQueryInfoKey( hKey, NULL, NULL, NULL, &dwSubKeys, NULL, NULL, NULL, NULL, NULL, NULL, NULL );
      if( dwSubKeys )
      {
        for( n = 0; n < dwSubKeys; n++)
        {
          dwName = MAX_KEY_LENGTH;
          if( RegEnumKeyEx( hKey, n, cSubKeyName, &dwName, NULL, NULL, NULL, NULL) == ERROR_SUCCESS )
          {
            current = (struct instanceInfo *) malloc(sizeof(struct instanceInfo));
            memset( current, 0, sizeof(struct instanceInfo));

            // fill out each instanceInfo
            strcpy( current->cInstanceID, cSubKeyName);
            strcpy( instanceIdKeyName, usbList[i].cKeyName);
            strcat( instanceIdKeyName, "\\");
            strcat( instanceIdKeyName, current->cInstanceID);

            // open each instanceID Registry Key to get the value data
            if( RegOpenKeyEx( hKey, cSubKeyName, 0, KEY_READ, &hInstanceKey) == ERROR_SUCCESS )
            {
              RegQueryInfoKey( hInstanceKey, NULL, NULL, NULL, NULL, NULL, NULL,
                                 &dwValues, &dwMaxValueNameLen, &dwMaxValueLen, NULL, NULL);
              if( dwValues )
              {
                ValName = (LPSTR) malloc( dwMaxValueNameLen+1 );
                Val = (LPBYTE) malloc( dwMaxValueLen );
                for( j = 0; j < dwValues; j++)
                {
                  ValNameLen = dwMaxValueNameLen + 1;
                  ValLen = dwMaxValueLen + 1;
                  RegEnumValue( hInstanceKey, j, ValName, &ValNameLen,
                                  NULL, &ValType, Val, &ValLen);
                  if( ValType == REG_SZ || ValType == REG_MULTI_SZ )
                  {
                    if( strcmp( "ParentIdPrefix", ValName) == 0 )
                      strcpy(current->cParentIdPrefix, (LPTSTR) Val);
                    if( strcmp( "FriendlyName", ValName) == 0 )
                      strcpy( current->cFriendlyName, (LPTSTR) Val);
                    if( strcmp( "HardwareID", ValName) == 0 )
                      strcpy( current->cHardwareID, (LPTSTR) Val);
                    if( strcmp( "Driver", ValName) == 0 )
                      strcpy( current->cDriver, (LPTSTR) Val);
                  }
                }
                free( ValName );
                free( Val );
              }
              RegCloseKey( hInstanceKey );
            }
            if( usbList[i].instance == NULL )
              usbList[i].instance = current;
            else
              prev->next = current;
            current->next = NULL ;
            prev = current;
          }
        }
      }
      RegCloseKey( hKey );
    }
  }
if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE, TEXT( DEVINTERFACE_DISK ), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {

		RegQueryInfoKey ( hKey, NULL, NULL, NULL, &dwSubKeys, NULL, NULL, NULL, NULL, NULL, NULL, NULL ) ;

		for (int i = 0; i < iSubKeyCount; i++) {
			current = usbList[i].instance ;

			if (dwSubKeys) {
				for (int i = 0; i < dwSubKeys; i++){
					dwName = MAX_KEY_LENGTH ; //we must reset this EVERY TIME!

					if ( RegEnumKeyEx(hKey, i, cSubKeyName, &dwName, NULL, NULL, NULL, &ft ) == ERROR_SUCCESS) {

						if ( strstr( cSubKeyName, current->cInstanceID) != NULL ) {

							/*
							   we have openned a key here that uses the ParentId Prefix. we should see if there is
						  	   a Control subkey. and if there is we should try to get the times from that. As they
							   update more often than the main key.
							*/

							TCHAR cControlKeyName[MAX_PATH +1] ;
							strcpy(cControlKeyName, cSubKeyName) ;
							strcat(cControlKeyName, TEXT("\\Control") ) ;

							if ( ( RegOpenKeyEx( hKey, cControlKeyName, 0, KEY_READ, &hControlKey) == ERROR_SUCCESS) &&
									( RegQueryInfoKey(hControlKey, NULL, NULL, NULL, NULL, NULL, NULL,
											NULL, NULL, NULL, NULL, &ft ) == ERROR_SUCCESS) ) {

								FileTimeToSystemTime( &ft, &stGMT) ;
								SystemTimeToTzSpecificLocalTime(NULL, &stGMT, &stLocal);
								current->stDiskStamp.wMonth = stLocal.wMonth ;
								current->stDiskStamp.wDay = stLocal.wDay ;
								current->stDiskStamp.wYear = stLocal.wYear ;
								current->stDiskStamp.wHour = stLocal.wHour ;
								current->stDiskStamp.wMinute = stLocal.wMinute ;

								RegCloseKey( hControlKey ) ;

							} else {

								FileTimeToSystemTime( &ft, &stGMT) ;
								SystemTimeToTzSpecificLocalTime(NULL, &stGMT, &stLocal);
								current->stDiskStamp.wMonth = stLocal.wMonth ;
								current->stDiskStamp.wDay = stLocal.wDay ;
								current->stDiskStamp.wYear = stLocal.wYear ;
								current->stDiskStamp.wHour = stLocal.wHour ;
								current->stDiskStamp.wMinute = stLocal.wMinute ;
							}
						}
					}
				}
			}

			while (current->next != NULL) {
				current = current->next ;

				if (dwSubKeys) {
					for (int i = 0; i < dwSubKeys; i++){
						dwName = MAX_KEY_LENGTH ; //we must reset this EVERY TIME!

						if ( RegEnumKeyEx(hKey, i, cSubKeyName, &dwName, NULL, NULL, NULL, &ft ) == ERROR_SUCCESS) {

							if ( strstr( cSubKeyName, current->cInstanceID) != NULL ) {
								/*
								   we have openned a key here that uses the ParentId Prefix. we should see if there is
								   a Control subkey. and if there is we should try to get the times from that. As they
								   update more often than the main key.
								*/

								TCHAR cControlKeyName[MAX_PATH +1] ;
								strcpy(cControlKeyName, cSubKeyName) ;
								strcat(cControlKeyName, TEXT("\\Control") ) ;

								if ( ( RegOpenKeyEx( hKey, cControlKeyName, 0, KEY_READ, &hControlKey) == ERROR_SUCCESS) &&
										( RegQueryInfoKey(hControlKey, NULL, NULL, NULL, NULL, NULL, NULL,
														  NULL, NULL, NULL, NULL, &ft ) == ERROR_SUCCESS) ) {
									FileTimeToSystemTime( &ft, &stGMT) ;
									SystemTimeToTzSpecificLocalTime(NULL, &stGMT, &stLocal);
									current->stDiskStamp.wMonth = stLocal.wMonth ;
									current->stDiskStamp.wDay = stLocal.wDay ;
									current->stDiskStamp.wYear = stLocal.wYear ;
									current->stDiskStamp.wHour = stLocal.wHour ;
									current->stDiskStamp.wMinute = stLocal.wMinute ;

									RegCloseKey( hControlKey ) ;

								} else {
									FileTimeToSystemTime( &ft, &stGMT) ;
									SystemTimeToTzSpecificLocalTime(NULL, &stGMT, &stLocal);
							current->stDiskStamp.wMonth = stLocal.wMonth ;
							current->stDiskStamp.wDay = stLocal.wDay ;
						    current->stDiskStamp.wYear = stLocal.wYear ;
							current->stDiskStamp.wHour = stLocal.wHour ;
							current->stDiskStamp.wMinute = stLocal.wMinute ;
								}
							}
						}
					}
				}
			}
		}
		RegCloseKey( hKey ) ;
	}
	if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE, TEXT( DEVINTERFACE_VOLUME ), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {

		RegQueryInfoKey ( hKey, NULL, NULL, NULL, &dwSubKeys, NULL, NULL, NULL, NULL, NULL, NULL, NULL ) ;

		for (int i = 0; i < iSubKeyCount; i++) {
			current = usbList[i].instance ;

			if (dwSubKeys) {
				for (int i = 0; i < dwSubKeys; i++){
					dwName = MAX_KEY_LENGTH ; //we must reset this EVERY TIME!

					if ( RegEnumKeyEx(hKey, i, cSubKeyName, &dwName, NULL, NULL, NULL, &ft ) == ERROR_SUCCESS) {

						if ( strstr( cSubKeyName, current->cParentIdPrefix) != NULL ) {
							/*
							   we have openned a key here that uses the ParentId Prefix. we should see if there is
						  	   a Control subkey. and if there is we should try to get the times from that. As they
							   update more often than the main key.
							*/

							TCHAR cControlKeyName[MAX_PATH +1] ;
							strcpy(cControlKeyName, cSubKeyName) ;
							strcat(cControlKeyName, TEXT("\\Control") ) ;

							if ( ( RegOpenKeyEx( hKey, cControlKeyName, 0, KEY_READ, &hControlKey) == ERROR_SUCCESS) &&
									( RegQueryInfoKey(hControlKey, NULL, NULL, NULL, NULL, NULL, NULL,
											NULL, NULL, NULL, NULL, &ft ) == ERROR_SUCCESS) ) {

								FileTimeToSystemTime( &ft, &stGMT) ;
								SystemTimeToTzSpecificLocalTime(NULL, &stGMT, &stLocal);
								current->stVolumeStamp.wMonth = stLocal.wMonth ;
								current->stVolumeStamp.wDay = stLocal.wDay ;
								current->stVolumeStamp.wYear = stLocal.wYear ;
								current->stVolumeStamp.wHour = stLocal.wHour ;
								current->stVolumeStamp.wMinute = stLocal.wMinute ;

								RegCloseKey( hControlKey ) ;

							} else {

							FileTimeToSystemTime( &ft, &stGMT) ;
							SystemTimeToTzSpecificLocalTime(NULL, &stGMT, &stLocal);
							current->stVolumeStamp.wMonth = stLocal.wMonth ;
							current->stVolumeStamp.wDay = stLocal.wDay ;
						    current->stVolumeStamp.wYear = stLocal.wYear ;
							current->stVolumeStamp.wHour = stLocal.wHour ;
							current->stVolumeStamp.wMinute = stLocal.wMinute ;

							}
						}
					}
				}
			}

			while (current->next != NULL) {

				current = current->next ;

				if (dwSubKeys) {
					for (int i = 0; i < dwSubKeys; i++){

						dwName = MAX_KEY_LENGTH ; //we must reset this EVERY TIME!

						if ( RegEnumKeyEx(hKey, i, cSubKeyName, &dwName, NULL, NULL, NULL, &ft ) == ERROR_SUCCESS) {

							if ( strstr( cSubKeyName, current->cParentIdPrefix) != NULL ) {
								/*
								   we have openned a key here that uses the ParentId Prefix. we should see if there is
								   a Control subkey. and if there is we should try to get the times from that. As they
								   update more often than the main key.
								*/

								TCHAR cControlKeyName[MAX_PATH +1] ;
								strcpy(cControlKeyName, cSubKeyName) ;
								strcat(cControlKeyName, TEXT("\\Control") ) ;

								if ( ( RegOpenKeyEx( hKey, cControlKeyName, 0, KEY_READ, &hControlKey) == ERROR_SUCCESS) &&
										( RegQueryInfoKey(hControlKey, NULL, NULL, NULL, NULL, NULL, NULL,
														  NULL, NULL, NULL, NULL, &ft ) == ERROR_SUCCESS) ) {
									FileTimeToSystemTime( &ft, &stGMT) ;
									SystemTimeToTzSpecificLocalTime(NULL, &stGMT, &stLocal);
									current->stVolumeStamp.wMonth = stLocal.wMonth ;
									current->stVolumeStamp.wDay = stLocal.wDay ;
									current->stVolumeStamp.wYear = stLocal.wYear ;
									current->stVolumeStamp.wHour = stLocal.wHour ;
									current->stVolumeStamp.wMinute = stLocal.wMinute ;

									RegCloseKey( hControlKey ) ;

								} else {
									FileTimeToSystemTime( &ft, &stGMT) ;
									SystemTimeToTzSpecificLocalTime(NULL, &stGMT, &stLocal);
									current->stVolumeStamp.wMonth = stLocal.wMonth ;
									current->stVolumeStamp.wDay = stLocal.wDay ;
									current->stVolumeStamp.wYear = stLocal.wYear ;
									current->stVolumeStamp.wHour = stLocal.wHour ;
									current->stVolumeStamp.wMinute = stLocal.wMinute ;
								}
							}
						}
					}
				}
			}
		}
		RegCloseKey( hKey ) ;
	}
	if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\MountedDevices"),
				0, KEY_READ, &hKey) == ERROR_SUCCESS) {

		RegQueryInfoKey(hKey, NULL, NULL, NULL, NULL, NULL, NULL,
				&dwValues, &dwMaxValueNameLen, &dwMaxValueLen, NULL, &ft ) ;
		ValName =reinterpret_cast<LPTSTR> (malloc (dwMaxValueNameLen+1)); // Allow for null.
		Val =reinterpret_cast<LPBYTE>(malloc (dwMaxValueLen)); // Size in bytes.

		FileTimeToSystemTime( &ft, &stGMT) ;
		SystemTimeToTzSpecificLocalTime(NULL, &stGMT, &stLocal);

		if (dwValues) {
			for (int v=0; v<dwValues; v++){

				ValNameLen = dwMaxValueNameLen ;
				ValLen = dwMaxValueLen ;
				char * valueString ;
				RegEnumValue (hKey, v, ValName, &ValNameLen, NULL, &ValType, Val, &ValLen);

				// eleminate older volume's and only check for recent DosDevices
				if (strstr(  (LPSTR)ValName, "\\DosDev") != NULL) {

					// build a usable string out of the REG_BINARY data in the registry by
					// skipping the white space in between each character.
					if (ValType == REG_BINARY)
						/*for (int j = 0; j < ValLen; j++, Val++)
							valueString[j] = Val[j] ;*/
							valueString = reinterpret_cast<char *>(Val);

				//	valueString[ValLen+1] = '\0' ; 	//append a null by to the end of our string
													//as a null character isn't guaranteed

					// We only want to look at removable media and ignore fixed devices
					if (strstr( valueString, "\\??\\STORAGE#RemovableMedia#") != NULL) {

						// Now cycle through each and see if hte ParentID is in valueString, if it is
						// then copy ValName to lastDriveLetter
						for (int i = 0; i < iSubKeyCount; i++) {

							current = usbList[i].instance ;

							if (  ( strstr( valueString, current->cParentIdPrefix) != NULL)
									&& (strlen(current->cParentIdPrefix) > 1 ) )
								strcpy(current->cLastDriveLetter, (LPTSTR) ValName) ;
							while (current->next != NULL) {
								current = current->next ;
								if ( ( strstr( valueString, current->cParentIdPrefix) != NULL )
										&& (strlen(current->cParentIdPrefix) > 1 ) )
									strcpy(current->cLastDriveLetter, (LPTSTR)ValName) ;
							}
						}
						// Done Cycling Through the list.
					}
				}
			}
		}

	free(Val) ; free(ValName) ;
	}



}

void ExportToCSV(struct usbInfo * usbList, const char fPath[])
{
    setlocale(LC_ALL,"Ru");
    int iSubKeyCount = countSubKeys( USBSTOR ) ;
    struct instanceInfo * current ;
	for (int i = 0; i < iSubKeyCount; i++) {
		current = usbList[i].instance ;
		while (current!=NULL){
		if (i==0)
		{
		   char dStamp[50];
           char volStamp[50];
           char buff[256];
          sprintf(buff,TEXT("%s"), current->cInstanceID) ;
          if (strlen(buff) ==NULL)
              strcpy(buff,"Not assigned!");
           char buff1[256];
          sprintf(buff1,TEXT("%s"), current->cParentIdPrefix) ;
          if (strlen(buff1) ==NULL)
              strcpy(buff1,"Not assigned!");
          char buff2[256];
          sprintf(buff2,TEXT("%s"), current->cDriver) ;
          if (strlen(buff2) ==NULL)
             strcpy(buff2,"Not assigned!");
		  sprintf(dStamp,TEXT("%02i/%02i/%04i %02i:%02i"), current->stDiskStamp.wMonth, current->stDiskStamp.wDay,
				current->stDiskStamp.wYear, current->stDiskStamp.wHour, current->stDiskStamp.wMinute) ;
		  sprintf(volStamp,TEXT("%02i/%02i/%04i %02i:%02i"), current->stVolumeStamp.wMonth, current->stVolumeStamp.wDay,
				current->stVolumeStamp.wYear, current->stVolumeStamp.wHour, current->stVolumeStamp.wMinute) ;
				FILE *fp = fopen(fPath,"w");
                if ( fp )
                   {
                     {
                       fprintf(fp,"%s;%s;%s;%s;%s;%s\n",current->cFriendlyName,buff,buff1,buff2,dStamp,volStamp);
                      }
                      fclose(fp);
                     }
                 current = current->next;

		}else
		{
		   char dStamp[50];
           char volStamp[50];
           char buff[256];
          sprintf(buff,TEXT("%s"), current->cInstanceID) ;
          if (strlen(buff) ==NULL)
              strcpy(buff,"Not assigned!");
           char buff1[256];
          sprintf(buff1,TEXT("%s"), current->cParentIdPrefix) ;
          if (strlen(buff1) ==NULL)
              strcpy(buff1,"Not assigned!");
          char buff2[256];
          sprintf(buff2,TEXT("%s"), current->cDriver) ;
          if (strlen(buff2) ==NULL)
              strcpy(buff2,"Not assigned!");
		  sprintf(dStamp,TEXT("%02i/%02i/%04i %02i:%02i"), current->stDiskStamp.wMonth, current->stDiskStamp.wDay,
				current->stDiskStamp.wYear, current->stDiskStamp.wHour, current->stDiskStamp.wMinute) ;
		  sprintf(volStamp,TEXT("%02i/%02i/%04i %02i:%02i"), current->stVolumeStamp.wMonth, current->stVolumeStamp.wDay,
				current->stVolumeStamp.wYear, current->stVolumeStamp.wHour, current->stVolumeStamp.wMinute) ;
				FILE *fp = fopen(fPath,"a+");
                if ( fp )
                   {
                     {
                       fprintf(fp,"%s;%s;%s;%s;%s;%s\n",current->cFriendlyName,buff,buff1,buff2,dStamp,volStamp);
                      }
                      fclose(fp);
                     }
		current = current->next;
		}
		}

}
}

void cleanDeviceList( struct usbInfo * usbList)
{
	int iSubKeyCount = countSubKeys( USBSTOR ) ;
	struct instanceInfo * next, * current ;

	for (int i = 0; i < iSubKeyCount; i++) {
		current = usbList[i].instance ;
		while (current != NULL) {
			next = current->next ;
			free(current) ;
			current = next ;
		}
	}

}



void FillHistory()
{

}

