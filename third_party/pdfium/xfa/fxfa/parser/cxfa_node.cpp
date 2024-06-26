// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_node.h"

#include <algorithm>
#include <map>
#include <memory>
#include <set>
#include <utility>
#include <vector>

#include "core/fxcrt/autorestorer.h"
#include "core/fxcrt/cfx_readonlymemorystream.h"
#include "core/fxcrt/fx_codepage.h"
#include "core/fxcrt/fx_extension.h"
#include "core/fxcrt/xml/cfx_xmldocument.h"
#include "core/fxcrt/xml/cfx_xmlelement.h"
#include "core/fxcrt/xml/cfx_xmlnode.h"
#include "core/fxcrt/xml/cfx_xmltext.h"
#include "core/fxge/dib/cfx_dibitmap.h"
#include "fxjs/xfa/cfxjse_engine.h"
#include "fxjs/xfa/cfxjse_value.h"
#include "fxjs/xfa/cjx_node.h"
#include "third_party/base/compiler_specific.h"
#include "third_party/base/logging.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/span.h"
#include "third_party/base/stl_util.h"
#include "xfa/fde/cfde_textout.h"
#include "xfa/fgas/crt/cfgas_decimal.h"
#include "xfa/fgas/crt/locale_iface.h"
#include "xfa/fgas/font/cfgas_fontmgr.h"
#include "xfa/fgas/font/cfgas_gefont.h"
#include "xfa/fxfa/cxfa_eventparam.h"
#include "xfa/fxfa/cxfa_ffapp.h"
#include "xfa/fxfa/cxfa_ffdocview.h"
#include "xfa/fxfa/cxfa_ffnotify.h"
#include "xfa/fxfa/cxfa_ffwidget.h"
#include "xfa/fxfa/cxfa_fontmgr.h"
#include "xfa/fxfa/cxfa_textprovider.h"
#include "xfa/fxfa/parser/cxfa_accessiblecontent.h"
#include "xfa/fxfa/parser/cxfa_acrobat.h"
#include "xfa/fxfa/parser/cxfa_acrobat7.h"
#include "xfa/fxfa/parser/cxfa_adbe_jsconsole.h"
#include "xfa/fxfa/parser/cxfa_adbe_jsdebugger.h"
#include "xfa/fxfa/parser/cxfa_addsilentprint.h"
#include "xfa/fxfa/parser/cxfa_addviewerpreferences.h"
#include "xfa/fxfa/parser/cxfa_adjustdata.h"
#include "xfa/fxfa/parser/cxfa_adobeextensionlevel.h"
#include "xfa/fxfa/parser/cxfa_agent.h"
#include "xfa/fxfa/parser/cxfa_alwaysembed.h"
#include "xfa/fxfa/parser/cxfa_amd.h"
#include "xfa/fxfa/parser/cxfa_appearancefilter.h"
#include "xfa/fxfa/parser/cxfa_arc.h"
#include "xfa/fxfa/parser/cxfa_area.h"
#include "xfa/fxfa/parser/cxfa_arraynodelist.h"
#include "xfa/fxfa/parser/cxfa_assist.h"
#include "xfa/fxfa/parser/cxfa_attachnodelist.h"
#include "xfa/fxfa/parser/cxfa_attributes.h"
#include "xfa/fxfa/parser/cxfa_autosave.h"
#include "xfa/fxfa/parser/cxfa_barcode.h"
#include "xfa/fxfa/parser/cxfa_base.h"
#include "xfa/fxfa/parser/cxfa_batchoutput.h"
#include "xfa/fxfa/parser/cxfa_behavioroverride.h"
#include "xfa/fxfa/parser/cxfa_bind.h"
#include "xfa/fxfa/parser/cxfa_binditems.h"
#include "xfa/fxfa/parser/cxfa_bookend.h"
#include "xfa/fxfa/parser/cxfa_boolean.h"
#include "xfa/fxfa/parser/cxfa_border.h"
#include "xfa/fxfa/parser/cxfa_break.h"
#include "xfa/fxfa/parser/cxfa_breakafter.h"
#include "xfa/fxfa/parser/cxfa_breakbefore.h"
#include "xfa/fxfa/parser/cxfa_button.h"
#include "xfa/fxfa/parser/cxfa_cache.h"
#include "xfa/fxfa/parser/cxfa_calculate.h"
#include "xfa/fxfa/parser/cxfa_calendarsymbols.h"
#include "xfa/fxfa/parser/cxfa_caption.h"
#include "xfa/fxfa/parser/cxfa_certificate.h"
#include "xfa/fxfa/parser/cxfa_certificates.h"
#include "xfa/fxfa/parser/cxfa_change.h"
#include "xfa/fxfa/parser/cxfa_checkbutton.h"
#include "xfa/fxfa/parser/cxfa_choicelist.h"
#include "xfa/fxfa/parser/cxfa_color.h"
#include "xfa/fxfa/parser/cxfa_comb.h"
#include "xfa/fxfa/parser/cxfa_command.h"
#include "xfa/fxfa/parser/cxfa_common.h"
#include "xfa/fxfa/parser/cxfa_compress.h"
#include "xfa/fxfa/parser/cxfa_compression.h"
#include "xfa/fxfa/parser/cxfa_compresslogicalstructure.h"
#include "xfa/fxfa/parser/cxfa_compressobjectstream.h"
#include "xfa/fxfa/parser/cxfa_config.h"
#include "xfa/fxfa/parser/cxfa_conformance.h"
#include "xfa/fxfa/parser/cxfa_connect.h"
#include "xfa/fxfa/parser/cxfa_connectionset.h"
#include "xfa/fxfa/parser/cxfa_connectstring.h"
#include "xfa/fxfa/parser/cxfa_contentarea.h"
#include "xfa/fxfa/parser/cxfa_contentcopy.h"
#include "xfa/fxfa/parser/cxfa_copies.h"
#include "xfa/fxfa/parser/cxfa_corner.h"
#include "xfa/fxfa/parser/cxfa_creator.h"
#include "xfa/fxfa/parser/cxfa_currencysymbol.h"
#include "xfa/fxfa/parser/cxfa_currencysymbols.h"
#include "xfa/fxfa/parser/cxfa_currentpage.h"
#include "xfa/fxfa/parser/cxfa_data.h"
#include "xfa/fxfa/parser/cxfa_datagroup.h"
#include "xfa/fxfa/parser/cxfa_datamodel.h"
#include "xfa/fxfa/parser/cxfa_datavalue.h"
#include "xfa/fxfa/parser/cxfa_date.h"
#include "xfa/fxfa/parser/cxfa_datepattern.h"
#include "xfa/fxfa/parser/cxfa_datepatterns.h"
#include "xfa/fxfa/parser/cxfa_datetime.h"
#include "xfa/fxfa/parser/cxfa_datetimeedit.h"
#include "xfa/fxfa/parser/cxfa_datetimesymbols.h"
#include "xfa/fxfa/parser/cxfa_day.h"
#include "xfa/fxfa/parser/cxfa_daynames.h"
#include "xfa/fxfa/parser/cxfa_debug.h"
#include "xfa/fxfa/parser/cxfa_decimal.h"
#include "xfa/fxfa/parser/cxfa_defaulttypeface.h"
#include "xfa/fxfa/parser/cxfa_defaultui.h"
#include "xfa/fxfa/parser/cxfa_delete.h"
#include "xfa/fxfa/parser/cxfa_delta.h"
#include "xfa/fxfa/parser/cxfa_deltas.h"
#include "xfa/fxfa/parser/cxfa_desc.h"
#include "xfa/fxfa/parser/cxfa_destination.h"
#include "xfa/fxfa/parser/cxfa_digestmethod.h"
#include "xfa/fxfa/parser/cxfa_digestmethods.h"
#include "xfa/fxfa/parser/cxfa_document.h"
#include "xfa/fxfa/parser/cxfa_document_parser.h"
#include "xfa/fxfa/parser/cxfa_documentassembly.h"
#include "xfa/fxfa/parser/cxfa_draw.h"
#include "xfa/fxfa/parser/cxfa_driver.h"
#include "xfa/fxfa/parser/cxfa_dsigdata.h"
#include "xfa/fxfa/parser/cxfa_duplexoption.h"
#include "xfa/fxfa/parser/cxfa_dynamicrender.h"
#include "xfa/fxfa/parser/cxfa_edge.h"
#include "xfa/fxfa/parser/cxfa_effectiveinputpolicy.h"
#include "xfa/fxfa/parser/cxfa_effectiveoutputpolicy.h"
#include "xfa/fxfa/parser/cxfa_embed.h"
#include "xfa/fxfa/parser/cxfa_encoding.h"
#include "xfa/fxfa/parser/cxfa_encodings.h"
#include "xfa/fxfa/parser/cxfa_encrypt.h"
#include "xfa/fxfa/parser/cxfa_encryption.h"
#include "xfa/fxfa/parser/cxfa_encryptionlevel.h"
#include "xfa/fxfa/parser/cxfa_encryptionmethod.h"
#include "xfa/fxfa/parser/cxfa_encryptionmethods.h"
#include "xfa/fxfa/parser/cxfa_enforce.h"
#include "xfa/fxfa/parser/cxfa_equate.h"
#include "xfa/fxfa/parser/cxfa_equaterange.h"
#include "xfa/fxfa/parser/cxfa_era.h"
#include "xfa/fxfa/parser/cxfa_eranames.h"
#include "xfa/fxfa/parser/cxfa_event.h"
#include "xfa/fxfa/parser/cxfa_exclgroup.h"
#include "xfa/fxfa/parser/cxfa_exclude.h"
#include "xfa/fxfa/parser/cxfa_excludens.h"
#include "xfa/fxfa/parser/cxfa_exdata.h"
#include "xfa/fxfa/parser/cxfa_execute.h"
#include "xfa/fxfa/parser/cxfa_exobject.h"
#include "xfa/fxfa/parser/cxfa_extras.h"
#include "xfa/fxfa/parser/cxfa_field.h"
#include "xfa/fxfa/parser/cxfa_fill.h"
#include "xfa/fxfa/parser/cxfa_filter.h"
#include "xfa/fxfa/parser/cxfa_fliplabel.h"
#include "xfa/fxfa/parser/cxfa_float.h"
#include "xfa/fxfa/parser/cxfa_font.h"
#include "xfa/fxfa/parser/cxfa_fontinfo.h"
#include "xfa/fxfa/parser/cxfa_form.h"
#include "xfa/fxfa/parser/cxfa_format.h"
#include "xfa/fxfa/parser/cxfa_formfieldfilling.h"
#include "xfa/fxfa/parser/cxfa_groupparent.h"
#include "xfa/fxfa/parser/cxfa_handler.h"
#include "xfa/fxfa/parser/cxfa_hyphenation.h"
#include "xfa/fxfa/parser/cxfa_ifempty.h"
#include "xfa/fxfa/parser/cxfa_image.h"
#include "xfa/fxfa/parser/cxfa_imageedit.h"
#include "xfa/fxfa/parser/cxfa_includexdpcontent.h"
#include "xfa/fxfa/parser/cxfa_incrementalload.h"
#include "xfa/fxfa/parser/cxfa_incrementalmerge.h"
#include "xfa/fxfa/parser/cxfa_insert.h"
#include "xfa/fxfa/parser/cxfa_instancemanager.h"
#include "xfa/fxfa/parser/cxfa_integer.h"
#include "xfa/fxfa/parser/cxfa_interactive.h"
#include "xfa/fxfa/parser/cxfa_issuers.h"
#include "xfa/fxfa/parser/cxfa_items.h"
#include "xfa/fxfa/parser/cxfa_jog.h"
#include "xfa/fxfa/parser/cxfa_keep.h"
#include "xfa/fxfa/parser/cxfa_keyusage.h"
#include "xfa/fxfa/parser/cxfa_labelprinter.h"
#include "xfa/fxfa/parser/cxfa_layout.h"
#include "xfa/fxfa/parser/cxfa_layoutprocessor.h"
#include "xfa/fxfa/parser/cxfa_level.h"
#include "xfa/fxfa/parser/cxfa_line.h"
#include "xfa/fxfa/parser/cxfa_linear.h"
#include "xfa/fxfa/parser/cxfa_linearized.h"
#include "xfa/fxfa/parser/cxfa_locale.h"
#include "xfa/fxfa/parser/cxfa_localeset.h"
#include "xfa/fxfa/parser/cxfa_localevalue.h"
#include "xfa/fxfa/parser/cxfa_lockdocument.h"
#include "xfa/fxfa/parser/cxfa_log.h"
#include "xfa/fxfa/parser/cxfa_manifest.h"
#include "xfa/fxfa/parser/cxfa_map.h"
#include "xfa/fxfa/parser/cxfa_margin.h"
#include "xfa/fxfa/parser/cxfa_mdp.h"
#include "xfa/fxfa/parser/cxfa_measurement.h"
#include "xfa/fxfa/parser/cxfa_medium.h"
#include "xfa/fxfa/parser/cxfa_mediuminfo.h"
#include "xfa/fxfa/parser/cxfa_meridiem.h"
#include "xfa/fxfa/parser/cxfa_meridiemnames.h"
#include "xfa/fxfa/parser/cxfa_message.h"
#include "xfa/fxfa/parser/cxfa_messaging.h"
#include "xfa/fxfa/parser/cxfa_mode.h"
#include "xfa/fxfa/parser/cxfa_modifyannots.h"
#include "xfa/fxfa/parser/cxfa_month.h"
#include "xfa/fxfa/parser/cxfa_monthnames.h"
#include "xfa/fxfa/parser/cxfa_msgid.h"
#include "xfa/fxfa/parser/cxfa_nameattr.h"
#include "xfa/fxfa/parser/cxfa_neverembed.h"
#include "xfa/fxfa/parser/cxfa_nodeiteratortemplate.h"
#include "xfa/fxfa/parser/cxfa_numberofcopies.h"
#include "xfa/fxfa/parser/cxfa_numberpattern.h"
#include "xfa/fxfa/parser/cxfa_numberpatterns.h"
#include "xfa/fxfa/parser/cxfa_numbersymbol.h"
#include "xfa/fxfa/parser/cxfa_numbersymbols.h"
#include "xfa/fxfa/parser/cxfa_numericedit.h"
#include "xfa/fxfa/parser/cxfa_occur.h"
#include "xfa/fxfa/parser/cxfa_oid.h"
#include "xfa/fxfa/parser/cxfa_oids.h"
#include "xfa/fxfa/parser/cxfa_openaction.h"
#include "xfa/fxfa/parser/cxfa_operation.h"
#include "xfa/fxfa/parser/cxfa_output.h"
#include "xfa/fxfa/parser/cxfa_outputbin.h"
#include "xfa/fxfa/parser/cxfa_outputxsl.h"
#include "xfa/fxfa/parser/cxfa_overflow.h"
#include "xfa/fxfa/parser/cxfa_overprint.h"
#include "xfa/fxfa/parser/cxfa_packet.h"
#include "xfa/fxfa/parser/cxfa_packets.h"
#include "xfa/fxfa/parser/cxfa_pagearea.h"
#include "xfa/fxfa/parser/cxfa_pageoffset.h"
#include "xfa/fxfa/parser/cxfa_pagerange.h"
#include "xfa/fxfa/parser/cxfa_pageset.h"
#include "xfa/fxfa/parser/cxfa_pagination.h"
#include "xfa/fxfa/parser/cxfa_paginationoverride.h"
#include "xfa/fxfa/parser/cxfa_para.h"
#include "xfa/fxfa/parser/cxfa_part.h"
#include "xfa/fxfa/parser/cxfa_password.h"
#include "xfa/fxfa/parser/cxfa_passwordedit.h"
#include "xfa/fxfa/parser/cxfa_pattern.h"
#include "xfa/fxfa/parser/cxfa_pcl.h"
#include "xfa/fxfa/parser/cxfa_pdf.h"
#include "xfa/fxfa/parser/cxfa_pdfa.h"
#include "xfa/fxfa/parser/cxfa_permissions.h"
#include "xfa/fxfa/parser/cxfa_picktraybypdfsize.h"
#include "xfa/fxfa/parser/cxfa_picture.h"
#include "xfa/fxfa/parser/cxfa_plaintextmetadata.h"
#include "xfa/fxfa/parser/cxfa_presence.h"
#include "xfa/fxfa/parser/cxfa_present.h"
#include "xfa/fxfa/parser/cxfa_print.h"
#include "xfa/fxfa/parser/cxfa_printername.h"
#include "xfa/fxfa/parser/cxfa_printhighquality.h"
#include "xfa/fxfa/parser/cxfa_printscaling.h"
#include "xfa/fxfa/parser/cxfa_producer.h"
#include "xfa/fxfa/parser/cxfa_proto.h"
#include "xfa/fxfa/parser/cxfa_ps.h"
#include "xfa/fxfa/parser/cxfa_psmap.h"
#include "xfa/fxfa/parser/cxfa_query.h"
#include "xfa/fxfa/parser/cxfa_radial.h"
#include "xfa/fxfa/parser/cxfa_range.h"
#include "xfa/fxfa/parser/cxfa_reason.h"
#include "xfa/fxfa/parser/cxfa_reasons.h"
#include "xfa/fxfa/parser/cxfa_record.h"
#include "xfa/fxfa/parser/cxfa_recordset.h"
#include "xfa/fxfa/parser/cxfa_rectangle.h"
#include "xfa/fxfa/parser/cxfa_ref.h"
#include "xfa/fxfa/parser/cxfa_relevant.h"
#include "xfa/fxfa/parser/cxfa_rename.h"
#include "xfa/fxfa/parser/cxfa_renderpolicy.h"
#include "xfa/fxfa/parser/cxfa_rootelement.h"
#include "xfa/fxfa/parser/cxfa_runscripts.h"
#include "xfa/fxfa/parser/cxfa_script.h"
#include "xfa/fxfa/parser/cxfa_scriptmodel.h"
#include "xfa/fxfa/parser/cxfa_select.h"
#include "xfa/fxfa/parser/cxfa_setproperty.h"
#include "xfa/fxfa/parser/cxfa_severity.h"
#include "xfa/fxfa/parser/cxfa_sharptext.h"
#include "xfa/fxfa/parser/cxfa_sharpxhtml.h"
#include "xfa/fxfa/parser/cxfa_sharpxml.h"
#include "xfa/fxfa/parser/cxfa_signature.h"
#include "xfa/fxfa/parser/cxfa_signatureproperties.h"
#include "xfa/fxfa/parser/cxfa_signdata.h"
#include "xfa/fxfa/parser/cxfa_signing.h"
#include "xfa/fxfa/parser/cxfa_silentprint.h"
#include "xfa/fxfa/parser/cxfa_soapaction.h"
#include "xfa/fxfa/parser/cxfa_soapaddress.h"
#include "xfa/fxfa/parser/cxfa_solid.h"
#include "xfa/fxfa/parser/cxfa_source.h"
#include "xfa/fxfa/parser/cxfa_sourceset.h"
#include "xfa/fxfa/parser/cxfa_speak.h"
#include "xfa/fxfa/parser/cxfa_staple.h"
#include "xfa/fxfa/parser/cxfa_startnode.h"
#include "xfa/fxfa/parser/cxfa_startpage.h"
#include "xfa/fxfa/parser/cxfa_stipple.h"
#include "xfa/fxfa/parser/cxfa_stroke.h"
#include "xfa/fxfa/parser/cxfa_subform.h"
#include "xfa/fxfa/parser/cxfa_subformset.h"
#include "xfa/fxfa/parser/cxfa_subjectdn.h"
#include "xfa/fxfa/parser/cxfa_subjectdns.h"
#include "xfa/fxfa/parser/cxfa_submit.h"
#include "xfa/fxfa/parser/cxfa_submitformat.h"
#include "xfa/fxfa/parser/cxfa_submiturl.h"
#include "xfa/fxfa/parser/cxfa_subsetbelow.h"
#include "xfa/fxfa/parser/cxfa_suppressbanner.h"
#include "xfa/fxfa/parser/cxfa_tagged.h"
#include "xfa/fxfa/parser/cxfa_template.h"
#include "xfa/fxfa/parser/cxfa_templatecache.h"
#include "xfa/fxfa/parser/cxfa_text.h"
#include "xfa/fxfa/parser/cxfa_textedit.h"
#include "xfa/fxfa/parser/cxfa_threshold.h"
#include "xfa/fxfa/parser/cxfa_time.h"
#include "xfa/fxfa/parser/cxfa_timepattern.h"
#include "xfa/fxfa/parser/cxfa_timepatterns.h"
#include "xfa/fxfa/parser/cxfa_timestamp.h"
#include "xfa/fxfa/parser/cxfa_to.h"
#include "xfa/fxfa/parser/cxfa_tooltip.h"
#include "xfa/fxfa/parser/cxfa_trace.h"
#include "xfa/fxfa/parser/cxfa_transform.h"
#include "xfa/fxfa/parser/cxfa_traversal.h"
#include "xfa/fxfa/parser/cxfa_traverse.h"
#include "xfa/fxfa/parser/cxfa_traversestrategy_xfacontainernode.h"
#include "xfa/fxfa/parser/cxfa_type.h"
#include "xfa/fxfa/parser/cxfa_typeface.h"
#include "xfa/fxfa/parser/cxfa_typefaces.h"
#include "xfa/fxfa/parser/cxfa_ui.h"
#include "xfa/fxfa/parser/cxfa_update.h"
#include "xfa/fxfa/parser/cxfa_uri.h"
#include "xfa/fxfa/parser/cxfa_user.h"
#include "xfa/fxfa/parser/cxfa_validate.h"
#include "xfa/fxfa/parser/cxfa_validateapprovalsignatures.h"
#include "xfa/fxfa/parser/cxfa_validationmessaging.h"
#include "xfa/fxfa/parser/cxfa_value.h"
#include "xfa/fxfa/parser/cxfa_variables.h"
#include "xfa/fxfa/parser/cxfa_version.h"
#include "xfa/fxfa/parser/cxfa_versioncontrol.h"
#include "xfa/fxfa/parser/cxfa_viewerpreferences.h"
#include "xfa/fxfa/parser/cxfa_webclient.h"
#include "xfa/fxfa/parser/cxfa_whitespace.h"
#include "xfa/fxfa/parser/cxfa_window.h"
#include "xfa/fxfa/parser/cxfa_wsdladdress.h"
#include "xfa/fxfa/parser/cxfa_wsdlconnection.h"
#include "xfa/fxfa/parser/cxfa_xdc.h"
#include "xfa/fxfa/parser/cxfa_xdp.h"
#include "xfa/fxfa/parser/cxfa_xfa.h"
#include "xfa/fxfa/parser/cxfa_xmlconnection.h"
#include "xfa/fxfa/parser/cxfa_xsdconnection.h"
#include "xfa/fxfa/parser/cxfa_xsl.h"
#include "xfa/fxfa/parser/cxfa_zpl.h"
#include "xfa/fxfa/parser/xfa_basic_data.h"
#include "xfa/fxfa/parser/xfa_utils.h"

class CXFA_FieldLayoutData;
class CXFA_ImageEditData;
class CXFA_ImageLayoutData;
class CXFA_TextEditData;
class CXFA_TextLayoutData;

