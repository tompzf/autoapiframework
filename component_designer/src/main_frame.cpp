/********************************************************************************
 * Copyright (c) 2025-2026 ZF Friedrichshafen AG
 * 
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Contributors:
 *   Thomas Pfleiderer - initial API and implementation
 ********************************************************************************/
 
#include "main_frame.h"

#include <wx/filename.h>
#include <wx/icon.h>
#include <wx/image.h>
#include <wx/listctrl.h>
#include <wx/notebook.h>
#include <wx/stdpaths.h>

#include <algorithm>
#include <vector>

namespace acd 
{
    namespace 
    {
        constexpr const char* kMetaModelFileName = "autoapiframework_meta_model.yaml";
        constexpr const char* kLogoFileName = "autoapiframework_logo.png";
        constexpr const char* kIconFileName = "autoapiframework_icon.png";
        constexpr int kMinColumnWidth = 90;
        constexpr int kMaxColumnWidth = 320;

        wxString ToWx(const std::string& text) { return wxString::FromUTF8(text.c_str()); }

        std::string ToStd(const wxString& text) { return std::string(text.utf8_str()); }

        wxString FindRuntimeFile(const char* fileName)
        {
            wxFileName executable(wxStandardPaths::Get().GetExecutablePath());
            const wxString executableDirectory = executable.GetPath();
            const wxString candidates[] = {
                wxFileName(executableDirectory, fileName).GetFullPath(),
                wxFileName(wxGetCwd(), fileName).GetFullPath(),
                wxFileName(wxGetCwd() + "/..", fileName).GetFullPath(),
                wxFileName(wxGetCwd() + "/component_designer", fileName).GetFullPath()};

            for (const wxString& candidate : candidates)
            {
                if (wxFileName::FileExists(candidate))
                {
                    return candidate;
                }
            }
            return wxEmptyString;
        }

        /// Single line preview of a possibly multi line scalar.
        wxString OneLine(const std::string& text) 
        {
            wxString value = ToWx(text);
            value.Replace("\n", " ");
            value.Replace("\t", " ");
            return value;
        }

        wxString CellValue(const YamlNodePtr& entry, const std::string& key) 
        {
            if (!entry || !entry->IsMap()) 
            {
                return wxEmptyString;
            }
            const YamlNodePtr value = entry->Find(key);
            if (!value) 
            {
                return wxEmptyString;
            }
            if (value->IsScalar()) 
            {
                return OneLine(value->GetScalar());
            }
            if (value->IsSequence()) 
            {
                wxString joined;
                for (const YamlNodePtr& item : value->GetSequence()) 
                {
                    if (item && item->IsScalar()) 
                    {
                        if (!joined.empty()) 
                        {
                            joined += ", ";
                        }
                        joined += OneLine(item->GetScalar());
                    }
                }
                return joined;
            }
            return "<map>";
        }

        wxListCtrl* CreateReportList(wxWindow* parent) 
        {
            return new wxListCtrl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_HRULES | wxLC_VRULES);
        }

