fpdfsdk/pwl is a Widget Library for AcroForms.

CPWL_Wnd is the base class that widget classes extend. The derived widget
classes are controllers for each widget. The hierarchy is:

* CPWL_Wnd
    * CPWL_Button
        * CPWL_CheckBox
        * CPWL_PushButton
        * CPWL_RadioButton
    * CPWL_Caret
    * CPWL_EditCtrl
        * CPWL_Edit
    * CPWL_Icon
    * CPWL_ListBox
        * CPWL_CBListBox (combo box)
    * CPWL_ScrollBar

Widgets are rendered to Appearance Streams, with the case all centralized in
CPWL_AppStream.