namespace {

constexpr uint8_t kMaxExecuteRecursion = 2;

constexpr uint8_t g_inv_base64[128] = {
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 62,  255,
    255, 255, 63,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  255, 255,
    255, 255, 255, 255, 255, 0,   1,   2,   3,   4,   5,   6,   7,   8,   9,
    10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
    25,  255, 255, 255, 255, 255, 255, 26,  27,  28,  29,  30,  31,  32,  33,
    34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
    49,  50,  51,  255, 255, 255, 255, 255,
};

uint8_t* XFA_RemoveBase64Whitespace(const uint8_t* pStr, int32_t iLen) {
  uint8_t* pCP;
  int32_t i = 0, j = 0;
  if (iLen == 0) {
    iLen = strlen((char*)pStr);
  }
  pCP = FX_Alloc(uint8_t, iLen + 1);
  for (; i < iLen; i++) {
    if ((pStr[i] & 128) == 0) {
      if (g_inv_base64[pStr[i]] != 0xFF || pStr[i] == '=') {
        pCP[j++] = pStr[i];
      }
    }
  }
  pCP[j] = '\0';
  return pCP;
}

int32_t XFA_Base64Decode(const char* pStr, uint8_t* pOutBuffer) {
  if (!pStr) {
    return 0;
  }
  uint8_t* pBuffer =
      XFA_RemoveBase64Whitespace((uint8_t*)pStr, strlen((char*)pStr));
  if (!pBuffer) {
    return 0;
  }
  int32_t iLen = strlen((char*)pBuffer);
  int32_t i = 0, j = 0;
  uint32_t dwLimb = 0;
  for (; i + 3 < iLen; i += 4) {
    if (pBuffer[i] == '=' || pBuffer[i + 1] == '=' || pBuffer[i + 2] == '=' ||
        pBuffer[i + 3] == '=') {
      if (pBuffer[i] == '=' || pBuffer[i + 1] == '=') {
        break;
      }
      if (pBuffer[i + 2] == '=') {
        dwLimb = ((uint32_t)g_inv_base64[pBuffer[i]] << 6) |
                 ((uint32_t)g_inv_base64[pBuffer[i + 1]]);
        pOutBuffer[j] = (uint8_t)(dwLimb >> 4) & 0xFF;
        j++;
      } else {
        dwLimb = ((uint32_t)g_inv_base64[pBuffer[i]] << 12) |
                 ((uint32_t)g_inv_base64[pBuffer[i + 1]] << 6) |
                 ((uint32_t)g_inv_base64[pBuffer[i + 2]]);
        pOutBuffer[j] = (uint8_t)(dwLimb >> 10) & 0xFF;
        pOutBuffer[j + 1] = (uint8_t)(dwLimb >> 2) & 0xFF;
        j += 2;
      }
    } else {
      dwLimb = ((uint32_t)g_inv_base64[pBuffer[i]] << 18) |
               ((uint32_t)g_inv_base64[pBuffer[i + 1]] << 12) |
               ((uint32_t)g_inv_base64[pBuffer[i + 2]] << 6) |
               ((uint32_t)g_inv_base64[pBuffer[i + 3]]);
      pOutBuffer[j] = (uint8_t)(dwLimb >> 16) & 0xff;
      pOutBuffer[j + 1] = (uint8_t)(dwLimb >> 8) & 0xff;
      pOutBuffer[j + 2] = (uint8_t)(dwLimb)&0xff;
      j += 3;
    }
  }
  FX_Free(pBuffer);
  return j;
}

FXCODEC_IMAGE_TYPE XFA_GetImageType(const WideString& wsType) {
  WideString wsContentType(wsType);
  if (wsContentType.EqualsASCIINoCase("image/jpg"))
    return FXCODEC_IMAGE_JPG;

#ifdef PDF_ENABLE_XFA_BMP
  if (wsContentType.EqualsASCIINoCase("image/bmp"))
    return FXCODEC_IMAGE_BMP;
#endif  // PDF_ENABLE_XFA_BMP

#ifdef PDF_ENABLE_XFA_GIF
  if (wsContentType.EqualsASCIINoCase("image/gif"))
    return FXCODEC_IMAGE_GIF;
#endif  // PDF_ENABLE_XFA_GIF

#ifdef PDF_ENABLE_XFA_PNG
  if (wsContentType.EqualsASCIINoCase("image/png"))
    return FXCODEC_IMAGE_PNG;
#endif  // PDF_ENABLE_XFA_PNG

#ifdef PDF_ENABLE_XFA_TIFF
  if (wsContentType.EqualsASCII("image/tif"))
    return FXCODEC_IMAGE_TIFF;
#endif  // PDF_ENABLE_XFA_TIFF

  return FXCODEC_IMAGE_UNKNOWN;
}

RetainPtr<CFX_DIBitmap> XFA_LoadImageData(CXFA_FFDoc* pDoc,
                                          CXFA_Image* pImage,
                                          bool& bNameImage,
                                          int32_t& iImageXDpi,
                                          int32_t& iImageYDpi) {
  WideString wsHref = pImage->GetHref();
  WideString wsImage = pImage->GetContent();
  if (wsHref.IsEmpty() && wsImage.IsEmpty())
    return nullptr;

  FXCODEC_IMAGE_TYPE type = XFA_GetImageType(pImage->GetContentType());
  std::vector<uint8_t> buffer;
  RetainPtr<IFX_SeekableReadStream> pImageFileRead;
  if (wsImage.GetLength() > 0) {
    XFA_AttributeValue iEncoding = pImage->GetTransferEncoding();
    if (iEncoding == XFA_AttributeValue::Base64) {
      ByteString bsData = wsImage.ToUTF8();
      buffer.resize(bsData.GetLength());
      int32_t iRead = XFA_Base64Decode(bsData.c_str(), buffer.data());
      if (iRead > 0) {
        pImageFileRead = pdfium::MakeRetain<CFX_ReadOnlyMemoryStream>(
            pdfium::make_span(buffer.data(), iRead));
      }
    } else {
      pImageFileRead = pdfium::MakeRetain<CFX_ReadOnlyMemoryStream>(
          wsImage.ToDefANSI().AsRawSpan());
    }
  } else {
    WideString wsURL = wsHref;
    if (!(wsURL.Left(7).EqualsASCII("http://") ||
          wsURL.Left(6).EqualsASCII("ftp://"))) {
      RetainPtr<CFX_DIBitmap> pBitmap =
          pDoc->GetPDFNamedImage(wsURL.AsStringView(), iImageXDpi, iImageYDpi);
      if (pBitmap) {
        bNameImage = true;
        return pBitmap;
      }
    }
    pImageFileRead = pDoc->GetDocEnvironment()->OpenLinkedFile(pDoc, wsURL);
  }
  if (!pImageFileRead)
    return nullptr;

  bNameImage = false;
  RetainPtr<CFX_DIBitmap> pBitmap =
      XFA_LoadImageFromBuffer(pImageFileRead, type, iImageXDpi, iImageYDpi);
  return pBitmap;
}

bool SplitDateTime(const WideString& wsDateTime,
                   WideString& wsDate,
                   WideString& wsTime) {
  wsDate.clear();
  wsTime.clear();
  if (wsDateTime.IsEmpty())
    return false;

  auto nSplitIndex = wsDateTime.Find('T');
  if (!nSplitIndex.has_value())
    nSplitIndex = wsDateTime.Find(' ');
  if (!nSplitIndex.has_value())
    return false;

  wsDate = wsDateTime.Left(nSplitIndex.value());
  if (!wsDate.IsEmpty()) {
    if (!std::any_of(wsDate.begin(), wsDate.end(),
                     [](wchar_t c) { return FXSYS_IsDecimalDigit(c); })) {
      return false;
    }
  }
  wsTime = wsDateTime.Right(wsDateTime.GetLength() - nSplitIndex.value() - 1);
  if (!wsTime.IsEmpty()) {
    if (!std::any_of(wsTime.begin(), wsTime.end(),
                     [](wchar_t c) { return FXSYS_IsDecimalDigit(c); })) {
      return false;
    }
  }
  return true;
}

std::vector<CXFA_Node*> NodesSortedByDocumentIdx(
    const std::set<CXFA_Node*>& rgNodeSet) {
  if (rgNodeSet.empty())
    return std::vector<CXFA_Node*>();

  std::vector<CXFA_Node*> rgNodeArray;
  CXFA_Node* pCommonParent = (*rgNodeSet.begin())->GetParent();
  for (CXFA_Node* pNode = pCommonParent->GetFirstChild(); pNode;
       pNode = pNode->GetNextSibling()) {
    if (pdfium::ContainsValue(rgNodeSet, pNode))
      rgNodeArray.push_back(pNode);
  }
  return rgNodeArray;
}

using CXFA_NodeSetPair = std::pair<std::set<CXFA_Node*>, std::set<CXFA_Node*>>;
using CXFA_NodeSetPairMap =
    std::map<uint32_t, std::unique_ptr<CXFA_NodeSetPair>>;
using CXFA_NodeSetPairMapMap =
    std::map<CXFA_Node*, std::unique_ptr<CXFA_NodeSetPairMap>>;

CXFA_NodeSetPair* NodeSetPairForNode(CXFA_Node* pNode,
                                     CXFA_NodeSetPairMapMap* pMap) {
  CXFA_Node* pParentNode = pNode->GetParent();
  uint32_t dwNameHash = pNode->GetNameHash();
  if (!pParentNode || !dwNameHash)
    return nullptr;

  if (!(*pMap)[pParentNode])
    (*pMap)[pParentNode] = pdfium::MakeUnique<CXFA_NodeSetPairMap>();

  CXFA_NodeSetPairMap* pNodeSetPairMap = (*pMap)[pParentNode].get();
  if (!(*pNodeSetPairMap)[dwNameHash])
    (*pNodeSetPairMap)[dwNameHash] = pdfium::MakeUnique<CXFA_NodeSetPair>();

  return (*pNodeSetPairMap)[dwNameHash].get();
}

void ReorderDataNodes(const std::set<CXFA_Node*>& sSet1,
                      const std::set<CXFA_Node*>& sSet2,
                      bool bInsertBefore) {
  CXFA_NodeSetPairMapMap rgMap;
  for (CXFA_Node* pNode : sSet1) {
    CXFA_NodeSetPair* pNodeSetPair = NodeSetPairForNode(pNode, &rgMap);
    if (pNodeSetPair)
      pNodeSetPair->first.insert(pNode);
  }
  for (CXFA_Node* pNode : sSet2) {
    CXFA_NodeSetPair* pNodeSetPair = NodeSetPairForNode(pNode, &rgMap);
    if (pNodeSetPair) {
      if (pdfium::ContainsValue(pNodeSetPair->first, pNode))
        pNodeSetPair->first.erase(pNode);
      else
        pNodeSetPair->second.insert(pNode);
    }
  }
  for (const auto& iter1 : rgMap) {
    CXFA_NodeSetPairMap* pNodeSetPairMap = iter1.second.get();
    if (!pNodeSetPairMap)
      continue;

    for (const auto& iter2 : *pNodeSetPairMap) {
      CXFA_NodeSetPair* pNodeSetPair = iter2.second.get();
      if (!pNodeSetPair)
        continue;
      if (!pNodeSetPair->first.empty() && !pNodeSetPair->second.empty()) {
        std::vector<CXFA_Node*> rgNodeArray1 =
            NodesSortedByDocumentIdx(pNodeSetPair->first);
        std::vector<CXFA_Node*> rgNodeArray2 =
            NodesSortedByDocumentIdx(pNodeSetPair->second);
        CXFA_Node* pParentNode = nullptr;
        CXFA_Node* pBeforeNode = nullptr;
        if (bInsertBefore) {
          pBeforeNode = rgNodeArray2.front();
          pParentNode = pBeforeNode->GetParent();
        } else {
          CXFA_Node* pLastNode = rgNodeArray2.back();
          pParentNode = pLastNode->GetParent();
          pBeforeNode = pLastNode->GetNextSibling();
        }
        for (auto* pCurNode : rgNodeArray1) {
          pParentNode->RemoveChild(pCurNode, true);
          pParentNode->InsertChild(pCurNode, pBeforeNode);
        }
      }
    }
    pNodeSetPairMap->clear();
  }
}

float GetEdgeThickness(const std::vector<CXFA_Stroke*>& strokes,
                       bool b3DStyle,
                       int32_t nIndex) {
  float fThickness = 0.0f;
  CXFA_Stroke* stroke = strokes[nIndex * 2 + 1];
  if (stroke->IsVisible()) {
    if (nIndex == 0)
      fThickness += 2.5f;

    fThickness += stroke->GetThickness() * (b3DStyle ? 4 : 2);
  }
  return fThickness;
}

WideString FormatNumStr(const WideString& wsValue, LocaleIface* pLocale) {
  if (wsValue.IsEmpty())
    return WideString();

  WideString wsSrcNum = wsValue;
  WideString wsGroupSymbol = pLocale->GetGroupingSymbol();
  bool bNeg = false;
  if (wsSrcNum[0] == '-') {
    bNeg = true;
    wsSrcNum.Delete(0, 1);
  }

  auto dot_index = wsSrcNum.Find('.');
  dot_index = !dot_index.has_value() ? wsSrcNum.GetLength() : dot_index;

  if (dot_index.value() < 1)
    return WideString();

  size_t nPos = dot_index.value() % 3;
  WideString wsOutput;
  for (size_t i = 0; i < dot_index.value(); i++) {
    if (i % 3 == nPos && i != 0)
      wsOutput += wsGroupSymbol;

    wsOutput += wsSrcNum[i];
  }
  if (dot_index.value() < wsSrcNum.GetLength()) {
    wsOutput += pLocale->GetDecimalSymbol();
    wsOutput += wsSrcNum.Right(wsSrcNum.GetLength() - dot_index.value() - 1);
  }
  if (bNeg)
    return pLocale->GetMinusSymbol() + wsOutput;

  return wsOutput;
}

}  // namespace

class CXFA_WidgetLayoutData {
 public:
  CXFA_WidgetLayoutData() = default;
  virtual ~CXFA_WidgetLayoutData() = default;

  virtual CXFA_FieldLayoutData* AsFieldLayoutData() { return nullptr; }
  virtual CXFA_ImageLayoutData* AsImageLayoutData() { return nullptr; }
  virtual CXFA_TextLayoutData* AsTextLayoutData() { return nullptr; }

  float m_fWidgetHeight = -1.0f;
};

class CXFA_TextLayoutData final : public CXFA_WidgetLayoutData {
 public:
  CXFA_TextLayoutData() = default;
  ~CXFA_TextLayoutData() override = default;

  CXFA_TextLayoutData* AsTextLayoutData() override { return this; }

  CXFA_TextLayout* GetTextLayout() const { return m_pTextLayout.get(); }
  CXFA_TextProvider* GetTextProvider() const { return m_pTextProvider.get(); }

  void LoadText(CXFA_FFDoc* doc, CXFA_Node* pNode) {
    if (m_pTextLayout)
      return;

    m_pTextProvider =
        pdfium::MakeUnique<CXFA_TextProvider>(pNode, XFA_TEXTPROVIDERTYPE_Text);
    m_pTextLayout =
        pdfium::MakeUnique<CXFA_TextLayout>(doc, m_pTextProvider.get());
  }

 private:
  std::unique_ptr<CXFA_TextLayout> m_pTextLayout;
  std::unique_ptr<CXFA_TextProvider> m_pTextProvider;
};

class CXFA_ImageLayoutData final : public CXFA_WidgetLayoutData {
 public:
  CXFA_ImageLayoutData() = default;
  ~CXFA_ImageLayoutData() override = default;

  CXFA_ImageLayoutData* AsImageLayoutData() override { return this; }

  bool LoadImageData(CXFA_FFDoc* doc, CXFA_Node* pNode) {
    if (m_pDIBitmap)
      return true;

    CXFA_Value* value = pNode->GetFormValueIfExists();
    if (!value)
      return false;

    CXFA_Image* image = value->GetImageIfExists();
    if (!image)
      return false;

    pNode->SetImageImage(XFA_LoadImageData(doc, image, m_bNamedImage,
                                           m_iImageXDpi, m_iImageYDpi));
    return !!m_pDIBitmap;
  }

  bool m_bNamedImage = false;
  int32_t m_iImageXDpi = 0;
  int32_t m_iImageYDpi = 0;
  RetainPtr<CFX_DIBitmap> m_pDIBitmap;
};

class CXFA_FieldLayoutData : public CXFA_WidgetLayoutData {
 public:
  CXFA_FieldLayoutData() = default;
  ~CXFA_FieldLayoutData() override = default;

  CXFA_FieldLayoutData* AsFieldLayoutData() override { return this; }

  virtual CXFA_ImageEditData* AsImageEditData() { return nullptr; }
  virtual CXFA_TextEditData* AsTextEditData() { return nullptr; }

  bool LoadCaption(CXFA_FFDoc* doc, CXFA_Node* pNode) {
    if (m_pCapTextLayout)
      return true;
    CXFA_Caption* caption = pNode->GetCaptionIfExists();
    if (!caption || caption->IsHidden())
      return false;

    m_pCapTextProvider = pdfium::MakeUnique<CXFA_TextProvider>(
        pNode, XFA_TEXTPROVIDERTYPE_Caption);
    m_pCapTextLayout =
        pdfium::MakeUnique<CXFA_TextLayout>(doc, m_pCapTextProvider.get());
    return true;
  }

  std::unique_ptr<CXFA_TextLayout> m_pCapTextLayout;
  std::unique_ptr<CXFA_TextProvider> m_pCapTextProvider;
  std::unique_ptr<CFDE_TextOut> m_pTextOut;
  std::vector<float> m_FieldSplitArray;
};

class CXFA_TextEditData final : public CXFA_FieldLayoutData {
 public:
  CXFA_TextEditData() = default;
  ~CXFA_TextEditData() override = default;

  CXFA_TextEditData* AsTextEditData() override { return this; }
};

class CXFA_ImageEditData final : public CXFA_FieldLayoutData {
 public:
  CXFA_ImageEditData() = default;
  ~CXFA_ImageEditData() override = default;

  CXFA_ImageEditData* AsImageEditData() override { return this; }

  bool LoadImageData(CXFA_FFDoc* doc, CXFA_Node* pNode) {
    if (m_pDIBitmap)
      return true;

    CXFA_Value* value = pNode->GetFormValueIfExists();
    if (!value)
      return false;

    CXFA_Image* image = value->GetImageIfExists();
    if (!image)
      return false;

    pNode->SetImageEditImage(XFA_LoadImageData(doc, image, m_bNamedImage,
                                               m_iImageXDpi, m_iImageYDpi));
    return !!m_pDIBitmap;
  }

  bool m_bNamedImage = false;
  int32_t m_iImageXDpi = 0;
  int32_t m_iImageYDpi = 0;
  RetainPtr<CFX_DIBitmap> m_pDIBitmap;
};

CXFA_Node::CXFA_Node(CXFA_Document* pDoc,
                     XFA_PacketType ePacket,
                     uint32_t validPackets,
                     XFA_ObjectType oType,
                     XFA_Element eType,
                     const PropertyData* properties,
                     const AttributeData* attributes,
                     std::unique_ptr<CJX_Object> js_node)
    : CXFA_Object(pDoc, oType, eType, std::move(js_node)),
      m_Properties(properties),
      m_Attributes(attributes),
      m_ValidPackets(validPackets),
      m_ePacket(ePacket) {
  ASSERT(m_pDocument);
}

CXFA_Node::CXFA_Node(CXFA_Document* pDoc,
                     XFA_PacketType ePacket,
                     uint32_t validPackets,
                     XFA_ObjectType oType,
                     XFA_Element eType,
                     const PropertyData* properties,
                     const AttributeData* attributes)
    : CXFA_Node(pDoc,
                ePacket,
                validPackets,
                oType,
                eType,
                properties,
                attributes,
                pdfium::MakeUnique<CJX_Node>(this)) {}

CXFA_Node::~CXFA_Node() = default;

CXFA_Node* CXFA_Node::Clone(bool bRecursive) {
  CXFA_Node* pClone = m_pDocument->CreateNode(m_ePacket, m_elementType);
  if (!pClone)
    return nullptr;

  JSObject()->MergeAllData(pClone);
  pClone->UpdateNameHash();
  if (IsNeedSavingXMLNode()) {
    CFX_XMLNode* pCloneXML;
    if (IsAttributeInXML()) {
      WideString wsName = JSObject()
                              ->TryAttribute(XFA_Attribute::Name, false)
                              .value_or(WideString());
      auto* pCloneXMLElement = GetDocument()
                                   ->GetNotify()
                                   ->GetHDOC()
                                   ->GetXMLDocument()
                                   ->CreateNode<CFX_XMLElement>(wsName);

      WideString wsValue = JSObject()->GetCData(XFA_Attribute::Value);
      if (!wsValue.IsEmpty()) {
        auto* text = GetDocument()
                         ->GetNotify()
                         ->GetHDOC()
                         ->GetXMLDocument()
                         ->CreateNode<CFX_XMLText>(wsValue);
        pCloneXMLElement->AppendChild(text);
      }

      pCloneXML = pCloneXMLElement;

      pClone->JSObject()->SetEnum(XFA_Attribute::Contains,
                                  XFA_AttributeValue::Unknown, false);
    } else {
      pCloneXML = xml_node_->Clone(
          GetDocument()->GetNotify()->GetHDOC()->GetXMLDocument());
    }
    pClone->SetXMLMappingNode(pCloneXML);
  }
  if (bRecursive) {
    for (CXFA_Node* pChild = GetFirstChild(); pChild;
         pChild = pChild->GetNextSibling()) {
      pClone->InsertChild(pChild->Clone(bRecursive), nullptr);
    }
  }
  pClone->SetFlagAndNotify(XFA_NodeFlag_Initialized);
  pClone->SetBindingNode(nullptr);
  return pClone;
}

CXFA_Node* CXFA_Node::GetNextContainerSibling() const {
  for (auto* pNode = next_sibling_; pNode; pNode = pNode->next_sibling_) {
    if (pNode->GetObjectType() == XFA_ObjectType::ContainerNode)
      return pNode;
  }
  return nullptr;
}

CXFA_Node* CXFA_Node::GetPrevContainerSibling() const {
  for (auto* pNode = prev_sibling_; pNode; pNode = pNode->prev_sibling_) {
    if (pNode->GetObjectType() == XFA_ObjectType::ContainerNode)
      return pNode;
  }
  return nullptr;
}

CXFA_Node* CXFA_Node::GetFirstContainerChild() const {
  for (auto* pNode = first_child_; pNode; pNode = pNode->next_sibling_) {
    if (pNode->GetObjectType() == XFA_ObjectType::ContainerNode)
      return pNode;
  }
  return nullptr;
}

CXFA_Node* CXFA_Node::GetContainerParent() const {
  for (auto* pNode = parent_; pNode; pNode = pNode->parent_) {
    if (pNode->GetObjectType() == XFA_ObjectType::ContainerNode)
      return pNode;
  }
  return nullptr;
}

bool CXFA_Node::IsValidInPacket(XFA_PacketType packet) const {
  return !!(m_ValidPackets & (1 << static_cast<uint8_t>(packet)));
}

const CXFA_Node::PropertyData* CXFA_Node::GetPropertyData(
    XFA_Element property) const {
  if (m_Properties == nullptr)
    return nullptr;

  for (size_t i = 0;; ++i) {
    const PropertyData* data = m_Properties + i;
    if (data->property == XFA_Element::Unknown)
      break;
    if (data->property == property)
      return data;
  }
  return nullptr;
}

bool CXFA_Node::HasProperty(XFA_Element property) const {
  return !!GetPropertyData(property);
}

bool CXFA_Node::HasPropertyFlags(XFA_Element property, uint8_t flags) const {
  const PropertyData* data = GetPropertyData(property);
  return data && !!(data->flags & flags);
}

uint8_t CXFA_Node::PropertyOccuranceCount(XFA_Element property) const {
  const PropertyData* data = GetPropertyData(property);
  return data ? data->occurance_count : 0;
}

Optional<XFA_Element> CXFA_Node::GetFirstPropertyWithFlag(uint8_t flag) {
  if (m_Properties == nullptr)
    return {};

  for (size_t i = 0;; ++i) {
    const PropertyData* data = m_Properties + i;
    if (data->property == XFA_Element::Unknown)
      break;
    if (data->flags & flag)
      return {data->property};
  }
  return {};
}

const CXFA_Node::AttributeData* CXFA_Node::GetAttributeData(
    XFA_Attribute attr) const {
  if (m_Attributes == nullptr)
    return nullptr;

  for (size_t i = 0;; ++i) {
    const AttributeData* cur_attr = &m_Attributes[i];
    if (cur_attr->attribute == XFA_Attribute::Unknown)
      break;
    if (cur_attr->attribute == attr)
      return cur_attr;
  }
  return nullptr;
}

bool CXFA_Node::HasAttribute(XFA_Attribute attr) const {
  return !!GetAttributeData(attr);
}

// Note: This Method assumes that i is a valid index ....
XFA_Attribute CXFA_Node::GetAttribute(size_t i) const {
  if (m_Attributes == nullptr)
    return XFA_Attribute::Unknown;
  return m_Attributes[i].attribute;
}

XFA_AttributeType CXFA_Node::GetAttributeType(XFA_Attribute type) const {
  const AttributeData* data = GetAttributeData(type);
  return data ? data->type : XFA_AttributeType::CData;
}

std::vector<CXFA_Node*> CXFA_Node::GetNodeList(uint32_t dwTypeFilter,
                                               XFA_Element eTypeFilter) {
  if (eTypeFilter != XFA_Element::Unknown) {
    std::vector<CXFA_Node*> nodes;
    for (CXFA_Node* pChild = first_child_; pChild;
         pChild = pChild->next_sibling_) {
      if (pChild->GetElementType() == eTypeFilter)
        nodes.push_back(pChild);
    }
    return nodes;
  }

  if (dwTypeFilter == (XFA_NODEFILTER_Children | XFA_NODEFILTER_Properties)) {
    std::vector<CXFA_Node*> nodes;
    for (CXFA_Node* pChild = first_child_; pChild;
         pChild = pChild->next_sibling_)
      nodes.push_back(pChild);
    return nodes;
  }

  if (dwTypeFilter == 0)
    return std::vector<CXFA_Node*>();

  bool bFilterChildren = !!(dwTypeFilter & XFA_NODEFILTER_Children);
  bool bFilterProperties = !!(dwTypeFilter & XFA_NODEFILTER_Properties);
  bool bFilterOneOfProperties = !!(dwTypeFilter & XFA_NODEFILTER_OneOfProperty);
  std::vector<CXFA_Node*> nodes;
  for (CXFA_Node* pChild = first_child_; pChild;
       pChild = pChild->next_sibling_) {
    if (HasProperty(pChild->GetElementType())) {
      if (bFilterProperties) {
        nodes.push_back(pChild);
      } else if (bFilterOneOfProperties &&
                 HasPropertyFlags(pChild->GetElementType(),
                                  XFA_PROPERTYFLAG_OneOf)) {
        nodes.push_back(pChild);
      } else if (bFilterChildren &&
                 (pChild->GetElementType() == XFA_Element::Variables ||
                  pChild->GetElementType() == XFA_Element::PageSet)) {
        nodes.push_back(pChild);
      }
    } else if (bFilterChildren) {
      nodes.push_back(pChild);
    }
  }

  if (!bFilterOneOfProperties || !nodes.empty())
    return nodes;
  if (m_Properties == nullptr)
    return nodes;

  Optional<XFA_Element> property =
      GetFirstPropertyWithFlag(XFA_PROPERTYFLAG_DefaultOneOf);
  if (!property)
    return nodes;

  CXFA_Node* pNewNode = m_pDocument->CreateNode(GetPacketType(), *property);
  if (pNewNode) {
    InsertChild(pNewNode, nullptr);
    pNewNode->SetFlagAndNotify(XFA_NodeFlag_Initialized);
    nodes.push_back(pNewNode);
  }
  return nodes;
}

CXFA_Node* CXFA_Node::CreateSamePacketNode(XFA_Element eType) {
  CXFA_Node* pNode = m_pDocument->CreateNode(m_ePacket, eType);
  pNode->SetFlagAndNotify(XFA_NodeFlag_Initialized);
  return pNode;
}

CXFA_Node* CXFA_Node::CloneTemplateToForm(bool bRecursive) {
  ASSERT(m_ePacket == XFA_PacketType::Template);
  CXFA_Node* pClone =
      m_pDocument->CreateNode(XFA_PacketType::Form, m_elementType);
  if (!pClone)
    return nullptr;

  pClone->SetTemplateNode(this);
  pClone->UpdateNameHash();
  pClone->SetXMLMappingNode(GetXMLMappingNode());
  if (bRecursive) {
    for (CXFA_Node* pChild = GetFirstChild(); pChild;
         pChild = pChild->GetNextSibling()) {
      pClone->InsertChild(pChild->CloneTemplateToForm(bRecursive), nullptr);
    }
  }
  pClone->SetFlagAndNotify(XFA_NodeFlag_Initialized);
  return pClone;
}

CXFA_Node* CXFA_Node::GetTemplateNodeIfExists() const {
  return m_pAuxNode;
}

void CXFA_Node::SetTemplateNode(CXFA_Node* pTemplateNode) {
  m_pAuxNode = pTemplateNode;
}

CXFA_Node* CXFA_Node::GetBindData() {
  ASSERT(GetPacketType() == XFA_PacketType::Form);
  return GetBindingNode();
}

int32_t CXFA_Node::AddBindItem(CXFA_Node* pFormNode) {
  ASSERT(pFormNode);

  if (BindsFormItems()) {
    bool found = false;
    for (auto* v : binding_nodes_) {
      if (v == pFormNode) {
        found = true;
        break;
      }
    }
    if (!found)
      binding_nodes_.emplace_back(pFormNode);
    return pdfium::CollectionSize<int32_t>(binding_nodes_);
  }

  CXFA_Node* pOldFormItem = GetBindingNode();
  if (!pOldFormItem) {
    SetBindingNode(pFormNode);
    return 1;
  }
  if (pOldFormItem == pFormNode)
    return 1;

  std::vector<CXFA_Node*> items;
  items.push_back(pOldFormItem);
  items.push_back(pFormNode);
  binding_nodes_ = std::move(items);

  m_uNodeFlags |= XFA_NodeFlag_BindFormItems;
  return 2;
}

int32_t CXFA_Node::RemoveBindItem(CXFA_Node* pFormNode) {
  if (BindsFormItems()) {
    auto it =
        std::find(binding_nodes_.begin(), binding_nodes_.end(), pFormNode);
    if (it != binding_nodes_.end())
      binding_nodes_.erase(it);

    if (binding_nodes_.size() == 1) {
      m_uNodeFlags &= ~XFA_NodeFlag_BindFormItems;
      return 1;
    }
    return pdfium::CollectionSize<int32_t>(binding_nodes_);
  }

  CXFA_Node* pOldFormItem = GetBindingNode();
  if (pOldFormItem != pFormNode)
    return pOldFormItem ? 1 : 0;

  SetBindingNode(nullptr);
  return 0;
}

bool CXFA_Node::HasBindItem() {
  return GetPacketType() == XFA_PacketType::Datasets && GetBindingNode();
}

CXFA_Node* CXFA_Node::GetContainerNode() {
  if (GetPacketType() != XFA_PacketType::Form)
    return nullptr;
  XFA_Element eType = GetElementType();
  if (eType == XFA_Element::ExclGroup)
    return nullptr;
  CXFA_Node* pParentNode = GetParent();
  if (pParentNode && pParentNode->GetElementType() == XFA_Element::ExclGroup)
    return nullptr;

  if (eType == XFA_Element::Field) {
    if (IsChoiceListMultiSelect())
      return nullptr;

    WideString wsPicture = GetPictureContent(XFA_VALUEPICTURE_DataBind);
    if (!wsPicture.IsEmpty())
      return this;

    CXFA_Node* pDataNode = GetBindData();
    if (!pDataNode)
      return nullptr;

    CXFA_Node* pFieldNode = nullptr;
    for (auto* pFormNode : *(pDataNode->GetBindItems())) {
      if (!pFormNode || pFormNode->HasRemovedChildren())
        continue;
      pFieldNode = pFormNode->IsWidgetReady() ? pFormNode : nullptr;
      if (pFieldNode)
        wsPicture = pFieldNode->GetPictureContent(XFA_VALUEPICTURE_DataBind);
      if (!wsPicture.IsEmpty())
        break;

      pFieldNode = nullptr;
    }
    return pFieldNode;
  }

  CXFA_Node* pGrandNode = pParentNode ? pParentNode->GetParent() : nullptr;
  CXFA_Node* pValueNode =
      (pParentNode && pParentNode->GetElementType() == XFA_Element::Value)
          ? pParentNode
          : nullptr;
  if (!pValueNode) {
    pValueNode =
        (pGrandNode && pGrandNode->GetElementType() == XFA_Element::Value)
            ? pGrandNode
            : nullptr;
  }
  CXFA_Node* pParentOfValueNode =
      pValueNode ? pValueNode->GetParent() : nullptr;
  return pParentOfValueNode ? pParentOfValueNode->GetContainerNode() : nullptr;
}

LocaleIface* CXFA_Node::GetLocale() {
  Optional<WideString> localeName = GetLocaleName();
  if (!localeName.has_value())
    return nullptr;
  if (localeName.value().EqualsASCII("ambient"))
    return GetDocument()->GetLocaleMgr()->GetDefLocale();
  return GetDocument()->GetLocaleMgr()->GetLocaleByName(localeName.value());
}

Optional<WideString> CXFA_Node::GetLocaleName() {
  CXFA_Node* pForm = GetDocument()->GetXFAObject(XFA_HASHCODE_Form)->AsNode();
  CXFA_Subform* pTopSubform =
      pForm->GetFirstChildByClass<CXFA_Subform>(XFA_Element::Subform);
  if (!pTopSubform)
    return {};

  CXFA_Node* pLocaleNode = this;
  do {
    Optional<WideString> localeName =
        pLocaleNode->JSObject()->TryCData(XFA_Attribute::Locale, false);
    if (localeName)
      return localeName;

    pLocaleNode = pLocaleNode->GetParent();
  } while (pLocaleNode && pLocaleNode != pTopSubform);

  CXFA_Node* pConfig = ToNode(GetDocument()->GetXFAObject(XFA_HASHCODE_Config));
  Optional<WideString> localeName = {
      GetDocument()->GetLocaleMgr()->GetConfigLocaleName(pConfig)};
  if (localeName && !localeName->IsEmpty())
    return localeName;

  if (pTopSubform) {
    localeName =
        pTopSubform->JSObject()->TryCData(XFA_Attribute::Locale, false);
    if (localeName)
      return localeName;
  }

  LocaleIface* pLocale = GetDocument()->GetLocaleMgr()->GetDefLocale();
  if (!pLocale)
    return {};

  return {pLocale->GetName()};
}

XFA_AttributeValue CXFA_Node::GetIntact() {
  CXFA_Keep* pKeep = GetFirstChildByClass<CXFA_Keep>(XFA_Element::Keep);
  auto layout = JSObject()->TryEnum(XFA_Attribute::Layout, true);
  XFA_AttributeValue eLayoutType =
      layout.value_or(XFA_AttributeValue::Position);
  if (pKeep) {
    Optional<XFA_AttributeValue> intact = GetIntactFromKeep(pKeep, eLayoutType);
    if (intact)
      return *intact;
  }

  switch (GetElementType()) {
    case XFA_Element::Subform:
      switch (eLayoutType) {
        case XFA_AttributeValue::Position:
        case XFA_AttributeValue::Row:
          return XFA_AttributeValue::ContentArea;
        default:
          return XFA_AttributeValue::None;
      }
    case XFA_Element::Field: {
      CXFA_Node* parent = GetParent();
      if (!parent || parent->GetElementType() == XFA_Element::PageArea)
        return XFA_AttributeValue::ContentArea;
      if (parent->GetIntact() != XFA_AttributeValue::None)
        return XFA_AttributeValue::ContentArea;

      auto value = parent->JSObject()->TryEnum(XFA_Attribute::Layout, true);
      XFA_AttributeValue eParLayout =
          value.value_or(XFA_AttributeValue::Position);
      if (eParLayout == XFA_AttributeValue::Position ||
          eParLayout == XFA_AttributeValue::Row ||
          eParLayout == XFA_AttributeValue::Table) {
        return XFA_AttributeValue::None;
      }

      XFA_VERSION version = m_pDocument->GetCurVersionMode();
      if (eParLayout == XFA_AttributeValue::Tb && version < XFA_VERSION_208) {
        Optional<CXFA_Measurement> measureH =
            JSObject()->TryMeasure(XFA_Attribute::H, false);
        if (measureH)
          return XFA_AttributeValue::ContentArea;
      }
      return XFA_AttributeValue::None;
    }
    case XFA_Element::Draw:
      return XFA_AttributeValue::ContentArea;
    default:
      return XFA_AttributeValue::None;
  }
}

