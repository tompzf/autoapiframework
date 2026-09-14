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
 
#ifndef ACD_MAIN_FRAME_H
#define ACD_MAIN_FRAME_H

#include <wx/wx.h>

#include <string>

#include "function_specification.h"
#include "meta_model.h"

class wxListCtrl;
class wxNotebook;

namespace acd 
{
    /// Main window of the AutoAPI Component Designer application.
    class MainFrame : public wxFrame 
    {
    public:
        MainFrame();

    private:
        enum 
        {
            ID_OpenSpecification = wxID_HIGHEST + 1,
            ID_SaveSpecification,
            ID_SaveSpecificationAs,
            ID_OpenMetaModel,
            ID_Add,
            ID_Edit,
            ID_Validation
        };

        void BuildUi();
        void AutoLoadMetaModel();

        void OnOpenSpecification(wxCommandEvent& event);
        void OnSaveSpecification(wxCommandEvent& event);
        void OnSaveSpecificationAs(wxCommandEvent& event);
        void OnOpenMetaModel(wxCommandEvent& event);
        void OnAdd(wxCommandEvent& event);
        void OnEdit(wxCommandEvent& event);
        void OnValidation(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnExit(wxCommandEvent& event);

        void LoadSpecification(const wxString& path);
        std::string LoadMetaModel(const wxString& path, bool reportErrors);

        void RefreshAll();
        void FillAttributes();
        void FillCollection(wxListCtrl* list, const std::string& collectionKey,
                            const std::string& interfaceTypeName);
        wxListCtrl* GetSelectedCollectionList() const;
        void UpdateCollectionButtonStates();
        void UpdateTitleAndStatus();

        FunctionSpecification m_specification;
        MetaModel   m_metaModel;
        std::string m_metaModelVersion = "";

        wxNotebook* m_notebook = nullptr;
        wxListCtrl* m_attributeList = nullptr;
        wxListCtrl* m_signalList = nullptr;
        wxListCtrl* m_parameterList = nullptr;
        wxListCtrl* m_schedulingList = nullptr;
        wxTextCtrl* m_nameText = nullptr;
        wxTextCtrl* m_versionText = nullptr;
        wxTextCtrl* m_descriptionText = nullptr;
        wxStaticText* m_metaModelLabel = nullptr;
        wxButton* m_saveButton = nullptr;
        wxButton* m_saveAsButton = nullptr;
        wxButton* m_addButton = nullptr;
        wxButton* m_editButton = nullptr;
        wxButton* m_validationButton = nullptr;

        wxDECLARE_EVENT_TABLE();
    };

} // namespace acd

#endif // ACD_MAIN_FRAME_H
