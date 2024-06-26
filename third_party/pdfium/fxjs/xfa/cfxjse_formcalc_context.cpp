// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/xfa/cfxjse_formcalc_context.h"

#include <algorithm>
#include <cstdlib>
#include <string>
#include <utility>

#include "core/fxcrt/cfx_widetextbuf.h"
#include "core/fxcrt/fx_extension.h"
#include "core/fxcrt/fx_random.h"
#include "fxjs/xfa/cfxjse_arguments.h"
#include "fxjs/xfa/cfxjse_class.h"
#include "fxjs/xfa/cfxjse_context.h"
#include "fxjs/xfa/cfxjse_engine.h"
#include "fxjs/xfa/cfxjse_value.h"
#include "fxjs/xfa/cjx_object.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"
#include "xfa/fgas/crt/cfgas_decimal.h"
#include "xfa/fgas/crt/locale_iface.h"
#include "xfa/fxfa/cxfa_ffnotify.h"
#include "xfa/fxfa/fm2js/cxfa_fmparser.h"
#include "xfa/fxfa/fm2js/cxfa_fmtojavascriptdepth.h"
#include "xfa/fxfa/parser/cxfa_document.h"
#include "xfa/fxfa/parser/cxfa_localevalue.h"
#include "xfa/fxfa/parser/cxfa_node.h"
#include "xfa/fxfa/parser/cxfa_timezoneprovider.h"
#include "xfa/fxfa/parser/xfa_utils.h"

using pdfium::fxjse::kFuncTag;
using pdfium::fxjse::kClassTag;

