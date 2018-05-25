#include "NetVSTEditor.h"
#include "resource.h"
#include "NetVST.h"

// APPROXIMATE initial dialog size
#define EDITOR_WIDTH 400
#define EDITOR_HEIGHT 200

#define WINDOW_CLASS "NetVSTEd"

NetVSTEditor::NetVSTEditor (AudioEffect* effect)
    : AEffEditor(effect)
{
      WIDTH = EDITOR_WIDTH;
      HEIGHT = EDITOR_HEIGHT;

      myRect.top    = 0;
      myRect.left   = 0;
      myRect.bottom = HEIGHT;
      myRect.right  = WIDTH;

      effect->setEditor(this);
}

NetVSTEditor::~NetVSTEditor()
{
}

bool NetVSTEditor::open (void* ptr)
{ 
    systemWindow = ptr;
    hwnd = CreateDialog(GetInstance(), MAKEINTRESOURCE(IDD_DIALOG1), (HWND)ptr, dp);

    SetWindowLongPtr(hwnd, GWLP_USERDATA, (UINT)this);
    ShowWindow(hwnd, SW_SHOW);

    RECT rc;
    GetClientRect(hwnd, &rc);
    myRect.left = (VstInt16)rc.left;
    myRect.top = (VstInt16)rc.top;
    myRect.right = (VstInt16)rc.right;
    myRect.bottom = (VstInt16)rc.bottom;

    return true;
}

void NetVSTEditor::close ()
{
    DestroyWindow(hwnd);
    hwnd = 0;
    systemWindow = 0;
}

INT_PTR CALLBACK NetVSTEditor::dp(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    NetVSTEditor* instancePtr = (NetVSTEditor*)GetWindowLongPtr(hDlg, GWLP_USERDATA);
    if (instancePtr != NULL)
    {
        return instancePtr->instanceCallback(hDlg, message, wParam, lParam);
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK NetVSTEditor::instanceCallback(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

    NetVST *pVst = (NetVST*)getEffect();
    int ip1, ip2, ip3, ip4, portNum;
    char* pErr;

	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_REVERTBTN:
            UpdateAddrPortDisplay();
            return (INT_PTR)TRUE;
        case IDC_UPDATEBTN:
            pErr = ParseAddrPort(&ip1, &ip2, &ip3, &ip4, &portNum);
            if (pErr) SetDlgItemText(hwnd, IDC_MAINLABEL, pErr);
            else
            {
                getEffect()->setParameter(kOn, 0.0f);
                getEffect()->setParameter(kIP1, ip1 / 255.0f);
                getEffect()->setParameter(kIP2, ip2 / 255.0f);
                getEffect()->setParameter(kIP3, ip3 / 255.0f);
                getEffect()->setParameter(kIP4, ip4 / 255.0f);
                getEffect()->setParameter(kPort, (portNum - 27016) / 32.0f);
                UpdateAddrPortDisplay();
            }
            return (INT_PTR)TRUE;
        case IDC_NETCHECK:
            if (HIWORD(wParam) == BN_CLICKED)
            {
                float v = 0.0f;
                if (SendDlgItemMessage(hDlg, IDC_NETCHECK, BM_GETCHECK, 0, 0)) v = 1.0f;
                getEffect()->setParameter(kOn, v);
                return (INT_PTR)TRUE;
            }
        }
		break;
	}
	return (INT_PTR)FALSE;
}

void NetVSTEditor::UpdateAddrPortDisplay()
{
    NetVST *pVst = (NetVST*)getEffect();
    char msg[50];
    int ip1, ip2, ip3, ip4, portNum;

    ip1 = (int)(255 * pVst->getParameter(kIP1));
    ip2 = (int)(255 * pVst->getParameter(kIP2));
    ip3 = (int)(255 * pVst->getParameter(kIP3));
    ip4 = (int)(255 * pVst->getParameter(kIP4));
    portNum = (int)(27016 + 32 * pVst->getParameter(kPort));
    sprintf(msg, "Server: %d.%d.%d.%d:%d", ip1, ip2, ip3, ip4, portNum);
    SetDlgItemText(hwnd, IDC_MAINLABEL, msg);
}

void NetVSTEditor::UpdateNetworkOnDisplay()
{
    NetVST *pVst = (NetVST*)getEffect();
    bool on = pVst->getParameter(kOn) > 0.5f;
    SendDlgItemMessage(hwnd, IDC_NETCHECK, BM_SETCHECK, on ? BST_CHECKED : BST_UNCHECKED, 0);
}

static char badFormat[] = "Format must be ip1.ip2.ip3.ip4:port";
static char badIP[] = "IP numbers must be 0-255";
static char badPort[] = "Port number must be 27016-27031";
char* NetVSTEditor::ParseAddrPort(int* pIP1, int* pIP2, int* pIP3, int* pIP4, int* pPortNum)
{
    char str[50];
    int ip1, ip2, ip3, ip4, portNum;

    GetDlgItemText(hwnd, IDC_ADDRPORT, str, sizeof(str));
    if (sscanf(str, "%d.%d.%d.%d:%d", &ip1, &ip2, &ip3, &ip4, &portNum) != 5) return badFormat;
    if (ip1 < 0 || ip1 > 255 || ip2 < 0 || ip2 > 255 ||
        ip3 < 0 || ip3 > 255 || ip4 < 0 || ip4 > 255) return badIP;
    if (portNum < 27016 || portNum > 27031) return badPort;

    *pIP1 = ip1; *pIP2 = ip2; *pIP3 = ip3; *pIP4 = ip4; *pPortNum = portNum;
    return 0;
}

// Update the GUI after a parameter has been set in the NetFilter e.g. by automation
void NetVSTEditor::setParameter (VstInt32 index, float value)
{
    UpdateAddrPortDisplay();
    UpdateNetworkOnDisplay();
}
