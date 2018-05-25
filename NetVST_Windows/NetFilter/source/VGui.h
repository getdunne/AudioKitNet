#pragma once
#define WIN32_LEAN_AND_MEAN 1

#include <iostream>
#include "public.sdk/source/vst2.x/aeffeditor.h"
#include <windows.h>

extern void* hInstance;
inline HINSTANCE GetInstance() { return (HINSTANCE)hInstance; }

class VGui : public AEffEditor
{
private:
    ERect myRect;
    HWND hwnd;
    int WIDTH;
    int HEIGHT;

public:
   VGui(AudioEffect* effect, const int w, const int h) : AEffEditor(effect)
   {
      WIDTH = w;
      HEIGHT = h;

      myRect.top    = 0;
      myRect.left   = 0;
      myRect.bottom = HEIGHT;
      myRect.right  = WIDTH;

      effect->setEditor(this);
   }

   ~VGui() {}

   AudioEffect* getEffect () { return effect;}
   bool getRect (ERect** rect) { *rect = &myRect; return true;}
   
   bool isOpen () { return systemWindow != 0; }

   void idle () {}

   bool open (void* ptr)
   { 
       systemWindow = ptr;

       WNDCLASS wc;
       wc.style      = CS_GLOBALCLASS;
       wc.lpfnWndProc      = wp;
       wc.cbClsExtra      = 0;
       wc.cbWndExtra      = 0;
       wc.hInstance      = GetInstance();
       wc.hIcon      = 0;
       wc.hCursor      = LoadCursor (NULL, IDC_ARROW);
       wc.hbrBackground   = 0;
       wc.lpszMenuName      = 0;
       wc.lpszClassName   = "VGui";
       RegisterClass(&wc);

       hwnd = CreateWindow("VGui", 0, WS_CHILD, 0, 0, WIDTH, HEIGHT, (HWND)systemWindow, NULL, GetInstance(), NULL);
       SetWindowLong(hwnd, GWL_USERDATA, (UINT)this);
       ShowWindow(hwnd, SW_SHOW);

       return true; 
   }

   void close ()
   {
       UnregisterClass ("VGui", GetInstance());
       systemWindow = 0;
   }

   LRESULT instanceCallback(UINT message, WPARAM wParam, LPARAM lParam)
   {
       switch (message) 
       {
       case WM_LBUTTONDOWN:
           {
               OutputDebugString("WM_LBUTTONDOWN\n");
               return 0;
           }
           break;
       case WM_MOUSEMOVE:
           {
               OutputDebugString("WM_MOUSEMOVE\n");
               return 0;
           }
           break;
       case WM_LBUTTONUP:
           {
               OutputDebugString("WM_LBUTTONUP\n");
               return 0;
           }
           break;
       case WM_PAINT:
           {
               PAINTSTRUCT ps;
               HDC         hdc;
               hdc = BeginPaint(hwnd, &ps);

               RECT r;
               //clear
               r.top = 0;
               r.left = 0;
               r.right = WIDTH; 
               r.bottom = HEIGHT;
               FillRect(hdc, &r, CreateSolidBrush( RGB(0,0,0) ) );

               //red rect, cuz iz pretty!
               r.top = 50;
               r.left = 20;
               r.right = 140;
               r.bottom = 150;
               FrameRect(hdc, &r,CreateSolidBrush( RGB(255,0,0) ));

               EndPaint(hwnd, &ps);
               return 0;
           }
           break;
       }

       return DefWindowProc(hwnd, message, wParam, lParam);
   }

   static LRESULT CALLBACK wp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
   {
       VGui* instancePtr = (VGui*)GetWindowLong(hWnd, GWL_USERDATA);

       if (instancePtr != NULL)
       {
           return instancePtr ->instanceCallback(message, wParam, lParam);
       }

       return DefWindowProc(hWnd, message, wParam, lParam);
   }
};