namespace {

const double kFinancialPrecision = 0.00000001;

const wchar_t kStrCode[] = L"0123456789abcdef";

struct XFA_FMHtmlReserveCode {
  uint32_t m_uCode;
  const char* m_htmlReserve;
};

// Sorted by |m_htmlReserve|.
const XFA_FMHtmlReserveCode kReservesForDecode[] = {
    {198, "AElig"},   {193, "Aacute"},   {194, "Acirc"},    {192, "Agrave"},
    {913, "Alpha"},   {197, "Aring"},    {195, "Atilde"},   {196, "Auml"},
    {914, "Beta"},    {199, "Ccedil"},   {935, "Chi"},      {8225, "Dagger"},
    {916, "Delta"},   {208, "ETH"},      {201, "Eacute"},   {202, "Ecirc"},
    {200, "Egrave"},  {917, "Epsilon"},  {919, "Eta"},      {203, "Euml"},
    {915, "Gamma"},   {922, "Kappa"},    {923, "Lambda"},   {924, "Mu"},
    {209, "Ntilde"},  {925, "Nu"},       {338, "OElig"},    {211, "Oacute"},
    {212, "Ocirc"},   {210, "Ograve"},   {937, "Omega"},    {927, "Omicron"},
    {216, "Oslash"},  {213, "Otilde"},   {214, "Ouml"},     {934, "Phi"},
    {928, "Pi"},      {936, "Psi"},      {929, "Rho"},      {352, "Scaron"},
    {931, "Sigma"},   {222, "THORN"},    {932, "Tau"},      {920, "Theta"},
    {218, "Uacute"},  {219, "Ucirc"},    {217, "Ugrave"},   {933, "Upsilon"},
    {220, "Uuml"},    {926, "Xi"},       {221, "Yacute"},   {376, "Yuml"},
    {918, "Zeta"},    {225, "aacute"},   {226, "acirc"},    {180, "acute"},
    {230, "aelig"},   {224, "agrave"},   {8501, "alefsym"}, {945, "alpha"},
    {38, "amp"},      {8743, "and"},     {8736, "ang"},     {39, "apos"},
    {229, "aring"},   {8776, "asymp"},   {227, "atilde"},   {228, "auml"},
    {8222, "bdquo"},  {946, "beta"},     {166, "brvbar"},   {8226, "bull"},
    {8745, "cap"},    {231, "ccedil"},   {184, "cedil"},    {162, "cent"},
    {967, "chi"},     {710, "circ"},     {9827, "clubs"},   {8773, "cong"},
    {169, "copy"},    {8629, "crarr"},   {8746, "cup"},     {164, "current"},
    {8659, "dArr"},   {8224, "dagger"},  {8595, "darr"},    {176, "deg"},
    {948, "delta"},   {9830, "diams"},   {247, "divide"},   {233, "eacute"},
    {234, "ecirc"},   {232, "egrave"},   {8709, "empty"},   {8195, "emsp"},
    {8194, "ensp"},   {949, "epsilon"},  {8801, "equiv"},   {951, "eta"},
    {240, "eth"},     {235, "euml"},     {8364, "euro"},    {8707, "exist"},
    {402, "fnof"},    {8704, "forall"},  {189, "frac12"},   {188, "frac14"},
    {190, "frac34"},  {8260, "frasl"},   {947, "gamma"},    {8805, "ge"},
    {62, "gt"},       {8660, "hArr"},    {8596, "harr"},    {9829, "hearts"},
    {8230, "hellip"}, {237, "iacute"},   {238, "icirc"},    {161, "iexcl"},
    {236, "igrave"},  {8465, "image"},   {8734, "infin"},   {8747, "int"},
    {953, "iota"},    {191, "iquest"},   {8712, "isin"},    {239, "iuml"},
    {954, "kappa"},   {8656, "lArr"},    {205, "lacute"},   {955, "lambda"},
    {9001, "lang"},   {171, "laquo"},    {8592, "larr"},    {8968, "lceil"},
    {206, "lcirc"},   {8220, "ldquo"},   {8804, "le"},      {8970, "lfloor"},
    {204, "lgrave"},  {921, "lota"},     {8727, "lowast"},  {9674, "loz"},
    {8206, "lrm"},    {8249, "lsaquo"},  {8216, "lsquo"},   {60, "lt"},
    {207, "luml"},    {175, "macr"},     {8212, "mdash"},   {181, "micro"},
    {183, "middot"},  {8722, "minus"},   {956, "mu"},       {8711, "nabla"},
    {160, "nbsp"},    {8211, "ndash"},   {8800, "ne"},      {8715, "ni"},
    {172, "not"},     {8713, "notin"},   {8836, "nsub"},    {241, "ntilde"},
    {957, "nu"},      {243, "oacute"},   {244, "ocirc"},    {339, "oelig"},
    {242, "ograve"},  {8254, "oline"},   {969, "omega"},    {959, "omicron"},
    {8853, "oplus"},  {8744, "or"},      {170, "ordf"},     {186, "ordm"},
    {248, "oslash"},  {245, "otilde"},   {8855, "otimes"},  {246, "ouml"},
    {182, "para"},    {8706, "part"},    {8240, "permil"},  {8869, "perp"},
    {966, "phi"},     {960, "pi"},       {982, "piv"},      {177, "plusmn"},
    {8242, "prime"},  {8719, "prod"},    {8733, "prop"},    {968, "psi"},
    {163, "pund"},    {34, "quot"},      {8658, "rArr"},    {8730, "radic"},
    {9002, "rang"},   {187, "raquo"},    {8594, "rarr"},    {8969, "rceil"},
    {8476, "real"},   {174, "reg"},      {8971, "rfloor"},  {961, "rho"},
    {8207, "rlm"},    {8250, "rsaquo"},  {8217, "rsquo"},   {353, "saron"},
    {8218, "sbquo"},  {8901, "sdot"},    {167, "sect"},     {173, "shy"},
    {963, "sigma"},   {962, "sigmaf"},   {8764, "sim"},     {9824, "spades"},
    {8834, "sub"},    {8838, "sube"},    {8721, "sum"},     {8835, "sup"},
    {185, "sup1"},    {178, "sup2"},     {179, "sup3"},     {8839, "supe"},
    {223, "szlig"},   {964, "tau"},      {8221, "tdquo"},   {8756, "there4"},
    {952, "theta"},   {977, "thetasym"}, {8201, "thinsp"},  {254, "thorn"},
    {732, "tilde"},   {215, "times"},    {8482, "trade"},   {8657, "uArr"},
    {250, "uacute"},  {8593, "uarr"},    {251, "ucirc"},    {249, "ugrave"},
    {168, "uml"},     {978, "upsih"},    {965, "upsilon"},  {252, "uuml"},
    {8472, "weierp"}, {958, "xi"},       {253, "yacute"},   {165, "yen"},
    {255, "yuml"},    {950, "zeta"},     {8205, "zwj"},     {8204, "zwnj"},
};

// Sorted by |m_uCode|.
const XFA_FMHtmlReserveCode kReservesForEncode[] = {
    {34, "quot"},     {38, "amp"},      {39, "apos"},      {60, "lt"},
    {62, "gt"},       {160, "nbsp"},    {161, "iexcl"},    {162, "cent"},
    {163, "pund"},    {164, "current"}, {165, "yen"},      {166, "brvbar"},
    {167, "sect"},    {168, "uml"},     {169, "copy"},     {170, "ordf"},
    {171, "laquo"},   {172, "not"},     {173, "shy"},      {174, "reg"},
    {175, "macr"},    {176, "deg"},     {177, "plusmn"},   {178, "sup2"},
    {179, "sup3"},    {180, "acute"},   {181, "micro"},    {182, "para"},
    {183, "middot"},  {184, "cedil"},   {185, "sup1"},     {186, "ordm"},
    {187, "raquo"},   {188, "frac14"},  {189, "frac12"},   {190, "frac34"},
    {191, "iquest"},  {192, "Agrave"},  {193, "Aacute"},   {194, "Acirc"},
    {195, "Atilde"},  {196, "Auml"},    {197, "Aring"},    {198, "AElig"},
    {199, "Ccedil"},  {200, "Egrave"},  {201, "Eacute"},   {202, "Ecirc"},
    {203, "Euml"},    {204, "lgrave"},  {205, "lacute"},   {206, "lcirc"},
    {207, "luml"},    {208, "ETH"},     {209, "Ntilde"},   {210, "Ograve"},
    {211, "Oacute"},  {212, "Ocirc"},   {213, "Otilde"},   {214, "Ouml"},
    {215, "times"},   {216, "Oslash"},  {217, "Ugrave"},   {218, "Uacute"},
    {219, "Ucirc"},   {220, "Uuml"},    {221, "Yacute"},   {222, "THORN"},
    {223, "szlig"},   {224, "agrave"},  {225, "aacute"},   {226, "acirc"},
    {227, "atilde"},  {228, "auml"},    {229, "aring"},    {230, "aelig"},
    {231, "ccedil"},  {232, "egrave"},  {233, "eacute"},   {234, "ecirc"},
    {235, "euml"},    {236, "igrave"},  {237, "iacute"},   {238, "icirc"},
    {239, "iuml"},    {240, "eth"},     {241, "ntilde"},   {242, "ograve"},
    {243, "oacute"},  {244, "ocirc"},   {245, "otilde"},   {246, "ouml"},
    {247, "divide"},  {248, "oslash"},  {249, "ugrave"},   {250, "uacute"},
    {251, "ucirc"},   {252, "uuml"},    {253, "yacute"},   {254, "thorn"},
    {255, "yuml"},    {338, "OElig"},   {339, "oelig"},    {352, "Scaron"},
    {353, "saron"},   {376, "Yuml"},    {402, "fnof"},     {710, "circ"},
    {732, "tilde"},   {913, "Alpha"},   {914, "Beta"},     {915, "Gamma"},
    {916, "Delta"},   {917, "Epsilon"}, {918, "Zeta"},     {919, "Eta"},
    {920, "Theta"},   {921, "lota"},    {922, "Kappa"},    {923, "Lambda"},
    {924, "Mu"},      {925, "Nu"},      {926, "Xi"},       {927, "Omicron"},
    {928, "Pi"},      {929, "Rho"},     {931, "Sigma"},    {932, "Tau"},
    {933, "Upsilon"}, {934, "Phi"},     {935, "Chi"},      {936, "Psi"},
    {937, "Omega"},   {945, "alpha"},   {946, "beta"},     {947, "gamma"},
    {948, "delta"},   {949, "epsilon"}, {950, "zeta"},     {951, "eta"},
    {952, "theta"},   {953, "iota"},    {954, "kappa"},    {955, "lambda"},
    {956, "mu"},      {957, "nu"},      {958, "xi"},       {959, "omicron"},
    {960, "pi"},      {961, "rho"},     {962, "sigmaf"},   {963, "sigma"},
    {964, "tau"},     {965, "upsilon"}, {966, "phi"},      {967, "chi"},
    {968, "psi"},     {969, "omega"},   {977, "thetasym"}, {978, "upsih"},
    {982, "piv"},     {8194, "ensp"},   {8195, "emsp"},    {8201, "thinsp"},
    {8204, "zwnj"},   {8205, "zwj"},    {8206, "lrm"},     {8207, "rlm"},
    {8211, "ndash"},  {8212, "mdash"},  {8216, "lsquo"},   {8217, "rsquo"},
    {8218, "sbquo"},  {8220, "ldquo"},  {8221, "tdquo"},   {8222, "bdquo"},
    {8224, "dagger"}, {8225, "Dagger"}, {8226, "bull"},    {8230, "hellip"},
    {8240, "permil"}, {8242, "prime"},  {8249, "lsaquo"},  {8250, "rsaquo"},
    {8254, "oline"},  {8260, "frasl"},  {8364, "euro"},    {8465, "image"},
    {8472, "weierp"}, {8476, "real"},   {8482, "trade"},   {8501, "alefsym"},
    {8592, "larr"},   {8593, "uarr"},   {8594, "rarr"},    {8595, "darr"},
    {8596, "harr"},   {8629, "crarr"},  {8656, "lArr"},    {8657, "uArr"},
    {8658, "rArr"},   {8659, "dArr"},   {8660, "hArr"},    {8704, "forall"},
    {8706, "part"},   {8707, "exist"},  {8709, "empty"},   {8711, "nabla"},
    {8712, "isin"},   {8713, "notin"},  {8715, "ni"},      {8719, "prod"},
    {8721, "sum"},    {8722, "minus"},  {8727, "lowast"},  {8730, "radic"},
    {8733, "prop"},   {8734, "infin"},  {8736, "ang"},     {8743, "and"},
    {8744, "or"},     {8745, "cap"},    {8746, "cup"},     {8747, "int"},
    {8756, "there4"}, {8764, "sim"},    {8773, "cong"},    {8776, "asymp"},
    {8800, "ne"},     {8801, "equiv"},  {8804, "le"},      {8805, "ge"},
    {8834, "sub"},    {8835, "sup"},    {8836, "nsub"},    {8838, "sube"},
    {8839, "supe"},   {8853, "oplus"},  {8855, "otimes"},  {8869, "perp"},
    {8901, "sdot"},   {8968, "lceil"},  {8969, "rceil"},   {8970, "lfloor"},
    {8971, "rfloor"}, {9001, "lang"},   {9002, "rang"},    {9674, "loz"},
    {9824, "spades"}, {9827, "clubs"},  {9829, "hearts"},  {9830, "diams"},
};

const FXJSE_FUNCTION_DESCRIPTOR kFormCalcFM2JSFunctions[] = {
    {kFuncTag, "Abs", CFXJSE_FormCalcContext::Abs},
    {kFuncTag, "Avg", CFXJSE_FormCalcContext::Avg},
    {kFuncTag, "Ceil", CFXJSE_FormCalcContext::Ceil},
    {kFuncTag, "Count", CFXJSE_FormCalcContext::Count},
    {kFuncTag, "Floor", CFXJSE_FormCalcContext::Floor},
    {kFuncTag, "Max", CFXJSE_FormCalcContext::Max},
    {kFuncTag, "Min", CFXJSE_FormCalcContext::Min},
    {kFuncTag, "Mod", CFXJSE_FormCalcContext::Mod},
    {kFuncTag, "Round", CFXJSE_FormCalcContext::Round},
    {kFuncTag, "Sum", CFXJSE_FormCalcContext::Sum},
    {kFuncTag, "Date", CFXJSE_FormCalcContext::Date},
    {kFuncTag, "Date2Num", CFXJSE_FormCalcContext::Date2Num},
    {kFuncTag, "DateFmt", CFXJSE_FormCalcContext::DateFmt},
    {kFuncTag, "IsoDate2Num", CFXJSE_FormCalcContext::IsoDate2Num},
    {kFuncTag, "IsoTime2Num", CFXJSE_FormCalcContext::IsoTime2Num},
    {kFuncTag, "LocalDateFmt", CFXJSE_FormCalcContext::LocalDateFmt},
    {kFuncTag, "LocalTimeFmt", CFXJSE_FormCalcContext::LocalTimeFmt},
    {kFuncTag, "Num2Date", CFXJSE_FormCalcContext::Num2Date},
    {kFuncTag, "Num2GMTime", CFXJSE_FormCalcContext::Num2GMTime},
    {kFuncTag, "Num2Time", CFXJSE_FormCalcContext::Num2Time},
    {kFuncTag, "Time", CFXJSE_FormCalcContext::Time},
    {kFuncTag, "Time2Num", CFXJSE_FormCalcContext::Time2Num},
    {kFuncTag, "TimeFmt", CFXJSE_FormCalcContext::TimeFmt},
    {kFuncTag, "Apr", CFXJSE_FormCalcContext::Apr},
    {kFuncTag, "Cterm", CFXJSE_FormCalcContext::CTerm},
    {kFuncTag, "FV", CFXJSE_FormCalcContext::FV},
    {kFuncTag, "Ipmt", CFXJSE_FormCalcContext::IPmt},
    {kFuncTag, "NPV", CFXJSE_FormCalcContext::NPV},
    {kFuncTag, "Pmt", CFXJSE_FormCalcContext::Pmt},
    {kFuncTag, "PPmt", CFXJSE_FormCalcContext::PPmt},
    {kFuncTag, "PV", CFXJSE_FormCalcContext::PV},
    {kFuncTag, "Rate", CFXJSE_FormCalcContext::Rate},
    {kFuncTag, "Term", CFXJSE_FormCalcContext::Term},
    {kFuncTag, "Choose", CFXJSE_FormCalcContext::Choose},
    {kFuncTag, "Exists", CFXJSE_FormCalcContext::Exists},
    {kFuncTag, "HasValue", CFXJSE_FormCalcContext::HasValue},
    {kFuncTag, "Oneof", CFXJSE_FormCalcContext::Oneof},
    {kFuncTag, "Within", CFXJSE_FormCalcContext::Within},
    {kFuncTag, "If", CFXJSE_FormCalcContext::If},
    {kFuncTag, "Eval", CFXJSE_FormCalcContext::Eval},
    {kFuncTag, "Translate", CFXJSE_FormCalcContext::eval_translation},
    {kFuncTag, "Ref", CFXJSE_FormCalcContext::Ref},
    {kFuncTag, "UnitType", CFXJSE_FormCalcContext::UnitType},
    {kFuncTag, "UnitValue", CFXJSE_FormCalcContext::UnitValue},
    {kFuncTag, "At", CFXJSE_FormCalcContext::At},
    {kFuncTag, "Concat", CFXJSE_FormCalcContext::Concat},
    {kFuncTag, "Decode", CFXJSE_FormCalcContext::Decode},
    {kFuncTag, "Encode", CFXJSE_FormCalcContext::Encode},
    {kFuncTag, "Format", CFXJSE_FormCalcContext::Format},
    {kFuncTag, "Left", CFXJSE_FormCalcContext::Left},
    {kFuncTag, "Len", CFXJSE_FormCalcContext::Len},
    {kFuncTag, "Lower", CFXJSE_FormCalcContext::Lower},
    {kFuncTag, "Ltrim", CFXJSE_FormCalcContext::Ltrim},
    {kFuncTag, "Parse", CFXJSE_FormCalcContext::Parse},
    {kFuncTag, "Replace", CFXJSE_FormCalcContext::Replace},
    {kFuncTag, "Right", CFXJSE_FormCalcContext::Right},
    {kFuncTag, "Rtrim", CFXJSE_FormCalcContext::Rtrim},
    {kFuncTag, "Space", CFXJSE_FormCalcContext::Space},
    {kFuncTag, "Str", CFXJSE_FormCalcContext::Str},
    {kFuncTag, "Stuff", CFXJSE_FormCalcContext::Stuff},
    {kFuncTag, "Substr", CFXJSE_FormCalcContext::Substr},
    {kFuncTag, "Uuid", CFXJSE_FormCalcContext::Uuid},
    {kFuncTag, "Upper", CFXJSE_FormCalcContext::Upper},
    {kFuncTag, "WordNum", CFXJSE_FormCalcContext::WordNum},
    {kFuncTag, "Get", CFXJSE_FormCalcContext::Get},
    {kFuncTag, "Post", CFXJSE_FormCalcContext::Post},
    {kFuncTag, "Put", CFXJSE_FormCalcContext::Put},
    {kFuncTag, "pos_op", CFXJSE_FormCalcContext::positive_operator},
    {kFuncTag, "neg_op", CFXJSE_FormCalcContext::negative_operator},
    {kFuncTag, "log_or_op", CFXJSE_FormCalcContext::logical_or_operator},
    {kFuncTag, "log_and_op", CFXJSE_FormCalcContext::logical_and_operator},
    {kFuncTag, "log_not_op", CFXJSE_FormCalcContext::logical_not_operator},
    {kFuncTag, "eq_op", CFXJSE_FormCalcContext::equality_operator},
    {kFuncTag, "neq_op", CFXJSE_FormCalcContext::notequality_operator},
    {kFuncTag, "lt_op", CFXJSE_FormCalcContext::less_operator},
    {kFuncTag, "le_op", CFXJSE_FormCalcContext::lessequal_operator},
    {kFuncTag, "gt_op", CFXJSE_FormCalcContext::greater_operator},
    {kFuncTag, "ge_op", CFXJSE_FormCalcContext::greaterequal_operator},
    {kFuncTag, "plus_op", CFXJSE_FormCalcContext::plus_operator},
    {kFuncTag, "minus_op", CFXJSE_FormCalcContext::minus_operator},
    {kFuncTag, "mul_op", CFXJSE_FormCalcContext::multiple_operator},
    {kFuncTag, "div_op", CFXJSE_FormCalcContext::divide_operator},
    {kFuncTag, "asgn_val_op", CFXJSE_FormCalcContext::assign_value_operator},
    {kFuncTag, "dot_acc", CFXJSE_FormCalcContext::dot_accessor},
    {kFuncTag, "dotdot_acc", CFXJSE_FormCalcContext::dotdot_accessor},
    {kFuncTag, "concat_obj", CFXJSE_FormCalcContext::concat_fm_object},
    {kFuncTag, "is_obj", CFXJSE_FormCalcContext::is_fm_object},
    {kFuncTag, "is_ary", CFXJSE_FormCalcContext::is_fm_array},
    {kFuncTag, "get_val", CFXJSE_FormCalcContext::get_fm_value},
    {kFuncTag, "get_jsobj", CFXJSE_FormCalcContext::get_fm_jsobj},
    {kFuncTag, "var_filter", CFXJSE_FormCalcContext::fm_var_filter},
};

const uint8_t kAltTableDate[] = {
    255, 255, 255, 3,   9,   255, 255, 255, 255, 255, 255,
    255, 2,   255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 1,   255, 255, 255, 255, 255, 255, 255, 255,
};
static_assert(FX_ArraySize(kAltTableDate) == L'a' - L'A' + 1,
              "Invalid kAltTableDate size.");

const uint8_t kAltTableTime[] = {
    14,  255, 255, 3,   9,   255, 255, 15,  255, 255, 255,
    255, 6,   255, 255, 255, 255, 255, 7,   255, 255, 255,
    255, 255, 1,   17,  255, 255, 255, 255, 255, 255, 255,
};
static_assert(FX_ArraySize(kAltTableTime) == L'a' - L'A' + 1,
              "Invalid kAltTableTime size.");

void AlternateDateTimeSymbols(WideString* pPattern,
                              const WideString& wsAltSymbols,
                              bool bIsDate) {
  const uint8_t* pAltTable = bIsDate ? kAltTableDate : kAltTableTime;
  int32_t nLength = pPattern->GetLength();
  bool bInConstRange = false;
  bool bEscape = false;
  int32_t i = 0;
  while (i < nLength) {
    wchar_t wc = (*pPattern)[i];
    if (wc == L'\'') {
      bInConstRange = !bInConstRange;
      if (bEscape) {
        i++;
      } else {
        pPattern->Delete(i);
        nLength--;
      }
      bEscape = !bEscape;
      continue;
    }
    if (!bInConstRange && wc >= L'A' && wc <= L'a') {
      uint8_t nAlt = pAltTable[wc - L'A'];
      if (nAlt != 255)
        pPattern->SetAt(i, wsAltSymbols[nAlt]);
    }
    i++;
    bEscape = false;
  }
}

std::pair<bool, uint32_t> PatternStringType(ByteStringView szPattern) {
  WideString wsPattern = WideString::FromUTF8(szPattern);
  if (L"datetime" == wsPattern.Left(8))
    return {true, XFA_VT_DATETIME};
  if (L"date" == wsPattern.Left(4)) {
    auto pos = wsPattern.Find(L"time");
    uint32_t type =
        pos.has_value() && pos.value() != 0 ? XFA_VT_DATETIME : XFA_VT_DATE;
    return {true, type};
  }
  if (L"time" == wsPattern.Left(4))
    return {true, XFA_VT_TIME};
  if (L"text" == wsPattern.Left(4))
    return {true, XFA_VT_TEXT};
  if (L"num" == wsPattern.Left(3)) {
    uint32_t type;
    if (L"integer" == wsPattern.Mid(4, 7)) {
      type = XFA_VT_INTEGER;
    } else if (L"decimal" == wsPattern.Mid(4, 7)) {
      type = XFA_VT_DECIMAL;
    } else if (L"currency" == wsPattern.Mid(4, 8)) {
      type = XFA_VT_FLOAT;
    } else if (L"percent" == wsPattern.Mid(4, 7)) {
      type = XFA_VT_FLOAT;
    } else {
      type = XFA_VT_FLOAT;
    }
    return {true, type};
  }

  uint32_t type = XFA_VT_NULL;
  wsPattern.MakeLower();
  const wchar_t* pData = wsPattern.c_str();
  int32_t iLength = wsPattern.GetLength();
  int32_t iIndex = 0;
  bool bSingleQuotation = false;
  while (iIndex < iLength) {
    wchar_t wsPatternChar = pData[iIndex];
    if (wsPatternChar == 0x27) {
      bSingleQuotation = !bSingleQuotation;
      iIndex++;
      continue;
    }
    if (bSingleQuotation) {
      iIndex++;
      continue;
    }

    if (wsPatternChar == 'h' || wsPatternChar == 'k')
      return {false, XFA_VT_TIME};
    if (wsPatternChar == 'x' || wsPatternChar == 'o' || wsPatternChar == '0')
      return {false, XFA_VT_TEXT};
    if (wsPatternChar == 'v' || wsPatternChar == '8' || wsPatternChar == '$')
      return {false, XFA_VT_FLOAT};
    if (wsPatternChar == 'y' || wsPatternChar == 'j') {
      iIndex++;
      wchar_t timePatternChar;
      while (iIndex < iLength) {
        timePatternChar = pData[iIndex];
        if (timePatternChar == 0x27) {
          bSingleQuotation = !bSingleQuotation;
          iIndex++;
          continue;
        }
        if (!bSingleQuotation && timePatternChar == 't')
          return {false, XFA_VT_DATETIME};
        iIndex++;
      }
      return {false, XFA_VT_DATE};
    }

    if (wsPatternChar == 'a') {
      type = XFA_VT_TEXT;
    } else if (wsPatternChar == 'z' || wsPatternChar == 's' ||
               wsPatternChar == 'e' || wsPatternChar == ',' ||
               wsPatternChar == '.') {
      type = XFA_VT_FLOAT;
    }
    iIndex++;
  }

  if (type == XFA_VT_NULL)
    type = XFA_VT_TEXT | XFA_VT_FLOAT;
  return {false, type};
}

CFXJSE_FormCalcContext* ToFormCalcContext(CFXJSE_Value* pValue) {
  CFXJSE_HostObject* pHostObj = pValue->ToHostObject();
  return pHostObj ? pHostObj->AsFormCalcContext() : nullptr;
}

LocaleIface* LocaleFromString(CXFA_Document* pDoc,
                              CXFA_LocaleMgr* pMgr,
                              ByteStringView szLocale) {
  if (!szLocale.IsEmpty())
    return pMgr->GetLocaleByName(WideString::FromUTF8(szLocale));

  CXFA_Node* pThisNode = ToNode(pDoc->GetScriptContext()->GetThisObject());
  ASSERT(pThisNode);
  return pThisNode->GetLocale();
}

WideString FormatFromString(LocaleIface* pLocale, ByteStringView szFormat) {
  if (!szFormat.IsEmpty())
    return WideString::FromUTF8(szFormat);

  return pLocale->GetDatePattern(FX_LOCALEDATETIMESUBCATEGORY_Default);
}

FX_LOCALEDATETIMESUBCATEGORY SubCategoryFromInt(int32_t iStyle) {
  switch (iStyle) {
    case 1:
      return FX_LOCALEDATETIMESUBCATEGORY_Short;
    case 3:
      return FX_LOCALEDATETIMESUBCATEGORY_Long;
    case 4:
      return FX_LOCALEDATETIMESUBCATEGORY_Full;
    case 0:
    case 2:
    default:
      return FX_LOCALEDATETIMESUBCATEGORY_Medium;
  }
}

ByteString GetLocalDateTimeFormat(CXFA_Document* pDoc,
                                  int32_t iStyle,
                                  ByteStringView szLocale,
                                  bool bStandard,
                                  bool bIsDate) {
  CXFA_LocaleMgr* pMgr = pDoc->GetLocaleMgr();
  LocaleIface* pLocale = LocaleFromString(pDoc, pMgr, szLocale);
  if (!pLocale)
    return ByteString();

  FX_LOCALEDATETIMESUBCATEGORY category = SubCategoryFromInt(iStyle);
  WideString strRet = bIsDate ? pLocale->GetDatePattern(category)
                              : pLocale->GetTimePattern(category);
  if (!bStandard)
    AlternateDateTimeSymbols(&strRet, pLocale->GetDateTimeSymbols(), bIsDate);
  return strRet.ToUTF8();
}

bool IsWhitespace(char c) {
  return c == 0x20 || c == 0x09 || c == 0x0B || c == 0x0C || c == 0x0A ||
         c == 0x0D;
}

bool IsPartOfNumber(char ch) {
  return std::isdigit(ch) || ch == '-' || ch == '.';
}

bool IsPartOfNumberW(wchar_t ch) {
  return FXSYS_IsDecimalDigit(ch) || ch == L'-' || ch == L'.';
}

ByteString GUIDString(bool bSeparator) {
  uint8_t data[16];
  FX_Random_GenerateMT(reinterpret_cast<uint32_t*>(data), 4);
  data[6] = (data[6] & 0x0F) | 0x40;

  ByteString bsStr;
  {
    // Span's lifetime must end before ReleaseBuffer() below.
    pdfium::span<char> pBuf = bsStr.GetBuffer(40);
    size_t out_index = 0;
    for (size_t i = 0; i < 16; ++i, out_index += 2) {
      if (bSeparator && (i == 4 || i == 6 || i == 8 || i == 10))
        pBuf[out_index++] = L'-';

      FXSYS_IntToTwoHexChars(data[i], &pBuf[out_index]);
    }
  }
  bsStr.ReleaseBuffer(bSeparator ? 36 : 32);
  return bsStr;
}

bool IsIsoDateFormat(const char* pData,
                     int32_t iLength,
                     int32_t* pStyle,
                     int32_t* pYear,
                     int32_t* pMonth,
                     int32_t* pDay) {
  int32_t& iStyle = *pStyle;
  int32_t& iYear = *pYear;
  int32_t& iMonth = *pMonth;
  int32_t& iDay = *pDay;

  iYear = 0;
  iMonth = 1;
  iDay = 1;

  if (iLength < 4)
    return false;

  char strYear[5];
  strYear[4] = '\0';
  for (int32_t i = 0; i < 4; ++i) {
    if (!std::isdigit(pData[i]))
      return false;

    strYear[i] = pData[i];
  }
  iYear = FXSYS_atoi(strYear);
  iStyle = 0;
  if (iLength == 4)
    return true;

  iStyle = pData[4] == '-' ? 1 : 0;

  char strTemp[3];
  strTemp[2] = '\0';
  int32_t iPosOff = iStyle == 0 ? 4 : 5;
  if (!std::isdigit(pData[iPosOff]) || !std::isdigit(pData[iPosOff + 1]))
    return false;

  strTemp[0] = pData[iPosOff];
  strTemp[1] = pData[iPosOff + 1];
  iMonth = FXSYS_atoi(strTemp);
  if (iMonth > 12 || iMonth < 1)
    return false;

  if (iStyle == 0) {
    iPosOff += 2;
    if (iLength == 6)
      return true;
  } else {
    iPosOff += 3;
    if (iLength == 7)
      return true;
  }
  if (!std::isdigit(pData[iPosOff]) || !std::isdigit(pData[iPosOff + 1]))
    return false;

  strTemp[0] = pData[iPosOff];
  strTemp[1] = pData[iPosOff + 1];
  iDay = FXSYS_atoi(strTemp);
  if (iPosOff + 2 < iLength)
    return false;

  if (iMonth == 2) {
    bool bIsLeap = (!(iYear % 4) && (iYear % 100)) || !(iYear % 400);
    return iDay <= (bIsLeap ? 29 : 28);
  }

  if (iMonth < 8)
    return iDay <= (iMonth % 2 == 0 ? 30 : 31);
  return iDay <= (iMonth % 2 == 0 ? 31 : 30);
}

bool IsIsoTimeFormat(const char* pData,
                     int32_t iLength,
                     int32_t* pHour,
                     int32_t* pMinute,
                     int32_t* pSecond,
                     int32_t* pMilliSecond,
                     int32_t* pZoneHour,
                     int32_t* pZoneMinute) {
  int32_t& iHour = *pHour;
  int32_t& iMinute = *pMinute;
  int32_t& iSecond = *pSecond;
  int32_t& iMilliSecond = *pMilliSecond;
  int32_t& iZoneHour = *pZoneHour;
  int32_t& iZoneMinute = *pZoneMinute;

  iHour = 0;
  iMinute = 0;
  iSecond = 0;
  iMilliSecond = 0;
  iZoneHour = 0;
  iZoneMinute = 0;
  if (!pData)
    return false;

  char strTemp[3];
  strTemp[2] = '\0';
  int32_t iZone = 0;
  int32_t i = 0;
  while (i < iLength) {
    if (!std::isdigit(pData[i]) && pData[i] != ':') {
      iZone = i;
      break;
    }
    ++i;
  }
  if (i == iLength)
    iZone = iLength;

  int32_t iPos = 0;
  int32_t iIndex = 0;
  while (iIndex < iZone) {
    if (!std::isdigit(pData[iIndex]))
      return false;

    strTemp[0] = pData[iIndex];
    if (!std::isdigit(pData[iIndex + 1]))
      return false;

    strTemp[1] = pData[iIndex + 1];
    if (FXSYS_atoi(strTemp) > 60)
      return false;

    if (pData[2] == ':') {
      if (iPos == 0) {
        iHour = FXSYS_atoi(strTemp);
        ++iPos;
      } else if (iPos == 1) {
        iMinute = FXSYS_atoi(strTemp);
        ++iPos;
      } else {
        iSecond = FXSYS_atoi(strTemp);
      }
      iIndex += 3;
    } else {
      if (iPos == 0) {
        iHour = FXSYS_atoi(strTemp);
        ++iPos;
      } else if (iPos == 1) {
        iMinute = FXSYS_atoi(strTemp);
        ++iPos;
      } else if (iPos == 2) {
        iSecond = FXSYS_atoi(strTemp);
        ++iPos;
      }
      iIndex += 2;
    }
  }

  if (iIndex < iLength && pData[iIndex] == '.') {
    constexpr int kSubSecondLength = 3;
    if (iIndex + kSubSecondLength >= iLength)
      return false;

    ++iIndex;
    char strSec[kSubSecondLength + 1];
    for (int j = 0; j < kSubSecondLength; ++j) {
      char c = pData[iIndex + j];
      if (!std::isdigit(c))
        return false;
      strSec[j] = c;
    }
    strSec[kSubSecondLength] = '\0';

    iMilliSecond = FXSYS_atoi(strSec);
    if (iMilliSecond > 100) {
      iMilliSecond = 0;
      return false;
    }
    iIndex += kSubSecondLength;
  }

  if (iIndex < iLength && FXSYS_towlower(pData[iIndex]) == 'z')
    return true;

  int32_t iSign = 1;
  if (iIndex < iLength) {
    if (pData[iIndex] == '+') {
      ++iIndex;
    } else if (pData[iIndex] == '-') {
      iSign = -1;
      ++iIndex;
    }
  }
  iPos = 0;
  while (iIndex < iLength) {
    if (!std::isdigit(pData[iIndex]))
      return false;

    strTemp[0] = pData[iIndex];
    if (!std::isdigit(pData[iIndex + 1]))
      return false;

    strTemp[1] = pData[iIndex + 1];
    if (FXSYS_atoi(strTemp) > 60)
      return false;

    if (pData[2] == ':') {
      if (iPos == 0) {
        iZoneHour = FXSYS_atoi(strTemp);
      } else if (iPos == 1) {
        iZoneMinute = FXSYS_atoi(strTemp);
      }
      iIndex += 3;
    } else {
      if (!iPos) {
        iZoneHour = FXSYS_atoi(strTemp);
        ++iPos;
      } else if (iPos == 1) {
        iZoneMinute = FXSYS_atoi(strTemp);
        ++iPos;
      }
      iIndex += 2;
    }
  }
  if (iIndex < iLength)
    return false;

  iZoneHour *= iSign;
  return true;
}

bool IsIsoDateTimeFormat(const char* pData,
                         int32_t iLength,
                         int32_t* pYear,
                         int32_t* pMonth,
                         int32_t* pDay,
                         int32_t* pHour,
                         int32_t* pMinute,
                         int32_t* pSecond,
                         int32_t* pMilliSecond,
                         int32_t* pZoneHour,
                         int32_t* pZoneMinute) {
  int32_t& iYear = *pYear;
  int32_t& iMonth = *pMonth;
  int32_t& iDay = *pDay;
  int32_t& iHour = *pHour;
  int32_t& iMinute = *pMinute;
  int32_t& iSecond = *pSecond;
  int32_t& iMilliSecond = *pMilliSecond;
  int32_t& iZoneHour = *pZoneHour;
  int32_t& iZoneMinute = *pZoneMinute;

  iYear = 0;
  iMonth = 0;
  iDay = 0;
  iHour = 0;
  iMinute = 0;
  iSecond = 0;
  if (!pData)
    return false;

  int32_t iIndex = 0;
  while (pData[iIndex] != 'T' && pData[iIndex] != 't') {
    if (iIndex >= iLength)
      return false;
    ++iIndex;
  }
  if (iIndex != 8 && iIndex != 10)
    return false;

  int32_t iStyle = -1;
  if (!IsIsoDateFormat(pData, iIndex, &iStyle, &iYear, &iMonth, &iDay))
    return false;
  if (pData[iIndex] != 'T' && pData[iIndex] != 't')
    return true;

  ++iIndex;
  return IsIsoTimeFormat(pData + iIndex, iLength - iIndex, &iHour, &iMinute,
                         &iSecond, &iMilliSecond, &iZoneHour, &iZoneMinute);
}

int32_t DateString2Num(ByteStringView szDateString) {
  int32_t iLength = szDateString.GetLength();
  int32_t iYear = 0;
  int32_t iMonth = 0;
  int32_t iDay = 0;
  if (iLength <= 10) {
    int32_t iStyle = -1;
    if (!IsIsoDateFormat(szDateString.unterminated_c_str(), iLength, &iStyle,
                         &iYear, &iMonth, &iDay)) {
      return 0;
    }
  } else {
    int32_t iHour = 0;
    int32_t iMinute = 0;
    int32_t iSecond = 0;
    int32_t iMilliSecond = 0;
    int32_t iZoneHour = 0;
    int32_t iZoneMinute = 0;
    if (!IsIsoDateTimeFormat(szDateString.unterminated_c_str(), iLength, &iYear,
                             &iMonth, &iDay, &iHour, &iMinute, &iSecond,
                             &iMilliSecond, &iZoneHour, &iZoneMinute)) {
      return 0;
    }
  }

  float dDays = 0;
  int32_t i = 1;
  if (iYear < 1900)
    return 0;

  while (iYear - i >= 1900) {
    dDays +=
        ((!((iYear - i) % 4) && ((iYear - i) % 100)) || !((iYear - i) % 400))
            ? 366
            : 365;
    ++i;
  }
  i = 1;
  while (i < iMonth) {
    if (i == 2)
      dDays += ((!(iYear % 4) && (iYear % 100)) || !(iYear % 400)) ? 29 : 28;
    else if (i <= 7)
      dDays += (i % 2 == 0) ? 30 : 31;
    else
      dDays += (i % 2 == 0) ? 31 : 30;

    ++i;
  }
  i = 0;
  while (iDay - i > 0) {
    dDays += 1;
    ++i;
  }
  return (int32_t)dDays;
}

void GetLocalTimeZone(int32_t* pHour, int32_t* pMin, int32_t* pSec) {
  time_t now;
  FXSYS_time(&now);

  struct tm* pGmt = gmtime(&now);
  struct tm* pLocal = FXSYS_localtime(&now);
  *pHour = pLocal->tm_hour - pGmt->tm_hour;
  *pMin = pLocal->tm_min - pGmt->tm_min;
  *pSec = pLocal->tm_sec - pGmt->tm_sec;
}

bool HTMLSTR2Code(const WideString& pData, uint32_t* iCode) {
  auto cmpFunc = [](const XFA_FMHtmlReserveCode& iter, ByteStringView val) {
    return strcmp(val.unterminated_c_str(), iter.m_htmlReserve) > 0;
  };
  if (!pData.IsASCII())
    return false;
  ByteString temp = pData.ToASCII();
  const XFA_FMHtmlReserveCode* result = std::lower_bound(
      std::begin(kReservesForDecode), std::end(kReservesForDecode),
      temp.AsStringView(), cmpFunc);
  if (result != std::end(kReservesForEncode) &&
      !strcmp(temp.c_str(), result->m_htmlReserve)) {
    *iCode = result->m_uCode;
    return true;
  }
  return false;
}

bool HTMLCode2STR(uint32_t iCode, WideString* wsHTMLReserve) {
  auto cmpFunc = [](const XFA_FMHtmlReserveCode iter, const uint32_t& val) {
    return iter.m_uCode < val;
  };
  const XFA_FMHtmlReserveCode* result =
      std::lower_bound(std::begin(kReservesForEncode),
                       std::end(kReservesForEncode), iCode, cmpFunc);
  if (result != std::end(kReservesForEncode) && result->m_uCode == iCode) {
    *wsHTMLReserve = WideString::FromASCII(result->m_htmlReserve);
    return true;
  }
  return false;
}

WideString DecodeURL(const WideString& wsURLString) {
  const wchar_t* pData = wsURLString.c_str();
  size_t iLen = wsURLString.GetLength();
  CFX_WideTextBuf wsResultBuf;
  for (size_t i = 0; i < iLen; ++i) {
    wchar_t ch = pData[i];
    if ('%' != ch) {
      wsResultBuf.AppendChar(ch);
      continue;
    }

    wchar_t chTemp = 0;
    int32_t iCount = 0;
    while (iCount < 2) {
      if (++i >= iLen)
        break;
      chTemp *= 16;
      ch = pData[i];
      if (!FXSYS_IsWideHexDigit(ch))
        return WideString();
      chTemp += FXSYS_WideHexCharToInt(ch);
      ++iCount;
    }
    wsResultBuf.AppendChar(chTemp);
  }
  wsResultBuf.AppendChar(0);
  return wsResultBuf.MakeString();
}

WideString DecodeMLInternal(const WideString& wsHTMLString, bool bIsHTML) {
  const wchar_t* pData = wsHTMLString.c_str();
  size_t iLen = wsHTMLString.GetLength();
  CFX_WideTextBuf wsResultBuf;
  for (size_t i = 0; i < iLen; ++i) {
    wchar_t ch = pData[i];
    if (ch != '&') {
      wsResultBuf.AppendChar(ch);
      continue;
    }

    if (++i >= iLen)
      break;
    ch = pData[i];
    if (ch == '#') {
      if (++i >= iLen)
        break;
      ch = pData[i];
      if (ch != 'x' && ch != 'X')
        return WideString();
      if (++i >= iLen)
        break;
      ch = pData[i];
      uint32_t iCode = 0;
      while (ch != ';' && i < iLen) {
        iCode *= 16;
        if (!FXSYS_IsWideHexDigit(ch))
          return WideString();
        iCode += FXSYS_WideHexCharToInt(ch);
        if (++i >= iLen)
          break;
        ch = pData[i];
      }
      wsResultBuf.AppendChar(iCode);
      continue;
    }

    wchar_t strString[9];
    size_t iStrIndex = 0;
    while (ch != ';' && i < iLen) {
      if (iStrIndex < 8)
        strString[iStrIndex++] = ch;
      if (++i >= iLen)
        break;
      ch = pData[i];
    }
    strString[iStrIndex] = 0;
    if (bIsHTML) {
      uint32_t iData = 0;
      if (HTMLSTR2Code(strString, &iData))
        wsResultBuf.AppendChar((wchar_t)iData);
    } else {
      if (wcscmp(strString, L"quot") == 0)
        wsResultBuf.AppendChar('"');
      else if (wcscmp(strString, L"amp") == 0)
        wsResultBuf.AppendChar('&');
      else if (wcscmp(strString, L"apos") == 0)
        wsResultBuf.AppendChar('\'');
      else if (wcscmp(strString, L"lt") == 0)
        wsResultBuf.AppendChar('<');
      else if (wcscmp(strString, L"gt") == 0)
        wsResultBuf.AppendChar('>');
    }
  }

  wsResultBuf.AppendChar(0);
  return wsResultBuf.MakeString();
}

WideString DecodeHTML(const WideString& wsHTMLString) {
  return DecodeMLInternal(wsHTMLString, true);
}

WideString DecodeXML(const WideString& wsXMLString) {
  return DecodeMLInternal(wsXMLString, false);
}

WideString EncodeURL(const ByteString& szURLString) {
  static const wchar_t kStrUnsafe[] = {' ', '<',  '>', '"', '#', '%', '{', '}',
                                       '|', '\\', '^', '~', '[', ']', '`'};
  static const wchar_t kStrReserved[] = {';', '/', '?', ':', '@', '=', '&'};
  static const wchar_t kStrSpecial[] = {'$',  '-', '+', '!', '*',
                                        '\'', '(', ')', ','};

  WideString wsURLString = WideString::FromUTF8(szURLString.AsStringView());
  CFX_WideTextBuf wsResultBuf;
  wchar_t strEncode[4];
  strEncode[0] = '%';
  strEncode[3] = 0;
  for (wchar_t ch : wsURLString) {
    size_t i = 0;
    size_t iCount = FX_ArraySize(kStrUnsafe);
    while (i < iCount) {
      if (ch == kStrUnsafe[i]) {
        int32_t iIndex = ch / 16;
        strEncode[1] = kStrCode[iIndex];
        strEncode[2] = kStrCode[ch - iIndex * 16];
        wsResultBuf << strEncode;
        break;
      }
      ++i;
    }
    if (i < iCount)
      continue;

    i = 0;
    iCount = FX_ArraySize(kStrReserved);
    while (i < iCount) {
      if (ch == kStrReserved[i]) {
        int32_t iIndex = ch / 16;
        strEncode[1] = kStrCode[iIndex];
        strEncode[2] = kStrCode[ch - iIndex * 16];
        wsResultBuf << strEncode;
        break;
      }
      ++i;
    }
    if (i < iCount)
      continue;

    i = 0;
    iCount = FX_ArraySize(kStrSpecial);
    while (i < iCount) {
      if (ch == kStrSpecial[i]) {
        wsResultBuf.AppendChar(ch);
        break;
      }
      ++i;
    }
    if (i < iCount)
      continue;

    if ((ch >= 0x80 && ch <= 0xff) || ch <= 0x1f || ch == 0x7f) {
      int32_t iIndex = ch / 16;
      strEncode[1] = kStrCode[iIndex];
      strEncode[2] = kStrCode[ch - iIndex * 16];
      wsResultBuf << strEncode;
    } else if (ch >= 0x20 && ch <= 0x7e) {
      wsResultBuf.AppendChar(ch);
    } else {
      const wchar_t iRadix = 16;
      WideString strTmp;
      while (ch >= iRadix) {
        wchar_t tmp = kStrCode[ch % iRadix];
        ch /= iRadix;
        strTmp += tmp;
      }
      strTmp += kStrCode[ch];
      int32_t iLen = strTmp.GetLength();
      if (iLen < 2)
        break;

      int32_t iIndex = 0;
      if (iLen % 2 != 0) {
        strEncode[1] = '0';
        strEncode[2] = strTmp[iLen - 1];
        iIndex = iLen - 2;
      } else {
        strEncode[1] = strTmp[iLen - 1];
        strEncode[2] = strTmp[iLen - 2];
        iIndex = iLen - 3;
      }
      wsResultBuf << strEncode;
      while (iIndex > 0) {
        strEncode[1] = strTmp[iIndex];
        strEncode[2] = strTmp[iIndex - 1];
        iIndex -= 2;
        wsResultBuf << strEncode;
      }
    }
  }
  wsResultBuf.AppendChar(0);
  return wsResultBuf.MakeString();
}

WideString EncodeHTML(const ByteString& szHTMLString) {
  WideString wsHTMLString = WideString::FromUTF8(szHTMLString.AsStringView());
  wchar_t strEncode[9];
  strEncode[0] = '&';
  strEncode[1] = '#';
  strEncode[2] = 'x';
  strEncode[5] = ';';
  strEncode[6] = 0;
  strEncode[7] = ';';
  strEncode[8] = 0;
  CFX_WideTextBuf wsResultBuf;
  int32_t iLen = wsHTMLString.GetLength();
  int32_t i = 0;
  const wchar_t* pData = wsHTMLString.c_str();
  while (i < iLen) {
    uint32_t ch = pData[i];
    WideString htmlReserve;
    if (HTMLCode2STR(ch, &htmlReserve)) {
      wsResultBuf.AppendChar(L'&');
      wsResultBuf << htmlReserve;
      wsResultBuf.AppendChar(L';');
    } else if (ch >= 32 && ch <= 126) {
      wsResultBuf.AppendChar((wchar_t)ch);
    } else if (ch < 256) {
      int32_t iIndex = ch / 16;
      strEncode[3] = kStrCode[iIndex];
      strEncode[4] = kStrCode[ch - iIndex * 16];
      strEncode[5] = ';';
      strEncode[6] = 0;
      wsResultBuf << strEncode;
    } else {
      int32_t iBigByte = ch / 256;
      int32_t iLittleByte = ch % 256;
      strEncode[3] = kStrCode[iBigByte / 16];
      strEncode[4] = kStrCode[iBigByte % 16];
      strEncode[5] = kStrCode[iLittleByte / 16];
      strEncode[6] = kStrCode[iLittleByte % 16];
      wsResultBuf << strEncode;
    }
    ++i;
  }
  wsResultBuf.AppendChar(0);
  return wsResultBuf.MakeString();
}

WideString EncodeXML(const ByteString& szXMLString) {
  WideString wsXMLString = WideString::FromUTF8(szXMLString.AsStringView());
  CFX_WideTextBuf wsResultBuf;
  wchar_t strEncode[9];
  strEncode[0] = '&';
  strEncode[1] = '#';
  strEncode[2] = 'x';
  strEncode[5] = ';';
  strEncode[6] = 0;
  strEncode[7] = ';';
  strEncode[8] = 0;
  for (wchar_t ch : wsXMLString) {
    switch (ch) {
      case '"':
        wsResultBuf.AppendChar('&');
        wsResultBuf << WideStringView(L"quot");
        wsResultBuf.AppendChar(';');
        break;
      case '&':
        wsResultBuf.AppendChar('&');
        wsResultBuf << WideStringView(L"amp");
        wsResultBuf.AppendChar(';');
        break;
      case '\'':
        wsResultBuf.AppendChar('&');
        wsResultBuf << WideStringView(L"apos");
        wsResultBuf.AppendChar(';');
        break;
      case '<':
        wsResultBuf.AppendChar('&');
        wsResultBuf << WideStringView(L"lt");
        wsResultBuf.AppendChar(';');
        break;
      case '>':
        wsResultBuf.AppendChar('&');
        wsResultBuf << WideStringView(L"gt");
        wsResultBuf.AppendChar(';');
        break;
      default: {
        if (ch >= 32 && ch <= 126) {
          wsResultBuf.AppendChar(ch);
        } else if (ch < 256) {
          int32_t iIndex = ch / 16;
          strEncode[3] = kStrCode[iIndex];
          strEncode[4] = kStrCode[ch - iIndex * 16];
          strEncode[5] = ';';
          strEncode[6] = 0;
          wsResultBuf << strEncode;
        } else {
          int32_t iBigByte = ch / 256;
          int32_t iLittleByte = ch % 256;
          strEncode[3] = kStrCode[iBigByte / 16];
          strEncode[4] = kStrCode[iBigByte % 16];
          strEncode[5] = kStrCode[iLittleByte / 16];
          strEncode[6] = kStrCode[iLittleByte % 16];
          wsResultBuf << strEncode;
        }
        break;
      }
    }
  }
  wsResultBuf.AppendChar(0);
  return wsResultBuf.MakeString();
}

ByteString TrillionUS(ByteStringView szData) {
  static const ByteStringView pUnits[] = {"zero",  "one",  "two", "three",
                                          "four",  "five", "six", "seven",
                                          "eight", "nine"};
  static const ByteStringView pCapUnits[] = {"Zero",  "One",  "Two", "Three",
                                             "Four",  "Five", "Six", "Seven",
                                             "Eight", "Nine"};
  static const ByteStringView pTens[] = {
      "Ten",     "Eleven",  "Twelve",    "Thirteen", "Fourteen",
      "Fifteen", "Sixteen", "Seventeen", "Eighteen", "Nineteen"};
  static const ByteStringView pLastTens[] = {"Twenty", "Thirty", "Forty",
                                             "Fifty",  "Sixty",  "Seventy",
                                             "Eighty", "Ninety"};
  static const ByteStringView pComm[] = {" Hundred ", " Thousand ", " Million ",
                                         " Billion ", "Trillion"};
  const char* pData = szData.unterminated_c_str();
  int32_t iLength = szData.GetLength();
  int32_t iComm = 0;
  if (iLength > 12)
    iComm = 4;
  else if (iLength > 9)
    iComm = 3;
  else if (iLength > 6)
    iComm = 2;
  else if (iLength > 3)
    iComm = 1;

  int32_t iFirstCount = iLength % 3;
  if (iFirstCount == 0)
    iFirstCount = 3;

  std::ostringstream strBuf;
  int32_t iIndex = 0;
  if (iFirstCount == 3) {
    if (pData[iIndex] != '0') {
      strBuf << pCapUnits[pData[iIndex] - '0'];
      strBuf << pComm[0];
    }
    if (pData[iIndex + 1] == '0') {
      strBuf << pCapUnits[pData[iIndex + 2] - '0'];
    } else {
      if (pData[iIndex + 1] > '1') {
        strBuf << pLastTens[pData[iIndex + 1] - '2'];
        strBuf << "-";
        strBuf << pUnits[pData[iIndex + 2] - '0'];
      } else if (pData[iIndex + 1] == '1') {
        strBuf << pTens[pData[iIndex + 2] - '0'];
      } else if (pData[iIndex + 1] == '0') {
        strBuf << pCapUnits[pData[iIndex + 2] - '0'];
      }
    }
    iIndex += 3;
  } else if (iFirstCount == 2) {
    if (pData[iIndex] == '0') {
      strBuf << pCapUnits[pData[iIndex + 1] - '0'];
    } else {
      if (pData[iIndex] > '1') {
        strBuf << pLastTens[pData[iIndex] - '2'];
        strBuf << "-";
        strBuf << pUnits[pData[iIndex + 1] - '0'];
      } else if (pData[iIndex] == '1') {
        strBuf << pTens[pData[iIndex + 1] - '0'];
      } else if (pData[iIndex] == '0') {
        strBuf << pCapUnits[pData[iIndex + 1] - '0'];
      }
    }
    iIndex += 2;
  } else if (iFirstCount == 1) {
    strBuf << pCapUnits[pData[iIndex] - '0'];
    iIndex += 1;
  }
  if (iLength > 3 && iFirstCount > 0) {
    strBuf << pComm[iComm];
    --iComm;
  }
  while (iIndex < iLength) {
    if (pData[iIndex] != '0') {
      strBuf << pCapUnits[pData[iIndex] - '0'];
      strBuf << pComm[0];
    }
    if (pData[iIndex + 1] == '0') {
      strBuf << pCapUnits[pData[iIndex + 2] - '0'];
    } else {
      if (pData[iIndex + 1] > '1') {
        strBuf << pLastTens[pData[iIndex + 1] - '2'];
        strBuf << "-";
        strBuf << pUnits[pData[iIndex + 2] - '0'];
      } else if (pData[iIndex + 1] == '1') {
        strBuf << pTens[pData[iIndex + 2] - '0'];
      } else if (pData[iIndex + 1] == '0') {
        strBuf << pCapUnits[pData[iIndex + 2] - '0'];
      }
    }
    if (iIndex < iLength - 3) {
      strBuf << pComm[iComm];
      --iComm;
    }
    iIndex += 3;
  }
  return ByteString(strBuf);
}

ByteString WordUS(const ByteString& szData, int32_t iStyle) {
  const char* pData = szData.c_str();
  int32_t iLength = szData.GetLength();
  if (iStyle < 0 || iStyle > 2) {
    return ByteString();
  }

  std::ostringstream strBuf;

  int32_t iIndex = 0;
  while (iIndex < iLength) {
    if (pData[iIndex] == '.')
      break;
    ++iIndex;
  }
  int32_t iInteger = iIndex;
  iIndex = 0;
  while (iIndex < iInteger) {
    int32_t iCount = (iInteger - iIndex) % 12;
    if (!iCount && iInteger - iIndex > 0)
      iCount = 12;

    strBuf << TrillionUS(ByteStringView(pData + iIndex, iCount));
    iIndex += iCount;
    if (iIndex < iInteger)
      strBuf << " Trillion ";
  }

  if (iStyle > 0)
    strBuf << " Dollars";

  if (iStyle > 1 && iInteger < iLength) {
    strBuf << " And ";
    iIndex = iInteger + 1;
    while (iIndex < iLength) {
      int32_t iCount = (iLength - iIndex) % 12;
      if (!iCount && iLength - iIndex > 0)
        iCount = 12;

      strBuf << TrillionUS(ByteStringView(pData + iIndex, iCount));
      iIndex += iCount;
      if (iIndex < iLength)
        strBuf << " Trillion ";
    }
    strBuf << " Cents";
  }
  return ByteString(strBuf);
}

}  // namespace