CXFA_Node* CXFA_Node::GetDataDescriptionNode() {
  if (m_ePacket == XFA_PacketType::Datasets)
    return m_pAuxNode;
  return nullptr;
}

void CXFA_Node::SetDataDescriptionNode(CXFA_Node* pDataDescriptionNode) {
  ASSERT(m_ePacket == XFA_PacketType::Datasets);
  m_pAuxNode = pDataDescriptionNode;
}

CXFA_Node* CXFA_Node::GetModelNode() {
  switch (GetPacketType()) {
    case XFA_PacketType::Xdp:
      return m_pDocument->GetRoot();
    case XFA_PacketType::Config:
      return ToNode(m_pDocument->GetXFAObject(XFA_HASHCODE_Config));
    case XFA_PacketType::Template:
      return ToNode(m_pDocument->GetXFAObject(XFA_HASHCODE_Template));
    case XFA_PacketType::Form:
      return ToNode(m_pDocument->GetXFAObject(XFA_HASHCODE_Form));
    case XFA_PacketType::Datasets:
      return ToNode(m_pDocument->GetXFAObject(XFA_HASHCODE_Datasets));
    case XFA_PacketType::LocaleSet:
      return ToNode(m_pDocument->GetXFAObject(XFA_HASHCODE_LocaleSet));
    case XFA_PacketType::ConnectionSet:
      return ToNode(m_pDocument->GetXFAObject(XFA_HASHCODE_ConnectionSet));
    case XFA_PacketType::SourceSet:
      return ToNode(m_pDocument->GetXFAObject(XFA_HASHCODE_SourceSet));
    case XFA_PacketType::Xdc:
      return ToNode(m_pDocument->GetXFAObject(XFA_HASHCODE_Xdc));
    default:
      return this;
  }
}

size_t CXFA_Node::CountChildren(XFA_Element eType, bool bOnlyChild) {
  size_t count = 0;
  for (CXFA_Node* pNode = first_child_; pNode;
       pNode = pNode->GetNextSibling()) {
    if (pNode->GetElementType() != eType && eType != XFA_Element::Unknown)
      continue;
    if (bOnlyChild && HasProperty(pNode->GetElementType()))
      continue;
    ++count;
  }
  return count;
}

CXFA_Node* CXFA_Node::GetChildInternal(size_t index,
                                       XFA_Element eType,
                                       bool bOnlyChild) {
  size_t count = 0;
  for (CXFA_Node* pNode = first_child_; pNode;
       pNode = pNode->GetNextSibling()) {
    if (pNode->GetElementType() != eType && eType != XFA_Element::Unknown)
      continue;
    if (bOnlyChild && HasProperty(pNode->GetElementType()))
      continue;
    if (count == index)
      return pNode;

    ++count;
  }
  return nullptr;
}

void CXFA_Node::InsertChild(int32_t index, CXFA_Node* pNode) {
  if (!pNode || pNode->parent_ != nullptr) {
    PDFIUM_IMMEDIATE_CRASH();
  }

  pNode->parent_ = this;
  pNode->ClearFlag(XFA_NodeFlag_HasRemovedChildren);

  if (!first_child_) {
    ASSERT(!last_child_);

    pNode->prev_sibling_ = nullptr;
    pNode->next_sibling_ = nullptr;
    first_child_ = pNode;
    last_child_ = pNode;
    index = 0;
  } else if (index == 0) {
    pNode->prev_sibling_ = nullptr;
    pNode->next_sibling_ = first_child_;
    first_child_->prev_sibling_ = pNode;
    first_child_ = pNode;
  } else if (index < 0) {
    pNode->prev_sibling_ = last_child_;
    pNode->next_sibling_ = nullptr;
    last_child_->next_sibling_ = pNode;
    last_child_ = pNode;
  } else {
    CXFA_Node* pPrev = first_child_;
    int32_t count = 0;
    while (++count < index && pPrev->next_sibling_)
      pPrev = pPrev->next_sibling_;

    pNode->prev_sibling_ = pPrev;
    pNode->next_sibling_ = pPrev->next_sibling_;
    if (pPrev->next_sibling_)
      pPrev->next_sibling_->prev_sibling_ = pNode;
    pPrev->next_sibling_ = pNode;

    // Move the last child pointer if needed.
    if (pPrev == last_child_)
      last_child_ = pNode;

    index = count;
  }

  CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
  if (pNotify)
    pNotify->OnChildAdded(this);

  if (!IsNeedSavingXMLNode() || !pNode->xml_node_)
    return;

  ASSERT(!pNode->xml_node_->GetParent());
  xml_node_->InsertChildNode(pNode->xml_node_.Get(), index);
}

void CXFA_Node::InsertChild(CXFA_Node* pNode, CXFA_Node* pBeforeNode) {
  if (pBeforeNode && pBeforeNode->parent_ != this) {
    PDFIUM_IMMEDIATE_CRASH();
  }

  int32_t index = -1;
  if (!first_child_ || pBeforeNode == first_child_) {
    index = 0;
  } else if (!pBeforeNode) {
    index = -1;
  } else {
    index = 0;
    CXFA_Node* prev = first_child_;
    while (prev && prev != pBeforeNode) {
      prev = prev->next_sibling_;
      ++index;
    }
  }
  InsertChild(index, pNode);
}

void CXFA_Node::RemoveChild(CXFA_Node* pNode, bool bNotify) {
  if (!pNode || pNode->parent_ != this) {
    PDFIUM_IMMEDIATE_CRASH();
  }

  pNode->SetFlag(XFA_NodeFlag_HasRemovedChildren);

  if (first_child_ == pNode && last_child_ == pNode) {
    first_child_ = nullptr;
    last_child_ = nullptr;
  } else if (first_child_ == pNode) {
    first_child_ = pNode->next_sibling_;
    first_child_->prev_sibling_ = nullptr;
  } else if (last_child_ == pNode) {
    last_child_ = pNode->prev_sibling_;
    last_child_->next_sibling_ = nullptr;
  } else {
    CXFA_Node* pPrev = pNode->prev_sibling_;
    pPrev->next_sibling_ = pNode->next_sibling_;
    pPrev->next_sibling_->prev_sibling_ = pPrev;
  }
  pNode->next_sibling_ = nullptr;
  pNode->prev_sibling_ = nullptr;
  pNode->parent_ = nullptr;

  OnRemoved(bNotify);

  if (!IsNeedSavingXMLNode() || !pNode->xml_node_)
    return;

  if (!pNode->IsAttributeInXML()) {
    xml_node_->RemoveChildNode(pNode->xml_node_.Get());
    return;
  }

  ASSERT(pNode->xml_node_.Get() == xml_node_.Get());
  CFX_XMLElement* pXMLElement = ToXMLElement(pNode->xml_node_.Get());
  if (pXMLElement) {
    WideString wsAttributeName =
        pNode->JSObject()->GetCData(XFA_Attribute::QualifiedName);
    pXMLElement->RemoveAttribute(wsAttributeName);
  }

  WideString wsName = pNode->JSObject()
                          ->TryAttribute(XFA_Attribute::Name, false)
                          .value_or(WideString());

  auto* pNewXMLElement = GetDocument()
                             ->GetNotify()
                             ->GetHDOC()
                             ->GetXMLDocument()
                             ->CreateNode<CFX_XMLElement>(wsName);
  WideString wsValue = JSObject()->GetCData(XFA_Attribute::Value);
  if (!wsValue.IsEmpty()) {
    auto* text = GetDocument()
                     ->GetNotify()
                     ->GetHDOC()
                     ->GetXMLDocument()
                     ->CreateNode<CFX_XMLText>(wsValue);
    pNewXMLElement->AppendChild(text);
  }
  pNode->xml_node_ = pNewXMLElement;
  pNode->JSObject()->SetEnum(XFA_Attribute::Contains,
                             XFA_AttributeValue::Unknown, false);
}

CXFA_Node* CXFA_Node::GetFirstChildByName(WideStringView wsName) const {
  return GetFirstChildByName(FX_HashCode_GetW(wsName, false));
}

CXFA_Node* CXFA_Node::GetFirstChildByName(uint32_t dwNameHash) const {
  for (CXFA_Node* pNode = GetFirstChild(); pNode;
       pNode = pNode->GetNextSibling()) {
    if (pNode->GetNameHash() == dwNameHash)
      return pNode;
  }
  return nullptr;
}

CXFA_Node* CXFA_Node::GetFirstChildByClassInternal(XFA_Element eType) const {
  for (CXFA_Node* pNode = GetFirstChild(); pNode;
       pNode = pNode->GetNextSibling()) {
    if (pNode->GetElementType() == eType)
      return pNode;
  }
  return nullptr;
}

CXFA_Node* CXFA_Node::GetNextSameNameSibling(uint32_t dwNameHash) const {
  for (CXFA_Node* pNode = GetNextSibling(); pNode;
       pNode = pNode->GetNextSibling()) {
    if (pNode->GetNameHash() == dwNameHash)
      return pNode;
  }
  return nullptr;
}

CXFA_Node* CXFA_Node::GetNextSameNameSiblingInternal(
    WideStringView wsNodeName) const {
  return GetNextSameNameSibling(FX_HashCode_GetW(wsNodeName, false));
}

CXFA_Node* CXFA_Node::GetNextSameClassSiblingInternal(XFA_Element eType) const {
  for (CXFA_Node* pNode = GetNextSibling(); pNode;
       pNode = pNode->GetNextSibling()) {
    if (pNode->GetElementType() == eType)
      return pNode;
  }
  return nullptr;
}

CXFA_Node* CXFA_Node::GetInstanceMgrOfSubform() {
  CXFA_Node* pInstanceMgr = nullptr;
  if (m_ePacket == XFA_PacketType::Form) {
    CXFA_Node* pParentNode = GetParent();
    if (!pParentNode || pParentNode->GetElementType() == XFA_Element::Area)
      return pInstanceMgr;

    for (CXFA_Node* pNode = GetPrevSibling(); pNode;
         pNode = pNode->GetPrevSibling()) {
      XFA_Element eType = pNode->GetElementType();
      if ((eType == XFA_Element::Subform || eType == XFA_Element::SubformSet) &&
          pNode->m_dwNameHash != m_dwNameHash) {
        break;
      }
      if (eType == XFA_Element::InstanceManager) {
        WideString wsName = JSObject()->GetCData(XFA_Attribute::Name);
        WideString wsInstName =
            pNode->JSObject()->GetCData(XFA_Attribute::Name);
        if (wsInstName.GetLength() > 0 && wsInstName[0] == '_' &&
            wsInstName.Right(wsInstName.GetLength() - 1) == wsName) {
          pInstanceMgr = pNode;
        }
        break;
      }
    }
  }
  return pInstanceMgr;
}

CXFA_Occur* CXFA_Node::GetOccurIfExists() {
  return GetFirstChildByClass<CXFA_Occur>(XFA_Element::Occur);
}

bool CXFA_Node::HasFlag(XFA_NodeFlag dwFlag) const {
  if (m_uNodeFlags & dwFlag)
    return true;
  if (dwFlag == XFA_NodeFlag_HasRemovedChildren)
    return parent_ && parent_->HasFlag(dwFlag);
  return false;
}

void CXFA_Node::SetFlagAndNotify(uint32_t dwFlag) {
  ASSERT(dwFlag == XFA_NodeFlag_Initialized);

  if (!IsInitialized()) {
    CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
    if (pNotify) {
      pNotify->OnNodeReady(this);
    }
  }
  m_uNodeFlags |= dwFlag;
}

void CXFA_Node::SetFlag(uint32_t dwFlag) {
  m_uNodeFlags |= dwFlag;
}

void CXFA_Node::ClearFlag(uint32_t dwFlag) {
  m_uNodeFlags &= ~dwFlag;
}

bool CXFA_Node::IsAttributeInXML() {
  return JSObject()->GetEnum(XFA_Attribute::Contains) ==
         XFA_AttributeValue::MetaData;
}

void CXFA_Node::OnRemoved(bool bNotify) {
  if (!bNotify)
    return;

  CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
  if (pNotify)
    pNotify->OnChildRemoved();
}

void CXFA_Node::UpdateNameHash() {
  WideString wsName = JSObject()->GetCData(XFA_Attribute::Name);
  m_dwNameHash = FX_HashCode_GetW(wsName.AsStringView(), false);
}

CFX_XMLNode* CXFA_Node::CreateXMLMappingNode() {
  if (!xml_node_) {
    xml_node_ = GetDocument()
                    ->GetNotify()
                    ->GetHDOC()
                    ->GetXMLDocument()
                    ->CreateNode<CFX_XMLElement>(
                        JSObject()->GetCData(XFA_Attribute::Name));
  }
  return xml_node_.Get();
}

bool CXFA_Node::IsNeedSavingXMLNode() {
  return xml_node_ && (GetPacketType() == XFA_PacketType::Datasets ||
                       GetElementType() == XFA_Element::Xfa);
}

CXFA_Node* CXFA_Node::GetItemIfExists(int32_t iIndex) {
  int32_t iCount = 0;
  uint32_t dwNameHash = 0;
  for (CXFA_Node* pNode = GetNextSibling(); pNode;
       pNode = pNode->GetNextSibling()) {
    XFA_Element eCurType = pNode->GetElementType();
    if (eCurType == XFA_Element::InstanceManager)
      break;
    if ((eCurType != XFA_Element::Subform) &&
        (eCurType != XFA_Element::SubformSet)) {
      continue;
    }
    if (iCount == 0) {
      WideString wsName = pNode->JSObject()->GetCData(XFA_Attribute::Name);
      WideString wsInstName = JSObject()->GetCData(XFA_Attribute::Name);
      if (wsInstName.GetLength() < 1 || wsInstName[0] != '_' ||
          wsInstName.Right(wsInstName.GetLength() - 1) != wsName) {
        return nullptr;
      }
      dwNameHash = pNode->GetNameHash();
    }
    if (dwNameHash != pNode->GetNameHash())
      break;

    iCount++;
    if (iCount > iIndex)
      return pNode;
  }
  return nullptr;
}

int32_t CXFA_Node::GetCount() {
  int32_t iCount = 0;
  uint32_t dwNameHash = 0;
  for (CXFA_Node* pNode = GetNextSibling(); pNode;
       pNode = pNode->GetNextSibling()) {
    XFA_Element eCurType = pNode->GetElementType();
    if (eCurType == XFA_Element::InstanceManager)
      break;
    if ((eCurType != XFA_Element::Subform) &&
        (eCurType != XFA_Element::SubformSet)) {
      continue;
    }
    if (iCount == 0) {
      WideString wsName = pNode->JSObject()->GetCData(XFA_Attribute::Name);
      WideString wsInstName = JSObject()->GetCData(XFA_Attribute::Name);
      if (wsInstName.GetLength() < 1 || wsInstName[0] != '_' ||
          wsInstName.Right(wsInstName.GetLength() - 1) != wsName) {
        return iCount;
      }
      dwNameHash = pNode->GetNameHash();
    }
    if (dwNameHash != pNode->GetNameHash())
      break;

    iCount++;
  }
  return iCount;
}

void CXFA_Node::InsertItem(CXFA_Node* pNewInstance,
                           int32_t iPos,
                           int32_t iCount,
                           bool bMoveDataBindingNodes) {
  if (iCount < 0)
    iCount = GetCount();
  if (iPos < 0)
    iPos = iCount;
  if (iPos == iCount) {
    CXFA_Node* item = GetItemIfExists(iCount - 1);
    if (!item)
      return;

    CXFA_Node* pNextSibling =
        iCount > 0 ? item->GetNextSibling() : GetNextSibling();
    GetParent()->InsertChild(pNewInstance, pNextSibling);
    if (bMoveDataBindingNodes) {
      std::set<CXFA_Node*> sNew;
      std::set<CXFA_Node*> sAfter;
      CXFA_NodeIteratorTemplate<CXFA_Node,
                                CXFA_TraverseStrategy_XFAContainerNode>
          sIteratorNew(pNewInstance);
      for (CXFA_Node* pNode = sIteratorNew.GetCurrent(); pNode;
           pNode = sIteratorNew.MoveToNext()) {
        CXFA_Node* pDataNode = pNode->GetBindData();
        if (!pDataNode)
          continue;

        sNew.insert(pDataNode);
      }
      CXFA_NodeIteratorTemplate<CXFA_Node,
                                CXFA_TraverseStrategy_XFAContainerNode>
          sIteratorAfter(pNextSibling);
      for (CXFA_Node* pNode = sIteratorAfter.GetCurrent(); pNode;
           pNode = sIteratorAfter.MoveToNext()) {
        CXFA_Node* pDataNode = pNode->GetBindData();
        if (!pDataNode)
          continue;

        sAfter.insert(pDataNode);
      }
      ReorderDataNodes(sNew, sAfter, false);
    }
  } else {
    CXFA_Node* pBeforeInstance = GetItemIfExists(iPos);
    if (!pBeforeInstance) {
      // TODO(dsinclair): What should happen here?
      return;
    }

    GetParent()->InsertChild(pNewInstance, pBeforeInstance);
    if (bMoveDataBindingNodes) {
      std::set<CXFA_Node*> sNew;
      std::set<CXFA_Node*> sBefore;
      CXFA_NodeIteratorTemplate<CXFA_Node,
                                CXFA_TraverseStrategy_XFAContainerNode>
          sIteratorNew(pNewInstance);
      for (CXFA_Node* pNode = sIteratorNew.GetCurrent(); pNode;
           pNode = sIteratorNew.MoveToNext()) {
        CXFA_Node* pDataNode = pNode->GetBindData();
        if (!pDataNode)
          continue;

        sNew.insert(pDataNode);
      }
      CXFA_NodeIteratorTemplate<CXFA_Node,
                                CXFA_TraverseStrategy_XFAContainerNode>
          sIteratorBefore(pBeforeInstance);
      for (CXFA_Node* pNode = sIteratorBefore.GetCurrent(); pNode;
           pNode = sIteratorBefore.MoveToNext()) {
        CXFA_Node* pDataNode = pNode->GetBindData();
        if (!pDataNode)
          continue;

        sBefore.insert(pDataNode);
      }
      ReorderDataNodes(sNew, sBefore, true);
    }
  }
}

void CXFA_Node::RemoveItem(CXFA_Node* pRemoveInstance,
                           bool bRemoveDataBinding) {
  GetParent()->RemoveChild(pRemoveInstance, true);
  if (!bRemoveDataBinding)
    return;

  CXFA_NodeIteratorTemplate<CXFA_Node, CXFA_TraverseStrategy_XFAContainerNode>
      sIterator(pRemoveInstance);
  for (CXFA_Node* pFormNode = sIterator.GetCurrent(); pFormNode;
       pFormNode = sIterator.MoveToNext()) {
    CXFA_Node* pDataNode = pFormNode->GetBindData();
    if (!pDataNode)
      continue;

    if (pDataNode->RemoveBindItem(pFormNode) == 0) {
      if (CXFA_Node* pDataParent = pDataNode->GetParent()) {
        pDataParent->RemoveChild(pDataNode, true);
      }
    }
    pFormNode->SetBindingNode(nullptr);
  }
}

CXFA_Node* CXFA_Node::CreateInstanceIfPossible(bool bDataMerge) {
  CXFA_Document* pDocument = GetDocument();
  CXFA_Node* pTemplateNode = GetTemplateNodeIfExists();
  if (!pTemplateNode)
    return nullptr;

  CXFA_Node* pFormParent = GetParent();
  CXFA_Node* pDataScope = nullptr;
  for (CXFA_Node* pRootBoundNode = pFormParent;
       pRootBoundNode && pRootBoundNode->IsContainerNode();
       pRootBoundNode = pRootBoundNode->GetParent()) {
    pDataScope = pRootBoundNode->GetBindData();
    if (pDataScope)
      break;
  }
  if (!pDataScope) {
    pDataScope = ToNode(pDocument->GetXFAObject(XFA_HASHCODE_Record));
    ASSERT(pDataScope);
  }

  CXFA_Node* pInstance = pDocument->DataMerge_CopyContainer(
      pTemplateNode, pFormParent, pDataScope, true, bDataMerge, true);
  if (pInstance) {
    pDocument->DataMerge_UpdateBindingRelations(pInstance);
    pFormParent->RemoveChild(pInstance, true);
  }
  return pInstance;
}

Optional<bool> CXFA_Node::GetDefaultBoolean(XFA_Attribute attr) const {
  Optional<void*> value = GetDefaultValue(attr, XFA_AttributeType::Boolean);
  if (!value)
    return {};
  return {!!*value};
}

Optional<int32_t> CXFA_Node::GetDefaultInteger(XFA_Attribute attr) const {
  Optional<void*> value = GetDefaultValue(attr, XFA_AttributeType::Integer);
  if (!value)
    return {};
  return {static_cast<int32_t>(reinterpret_cast<uintptr_t>(*value))};
}

Optional<CXFA_Measurement> CXFA_Node::GetDefaultMeasurement(
    XFA_Attribute attr) const {
  Optional<void*> value = GetDefaultValue(attr, XFA_AttributeType::Measure);
  if (!value)
    return {};

  WideString str = WideString(static_cast<const wchar_t*>(*value));
  return {CXFA_Measurement(str.AsStringView())};
}

Optional<WideString> CXFA_Node::GetDefaultCData(XFA_Attribute attr) const {
  Optional<void*> value = GetDefaultValue(attr, XFA_AttributeType::CData);
  if (!value)
    return {};

  return {WideString(static_cast<const wchar_t*>(*value))};
}

Optional<XFA_AttributeValue> CXFA_Node::GetDefaultEnum(
    XFA_Attribute attr) const {
  Optional<void*> value = GetDefaultValue(attr, XFA_AttributeType::Enum);
  if (!value)
    return {};
  return {static_cast<XFA_AttributeValue>(reinterpret_cast<uintptr_t>(*value))};
}

Optional<void*> CXFA_Node::GetDefaultValue(XFA_Attribute attr,
                                           XFA_AttributeType eType) const {
  const AttributeData* data = GetAttributeData(attr);
  if (!data)
    return {};
  if (data->type == eType)
    return {data->default_value};
  return {};
}

void CXFA_Node::SendAttributeChangeMessage(XFA_Attribute eAttribute,
                                           bool bScriptModify) {
  CXFA_LayoutProcessor* pLayoutPro = GetDocument()->GetLayoutProcessor();
  if (!pLayoutPro)
    return;

  CXFA_FFNotify* pNotify = GetDocument()->GetNotify();
  if (!pNotify)
    return;

  if (GetPacketType() != XFA_PacketType::Form) {
    pNotify->OnValueChanged(this, eAttribute, this, this);
    return;
  }

  bool bNeedFindContainer = false;
  switch (GetElementType()) {
    case XFA_Element::Caption:
      bNeedFindContainer = true;
      pNotify->OnValueChanged(this, eAttribute, this, GetParent());
      break;
    case XFA_Element::Font:
    case XFA_Element::Para: {
      bNeedFindContainer = true;
      CXFA_Node* pParentNode = GetParent();
      if (pParentNode->GetElementType() == XFA_Element::Caption) {
        pNotify->OnValueChanged(this, eAttribute, pParentNode,
                                pParentNode->GetParent());
      } else {
        pNotify->OnValueChanged(this, eAttribute, this, pParentNode);
      }
      break;
    }
    case XFA_Element::Margin: {
      bNeedFindContainer = true;
      CXFA_Node* pParentNode = GetParent();
      XFA_Element eParentType = pParentNode->GetElementType();
      if (pParentNode->IsContainerNode()) {
        pNotify->OnValueChanged(this, eAttribute, this, pParentNode);
      } else if (eParentType == XFA_Element::Caption) {
        pNotify->OnValueChanged(this, eAttribute, pParentNode,
                                pParentNode->GetParent());
      } else {
        CXFA_Node* pNode = pParentNode->GetParent();
        if (pNode && pNode->GetElementType() == XFA_Element::Ui)
          pNotify->OnValueChanged(this, eAttribute, pNode, pNode->GetParent());
      }
      break;
    }
    case XFA_Element::Comb: {
      CXFA_Node* pEditNode = GetParent();
      XFA_Element eUIType = pEditNode->GetElementType();
      if (pEditNode && (eUIType == XFA_Element::DateTimeEdit ||
                        eUIType == XFA_Element::NumericEdit ||
                        eUIType == XFA_Element::TextEdit)) {
        CXFA_Node* pUINode = pEditNode->GetParent();
        if (pUINode) {
          pNotify->OnValueChanged(this, eAttribute, pUINode,
                                  pUINode->GetParent());
        }
      }
      break;
    }
    case XFA_Element::Button:
    case XFA_Element::Barcode:
    case XFA_Element::ChoiceList:
    case XFA_Element::DateTimeEdit:
    case XFA_Element::NumericEdit:
    case XFA_Element::PasswordEdit:
    case XFA_Element::TextEdit: {
      CXFA_Node* pUINode = GetParent();
      if (pUINode) {
        pNotify->OnValueChanged(this, eAttribute, pUINode,
                                pUINode->GetParent());
      }
      break;
    }
    case XFA_Element::CheckButton: {
      bNeedFindContainer = true;
      CXFA_Node* pUINode = GetParent();
      if (pUINode) {
        pNotify->OnValueChanged(this, eAttribute, pUINode,
                                pUINode->GetParent());
      }
      break;
    }
    case XFA_Element::Keep:
    case XFA_Element::Bookend:
    case XFA_Element::Break:
    case XFA_Element::BreakAfter:
    case XFA_Element::BreakBefore:
    case XFA_Element::Overflow:
      bNeedFindContainer = true;
      break;
    case XFA_Element::Area:
    case XFA_Element::Draw:
    case XFA_Element::ExclGroup:
    case XFA_Element::Field:
    case XFA_Element::Subform:
    case XFA_Element::SubformSet:
      pLayoutPro->AddChangedContainer(this);
      pNotify->OnValueChanged(this, eAttribute, this, this);
      break;
    case XFA_Element::Sharptext:
    case XFA_Element::Sharpxml:
    case XFA_Element::SharpxHTML: {
      CXFA_Node* pTextNode = GetParent();
      if (!pTextNode)
        return;

      CXFA_Node* pValueNode = pTextNode->GetParent();
      if (!pValueNode)
        return;

      XFA_Element eType = pValueNode->GetElementType();
      if (eType == XFA_Element::Value) {
        bNeedFindContainer = true;
        CXFA_Node* pNode = pValueNode->GetParent();
        if (pNode && pNode->IsContainerNode()) {
          if (bScriptModify)
            pValueNode = pNode;

          pNotify->OnValueChanged(this, eAttribute, pValueNode, pNode);
        } else {
          pNotify->OnValueChanged(this, eAttribute, pNode, pNode->GetParent());
        }
      } else {
        if (eType == XFA_Element::Items) {
          CXFA_Node* pNode = pValueNode->GetParent();
          if (pNode && pNode->IsContainerNode()) {
            pNotify->OnValueChanged(this, eAttribute, pValueNode, pNode);
          }
        }
      }
      break;
    }
    default:
      break;
  }

  if (!bNeedFindContainer)
    return;

  CXFA_Node* pParent = this;
  while (pParent && !pParent->IsContainerNode())
    pParent = pParent->GetParent();

  if (pParent)
    pLayoutPro->AddChangedContainer(pParent);
}

void CXFA_Node::SyncValue(const WideString& wsValue, bool bNotify) {
  WideString wsFormatValue = wsValue;
  CXFA_Node* pContainerNode = GetContainerNode();
  if (pContainerNode)
    wsFormatValue = pContainerNode->GetFormatDataValue(wsValue);

  JSObject()->SetContent(wsValue, wsFormatValue, bNotify, false, true);
}

WideString CXFA_Node::GetRawValue() {
  return JSObject()->GetContent(false);
}

int32_t CXFA_Node::GetRotate() {
  Optional<int32_t> degrees =
      JSObject()->TryInteger(XFA_Attribute::Rotate, false);
  return degrees ? XFA_MapRotation(*degrees) / 90 * 90 : 0;
}

CXFA_Border* CXFA_Node::GetBorderIfExists() const {
  return JSObject()->GetProperty<CXFA_Border>(0, XFA_Element::Border);
}

CXFA_Border* CXFA_Node::GetOrCreateBorderIfPossible() {
  return JSObject()->GetOrCreateProperty<CXFA_Border>(0, XFA_Element::Border);
}

CXFA_Caption* CXFA_Node::GetCaptionIfExists() const {
  return JSObject()->GetProperty<CXFA_Caption>(0, XFA_Element::Caption);
}

CXFA_Font* CXFA_Node::GetOrCreateFontIfPossible() {
  return JSObject()->GetOrCreateProperty<CXFA_Font>(0, XFA_Element::Font);
}

CXFA_Font* CXFA_Node::GetFontIfExists() const {
  return JSObject()->GetProperty<CXFA_Font>(0, XFA_Element::Font);
}

float CXFA_Node::GetFontSize() const {
  CXFA_Font* font = GetFontIfExists();
  float fFontSize = font ? font->GetFontSize() : 10.0f;
  return fFontSize < 0.1f ? 10.0f : fFontSize;
}

