// *******************************************************************************
// Copyright (c) 2026 Contributors to the Eclipse Foundation
//
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
//
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
//
// SPDX-License-Identifier: Apache-2.0
// *******************************************************************************

#ifndef ACD_MAINFRAME_H
#define ACD_MAINFRAME_H

#include <wx/wx.h>

#include <string>

#include "FunctionSpecification.h"
#include "MetaModel.h"

class wxListCtrl;
class wxNotebook;

namespace acd {

/// Main window of the AutomotiveComponentDesigner application.
class MainFrame : public wxFrame {
public:
    MainFrame();

private:
    enum {
        ID_OpenSpecification = wxID_HIGHEST + 1,
        ID_SaveSpecificationAs,
        ID_OpenMetaModel
    };

    void BuildUi();
    void AutoLoadMetaModel();

    void OnOpenSpecification(wxCommandEvent& event);
    void OnSaveSpecificationAs(wxCommandEvent& event);
    void OnOpenMetaModel(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);

    void LoadSpecification(const wxString& path);
    void LoadMetaModel(const wxString& path, bool reportErrors);

    void RefreshAll();
    void FillAttributes();
    void FillCollection(wxListCtrl* list, const std::string& collectionKey,
                        const std::string& interfaceTypeName);
    void UpdateTitleAndStatus();

    FunctionSpecification m_specification;
    MetaModel m_metaModel;

    wxNotebook* m_notebook = nullptr;
    wxListCtrl* m_attributeList = nullptr;
    wxListCtrl* m_signalList = nullptr;
    wxListCtrl* m_parameterList = nullptr;
    wxListCtrl* m_schedulingList = nullptr;
    wxTextCtrl* m_descriptionText = nullptr;
    wxStaticText* m_metaModelLabel = nullptr;
    wxButton* m_saveButton = nullptr;

    wxDECLARE_EVENT_TABLE();
};

} // namespace acd

#endif // ACD_MAINFRAME_H
