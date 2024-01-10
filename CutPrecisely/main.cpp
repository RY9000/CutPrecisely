
#pragma comment(lib,"Shlwapi.lib")
#pragma comment(lib,"Pathcch.lib")
#pragma comment(lib,"Shcore.lib")

#include "Windows.h"
#include "resource.h"
#include "Shlobj.h"
#include "Shlwapi.h"
#include "Pathcch.h"
#include "stdio.h"
#include "ShellScalingAPI.h"
#include "wchar.h"


INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
VOID CALLBACK WaitOrTimerCallback(PVOID   lpParameter, BOOLEAN TimerOrWaitFired);
VOID CALLBACK WaitOrTimerCallback2(PVOID   lpParameter, BOOLEAN TimerOrWaitFired);
HWND CreateToolTip(int toolID, HWND hDlg, PTSTR pszText);
void maketime(wchar_t*);
void DeleteFrames();


wchar_t yuanwenjian[MAX_PATH] = L"\0";
wchar_t yuanwenjianming[MAX_PATH] = L"\0";
wchar_t linshimulu[MAX_PATH] = L"\0";
wchar_t linshiwenjian[MAX_PATH] = L"\0";
wchar_t linshiwenjianming[MAX_PATH] = L"\0";
wchar_t shuchumulu[MAX_PATH] = L"\0";
wchar_t shuchuwenjian[MAX_PATH] = L"\0";
wchar_t shuchuwenjianming[MAX_PATH] = L"\0";
wchar_t ModulePath[MAX_PATH] = L"\0";
wchar_t ImagePath[MAX_PATH] = L"\0";
wchar_t DirOfFrames[MAX_PATH] = L"\0";


#define cmdlength 100000
wchar_t cmdline[cmdlength];


HINSTANCE g_hInstance;
HWND g_hwndDlg;
HANDLE hNewWaitObject;
HBITMAP h_checked;
HBITMAP h_indeterminate;
HBITMAP h_unchecked;


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE);
	GetModuleFileName(NULL, ModulePath, MAX_PATH);
	PathRemoveFileSpec(ModulePath);
	wcscat_s(ModulePath, MAX_PATH, L"\\ffmpeg.exe");
	g_hInstance = hInstance;
	h_checked = LoadBitmap(hInstance, (LPCTSTR)IDB_checked);
	h_indeterminate = LoadBitmap(hInstance, (LPCTSTR)IDB_indeterminate);
	h_unchecked = LoadBitmap(hInstance, (LPCTSTR)IDB_unchecked);
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, DialogProc);
	return 0;
}


INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	g_hwndDlg = hwndDlg;
	OPENFILENAME ofn;
	BROWSEINFO bi;
	static int hw2sta = BST_CHECKED;
	static RECT rc_input = { 0 }, rc_temp = { 0 }, rc_ss = { 0 }, rc_to = { 0 };

	switch (uMsg)
	{
	case WM_INITDIALOG:
		SendDlgItemMessage(hwndDlg, IDC_ICQ1, WM_SETTEXT, 0, (LPARAM)L"90");
		SendDlgItemMessage(hwndDlg, IDC_ICQ2, WM_SETTEXT, 0, (LPARAM)L"9");
		//SendDlgItemMessage(hwndDlg, IDC_HWACCEL1, BM_SETCHECK, BST_CHECKED, 0);
		SendDlgItemMessage(hwndDlg, IDC_RADIO1, BM_SETCHECK, BST_CHECKED, 0);
		SendDlgItemMessage(hwndDlg, IDC_SPIN1, UDM_SETRANGE32, 0, 1000000000);
		SendDlgItemMessage(hwndDlg, IDC_SPIN2, UDM_SETRANGE32, 0, 1000000000);
		CreateToolTip(IDC_EDIT_qishitime, hwndDlg, (PTSTR)L"HHMMSS");
		CreateToolTip(IDC_EDIT_jieshutime, hwndDlg, (PTSTR)L"HHMMSS");
		CreateToolTip(IDC_EDIT_qishitime2, hwndDlg, (PTSTR)L"HHMMSS.mmm");
		CreateToolTip(IDC_EDIT_jieshutime2, hwndDlg, (PTSTR)L"HHMMSS.mmm");
		CreateToolTip(IDC_zhenlv, hwndDlg, (PTSTR)L"Frame rate");
		CreateToolTip(IDC_qishizhen, hwndDlg, (PTSTR)L"Start from 0");
		CreateToolTip(IDC_ICQ1, hwndDlg, (PTSTR)L"The larger the better");
		CreateToolTip(IDC_ICQ2, hwndDlg, (PTSTR)L"The smaller the better");
		CreateToolTip(IDC_biansu, hwndDlg, (PTSTR)L"0.04; 0.1; [0.5, 2.0]; auto-reset");
		WINDOWINFO wi;
		ZeroMemory(&wi, sizeof(WINDOWINFO));
		wi.cbSize = sizeof(WINDOWINFO);
		GetWindowInfo(GetDlgItem(hwndDlg, IDC_HWACCEL2), &wi);
		wi.dwStyle = WS_CLIPCHILDREN | wi.dwStyle;
		SetWindowLong(GetDlgItem(hwndDlg, IDC_HWACCEL2), GWL_STYLE, wi.dwStyle);
		static HWND h_check;
		h_check = GetDlgItem(hwndDlg, IDC_check);
		SetParent(h_check, GetDlgItem(hwndDlg, IDC_HWACCEL2));
		POINT pt;
		GetWindowInfo(GetDlgItem(hwndDlg, IDC_EDIT_shuru), &wi);
		pt.x = wi.rcClient.left;
		pt.y = wi.rcClient.top;
		ScreenToClient(hwndDlg, &pt);
		rc_input.left = pt.x;
		rc_input.top = pt.y;
		rc_input.right = pt.x + wi.rcClient.right - wi.rcClient.left;
		rc_input.bottom = pt.y + wi.rcClient.bottom - wi.rcClient.top;
		GetWindowInfo(GetDlgItem(hwndDlg, IDC_EDIT_dierbushuru), &wi);
		pt.x = wi.rcClient.left;
		pt.y = wi.rcClient.top;
		ScreenToClient(hwndDlg, &pt);
		rc_temp.left = pt.x;
		rc_temp.top = pt.y;
		rc_temp.right = pt.x + wi.rcClient.right - wi.rcClient.left;
		rc_temp.bottom = pt.y + wi.rcClient.bottom - wi.rcClient.top;
		GetWindowInfo(GetDlgItem(hwndDlg, IDC_qishizhen), &wi);
		pt.x = wi.rcClient.left;
		pt.y = wi.rcClient.top;
		ScreenToClient(hwndDlg, &pt);
		rc_ss.left = pt.x;
		rc_ss.top = pt.y;
		rc_ss.right = pt.x + wi.rcClient.right - wi.rcClient.left;
		rc_ss.bottom = pt.y + wi.rcClient.bottom - wi.rcClient.top;
		GetWindowInfo(GetDlgItem(hwndDlg, IDC_jieshuzhen), &wi);
		pt.x = wi.rcClient.left;
		pt.y = wi.rcClient.top;
		ScreenToClient(hwndDlg, &pt);
		rc_to.left = pt.x;
		rc_to.top = pt.y;
		rc_to.right = pt.x + wi.rcClient.right - wi.rcClient.left;
		rc_to.bottom = pt.y + wi.rcClient.bottom - wi.rcClient.top;
		return true;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
			EndDialog(hwndDlg, wParam);
			return TRUE;
		case IDC_HWACCEL1:
			if (IsDlgButtonChecked(hwndDlg, IDC_HWACCEL1))
				EnableWindow(GetDlgItem(hwndDlg, IDC_ICQ1), TRUE);
			else
				EnableWindow(GetDlgItem(hwndDlg, IDC_ICQ1), FALSE);
			return true;
		case IDC_HWACCEL2:
			if ((BN_CLICKED == HIWORD(wParam)) || (BN_DBLCLK == HIWORD(wParam)))
			{
				switch (hw2sta)
				{
				case BST_CHECKED:
					hw2sta = BST_INDETERMINATE;
					SendMessage(h_check, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)h_indeterminate);
					return true;
				case BST_INDETERMINATE:
					hw2sta = BST_UNCHECKED;
					SendMessage(h_check, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)h_unchecked);
					EnableWindow(GetDlgItem(hwndDlg, IDC_ICQ2), FALSE);
					return true;
				case BST_UNCHECKED:
					hw2sta = BST_CHECKED;
					SendMessage(h_check, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)h_checked);
					EnableWindow(GetDlgItem(hwndDlg, IDC_ICQ2), TRUE);
					return true;
				}
			}
			return false;
		case IDC_RADIO1:
		case IDC_RADIO2:
			if (IsDlgButtonChecked(hwndDlg, IDC_RADIO1))
			{
				EnableWindow(GetDlgItem(hwndDlg, IDC_EDIT_jieshutime2), FALSE);
				EnableWindow(GetDlgItem(hwndDlg, IDC_EDIT_qishitime2), FALSE);
				EnableWindow(GetDlgItem(hwndDlg, IDC_jieshuzhen), TRUE);
				EnableWindow(GetDlgItem(hwndDlg, IDC_qishizhen), TRUE);
				EnableWindow(GetDlgItem(hwndDlg, IDC_zhenlv), TRUE);
			}
			else
			{
				EnableWindow(GetDlgItem(hwndDlg, IDC_EDIT_jieshutime2), TRUE);
				EnableWindow(GetDlgItem(hwndDlg, IDC_EDIT_qishitime2), TRUE);
				EnableWindow(GetDlgItem(hwndDlg, IDC_jieshuzhen), FALSE);
				EnableWindow(GetDlgItem(hwndDlg, IDC_qishizhen), FALSE);
				EnableWindow(GetDlgItem(hwndDlg, IDC_zhenlv), FALSE);
			}
			return true;
		case IDC_BUTTON_shuru:
		{
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hwndDlg;
			ofn.lpstrFile = yuanwenjian;
			ofn.lpstrFile[0] = L'\0';
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrFilter = L"All\0*.*\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_DONTADDTORECENT;
			if (GetOpenFileName(&ofn))
				SendDlgItemMessage(hwndDlg, IDC_EDIT_shuru, WM_SETTEXT, 0, (LPARAM)yuanwenjian);
			return TRUE;
		}
		case IDC_BUTTON_dierbushuru:
		{
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hwndDlg;
			ofn.lpstrFile = linshiwenjian;
			ofn.lpstrFile[0] = L'\0';
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrFilter = L"All\0*.*\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_DONTADDTORECENT;
			if (GetOpenFileName(&ofn))
				SendDlgItemMessage(hwndDlg, IDC_EDIT_dierbushuru, WM_SETTEXT, 0, (LPARAM)linshiwenjian);
			wcscpy_s(linshiwenjianming, MAX_PATH, PathFindFileName(linshiwenjian));
			return TRUE;
		}
		case IDC_BUTTON_linshi:
		{
			ZeroMemory(&bi, sizeof(bi));
			bi.hwndOwner = hwndDlg;
			bi.pidlRoot = NULL;
			bi.pszDisplayName = linshimulu;
			linshimulu[0] = L'\0';
			bi.lpszTitle = L"Temp dir:";
			bi.ulFlags = 0;
			bi.lpfn = NULL;
			bi.lParam = 0;
			LPITEMIDLIST lpDlist;
			lpDlist = SHBrowseForFolder(&bi);
			SHGetPathFromIDList(lpDlist, linshimulu);
			CoTaskMemFree(lpDlist);
			SendDlgItemMessage(hwndDlg, IDC_EDIT_linshi, WM_SETTEXT, 0, (LPARAM)linshimulu);
			return true;
		}
		case IDC_BUTTON_shuchu:
		{
			ZeroMemory(&bi, sizeof(bi));
			bi.hwndOwner = hwndDlg;
			bi.pidlRoot = NULL;
			bi.pszDisplayName = shuchumulu;
			shuchuwenjian[0] = L'\0';
			bi.lpszTitle = L"Output dir:";
			bi.ulFlags = 0;
			bi.lpfn = NULL;
			bi.lParam = 0;
			LPITEMIDLIST lpDlist;
			lpDlist = SHBrowseForFolder(&bi);
			SHGetPathFromIDList(lpDlist, shuchumulu);
			CoTaskMemFree(lpDlist);
			wcscpy_s(shuchuwenjian, MAX_PATH, shuchumulu);
			PathCchAddBackslash(shuchuwenjian, MAX_PATH);
			wcscpy_s(shuchuwenjianming, MAX_PATH, linshiwenjianming);
			PathCchRenameExtension(shuchuwenjianming, MAX_PATH, L".mp4");
			wcscat_s(shuchuwenjian, MAX_PATH, shuchuwenjianming);
			SendDlgItemMessage(hwndDlg, IDC_EDIT_shuchu, WM_SETTEXT, 0, (LPARAM)shuchuwenjian);
			return TRUE;
		}
		case IDC_BUTTON_step1:
		{
			yuanwenjian[0] = L'\0';
			SendDlgItemMessage(hwndDlg, IDC_EDIT_shuru, WM_GETTEXT, MAX_PATH, (LPARAM)yuanwenjian);
			if (yuanwenjian[0] == L'\0')
			{
				MessageBox(hwndDlg, L"There's no input.", NULL, MB_OK | MB_ICONERROR);
				return true;
			}
			wcscpy_s(yuanwenjianming, MAX_PATH, PathFindFileName(yuanwenjian));
			PathQuoteSpaces(yuanwenjian);
			/////////////
			SendDlgItemMessage(hwndDlg, IDC_EDIT_linshi, WM_GETTEXT, MAX_PATH, (LPARAM)linshimulu);
			if (linshimulu[0] == L'\0')
			{
				MessageBox(hwndDlg, L"Temp dir is null.", NULL, MB_OK | MB_ICONERROR);
				return true;
			}
			wcscpy_s(linshiwenjian, MAX_PATH, linshimulu);
			PathCchAddBackslash(linshiwenjian, MAX_PATH);
			wcscpy_s(linshiwenjianming, MAX_PATH, yuanwenjianming);
			PathRemoveExtension(linshiwenjianming);
			wcscat_s(linshiwenjianming, MAX_PATH, L"_");

			wchar_t yiqishi[15] = L"\0";
			SendDlgItemMessage(hwndDlg, IDC_EDIT_qishitime, WM_GETTEXT, 15, (LPARAM)yiqishi);
			wcscat_s(linshiwenjianming, MAX_PATH, yiqishi);
			wcscat_s(linshiwenjianming, MAX_PATH, L".mov");
			wcscat_s(linshiwenjian, MAX_PATH, linshiwenjianming);
			PathQuoteSpaces(linshiwenjian);
			wcscpy_s(cmdline, cmdlength, L" -y ");

			int IsHW1 = IsDlgButtonChecked(hwndDlg, IDC_HWACCEL1);
			if (IsHW1)
				wcscat_s(cmdline, cmdlength, L" -hwaccel  qsv ");


			if (yiqishi[0] != L'\0')
			{
				maketime(yiqishi);
				wcscat_s(cmdline, cmdlength, L" -ss ");
				wcscat_s(cmdline, cmdlength, yiqishi);
			}

			wchar_t yijieshu[15] = L"\0";
			SendDlgItemMessage(hwndDlg, IDC_EDIT_jieshutime, WM_GETTEXT, 15, (LPARAM)yijieshu);
			if (yijieshu[0] != L'\0')
			{
				maketime(yijieshu);
				wcscat_s(cmdline, cmdlength, L" -to ");
				wcscat_s(cmdline, cmdlength, yijieshu);
			}

			wcscat_s(cmdline, cmdlength, L" -i ");
			wcscat_s(cmdline, cmdlength, yuanwenjian);

			if (IsHW1)
			{
				wcscat_s(cmdline, cmdlength, L" -c:v  mjpeg_qsv -global_quality ");
				wchar_t icq[8] = L"\0";
				SendDlgItemMessage(hwndDlg, IDC_ICQ1, WM_GETTEXT, 8, (LPARAM)icq);
				if (icq[0] != L'\0')
					wcscat_s(cmdline, cmdlength, icq);
				else
					wcscat_s(cmdline, cmdlength, L" 90 ");
			}
			else
				wcscat_s(cmdline, cmdlength, L" -c:v  mjpeg  -qmin  1  -q:v  1 ");

			wcscat_s(cmdline, cmdlength, L" -c:a pcm_s16le ");
			wcscat_s(cmdline, cmdlength, linshiwenjian);
			////////////
			
			{
				wchar_t ErrMsg[200] = L"\0";
				DWORD errID;
				STARTUPINFO si;
				PROCESS_INFORMATION pi;
				ZeroMemory(&si, sizeof(si));
				ZeroMemory(&pi, sizeof(pi));
				GetStartupInfo(&si);
				si.cb = sizeof(si);
				if (!CreateProcess(ModulePath, cmdline, NULL, NULL, FALSE, ABOVE_NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi))
				{
					errID = GetLastError();
					swprintf_s(ErrMsg, 200, L"Error code: %d。", errID);
					FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
						NULL, errID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
						ErrMsg + wcslen(ErrMsg), 200 - wcslen(ErrMsg), NULL);
					MessageBox(hwndDlg, ErrMsg, NULL, MB_OK | MB_ICONERROR);
					return true;
				}
				EnableWindow(GetDlgItem(hwndDlg, IDC_BUTTON_step1), FALSE);
				RegisterWaitForSingleObject(&hNewWaitObject, pi.hProcess, WaitOrTimerCallback, hwndDlg,
					INFINITE, WT_EXECUTEDEFAULT | WT_EXECUTEONLYONCE);
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
			}
			return true;
		}
		case IDC_BUTTON_step2:
		{
			linshiwenjian[0] = L'\0';
			SendDlgItemMessage(hwndDlg, IDC_EDIT_dierbushuru, WM_GETTEXT, MAX_PATH,
				(LPARAM)linshiwenjian);
			if (linshiwenjian[0] == L'\0')
			{
				MessageBox(hwndDlg, L"There's no input.", NULL, MB_OK | MB_ICONERROR);
				return false;
			}

			PathQuoteSpaces(linshiwenjian);
			///////////////
			shuchuwenjian[0] = L'\0';
			SendDlgItemMessage(hwndDlg, IDC_EDIT_shuchu, WM_GETTEXT, MAX_PATH, (LPARAM)shuchuwenjian);
			if (shuchuwenjian[0] == L'\0')
			{
				MessageBox(hwndDlg, L"There's no output.", NULL, MB_OK | MB_ICONERROR);
				return false;
			}
			PathQuoteSpaces(shuchuwenjian);
			wcscpy_s(cmdline, cmdlength, L" -y ");

			if (BST_CHECKED == hw2sta)
				wcscat_s(cmdline, cmdlength, L" -hwaccel  qsv ");
			else if (BST_INDETERMINATE == hw2sta)
				wcscat_s(cmdline, cmdlength, L" -init_hw_device  qsv=hw  -filter_hw_device  hw ");

			wchar_t strzhenlv[9] = L"\0", erqishi[15] = L"\0", erjieshu[15] = L"\0";
			int qishizhen, jieshuzhen;
			double zhenlv = 0.0;
			if (IsDlgButtonChecked(hwndDlg, IDC_RADIO1))
			{
				SendDlgItemMessage(hwndDlg, IDC_zhenlv, WM_GETTEXT, 9, (LPARAM)strzhenlv);
				if (strzhenlv[0] != L'\0')
				{
					BOOL b;
					zhenlv = _wtof(strzhenlv);
					qishizhen = GetDlgItemInt(hwndDlg, IDC_qishizhen, &b, FALSE);
					if ((b == TRUE) && (qishizhen != 0))
						swprintf_s(cmdline + wcslen(cmdline), cmdlength - wcslen(cmdline),
							L" -ss %02d:%02d:%02d.%03d ",
							(int)(1000.0 / zhenlv * qishizhen / 1000 / 3600),
							((int)(1000.0 / zhenlv * qishizhen / 1000) % 3600) / 60,
							(int)(1000.0 / zhenlv * qishizhen / 1000) % 60,
							(int)(1000.0 / zhenlv * qishizhen) % 1000);

					jieshuzhen = GetDlgItemInt(hwndDlg, IDC_jieshuzhen, &b, FALSE);
					if ((b == TRUE) && (jieshuzhen != 0))
					{
						double jieshushijian = 1000.0 / zhenlv * (jieshuzhen + 1) - 1;
						swprintf_s(cmdline + wcslen(cmdline), cmdlength - wcslen(cmdline),
							L" -to %02d:%02d:%02d.%03d ", 
							(int)(jieshushijian / 1000 / 3600), ((int)(jieshushijian / 1000) % 3600) / 60, 
							(int)(jieshushijian / 1000) % 60, (int)jieshushijian % 1000);
					}
				}
			}
			else
			{
				SendDlgItemMessage(hwndDlg, IDC_EDIT_qishitime2, WM_GETTEXT, 15, (LPARAM)erqishi);
				if (erqishi[0] != L'\0')
				{
					maketime(erqishi);
					wcscat_s(cmdline, cmdlength, L" -ss ");
					wcscat_s(cmdline, cmdlength, erqishi);
				}
				SendDlgItemMessage(hwndDlg, IDC_EDIT_jieshutime2, WM_GETTEXT, 15, (LPARAM)erjieshu);
				if (erjieshu[0] != L'\0')
				{
					maketime(erjieshu);
					wcscat_s(cmdline, cmdlength, L" -to ");
					wcscat_s(cmdline, cmdlength, erjieshu);
				}
			}

			wcscat_s(cmdline, cmdlength, L" -i ");
			wcscat_s(cmdline, cmdlength, linshiwenjian);

			bool IsVF = false;

			if (BST_INDETERMINATE == hw2sta)
			{
				wcscat_s(cmdline, cmdlength, L" -vf  hwupload=extra_hw_frames=64,format=qsv");
				IsVF = true;
			}

			wchar_t speed[9] = L"\0";
			SendDlgItemMessage(hwndDlg, IDC_biansu, WM_GETTEXT, 8, (LPARAM)speed);
			if (speed[0] != L'\0')
			{
				if (!IsVF)
				{
					wcscat_s(cmdline, cmdlength, L" -vf  ");
				}
				else
				{
					wcscat_s(cmdline, cmdlength, L",");
				}

				if (wcscmp(speed, L"0.1")==0)
					swprintf_s(cmdline + wcslen(cmdline), cmdlength - wcslen(cmdline),
						L"setpts=%.5f*PTS -af atempo=0.5,atempo=0.5,atempo=0.5,atempo=0.8,volume=0 ",
						1.0 / _wtof(speed));
				else if(wcscmp(speed, L"0.04") == 0)
					swprintf_s(cmdline + wcslen(cmdline), cmdlength - wcslen(cmdline),
						L"setpts=%.5f*PTS -af atempo=0.5,atempo=0.5,atempo=0.5,atempo=0.5,atempo=0.64,volume=0 ",
						1.0 / _wtof(speed));
				else
					swprintf_s(cmdline + wcslen(cmdline), cmdlength - wcslen(cmdline),
						L"setpts=%.5f*PTS -af atempo=%ls ", 1.0 / _wtof(speed), speed);

				SendDlgItemMessage(hwndDlg, IDC_biansu, WM_SETTEXT, 0, (LPARAM)L"");
			}

			if (hw2sta)
			{
				wcscat_s(cmdline, cmdlength, L" -c:v  h264_qsv  -preset  veryslow  -global_quality  ");
				wchar_t icq2[8] = L"\0";
				SendDlgItemMessage(hwndDlg, IDC_ICQ2, WM_GETTEXT, 8, (LPARAM)icq2);
				if (icq2[0] != L'\0')
					wcscat_s(cmdline, cmdlength, icq2);
				else
					wcscat_s(cmdline, cmdlength, L" 9 ");
			}
			else
				wcscat_s(cmdline, cmdlength, L" -c:v  libx264  -preset  veryslow  -crf  17 ");

			wcscat_s(cmdline, cmdlength, L"  -c:a  aac  -ar  48000  -ac  2  -ab  128k  ");
			wcscat_s(cmdline, cmdlength, shuchuwenjian);


			{
				wchar_t ErrMsg[200] = L"\0";
				DWORD errID;
				STARTUPINFO si;
				PROCESS_INFORMATION pi;
				ZeroMemory(&si, sizeof(si));
				ZeroMemory(&pi, sizeof(pi));
				GetStartupInfo(&si);
				si.cb = sizeof(si);
				if (!CreateProcess(ModulePath, cmdline, NULL, NULL, FALSE, ABOVE_NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi))
				{
					errID = GetLastError();
					swprintf_s(ErrMsg, 200, L"Error code: %d。", errID);
					FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
						NULL, errID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
						ErrMsg + wcslen(ErrMsg), 200 - wcslen(ErrMsg), NULL);
					MessageBox(hwndDlg, ErrMsg, NULL, MB_OK | MB_ICONERROR);
					return true;
				}
				EnableWindow(GetDlgItem(hwndDlg, IDC_BUTTON_step2), FALSE);
				RegisterWaitForSingleObject(&hNewWaitObject, pi.hProcess, WaitOrTimerCallback2,
					(PVOID)(long)zhenlv, INFINITE, WT_EXECUTEDEFAULT | WT_EXECUTEONLYONCE);
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
			}
			return true;
		}
		case IDC_BUTTON_DeleteTemp:
		{
			if (IDYES == MessageBox(hwndDlg, L"Delete temp?", L"Delete temp",
				MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2))
			{
				PathUnquoteSpaces(linshiwenjian);
				DeleteFile(linshiwenjian);
				PathUnquoteSpaces(ImagePath);
				DeleteFile(ImagePath);
				if (DirOfFrames[0] != L'\0')
				{
					DeleteFrames();
					RemoveDirectory(DirOfFrames);
					DirOfFrames[0] = L'\0';
				}
			}
			return true;
		}
		case IDC_BUTTON_Rename:
		{
			static unsigned int filecounter = 0;
			wchar_t oldname[MAX_PATH] = L"\0", newname[MAX_PATH] = L"\0", fileextension[10] = L"\0";
			SendDlgItemMessage(hwndDlg, IDC_EDIT_shuchu, WM_GETTEXT, MAX_PATH, (LPARAM)oldname);
			wcscpy_s(newname, MAX_PATH, oldname);
			wcscpy_s(fileextension, 10, PathFindExtension(newname));
			PathRemoveFileSpec(newname);
			swprintf_s(newname, MAX_PATH, L"%ls%03d%ls", newname, filecounter, fileextension);
			if (_wrename(oldname, newname))
				MessageBox(hwndDlg, L"Failed.", NULL, MB_OK | MB_ICONERROR);
			else
			{
				filecounter++;
				SendDlgItemMessage(hwndDlg, IDC_BUTTON_Rename, WM_SETTEXT, 0, (LPARAM)L" Rename");
			}
			return true;
		}
		default:return false;
		}
	case WM_DROPFILES:
		POINT ptmouse;
		DragQueryPoint((HDROP)wParam, &ptmouse);
		if ((ptmouse.x > rc_input.left) && (ptmouse.x < rc_input.right) && (ptmouse.y > rc_input.top) && (ptmouse.y < rc_input.bottom))
		{
			DragQueryFile((HDROP)wParam, 0, yuanwenjian, MAX_PATH);
			DragFinish((HDROP)wParam);
			SendDlgItemMessage(hwndDlg, IDC_EDIT_shuru, WM_SETTEXT, 0, (LPARAM)yuanwenjian);
		}
		else if ((ptmouse.x > rc_temp.left) && (ptmouse.x < rc_temp.right) && (ptmouse.y > rc_temp.top) && (ptmouse.y < rc_temp.bottom))
		{
			DragQueryFile((HDROP)wParam, 0, linshiwenjian, MAX_PATH);
			DragFinish((HDROP)wParam);
			SendDlgItemMessage(hwndDlg, IDC_EDIT_dierbushuru, WM_SETTEXT, 0, (LPARAM)linshiwenjian);
			wcscpy_s(linshiwenjianming, MAX_PATH, PathFindFileName(linshiwenjian));
		}
		else if ((ptmouse.y > rc_ss.top) && (ptmouse.y < rc_ss.bottom))
		{
			wchar_t framepath[MAX_PATH];
			DragQueryFile((HDROP)wParam, 0, framepath, MAX_PATH);
			DragFinish((HDROP)wParam);
			PathRemoveExtension(framepath);
			if ((ptmouse.x > rc_ss.left) && (ptmouse.x < rc_ss.right))
				SendDlgItemMessage(hwndDlg, IDC_qishizhen, WM_SETTEXT, 0, (LPARAM)PathFindFileName(framepath));
			else if ((ptmouse.x > rc_to.left) && (ptmouse.x < rc_to.right))
				SendDlgItemMessage(hwndDlg, IDC_jieshuzhen, WM_SETTEXT, 0, (LPARAM)PathFindFileName(framepath));
		}
		return true;
	}
	return FALSE;
}

