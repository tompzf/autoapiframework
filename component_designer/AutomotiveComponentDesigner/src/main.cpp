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

#include <wx/wx.h>

#include "MainFrame.h"

namespace acd {

class DesignerApp : public wxApp {
public:
    bool OnInit() override {
        SetAppName("AutomotiveComponentDesigner");
        MainFrame* frame = new MainFrame();
        frame->Show(true);
        return true;
    }
};

} // namespace acd

wxIMPLEMENT_APP(acd::DesignerApp);
