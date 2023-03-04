#ifndef RABBIT_H
#define RABBIT_H

#include "rt_defs.h"

class rt_timer {
    friend class rabbit;
    LONGLONG m_tick;
    float m_secondspcnt;
    float m_lastframes;
public:
    float LastFrameSeconds() { return  m_lastframes; }
    float LastFrameMseconds() { return  m_lastframes*1000.0f; }
protected:
    rt_timer():m_tick(0),m_secondspcnt(0.0f),m_lastframes(0.0f) {}

    void Proc() {
        static LONGLONG currtick = 0;
        QueryPerformanceCounter((LARGE_INTEGER*)&currtick);
        m_lastframes   = (currtick - m_tick) * m_secondspcnt;
        m_tick         =  currtick;
    }
    void Start() {
        Clear();
        int64_t cntsPerSec = 0;
        QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
        m_secondspcnt = float(1.0f / cntsPerSec);
        QueryPerformanceCounter((LARGE_INTEGER*)&m_tick);
    }
    void Clear(){ m_tick=0; m_secondspcnt=m_lastframes = 0.0f; }
public:
    static rt_timer * s_timer;

};

class info_object;
class flower_cube;
class test_udo;
class monkey;

class rabbit : public flag_c{

    friend class rt_window;
    friend class rt_d3dmanager;
protected:
    rabbit();
    rt_array<rt_object*> m_obj;


    flower_cube * m_fcube;
    test_udo * m_testudo;
    monkey * m_suz;

    rt_string16 m_hud;
    rt_string16 m_hud_f;
    ID3DXFont* m_font;

    uint8_t m_current_model;
public:
    void Run();
    void Update();

    static bool Init();
    static void Delete();

    uint8_t GetCurrentModel(){ return m_current_model;}
    static GMSG s_msg;
    static info_object * s_info;
    static rabbit * s_instance;
};

#endif // RABBIT_H
