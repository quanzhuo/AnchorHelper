#pragma once

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "HelperFrame.h"

class HelperApp : public wxApp
{
public:
    virtual bool OnInit();

    HelperFrame *GetFrame();

private:
    HelperFrame *frame;
};