float CXFA_Node::GetLineHeight() const {
  float fLineHeight = 0;
  CXFA_Para* para = GetParaIfExists();
  if (para)
    fLineHeight = para->GetLineHeight();

  if (fLineHeight < 1)
    fLineHeight = GetFontSize() * 1.2f;
  return fLineHeight;
}

FX_ARGB CXFA_Node::GetTextColor() const {
  CXFA_Font* font = GetFontIfExists();
  return font ? font->GetColor() : 0xFF000000;
}

CXFA_Margin* CXFA_Node::GetMarginIfExists() const {
  return JSObject()->GetProperty<CXFA_Margin>(0, XFA_Element::Margin);
}

CXFA_Para* CXFA_Node::GetParaIfExists() const {
  return JSObject()->GetProperty<CXFA_Para>(0, XFA_Element::Para);
}

bool CXFA_Node::IsOpenAccess() {
  for (auto* pNode = this; pNode; pNode = pNode->GetContainerParent()) {
    XFA_AttributeValue iAcc = pNode->JSObject()->GetEnum(XFA_Attribute::Access);
    if (iAcc != XFA_AttributeValue::Open)
      return false;
  }
  return true;
}

CXFA_Value* CXFA_Node::GetDefaultValueIfExists() {
  CXFA_Node* pTemNode = GetTemplateNodeIfExists();
  return pTemNode ? pTemNode->JSObject()->GetProperty<CXFA_Value>(
                        0, XFA_Element::Value)
                  : nullptr;
}

CXFA_Value* CXFA_Node::GetFormValueIfExists() const {
  return JSObject()->GetProperty<CXFA_Value>(0, XFA_Element::Value);
}

CXFA_Calculate* CXFA_Node::GetCalculateIfExists() const {
  return JSObject()->GetProperty<CXFA_Calculate>(0, XFA_Element::Calculate);
}

CXFA_Validate* CXFA_Node::GetValidateIfExists() const {
  return JSObject()->GetProperty<CXFA_Validate>(0, XFA_Element::Validate);
}

CXFA_Validate* CXFA_Node::GetOrCreateValidateIfPossible() {
  return JSObject()->GetOrCreateProperty<CXFA_Validate>(0,
                                                        XFA_Element::Validate);
}

CXFA_Bind* CXFA_Node::GetBindIfExists() const {
  return JSObject()->GetProperty<CXFA_Bind>(0, XFA_Element::Bind);
}

Optional<XFA_AttributeValue> CXFA_Node::GetIntactFromKeep(
    const CXFA_Keep* pKeep,
    XFA_AttributeValue eLayoutType) const {
  Optional<XFA_AttributeValue> intact =
      pKeep->JSObject()->TryEnum(XFA_Attribute::Intact, false);
  if (!intact.has_value())
    return {};

  if (intact.value() != XFA_AttributeValue::None ||
      eLayoutType != XFA_AttributeValue::Row ||
      m_pDocument->GetCurVersionMode() >= XFA_VERSION_208) {
    return intact;
  }

  CXFA_Node* pPreviewRow = GetPrevContainerSibling();
  if (!pPreviewRow || pPreviewRow->JSObject()->GetEnum(XFA_Attribute::Layout) !=
                          XFA_AttributeValue::Row) {
    return intact;
  }

  Optional<XFA_AttributeValue> value =
      pKeep->JSObject()->TryEnum(XFA_Attribute::Previous, false);
  if (value && (*value == XFA_AttributeValue::ContentArea ||
                *value == XFA_AttributeValue::PageArea)) {
    return XFA_AttributeValue::ContentArea;
  }

  CXFA_Keep* pNode =
      pPreviewRow->GetFirstChildByClass<CXFA_Keep>(XFA_Element::Keep);
  if (!pNode)
    return intact;

  Optional<XFA_AttributeValue> ret =
      pNode->JSObject()->TryEnum(XFA_Attribute::Next, false);
  if (!ret)
    return intact;

  return (*ret == XFA_AttributeValue::ContentArea ||
          *ret == XFA_AttributeValue::PageArea)
             ? XFA_AttributeValue::ContentArea
             : intact;
}

Optional<float> CXFA_Node::TryWidth() {
  return JSObject()->TryMeasureAsFloat(XFA_Attribute::W);
}

Optional<float> CXFA_Node::TryHeight() {
  return JSObject()->TryMeasureAsFloat(XFA_Attribute::H);
}

Optional<float> CXFA_Node::TryMinWidth() {
  return JSObject()->TryMeasureAsFloat(XFA_Attribute::MinW);
}

Optional<float> CXFA_Node::TryMinHeight() {
  return JSObject()->TryMeasureAsFloat(XFA_Attribute::MinH);
}

Optional<float> CXFA_Node::TryMaxWidth() {
  return JSObject()->TryMeasureAsFloat(XFA_Attribute::MaxW);
}

Optional<float> CXFA_Node::TryMaxHeight() {
  return JSObject()->TryMeasureAsFloat(XFA_Attribute::MaxH);
}

CXFA_Node* CXFA_Node::GetExclGroupIfExists() {
  CXFA_Node* pExcl = GetParent();
  if (!pExcl || pExcl->GetElementType() != XFA_Element::ExclGroup)
    return nullptr;
  return pExcl;
}

int32_t CXFA_Node::ProcessEvent(CXFA_FFDocView* docView,
                                XFA_AttributeValue iActivity,
                                CXFA_EventParam* pEventParam) {
  if (GetElementType() == XFA_Element::Draw)
    return XFA_EVENTERROR_NotExist;

  std::vector<CXFA_Event*> eventArray =
      GetEventByActivity(iActivity, pEventParam->m_bIsFormReady);
  bool first = true;
  int32_t iRet = XFA_EVENTERROR_NotExist;
  for (CXFA_Event* event : eventArray) {
    int32_t result = ProcessEvent(docView, event, pEventParam);
    if (first || result == XFA_EVENTERROR_Success)
      iRet = result;
    first = false;
  }
  return iRet;
}

int32_t CXFA_Node::ProcessEvent(CXFA_FFDocView* docView,
                                CXFA_Event* event,
                                CXFA_EventParam* pEventParam) {
  if (!event)
    return XFA_EVENTERROR_NotExist;

  switch (event->GetEventType()) {
    case XFA_Element::Execute:
      break;
    case XFA_Element::Script:
      return ExecuteScript(docView, event->GetScriptIfExists(), pEventParam);
    case XFA_Element::SignData:
      break;
    case XFA_Element::Submit: {
// TODO(crbug.com/867485): Submit is disabled for now. Fix it and reenable this
// code.
#ifdef PDF_XFA_ELEMENT_SUBMIT_ENABLED
      CXFA_Submit* submit = event->GetSubmitIfExists();
      if (!submit)
        return XFA_EVENTERROR_NotExist;
      return docView->GetDoc()->GetDocEnvironment()->Submit(docView->GetDoc(),
                                                            submit);
#else
      return XFA_EVENTERROR_Disabled;
#endif  // PDF_XFA_ELEMENT_SUBMIT_ENABLED
    }
    default:
      break;
  }
  return XFA_EVENTERROR_NotExist;
}

int32_t CXFA_Node::ProcessCalculate(CXFA_FFDocView* docView) {
  if (GetElementType() == XFA_Element::Draw)
    return XFA_EVENTERROR_NotExist;

  CXFA_Calculate* calc = GetCalculateIfExists();
  if (!calc)
    return XFA_EVENTERROR_NotExist;
  if (IsUserInteractive())
    return XFA_EVENTERROR_Disabled;

  CXFA_EventParam EventParam;
  EventParam.m_eType = XFA_EVENT_Calculate;
  int32_t iRet = ExecuteScript(docView, calc->GetScriptIfExists(), &EventParam);
  if (iRet != XFA_EVENTERROR_Success)
    return iRet;

  if (GetRawValue() != EventParam.m_wsResult) {
    SetValue(XFA_VALUEPICTURE_Raw, EventParam.m_wsResult);
    UpdateUIDisplay(docView, nullptr);
  }
  return XFA_EVENTERROR_Success;
}

void CXFA_Node::ProcessScriptTestValidate(CXFA_FFDocView* docView,
                                          CXFA_Validate* validate,
                                          int32_t iRet,
                                          bool bRetValue,
                                          bool bVersionFlag) {
  if (iRet != XFA_EVENTERROR_Success)
    return;
  if (bRetValue)
    return;

  IXFA_AppProvider* pAppProvider =
      docView->GetDoc()->GetApp()->GetAppProvider();
  if (!pAppProvider)
    return;

  WideString wsTitle = pAppProvider->GetAppTitle();
  WideString wsScriptMsg = validate->GetScriptMessageText();
  if (validate->GetScriptTest() == XFA_AttributeValue::Warning) {
    if (IsUserInteractive())
      return;
    if (wsScriptMsg.IsEmpty())
      wsScriptMsg = GetValidateMessage(false, bVersionFlag);

    if (bVersionFlag) {
      pAppProvider->MsgBox(wsScriptMsg, wsTitle,
                           static_cast<uint32_t>(AlertIcon::kWarning),
                           static_cast<uint32_t>(AlertButton::kOK));
      return;
    }
    if (pAppProvider->MsgBox(wsScriptMsg, wsTitle,
                             static_cast<uint32_t>(AlertIcon::kWarning),
                             static_cast<uint32_t>(AlertButton::kYesNo)) ==
        static_cast<uint32_t>(AlertReturn::kYes)) {
      SetFlag(XFA_NodeFlag_UserInteractive);
    }
    return;
  }

  if (wsScriptMsg.IsEmpty())
    wsScriptMsg = GetValidateMessage(true, bVersionFlag);
  pAppProvider->MsgBox(wsScriptMsg, wsTitle,
                       static_cast<uint32_t>(AlertIcon::kError),
                       static_cast<uint32_t>(AlertButton::kOK));
}

int32_t CXFA_Node::ProcessFormatTestValidate(CXFA_FFDocView* docView,
                                             CXFA_Validate* validate,
                                             bool bVersionFlag) {
  WideString wsPicture = validate->GetPicture();
  if (wsPicture.IsEmpty())
    return XFA_EVENTERROR_NotExist;

  WideString wsRawValue = GetRawValue();
  if (wsRawValue.IsEmpty())
    return XFA_EVENTERROR_Error;

  LocaleIface* pLocale = GetLocale();
  if (!pLocale)
    return XFA_EVENTERROR_NotExist;

  CXFA_LocaleValue lcValue = XFA_GetLocaleValue(this);
  if (lcValue.ValidateValue(lcValue.GetValue(), wsPicture, pLocale, nullptr))
    return XFA_EVENTERROR_Success;

  IXFA_AppProvider* pAppProvider =
      docView->GetDoc()->GetApp()->GetAppProvider();
  if (!pAppProvider)
    return XFA_EVENTERROR_NotExist;

  WideString wsFormatMsg = validate->GetFormatMessageText();
  WideString wsTitle = pAppProvider->GetAppTitle();
  if (validate->GetFormatTest() == XFA_AttributeValue::Error) {
    if (wsFormatMsg.IsEmpty())
      wsFormatMsg = GetValidateMessage(true, bVersionFlag);
    pAppProvider->MsgBox(wsFormatMsg, wsTitle,
                         static_cast<uint32_t>(AlertIcon::kError),
                         static_cast<uint32_t>(AlertButton::kOK));
    return XFA_EVENTERROR_Error;
  }

  if (wsFormatMsg.IsEmpty())
    wsFormatMsg = GetValidateMessage(false, bVersionFlag);

  if (bVersionFlag) {
    pAppProvider->MsgBox(wsFormatMsg, wsTitle,
                         static_cast<uint32_t>(AlertIcon::kWarning),
                         static_cast<uint32_t>(AlertButton::kOK));
    return XFA_EVENTERROR_Error;
  }

  if (pAppProvider->MsgBox(wsFormatMsg, wsTitle,
                           static_cast<uint32_t>(AlertIcon::kWarning),
                           static_cast<uint32_t>(AlertButton::kYesNo)) ==
      static_cast<uint32_t>(AlertReturn::kYes)) {
    SetFlag(XFA_NodeFlag_UserInteractive);
  }

  return XFA_EVENTERROR_Error;
}

int32_t CXFA_Node::ProcessNullTestValidate(CXFA_FFDocView* docView,
                                           CXFA_Validate* validate,
                                           int32_t iFlags,
                                           bool bVersionFlag) {
  if (!GetValue(XFA_VALUEPICTURE_Raw).IsEmpty())
    return XFA_EVENTERROR_Success;
  if (m_bIsNull && m_bPreNull)
    return XFA_EVENTERROR_Success;

  XFA_AttributeValue eNullTest = validate->GetNullTest();
  WideString wsNullMsg = validate->GetNullMessageText();
  if (iFlags & 0x01) {
    int32_t iRet = XFA_EVENTERROR_Success;
    if (eNullTest != XFA_AttributeValue::Disabled)
      iRet = XFA_EVENTERROR_Error;

    if (!wsNullMsg.IsEmpty()) {
      if (eNullTest != XFA_AttributeValue::Disabled) {
        docView->m_arrNullTestMsg.push_back(wsNullMsg);
        return XFA_EVENTERROR_Error;
      }
      return XFA_EVENTERROR_Success;
    }
    return iRet;
  }
  if (wsNullMsg.IsEmpty() && bVersionFlag &&
      eNullTest != XFA_AttributeValue::Disabled) {
    return XFA_EVENTERROR_Error;
  }
  IXFA_AppProvider* pAppProvider =
      docView->GetDoc()->GetApp()->GetAppProvider();
  if (!pAppProvider)
    return XFA_EVENTERROR_NotExist;

  WideString wsCaptionName;
  WideString wsTitle = pAppProvider->GetAppTitle();
  switch (eNullTest) {
    case XFA_AttributeValue::Error: {
      if (wsNullMsg.IsEmpty()) {
        wsCaptionName = GetValidateCaptionName(bVersionFlag);
        wsNullMsg = wsCaptionName + L" cannot be blank.";
      }
      pAppProvider->MsgBox(wsNullMsg, wsTitle,
                           static_cast<uint32_t>(AlertIcon::kStatus),
                           static_cast<uint32_t>(AlertButton::kOK));
      return XFA_EVENTERROR_Error;
    }
    case XFA_AttributeValue::Warning: {
      if (IsUserInteractive())
        return true;

      if (wsNullMsg.IsEmpty()) {
        wsCaptionName = GetValidateCaptionName(bVersionFlag);
        wsNullMsg = wsCaptionName +
                    L" cannot be blank. To ignore validations for " +
                    wsCaptionName + L", click Ignore.";
      }
      if (pAppProvider->MsgBox(wsNullMsg, wsTitle,
                               static_cast<uint32_t>(AlertIcon::kWarning),
                               static_cast<uint32_t>(AlertButton::kYesNo)) ==
          static_cast<uint32_t>(AlertReturn::kYes)) {
        SetFlag(XFA_NodeFlag_UserInteractive);
      }
      return XFA_EVENTERROR_Error;
    }
    case XFA_AttributeValue::Disabled:
    default:
      break;
  }
  return XFA_EVENTERROR_Success;
}

int32_t CXFA_Node::ProcessValidate(CXFA_FFDocView* docView, int32_t iFlags) {
  if (GetElementType() == XFA_Element::Draw)
    return XFA_EVENTERROR_NotExist;

  CXFA_Validate* validate = GetValidateIfExists();
  if (!validate)
    return XFA_EVENTERROR_NotExist;

  bool bInitDoc = validate->NeedsInitApp();
  bool bStatus = docView->GetLayoutStatus() < XFA_DOCVIEW_LAYOUTSTATUS_End;
  int32_t iFormat = 0;
  int32_t iRet = XFA_EVENTERROR_NotExist;
  CXFA_Script* script = validate->GetScriptIfExists();
  bool bRet = false;
  bool hasBoolResult = (bInitDoc || bStatus) && GetRawValue().IsEmpty();
  if (script) {
    CXFA_EventParam eParam;
    eParam.m_eType = XFA_EVENT_Validate;
    eParam.m_pTarget = this;
    std::tie(iRet, bRet) = ExecuteBoolScript(docView, script, &eParam);
  }

  XFA_VERSION version = docView->GetDoc()->GetXFADoc()->GetCurVersionMode();
  bool bVersionFlag = false;
  if (version < XFA_VERSION_208)
    bVersionFlag = true;

  if (bInitDoc) {
    validate->ClearFlag(XFA_NodeFlag_NeedsInitApp);
  } else {
    iFormat = ProcessFormatTestValidate(docView, validate, bVersionFlag);
    if (!bVersionFlag) {
      bVersionFlag =
          docView->GetDoc()->GetXFADoc()->HasFlag(XFA_DOCFLAG_Scripting);
    }

    iRet |= ProcessNullTestValidate(docView, validate, iFlags, bVersionFlag);
  }

  if (iFormat != XFA_EVENTERROR_Success && hasBoolResult)
    ProcessScriptTestValidate(docView, validate, iRet, bRet, bVersionFlag);

  return iRet | iFormat;
}

WideString CXFA_Node::GetValidateCaptionName(bool bVersionFlag) {
  WideString wsCaptionName;

  if (!bVersionFlag) {
    CXFA_Caption* caption = GetCaptionIfExists();
    if (caption) {
      CXFA_Value* capValue = caption->GetValueIfExists();
      if (capValue) {
        CXFA_Text* captionText = capValue->GetTextIfExists();
        if (captionText)
          wsCaptionName = captionText->GetContent();
      }
    }
  }
  if (!wsCaptionName.IsEmpty())
    return wsCaptionName;
  return JSObject()->GetCData(XFA_Attribute::Name);
}

WideString CXFA_Node::GetValidateMessage(bool bError, bool bVersionFlag) {
  WideString wsCaptionName = GetValidateCaptionName(bVersionFlag);
  if (bVersionFlag)
    return wsCaptionName + L" validation failed";
  WideString result =
      L"The value you entered for " + wsCaptionName + L" is invalid.";
  if (!bError) {
    result +=
        L" To ignore validations for " + wsCaptionName + L", click Ignore.";
  }
  return result;
}

int32_t CXFA_Node::ExecuteScript(CXFA_FFDocView* docView,
                                 CXFA_Script* script,
                                 CXFA_EventParam* pEventParam) {
  bool bRet;
  int32_t iRet;
  std::tie(iRet, bRet) = ExecuteBoolScript(docView, script, pEventParam);
  return iRet;
}

std::pair<int32_t, bool> CXFA_Node::ExecuteBoolScript(
    CXFA_FFDocView* docView,
    CXFA_Script* script,
    CXFA_EventParam* pEventParam) {
  if (m_ExecuteRecursionDepth > kMaxExecuteRecursion)
    return {XFA_EVENTERROR_Success, false};

  ASSERT(pEventParam);
  if (!script)
    return {XFA_EVENTERROR_NotExist, false};
  if (script->GetRunAt() == XFA_AttributeValue::Server)
    return {XFA_EVENTERROR_Disabled, false};

  WideString wsExpression = script->GetExpression();
  if (wsExpression.IsEmpty())
    return {XFA_EVENTERROR_NotExist, false};

  CXFA_Script::Type eScriptType = script->GetContentType();
  if (eScriptType == CXFA_Script::Type::Unknown)
    return {XFA_EVENTERROR_Success, false};

  CXFA_FFDoc* pDoc = docView->GetDoc();
  CFXJSE_Engine* pContext = pDoc->GetXFADoc()->GetScriptContext();
  pContext->SetEventParam(pEventParam);
  pContext->SetRunAtType(script->GetRunAt());

  std::vector<CXFA_Node*> refNodes;
  if (pEventParam->m_eType == XFA_EVENT_InitCalculate ||
      pEventParam->m_eType == XFA_EVENT_Calculate) {
    pContext->SetNodesOfRunScript(&refNodes);
  }

  auto pTmpRetValue = pdfium::MakeUnique<CFXJSE_Value>(pContext->GetIsolate());
  bool bRet = false;
  {
    AutoRestorer<uint8_t> restorer(&m_ExecuteRecursionDepth);
    ++m_ExecuteRecursionDepth;
    bRet = pContext->RunScript(eScriptType, wsExpression.AsStringView(),
                               pTmpRetValue.get(), this);
  }

  int32_t iRet = XFA_EVENTERROR_Error;
  if (bRet) {
    iRet = XFA_EVENTERROR_Success;
    if (pEventParam->m_eType == XFA_EVENT_Calculate ||
        pEventParam->m_eType == XFA_EVENT_InitCalculate) {
      if (!pTmpRetValue->IsUndefined()) {
        if (!pTmpRetValue->IsNull())
          pEventParam->m_wsResult = pTmpRetValue->ToWideString();

        iRet = XFA_EVENTERROR_Success;
      } else {
        iRet = XFA_EVENTERROR_Error;
      }
      if (pEventParam->m_eType == XFA_EVENT_InitCalculate) {
        if ((iRet == XFA_EVENTERROR_Success) &&
            (GetRawValue() != pEventParam->m_wsResult)) {
          SetValue(XFA_VALUEPICTURE_Raw, pEventParam->m_wsResult);
          docView->AddValidateNode(this);
        }
      }
      for (CXFA_Node* pRefNode : refNodes) {
        if (pRefNode == this)
          continue;

        CXFA_CalcData* pGlobalData = pRefNode->JSObject()->GetCalcData();
        if (!pGlobalData) {
          pRefNode->JSObject()->SetCalcData(
              pdfium::MakeUnique<CXFA_CalcData>());
          pGlobalData = pRefNode->JSObject()->GetCalcData();
        }
        if (!pdfium::ContainsValue(pGlobalData->m_Globals, this))
          pGlobalData->m_Globals.push_back(this);
      }
    }
  }
  pContext->SetNodesOfRunScript(nullptr);
  pContext->SetEventParam(nullptr);

  return {iRet, pTmpRetValue->IsBoolean() ? pTmpRetValue->ToBoolean() : false};
}

std::pair<XFA_FFWidgetType, CXFA_Ui*>
CXFA_Node::CreateChildUIAndValueNodesIfNeeded() {
  XFA_Element eType = GetElementType();
  ASSERT(eType == XFA_Element::Field || eType == XFA_Element::Draw);

  // Both Field and Draw have a UI property. We should always be able to
  // retrieve or create the UI element. If we can't something is wrong.
  CXFA_Ui* pUI = JSObject()->GetOrCreateProperty<CXFA_Ui>(0, XFA_Element::Ui);
  ASSERT(pUI);

  CXFA_Node* pUIChild = nullptr;
  // Search through the children of the UI node to see if we have any of our
  // One-Of entries. If so, that is the node associated with our UI.
  for (CXFA_Node* pChild = pUI->GetFirstChild(); pChild;
       pChild = pChild->GetNextSibling()) {
    if (pUI->IsAOneOfChild(pChild)) {
      pUIChild = pChild;
      break;
    }
  }

  XFA_FFWidgetType widget_type = XFA_FFWidgetType::kNone;
  XFA_Element expected_ui_child_type = XFA_Element::Unknown;

  // Both Field and Draw nodes have a Value child. So, we should either always
  // have it, or always create it. If we don't get the Value child for some
  // reason something has gone really wrong.
  CXFA_Value* value =
      JSObject()->GetOrCreateProperty<CXFA_Value>(0, XFA_Element::Value);
  ASSERT(value);

  // The Value nodes only have One-Of children. So, if we have a first child
  // that child must be the type we want to use.
  CXFA_Node* child = value->GetFirstChild();
  if (child) {
    switch (child->GetElementType()) {
      case XFA_Element::Boolean:
        expected_ui_child_type = XFA_Element::CheckButton;
        break;
      case XFA_Element::Integer:
      case XFA_Element::Decimal:
      case XFA_Element::Float:
        expected_ui_child_type = XFA_Element::NumericEdit;
        break;
      case XFA_Element::ExData:
      case XFA_Element::Text:
        expected_ui_child_type = XFA_Element::TextEdit;
        widget_type = XFA_FFWidgetType::kText;
        break;
      case XFA_Element::Date:
      case XFA_Element::Time:
      case XFA_Element::DateTime:
        expected_ui_child_type = XFA_Element::DateTimeEdit;
        break;
      case XFA_Element::Image:
        expected_ui_child_type = XFA_Element::ImageEdit;
        widget_type = XFA_FFWidgetType::kImage;
        break;
      case XFA_Element::Arc:
        expected_ui_child_type = XFA_Element::DefaultUi;
        widget_type = XFA_FFWidgetType::kArc;
        break;
      case XFA_Element::Line:
        expected_ui_child_type = XFA_Element::DefaultUi;
        widget_type = XFA_FFWidgetType::kLine;
        break;
      case XFA_Element::Rectangle:
        expected_ui_child_type = XFA_Element::DefaultUi;
        widget_type = XFA_FFWidgetType::kRectangle;
        break;
      default:
        NOTREACHED();
        break;
    }
  }

  if (eType == XFA_Element::Draw) {
    if (pUIChild && pUIChild->GetElementType() == XFA_Element::TextEdit) {
      widget_type = XFA_FFWidgetType::kText;
    } else if (pUIChild &&
               pUIChild->GetElementType() == XFA_Element::ImageEdit) {
      widget_type = XFA_FFWidgetType::kImage;
    } else if (widget_type == XFA_FFWidgetType::kNone) {
      widget_type = XFA_FFWidgetType::kText;
    }
  } else if (eType == XFA_Element::Field) {
    if (pUIChild && pUIChild->GetElementType() == XFA_Element::DefaultUi) {
      widget_type = XFA_FFWidgetType::kTextEdit;
    } else if (pUIChild) {
      widget_type = pUIChild->GetDefaultFFWidgetType();
    } else if (expected_ui_child_type == XFA_Element::Unknown) {
      widget_type = XFA_FFWidgetType::kTextEdit;
    }
  } else {
    NOTREACHED();
  }

  if (!pUIChild) {
    if (expected_ui_child_type == XFA_Element::Unknown)
      expected_ui_child_type = XFA_Element::TextEdit;
    pUIChild = pUI->JSObject()->GetOrCreateProperty<CXFA_Node>(
        0, expected_ui_child_type);
  }

  CreateValueNodeIfNeeded(value, pUIChild);
  return {widget_type, pUI};
}

XFA_FFWidgetType CXFA_Node::GetDefaultFFWidgetType() const {
  NOTREACHED();
  return XFA_FFWidgetType::kNone;
}

CXFA_Node* CXFA_Node::CreateUINodeIfNeeded(CXFA_Ui* ui, XFA_Element type) {
  return ui->JSObject()->GetOrCreateProperty<CXFA_Node>(0, type);
}

void CXFA_Node::CreateValueNodeIfNeeded(CXFA_Value* value,
                                        CXFA_Node* pUIChild) {
  // Value nodes only have one child. If we have one already we're done.
  if (value->GetFirstChild() != nullptr)
    return;

  // Create the Value node for our UI if needed.
  XFA_Element valueType = pUIChild->GetValueNodeType();
  if (pUIChild->GetElementType() == XFA_Element::CheckButton) {
    CXFA_Items* pItems = GetChild<CXFA_Items>(0, XFA_Element::Items, false);
    if (pItems) {
      CXFA_Node* pItem =
          pItems->GetChild<CXFA_Node>(0, XFA_Element::Unknown, false);
      if (pItem)
        valueType = pItem->GetElementType();
    }
  }
  value->JSObject()->GetOrCreateProperty<CXFA_Node>(0, valueType);
}

XFA_Element CXFA_Node::GetValueNodeType() const {
  return XFA_Element::Text;
}

CXFA_Node* CXFA_Node::GetUIChildNode() {
  ASSERT(HasCreatedUIWidget());

  if (ff_widget_type_ != XFA_FFWidgetType::kNone)
    return ui_ ? ui_->GetFirstChild() : nullptr;

  XFA_Element type = GetElementType();
  if (type == XFA_Element::Field || type == XFA_Element::Draw) {
    std::tie(ff_widget_type_, ui_) = CreateChildUIAndValueNodesIfNeeded();
  } else if (type == XFA_Element::Subform) {
    ff_widget_type_ = XFA_FFWidgetType::kSubform;
  } else if (type == XFA_Element::ExclGroup) {
    ff_widget_type_ = XFA_FFWidgetType::kExclGroup;
  } else {
    NOTREACHED();
  }
  return ui_ ? ui_->GetFirstChild() : nullptr;
}

XFA_FFWidgetType CXFA_Node::GetFFWidgetType() {
  GetUIChildNode();
  return ff_widget_type_;
}

CXFA_Border* CXFA_Node::GetUIBorder() {
  CXFA_Node* pUIChild = GetUIChildNode();
  return pUIChild ? pUIChild->JSObject()->GetProperty<CXFA_Border>(
                        0, XFA_Element::Border)
                  : nullptr;
}

CFX_RectF CXFA_Node::GetUIMargin() {
  CXFA_Node* pUIChild = GetUIChildNode();
  if (!pUIChild)
    return CFX_RectF();

  CXFA_Margin* mgUI =
      pUIChild->JSObject()->GetProperty<CXFA_Margin>(0, XFA_Element::Margin);
  if (!mgUI)
    return CFX_RectF();

  CXFA_Border* border = GetUIBorder();
  if (border && border->GetPresence() != XFA_AttributeValue::Visible)
    return CFX_RectF();

  Optional<float> left = mgUI->TryLeftInset();
  Optional<float> top = mgUI->TryTopInset();
  Optional<float> right = mgUI->TryRightInset();
  Optional<float> bottom = mgUI->TryBottomInset();
  if (border) {
    bool bVisible = false;
    float fThickness = 0;
    XFA_AttributeValue iType = XFA_AttributeValue::Unknown;
    std::tie(iType, bVisible, fThickness) = border->Get3DStyle();
    if (!left || !top || !right || !bottom) {
      std::vector<CXFA_Stroke*> strokes = border->GetStrokes();
      if (!top)
        top = GetEdgeThickness(strokes, bVisible, 0);
      if (!right)
        right = GetEdgeThickness(strokes, bVisible, 1);
      if (!bottom)
        bottom = GetEdgeThickness(strokes, bVisible, 2);
      if (!left)
        left = GetEdgeThickness(strokes, bVisible, 3);
    }
  }
  return CFX_RectF(left.value_or(0.0), top.value_or(0.0), right.value_or(0.0),
                   bottom.value_or(0.0));
}

