/* C++ code produced by gperf version 3.0.1 */
/* Command-line: 'E:\\mycode\\chromium.bb-master\\src\\third_party\\gperf\\bin\\gperf.exe' --key-positions='*' -P -n -m 50 -D  */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif


// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "CSSValueKeywords.h"
#include "core/css/HashTools.h"
#include <string.h>

namespace blink {
static const char valueListStringPool[] = {
"\0"
    "inherit\0"
    "initial\0"
    "none\0"
    "hidden\0"
    "inset\0"
    "groove\0"
    "outset\0"
    "ridge\0"
    "dotted\0"
    "dashed\0"
    "solid\0"
    "double\0"
    "caption\0"
    "icon\0"
    "menu\0"
    "message-box\0"
    "small-caption\0"
    "-webkit-mini-control\0"
    "-webkit-small-control\0"
    "-webkit-control\0"
    "status-bar\0"
    "italic\0"
    "oblique\0"
    "all\0"
    "small-caps\0"
    "common-ligatures\0"
    "no-common-ligatures\0"
    "discretionary-ligatures\0"
    "no-discretionary-ligatures\0"
    "historical-ligatures\0"
    "no-historical-ligatures\0"
    "contextual\0"
    "no-contextual\0"
    "normal\0"
    "bold\0"
    "bolder\0"
    "lighter\0"
    "100\0"
    "200\0"
    "300\0"
    "400\0"
    "500\0"
    "600\0"
    "700\0"
    "800\0"
    "900\0"
    "ultra-condensed\0"
    "extra-condensed\0"
    "condensed\0"
    "semi-condensed\0"
    "semi-expanded\0"
    "expanded\0"
    "extra-expanded\0"
    "ultra-expanded\0"
    "xx-small\0"
    "x-small\0"
    "small\0"
    "medium\0"
    "large\0"
    "x-large\0"
    "xx-large\0"
    "-webkit-xxx-large\0"
    "smaller\0"
    "larger\0"
    "serif\0"
    "sans-serif\0"
    "cursive\0"
    "fantasy\0"
    "monospace\0"
    "-webkit-body\0"
    "-webkit-pictograph\0"
    "aqua\0"
    "black\0"
    "blue\0"
    "fuchsia\0"
    "gray\0"
    "green\0"
    "lime\0"
    "maroon\0"
    "navy\0"
    "olive\0"
    "orange\0"
    "purple\0"
    "red\0"
    "silver\0"
    "teal\0"
    "white\0"
    "yellow\0"
    "transparent\0"
    "-webkit-link\0"
    "-webkit-activelink\0"
    "activeborder\0"
    "activecaption\0"
    "appworkspace\0"
    "background\0"
    "buttonface\0"
    "buttonhighlight\0"
    "buttonshadow\0"
    "buttontext\0"
    "captiontext\0"
    "graytext\0"
    "highlight\0"
    "highlighttext\0"
    "inactiveborder\0"
    "inactivecaption\0"
    "inactivecaptiontext\0"
    "infobackground\0"
    "infotext\0"
    "menutext\0"
    "scrollbar\0"
    "threeddarkshadow\0"
    "threedface\0"
    "threedhighlight\0"
    "threedlightshadow\0"
    "threedshadow\0"
    "window\0"
    "windowframe\0"
    "windowtext\0"
    "-internal-active-list-box-selection\0"
    "-internal-active-list-box-selection-text\0"
    "-internal-inactive-list-box-selection\0"
    "-internal-inactive-list-box-selection-text\0"
    "-webkit-focus-ring-color\0"
    "currentcolor\0"
    "grey\0"
    "-webkit-text\0"
    "repeat\0"
    "repeat-x\0"
    "repeat-y\0"
    "no-repeat\0"
    "clear\0"
    "copy\0"
    "source-over\0"
    "source-in\0"
    "source-out\0"
    "source-atop\0"
    "destination-over\0"
    "destination-in\0"
    "destination-out\0"
    "destination-atop\0"
    "xor\0"
    "plus-lighter\0"
    "baseline\0"
    "middle\0"
    "sub\0"
    "super\0"
    "text-top\0"
    "text-bottom\0"
    "top\0"
    "bottom\0"
    "-webkit-baseline-middle\0"
    "-webkit-auto\0"
    "left\0"
    "right\0"
    "center\0"
    "justify\0"
    "-webkit-left\0"
    "-webkit-right\0"
    "-webkit-center\0"
    "-webkit-match-parent\0"
    "inter-word\0"
    "distribute\0"
    "outside\0"
    "inside\0"
    "disc\0"
    "circle\0"
    "square\0"
    "decimal\0"
    "decimal-leading-zero\0"
    "arabic-indic\0"
    "bengali\0"
    "cambodian\0"
    "khmer\0"
    "devanagari\0"
    "gujarati\0"
    "gurmukhi\0"
    "kannada\0"
    "lao\0"
    "malayalam\0"
    "mongolian\0"
    "myanmar\0"
    "oriya\0"
    "persian\0"
    "urdu\0"
    "telugu\0"
    "tibetan\0"
    "thai\0"
    "lower-roman\0"
    "upper-roman\0"
    "lower-greek\0"
    "lower-alpha\0"
    "lower-latin\0"
    "upper-alpha\0"
    "upper-latin\0"
    "cjk-earthly-branch\0"
    "cjk-heavenly-stem\0"
    "ethiopic-halehame\0"
    "ethiopic-halehame-am\0"
    "ethiopic-halehame-ti-er\0"
    "ethiopic-halehame-ti-et\0"
    "hangul\0"
    "hangul-consonant\0"
    "korean-hangul-formal\0"
    "korean-hanja-formal\0"
    "korean-hanja-informal\0"
    "hebrew\0"
    "armenian\0"
    "lower-armenian\0"
    "upper-armenian\0"
    "georgian\0"
    "cjk-ideographic\0"
    "simp-chinese-formal\0"
    "simp-chinese-informal\0"
    "trad-chinese-formal\0"
    "trad-chinese-informal\0"
    "hiragana\0"
    "katakana\0"
    "hiragana-iroha\0"
    "katakana-iroha\0"
    "inline\0"
    "block\0"
    "list-item\0"
    "inline-block\0"
    "table\0"
    "inline-table\0"
    "table-row-group\0"
    "table-header-group\0"
    "table-footer-group\0"
    "table-row\0"
    "table-column-group\0"
    "table-column\0"
    "table-cell\0"
    "table-caption\0"
    "-webkit-box\0"
    "-webkit-inline-box\0"
    "flex\0"
    "inline-flex\0"
    "grid\0"
    "inline-grid\0"
    "-webkit-flex\0"
    "-webkit-inline-flex\0"
    "auto\0"
    "crosshair\0"
    "default\0"
    "pointer\0"
    "move\0"
    "vertical-text\0"
    "cell\0"
    "context-menu\0"
    "alias\0"
    "progress\0"
    "no-drop\0"
    "not-allowed\0"
    "zoom-in\0"
    "zoom-out\0"
    "e-resize\0"
    "ne-resize\0"
    "nw-resize\0"
    "n-resize\0"
    "se-resize\0"
    "sw-resize\0"
    "s-resize\0"
    "w-resize\0"
    "ew-resize\0"
    "ns-resize\0"
    "nesw-resize\0"
    "nwse-resize\0"
    "col-resize\0"
    "row-resize\0"
    "text\0"
    "wait\0"
    "help\0"
    "all-scroll\0"
    "-webkit-grab\0"
    "-webkit-grabbing\0"
    "-webkit-zoom-in\0"
    "-webkit-zoom-out\0"
    "ltr\0"
    "rtl\0"
    "capitalize\0"
    "uppercase\0"
    "lowercase\0"
    "visible\0"
    "collapse\0"
    "a3\0"
    "a4\0"
    "a5\0"
    "above\0"
    "absolute\0"
    "always\0"
    "avoid\0"
    "b4\0"
    "b5\0"
    "below\0"
    "bidi-override\0"
    "blink\0"
    "both\0"
    "close-quote\0"
    "embed\0"
    "fixed\0"
    "hand\0"
    "hide\0"
    "invert\0"
    "-webkit-isolate\0"
    "-webkit-isolate-override\0"
    "-webkit-plaintext\0"
    "landscape\0"
    "ledger\0"
    "legal\0"
    "letter\0"
    "line-through\0"
    "local\0"
    "no-close-quote\0"
    "no-open-quote\0"
    "nowrap\0"
    "open-quote\0"
    "overlay\0"
    "overline\0"
    "portrait\0"
    "pre\0"
    "pre-line\0"
    "pre-wrap\0"
    "relative\0"
    "scroll\0"
    "separate\0"
    "show\0"
    "static\0"
    "thick\0"
    "thin\0"
    "underline\0"
    "wavy\0"
    "-webkit-nowrap\0"
    "stretch\0"
    "start\0"
    "end\0"
    "clone\0"
    "slice\0"
    "reverse\0"
    "horizontal\0"
    "vertical\0"
    "inline-axis\0"
    "block-axis\0"
    "single\0"
    "multiple\0"
    "flex-start\0"
    "flex-end\0"
    "space-between\0"
    "space-around\0"
    "space-evenly\0"
    "row\0"
    "row-reverse\0"
    "column\0"
    "column-reverse\0"
    "wrap\0"
    "wrap-reverse\0"
    "dense\0"
    "read-only\0"
    "read-write\0"
    "read-write-plaintext-only\0"
    "element\0"
    "intrinsic\0"
    "min-intrinsic\0"
    "-webkit-min-content\0"
    "-webkit-max-content\0"
    "-webkit-fill-available\0"
    "-webkit-fit-content\0"
    "min-content\0"
    "max-content\0"
    "clip\0"
    "ellipsis\0"
    "discard\0"
    "break-all\0"
    "keep-all\0"
    "break-word\0"
    "space\0"
    "loose\0"
    "strict\0"
    "after-white-space\0"
    "checkbox\0"
    "radio\0"
    "push-button\0"
    "square-button\0"
    "button\0"
    "button-bevel\0"
    "inner-spin-button\0"
    "listbox\0"
    "listitem\0"
    "media-enter-fullscreen-button\0"
    "media-exit-fullscreen-button\0"
    "media-fullscreen-volume-slider\0"
    "media-fullscreen-volume-slider-thumb\0"
    "media-mute-button\0"
    "media-play-button\0"
    "media-overlay-play-button\0"
    "media-toggle-closed-captions-button\0"
    "media-slider\0"
    "media-sliderthumb\0"
    "media-volume-slider-container\0"
    "media-volume-slider\0"
    "media-volume-sliderthumb\0"
    "media-controls-background\0"
    "media-controls-fullscreen-background\0"
    "media-current-time-display\0"
    "media-time-remaining-display\0"
    "-internal-media-cast-off-button\0"
    "-internal-media-overlay-cast-off-button\0"
    "menulist\0"
    "menulist-button\0"
    "menulist-text\0"
    "menulist-textfield\0"
    "meter\0"
    "progress-bar\0"
    "progress-bar-value\0"
    "slider-horizontal\0"
    "slider-vertical\0"
    "sliderthumb-horizontal\0"
    "sliderthumb-vertical\0"
    "caret\0"
    "searchfield\0"
    "searchfield-decoration\0"
    "searchfield-results-decoration\0"
    "searchfield-cancel-button\0"
    "textfield\0"
    "relevancy-level-indicator\0"
    "continuous-capacity-level-indicator\0"
    "discrete-capacity-level-indicator\0"
    "rating-level-indicator\0"
    "textarea\0"
    "caps-lock-indicator\0"
    "round\0"
    "border\0"
    "border-box\0"
    "content\0"
    "content-box\0"
    "padding\0"
    "padding-box\0"
    "margin-box\0"
    "contain\0"
    "cover\0"
    "logical\0"
    "visual\0"
    "alternate\0"
    "alternate-reverse\0"
    "forwards\0"
    "backwards\0"
    "infinite\0"
    "running\0"
    "paused\0"
    "flat\0"
    "preserve-3d\0"
    "ease\0"
    "linear\0"
    "ease-in\0"
    "ease-out\0"
    "ease-in-out\0"
    "step-start\0"
    "step-middle\0"
    "step-end\0"
    "steps\0"
    "cubic-bezier\0"
    "document\0"
    "reset\0"
    "zoom\0"
    "visiblePainted\0"
    "visibleFill\0"
    "visibleStroke\0"
    "painted\0"
    "fill\0"
    "stroke\0"
    "bounding-box\0"
    "spell-out\0"
    "digits\0"
    "literal-punctuation\0"
    "no-punctuation\0"
    "antialiased\0"
    "subpixel-antialiased\0"
    "optimizeSpeed\0"
    "optimizeLegibility\0"
    "geometricPrecision\0"
    "economy\0"
    "exact\0"
    "lr\0"
    "rl\0"
    "tb\0"
    "lr-tb\0"
    "rl-tb\0"
    "tb-rl\0"
    "horizontal-tb\0"
    "vertical-rl\0"
    "vertical-lr\0"
    "horizontal-bt\0"
    "after\0"
    "before\0"
    "over\0"
    "under\0"
    "filled\0"
    "open\0"
    "dot\0"
    "double-circle\0"
    "triangle\0"
    "sesame\0"
    "ellipse\0"
    "closest-side\0"
    "closest-corner\0"
    "farthest-side\0"
    "farthest-corner\0"
    "sideways\0"
    "sideways-right\0"
    "upright\0"
    "vertical-right\0"
    "font\0"
    "glyphs\0"
    "inline-box\0"
    "replaced\0"
    "on\0"
    "off\0"
    "optimizeQuality\0"
    "pixelated\0"
    "-webkit-optimize-contrast\0"
    "nonzero\0"
    "evenodd\0"
    "at\0"
    "alphabetic\0"
    "fullscreen\0"
    "standalone\0"
    "minimal-ui\0"
    "browser\0"
    "coarse\0"
    "fine\0"
    "on-demand\0"
    "hover\0"
    "multiply\0"
    "screen\0"
    "darken\0"
    "lighten\0"
    "color-dodge\0"
    "color-burn\0"
    "hard-light\0"
    "soft-light\0"
    "difference\0"
    "exclusion\0"
    "hue\0"
    "saturation\0"
    "color\0"
    "luminosity\0"
    "scale-down\0"
    "balance\0"
    "-webkit-paged-x\0"
    "-webkit-paged-y\0"
    "drag\0"
    "no-drag\0"
    "span\0"
    "minmax\0"
    "each-line\0"
    "progressive\0"
    "interlace\0"
    "markers\0"
    "-internal-extend-to-zoom\0"
    "isolate\0"
    "start-touch\0"
    "wheel-event\0"
    "scroll-event\0"
    "pan-x\0"
    "pan-y\0"
    "pan-left\0"
    "pan-right\0"
    "pan-up\0"
    "pan-down\0"
    "manipulation\0"
    "last-baseline\0"
    "self-start\0"
    "self-end\0"
    "true\0"
    "safe\0"
    "legacy\0"
    "smooth\0"
    "contents\0"
    "scroll-position\0"
    "unset\0"
    "linear-gradient\0"
    "radial-gradient\0"
    "repeating-linear-gradient\0"
    "repeating-radial-gradient\0"
    "-webkit-canvas\0"
    "-webkit-cross-fade\0"
    "-webkit-gradient\0"
    "-webkit-linear-gradient\0"
    "-webkit-radial-gradient\0"
    "-webkit-repeating-linear-gradient\0"
    "-webkit-repeating-radial-gradient\0"
    "-webkit-image-set\0"
    "from\0"
    "to\0"
    "color-stop\0"
    "radial\0"
    "attr\0"
    "counter\0"
    "counters\0"
    "rect\0"
    "polygon\0"
    "format\0"
    "grayscale\0"
    "sepia\0"
    "saturate\0"
    "hue-rotate\0"
    "opacity\0"
    "brightness\0"
    "contrast\0"
    "blur\0"
    "drop-shadow\0"
    "url\0"
    "rgb\0"
    "rgba\0"
    "hsl\0"
    "hsla\0"
    "matrix\0"
    "matrix3d\0"
    "perspective\0"
    "rotate\0"
    "rotateX\0"
    "rotateY\0"
    "rotateZ\0"
    "rotate3d\0"
    "scale\0"
    "scaleX\0"
    "scaleY\0"
    "scaleZ\0"
    "scale3d\0"
    "skew\0"
    "skewX\0"
    "skewY\0"
    "translate\0"
    "translateX\0"
    "translateY\0"
    "translateZ\0"
    "translate3d\0"
    "path\0"
    "calc\0"
    "-webkit-calc\0"
    "mandatory\0"
    "proximity\0"
    "fit-content\0"
    "-internal-variable-value\0"
    "var\0"
    "aliceblue\0"
    "antiquewhite\0"
    "aquamarine\0"
    "azure\0"
    "beige\0"
    "bisque\0"
    "blanchedalmond\0"
    "blueviolet\0"
    "brown\0"
    "burlywood\0"
    "cadetblue\0"
    "chartreuse\0"
    "chocolate\0"
    "coral\0"
    "cornflowerblue\0"
    "cornsilk\0"
    "crimson\0"
    "cyan\0"
    "darkblue\0"
    "darkcyan\0"
    "darkgoldenrod\0"
    "darkgray\0"
    "darkgreen\0"
    "darkgrey\0"
    "darkkhaki\0"
    "darkmagenta\0"
    "darkolivegreen\0"
    "darkorange\0"
    "darkorchid\0"
    "darkred\0"
    "darksalmon\0"
    "darkseagreen\0"
    "darkslateblue\0"
    "darkslategray\0"
    "darkslategrey\0"
    "darkturquoise\0"
    "darkviolet\0"
    "deeppink\0"
    "deepskyblue\0"
    "dimgray\0"
    "dimgrey\0"
    "dodgerblue\0"
    "firebrick\0"
    "floralwhite\0"
    "forestgreen\0"
    "gainsboro\0"
    "ghostwhite\0"
    "gold\0"
    "goldenrod\0"
    "greenyellow\0"
    "honeydew\0"
    "hotpink\0"
    "indianred\0"
    "indigo\0"
    "ivory\0"
    "khaki\0"
    "lavender\0"
    "lavenderblush\0"
    "lawngreen\0"
    "lemonchiffon\0"
    "lightblue\0"
    "lightcoral\0"
    "lightcyan\0"
    "lightgoldenrodyellow\0"
    "lightgray\0"
    "lightgreen\0"
    "lightgrey\0"
    "lightpink\0"
    "lightsalmon\0"
    "lightseagreen\0"
    "lightskyblue\0"
    "lightslategray\0"
    "lightslategrey\0"
    "lightsteelblue\0"
    "lightyellow\0"
    "limegreen\0"
    "linen\0"
    "magenta\0"
    "mediumaquamarine\0"
    "mediumblue\0"
    "mediumorchid\0"
    "mediumpurple\0"
    "mediumseagreen\0"
    "mediumslateblue\0"
    "mediumspringgreen\0"
    "mediumturquoise\0"
    "mediumvioletred\0"
    "midnightblue\0"
    "mintcream\0"
    "mistyrose\0"
    "moccasin\0"
    "navajowhite\0"
    "oldlace\0"
    "olivedrab\0"
    "orangered\0"
    "orchid\0"
    "palegoldenrod\0"
    "palegreen\0"
    "paleturquoise\0"
    "palevioletred\0"
    "papayawhip\0"
    "peachpuff\0"
    "peru\0"
    "pink\0"
    "plum\0"
    "powderblue\0"
    "rebeccapurple\0"
    "rosybrown\0"
    "royalblue\0"
    "saddlebrown\0"
    "salmon\0"
    "sandybrown\0"
    "seagreen\0"
    "seashell\0"
    "sienna\0"
    "skyblue\0"
    "slateblue\0"
    "slategray\0"
    "slategrey\0"
    "snow\0"
    "springgreen\0"
    "steelblue\0"
    "tan\0"
    "thistle\0"
    "tomato\0"
    "turquoise\0"
    "violet\0"
    "wheat\0"
    "whitesmoke\0"
    "yellowgreen\0"
    "alpha\0"
    "luminance\0"
    "accumulate\0"
    "new\0"
    "sRGB\0"
    "linearRGB\0"
    "crispEdges\0"
    "butt\0"
    "miter\0"
    "bevel\0"
    "before-edge\0"
    "after-edge\0"
    "central\0"
    "text-before-edge\0"
    "text-after-edge\0"
    "ideographic\0"
    "hanging\0"
    "mathematical\0"
    "use-script\0"
    "no-change\0"
    "reset-size\0"
    "dynamic\0"
    "non-scaling-stroke\0"
};

static const unsigned short valueListStringOffsets[] = {
  0,
  1,
  9,
  17,
  22,
  29,
  35,
  42,
  49,
  55,
  62,
  69,
  75,
  82,
  90,
  95,
  100,
  112,
  126,
  147,
  169,
  185,
  196,
  203,
  211,
  215,
  226,
  243,
  263,
  287,
  314,
  335,
  359,
  370,
  384,
  391,
  396,
  403,
  411,
  415,
  419,
  423,
  427,
  431,
  435,
  439,
  443,
  447,
  463,
  479,
  489,
  504,
  518,
  527,
  542,
  557,
  566,
  574,
  580,
  587,
  593,
  601,
  610,
  628,
  636,
  643,
  649,
  660,
  668,
  676,
  686,
  699,
  718,
  723,
  729,
  734,
  742,
  747,
  753,
  758,
  765,
  770,
  776,
  783,
  790,
  794,
  801,
  806,
  812,
  819,
  831,
  844,
  863,
  876,
  890,
  903,
  914,
  925,
  941,
  954,
  965,
  977,
  986,
  996,
  1010,
  1025,
  1041,
  1061,
  1076,
  1085,
  1094,
  1104,
  1121,
  1132,
  1148,
  1166,
  1179,
  1186,
  1198,
  1209,
  1245,
  1286,
  1324,
  1367,
  1392,
  1405,
  1410,
  1423,
  1430,
  1439,
  1448,
  1458,
  1464,
  1469,
  1481,
  1491,
  1502,
  1514,
  1531,
  1546,
  1562,
  1579,
  1583,
  1596,
  1605,
  1612,
  1616,
  1622,
  1631,
  1643,
  1647,
  1654,
  1678,
  1691,
  1696,
  1702,
  1709,
  1717,
  1730,
  1744,
  1759,
  1780,
  1791,
  1802,
  1810,
  1817,
  1822,
  1829,
  1836,
  1844,
  1865,
  1878,
  1886,
  1896,
  1902,
  1913,
  1922,
  1931,
  1939,
  1943,
  1953,
  1963,
  1971,
  1977,
  1985,
  1990,
  1997,
  2005,
  2010,
  2022,
  2034,
  2046,
  2058,
  2070,
  2082,
  2094,
  2113,
  2131,
  2149,
  2170,
  2194,
  2218,
  2225,
  2242,
  2263,
  2283,
  2305,
  2312,
  2321,
  2336,
  2351,
  2360,
  2376,
  2396,
  2418,
  2438,
  2460,
  2469,
  2478,
  2493,
  2508,
  2515,
  2521,
  2531,
  2544,
  2550,
  2563,
  2579,
  2598,
  2617,
  2627,
  2646,
  2659,
  2670,
  2684,
  2696,
  2715,
  2720,
  2732,
  2737,
  2749,
  2762,
  2782,
  2787,
  2797,
  2805,
  2813,
  2818,
  2832,
  2837,
  2850,
  2856,
  2865,
  2873,
  2885,
  2893,
  2902,
  2911,
  2921,
  2931,
  2940,
  2950,
  2960,
  2969,
  2978,
  2988,
  2998,
  3010,
  3022,
  3033,
  3044,
  3049,
  3054,
  3059,
  3070,
  3083,
  3100,
  3116,
  3133,
  3137,
  3141,
  3152,
  3162,
  3172,
  3180,
  3189,
  3192,
  3195,
  3198,
  3204,
  3213,
  3220,
  3226,
  3229,
  3232,
  3238,
  3252,
  3258,
  3263,
  3275,
  3281,
  3287,
  3292,
  3297,
  3304,
  3320,
  3345,
  3363,
  3373,
  3380,
  3386,
  3393,
  3406,
  3412,
  3427,
  3441,
  3448,
  3459,
  3467,
  3476,
  3485,
  3489,
  3498,
  3507,
  3516,
  3523,
  3532,
  3537,
  3544,
  3550,
  3555,
  3565,
  3570,
  3585,
  3593,
  3599,
  3603,
  3609,
  3615,
  3623,
  3634,
  3643,
  3655,
  3666,
  3673,
  3682,
  3693,
  3702,
  3716,
  3729,
  3742,
  3746,
  3758,
  3765,
  3780,
  3785,
  3798,
  3804,
  3814,
  3825,
  3851,
  3859,
  3869,
  3883,
  3903,
  3923,
  3946,
  3966,
  3978,
  3990,
  3995,
  4004,
  4012,
  4022,
  4031,
  4042,
  4048,
  4054,
  4061,
  4079,
  4088,
  4094,
  4106,
  4120,
  4127,
  4140,
  4158,
  4166,
  4175,
  4205,
  4234,
  4265,
  4302,
  4320,
  4338,
  4364,
  4400,
  4413,
  4431,
  4461,
  4481,
  4506,
  4532,
  4569,
  4596,
  4625,
  4657,
  4697,
  4706,
  4722,
  4736,
  4755,
  4761,
  4774,
  4793,
  4811,
  4827,
  4850,
  4871,
  4877,
  4889,
  4912,
  4943,
  4969,
  4979,
  5005,
  5041,
  5075,
  5098,
  5107,
  5127,
  5133,
  5140,
  5151,
  5159,
  5171,
  5179,
  5191,
  5202,
  5210,
  5216,
  5224,
  5231,
  5241,
  5259,
  5268,
  5278,
  5287,
  5295,
  5302,
  5307,
  5319,
  5324,
  5331,
  5339,
  5348,
  5360,
  5371,
  5383,
  5392,
  5398,
  5411,
  5420,
  5426,
  5431,
  5446,
  5458,
  5472,
  5480,
  5485,
  5492,
  5505,
  5515,
  5522,
  5542,
  5557,
  5569,
  5590,
  5604,
  5623,
  5642,
  5650,
  5656,
  5659,
  5662,
  5665,
  5671,
  5677,
  5683,
  5697,
  5709,
  5721,
  5735,
  5741,
  5748,
  5753,
  5759,
  5766,
  5771,
  5775,
  5789,
  5798,
  5805,
  5813,
  5826,
  5841,
  5855,
  5871,
  5880,
  5895,
  5903,
  5918,
  5923,
  5930,
  5941,
  5950,
  5953,
  5957,
  5973,
  5983,
  6009,
  6017,
  6025,
  6028,
  6039,
  6050,
  6061,
  6072,
  6080,
  6087,
  6092,
  6102,
  6108,
  6117,
  6124,
  6131,
  6139,
  6151,
  6162,
  6173,
  6184,
  6195,
  6205,
  6209,
  6220,
  6226,
  6237,
  6248,
  6256,
  6272,
  6288,
  6293,
  6301,
  6306,
  6313,
  6323,
  6335,
  6345,
  6353,
  6378,
  6386,
  6398,
  6410,
  6423,
  6429,
  6435,
  6444,
  6454,
  6461,
  6470,
  6483,
  6497,
  6508,
  6517,
  6522,
  6527,
  6534,
  6541,
  6550,
  6566,
  6572,
  6588,
  6604,
  6630,
  6656,
  6671,
  6690,
  6707,
  6731,
  6755,
  6789,
  6823,
  6841,
  6846,
  6849,
  6860,
  6867,
  6872,
  6880,
  6889,
  6894,
  6902,
  6909,
  6919,
  6925,
  6934,
  6945,
  6953,
  6964,
  6973,
  6978,
  6990,
  6994,
  6998,
  7003,
  7007,
  7012,
  7019,
  7028,
  7040,
  7047,
  7055,
  7063,
  7071,
  7080,
  7086,
  7093,
  7100,
  7107,
  7115,
  7120,
  7126,
  7132,
  7142,
  7153,
  7164,
  7175,
  7187,
  7192,
  7197,
  7210,
  7220,
  7230,
  7242,
  7267,
  7271,
  7281,
  7294,
  7305,
  7311,
  7317,
  7324,
  7339,
  7350,
  7356,
  7366,
  7376,
  7387,
  7397,
  7403,
  7418,
  7427,
  7435,
  7440,
  7449,
  7458,
  7472,
  7481,
  7491,
  7500,
  7510,
  7522,
  7537,
  7548,
  7559,
  7567,
  7578,
  7591,
  7605,
  7619,
  7633,
  7647,
  7658,
  7667,
  7679,
  7687,
  7695,
  7706,
  7716,
  7728,
  7740,
  7750,
  7761,
  7766,
  7776,
  7788,
  7797,
  7805,
  7815,
  7822,
  7828,
  7834,
  7843,
  7857,
  7867,
  7880,
  7890,
  7901,
  7911,
  7932,
  7942,
  7953,
  7963,
  7973,
  7985,
  7999,
  8012,
  8027,
  8042,
  8057,
  8069,
  8079,
  8085,
  8093,
  8110,
  8121,
  8134,
  8147,
  8162,
  8178,
  8196,
  8212,
  8228,
  8241,
  8251,
  8261,
  8270,
  8282,
  8290,
  8300,
  8310,
  8317,
  8331,
  8341,
  8355,
  8369,
  8380,
  8390,
  8395,
  8400,
  8405,
  8416,
  8430,
  8440,
  8450,
  8462,
  8469,
  8480,
  8489,
  8498,
  8505,
  8513,
  8523,
  8533,
  8543,
  8548,
  8560,
  8570,
  8574,
  8582,
  8589,
  8599,
  8606,
  8612,
  8623,
  8635,
  8641,
  8651,
  8662,
  8666,
  8671,
  8681,
  8692,
  8697,
  8703,
  8709,
  8721,
  8732,
  8740,
  8757,
  8773,
  8785,
  8793,
  8806,
  8817,
  8827,
  8838,
  8846,
};

/* maximum key range = 7006, duplicates = 0 */

class CSSValueKeywordsHash
{
private:
  static inline unsigned int value_hash_function (const char *str, unsigned int len);
public:
  static const struct Value *findValueImpl (const char *str, unsigned int len);
};

inline unsigned int
CSSValueKeywordsHash::value_hash_function (register const char *str, register unsigned int len)
{
  static const unsigned short asso_values[] =
    {
      7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015,
      7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015,
      7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015,
      7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015,
      7015, 7015, 7015, 7015, 7015,  115,  137, 7015,    3, 7015,
        14,    4,   12,    7,    9,    8,    3,    6,    5, 7015,
      7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015,
      7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015,
      7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015,
      7015, 7015, 7015, 7015, 7015, 7015, 7015,    6,  419,  216,
         4,    3,  444,   57,   26,   87,  553,  701,   13,  106,
         3,   97,  236, 1549,    2,   24,    4,   50, 1386,  579,
       771, 1231,  768,   20,    5,    2, 7015, 7015, 7015, 7015,
      7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015,
      7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015,
      7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015,
      7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015,
      7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015,
      7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015,
      7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015,
      7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015,
      7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015,
      7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015,
      7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015,
      7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015,
      7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015, 7015
    };
  register int hval = 0;

  switch (len)
    {
      default:
        hval += asso_values[(unsigned char)str[41]];
      /*FALLTHROUGH*/
      case 41:
        hval += asso_values[(unsigned char)str[40]];
      /*FALLTHROUGH*/
      case 40:
        hval += asso_values[(unsigned char)str[39]];
      /*FALLTHROUGH*/
      case 39:
        hval += asso_values[(unsigned char)str[38]];
      /*FALLTHROUGH*/
      case 38:
        hval += asso_values[(unsigned char)str[37]];
      /*FALLTHROUGH*/
      case 37:
        hval += asso_values[(unsigned char)str[36]];
      /*FALLTHROUGH*/
      case 36:
        hval += asso_values[(unsigned char)str[35]];
      /*FALLTHROUGH*/
      case 35:
        hval += asso_values[(unsigned char)str[34]];
      /*FALLTHROUGH*/
      case 34:
        hval += asso_values[(unsigned char)str[33]];
      /*FALLTHROUGH*/
      case 33:
        hval += asso_values[(unsigned char)str[32]];
      /*FALLTHROUGH*/
      case 32:
        hval += asso_values[(unsigned char)str[31]];
      /*FALLTHROUGH*/
      case 31:
        hval += asso_values[(unsigned char)str[30]];
      /*FALLTHROUGH*/
      case 30:
        hval += asso_values[(unsigned char)str[29]];
      /*FALLTHROUGH*/
      case 29:
        hval += asso_values[(unsigned char)str[28]];
      /*FALLTHROUGH*/
      case 28:
        hval += asso_values[(unsigned char)str[27]];
      /*FALLTHROUGH*/
      case 27:
        hval += asso_values[(unsigned char)str[26]];
      /*FALLTHROUGH*/
      case 26:
        hval += asso_values[(unsigned char)str[25]];
      /*FALLTHROUGH*/
      case 25:
        hval += asso_values[(unsigned char)str[24]];
      /*FALLTHROUGH*/
      case 24:
        hval += asso_values[(unsigned char)str[23]];
      /*FALLTHROUGH*/
      case 23:
        hval += asso_values[(unsigned char)str[22]];
      /*FALLTHROUGH*/
      case 22:
        hval += asso_values[(unsigned char)str[21]];
      /*FALLTHROUGH*/
      case 21:
        hval += asso_values[(unsigned char)str[20]];
      /*FALLTHROUGH*/
      case 20:
        hval += asso_values[(unsigned char)str[19]];
      /*FALLTHROUGH*/
      case 19:
        hval += asso_values[(unsigned char)str[18]];
      /*FALLTHROUGH*/
      case 18:
        hval += asso_values[(unsigned char)str[17]];
      /*FALLTHROUGH*/
      case 17:
        hval += asso_values[(unsigned char)str[16]];
      /*FALLTHROUGH*/
      case 16:
        hval += asso_values[(unsigned char)str[15]];
      /*FALLTHROUGH*/
      case 15:
        hval += asso_values[(unsigned char)str[14]];
      /*FALLTHROUGH*/
      case 14:
        hval += asso_values[(unsigned char)str[13]];
      /*FALLTHROUGH*/
      case 13:
        hval += asso_values[(unsigned char)str[12]+1];
      /*FALLTHROUGH*/
      case 12:
        hval += asso_values[(unsigned char)str[11]];
      /*FALLTHROUGH*/
      case 11:
        hval += asso_values[(unsigned char)str[10]+1];
      /*FALLTHROUGH*/
      case 10:
        hval += asso_values[(unsigned char)str[9]];
      /*FALLTHROUGH*/
      case 9:
        hval += asso_values[(unsigned char)str[8]];
      /*FALLTHROUGH*/
      case 8:
        hval += asso_values[(unsigned char)str[7]];
      /*FALLTHROUGH*/
      case 7:
        hval += asso_values[(unsigned char)str[6]];
      /*FALLTHROUGH*/
      case 6:
        hval += asso_values[(unsigned char)str[5]];
      /*FALLTHROUGH*/
      case 5:
        hval += asso_values[(unsigned char)str[4]];
      /*FALLTHROUGH*/
      case 4:
        hval += asso_values[(unsigned char)str[3]];
      /*FALLTHROUGH*/
      case 3:
        hval += asso_values[(unsigned char)str[2]];
      /*FALLTHROUGH*/
      case 2:
        hval += asso_values[(unsigned char)str[1]+3];
      /*FALLTHROUGH*/
      case 1:
        hval += asso_values[(unsigned char)str[0]+1];
        break;
    }
  return hval;
}

struct stringpool_t
  {
    char stringpool_str0[sizeof("dot")];
    char stringpool_str1[sizeof("700")];
    char stringpool_str2[sizeof("200")];
    char stringpool_str3[sizeof("900")];
    char stringpool_str4[sizeof("800")];
    char stringpool_str5[sizeof("400")];
    char stringpool_str6[sizeof("600")];
    char stringpool_str7[sizeof("500")];
    char stringpool_str8[sizeof("caret")];
    char stringpool_str9[sizeof("cyan")];
    char stringpool_str10[sizeof("300")];
    char stringpool_str11[sizeof("dotted")];
    char stringpool_str12[sizeof("100")];
    char stringpool_str13[sizeof("content")];
    char stringpool_str14[sizeof("miter")];
    char stringpool_str15[sizeof("coral")];
    char stringpool_str16[sizeof("sienna")];
    char stringpool_str17[sizeof("span")];
    char stringpool_str18[sizeof("meter")];
    char stringpool_str19[sizeof("kannada")];
    char stringpool_str20[sizeof("middle")];
    char stringpool_str21[sizeof("coarse")];
    char stringpool_str22[sizeof("center")];
    char stringpool_str23[sizeof("rotate")];
    char stringpool_str24[sizeof("gold")];
    char stringpool_str25[sizeof("contents")];
    char stringpool_str26[sizeof("contrast")];
    char stringpool_str27[sizeof("condensed")];
    char stringpool_str28[sizeof("rotate3d")];
    char stringpool_str29[sizeof("to")];
    char stringpool_str30[sizeof("tb")];
    char stringpool_str31[sizeof("red")];
    char stringpool_str32[sizeof("cell")];
    char stringpool_str33[sizeof("tan")];
    char stringpool_str34[sizeof("central")];
    char stringpool_str35[sizeof("dense")];
    char stringpool_str36[sizeof("dashed")];
    char stringpool_str37[sizeof("font")];
    char stringpool_str38[sizeof("counter")];
    char stringpool_str39[sizeof("square")];
    char stringpool_str40[sizeof("fine")];
    char stringpool_str41[sizeof("saturate")];
    char stringpool_str42[sizeof("magenta")];
    char stringpool_str43[sizeof("reset")];
    char stringpool_str44[sizeof("menu")];
    char stringpool_str45[sizeof("round")];
    char stringpool_str46[sizeof("green")];
    char stringpool_str47[sizeof("counters")];
    char stringpool_str48[sizeof("single")];
    char stringpool_str49[sizeof("teal")];
    char stringpool_str50[sizeof("fill")];
    char stringpool_str51[sizeof("hand")];
    char stringpool_str52[sizeof("ridge")];
    char stringpool_str53[sizeof("filled")];
    char stringpool_str54[sizeof("seagreen")];
    char stringpool_str55[sizeof("none")];
    char stringpool_str56[sizeof("hide")];
    char stringpool_str57[sizeof("contain")];
    char stringpool_str58[sizeof("solid")];
    char stringpool_str59[sizeof("clear")];
    char stringpool_str60[sizeof("hidden")];
    char stringpool_str61[sizeof("seashell")];
    char stringpool_str62[sizeof("drag")];
    char stringpool_str63[sizeof("color")];
    char stringpool_str64[sizeof("rl")];
    char stringpool_str65[sizeof("right")];
    char stringpool_str66[sizeof("linen")];
    char stringpool_str67[sizeof("myanmar")];
    char stringpool_str68[sizeof("linear")];
    char stringpool_str69[sizeof("radial")];
    char stringpool_str70[sizeof("letter")];
    char stringpool_str71[sizeof("true")];
    char stringpool_str72[sizeof("goldenrod")];
    char stringpool_str73[sizeof("lr")];
    char stringpool_str74[sizeof("translate")];
    char stringpool_str75[sizeof("flat")];
    char stringpool_str76[sizeof("translate3d")];
    char stringpool_str77[sizeof("grid")];
    char stringpool_str78[sizeof("sesame")];
    char stringpool_str79[sizeof("large")];
    char stringpool_str80[sizeof("larger")];
    char stringpool_str81[sizeof("format")];
    char stringpool_str82[sizeof("column")];
    char stringpool_str83[sizeof("hiragana")];
    char stringpool_str84[sizeof("tb-rl")];
    char stringpool_str85[sizeof("digits")];
    char stringpool_str86[sizeof("ledger")];
    char stringpool_str87[sizeof("clone")];
    char stringpool_str88[sizeof("maroon")];
    char stringpool_str89[sizeof("lao")];
    char stringpool_str90[sizeof("legal")];
    char stringpool_str91[sizeof("menulist")];
    char stringpool_str92[sizeof("lighter")];
    char stringpool_str93[sizeof("lighten")];
    char stringpool_str94[sizeof("hangul")];
    char stringpool_str95[sizeof("radio")];
    char stringpool_str96[sizeof("b5")];
    char stringpool_str97[sizeof("b4")];
    char stringpool_str98[sizeof("zoom")];
    char stringpool_str99[sizeof("normal")];
    char stringpool_str100[sizeof("salmon")];
    char stringpool_str101[sizeof("lime")];
    char stringpool_str102[sizeof("border")];
    char stringpool_str103[sizeof("loose")];
    char stringpool_str104[sizeof("bold")];
    char stringpool_str105[sizeof("calc")];
    char stringpool_str106[sizeof("bolder")];
    char stringpool_str107[sizeof("telugu")];
    char stringpool_str108[sizeof("both")];
    char stringpool_str109[sizeof("circle")];
    char stringpool_str110[sizeof("space")];
    char stringpool_str111[sizeof("forestgreen")];
    char stringpool_str112[sizeof("disc")];
    char stringpool_str113[sizeof("saturation")];
    char stringpool_str114[sizeof("tomato")];
    char stringpool_str115[sizeof("open")];
    char stringpool_str116[sizeof("discard")];
    char stringpool_str117[sizeof("triangle")];
    char stringpool_str118[sizeof("rect")];
    char stringpool_str119[sizeof("mongolian")];
    char stringpool_str120[sizeof("small")];
    char stringpool_str121[sizeof("lightgreen")];
    char stringpool_str122[sizeof("medium")];
    char stringpool_str123[sizeof("smaller")];
    char stringpool_str124[sizeof("no-drag")];
    char stringpool_str125[sizeof("separate")];
    char stringpool_str126[sizeof("top")];
    char stringpool_str127[sizeof("limegreen")];
    char stringpool_str128[sizeof("hanging")];
    char stringpool_str129[sizeof("repeat")];
    char stringpool_str130[sizeof("collapse")];
    char stringpool_str131[sizeof("georgian")];
    char stringpool_str132[sizeof("from")];
    char stringpool_str133[sizeof("crosshair")];
    char stringpool_str134[sizeof("spell-out")];
    char stringpool_str135[sizeof("springgreen")];
    char stringpool_str136[sizeof("local")];
    char stringpool_str137[sizeof("listitem")];
    char stringpool_str138[sizeof("baseline")];
    char stringpool_str139[sizeof("orange")];
    char stringpool_str140[sizeof("mintcream")];
    char stringpool_str141[sizeof("sepia")];
    char stringpool_str142[sizeof("help")];
    char stringpool_str143[sizeof("orangered")];
    char stringpool_str144[sizeof("blur")];
    char stringpool_str145[sizeof("blue")];
    char stringpool_str146[sizeof("highlight")];
    char stringpool_str147[sizeof("crimson")];
    char stringpool_str148[sizeof("document")];
    char stringpool_str149[sizeof("beige")];
    char stringpool_str150[sizeof("hard-light")];
    char stringpool_str151[sizeof("slice")];
    char stringpool_str152[sizeof("bengali")];
    char stringpool_str153[sizeof("min-intrinsic")];
    char stringpool_str154[sizeof("a5")];
    char stringpool_str155[sizeof("clip")];
    char stringpool_str156[sizeof("keep-all")];
    char stringpool_str157[sizeof("dynamic")];
    char stringpool_str158[sizeof("a4")];
    char stringpool_str159[sizeof("a3")];
    char stringpool_str160[sizeof("bottom")];
    char stringpool_str161[sizeof("zoom-in")];
    char stringpool_str162[sizeof("transparent")];
    char stringpool_str163[sizeof("caption")];
    char stringpool_str164[sizeof("safe")];
    char stringpool_str165[sizeof("decimal")];
    char stringpool_str166[sizeof("screen")];
    char stringpool_str167[sizeof("balance")];
    char stringpool_str168[sizeof("list-item")];
    char stringpool_str169[sizeof("smooth")];
    char stringpool_str170[sizeof("no-repeat")];
    char stringpool_str171[sizeof("scale")];
    char stringpool_str172[sizeof("line-through")];
    char stringpool_str173[sizeof("ease")];
    char stringpool_str174[sizeof("azure")];
    char stringpool_str175[sizeof("scale3d")];
    char stringpool_str176[sizeof("aqua")];
    char stringpool_str177[sizeof("source-in")];
    char stringpool_str178[sizeof("minimal-ui")];
    char stringpool_str179[sizeof("closest-side")];
    char stringpool_str180[sizeof("logical")];
    char stringpool_str181[sizeof("table")];
    char stringpool_str182[sizeof("double")];
    char stringpool_str183[sizeof("zoom-out")];
    char stringpool_str184[sizeof("crispedges")];
    char stringpool_str185[sizeof("brightness")];
    char stringpool_str186[sizeof("tibetan")];
    char stringpool_str187[sizeof("cadetblue")];
    char stringpool_str188[sizeof("min-content")];
    char stringpool_str189[sizeof("after")];
    char stringpool_str190[sizeof("no-change")];
    char stringpool_str191[sizeof("replaced")];
    char stringpool_str192[sizeof("all")];
    char stringpool_str193[sizeof("srgb")];
    char stringpool_str194[sizeof("lightcoral")];
    char stringpool_str195[sizeof("alternate")];
    char stringpool_str196[sizeof("source-out")];
    char stringpool_str197[sizeof("default")];
    char stringpool_str198[sizeof("lightsalmon")];
    char stringpool_str199[sizeof("no-drop")];
    char stringpool_str200[sizeof("dodgerblue")];
    char stringpool_str201[sizeof("moccasin")];
    char stringpool_str202[sizeof("serif")];
    char stringpool_str203[sizeof("fit-content")];
    char stringpool_str204[sizeof("scroll")];
    char stringpool_str205[sizeof("oldlace")];
    char stringpool_str206[sizeof("left")];
    char stringpool_str207[sizeof("var")];
    char stringpool_str208[sizeof("skew")];
    char stringpool_str209[sizeof("monospace")];
    char stringpool_str210[sizeof("color-stop")];
    char stringpool_str211[sizeof("start")];
    char stringpool_str212[sizeof("hiragana-iroha")];
    char stringpool_str213[sizeof("slateblue")];
    char stringpool_str214[sizeof("landscape")];
    char stringpool_str215[sizeof("row")];
    char stringpool_str216[sizeof("distribute")];
    char stringpool_str217[sizeof("linearrgb")];
    char stringpool_str218[sizeof("self-end")];
    char stringpool_str219[sizeof("absolute")];
    char stringpool_str220[sizeof("rtl")];
    char stringpool_str221[sizeof("orchid")];
    char stringpool_str222[sizeof("space-around")];
    char stringpool_str223[sizeof("alias")];
    char stringpool_str224[sizeof("self-start")];
    char stringpool_str225[sizeof("rl-tb")];
    char stringpool_str226[sizeof("element")];
    char stringpool_str227[sizeof("forwards")];
    char stringpool_str228[sizeof("armenian")];
    char stringpool_str229[sizeof("ease-in")];
    char stringpool_str230[sizeof("visual")];
    char stringpool_str231[sizeof("aquamarine")];
    char stringpool_str232[sizeof("ltr")];
    char stringpool_str233[sizeof("exact")];
    char stringpool_str234[sizeof("lightblue")];
    char stringpool_str235[sizeof("lr-tb")];
    char stringpool_str236[sizeof("after-edge")];
    char stringpool_str237[sizeof("new")];
    char stringpool_str238[sizeof("color-burn")];
    char stringpool_str239[sizeof("violet")];
    char stringpool_str240[sizeof("sans-serif")];
    char stringpool_str241[sizeof("darken")];
    char stringpool_str242[sizeof("standalone")];
    char stringpool_str243[sizeof("darkred")];
    char stringpool_str244[sizeof("expanded")];
    char stringpool_str245[sizeof("cambodian")];
    char stringpool_str246[sizeof("manipulation")];
    char stringpool_str247[sizeof("markers")];
    char stringpool_str248[sizeof("ease-out")];
    char stringpool_str249[sizeof("katakana")];
    char stringpool_str250[sizeof("soft-light")];
    char stringpool_str251[sizeof("gainsboro")];
    char stringpool_str252[sizeof("lawngreen")];
    char stringpool_str253[sizeof("mediumblue")];
    char stringpool_str254[sizeof("off")];
    char stringpool_str255[sizeof("darkgreen")];
    char stringpool_str256[sizeof("alpha")];
    char stringpool_str257[sizeof("before")];
    char stringpool_str258[sizeof("blanchedalmond")];
    char stringpool_str259[sizeof("farthest-corner")];
    char stringpool_str260[sizeof("chartreuse")];
    char stringpool_str261[sizeof("rotatez")];
    char stringpool_str262[sizeof("rotatex")];
    char stringpool_str263[sizeof("media-slider")];
    char stringpool_str264[sizeof("mediumspringgreen")];
    char stringpool_str265[sizeof("khmer")];
    char stringpool_str266[sizeof("searchfield")];
    char stringpool_str267[sizeof("stretch")];
    char stringpool_str268[sizeof("cornsilk")];
    char stringpool_str269[sizeof("color-dodge")];
    char stringpool_str270[sizeof("mediumseagreen")];
    char stringpool_str271[sizeof("thin")];
    char stringpool_str272[sizeof("lightseagreen")];
    char stringpool_str273[sizeof("destination-out")];
    char stringpool_str274[sizeof("thai")];
    char stringpool_str275[sizeof("steps")];
    char stringpool_str276[sizeof("fixed")];
    char stringpool_str277[sizeof("table-cell")];
    char stringpool_str278[sizeof("read-write")];
    char stringpool_str279[sizeof("text")];
    char stringpool_str280[sizeof("linear-gradient")];
    char stringpool_str281[sizeof("hangul-consonant")];
    char stringpool_str282[sizeof("radial-gradient")];
    char stringpool_str283[sizeof("contextual")];
    char stringpool_str284[sizeof("hue")];
    char stringpool_str285[sizeof("menutext")];
    char stringpool_str286[sizeof("wait")];
    char stringpool_str287[sizeof("textarea")];
    char stringpool_str288[sizeof("small-caps")];
    char stringpool_str289[sizeof("matrix")];
    char stringpool_str290[sizeof("darkorange")];
    char stringpool_str291[sizeof("matrix3d")];
    char stringpool_str292[sizeof("thistle")];
    char stringpool_str293[sizeof("lower-greek")];
    char stringpool_str294[sizeof("mathematical")];
    char stringpool_str295[sizeof("strict")];
    char stringpool_str296[sizeof("s-resize")];
    char stringpool_str297[sizeof("static")];
    char stringpool_str298[sizeof("square-button")];
    char stringpool_str299[sizeof("minmax")];
    char stringpool_str300[sizeof("ellipse")];
    char stringpool_str301[sizeof("mediumpurple")];
    char stringpool_str302[sizeof("each-line")];
    char stringpool_str303[sizeof("nowrap")];
    char stringpool_str304[sizeof("translatez")];
    char stringpool_str305[sizeof("flex")];
    char stringpool_str306[sizeof("translatex")];
    char stringpool_str307[sizeof("below")];
    char stringpool_str308[sizeof("not-allowed")];
    char stringpool_str309[sizeof("vertical")];
    char stringpool_str310[sizeof("lowercase")];
    char stringpool_str311[sizeof("brown")];
    char stringpool_str312[sizeof("step-end")];
    char stringpool_str313[sizeof("running")];
    char stringpool_str314[sizeof("exclusion")];
    char stringpool_str315[sizeof("darksalmon")];
    char stringpool_str316[sizeof("browser")];
    char stringpool_str317[sizeof("nonzero")];
    char stringpool_str318[sizeof("step-start")];
    char stringpool_str319[sizeof("repeating-radial-gradient")];
    char stringpool_str320[sizeof("table-column")];
    char stringpool_str321[sizeof("n-resize")];
    char stringpool_str322[sizeof("ease-in-out")];
    char stringpool_str323[sizeof("butt")];
    char stringpool_str324[sizeof("rgb")];
    char stringpool_str325[sizeof("at")];
    char stringpool_str326[sizeof("scrollbar")];
    char stringpool_str327[sizeof("rgba")];
    char stringpool_str328[sizeof("attr")];
    char stringpool_str329[sizeof("all-scroll")];
    char stringpool_str330[sizeof("darkgoldenrod")];
    char stringpool_str331[sizeof("ellipsis")];
    char stringpool_str332[sizeof("lower-latin")];
    char stringpool_str333[sizeof("super")];
    char stringpool_str334[sizeof("no-historical-ligatures")];
    char stringpool_str335[sizeof("col-resize")];
    char stringpool_str336[sizeof("se-resize")];
    char stringpool_str337[sizeof("outset")];
    char stringpool_str338[sizeof("flex-end")];
    char stringpool_str339[sizeof("wrap")];
    char stringpool_str340[sizeof("horizontal")];
    char stringpool_str341[sizeof("reset-size")];
    char stringpool_str342[sizeof("flex-start")];
    char stringpool_str343[sizeof("vertical-lr")];
    char stringpool_str344[sizeof("steelblue")];
    char stringpool_str345[sizeof("hue-rotate")];
    char stringpool_str346[sizeof("button")];
    char stringpool_str347[sizeof("icon")];
    char stringpool_str348[sizeof("blink")];
    char stringpool_str349[sizeof("fullscreen")];
    char stringpool_str350[sizeof("embed")];
    char stringpool_str351[sizeof("destination-in")];
    char stringpool_str352[sizeof("lower-roman")];
    char stringpool_str353[sizeof("hebrew")];
    char stringpool_str354[sizeof("hotpink")];
    char stringpool_str355[sizeof("break-all")];
    char stringpool_str356[sizeof("ne-resize")];
    char stringpool_str357[sizeof("ideographic")];
    char stringpool_str358[sizeof("outside")];
    char stringpool_str359[sizeof("multiple")];
    char stringpool_str360[sizeof("geometricprecision")];
    char stringpool_str361[sizeof("difference")];
    char stringpool_str362[sizeof("visible")];
    char stringpool_str363[sizeof("chocolate")];
    char stringpool_str364[sizeof("darkorchid")];
    char stringpool_str365[sizeof("repeating-linear-gradient")];
    char stringpool_str366[sizeof("korean-hangul-formal")];
    char stringpool_str367[sizeof("closest-corner")];
    char stringpool_str368[sizeof("no-punctuation")];
    char stringpool_str369[sizeof("vertical-rl")];
    char stringpool_str370[sizeof("literal-punctuation")];
    char stringpool_str371[sizeof("katakana-iroha")];
    char stringpool_str372[sizeof("context-menu")];
    char stringpool_str373[sizeof("start-touch")];
    char stringpool_str374[sizeof("mediumorchid")];
    char stringpool_str375[sizeof("repeat-x")];
    char stringpool_str376[sizeof("fuchsia")];
    char stringpool_str377[sizeof("sub")];
    char stringpool_str378[sizeof("darkblue")];
    char stringpool_str379[sizeof("threedhighlight")];
    char stringpool_str380[sizeof("status-bar")];
    char stringpool_str381[sizeof("capitalize")];
    char stringpool_str382[sizeof("saddlebrown")];
    char stringpool_str383[sizeof("semi-condensed")];
    char stringpool_str384[sizeof("lightpink")];
    char stringpool_str385[sizeof("xor")];
    char stringpool_str386[sizeof("black")];
    char stringpool_str387[sizeof("scalez")];
    char stringpool_str388[sizeof("scalex")];
    char stringpool_str389[sizeof("darkseagreen")];
    char stringpool_str390[sizeof("no-common-ligatures")];
    char stringpool_str391[sizeof("captiontext")];
    char stringpool_str392[sizeof("farthest-side")];
    char stringpool_str393[sizeof("scale-down")];
    char stringpool_str394[sizeof("max-content")];
    char stringpool_str395[sizeof("rotatey")];
    char stringpool_str396[sizeof("table-row")];
    char stringpool_str397[sizeof("auto")];
    char stringpool_str398[sizeof("alternate-reverse")];
    char stringpool_str399[sizeof("deeppink")];
    char stringpool_str400[sizeof("luminance")];
    char stringpool_str401[sizeof("text-top")];
    char stringpool_str402[sizeof("aliceblue")];
    char stringpool_str403[sizeof("darkmagenta")];
    char stringpool_str404[sizeof("destination-atop")];
    char stringpool_str405[sizeof("grey")];
    char stringpool_str406[sizeof("accumulate")];
    char stringpool_str407[sizeof("gray")];
    char stringpool_str408[sizeof("x-large")];
    char stringpool_str409[sizeof("block")];
    char stringpool_str410[sizeof("fantasy")];
    char stringpool_str411[sizeof("e-resize")];
    char stringpool_str412[sizeof("mediumslateblue")];
    char stringpool_str413[sizeof("background")];
    char stringpool_str414[sizeof("currentcolor")];
    char stringpool_str415[sizeof("mandatory")];
    char stringpool_str416[sizeof("skewx")];
    char stringpool_str417[sizeof("semi-expanded")];
    char stringpool_str418[sizeof("extra-condensed")];
    char stringpool_str419[sizeof("double-circle")];
    char stringpool_str420[sizeof("trad-chinese-formal")];
    char stringpool_str421[sizeof("show")];
    char stringpool_str422[sizeof("stroke")];
    char stringpool_str423[sizeof("malayalam")];
    char stringpool_str424[sizeof("translatey")];
    char stringpool_str425[sizeof("move")];
    char stringpool_str426[sizeof("x-small")];
    char stringpool_str427[sizeof("cover")];
    char stringpool_str428[sizeof("historical-ligatures")];
    char stringpool_str429[sizeof("mistyrose")];
    char stringpool_str430[sizeof("textfield")];
    char stringpool_str431[sizeof("slategrey")];
    char stringpool_str432[sizeof("slategray")];
    char stringpool_str433[sizeof("before-edge")];
    char stringpool_str434[sizeof("floralwhite")];
    char stringpool_str435[sizeof("dimgrey")];
    char stringpool_str436[sizeof("dimgray")];
    char stringpool_str437[sizeof("silver")];
    char stringpool_str438[sizeof("horizontal-tb")];
    char stringpool_str439[sizeof("drop-shadow")];
    char stringpool_str440[sizeof("threedface")];
    char stringpool_str441[sizeof("listbox")];
    char stringpool_str442[sizeof("xx-large")];
    char stringpool_str443[sizeof("url")];
    char stringpool_str444[sizeof("italic")];
    char stringpool_str445[sizeof("gujarati")];
    char stringpool_str446[sizeof("trad-chinese-informal")];
    char stringpool_str447[sizeof("searchfield-decoration")];
    char stringpool_str448[sizeof("media-sliderthumb")];
    char stringpool_str449[sizeof("window")];
    char stringpool_str450[sizeof("reverse")];
    char stringpool_str451[sizeof("media-enter-fullscreen-button")];
    char stringpool_str452[sizeof("over")];
    char stringpool_str453[sizeof("copy")];
    char stringpool_str454[sizeof("hover")];
    char stringpool_str455[sizeof("wheat")];
    char stringpool_str456[sizeof("hsl")];
    char stringpool_str457[sizeof("urdu")];
    char stringpool_str458[sizeof("hsla")];
    char stringpool_str459[sizeof("yellow")];
    char stringpool_str460[sizeof("lightgrey")];
    char stringpool_str461[sizeof("firebrick")];
    char stringpool_str462[sizeof("lightgray")];
    char stringpool_str463[sizeof("lower-alpha")];
    char stringpool_str464[sizeof("khaki")];
    char stringpool_str465[sizeof("lavender")];
    char stringpool_str466[sizeof("alphabetic")];
    char stringpool_str467[sizeof("non-scaling-stroke")];
    char stringpool_str468[sizeof("read-only")];
    char stringpool_str469[sizeof("xx-small")];
    char stringpool_str470[sizeof("lower-armenian")];
    char stringpool_str471[sizeof("buttonhighlight")];
    char stringpool_str472[sizeof("relative")];
    char stringpool_str473[sizeof("ghostwhite")];
    char stringpool_str474[sizeof("margin-box")];
    char stringpool_str475[sizeof("table-column-group")];
    char stringpool_str476[sizeof("white")];
    char stringpool_str477[sizeof("grayscale")];
    char stringpool_str478[sizeof("overline")];
    char stringpool_str479[sizeof("devanagari")];
    char stringpool_str480[sizeof("path")];
    char stringpool_str481[sizeof("upright")];
    char stringpool_str482[sizeof("text-bottom")];
    char stringpool_str483[sizeof("menulist-text")];
    char stringpool_str484[sizeof("step-middle")];
    char stringpool_str485[sizeof("small-caption")];
    char stringpool_str486[sizeof("pre")];
    char stringpool_str487[sizeof("row-resize")];
    char stringpool_str488[sizeof("oriya")];
    char stringpool_str489[sizeof("darkslategrey")];
    char stringpool_str490[sizeof("darkslategray")];
    char stringpool_str491[sizeof("simp-chinese-formal")];
    char stringpool_str492[sizeof("sliderthumb-vertical")];
    char stringpool_str493[sizeof("peru")];
    char stringpool_str494[sizeof("text-after-edge")];
    char stringpool_str495[sizeof("border-box")];
    char stringpool_str496[sizeof("paused")];
    char stringpool_str497[sizeof("palegreen")];
    char stringpool_str498[sizeof("threedshadow")];
    char stringpool_str499[sizeof("highlighttext")];
    char stringpool_str500[sizeof("glyphs")];
    char stringpool_str501[sizeof("legacy")];
    char stringpool_str502[sizeof("bevel")];
    char stringpool_str503[sizeof("repeat-y")];
    char stringpool_str504[sizeof("pointer")];
    char stringpool_str505[sizeof("painted")];
    char stringpool_str506[sizeof("portrait")];
    char stringpool_str507[sizeof("yellowgreen")];
    char stringpool_str508[sizeof("groove")];
    char stringpool_str509[sizeof("w-resize")];
    char stringpool_str510[sizeof("lightcyan")];
    char stringpool_str511[sizeof("table-header-group")];
    char stringpool_str512[sizeof("horizontal-bt")];
    char stringpool_str513[sizeof("korean-hanja-formal")];
    char stringpool_str514[sizeof("persian")];
    char stringpool_str515[sizeof("scaley")];
    char stringpool_str516[sizeof("padding")];
    char stringpool_str517[sizeof("simp-chinese-informal")];
    char stringpool_str518[sizeof("last-baseline")];
    char stringpool_str519[sizeof("searchfield-results-decoration")];
    char stringpool_str520[sizeof("skyblue")];
    char stringpool_str521[sizeof("vertical-right")];
    char stringpool_str522[sizeof("backwards")];
    char stringpool_str523[sizeof("thick")];
    char stringpool_str524[sizeof("royalblue")];
    char stringpool_str525[sizeof("buttonface")];
    char stringpool_str526[sizeof("gurmukhi")];
    char stringpool_str527[sizeof("break-word")];
    char stringpool_str528[sizeof("sw-resize")];
    char stringpool_str529[sizeof("darkslateblue")];
    char stringpool_str530[sizeof("korean-hanja-informal")];
    char stringpool_str531[sizeof("on")];
    char stringpool_str532[sizeof("evenodd")];
    char stringpool_str533[sizeof("visiblefill")];
    char stringpool_str534[sizeof("threedlightshadow")];
    char stringpool_str535[sizeof("lemonchiffon")];
    char stringpool_str536[sizeof("plum")];
    char stringpool_str537[sizeof("opacity")];
    char stringpool_str538[sizeof("progress")];
    char stringpool_str539[sizeof("olive")];
    char stringpool_str540[sizeof("table-caption")];
    char stringpool_str541[sizeof("skewy")];
    char stringpool_str542[sizeof("pre-line")];
    char stringpool_str543[sizeof("avoid")];
    char stringpool_str544[sizeof("pan-right")];
    char stringpool_str545[sizeof("bisque")];
    char stringpool_str546[sizeof("slider-vertical")];
    char stringpool_str547[sizeof("sideways")];
    char stringpool_str548[sizeof("nw-resize")];
    char stringpool_str549[sizeof("palegoldenrod")];
    char stringpool_str550[sizeof("lightsteelblue")];
    char stringpool_str551[sizeof("buttontext")];
    char stringpool_str552[sizeof("sliderthumb-horizontal")];
    char stringpool_str553[sizeof("searchfield-cancel-button")];
    char stringpool_str554[sizeof("content-box")];
    char stringpool_str555[sizeof("uppercase")];
    char stringpool_str556[sizeof("source-over")];
    char stringpool_str557[sizeof("above")];
    char stringpool_str558[sizeof("honeydew")];
    char stringpool_str559[sizeof("space-between")];
    char stringpool_str560[sizeof("lightslategray")];
    char stringpool_str561[sizeof("oblique")];
    char stringpool_str562[sizeof("table-row-group")];
    char stringpool_str563[sizeof("blueviolet")];
    char stringpool_str564[sizeof("pan-up")];
    char stringpool_str565[sizeof("lightslategrey")];
    char stringpool_str566[sizeof("bounding-box")];
    char stringpool_str567[sizeof("buttonshadow")];
    char stringpool_str568[sizeof("upper-latin")];
    char stringpool_str569[sizeof("end")];
    char stringpool_str570[sizeof("discretionary-ligatures")];
    char stringpool_str571[sizeof("darkgrey")];
    char stringpool_str572[sizeof("message-box")];
    char stringpool_str573[sizeof("darkgray")];
    char stringpool_str574[sizeof("mediumaquamarine")];
    char stringpool_str575[sizeof("visiblepainted")];
    char stringpool_str576[sizeof("midnightblue")];
    char stringpool_str577[sizeof("slider-horizontal")];
    char stringpool_str578[sizeof("optimizespeed")];
    char stringpool_str579[sizeof("on-demand")];
    char stringpool_str580[sizeof("source-atop")];
    char stringpool_str581[sizeof("isolate")];
    char stringpool_str582[sizeof("media-volume-slider")];
    char stringpool_str583[sizeof("upper-roman")];
    char stringpool_str584[sizeof("open-quote")];
    char stringpool_str585[sizeof("graytext")];
    char stringpool_str586[sizeof("inset")];
    char stringpool_str587[sizeof("pan-left")];
    char stringpool_str588[sizeof("column-reverse")];
    char stringpool_str589[sizeof("menulist-textfield")];
    char stringpool_str590[sizeof("extra-expanded")];
    char stringpool_str591[sizeof("darkcyan")];
    char stringpool_str592[sizeof("nesw-resize")];
    char stringpool_str593[sizeof("arabic-indic")];
    char stringpool_str594[sizeof("destination-over")];
    char stringpool_str595[sizeof("scroll-event")];
    char stringpool_str596[sizeof("antialiased")];
    char stringpool_str597[sizeof("inline")];
    char stringpool_str598[sizeof("indianred")];
    char stringpool_str599[sizeof("greenyellow")];
    char stringpool_str600[sizeof("ew-resize")];
    char stringpool_str601[sizeof("inside")];
    char stringpool_str602[sizeof("cjk-ideographic")];
    char stringpool_str603[sizeof("inherit")];
    char stringpool_str604[sizeof("snow")];
    char stringpool_str605[sizeof("darkkhaki")];
    char stringpool_str606[sizeof("olivedrab")];
    char stringpool_str607[sizeof("windowtext")];
    char stringpool_str608[sizeof("scroll-position")];
    char stringpool_str609[sizeof("pink")];
    char stringpool_str610[sizeof("table-footer-group")];
    char stringpool_str611[sizeof("cursive")];
    char stringpool_str612[sizeof("menulist-button")];
    char stringpool_str613[sizeof("initial")];
    char stringpool_str614[sizeof("darkviolet")];
    char stringpool_str615[sizeof("-webkit-left")];
    char stringpool_str616[sizeof("vertical-text")];
    char stringpool_str617[sizeof("block-axis")];
    char stringpool_str618[sizeof("appworkspace")];
    char stringpool_str619[sizeof("cjk-earthly-branch")];
    char stringpool_str620[sizeof("lightyellow")];
    char stringpool_str621[sizeof("nwse-resize")];
    char stringpool_str622[sizeof("mediumvioletred")];
    char stringpool_str623[sizeof("sandybrown")];
    char stringpool_str624[sizeof("indigo")];
    char stringpool_str625[sizeof("interlace")];
    char stringpool_str626[sizeof("after-white-space")];
    char stringpool_str627[sizeof("pan-down")];
    char stringpool_str628[sizeof("always")];
    char stringpool_str629[sizeof("multiply")];
    char stringpool_str630[sizeof("pixelated")];
    char stringpool_str631[sizeof("rosybrown")];
    char stringpool_str632[sizeof("ultra-condensed")];
    char stringpool_str633[sizeof("economy")];
    char stringpool_str634[sizeof("-webkit-right")];
    char stringpool_str635[sizeof("common-ligatures")];
    char stringpool_str636[sizeof("-webkit-auto")];
    char stringpool_str637[sizeof("pan-x")];
    char stringpool_str638[sizeof("media-mute-button")];
    char stringpool_str639[sizeof("upper-alpha")];
    char stringpool_str640[sizeof("plus-lighter")];
    char stringpool_str641[sizeof("windowframe")];
    char stringpool_str642[sizeof("inline-grid")];
    char stringpool_str643[sizeof("media-controls-background")];
    char stringpool_str644[sizeof("close-quote")];
    char stringpool_str645[sizeof("ns-resize")];
    char stringpool_str646[sizeof("upper-armenian")];
    char stringpool_str647[sizeof("caps-lock-indicator")];
    char stringpool_str648[sizeof("whitesmoke")];
    char stringpool_str649[sizeof("luminosity")];
    char stringpool_str650[sizeof("intrinsic")];
    char stringpool_str651[sizeof("media-toggle-closed-captions-button")];
    char stringpool_str652[sizeof("pre-wrap")];
    char stringpool_str653[sizeof("visiblestroke")];
    char stringpool_str654[sizeof("-internal-media-cast-off-button")];
    char stringpool_str655[sizeof("purple")];
    char stringpool_str656[sizeof("wrap-reverse")];
    char stringpool_str657[sizeof("row-reverse")];
    char stringpool_str658[sizeof("media-volume-slider-container")];
    char stringpool_str659[sizeof("threeddarkshadow")];
    char stringpool_str660[sizeof("decimal-leading-zero")];
    char stringpool_str661[sizeof("powderblue")];
    char stringpool_str662[sizeof("turquoise")];
    char stringpool_str663[sizeof("lightgoldenrodyellow")];
    char stringpool_str664[sizeof("media-volume-sliderthumb")];
    char stringpool_str665[sizeof("sideways-right")];
    char stringpool_str666[sizeof("navy")];
    char stringpool_str667[sizeof("overlay")];
    char stringpool_str668[sizeof("infinite")];
    char stringpool_str669[sizeof("-webkit-min-content")];
    char stringpool_str670[sizeof("progress-bar")];
    char stringpool_str671[sizeof("cornflowerblue")];
    char stringpool_str672[sizeof("-webkit-calc")];
    char stringpool_str673[sizeof("-webkit-control")];
    char stringpool_str674[sizeof("rating-level-indicator")];
    char stringpool_str675[sizeof("no-contextual")];
    char stringpool_str676[sizeof("darkolivegreen")];
    char stringpool_str677[sizeof("inline-table")];
    char stringpool_str678[sizeof("no-discretionary-ligatures")];
    char stringpool_str679[sizeof("pan-y")];
    char stringpool_str680[sizeof("inter-word")];
    char stringpool_str681[sizeof("checkbox")];
    char stringpool_str682[sizeof("bidi-override")];
    char stringpool_str683[sizeof("ethiopic-halehame")];
    char stringpool_str684[sizeof("under")];
    char stringpool_str685[sizeof("no-open-quote")];
    char stringpool_str686[sizeof("polygon")];
    char stringpool_str687[sizeof("unset")];
    char stringpool_str688[sizeof("-internal-extend-to-zoom")];
    char stringpool_str689[sizeof("peachpuff")];
    char stringpool_str690[sizeof("text-before-edge")];
    char stringpool_str691[sizeof("-webkit-center")];
    char stringpool_str692[sizeof("-internal-variable-value")];
    char stringpool_str693[sizeof("burlywood")];
    char stringpool_str694[sizeof("-webkit-isolate")];
    char stringpool_str695[sizeof("media-fullscreen-volume-slider")];
    char stringpool_str696[sizeof("media-current-time-display")];
    char stringpool_str697[sizeof("-webkit-radial-gradient")];
    char stringpool_str698[sizeof("-webkit-fit-content")];
    char stringpool_str699[sizeof("underline")];
    char stringpool_str700[sizeof("wheel-event")];
    char stringpool_str701[sizeof("-webkit-match-parent")];
    char stringpool_str702[sizeof("optimizelegibility")];
    char stringpool_str703[sizeof("media-time-remaining-display")];
    char stringpool_str704[sizeof("space-evenly")];
    char stringpool_str705[sizeof("ivory")];
    char stringpool_str706[sizeof("-webkit-pictograph")];
    char stringpool_str707[sizeof("-webkit-grab")];
    char stringpool_str708[sizeof("-webkit-link")];
    char stringpool_str709[sizeof("-webkit-linear-gradient")];
    char stringpool_str710[sizeof("preserve-3d")];
    char stringpool_str711[sizeof("deepskyblue")];
    char stringpool_str712[sizeof("push-button")];
    char stringpool_str713[sizeof("ethiopic-halehame-am")];
    char stringpool_str714[sizeof("ultra-expanded")];
    char stringpool_str715[sizeof("-webkit-mini-control")];
    char stringpool_str716[sizeof("palevioletred")];
    char stringpool_str717[sizeof("inline-axis")];
    char stringpool_str718[sizeof("-webkit-baseline-middle")];
    char stringpool_str719[sizeof("rebeccapurple")];
    char stringpool_str720[sizeof("subpixel-antialiased")];
    char stringpool_str721[sizeof("ethiopic-halehame-ti-er")];
    char stringpool_str722[sizeof("ethiopic-halehame-ti-et")];
    char stringpool_str723[sizeof("justify")];
    char stringpool_str724[sizeof("-webkit-gradient")];
    char stringpool_str725[sizeof("navajowhite")];
    char stringpool_str726[sizeof("-webkit-cross-fade")];
    char stringpool_str727[sizeof("activeborder")];
    char stringpool_str728[sizeof("cubic-bezier")];
    char stringpool_str729[sizeof("media-controls-fullscreen-background")];
    char stringpool_str730[sizeof("wavy")];
    char stringpool_str731[sizeof("-webkit-paged-x")];
    char stringpool_str732[sizeof("infotext")];
    char stringpool_str733[sizeof("lavenderblush")];
    char stringpool_str734[sizeof("-webkit-small-control")];
    char stringpool_str735[sizeof("use-script")];
    char stringpool_str736[sizeof("button-bevel")];
    char stringpool_str737[sizeof("-webkit-text")];
    char stringpool_str738[sizeof("darkturquoise")];
    char stringpool_str739[sizeof("-webkit-image-set")];
    char stringpool_str740[sizeof("-webkit-grabbing")];
    char stringpool_str741[sizeof("invert")];
    char stringpool_str742[sizeof("activecaption")];
    char stringpool_str743[sizeof("inline-block")];
    char stringpool_str744[sizeof("padding-box")];
    char stringpool_str745[sizeof("inline-box")];
    char stringpool_str746[sizeof("-webkit-focus-ring-color")];
    char stringpool_str747[sizeof("read-write-plaintext-only")];
    char stringpool_str748[sizeof("-webkit-zoom-in")];
    char stringpool_str749[sizeof("progressive")];
    char stringpool_str750[sizeof("-webkit-zoom-out")];
    char stringpool_str751[sizeof("media-fullscreen-volume-slider-thumb")];
    char stringpool_str752[sizeof("-webkit-nowrap")];
    char stringpool_str753[sizeof("no-close-quote")];
    char stringpool_str754[sizeof("media-play-button")];
    char stringpool_str755[sizeof("-webkit-max-content")];
    char stringpool_str756[sizeof("-webkit-paged-y")];
    char stringpool_str757[sizeof("-webkit-plaintext")];
    char stringpool_str758[sizeof("-webkit-flex")];
    char stringpool_str759[sizeof("mediumturquoise")];
    char stringpool_str760[sizeof("papayawhip")];
    char stringpool_str761[sizeof("-webkit-inline-flex")];
    char stringpool_str762[sizeof("cjk-heavenly-stem")];
    char stringpool_str763[sizeof("perspective")];
    char stringpool_str764[sizeof("-webkit-activelink")];
    char stringpool_str765[sizeof("-webkit-box")];
    char stringpool_str766[sizeof("proximity")];
    char stringpool_str767[sizeof("lightskyblue")];
    char stringpool_str768[sizeof("-webkit-body")];
    char stringpool_str769[sizeof("inline-flex")];
    char stringpool_str770[sizeof("-webkit-inline-box")];
    char stringpool_str771[sizeof("-webkit-optimize-contrast")];
    char stringpool_str772[sizeof("media-exit-fullscreen-button")];
    char stringpool_str773[sizeof("paleturquoise")];
    char stringpool_str774[sizeof("relevancy-level-indicator")];
    char stringpool_str775[sizeof("progress-bar-value")];
    char stringpool_str776[sizeof("-webkit-canvas")];
    char stringpool_str777[sizeof("antiquewhite")];
    char stringpool_str778[sizeof("infobackground")];
    char stringpool_str779[sizeof("inner-spin-button")];
    char stringpool_str780[sizeof("-internal-active-list-box-selection")];
    char stringpool_str781[sizeof("-webkit-isolate-override")];
    char stringpool_str782[sizeof("inactiveborder")];
    char stringpool_str783[sizeof("-webkit-repeating-linear-gradient")];
    char stringpool_str784[sizeof("-webkit-repeating-radial-gradient")];
    char stringpool_str785[sizeof("media-overlay-play-button")];
    char stringpool_str786[sizeof("-webkit-fill-available")];
    char stringpool_str787[sizeof("continuous-capacity-level-indicator")];
    char stringpool_str788[sizeof("optimizequality")];
    char stringpool_str789[sizeof("-internal-inactive-list-box-selection")];
    char stringpool_str790[sizeof("-webkit-xxx-large")];
    char stringpool_str791[sizeof("discrete-capacity-level-indicator")];
    char stringpool_str792[sizeof("-internal-media-overlay-cast-off-button")];
    char stringpool_str793[sizeof("-internal-active-list-box-selection-text")];
    char stringpool_str794[sizeof("-internal-inactive-list-box-selection-text")];
    char stringpool_str795[sizeof("inactivecaption")];
    char stringpool_str796[sizeof("inactivecaptiontext")];
  };
static const struct stringpool_t stringpool_contents =
  {
    "dot",
    "700",
    "200",
    "900",
    "800",
    "400",
    "600",
    "500",
    "caret",
    "cyan",
    "300",
    "dotted",
    "100",
    "content",
    "miter",
    "coral",
    "sienna",
    "span",
    "meter",
    "kannada",
    "middle",
    "coarse",
    "center",
    "rotate",
    "gold",
    "contents",
    "contrast",
    "condensed",
    "rotate3d",
    "to",
    "tb",
    "red",
    "cell",
    "tan",
    "central",
    "dense",
    "dashed",
    "font",
    "counter",
    "square",
    "fine",
    "saturate",
    "magenta",
    "reset",
    "menu",
    "round",
    "green",
    "counters",
    "single",
    "teal",
    "fill",
    "hand",
    "ridge",
    "filled",
    "seagreen",
    "none",
    "hide",
    "contain",
    "solid",
    "clear",
    "hidden",
    "seashell",
    "drag",
    "color",
    "rl",
    "right",
    "linen",
    "myanmar",
    "linear",
    "radial",
    "letter",
    "true",
    "goldenrod",
    "lr",
    "translate",
    "flat",
    "translate3d",
    "grid",
    "sesame",
    "large",
    "larger",
    "format",
    "column",
    "hiragana",
    "tb-rl",
    "digits",
    "ledger",
    "clone",
    "maroon",
    "lao",
    "legal",
    "menulist",
    "lighter",
    "lighten",
    "hangul",
    "radio",
    "b5",
    "b4",
    "zoom",
    "normal",
    "salmon",
    "lime",
    "border",
    "loose",
    "bold",
    "calc",
    "bolder",
    "telugu",
    "both",
    "circle",
    "space",
    "forestgreen",
    "disc",
    "saturation",
    "tomato",
    "open",
    "discard",
    "triangle",
    "rect",
    "mongolian",
    "small",
    "lightgreen",
    "medium",
    "smaller",
    "no-drag",
    "separate",
    "top",
    "limegreen",
    "hanging",
    "repeat",
    "collapse",
    "georgian",
    "from",
    "crosshair",
    "spell-out",
    "springgreen",
    "local",
    "listitem",
    "baseline",
    "orange",
    "mintcream",
    "sepia",
    "help",
    "orangered",
    "blur",
    "blue",
    "highlight",
    "crimson",
    "document",
    "beige",
    "hard-light",
    "slice",
    "bengali",
    "min-intrinsic",
    "a5",
    "clip",
    "keep-all",
    "dynamic",
    "a4",
    "a3",
    "bottom",
    "zoom-in",
    "transparent",
    "caption",
    "safe",
    "decimal",
    "screen",
    "balance",
    "list-item",
    "smooth",
    "no-repeat",
    "scale",
    "line-through",
    "ease",
    "azure",
    "scale3d",
    "aqua",
    "source-in",
    "minimal-ui",
    "closest-side",
    "logical",
    "table",
    "double",
    "zoom-out",
    "crispedges",
    "brightness",
    "tibetan",
    "cadetblue",
    "min-content",
    "after",
    "no-change",
    "replaced",
    "all",
    "srgb",
    "lightcoral",
    "alternate",
    "source-out",
    "default",
    "lightsalmon",
    "no-drop",
    "dodgerblue",
    "moccasin",
    "serif",
    "fit-content",
    "scroll",
    "oldlace",
    "left",
    "var",
    "skew",
    "monospace",
    "color-stop",
    "start",
    "hiragana-iroha",
    "slateblue",
    "landscape",
    "row",
    "distribute",
    "linearrgb",
    "self-end",
    "absolute",
    "rtl",
    "orchid",
    "space-around",
    "alias",
    "self-start",
    "rl-tb",
    "element",
    "forwards",
    "armenian",
    "ease-in",
    "visual",
    "aquamarine",
    "ltr",
    "exact",
    "lightblue",
    "lr-tb",
    "after-edge",
    "new",
    "color-burn",
    "violet",
    "sans-serif",
    "darken",
    "standalone",
    "darkred",
    "expanded",
    "cambodian",
    "manipulation",
    "markers",
    "ease-out",
    "katakana",
    "soft-light",
    "gainsboro",
    "lawngreen",
    "mediumblue",
    "off",
    "darkgreen",
    "alpha",
    "before",
    "blanchedalmond",
    "farthest-corner",
    "chartreuse",
    "rotatez",
    "rotatex",
    "media-slider",
    "mediumspringgreen",
    "khmer",
    "searchfield",
    "stretch",
    "cornsilk",
    "color-dodge",
    "mediumseagreen",
    "thin",
    "lightseagreen",
    "destination-out",
    "thai",
    "steps",
    "fixed",
    "table-cell",
    "read-write",
    "text",
    "linear-gradient",
    "hangul-consonant",
    "radial-gradient",
    "contextual",
    "hue",
    "menutext",
    "wait",
    "textarea",
    "small-caps",
    "matrix",
    "darkorange",
    "matrix3d",
    "thistle",
    "lower-greek",
    "mathematical",
    "strict",
    "s-resize",
    "static",
    "square-button",
    "minmax",
    "ellipse",
    "mediumpurple",
    "each-line",
    "nowrap",
    "translatez",
    "flex",
    "translatex",
    "below",
    "not-allowed",
    "vertical",
    "lowercase",
    "brown",
    "step-end",
    "running",
    "exclusion",
    "darksalmon",
    "browser",
    "nonzero",
    "step-start",
    "repeating-radial-gradient",
    "table-column",
    "n-resize",
    "ease-in-out",
    "butt",
    "rgb",
    "at",
    "scrollbar",
    "rgba",
    "attr",
    "all-scroll",
    "darkgoldenrod",
    "ellipsis",
    "lower-latin",
    "super",
    "no-historical-ligatures",
    "col-resize",
    "se-resize",
    "outset",
    "flex-end",
    "wrap",
    "horizontal",
    "reset-size",
    "flex-start",
    "vertical-lr",
    "steelblue",
    "hue-rotate",
    "button",
    "icon",
    "blink",
    "fullscreen",
    "embed",
    "destination-in",
    "lower-roman",
    "hebrew",
    "hotpink",
    "break-all",
    "ne-resize",
    "ideographic",
    "outside",
    "multiple",
    "geometricprecision",
    "difference",
    "visible",
    "chocolate",
    "darkorchid",
    "repeating-linear-gradient",
    "korean-hangul-formal",
    "closest-corner",
    "no-punctuation",
    "vertical-rl",
    "literal-punctuation",
    "katakana-iroha",
    "context-menu",
    "start-touch",
    "mediumorchid",
    "repeat-x",
    "fuchsia",
    "sub",
    "darkblue",
    "threedhighlight",
    "status-bar",
    "capitalize",
    "saddlebrown",
    "semi-condensed",
    "lightpink",
    "xor",
    "black",
    "scalez",
    "scalex",
    "darkseagreen",
    "no-common-ligatures",
    "captiontext",
    "farthest-side",
    "scale-down",
    "max-content",
    "rotatey",
    "table-row",
    "auto",
    "alternate-reverse",
    "deeppink",
    "luminance",
    "text-top",
    "aliceblue",
    "darkmagenta",
    "destination-atop",
    "grey",
    "accumulate",
    "gray",
    "x-large",
    "block",
    "fantasy",
    "e-resize",
    "mediumslateblue",
    "background",
    "currentcolor",
    "mandatory",
    "skewx",
    "semi-expanded",
    "extra-condensed",
    "double-circle",
    "trad-chinese-formal",
    "show",
    "stroke",
    "malayalam",
    "translatey",
    "move",
    "x-small",
    "cover",
    "historical-ligatures",
    "mistyrose",
    "textfield",
    "slategrey",
    "slategray",
    "before-edge",
    "floralwhite",
    "dimgrey",
    "dimgray",
    "silver",
    "horizontal-tb",
    "drop-shadow",
    "threedface",
    "listbox",
    "xx-large",
    "url",
    "italic",
    "gujarati",
    "trad-chinese-informal",
    "searchfield-decoration",
    "media-sliderthumb",
    "window",
    "reverse",
    "media-enter-fullscreen-button",
    "over",
    "copy",
    "hover",
    "wheat",
    "hsl",
    "urdu",
    "hsla",
    "yellow",
    "lightgrey",
    "firebrick",
    "lightgray",
    "lower-alpha",
    "khaki",
    "lavender",
    "alphabetic",
    "non-scaling-stroke",
    "read-only",
    "xx-small",
    "lower-armenian",
    "buttonhighlight",
    "relative",
    "ghostwhite",
    "margin-box",
    "table-column-group",
    "white",
    "grayscale",
    "overline",
    "devanagari",
    "path",
    "upright",
    "text-bottom",
    "menulist-text",
    "step-middle",
    "small-caption",
    "pre",
    "row-resize",
    "oriya",
    "darkslategrey",
    "darkslategray",
    "simp-chinese-formal",
    "sliderthumb-vertical",
    "peru",
    "text-after-edge",
    "border-box",
    "paused",
    "palegreen",
    "threedshadow",
    "highlighttext",
    "glyphs",
    "legacy",
    "bevel",
    "repeat-y",
    "pointer",
    "painted",
    "portrait",
    "yellowgreen",
    "groove",
    "w-resize",
    "lightcyan",
    "table-header-group",
    "horizontal-bt",
    "korean-hanja-formal",
    "persian",
    "scaley",
    "padding",
    "simp-chinese-informal",
    "last-baseline",
    "searchfield-results-decoration",
    "skyblue",
    "vertical-right",
    "backwards",
    "thick",
    "royalblue",
    "buttonface",
    "gurmukhi",
    "break-word",
    "sw-resize",
    "darkslateblue",
    "korean-hanja-informal",
    "on",
    "evenodd",
    "visiblefill",
    "threedlightshadow",
    "lemonchiffon",
    "plum",
    "opacity",
    "progress",
    "olive",
    "table-caption",
    "skewy",
    "pre-line",
    "avoid",
    "pan-right",
    "bisque",
    "slider-vertical",
    "sideways",
    "nw-resize",
    "palegoldenrod",
    "lightsteelblue",
    "buttontext",
    "sliderthumb-horizontal",
    "searchfield-cancel-button",
    "content-box",
    "uppercase",
    "source-over",
    "above",
    "honeydew",
    "space-between",
    "lightslategray",
    "oblique",
    "table-row-group",
    "blueviolet",
    "pan-up",
    "lightslategrey",
    "bounding-box",
    "buttonshadow",
    "upper-latin",
    "end",
    "discretionary-ligatures",
    "darkgrey",
    "message-box",
    "darkgray",
    "mediumaquamarine",
    "visiblepainted",
    "midnightblue",
    "slider-horizontal",
    "optimizespeed",
    "on-demand",
    "source-atop",
    "isolate",
    "media-volume-slider",
    "upper-roman",
    "open-quote",
    "graytext",
    "inset",
    "pan-left",
    "column-reverse",
    "menulist-textfield",
    "extra-expanded",
    "darkcyan",
    "nesw-resize",
    "arabic-indic",
    "destination-over",
    "scroll-event",
    "antialiased",
    "inline",
    "indianred",
    "greenyellow",
    "ew-resize",
    "inside",
    "cjk-ideographic",
    "inherit",
    "snow",
    "darkkhaki",
    "olivedrab",
    "windowtext",
    "scroll-position",
    "pink",
    "table-footer-group",
    "cursive",
    "menulist-button",
    "initial",
    "darkviolet",
    "-webkit-left",
    "vertical-text",
    "block-axis",
    "appworkspace",
    "cjk-earthly-branch",
    "lightyellow",
    "nwse-resize",
    "mediumvioletred",
    "sandybrown",
    "indigo",
    "interlace",
    "after-white-space",
    "pan-down",
    "always",
    "multiply",
    "pixelated",
    "rosybrown",
    "ultra-condensed",
    "economy",
    "-webkit-right",
    "common-ligatures",
    "-webkit-auto",
    "pan-x",
    "media-mute-button",
    "upper-alpha",
    "plus-lighter",
    "windowframe",
    "inline-grid",
    "media-controls-background",
    "close-quote",
    "ns-resize",
    "upper-armenian",
    "caps-lock-indicator",
    "whitesmoke",
    "luminosity",
    "intrinsic",
    "media-toggle-closed-captions-button",
    "pre-wrap",
    "visiblestroke",
    "-internal-media-cast-off-button",
    "purple",
    "wrap-reverse",
    "row-reverse",
    "media-volume-slider-container",
    "threeddarkshadow",
    "decimal-leading-zero",
    "powderblue",
    "turquoise",
    "lightgoldenrodyellow",
    "media-volume-sliderthumb",
    "sideways-right",
    "navy",
    "overlay",
    "infinite",
    "-webkit-min-content",
    "progress-bar",
    "cornflowerblue",
    "-webkit-calc",
    "-webkit-control",
    "rating-level-indicator",
    "no-contextual",
    "darkolivegreen",
    "inline-table",
    "no-discretionary-ligatures",
    "pan-y",
    "inter-word",
    "checkbox",
    "bidi-override",
    "ethiopic-halehame",
    "under",
    "no-open-quote",
    "polygon",
    "unset",
    "-internal-extend-to-zoom",
    "peachpuff",
    "text-before-edge",
    "-webkit-center",
    "-internal-variable-value",
    "burlywood",
    "-webkit-isolate",
    "media-fullscreen-volume-slider",
    "media-current-time-display",
    "-webkit-radial-gradient",
    "-webkit-fit-content",
    "underline",
    "wheel-event",
    "-webkit-match-parent",
    "optimizelegibility",
    "media-time-remaining-display",
    "space-evenly",
    "ivory",
    "-webkit-pictograph",
    "-webkit-grab",
    "-webkit-link",
    "-webkit-linear-gradient",
    "preserve-3d",
    "deepskyblue",
    "push-button",
    "ethiopic-halehame-am",
    "ultra-expanded",
    "-webkit-mini-control",
    "palevioletred",
    "inline-axis",
    "-webkit-baseline-middle",
    "rebeccapurple",
    "subpixel-antialiased",
    "ethiopic-halehame-ti-er",
    "ethiopic-halehame-ti-et",
    "justify",
    "-webkit-gradient",
    "navajowhite",
    "-webkit-cross-fade",
    "activeborder",
    "cubic-bezier",
    "media-controls-fullscreen-background",
    "wavy",
    "-webkit-paged-x",
    "infotext",
    "lavenderblush",
    "-webkit-small-control",
    "use-script",
    "button-bevel",
    "-webkit-text",
    "darkturquoise",
    "-webkit-image-set",
    "-webkit-grabbing",
    "invert",
    "activecaption",
    "inline-block",
    "padding-box",
    "inline-box",
    "-webkit-focus-ring-color",
    "read-write-plaintext-only",
    "-webkit-zoom-in",
    "progressive",
    "-webkit-zoom-out",
    "media-fullscreen-volume-slider-thumb",
    "-webkit-nowrap",
    "no-close-quote",
    "media-play-button",
    "-webkit-max-content",
    "-webkit-paged-y",
    "-webkit-plaintext",
    "-webkit-flex",
    "mediumturquoise",
    "papayawhip",
    "-webkit-inline-flex",
    "cjk-heavenly-stem",
    "perspective",
    "-webkit-activelink",
    "-webkit-box",
    "proximity",
    "lightskyblue",
    "-webkit-body",
    "inline-flex",
    "-webkit-inline-box",
    "-webkit-optimize-contrast",
    "media-exit-fullscreen-button",
    "paleturquoise",
    "relevancy-level-indicator",
    "progress-bar-value",
    "-webkit-canvas",
    "antiquewhite",
    "infobackground",
    "inner-spin-button",
    "-internal-active-list-box-selection",
    "-webkit-isolate-override",
    "inactiveborder",
    "-webkit-repeating-linear-gradient",
    "-webkit-repeating-radial-gradient",
    "media-overlay-play-button",
    "-webkit-fill-available",
    "continuous-capacity-level-indicator",
    "optimizequality",
    "-internal-inactive-list-box-selection",
    "-webkit-xxx-large",
    "discrete-capacity-level-indicator",
    "-internal-media-overlay-cast-off-button",
    "-internal-active-list-box-selection-text",
    "-internal-inactive-list-box-selection-text",
    "inactivecaption",
    "inactivecaptiontext"
  };
#define stringpool ((const char *) &stringpool_contents)
const struct Value *
CSSValueKeywordsHash::findValueImpl (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 797,
      MIN_WORD_LENGTH = 2,
      MAX_WORD_LENGTH = 42,
      MIN_HASH_VALUE = 9,
      MAX_HASH_VALUE = 7014
    };

