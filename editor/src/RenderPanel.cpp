#include "RenderPanel.h"

RenderPanel::RenderPanel(wxWindow* parent) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE) {
	renderer.InitRenderer(this->GetSize().GetWidth(), this->GetSize().GetHeight(), this->GetHWND());
}

void RenderPanel::OnPaint(wxPaintEvent& event) {
	wxPaintDC dc(this);
	//renderer.InitRenderer(this->GetSize().GetWidth(), this->GetSize().GetHeight(), this->GetHWND());
	renderer.DrawFrame(this->GetSize().GetWidth(), this->GetSize().GetHeight(), this->GetHWND());
	
}

void RenderPanel::OnSize(wxSizeEvent& event) {
	//renderer.init = false;
	//renderer.InitRenderer(this->GetSize().GetWidth(), this->GetSize().GetHeight(), this->GetHWND());
	//renderer.DrawFrame(event.GetSize().GetWidth(), event.GetSize().GetHeight());
	event.Skip();
}

void RenderPanel::Idle(wxIdleEvent& event) {
	//this->Update();
	//renderer.DrawFrame(this->GetSize().GetWidth(), this->GetSize().GetHeight());
	event.RequestMore();
}

void RenderPanel::OnMouseMove(wxMouseEvent& event) {
	
}

void RenderPanel::OnKeyDown(wxKeyEvent& event) {

}

void RenderPanel::OnKeyUp(wxKeyEvent& event) {

}