std::vector<CXFA_Event*> CXFA_Node::GetEventByActivity(
    XFA_AttributeValue iActivity,
    bool bIsFormReady) {
  std::vector<CXFA_Event*> events;
  for (CXFA_Node* node : GetNodeList(0, XFA_Element::Event)) {
    auto* event = static_cast<CXFA_Event*>(node);
    if (event->GetActivity() != iActivity)
      continue;

    if (iActivity != XFA_AttributeValue::Ready) {
      events.push_back(event);
      continue;
    }

    WideString wsRef = event->GetRef();
    if (bIsFormReady) {
      if (wsRef == WideStringView(L"$form"))
        events.push_back(event);
      continue;
    }

    if (wsRef == WideStringView(L"$layout"))
      events.push_back(event);
  }
  return events;
}

void CXFA_Node::ResetData() {
  WideString wsValue;
  switch (GetFFWidgetType()) {
    case XFA_FFWidgetType::kImageEdit: {
      CXFA_Value* imageValue = GetDefaultValueIfExists();
      CXFA_Image* image = imageValue ? imageValue->GetImageIfExists() : nullptr;
      WideString wsContentType, wsHref;
      if (image) {
        wsValue = image->GetContent();
        wsContentType = image->GetContentType();
        wsHref = image->GetHref();
      }
      SetImageEdit(wsContentType, wsHref, wsValue);
      break;
    }
    case XFA_FFWidgetType::kExclGroup: {
      CXFA_Node* pNextChild = GetFirstContainerChild();
      while (pNextChild) {
        CXFA_Node* pChild = pNextChild;
        if (!pChild->IsWidgetReady())
          continue;

        bool done = false;
        if (wsValue.IsEmpty()) {
          CXFA_Value* defValue = pChild->GetDefaultValueIfExists();
          if (defValue) {
            wsValue = defValue->GetChildValueContent();
            SetValue(XFA_VALUEPICTURE_Raw, wsValue);
            pChild->SetValue(XFA_VALUEPICTURE_Raw, wsValue);
            done = true;
          }
        }
        if (!done) {
          CXFA_Items* pItems =
              pChild->GetChild<CXFA_Items>(0, XFA_Element::Items, false);
          if (!pItems)
            continue;

          WideString itemText;
          if (pItems->CountChildren(XFA_Element::Unknown, false) > 1) {
            itemText =
                pItems->GetChild<CXFA_Node>(1, XFA_Element::Unknown, false)
                    ->JSObject()
                    ->GetContent(false);
          }
          pChild->SetValue(XFA_VALUEPICTURE_Raw, itemText);
        }
        pNextChild = pChild->GetNextContainerSibling();
      }
      break;
    }
    case XFA_FFWidgetType::kChoiceList:
      ClearAllSelections();
      FALLTHROUGH;
    default: {
      CXFA_Value* defValue = GetDefaultValueIfExists();
      if (defValue)
        wsValue = defValue->GetChildValueContent();

      SetValue(XFA_VALUEPICTURE_Raw, wsValue);
      break;
    }
  }
}

void CXFA_Node::SetImageEdit(const WideString& wsContentType,
                             const WideString& wsHref,
                             const WideString& wsData) {
  CXFA_Value* formValue = GetFormValueIfExists();
  CXFA_Image* image = formValue ? formValue->GetImageIfExists() : nullptr;
  if (image) {
    image->SetContentType(WideString(wsContentType));
    image->SetHref(wsHref);
  }

  JSObject()->SetContent(wsData, GetFormatDataValue(wsData), true, false, true);

  CXFA_Node* pBind = GetBindData();
  if (!pBind) {
    if (image)
      image->SetTransferEncoding(XFA_AttributeValue::Base64);
    return;
  }
  pBind->JSObject()->SetCData(XFA_Attribute::ContentType, wsContentType, false,
                              false);
  CXFA_Node* pHrefNode = pBind->GetFirstChild();
  if (pHrefNode) {
    pHrefNode->JSObject()->SetCData(XFA_Attribute::Value, wsHref, false, false);
    return;
  }
  CFX_XMLElement* pElement = ToXMLElement(pBind->GetXMLMappingNode());
  pElement->SetAttribute(L"href", wsHref);
}

CXFA_FFWidget* CXFA_Node::GetNextWidget(CXFA_FFWidget* pWidget) {
  return ToFFWidget(pWidget->GetNext());
}

void CXFA_Node::UpdateUIDisplay(CXFA_FFDocView* docView,
                                CXFA_FFWidget* pExcept) {
  CXFA_FFWidget* pWidget = docView->GetWidgetForNode(this);
  for (; pWidget; pWidget = GetNextWidget(pWidget)) {
    if (pWidget == pExcept || !pWidget->IsLoaded() ||
        (GetFFWidgetType() != XFA_FFWidgetType::kCheckButton &&
         pWidget->IsFocused())) {
      continue;
    }
    pWidget->UpdateFWLData();
    pWidget->InvalidateRect();
  }
}

void CXFA_Node::CalcCaptionSize(CXFA_FFDoc* doc, CFX_SizeF* pszCap) {
  CXFA_Caption* caption = GetCaptionIfExists();
  if (!caption || !caption->IsVisible())
    return;

  LoadCaption(doc);

  const float fCapReserve = caption->GetReserve();
  const XFA_AttributeValue iCapPlacement = caption->GetPlacementType();
  const bool bReserveExit = fCapReserve > 0.01;
  const bool bVert = iCapPlacement == XFA_AttributeValue::Top ||
                     iCapPlacement == XFA_AttributeValue::Bottom;
  CXFA_TextLayout* pCapTextLayout =
      m_pLayoutData->AsFieldLayoutData()->m_pCapTextLayout.get();
  if (pCapTextLayout) {
    if (!bVert && GetFFWidgetType() != XFA_FFWidgetType::kButton)
      pszCap->width = fCapReserve;

    CFX_SizeF minSize;
    *pszCap = pCapTextLayout->CalcSize(minSize, *pszCap);
    if (bReserveExit)
      bVert ? pszCap->height = fCapReserve : pszCap->width = fCapReserve;
  } else {
    float fFontSize = 10.0f;
    CXFA_Font* font = caption->GetFontIfExists();
    if (font) {
      fFontSize = font->GetFontSize();
    } else {
      CXFA_Font* widgetfont = GetFontIfExists();
      if (widgetfont)
        fFontSize = widgetfont->GetFontSize();
    }

    if (bVert) {
      pszCap->height = fCapReserve > 0 ? fCapReserve : fFontSize;
    } else {
      pszCap->width = fCapReserve > 0 ? fCapReserve : 0;
      pszCap->height = fFontSize;
    }
  }

  CXFA_Margin* captionMargin = caption->GetMarginIfExists();
  if (!captionMargin)
    return;

  float fLeftInset = captionMargin->GetLeftInset();
  float fTopInset = captionMargin->GetTopInset();
  float fRightInset = captionMargin->GetRightInset();
  float fBottomInset = captionMargin->GetBottomInset();
  if (bReserveExit) {
    bVert ? (pszCap->width += fLeftInset + fRightInset)
          : (pszCap->height += fTopInset + fBottomInset);
  } else {
    pszCap->width += fLeftInset + fRightInset;
    pszCap->height += fTopInset + fBottomInset;
  }
}

bool CXFA_Node::CalculateFieldAutoSize(CXFA_FFDoc* doc, CFX_SizeF* pSize) {
  CFX_SizeF szCap;
  CalcCaptionSize(doc, &szCap);

  CFX_RectF rtUIMargin = GetUIMargin();
  pSize->width += rtUIMargin.left + rtUIMargin.width;
  pSize->height += rtUIMargin.top + rtUIMargin.height;
  if (szCap.width > 0 && szCap.height > 0) {
    CXFA_Caption* caption = GetCaptionIfExists();
    XFA_AttributeValue placement = caption
                                       ? caption->GetPlacementType()
                                       : CXFA_Caption::kDefaultPlacementType;
    switch (placement) {
      case XFA_AttributeValue::Left:
      case XFA_AttributeValue::Right:
      case XFA_AttributeValue::Inline: {
        pSize->width += szCap.width;
        pSize->height = std::max(pSize->height, szCap.height);
      } break;
      case XFA_AttributeValue::Top:
      case XFA_AttributeValue::Bottom: {
        pSize->height += szCap.height;
        pSize->width = std::max(pSize->width, szCap.width);
        break;
      }
      default:
        break;
    }
  }
  return CalculateWidgetAutoSize(pSize);
}

bool CXFA_Node::CalculateWidgetAutoSize(CFX_SizeF* pSize) {
  CXFA_Margin* margin = GetMarginIfExists();
  if (margin) {
    pSize->width += margin->GetLeftInset() + margin->GetRightInset();
    pSize->height += margin->GetTopInset() + margin->GetBottomInset();
  }

  CXFA_Para* para = GetParaIfExists();
  if (para)
    pSize->width += para->GetMarginLeft() + para->GetTextIndent();

  Optional<float> width = TryWidth();
  if (width) {
    pSize->width = *width;
  } else {
    Optional<float> min = TryMinWidth();
    if (min)
      pSize->width = std::max(pSize->width, *min);

    Optional<float> max = TryMaxWidth();
    if (max && *max > 0)
      pSize->width = std::min(pSize->width, *max);
  }

  Optional<float> height = TryHeight();
  if (height) {
    pSize->height = *height;
  } else {
    Optional<float> min = TryMinHeight();
    if (min)
      pSize->height = std::max(pSize->height, *min);

    Optional<float> max = TryMaxHeight();
    if (max && *max > 0)
      pSize->height = std::min(pSize->height, *max);
  }
  return true;
}

void CXFA_Node::CalculateTextContentSize(CXFA_FFDoc* doc, CFX_SizeF* pSize) {
  float fFontSize = GetFontSize();
  WideString wsText = GetValue(XFA_VALUEPICTURE_Display);
  if (wsText.IsEmpty()) {
    pSize->height += fFontSize;
    return;
  }

  wchar_t wcEnter = '\n';
  wchar_t wsLast = wsText[wsText.GetLength() - 1];
  if (wsLast == wcEnter)
    wsText = wsText + wcEnter;

  CXFA_FieldLayoutData* layoutData = m_pLayoutData->AsFieldLayoutData();
  if (!layoutData->m_pTextOut) {
    layoutData->m_pTextOut = pdfium::MakeUnique<CFDE_TextOut>();
    CFDE_TextOut* pTextOut = layoutData->m_pTextOut.get();
    pTextOut->SetFont(GetFDEFont(doc));
    pTextOut->SetFontSize(fFontSize);
    pTextOut->SetLineBreakTolerance(fFontSize * 0.2f);
    pTextOut->SetLineSpace(GetLineHeight());

    FDE_TextStyle dwStyles;
    dwStyles.last_line_height_ = true;
    if (GetFFWidgetType() == XFA_FFWidgetType::kTextEdit && IsMultiLine())
      dwStyles.line_wrap_ = true;

    pTextOut->SetStyles(dwStyles);
  }
  layoutData->m_pTextOut->CalcLogicSize(wsText, pSize);
}

bool CXFA_Node::CalculateTextEditAutoSize(CXFA_FFDoc* doc, CFX_SizeF* pSize) {
  if (pSize->width > 0) {
    CFX_SizeF szOrz = *pSize;
    CFX_SizeF szCap;
    CalcCaptionSize(doc, &szCap);
    bool bCapExit = szCap.width > 0.01 && szCap.height > 0.01;
    XFA_AttributeValue iCapPlacement = XFA_AttributeValue::Unknown;
    if (bCapExit) {
      CXFA_Caption* caption = GetCaptionIfExists();
      iCapPlacement = caption ? caption->GetPlacementType()
                              : CXFA_Caption::kDefaultPlacementType;
      switch (iCapPlacement) {
        case XFA_AttributeValue::Left:
        case XFA_AttributeValue::Right:
        case XFA_AttributeValue::Inline: {
          pSize->width -= szCap.width;
          break;
        }
        default:
          break;
      }
    }
    CFX_RectF rtUIMargin = GetUIMargin();
    pSize->width -= rtUIMargin.left + rtUIMargin.width;
    CXFA_Margin* margin = GetMarginIfExists();
    if (margin)
      pSize->width -= margin->GetLeftInset() + margin->GetRightInset();

    CalculateTextContentSize(doc, pSize);
    pSize->height += rtUIMargin.top + rtUIMargin.height;
    if (bCapExit) {
      switch (iCapPlacement) {
        case XFA_AttributeValue::Left:
        case XFA_AttributeValue::Right:
        case XFA_AttributeValue::Inline: {
          pSize->height = std::max(pSize->height, szCap.height);
        } break;
        case XFA_AttributeValue::Top:
        case XFA_AttributeValue::Bottom: {
          pSize->height += szCap.height;
          break;
        }
        default:
          break;
      }
    }
    pSize->width = szOrz.width;
    return CalculateWidgetAutoSize(pSize);
  }
  CalculateTextContentSize(doc, pSize);
  return CalculateFieldAutoSize(doc, pSize);
}

bool CXFA_Node::CalculateCheckButtonAutoSize(CXFA_FFDoc* doc,
                                             CFX_SizeF* pSize) {
  float fCheckSize = GetCheckButtonSize();
  *pSize = CFX_SizeF(fCheckSize, fCheckSize);
  return CalculateFieldAutoSize(doc, pSize);
}

bool CXFA_Node::CalculatePushButtonAutoSize(CXFA_FFDoc* doc, CFX_SizeF* pSize) {
  CalcCaptionSize(doc, pSize);
  return CalculateWidgetAutoSize(pSize);
}

CFX_SizeF CXFA_Node::CalculateImageSize(float img_width,
                                        float img_height,
                                        const CFX_Size& dpi) {
  CFX_RectF rtImage(0, 0, XFA_UnitPx2Pt(img_width, dpi.width),
                    XFA_UnitPx2Pt(img_height, dpi.height));

  CFX_RectF rtFit;
  Optional<float> width = TryWidth();
  if (width) {
    rtFit.width = *width;
    GetWidthWithoutMargin(rtFit.width);
  } else {
    rtFit.width = rtImage.width;
  }

  Optional<float> height = TryHeight();
  if (height) {
    rtFit.height = *height;
    GetHeightWithoutMargin(rtFit.height);
  } else {
    rtFit.height = rtImage.height;
  }

  return rtFit.Size();
}

bool CXFA_Node::CalculateImageAutoSize(CXFA_FFDoc* doc, CFX_SizeF* pSize) {
  if (!GetImageImage())
    LoadImageImage(doc);

  pSize->clear();
  RetainPtr<CFX_DIBitmap> pBitmap = GetImageImage();
  if (!pBitmap)
    return CalculateWidgetAutoSize(pSize);

  *pSize = CalculateImageSize(pBitmap->GetWidth(), pBitmap->GetHeight(),
                              GetImageDpi());
  return CalculateWidgetAutoSize(pSize);
}

bool CXFA_Node::CalculateImageEditAutoSize(CXFA_FFDoc* doc, CFX_SizeF* pSize) {
  if (!GetImageEditImage())
    LoadImageEditImage(doc);

  pSize->clear();
  RetainPtr<CFX_DIBitmap> pBitmap = GetImageEditImage();
  if (!pBitmap)
    return CalculateFieldAutoSize(doc, pSize);

  *pSize = CalculateImageSize(pBitmap->GetWidth(), pBitmap->GetHeight(),
                              GetImageEditDpi());
  return CalculateFieldAutoSize(doc, pSize);
}

bool CXFA_Node::LoadImageImage(CXFA_FFDoc* doc) {
  InitLayoutData();
  return m_pLayoutData->AsImageLayoutData()->LoadImageData(doc, this);
}

bool CXFA_Node::LoadImageEditImage(CXFA_FFDoc* doc) {
  InitLayoutData();
  return m_pLayoutData->AsFieldLayoutData()->AsImageEditData()->LoadImageData(
      doc, this);
}

CFX_Size CXFA_Node::GetImageDpi() const {
  CXFA_ImageLayoutData* pData = m_pLayoutData->AsImageLayoutData();
  return CFX_Size(pData->m_iImageXDpi, pData->m_iImageYDpi);
}

CFX_Size CXFA_Node::GetImageEditDpi() const {
  CXFA_ImageEditData* pData =
      m_pLayoutData->AsFieldLayoutData()->AsImageEditData();
  return CFX_Size(pData->m_iImageXDpi, pData->m_iImageYDpi);
}

float CXFA_Node::CalculateWidgetAutoWidth(float fWidthCalc) {
  CXFA_Margin* margin = GetMarginIfExists();
  if (margin)
    fWidthCalc += margin->GetLeftInset() + margin->GetRightInset();

  Optional<float> min = TryMinWidth();
  if (min)
    fWidthCalc = std::max(fWidthCalc, *min);

  Optional<float> max = TryMaxWidth();
  if (max && *max > 0)
    fWidthCalc = std::min(fWidthCalc, *max);

  return fWidthCalc;
}

float CXFA_Node::GetWidthWithoutMargin(float fWidthCalc) const {
  CXFA_Margin* margin = GetMarginIfExists();
  if (margin)
    fWidthCalc -= margin->GetLeftInset() + margin->GetRightInset();
  return fWidthCalc;
}

float CXFA_Node::CalculateWidgetAutoHeight(float fHeightCalc) {
  CXFA_Margin* margin = GetMarginIfExists();
  if (margin)
    fHeightCalc += margin->GetTopInset() + margin->GetBottomInset();

  Optional<float> min = TryMinHeight();
  if (min)
    fHeightCalc = std::max(fHeightCalc, *min);

  Optional<float> max = TryMaxHeight();
  if (max && *max > 0)
    fHeightCalc = std::min(fHeightCalc, *max);

  return fHeightCalc;
}

float CXFA_Node::GetHeightWithoutMargin(float fHeightCalc) const {
  CXFA_Margin* margin = GetMarginIfExists();
  if (margin)
    fHeightCalc -= margin->GetTopInset() + margin->GetBottomInset();
  return fHeightCalc;
}

void CXFA_Node::StartWidgetLayout(CXFA_FFDoc* doc,
                                  float* pCalcWidth,
                                  float* pCalcHeight) {
  InitLayoutData();

  if (GetFFWidgetType() == XFA_FFWidgetType::kText) {
    m_pLayoutData->m_fWidgetHeight = TryHeight().value_or(-1);
    StartTextLayout(doc, pCalcWidth, pCalcHeight);
    return;
  }
  if (*pCalcWidth > 0 && *pCalcHeight > 0)
    return;

  m_pLayoutData->m_fWidgetHeight = -1;
  float fWidth = 0;
  if (*pCalcWidth > 0 && *pCalcHeight < 0) {
    Optional<float> height = TryHeight();
    if (height) {
      *pCalcHeight = *height;
    } else {
      CFX_SizeF size = CalculateAccWidthAndHeight(doc, *pCalcWidth);
      *pCalcWidth = size.width;
      *pCalcHeight = size.height;
    }

    m_pLayoutData->m_fWidgetHeight = *pCalcHeight;
    return;
  }
  if (*pCalcWidth < 0 && *pCalcHeight < 0) {
    Optional<float> height;
    Optional<float> width = TryWidth();
    if (width) {
      fWidth = *width;

      height = TryHeight();
      if (height)
        *pCalcHeight = *height;
    }
    if (!width || !height) {
      CFX_SizeF size = CalculateAccWidthAndHeight(doc, fWidth);
      *pCalcWidth = size.width;
      *pCalcHeight = size.height;
    } else {
      *pCalcWidth = fWidth;
    }
  }
  m_pLayoutData->m_fWidgetHeight = *pCalcHeight;
}

CFX_SizeF CXFA_Node::CalculateAccWidthAndHeight(CXFA_FFDoc* doc, float fWidth) {
  CFX_SizeF sz(fWidth, m_pLayoutData->m_fWidgetHeight);
  switch (GetFFWidgetType()) {
    case XFA_FFWidgetType::kBarcode:
    case XFA_FFWidgetType::kChoiceList:
    case XFA_FFWidgetType::kSignature:
      CalculateFieldAutoSize(doc, &sz);
      break;
    case XFA_FFWidgetType::kImageEdit:
      CalculateImageEditAutoSize(doc, &sz);
      break;
    case XFA_FFWidgetType::kButton:
      CalculatePushButtonAutoSize(doc, &sz);
      break;
    case XFA_FFWidgetType::kCheckButton:
      CalculateCheckButtonAutoSize(doc, &sz);
      break;
    case XFA_FFWidgetType::kDateTimeEdit:
    case XFA_FFWidgetType::kNumericEdit:
    case XFA_FFWidgetType::kPasswordEdit:
    case XFA_FFWidgetType::kTextEdit:
      CalculateTextEditAutoSize(doc, &sz);
      break;
    case XFA_FFWidgetType::kImage:
      CalculateImageAutoSize(doc, &sz);
      break;
    case XFA_FFWidgetType::kArc:
    case XFA_FFWidgetType::kLine:
    case XFA_FFWidgetType::kRectangle:
    case XFA_FFWidgetType::kSubform:
    case XFA_FFWidgetType::kExclGroup:
      CalculateWidgetAutoSize(&sz);
      break;
    case XFA_FFWidgetType::kText:
    case XFA_FFWidgetType::kNone:
      break;
  }

  m_pLayoutData->m_fWidgetHeight = sz.height;
  return sz;
}

bool CXFA_Node::FindSplitPos(CXFA_FFDocView* docView,
                             int32_t iBlockIndex,
                             float* pCalcHeight) {
  if (GetFFWidgetType() == XFA_FFWidgetType::kSubform)
    return false;

  switch (GetFFWidgetType()) {
    case XFA_FFWidgetType::kText:
    case XFA_FFWidgetType::kTextEdit:
    case XFA_FFWidgetType::kNumericEdit:
    case XFA_FFWidgetType::kPasswordEdit:
      break;
    default:
      *pCalcHeight = 0;
      return true;
  }

  float fTopInset = 0;
  float fBottomInset = 0;
  if (iBlockIndex == 0) {
    CXFA_Margin* margin = GetMarginIfExists();
    if (margin) {
      fTopInset = margin->GetTopInset();
      fBottomInset = margin->GetBottomInset();
    }

    CFX_RectF rtUIMargin = GetUIMargin();
    fTopInset += rtUIMargin.top;
    fBottomInset += rtUIMargin.width;
  }
  if (GetFFWidgetType() == XFA_FFWidgetType::kText) {
    float fHeight = *pCalcHeight;
    if (iBlockIndex == 0) {
      *pCalcHeight -= fTopInset;
      *pCalcHeight = std::max(*pCalcHeight, 0.0f);
    }
    CXFA_TextLayout* pTextLayout =
        m_pLayoutData->AsTextLayoutData()->GetTextLayout();
    *pCalcHeight =
        pTextLayout->DoLayout(iBlockIndex, *pCalcHeight, *pCalcHeight,
                              m_pLayoutData->m_fWidgetHeight - fTopInset);
    if (*pCalcHeight != 0) {
      if (iBlockIndex == 0)
        *pCalcHeight += fTopInset;
      if (fabs(fHeight - *pCalcHeight) < kXFAWidgetPrecision)
        return false;
    }
    return true;
  }

  XFA_AttributeValue iCapPlacement = XFA_AttributeValue::Unknown;
  float fCapReserve = 0;
  if (iBlockIndex == 0) {
    CXFA_Caption* caption = GetCaptionIfExists();
    if (caption && !caption->IsHidden()) {
      iCapPlacement = caption->GetPlacementType();
      fCapReserve = caption->GetReserve();
    }
    if (iCapPlacement == XFA_AttributeValue::Top &&
        *pCalcHeight < fCapReserve + fTopInset) {
      *pCalcHeight = 0;
      return true;
    }
    if (iCapPlacement == XFA_AttributeValue::Bottom &&
        m_pLayoutData->m_fWidgetHeight - fCapReserve - fBottomInset) {
      *pCalcHeight = 0;
      return true;
    }
    if (iCapPlacement != XFA_AttributeValue::Top)
      fCapReserve = 0;
  }
  CXFA_FieldLayoutData* pFieldData = m_pLayoutData->AsFieldLayoutData();
  int32_t iLinesCount = 0;
  float fHeight = m_pLayoutData->m_fWidgetHeight;
  if (GetValue(XFA_VALUEPICTURE_Display).IsEmpty()) {
    iLinesCount = 1;
  } else {
    if (!pFieldData->m_pTextOut) {
      CFX_SizeF size =
          CalculateAccWidthAndHeight(docView->GetDoc(), TryWidth().value_or(0));
      fHeight = size.height;
    }

    iLinesCount = pFieldData->m_pTextOut->GetTotalLines();
  }
  std::vector<float>* pFieldArray = &pFieldData->m_FieldSplitArray;
  int32_t iFieldSplitCount = pdfium::CollectionSize<int32_t>(*pFieldArray);
  if (iFieldSplitCount < (iBlockIndex * 3))
    return false;

  for (int32_t i = 0; i < iBlockIndex * 3; i += 3) {
    iLinesCount -= (int32_t)(*pFieldArray)[i + 1];
    fHeight -= (*pFieldArray)[i + 2];
  }
  if (iLinesCount == 0)
    return false;

  float fLineHeight = GetLineHeight();
  float fFontSize = GetFontSize();
  float fTextHeight = iLinesCount * fLineHeight - fLineHeight + fFontSize;
  float fSpaceAbove = 0;
  float fStartOffset = 0;
  if (fHeight > 0.1f && iBlockIndex == 0) {
    fStartOffset = fTopInset;
    fHeight -= (fTopInset + fBottomInset);
    CXFA_Para* para = GetParaIfExists();
    if (para) {
      fSpaceAbove = para->GetSpaceAbove();
      float fSpaceBelow = para->GetSpaceBelow();
      fHeight -= (fSpaceAbove + fSpaceBelow);
      switch (para->GetVerticalAlign()) {
        case XFA_AttributeValue::Top:
          fStartOffset += fSpaceAbove;
          break;
        case XFA_AttributeValue::Middle:
          fStartOffset += ((fHeight - fTextHeight) / 2 + fSpaceAbove);
          break;
        case XFA_AttributeValue::Bottom:
          fStartOffset += (fHeight - fTextHeight + fSpaceAbove);
          break;
        default:
          NOTREACHED();
          break;
      }
    }
    if (fStartOffset < 0.1f)
      fStartOffset = 0;
  }
  for (int32_t i = iBlockIndex - 1; iBlockIndex > 0 && i < iBlockIndex; i++) {
    fStartOffset = (*pFieldArray)[i * 3] - (*pFieldArray)[i * 3 + 2];
    if (fStartOffset < 0.1f)
      fStartOffset = 0;
  }
  if (iFieldSplitCount / 3 == (iBlockIndex + 1))
    (*pFieldArray)[0] = fStartOffset;
  else
    pFieldArray->push_back(fStartOffset);

  XFA_VERSION version = docView->GetDoc()->GetXFADoc()->GetCurVersionMode();
  bool bCanSplitNoContent = false;
  auto value = GetParent()->JSObject()->TryEnum(XFA_Attribute::Layout, true);
  XFA_AttributeValue eLayoutMode = value.value_or(XFA_AttributeValue::Position);
  if ((eLayoutMode == XFA_AttributeValue::Position ||
       eLayoutMode == XFA_AttributeValue::Tb ||
       eLayoutMode == XFA_AttributeValue::Row ||
       eLayoutMode == XFA_AttributeValue::Table) &&
      version > XFA_VERSION_208) {
    bCanSplitNoContent = true;
  }
  if ((eLayoutMode == XFA_AttributeValue::Tb ||
       eLayoutMode == XFA_AttributeValue::Row ||
       eLayoutMode == XFA_AttributeValue::Table) &&
      version <= XFA_VERSION_208) {
    if (fStartOffset < *pCalcHeight) {
      bCanSplitNoContent = true;
    } else {
      *pCalcHeight = 0;
      return true;
    }
  }
  if (bCanSplitNoContent) {
    if ((*pCalcHeight - fTopInset - fSpaceAbove < fLineHeight)) {
      *pCalcHeight = 0;
      return true;
    }
    if (fStartOffset + kXFAWidgetPrecision >= *pCalcHeight) {
      if (iFieldSplitCount / 3 == (iBlockIndex + 1)) {
        (*pFieldArray)[iBlockIndex * 3 + 1] = 0;
        (*pFieldArray)[iBlockIndex * 3 + 2] = *pCalcHeight;
      } else {
        pFieldArray->push_back(0);
        pFieldArray->push_back(*pCalcHeight);
      }
      return false;
    }
    if (*pCalcHeight - fStartOffset < fLineHeight) {
      *pCalcHeight = fStartOffset;
      if (iFieldSplitCount / 3 == (iBlockIndex + 1)) {
        (*pFieldArray)[iBlockIndex * 3 + 1] = 0;
        (*pFieldArray)[iBlockIndex * 3 + 2] = *pCalcHeight;
      } else {
        pFieldArray->push_back(0);
        pFieldArray->push_back(*pCalcHeight);
      }
      return true;
    }
    float fTextNum =
        *pCalcHeight + kXFAWidgetPrecision - fCapReserve - fStartOffset;
    int32_t iLineNum =
        (int32_t)((fTextNum + (fLineHeight - fFontSize)) / fLineHeight);
    if (iLineNum >= iLinesCount) {
      if (*pCalcHeight - fStartOffset - fTextHeight >= fFontSize) {
        if (iFieldSplitCount / 3 == (iBlockIndex + 1)) {
          (*pFieldArray)[iBlockIndex * 3 + 1] = iLinesCount;
          (*pFieldArray)[iBlockIndex * 3 + 2] = *pCalcHeight;
        } else {
          pFieldArray->push_back(iLinesCount);
          pFieldArray->push_back(*pCalcHeight);
        }
        return false;
      }
      if (fHeight - fStartOffset - fTextHeight < fFontSize) {
        iLineNum -= 1;
        if (iLineNum == 0) {
          *pCalcHeight = 0;
          return true;
        }
      } else {
        iLineNum = (int32_t)(fTextNum / fLineHeight);
      }
    }
    if (iLineNum > 0) {
      float fSplitHeight = iLineNum * fLineHeight + fCapReserve + fStartOffset;
      if (iFieldSplitCount / 3 == (iBlockIndex + 1)) {
        (*pFieldArray)[iBlockIndex * 3 + 1] = iLineNum;
        (*pFieldArray)[iBlockIndex * 3 + 2] = fSplitHeight;
      } else {
        pFieldArray->push_back(iLineNum);
        pFieldArray->push_back(fSplitHeight);
      }
      if (fabs(fSplitHeight - *pCalcHeight) < kXFAWidgetPrecision)
        return false;

      *pCalcHeight = fSplitHeight;
      return true;
    }
  }
  *pCalcHeight = 0;
  return true;
}

