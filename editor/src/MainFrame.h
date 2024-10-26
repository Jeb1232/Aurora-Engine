#pragma once
#include"RenderPanel.h"
#include<wx/wx.h>
#include<wx/aui/aui.h>
#include<wx/aui/auibar.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/stc/stc.h>
#include <wx/dirctrl.h>
#include <wx/srchctrl.h>
#include <wx/listctrl.h>
#include <wx/treelist.h>
#include<wx/dialog.h>
#include<wx/aboutdlg.h>
#include<wx/icon.h>

class MainFrame : public wxFrame
{
public:
	wxAuiManager m_mgr;
	MainFrame(const wxString& title);
	wxMenuBar* menuBar;
	wxMenu* fileMenu;
	wxMenu* editMenu;
	wxMenu* toolsMenu;
	wxMenu* helpMenu;
	wxAuiToolBar* mainToolBar;
	wxAuiToolBarItem* newScene;
	wxAuiToolBarItem* openScene;
	wxAuiToolBarItem* Save;
	wxAuiToolBarItem* Undo;
	wxAuiToolBarItem* Redo;
	wxAuiToolBarItem* tMove;
	wxAuiToolBarItem* tRotate;
	wxAuiToolBarItem* tScale;
	wxAuiToolBarItem* Play;
	wxAuiToolBarItem* Stop;
	wxStatusBar* m_statusBar2;
	wxTreeCtrl* SceneGraph;
	RenderPanel* Viewport;
	wxPropertyGrid* objectProperties;
	wxPGProperty* Position;
	wxPGProperty* pX;
	wxPGProperty* pY;
	wxPGProperty* pZ;
	wxPGProperty* Rotation;
	wxPGProperty* rX;
	wxPGProperty* rY;
	wxPGProperty* rZ;
	wxPGProperty* Scale;
	wxPGProperty* sX;
	wxPGProperty* sY;
	wxPGProperty* sZ;
	wxGenericDirCtrl* projectFolder;
	wxListCtrl* Project;
	wxListCtrl* entityMenu;

	void ShowSimpleAboutDialog(wxCommandEvent& WXUNUSED(event))
	{
		//wxArrayString arrString;
		//arrString.Add(_(""));
		wxAboutDialogInfo info;
		info.SetName(_("Editor"));
		info.SetVersion(_("0.1 Alpha"));
		info.SetDescription(_("The editor for the Aurora Game Engine."));
		//info.SetDevelopers(arrString);
		info.SetCopyright("(C) 2024 Julian Scott. All rights reserved");

		wxAboutBox(info);
	}

	void OnExit(wxCommandEvent& event) {
		this->Close();
	}
};

