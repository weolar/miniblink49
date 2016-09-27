
class CHyperlink
{
public:
    static void create(HWND parent, const RECT& rect, const wchar_t* txt, int command)
    {
        WNDCLASS wc;

        wc.style = 0;
        wc.lpfnWndProc = (WNDPROC)CHyperlink::WndProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = sizeof(CHyperlink*);
        wc.hInstance = NULL;
        wc.hIcon = NULL;
        wc.hCursor = LoadCursor(NULL, IDC_HAND);
        wc.hbrBackground = NULL;
        wc.lpszMenuName = NULL;
        wc.lpszClassName = L"HyperLinkWindowClass";
        RegisterClass(&wc);

        HWND hWnd = CreateWindow(wc.lpszClassName, L"", WS_CHILD | WS_VISIBLE,
            rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
            parent, NULL, NULL, NULL);

        CHyperlink* hl = new CHyperlink;
        hl->setCommand(command);
        hl->setText(txt);

        SetWindowLong(hWnd, GWL_USERDATA, (LONG)hl);
    }

    static int WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        CHyperlink* hl = (CHyperlink*)GetWindowLong(hWnd, GWL_USERDATA);
        if (hl == NULL)
            return DefWindowProc(hWnd, uMsg, wParam, lParam);

        switch (uMsg)
        {
        case WM_LBUTTONDOWN:
            SendMessage(GetParent(GetParent(hWnd)), WM_COMMAND, hl->getCommand(), 0);
            EndDialog(GetParent(hWnd), IDOK);
            break;

        case WM_PAINT:
            {
                HDC hDC;
                PAINTSTRUCT ps;
                hDC = BeginPaint(hWnd, &ps);

                POINT pt;
                GetCursorPos(&pt);
                ScreenToClient(hWnd, &pt);

                RECT rect;
                GetClientRect(hWnd, &rect);

                if (PtInRect(&rect, pt))
                    SetTextColor(hDC, RGB(255, 0, 0));
                else
                    SetTextColor(hDC, RGB(0, 0, 255));

                SetBkMode(hDC, TRANSPARENT);

                DrawText(hDC, hl->getText(), wcslen(hl->getText()), &rect, DT_LEFT);

                EndPaint(hWnd, &ps);
            }
            break;

        case WM_DESTROY:
            delete hl;
            SetWindowLong(hWnd, GWL_USERDATA, 0);
            break;

        case WM_MOUSEMOVE:
            TRACKMOUSEEVENT tm;
            tm.cbSize = sizeof(tm);
            tm.hwndTrack = hWnd;
            tm.dwFlags = TME_HOVER|TME_LEAVE;
            tm.dwHoverTime = 10;
            TrackMouseEvent(&tm);
            break;

        case WM_MOUSEHOVER:
        case WM_MOUSELEAVE:
            InvalidateRect(hWnd, NULL, FALSE);
            break;

        default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
        }

        return 0;
    }

    const wchar_t* getText() const
    {
        return m_text;
    }

    void setText(const wchar_t* txt)
    {
        wcscpy_s(m_text, 1024, txt);
    }

    int getCommand() const
    {
        return m_command;
    }

    void setCommand(int command)
    {
        m_command = command;
    }

private:
    wchar_t m_text[1024];
    int m_command;
};