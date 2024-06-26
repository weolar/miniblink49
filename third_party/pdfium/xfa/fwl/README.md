xfa/fwl is a Widget Library for XFA Forms.

CFWL_Widget is the base class that widget classes extend. The derived widget
classes are both controllers and renderers for each widget. The hierarchy is:

* CFWL_Widget
    * CFWL_Form
    * CFWL_Caret
    * CFWL_CheckBox
    * CFWL_ComboBox
    * CFWL_DateTimePicker
    * CFWL_Edit
        * CFWL_Barcode
        * CFWL_ComboEdit
        * CFWL_DateTimeEdit
    * CFWL_ListBox
        * CFWL_ComboList
    * CFWL_MonthCalendar
    * CFWL_PictureBox
    * CFWL_PushButton
    * CFWL_ScrollBar
    * CFWL_SpinButton

These CFWL widgets are instantiated by and closely related to the CXFA classes
in the xfa/fxfa directory. See xfa/fxfa/README.md.

CFWL_Widget implements IFWL_WidgetDelegate through which it receives messages,
events and draw calls.

Messages consist of user input for a widget to handle. Each type of message is
identified by an enum value in Message::Type and has its own class derived from
the base CFWL_Message.

* CFWL_Message
    * CFWL_MessageKey
    * CFWL_MessageKillFocus
    * CFWL_MessageMouse
    * CFWL_MessageMouseWheel
    * CFWL_MessageSetFocus

Events are originated in the widgets and are then handled by other CFWL_Widget
and CXFA classes.

* CFWL_Event
    * CFWL_EventCheckWord
    * CFWL_EventMouse
    * CFWL_EventScroll
    * CFWL_EventSelectChanged
    * CFWL_EventTextWillChange
    * CFWL_EventValidate

The widgets use IFWL_ThemeProvider for rendering everything, calling
DrawBackground() and DrawText() and passing many options in their parameters,
respectively of types CFWL_ThemeBackground and CFWL_ThemeText. See
xfa/fwl/theme/README.md.

