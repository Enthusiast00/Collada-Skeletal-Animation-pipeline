#include "rt_window.h"

#include "rabbit.h"
#include "rt_d3dmanager.h"

#include "info_object.h"

rt_window * rt_window::s_window = NULL;

rt_window::rt_window(){
    m_hwnd = NULL;
}

bool rt_window::Init(){

    g_instance = GetModuleHandle(NULL);
    WNDCLASSEX wclass;
    RT_ZERO(wclass);

    wclass.cbSize = sizeof (WNDCLASSEX);
    wclass.style = CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS;
    wclass.lpfnWndProc = WndProc;
    wclass.hInstance = g_instance;
    wclass.hIcon = LoadIcon(0, IDI_APPLICATION);
    wclass.hIconSm = LoadIcon(g_instance, IDI_APPLICATION );
    wclass.hCursor = LoadCursor(0, IDC_ARROW);
    wclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wclass.lpszClassName = RTW_CLASS_NAME;
    if( !RegisterClassEx (&wclass) ) { RT_THROW("win reg"); }

    RECT R = {0, 0,RT_DEFAULT_WIDTH, RT_DEFAULT_HEIGHT};
    AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
    m_hwnd = CreateWindowEx (0, RTW_CLASS_NAME,RTW_TITLE,WS_OVERLAPPEDWINDOW,
                             RT_DEFAULT_X,RT_DEFAULT_Y,R.right,R.bottom,NULL,NULL,g_instance,NULL);
    ShowWindow (m_hwnd, SW_SHOWNORMAL);
    UpdateWindow (m_hwnd) ;
    return true;
}

