#include "App.h"
#include"MainFrame.h"

wxIMPLEMENT_APP(App);

bool App::OnInit() {
	MainFrame* mainFrame = new MainFrame("Editor");
	mainFrame->Show();
	return true;
}