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
        EVT_BUTTON(MainFrame::ID_OpenSpecification, MainFrame::OnOpenSpecification)
        EVT_BUTTON(MainFrame::ID_SaveSpecificationAs, MainFrame::OnSaveSpecificationAs)
        EVT_BUTTON(MainFrame::ID_OpenMetaModel, MainFrame::OnOpenMetaModel)
        EVT_MENU(MainFrame::ID_OpenSpecification, MainFrame::OnOpenSpecification)
        EVT_MENU(MainFrame::ID_SaveSpecificationAs, MainFrame::OnSaveSpecificationAs)
        EVT_MENU(MainFrame::ID_OpenMetaModel, MainFrame::OnOpenMetaModel)
        EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
        EVT_MENU(wxID_EXIT, MainFrame::OnExit)
    wxEND_EVENT_TABLE()

    MainFrame::MainFrame()
        : wxFrame(nullptr, wxID_ANY, "AutoAPI Component Designer", wxDefaultPosition, wxSize(1200, 750)) 
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
        UpdateTitleAndStatus();
    }

    void MainFrame::BuildUi() 
    {
        wxMenu* fileMenu = new wxMenu();
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
        wxButton* readButton = new wxButton(panel, ID_OpenSpecification, "Read .acs file");
        m_saveButton = new wxButton(panel, ID_SaveSpecificationAs, "Write .acs file as...");
        m_saveButton->Enable(false);
        wxButton* metaButton = new wxButton(panel, ID_OpenMetaModel, "Load meta model...");
        m_metaModelLabel = new wxStaticText(panel, wxID_ANY, "Meta model: <not loaded>");

        buttonSizer->Add(readButton, 0, wxALL, 5);
        buttonSizer->Add(m_saveButton, 0, wxALL, 5);
        buttonSizer->Add(metaButton, 0, wxALL, 5);
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

        m_notebook = new wxNotebook(panel, wxID_ANY);

        wxPanel* attributePage = new wxPanel(m_notebook);
        m_attributeList = CreateReportList(attributePage);
        m_descriptionText = new wxTextCtrl(attributePage, wxID_ANY, wxEmptyString, wxDefaultPosition,
                                        wxSize(-1, 140), wxTE_MULTILINE | wxTE_READONLY | wxTE_BESTWRAP);
        wxBoxSizer* attributeSizer = new wxBoxSizer(wxVERTICAL);
        attributeSizer->Add(new wxStaticText(attributePage, wxID_ANY, "Attributes"), 0, wxLEFT | wxTOP, 5);
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

        m_notebook->AddPage(attributePage, "Attributes", true);
        m_notebook->AddPage(signalPage, "Signal collection");
        m_notebook->AddPage(parameterPage, "Parameter collection");
        m_notebook->AddPage(schedulingPage, "Scheduling collection");

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
                LoadMetaModel(candidate, false);
                if (m_metaModel.IsLoaded()) 
                {
                    return;
                }
            }
        }
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

    void MainFrame::OnSaveSpecificationAs(wxCommandEvent&) 
    {
        if (!m_specification.IsLoaded()) 
        {
            wxMessageBox("Read a function specification first.", "AutoAPI Component Designer",
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
            wxMessageBox("Could not write file:\n" + ToWx(error), "AutoAPI Component Designer",
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
        LoadMetaModel(dialog.GetPath(), true);
        RefreshAll();
    }

    void MainFrame::OnAbout(wxCommandEvent&) 
    {
        wxMessageBox("AutoAPI Component Designer\n\n"
                    "Reads, displays and writes Eclipse autoapiframework function\n"
                    "specifications (*.acs, YAML content) based on the framework meta model.",
                    "About AutoAPI Component Designer", wxOK | wxICON_INFORMATION, this);
    }

    void MainFrame::OnExit(wxCommandEvent&) { Close(true); }

    void MainFrame::LoadSpecification(const wxString& path) 
    {
        std::string error;
        FunctionSpecification specification;
        if (!specification.Load(ToStd(path), error)) 
        {
            wxMessageBox("Could not read function specification:\n" + ToWx(error),
                        "AutoAPI Component Designer", wxOK | wxICON_ERROR, this);
            return;
        }
        m_specification = std::move(specification);
        m_saveButton->Enable(true);
        RefreshAll();
        UpdateTitleAndStatus();
    }

    void MainFrame::LoadMetaModel(const wxString& path, bool reportErrors) 
    {
        std::string error;
        if (!m_metaModel.Load(ToStd(path), error)) 
        {
            if (reportErrors) 
            {
                wxMessageBox("Could not read meta model:\n" + ToWx(error), "AutoAPI Component Designer",
                            wxOK | wxICON_ERROR, this);
            }
            return;
        }
        m_metaModelLabel->SetLabel("Meta model: " + ToWx(m_metaModel.GetName()) + " " +
                                ToWx(m_metaModel.GetVersion()));
        m_metaModelLabel->GetParent()->Layout();
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
    }

    void MainFrame::FillAttributes() 
    {
        m_attributeList->DeleteAllItems();
        long row = 0;
        for (const auto& attribute : m_specification.GetAttributes()) 
        {
            m_attributeList->InsertItem(row, ToWx(attribute.first));
            m_attributeList->SetItem(row, 1, OneLine(attribute.second));
            ++row;
        }
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

    void MainFrame::UpdateTitleAndStatus() 
    {
        if (!m_specification.IsLoaded()) 
        {
            SetTitle("AutoAPI Component Designer");
            SetStatusText("No function specification loaded", 0);
            SetStatusText(m_metaModel.IsLoaded() ? "Meta model loaded" : "No meta model", 1);
            return;
        }
        SetTitle("AutoAPI Component Designer - " + ToWx(m_specification.GetName()) + " [" +
                wxFileName(ToWx(m_specification.GetSourcePath())).GetFullName() + "]");
        SetStatusText("Read: " + ToWx(m_specification.GetSourcePath()), 0);
        SetStatusText("Meta model reference: " + ToWx(m_specification.GetMetaModelName()) + " " +
                        ToWx(m_specification.GetMetaModelVersion()),
                    1);
    }

} // namespace acd