void CXFA_Node::InitLayoutData() {
  if (m_pLayoutData)
    return;

  switch (GetFFWidgetType()) {
    case XFA_FFWidgetType::kText:
      m_pLayoutData = pdfium::MakeUnique<CXFA_TextLayoutData>();
      return;
    case XFA_FFWidgetType::kTextEdit:
      m_pLayoutData = pdfium::MakeUnique<CXFA_TextEditData>();
      return;
    case XFA_FFWidgetType::kImage:
      m_pLayoutData = pdfium::MakeUnique<CXFA_ImageLayoutData>();
      return;
    case XFA_FFWidgetType::kImageEdit:
      m_pLayoutData = pdfium::MakeUnique<CXFA_ImageEditData>();
      return;
    default:
      break;
  }
  if (GetElementType() == XFA_Element::Field) {
    m_pLayoutData = pdfium::MakeUnique<CXFA_FieldLayoutData>();
    return;
  }
  m_pLayoutData = pdfium::MakeUnique<CXFA_WidgetLayoutData>();
}

void CXFA_Node::StartTextLayout(CXFA_FFDoc* doc,
                                float* pCalcWidth,
                                float* pCalcHeight) {
  InitLayoutData();

  CXFA_TextLayoutData* pTextLayoutData = m_pLayoutData->AsTextLayoutData();
  pTextLayoutData->LoadText(doc, this);

  CXFA_TextLayout* pTextLayout = pTextLayoutData->GetTextLayout();
  float fTextHeight = 0;
  if (*pCalcWidth > 0 && *pCalcHeight > 0) {
    float fWidth = GetWidthWithoutMargin(*pCalcWidth);
    pTextLayout->StartLayout(fWidth);
    fTextHeight = *pCalcHeight;
    fTextHeight = GetHeightWithoutMargin(fTextHeight);
    pTextLayout->DoLayout(0, fTextHeight, -1, fTextHeight);
    return;
  }
  if (*pCalcWidth > 0 && *pCalcHeight < 0) {
    float fWidth = GetWidthWithoutMargin(*pCalcWidth);
    pTextLayout->StartLayout(fWidth);
  }
  if (*pCalcWidth < 0 && *pCalcHeight < 0) {
    Optional<float> width = TryWidth();
    if (width) {
      pTextLayout->StartLayout(GetWidthWithoutMargin(*width));
      *pCalcWidth = *width;
    } else {
      float fMaxWidth = CalculateWidgetAutoWidth(pTextLayout->StartLayout(-1));
      pTextLayout->StartLayout(GetWidthWithoutMargin(fMaxWidth));
      *pCalcWidth = fMaxWidth;
    }
  }
  if (m_pLayoutData->m_fWidgetHeight < 0) {
    m_pLayoutData->m_fWidgetHeight = pTextLayout->GetLayoutHeight();
    m_pLayoutData->m_fWidgetHeight =
        CalculateWidgetAutoHeight(m_pLayoutData->m_fWidgetHeight);
  }
  fTextHeight = m_pLayoutData->m_fWidgetHeight;
  fTextHeight = GetHeightWithoutMargin(fTextHeight);
  pTextLayout->DoLayout(0, fTextHeight, -1, fTextHeight);
  *pCalcHeight = m_pLayoutData->m_fWidgetHeight;
}

bool CXFA_Node::LoadCaption(CXFA_FFDoc* doc) {
  InitLayoutData();
  return m_pLayoutData->AsFieldLayoutData()->LoadCaption(doc, this);
}

CXFA_TextLayout* CXFA_Node::GetCaptionTextLayout() {
  return m_pLayoutData
             ? m_pLayoutData->AsFieldLayoutData()->m_pCapTextLayout.get()
             : nullptr;
}

CXFA_TextLayout* CXFA_Node::GetTextLayout() {
  return m_pLayoutData ? m_pLayoutData->AsTextLayoutData()->GetTextLayout()
                       : nullptr;
}

RetainPtr<CFX_DIBitmap> CXFA_Node::GetImageImage() {
  return m_pLayoutData ? m_pLayoutData->AsImageLayoutData()->m_pDIBitmap
                       : nullptr;
}

RetainPtr<CFX_DIBitmap> CXFA_Node::GetImageEditImage() {
  return m_pLayoutData ? m_pLayoutData->AsFieldLayoutData()
                             ->AsImageEditData()
                             ->m_pDIBitmap
                       : nullptr;
}

void CXFA_Node::SetImageImage(const RetainPtr<CFX_DIBitmap>& newImage) {
  CXFA_ImageLayoutData* pData = m_pLayoutData->AsImageLayoutData();
  if (pData->m_pDIBitmap != newImage)
    pData->m_pDIBitmap = newImage;
}

void CXFA_Node::SetImageEditImage(const RetainPtr<CFX_DIBitmap>& newImage) {
  CXFA_ImageEditData* pData =
      m_pLayoutData->AsFieldLayoutData()->AsImageEditData();
  if (pData->m_pDIBitmap != newImage)
    pData->m_pDIBitmap = newImage;
}

RetainPtr<CFGAS_GEFont> CXFA_Node::GetFDEFont(CXFA_FFDoc* doc) {
  WideString wsFontName = L"Courier";
  uint32_t dwFontStyle = 0;
  CXFA_Font* font = GetFontIfExists();
  if (font) {
    if (font->IsBold())
      dwFontStyle |= FXFONT_BOLD;
    if (font->IsItalic())
      dwFontStyle |= FXFONT_ITALIC;

    wsFontName = font->GetTypeface();
  }
  return doc->GetApp()->GetXFAFontMgr()->GetFont(doc, wsFontName.AsStringView(),
                                                 dwFontStyle);
}

bool CXFA_Node::HasButtonRollover() {
  CXFA_Items* pItems = GetChild<CXFA_Items>(0, XFA_Element::Items, false);
  if (!pItems)
    return false;

  for (CXFA_Node* pText = pItems->GetFirstChild(); pText;
       pText = pText->GetNextSibling()) {
    if (pText->JSObject()
            ->GetCData(XFA_Attribute::Name)
            .EqualsASCII("rollover")) {
      return !pText->JSObject()->GetContent(false).IsEmpty();
    }
  }
  return false;
}

bool CXFA_Node::HasButtonDown() {
  CXFA_Items* pItems = GetChild<CXFA_Items>(0, XFA_Element::Items, false);
  if (!pItems)
    return false;

  for (CXFA_Node* pText = pItems->GetFirstChild(); pText;
       pText = pText->GetNextSibling()) {
    if (pText->JSObject()->GetCData(XFA_Attribute::Name).EqualsASCII("down")) {
      return !pText->JSObject()->GetContent(false).IsEmpty();
    }
  }
  return false;
}

bool CXFA_Node::IsRadioButton() {
  CXFA_Node* pParent = GetParent();
  return pParent && pParent->GetElementType() == XFA_Element::ExclGroup;
}

float CXFA_Node::GetCheckButtonSize() {
  CXFA_Node* pUIChild = GetUIChildNode();
  if (pUIChild) {
    return pUIChild->JSObject()
        ->GetMeasure(XFA_Attribute::Size)
        .ToUnit(XFA_Unit::Pt);
  }
  return CXFA_Measurement(10, XFA_Unit::Pt).ToUnit(XFA_Unit::Pt);
}

XFA_CHECKSTATE CXFA_Node::GetCheckState() {
  WideString wsValue = GetRawValue();
  if (wsValue.IsEmpty())
    return XFA_CHECKSTATE_Off;

  auto* pItems = GetChild<CXFA_Items>(0, XFA_Element::Items, false);
  if (!pItems)
    return XFA_CHECKSTATE_Off;

  CXFA_Node* pText = pItems->GetFirstChild();
  int32_t i = 0;
  while (pText) {
    Optional<WideString> wsContent = pText->JSObject()->TryContent(false, true);
    if (wsContent && *wsContent == wsValue)
      return static_cast<XFA_CHECKSTATE>(i);

    i++;
    pText = pText->GetNextSibling();
  }
  return XFA_CHECKSTATE_Off;
}

void CXFA_Node::SetCheckState(XFA_CHECKSTATE eCheckState, bool bNotify) {
  CXFA_Node* node = GetExclGroupIfExists();
  if (!node) {
    CXFA_Items* pItems = GetChild<CXFA_Items>(0, XFA_Element::Items, false);
    if (!pItems)
      return;

    int32_t i = -1;
    CXFA_Node* pText = pItems->GetFirstChild();
    WideString wsContent;
    while (pText) {
      i++;
      if (i == eCheckState) {
        wsContent = pText->JSObject()->GetContent(false);
        break;
      }
      pText = pText->GetNextSibling();
    }
    SyncValue(wsContent, bNotify);

    return;
  }

  WideString wsValue;
  if (eCheckState != XFA_CHECKSTATE_Off) {
    if (CXFA_Items* pItems =
            GetChild<CXFA_Items>(0, XFA_Element::Items, false)) {
      CXFA_Node* pText = pItems->GetFirstChild();
      if (pText)
        wsValue = pText->JSObject()->GetContent(false);
    }
  }
  CXFA_Node* pChild = node->GetFirstChild();
  for (; pChild; pChild = pChild->GetNextSibling()) {
    if (pChild->GetElementType() != XFA_Element::Field)
      continue;

    CXFA_Items* pItem =
        pChild->GetChild<CXFA_Items>(0, XFA_Element::Items, false);
    if (!pItem)
      continue;

    CXFA_Node* pItemchild = pItem->GetFirstChild();
    if (!pItemchild)
      continue;

    WideString text = pItemchild->JSObject()->GetContent(false);
    WideString wsChildValue = text;
    if (wsValue != text) {
      pItemchild = pItemchild->GetNextSibling();
      if (pItemchild)
        wsChildValue = pItemchild->JSObject()->GetContent(false);
      else
        wsChildValue.clear();
    }
    pChild->SyncValue(wsChildValue, bNotify);
  }
  node->SyncValue(wsValue, bNotify);
}

CXFA_Node* CXFA_Node::GetSelectedMember() {
  CXFA_Node* pSelectedMember = nullptr;
  WideString wsState = GetRawValue();
  if (wsState.IsEmpty())
    return pSelectedMember;

  for (CXFA_Node* pNode = ToNode(GetFirstChild()); pNode;
       pNode = pNode->GetNextSibling()) {
    if (pNode->GetCheckState() == XFA_CHECKSTATE_On) {
      pSelectedMember = pNode;
      break;
    }
  }
  return pSelectedMember;
}

CXFA_Node* CXFA_Node::SetSelectedMember(WideStringView wsName, bool bNotify) {
  uint32_t nameHash = FX_HashCode_GetW(wsName, false);
  for (CXFA_Node* pNode = ToNode(GetFirstChild()); pNode;
       pNode = pNode->GetNextSibling()) {
    if (pNode->GetNameHash() == nameHash) {
      pNode->SetCheckState(XFA_CHECKSTATE_On, bNotify);
      return pNode;
    }
  }
  return nullptr;
}

void CXFA_Node::SetSelectedMemberByValue(WideStringView wsValue,
                                         bool bNotify,
                                         bool bScriptModify,
                                         bool bSyncData) {
  WideString wsExclGroup;
  for (CXFA_Node* pNode = GetFirstChild(); pNode;
       pNode = pNode->GetNextSibling()) {
    if (pNode->GetElementType() != XFA_Element::Field)
      continue;

    CXFA_Items* pItem =
        pNode->GetChild<CXFA_Items>(0, XFA_Element::Items, false);
    if (!pItem)
      continue;

    CXFA_Node* pItemchild = pItem->GetFirstChild();
    if (!pItemchild)
      continue;

    WideString wsChildValue = pItemchild->JSObject()->GetContent(false);
    if (wsValue != wsChildValue) {
      pItemchild = pItemchild->GetNextSibling();
      if (pItemchild)
        wsChildValue = pItemchild->JSObject()->GetContent(false);
      else
        wsChildValue.clear();
    } else {
      wsExclGroup = wsValue;
    }
    pNode->JSObject()->SetContent(wsChildValue, wsChildValue, bNotify,
                                  bScriptModify, false);
  }
  JSObject()->SetContent(wsExclGroup, wsExclGroup, bNotify, bScriptModify,
                         bSyncData);
}

CXFA_Node* CXFA_Node::GetExclGroupFirstMember() {
  CXFA_Node* pNode = GetFirstChild();
  while (pNode) {
    if (pNode->GetElementType() == XFA_Element::Field)
      return pNode;

    pNode = pNode->GetNextSibling();
  }
  return nullptr;
}

CXFA_Node* CXFA_Node::GetExclGroupNextMember(CXFA_Node* pNode) {
  if (!pNode)
    return nullptr;

  CXFA_Node* pNodeField = pNode->GetNextSibling();
  while (pNodeField) {
    if (pNodeField->GetElementType() == XFA_Element::Field)
      return pNodeField;

    pNodeField = pNodeField->GetNextSibling();
  }
  return nullptr;
}

bool CXFA_Node::IsChoiceListCommitOnSelect() {
  CXFA_Node* pUIChild = GetUIChildNode();
  if (pUIChild) {
    return pUIChild->JSObject()->GetEnum(XFA_Attribute::CommitOn) ==
           XFA_AttributeValue::Select;
  }
  return true;
}

bool CXFA_Node::IsChoiceListAllowTextEntry() {
  CXFA_Node* pUIChild = GetUIChildNode();
  return pUIChild && pUIChild->JSObject()->GetBoolean(XFA_Attribute::TextEntry);
}

bool CXFA_Node::IsChoiceListMultiSelect() {
  CXFA_Node* pUIChild = GetUIChildNode();
  if (pUIChild) {
    return pUIChild->JSObject()->GetEnum(XFA_Attribute::Open) ==
           XFA_AttributeValue::MultiSelect;
  }
  return false;
}

bool CXFA_Node::IsListBox() {
  CXFA_Node* pUIChild = GetUIChildNode();
  if (!pUIChild)
    return false;

  XFA_AttributeValue attr = pUIChild->JSObject()->GetEnum(XFA_Attribute::Open);
  return attr == XFA_AttributeValue::Always ||
         attr == XFA_AttributeValue::MultiSelect;
}

int32_t CXFA_Node::CountChoiceListItems(bool bSaveValue) {
  std::vector<CXFA_Node*> pItems;
  int32_t iCount = 0;
  for (CXFA_Node* pNode = GetFirstChild(); pNode;
       pNode = pNode->GetNextSibling()) {
    if (pNode->GetElementType() != XFA_Element::Items)
      continue;
    iCount++;
    pItems.push_back(pNode);
    if (iCount == 2)
      break;
  }
  if (iCount == 0)
    return 0;

  CXFA_Node* pItem = pItems[0];
  if (iCount > 1) {
    bool bItemOneHasSave =
        pItems[0]->JSObject()->GetBoolean(XFA_Attribute::Save);
    bool bItemTwoHasSave =
        pItems[1]->JSObject()->GetBoolean(XFA_Attribute::Save);
    if (bItemOneHasSave != bItemTwoHasSave && bSaveValue == bItemTwoHasSave)
      pItem = pItems[1];
  }
  return pItem->CountChildren(XFA_Element::Unknown, false);
}

Optional<WideString> CXFA_Node::GetChoiceListItem(int32_t nIndex,
                                                  bool bSaveValue) {
  std::vector<CXFA_Node*> pItemsArray;
  int32_t iCount = 0;
  for (CXFA_Node* pNode = GetFirstChild(); pNode;
       pNode = pNode->GetNextSibling()) {
    if (pNode->GetElementType() != XFA_Element::Items)
      continue;

    ++iCount;
    pItemsArray.push_back(pNode);
    if (iCount == 2)
      break;
  }
  if (iCount == 0)
    return {};

  CXFA_Node* pItems = pItemsArray[0];
  if (iCount > 1) {
    bool bItemOneHasSave =
        pItemsArray[0]->JSObject()->GetBoolean(XFA_Attribute::Save);
    bool bItemTwoHasSave =
        pItemsArray[1]->JSObject()->GetBoolean(XFA_Attribute::Save);
    if (bItemOneHasSave != bItemTwoHasSave && bSaveValue == bItemTwoHasSave)
      pItems = pItemsArray[1];
  }
  if (!pItems)
    return {};

  CXFA_Node* pItem =
      pItems->GetChild<CXFA_Node>(nIndex, XFA_Element::Unknown, false);
  if (pItem)
    return {pItem->JSObject()->GetContent(false)};
  return {};
}

std::vector<WideString> CXFA_Node::GetChoiceListItems(bool bSaveValue) {
  std::vector<CXFA_Node*> items;
  for (CXFA_Node* pNode = GetFirstChild(); pNode && items.size() < 2;
       pNode = pNode->GetNextSibling()) {
    if (pNode->GetElementType() == XFA_Element::Items)
      items.push_back(pNode);
  }
  if (items.empty())
    return std::vector<WideString>();

  CXFA_Node* pItem = items.front();
  if (items.size() > 1) {
    bool bItemOneHasSave =
        items[0]->JSObject()->GetBoolean(XFA_Attribute::Save);
    bool bItemTwoHasSave =
        items[1]->JSObject()->GetBoolean(XFA_Attribute::Save);
    if (bItemOneHasSave != bItemTwoHasSave && bSaveValue == bItemTwoHasSave)
      pItem = items[1];
  }

  std::vector<WideString> wsTextArray;
  for (CXFA_Node* pNode = pItem->GetFirstChild(); pNode;
       pNode = pNode->GetNextSibling()) {
    wsTextArray.emplace_back(pNode->JSObject()->GetContent(false));
  }
  return wsTextArray;
}

int32_t CXFA_Node::CountSelectedItems() {
  std::vector<WideString> wsValueArray = GetSelectedItemsValue();
  if (IsListBox() || !IsChoiceListAllowTextEntry())
    return pdfium::CollectionSize<int32_t>(wsValueArray);

  int32_t iSelected = 0;
  std::vector<WideString> wsSaveTextArray = GetChoiceListItems(true);
  for (const auto& value : wsValueArray) {
    if (pdfium::ContainsValue(wsSaveTextArray, value))
      iSelected++;
  }
  return iSelected;
}

int32_t CXFA_Node::GetSelectedItem(int32_t nIndex) {
  std::vector<WideString> wsValueArray = GetSelectedItemsValue();
  if (!pdfium::IndexInBounds(wsValueArray, nIndex))
    return -1;

  std::vector<WideString> wsSaveTextArray = GetChoiceListItems(true);
  auto it = std::find(wsSaveTextArray.begin(), wsSaveTextArray.end(),
                      wsValueArray[nIndex]);
  return it != wsSaveTextArray.end() ? it - wsSaveTextArray.begin() : -1;
}

std::vector<int32_t> CXFA_Node::GetSelectedItems() {
  std::vector<int32_t> iSelArray;
  std::vector<WideString> wsValueArray = GetSelectedItemsValue();
  std::vector<WideString> wsSaveTextArray = GetChoiceListItems(true);
  for (const auto& value : wsValueArray) {
    auto it = std::find(wsSaveTextArray.begin(), wsSaveTextArray.end(), value);
    if (it != wsSaveTextArray.end())
      iSelArray.push_back(it - wsSaveTextArray.begin());
  }
  return iSelArray;
}

std::vector<WideString> CXFA_Node::GetSelectedItemsValue() {
  std::vector<WideString> wsSelTextArray;
  WideString wsValue = GetRawValue();
  if (IsChoiceListMultiSelect()) {
    if (!wsValue.IsEmpty()) {
      size_t iStart = 0;
      size_t iLength = wsValue.GetLength();
      auto iEnd = wsValue.Find(L'\n', iStart);
      iEnd = (!iEnd.has_value()) ? iLength : iEnd;
      while (iEnd >= iStart) {
        wsSelTextArray.push_back(wsValue.Mid(iStart, iEnd.value() - iStart));
        iStart = iEnd.value() + 1;
        if (iStart >= iLength)
          break;
        iEnd = wsValue.Find(L'\n', iStart);
        if (!iEnd.has_value())
          wsSelTextArray.push_back(wsValue.Mid(iStart, iLength - iStart));
      }
    }
  } else {
    wsSelTextArray.push_back(wsValue);
  }
  return wsSelTextArray;
}

bool CXFA_Node::GetItemState(int32_t nIndex) {
  std::vector<WideString> wsSaveTextArray = GetChoiceListItems(true);
  return pdfium::IndexInBounds(wsSaveTextArray, nIndex) &&
         pdfium::ContainsValue(GetSelectedItemsValue(),
                               wsSaveTextArray[nIndex]);
}

void CXFA_Node::SetItemState(int32_t nIndex,
                             bool bSelected,
                             bool bNotify,
                             bool bScriptModify,
                             bool bSyncData) {
  std::vector<WideString> wsSaveTextArray = GetChoiceListItems(true);
  if (!pdfium::IndexInBounds(wsSaveTextArray, nIndex))
    return;

  int32_t iSel = -1;
  std::vector<WideString> wsValueArray = GetSelectedItemsValue();
  auto value_iter = std::find(wsValueArray.begin(), wsValueArray.end(),
                              wsSaveTextArray[nIndex]);
  if (value_iter != wsValueArray.end())
    iSel = value_iter - wsValueArray.begin();

  if (IsChoiceListMultiSelect()) {
    if (bSelected) {
      if (iSel < 0) {
        WideString wsValue = GetRawValue();
        if (!wsValue.IsEmpty()) {
          wsValue += L"\n";
        }
        wsValue += wsSaveTextArray[nIndex];
        JSObject()->SetContent(wsValue, wsValue, bNotify, bScriptModify,
                               bSyncData);
      }
    } else if (iSel >= 0) {
      std::vector<int32_t> iSelArray = GetSelectedItems();
      auto selected_iter =
          std::find(iSelArray.begin(), iSelArray.end(), nIndex);
      if (selected_iter != iSelArray.end())
        iSelArray.erase(selected_iter);
      SetSelectedItems(iSelArray, bNotify, bScriptModify, bSyncData);
    }
  } else {
    if (bSelected) {
      if (iSel < 0) {
        WideString wsSaveText = wsSaveTextArray[nIndex];
        JSObject()->SetContent(wsSaveText, GetFormatDataValue(wsSaveText),
                               bNotify, bScriptModify, bSyncData);
      }
    } else if (iSel >= 0) {
      JSObject()->SetContent(WideString(), WideString(), bNotify, bScriptModify,
                             bSyncData);
    }
  }
}

void CXFA_Node::SetSelectedItems(const std::vector<int32_t>& iSelArray,
                                 bool bNotify,
                                 bool bScriptModify,
                                 bool bSyncData) {
  WideString wsValue;
  int32_t iSize = pdfium::CollectionSize<int32_t>(iSelArray);
  if (iSize >= 1) {
    std::vector<WideString> wsSaveTextArray = GetChoiceListItems(true);
    WideString wsItemValue;
    for (int32_t i = 0; i < iSize; i++) {
      wsItemValue = (iSize == 1) ? wsSaveTextArray[iSelArray[i]]
                                 : wsSaveTextArray[iSelArray[i]] + L"\n";
      wsValue += wsItemValue;
    }
  }
  WideString wsFormat(wsValue);
  if (!IsChoiceListMultiSelect())
    wsFormat = GetFormatDataValue(wsValue);

  JSObject()->SetContent(wsValue, wsFormat, bNotify, bScriptModify, bSyncData);
}

void CXFA_Node::ClearAllSelections() {
  CXFA_Node* pBind = GetBindData();
  if (!pBind || !IsChoiceListMultiSelect()) {
    SyncValue(WideString(), false);
    return;
  }

  while (CXFA_Node* pChildNode = pBind->GetFirstChild())
    pBind->RemoveChild(pChildNode, true);
}

void CXFA_Node::InsertItem(const WideString& wsLabel,
                           const WideString& wsValue,
                           bool bNotify) {
  int32_t nIndex = -1;
  WideString wsNewValue(wsValue);
  if (wsNewValue.IsEmpty())
    wsNewValue = wsLabel;

  std::vector<CXFA_Node*> listitems;
  for (CXFA_Node* pItem = GetFirstChild(); pItem;
       pItem = pItem->GetNextSibling()) {
    if (pItem->GetElementType() == XFA_Element::Items)
      listitems.push_back(pItem);
  }
  if (listitems.empty()) {
    CXFA_Node* pItems = CreateSamePacketNode(XFA_Element::Items);
    InsertChild(-1, pItems);
    InsertListTextItem(pItems, wsLabel, nIndex);
    CXFA_Node* pSaveItems = CreateSamePacketNode(XFA_Element::Items);
    InsertChild(-1, pSaveItems);
    pSaveItems->JSObject()->SetBoolean(XFA_Attribute::Save, true, false);
    InsertListTextItem(pSaveItems, wsNewValue, nIndex);
  } else if (listitems.size() > 1) {
    for (int32_t i = 0; i < 2; i++) {
      CXFA_Node* pNode = listitems[i];
      bool bHasSave = pNode->JSObject()->GetBoolean(XFA_Attribute::Save);
      if (bHasSave)
        InsertListTextItem(pNode, wsNewValue, nIndex);
      else
        InsertListTextItem(pNode, wsLabel, nIndex);
    }
  } else {
    CXFA_Node* pNode = listitems[0];
    pNode->JSObject()->SetBoolean(XFA_Attribute::Save, false, false);
    pNode->JSObject()->SetEnum(XFA_Attribute::Presence,
                               XFA_AttributeValue::Visible, false);
    CXFA_Node* pSaveItems = CreateSamePacketNode(XFA_Element::Items);
    InsertChild(-1, pSaveItems);
    pSaveItems->JSObject()->SetBoolean(XFA_Attribute::Save, true, false);
    pSaveItems->JSObject()->SetEnum(XFA_Attribute::Presence,
                                    XFA_AttributeValue::Hidden, false);
    CXFA_Node* pListNode = pNode->GetFirstChild();
    int32_t i = 0;
    while (pListNode) {
      InsertListTextItem(pSaveItems, pListNode->JSObject()->GetContent(false),
                         i);
      ++i;

      pListNode = pListNode->GetNextSibling();
    }
    InsertListTextItem(pNode, wsLabel, nIndex);
    InsertListTextItem(pSaveItems, wsNewValue, nIndex);
  }
  if (!bNotify)
    return;

  GetDocument()->GetNotify()->OnWidgetListItemAdded(this, wsLabel.c_str(),
                                                    wsValue.c_str(), nIndex);
}

WideString CXFA_Node::GetItemLabel(WideStringView wsValue) const {
  std::vector<CXFA_Node*> listitems;
  CXFA_Node* pItems = GetFirstChild();
  for (; pItems; pItems = pItems->GetNextSibling()) {
    if (pItems->GetElementType() != XFA_Element::Items)
      continue;
    listitems.push_back(pItems);
  }

  if (listitems.size() <= 1)
    return WideString(wsValue);

  CXFA_Node* pLabelItems = listitems[0];
  bool bSave = pLabelItems->JSObject()->GetBoolean(XFA_Attribute::Save);
  CXFA_Node* pSaveItems = nullptr;
  if (bSave) {
    pSaveItems = pLabelItems;
    pLabelItems = listitems[1];
  } else {
    pSaveItems = listitems[1];
  }

  int32_t iCount = 0;
  int32_t iSearch = -1;
  for (CXFA_Node* pChildItem = pSaveItems->GetFirstChild(); pChildItem;
       pChildItem = pChildItem->GetNextSibling()) {
    if (pChildItem->JSObject()->GetContent(false) == wsValue) {
      iSearch = iCount;
      break;
    }
    iCount++;
  }
  if (iSearch < 0)
    return WideString();

  CXFA_Node* pText =
      pLabelItems->GetChild<CXFA_Node>(iSearch, XFA_Element::Unknown, false);
  return pText ? pText->JSObject()->GetContent(false) : WideString();
}

