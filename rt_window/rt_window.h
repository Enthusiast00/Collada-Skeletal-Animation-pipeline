#ifndef RT_WINDOW_H
#define RT_WINDOW_H

#include "rt_defs.h"

class rt_window : public flag_c{

    friend class rabbit;
    friend class rt_d3dmanager;
protected:
    HWND m_hwnd;
    POINT m_pointerpos;

    rt_window();
    bool Init();
    void Update();
    void SetTitle(const char* title);
    void EnableFullScreenMode(bool enable);
public:
    static rt_window * s_window;
private:
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
};

#endif // RT_WINDOW_H
