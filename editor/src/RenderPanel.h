#pragma once
#include<thread>
#include"wx/wx.h"
#include"../../engine/Engine/renderer/Renderer.h"
class RenderPanel : public wxPanel
{
public:
    Renderer renderer;
    RenderPanel(wxWindow* parent);
protected:
    void OnSize(wxSizeEvent& event);
    void OnPaint(wxPaintEvent& event);
    void Idle(wxIdleEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnKeyUp(wxKeyEvent& event);
    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(RenderPanel, wxPanel)
EVT_PAINT(RenderPanel::OnPaint)
EVT_IDLE(RenderPanel::Idle)
EVT_SIZE(RenderPanel::OnSize)
EVT_MOTION(RenderPanel::OnMouseMove)
EVT_KEY_DOWN(RenderPanel::OnKeyDown)
EVT_KEY_UP(RenderPanel::OnKeyUp)
wxEND_EVENT_TABLE()