WideString CXFA_Node::GetItemValue(WideStringView wsLabel) {
  int32_t iCount = 0;
  std::vector<CXFA_Node*> listitems;
  for (CXFA_Node* pItems = GetFirstChild(); pItems;
       pItems = pItems->GetNextSibling()) {
    if (pItems->GetElementType() != XFA_Element::Items)
      continue;
    iCount++;
    listitems.push_back(pItems);
  }
  if (iCount <= 1)
    return WideString(wsLabel);

  CXFA_Node* pLabelItems = listitems[0];
  bool bSave = pLabelItems->JSObject()->GetBoolean(XFA_Attribute::Save);
  CXFA_Node* pSaveItems = nullptr;
  if (bSave) {
    pSaveItems = pLabelItems;
    pLabelItems = listitems[1];
  } else {
    pSaveItems = listitems[1];
  }
  iCount = 0;

  int32_t iSearch = -1;
  WideString wsContent;
  CXFA_Node* pChildItem = pLabelItems->GetFirstChild();
  for (; pChildItem; pChildItem = pChildItem->GetNextSibling()) {
    if (pChildItem->JSObject()->GetContent(false) == wsLabel) {
      iSearch = iCount;
      break;
    }
    iCount++;
  }
  if (iSearch < 0)
    return WideString();

  CXFA_Node* pText =
      pSaveItems->GetChild<CXFA_Node>(iSearch, XFA_Element::Unknown, false);
  return pText ? pText->JSObject()->GetContent(false) : WideString();
}

bool CXFA_Node::DeleteItem(int32_t nIndex, bool bNotify, bool bScriptModify) {
  bool bSetValue = false;
  CXFA_Node* pItems = GetFirstChild();
  for (; pItems; pItems = pItems->GetNextSibling()) {
    if (pItems->GetElementType() != XFA_Element::Items)
      continue;

    if (nIndex < 0) {
      while (CXFA_Node* pNode = pItems->GetFirstChild()) {
        pItems->RemoveChild(pNode, true);
      }
    } else {
      if (!bSetValue && pItems->JSObject()->GetBoolean(XFA_Attribute::Save)) {
        SetItemState(nIndex, false, true, bScriptModify, true);
        bSetValue = true;
      }
      int32_t i = 0;
      CXFA_Node* pNode = pItems->GetFirstChild();
      while (pNode) {
        if (i == nIndex) {
          pItems->RemoveChild(pNode, true);
          break;
        }
        i++;
        pNode = pNode->GetNextSibling();
      }
    }
  }
  if (bNotify)
    GetDocument()->GetNotify()->OnWidgetListItemRemoved(this, nIndex);
  return true;
}

bool CXFA_Node::IsHorizontalScrollPolicyOff() {
  CXFA_Node* pUIChild = GetUIChildNode();
  if (pUIChild) {
    return pUIChild->JSObject()->GetEnum(XFA_Attribute::HScrollPolicy) ==
           XFA_AttributeValue::Off;
  }
  return false;
}

bool CXFA_Node::IsVerticalScrollPolicyOff() {
  CXFA_Node* pUIChild = GetUIChildNode();
  if (pUIChild) {
    return pUIChild->JSObject()->GetEnum(XFA_Attribute::VScrollPolicy) ==
           XFA_AttributeValue::Off;
  }
  return false;
}

Optional<int32_t> CXFA_Node::GetNumberOfCells() {
  CXFA_Node* pUIChild = GetUIChildNode();
  if (!pUIChild)
    return {};
  if (CXFA_Comb* pNode =
          pUIChild->GetChild<CXFA_Comb>(0, XFA_Element::Comb, false))
    return {pNode->JSObject()->GetInteger(XFA_Attribute::NumberOfCells)};
  return {};
}

bool CXFA_Node::IsMultiLine() {
  CXFA_Node* pUIChild = GetUIChildNode();
  return pUIChild && pUIChild->JSObject()->GetBoolean(XFA_Attribute::MultiLine);
}

std::pair<XFA_Element, int32_t> CXFA_Node::GetMaxChars() {
  if (CXFA_Value* pNode = GetChild<CXFA_Value>(0, XFA_Element::Value, false)) {
    if (CXFA_Node* pChild = pNode->GetFirstChild()) {
      switch (pChild->GetElementType()) {
        case XFA_Element::Text:
          return {XFA_Element::Text,
                  pChild->JSObject()->GetInteger(XFA_Attribute::MaxChars)};
        case XFA_Element::ExData: {
          int32_t iMax =
              pChild->JSObject()->GetInteger(XFA_Attribute::MaxLength);
          return {XFA_Element::ExData, iMax < 0 ? 0 : iMax};
        }
        default:
          break;
      }
    }
  }
  return {XFA_Element::Unknown, 0};
}

int32_t CXFA_Node::GetFracDigits() {
  CXFA_Value* pNode = GetChild<CXFA_Value>(0, XFA_Element::Value, false);
  if (!pNode)
    return -1;

  CXFA_Decimal* pChild =
      pNode->GetChild<CXFA_Decimal>(0, XFA_Element::Decimal, false);
  if (!pChild)
    return -1;

  return pChild->JSObject()
      ->TryInteger(XFA_Attribute::FracDigits, true)
      .value_or(-1);
}

int32_t CXFA_Node::GetLeadDigits() {
  CXFA_Value* pNode = GetChild<CXFA_Value>(0, XFA_Element::Value, false);
  if (!pNode)
    return -1;

  CXFA_Decimal* pChild =
      pNode->GetChild<CXFA_Decimal>(0, XFA_Element::Decimal, false);
  if (!pChild)
    return -1;

  return pChild->JSObject()
      ->TryInteger(XFA_Attribute::LeadDigits, true)
      .value_or(-1);
}

bool CXFA_Node::SetValue(XFA_VALUEPICTURE eValueType,
                         const WideString& wsValue) {
  if (wsValue.IsEmpty()) {
    SyncValue(wsValue, true);
    return true;
  }

  SetPreNull(IsNull());
  SetIsNull(false);

  WideString wsNewText(wsValue);
  WideString wsPicture = GetPictureContent(eValueType);
  bool bValidate = true;
  bool bSyncData = false;
  CXFA_Node* pNode = GetUIChildNode();
  if (!pNode)
    return true;

  XFA_Element eType = pNode->GetElementType();
  if (!wsPicture.IsEmpty()) {
    CXFA_LocaleMgr* pLocaleMgr = GetDocument()->GetLocaleMgr();
    LocaleIface* pLocale = GetLocale();
    CXFA_LocaleValue widgetValue = XFA_GetLocaleValue(this);
    bValidate =
        widgetValue.ValidateValue(wsValue, wsPicture, pLocale, &wsPicture);
    if (bValidate) {
      widgetValue = CXFA_LocaleValue(widgetValue.GetType(), wsNewText,
                                     wsPicture, pLocale, pLocaleMgr);
      wsNewText = widgetValue.GetValue();
      if (eType == XFA_Element::NumericEdit)
        wsNewText = NumericLimit(wsNewText);

      bSyncData = true;
    }
  } else if (eType == XFA_Element::NumericEdit) {
    if (!wsNewText.EqualsASCII("0"))
      wsNewText = NumericLimit(wsNewText);

    bSyncData = true;
  }
  if (eType != XFA_Element::NumericEdit || bSyncData)
    SyncValue(wsNewText, true);

  return bValidate;
}

WideString CXFA_Node::GetPictureContent(XFA_VALUEPICTURE ePicture) {
  if (ePicture == XFA_VALUEPICTURE_Raw)
    return WideString();

  CXFA_LocaleValue widgetValue = XFA_GetLocaleValue(this);
  switch (ePicture) {
    case XFA_VALUEPICTURE_Display: {
      if (CXFA_Format* pFormat =
              GetChild<CXFA_Format>(0, XFA_Element::Format, false)) {
        if (CXFA_Picture* pPicture = pFormat->GetChild<CXFA_Picture>(
                0, XFA_Element::Picture, false)) {
          Optional<WideString> picture =
              pPicture->JSObject()->TryContent(false, true);
          if (picture)
            return *picture;
        }
      }

      LocaleIface* pLocale = GetLocale();
      if (!pLocale)
        return WideString();

      uint32_t dwType = widgetValue.GetType();
      switch (dwType) {
        case XFA_VT_DATE:
          return pLocale->GetDatePattern(FX_LOCALEDATETIMESUBCATEGORY_Medium);
        case XFA_VT_TIME:
          return pLocale->GetTimePattern(FX_LOCALEDATETIMESUBCATEGORY_Medium);
        case XFA_VT_DATETIME:
          return pLocale->GetDatePattern(FX_LOCALEDATETIMESUBCATEGORY_Medium) +
                 L"T" +
                 pLocale->GetTimePattern(FX_LOCALEDATETIMESUBCATEGORY_Medium);
        case XFA_VT_DECIMAL:
        case XFA_VT_FLOAT:
        default:
          return WideString();
      }
    }
    case XFA_VALUEPICTURE_Edit: {
      CXFA_Ui* pUI = GetChild<CXFA_Ui>(0, XFA_Element::Ui, false);
      if (pUI) {
        if (CXFA_Picture* pPicture =
                pUI->GetChild<CXFA_Picture>(0, XFA_Element::Picture, false)) {
          Optional<WideString> picture =
              pPicture->JSObject()->TryContent(false, true);
          if (picture)
            return *picture;
        }
      }

      LocaleIface* pLocale = GetLocale();
      if (!pLocale)
        return WideString();

      uint32_t dwType = widgetValue.GetType();
      switch (dwType) {
        case XFA_VT_DATE:
          return pLocale->GetDatePattern(FX_LOCALEDATETIMESUBCATEGORY_Short);
        case XFA_VT_TIME:
          return pLocale->GetTimePattern(FX_LOCALEDATETIMESUBCATEGORY_Short);
        case XFA_VT_DATETIME:
          return pLocale->GetDatePattern(FX_LOCALEDATETIMESUBCATEGORY_Short) +
                 L"T" +
                 pLocale->GetTimePattern(FX_LOCALEDATETIMESUBCATEGORY_Short);
        default:
          return WideString();
      }
    }
    case XFA_VALUEPICTURE_DataBind: {
      CXFA_Bind* bind = GetBindIfExists();
      if (bind)
        return bind->GetPicture();
      break;
    }
    default:
      break;
  }
  return WideString();
}

WideString CXFA_Node::GetValue(XFA_VALUEPICTURE eValueType) {
  WideString wsValue = JSObject()->GetContent(false);

  if (eValueType == XFA_VALUEPICTURE_Display)
    wsValue = GetItemLabel(wsValue.AsStringView());

  WideString wsPicture = GetPictureContent(eValueType);
  CXFA_Node* pNode = GetUIChildNode();
  if (!pNode)
    return wsValue;

  switch (pNode->GetElementType()) {
    case XFA_Element::ChoiceList: {
      if (eValueType == XFA_VALUEPICTURE_Display) {
        int32_t iSelItemIndex = GetSelectedItem(0);
        if (iSelItemIndex >= 0) {
          wsValue =
              GetChoiceListItem(iSelItemIndex, false).value_or(WideString());
          wsPicture.clear();
        }
      }
      break;
    }
    case XFA_Element::NumericEdit:
      if (eValueType != XFA_VALUEPICTURE_Raw && wsPicture.IsEmpty()) {
        LocaleIface* pLocale = GetLocale();
        if (eValueType == XFA_VALUEPICTURE_Display && pLocale)
          wsValue = FormatNumStr(NormalizeNumStr(wsValue), pLocale);
      }
      break;
    default:
      break;
  }
  if (wsPicture.IsEmpty())
    return wsValue;

  if (LocaleIface* pLocale = GetLocale()) {
    CXFA_LocaleValue widgetValue = XFA_GetLocaleValue(this);
    CXFA_LocaleMgr* pLocaleMgr = GetDocument()->GetLocaleMgr();
    switch (widgetValue.GetType()) {
      case XFA_VT_DATE: {
        WideString wsDate, wsTime;
        if (SplitDateTime(wsValue, wsDate, wsTime)) {
          CXFA_LocaleValue date(XFA_VT_DATE, wsDate, pLocaleMgr);
          if (date.FormatPatterns(wsValue, wsPicture, pLocale, eValueType))
            return wsValue;
        }
        break;
      }
      case XFA_VT_TIME: {
        WideString wsDate, wsTime;
        if (SplitDateTime(wsValue, wsDate, wsTime)) {
          CXFA_LocaleValue time(XFA_VT_TIME, wsTime, pLocaleMgr);
          if (time.FormatPatterns(wsValue, wsPicture, pLocale, eValueType))
            return wsValue;
        }
        break;
      }
      default:
        break;
    }
    widgetValue.FormatPatterns(wsValue, wsPicture, pLocale, eValueType);
  }
  return wsValue;
}

WideString CXFA_Node::GetNormalizeDataValue(const WideString& wsValue) {
  if (wsValue.IsEmpty())
    return WideString();

  WideString wsPicture = GetPictureContent(XFA_VALUEPICTURE_DataBind);
  if (wsPicture.IsEmpty())
    return wsValue;

  CXFA_LocaleMgr* pLocaleMgr = GetDocument()->GetLocaleMgr();
  LocaleIface* pLocale = GetLocale();
  CXFA_LocaleValue widgetValue = XFA_GetLocaleValue(this);
  if (widgetValue.ValidateValue(wsValue, wsPicture, pLocale, &wsPicture)) {
    widgetValue = CXFA_LocaleValue(widgetValue.GetType(), wsValue, wsPicture,
                                   pLocale, pLocaleMgr);
    return widgetValue.GetValue();
  }
  return wsValue;
}

WideString CXFA_Node::GetFormatDataValue(const WideString& wsValue) {
  if (wsValue.IsEmpty())
    return WideString();

  WideString wsPicture = GetPictureContent(XFA_VALUEPICTURE_DataBind);
  if (wsPicture.IsEmpty())
    return wsValue;

  WideString wsFormattedValue = wsValue;
  if (LocaleIface* pLocale = GetLocale()) {
    CXFA_Value* pNodeValue = GetChild<CXFA_Value>(0, XFA_Element::Value, false);
    if (!pNodeValue)
      return wsValue;

    CXFA_Node* pValueChild = pNodeValue->GetFirstChild();
    if (!pValueChild)
      return wsValue;

    int32_t iVTType = XFA_VT_NULL;
    switch (pValueChild->GetElementType()) {
      case XFA_Element::Decimal:
        iVTType = XFA_VT_DECIMAL;
        break;
      case XFA_Element::Float:
        iVTType = XFA_VT_FLOAT;
        break;
      case XFA_Element::Date:
        iVTType = XFA_VT_DATE;
        break;
      case XFA_Element::Time:
        iVTType = XFA_VT_TIME;
        break;
      case XFA_Element::DateTime:
        iVTType = XFA_VT_DATETIME;
        break;
      case XFA_Element::Boolean:
        iVTType = XFA_VT_BOOLEAN;
        break;
      case XFA_Element::Integer:
        iVTType = XFA_VT_INTEGER;
        break;
      case XFA_Element::Text:
        iVTType = XFA_VT_TEXT;
        break;
      default:
        iVTType = XFA_VT_NULL;
        break;
    }
    CXFA_LocaleMgr* pLocaleMgr = GetDocument()->GetLocaleMgr();
    CXFA_LocaleValue widgetValue(iVTType, wsValue, pLocaleMgr);
    switch (widgetValue.GetType()) {
      case XFA_VT_DATE: {
        WideString wsDate, wsTime;
        if (SplitDateTime(wsValue, wsDate, wsTime)) {
          CXFA_LocaleValue date(XFA_VT_DATE, wsDate, pLocaleMgr);
          if (date.FormatPatterns(wsFormattedValue, wsPicture, pLocale,
                                  XFA_VALUEPICTURE_DataBind)) {
            return wsFormattedValue;
          }
        }
        break;
      }
      case XFA_VT_TIME: {
        WideString wsDate, wsTime;
        if (SplitDateTime(wsValue, wsDate, wsTime)) {
          CXFA_LocaleValue time(XFA_VT_TIME, wsTime, pLocaleMgr);
          if (time.FormatPatterns(wsFormattedValue, wsPicture, pLocale,
                                  XFA_VALUEPICTURE_DataBind)) {
            return wsFormattedValue;
          }
        }
        break;
      }
      default:
        break;
    }
    widgetValue.FormatPatterns(wsFormattedValue, wsPicture, pLocale,
                               XFA_VALUEPICTURE_DataBind);
  }
  return wsFormattedValue;
}

WideString CXFA_Node::NormalizeNumStr(const WideString& wsValue) {
  if (wsValue.IsEmpty())
    return WideString();

  WideString wsOutput = wsValue;
  wsOutput.TrimLeft('0');

  if (!wsOutput.IsEmpty() && wsOutput.Contains('.') && GetFracDigits() != -1) {
    wsOutput.TrimRight(L"0");
    wsOutput.TrimRight(L".");
  }
  if (wsOutput.IsEmpty() || wsOutput[0] == '.')
    wsOutput.InsertAtFront('0');

  return wsOutput;
}

void CXFA_Node::InsertListTextItem(CXFA_Node* pItems,
                                   const WideString& wsText,
                                   int32_t nIndex) {
  CXFA_Node* pText = pItems->CreateSamePacketNode(XFA_Element::Text);
  pItems->InsertChild(nIndex, pText);
  pText->JSObject()->SetContent(wsText, wsText, false, false, false);
}

WideString CXFA_Node::NumericLimit(const WideString& wsValue) {
  int32_t iLead = GetLeadDigits();
  int32_t iTread = GetFracDigits();

  if ((iLead == -1) && (iTread == -1))
    return wsValue;

  WideString wsRet;
  int32_t iLead_ = 0, iTread_ = -1;
  int32_t iCount = wsValue.GetLength();
  if (iCount == 0)
    return wsValue;

  int32_t i = 0;
  if (wsValue[i] == L'-') {
    wsRet += L'-';
    i++;
  }
  for (; i < iCount; i++) {
    wchar_t wc = wsValue[i];
    if (FXSYS_IsDecimalDigit(wc)) {
      if (iLead >= 0) {
        iLead_++;
        if (iLead_ > iLead)
          return L"0";
      } else if (iTread_ >= 0) {
        iTread_++;
        if (iTread_ > iTread) {
          if (iTread != -1) {
            CFGAS_Decimal wsDeci = CFGAS_Decimal(wsValue.AsStringView());
            wsDeci.SetScale(iTread);
            wsRet = wsDeci;
          }
          return wsRet;
        }
      }
    } else if (wc == L'.') {
      iTread_ = 0;
      iLead = -1;
    }
    wsRet += wc;
  }
  return wsRet;
}

bool CXFA_Node::PresenceRequiresSpace() const {
  auto value = JSObject()->TryEnum(XFA_Attribute::Presence, true);
  XFA_AttributeValue ePresence = value.value_or(XFA_AttributeValue::Visible);
  return ePresence == XFA_AttributeValue::Visible ||
         ePresence == XFA_AttributeValue::Invisible;
}

void CXFA_Node::SetToXML(const WideString& value) {
  auto* pNode = GetXMLMappingNode();
  switch (pNode->GetType()) {
    case FX_XMLNODE_Element: {
      auto* elem = static_cast<CFX_XMLElement*>(pNode);
      if (IsAttributeInXML()) {
        elem->SetAttribute(JSObject()->GetCData(XFA_Attribute::QualifiedName),
                           value);
        return;
      }

      bool bDeleteChildren = true;
      if (GetPacketType() == XFA_PacketType::Datasets) {
        for (CXFA_Node* pChildDataNode = GetFirstChild(); pChildDataNode;
             pChildDataNode = pChildDataNode->GetNextSibling()) {
          if (!pChildDataNode->GetBindItems()->empty()) {
            bDeleteChildren = false;
            break;
          }
        }
      }
      if (bDeleteChildren)
        elem->DeleteChildren();

      auto* text = GetDocument()
                       ->GetNotify()
                       ->GetHDOC()
                       ->GetXMLDocument()
                       ->CreateNode<CFX_XMLText>(value);
      elem->AppendChild(text);
      break;
    }
    case FX_XMLNODE_Text:
      ToXMLText(GetXMLMappingNode())->SetText(value);
      break;
    default:
      NOTREACHED();
  }
}