  static const struct Value value_word_list[] =
    {
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str0, CSSValueDot},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1, CSSValue700},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str2, CSSValue200},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str3, CSSValue900},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str4, CSSValue800},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str5, CSSValue400},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str6, CSSValue600},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str7, CSSValue500},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str8, CSSValueCaret},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str9, CSSValueCyan},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str10, CSSValue300},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str11, CSSValueDotted},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str12, CSSValue100},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str13, CSSValueContent},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str14, CSSValueMiter},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str15, CSSValueCoral},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str16, CSSValueSienna},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str17, CSSValueSpan},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str18, CSSValueMeter},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str19, CSSValueKannada},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str20, CSSValueMiddle},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str21, CSSValueCoarse},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str22, CSSValueCenter},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str23, CSSValueRotate},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str24, CSSValueGold},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str25, CSSValueContents},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str26, CSSValueContrast},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str27, CSSValueCondensed},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str28, CSSValueRotate3d},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str29, CSSValueTo},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str30, CSSValueTb},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str31, CSSValueRed},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str32, CSSValueCell},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str33, CSSValueTan},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str34, CSSValueCentral},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str35, CSSValueDense},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str36, CSSValueDashed},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str37, CSSValueFont},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str38, CSSValueCounter},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str39, CSSValueSquare},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str40, CSSValueFine},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str41, CSSValueSaturate},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str42, CSSValueMagenta},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str43, CSSValueReset},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str44, CSSValueMenu},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str45, CSSValueRound},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str46, CSSValueGreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str47, CSSValueCounters},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str48, CSSValueSingle},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str49, CSSValueTeal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str50, CSSValueFill},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str51, CSSValueHand},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str52, CSSValueRidge},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str53, CSSValueFilled},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str54, CSSValueSeagreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str55, CSSValueNone},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str56, CSSValueHide},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str57, CSSValueContain},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str58, CSSValueSolid},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str59, CSSValueClear},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str60, CSSValueHidden},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str61, CSSValueSeashell},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str62, CSSValueDrag},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str63, CSSValueColor},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str64, CSSValueRl},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str65, CSSValueRight},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str66, CSSValueLinen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str67, CSSValueMyanmar},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str68, CSSValueLinear},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str69, CSSValueRadial},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str70, CSSValueLetter},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str71, CSSValueTrue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str72, CSSValueGoldenrod},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str73, CSSValueLr},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str74, CSSValueTranslate},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str75, CSSValueFlat},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str76, CSSValueTranslate3d},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str77, CSSValueGrid},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str78, CSSValueSesame},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str79, CSSValueLarge},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str80, CSSValueLarger},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str81, CSSValueFormat},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str82, CSSValueColumn},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str83, CSSValueHiragana},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str84, CSSValueTbRl},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str85, CSSValueDigits},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str86, CSSValueLedger},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str87, CSSValueClone},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str88, CSSValueMaroon},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str89, CSSValueLao},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str90, CSSValueLegal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str91, CSSValueMenulist},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str92, CSSValueLighter},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str93, CSSValueLighten},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str94, CSSValueHangul},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str95, CSSValueRadio},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str96, CSSValueB5},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str97, CSSValueB4},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str98, CSSValueZoom},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str99, CSSValueNormal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str100, CSSValueSalmon},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str101, CSSValueLime},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str102, CSSValueBorder},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str103, CSSValueLoose},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str104, CSSValueBold},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str105, CSSValueCalc},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str106, CSSValueBolder},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str107, CSSValueTelugu},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str108, CSSValueBoth},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str109, CSSValueCircle},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str110, CSSValueSpace},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str111, CSSValueForestgreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str112, CSSValueDisc},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str113, CSSValueSaturation},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str114, CSSValueTomato},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str115, CSSValueOpen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str116, CSSValueDiscard},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str117, CSSValueTriangle},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str118, CSSValueRect},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str119, CSSValueMongolian},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str120, CSSValueSmall},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str121, CSSValueLightgreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str122, CSSValueMedium},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str123, CSSValueSmaller},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str124, CSSValueNoDrag},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str125, CSSValueSeparate},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str126, CSSValueTop},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str127, CSSValueLimegreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str128, CSSValueHanging},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str129, CSSValueRepeat},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str130, CSSValueCollapse},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str131, CSSValueGeorgian},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str132, CSSValueFrom},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str133, CSSValueCrosshair},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str134, CSSValueSpellOut},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str135, CSSValueSpringgreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str136, CSSValueLocal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str137, CSSValueListitem},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str138, CSSValueBaseline},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str139, CSSValueOrange},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str140, CSSValueMintcream},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str141, CSSValueSepia},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str142, CSSValueHelp},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str143, CSSValueOrangered},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str144, CSSValueBlur},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str145, CSSValueBlue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str146, CSSValueHighlight},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str147, CSSValueCrimson},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str148, CSSValueDocument},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str149, CSSValueBeige},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str150, CSSValueHardLight},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str151, CSSValueSlice},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str152, CSSValueBengali},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str153, CSSValueMinIntrinsic},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str154, CSSValueA5},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str155, CSSValueClip},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str156, CSSValueKeepAll},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str157, CSSValueDynamic},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str158, CSSValueA4},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str159, CSSValueA3},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str160, CSSValueBottom},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str161, CSSValueZoomIn},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str162, CSSValueTransparent},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str163, CSSValueCaption},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str164, CSSValueSafe},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str165, CSSValueDecimal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str166, CSSValueScreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str167, CSSValueBalance},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str168, CSSValueListItem},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str169, CSSValueSmooth},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str170, CSSValueNoRepeat},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str171, CSSValueScale},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str172, CSSValueLineThrough},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str173, CSSValueEase},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str174, CSSValueAzure},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str175, CSSValueScale3d},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str176, CSSValueAqua},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str177, CSSValueSourceIn},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str178, CSSValueMinimalUi},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str179, CSSValueClosestSide},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str180, CSSValueLogical},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str181, CSSValueTable},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str182, CSSValueDouble},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str183, CSSValueZoomOut},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str184, CSSValueCrispEdges},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str185, CSSValueBrightness},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str186, CSSValueTibetan},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str187, CSSValueCadetblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str188, CSSValueMinContent},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str189, CSSValueAfter},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str190, CSSValueNoChange},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str191, CSSValueReplaced},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str192, CSSValueAll},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str193, CSSValueSRGB},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str194, CSSValueLightcoral},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str195, CSSValueAlternate},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str196, CSSValueSourceOut},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str197, CSSValueDefault},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str198, CSSValueLightsalmon},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str199, CSSValueNoDrop},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str200, CSSValueDodgerblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str201, CSSValueMoccasin},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str202, CSSValueSerif},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str203, CSSValueFitContent},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str204, CSSValueScroll},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str205, CSSValueOldlace},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str206, CSSValueLeft},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str207, CSSValueVar},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str208, CSSValueSkew},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str209, CSSValueMonospace},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str210, CSSValueColorStop},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str211, CSSValueStart},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str212, CSSValueHiraganaIroha},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str213, CSSValueSlateblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str214, CSSValueLandscape},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str215, CSSValueRow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str216, CSSValueDistribute},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str217, CSSValueLinearRGB},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str218, CSSValueSelfEnd},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str219, CSSValueAbsolute},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str220, CSSValueRtl},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str221, CSSValueOrchid},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str222, CSSValueSpaceAround},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str223, CSSValueAlias},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str224, CSSValueSelfStart},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str225, CSSValueRlTb},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str226, CSSValueElement},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str227, CSSValueForwards},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str228, CSSValueArmenian},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str229, CSSValueEaseIn},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str230, CSSValueVisual},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str231, CSSValueAquamarine},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str232, CSSValueLtr},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str233, CSSValueExact},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str234, CSSValueLightblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str235, CSSValueLrTb},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str236, CSSValueAfterEdge},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str237, CSSValueNew},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str238, CSSValueColorBurn},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str239, CSSValueViolet},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str240, CSSValueSansSerif},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str241, CSSValueDarken},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str242, CSSValueStandalone},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str243, CSSValueDarkred},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str244, CSSValueExpanded},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str245, CSSValueCambodian},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str246, CSSValueManipulation},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str247, CSSValueMarkers},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str248, CSSValueEaseOut},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str249, CSSValueKatakana},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str250, CSSValueSoftLight},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str251, CSSValueGainsboro},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str252, CSSValueLawngreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str253, CSSValueMediumblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str254, CSSValueOff},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str255, CSSValueDarkgreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str256, CSSValueAlpha},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str257, CSSValueBefore},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str258, CSSValueBlanchedalmond},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str259, CSSValueFarthestCorner},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str260, CSSValueChartreuse},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str261, CSSValueRotateZ},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str262, CSSValueRotateX},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str263, CSSValueMediaSlider},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str264, CSSValueMediumspringgreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str265, CSSValueKhmer},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str266, CSSValueSearchfield},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str267, CSSValueStretch},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str268, CSSValueCornsilk},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str269, CSSValueColorDodge},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str270, CSSValueMediumseagreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str271, CSSValueThin},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str272, CSSValueLightseagreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str273, CSSValueDestinationOut},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str274, CSSValueThai},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str275, CSSValueSteps},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str276, CSSValueFixed},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str277, CSSValueTableCell},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str278, CSSValueReadWrite},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str279, CSSValueText},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str280, CSSValueLinearGradient},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str281, CSSValueHangulConsonant},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str282, CSSValueRadialGradient},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str283, CSSValueContextual},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str284, CSSValueHue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str285, CSSValueMenutext},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str286, CSSValueWait},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str287, CSSValueTextarea},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str288, CSSValueSmallCaps},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str289, CSSValueMatrix},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str290, CSSValueDarkorange},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str291, CSSValueMatrix3d},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str292, CSSValueThistle},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str293, CSSValueLowerGreek},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str294, CSSValueMathematical},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str295, CSSValueStrict},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str296, CSSValueSResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str297, CSSValueStatic},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str298, CSSValueSquareButton},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str299, CSSValueMinmax},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str300, CSSValueEllipse},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str301, CSSValueMediumpurple},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str302, CSSValueEachLine},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str303, CSSValueNowrap},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str304, CSSValueTranslateZ},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str305, CSSValueFlex},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str306, CSSValueTranslateX},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str307, CSSValueBelow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str308, CSSValueNotAllowed},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str309, CSSValueVertical},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str310, CSSValueLowercase},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str311, CSSValueBrown},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str312, CSSValueStepEnd},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str313, CSSValueRunning},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str314, CSSValueExclusion},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str315, CSSValueDarksalmon},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str316, CSSValueBrowser},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str317, CSSValueNonzero},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str318, CSSValueStepStart},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str319, CSSValueRepeatingRadialGradient},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str320, CSSValueTableColumn},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str321, CSSValueNResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str322, CSSValueEaseInOut},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str323, CSSValueButt},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str324, CSSValueRgb},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str325, CSSValueAt},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str326, CSSValueScrollbar},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str327, CSSValueRgba},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str328, CSSValueAttr},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str329, CSSValueAllScroll},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str330, CSSValueDarkgoldenrod},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str331, CSSValueEllipsis},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str332, CSSValueLowerLatin},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str333, CSSValueSuper},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str334, CSSValueNoHistoricalLigatures},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str335, CSSValueColResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str336, CSSValueSeResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str337, CSSValueOutset},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str338, CSSValueFlexEnd},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str339, CSSValueWrap},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str340, CSSValueHorizontal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str341, CSSValueResetSize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str342, CSSValueFlexStart},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str343, CSSValueVerticalLr},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str344, CSSValueSteelblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str345, CSSValueHueRotate},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str346, CSSValueButton},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str347, CSSValueIcon},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str348, CSSValueBlink},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str349, CSSValueFullscreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str350, CSSValueEmbed},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str351, CSSValueDestinationIn},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str352, CSSValueLowerRoman},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str353, CSSValueHebrew},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str354, CSSValueHotpink},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str355, CSSValueBreakAll},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str356, CSSValueNeResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str357, CSSValueIdeographic},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str358, CSSValueOutside},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str359, CSSValueMultiple},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str360, CSSValueGeometricPrecision},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str361, CSSValueDifference},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str362, CSSValueVisible},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str363, CSSValueChocolate},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str364, CSSValueDarkorchid},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str365, CSSValueRepeatingLinearGradient},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str366, CSSValueKoreanHangulFormal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str367, CSSValueClosestCorner},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str368, CSSValueNoPunctuation},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str369, CSSValueVerticalRl},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str370, CSSValueLiteralPunctuation},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str371, CSSValueKatakanaIroha},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str372, CSSValueContextMenu},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str373, CSSValueStartTouch},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str374, CSSValueMediumorchid},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str375, CSSValueRepeatX},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str376, CSSValueFuchsia},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str377, CSSValueSub},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str378, CSSValueDarkblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str379, CSSValueThreedhighlight},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str380, CSSValueStatusBar},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str381, CSSValueCapitalize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str382, CSSValueSaddlebrown},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str383, CSSValueSemiCondensed},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str384, CSSValueLightpink},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str385, CSSValueXor},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str386, CSSValueBlack},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str387, CSSValueScaleZ},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str388, CSSValueScaleX},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str389, CSSValueDarkseagreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str390, CSSValueNoCommonLigatures},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str391, CSSValueCaptiontext},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str392, CSSValueFarthestSide},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str393, CSSValueScaleDown},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str394, CSSValueMaxContent},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str395, CSSValueRotateY},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str396, CSSValueTableRow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str397, CSSValueAuto},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str398, CSSValueAlternateReverse},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str399, CSSValueDeeppink},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str400, CSSValueLuminance},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str401, CSSValueTextTop},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str402, CSSValueAliceblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str403, CSSValueDarkmagenta},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str404, CSSValueDestinationAtop},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str405, CSSValueGrey},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str406, CSSValueAccumulate},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str407, CSSValueGray},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str408, CSSValueXLarge},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str409, CSSValueBlock},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str410, CSSValueFantasy},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str411, CSSValueEResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str412, CSSValueMediumslateblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str413, CSSValueBackground},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str414, CSSValueCurrentcolor},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str415, CSSValueMandatory},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str416, CSSValueSkewX},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str417, CSSValueSemiExpanded},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str418, CSSValueExtraCondensed},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str419, CSSValueDoubleCircle},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str420, CSSValueTradChineseFormal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str421, CSSValueShow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str422, CSSValueStroke},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str423, CSSValueMalayalam},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str424, CSSValueTranslateY},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str425, CSSValueMove},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str426, CSSValueXSmall},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str427, CSSValueCover},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str428, CSSValueHistoricalLigatures},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str429, CSSValueMistyrose},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str430, CSSValueTextfield},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str431, CSSValueSlategrey},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str432, CSSValueSlategray},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str433, CSSValueBeforeEdge},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str434, CSSValueFloralwhite},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str435, CSSValueDimgrey},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str436, CSSValueDimgray},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str437, CSSValueSilver},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str438, CSSValueHorizontalTb},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str439, CSSValueDropShadow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str440, CSSValueThreedface},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str441, CSSValueListbox},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str442, CSSValueXxLarge},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str443, CSSValueUrl},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str444, CSSValueItalic},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str445, CSSValueGujarati},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str446, CSSValueTradChineseInformal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str447, CSSValueSearchfieldDecoration},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str448, CSSValueMediaSliderthumb},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str449, CSSValueWindow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str450, CSSValueReverse},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str451, CSSValueMediaEnterFullscreenButton},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str452, CSSValueOver},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str453, CSSValueCopy},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str454, CSSValueHover},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str455, CSSValueWheat},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str456, CSSValueHsl},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str457, CSSValueUrdu},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str458, CSSValueHsla},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str459, CSSValueYellow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str460, CSSValueLightgrey},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str461, CSSValueFirebrick},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str462, CSSValueLightgray},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str463, CSSValueLowerAlpha},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str464, CSSValueKhaki},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str465, CSSValueLavender},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str466, CSSValueAlphabetic},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str467, CSSValueNonScalingStroke},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str468, CSSValueReadOnly},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str469, CSSValueXxSmall},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str470, CSSValueLowerArmenian},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str471, CSSValueButtonhighlight},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str472, CSSValueRelative},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str473, CSSValueGhostwhite},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str474, CSSValueMarginBox},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str475, CSSValueTableColumnGroup},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str476, CSSValueWhite},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str477, CSSValueGrayscale},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str478, CSSValueOverline},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str479, CSSValueDevanagari},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str480, CSSValuePath},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str481, CSSValueUpright},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str482, CSSValueTextBottom},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str483, CSSValueMenulistText},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str484, CSSValueStepMiddle},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str485, CSSValueSmallCaption},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str486, CSSValuePre},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str487, CSSValueRowResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str488, CSSValueOriya},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str489, CSSValueDarkslategrey},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str490, CSSValueDarkslategray},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str491, CSSValueSimpChineseFormal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str492, CSSValueSliderthumbVertical},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str493, CSSValuePeru},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str494, CSSValueTextAfterEdge},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str495, CSSValueBorderBox},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str496, CSSValuePaused},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str497, CSSValuePalegreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str498, CSSValueThreedshadow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str499, CSSValueHighlighttext},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str500, CSSValueGlyphs},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str501, CSSValueLegacy},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str502, CSSValueBevel},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str503, CSSValueRepeatY},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str504, CSSValuePointer},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str505, CSSValuePainted},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str506, CSSValuePortrait},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str507, CSSValueYellowgreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str508, CSSValueGroove},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str509, CSSValueWResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str510, CSSValueLightcyan},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str511, CSSValueTableHeaderGroup},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str512, CSSValueHorizontalBt},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str513, CSSValueKoreanHanjaFormal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str514, CSSValuePersian},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str515, CSSValueScaleY},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str516, CSSValuePadding},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str517, CSSValueSimpChineseInformal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str518, CSSValueLastBaseline},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str519, CSSValueSearchfieldResultsDecoration},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str520, CSSValueSkyblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str521, CSSValueVerticalRight},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str522, CSSValueBackwards},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str523, CSSValueThick},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str524, CSSValueRoyalblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str525, CSSValueButtonface},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str526, CSSValueGurmukhi},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str527, CSSValueBreakWord},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str528, CSSValueSwResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str529, CSSValueDarkslateblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str530, CSSValueKoreanHanjaInformal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str531, CSSValueOn},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str532, CSSValueEvenodd},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str533, CSSValueVisibleFill},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str534, CSSValueThreedlightshadow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str535, CSSValueLemonchiffon},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str536, CSSValuePlum},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str537, CSSValueOpacity},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str538, CSSValueProgress},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str539, CSSValueOlive},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str540, CSSValueTableCaption},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str541, CSSValueSkewY},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str542, CSSValuePreLine},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str543, CSSValueAvoid},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str544, CSSValuePanRight},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str545, CSSValueBisque},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str546, CSSValueSliderVertical},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str547, CSSValueSideways},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str548, CSSValueNwResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str549, CSSValuePalegoldenrod},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str550, CSSValueLightsteelblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str551, CSSValueButtontext},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str552, CSSValueSliderthumbHorizontal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str553, CSSValueSearchfieldCancelButton},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str554, CSSValueContentBox},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str555, CSSValueUppercase},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str556, CSSValueSourceOver},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str557, CSSValueAbove},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str558, CSSValueHoneydew},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str559, CSSValueSpaceBetween},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str560, CSSValueLightslategray},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str561, CSSValueOblique},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str562, CSSValueTableRowGroup},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str563, CSSValueBlueviolet},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str564, CSSValuePanUp},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str565, CSSValueLightslategrey},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str566, CSSValueBoundingBox},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str567, CSSValueButtonshadow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str568, CSSValueUpperLatin},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str569, CSSValueEnd},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str570, CSSValueDiscretionaryLigatures},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str571, CSSValueDarkgrey},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str572, CSSValueMessageBox},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str573, CSSValueDarkgray},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str574, CSSValueMediumaquamarine},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str575, CSSValueVisiblePainted},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str576, CSSValueMidnightblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str577, CSSValueSliderHorizontal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str578, CSSValueOptimizeSpeed},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str579, CSSValueOnDemand},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str580, CSSValueSourceAtop},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str581, CSSValueIsolate},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str582, CSSValueMediaVolumeSlider},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str583, CSSValueUpperRoman},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str584, CSSValueOpenQuote},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str585, CSSValueGraytext},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str586, CSSValueInset},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str587, CSSValuePanLeft},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str588, CSSValueColumnReverse},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str589, CSSValueMenulistTextfield},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str590, CSSValueExtraExpanded},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str591, CSSValueDarkcyan},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str592, CSSValueNeswResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str593, CSSValueArabicIndic},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str594, CSSValueDestinationOver},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str595, CSSValueScrollEvent},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str596, CSSValueAntialiased},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str597, CSSValueInline},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str598, CSSValueIndianred},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str599, CSSValueGreenyellow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str600, CSSValueEwResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str601, CSSValueInside},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str602, CSSValueCjkIdeographic},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str603, CSSValueInherit},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str604, CSSValueSnow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str605, CSSValueDarkkhaki},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str606, CSSValueOlivedrab},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str607, CSSValueWindowtext},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str608, CSSValueScrollPosition},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str609, CSSValuePink},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str610, CSSValueTableFooterGroup},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str611, CSSValueCursive},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str612, CSSValueMenulistButton},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str613, CSSValueInitial},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str614, CSSValueDarkviolet},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str615, CSSValueWebkitLeft},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str616, CSSValueVerticalText},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str617, CSSValueBlockAxis},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str618, CSSValueAppworkspace},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str619, CSSValueCjkEarthlyBranch},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str620, CSSValueLightyellow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str621, CSSValueNwseResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str622, CSSValueMediumvioletred},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str623, CSSValueSandybrown},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str624, CSSValueIndigo},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str625, CSSValueInterlace},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str626, CSSValueAfterWhiteSpace},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str627, CSSValuePanDown},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str628, CSSValueAlways},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str629, CSSValueMultiply},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str630, CSSValuePixelated},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str631, CSSValueRosybrown},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str632, CSSValueUltraCondensed},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str633, CSSValueEconomy},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str634, CSSValueWebkitRight},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str635, CSSValueCommonLigatures},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str636, CSSValueWebkitAuto},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str637, CSSValuePanX},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str638, CSSValueMediaMuteButton},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str639, CSSValueUpperAlpha},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str640, CSSValuePlusLighter},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str641, CSSValueWindowframe},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str642, CSSValueInlineGrid},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str643, CSSValueMediaControlsBackground},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str644, CSSValueCloseQuote},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str645, CSSValueNsResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str646, CSSValueUpperArmenian},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str647, CSSValueCapsLockIndicator},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str648, CSSValueWhitesmoke},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str649, CSSValueLuminosity},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str650, CSSValueIntrinsic},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str651, CSSValueMediaToggleClosedCaptionsButton},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str652, CSSValuePreWrap},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str653, CSSValueVisibleStroke},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str654, CSSValueInternalMediaCastOffButton},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str655, CSSValuePurple},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str656, CSSValueWrapReverse},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str657, CSSValueRowReverse},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str658, CSSValueMediaVolumeSliderContainer},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str659, CSSValueThreeddarkshadow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str660, CSSValueDecimalLeadingZero},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str661, CSSValuePowderblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str662, CSSValueTurquoise},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str663, CSSValueLightgoldenrodyellow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str664, CSSValueMediaVolumeSliderthumb},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str665, CSSValueSidewaysRight},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str666, CSSValueNavy},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str667, CSSValueOverlay},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str668, CSSValueInfinite},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str669, CSSValueWebkitMinContent},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str670, CSSValueProgressBar},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str671, CSSValueCornflowerblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str672, CSSValueWebkitCalc},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str673, CSSValueWebkitControl},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str674, CSSValueRatingLevelIndicator},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str675, CSSValueNoContextual},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str676, CSSValueDarkolivegreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str677, CSSValueInlineTable},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str678, CSSValueNoDiscretionaryLigatures},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str679, CSSValuePanY},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str680, CSSValueInterWord},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str681, CSSValueCheckbox},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str682, CSSValueBidiOverride},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str683, CSSValueEthiopicHalehame},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str684, CSSValueUnder},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str685, CSSValueNoOpenQuote},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str686, CSSValuePolygon},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str687, CSSValueUnset},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str688, CSSValueInternalExtendToZoom},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str689, CSSValuePeachpuff},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str690, CSSValueTextBeforeEdge},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str691, CSSValueWebkitCenter},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str692, CSSValueInternalVariableValue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str693, CSSValueBurlywood},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str694, CSSValueWebkitIsolate},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str695, CSSValueMediaFullscreenVolumeSlider},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str696, CSSValueMediaCurrentTimeDisplay},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str697, CSSValueWebkitRadialGradient},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str698, CSSValueWebkitFitContent},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str699, CSSValueUnderline},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str700, CSSValueWheelEvent},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str701, CSSValueWebkitMatchParent},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str702, CSSValueOptimizeLegibility},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str703, CSSValueMediaTimeRemainingDisplay},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str704, CSSValueSpaceEvenly},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str705, CSSValueIvory},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str706, CSSValueWebkitPictograph},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str707, CSSValueWebkitGrab},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str708, CSSValueWebkitLink},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str709, CSSValueWebkitLinearGradient},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str710, CSSValuePreserve3d},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str711, CSSValueDeepskyblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str712, CSSValuePushButton},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str713, CSSValueEthiopicHalehameAm},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str714, CSSValueUltraExpanded},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str715, CSSValueWebkitMiniControl},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str716, CSSValuePalevioletred},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str717, CSSValueInlineAxis},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str718, CSSValueWebkitBaselineMiddle},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str719, CSSValueRebeccapurple},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str720, CSSValueSubpixelAntialiased},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str721, CSSValueEthiopicHalehameTiEr},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str722, CSSValueEthiopicHalehameTiEt},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str723, CSSValueJustify},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str724, CSSValueWebkitGradient},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str725, CSSValueNavajowhite},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str726, CSSValueWebkitCrossFade},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str727, CSSValueActiveborder},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str728, CSSValueCubicBezier},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str729, CSSValueMediaControlsFullscreenBackground},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str730, CSSValueWavy},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str731, CSSValueWebkitPagedX},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str732, CSSValueInfotext},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str733, CSSValueLavenderblush},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str734, CSSValueWebkitSmallControl},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str735, CSSValueUseScript},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str736, CSSValueButtonBevel},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str737, CSSValueWebkitText},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str738, CSSValueDarkturquoise},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str739, CSSValueWebkitImageSet},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str740, CSSValueWebkitGrabbing},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str741, CSSValueInvert},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str742, CSSValueActivecaption},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str743, CSSValueInlineBlock},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str744, CSSValuePaddingBox},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str745, CSSValueInlineBox},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str746, CSSValueWebkitFocusRingColor},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str747, CSSValueReadWritePlaintextOnly},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str748, CSSValueWebkitZoomIn},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str749, CSSValueProgressive},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str750, CSSValueWebkitZoomOut},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str751, CSSValueMediaFullscreenVolumeSliderThumb},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str752, CSSValueWebkitNowrap},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str753, CSSValueNoCloseQuote},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str754, CSSValueMediaPlayButton},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str755, CSSValueWebkitMaxContent},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str756, CSSValueWebkitPagedY},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str757, CSSValueWebkitPlaintext},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str758, CSSValueWebkitFlex},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str759, CSSValueMediumturquoise},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str760, CSSValuePapayawhip},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str761, CSSValueWebkitInlineFlex},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str762, CSSValueCjkHeavenlyStem},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str763, CSSValuePerspective},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str764, CSSValueWebkitActivelink},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str765, CSSValueWebkitBox},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str766, CSSValueProximity},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str767, CSSValueLightskyblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str768, CSSValueWebkitBody},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str769, CSSValueInlineFlex},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str770, CSSValueWebkitInlineBox},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str771, CSSValueWebkitOptimizeContrast},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str772, CSSValueMediaExitFullscreenButton},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str773, CSSValuePaleturquoise},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str774, CSSValueRelevancyLevelIndicator},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str775, CSSValueProgressBarValue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str776, CSSValueWebkitCanvas},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str777, CSSValueAntiquewhite},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str778, CSSValueInfobackground},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str779, CSSValueInnerSpinButton},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str780, CSSValueInternalActiveListBoxSelection},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str781, CSSValueWebkitIsolateOverride},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str782, CSSValueInactiveborder},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str783, CSSValueWebkitRepeatingLinearGradient},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str784, CSSValueWebkitRepeatingRadialGradient},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str785, CSSValueMediaOverlayPlayButton},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str786, CSSValueWebkitFillAvailable},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str787, CSSValueContinuousCapacityLevelIndicator},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str788, CSSValueOptimizeQuality},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str789, CSSValueInternalInactiveListBoxSelection},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str790, CSSValueWebkitXxxLarge},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str791, CSSValueDiscreteCapacityLevelIndicator},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str792, CSSValueInternalMediaOverlayCastOffButton},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str793, CSSValueInternalActiveListBoxSelectionText},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str794, CSSValueInternalInactiveListBoxSelectionText},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str795, CSSValueInactivecaption},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str796, CSSValueInactivecaptiontext}
    };

  static const short lookup[] =
    {
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,   0,
        1,   2,   3,   4,   5,   6,   7,   8,   9,  10,
       11,  12,  -1,  13,  -1,  14,  -1,  15,  -1,  -1,
       -1,  -1,  16,  -1,  -1,  -1,  -1,  17,  18,  19,
       20,  21,  22,  23,  -1,  24,  -1,  25,  -1,  26,
       27,  28,  29,  30,  31,  -1,  32,  33,  34,  35,
       -1,  -1,  -1,  -1,  36,  -1,  37,  -1,  38,  39,
       -1,  -1,  -1,  -1,  -1,  -1,  40,  41,  -1,  -1,
       42,  43,  44,  45,  -1,  46,  -1,  -1,  -1,  -1,
       -1,  -1,  47,  48,  -1,  49,  50,  -1,  51,  -1,
       -1,  52,  -1,  53,  54,  55,  -1,  56,  -1,  57,
       58,  -1,  59,  -1,  60,  61,  62,  -1,  63,  -1,
       -1,  64,  -1,  -1,  65,  -1,  -1,  -1,  66,  -1,
       -1,  67,  -1,  68,  -1,  -1,  -1,  -1,  69,  -1,
       -1,  -1,  -1,  -1,  -1,  70,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  71,  72,  -1,  73,  -1,  -1,  74,
       -1,  -1,  -1,  -1,  75,  -1,  76,  77,  -1,  78,
       -1,  -1,  79,  -1,  80,  -1,  -1,  81,  82,  -1,
       83,  -1,  -1,  84,  -1,  -1,  -1,  -1,  85,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  86,  -1,
       -1,  -1,  -1,  -1,  87,  -1,  88,  89,  90,  -1,
       91,  92,  93,  -1,  94,  -1,  95,  -1,  -1,  96,
       -1,  -1,  -1,  -1,  97,  98,  99, 100, 101, 102,
       -1,  -1, 103,  -1,  -1, 104,  -1, 105,  -1,  -1,
      106,  -1,  -1,  -1,  -1,  -1, 107,  -1, 108,  -1,
       -1, 109,  -1, 110, 111,  -1, 112,  -1,  -1,  -1,
       -1, 113,  -1,  -1,  -1, 114, 115,  -1, 116, 117,
      118, 119, 120,  -1, 121,  -1, 122, 123,  -1,  -1,
       -1,  -1,  -1, 124,  -1,  -1,  -1, 125, 126,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 127,  -1, 128, 129,
       -1, 130,  -1,  -1, 131,  -1,  -1,  -1,  -1,  -1,
      132,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      133,  -1,  -1, 134,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 135,
       -1,  -1,  -1, 136,  -1,  -1,  -1, 137,  -1,  -1,
       -1,  -1,  -1, 138,  -1, 139, 140,  -1,  -1, 141,
       -1,  -1, 142,  -1, 143, 144, 145,  -1,  -1,  -1,
      146, 147,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 148,  -1, 149,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 150,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 151, 152,  -1,
       -1,  -1,  -1,  -1,  -1, 153,  -1,  -1,  -1,  -1,
       -1,  -1, 154,  -1, 155, 156, 157, 158, 159, 160,
      161,  -1,  -1, 162,  -1, 163,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 164,  -1, 165,  -1, 166,
       -1, 167, 168,  -1, 169,  -1,  -1,  -1, 170,  -1,
      171, 172,  -1,  -1,  -1, 173, 174,  -1, 175, 176,
       -1,  -1, 177, 178,  -1, 179,  -1, 180,  -1, 181,
      182, 183, 184,  -1, 185,  -1,  -1,  -1, 186,  -1,
       -1,  -1,  -1,  -1, 187,  -1,  -1,  -1,  -1,  -1,
      188,  -1,  -1,  -1,  -1, 189,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 190,  -1,  -1, 191, 192,
      193,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      194, 195,  -1, 196,  -1,  -1, 197,  -1,  -1, 198,
       -1,  -1,  -1, 199,  -1,  -1, 200, 201,  -1,  -1,
       -1,  -1,  -1, 202,  -1, 203,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 204,  -1, 205,  -1,  -1,  -1,  -1,
      206,  -1,  -1,  -1,  -1, 207,  -1,  -1,  -1, 208,
      209,  -1,  -1,  -1, 210, 211, 212,  -1,  -1, 213,
       -1,  -1, 214,  -1,  -1, 215,  -1,  -1,  -1, 216,
       -1, 217, 218, 219,  -1,  -1, 220,  -1,  -1, 221,
       -1,  -1,  -1,  -1, 222,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 223,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 224,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 225,
      226,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 227, 228,  -1,  -1,
      229,  -1,  -1,  -1,  -1, 230, 231, 232,  -1,  -1,
      233, 234,  -1,  -1, 235,  -1,  -1, 236,  -1,  -1,
       -1,  -1, 237,  -1,  -1,  -1,  -1, 238,  -1, 239,
      240,  -1,  -1,  -1,  -1,  -1, 241,  -1, 242, 243,
      244,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      245,  -1, 246,  -1,  -1,  -1,  -1,  -1,  -1, 247,
       -1, 248,  -1, 249,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 250,  -1,  -1, 251,
      252, 253,  -1,  -1,  -1,  -1,  -1, 254,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 255,  -1,
       -1,  -1,  -1,  -1, 256,  -1,  -1,  -1, 257,  -1,
       -1, 258,  -1,  -1,  -1, 259,  -1,  -1,  -1, 260,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 261,  -1,  -1, 262, 263, 264,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 265,  -1,  -1,  -1,  -1,
      266,  -1,  -1,  -1, 267,  -1, 268,  -1,  -1, 269,
      270, 271, 272, 273, 274,  -1, 275,  -1, 276, 277,
      278, 279,  -1,  -1, 280,  -1, 281,  -1,  -1, 282,
      283, 284,  -1,  -1, 285,  -1, 286,  -1, 287, 288,
       -1, 289,  -1,  -1,  -1,  -1,  -1,  -1, 290, 291,
       -1,  -1, 292,  -1,  -1, 293,  -1,  -1,  -1,  -1,
       -1, 294, 295,  -1, 296,  -1, 297,  -1,  -1,  -1,
       -1, 298, 299,  -1, 300,  -1,  -1,  -1,  -1, 301,
       -1, 302,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 303,  -1,  -1,  -1,  -1, 304, 305,  -1,
      306, 307, 308, 309,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 310,  -1,  -1,  -1, 311,  -1, 312, 313,  -1,
       -1,  -1,  -1,  -1, 314,  -1,  -1,  -1,  -1, 315,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 316, 317,  -1,  -1,  -1,  -1, 318,  -1,  -1,
       -1, 319,  -1,  -1,  -1,  -1, 320, 321,  -1,  -1,
       -1,  -1, 322,  -1,  -1, 323, 324,  -1, 325,  -1,
      326,  -1, 327,  -1, 328,  -1,  -1,  -1,  -1, 329,
       -1, 330, 331,  -1, 332,  -1, 333,  -1, 334,  -1,
       -1, 335,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 336,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 337,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 338,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 339,  -1,  -1,  -1,  -1,  -1, 340,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 341,  -1,
       -1,  -1,  -1, 342,  -1, 343,  -1, 344,  -1,  -1,
       -1,  -1, 345,  -1,  -1, 346,  -1, 347,  -1,  -1,
       -1,  -1,  -1,  -1, 348, 349, 350,  -1,  -1, 351,
       -1,  -1,  -1,  -1,  -1, 352, 353,  -1,  -1,  -1,
      354,  -1,  -1, 355,  -1, 356,  -1,  -1, 357, 358,
      359,  -1, 360,  -1, 361,  -1,  -1,  -1, 362,  -1,
       -1, 363, 364, 365, 366,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 367, 368, 369,  -1, 370, 371,
       -1,  -1, 372,  -1, 373,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 374,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 375,  -1, 376,  -1,  -1,
       -1,  -1,  -1,  -1, 377, 378,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 379, 380,
       -1,  -1, 381,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 382,  -1,  -1,  -1,
      383,  -1,  -1, 384,  -1, 385, 386,  -1, 387,  -1,
       -1, 388,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 389,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 390, 391, 392,  -1,  -1,  -1, 393, 394,
       -1,  -1,  -1,  -1, 395,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 396,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 397,  -1,  -1, 398, 399,  -1,  -1,  -1,  -1,
       -1, 400,  -1, 401,  -1,  -1,  -1, 402, 403, 404,
      405, 406,  -1, 407,  -1, 408,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 409,  -1, 410,
       -1,  -1,  -1,  -1, 411,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 412,  -1,  -1,  -1,  -1,
      413,  -1,  -1, 414, 415,  -1,  -1,  -1,  -1,  -1,
      416,  -1,  -1,  -1,  -1,  -1, 417,  -1,  -1, 418,
       -1, 419, 420,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 421,  -1,  -1,  -1,  -1, 422,  -1, 423,  -1,
      424,  -1,  -1,  -1, 425,  -1, 426, 427, 428,  -1,
       -1, 429, 430,  -1,  -1,  -1,  -1, 431,  -1,  -1,
      432, 433, 434,  -1,  -1, 435,  -1,  -1, 436,  -1,
       -1, 437,  -1,  -1,  -1,  -1, 438,  -1,  -1, 439,
       -1,  -1, 440,  -1, 441,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 442,  -1, 443,
       -1,  -1,  -1,  -1, 444, 445,  -1,  -1,  -1,  -1,
       -1,  -1, 446, 447,  -1,  -1, 448, 449, 450, 451,
       -1,  -1, 452, 453,  -1,  -1,  -1,  -1,  -1,  -1,
      454,  -1,  -1,  -1,  -1, 455, 456,  -1,  -1,  -1,
      457,  -1, 458,  -1,  -1,  -1, 459,  -1,  -1, 460,
      461,  -1, 462,  -1,  -1,  -1,  -1, 463, 464,  -1,
       -1, 465,  -1, 466,  -1,  -1,  -1,  -1, 467, 468,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 469,  -1,
      470,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      471,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 472,
       -1, 473,  -1,  -1,  -1,  -1,  -1,  -1, 474,  -1,
       -1,  -1,  -1,  -1,  -1, 475, 476,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 477,  -1,  -1, 478,  -1,
       -1,  -1, 479, 480,  -1,  -1, 481,  -1,  -1,  -1,
      482,  -1,  -1,  -1, 483, 484, 485,  -1,  -1,  -1,
       -1,  -1, 486,  -1,  -1,  -1,  -1, 487,  -1,  -1,
      488,  -1, 489,  -1,  -1, 490,  -1,  -1,  -1,  -1,
       -1, 491,  -1,  -1, 492,  -1,  -1, 493, 494,  -1,
       -1, 495,  -1,  -1, 496,  -1,  -1, 497, 498, 499,
      500,  -1, 501,  -1, 502, 503,  -1,  -1,  -1,  -1,
      504,  -1,  -1,  -1, 505,  -1, 506,  -1, 507, 508,
       -1, 509, 510,  -1,  -1,  -1,  -1,  -1, 511,  -1,
       -1,  -1,  -1,  -1,  -1, 512,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 513,  -1,  -1,  -1,  -1,  -1, 514,  -1,  -1,
       -1, 515,  -1,  -1,  -1,  -1,  -1,  -1, 516,  -1,
       -1, 517,  -1,  -1,  -1,  -1,  -1,  -1, 518,  -1,
       -1, 519,  -1, 520,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 521, 522,  -1,  -1, 523,  -1,  -1,  -1,  -1,
       -1, 524,  -1,  -1, 525,  -1,  -1,  -1,  -1, 526,
       -1,  -1,  -1, 527, 528,  -1,  -1,  -1,  -1, 529,
       -1, 530,  -1,  -1,  -1, 531, 532,  -1, 533,  -1,
       -1,  -1,  -1, 534, 535,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 536,  -1, 537,  -1, 538,  -1,  -1, 539,
       -1,  -1,  -1, 540,  -1,  -1,  -1,  -1,  -1,  -1,
      541,  -1,  -1, 542,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 543,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 544,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 545,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 546, 547,  -1,  -1, 548,  -1,  -1,
      549, 550,  -1,  -1,  -1,  -1,  -1, 551, 552,  -1,
       -1,  -1, 553,  -1,  -1, 554,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      555,  -1, 556,  -1,  -1,  -1,  -1,  -1, 557,  -1,
       -1,  -1, 558,  -1,  -1,  -1,  -1, 559,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 560,  -1,  -1,  -1,  -1,  -1,
       -1, 561,  -1,  -1,  -1,  -1,  -1, 562,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 563, 564,  -1, 565,
       -1,  -1,  -1, 566,  -1,  -1,  -1,  -1,  -1,  -1,
      567,  -1,  -1, 568,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 569,  -1,  -1,
       -1,  -1, 570, 571,  -1, 572, 573,  -1,  -1,  -1,
       -1,  -1,  -1, 574,  -1,  -1, 575,  -1, 576,  -1,
      577,  -1,  -1,  -1,  -1, 578, 579,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 580,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 581, 582,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 583,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 584,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 585,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 586,  -1, 587,  -1, 588,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 589,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 590,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 591,  -1,  -1, 592,
       -1,  -1,  -1,  -1,  -1, 593,  -1,  -1,  -1,  -1,
      594, 595,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 596,  -1,  -1,  -1,  -1,  -1,  -1, 597,  -1,
       -1, 598,  -1, 599, 600,  -1,  -1,  -1,  -1,  -1,
      601, 602,  -1,  -1, 603,  -1,  -1,  -1,  -1, 604,
       -1, 605,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      606,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 607,
       -1,  -1,  -1,  -1,  -1, 608,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 609,  -1,  -1,  -1,
       -1, 610,  -1,  -1,  -1,  -1,  -1, 611,  -1,  -1,
       -1,  -1,  -1,  -1, 612,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 613,
      614,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 615,  -1,  -1,  -1,  -1,  -1, 616,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      617, 618, 619,  -1, 620, 621,  -1,  -1, 622,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 623, 624,  -1, 625,
      626,  -1,  -1,  -1, 627,  -1, 628,  -1, 629,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 630,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 631,  -1,  -1,  -1,  -1,  -1,  -1, 632,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 633,  -1,  -1, 634,  -1,  -1, 635,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 636,  -1,  -1,
       -1,  -1, 637,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 638,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 639,  -1, 640, 641,
       -1,  -1, 642,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 643,  -1, 644, 645,  -1,  -1,  -1, 646,
       -1,  -1,  -1,  -1,  -1,  -1, 647, 648,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 649,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 650,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 651,
      652,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 653,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 654,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 655,  -1,  -1,  -1,  -1,  -1,
       -1, 656,  -1,  -1, 657,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 658,
       -1,  -1,  -1,  -1,  -1, 659,  -1,  -1,  -1, 660,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 661,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 662,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 663,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 664,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 665,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 666,  -1,
       -1,  -1, 667,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      668,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 669,  -1,  -1,
       -1, 670,  -1,  -1,  -1,  -1,  -1, 671,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 672,  -1,
       -1,  -1, 673,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 674,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      675,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 676,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 677,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 678, 679,  -1,  -1,  -1,  -1,  -1, 680,  -1,
       -1,  -1, 681,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 682,  -1,  -1,  -1,
      683,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 684,  -1,  -1,  -1,  -1,  -1,
       -1, 685, 686,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 687,  -1,  -1,  -1,
       -1,  -1,  -1, 688,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 689, 690,  -1,
      691,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 692,
      693,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      694,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 695,  -1,  -1,  -1,  -1,  -1,  -1, 696,
       -1,  -1,  -1,  -1,  -1, 697,  -1,  -1, 698,  -1,
      699, 700,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 701,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 702,  -1,  -1,  -1,  -1,  -1,  -1, 703,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      704,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 705,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 706,  -1,
       -1, 707, 708,  -1,  -1,  -1, 709,  -1,  -1,  -1,
       -1,  -1, 710,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      711,  -1,  -1,  -1,  -1,  -1, 712, 713,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 714,  -1,  -1,  -1,  -1,
      715,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 716,
       -1, 717,  -1,  -1,  -1,  -1,  -1, 718,  -1,  -1,
       -1,  -1,  -1,  -1, 719,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 720,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 721,  -1, 722,  -1,
       -1,  -1, 723,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 724,
       -1,  -1,  -1, 725,  -1,  -1,  -1,  -1, 726,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 727,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 728,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 729,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 730,  -1, 731,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 732,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      733,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 734,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 735,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 736, 737,  -1,  -1,  -1,
       -1,  -1,  -1, 738,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 739, 740,  -1,  -1, 741,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 742,  -1,  -1,  -1,  -1, 743,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      744,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      745,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 746,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 747,  -1, 748,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 749,  -1,  -1,  -1,  -1,  -1, 750,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 751,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 752,  -1,  -1,  -1,  -1, 753,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 754,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      755,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 756,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 757,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 758,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 759,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      760,  -1,  -1, 761, 762,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 763,  -1, 764,
       -1, 765, 766, 767, 768,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 769,  -1,  -1,  -1,  -1, 770,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 771,  -1,  -1,  -1,
       -1,  -1, 772,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 773,  -1,  -1,  -1,  -1,  -1, 774,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 775,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 776,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 777,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 778,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 779,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      780,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 781,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 782,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 783,  -1,  -1,  -1, 784,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 785,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      786,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      787,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 788,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      789,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 790,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 791,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 792,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 793,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 794,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 795,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 796
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = value_hash_function (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register int index = lookup[key];

          if (index >= 0)
            {
              register const char *s = value_word_list[index].nameOffset + stringpool;

              if (*str == *s && !strncmp (str + 1, s + 1, len - 1) && s[len] == '\0')
                return &value_word_list[index];
            }
        }
    }
  return 0;
}

const Value* findValue(register const char* str, register unsigned int len)
{
    return CSSValueKeywordsHash::findValueImpl(str, len);
}

const char* getValueName(unsigned short id)
{
    if (id >= numCSSValueKeywords || id <= 0)
        return 0;
    return valueListStringPool + valueListStringOffsets[id];
}

bool isValueAllowedInMode(unsigned short id, CSSParserMode mode)
{
    switch (id) {
        case CSSValueInternalActiveListBoxSelection:
        case CSSValueInternalActiveListBoxSelectionText:
        case CSSValueInternalInactiveListBoxSelection:
        case CSSValueInternalInactiveListBoxSelectionText:
        case CSSValueInternalMediaCastOffButton:
        case CSSValueInternalMediaOverlayCastOffButton:
        case CSSValueInternalExtendToZoom:
        case CSSValueInternalVariableValue:
            return isUASheetBehavior(mode);
        case CSSValueWebkitFocusRingColor:
    case CSSValueWebkitText:
            return isUASheetBehavior(mode) || isQuirksModeBehavior(mode);
        default:
            return true;
    }
}

} // namespace blink