const FXJSE_CLASS_DESCRIPTOR kFormCalcFM2JSDescriptor = {
    kClassTag,                              // tag
    "XFA_FM2JS_FormCalcClass",              // name
    kFormCalcFM2JSFunctions,                // methods
    FX_ArraySize(kFormCalcFM2JSFunctions),  // number of methods
    nullptr,                                // dynamic prop type
    nullptr,                                // dynamic prop getter
    nullptr,                                // dynamic prop setter
    nullptr,                                // dynamic prop method call
};

// static
void CFXJSE_FormCalcContext::Abs(CFXJSE_Value* pThis,
                                 ByteStringView szFuncName,
                                 CFXJSE_Arguments& args) {
  if (args.GetLength() != 1) {
    ToFormCalcContext(pThis)->ThrowParamCountMismatchException(L"Abs");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = args.GetValue(0);
  if (ValueIsNull(pThis, argOne.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  double dValue = ValueToDouble(pThis, argOne.get());
  if (dValue < 0)
    dValue = -dValue;

  args.GetReturnValue()->SetDouble(dValue);
}

// static
void CFXJSE_FormCalcContext::Avg(CFXJSE_Value* pThis,
                                 ByteStringView szFuncName,
                                 CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc < 1) {
    args.GetReturnValue()->SetNull();
    return;
  }

  v8::Isolate* pIsolate = ToFormCalcContext(pThis)->GetScriptRuntime();
  uint32_t uCount = 0;
  double dSum = 0.0;
  for (int32_t i = 0; i < argc; i++) {
    std::unique_ptr<CFXJSE_Value> argValue = args.GetValue(i);
    if (argValue->IsNull())
      continue;

    if (!argValue->IsArray()) {
      dSum += ValueToDouble(pThis, argValue.get());
      uCount++;
      continue;
    }

    auto lengthValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
    argValue->GetObjectProperty("length", lengthValue.get());
    int32_t iLength = lengthValue->ToInteger();

    if (iLength > 2) {
      auto propertyValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
      argValue->GetObjectPropertyByIdx(1, propertyValue.get());

      auto jsObjectValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
      if (propertyValue->IsNull()) {
        for (int32_t j = 2; j < iLength; j++) {
          argValue->GetObjectPropertyByIdx(j, jsObjectValue.get());
          auto defaultPropValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
          GetObjectDefaultValue(jsObjectValue.get(), defaultPropValue.get());
          if (defaultPropValue->IsNull())
            continue;

          dSum += ValueToDouble(pThis, defaultPropValue.get());
          uCount++;
        }
      } else {
        for (int32_t j = 2; j < iLength; j++) {
          argValue->GetObjectPropertyByIdx(j, jsObjectValue.get());
          auto newPropertyValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
          jsObjectValue->GetObjectProperty(
              propertyValue->ToString().AsStringView(), newPropertyValue.get());
          if (newPropertyValue->IsNull())
            continue;

          dSum += ValueToDouble(pThis, newPropertyValue.get());
          uCount++;
        }
      }
    }
  }
  if (uCount == 0) {
    args.GetReturnValue()->SetNull();
    return;
  }

  args.GetReturnValue()->SetDouble(dSum / uCount);
}

// static
void CFXJSE_FormCalcContext::Ceil(CFXJSE_Value* pThis,
                                  ByteStringView szFuncName,
                                  CFXJSE_Arguments& args) {
  if (args.GetLength() != 1) {
    ToFormCalcContext(pThis)->ThrowParamCountMismatchException(L"Ceil");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argValue = GetSimpleValue(pThis, args, 0);
  if (ValueIsNull(pThis, argValue.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  args.GetReturnValue()->SetFloat(ceil(ValueToFloat(pThis, argValue.get())));
}

// static
void CFXJSE_FormCalcContext::Count(CFXJSE_Value* pThis,
                                   ByteStringView szFuncName,
                                   CFXJSE_Arguments& args) {
  CFXJSE_FormCalcContext* pContext = ToFormCalcContext(pThis);
  v8::Isolate* pIsolate = pContext->GetScriptRuntime();
  int32_t iCount = 0;
  for (int32_t i = 0; i < args.GetLength(); i++) {
    std::unique_ptr<CFXJSE_Value> argValue = args.GetValue(i);
    if (argValue->IsNull())
      continue;

    if (argValue->IsArray()) {
      auto lengthValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
      argValue->GetObjectProperty("length", lengthValue.get());

      int32_t iLength = lengthValue->ToInteger();
      if (iLength <= 2) {
        pContext->ThrowArgumentMismatchException();
        return;
      }

      auto propertyValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
      auto jsObjectValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
      auto newPropertyValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
      argValue->GetObjectPropertyByIdx(1, propertyValue.get());
      argValue->GetObjectPropertyByIdx(2, jsObjectValue.get());
      if (propertyValue->IsNull()) {
        for (int32_t j = 2; j < iLength; j++) {
          argValue->GetObjectPropertyByIdx(j, jsObjectValue.get());
          GetObjectDefaultValue(jsObjectValue.get(), newPropertyValue.get());
          if (!newPropertyValue->IsNull())
            iCount++;
        }
      } else {
        for (int32_t j = 2; j < iLength; j++) {
          argValue->GetObjectPropertyByIdx(j, jsObjectValue.get());
          jsObjectValue->GetObjectProperty(
              propertyValue->ToString().AsStringView(), newPropertyValue.get());
          iCount += newPropertyValue->IsNull() ? 0 : 1;
        }
      }
    } else if (argValue->IsObject()) {
      auto newPropertyValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
      GetObjectDefaultValue(argValue.get(), newPropertyValue.get());
      if (!newPropertyValue->IsNull())
        iCount++;
    } else {
      iCount++;
    }
  }
  args.GetReturnValue()->SetInteger(iCount);
}

// static
void CFXJSE_FormCalcContext::Floor(CFXJSE_Value* pThis,
                                   ByteStringView szFuncName,
                                   CFXJSE_Arguments& args) {
  if (args.GetLength() != 1) {
    ToFormCalcContext(pThis)->ThrowParamCountMismatchException(L"Floor");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argValue = GetSimpleValue(pThis, args, 0);
  if (ValueIsNull(pThis, argValue.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  args.GetReturnValue()->SetFloat(floor(ValueToFloat(pThis, argValue.get())));
}

// static
void CFXJSE_FormCalcContext::Max(CFXJSE_Value* pThis,
                                 ByteStringView szFuncName,
                                 CFXJSE_Arguments& args) {
  CFXJSE_FormCalcContext* pContext = ToFormCalcContext(pThis);
  v8::Isolate* pIsolate = pContext->GetScriptRuntime();
  uint32_t uCount = 0;
  double dMaxValue = 0.0;
  for (int32_t i = 0; i < args.GetLength(); i++) {
    std::unique_ptr<CFXJSE_Value> argValue = args.GetValue(i);
    if (argValue->IsNull())
      continue;

    if (argValue->IsArray()) {
      auto lengthValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
      argValue->GetObjectProperty("length", lengthValue.get());
      int32_t iLength = lengthValue->ToInteger();
      if (iLength <= 2) {
        pContext->ThrowArgumentMismatchException();
        return;
      }

      auto propertyValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
      auto jsObjectValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
      auto newPropertyValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
      argValue->GetObjectPropertyByIdx(1, propertyValue.get());
      argValue->GetObjectPropertyByIdx(2, jsObjectValue.get());
      if (propertyValue->IsNull()) {
        for (int32_t j = 2; j < iLength; j++) {
          argValue->GetObjectPropertyByIdx(j, jsObjectValue.get());
          GetObjectDefaultValue(jsObjectValue.get(), newPropertyValue.get());
          if (newPropertyValue->IsNull())
            continue;

          uCount++;
          double dValue = ValueToDouble(pThis, newPropertyValue.get());
          dMaxValue = (uCount == 1) ? dValue : std::max(dMaxValue, dValue);
        }
      } else {
        for (int32_t j = 2; j < iLength; j++) {
          argValue->GetObjectPropertyByIdx(j, jsObjectValue.get());
          jsObjectValue->GetObjectProperty(
              propertyValue->ToString().AsStringView(), newPropertyValue.get());
          if (newPropertyValue->IsNull())
            continue;

          uCount++;
          double dValue = ValueToDouble(pThis, newPropertyValue.get());
          dMaxValue = (uCount == 1) ? dValue : std::max(dMaxValue, dValue);
        }
      }
    } else if (argValue->IsObject()) {
      auto newPropertyValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
      GetObjectDefaultValue(argValue.get(), newPropertyValue.get());
      if (newPropertyValue->IsNull())
        continue;

      uCount++;
      double dValue = ValueToDouble(pThis, newPropertyValue.get());
      dMaxValue = (uCount == 1) ? dValue : std::max(dMaxValue, dValue);
    } else {
      uCount++;
      double dValue = ValueToDouble(pThis, argValue.get());
      dMaxValue = (uCount == 1) ? dValue : std::max(dMaxValue, dValue);
    }
  }
  if (uCount == 0) {
    args.GetReturnValue()->SetNull();
    return;
  }

  args.GetReturnValue()->SetDouble(dMaxValue);
}

// static
void CFXJSE_FormCalcContext::Min(CFXJSE_Value* pThis,
                                 ByteStringView szFuncName,
                                 CFXJSE_Arguments& args) {
  CFXJSE_FormCalcContext* pContext = ToFormCalcContext(pThis);
  v8::Isolate* pIsolate = pContext->GetScriptRuntime();
  uint32_t uCount = 0;
  double dMinValue = 0.0;
  for (int32_t i = 0; i < args.GetLength(); i++) {
    std::unique_ptr<CFXJSE_Value> argValue = args.GetValue(i);
    if (argValue->IsNull())
      continue;

    if (argValue->IsArray()) {
      auto lengthValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
      argValue->GetObjectProperty("length", lengthValue.get());
      int32_t iLength = lengthValue->ToInteger();
      if (iLength <= 2) {
        pContext->ThrowArgumentMismatchException();
        return;
      }

      auto propertyValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
      auto jsObjectValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
      auto newPropertyValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
      argValue->GetObjectPropertyByIdx(1, propertyValue.get());
      argValue->GetObjectPropertyByIdx(2, jsObjectValue.get());
      if (propertyValue->IsNull()) {
        for (int32_t j = 2; j < iLength; j++) {
          argValue->GetObjectPropertyByIdx(j, jsObjectValue.get());
          GetObjectDefaultValue(jsObjectValue.get(), newPropertyValue.get());
          if (newPropertyValue->IsNull())
            continue;

          uCount++;
          double dValue = ValueToDouble(pThis, newPropertyValue.get());
          dMinValue = uCount == 1 ? dValue : std::min(dMinValue, dValue);
        }
      } else {
        for (int32_t j = 2; j < iLength; j++) {
          argValue->GetObjectPropertyByIdx(j, jsObjectValue.get());
          jsObjectValue->GetObjectProperty(
              propertyValue->ToString().AsStringView(), newPropertyValue.get());
          if (newPropertyValue->IsNull())
            continue;

          uCount++;
          double dValue = ValueToDouble(pThis, newPropertyValue.get());
          dMinValue = uCount == 1 ? dValue : std::min(dMinValue, dValue);
        }
      }
    } else if (argValue->IsObject()) {
      auto newPropertyValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
      GetObjectDefaultValue(argValue.get(), newPropertyValue.get());
      if (newPropertyValue->IsNull())
        continue;

      uCount++;
      double dValue = ValueToDouble(pThis, newPropertyValue.get());
      dMinValue = uCount == 1 ? dValue : std::min(dMinValue, dValue);
    } else {
      uCount++;
      double dValue = ValueToDouble(pThis, argValue.get());
      dMinValue = uCount == 1 ? dValue : std::min(dMinValue, dValue);
    }
  }
  if (uCount == 0) {
    args.GetReturnValue()->SetNull();
    return;
  }

  args.GetReturnValue()->SetDouble(dMinValue);
}

// static
void CFXJSE_FormCalcContext::Mod(CFXJSE_Value* pThis,
                                 ByteStringView szFuncName,
                                 CFXJSE_Arguments& args) {
  CFXJSE_FormCalcContext* pContext = ToFormCalcContext(pThis);
  if (args.GetLength() != 2) {
    pContext->ThrowParamCountMismatchException(L"Mod");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = args.GetValue(0);
  std::unique_ptr<CFXJSE_Value> argTwo = args.GetValue(1);
  if (argOne->IsNull() || argTwo->IsNull()) {
    args.GetReturnValue()->SetNull();
    return;
  }

  bool argOneResult;
  double dDividend = ExtractDouble(pThis, argOne.get(), &argOneResult);
  bool argTwoResult;
  double dDivisor = ExtractDouble(pThis, argTwo.get(), &argTwoResult);
  if (!argOneResult || !argTwoResult) {
    pContext->ThrowArgumentMismatchException();
    return;
  }

  if (dDivisor == 0.0) {
    pContext->ThrowDivideByZeroException();
    return;
  }

  args.GetReturnValue()->SetDouble(dDividend -
                                   dDivisor * (int32_t)(dDividend / dDivisor));
}

// static
void CFXJSE_FormCalcContext::Round(CFXJSE_Value* pThis,
                                   ByteStringView szFuncName,
                                   CFXJSE_Arguments& args) {
  CFXJSE_FormCalcContext* pContext = ToFormCalcContext(pThis);
  int32_t argc = args.GetLength();
  if (argc < 1 || argc > 2) {
    pContext->ThrowParamCountMismatchException(L"Round");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = args.GetValue(0);
  if (argOne->IsNull()) {
    args.GetReturnValue()->SetNull();
    return;
  }

  bool dValueRet;
  double dValue = ExtractDouble(pThis, argOne.get(), &dValueRet);
  if (!dValueRet) {
    pContext->ThrowArgumentMismatchException();
    return;
  }

  uint8_t uPrecision = 0;
  if (argc > 1) {
    std::unique_ptr<CFXJSE_Value> argTwo = args.GetValue(1);
    if (argTwo->IsNull()) {
      args.GetReturnValue()->SetNull();
      return;
    }

    bool dPrecisionRet;
    double dPrecision = ExtractDouble(pThis, argTwo.get(), &dPrecisionRet);
    if (!dPrecisionRet) {
      pContext->ThrowArgumentMismatchException();
      return;
    }

    uPrecision = static_cast<uint8_t>(pdfium::clamp(dPrecision, 0.0, 12.0));
  }

  CFGAS_Decimal decimalValue(static_cast<float>(dValue), uPrecision);
  args.GetReturnValue()->SetDouble(decimalValue);
}

// static
void CFXJSE_FormCalcContext::Sum(CFXJSE_Value* pThis,
                                 ByteStringView szFuncName,
                                 CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc == 0) {
    args.GetReturnValue()->SetNull();
    return;
  }

  CFXJSE_FormCalcContext* pContext = ToFormCalcContext(pThis);
  v8::Isolate* pIsolate = pContext->GetScriptRuntime();
  uint32_t uCount = 0;
  double dSum = 0.0;
  for (int32_t i = 0; i < argc; i++) {
    std::unique_ptr<CFXJSE_Value> argValue = args.GetValue(i);
    if (argValue->IsNull())
      continue;

    if (argValue->IsArray()) {
      auto lengthValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
      argValue->GetObjectProperty("length", lengthValue.get());
      int32_t iLength = lengthValue->ToInteger();
      if (iLength <= 2) {
        pContext->ThrowArgumentMismatchException();
        return;
      }

      auto propertyValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
      argValue->GetObjectPropertyByIdx(1, propertyValue.get());
      auto jsObjectValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
      auto newPropertyValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
      if (propertyValue->IsNull()) {
        for (int32_t j = 2; j < iLength; j++) {
          argValue->GetObjectPropertyByIdx(j, jsObjectValue.get());
          GetObjectDefaultValue(jsObjectValue.get(), newPropertyValue.get());
          if (newPropertyValue->IsNull())
            continue;

          dSum += ValueToDouble(pThis, jsObjectValue.get());
          uCount++;
        }
      } else {
        for (int32_t j = 2; j < iLength; j++) {
          argValue->GetObjectPropertyByIdx(j, jsObjectValue.get());
          jsObjectValue->GetObjectProperty(
              propertyValue->ToString().AsStringView(), newPropertyValue.get());
          if (newPropertyValue->IsNull())
            continue;

          dSum += ValueToDouble(pThis, newPropertyValue.get());
          uCount++;
        }
      }
    } else if (argValue->IsObject()) {
      auto newPropertyValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
      GetObjectDefaultValue(argValue.get(), newPropertyValue.get());
      if (newPropertyValue->IsNull())
        continue;

      dSum += ValueToDouble(pThis, argValue.get());
      uCount++;
    } else {
      dSum += ValueToDouble(pThis, argValue.get());
      uCount++;
    }
  }
  if (uCount == 0) {
    args.GetReturnValue()->SetNull();
    return;
  }

  args.GetReturnValue()->SetDouble(dSum);
}

// static
void CFXJSE_FormCalcContext::Date(CFXJSE_Value* pThis,
                                  ByteStringView szFuncName,
                                  CFXJSE_Arguments& args) {
  if (args.GetLength() != 0) {
    ToFormCalcContext(pThis)->ThrowParamCountMismatchException(L"Date");
    return;
  }

  time_t currentTime;
  FXSYS_time(&currentTime);
  struct tm* pTmStruct = gmtime(&currentTime);

  args.GetReturnValue()->SetInteger(DateString2Num(
      ByteString::Format("%d%02d%02d", pTmStruct->tm_year + 1900,
                         pTmStruct->tm_mon + 1, pTmStruct->tm_mday)
          .AsStringView()));
}

// static
void CFXJSE_FormCalcContext::Date2Num(CFXJSE_Value* pThis,
                                      ByteStringView szFuncName,
                                      CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc < 1 || argc > 3) {
    ToFormCalcContext(pThis)->ThrowParamCountMismatchException(L"Date2Num");
    return;
  }

  std::unique_ptr<CFXJSE_Value> dateValue = GetSimpleValue(pThis, args, 0);
  if (ValueIsNull(pThis, dateValue.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  ByteString dateString = ValueToUTF8String(dateValue.get());
  ByteString formatString;
  if (argc > 1) {
    std::unique_ptr<CFXJSE_Value> formatValue = GetSimpleValue(pThis, args, 1);
    if (ValueIsNull(pThis, formatValue.get())) {
      args.GetReturnValue()->SetNull();
      return;
    }
    formatString = ValueToUTF8String(formatValue.get());
  }

  ByteString localString;
  if (argc > 2) {
    std::unique_ptr<CFXJSE_Value> localValue = GetSimpleValue(pThis, args, 2);
    if (ValueIsNull(pThis, localValue.get())) {
      args.GetReturnValue()->SetNull();
      return;
    }
    localString = ValueToUTF8String(localValue.get());
  }

  ByteString szIsoDateString =
      Local2IsoDate(pThis, dateString.AsStringView(),
                    formatString.AsStringView(), localString.AsStringView());
  args.GetReturnValue()->SetInteger(
      DateString2Num(szIsoDateString.AsStringView()));
}

// static
void CFXJSE_FormCalcContext::DateFmt(CFXJSE_Value* pThis,
                                     ByteStringView szFuncName,
                                     CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc > 2) {
    ToFormCalcContext(pThis)->ThrowParamCountMismatchException(L"Date2Num");
    return;
  }

  int32_t iStyle = 0;
  if (argc > 0) {
    std::unique_ptr<CFXJSE_Value> argStyle = GetSimpleValue(pThis, args, 0);
    if (argStyle->IsNull()) {
      args.GetReturnValue()->SetNull();
      return;
    }

    iStyle = (int32_t)ValueToFloat(pThis, argStyle.get());
    if (iStyle < 0 || iStyle > 4)
      iStyle = 0;
  }

  ByteString szLocal;
  if (argc > 1) {
    std::unique_ptr<CFXJSE_Value> argLocal = GetSimpleValue(pThis, args, 1);
    if (argLocal->IsNull()) {
      args.GetReturnValue()->SetNull();
      return;
    }
    szLocal = ValueToUTF8String(argLocal.get());
  }

  ByteString formatStr =
      GetStandardDateFormat(pThis, iStyle, szLocal.AsStringView());
  args.GetReturnValue()->SetString(formatStr.AsStringView());
}

// static
void CFXJSE_FormCalcContext::IsoDate2Num(CFXJSE_Value* pThis,
                                         ByteStringView szFuncName,
                                         CFXJSE_Arguments& args) {
  if (args.GetLength() != 1) {
    ToFormCalcContext(pThis)->ThrowParamCountMismatchException(L"IsoDate2Num");
    return;
  }
  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  if (argOne->IsNull()) {
    args.GetReturnValue()->SetNull();
    return;
  }
  ByteString szArgString = ValueToUTF8String(argOne.get());
  args.GetReturnValue()->SetInteger(DateString2Num(szArgString.AsStringView()));
}

// static
void CFXJSE_FormCalcContext::IsoTime2Num(CFXJSE_Value* pThis,
                                         ByteStringView szFuncName,
                                         CFXJSE_Arguments& args) {
  CFXJSE_FormCalcContext* pContext = ToFormCalcContext(pThis);
  if (args.GetLength() != 1) {
    pContext->ThrowParamCountMismatchException(L"IsoTime2Num");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  if (ValueIsNull(pThis, argOne.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  CXFA_Document* pDoc = pContext->GetDocument();
  CXFA_LocaleMgr* pMgr = pDoc->GetLocaleMgr();
  ByteString szArgString = ValueToUTF8String(argOne.get());
  auto pos = szArgString.Find('T', 0);
  if (!pos.has_value() || pos.value() == szArgString.GetLength() - 1) {
    args.GetReturnValue()->SetInteger(0);
    return;
  }
  szArgString = szArgString.Right(szArgString.GetLength() - (pos.value() + 1));

  CXFA_LocaleValue timeValue(
      XFA_VT_TIME, WideString::FromUTF8(szArgString.AsStringView()), pMgr);
  if (!timeValue.IsValid()) {
    args.GetReturnValue()->SetInteger(0);
    return;
  }

  CFX_DateTime uniTime = timeValue.GetTime();
  int32_t hour = uniTime.GetHour();
  int32_t min = uniTime.GetMinute();
  int32_t second = uniTime.GetSecond();
  int32_t milSecond = uniTime.GetMillisecond();

  // TODO(dsinclair): See if there is other time conversion code in pdfium and
  //   consolidate.
  int32_t mins = hour * 60 + min;
  mins -= (pMgr->GetDefLocale()->GetTimeZone().tzHour * 60);
  while (mins > 1440)
    mins -= 1440;
  while (mins < 0)
    mins += 1440;
  hour = mins / 60;
  min = mins % 60;

  args.GetReturnValue()->SetInteger(hour * 3600000 + min * 60000 +
                                    second * 1000 + milSecond + 1);
}

// static
void CFXJSE_FormCalcContext::LocalDateFmt(CFXJSE_Value* pThis,
                                          ByteStringView szFuncName,
                                          CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc > 2) {
    ToFormCalcContext(pThis)->ThrowParamCountMismatchException(L"LocalDateFmt");
    return;
  }

  int32_t iStyle = 0;
  if (argc > 0) {
    std::unique_ptr<CFXJSE_Value> argStyle = GetSimpleValue(pThis, args, 0);
    if (argStyle->IsNull()) {
      args.GetReturnValue()->SetNull();
      return;
    }
    iStyle = (int32_t)ValueToFloat(pThis, argStyle.get());
    if (iStyle > 4 || iStyle < 0)
      iStyle = 0;
  }

  ByteString szLocal;
  if (argc > 1) {
    std::unique_ptr<CFXJSE_Value> argLocal = GetSimpleValue(pThis, args, 1);
    if (argLocal->IsNull()) {
      args.GetReturnValue()->SetNull();
      return;
    }
    szLocal = ValueToUTF8String(argLocal.get());
  }

  ByteString formatStr =
      GetLocalDateFormat(pThis, iStyle, szLocal.AsStringView(), false);
  args.GetReturnValue()->SetString(formatStr.AsStringView());
}

// static
void CFXJSE_FormCalcContext::LocalTimeFmt(CFXJSE_Value* pThis,
                                          ByteStringView szFuncName,
                                          CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc > 2) {
    ToFormCalcContext(pThis)->ThrowParamCountMismatchException(L"LocalTimeFmt");
    return;
  }

  int32_t iStyle = 0;
  if (argc > 0) {
    std::unique_ptr<CFXJSE_Value> argStyle = GetSimpleValue(pThis, args, 0);
    if (argStyle->IsNull()) {
      args.GetReturnValue()->SetNull();
      return;
    }
    iStyle = (int32_t)ValueToFloat(pThis, argStyle.get());
    if (iStyle > 4 || iStyle < 0)
      iStyle = 0;
  }

  ByteString szLocal;
  if (argc > 1) {
    std::unique_ptr<CFXJSE_Value> argLocal = GetSimpleValue(pThis, args, 1);
    if (argLocal->IsNull()) {
      args.GetReturnValue()->SetNull();
      return;
    }
    szLocal = ValueToUTF8String(argLocal.get());
  }

  ByteString formatStr =
      GetLocalTimeFormat(pThis, iStyle, szLocal.AsStringView(), false);
  args.GetReturnValue()->SetString(formatStr.AsStringView());
}

// static
void CFXJSE_FormCalcContext::Num2Date(CFXJSE_Value* pThis,
                                      ByteStringView szFuncName,
                                      CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc < 1 || argc > 3) {
    ToFormCalcContext(pThis)->ThrowParamCountMismatchException(L"Num2Date");
    return;
  }

  std::unique_ptr<CFXJSE_Value> dateValue = GetSimpleValue(pThis, args, 0);
  if (ValueIsNull(pThis, dateValue.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }
  int32_t dDate = (int32_t)ValueToFloat(pThis, dateValue.get());
  if (dDate < 1) {
    args.GetReturnValue()->SetNull();
    return;
  }

  ByteString formatString;
  if (argc > 1) {
    std::unique_ptr<CFXJSE_Value> formatValue = GetSimpleValue(pThis, args, 1);
    if (ValueIsNull(pThis, formatValue.get())) {
      args.GetReturnValue()->SetNull();
      return;
    }
    formatString = ValueToUTF8String(formatValue.get());
  }

  ByteString localString;
  if (argc > 2) {
    std::unique_ptr<CFXJSE_Value> localValue = GetSimpleValue(pThis, args, 2);
    if (ValueIsNull(pThis, localValue.get())) {
      args.GetReturnValue()->SetNull();
      return;
    }
    localString = ValueToUTF8String(localValue.get());
  }

  int32_t iYear = 1900;
  int32_t iMonth = 1;
  int32_t iDay = 1;
  int32_t i = 0;
  while (dDate > 0) {
    if (iMonth == 2) {
      if ((!((iYear + i) % 4) && ((iYear + i) % 100)) || !((iYear + i) % 400)) {
        if (dDate > 29) {
          ++iMonth;
          if (iMonth > 12) {
            iMonth = 1;
            ++i;
          }
          iDay = 1;
          dDate -= 29;
        } else {
          iDay += static_cast<int32_t>(dDate) - 1;
          dDate = 0;
        }
      } else {
        if (dDate > 28) {
          ++iMonth;
          if (iMonth > 12) {
            iMonth = 1;
            ++i;
          }
          iDay = 1;
          dDate -= 28;
        } else {
          iDay += static_cast<int32_t>(dDate) - 1;
          dDate = 0;
        }
      }
    } else if (iMonth < 8) {
      if ((iMonth % 2 == 0)) {
        if (dDate > 30) {
          ++iMonth;
          if (iMonth > 12) {
            iMonth = 1;
            ++i;
          }
          iDay = 1;
          dDate -= 30;
        } else {
          iDay += static_cast<int32_t>(dDate) - 1;
          dDate = 0;
        }
      } else {
        if (dDate > 31) {
          ++iMonth;
          if (iMonth > 12) {
            iMonth = 1;
            ++i;
          }
          iDay = 1;
          dDate -= 31;
        } else {
          iDay += static_cast<int32_t>(dDate) - 1;
          dDate = 0;
        }
      }
    } else {
      if (iMonth % 2 != 0) {
        if (dDate > 30) {
          ++iMonth;
          if (iMonth > 12) {
            iMonth = 1;
            ++i;
          }
          iDay = 1;
          dDate -= 30;
        } else {
          iDay += static_cast<int32_t>(dDate) - 1;
          dDate = 0;
        }
      } else {
        if (dDate > 31) {
          ++iMonth;
          if (iMonth > 12) {
            iMonth = 1;
            ++i;
          }
          iDay = 1;
          dDate -= 31;
        } else {
          iDay += static_cast<int32_t>(dDate) - 1;
          dDate = 0;
        }
      }
    }
  }

  ByteString szLocalDateString = IsoDate2Local(
      pThis,
      ByteString::Format("%d%02d%02d", iYear + i, iMonth, iDay).AsStringView(),
      formatString.AsStringView(), localString.AsStringView());
  args.GetReturnValue()->SetString(szLocalDateString.AsStringView());
}

// static
void CFXJSE_FormCalcContext::Num2GMTime(CFXJSE_Value* pThis,
                                        ByteStringView szFuncName,
                                        CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc < 1 || argc > 3) {
    ToFormCalcContext(pThis)->ThrowParamCountMismatchException(L"Num2GMTime");
    return;
  }

  std::unique_ptr<CFXJSE_Value> timeValue = GetSimpleValue(pThis, args, 0);
  if (timeValue->IsNull()) {
    args.GetReturnValue()->SetNull();
    return;
  }
  int32_t iTime = (int32_t)ValueToFloat(pThis, timeValue.get());
  if (abs(iTime) < 1.0) {
    args.GetReturnValue()->SetNull();
    return;
  }

  ByteString formatString;
  if (argc > 1) {
    std::unique_ptr<CFXJSE_Value> formatValue = GetSimpleValue(pThis, args, 1);
    if (formatValue->IsNull()) {
      args.GetReturnValue()->SetNull();
      return;
    }
    formatString = ValueToUTF8String(formatValue.get());
  }

  ByteString localString;
  if (argc > 2) {
    std::unique_ptr<CFXJSE_Value> localValue = GetSimpleValue(pThis, args, 2);
    if (localValue->IsNull()) {
      args.GetReturnValue()->SetNull();
      return;
    }
    localString = ValueToUTF8String(localValue.get());
  }

  ByteString szGMTTimeString =
      Num2AllTime(pThis, iTime, formatString.AsStringView(),
                  localString.AsStringView(), true);
  args.GetReturnValue()->SetString(szGMTTimeString.AsStringView());
}

// static
void CFXJSE_FormCalcContext::Num2Time(CFXJSE_Value* pThis,
                                      ByteStringView szFuncName,
                                      CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc < 1 || argc > 3) {
    ToFormCalcContext(pThis)->ThrowParamCountMismatchException(L"Num2Time");
    return;
  }

  std::unique_ptr<CFXJSE_Value> timeValue = GetSimpleValue(pThis, args, 0);
  if (timeValue->IsNull()) {
    args.GetReturnValue()->SetNull();
    return;
  }
  float fTime = ValueToFloat(pThis, timeValue.get());
  if (fabs(fTime) < 1.0) {
    args.GetReturnValue()->SetNull();
    return;
  }

  ByteString formatString;
  if (argc > 1) {
    std::unique_ptr<CFXJSE_Value> formatValue = GetSimpleValue(pThis, args, 1);
    if (formatValue->IsNull()) {
      args.GetReturnValue()->SetNull();
      return;
    }
    formatString = ValueToUTF8String(formatValue.get());
  }

  ByteString localString;
  if (argc > 2) {
    std::unique_ptr<CFXJSE_Value> localValue = GetSimpleValue(pThis, args, 2);
    if (localValue->IsNull()) {
      args.GetReturnValue()->SetNull();
      return;
    }
    localString = ValueToUTF8String(localValue.get());
  }

  ByteString szLocalTimeString = Num2AllTime(pThis, static_cast<int32_t>(fTime),
                                             formatString.AsStringView(),
                                             localString.AsStringView(), false);
  args.GetReturnValue()->SetString(szLocalTimeString.AsStringView());
}

// static
void CFXJSE_FormCalcContext::Time(CFXJSE_Value* pThis,
                                  ByteStringView szFuncName,
                                  CFXJSE_Arguments& args) {
  if (args.GetLength() != 0) {
    ToFormCalcContext(pThis)->ThrowParamCountMismatchException(L"Time");
    return;
  }

  time_t now;
  FXSYS_time(&now);

  struct tm* pGmt = gmtime(&now);
  args.GetReturnValue()->SetInteger(
      (pGmt->tm_hour * 3600 + pGmt->tm_min * 60 + pGmt->tm_sec) * 1000);
}

// static
void CFXJSE_FormCalcContext::Time2Num(CFXJSE_Value* pThis,
                                      ByteStringView szFuncName,
                                      CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc < 1 || argc > 3) {
    ToFormCalcContext(pThis)->ThrowParamCountMismatchException(L"Time2Num");
    return;
  }

  ByteString timeString;
  std::unique_ptr<CFXJSE_Value> timeValue = GetSimpleValue(pThis, args, 0);
  if (ValueIsNull(pThis, timeValue.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }
  timeString = ValueToUTF8String(timeValue.get());

  ByteString formatString;
  if (argc > 1) {
    std::unique_ptr<CFXJSE_Value> formatValue = GetSimpleValue(pThis, args, 1);
    if (ValueIsNull(pThis, formatValue.get())) {
      args.GetReturnValue()->SetNull();
      return;
    }
    formatString = ValueToUTF8String(formatValue.get());
  }

  ByteString localString;
  if (argc > 2) {
    std::unique_ptr<CFXJSE_Value> localValue = GetSimpleValue(pThis, args, 2);
    if (ValueIsNull(pThis, localValue.get())) {
      args.GetReturnValue()->SetNull();
      return;
    }
    localString = ValueToUTF8String(localValue.get());
  }

  CXFA_Document* pDoc = ToFormCalcContext(pThis)->GetDocument();
  CXFA_LocaleMgr* pMgr = pDoc->GetLocaleMgr();
  LocaleIface* pLocale = nullptr;
  if (localString.IsEmpty()) {
    CXFA_Node* pThisNode = ToNode(pDoc->GetScriptContext()->GetThisObject());
    ASSERT(pThisNode);
    pLocale = pThisNode->GetLocale();
  } else {
    pLocale =
        pMgr->GetLocaleByName(WideString::FromUTF8(localString.AsStringView()));
  }

  WideString wsFormat;
  if (formatString.IsEmpty())
    wsFormat = pLocale->GetTimePattern(FX_LOCALEDATETIMESUBCATEGORY_Default);
  else
    wsFormat = WideString::FromUTF8(formatString.AsStringView());

  wsFormat = L"time{" + wsFormat + L"}";
  CXFA_LocaleValue localeValue(XFA_VT_TIME,
                               WideString::FromUTF8(timeString.AsStringView()),
                               wsFormat, pLocale, pMgr);
  if (!localeValue.IsValid()) {
    args.GetReturnValue()->SetInteger(0);
    return;
  }

  CFX_DateTime uniTime = localeValue.GetTime();
  int32_t hour = uniTime.GetHour();
  int32_t min = uniTime.GetMinute();
  int32_t second = uniTime.GetSecond();
  int32_t milSecond = uniTime.GetMillisecond();
  int32_t mins = hour * 60 + min;

  mins -= (CXFA_TimeZoneProvider().GetTimeZone().tzHour * 60);
  while (mins > 1440)
    mins -= 1440;

  while (mins < 0)
    mins += 1440;

  hour = mins / 60;
  min = mins % 60;
  args.GetReturnValue()->SetInteger(hour * 3600000 + min * 60000 +
                                    second * 1000 + milSecond + 1);
}

// static
void CFXJSE_FormCalcContext::TimeFmt(CFXJSE_Value* pThis,
                                     ByteStringView szFuncName,
                                     CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc > 2) {
    ToFormCalcContext(pThis)->ThrowParamCountMismatchException(L"TimeFmt");
    return;
  }

  int32_t iStyle = 0;
  if (argc > 0) {
    std::unique_ptr<CFXJSE_Value> argStyle = GetSimpleValue(pThis, args, 0);
    if (argStyle->IsNull()) {
      args.GetReturnValue()->SetNull();
      return;
    }
    iStyle = (int32_t)ValueToFloat(pThis, argStyle.get());
    if (iStyle > 4 || iStyle < 0)
      iStyle = 0;
  }

  ByteString szLocal;
  if (argc > 1) {
    std::unique_ptr<CFXJSE_Value> argLocal = GetSimpleValue(pThis, args, 1);
    if (argLocal->IsNull()) {
      args.GetReturnValue()->SetNull();
      return;
    }
    szLocal = ValueToUTF8String(argLocal.get());
  }

  ByteString formatStr =
      GetStandardTimeFormat(pThis, iStyle, szLocal.AsStringView());
  args.GetReturnValue()->SetString(formatStr.AsStringView());
}

// static
ByteString CFXJSE_FormCalcContext::Local2IsoDate(CFXJSE_Value* pThis,
                                                 ByteStringView szDate,
                                                 ByteStringView szFormat,
                                                 ByteStringView szLocale) {
  CXFA_Document* pDoc = ToFormCalcContext(pThis)->GetDocument();
  if (!pDoc)
    return ByteString();

  CXFA_LocaleMgr* pMgr = pDoc->GetLocaleMgr();
  LocaleIface* pLocale = LocaleFromString(pDoc, pMgr, szLocale);
  if (!pLocale)
    return ByteString();

  WideString wsFormat = FormatFromString(pLocale, szFormat);
  CFX_DateTime dt = CXFA_LocaleValue(XFA_VT_DATE, WideString::FromUTF8(szDate),
                                     wsFormat, pLocale, pMgr)
                        .GetDate();

  return ByteString::Format("%4d-%02d-%02d", dt.GetYear(), dt.GetMonth(),
                            dt.GetDay());
}

// static
ByteString CFXJSE_FormCalcContext::IsoDate2Local(CFXJSE_Value* pThis,
                                                 ByteStringView szDate,
                                                 ByteStringView szFormat,
                                                 ByteStringView szLocale) {
  CXFA_Document* pDoc = ToFormCalcContext(pThis)->GetDocument();
  if (!pDoc)
    return ByteString();

  CXFA_LocaleMgr* pMgr = pDoc->GetLocaleMgr();
  LocaleIface* pLocale = LocaleFromString(pDoc, pMgr, szLocale);
  if (!pLocale)
    return ByteString();

  WideString wsFormat = FormatFromString(pLocale, szFormat);
  WideString wsRet;
  CXFA_LocaleValue(XFA_VT_DATE, WideString::FromUTF8(szDate), pMgr)
      .FormatPatterns(wsRet, wsFormat, pLocale, XFA_VALUEPICTURE_Display);
  return wsRet.ToUTF8();
}

// static
ByteString CFXJSE_FormCalcContext::IsoTime2Local(CFXJSE_Value* pThis,
                                                 ByteStringView szTime,
                                                 ByteStringView szFormat,
                                                 ByteStringView szLocale) {
  CXFA_Document* pDoc = ToFormCalcContext(pThis)->GetDocument();
  if (!pDoc)
    return ByteString();

  CXFA_LocaleMgr* pMgr = pDoc->GetLocaleMgr();
  LocaleIface* pLocale = LocaleFromString(pDoc, pMgr, szLocale);
  if (!pLocale)
    return ByteString();

  WideString wsFormat = {
      L"time{", FormatFromString(pLocale, szFormat).AsStringView(), L"}"};
  CXFA_LocaleValue widgetValue(XFA_VT_TIME, WideString::FromUTF8(szTime), pMgr);
  WideString wsRet;
  widgetValue.FormatPatterns(wsRet, wsFormat, pLocale,
                             XFA_VALUEPICTURE_Display);
  return wsRet.ToUTF8();
}

// static
ByteString CFXJSE_FormCalcContext::GetLocalDateFormat(CFXJSE_Value* pThis,
                                                      int32_t iStyle,
                                                      ByteStringView szLocale,
                                                      bool bStandard) {
  CXFA_Document* pDoc = ToFormCalcContext(pThis)->GetDocument();
  if (!pDoc)
    return ByteString();

  return GetLocalDateTimeFormat(pDoc, iStyle, szLocale, bStandard,
                                /*bIsDate=*/true);
}

// static
ByteString CFXJSE_FormCalcContext::GetLocalTimeFormat(CFXJSE_Value* pThis,
                                                      int32_t iStyle,
                                                      ByteStringView szLocale,
                                                      bool bStandard) {
  CXFA_Document* pDoc = ToFormCalcContext(pThis)->GetDocument();
  if (!pDoc)
    return ByteString();

  return GetLocalDateTimeFormat(pDoc, iStyle, szLocale, bStandard,
                                /*bIsDate=*/false);
}

// static
ByteString CFXJSE_FormCalcContext::GetStandardDateFormat(
    CFXJSE_Value* pThis,
    int32_t iStyle,
    ByteStringView szLocalStr) {
  return GetLocalDateFormat(pThis, iStyle, szLocalStr, true);
}

// static
ByteString CFXJSE_FormCalcContext::GetStandardTimeFormat(
    CFXJSE_Value* pThis,
    int32_t iStyle,
    ByteStringView szLocalStr) {
  return GetLocalTimeFormat(pThis, iStyle, szLocalStr, true);
}

// static
ByteString CFXJSE_FormCalcContext::Num2AllTime(CFXJSE_Value* pThis,
                                               int32_t iTime,
                                               ByteStringView szFormat,
                                               ByteStringView szLocale,
                                               bool bGM) {
  int32_t iHour = 0;
  int32_t iMin = 0;
  int32_t iSec = 0;
  iHour = static_cast<int>(iTime) / 3600000;
  iMin = (static_cast<int>(iTime) - iHour * 3600000) / 60000;
  iSec = (static_cast<int>(iTime) - iHour * 3600000 - iMin * 60000) / 1000;

  if (!bGM) {
    int32_t iZoneHour;
    int32_t iZoneMin;
    int32_t iZoneSec;
    GetLocalTimeZone(&iZoneHour, &iZoneMin, &iZoneSec);
    iHour += iZoneHour;
    iMin += iZoneMin;
    iSec += iZoneSec;
  }

  return IsoTime2Local(
      pThis,
      ByteString::Format("%02d:%02d:%02d", iHour, iMin, iSec).AsStringView(),
      szFormat, szLocale);
}

// static
void CFXJSE_FormCalcContext::Apr(CFXJSE_Value* pThis,
                                 ByteStringView szFuncName,
                                 CFXJSE_Arguments& args) {
  CFXJSE_FormCalcContext* pContext = ToFormCalcContext(pThis);
  if (args.GetLength() != 3) {
    pContext->ThrowParamCountMismatchException(L"Apr");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argTwo = GetSimpleValue(pThis, args, 1);
  std::unique_ptr<CFXJSE_Value> argThree = GetSimpleValue(pThis, args, 2);
  if (ValueIsNull(pThis, argOne.get()) || ValueIsNull(pThis, argTwo.get()) ||
      ValueIsNull(pThis, argThree.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  double nPrincipal = ValueToDouble(pThis, argOne.get());
  double nPayment = ValueToDouble(pThis, argTwo.get());
  double nPeriods = ValueToDouble(pThis, argThree.get());
  if (nPrincipal <= 0 || nPayment <= 0 || nPeriods <= 0) {
    pContext->ThrowArgumentMismatchException();
    return;
  }

  double r = 2 * (nPeriods * nPayment - nPrincipal) / (nPeriods * nPrincipal);
  double nTemp = 1;
  for (int32_t i = 0; i < nPeriods; ++i)
    nTemp *= (1 + r);

  double nRet = r * nTemp / (nTemp - 1) - nPayment / nPrincipal;
  while (fabs(nRet) > kFinancialPrecision) {
    double nDerivative =
        ((nTemp + r * nPeriods * (nTemp / (1 + r))) * (nTemp - 1) -
         (r * nTemp * nPeriods * (nTemp / (1 + r)))) /
        ((nTemp - 1) * (nTemp - 1));
    if (nDerivative == 0) {
      args.GetReturnValue()->SetNull();
      return;
    }

    r = r - nRet / nDerivative;
    nTemp = 1;
    for (int32_t i = 0; i < nPeriods; ++i) {
      nTemp *= (1 + r);
    }
    nRet = r * nTemp / (nTemp - 1) - nPayment / nPrincipal;
  }
  args.GetReturnValue()->SetDouble(r * 12);
}

// static
void CFXJSE_FormCalcContext::CTerm(CFXJSE_Value* pThis,
                                   ByteStringView szFuncName,
                                   CFXJSE_Arguments& args) {
  CFXJSE_FormCalcContext* pContext = ToFormCalcContext(pThis);
  if (args.GetLength() != 3) {
    pContext->ThrowParamCountMismatchException(L"CTerm");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argTwo = GetSimpleValue(pThis, args, 1);
  std::unique_ptr<CFXJSE_Value> argThree = GetSimpleValue(pThis, args, 2);
  if (ValueIsNull(pThis, argOne.get()) || ValueIsNull(pThis, argTwo.get()) ||
      ValueIsNull(pThis, argThree.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  float nRate = ValueToFloat(pThis, argOne.get());
  float nFutureValue = ValueToFloat(pThis, argTwo.get());
  float nInitAmount = ValueToFloat(pThis, argThree.get());
  if ((nRate <= 0) || (nFutureValue <= 0) || (nInitAmount <= 0)) {
    pContext->ThrowArgumentMismatchException();
    return;
  }

  args.GetReturnValue()->SetFloat(log((float)(nFutureValue / nInitAmount)) /
                                  log((float)(1 + nRate)));
}

// static
void CFXJSE_FormCalcContext::FV(CFXJSE_Value* pThis,
                                ByteStringView szFuncName,
                                CFXJSE_Arguments& args) {
  CFXJSE_FormCalcContext* pContext = ToFormCalcContext(pThis);
  if (args.GetLength() != 3) {
    pContext->ThrowParamCountMismatchException(L"FV");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argTwo = GetSimpleValue(pThis, args, 1);
  std::unique_ptr<CFXJSE_Value> argThree = GetSimpleValue(pThis, args, 2);
  if (ValueIsNull(pThis, argOne.get()) || ValueIsNull(pThis, argTwo.get()) ||
      ValueIsNull(pThis, argThree.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  double nAmount = ValueToDouble(pThis, argOne.get());
  double nRate = ValueToDouble(pThis, argTwo.get());
  double nPeriod = ValueToDouble(pThis, argThree.get());
  if ((nRate < 0) || (nPeriod <= 0) || (nAmount <= 0)) {
    pContext->ThrowArgumentMismatchException();
    return;
  }

  double dResult = 0;
  if (nRate) {
    double nTemp = 1;
    for (int i = 0; i < nPeriod; ++i) {
      nTemp *= 1 + nRate;
    }
    dResult = nAmount * (nTemp - 1) / nRate;
  } else {
    dResult = nAmount * nPeriod;
  }

  args.GetReturnValue()->SetDouble(dResult);
}

// static
void CFXJSE_FormCalcContext::IPmt(CFXJSE_Value* pThis,
                                  ByteStringView szFuncName,
                                  CFXJSE_Arguments& args) {
  CFXJSE_FormCalcContext* pContext = ToFormCalcContext(pThis);
  if (args.GetLength() != 5) {
    pContext->ThrowParamCountMismatchException(L"IPmt");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argTwo = GetSimpleValue(pThis, args, 1);
  std::unique_ptr<CFXJSE_Value> argThree = GetSimpleValue(pThis, args, 2);
  std::unique_ptr<CFXJSE_Value> argFour = GetSimpleValue(pThis, args, 3);
  std::unique_ptr<CFXJSE_Value> argFive = GetSimpleValue(pThis, args, 4);
  if (ValueIsNull(pThis, argOne.get()) || ValueIsNull(pThis, argTwo.get()) ||
      ValueIsNull(pThis, argThree.get()) || ValueIsNull(pThis, argFour.get()) ||
      ValueIsNull(pThis, argFive.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  float nPrincipalAmount = ValueToFloat(pThis, argOne.get());
  float nRate = ValueToFloat(pThis, argTwo.get());
  float nPayment = ValueToFloat(pThis, argThree.get());
  float nFirstMonth = ValueToFloat(pThis, argFour.get());
  float nNumberOfMonths = ValueToFloat(pThis, argFive.get());
  if ((nPrincipalAmount <= 0) || (nRate <= 0) || (nPayment <= 0) ||
      (nFirstMonth < 0) || (nNumberOfMonths < 0)) {
    pContext->ThrowArgumentMismatchException();
    return;
  }

  float nRateOfMonth = nRate / 12;
  int32_t iNums =
      (int32_t)((log10((float)(nPayment / nPrincipalAmount)) -
                 log10((float)(nPayment / nPrincipalAmount - nRateOfMonth))) /
                log10((float)(1 + nRateOfMonth)));
  int32_t iEnd = std::min((int32_t)(nFirstMonth + nNumberOfMonths - 1), iNums);

  if (nPayment < nPrincipalAmount * nRateOfMonth) {
    args.GetReturnValue()->SetFloat(0);
    return;
  }

  int32_t i = 0;
  for (i = 0; i < nFirstMonth - 1; ++i)
    nPrincipalAmount -= nPayment - nPrincipalAmount * nRateOfMonth;

  float nSum = 0;
  for (; i < iEnd; ++i) {
    nSum += nPrincipalAmount * nRateOfMonth;
    nPrincipalAmount -= nPayment - nPrincipalAmount * nRateOfMonth;
  }
  args.GetReturnValue()->SetFloat(nSum);
}

// static
void CFXJSE_FormCalcContext::NPV(CFXJSE_Value* pThis,
                                 ByteStringView szFuncName,
                                 CFXJSE_Arguments& args) {
  CFXJSE_FormCalcContext* pContext = ToFormCalcContext(pThis);
  int32_t argc = args.GetLength();
  if (argc < 3) {
    pContext->ThrowParamCountMismatchException(L"NPV");
    return;
  }

  std::vector<std::unique_ptr<CFXJSE_Value>> argValues;
  for (int32_t i = 0; i < argc; i++) {
    argValues.push_back(GetSimpleValue(pThis, args, i));
    if (ValueIsNull(pThis, argValues[i].get())) {
      args.GetReturnValue()->SetNull();
      return;
    }
  }

  double nRate = ValueToDouble(pThis, argValues[0].get());
  if (nRate <= 0) {
    pContext->ThrowArgumentMismatchException();
    return;
  }

  std::vector<double> data(argc - 1);
  for (int32_t i = 1; i < argc; i++)
    data.push_back(ValueToDouble(pThis, argValues[i].get()));

  double nSum = 0;
  int32_t iIndex = 0;
  for (int32_t i = 0; i < argc - 1; i++) {
    double nTemp = 1;
    for (int32_t j = 0; j <= i; j++)
      nTemp *= 1 + nRate;

    double nNum = data[iIndex++];
    nSum += nNum / nTemp;
  }
  args.GetReturnValue()->SetDouble(nSum);
}

// static
void CFXJSE_FormCalcContext::Pmt(CFXJSE_Value* pThis,
                                 ByteStringView szFuncName,
                                 CFXJSE_Arguments& args) {
  CFXJSE_FormCalcContext* pContext = ToFormCalcContext(pThis);
  if (args.GetLength() != 3) {
    pContext->ThrowParamCountMismatchException(L"Pmt");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argTwo = GetSimpleValue(pThis, args, 1);
  std::unique_ptr<CFXJSE_Value> argThree = GetSimpleValue(pThis, args, 2);
  if (ValueIsNull(pThis, argOne.get()) || ValueIsNull(pThis, argTwo.get()) ||
      ValueIsNull(pThis, argThree.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  float nPrincipal = ValueToFloat(pThis, argOne.get());
  float nRate = ValueToFloat(pThis, argTwo.get());
  float nPeriods = ValueToFloat(pThis, argThree.get());
  if ((nPrincipal <= 0) || (nRate <= 0) || (nPeriods <= 0)) {
    pContext->ThrowArgumentMismatchException();
    return;
  }

  float nTmp = 1 + nRate;
  float nSum = nTmp;
  for (int32_t i = 0; i < nPeriods - 1; ++i)
    nSum *= nTmp;

  args.GetReturnValue()->SetFloat((nPrincipal * nRate * nSum) / (nSum - 1));
}

// static
void CFXJSE_FormCalcContext::PPmt(CFXJSE_Value* pThis,
                                  ByteStringView szFuncName,
                                  CFXJSE_Arguments& args) {
  CFXJSE_FormCalcContext* pContext = ToFormCalcContext(pThis);
  if (args.GetLength() != 5) {
    pContext->ThrowParamCountMismatchException(L"PPmt");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argTwo = GetSimpleValue(pThis, args, 1);
  std::unique_ptr<CFXJSE_Value> argThree = GetSimpleValue(pThis, args, 2);
  std::unique_ptr<CFXJSE_Value> argFour = GetSimpleValue(pThis, args, 3);
  std::unique_ptr<CFXJSE_Value> argFive = GetSimpleValue(pThis, args, 4);
  if (ValueIsNull(pThis, argOne.get()) || ValueIsNull(pThis, argTwo.get()) ||
      ValueIsNull(pThis, argThree.get()) || ValueIsNull(pThis, argFour.get()) ||
      ValueIsNull(pThis, argFive.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  float nPrincipalAmount = ValueToFloat(pThis, argOne.get());
  float nRate = ValueToFloat(pThis, argTwo.get());
  float nPayment = ValueToFloat(pThis, argThree.get());
  float nFirstMonth = ValueToFloat(pThis, argFour.get());
  float nNumberOfMonths = ValueToFloat(pThis, argFive.get());
  if ((nPrincipalAmount <= 0) || (nRate <= 0) || (nPayment <= 0) ||
      (nFirstMonth < 0) || (nNumberOfMonths < 0)) {
    pContext->ThrowArgumentMismatchException();
    return;
  }

  float nRateOfMonth = nRate / 12;
  int32_t iNums =
      (int32_t)((log10((float)(nPayment / nPrincipalAmount)) -
                 log10((float)(nPayment / nPrincipalAmount - nRateOfMonth))) /
                log10((float)(1 + nRateOfMonth)));
  int32_t iEnd = std::min((int32_t)(nFirstMonth + nNumberOfMonths - 1), iNums);
  if (nPayment < nPrincipalAmount * nRateOfMonth) {
    pContext->ThrowArgumentMismatchException();
    return;
  }

  int32_t i = 0;
  for (i = 0; i < nFirstMonth - 1; ++i)
    nPrincipalAmount -= nPayment - nPrincipalAmount * nRateOfMonth;

  float nTemp = 0;
  float nSum = 0;
  for (; i < iEnd; ++i) {
    nTemp = nPayment - nPrincipalAmount * nRateOfMonth;
    nSum += nTemp;
    nPrincipalAmount -= nTemp;
  }
  args.GetReturnValue()->SetFloat(nSum);
}

// static
void CFXJSE_FormCalcContext::PV(CFXJSE_Value* pThis,
                                ByteStringView szFuncName,
                                CFXJSE_Arguments& args) {
  CFXJSE_FormCalcContext* pContext = ToFormCalcContext(pThis);
  if (args.GetLength() != 3) {
    pContext->ThrowParamCountMismatchException(L"PV");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argTwo = GetSimpleValue(pThis, args, 1);
  std::unique_ptr<CFXJSE_Value> argThree = GetSimpleValue(pThis, args, 2);
  if (ValueIsNull(pThis, argOne.get()) || ValueIsNull(pThis, argTwo.get()) ||
      ValueIsNull(pThis, argThree.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  double nAmount = ValueToDouble(pThis, argOne.get());
  double nRate = ValueToDouble(pThis, argTwo.get());
  double nPeriod = ValueToDouble(pThis, argThree.get());
  if ((nAmount <= 0) || (nRate < 0) || (nPeriod <= 0)) {
    pContext->ThrowArgumentMismatchException();
    return;
  }

  double nTemp = 1;
  for (int32_t i = 0; i < nPeriod; ++i)
    nTemp *= 1 + nRate;

  nTemp = 1 / nTemp;
  args.GetReturnValue()->SetDouble(nAmount * ((1 - nTemp) / nRate));
}

// static
void CFXJSE_FormCalcContext::Rate(CFXJSE_Value* pThis,
                                  ByteStringView szFuncName,
                                  CFXJSE_Arguments& args) {
  CFXJSE_FormCalcContext* pContext = ToFormCalcContext(pThis);
  if (args.GetLength() != 3) {
    pContext->ThrowParamCountMismatchException(L"Rate");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argTwo = GetSimpleValue(pThis, args, 1);
  std::unique_ptr<CFXJSE_Value> argThree = GetSimpleValue(pThis, args, 2);
  if (ValueIsNull(pThis, argOne.get()) || ValueIsNull(pThis, argTwo.get()) ||
      ValueIsNull(pThis, argThree.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  float nFuture = ValueToFloat(pThis, argOne.get());
  float nPresent = ValueToFloat(pThis, argTwo.get());
  float nTotalNumber = ValueToFloat(pThis, argThree.get());
  if ((nFuture <= 0) || (nPresent < 0) || (nTotalNumber <= 0)) {
    pContext->ThrowArgumentMismatchException();
    return;
  }

  args.GetReturnValue()->SetFloat(
      FXSYS_pow((float)(nFuture / nPresent), (float)(1 / nTotalNumber)) - 1);
}

// static
void CFXJSE_FormCalcContext::Term(CFXJSE_Value* pThis,
                                  ByteStringView szFuncName,
                                  CFXJSE_Arguments& args) {
  CFXJSE_FormCalcContext* pContext = ToFormCalcContext(pThis);
  if (args.GetLength() != 3) {
    pContext->ThrowParamCountMismatchException(L"Term");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argTwo = GetSimpleValue(pThis, args, 1);
  std::unique_ptr<CFXJSE_Value> argThree = GetSimpleValue(pThis, args, 2);
  if (ValueIsNull(pThis, argOne.get()) || ValueIsNull(pThis, argTwo.get()) ||
      ValueIsNull(pThis, argThree.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  float nMount = ValueToFloat(pThis, argOne.get());
  float nRate = ValueToFloat(pThis, argTwo.get());
  float nFuture = ValueToFloat(pThis, argThree.get());
  if ((nMount <= 0) || (nRate <= 0) || (nFuture <= 0)) {
    pContext->ThrowArgumentMismatchException();
    return;
  }

  args.GetReturnValue()->SetFloat(log((float)(nFuture / nMount * nRate) + 1) /
                                  log((float)(1 + nRate)));
}

// static
void CFXJSE_FormCalcContext::Choose(CFXJSE_Value* pThis,
                                    ByteStringView szFuncName,
                                    CFXJSE_Arguments& args) {
  CFXJSE_FormCalcContext* pContext = ToFormCalcContext(pThis);
  int32_t argc = args.GetLength();
  if (argc < 2) {
    pContext->ThrowParamCountMismatchException(L"Choose");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = args.GetValue(0);
  if (ValueIsNull(pThis, argOne.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  int32_t iIndex = (int32_t)ValueToFloat(pThis, argOne.get());
  if (iIndex < 1) {
    args.GetReturnValue()->SetString("");
    return;
  }

  bool bFound = false;
  bool bStopCounterFlags = false;
  int32_t iArgIndex = 1;
  int32_t iValueIndex = 0;
  v8::Isolate* pIsolate = pContext->GetScriptRuntime();
  while (!bFound && !bStopCounterFlags && (iArgIndex < argc)) {
    std::unique_ptr<CFXJSE_Value> argIndexValue = args.GetValue(iArgIndex);
    if (argIndexValue->IsArray()) {
      auto lengthValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
      argIndexValue->GetObjectProperty("length", lengthValue.get());
      int32_t iLength = lengthValue->ToInteger();
      if (iLength > 3)
        bStopCounterFlags = true;

      iValueIndex += (iLength - 2);
      if (iValueIndex >= iIndex) {
        auto propertyValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
        auto jsObjectValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
        auto newPropertyValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
        argIndexValue->GetObjectPropertyByIdx(1, propertyValue.get());
        argIndexValue->GetObjectPropertyByIdx(
            (iLength - 1) - (iValueIndex - iIndex), jsObjectValue.get());
        if (propertyValue->IsNull()) {
          GetObjectDefaultValue(jsObjectValue.get(), newPropertyValue.get());
        } else {
          jsObjectValue->GetObjectProperty(
              propertyValue->ToString().AsStringView(), newPropertyValue.get());
        }
        ByteString bsChosen = ValueToUTF8String(newPropertyValue.get());
        args.GetReturnValue()->SetString(bsChosen.AsStringView());
        bFound = true;
      }
    } else {
      iValueIndex++;
      if (iValueIndex == iIndex) {
        ByteString bsChosen = ValueToUTF8String(argIndexValue.get());
        args.GetReturnValue()->SetString(bsChosen.AsStringView());
        bFound = true;
      }
    }
    iArgIndex++;
  }
  if (!bFound)
    args.GetReturnValue()->SetString("");
}

// static
void CFXJSE_FormCalcContext::Exists(CFXJSE_Value* pThis,
                                    ByteStringView szFuncName,
                                    CFXJSE_Arguments& args) {
  if (args.GetLength() != 1) {
    ToFormCalcContext(pThis)->ThrowParamCountMismatchException(L"Exists");
    return;
  }
  args.GetReturnValue()->SetInteger(args.GetValue(0)->IsObject());
}

// static
void CFXJSE_FormCalcContext::HasValue(CFXJSE_Value* pThis,
                                      ByteStringView szFuncName,
                                      CFXJSE_Arguments& args) {
  if (args.GetLength() != 1) {
    ToFormCalcContext(pThis)->ThrowParamCountMismatchException(L"HasValue");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  if (!argOne->IsString()) {
    args.GetReturnValue()->SetInteger(argOne->IsNumber() ||
                                      argOne->IsBoolean());
    return;
  }

  ByteString valueStr = argOne->ToString();
  valueStr.TrimLeft();
  args.GetReturnValue()->SetInteger(!valueStr.IsEmpty());
}

// static
void CFXJSE_FormCalcContext::Oneof(CFXJSE_Value* pThis,
                                   ByteStringView szFuncName,
                                   CFXJSE_Arguments& args) {
  if (args.GetLength() < 2) {
    ToFormCalcContext(pThis)->ThrowParamCountMismatchException(L"Oneof");
    return;
  }

  bool bFlags = false;
  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  std::vector<std::unique_ptr<CFXJSE_Value>> parameterValues =
      unfoldArgs(pThis, args);
  for (const auto& value : parameterValues) {
    if (simpleValueCompare(pThis, argOne.get(), value.get())) {
      bFlags = true;
      break;
    }
  }

  args.GetReturnValue()->SetInteger(bFlags);
}

// static
void CFXJSE_FormCalcContext::Within(CFXJSE_Value* pThis,
                                    ByteStringView szFuncName,
                                    CFXJSE_Arguments& args) {
  if (args.GetLength() != 3) {
    ToFormCalcContext(pThis)->ThrowParamCountMismatchException(L"Within");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  if (argOne->IsNull()) {
    args.GetReturnValue()->SetUndefined();
    return;
  }

  std::unique_ptr<CFXJSE_Value> argLow = GetSimpleValue(pThis, args, 1);
  std::unique_ptr<CFXJSE_Value> argHigh = GetSimpleValue(pThis, args, 2);
  if (argOne->IsNumber()) {
    float oneNumber = ValueToFloat(pThis, argOne.get());
    float lowNumber = ValueToFloat(pThis, argLow.get());
    float heightNumber = ValueToFloat(pThis, argHigh.get());
    args.GetReturnValue()->SetInteger((oneNumber >= lowNumber) &&
                                      (oneNumber <= heightNumber));
    return;
  }

  ByteString oneString = ValueToUTF8String(argOne.get());
  ByteString lowString = ValueToUTF8String(argLow.get());
  ByteString heightString = ValueToUTF8String(argHigh.get());
  args.GetReturnValue()->SetInteger(
      (oneString.Compare(lowString.AsStringView()) >= 0) &&
      (oneString.Compare(heightString.AsStringView()) <= 0));
}

// static
void CFXJSE_FormCalcContext::If(CFXJSE_Value* pThis,
                                ByteStringView szFuncName,
                                CFXJSE_Arguments& args) {
  if (args.GetLength() != 3) {
    ToFormCalcContext(pThis)->ThrowParamCountMismatchException(L"If");
    return;
  }

  args.GetReturnValue()->Assign(GetSimpleValue(pThis, args, 0)->ToBoolean()
                                    ? GetSimpleValue(pThis, args, 1).get()
                                    : GetSimpleValue(pThis, args, 2).get());
}

// static
void CFXJSE_FormCalcContext::Eval(CFXJSE_Value* pThis,
                                  ByteStringView szFuncName,
                                  CFXJSE_Arguments& args) {
  CFXJSE_FormCalcContext* pContext = ToFormCalcContext(pThis);
  if (args.GetLength() != 1) {
    pContext->ThrowParamCountMismatchException(L"Eval");
    return;
  }

  v8::Isolate* pIsolate = pContext->GetScriptRuntime();
  std::unique_ptr<CFXJSE_Value> scriptValue = GetSimpleValue(pThis, args, 0);
  ByteString utf8ScriptString = ValueToUTF8String(scriptValue.get());
  if (utf8ScriptString.IsEmpty()) {
    args.GetReturnValue()->SetNull();
    return;
  }

  CFX_WideTextBuf wsJavaScriptBuf;
  if (!CFXJSE_FormCalcContext::Translate(
          WideString::FromUTF8(utf8ScriptString.AsStringView()).AsStringView(),
          &wsJavaScriptBuf)) {
    pContext->ThrowCompilerErrorException();
    return;
  }

  std::unique_ptr<CFXJSE_Context> pNewContext(
      CFXJSE_Context::Create(pIsolate, nullptr, nullptr));

  auto returnValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
  pNewContext->ExecuteScript(
      FX_UTF8Encode(wsJavaScriptBuf.AsStringView()).c_str(), returnValue.get(),
      nullptr);

  args.GetReturnValue()->Assign(returnValue.get());
}

// static
void CFXJSE_FormCalcContext::Ref(CFXJSE_Value* pThis,
                                 ByteStringView szFuncName,
                                 CFXJSE_Arguments& args) {
  CFXJSE_FormCalcContext* pContext = ToFormCalcContext(pThis);
  v8::Isolate* pIsolate = pContext->GetScriptRuntime();
  if (args.GetLength() != 1) {
    pContext->ThrowParamCountMismatchException(L"Ref");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = args.GetValue(0);
  if (!argOne->IsArray() && !argOne->IsObject() && !argOne->IsBoolean() &&
      !argOne->IsString() && !argOne->IsNull() && !argOne->IsNumber()) {
    pContext->ThrowArgumentMismatchException();
    return;
  }

  if (argOne->IsBoolean() || argOne->IsString() || argOne->IsNumber()) {
    args.GetReturnValue()->Assign(argOne.get());
    return;
  }

  std::vector<std::unique_ptr<CFXJSE_Value>> values;
  for (int32_t i = 0; i < 3; i++)
    values.push_back(pdfium::MakeUnique<CFXJSE_Value>(pIsolate));

  int intVal = 3;
  if (argOne->IsNull()) {
    // TODO(dsinclair): Why is this 4 when the others are all 3?
    intVal = 4;
    values[2]->SetNull();
  } else if (argOne->IsArray()) {
#ifndef NDEBUG
    auto lengthValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
    argOne->GetObjectProperty("length", lengthValue.get());
    ASSERT(lengthValue->ToInteger() >= 3);
#endif

    auto propertyValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
    auto jsObjectValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
    argOne->GetObjectPropertyByIdx(1, propertyValue.get());
    argOne->GetObjectPropertyByIdx(2, jsObjectValue.get());
    if (!propertyValue->IsNull() || jsObjectValue->IsNull()) {
      pContext->ThrowArgumentMismatchException();
      return;
    }

    values[2]->Assign(jsObjectValue.get());
  } else if (argOne->IsObject()) {
    values[2]->Assign(argOne.get());
  }

  values[0]->SetInteger(intVal);
  values[1]->SetNull();
  args.GetReturnValue()->SetArray(values);
}

// static
void CFXJSE_FormCalcContext::UnitType(CFXJSE_Value* pThis,
                                      ByteStringView szFuncName,
                                      CFXJSE_Arguments& args) {
  if (args.GetLength() != 1) {
    ToFormCalcContext(pThis)->ThrowParamCountMismatchException(L"UnitType");
    return;
  }

  std::unique_ptr<CFXJSE_Value> unitspanValue = GetSimpleValue(pThis, args, 0);
  if (unitspanValue->IsNull()) {
    args.GetReturnValue()->SetNull();
    return;
  }

  ByteString unitspanString = ValueToUTF8String(unitspanValue.get());
  if (unitspanString.IsEmpty()) {
    args.GetReturnValue()->SetString("in");
    return;
  }

  enum XFA_FM2JS_VALUETYPE_ParserStatus {
    VALUETYPE_START,
    VALUETYPE_HAVEINVALIDCHAR,
    VALUETYPE_HAVEDIGIT,
    VALUETYPE_HAVEDIGITWHITE,
    VALUETYPE_ISCM,
    VALUETYPE_ISMM,
    VALUETYPE_ISPT,
    VALUETYPE_ISMP,
    VALUETYPE_ISIN,
  };
  unitspanString.MakeLower();
  WideString wsTypeString = WideString::FromUTF8(unitspanString.AsStringView());
  const wchar_t* pData = wsTypeString.c_str();
  int32_t u = 0;
  int32_t uLen = wsTypeString.GetLength();
  while (IsWhitespace(pData[u]))
    u++;

  XFA_FM2JS_VALUETYPE_ParserStatus eParserStatus = VALUETYPE_START;
  wchar_t typeChar;
  // TODO(dsinclair): Cleanup this parser, figure out what the various checks
  //    are for.
  while (u < uLen) {
    typeChar = pData[u];
    if (IsWhitespace(typeChar)) {
      if (eParserStatus != VALUETYPE_HAVEDIGIT &&
          eParserStatus != VALUETYPE_HAVEDIGITWHITE) {
        eParserStatus = VALUETYPE_ISIN;
        break;
      }
      eParserStatus = VALUETYPE_HAVEDIGITWHITE;
    } else if (IsPartOfNumberW(typeChar)) {
      if (eParserStatus == VALUETYPE_HAVEDIGITWHITE) {
        eParserStatus = VALUETYPE_ISIN;
        break;
      }
      eParserStatus = VALUETYPE_HAVEDIGIT;
    } else if ((typeChar == 'c' || typeChar == 'p') && (u + 1 < uLen)) {
      wchar_t nextChar = pData[u + 1];
      if ((eParserStatus == VALUETYPE_START ||
           eParserStatus == VALUETYPE_HAVEDIGIT ||
           eParserStatus == VALUETYPE_HAVEDIGITWHITE) &&
          !IsPartOfNumberW(nextChar)) {
        eParserStatus = (typeChar == 'c') ? VALUETYPE_ISCM : VALUETYPE_ISPT;
        break;
      }
      eParserStatus = VALUETYPE_HAVEINVALIDCHAR;
    } else if (typeChar == 'm' && (u + 1 < uLen)) {
      wchar_t nextChar = pData[u + 1];
      if ((eParserStatus == VALUETYPE_START ||
           eParserStatus == VALUETYPE_HAVEDIGIT ||
           eParserStatus == VALUETYPE_HAVEDIGITWHITE) &&
          !IsPartOfNumberW(nextChar)) {
        eParserStatus = VALUETYPE_ISMM;
        if (nextChar == 'p' || ((u + 5 < uLen) && pData[u + 1] == 'i' &&
                                pData[u + 2] == 'l' && pData[u + 3] == 'l' &&
                                pData[u + 4] == 'i' && pData[u + 5] == 'p')) {
          eParserStatus = VALUETYPE_ISMP;
        }
        break;
      }
    } else {
      eParserStatus = VALUETYPE_HAVEINVALIDCHAR;
    }
    u++;
  }
  switch (eParserStatus) {
    case VALUETYPE_ISCM:
      args.GetReturnValue()->SetString("cm");
      break;
    case VALUETYPE_ISMM:
      args.GetReturnValue()->SetString("mm");
      break;
    case VALUETYPE_ISPT:
      args.GetReturnValue()->SetString("pt");
      break;
    case VALUETYPE_ISMP:
      args.GetReturnValue()->SetString("mp");
      break;
    default:
      args.GetReturnValue()->SetString("in");
      break;
  }
}

// static
void CFXJSE_FormCalcContext::UnitValue(CFXJSE_Value* pThis,
                                       ByteStringView szFuncName,
                                       CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc < 1 || argc > 2) {
    ToFormCalcContext(pThis)->ThrowParamCountMismatchException(L"UnitValue");
    return;
  }

  std::unique_ptr<CFXJSE_Value> unitspanValue = GetSimpleValue(pThis, args, 0);
  if (unitspanValue->IsNull()) {
    args.GetReturnValue()->SetNull();
    return;
  }

  ByteString unitspanString = ValueToUTF8String(unitspanValue.get());
  const char* pData = unitspanString.c_str();
  if (!pData) {
    args.GetReturnValue()->SetInteger(0);
    return;
  }

  size_t u = 0;
  while (IsWhitespace(pData[u]))
    ++u;

  while (u < unitspanString.GetLength()) {
    if (!IsPartOfNumber(pData[u]))
      break;
    ++u;
  }

  char* pTemp = nullptr;
  double dFirstNumber = strtod(pData, &pTemp);
  while (IsWhitespace(pData[u]))
    ++u;

  size_t uLen = unitspanString.GetLength();
  ByteString strFirstUnit;
  while (u < uLen) {
    if (pData[u] == ' ')
      break;

    strFirstUnit += pData[u];
    ++u;
  }
  strFirstUnit.MakeLower();

  ByteString strUnit;
  if (argc > 1) {
    std::unique_ptr<CFXJSE_Value> unitValue = GetSimpleValue(pThis, args, 1);
    ByteString unitTempString = ValueToUTF8String(unitValue.get());
    const char* pChar = unitTempString.c_str();
    size_t uVal = 0;
    while (IsWhitespace(pChar[uVal]))
      ++uVal;

    while (uVal < unitTempString.GetLength()) {
      if (!std::isdigit(pChar[uVal]) && pChar[uVal] != '.')
        break;
      ++uVal;
    }
    while (IsWhitespace(pChar[uVal]))
      ++uVal;

    size_t uValLen = unitTempString.GetLength();
    while (uVal < uValLen) {
      if (pChar[uVal] == ' ')
        break;

      strUnit += pChar[uVal];
      ++uVal;
    }
    strUnit.MakeLower();
  } else {
    strUnit = strFirstUnit;
  }

  double dResult = 0;
  if (strFirstUnit == "in" || strFirstUnit == "inches") {
    if (strUnit == "mm" || strUnit == "millimeters")
      dResult = dFirstNumber * 25.4;
    else if (strUnit == "cm" || strUnit == "centimeters")
      dResult = dFirstNumber * 2.54;
    else if (strUnit == "pt" || strUnit == "points")
      dResult = dFirstNumber / 72;
    else if (strUnit == "mp" || strUnit == "millipoints")
      dResult = dFirstNumber / 72000;
    else
      dResult = dFirstNumber;
  } else if (strFirstUnit == "mm" || strFirstUnit == "millimeters") {
    if (strUnit == "mm" || strUnit == "millimeters")
      dResult = dFirstNumber;
    else if (strUnit == "cm" || strUnit == "centimeters")
      dResult = dFirstNumber / 10;
    else if (strUnit == "pt" || strUnit == "points")
      dResult = dFirstNumber / 25.4 / 72;
    else if (strUnit == "mp" || strUnit == "millipoints")
      dResult = dFirstNumber / 25.4 / 72000;
    else
      dResult = dFirstNumber / 25.4;
  } else if (strFirstUnit == "cm" || strFirstUnit == "centimeters") {
    if (strUnit == "mm" || strUnit == "millimeters")
      dResult = dFirstNumber * 10;
    else if (strUnit == "cm" || strUnit == "centimeters")
      dResult = dFirstNumber;
    else if (strUnit == "pt" || strUnit == "points")
      dResult = dFirstNumber / 2.54 / 72;
    else if (strUnit == "mp" || strUnit == "millipoints")
      dResult = dFirstNumber / 2.54 / 72000;
    else
      dResult = dFirstNumber / 2.54;
  } else if (strFirstUnit == "pt" || strFirstUnit == "points") {
    if (strUnit == "mm" || strUnit == "millimeters")
      dResult = dFirstNumber / 72 * 25.4;
    else if (strUnit == "cm" || strUnit == "centimeters")
      dResult = dFirstNumber / 72 * 2.54;
    else if (strUnit == "pt" || strUnit == "points")
      dResult = dFirstNumber;
    else if (strUnit == "mp" || strUnit == "millipoints")
      dResult = dFirstNumber * 1000;
    else
      dResult = dFirstNumber / 72;
  } else if (strFirstUnit == "mp" || strFirstUnit == "millipoints") {
    if (strUnit == "mm" || strUnit == "millimeters")
      dResult = dFirstNumber / 72000 * 25.4;
    else if (strUnit == "cm" || strUnit == "centimeters")
      dResult = dFirstNumber / 72000 * 2.54;
    else if (strUnit == "pt" || strUnit == "points")
      dResult = dFirstNumber / 1000;
    else if (strUnit == "mp" || strUnit == "millipoints")
      dResult = dFirstNumber;
    else
      dResult = dFirstNumber / 72000;
  }
  args.GetReturnValue()->SetDouble(dResult);
}

// static
void CFXJSE_FormCalcContext::At(CFXJSE_Value* pThis,
                                ByteStringView szFuncName,
                                CFXJSE_Arguments& args) {
  if (args.GetLength() != 2) {
    ToFormCalcContext(pThis)->ThrowParamCountMismatchException(L"At");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argTwo = GetSimpleValue(pThis, args, 1);
  if (ValueIsNull(pThis, argOne.get()) || ValueIsNull(pThis, argTwo.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  ByteString stringTwo = ValueToUTF8String(argTwo.get());
  if (stringTwo.IsEmpty()) {
    args.GetReturnValue()->SetInteger(1);
    return;
  }

  ByteString stringOne = ValueToUTF8String(argOne.get());
  auto pos = stringOne.Find(stringTwo.AsStringView());
  args.GetReturnValue()->SetInteger(pos.has_value() ? pos.value() + 1 : 0);
}

// static
void CFXJSE_FormCalcContext::Concat(CFXJSE_Value* pThis,
                                    ByteStringView szFuncName,
                                    CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc < 1) {
    ToFormCalcContext(pThis)->ThrowParamCountMismatchException(L"Concat");
    return;
  }

  ByteString resultString;
  bool bAllNull = true;
  for (int32_t i = 0; i < argc; i++) {
    std::unique_ptr<CFXJSE_Value> value = GetSimpleValue(pThis, args, i);
    if (ValueIsNull(pThis, value.get()))
      continue;

    bAllNull = false;
    resultString += ValueToUTF8String(value.get());
  }

  if (bAllNull) {
    args.GetReturnValue()->SetNull();
    return;
  }

  args.GetReturnValue()->SetString(resultString.AsStringView());
}

// static
void CFXJSE_FormCalcContext::Decode(CFXJSE_Value* pThis,
                                    ByteStringView szFuncName,
                                    CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc < 1 || argc > 2) {
    ToFormCalcContext(pThis)->ThrowParamCountMismatchException(L"Decode");
    return;
  }

  if (argc == 1) {
    std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
    if (ValueIsNull(pThis, argOne.get())) {
      args.GetReturnValue()->SetNull();
      return;
    }

    WideString decoded = DecodeURL(
        WideString::FromUTF8(ValueToUTF8String(argOne.get()).AsStringView()));

    args.GetReturnValue()->SetString(
        FX_UTF8Encode(decoded.AsStringView()).AsStringView());
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argTwo = GetSimpleValue(pThis, args, 1);
  if (ValueIsNull(pThis, argOne.get()) || ValueIsNull(pThis, argTwo.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  ByteString toDecodeString = ValueToUTF8String(argOne.get());
  ByteString identifyString = ValueToUTF8String(argTwo.get());
  WideString decoded;

  WideString toDecodeWideString =
      WideString::FromUTF8(toDecodeString.AsStringView());

  if (identifyString.EqualNoCase("html"))
    decoded = DecodeHTML(toDecodeWideString);
  else if (identifyString.EqualNoCase("xml"))
    decoded = DecodeXML(toDecodeWideString);
  else
    decoded = DecodeURL(toDecodeWideString);

  args.GetReturnValue()->SetString(
      FX_UTF8Encode(decoded.AsStringView()).AsStringView());
}

// static
void CFXJSE_FormCalcContext::Encode(CFXJSE_Value* pThis,
                                    ByteStringView szFuncName,
                                    CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc < 1 || argc > 2) {
    ToFormCalcContext(pThis)->ThrowParamCountMismatchException(L"Encode");
    return;
  }

  if (argc == 1) {
    std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
    if (ValueIsNull(pThis, argOne.get())) {
      args.GetReturnValue()->SetNull();
      return;
    }

    WideString encoded = EncodeURL(ValueToUTF8String(argOne.get()));
    args.GetReturnValue()->SetString(
        FX_UTF8Encode(encoded.AsStringView()).AsStringView());
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argTwo = GetSimpleValue(pThis, args, 1);
  if (ValueIsNull(pThis, argOne.get()) || ValueIsNull(pThis, argTwo.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  ByteString toEncodeString = ValueToUTF8String(argOne.get());
  ByteString identifyString = ValueToUTF8String(argTwo.get());
  WideString encoded;
  if (identifyString.EqualNoCase("html"))
    encoded = EncodeHTML(toEncodeString);
  else if (identifyString.EqualNoCase("xml"))
    encoded = EncodeXML(toEncodeString);
  else
    encoded = EncodeURL(toEncodeString);

  args.GetReturnValue()->SetString(
      FX_UTF8Encode(encoded.AsStringView()).AsStringView());
}

// static
void CFXJSE_FormCalcContext::Format(CFXJSE_Value* pThis,
                                    ByteStringView szFuncName,
                                    CFXJSE_Arguments& args) {
  CFXJSE_FormCalcContext* pContext = ToFormCalcContext(pThis);
  if (args.GetLength() < 2) {
    pContext->ThrowParamCountMismatchException(L"Format");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  ByteString szPattern = ValueToUTF8String(argOne.get());

  std::unique_ptr<CFXJSE_Value> argTwo = GetSimpleValue(pThis, args, 1);
  ByteString szValue = ValueToUTF8String(argTwo.get());

  CXFA_Document* pDoc = pContext->GetDocument();
  CXFA_LocaleMgr* pMgr = pDoc->GetLocaleMgr();
  CXFA_Node* pThisNode = ToNode(pDoc->GetScriptContext()->GetThisObject());
  ASSERT(pThisNode);

  LocaleIface* pLocale = pThisNode->GetLocale();
  WideString wsPattern = WideString::FromUTF8(szPattern.AsStringView());
  WideString wsValue = WideString::FromUTF8(szValue.AsStringView());
  bool bPatternIsString;
  uint32_t dwPatternType;
  std::tie(bPatternIsString, dwPatternType) =
      PatternStringType(szPattern.AsStringView());
  if (!bPatternIsString) {
    switch (dwPatternType) {
      case XFA_VT_DATETIME: {
        auto iTChar = wsPattern.Find(L'T');
        if (!iTChar.has_value()) {
          args.GetReturnValue()->SetString("");
          return;
        }
        WideString wsDatePattern(L"date{");
        wsDatePattern += wsPattern.Left(iTChar.value()) + L"} ";

        WideString wsTimePattern(L"time{");
        wsTimePattern +=
            wsPattern.Right(wsPattern.GetLength() - (iTChar.value() + 1)) +
            L"}";
        wsPattern = wsDatePattern + wsTimePattern;
      } break;
      case XFA_VT_DATE: {
        wsPattern = L"date{" + wsPattern + L"}";
      } break;
      case XFA_VT_TIME: {
        wsPattern = L"time{" + wsPattern + L"}";
      } break;
      case XFA_VT_TEXT: {
        wsPattern = L"text{" + wsPattern + L"}";
      } break;
      case XFA_VT_FLOAT: {
        wsPattern = L"num{" + wsPattern + L"}";
      } break;
      default: {
        WideString wsTestPattern = L"num{" + wsPattern + L"}";
        CXFA_LocaleValue tempLocaleValue(XFA_VT_FLOAT, wsValue, wsTestPattern,
                                         pLocale, pMgr);
        if (tempLocaleValue.IsValid()) {
          wsPattern = std::move(wsTestPattern);
          dwPatternType = XFA_VT_FLOAT;
        } else {
          wsPattern = L"text{" + wsPattern + L"}";
          dwPatternType = XFA_VT_TEXT;
        }
      } break;
    }
  }
  CXFA_LocaleValue localeValue(dwPatternType, wsValue, wsPattern, pLocale,
                               pMgr);
  WideString wsRet;
  if (!localeValue.FormatPatterns(wsRet, wsPattern, pLocale,
                                  XFA_VALUEPICTURE_Display)) {
    args.GetReturnValue()->SetString("");
    return;
  }

  args.GetReturnValue()->SetString(wsRet.ToUTF8().AsStringView());
}

// static
void CFXJSE_FormCalcContext::Left(CFXJSE_Value* pThis,
                                  ByteStringView szFuncName,
                                  CFXJSE_Arguments& args) {
  if (args.GetLength() != 2) {
    ToFormCalcContext(pThis)->ThrowParamCountMismatchException(L"Left");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argTwo = GetSimpleValue(pThis, args, 1);
  if ((ValueIsNull(pThis, argOne.get())) ||
      (ValueIsNull(pThis, argTwo.get()))) {
    args.GetReturnValue()->SetNull();
    return;
  }

  ByteString sourceString = ValueToUTF8String(argOne.get());
  int32_t count = std::max(0, ValueToInteger(pThis, argTwo.get()));
  args.GetReturnValue()->SetString(sourceString.Left(count).AsStringView());
}

// static
void CFXJSE_FormCalcContext::Len(CFXJSE_Value* pThis,
                                 ByteStringView szFuncName,
                                 CFXJSE_Arguments& args) {
  if (args.GetLength() != 1) {
    ToFormCalcContext(pThis)->ThrowParamCountMismatchException(L"Len");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  if (ValueIsNull(pThis, argOne.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  ByteString sourceString = ValueToUTF8String(argOne.get());
  args.GetReturnValue()->SetInteger(sourceString.GetLength());
}

// static
void CFXJSE_FormCalcContext::Lower(CFXJSE_Value* pThis,
                                   ByteStringView szFuncName,
                                   CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc < 1 || argc > 2) {
    ToFormCalcContext(pThis)->ThrowParamCountMismatchException(L"Lower");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  if (ValueIsNull(pThis, argOne.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  CFX_WideTextBuf lowStringBuf;
  ByteString argString = ValueToUTF8String(argOne.get());
  WideString wsArgString = WideString::FromUTF8(argString.AsStringView());
  for (wchar_t ch : wsArgString) {
    if ((ch >= 0x41 && ch <= 0x5A) || (ch >= 0xC0 && ch <= 0xDE))
      ch += 32;
    else if (ch == 0x100 || ch == 0x102 || ch == 0x104)
      ch += 1;
    lowStringBuf.AppendChar(ch);
  }
  lowStringBuf.AppendChar(0);

  args.GetReturnValue()->SetString(
      FX_UTF8Encode(lowStringBuf.AsStringView()).AsStringView());
}

// static
void CFXJSE_FormCalcContext::Ltrim(CFXJSE_Value* pThis,
                                   ByteStringView szFuncName,
                                   CFXJSE_Arguments& args) {
  if (args.GetLength() != 1) {
    ToFormCalcContext(pThis)->ThrowParamCountMismatchException(L"Ltrim");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  if (ValueIsNull(pThis, argOne.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  ByteString sourceString = ValueToUTF8String(argOne.get());
  sourceString.TrimLeft();
  args.GetReturnValue()->SetString(sourceString.AsStringView());
}

// static
void CFXJSE_FormCalcContext::Parse(CFXJSE_Value* pThis,
                                   ByteStringView szFuncName,
                                   CFXJSE_Arguments& args) {
  CFXJSE_FormCalcContext* pContext = ToFormCalcContext(pThis);
  if (args.GetLength() != 2) {
    pContext->ThrowParamCountMismatchException(L"Parse");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argTwo = GetSimpleValue(pThis, args, 1);
  if (ValueIsNull(pThis, argTwo.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  ByteString szPattern = ValueToUTF8String(argOne.get());
  ByteString szValue = ValueToUTF8String(argTwo.get());
  CXFA_Document* pDoc = pContext->GetDocument();
  CXFA_LocaleMgr* pMgr = pDoc->GetLocaleMgr();
  CXFA_Node* pThisNode = ToNode(pDoc->GetScriptContext()->GetThisObject());
  ASSERT(pThisNode);

  LocaleIface* pLocale = pThisNode->GetLocale();
  WideString wsPattern = WideString::FromUTF8(szPattern.AsStringView());
  WideString wsValue = WideString::FromUTF8(szValue.AsStringView());
  bool bPatternIsString;
  uint32_t dwPatternType;
  std::tie(bPatternIsString, dwPatternType) =
      PatternStringType(szPattern.AsStringView());
  if (bPatternIsString) {
    CXFA_LocaleValue localeValue(dwPatternType, wsValue, wsPattern, pLocale,
                                 pMgr);
    if (!localeValue.IsValid()) {
      args.GetReturnValue()->SetString("");
      return;
    }
    args.GetReturnValue()->SetString(
        localeValue.GetValue().ToUTF8().AsStringView());
    return;
  }

  switch (dwPatternType) {
    case XFA_VT_DATETIME: {
      auto iTChar = wsPattern.Find(L'T');
      if (!iTChar.has_value()) {
        args.GetReturnValue()->SetString("");
        return;
      }
      WideString wsDatePattern(L"date{" + wsPattern.Left(iTChar.value()) +
                               L"} ");
      WideString wsTimePattern(
          L"time{" +
          wsPattern.Right(wsPattern.GetLength() - (iTChar.value() + 1)) + L"}");
      wsPattern = wsDatePattern + wsTimePattern;
      CXFA_LocaleValue localeValue(dwPatternType, wsValue, wsPattern, pLocale,
                                   pMgr);
      if (!localeValue.IsValid()) {
        args.GetReturnValue()->SetString("");
        return;
      }
      args.GetReturnValue()->SetString(
          localeValue.GetValue().ToUTF8().AsStringView());
      return;
    }
    case XFA_VT_DATE: {
      wsPattern = L"date{" + wsPattern + L"}";
      CXFA_LocaleValue localeValue(dwPatternType, wsValue, wsPattern, pLocale,
                                   pMgr);
      if (!localeValue.IsValid()) {
        args.GetReturnValue()->SetString("");
        return;
      }
      args.GetReturnValue()->SetString(
          localeValue.GetValue().ToUTF8().AsStringView());
      return;
    }
    case XFA_VT_TIME: {
      wsPattern = L"time{" + wsPattern + L"}";
      CXFA_LocaleValue localeValue(dwPatternType, wsValue, wsPattern, pLocale,
                                   pMgr);
      if (!localeValue.IsValid()) {
        args.GetReturnValue()->SetString("");
        return;
      }
      args.GetReturnValue()->SetString(
          localeValue.GetValue().ToUTF8().AsStringView());
      return;
    }
    case XFA_VT_TEXT: {
      wsPattern = L"text{" + wsPattern + L"}";
      CXFA_LocaleValue localeValue(XFA_VT_TEXT, wsValue, wsPattern, pLocale,
                                   pMgr);
      if (!localeValue.IsValid()) {
        args.GetReturnValue()->SetString("");
        return;
      }
      args.GetReturnValue()->SetString(
          localeValue.GetValue().ToUTF8().AsStringView());
      return;
    }
    case XFA_VT_FLOAT: {
      wsPattern = L"num{" + wsPattern + L"}";
      CXFA_LocaleValue localeValue(XFA_VT_FLOAT, wsValue, wsPattern, pLocale,
                                   pMgr);
      if (!localeValue.IsValid()) {
        args.GetReturnValue()->SetString("");
        return;
      }
      args.GetReturnValue()->SetDouble(localeValue.GetDoubleNum());
      return;
    }
    default: {
      WideString wsTestPattern = L"num{" + wsPattern + L"}";
      CXFA_LocaleValue localeValue(XFA_VT_FLOAT, wsValue, wsTestPattern,
                                   pLocale, pMgr);
      if (localeValue.IsValid()) {
        args.GetReturnValue()->SetDouble(localeValue.GetDoubleNum());
        return;
      }

      wsTestPattern = L"text{" + wsPattern + L"}";
      CXFA_LocaleValue localeValue2(XFA_VT_TEXT, wsValue, wsTestPattern,
                                    pLocale, pMgr);
      if (!localeValue2.IsValid()) {
        args.GetReturnValue()->SetString("");
        return;
      }
      args.GetReturnValue()->SetString(
          localeValue2.GetValue().ToUTF8().AsStringView());
      return;
    }
  }
}

// static
void CFXJSE_FormCalcContext::Replace(CFXJSE_Value* pThis,
                                     ByteStringView szFuncName,
                                     CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc < 2 || argc > 3) {
    ToFormCalcContext(pThis)->ThrowParamCountMismatchException(L"Replace");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argTwo = GetSimpleValue(pThis, args, 1);
  ByteString oneString;
  ByteString twoString;
  if (!ValueIsNull(pThis, argOne.get()) && !ValueIsNull(pThis, argTwo.get())) {
    oneString = ValueToUTF8String(argOne.get());
    twoString = ValueToUTF8String(argTwo.get());
  }

  ByteString threeString;
  if (argc > 2) {
    std::unique_ptr<CFXJSE_Value> argThree = GetSimpleValue(pThis, args, 2);
    threeString = ValueToUTF8String(argThree.get());
  }

  size_t iFindLen = twoString.GetLength();
  std::ostringstream resultString;
  size_t iFindIndex = 0;
  for (size_t u = 0; u < oneString.GetLength(); ++u) {
    char ch = static_cast<char>(oneString[u]);
    if (ch != static_cast<char>(twoString[iFindIndex])) {
      resultString << ch;
      continue;
    }

    size_t iTemp = u + 1;
    ++iFindIndex;
    while (iFindIndex < iFindLen) {
      uint8_t chTemp = oneString[iTemp];
      if (chTemp != twoString[iFindIndex]) {
        iFindIndex = 0;
        break;
      }

      ++iTemp;
      ++iFindIndex;
    }
    if (iFindIndex == iFindLen) {
      resultString << threeString;
      u += iFindLen - 1;
      iFindIndex = 0;
    } else {
      resultString << ch;
    }
  }
  resultString << '\0';
  args.GetReturnValue()->SetString(ByteStringView(resultString.str().c_str()));
}

// static
void CFXJSE_FormCalcContext::Right(CFXJSE_Value* pThis,
                                   ByteStringView szFuncName,
                                   CFXJSE_Arguments& args) {
  if (args.GetLength() != 2) {
    ToFormCalcContext(pThis)->ThrowParamCountMismatchException(L"Right");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argTwo = GetSimpleValue(pThis, args, 1);
  if ((ValueIsNull(pThis, argOne.get())) ||
      (ValueIsNull(pThis, argTwo.get()))) {
    args.GetReturnValue()->SetNull();
    return;
  }

  ByteString sourceString = ValueToUTF8String(argOne.get());
  int32_t count = std::max(0, ValueToInteger(pThis, argTwo.get()));
  args.GetReturnValue()->SetString(sourceString.Right(count).AsStringView());
}

// static
void CFXJSE_FormCalcContext::Rtrim(CFXJSE_Value* pThis,
                                   ByteStringView szFuncName,
                                   CFXJSE_Arguments& args) {
  if (args.GetLength() != 1) {
    ToFormCalcContext(pThis)->ThrowParamCountMismatchException(L"Rtrim");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  if (ValueIsNull(pThis, argOne.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  ByteString sourceString = ValueToUTF8String(argOne.get());
  sourceString.TrimRight();
  args.GetReturnValue()->SetString(sourceString.AsStringView());
}

// static
void CFXJSE_FormCalcContext::Space(CFXJSE_Value* pThis,
                                   ByteStringView szFuncName,
                                   CFXJSE_Arguments& args) {
  if (args.GetLength() != 1) {
    ToFormCalcContext(pThis)->ThrowParamCountMismatchException(L"Space");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  if (argOne->IsNull()) {
    args.GetReturnValue()->SetNull();
    return;
  }

  int32_t count = std::max(0, ValueToInteger(pThis, argOne.get()));
  std::ostringstream spaceString;
  int32_t index = 0;
  while (index < count) {
    spaceString << ' ';
    index++;
  }
  spaceString << '\0';
  args.GetReturnValue()->SetString(ByteStringView(spaceString.str().c_str()));
}

// static
void CFXJSE_FormCalcContext::Str(CFXJSE_Value* pThis,
                                 ByteStringView szFuncName,
                                 CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc < 1 || argc > 3) {
    ToFormCalcContext(pThis)->ThrowParamCountMismatchException(L"Str");
    return;
  }

  std::unique_ptr<CFXJSE_Value> numberValue = GetSimpleValue(pThis, args, 0);
  if (numberValue->IsNull()) {
    args.GetReturnValue()->SetNull();
    return;
  }
  float fNumber = ValueToFloat(pThis, numberValue.get());

  int32_t iWidth = 10;
  if (argc > 1) {
    std::unique_ptr<CFXJSE_Value> widthValue = GetSimpleValue(pThis, args, 1);
    iWidth = static_cast<int32_t>(ValueToFloat(pThis, widthValue.get()));
  }

  int32_t iPrecision = 0;
  if (argc > 2) {
    std::unique_ptr<CFXJSE_Value> precisionValue =
        GetSimpleValue(pThis, args, 2);
    iPrecision = std::max(
        0, static_cast<int32_t>(ValueToFloat(pThis, precisionValue.get())));
  }

  ByteString formatStr = "%";
  if (iPrecision) {
    formatStr += ".";
    formatStr += ByteString::FormatInteger(iPrecision);
  }
  formatStr += "f";
  ByteString numberString = ByteString::Format(formatStr.c_str(), fNumber);

  const char* pData = numberString.c_str();
  int32_t iLength = numberString.GetLength();
  int32_t u = 0;
  while (u < iLength) {
    if (pData[u] == '.')
      break;

    ++u;
  }

  std::ostringstream resultBuf;
  if (u > iWidth || (iPrecision + u) >= iWidth) {
    int32_t i = 0;
    while (i < iWidth) {
      resultBuf << '*';
      ++i;
    }
    resultBuf << '\0';
    args.GetReturnValue()->SetString(ByteStringView(resultBuf.str().c_str()));
    return;
  }

  if (u == iLength) {
    if (iLength > iWidth) {
      int32_t i = 0;
      while (i < iWidth) {
        resultBuf << '*';
        ++i;
      }
    } else {
      int32_t i = 0;
      while (i < iWidth - iLength) {
        resultBuf << ' ';
        ++i;
      }
      resultBuf << pData;
    }
    args.GetReturnValue()->SetString(ByteStringView(resultBuf.str().c_str()));
    return;
  }

  int32_t iLeavingSpace = iWidth - u - iPrecision;
  if (iPrecision != 0)
    iLeavingSpace--;

  int32_t i = 0;
  while (i < iLeavingSpace) {
    resultBuf << ' ';
    ++i;
  }
  i = 0;
  while (i < u) {
    resultBuf << pData[i];
    ++i;
  }
  if (iPrecision != 0)
    resultBuf << '.';

  u++;
  i = 0;
  while (u < iLength) {
    if (i >= iPrecision)
      break;

    resultBuf << pData[u];
    ++i;
    ++u;
  }
  while (i < iPrecision) {
    resultBuf << '0';
    ++i;
  }
  resultBuf << '\0';
  args.GetReturnValue()->SetString(ByteStringView(resultBuf.str().c_str()));
}

// static
void CFXJSE_FormCalcContext::Stuff(CFXJSE_Value* pThis,
                                   ByteStringView szFuncName,
                                   CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc < 3 || argc > 4) {
    ToFormCalcContext(pThis)->ThrowParamCountMismatchException(L"Stuff");
    return;
  }

  ByteString sourceString;
  ByteString insertString;
  int32_t iLength = 0;
  int32_t iStart = 0;
  int32_t iDelete = 0;
  std::unique_ptr<CFXJSE_Value> sourceValue = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> startValue = GetSimpleValue(pThis, args, 1);
  std::unique_ptr<CFXJSE_Value> deleteValue = GetSimpleValue(pThis, args, 2);
  if (!sourceValue->IsNull() && !startValue->IsNull() &&
      !deleteValue->IsNull()) {
    sourceString = ValueToUTF8String(sourceValue.get());
    iLength = sourceString.GetLength();
    iStart = pdfium::clamp(
        static_cast<int32_t>(ValueToFloat(pThis, startValue.get())), 1,
        iLength);
    iDelete = std::max(
        0, static_cast<int32_t>(ValueToFloat(pThis, deleteValue.get())));
  }

  if (argc > 3) {
    std::unique_ptr<CFXJSE_Value> insertValue = GetSimpleValue(pThis, args, 3);
    insertString = ValueToUTF8String(insertValue.get());
  }

  iStart -= 1;
  std::ostringstream resultString;
  int32_t i = 0;
  while (i < iStart) {
    resultString << static_cast<char>(sourceString[i]);
    ++i;
  }
  resultString << insertString.AsStringView();
  i = iStart + iDelete;
  while (i < iLength) {
    resultString << static_cast<char>(sourceString[i]);
    ++i;
  }
  resultString << '\0';
  args.GetReturnValue()->SetString(ByteStringView(resultString.str().c_str()));
}

// static
void CFXJSE_FormCalcContext::Substr(CFXJSE_Value* pThis,
                                    ByteStringView szFuncName,
                                    CFXJSE_Arguments& args) {
  if (args.GetLength() != 3) {
    ToFormCalcContext(pThis)->ThrowParamCountMismatchException(L"Substr");
    return;
  }

  std::unique_ptr<CFXJSE_Value> stringValue = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> startValue = GetSimpleValue(pThis, args, 1);
  std::unique_ptr<CFXJSE_Value> endValue = GetSimpleValue(pThis, args, 2);
  if (ValueIsNull(pThis, stringValue.get()) ||
      (ValueIsNull(pThis, startValue.get())) ||
      (ValueIsNull(pThis, endValue.get()))) {
    args.GetReturnValue()->SetNull();
    return;
  }

  int32_t iStart = 0;
  int32_t iCount = 0;
  ByteString szSourceStr = ValueToUTF8String(stringValue.get());
  int32_t iLength = szSourceStr.GetLength();
  if (iLength == 0) {
    args.GetReturnValue()->SetString("");
    return;
  }

  iStart = pdfium::clamp(
      iLength, 1, static_cast<int32_t>(ValueToFloat(pThis, startValue.get())));
  iCount =
      std::max(0, static_cast<int32_t>(ValueToFloat(pThis, endValue.get())));

  iStart -= 1;
  args.GetReturnValue()->SetString(
      szSourceStr.Mid(iStart, iCount).AsStringView());
}

// static
void CFXJSE_FormCalcContext::Uuid(CFXJSE_Value* pThis,
                                  ByteStringView szFuncName,
                                  CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc < 0 || argc > 1) {
    ToFormCalcContext(pThis)->ThrowParamCountMismatchException(L"Uuid");
    return;
  }

  int32_t iNum = 0;
  if (argc > 0) {
    std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
    iNum = static_cast<int32_t>(ValueToFloat(pThis, argOne.get()));
  }
  args.GetReturnValue()->SetString(GUIDString(!!iNum).AsStringView());
}

// static
void CFXJSE_FormCalcContext::Upper(CFXJSE_Value* pThis,
                                   ByteStringView szFuncName,
                                   CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc < 1 || argc > 2) {
    ToFormCalcContext(pThis)->ThrowParamCountMismatchException(L"Upper");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  if (ValueIsNull(pThis, argOne.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  CFX_WideTextBuf upperStringBuf;
  ByteString argString = ValueToUTF8String(argOne.get());
  WideString wsArgString = WideString::FromUTF8(argString.AsStringView());
  const wchar_t* pData = wsArgString.c_str();
  size_t i = 0;
  while (i < wsArgString.GetLength()) {
    int32_t ch = pData[i];
    if ((ch >= 0x61 && ch <= 0x7A) || (ch >= 0xE0 && ch <= 0xFE))
      ch -= 32;
    else if (ch == 0x101 || ch == 0x103 || ch == 0x105)
      ch -= 1;

    upperStringBuf.AppendChar(ch);
    ++i;
  }
  upperStringBuf.AppendChar(0);

  args.GetReturnValue()->SetString(
      FX_UTF8Encode(upperStringBuf.AsStringView()).AsStringView());
}

// static
void CFXJSE_FormCalcContext::WordNum(CFXJSE_Value* pThis,
                                     ByteStringView szFuncName,
                                     CFXJSE_Arguments& args) {
  int32_t argc = args.GetLength();
  if (argc < 1 || argc > 3) {
    ToFormCalcContext(pThis)->ThrowParamCountMismatchException(L"WordNum");
    return;
  }

  std::unique_ptr<CFXJSE_Value> numberValue = GetSimpleValue(pThis, args, 0);
  if (numberValue->IsNull()) {
    args.GetReturnValue()->SetNull();
    return;
  }
  float fNumber = ValueToFloat(pThis, numberValue.get());

  int32_t iIdentifier = 0;
  if (argc > 1) {
    std::unique_ptr<CFXJSE_Value> identifierValue =
        GetSimpleValue(pThis, args, 1);
    if (identifierValue->IsNull()) {
      args.GetReturnValue()->SetNull();
      return;
    }
    iIdentifier =
        static_cast<int32_t>(ValueToFloat(pThis, identifierValue.get()));
  }

  ByteString localeString;
  if (argc > 2) {
    std::unique_ptr<CFXJSE_Value> localeValue = GetSimpleValue(pThis, args, 2);
    if (localeValue->IsNull()) {
      args.GetReturnValue()->SetNull();
      return;
    }
    localeString = ValueToUTF8String(localeValue.get());
  }

  if (fNumber < 0.0f || fNumber > 922337203685477550.0f) {
    args.GetReturnValue()->SetString("*");
    return;
  }

  args.GetReturnValue()->SetString(
      WordUS(ByteString::Format("%.2f", fNumber), iIdentifier).AsStringView());
}

// static
void CFXJSE_FormCalcContext::Get(CFXJSE_Value* pThis,
                                 ByteStringView szFuncName,
                                 CFXJSE_Arguments& args) {
  CFXJSE_FormCalcContext* pContext = ToFormCalcContext(pThis);
  if (args.GetLength() != 1) {
    pContext->ThrowParamCountMismatchException(L"Get");
    return;
  }

  CXFA_Document* pDoc = pContext->GetDocument();
  if (!pDoc)
    return;

  IXFA_AppProvider* pAppProvider = pDoc->GetNotify()->GetAppProvider();
  if (!pAppProvider)
    return;

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  ByteString urlString = ValueToUTF8String(argOne.get());
  RetainPtr<IFX_SeekableReadStream> pFile =
      pAppProvider->DownloadURL(WideString::FromUTF8(urlString.AsStringView()));
  if (!pFile)
    return;

  int32_t size = pFile->GetSize();
  std::vector<uint8_t> dataBuf(size);
  pFile->ReadBlock(dataBuf.data(), size);
  args.GetReturnValue()->SetString(ByteStringView(dataBuf));
}

// static
void CFXJSE_FormCalcContext::Post(CFXJSE_Value* pThis,
                                  ByteStringView szFuncName,
                                  CFXJSE_Arguments& args) {
  CFXJSE_FormCalcContext* pContext = ToFormCalcContext(pThis);
  int32_t argc = args.GetLength();
  if (argc < 2 || argc > 5) {
    pContext->ThrowParamCountMismatchException(L"Post");
    return;
  }

  CXFA_Document* pDoc = pContext->GetDocument();
  if (!pDoc)
    return;

  IXFA_AppProvider* pAppProvider = pDoc->GetNotify()->GetAppProvider();
  if (!pAppProvider)
    return;

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  ByteString bsURL = ValueToUTF8String(argOne.get());

  std::unique_ptr<CFXJSE_Value> argTwo = GetSimpleValue(pThis, args, 1);
  ByteString bsData = ValueToUTF8String(argTwo.get());

  ByteString bsContentType;
  if (argc > 2) {
    std::unique_ptr<CFXJSE_Value> argThree = GetSimpleValue(pThis, args, 2);
    bsContentType = ValueToUTF8String(argThree.get());
  }

  ByteString bsEncode;
  if (argc > 3) {
    std::unique_ptr<CFXJSE_Value> argFour = GetSimpleValue(pThis, args, 3);
    bsEncode = ValueToUTF8String(argFour.get());
  }

  ByteString bsHeader;
  if (argc > 4) {
    std::unique_ptr<CFXJSE_Value> argFive = GetSimpleValue(pThis, args, 4);
    bsHeader = ValueToUTF8String(argFive.get());
  }

  WideString decodedResponse;
  if (!pAppProvider->PostRequestURL(
          WideString::FromUTF8(bsURL.AsStringView()),
          WideString::FromUTF8(bsData.AsStringView()),
          WideString::FromUTF8(bsContentType.AsStringView()),
          WideString::FromUTF8(bsEncode.AsStringView()),
          WideString::FromUTF8(bsHeader.AsStringView()), decodedResponse)) {
    pContext->ThrowServerDeniedException();
    return;
  }
  args.GetReturnValue()->SetString(decodedResponse.ToUTF8().AsStringView());
}

// static
void CFXJSE_FormCalcContext::Put(CFXJSE_Value* pThis,
                                 ByteStringView szFuncName,
                                 CFXJSE_Arguments& args) {
  CFXJSE_FormCalcContext* pContext = ToFormCalcContext(pThis);
  int32_t argc = args.GetLength();
  if (argc < 2 || argc > 3) {
    pContext->ThrowParamCountMismatchException(L"Put");
    return;
  }

  CXFA_Document* pDoc = pContext->GetDocument();
  if (!pDoc)
    return;

  IXFA_AppProvider* pAppProvider = pDoc->GetNotify()->GetAppProvider();
  if (!pAppProvider)
    return;

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  ByteString bsURL = ValueToUTF8String(argOne.get());

  std::unique_ptr<CFXJSE_Value> argTwo = GetSimpleValue(pThis, args, 1);
  ByteString bsData = ValueToUTF8String(argTwo.get());

  ByteString bsEncode;
  if (argc > 2) {
    std::unique_ptr<CFXJSE_Value> argThree = GetSimpleValue(pThis, args, 2);
    bsEncode = ValueToUTF8String(argThree.get());
  }

  if (!pAppProvider->PutRequestURL(
          WideString::FromUTF8(bsURL.AsStringView()),
          WideString::FromUTF8(bsData.AsStringView()),
          WideString::FromUTF8(bsEncode.AsStringView()))) {
    pContext->ThrowServerDeniedException();
    return;
  }

  args.GetReturnValue()->SetString("");
}

// static
void CFXJSE_FormCalcContext::assign_value_operator(CFXJSE_Value* pThis,
                                                   ByteStringView szFuncName,
                                                   CFXJSE_Arguments& args) {
  CFXJSE_FormCalcContext* pContext = ToFormCalcContext(pThis);
  if (args.GetLength() != 2) {
    pContext->ThrowCompilerErrorException();
    return;
  }

  std::unique_ptr<CFXJSE_Value> lValue = args.GetValue(0);
  std::unique_ptr<CFXJSE_Value> rValue = GetSimpleValue(pThis, args, 1);
  if (lValue->IsArray()) {
    v8::Isolate* pIsolate = pContext->GetScriptRuntime();
    auto leftLengthValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
    lValue->GetObjectProperty("length", leftLengthValue.get());
    int32_t iLeftLength = leftLengthValue->ToInteger();
    auto jsObjectValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
    auto propertyValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
    lValue->GetObjectPropertyByIdx(1, propertyValue.get());
    if (propertyValue->IsNull()) {
      for (int32_t i = 2; i < iLeftLength; i++) {
        lValue->GetObjectPropertyByIdx(i, jsObjectValue.get());
        if (!SetObjectDefaultValue(jsObjectValue.get(), rValue.get())) {
          pContext->ThrowNoDefaultPropertyException(szFuncName);
          return;
        }
      }
    } else {
      for (int32_t i = 2; i < iLeftLength; i++) {
        lValue->GetObjectPropertyByIdx(i, jsObjectValue.get());
        jsObjectValue->SetObjectProperty(
            propertyValue->ToString().AsStringView(), rValue.get());
      }
    }
  } else if (lValue->IsObject()) {
    if (!SetObjectDefaultValue(lValue.get(), rValue.get())) {
      pContext->ThrowNoDefaultPropertyException(szFuncName);
      return;
    }
  }
  args.GetReturnValue()->Assign(rValue.get());
}

// static
void CFXJSE_FormCalcContext::logical_or_operator(CFXJSE_Value* pThis,
                                                 ByteStringView szFuncName,
                                                 CFXJSE_Arguments& args) {
  if (args.GetLength() != 2) {
    ToFormCalcContext(pThis)->ThrowCompilerErrorException();
    return;
  }

  std::unique_ptr<CFXJSE_Value> argFirst = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argSecond = GetSimpleValue(pThis, args, 1);
  if (argFirst->IsNull() && argSecond->IsNull()) {
    args.GetReturnValue()->SetNull();
    return;
  }

  float first = ValueToFloat(pThis, argFirst.get());
  float second = ValueToFloat(pThis, argSecond.get());
  args.GetReturnValue()->SetInteger((first || second) ? 1 : 0);
}

// static
void CFXJSE_FormCalcContext::logical_and_operator(CFXJSE_Value* pThis,
                                                  ByteStringView szFuncName,
                                                  CFXJSE_Arguments& args) {
  if (args.GetLength() != 2) {
    ToFormCalcContext(pThis)->ThrowCompilerErrorException();
    return;
  }

  std::unique_ptr<CFXJSE_Value> argFirst = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argSecond = GetSimpleValue(pThis, args, 1);
  if (argFirst->IsNull() && argSecond->IsNull()) {
    args.GetReturnValue()->SetNull();
    return;
  }

  float first = ValueToFloat(pThis, argFirst.get());
  float second = ValueToFloat(pThis, argSecond.get());
  args.GetReturnValue()->SetInteger((first && second) ? 1 : 0);
}

// static
void CFXJSE_FormCalcContext::equality_operator(CFXJSE_Value* pThis,
                                               ByteStringView szFuncName,
                                               CFXJSE_Arguments& args) {
  if (args.GetLength() != 2) {
    ToFormCalcContext(pThis)->ThrowCompilerErrorException();
    return;
  }

  if (fm_ref_equal(pThis, args)) {
    args.GetReturnValue()->SetInteger(1);
    return;
  }

  std::unique_ptr<CFXJSE_Value> argFirst = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argSecond = GetSimpleValue(pThis, args, 1);
  if (argFirst->IsNull() || argSecond->IsNull()) {
    args.GetReturnValue()->SetInteger(
        (argFirst->IsNull() && argSecond->IsNull()) ? 1 : 0);
    return;
  }

  if (argFirst->IsString() && argSecond->IsString()) {
    args.GetReturnValue()->SetInteger(argFirst->ToString() ==
                                      argSecond->ToString());
    return;
  }

  double first = ValueToDouble(pThis, argFirst.get());
  double second = ValueToDouble(pThis, argSecond.get());
  args.GetReturnValue()->SetInteger((first == second) ? 1 : 0);
}

// static
void CFXJSE_FormCalcContext::notequality_operator(CFXJSE_Value* pThis,
                                                  ByteStringView szFuncName,
                                                  CFXJSE_Arguments& args) {
  if (args.GetLength() != 2) {
    ToFormCalcContext(pThis)->ThrowCompilerErrorException();
    return;
  }

  if (fm_ref_equal(pThis, args)) {
    args.GetReturnValue()->SetInteger(0);
    return;
  }

  std::unique_ptr<CFXJSE_Value> argFirst = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argSecond = GetSimpleValue(pThis, args, 1);
  if (argFirst->IsNull() || argSecond->IsNull()) {
    args.GetReturnValue()->SetInteger(
        (argFirst->IsNull() && argSecond->IsNull()) ? 0 : 1);
    return;
  }

  if (argFirst->IsString() && argSecond->IsString()) {
    args.GetReturnValue()->SetInteger(argFirst->ToString() !=
                                      argSecond->ToString());
    return;
  }

  double first = ValueToDouble(pThis, argFirst.get());
  double second = ValueToDouble(pThis, argSecond.get());
  args.GetReturnValue()->SetInteger(first != second);
}

// static
bool CFXJSE_FormCalcContext::fm_ref_equal(CFXJSE_Value* pThis,
                                          CFXJSE_Arguments& args) {
  std::unique_ptr<CFXJSE_Value> argFirst = args.GetValue(0);
  std::unique_ptr<CFXJSE_Value> argSecond = args.GetValue(1);
  if (!argFirst->IsArray() || !argSecond->IsArray())
    return false;

  v8::Isolate* pIsolate = ToFormCalcContext(pThis)->GetScriptRuntime();
  auto firstFlagValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
  auto secondFlagValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
  argFirst->GetObjectPropertyByIdx(0, firstFlagValue.get());
  argSecond->GetObjectPropertyByIdx(0, secondFlagValue.get());
  if (firstFlagValue->ToInteger() != 3 || secondFlagValue->ToInteger() != 3)
    return false;

  auto firstJSObject = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
  auto secondJSObject = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
  argFirst->GetObjectPropertyByIdx(2, firstJSObject.get());
  argSecond->GetObjectPropertyByIdx(2, secondJSObject.get());
  if (firstJSObject->IsNull() || secondJSObject->IsNull())
    return false;

  return firstJSObject->ToHostObject() == secondJSObject->ToHostObject();
}

// static
void CFXJSE_FormCalcContext::less_operator(CFXJSE_Value* pThis,
                                           ByteStringView szFuncName,
                                           CFXJSE_Arguments& args) {
  if (args.GetLength() != 2) {
    ToFormCalcContext(pThis)->ThrowCompilerErrorException();
    return;
  }

  std::unique_ptr<CFXJSE_Value> argFirst = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argSecond = GetSimpleValue(pThis, args, 1);
  if (argFirst->IsNull() || argSecond->IsNull()) {
    args.GetReturnValue()->SetInteger(0);
    return;
  }

  if (argFirst->IsString() && argSecond->IsString()) {
    int result =
        argFirst->ToString().Compare(argSecond->ToString().AsStringView()) < 0;
    args.GetReturnValue()->SetInteger(result);
    return;
  }

  double first = ValueToDouble(pThis, argFirst.get());
  double second = ValueToDouble(pThis, argSecond.get());
  args.GetReturnValue()->SetInteger((first < second) ? 1 : 0);
}

// static
void CFXJSE_FormCalcContext::lessequal_operator(CFXJSE_Value* pThis,
                                                ByteStringView szFuncName,
                                                CFXJSE_Arguments& args) {
  if (args.GetLength() != 2) {
    ToFormCalcContext(pThis)->ThrowCompilerErrorException();
    return;
  }

  std::unique_ptr<CFXJSE_Value> argFirst = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argSecond = GetSimpleValue(pThis, args, 1);
  if (argFirst->IsNull() || argSecond->IsNull()) {
    args.GetReturnValue()->SetInteger(
        (argFirst->IsNull() && argSecond->IsNull()) ? 1 : 0);
    return;
  }

  if (argFirst->IsString() && argSecond->IsString()) {
    int result =
        argFirst->ToString().Compare(argSecond->ToString().AsStringView()) <= 0;
    args.GetReturnValue()->SetInteger(result);
    return;
  }

  double first = ValueToDouble(pThis, argFirst.get());
  double second = ValueToDouble(pThis, argSecond.get());
  args.GetReturnValue()->SetInteger((first <= second) ? 1 : 0);
}

// static
void CFXJSE_FormCalcContext::greater_operator(CFXJSE_Value* pThis,
                                              ByteStringView szFuncName,
                                              CFXJSE_Arguments& args) {
  if (args.GetLength() != 2) {
    ToFormCalcContext(pThis)->ThrowCompilerErrorException();
    return;
  }

  std::unique_ptr<CFXJSE_Value> argFirst = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argSecond = GetSimpleValue(pThis, args, 1);
  if (argFirst->IsNull() || argSecond->IsNull()) {
    args.GetReturnValue()->SetInteger(0);
    return;
  }

  if (argFirst->IsString() && argSecond->IsString()) {
    int result =
        argFirst->ToString().Compare(argSecond->ToString().AsStringView()) > 0;
    args.GetReturnValue()->SetInteger(result);
    return;
  }

  double first = ValueToDouble(pThis, argFirst.get());
  double second = ValueToDouble(pThis, argSecond.get());
  args.GetReturnValue()->SetInteger((first > second) ? 1 : 0);
}

// static
void CFXJSE_FormCalcContext::greaterequal_operator(CFXJSE_Value* pThis,
                                                   ByteStringView szFuncName,
                                                   CFXJSE_Arguments& args) {
  if (args.GetLength() != 2) {
    ToFormCalcContext(pThis)->ThrowCompilerErrorException();
    return;
  }

  std::unique_ptr<CFXJSE_Value> argFirst = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argSecond = GetSimpleValue(pThis, args, 1);
  if (argFirst->IsNull() || argSecond->IsNull()) {
    args.GetReturnValue()->SetInteger(
        (argFirst->IsNull() && argSecond->IsNull()) ? 1 : 0);
    return;
  }

  if (argFirst->IsString() && argSecond->IsString()) {
    int result =
        argFirst->ToString().Compare(argSecond->ToString().AsStringView()) >= 0;
    args.GetReturnValue()->SetInteger(result);
    return;
  }

  double first = ValueToDouble(pThis, argFirst.get());
  double second = ValueToDouble(pThis, argSecond.get());
  args.GetReturnValue()->SetInteger((first >= second) ? 1 : 0);
}

// static
void CFXJSE_FormCalcContext::plus_operator(CFXJSE_Value* pThis,
                                           ByteStringView szFuncName,
                                           CFXJSE_Arguments& args) {
  if (args.GetLength() != 2) {
    ToFormCalcContext(pThis)->ThrowCompilerErrorException();
    return;
  }

  std::unique_ptr<CFXJSE_Value> argFirst = args.GetValue(0);
  std::unique_ptr<CFXJSE_Value> argSecond = args.GetValue(1);
  if (ValueIsNull(pThis, argFirst.get()) &&
      ValueIsNull(pThis, argSecond.get())) {
    args.GetReturnValue()->SetNull();
    return;
  }

  double first = ValueToDouble(pThis, argFirst.get());
  double second = ValueToDouble(pThis, argSecond.get());
  args.GetReturnValue()->SetDouble(first + second);
}

// static
void CFXJSE_FormCalcContext::minus_operator(CFXJSE_Value* pThis,
                                            ByteStringView szFuncName,
                                            CFXJSE_Arguments& args) {
  if (args.GetLength() != 2) {
    ToFormCalcContext(pThis)->ThrowCompilerErrorException();
    return;
  }

  std::unique_ptr<CFXJSE_Value> argFirst = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argSecond = GetSimpleValue(pThis, args, 1);
  if (argFirst->IsNull() && argSecond->IsNull()) {
    args.GetReturnValue()->SetNull();
    return;
  }

  double first = ValueToDouble(pThis, argFirst.get());
  double second = ValueToDouble(pThis, argSecond.get());
  args.GetReturnValue()->SetDouble(first - second);
}

// static
void CFXJSE_FormCalcContext::multiple_operator(CFXJSE_Value* pThis,
                                               ByteStringView szFuncName,
                                               CFXJSE_Arguments& args) {
  if (args.GetLength() != 2) {
    ToFormCalcContext(pThis)->ThrowCompilerErrorException();
    return;
  }

  std::unique_ptr<CFXJSE_Value> argFirst = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argSecond = GetSimpleValue(pThis, args, 1);
  if (argFirst->IsNull() && argSecond->IsNull()) {
    args.GetReturnValue()->SetNull();
    return;
  }

  double first = ValueToDouble(pThis, argFirst.get());
  double second = ValueToDouble(pThis, argSecond.get());
  args.GetReturnValue()->SetDouble(first * second);
}

// static
void CFXJSE_FormCalcContext::divide_operator(CFXJSE_Value* pThis,
                                             ByteStringView szFuncName,
                                             CFXJSE_Arguments& args) {
  CFXJSE_FormCalcContext* pContext = ToFormCalcContext(pThis);
  if (args.GetLength() != 2) {
    pContext->ThrowCompilerErrorException();
    return;
  }

  std::unique_ptr<CFXJSE_Value> argFirst = GetSimpleValue(pThis, args, 0);
  std::unique_ptr<CFXJSE_Value> argSecond = GetSimpleValue(pThis, args, 1);
  if (argFirst->IsNull() && argSecond->IsNull()) {
    args.GetReturnValue()->SetNull();
    return;
  }

  double second = ValueToDouble(pThis, argSecond.get());
  if (second == 0.0) {
    pContext->ThrowDivideByZeroException();
    return;
  }

  double first = ValueToDouble(pThis, argFirst.get());
  args.GetReturnValue()->SetDouble(first / second);
}

// static
void CFXJSE_FormCalcContext::positive_operator(CFXJSE_Value* pThis,
                                               ByteStringView szFuncName,
                                               CFXJSE_Arguments& args) {
  if (args.GetLength() != 1) {
    ToFormCalcContext(pThis)->ThrowCompilerErrorException();
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  if (argOne->IsNull()) {
    args.GetReturnValue()->SetNull();
    return;
  }
  args.GetReturnValue()->SetDouble(0.0 + ValueToDouble(pThis, argOne.get()));
}

// static
void CFXJSE_FormCalcContext::negative_operator(CFXJSE_Value* pThis,
                                               ByteStringView szFuncName,
                                               CFXJSE_Arguments& args) {
  if (args.GetLength() != 1) {
    ToFormCalcContext(pThis)->ThrowCompilerErrorException();
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  if (argOne->IsNull()) {
    args.GetReturnValue()->SetNull();
    return;
  }
  args.GetReturnValue()->SetDouble(0.0 - ValueToDouble(pThis, argOne.get()));
}

// static
void CFXJSE_FormCalcContext::logical_not_operator(CFXJSE_Value* pThis,
                                                  ByteStringView szFuncName,
                                                  CFXJSE_Arguments& args) {
  if (args.GetLength() != 1) {
    ToFormCalcContext(pThis)->ThrowCompilerErrorException();
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  if (argOne->IsNull()) {
    args.GetReturnValue()->SetNull();
    return;
  }

  double first = ValueToDouble(pThis, argOne.get());
  args.GetReturnValue()->SetInteger((first == 0.0) ? 1 : 0);
}

// static
void CFXJSE_FormCalcContext::dot_accessor(CFXJSE_Value* pThis,
                                          ByteStringView szFuncName,
                                          CFXJSE_Arguments& args) {
  CFXJSE_FormCalcContext* pContext = ToFormCalcContext(pThis);
  v8::Isolate* pIsolate = pContext->GetScriptRuntime();
  int32_t argc = args.GetLength();
  if (argc < 4 || argc > 5) {
    pContext->ThrowCompilerErrorException();
    return;
  }

  bool bIsStar = true;
  int32_t iIndexValue = 0;
  if (argc > 4) {
    bIsStar = false;
    iIndexValue = ValueToInteger(pThis, args.GetValue(4).get());
  }

  ByteString szName = args.GetUTF8String(2);
  ByteString szSomExp = GenerateSomExpression(
      szName.AsStringView(), args.GetInt32(3), iIndexValue, bIsStar);

  std::unique_ptr<CFXJSE_Value> argAccessor = args.GetValue(0);
  if (argAccessor->IsArray()) {
    auto pLengthValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
    argAccessor->GetObjectProperty("length", pLengthValue.get());
    int32_t iLength = pLengthValue->ToInteger();
    if (iLength < 3) {
      pContext->ThrowArgumentMismatchException();
      return;
    }

    auto hJSObjValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
    std::vector<std::vector<std::unique_ptr<CFXJSE_Value>>> resolveValues(
        iLength - 2);
    bool bAttribute = false;
    int32_t iCounter = 0;
    for (int32_t i = 2; i < iLength; i++) {
      argAccessor->GetObjectPropertyByIdx(i, hJSObjValue.get());

      XFA_RESOLVENODE_RS resolveNodeRS;
      if (ResolveObjects(pThis, hJSObjValue.get(), szSomExp.AsStringView(),
                         &resolveNodeRS, true, szName.IsEmpty())) {
        ParseResolveResult(pThis, resolveNodeRS, hJSObjValue.get(),
                           &resolveValues[i - 2], &bAttribute);
        iCounter += resolveValues[i - 2].size();
      }
    }
    if (iCounter < 1) {
      pContext->ThrowPropertyNotInObjectException(
          WideString::FromUTF8(szName.AsStringView()),
          WideString::FromUTF8(szSomExp.AsStringView()));
      return;
    }

    std::vector<std::unique_ptr<CFXJSE_Value>> values;
    for (int32_t i = 0; i < iCounter + 2; i++)
      values.push_back(pdfium::MakeUnique<CFXJSE_Value>(pIsolate));

    values[0]->SetInteger(1);
    if (bAttribute)
      values[1]->SetString(szName.AsStringView());
    else
      values[1]->SetNull();

    int32_t iIndex = 2;
    for (int32_t i = 0; i < iLength - 2; i++) {
      for (size_t j = 0; j < resolveValues[i].size(); j++) {
        values[iIndex]->Assign(resolveValues[i][j].get());
        iIndex++;
      }
    }
    args.GetReturnValue()->SetArray(values);
    return;
  }

  XFA_RESOLVENODE_RS resolveNodeRS;
  bool iRet = false;
  ByteString bsAccessorName = args.GetUTF8String(1);
  if (argAccessor->IsObject() ||
      (argAccessor->IsNull() && bsAccessorName.IsEmpty())) {
    iRet = ResolveObjects(pThis, argAccessor.get(), szSomExp.AsStringView(),
                          &resolveNodeRS, true, szName.IsEmpty());
  } else if (!argAccessor->IsObject() && !bsAccessorName.IsEmpty() &&
             GetObjectForName(pThis, argAccessor.get(),
                              bsAccessorName.AsStringView())) {
    iRet = ResolveObjects(pThis, argAccessor.get(), szSomExp.AsStringView(),
                          &resolveNodeRS, true, szName.IsEmpty());
  }
  if (!iRet) {
    pContext->ThrowPropertyNotInObjectException(
        WideString::FromUTF8(szName.AsStringView()),
        WideString::FromUTF8(szSomExp.AsStringView()));
    return;
  }

  std::vector<std::unique_ptr<CFXJSE_Value>> resolveValues;
  bool bAttribute = false;
  ParseResolveResult(pThis, resolveNodeRS, argAccessor.get(), &resolveValues,
                     &bAttribute);

  std::vector<std::unique_ptr<CFXJSE_Value>> values;
  for (size_t i = 0; i < resolveValues.size() + 2; i++)
    values.push_back(pdfium::MakeUnique<CFXJSE_Value>(pIsolate));

  values[0]->SetInteger(1);
  if (bAttribute)
    values[1]->SetString(szName.AsStringView());
  else
    values[1]->SetNull();

  for (size_t i = 0; i < resolveValues.size(); i++)
    values[i + 2]->Assign(resolveValues[i].get());

  args.GetReturnValue()->SetArray(values);
}

// static
void CFXJSE_FormCalcContext::dotdot_accessor(CFXJSE_Value* pThis,
                                             ByteStringView szFuncName,
                                             CFXJSE_Arguments& args) {
  CFXJSE_FormCalcContext* pContext = ToFormCalcContext(pThis);
  v8::Isolate* pIsolate = pContext->GetScriptRuntime();
  int32_t argc = args.GetLength();
  if (argc < 4 || argc > 5) {
    pContext->ThrowCompilerErrorException();
    return;
  }

  bool bIsStar = true;
  int32_t iIndexValue = 0;
  if (argc > 4) {
    bIsStar = false;
    iIndexValue = ValueToInteger(pThis, args.GetValue(4).get());
  }

  ByteString szName = args.GetUTF8String(2);
  ByteString szSomExp = GenerateSomExpression(
      szName.AsStringView(), args.GetInt32(3), iIndexValue, bIsStar);

  std::unique_ptr<CFXJSE_Value> argAccessor = args.GetValue(0);
  if (argAccessor->IsArray()) {
    auto pLengthValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
    argAccessor->GetObjectProperty("length", pLengthValue.get());
    int32_t iLength = pLengthValue->ToInteger();
    if (iLength < 3) {
      pContext->ThrowArgumentMismatchException();
      return;
    }

    int32_t iCounter = 0;

    std::vector<std::vector<std::unique_ptr<CFXJSE_Value>>> resolveValues(
        iLength - 2);
    auto hJSObjValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
    bool bAttribute = false;
    for (int32_t i = 2; i < iLength; i++) {
      argAccessor->GetObjectPropertyByIdx(i, hJSObjValue.get());
      XFA_RESOLVENODE_RS resolveNodeRS;
      if (ResolveObjects(pThis, hJSObjValue.get(), szSomExp.AsStringView(),
                         &resolveNodeRS, false, false)) {
        ParseResolveResult(pThis, resolveNodeRS, hJSObjValue.get(),
                           &resolveValues[i - 2], &bAttribute);
        iCounter += resolveValues[i - 2].size();
      }
    }
    if (iCounter < 1) {
      pContext->ThrowPropertyNotInObjectException(
          WideString::FromUTF8(szName.AsStringView()),
          WideString::FromUTF8(szSomExp.AsStringView()));
      return;
    }

    std::vector<std::unique_ptr<CFXJSE_Value>> values;
    for (int32_t i = 0; i < iCounter + 2; i++)
      values.push_back(pdfium::MakeUnique<CFXJSE_Value>(pIsolate));

    values[0]->SetInteger(1);
    if (bAttribute)
      values[1]->SetString(szName.AsStringView());
    else
      values[1]->SetNull();

    int32_t iIndex = 2;
    for (int32_t i = 0; i < iLength - 2; i++) {
      for (size_t j = 0; j < resolveValues[i].size(); j++) {
        values[iIndex]->Assign(resolveValues[i][j].get());
        iIndex++;
      }
    }
    args.GetReturnValue()->SetArray(values);
    return;
  }

  XFA_RESOLVENODE_RS resolveNodeRS;
  bool iRet = false;
  ByteString bsAccessorName = args.GetUTF8String(1);
  if (argAccessor->IsObject() ||
      (argAccessor->IsNull() && bsAccessorName.IsEmpty())) {
    iRet = ResolveObjects(pThis, argAccessor.get(), szSomExp.AsStringView(),
                          &resolveNodeRS, false, false);
  } else if (!argAccessor->IsObject() && !bsAccessorName.IsEmpty() &&
             GetObjectForName(pThis, argAccessor.get(),
                              bsAccessorName.AsStringView())) {
    iRet = ResolveObjects(pThis, argAccessor.get(), szSomExp.AsStringView(),
                          &resolveNodeRS, false, false);
  }
  if (!iRet) {
    pContext->ThrowPropertyNotInObjectException(
        WideString::FromUTF8(szName.AsStringView()),
        WideString::FromUTF8(szSomExp.AsStringView()));
    return;
  }

  std::vector<std::unique_ptr<CFXJSE_Value>> resolveValues;
  bool bAttribute = false;
  ParseResolveResult(pThis, resolveNodeRS, argAccessor.get(), &resolveValues,
                     &bAttribute);

  std::vector<std::unique_ptr<CFXJSE_Value>> values;
  for (size_t i = 0; i < resolveValues.size() + 2; i++)
    values.push_back(pdfium::MakeUnique<CFXJSE_Value>(pIsolate));

  values[0]->SetInteger(1);
  if (bAttribute)
    values[1]->SetString(szName.AsStringView());
  else
    values[1]->SetNull();

  for (size_t i = 0; i < resolveValues.size(); i++)
    values[i + 2]->Assign(resolveValues[i].get());

  args.GetReturnValue()->SetArray(values);
}

// static
void CFXJSE_FormCalcContext::eval_translation(CFXJSE_Value* pThis,
                                              ByteStringView szFuncName,
                                              CFXJSE_Arguments& args) {
  CFXJSE_FormCalcContext* pContext = ToFormCalcContext(pThis);
  if (args.GetLength() != 1) {
    pContext->ThrowParamCountMismatchException(L"Eval");
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = GetSimpleValue(pThis, args, 0);
  ByteString argString = ValueToUTF8String(argOne.get());
  if (argString.IsEmpty()) {
    pContext->ThrowArgumentMismatchException();
    return;
  }

  WideString scriptString = WideString::FromUTF8(argString.AsStringView());
  CFX_WideTextBuf wsJavaScriptBuf;
  if (!CFXJSE_FormCalcContext::Translate(scriptString.AsStringView(),
                                         &wsJavaScriptBuf)) {
    pContext->ThrowCompilerErrorException();
    return;
  }

  args.GetReturnValue()->SetString(
      FX_UTF8Encode(wsJavaScriptBuf.AsStringView()).AsStringView());
}

// static
void CFXJSE_FormCalcContext::is_fm_object(CFXJSE_Value* pThis,
                                          ByteStringView szFuncName,
                                          CFXJSE_Arguments& args) {
  if (args.GetLength() != 1) {
    args.GetReturnValue()->SetBoolean(false);
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = args.GetValue(0);
  args.GetReturnValue()->SetBoolean(argOne->IsObject());
}

// static
void CFXJSE_FormCalcContext::is_fm_array(CFXJSE_Value* pThis,
                                         ByteStringView szFuncName,
                                         CFXJSE_Arguments& args) {
  if (args.GetLength() != 1) {
    args.GetReturnValue()->SetBoolean(false);
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = args.GetValue(0);
  args.GetReturnValue()->SetBoolean(argOne->IsArray());
}

// static
void CFXJSE_FormCalcContext::get_fm_value(CFXJSE_Value* pThis,
                                          ByteStringView szFuncName,
                                          CFXJSE_Arguments& args) {
  CFXJSE_FormCalcContext* pContext = ToFormCalcContext(pThis);
  if (args.GetLength() != 1) {
    pContext->ThrowCompilerErrorException();
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = args.GetValue(0);
  if (argOne->IsArray()) {
    v8::Isolate* pIsolate = pContext->GetScriptRuntime();
    auto propertyValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
    auto jsObjectValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
    argOne->GetObjectPropertyByIdx(1, propertyValue.get());
    argOne->GetObjectPropertyByIdx(2, jsObjectValue.get());
    if (propertyValue->IsNull()) {
      GetObjectDefaultValue(jsObjectValue.get(), args.GetReturnValue());
      return;
    }

    jsObjectValue->GetObjectProperty(propertyValue->ToString().AsStringView(),
                                     args.GetReturnValue());
    return;
  }

  if (argOne->IsObject()) {
    GetObjectDefaultValue(argOne.get(), args.GetReturnValue());
    return;
  }

  args.GetReturnValue()->Assign(argOne.get());
}

// static
void CFXJSE_FormCalcContext::get_fm_jsobj(CFXJSE_Value* pThis,
                                          ByteStringView szFuncName,
                                          CFXJSE_Arguments& args) {
  if (args.GetLength() != 1) {
    ToFormCalcContext(pThis)->ThrowCompilerErrorException();
    return;
  }

  std::unique_ptr<CFXJSE_Value> argOne = args.GetValue(0);
  if (!argOne->IsArray()) {
    args.GetReturnValue()->Assign(argOne.get());
    return;
  }

#ifndef NDEBUG
  CFXJSE_FormCalcContext* pContext = ToFormCalcContext(pThis);
  v8::Isolate* pIsolate = pContext->GetScriptRuntime();
  auto lengthValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
  argOne->GetObjectProperty("length", lengthValue.get());
  ASSERT(lengthValue->ToInteger() >= 3);
#endif

  argOne->GetObjectPropertyByIdx(2, args.GetReturnValue());
}

// static
void CFXJSE_FormCalcContext::fm_var_filter(CFXJSE_Value* pThis,
                                           ByteStringView szFuncName,
                                           CFXJSE_Arguments& args) {
  CFXJSE_FormCalcContext* pContext = ToFormCalcContext(pThis);
  if (args.GetLength() != 1) {
    pContext->ThrowCompilerErrorException();
    return;
  }

  v8::Isolate* pIsolate = pContext->GetScriptRuntime();
  std::unique_ptr<CFXJSE_Value> argOne = args.GetValue(0);
  if (!argOne->IsArray()) {
    std::unique_ptr<CFXJSE_Value> simpleValue = GetSimpleValue(pThis, args, 0);
    args.GetReturnValue()->Assign(simpleValue.get());
    return;
  }

#ifndef NDEBUG
  auto lengthValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
  argOne->GetObjectProperty("length", lengthValue.get());
  ASSERT(lengthValue->ToInteger() >= 3);
#endif

  auto flagsValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
  argOne->GetObjectPropertyByIdx(0, flagsValue.get());
  int32_t iFlags = flagsValue->ToInteger();
  if (iFlags != 3 && iFlags != 4) {
    std::unique_ptr<CFXJSE_Value> simpleValue = GetSimpleValue(pThis, args, 0);
    args.GetReturnValue()->Assign(simpleValue.get());
    return;
  }

  if (iFlags == 4) {
    std::vector<std::unique_ptr<CFXJSE_Value>> values;
    for (int32_t i = 0; i < 3; i++)
      values.push_back(pdfium::MakeUnique<CFXJSE_Value>(pIsolate));

    values[0]->SetInteger(3);
    values[1]->SetNull();
    values[2]->SetNull();
    args.GetReturnValue()->SetArray(values);
    return;
  }

  auto objectValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
  argOne->GetObjectPropertyByIdx(2, objectValue.get());
  if (objectValue->IsNull()) {
    pContext->ThrowCompilerErrorException();
    return;
  }
  args.GetReturnValue()->Assign(argOne.get());
}

// static
void CFXJSE_FormCalcContext::concat_fm_object(CFXJSE_Value* pThis,
                                              ByteStringView szFuncName,
                                              CFXJSE_Arguments& args) {
  v8::Isolate* pIsolate = ToFormCalcContext(pThis)->GetScriptRuntime();
  uint32_t iLength = 0;
  int32_t argc = args.GetLength();
  std::vector<std::unique_ptr<CFXJSE_Value>> argValues;
  for (int32_t i = 0; i < argc; i++) {
    argValues.push_back(args.GetValue(i));
    if (argValues[i]->IsArray()) {
      auto lengthValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
      argValues[i]->GetObjectProperty("length", lengthValue.get());
      int32_t length = lengthValue->ToInteger();
      iLength = iLength + ((length > 2) ? (length - 2) : 0);
    }
    iLength += 1;
  }

  std::vector<std::unique_ptr<CFXJSE_Value>> returnValues;
  for (int32_t i = 0; i < (int32_t)iLength; i++)
    returnValues.push_back(pdfium::MakeUnique<CFXJSE_Value>(pIsolate));

  int32_t index = 0;
  for (int32_t i = 0; i < argc; i++) {
    if (argValues[i]->IsArray()) {
      auto lengthValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
      argValues[i]->GetObjectProperty("length", lengthValue.get());

      int32_t length = lengthValue->ToInteger();
      for (int32_t j = 2; j < length; j++) {
        argValues[i]->GetObjectPropertyByIdx(j, returnValues[index].get());
        index++;
      }
    }
    returnValues[index]->Assign(argValues[i].get());
    index++;
  }
  args.GetReturnValue()->SetArray(returnValues);
}

// static
std::unique_ptr<CFXJSE_Value> CFXJSE_FormCalcContext::GetSimpleValue(
    CFXJSE_Value* pThis,
    CFXJSE_Arguments& args,
    uint32_t index) {
  v8::Isolate* pIsolate = ToFormCalcContext(pThis)->GetScriptRuntime();
  ASSERT(index < (uint32_t)args.GetLength());

  std::unique_ptr<CFXJSE_Value> argIndex = args.GetValue(index);
  if (!argIndex->IsArray() && !argIndex->IsObject())
    return argIndex;

  if (argIndex->IsArray()) {
    auto lengthValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
    argIndex->GetObjectProperty("length", lengthValue.get());
    int32_t iLength = lengthValue->ToInteger();
    auto simpleValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
    if (iLength < 3) {
      simpleValue.get()->SetUndefined();
      return simpleValue;
    }

    auto propertyValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
    auto jsObjectValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
    argIndex->GetObjectPropertyByIdx(1, propertyValue.get());
    argIndex->GetObjectPropertyByIdx(2, jsObjectValue.get());
    if (propertyValue->IsNull()) {
      GetObjectDefaultValue(jsObjectValue.get(), simpleValue.get());
      return simpleValue;
    }

    jsObjectValue->GetObjectProperty(propertyValue->ToString().AsStringView(),
                                     simpleValue.get());
    return simpleValue;
  }

  auto defaultValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
  GetObjectDefaultValue(argIndex.get(), defaultValue.get());
  return defaultValue;
}

// static
bool CFXJSE_FormCalcContext::ValueIsNull(CFXJSE_Value* pThis,
                                         CFXJSE_Value* arg) {
  if (!arg || arg->IsNull())
    return true;

  if (!arg->IsArray() && !arg->IsObject())
    return false;

  v8::Isolate* pIsolate = ToFormCalcContext(pThis)->GetScriptRuntime();
  if (arg->IsArray()) {
    int32_t iLength = hvalue_get_array_length(pThis, arg);
    if (iLength < 3)
      return true;

    auto propertyValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
    auto jsObjectValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
    arg->GetObjectPropertyByIdx(1, propertyValue.get());
    arg->GetObjectPropertyByIdx(2, jsObjectValue.get());
    if (propertyValue->IsNull()) {
      auto defaultValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
      GetObjectDefaultValue(jsObjectValue.get(), defaultValue.get());
      return defaultValue->IsNull();
    }

    auto newPropertyValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
    jsObjectValue->GetObjectProperty(propertyValue->ToString().AsStringView(),
                                     newPropertyValue.get());
    return newPropertyValue->IsNull();
  }

  auto defaultValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
  GetObjectDefaultValue(arg, defaultValue.get());
  return defaultValue->IsNull();
}

// static
int32_t CFXJSE_FormCalcContext::hvalue_get_array_length(CFXJSE_Value* pThis,
                                                        CFXJSE_Value* arg) {
  if (!arg || !arg->IsArray())
    return 0;

  v8::Isolate* pIsolate = ToFormCalcContext(pThis)->GetScriptRuntime();
  auto lengthValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
  arg->GetObjectProperty("length", lengthValue.get());
  return lengthValue->ToInteger();
}

// static
bool CFXJSE_FormCalcContext::simpleValueCompare(CFXJSE_Value* pThis,
                                                CFXJSE_Value* firstValue,
                                                CFXJSE_Value* secondValue) {
  if (!firstValue)
    return false;

  if (firstValue->IsString()) {
    ByteString firstString = ValueToUTF8String(firstValue);
    ByteString secondString = ValueToUTF8String(secondValue);
    return firstString == secondString;
  }
  if (firstValue->IsNumber()) {
    float first = ValueToFloat(pThis, firstValue);
    float second = ValueToFloat(pThis, secondValue);
    return first == second;
  }
  if (firstValue->IsBoolean())
    return firstValue->ToBoolean() == secondValue->ToBoolean();

  return firstValue->IsNull() && secondValue && secondValue->IsNull();
}

// static
std::vector<std::unique_ptr<CFXJSE_Value>> CFXJSE_FormCalcContext::unfoldArgs(
    CFXJSE_Value* pThis,
    CFXJSE_Arguments& args) {
  std::vector<std::unique_ptr<CFXJSE_Value>> results;

  int32_t iCount = 0;
  v8::Isolate* pIsolate = ToFormCalcContext(pThis)->GetScriptRuntime();
  int32_t argc = args.GetLength();
  std::vector<std::unique_ptr<CFXJSE_Value>> argsValue;
  static constexpr int kStart = 1;
  for (int32_t i = 0; i < argc - kStart; i++) {
    argsValue.push_back(args.GetValue(i + kStart));
    if (argsValue[i]->IsArray()) {
      auto lengthValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
      argsValue[i]->GetObjectProperty("length", lengthValue.get());
      int32_t iLength = lengthValue->ToInteger();
      iCount += ((iLength > 2) ? (iLength - 2) : 0);
    } else {
      iCount += 1;
    }
  }

  for (int32_t i = 0; i < iCount; i++)
    results.push_back(pdfium::MakeUnique<CFXJSE_Value>(pIsolate));

  int32_t index = 0;
  for (int32_t i = 0; i < argc - kStart; i++) {
    if (argsValue[i]->IsArray()) {
      auto lengthValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
      argsValue[i]->GetObjectProperty("length", lengthValue.get());
      int32_t iLength = lengthValue->ToInteger();
      if (iLength < 3)
        continue;

      auto propertyValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
      auto jsObjectValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
      argsValue[i]->GetObjectPropertyByIdx(1, propertyValue.get());
      if (propertyValue->IsNull()) {
        for (int32_t j = 2; j < iLength; j++) {
          argsValue[i]->GetObjectPropertyByIdx(j, jsObjectValue.get());
          GetObjectDefaultValue(jsObjectValue.get(), results[index].get());
          index++;
        }
      } else {
        for (int32_t j = 2; j < iLength; j++) {
          argsValue[i]->GetObjectPropertyByIdx(j, jsObjectValue.get());
          jsObjectValue->GetObjectProperty(
              propertyValue->ToString().AsStringView(), results[index].get());
          index++;
        }
      }
    } else if (argsValue[i]->IsObject()) {
      GetObjectDefaultValue(argsValue[i].get(), results[index].get());
      index++;
    } else {
      results[index]->Assign(argsValue[i].get());
      index++;
    }
  }
  return results;
}

// static
void CFXJSE_FormCalcContext::GetObjectDefaultValue(
    CFXJSE_Value* pValue,
    CFXJSE_Value* pDefaultValue) {
  CXFA_Node* pNode = ToNode(CFXJSE_Engine::ToObject(pValue));
  if (!pNode) {
    pDefaultValue->SetNull();
    return;
  }
  pNode->JSObject()->ScriptSomDefaultValue(pDefaultValue, false,
                                           XFA_Attribute::Unknown);
}

// static
bool CFXJSE_FormCalcContext::SetObjectDefaultValue(CFXJSE_Value* pValue,
                                                   CFXJSE_Value* hNewValue) {
  CXFA_Node* pNode = ToNode(CFXJSE_Engine::ToObject(pValue));
  if (!pNode)
    return false;

  pNode->JSObject()->ScriptSomDefaultValue(hNewValue, true,
                                           XFA_Attribute::Unknown);
  return true;
}

// static
ByteString CFXJSE_FormCalcContext::GenerateSomExpression(ByteStringView szName,
                                                         int32_t iIndexFlags,
                                                         int32_t iIndexValue,
                                                         bool bIsStar) {
  if (bIsStar)
    return ByteString(szName, "[*]");

  if (iIndexFlags == 0)
    return ByteString(szName);

  if (iIndexFlags == 1 || iIndexValue == 0) {
    return ByteString(szName, "[") + ByteString::FormatInteger(iIndexValue) +
           "]";
  }
  ByteString szSomExp;
  if (iIndexFlags == 2) {
    szSomExp = (iIndexValue < 0) ? (szName + "[-") : (szName + "[+");
    iIndexValue = (iIndexValue < 0) ? (0 - iIndexValue) : iIndexValue;
    szSomExp += ByteString::FormatInteger(iIndexValue);
    szSomExp += "]";
  } else {
    szSomExp = (iIndexValue < 0) ? (szName + "[") : (szName + "[-");
    iIndexValue = (iIndexValue < 0) ? (0 - iIndexValue) : iIndexValue;
    szSomExp += ByteString::FormatInteger(iIndexValue);
    szSomExp += "]";
  }
  return szSomExp;
}

// static
bool CFXJSE_FormCalcContext::GetObjectForName(CFXJSE_Value* pThis,
                                              CFXJSE_Value* accessorValue,
                                              ByteStringView szAccessorName) {
  CXFA_Document* pDoc = ToFormCalcContext(pThis)->GetDocument();
  if (!pDoc)
    return false;

  CFXJSE_Engine* pScriptContext = pDoc->GetScriptContext();
  XFA_RESOLVENODE_RS resolveNodeRS;
  uint32_t dwFlags = XFA_RESOLVENODE_Children | XFA_RESOLVENODE_Properties |
                     XFA_RESOLVENODE_Siblings | XFA_RESOLVENODE_Parent;
  bool iRet = pScriptContext->ResolveObjects(
      pScriptContext->GetThisObject(),
      WideString::FromUTF8(szAccessorName).AsStringView(), &resolveNodeRS,
      dwFlags, nullptr);
  if (iRet && resolveNodeRS.dwFlags == XFA_ResolveNode_RSType_Nodes) {
    accessorValue->Assign(
        pScriptContext->GetJSValueFromMap(resolveNodeRS.objects.front().Get()));
    return true;
  }
  return false;
}

// static
bool CFXJSE_FormCalcContext::ResolveObjects(CFXJSE_Value* pThis,
                                            CFXJSE_Value* pRefValue,
                                            ByteStringView bsSomExp,
                                            XFA_RESOLVENODE_RS* resolveNodeRS,
                                            bool bdotAccessor,
                                            bool bHasNoResolveName) {
  CXFA_Document* pDoc = ToFormCalcContext(pThis)->GetDocument();
  if (!pDoc)
    return false;

  WideString wsSomExpression = WideString::FromUTF8(bsSomExp);
  CFXJSE_Engine* pScriptContext = pDoc->GetScriptContext();
  CXFA_Object* pNode = nullptr;
  uint32_t dFlags = 0UL;
  if (bdotAccessor) {
    if (pRefValue && pRefValue->IsNull()) {
      pNode = pScriptContext->GetThisObject();
      dFlags = XFA_RESOLVENODE_Siblings | XFA_RESOLVENODE_Parent;
    } else {
      pNode = CFXJSE_Engine::ToObject(pRefValue);
      if (!pNode)
        return false;

      if (bHasNoResolveName) {
        WideString wsName;
        if (CXFA_Node* pXFANode = pNode->AsNode()) {
          Optional<WideString> ret =
              pXFANode->JSObject()->TryAttribute(XFA_Attribute::Name, false);
          if (ret)
            wsName = *ret;
        }
        if (wsName.IsEmpty())
          wsName = L"#" + WideString::FromASCII(pNode->GetClassName());

        wsSomExpression = wsName + wsSomExpression;
        dFlags = XFA_RESOLVENODE_Siblings;
      } else {
        dFlags = (bsSomExp == "*")
                     ? (XFA_RESOLVENODE_Children)
                     : (XFA_RESOLVENODE_Children | XFA_RESOLVENODE_Attributes |
                        XFA_RESOLVENODE_Properties);
      }
    }
  } else {
    pNode = CFXJSE_Engine::ToObject(pRefValue);
    dFlags = XFA_RESOLVENODE_AnyChild;
  }
  return pScriptContext->ResolveObjects(pNode, wsSomExpression.AsStringView(),
                                        resolveNodeRS, dFlags, nullptr);
}

// static
void CFXJSE_FormCalcContext::ParseResolveResult(
    CFXJSE_Value* pThis,
    const XFA_RESOLVENODE_RS& resolveNodeRS,
    CFXJSE_Value* pParentValue,
    std::vector<std::unique_ptr<CFXJSE_Value>>* resultValues,
    bool* bAttribute) {
  ASSERT(bAttribute);

  resultValues->clear();

  CFXJSE_FormCalcContext* pContext = ToFormCalcContext(pThis);
  v8::Isolate* pIsolate = pContext->GetScriptRuntime();

  if (resolveNodeRS.dwFlags == XFA_ResolveNode_RSType_Nodes) {
    *bAttribute = false;
    CFXJSE_Engine* pScriptContext = pContext->GetDocument()->GetScriptContext();
    for (auto& pObject : resolveNodeRS.objects) {
      resultValues->push_back(pdfium::MakeUnique<CFXJSE_Value>(pIsolate));
      resultValues->back()->Assign(
          pScriptContext->GetJSValueFromMap(pObject.Get()));
    }
    return;
  }

  *bAttribute = true;
  if (resolveNodeRS.script_attribute.callback &&
      resolveNodeRS.script_attribute.eValueType == XFA_ScriptType::Object) {
    for (auto& pObject : resolveNodeRS.objects) {
      auto pValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
      CJX_Object* jsObject = pObject->JSObject();
      (*resolveNodeRS.script_attribute.callback)(
          jsObject, pValue.get(), false,
          resolveNodeRS.script_attribute.attribute);
      resultValues->push_back(std::move(pValue));
      *bAttribute = false;
    }
  }
  if (!*bAttribute)
    return;
  if (!pParentValue || !pParentValue->IsObject())
    return;

  resultValues->push_back(pdfium::MakeUnique<CFXJSE_Value>(pIsolate));
  resultValues->back()->Assign(pParentValue);
}

// static
int32_t CFXJSE_FormCalcContext::ValueToInteger(CFXJSE_Value* pThis,
                                               CFXJSE_Value* pValue) {
  if (!pValue)
    return 0;

  v8::Isolate* pIsolate = ToFormCalcContext(pThis)->GetScriptRuntime();
  if (pValue->IsArray()) {
    auto propertyValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
    auto jsObjectValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
    auto newPropertyValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
    pValue->GetObjectPropertyByIdx(1, propertyValue.get());
    pValue->GetObjectPropertyByIdx(2, jsObjectValue.get());
    if (propertyValue->IsNull()) {
      GetObjectDefaultValue(jsObjectValue.get(), newPropertyValue.get());
      return ValueToInteger(pThis, newPropertyValue.get());
    }

    jsObjectValue->GetObjectProperty(propertyValue->ToString().AsStringView(),
                                     newPropertyValue.get());
    return ValueToInteger(pThis, newPropertyValue.get());
  }
  if (pValue->IsObject()) {
    auto newPropertyValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
    GetObjectDefaultValue(pValue, newPropertyValue.get());
    return ValueToInteger(pThis, newPropertyValue.get());
  }
  if (pValue->IsString())
    return FXSYS_atoi(pValue->ToString().c_str());
  return pValue->ToInteger();
}

// static
float CFXJSE_FormCalcContext::ValueToFloat(CFXJSE_Value* pThis,
                                           CFXJSE_Value* arg) {
  if (!arg)
    return 0.0f;

  v8::Isolate* pIsolate = ToFormCalcContext(pThis)->GetScriptRuntime();
  if (arg->IsArray()) {
    auto propertyValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
    auto jsObjectValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
    auto newPropertyValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
    arg->GetObjectPropertyByIdx(1, propertyValue.get());
    arg->GetObjectPropertyByIdx(2, jsObjectValue.get());
    if (propertyValue->IsNull()) {
      GetObjectDefaultValue(jsObjectValue.get(), newPropertyValue.get());
      return ValueToFloat(pThis, newPropertyValue.get());
    }
    jsObjectValue->GetObjectProperty(propertyValue->ToString().AsStringView(),
                                     newPropertyValue.get());
    return ValueToFloat(pThis, newPropertyValue.get());
  }
  if (arg->IsObject()) {
    auto newPropertyValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
    GetObjectDefaultValue(arg, newPropertyValue.get());
    return ValueToFloat(pThis, newPropertyValue.get());
  }
  if (arg->IsString())
    return strtof(arg->ToString().c_str(), nullptr);
  if (arg->IsUndefined())
    return 0;

  return arg->ToFloat();
}

// static
double CFXJSE_FormCalcContext::ValueToDouble(CFXJSE_Value* pThis,
                                             CFXJSE_Value* arg) {
  if (!arg)
    return 0;

  v8::Isolate* pIsolate = ToFormCalcContext(pThis)->GetScriptRuntime();
  if (arg->IsArray()) {
    auto propertyValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
    auto jsObjectValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
    auto newPropertyValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
    arg->GetObjectPropertyByIdx(1, propertyValue.get());
    arg->GetObjectPropertyByIdx(2, jsObjectValue.get());
    if (propertyValue->IsNull()) {
      GetObjectDefaultValue(jsObjectValue.get(), newPropertyValue.get());
      return ValueToDouble(pThis, newPropertyValue.get());
    }
    jsObjectValue->GetObjectProperty(propertyValue->ToString().AsStringView(),
                                     newPropertyValue.get());
    return ValueToDouble(pThis, newPropertyValue.get());
  }
  if (arg->IsObject()) {
    auto newPropertyValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
    GetObjectDefaultValue(arg, newPropertyValue.get());
    return ValueToDouble(pThis, newPropertyValue.get());
  }
  if (arg->IsString())
    return strtod(arg->ToString().c_str(), nullptr);
  if (arg->IsUndefined())
    return 0;
  return arg->ToDouble();
}

// static.
double CFXJSE_FormCalcContext::ExtractDouble(CFXJSE_Value* pThis,
                                             CFXJSE_Value* src,
                                             bool* ret) {
  ASSERT(ret);
  *ret = true;

  if (!src)
    return 0;

  if (!src->IsArray())
    return ValueToDouble(pThis, src);

  v8::Isolate* pIsolate = ToFormCalcContext(pThis)->GetScriptRuntime();
  auto lengthValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
  src->GetObjectProperty("length", lengthValue.get());
  int32_t iLength = lengthValue->ToInteger();
  if (iLength <= 2) {
    *ret = false;
    return 0.0;
  }

  auto propertyValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
  auto jsObjectValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
  src->GetObjectPropertyByIdx(1, propertyValue.get());
  src->GetObjectPropertyByIdx(2, jsObjectValue.get());
  if (propertyValue->IsNull())
    return ValueToDouble(pThis, jsObjectValue.get());

  auto newPropertyValue = pdfium::MakeUnique<CFXJSE_Value>(pIsolate);
  jsObjectValue->GetObjectProperty(propertyValue->ToString().AsStringView(),
                                   newPropertyValue.get());
  return ValueToDouble(pThis, newPropertyValue.get());
}

// static
ByteString CFXJSE_FormCalcContext::ValueToUTF8String(CFXJSE_Value* arg) {
  if (!arg || arg->IsNull() || arg->IsUndefined())
    return ByteString();
  if (arg->IsBoolean())
    return arg->ToBoolean() ? "1" : "0";
  return arg->ToString();
}

// static.
bool CFXJSE_FormCalcContext::Translate(WideStringView wsFormcalc,
                                       CFX_WideTextBuf* wsJavascript) {
  if (wsFormcalc.IsEmpty()) {
    wsJavascript->Clear();
    return true;
  }

  CXFA_FMParser parser(wsFormcalc);
  std::unique_ptr<CXFA_FMAST> ast = parser.Parse();
  if (!ast || parser.HasError())
    return false;

  CXFA_FMToJavaScriptDepth::Reset();
  if (!ast->ToJavaScript(wsJavascript))
    return false;

  wsJavascript->AppendChar(0);
  return !CXFA_IsTooBig(wsJavascript);
}

CFXJSE_FormCalcContext::CFXJSE_FormCalcContext(v8::Isolate* pScriptIsolate,
                                               CFXJSE_Context* pScriptContext,
                                               CXFA_Document* pDoc)
    : m_pIsolate(pScriptIsolate),
      m_pValue(pdfium::MakeUnique<CFXJSE_Value>(pScriptIsolate)),
      m_pDocument(pDoc) {
  m_pValue->SetObject(
      this,
      CFXJSE_Class::Create(pScriptContext, &kFormCalcFM2JSDescriptor, false));
}

CFXJSE_FormCalcContext::~CFXJSE_FormCalcContext() = default;

CFXJSE_FormCalcContext* CFXJSE_FormCalcContext::AsFormCalcContext() {
  return this;
}

void CFXJSE_FormCalcContext::GlobalPropertyGetter(CFXJSE_Value* pValue) {
  pValue->Assign(m_pValue.get());
}

void CFXJSE_FormCalcContext::ThrowNoDefaultPropertyException(
    ByteStringView name) const {
  ThrowException(WideString::FromUTF8(name) +
                 WideString::FromASCII(" doesn't have a default property."));
}

void CFXJSE_FormCalcContext::ThrowCompilerErrorException() const {
  ThrowException(WideString::FromASCII("Compiler error."));
}

void CFXJSE_FormCalcContext::ThrowDivideByZeroException() const {
  ThrowException(WideString::FromASCII("Divide by zero."));
}

void CFXJSE_FormCalcContext::ThrowServerDeniedException() const {
  ThrowException(WideString::FromASCII("Server does not permit operation."));
}

void CFXJSE_FormCalcContext::ThrowPropertyNotInObjectException(
    const WideString& name,
    const WideString& exp) const {
  ThrowException(
      WideString::FromASCII("An attempt was made to reference property '") +
      name + WideString::FromASCII("' of a non-object in SOM expression ") +
      exp + L".");
}

void CFXJSE_FormCalcContext::ThrowParamCountMismatchException(
    const WideString& method) const {
  ThrowException(
      WideString::FromASCII("Incorrect number of parameters calling method '") +
      method + L"'.");
}

void CFXJSE_FormCalcContext::ThrowArgumentMismatchException() const {
  ThrowException(WideString::FromASCII(
      "Argument mismatch in property or function argument."));
}

void CFXJSE_FormCalcContext::ThrowException(const WideString& str) const {
  ASSERT(!str.IsEmpty());
  FXJSE_ThrowMessage(str.ToUTF8().AsStringView());
}