// static
std::unique_ptr<CXFA_Node> CXFA_Node::Create(CXFA_Document* doc,
                                             XFA_Element element,
                                             XFA_PacketType packet) {
  std::unique_ptr<CXFA_Node> node;
  switch (element) {
    case XFA_Element::Ps:
      node = pdfium::MakeUnique<CXFA_Ps>(doc, packet);
      break;
    case XFA_Element::To:
      node = pdfium::MakeUnique<CXFA_To>(doc, packet);
      break;
    case XFA_Element::Ui:
      node = pdfium::MakeUnique<CXFA_Ui>(doc, packet);
      break;
    case XFA_Element::RecordSet:
      node = pdfium::MakeUnique<CXFA_RecordSet>(doc, packet);
      break;
    case XFA_Element::SubsetBelow:
      node = pdfium::MakeUnique<CXFA_SubsetBelow>(doc, packet);
      break;
    case XFA_Element::SubformSet:
      node = pdfium::MakeUnique<CXFA_SubformSet>(doc, packet);
      break;
    case XFA_Element::AdobeExtensionLevel:
      node = pdfium::MakeUnique<CXFA_AdobeExtensionLevel>(doc, packet);
      break;
    case XFA_Element::Typeface:
      node = pdfium::MakeUnique<CXFA_Typeface>(doc, packet);
      break;
    case XFA_Element::Break:
      node = pdfium::MakeUnique<CXFA_Break>(doc, packet);
      break;
    case XFA_Element::FontInfo:
      node = pdfium::MakeUnique<CXFA_FontInfo>(doc, packet);
      break;
    case XFA_Element::NumberPattern:
      node = pdfium::MakeUnique<CXFA_NumberPattern>(doc, packet);
      break;
    case XFA_Element::DynamicRender:
      node = pdfium::MakeUnique<CXFA_DynamicRender>(doc, packet);
      break;
    case XFA_Element::PrintScaling:
      node = pdfium::MakeUnique<CXFA_PrintScaling>(doc, packet);
      break;
    case XFA_Element::CheckButton:
      node = pdfium::MakeUnique<CXFA_CheckButton>(doc, packet);
      break;
    case XFA_Element::DatePatterns:
      node = pdfium::MakeUnique<CXFA_DatePatterns>(doc, packet);
      break;
    case XFA_Element::SourceSet:
      node = pdfium::MakeUnique<CXFA_SourceSet>(doc, packet);
      break;
    case XFA_Element::Amd:
      node = pdfium::MakeUnique<CXFA_Amd>(doc, packet);
      break;
    case XFA_Element::Arc:
      node = pdfium::MakeUnique<CXFA_Arc>(doc, packet);
      break;
    case XFA_Element::Day:
      node = pdfium::MakeUnique<CXFA_Day>(doc, packet);
      break;
    case XFA_Element::Era:
      node = pdfium::MakeUnique<CXFA_Era>(doc, packet);
      break;
    case XFA_Element::Jog:
      node = pdfium::MakeUnique<CXFA_Jog>(doc, packet);
      break;
    case XFA_Element::Log:
      node = pdfium::MakeUnique<CXFA_Log>(doc, packet);
      break;
    case XFA_Element::Map:
      node = pdfium::MakeUnique<CXFA_Map>(doc, packet);
      break;
    case XFA_Element::Mdp:
      node = pdfium::MakeUnique<CXFA_Mdp>(doc, packet);
      break;
    case XFA_Element::BreakBefore:
      node = pdfium::MakeUnique<CXFA_BreakBefore>(doc, packet);
      break;
    case XFA_Element::Oid:
      node = pdfium::MakeUnique<CXFA_Oid>(doc, packet);
      break;
    case XFA_Element::Pcl:
      node = pdfium::MakeUnique<CXFA_Pcl>(doc, packet);
      break;
    case XFA_Element::Pdf:
      node = pdfium::MakeUnique<CXFA_Pdf>(doc, packet);
      break;
    case XFA_Element::Ref:
      node = pdfium::MakeUnique<CXFA_Ref>(doc, packet);
      break;
    case XFA_Element::Uri:
      node = pdfium::MakeUnique<CXFA_Uri>(doc, packet);
      break;
    case XFA_Element::Xdc:
      node = pdfium::MakeUnique<CXFA_Xdc>(doc, packet);
      break;
    case XFA_Element::Xdp:
      node = pdfium::MakeUnique<CXFA_Xdp>(doc, packet);
      break;
    case XFA_Element::Xfa:
      node = pdfium::MakeUnique<CXFA_Xfa>(doc, packet);
      break;
    case XFA_Element::Xsl:
      node = pdfium::MakeUnique<CXFA_Xsl>(doc, packet);
      break;
    case XFA_Element::Zpl:
      node = pdfium::MakeUnique<CXFA_Zpl>(doc, packet);
      break;
    case XFA_Element::Cache:
      node = pdfium::MakeUnique<CXFA_Cache>(doc, packet);
      break;
    case XFA_Element::Margin:
      node = pdfium::MakeUnique<CXFA_Margin>(doc, packet);
      break;
    case XFA_Element::KeyUsage:
      node = pdfium::MakeUnique<CXFA_KeyUsage>(doc, packet);
      break;
    case XFA_Element::Exclude:
      node = pdfium::MakeUnique<CXFA_Exclude>(doc, packet);
      break;
    case XFA_Element::ChoiceList:
      node = pdfium::MakeUnique<CXFA_ChoiceList>(doc, packet);
      break;
    case XFA_Element::Level:
      node = pdfium::MakeUnique<CXFA_Level>(doc, packet);
      break;
    case XFA_Element::LabelPrinter:
      node = pdfium::MakeUnique<CXFA_LabelPrinter>(doc, packet);
      break;
    case XFA_Element::CalendarSymbols:
      node = pdfium::MakeUnique<CXFA_CalendarSymbols>(doc, packet);
      break;
    case XFA_Element::Para:
      node = pdfium::MakeUnique<CXFA_Para>(doc, packet);
      break;
    case XFA_Element::Part:
      node = pdfium::MakeUnique<CXFA_Part>(doc, packet);
      break;
    case XFA_Element::Pdfa:
      node = pdfium::MakeUnique<CXFA_Pdfa>(doc, packet);
      break;
    case XFA_Element::Filter:
      node = pdfium::MakeUnique<CXFA_Filter>(doc, packet);
      break;
    case XFA_Element::Present:
      node = pdfium::MakeUnique<CXFA_Present>(doc, packet);
      break;
    case XFA_Element::Pagination:
      node = pdfium::MakeUnique<CXFA_Pagination>(doc, packet);
      break;
    case XFA_Element::Encoding:
      node = pdfium::MakeUnique<CXFA_Encoding>(doc, packet);
      break;
    case XFA_Element::Event:
      node = pdfium::MakeUnique<CXFA_Event>(doc, packet);
      break;
    case XFA_Element::Whitespace:
      node = pdfium::MakeUnique<CXFA_Whitespace>(doc, packet);
      break;
    case XFA_Element::DefaultUi:
      node = pdfium::MakeUnique<CXFA_DefaultUi>(doc, packet);
      break;
    case XFA_Element::DataModel:
      node = pdfium::MakeUnique<CXFA_DataModel>(doc, packet);
      break;
    case XFA_Element::Barcode:
      node = pdfium::MakeUnique<CXFA_Barcode>(doc, packet);
      break;
    case XFA_Element::TimePattern:
      node = pdfium::MakeUnique<CXFA_TimePattern>(doc, packet);
      break;
    case XFA_Element::BatchOutput:
      node = pdfium::MakeUnique<CXFA_BatchOutput>(doc, packet);
      break;
    case XFA_Element::Enforce:
      node = pdfium::MakeUnique<CXFA_Enforce>(doc, packet);
      break;
    case XFA_Element::CurrencySymbols:
      node = pdfium::MakeUnique<CXFA_CurrencySymbols>(doc, packet);
      break;
    case XFA_Element::AddSilentPrint:
      node = pdfium::MakeUnique<CXFA_AddSilentPrint>(doc, packet);
      break;
    case XFA_Element::Rename:
      node = pdfium::MakeUnique<CXFA_Rename>(doc, packet);
      break;
    case XFA_Element::Operation:
      node = pdfium::MakeUnique<CXFA_Operation>(doc, packet);
      break;
    case XFA_Element::Typefaces:
      node = pdfium::MakeUnique<CXFA_Typefaces>(doc, packet);
      break;
    case XFA_Element::SubjectDNs:
      node = pdfium::MakeUnique<CXFA_SubjectDNs>(doc, packet);
      break;
    case XFA_Element::Issuers:
      node = pdfium::MakeUnique<CXFA_Issuers>(doc, packet);
      break;
    case XFA_Element::WsdlConnection:
      node = pdfium::MakeUnique<CXFA_WsdlConnection>(doc, packet);
      break;
    case XFA_Element::Debug:
      node = pdfium::MakeUnique<CXFA_Debug>(doc, packet);
      break;
    case XFA_Element::Delta:
      node = pdfium::MakeUnique<CXFA_Delta>(doc, packet);
      break;
    case XFA_Element::EraNames:
      node = pdfium::MakeUnique<CXFA_EraNames>(doc, packet);
      break;
    case XFA_Element::ModifyAnnots:
      node = pdfium::MakeUnique<CXFA_ModifyAnnots>(doc, packet);
      break;
    case XFA_Element::StartNode:
      node = pdfium::MakeUnique<CXFA_StartNode>(doc, packet);
      break;
    case XFA_Element::Button:
      node = pdfium::MakeUnique<CXFA_Button>(doc, packet);
      break;
    case XFA_Element::Format:
      node = pdfium::MakeUnique<CXFA_Format>(doc, packet);
      break;
    case XFA_Element::Border:
      node = pdfium::MakeUnique<CXFA_Border>(doc, packet);
      break;
    case XFA_Element::Area:
      node = pdfium::MakeUnique<CXFA_Area>(doc, packet);
      break;
    case XFA_Element::Hyphenation:
      node = pdfium::MakeUnique<CXFA_Hyphenation>(doc, packet);
      break;
    case XFA_Element::Text:
      node = pdfium::MakeUnique<CXFA_Text>(doc, packet);
      break;
    case XFA_Element::Time:
      node = pdfium::MakeUnique<CXFA_Time>(doc, packet);
      break;
    case XFA_Element::Type:
      node = pdfium::MakeUnique<CXFA_Type>(doc, packet);
      break;
    case XFA_Element::Overprint:
      node = pdfium::MakeUnique<CXFA_Overprint>(doc, packet);
      break;
    case XFA_Element::Certificates:
      node = pdfium::MakeUnique<CXFA_Certificates>(doc, packet);
      break;
    case XFA_Element::EncryptionMethods:
      node = pdfium::MakeUnique<CXFA_EncryptionMethods>(doc, packet);
      break;
    case XFA_Element::SetProperty:
      node = pdfium::MakeUnique<CXFA_SetProperty>(doc, packet);
      break;
    case XFA_Element::PrinterName:
      node = pdfium::MakeUnique<CXFA_PrinterName>(doc, packet);
      break;
    case XFA_Element::StartPage:
      node = pdfium::MakeUnique<CXFA_StartPage>(doc, packet);
      break;
    case XFA_Element::PageOffset:
      node = pdfium::MakeUnique<CXFA_PageOffset>(doc, packet);
      break;
    case XFA_Element::DateTime:
      node = pdfium::MakeUnique<CXFA_DateTime>(doc, packet);
      break;
    case XFA_Element::Comb:
      node = pdfium::MakeUnique<CXFA_Comb>(doc, packet);
      break;
    case XFA_Element::Pattern:
      node = pdfium::MakeUnique<CXFA_Pattern>(doc, packet);
      break;
    case XFA_Element::IfEmpty:
      node = pdfium::MakeUnique<CXFA_IfEmpty>(doc, packet);
      break;
    case XFA_Element::SuppressBanner:
      node = pdfium::MakeUnique<CXFA_SuppressBanner>(doc, packet);
      break;
    case XFA_Element::OutputBin:
      node = pdfium::MakeUnique<CXFA_OutputBin>(doc, packet);
      break;
    case XFA_Element::Field:
      node = pdfium::MakeUnique<CXFA_Field>(doc, packet);
      break;
    case XFA_Element::Agent:
      node = pdfium::MakeUnique<CXFA_Agent>(doc, packet);
      break;
    case XFA_Element::OutputXSL:
      node = pdfium::MakeUnique<CXFA_OutputXSL>(doc, packet);
      break;
    case XFA_Element::AdjustData:
      node = pdfium::MakeUnique<CXFA_AdjustData>(doc, packet);
      break;
    case XFA_Element::AutoSave:
      node = pdfium::MakeUnique<CXFA_AutoSave>(doc, packet);
      break;
    case XFA_Element::ContentArea:
      node = pdfium::MakeUnique<CXFA_ContentArea>(doc, packet);
      break;
    case XFA_Element::WsdlAddress:
      node = pdfium::MakeUnique<CXFA_WsdlAddress>(doc, packet);
      break;
    case XFA_Element::Solid:
      node = pdfium::MakeUnique<CXFA_Solid>(doc, packet);
      break;
    case XFA_Element::DateTimeSymbols:
      node = pdfium::MakeUnique<CXFA_DateTimeSymbols>(doc, packet);
      break;
    case XFA_Element::EncryptionLevel:
      node = pdfium::MakeUnique<CXFA_EncryptionLevel>(doc, packet);
      break;
    case XFA_Element::Edge:
      node = pdfium::MakeUnique<CXFA_Edge>(doc, packet);
      break;
    case XFA_Element::Stipple:
      node = pdfium::MakeUnique<CXFA_Stipple>(doc, packet);
      break;
    case XFA_Element::Attributes:
      node = pdfium::MakeUnique<CXFA_Attributes>(doc, packet);
      break;
    case XFA_Element::VersionControl:
      node = pdfium::MakeUnique<CXFA_VersionControl>(doc, packet);
      break;
    case XFA_Element::Meridiem:
      node = pdfium::MakeUnique<CXFA_Meridiem>(doc, packet);
      break;
    case XFA_Element::ExclGroup:
      node = pdfium::MakeUnique<CXFA_ExclGroup>(doc, packet);
      break;
    case XFA_Element::ToolTip:
      node = pdfium::MakeUnique<CXFA_ToolTip>(doc, packet);
      break;
    case XFA_Element::Compress:
      node = pdfium::MakeUnique<CXFA_Compress>(doc, packet);
      break;
    case XFA_Element::Reason:
      node = pdfium::MakeUnique<CXFA_Reason>(doc, packet);
      break;
    case XFA_Element::Execute:
      node = pdfium::MakeUnique<CXFA_Execute>(doc, packet);
      break;
    case XFA_Element::ContentCopy:
      node = pdfium::MakeUnique<CXFA_ContentCopy>(doc, packet);
      break;
    case XFA_Element::DateTimeEdit:
      node = pdfium::MakeUnique<CXFA_DateTimeEdit>(doc, packet);
      break;
    case XFA_Element::Config:
      node = pdfium::MakeUnique<CXFA_Config>(doc, packet);
      break;
    case XFA_Element::Image:
      node = pdfium::MakeUnique<CXFA_Image>(doc, packet);
      break;
    case XFA_Element::SharpxHTML:
      node = pdfium::MakeUnique<CXFA_SharpxHTML>(doc, packet);
      break;
    case XFA_Element::NumberOfCopies:
      node = pdfium::MakeUnique<CXFA_NumberOfCopies>(doc, packet);
      break;
    case XFA_Element::BehaviorOverride:
      node = pdfium::MakeUnique<CXFA_BehaviorOverride>(doc, packet);
      break;
    case XFA_Element::TimeStamp:
      node = pdfium::MakeUnique<CXFA_TimeStamp>(doc, packet);
      break;
    case XFA_Element::Month:
      node = pdfium::MakeUnique<CXFA_Month>(doc, packet);
      break;
    case XFA_Element::ViewerPreferences:
      node = pdfium::MakeUnique<CXFA_ViewerPreferences>(doc, packet);
      break;
    case XFA_Element::ScriptModel:
      node = pdfium::MakeUnique<CXFA_ScriptModel>(doc, packet);
      break;
    case XFA_Element::Decimal:
      node = pdfium::MakeUnique<CXFA_Decimal>(doc, packet);
      break;
    case XFA_Element::Subform:
      node = pdfium::MakeUnique<CXFA_Subform>(doc, packet);
      break;
    case XFA_Element::Select:
      node = pdfium::MakeUnique<CXFA_Select>(doc, packet);
      break;
    case XFA_Element::Window:
      node = pdfium::MakeUnique<CXFA_Window>(doc, packet);
      break;
    case XFA_Element::LocaleSet:
      node = pdfium::MakeUnique<CXFA_LocaleSet>(doc, packet);
      break;
    case XFA_Element::Handler:
      node = pdfium::MakeUnique<CXFA_Handler>(doc, packet);
      break;
    case XFA_Element::Presence:
      node = pdfium::MakeUnique<CXFA_Presence>(doc, packet);
      break;
    case XFA_Element::Record:
      node = pdfium::MakeUnique<CXFA_Record>(doc, packet);
      break;
    case XFA_Element::Embed:
      node = pdfium::MakeUnique<CXFA_Embed>(doc, packet);
      break;
    case XFA_Element::Version:
      node = pdfium::MakeUnique<CXFA_Version>(doc, packet);
      break;
    case XFA_Element::Command:
      node = pdfium::MakeUnique<CXFA_Command>(doc, packet);
      break;
    case XFA_Element::Copies:
      node = pdfium::MakeUnique<CXFA_Copies>(doc, packet);
      break;
    case XFA_Element::Staple:
      node = pdfium::MakeUnique<CXFA_Staple>(doc, packet);
      break;
    case XFA_Element::SubmitFormat:
      node = pdfium::MakeUnique<CXFA_SubmitFormat>(doc, packet);
      break;
    case XFA_Element::Boolean:
      node = pdfium::MakeUnique<CXFA_Boolean>(doc, packet);
      break;
    case XFA_Element::Message:
      node = pdfium::MakeUnique<CXFA_Message>(doc, packet);
      break;
    case XFA_Element::Output:
      node = pdfium::MakeUnique<CXFA_Output>(doc, packet);
      break;
    case XFA_Element::PsMap:
      node = pdfium::MakeUnique<CXFA_PsMap>(doc, packet);
      break;
    case XFA_Element::ExcludeNS:
      node = pdfium::MakeUnique<CXFA_ExcludeNS>(doc, packet);
      break;
    case XFA_Element::Assist:
      node = pdfium::MakeUnique<CXFA_Assist>(doc, packet);
      break;
    case XFA_Element::Picture:
      node = pdfium::MakeUnique<CXFA_Picture>(doc, packet);
      break;
    case XFA_Element::Traversal:
      node = pdfium::MakeUnique<CXFA_Traversal>(doc, packet);
      break;
    case XFA_Element::SilentPrint:
      node = pdfium::MakeUnique<CXFA_SilentPrint>(doc, packet);
      break;
    case XFA_Element::WebClient:
      node = pdfium::MakeUnique<CXFA_WebClient>(doc, packet);
      break;
    case XFA_Element::Producer:
      node = pdfium::MakeUnique<CXFA_Producer>(doc, packet);
      break;
    case XFA_Element::Corner:
      node = pdfium::MakeUnique<CXFA_Corner>(doc, packet);
      break;
    case XFA_Element::MsgId:
      node = pdfium::MakeUnique<CXFA_MsgId>(doc, packet);
      break;
    case XFA_Element::Color:
      node = pdfium::MakeUnique<CXFA_Color>(doc, packet);
      break;
    case XFA_Element::Keep:
      node = pdfium::MakeUnique<CXFA_Keep>(doc, packet);
      break;
    case XFA_Element::Query:
      node = pdfium::MakeUnique<CXFA_Query>(doc, packet);
      break;
    case XFA_Element::Insert:
      node = pdfium::MakeUnique<CXFA_Insert>(doc, packet);
      break;
    case XFA_Element::ImageEdit:
      node = pdfium::MakeUnique<CXFA_ImageEdit>(doc, packet);
      break;
    case XFA_Element::Validate:
      node = pdfium::MakeUnique<CXFA_Validate>(doc, packet);
      break;
    case XFA_Element::DigestMethods:
      node = pdfium::MakeUnique<CXFA_DigestMethods>(doc, packet);
      break;
    case XFA_Element::NumberPatterns:
      node = pdfium::MakeUnique<CXFA_NumberPatterns>(doc, packet);
      break;
    case XFA_Element::PageSet:
      node = pdfium::MakeUnique<CXFA_PageSet>(doc, packet);
      break;
    case XFA_Element::Integer:
      node = pdfium::MakeUnique<CXFA_Integer>(doc, packet);
      break;
    case XFA_Element::SoapAddress:
      node = pdfium::MakeUnique<CXFA_SoapAddress>(doc, packet);
      break;
    case XFA_Element::Equate:
      node = pdfium::MakeUnique<CXFA_Equate>(doc, packet);
      break;
    case XFA_Element::FormFieldFilling:
      node = pdfium::MakeUnique<CXFA_FormFieldFilling>(doc, packet);
      break;
    case XFA_Element::PageRange:
      node = pdfium::MakeUnique<CXFA_PageRange>(doc, packet);
      break;
    case XFA_Element::Update:
      node = pdfium::MakeUnique<CXFA_Update>(doc, packet);
      break;
    case XFA_Element::ConnectString:
      node = pdfium::MakeUnique<CXFA_ConnectString>(doc, packet);
      break;
    case XFA_Element::Mode:
      node = pdfium::MakeUnique<CXFA_Mode>(doc, packet);
      break;
    case XFA_Element::Layout:
      node = pdfium::MakeUnique<CXFA_Layout>(doc, packet);
      break;
    case XFA_Element::Sharpxml:
      node = pdfium::MakeUnique<CXFA_Sharpxml>(doc, packet);
      break;
    case XFA_Element::XsdConnection:
      node = pdfium::MakeUnique<CXFA_XsdConnection>(doc, packet);
      break;
    case XFA_Element::Traverse:
      node = pdfium::MakeUnique<CXFA_Traverse>(doc, packet);
      break;
    case XFA_Element::Encodings:
      node = pdfium::MakeUnique<CXFA_Encodings>(doc, packet);
      break;
    case XFA_Element::Template:
      node = pdfium::MakeUnique<CXFA_Template>(doc, packet);
      break;
    case XFA_Element::Acrobat:
      node = pdfium::MakeUnique<CXFA_Acrobat>(doc, packet);
      break;
    case XFA_Element::ValidationMessaging:
      node = pdfium::MakeUnique<CXFA_ValidationMessaging>(doc, packet);
      break;
    case XFA_Element::Signing:
      node = pdfium::MakeUnique<CXFA_Signing>(doc, packet);
      break;
    case XFA_Element::Script:
      node = pdfium::MakeUnique<CXFA_Script>(doc, packet);
      break;
    case XFA_Element::AddViewerPreferences:
      node = pdfium::MakeUnique<CXFA_AddViewerPreferences>(doc, packet);
      break;
    case XFA_Element::AlwaysEmbed:
      node = pdfium::MakeUnique<CXFA_AlwaysEmbed>(doc, packet);
      break;
    case XFA_Element::PasswordEdit:
      node = pdfium::MakeUnique<CXFA_PasswordEdit>(doc, packet);
      break;
    case XFA_Element::NumericEdit:
      node = pdfium::MakeUnique<CXFA_NumericEdit>(doc, packet);
      break;
    case XFA_Element::EncryptionMethod:
      node = pdfium::MakeUnique<CXFA_EncryptionMethod>(doc, packet);
      break;
    case XFA_Element::Change:
      node = pdfium::MakeUnique<CXFA_Change>(doc, packet);
      break;
    case XFA_Element::PageArea:
      node = pdfium::MakeUnique<CXFA_PageArea>(doc, packet);
      break;
    case XFA_Element::SubmitUrl:
      node = pdfium::MakeUnique<CXFA_SubmitUrl>(doc, packet);
      break;
    case XFA_Element::Oids:
      node = pdfium::MakeUnique<CXFA_Oids>(doc, packet);
      break;
    case XFA_Element::Signature:
      node = pdfium::MakeUnique<CXFA_Signature>(doc, packet);
      break;
    case XFA_Element::ADBE_JSConsole:
      node = pdfium::MakeUnique<CXFA_ADBE_JSConsole>(doc, packet);
      break;
    case XFA_Element::Caption:
      node = pdfium::MakeUnique<CXFA_Caption>(doc, packet);
      break;
    case XFA_Element::Relevant:
      node = pdfium::MakeUnique<CXFA_Relevant>(doc, packet);
      break;
    case XFA_Element::FlipLabel:
      node = pdfium::MakeUnique<CXFA_FlipLabel>(doc, packet);
      break;
    case XFA_Element::ExData:
      node = pdfium::MakeUnique<CXFA_ExData>(doc, packet);
      break;
    case XFA_Element::DayNames:
      node = pdfium::MakeUnique<CXFA_DayNames>(doc, packet);
      break;
    case XFA_Element::SoapAction:
      node = pdfium::MakeUnique<CXFA_SoapAction>(doc, packet);
      break;
    case XFA_Element::DefaultTypeface:
      node = pdfium::MakeUnique<CXFA_DefaultTypeface>(doc, packet);
      break;
    case XFA_Element::Manifest:
      node = pdfium::MakeUnique<CXFA_Manifest>(doc, packet);
      break;
    case XFA_Element::Overflow:
      node = pdfium::MakeUnique<CXFA_Overflow>(doc, packet);
      break;
    case XFA_Element::Linear:
      node = pdfium::MakeUnique<CXFA_Linear>(doc, packet);
      break;
    case XFA_Element::CurrencySymbol:
      node = pdfium::MakeUnique<CXFA_CurrencySymbol>(doc, packet);
      break;
    case XFA_Element::Delete:
      node = pdfium::MakeUnique<CXFA_Delete>(doc, packet);
      break;
    case XFA_Element::DigestMethod:
      node = pdfium::MakeUnique<CXFA_DigestMethod>(doc, packet);
      break;
    case XFA_Element::InstanceManager:
      node = pdfium::MakeUnique<CXFA_InstanceManager>(doc, packet);
      break;
    case XFA_Element::EquateRange:
      node = pdfium::MakeUnique<CXFA_EquateRange>(doc, packet);
      break;
    case XFA_Element::Medium:
      node = pdfium::MakeUnique<CXFA_Medium>(doc, packet);
      break;
    case XFA_Element::TextEdit:
      node = pdfium::MakeUnique<CXFA_TextEdit>(doc, packet);
      break;
    case XFA_Element::TemplateCache:
      node = pdfium::MakeUnique<CXFA_TemplateCache>(doc, packet);
      break;
    case XFA_Element::CompressObjectStream:
      node = pdfium::MakeUnique<CXFA_CompressObjectStream>(doc, packet);
      break;
    case XFA_Element::DataValue:
      node = pdfium::MakeUnique<CXFA_DataValue>(doc, packet);
      break;
    case XFA_Element::AccessibleContent:
      node = pdfium::MakeUnique<CXFA_AccessibleContent>(doc, packet);
      break;
    case XFA_Element::IncludeXDPContent:
      node = pdfium::MakeUnique<CXFA_IncludeXDPContent>(doc, packet);
      break;
    case XFA_Element::XmlConnection:
      node = pdfium::MakeUnique<CXFA_XmlConnection>(doc, packet);
      break;
    case XFA_Element::ValidateApprovalSignatures:
      node = pdfium::MakeUnique<CXFA_ValidateApprovalSignatures>(doc, packet);
      break;
    case XFA_Element::SignData:
      node = pdfium::MakeUnique<CXFA_SignData>(doc, packet);
      break;
    case XFA_Element::Packets:
      node = pdfium::MakeUnique<CXFA_Packets>(doc, packet);
      break;
    case XFA_Element::DatePattern:
      node = pdfium::MakeUnique<CXFA_DatePattern>(doc, packet);
      break;
    case XFA_Element::DuplexOption:
      node = pdfium::MakeUnique<CXFA_DuplexOption>(doc, packet);
      break;
    case XFA_Element::Base:
      node = pdfium::MakeUnique<CXFA_Base>(doc, packet);
      break;
    case XFA_Element::Bind:
      node = pdfium::MakeUnique<CXFA_Bind>(doc, packet);
      break;
    case XFA_Element::Compression:
      node = pdfium::MakeUnique<CXFA_Compression>(doc, packet);
      break;
    case XFA_Element::User:
      node = pdfium::MakeUnique<CXFA_User>(doc, packet);
      break;
    case XFA_Element::Rectangle:
      node = pdfium::MakeUnique<CXFA_Rectangle>(doc, packet);
      break;
    case XFA_Element::EffectiveOutputPolicy:
      node = pdfium::MakeUnique<CXFA_EffectiveOutputPolicy>(doc, packet);
      break;
    case XFA_Element::ADBE_JSDebugger:
      node = pdfium::MakeUnique<CXFA_ADBE_JSDebugger>(doc, packet);
      break;
    case XFA_Element::Acrobat7:
      node = pdfium::MakeUnique<CXFA_Acrobat7>(doc, packet);
      break;
    case XFA_Element::Interactive:
      node = pdfium::MakeUnique<CXFA_Interactive>(doc, packet);
      break;
    case XFA_Element::Locale:
      node = pdfium::MakeUnique<CXFA_Locale>(doc, packet);
      break;
    case XFA_Element::CurrentPage:
      node = pdfium::MakeUnique<CXFA_CurrentPage>(doc, packet);
      break;
    case XFA_Element::Data:
      node = pdfium::MakeUnique<CXFA_Data>(doc, packet);
      break;
    case XFA_Element::Date:
      node = pdfium::MakeUnique<CXFA_Date>(doc, packet);
      break;
    case XFA_Element::Desc:
      node = pdfium::MakeUnique<CXFA_Desc>(doc, packet);
      break;
    case XFA_Element::Encrypt:
      node = pdfium::MakeUnique<CXFA_Encrypt>(doc, packet);
      break;
    case XFA_Element::Draw:
      node = pdfium::MakeUnique<CXFA_Draw>(doc, packet);
      break;
    case XFA_Element::Encryption:
      node = pdfium::MakeUnique<CXFA_Encryption>(doc, packet);
      break;
    case XFA_Element::MeridiemNames:
      node = pdfium::MakeUnique<CXFA_MeridiemNames>(doc, packet);
      break;
    case XFA_Element::Messaging:
      node = pdfium::MakeUnique<CXFA_Messaging>(doc, packet);
      break;
    case XFA_Element::Speak:
      node = pdfium::MakeUnique<CXFA_Speak>(doc, packet);
      break;
    case XFA_Element::DataGroup:
      node = pdfium::MakeUnique<CXFA_DataGroup>(doc, packet);
      break;
    case XFA_Element::Common:
      node = pdfium::MakeUnique<CXFA_Common>(doc, packet);
      break;
    case XFA_Element::Sharptext:
      node = pdfium::MakeUnique<CXFA_Sharptext>(doc, packet);
      break;
    case XFA_Element::PaginationOverride:
      node = pdfium::MakeUnique<CXFA_PaginationOverride>(doc, packet);
      break;
    case XFA_Element::Reasons:
      node = pdfium::MakeUnique<CXFA_Reasons>(doc, packet);
      break;
    case XFA_Element::SignatureProperties:
      node = pdfium::MakeUnique<CXFA_SignatureProperties>(doc, packet);
      break;
    case XFA_Element::Threshold:
      node = pdfium::MakeUnique<CXFA_Threshold>(doc, packet);
      break;
    case XFA_Element::AppearanceFilter:
      node = pdfium::MakeUnique<CXFA_AppearanceFilter>(doc, packet);
      break;
    case XFA_Element::Fill:
      node = pdfium::MakeUnique<CXFA_Fill>(doc, packet);
      break;
    case XFA_Element::Font:
      node = pdfium::MakeUnique<CXFA_Font>(doc, packet);
      break;
    case XFA_Element::Form:
      node = pdfium::MakeUnique<CXFA_Form>(doc, packet);
      break;
    case XFA_Element::MediumInfo:
      node = pdfium::MakeUnique<CXFA_MediumInfo>(doc, packet);
      break;
    case XFA_Element::Certificate:
      node = pdfium::MakeUnique<CXFA_Certificate>(doc, packet);
      break;
    case XFA_Element::Password:
      node = pdfium::MakeUnique<CXFA_Password>(doc, packet);
      break;
    case XFA_Element::RunScripts:
      node = pdfium::MakeUnique<CXFA_RunScripts>(doc, packet);
      break;
    case XFA_Element::Trace:
      node = pdfium::MakeUnique<CXFA_Trace>(doc, packet);
      break;
    case XFA_Element::Float:
      node = pdfium::MakeUnique<CXFA_Float>(doc, packet);
      break;
    case XFA_Element::RenderPolicy:
      node = pdfium::MakeUnique<CXFA_RenderPolicy>(doc, packet);
      break;
    case XFA_Element::Destination:
      node = pdfium::MakeUnique<CXFA_Destination>(doc, packet);
      break;
    case XFA_Element::Value:
      node = pdfium::MakeUnique<CXFA_Value>(doc, packet);
      break;
    case XFA_Element::Bookend:
      node = pdfium::MakeUnique<CXFA_Bookend>(doc, packet);
      break;
    case XFA_Element::ExObject:
      node = pdfium::MakeUnique<CXFA_ExObject>(doc, packet);
      break;
    case XFA_Element::OpenAction:
      node = pdfium::MakeUnique<CXFA_OpenAction>(doc, packet);
      break;
    case XFA_Element::NeverEmbed:
      node = pdfium::MakeUnique<CXFA_NeverEmbed>(doc, packet);
      break;
    case XFA_Element::BindItems:
      node = pdfium::MakeUnique<CXFA_BindItems>(doc, packet);
      break;
    case XFA_Element::Calculate:
      node = pdfium::MakeUnique<CXFA_Calculate>(doc, packet);
      break;
    case XFA_Element::Print:
      node = pdfium::MakeUnique<CXFA_Print>(doc, packet);
      break;
    case XFA_Element::Extras:
      node = pdfium::MakeUnique<CXFA_Extras>(doc, packet);
      break;
    case XFA_Element::Proto:
      node = pdfium::MakeUnique<CXFA_Proto>(doc, packet);
      break;
    case XFA_Element::DSigData:
      node = pdfium::MakeUnique<CXFA_DSigData>(doc, packet);
      break;
    case XFA_Element::Creator:
      node = pdfium::MakeUnique<CXFA_Creator>(doc, packet);
      break;
    case XFA_Element::Connect:
      node = pdfium::MakeUnique<CXFA_Connect>(doc, packet);
      break;
    case XFA_Element::Permissions:
      node = pdfium::MakeUnique<CXFA_Permissions>(doc, packet);
      break;
    case XFA_Element::ConnectionSet:
      node = pdfium::MakeUnique<CXFA_ConnectionSet>(doc, packet);
      break;
    case XFA_Element::Submit:
      node = pdfium::MakeUnique<CXFA_Submit>(doc, packet);
      break;
    case XFA_Element::Range:
      node = pdfium::MakeUnique<CXFA_Range>(doc, packet);
      break;
    case XFA_Element::Linearized:
      node = pdfium::MakeUnique<CXFA_Linearized>(doc, packet);
      break;
    case XFA_Element::Packet:
      node = pdfium::MakeUnique<CXFA_Packet>(doc, packet);
      break;
    case XFA_Element::RootElement:
      node = pdfium::MakeUnique<CXFA_RootElement>(doc, packet);
      break;
    case XFA_Element::PlaintextMetadata:
      node = pdfium::MakeUnique<CXFA_PlaintextMetadata>(doc, packet);
      break;
    case XFA_Element::NumberSymbols:
      node = pdfium::MakeUnique<CXFA_NumberSymbols>(doc, packet);
      break;
    case XFA_Element::PrintHighQuality:
      node = pdfium::MakeUnique<CXFA_PrintHighQuality>(doc, packet);
      break;
    case XFA_Element::Driver:
      node = pdfium::MakeUnique<CXFA_Driver>(doc, packet);
      break;
    case XFA_Element::IncrementalLoad:
      node = pdfium::MakeUnique<CXFA_IncrementalLoad>(doc, packet);
      break;
    case XFA_Element::SubjectDN:
      node = pdfium::MakeUnique<CXFA_SubjectDN>(doc, packet);
      break;
    case XFA_Element::CompressLogicalStructure:
      node = pdfium::MakeUnique<CXFA_CompressLogicalStructure>(doc, packet);
      break;
    case XFA_Element::IncrementalMerge:
      node = pdfium::MakeUnique<CXFA_IncrementalMerge>(doc, packet);
      break;
    case XFA_Element::Radial:
      node = pdfium::MakeUnique<CXFA_Radial>(doc, packet);
      break;
    case XFA_Element::Variables:
      node = pdfium::MakeUnique<CXFA_Variables>(doc, packet);
      break;
    case XFA_Element::TimePatterns:
      node = pdfium::MakeUnique<CXFA_TimePatterns>(doc, packet);
      break;
    case XFA_Element::EffectiveInputPolicy:
      node = pdfium::MakeUnique<CXFA_EffectiveInputPolicy>(doc, packet);
      break;
    case XFA_Element::NameAttr:
      node = pdfium::MakeUnique<CXFA_NameAttr>(doc, packet);
      break;
    case XFA_Element::Conformance:
      node = pdfium::MakeUnique<CXFA_Conformance>(doc, packet);
      break;
    case XFA_Element::Transform:
      node = pdfium::MakeUnique<CXFA_Transform>(doc, packet);
      break;
    case XFA_Element::LockDocument:
      node = pdfium::MakeUnique<CXFA_LockDocument>(doc, packet);
      break;
    case XFA_Element::BreakAfter:
      node = pdfium::MakeUnique<CXFA_BreakAfter>(doc, packet);
      break;
    case XFA_Element::Line:
      node = pdfium::MakeUnique<CXFA_Line>(doc, packet);
      break;
    case XFA_Element::Source:
      node = pdfium::MakeUnique<CXFA_Source>(doc, packet);
      break;
    case XFA_Element::Occur:
      node = pdfium::MakeUnique<CXFA_Occur>(doc, packet);
      break;
    case XFA_Element::PickTrayByPDFSize:
      node = pdfium::MakeUnique<CXFA_PickTrayByPDFSize>(doc, packet);
      break;
    case XFA_Element::MonthNames:
      node = pdfium::MakeUnique<CXFA_MonthNames>(doc, packet);
      break;
    case XFA_Element::Severity:
      node = pdfium::MakeUnique<CXFA_Severity>(doc, packet);
      break;
    case XFA_Element::GroupParent:
      node = pdfium::MakeUnique<CXFA_GroupParent>(doc, packet);
      break;
    case XFA_Element::DocumentAssembly:
      node = pdfium::MakeUnique<CXFA_DocumentAssembly>(doc, packet);
      break;
    case XFA_Element::NumberSymbol:
      node = pdfium::MakeUnique<CXFA_NumberSymbol>(doc, packet);
      break;
    case XFA_Element::Tagged:
      node = pdfium::MakeUnique<CXFA_Tagged>(doc, packet);
      break;
    case XFA_Element::Items:
      node = pdfium::MakeUnique<CXFA_Items>(doc, packet);
      break;
    default:
      NOTREACHED();
      return nullptr;
  }
  if (!node || !node->IsValidInPacket(packet))
    return nullptr;
  return node;
}
