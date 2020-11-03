#include "HelperApp.h"
#include "HelperFrame.h"
#include "Discovery.h"

wxIMPLEMENT_APP(HelperApp);

bool HelperApp::OnInit()
{
    HelperFrame *helper = new HelperFrame(_T("AnchorHelper"));
    helper->Show();
    frame = helper;
    // start Discovery thread
    Discovery *discovery = new Discovery(helper);
    discovery->Start();

    return true;
}

HelperFrame *HelperApp::GetFrame()
{
    return frame;
}