VOID CALLBACK WaitOrTimerCallback(PVOID lpParameter, BOOLEAN TimerOrWaitFired)
{
	UnregisterWait(hNewWaitObject);

	
	if (IsDlgButtonChecked((HWND)lpParameter, IDC_FRAMES))
	{
		wcscpy_s(DirOfFrames, MAX_PATH, linshiwenjian);
		PathUnquoteSpaces(DirOfFrames);
		PathRemoveFileSpec(DirOfFrames);
		wcscat_s(DirOfFrames, MAX_PATH, L"\\CutPrecisely_AllFrames");
		CreateDirectory(DirOfFrames, NULL);
		swprintf_s(cmdline, cmdlength, 
			L" -y -hwaccel qsv -an -i %ls -vsync 0 -c:v mjpeg_qsv -global_quality 18 -f image2 -start_number 0 \"%ls\\%%d.jpg\"",
			linshiwenjian, DirOfFrames);
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		ZeroMemory(&si, sizeof(si));
		ZeroMemory(&pi, sizeof(pi));
		GetStartupInfo(&si);
		si.cb = sizeof(si);
		CreateProcess(ModulePath, cmdline, NULL, NULL, FALSE, ABOVE_NORMAL_PRIORITY_CLASS, NULL, NULL,
			&si, &pi);
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		ShellExecute(NULL, L"open", DirOfFrames, NULL, NULL, SW_SHOWNORMAL);
	}

	EnableWindow(GetDlgItem((HWND)lpParameter, IDC_BUTTON_step1), true);
	PathUnquoteSpaces(linshiwenjian);
	SendDlgItemMessage((HWND)lpParameter, IDC_EDIT_dierbushuru, WM_SETTEXT, 0, (LPARAM)linshiwenjian);

	SendDlgItemMessage((HWND)lpParameter, IDC_EDIT_shuchu, WM_GETTEXT, 0, (LPARAM)shuchuwenjian);
	if (shuchuwenjian[0] == L'\0')
		return;
	wcscpy_s(shuchuwenjianming, MAX_PATH, linshiwenjianming);
	PathCchRenameExtension(shuchuwenjianming, MAX_PATH, L".mp4");
	PathUnquoteSpaces(shuchuwenjian);
	PathRemoveFileSpec(shuchuwenjian);
	PathCchAddBackslash(shuchuwenjian, MAX_PATH);
	wcscat_s(shuchuwenjian, MAX_PATH, shuchuwenjianming);
	SendDlgItemMessage((HWND)lpParameter, IDC_EDIT_shuchu, WM_SETTEXT, 0, (LPARAM)shuchuwenjian);
	return;
}

