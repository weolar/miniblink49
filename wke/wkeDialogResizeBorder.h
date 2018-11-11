// DialogResizeBorder.h
#ifndef DIALOG_RESIZE_BORDER
#define DIALOG_RESIZE_BORDER
#include <windows.h>
#include <map>

#define WM_BORDER_RESIZE_START WM_USER + 1000//WPARAM:BorderDialog, LPARAM:HitTestType  返回：S_OK生效
#define WM_BORDER_RESIZE_END WM_USER + 1001//WPARAM:BorderDialog, LPARAM:HitTestType  返回：S_OK生效
#define WM_BORDER_RESIZE WM_USER + 1002 //WPARAM:BorderDialog, LPARAM:HitTestType  返回：S_OK生效

#define WM_SYNCBORDER WM_USER + 2000//同步拖拉区域消息

#define GAP 6//拖拉区域大小

class CDialogResizeBorder
{
public:
	/*nHitTestType:
	HTLEFT
	HTRIGHT
	HTTOP
	HTTOPLEFT
	HTTOPRIGHT
	HTBOTTOM
	HTBOTTOMLEFT
	HTBOTTOMRIGHT
	*/
	CDialogResizeBorder(HINSTANCE hinst, HWND hwndOwner, int nHitTestType);

	~CDialogResizeBorder();

	virtual BOOL DoModeless();

	virtual void SyncBorder(bool bCheckShowed = true);

	inline LONG GetGap();

	HWND m_hWnd;

protected:
	// API中注册的消息处理函数，不能是成员函数，因为成员函数有this指针
	static INT_PTR CALLBACK DialogProc(HWND hWnd,
		UINT uMsg, WPARAM wParam, LPARAM lParam);

	static BOOL _ModifyStyle(HWND hWnd, int nStyleOffset,
		DWORD dwRemove, DWORD dwAdd, UINT nFlags);

	static BOOL ModifyStyleEx(HWND hWnd, DWORD dwRemove, DWORD dwAdd, UINT nFlags = 0);

protected:
	void syncBorder(bool bCheckShowed = true);
	void resizeWindow();

private:
	HWND  m_hOwnerWnd;
	HINSTANCE m_hInst;

	static std::map<HWND, HCURSOR> ms_mpCursors;
	int m_nHitTestType;
	static std::map<HWND, int> ms_mpHitTestType;
	static std::map<HWND, CDialogResizeBorder*> ms_wndClass;

	//不透明度，应为 1~255，一般为1（不能为0，否则不能接受鼠标事件），只有调试时设为其它
	static int ms_opaque;
};

#endif // DIALOG_RESIZE_BORDER
