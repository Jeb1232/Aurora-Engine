#include "MainFrame.h"

MainFrame::MainFrame(const wxString& title): wxFrame(nullptr, wxID_ANY, title){
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);
	this->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	m_mgr.SetManagedWindow(this);
	m_mgr.SetFlags(wxAUI_MGR_DEFAULT);

	wxInitAllImageHandlers();

	menuBar = new wxMenuBar(0);
	fileMenu = new wxMenu();
	wxMenuItem* Exit;
	Exit = new wxMenuItem(fileMenu, wxID_EXIT, wxString(_("Quit")), wxEmptyString, wxITEM_NORMAL);
	fileMenu->Append(Exit);
	this->Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);

	menuBar->Append(fileMenu, _("File"));

	editMenu = new wxMenu();
	menuBar->Append(editMenu, _("Edit"));

	toolsMenu = new wxMenu();
	menuBar->Append(toolsMenu, _("Tools"));

	helpMenu = new wxMenu();
	wxMenuItem* About;
	About = new wxMenuItem(helpMenu, wxID_ABOUT, wxString(_("About")), wxEmptyString, wxITEM_NORMAL);
	helpMenu->Append(About);
	helpMenu->Bind(wxEVT_MENU, &MainFrame::ShowSimpleAboutDialog, this, About->GetId());

	menuBar->Append(helpMenu, _("Help"));

	this->SetMenuBar(menuBar);

	mainToolBar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT);
	newScene = mainToolBar->AddTool(wxID_ANY, _("New"), wxBitmap(wxT("res/icons/page_white_add.png"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);

	openScene = mainToolBar->AddTool(wxID_ANY, _("Open"), wxBitmap(wxT("res/icons/folder.png"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);

	Save = mainToolBar->AddTool(wxID_ANY, _("Save"), wxBitmap(wxT("res/icons/page_save.png"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);

	mainToolBar->AddSeparator();

	Undo = mainToolBar->AddTool(wxID_ANY, _("Undo"), wxBitmap(wxT("res/icons/arrow_undo.png"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, _("Undo"), wxEmptyString, NULL);

	Redo = mainToolBar->AddTool(wxID_ANY, _("Redo"), wxBitmap(wxT("res/icons/arrow_redo.png"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, _("Redo"), wxEmptyString, NULL);

	mainToolBar->AddSeparator();

	tMove = mainToolBar->AddTool(wxID_ANY, _("Move"), wxBitmap(wxT("res/icons/arrow_up.png"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, _("Move"), _("Move an object"), NULL);

	tRotate = mainToolBar->AddTool(wxID_ANY, _("Rotate"), wxBitmap(wxT("res/icons/arrow_refresh_small.png"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, _("Rotate"), _("Rotate an object"), NULL);

	tScale = mainToolBar->AddTool(wxID_ANY, _("Scale"), wxBitmap(wxT("res/icons/arrow_out.png"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, _("Scale"), _("Scale an object"), NULL);

	mainToolBar->AddSeparator();

	Play = mainToolBar->AddTool(wxID_ANY, _("Play"), wxBitmap(wxT("res/icons/control_play.png"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, _("Play"), _("Play your game"), NULL);

	Stop = mainToolBar->AddTool(wxID_ANY, _("Stop"), wxBitmap(wxT("res/icons/control_stop.png"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, _("Stop"), _("Stop playing your game"), NULL);

	mainToolBar->Realize();
	m_mgr.AddPane(mainToolBar, wxAuiPaneInfo().Name(wxT("toolBar")).Top().CaptionVisible(false).CloseButton(false).PaneBorder(false).Dock().Resizable().FloatingSize(wxDefaultSize).DockFixed(true).BottomDockable(false).LeftDockable(false).RightDockable(false).Floatable(false));

	m_statusBar2 = this->CreateStatusBar(1, wxSTB_SIZEGRIP, wxID_ANY);
	SceneGraph = new wxTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE);
	SceneGraph->AddRoot(_("Root"));
	SceneGraph->InsertItem(SceneGraph->GetRootItem(), 0, _("Child Object"));
	m_mgr.AddPane(SceneGraph, wxAuiPaneInfo().Name(wxT("sceneGraph")).Left().Caption(_("Scene Graph")).Dock().Resizable().FloatingSize(wxDefaultSize).Floatable(true));

	Viewport = new RenderPanel(this);
	m_mgr.AddPane(Viewport, wxAuiPaneInfo().Name(wxT("viewport")).Center().Caption(_("Viewport")).Dock().Resizable().FloatingSize(wxSize(42, 59)).Floatable(true));

	objectProperties = new wxPropertyGrid(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE);
	m_mgr.AddPane(objectProperties, wxAuiPaneInfo().Name(wxT("objectProperties")).Right().Caption(_("Properties")).Dock().Resizable().FloatingSize(wxDefaultSize).Floatable(true));

	Position = objectProperties->Append(new wxPropertyCategory(_("Position"), wxT("Position")));
	pX = objectProperties->Append(new wxFloatProperty(_("X")));
	pY = objectProperties->Append(new wxFloatProperty(_("Y")));
	pZ = objectProperties->Append(new wxFloatProperty(_("Z")));
	Rotation = objectProperties->Append(new wxPropertyCategory(_("Rotation"), wxT("Rotation")));
	rX = objectProperties->Append(new wxFloatProperty(_("X")));
	rY = objectProperties->Append(new wxFloatProperty(_("Y")));
	rZ = objectProperties->Append(new wxFloatProperty(_("Z")));
	Scale = objectProperties->Append(new wxPropertyCategory(_("Scale"), wxT("Scale")));
	sX = objectProperties->Append(new wxFloatProperty(_("X")));
	sY = objectProperties->Append(new wxFloatProperty(_("Y")));
	sZ = objectProperties->Append(new wxFloatProperty(_("Z")));
	projectFolder = new wxGenericDirCtrl(this, wxID_ANY, _("C:\\Users\\Owner\\source\\repos\\Aurora Engine\\x64\\Release\\data"), wxDefaultPosition, wxDefaultSize, wxDIRCTRL_3D_INTERNAL | wxSUNKEN_BORDER, wxEmptyString, 0);

	projectFolder->ShowHidden(false);
	m_mgr.AddPane(projectFolder, wxAuiPaneInfo().Bottom().Caption(_("Folder")).Dock().Resizable().FloatingSize(wxDefaultSize).Floatable(true));

	Project = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_ICON);
	m_mgr.AddPane(Project, wxAuiPaneInfo().Bottom().Caption(_("Project")).Dock().Resizable().FloatingSize(wxDefaultSize).Floatable(true));

	entityMenu = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_ICON);
	m_mgr.AddPane(entityMenu, wxAuiPaneInfo().Left().Caption(_("Add Entity")).Dock().Resizable().FloatingSize(wxSize(112, 115)).Floatable(true));


	m_mgr.Update();
	this->Centre(wxBOTH);
}