VOID CALLBACK WaitOrTimerCallback2(PVOID lpParameter, BOOLEAN TimerOrWaitFired)
{
	UnregisterWait(hNewWaitObject);
	SendDlgItemMessage(g_hwndDlg, IDC_BUTTON_Rename, WM_SETTEXT, 0, (LPARAM)L" Rename *");

	int lines = (((long)lpParameter) == 0) ? 30 : ((long)lpParameter);
	lines = lines / 6 + 1;
	wcscpy_s(ImagePath, MAX_PATH, shuchuwenjian);
	PathUnquoteSpaces(ImagePath);
	PathCchRenameExtension(ImagePath, MAX_PATH, L".jpg");
	swprintf_s(cmdline, cmdlength,
		L" -y -an -t 0.5 -i %ls -sseof -0.5 -i %ls -filter_complex concat=n=2,tile=6x%d:padding=20:margin=20 -vsync 0 -f image2 \"%ls\"",
		shuchuwenjian, shuchuwenjian, lines, ImagePath);
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	GetStartupInfo(&si);
	si.cb = sizeof(si);
	CreateProcess(ModulePath, cmdline, NULL, NULL, FALSE, ABOVE_NORMAL_PRIORITY_CLASS, NULL, NULL,
		&si, &pi);
	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	EnableWindow(GetDlgItem(g_hwndDlg, IDC_BUTTON_step2), true);
	ShellExecute(NULL, L"open", ImagePath, NULL, NULL, SW_SHOWNORMAL);
	return;
}

