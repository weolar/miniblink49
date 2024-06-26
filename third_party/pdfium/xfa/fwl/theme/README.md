xfa/fwl/theme contains code for rendering XFA widgets.

TP stands for Theme Part.

CFWL_WidgetTP contains much of the code common to more than one widget.

The other CFWL_TP classes derive from it and know how to draw the pieces
specific to their respective widget.

The inheritance hierarchy for this directory is:

* CFWL_WidgetTP
    * CFWL_BarcodeTP
    * CFWL_CaretTP
    * CFWL_CheckboxTP
    * CFWL_ComboBowTP
    * CFWL_DateTimePickerTP
    * CFWL_EditTP
    * CFWL_ListBoxTP
    * CFWL_MonthCalendarTP
    * CFWL_PictureBoxTP
    * CFWL_PushButtonTP
    * CFWL_ScrollBarTP

All these widget TP classes are composed into CXFA_FWLTheme, which implements
IFWL_ThemeProvider (and is the only class that does). CXFA_FWLTheme receives
DrawBackground() calls from CFWL widgets to draw themselves and routes them to
the TP (Theme Part) corresponding to that widget.

