xfa/fxfa contains a set of CXFA_LayoutItems that model forms containing widgets.

The main hierarchy in this directory are the form elements:

* CXFA_LayoutItem
    * CXFA_ContentLayoutItem
        * CXFA_FFPageView
        * CXFA_FFWidget
            * CXFA_FFArc
            * CXFA_FFImage
            * CXFA_FFLine
            * CXFA_FFRectangle
            * CXFA_FFText
            * CXFA_FFExclGroup
            * CXFA_FFField
                * CXFA_FFCheckButton
                * CXFA_FFComboBox
                * CXFA_FFImageEdit
                * CXFA_FFListBox
                * CXFA_FFPushButton
                * CXFA_FFSignature
                * CXFA_FFTextEdit
                    * CXFA_FFBarcode
                    * CXFA_FFDateTimeEdit
                    * CXFA_FFNumericEdit
                    * CXFA_FFPasswordEdit

CXFA_FFField is the base class for widgets. It owns a lower level CFWL widget
instance from xfa/fwl. The correspondence is:

* CXFA_FFCheckButton -> CFWL_CheckBox
* CXFA_FFComboBox -> CFWL_ComboBox
* CXFA_FFImageEdit -> CFWL_PictureBox
* CXFA_FFListBox -> CFWL_ListBox
* CXFA_FFPushButton -> CFWL_PushButton
* CXFA_FFSignature -> none
* CXFA_FFTextEdit -> CFWL_Edit
    * CXFA_FFBarcode -> CFWL_Barcode
    * CXFA_FFDateTimeEdit -> CFWL_DateTimePicker
    * CXFA_FFNumericEdit -> CFWL_Edit
    * CXFA_FFPasswordEdit > CFWL_Edit