void rt_window::Update(){

    MSG messages;
    RT_ZERO(messages);
    while(PeekMessage(&messages,NULL,0,0,PM_REMOVE)) {
        if (messages.message == WM_QUIT) {
            RT_INST->RemoveFlags(RABBIT_RUNNING);

        } else {
            TranslateMessage(&messages);
            DispatchMessage(&messages);
        }
    }
    GetCursorPos(& m_pointerpos);
}
void rt_window::SetTitle(const char* title) {
    if( RT_INST->FlagTest(RABBIT_INIT) ) { SetWindowText(m_hwnd,title); }
}
void rt_window::EnableFullScreenMode(bool enable){
    if( enable ) {
        if( !RT_D3D->m_d3dpp.Windowed ) { return; }
        int width  = GetSystemMetrics(SM_CXSCREEN);
        int height = GetSystemMetrics(SM_CYSCREEN);
        RT_D3D->m_d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
        RT_D3D->m_d3dpp.BackBufferWidth  = width;
        RT_D3D->m_d3dpp.BackBufferHeight = height;
        RT_D3D->m_d3dpp.Windowed         = false;
        SetWindowLongPtr(m_hwnd, GWL_STYLE, WS_POPUP);
        SetWindowPos(m_hwnd, HWND_TOP, 0, 0, width, height, SWP_NOZORDER | SWP_SHOWWINDOW);
    }else{
        if( RT_D3D->m_d3dpp.Windowed ) { return; }
        RECT R = {0, 0, RT_DEFAULT_WIDTH, RT_DEFAULT_HEIGHT};
        AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
        RT_D3D->m_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
        RT_D3D->m_d3dpp.BackBufferWidth  = R.right;
        RT_D3D->m_d3dpp.BackBufferHeight = R.bottom;
        RT_D3D->m_d3dpp.Windowed         = true;
        SetWindowLongPtr(m_hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
        SetWindowPos(m_hwnd, HWND_TOP,RT_DEFAULT_X,RT_DEFAULT_Y, R.right, R.bottom, SWP_NOZORDER | SWP_SHOWWINDOW);
    }
    RT_D3D->Reset();
}

LRESULT CALLBACK rt_window::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

    if(RT_INST->FlagTest(RABBIT_RUNNING)){
        if( (GetAsyncKeyState(VK_LSHIFT)   & 0x8000)!=0 ) { RT_WIN->AddFlags(RT_WIN_LSHIFT); }
        else{  RT_WIN->RemoveFlags(RT_WIN_LSHIFT); }
        if( (GetAsyncKeyState(VK_RSHIFT)   & 0x8000)!=0 ) { RT_WIN->AddFlags(RT_WIN_RSHIFT); }
        else{  RT_WIN->RemoveFlags(RT_WIN_RSHIFT); }

        if( (GetAsyncKeyState(VK_LCONTROL)   & 0x8000)!=0 ) { RT_WIN->AddFlags(RT_WIN_LCTRL); }
        else{  RT_WIN->RemoveFlags(RT_WIN_LCTRL); }
        if( (GetAsyncKeyState(VK_RCONTROL)   & 0x8000)!=0 ) { RT_WIN->AddFlags(RT_WIN_RCTRL); }
        else{  RT_WIN->RemoveFlags(RT_WIN_RCTRL); }

        switch( msg ) {
        case WM_KEYUP : {
            if(wparam == 0x46){
                RT_WIN->EnableFullScreenMode( RT_D3D->m_d3dpp.Windowed==TRUE );
            }
        }break;

        case WM_LBUTTONDOWN : { RT_WIN->AddFlags(RT_WIN_LBUTTOND); }break;
        case WM_LBUTTONUP : { RT_WIN->RemoveFlags(RT_WIN_LBUTTOND); }break;
        case WM_RBUTTONDOWN : { RT_WIN->AddFlags(RT_WIN_RBUTTOND); }break;
        case WM_RBUTTONUP : { RT_WIN->RemoveFlags(RT_WIN_RBUTTOND); }break;

        case WM_ACTIVATE: {
            if( LOWORD(wparam) == WA_INACTIVE ) { RT_INST->AddFlags(RABBIT_PAUSED); }
            else { RT_INST->RemoveFlags(RABBIT_PAUSED); }
        }
        case WM_SIZE: {
            RT_D3D->m_d3dpp.BackBufferWidth  = LOWORD( lparam);
            RT_D3D->m_d3dpp.BackBufferHeight = HIWORD( lparam);
            if( wparam == SIZE_MINIMIZED ){
                RT_INST->AddFlags(RABBIT_PAUSED);
                RT_WIN->AddFlags(RT_WIN_MINORMAXED);
            }
            else if( wparam == SIZE_MAXIMIZED ) {
                RT_INST->RemoveFlags(RABBIT_PAUSED);
                RT_WIN->AddFlags(RT_WIN_MINORMAXED);
            }
            else if(  wparam == SIZE_RESTORED ){
                RT_INST->RemoveFlags(RABBIT_PAUSED);
                if( RT_WIN->FlagTest(RT_WIN_MINORMAXED) && RT_D3D->m_d3dpp.Windowed ){ RT_D3D->Reset(); }
                RT_WIN->RemoveFlags(RT_WIN_MINORMAXED);
            }
        }
        case WM_EXITSIZEMOVE: {
            RECT clientRect = {0, 0, 0, 0};
            GetClientRect(RT_WIN->m_hwnd, &clientRect);
            RT_D3D->m_d3dpp.BackBufferWidth  = clientRect.right;
            RT_D3D->m_d3dpp.BackBufferHeight = clientRect.bottom;
            RT_D3D->Reset();
        }break;

        case WM_MOUSEMOVE: {

            if ( !RT_WIN->FlagTest(RT_WIN_LEAVE) ) {
                TRACKMOUSEEVENT event_;
                event_.cbSize       = sizeof(TRACKMOUSEEVENT);
                event_.dwFlags      = TME_LEAVE | TME_HOVER;
                event_.hwndTrack    = hwnd;
                event_.dwHoverTime  = 10;

                TrackMouseEvent(&event_);
                RT_WIN->AddFlags(RT_WIN_LEAVE);
            }
        }break;
        case WM_MOUSELEAVE: {
            if (RT_WIN->FlagTest(RT_WIN_LEAVE)) { RT_WIN->RemoveFlags(RT_WIN_LEAVE); }
        }break;

        }
        RT_EVENT.isevent = true;
        RT_EVENT.msg = msg;
        RT_EVENT.wparam = wparam;
        RT_EVENT.lparam = lparam;

        RT_INST->Update();
        RT_D3D->Update();
        RT_INFO->Update();
        for(uint32_t i=0;i<RT_INST->m_obj.Size();i++){
            if(!RT_INST->m_obj[i]->FlagTest(RT_MESH_DISABLE)){ RT_INST->m_obj[i]->Update();}
        }
        RT_EVENT.isevent = false;
    }


    switch( msg ) {

    case WM_CLOSE: { DestroyWindow(hwnd);}
    case WM_DESTROY : { PostQuitMessage(0); }

    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}