        void AutoSizeColumns(wxListCtrl* list) 
        {
            for (int column = 0; column < list->GetColumnCount(); ++column) 
            {
                list->SetColumnWidth(column, wxLIST_AUTOSIZE);
                const int contentWidth = list->GetColumnWidth(column);
                list->SetColumnWidth(column, wxLIST_AUTOSIZE_USEHEADER);
                int width = std::max(contentWidth, list->GetColumnWidth(column));
                width = std::max(kMinColumnWidth, std::min(width, kMaxColumnWidth));
                list->SetColumnWidth(column, width);
            }
        }

    } // namespace

    wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
        EVT_BUTTON(MainFrame::ID_NewSpecification, MainFrame::OnNewSpecification)
        EVT_BUTTON(MainFrame::ID_OpenSpecification, MainFrame::OnOpenSpecification)
        EVT_BUTTON(MainFrame::ID_SaveSpecification, MainFrame::OnSaveSpecification)
        EVT_BUTTON(MainFrame::ID_SaveSpecificationAs, MainFrame::OnSaveSpecificationAs)
        EVT_BUTTON(MainFrame::ID_OpenMetaModel, MainFrame::OnOpenMetaModel)
        EVT_BUTTON(MainFrame::ID_ShowMetaModel, MainFrame::OnShowMetaModel)
        EVT_BUTTON(MainFrame::ID_AddViaVss, MainFrame::OnAddViaVss)
        EVT_BUTTON(MainFrame::ID_Add, MainFrame::OnAdd)
        EVT_BUTTON(MainFrame::ID_Delete, MainFrame::OnDelete)
        EVT_BUTTON(MainFrame::ID_Edit, MainFrame::OnEdit)
        EVT_BUTTON(MainFrame::ID_Validation, MainFrame::OnValidation)
        EVT_BUTTON(MainFrame::ID_Show, MainFrame::OnShow)
        EVT_BUTTON(MainFrame::ID_CreateAPI, MainFrame::OnCreateAPI)
        EVT_MENU(MainFrame::ID_NewSpecification, MainFrame::OnNewSpecification)
        EVT_MENU(MainFrame::ID_OpenSpecification, MainFrame::OnOpenSpecification)
        EVT_MENU(MainFrame::ID_SaveSpecificationAs, MainFrame::OnSaveSpecificationAs)
        EVT_MENU(MainFrame::ID_OpenMetaModel, MainFrame::OnOpenMetaModel)
        EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
        EVT_MENU(wxID_EXIT, MainFrame::OnExit)
    wxEND_EVENT_TABLE()

    MainFrame::MainFrame()
        : wxFrame(nullptr, wxID_ANY, kApplicationName, wxDefaultPosition, wxSize(1200, 750)) 
    {
#ifdef __WXMSW__
        SetIcon(wxIcon("IDI_APP_ICON", wxBITMAP_TYPE_ICO_RESOURCE));
#endif
#ifdef __WXGTK__
        wxIcon appIcon(FindRuntimeFile(kIconFileName), wxBITMAP_TYPE_PNG);
        if (appIcon.IsOk())
        {
            SetIcon(appIcon);
        }
#endif
        BuildUi();
        AutoLoadMetaModel();
        UpdateMetaModelButtonStates();
        UpdateTitleAndStatus();
    }

    void MainFrame::BuildUi() 
    {
        wxMenu* fileMenu = new wxMenu();
        fileMenu->Append(ID_NewSpecification, "&New function specification\tCtrl-N");
        fileMenu->Append(ID_OpenSpecification, "&Read function specification...\tCtrl-O");
        fileMenu->Append(ID_SaveSpecificationAs, "&Write function specification as...\tCtrl-S");
        fileMenu->AppendSeparator();
        fileMenu->Append(ID_OpenMetaModel, "Load &meta model...");
        fileMenu->AppendSeparator();
        fileMenu->Append(wxID_EXIT);

        wxMenu* helpMenu = new wxMenu();
        helpMenu->Append(wxID_ABOUT);

        wxMenuBar* menuBar = new wxMenuBar();
        menuBar->Append(fileMenu, "&File");
        menuBar->Append(helpMenu, "&Help");
        SetMenuBar(menuBar);

        CreateStatusBar(2);

        wxPanel* panel = new wxPanel(this);
        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

        wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
        m_newButton = new wxButton(panel, ID_NewSpecification, "New");
        m_readButton = new wxButton(panel, ID_OpenSpecification, "Read .acs file");
        m_saveButton = new wxButton(panel, ID_SaveSpecification, "Save");
        m_saveButton->Enable(false);
        m_saveAsButton = new wxButton(panel, ID_SaveSpecificationAs, "Write .acs file as...");
        m_saveAsButton->Enable(false);
        m_metaButton = new wxButton(panel, ID_OpenMetaModel, "Load meta model...");
        m_showMetaModelButton = new wxButton(panel, ID_ShowMetaModel, "Show meta model");
        m_metaModelLabel = new wxStaticText(panel, wxID_ANY, "Meta model: <not loaded>");

        buttonSizer->Add(m_newButton, 0, wxALL, 5);
        buttonSizer->Add(m_readButton, 0, wxALL, 5);
        buttonSizer->Add(m_saveButton, 0, wxALL, 5);
        buttonSizer->Add(m_saveAsButton, 0, wxALL, 5);
        buttonSizer->Add(m_metaButton, 0, wxALL, 5);
        buttonSizer->Add(m_showMetaModelButton, 0, wxALL, 5);
        buttonSizer->AddStretchSpacer();
        buttonSizer->Add(m_metaModelLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
        wxImage logoImage(FindRuntimeFile(kLogoFileName), wxBITMAP_TYPE_PNG);
        if (logoImage.IsOk())
        {
            const int logoHeight = 55;
            const int logoWidth = logoImage.GetWidth() * logoHeight / logoImage.GetHeight();
            logoImage = logoImage.Scale(logoWidth, logoHeight, wxIMAGE_QUALITY_HIGH);
            buttonSizer->Add(new wxStaticBitmap(panel, wxID_ANY, wxBitmap(logoImage)), 0,
                            wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 8);
        }
        mainSizer->Add(buttonSizer, 0, wxEXPAND);

        wxBoxSizer* editButtonSizer = new wxBoxSizer(wxHORIZONTAL);
        m_addViaVssButton = new wxButton(panel, ID_AddViaVss, "Add via vss");
        m_addViaVssButton->Enable(false);
        m_addButton = new wxButton(panel, ID_Add, "Add");
        m_addButton->Enable(false);
        m_deleteButton = new wxButton(panel, ID_Delete, "Delete");
        m_deleteButton->Enable(false);
        m_editButton = new wxButton(panel, ID_Edit, "Edit");
        m_editButton->Enable(false);
        m_validationButton = new wxButton(panel, ID_Validation, "Syntax check");
        m_validationButton->Enable(false);
        m_showButton = new wxButton(panel, ID_Show, "Show");
        m_showButton->Enable(false);
        m_createAPIButton = new wxButton(panel, ID_CreateAPI, "Create API");
        m_createAPIButton->Enable(false);
        editButtonSizer->Add(m_addViaVssButton, 0, wxALL, 5);
        editButtonSizer->Add(m_addButton, 0, wxALL, 5);
        editButtonSizer->Add(m_deleteButton, 0, wxALL, 5);
        editButtonSizer->Add(m_editButton, 0, wxALL, 5);
        editButtonSizer->Add(m_validationButton, 0, wxALL, 5);
        editButtonSizer->Add(m_showButton, 0, wxALL, 5);
        editButtonSizer->Add(m_createAPIButton, 0, wxALL, 5);
        mainSizer->Add(editButtonSizer, 0, wxEXPAND);

        m_notebook = new wxNotebook(panel, wxID_ANY);

        wxPanel* attributePage = new wxPanel(m_notebook);
        m_attributeList = CreateReportList(attributePage);
        m_nameText = new wxTextCtrl(attributePage, wxID_ANY);
        m_versionText = new wxTextCtrl(attributePage, wxID_ANY);
        m_descriptionText = new wxTextCtrl(attributePage, wxID_ANY, wxEmptyString, wxDefaultPosition,
                                        wxSize(-1, 140), wxTE_MULTILINE | wxTE_BESTWRAP);
        m_nameText->Bind(wxEVT_TEXT, [this](wxCommandEvent&)
        {
            m_specification.SetName(ToStd(m_nameText->GetValue()));
        });
        m_versionText->Bind(wxEVT_TEXT, [this](wxCommandEvent&)
        {
            m_specification.SetVersion(ToStd(m_versionText->GetValue()));
        });
        m_descriptionText->Bind(wxEVT_TEXT, [this](wxCommandEvent&)
        {
            m_specification.SetDescription(ToStd(m_descriptionText->GetValue()));
        });
        wxBoxSizer* attributeSizer = new wxBoxSizer(wxVERTICAL);
        wxFlexGridSizer* editableAttributes = new wxFlexGridSizer(2, 5, 5);
        editableAttributes->AddGrowableCol(1, 1);
        editableAttributes->Add(new wxStaticText(attributePage, wxID_ANY, "Name"), 0,
                                wxALIGN_CENTER_VERTICAL);
        editableAttributes->Add(m_nameText, 1, wxEXPAND);
        editableAttributes->Add(new wxStaticText(attributePage, wxID_ANY, "Version"), 0,
                                wxALIGN_CENTER_VERTICAL);
        editableAttributes->Add(m_versionText, 1, wxEXPAND);
        attributeSizer->Add(editableAttributes, 0, wxEXPAND | wxALL, 5);
        attributeSizer->Add(new wxStaticText(attributePage, wxID_ANY, "Attributes (read only)"), 0, wxLEFT | wxTOP, 5);
        attributeSizer->Add(m_attributeList, 1, wxEXPAND | wxALL, 5);
        attributeSizer->Add(new wxStaticText(attributePage, wxID_ANY, "Description"), 0, wxLEFT, 5);
        attributeSizer->Add(m_descriptionText, 0, wxEXPAND | wxALL, 5);
        attributePage->SetSizer(attributeSizer);

        auto makeListPage = [this](wxListCtrl** target) 
        {
            wxPanel* page = new wxPanel(m_notebook);
            *target = CreateReportList(page);
            wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
            sizer->Add(*target, 1, wxEXPAND | wxALL, 5);
            page->SetSizer(sizer);
            return page;
        };

        wxPanel* signalPage = makeListPage(&m_signalList);
        wxPanel* parameterPage = makeListPage(&m_parameterList);
        wxPanel* schedulingPage = makeListPage(&m_schedulingList);
        auto bindEditButtonState = [this](wxListCtrl* list)
        {
            list->Bind(wxEVT_LIST_ITEM_SELECTED, [this](wxListEvent&)
            {
                UpdateCollectionButtonStates();
            });
            list->Bind(wxEVT_LIST_ITEM_DESELECTED, [this](wxListEvent&)
            {
                UpdateCollectionButtonStates();
            });
        };
        bindEditButtonState(m_signalList);
        bindEditButtonState(m_parameterList);
        bindEditButtonState(m_schedulingList);

        m_notebook->AddPage(attributePage, "Attributes", true);
        m_notebook->AddPage(signalPage, "Signal collection");
        m_notebook->AddPage(parameterPage, "Parameter collection");
        m_notebook->AddPage(schedulingPage, "Scheduling collection");
        m_notebook->Bind(wxEVT_NOTEBOOK_PAGE_CHANGED, [this](wxBookCtrlEvent& event)
        {
            UpdateCollectionButtonStates();
            event.Skip();
        });

        mainSizer->Add(m_notebook, 1, wxEXPAND | wxALL, 5);
        panel->SetSizer(mainSizer);

        m_attributeList->AppendColumn("Attribute", wxLIST_FORMAT_LEFT, 260);
        m_attributeList->AppendColumn("Value", wxLIST_FORMAT_LEFT, 600);
    }

    void MainFrame::AutoLoadMetaModel() 
    {
        wxFileName executable(wxStandardPaths::Get().GetExecutablePath());
        const wxString exeDir = executable.GetPath();

        const wxString candidates[] = 
        {
            wxFileName(exeDir, kMetaModelFileName).GetFullPath(),
            wxFileName(exeDir + "/..", kMetaModelFileName).GetFullPath(),
            wxFileName(exeDir + "/../..", kMetaModelFileName).GetFullPath(),
            wxFileName(exeDir + "/../../..", kMetaModelFileName).GetFullPath(),
            wxFileName(wxGetCwd(), kMetaModelFileName).GetFullPath(),
            wxFileName(wxGetCwd() + "/..", kMetaModelFileName).GetFullPath(),
            wxFileName(wxGetCwd() + "/component_designer", kMetaModelFileName).GetFullPath()
        };

        for (const wxString& candidate : candidates) 
        {
            if (wxFileName::FileExists(candidate)) 
            {
                m_metaModelVersion = LoadMetaModel(candidate, true);
                if (m_metaModel.IsLoaded()) 
                {
                    return;
                }
            }
        }
    }

    void MainFrame::OnNewSpecification(wxCommandEvent&)
    {
        if (m_specification.IsLoaded())
        {
            return;
        }

        m_specification.New(m_metaModel.GetName(), m_metaModel.GetVersion());
        m_newButton->Enable(false);
        m_saveAsButton->Enable(true);
        m_showButton->Enable(true);
        m_validationButton->Enable(true);        
		m_createAPIButton->Enable(true);
        RefreshAll();
        UpdateTitleAndStatus();
    }

    void MainFrame::OnOpenSpecification(wxCommandEvent&) 
    {
        wxFileDialog dialog(this, "Read function specification", wxEmptyString, wxEmptyString,
                            "Function specification (*.acs;*.afs)|*.acs;*.afs|"
                            "Function specification (*.acs.yaml;*.afs.yaml)|*.acs.yaml;*.afs.yaml|"
                            "YAML files (*.yaml;*.yml)|*.yaml;*.yml|All files (*.*)|*.*",
                            wxFD_OPEN | wxFD_FILE_MUST_EXIST);
        if (dialog.ShowModal() == wxID_CANCEL) 
        {
            return;
        }
        LoadSpecification(dialog.GetPath());
    }

    void MainFrame::OnSaveSpecification(wxCommandEvent&)
    {
        if (!m_specification.IsLoaded())
        {
            return;
        }

        const wxString path = ToWx(m_specification.GetSourcePath());
        std::string error;
        if (!m_specification.Save(ToStd(path), error))
        {
            wxMessageBox("Could not write file:\n\n" + ToWx(error), kApplicationName,
                        wxOK | wxICON_ERROR, this);
            return;
        }
        SetStatusText("Written: " + path, 0);
    }

    void MainFrame::OnSaveSpecificationAs(wxCommandEvent&) 
    {
        if (!m_specification.IsLoaded()) 
        {
            wxMessageBox("Read a function specification first.", kApplicationName,
                        wxOK | wxICON_INFORMATION, this);
            return;
        }

        wxFileName source(ToWx(m_specification.GetSourcePath()));
        wxString suggested = source.GetFullName();
        const wxString suffix = ".acs";
        if (suggested.EndsWith(".acs") || suggested.EndsWith(".afs")) 
        {
            suggested = suggested.Left(suggested.length() - suffix.length()) + "_copy" + suffix;
        } 
        else if (suggested.EndsWith(".acs.yaml") || suggested.EndsWith(".afs.yaml")) 
        {
            suggested = suggested.Left(suggested.length() - wxString(".acs.yaml").length()) + "_copy" + suffix;
        } 
        else 
        {
            suggested += "_copy" + suffix;
        }

        wxFileDialog dialog(this, "Write function specification as", source.GetPath(), suggested,
                            "Function specification (*.acs)|*.acs|All files (*.*)|*.*",
                            wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
        if (dialog.ShowModal() == wxID_CANCEL) 
        {
            return;
        }

        std::string error;
        if (!m_specification.Save(ToStd(dialog.GetPath()), error)) 
        {
            wxMessageBox("Could not write file:\n\n" + ToWx(error), kApplicationName,
                        wxOK | wxICON_ERROR, this);
            return;
        }
        SetStatusText("Written: " + dialog.GetPath(), 0);
    }

    void MainFrame::OnOpenMetaModel(wxCommandEvent&) 
    {
        wxFileDialog dialog(this, "Load meta model", wxEmptyString, kMetaModelFileName,
                            "YAML files (*.yaml;*.yml)|*.yaml;*.yml|All files (*.*)|*.*",
                            wxFD_OPEN | wxFD_FILE_MUST_EXIST);
        if (dialog.ShowModal() == wxID_CANCEL) 
        {
            return;
        }
        m_metaModelVersion = LoadMetaModel(dialog.GetPath(), true);
        SetStatusText(m_metaModel.IsLoaded() ? "Meta model loaded" : "No meta model", 1);

        UpdateMetaModelButtonStates();
        RefreshAll();
    }

    void MainFrame::OnShowMetaModel(wxCommandEvent&)
    {
        if (!m_metaModel.IsLoaded())
        {
            return;
        }

        // wxMessageBox("Could not write file:\n\nGetIinterfaceTypesCount. " + std::to_string(m_metaModel.GetIinterfaceTypesCount())
        // + "GetEnumCount: ", kApplicationName,
        //                 wxOK | wxICON_ERROR, this);



        wxDialog dialog(this, wxID_ANY, "Meta model", wxDefaultPosition,
                        wxSize(900, 650), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(new wxTextCtrl(&dialog, wxID_ANY, ToWx(m_metaModel.GetFileContent()),
                                 wxDefaultPosition, wxDefaultSize,
                                 wxTE_MULTILINE | wxTE_READONLY | wxTE_DONTWRAP),
                   1, wxEXPAND | wxALL, 8);
        sizer->Add(dialog.CreateSeparatedButtonSizer(wxOK), 0, wxEXPAND | wxALL, 8);
        dialog.SetSizer(sizer);
        dialog.CentreOnParent();
        dialog.ShowModal();
    }

    void MainFrame::OnAddViaVss(wxCommandEvent&)
    {
        wxMessageBox("Add signal via vss:\n\n NOT IMPLEMENTED ",
                kApplicationName,
                wxOK | wxICON_ERROR, this);   
    }

    void MainFrame::OnAdd(wxCommandEvent&)
    {
        const int selectedTab = m_notebook->GetSelection();

        std::string tabName = "";
        switch (selectedTab)
        {
        case 1:
            tabName = "Signals";
            break;
        case 2:
            tabName = "Parameters";
            break;
        case 3:
            tabName = "Scheduling";
            break;
        default:
            tabName = "Attributes";
            break;
        }

        wxMessageBox("Add item to the selected tab:\n\nTab: " + tabName + "\n\n NOT IMPLEMENTED ",
                        kApplicationName,
                        wxOK | wxICON_ERROR, this);            

        wxUnusedVar(selectedTab);   
    }

    void MainFrame::OnDelete(wxCommandEvent&)
    {
        wxListCtrl* selectedList = GetSelectedCollectionList();
        const char* collectionKey = GetSelectedCollectionKey();
        if (!selectedList || !collectionKey)
        {
            return;
        }

        const long selectedRow = selectedList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if (selectedRow < 0)
        {
            return;
        }

        if (m_specification.RemoveCollectionItem(collectionKey, static_cast<std::size_t>(selectedRow)))
        {
            RefreshAll();
            SetStatusText(wxString::Format("Deleted item %ld", selectedRow + 1), 0);
        }
    }

    void MainFrame::OnEdit(wxCommandEvent&) 
    {
        wxListCtrl* selectedList = GetSelectedCollectionList();
        const char* collectionKey = GetSelectedCollectionKey();
        const long selectedRow = selectedList
            ? selectedList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED)
            : -1;
        if (!collectionKey || selectedRow < 0)
        {
            return;
        }

        const std::vector<YamlNodePtr> entries = m_specification.GetCollection(collectionKey);
        if (static_cast<std::size_t>(selectedRow) >= entries.size() || !entries[selectedRow] ||
            !entries[selectedRow]->IsMap())
        {
            return;
        }

        struct EditableValue
        {
            YamlNodePtr node;
            wxTextCtrl* control;
        };

        wxDialog dialog(this, wxID_ANY, "Edit item", wxDefaultPosition, wxDefaultSize,
                        wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
        wxFlexGridSizer* fields = new wxFlexGridSizer(2, 6, 8);
        fields->AddGrowableCol(1, 1);
        std::vector<EditableValue> editableValues;

        const auto addField = [&fields, &editableValues, &dialog](const wxString& name,
                                                                    const YamlNodePtr& node)
        {
            if (!node || !node->IsScalar())
            {
                return;
            }
            fields->Add(new wxStaticText(&dialog, wxID_ANY, name), 0, wxALIGN_CENTER_VERTICAL);
            wxTextCtrl* control = new wxTextCtrl(&dialog, wxID_ANY, ToWx(node->GetScalar()));
            fields->Add(control, 1, wxEXPAND);
            editableValues.push_back({node, control});
        };

        for (const auto& entry : entries[selectedRow]->GetMap())
        {
            if (entry.second && entry.second->IsMap())
            {
                for (const auto& nested : entry.second->GetMap())
                {
                    addField(ToWx(entry.first + "." + nested.first), nested.second);
                }
            }
            else
            {
                addField(ToWx(entry.first), entry.second);
            }
        }

        if (editableValues.empty())
        {
            return;
        }

        sizer->Add(fields, 1, wxEXPAND | wxALL, 12);
        sizer->Add(dialog.CreateSeparatedButtonSizer(wxOK | wxCANCEL), 0, wxEXPAND | wxALL, 8);
        dialog.SetSizerAndFit(sizer);
        dialog.SetMinSize(wxSize(450, -1));
        dialog.CentreOnParent();
        if (dialog.ShowModal() != wxID_OK)
        {
            return;
        }

        for (const EditableValue& value : editableValues)
        {
            value.node->SetScalar(ToStd(value.control->GetValue()));
        }
        RefreshAll();
        SetStatusText(wxString::Format("Edited item %ld", selectedRow + 1), 0);
    }

    void MainFrame::OnValidation(wxCommandEvent&)
    {
        wxMessageBox("Validation:\n\n NOT IMPLEMENTED ",
                        kApplicationName,
                        wxOK | wxICON_ERROR, this);          
    }

    void MainFrame::OnShow(wxCommandEvent&)
    {
        if (!m_specification.IsLoaded())
        {
            return;
        }

        wxDialog dialog(this, wxID_ANY, "Function specification", wxDefaultPosition,
                        wxSize(900, 650), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(new wxTextCtrl(&dialog, wxID_ANY, ToWx(m_specification.ToText()),
                                wxDefaultPosition, wxDefaultSize,
                                wxTE_MULTILINE | wxTE_READONLY | wxTE_DONTWRAP),
                   1, wxEXPAND | wxALL, 8);
        wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
        wxButton* syntaxCheckButton = new wxButton(&dialog, wxID_ANY, "Syntax check");
        syntaxCheckButton->Bind(wxEVT_BUTTON, &MainFrame::OnValidation, this);
        buttonSizer->Add(syntaxCheckButton);
        buttonSizer->AddStretchSpacer();
        buttonSizer->Add(dialog.CreateButtonSizer(wxOK));
        sizer->Add(buttonSizer, 0, wxEXPAND | wxALL, 8);
        dialog.SetSizer(sizer);
        dialog.CentreOnParent();
        dialog.ShowModal();
    }

    void MainFrame::OnCreateAPI(wxCommandEvent&)
    {
        wxMessageBox("Create API:\n\n NOT IMPLEMENTED ",
                        kApplicationName,
                        wxOK | wxICON_ERROR, this);               
    }

    void MainFrame::OnAbout(wxCommandEvent&) 
    {
        wxMessageBox(wxString(kApplicationName) + "\n\n"
                    "Reads, displays and writes Eclipse autoapiframework function\n"
                    "specifications (*.acs, YAML content) based on the framework meta model.",
                    "About " + wxString(kApplicationName), wxOK | wxICON_INFORMATION, this);                   
    }

    void MainFrame::OnExit(wxCommandEvent&) { Close(true); }

    void MainFrame::LoadSpecification(const wxString& path) 
    {
        std::string error;
        FunctionSpecification specification;
        if (!specification.Load(m_metaModelVersion, ToStd(path), error)) 
        {
            wxMessageBox("Could not read function specification:\n\n" + ToWx(error),
                        kApplicationName, wxOK | wxICON_ERROR, this);
            return;
        }
        m_specification = std::move(specification);
        m_newButton->Enable(false);
        m_saveButton->Enable(true);
        m_saveAsButton->Enable(true);
        m_showButton->Enable(true);
        m_validationButton->Enable(true);        
		m_createAPIButton->Enable(true);
        RefreshAll();
        UpdateTitleAndStatus();
    }

    std::string MainFrame::LoadMetaModel(const wxString& path, bool reportErrors) 
    {
        std::string error;
        if (!m_metaModel.Load(ToStd(path), error)) 
        {
            if (reportErrors) 
            {
                wxMessageBox("Could not read meta model:\n\n" + ToWx(error), kApplicationName,
                            wxOK | wxICON_ERROR, this);
            }
            return "";
        }
        m_metaModelLabel->SetLabel("Meta model: " + ToWx(m_metaModel.GetName()) + " " +
                                ToWx(m_metaModel.GetVersion()));
        m_metaModelLabel->GetParent()->Layout();
        return m_metaModel.GetVersion();
    }

    void MainFrame::RefreshAll() 
    {
        FillAttributes();
        FillCollection(m_signalList, FunctionSpecification::kDataInterfacesKey, "Data");
        FillCollection(m_parameterList, FunctionSpecification::kParametersKey, "Parameter");
        FillCollection(m_schedulingList, FunctionSpecification::kSchedulingKey, "Scheduling");

        m_notebook->SetPageText(1, wxString::Format(
            "Signal collection (%d)",
            static_cast<int>(m_specification.GetCollection(FunctionSpecification::kDataInterfacesKey).size())));
        m_notebook->SetPageText(2, wxString::Format(
            "Parameter collection (%d)",
            static_cast<int>(m_specification.GetCollection(FunctionSpecification::kParametersKey).size())));
        m_notebook->SetPageText(3, wxString::Format(
            "Scheduling collection (%d)",
            static_cast<int>(m_specification.GetCollection(FunctionSpecification::kSchedulingKey).size())));
        UpdateCollectionButtonStates();
    }

    void MainFrame::FillAttributes() 
    {
        m_attributeList->DeleteAllItems();
        long row = 0;
        for (const auto& attribute : m_specification.GetAttributes()) 
        {
            if (attribute.first == "name" || attribute.first == "version" ||
                attribute.first == "description")
            {
                continue;
            }
            m_attributeList->InsertItem(row, ToWx(attribute.first));
            m_attributeList->SetItem(row, 1, OneLine(attribute.second));
            ++row;
        }
        m_nameText->SetValue(ToWx(m_specification.GetName()));
        m_versionText->SetValue(ToWx(m_specification.GetVersion()));
        m_descriptionText->SetValue(ToWx(m_specification.GetDescription()));
    }

    void MainFrame::FillCollection(wxListCtrl* list, const std::string& collectionKey,
                                const std::string& interfaceTypeName) 
    {
        const std::vector<YamlNodePtr> entries = m_specification.GetCollection(collectionKey);
        const std::vector<std::string> columns = m_metaModel.ColumnsFor(interfaceTypeName, entries);

        list->ClearAll();
        list->AppendColumn("#", wxLIST_FORMAT_RIGHT, 40);
        for (const std::string& column : columns) 
        {
            list->AppendColumn(ToWx(column), wxLIST_FORMAT_LEFT, kMinColumnWidth);
        }

        long row = 0;
        for (const YamlNodePtr& entry : entries) 
        {
            list->InsertItem(row, wxString::Format("%ld", row + 1));
            for (std::size_t column = 0; column < columns.size(); ++column) 
            {
                list->SetItem(row, static_cast<int>(column) + 1, CellValue(entry, columns[column]));
            }
            ++row;
        }
        AutoSizeColumns(list);
    }

    wxListCtrl* MainFrame::GetSelectedCollectionList() const
    {
        switch (m_notebook->GetSelection())
        {
        case 1:
            return m_signalList;
        case 2:
            return m_parameterList;
        case 3:
            return m_schedulingList;
        default:
            return nullptr;
        }
    }

    const char* MainFrame::GetSelectedCollectionKey() const
    {
        switch (m_notebook->GetSelection())
        {
        case 1:
            return FunctionSpecification::kDataInterfacesKey;
        case 2:
            return FunctionSpecification::kParametersKey;
        case 3:
            return FunctionSpecification::kSchedulingKey;
        default:
            return nullptr;
        }
    }

    void MainFrame::UpdateCollectionButtonStates()
    {
        wxListCtrl* selectedList = GetSelectedCollectionList();
        const bool hasSelectedItem = selectedList && selectedList->GetSelectedItemCount() > 0;
        m_addViaVssButton->Enable(m_notebook->GetSelection() == 1);
        m_addButton->Enable(selectedList != nullptr);
        m_deleteButton->Enable(hasSelectedItem);
        m_editButton->Enable(hasSelectedItem);
    }

    void MainFrame::UpdateMetaModelButtonStates()
    {
        const bool metaModelLoaded = m_metaModel.IsLoaded();
        const bool specificationLoaded = m_specification.IsLoaded();
        m_newButton->Enable(metaModelLoaded && !specificationLoaded);
        m_readButton->Enable(metaModelLoaded);
        m_metaButton->Enable(!metaModelLoaded);
        m_showMetaModelButton->Enable(metaModelLoaded);
    }

    void MainFrame::UpdateTitleAndStatus() 
    {
        if (!m_specification.IsLoaded()) 
        {
            SetTitle(kApplicationName);
            SetStatusText("No function specification loaded", 0);
            SetStatusText(m_metaModel.IsLoaded() ? "Meta model loaded" : "No meta model", 1);
            return;
        }
        SetTitle(wxString(kApplicationName) + " - " + ToWx(m_specification.GetName()) + " [" +
                wxFileName(ToWx(m_specification.GetSourcePath())).GetFullName() + "]");
        SetStatusText("Read: " + ToWx(m_specification.GetSourcePath()), 0);
        SetStatusText("Meta model reference: " + ToWx(m_specification.GetMetaModelName()) + " " +
                        ToWx(m_specification.GetMetaModelVersion()),
                    1);
    }

} // namespace acd
