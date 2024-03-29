﻿#include <windows.h>
#include <commctrl.h>
#include "resource.h"
#include <string>
#include <format>
#define _USE_MATH_DEFINES
#include <math.h>


INT_PTR CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    HWND hDlg;
    hDlg = CreateDialog(
        hInstance,
        MAKEINTRESOURCE(IDD_DIALOG1),
        NULL, // 親ウィンドウはなし
        DialogProc // ダイアログボックスプロシージャ
    );
    ShowWindow(hDlg, nCmdShow);
    InvalidateRect(hDlg, NULL, true);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        // ダイアログボックス上でのメッセージだけを仕分け
        if (!IsDialogMessage(hDlg, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (int)msg.wParam;
}

INT_PTR CALLBACK DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    static HFONT hFont;
    static wchar_t strTime[128];

    static HBRUSH bkColorBrush = CreateSolidBrush(RGB(240, 255, 240)); //背景色のブラシを用意

    int minFontSize = 60;
    int maxFontSize = 120;
    int fontSizeDelta = 10;

    static bool is24h = true;
    static std::wstring ampm[] = { L"AM", L"PM" };
    static HFONT hAmPmFont;
    static int currentFontSize;

    static bool isDigital = true;

    //ダイアログプロシージャ
    switch (uMsg)
    {
    case WM_CTLCOLORDLG:
        // WM_CTLCOLORDLG はシステムがダイアログ ボックスを描画する前に、ダイアログ ボックスに送信
        // ダイアログの背景色を設定
        return (LRESULT)bkColorBrush;
    case WM_CTLCOLORSTATIC: // STATICの背景色
        SetBkMode(((HDC)wParam), TRANSPARENT);
        SetTextColor(((HDC)wParam), RGB(200, 0, 0));//文字の色
        return (LRESULT)bkColorBrush;
    case WM_INITDIALOG: {
        // 1秒ごとにタイマー呼び出し
        SetTimer(hDlg, 1, 1000, NULL);

        // トラックバー（Slider Control）の初期化
        // 指定のダイアログボックス内のコントロールへメッセージを送る
        for (int size = minFontSize; size <= maxFontSize; size += fontSizeDelta) {
            SendDlgItemMessage(hDlg,
                IDC_COMBO1,
                CB_ADDSTRING,
                0,
                (LPARAM)std::format(L"{:d}", size).c_str());
        }

        // maxFontSizeを選択
        SendDlgItemMessage(hDlg,
            IDC_COMBO1,
            CB_SETCURSEL,
            (maxFontSize - minFontSize) / fontSizeDelta,
            0);


        SendDlgItemMessage(hDlg,
            IDC_COMBO2,
            CB_ADDSTRING,
            0,
            (LPARAM)L"12H表示");

        SendDlgItemMessage(hDlg,
            IDC_COMBO2,
            CB_ADDSTRING,
            0,
            (LPARAM)L"24H表示");

        // 24Hを選択
        SendDlgItemMessage(hDlg,
            IDC_COMBO2,
            CB_SETCURSEL,
            1,
            0);

        // ラジオボタンにチェックを入れる
        SendDlgItemMessage(hDlg, IDC_RADIO_DIGITAL, BM_SETCHECK, BST_CHECKED, 0);


        currentFontSize = maxFontSize;

        // フォントを作成する
        hFont = CreateFont(
            currentFontSize,          // フォントの高さ(大きさ)。
            0,                    // フォントの幅。0でデフォルト。
            0, 0,                 // 角度
            FW_DONTCARE,          // 文字の太さ
            FALSE,                // フォントがイタリックならTRUE
            FALSE,                // 下線を引くならTRUE
            FALSE,                // 取り消し線を引くならTRUE
            DEFAULT_CHARSET,      // フォントの文字セット
            OUT_DEFAULT_PRECIS,   // 出力精度の設定
            CLIP_DEFAULT_PRECIS,  // クリッピング精度
            DRAFT_QUALITY,        // フォントの出力品質
            DEFAULT_PITCH,        // フォントのピッチとファミリを指定
            L"ＭＳ Ｐゴシック" // フォント名
        );

        hAmPmFont = CreateFont(
            currentFontSize / 2,          // フォントの高さ(大きさ)。
            0,                    // フォントの幅。0でデフォルト。
            0, 0,                 // 角度
            FW_DONTCARE,          // 文字の太さ
            FALSE,                // フォントがイタリックならTRUE
            FALSE,                // 下線を引くならTRUE
            FALSE,                // 取り消し線を引くならTRUE
            DEFAULT_CHARSET,      // フォントの文字セット
            OUT_DEFAULT_PRECIS,   // 出力精度の設定
            CLIP_DEFAULT_PRECIS,  // クリッピング精度
            DRAFT_QUALITY,        // フォントの出力品質
            DEFAULT_PITCH,        // フォントのピッチとファミリを指定
            L"ＭＳ Ｐゴシック" // フォント名
        );
        return TRUE;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_RADIO_DIGITAL:
            if (HIWORD(wParam) == BN_CLICKED) {
                isDigital = true; 
            }
            return TRUE;
        case IDC_RADIO_ANALOG:
            if (HIWORD(wParam) == BN_CLICKED) {
                isDigital = false;
            }
            return TRUE;
        case IDC_COMBO1:
            if (HIWORD(wParam) == CBN_SELCHANGE) {
                int index = (int)SendDlgItemMessage(hDlg, IDC_COMBO1, CB_GETCURSEL, 0, 0);
                DeleteObject(hFont);
                currentFontSize = minFontSize + fontSizeDelta * index;
                hFont = CreateFont(
                    currentFontSize, // フォントの高さ(大きさ)。
                    0,
                    0, 0,
                    FW_DONTCARE,
                    FALSE,
                    FALSE,
                    FALSE,
                    DEFAULT_CHARSET,
                    OUT_DEFAULT_PRECIS,
                    CLIP_DEFAULT_PRECIS,
                    DRAFT_QUALITY,
                    DEFAULT_PITCH,
                    L"ＭＳ Ｐゴシック"
                );
                hAmPmFont = CreateFont(
                    currentFontSize / 2, // フォントの高さ(大きさ)。
                    0,
                    0, 0,
                    FW_DONTCARE,
                    FALSE,
                    FALSE,
                    FALSE,
                    DEFAULT_CHARSET,
                    OUT_DEFAULT_PRECIS,
                    CLIP_DEFAULT_PRECIS,
                    DRAFT_QUALITY,
                    DEFAULT_PITCH,
                    L"ＭＳ Ｐゴシック"
                );
            }
            return TRUE;
        case IDC_COMBO2:
            if (HIWORD(wParam) == CBN_SELCHANGE) {
                int index = (int)SendDlgItemMessage(hDlg, IDC_COMBO2, CB_GETCURSEL, 0, 0);
                if (index == 0) is24h = false;
                else is24h = true;
            }
            return TRUE;
        }
        return TRUE;
    case WM_TIMER:
        InvalidateRect(hDlg, NULL, true);
        return TRUE;
    case WM_PAINT: {
        SYSTEMTIME stTime;
        GetLocalTime(&stTime);

        if (isDigital) {
            hdc = BeginPaint(hDlg, &ps);       // 描画開始
            SelectObject(hdc, hFont);
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(180, 220, 180));

            RECT rc = { 10, 30, 450, 300 };
            if (is24h) {
                // 24時間表示の場合
                std::wstring strClock = std::format(L"{:02}:{:02}:{:02}", stTime.wHour, stTime.wMinute, stTime.wSecond);
                DrawText(hdc, strClock.c_str(), -1, &rc, DT_CENTER | DT_VCENTER);
            }
            else {
                // 12時間表示の場合
                div_t divHour = div(stTime.wHour, 12);
                // divHour.rem は stTime.wHourを12で割った余り（0から11）
                std::wstring strClock = std::format(L"{:02}:{:02}:{:02}", divHour.rem, stTime.wMinute, stTime.wSecond);
                DrawText(hdc, strClock.c_str(), -1, &rc, DT_CENTER | DT_VCENTER);

                // AM/PM表示。表示位置は時刻表示の下
                rc.top += currentFontSize;
                SelectObject(hdc, hAmPmFont);
                // divHour.quot は stTime.wHour を12で割った商（0か1）
                DrawText(hdc, ampm[divHour.quot].c_str(), -1, &rc, DT_CENTER | DT_VCENTER);
            }

            EndPaint(hDlg, &ps);              // 描画終了
        }
        else {
            hdc = BeginPaint(hDlg, &ps);
            int centerX = 235; // 時計の中心のX座標
            int centerY = 100;  // 時計の中心のY座標
            int radius = 90;  // 時計の半径
            int scaleLen = 7; // 目盛りの長さ
            // 目盛りを描画
            for (int i = 0; i < 12; i++) {
            double angle = 2 * M_PI / 12.0 * i;
                double fromX = (radius - scaleLen) * sin(angle);
                double fromY = (radius - scaleLen) * cos(angle);
                double toX = radius * sin(angle);
                double toY = radius * cos(angle);
                MoveToEx(hdc, centerX + fromX, centerY + fromY, NULL);
                LineTo(hdc, centerX + toX, centerY + toY);
            }
            // 短針を描画
            int shortLen = 40; // 短針の長さ
            int hour = stTime.wHour % 12;
            double toX = shortLen * sin(2 * M_PI / 12.0 * (hour + stTime.wMinute / 60.0));
            double toY = shortLen * cos(2 * M_PI / 12.0 * (hour + stTime.wMinute / 60.0));
            MoveToEx(hdc, centerX, centerY, NULL);
            LineTo(hdc, centerX + toX, centerY - toY);

            // 長針を描画
            int longLen = 80; // 短針の長さ
            toX = longLen * sin(2 * M_PI / 60.0 * stTime.wMinute);
            toY = longLen * cos(2 * M_PI / 60.0 * stTime.wMinute);
            MoveToEx(hdc, centerX, centerY, NULL);
            LineTo(hdc, centerX + toX, centerY - toY);

            EndPaint(hDlg, &ps);
        }
        return TRUE;
    }
    case WM_CLOSE:
        DestroyWindow(hDlg);
        return TRUE;
    case WM_DESTROY:
        DeleteObject(hFont);
        DeleteObject(hAmPmFont);
        PostQuitMessage(0);
        return TRUE;
    }
    return FALSE;
}