void maketime(wchar_t* time)
{
	wchar_t wcmd[15] = L"\0";
	wchar_t* t = time, * p = t + wcslen(time);

	wcsncat_s(wcmd, 15, t, 2);
	t = t + 2;
	wcscat_s(wcmd, 15, L":");
	wcsncat_s(wcmd, 15, t, 2);
	t = t + 2;
	wcscat_s(wcmd, 15, L":");
	wcsncat_s(wcmd, 15, t, 2);
	t = t + 2;
	while ((*t) != L'\0')
	{
		wcsncat_s(wcmd, 15, t, 1);
		t++;
	}
	wcscpy_s(time, 15, wcmd);
	return;
}


HWND CreateToolTip(int toolID, HWND hDlg, PTSTR pszText)
{
	if (!toolID || !hDlg || !pszText)
	{
		return FALSE;
	}
	// Get the window of the tool.
	HWND hwndTool = GetDlgItem(hDlg, toolID);
	// Create the tooltip. g_hInst is the global instance handle.
	HWND hwndTip = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		hDlg, NULL, g_hInstance, NULL);
	if (!hwndTool || !hwndTip)
	{
		return (HWND)NULL;
	}
	// Associate the tooltip with the tool.
	TOOLINFO toolInfo = { 0 };
	toolInfo.cbSize = sizeof(toolInfo) - sizeof(void*);
	toolInfo.hwnd = hDlg;
	toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
	toolInfo.uId = (UINT_PTR)hwndTool;
	toolInfo.lpszText = pszText;
	SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);
	return hwndTip;
}

void DeleteFrames()
{
	if (DirOfFrames[0] != L'\0')
	{
		wchar_t p[MAX_PATH + 1];
		swprintf_s(p, MAX_PATH, L"%ls\\*", DirOfFrames);
		p[wcslen(p) + 1] = L'\0';
		SHFILEOPSTRUCTW FileOpS = { 0 };
		FileOpS.wFunc = FO_DELETE;
		FileOpS.pFrom = p;
		FileOpS.fFlags = FOF_NOCONFIRMATION;
		SHFileOperation(&FileOpS);
	}
	return;
}
