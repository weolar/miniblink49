/* C++ code produced by gperf version 3.0.1 */
/* Command-line: ../../../../third_party/gperf/bin/gperf.exe --key-positions='*' -P -n -m 50 -D  */

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
  7240,
  7253,
  7264,
  7270,
  7276,
  7283,
  7298,
  7309,
  7315,
  7325,
  7335,
  7346,
  7356,
  7362,
  7377,
  7386,
  7394,
  7399,
  7408,
  7417,
  7431,
  7440,
  7450,
  7459,
  7469,
  7481,
  7496,
  7507,
  7518,
  7526,
  7537,
  7550,
  7564,
  7578,
  7592,
  7606,
  7617,
  7626,
  7638,
  7646,
  7654,
  7665,
  7675,
  7687,
  7699,
  7709,
  7720,
  7725,
  7735,
  7747,
  7756,
  7764,
  7774,
  7781,
  7787,
  7793,
  7802,
  7816,
  7826,
  7839,
  7849,
  7860,
  7870,
  7891,
  7901,
  7912,
  7922,
  7932,
  7944,
  7958,
  7971,
  7986,
  8001,
  8016,
  8028,
  8038,
  8044,
  8052,
  8069,
  8080,
  8093,
  8106,
  8121,
  8137,
  8155,
  8171,
  8187,
  8200,
  8210,
  8220,
  8229,
  8241,
  8249,
  8259,
  8269,
  8276,
  8290,
  8300,
  8314,
  8328,
  8339,
  8349,
  8354,
  8359,
  8364,
  8375,
  8389,
  8399,
  8409,
  8421,
  8428,
  8439,
  8448,
  8457,
  8464,
  8472,
  8482,
  8492,
  8502,
  8507,
  8519,
  8529,
  8533,
  8541,
  8548,
  8558,
  8565,
  8571,
  8582,
  8594,
  8600,
  8610,
  8621,
  8625,
  8630,
  8640,
  8651,
  8656,
  8662,
  8668,
  8680,
  8691,
  8699,
  8716,
  8732,
  8744,
  8752,
  8765,
  8776,
  8786,
  8797,
  8805,
};

/* maximum key range = 6337, duplicates = 0 */

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
      6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370,
      6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370,
      6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370,
      6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370,
      6370, 6370, 6370, 6370, 6370,  100,  208, 6370,   14, 6370,
        19,   16,   18,   15,   12,   26,   22,   14,   13, 6370,
      6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370,
      6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370,
      6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370,
      6370, 6370, 6370, 6370, 6370, 6370, 6370,   13,  359,  118,
        12,   11,  386,   61,   65,  104,  941,  656,   15,   89,
        11,  168,  241, 1001,   10,   34,   12,   30, 1009,  668,
       861, 1261,  176,   22,   11,   10, 6370, 6370, 6370, 6370,
      6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370,
      6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370,
      6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370,
      6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370,
      6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370,
      6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370,
      6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370,
      6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370,
      6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370,
      6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370,
      6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370,
      6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370,
      6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370, 6370
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
    char stringpool_str1[sizeof("to")];
    char stringpool_str2[sizeof("tb")];
    char stringpool_str3[sizeof("500")];
    char stringpool_str4[sizeof("900")];
    char stringpool_str5[sizeof("800")];
    char stringpool_str6[sizeof("400")];
    char stringpool_str7[sizeof("200")];
    char stringpool_str8[sizeof("cyan")];
    char stringpool_str9[sizeof("300")];
    char stringpool_str10[sizeof("100")];
    char stringpool_str11[sizeof("700")];
    char stringpool_str12[sizeof("tan")];
    char stringpool_str13[sizeof("600")];
    char stringpool_str14[sizeof("caret")];
    char stringpool_str15[sizeof("miter")];
    char stringpool_str16[sizeof("coral")];
    char stringpool_str17[sizeof("dotted")];
    char stringpool_str18[sizeof("span")];
    char stringpool_str19[sizeof("sienna")];
    char stringpool_str20[sizeof("middle")];
    char stringpool_str21[sizeof("content")];
    char stringpool_str22[sizeof("kannada")];
    char stringpool_str23[sizeof("square")];
    char stringpool_str24[sizeof("coarse")];
    char stringpool_str25[sizeof("rotate")];
    char stringpool_str26[sizeof("font")];
    char stringpool_str27[sizeof("counter")];
    char stringpool_str28[sizeof("round")];
    char stringpool_str29[sizeof("fine")];
    char stringpool_str30[sizeof("true")];
    char stringpool_str31[sizeof("gold")];
    char stringpool_str32[sizeof("fill")];
    char stringpool_str33[sizeof("cell")];
    char stringpool_str34[sizeof("meter")];
    char stringpool_str35[sizeof("red")];
    char stringpool_str36[sizeof("saturate")];
    char stringpool_str37[sizeof("contents")];
    char stringpool_str38[sizeof("contrast")];
    char stringpool_str39[sizeof("drag")];
    char stringpool_str40[sizeof("menu")];
    char stringpool_str41[sizeof("lr")];
    char stringpool_str42[sizeof("rotate3d")];
    char stringpool_str43[sizeof("center")];
    char stringpool_str44[sizeof("teal")];
    char stringpool_str45[sizeof("condensed")];
    char stringpool_str46[sizeof("single")];
    char stringpool_str47[sizeof("green")];
    char stringpool_str48[sizeof("filled")];
    char stringpool_str49[sizeof("counters")];
    char stringpool_str50[sizeof("magenta")];
    char stringpool_str51[sizeof("dense")];
    char stringpool_str52[sizeof("ridge")];
    char stringpool_str53[sizeof("linen")];
    char stringpool_str54[sizeof("central")];
    char stringpool_str55[sizeof("hand")];
    char stringpool_str56[sizeof("b5")];
    char stringpool_str57[sizeof("hide")];
    char stringpool_str58[sizeof("b4")];
    char stringpool_str59[sizeof("dashed")];
    char stringpool_str60[sizeof("linear")];
    char stringpool_str61[sizeof("solid")];
    char stringpool_str62[sizeof("bold")];
    char stringpool_str63[sizeof("reset")];
    char stringpool_str64[sizeof("calc")];
    char stringpool_str65[sizeof("myanmar")];
    char stringpool_str66[sizeof("hidden")];
    char stringpool_str67[sizeof("tb-rl")];
    char stringpool_str68[sizeof("column")];
    char stringpool_str69[sizeof("translate")];
    char stringpool_str70[sizeof("border")];
    char stringpool_str71[sizeof("contain")];
    char stringpool_str72[sizeof("bolder")];
    char stringpool_str73[sizeof("disc")];
    char stringpool_str74[sizeof("circle")];
    char stringpool_str75[sizeof("large")];
    char stringpool_str76[sizeof("right")];
    char stringpool_str77[sizeof("space")];
    char stringpool_str78[sizeof("radial")];
    char stringpool_str79[sizeof("larger")];
    char stringpool_str80[sizeof("seagreen")];
    char stringpool_str81[sizeof("format")];
    char stringpool_str82[sizeof("translate3d")];
    char stringpool_str83[sizeof("letter")];
    char stringpool_str84[sizeof("none")];
    char stringpool_str85[sizeof("rl")];
    char stringpool_str86[sizeof("lime")];
    char stringpool_str87[sizeof("both")];
    char stringpool_str88[sizeof("grid")];
    char stringpool_str89[sizeof("discard")];
    char stringpool_str90[sizeof("clear")];
    char stringpool_str91[sizeof("color")];
    char stringpool_str92[sizeof("sesame")];
    char stringpool_str93[sizeof("rect")];
    char stringpool_str94[sizeof("seashell")];
    char stringpool_str95[sizeof("telugu")];
    char stringpool_str96[sizeof("hangul")];
    char stringpool_str97[sizeof("digits")];
    char stringpool_str98[sizeof("hiragana")];
    char stringpool_str99[sizeof("legal")];
    char stringpool_str100[sizeof("local")];
    char stringpool_str101[sizeof("ledger")];
    char stringpool_str102[sizeof("flat")];
    char stringpool_str103[sizeof("lighter")];
    char stringpool_str104[sizeof("lighten")];
    char stringpool_str105[sizeof("rotatez")];
    char stringpool_str106[sizeof("lao")];
    char stringpool_str107[sizeof("triangle")];
    char stringpool_str108[sizeof("top")];
    char stringpool_str109[sizeof("menulist")];
    char stringpool_str110[sizeof("zoom")];
    char stringpool_str111[sizeof("mintcream")];
    char stringpool_str112[sizeof("document")];
    char stringpool_str113[sizeof("small")];
    char stringpool_str114[sizeof("open")];
    char stringpool_str115[sizeof("balance")];
    char stringpool_str116[sizeof("normal")];
    char stringpool_str117[sizeof("salmon")];
    char stringpool_str118[sizeof("limegreen")];
    char stringpool_str119[sizeof("medium")];
    char stringpool_str120[sizeof("loose")];
    char stringpool_str121[sizeof("goldenrod")];
    char stringpool_str122[sizeof("baseline")];
    char stringpool_str123[sizeof("smaller")];
    char stringpool_str124[sizeof("tomato")];
    char stringpool_str125[sizeof("blur")];
    char stringpool_str126[sizeof("blue")];
    char stringpool_str127[sizeof("radio")];
    char stringpool_str128[sizeof("translatez")];
    char stringpool_str129[sizeof("lightgreen")];
    char stringpool_str130[sizeof("from")];
    char stringpool_str131[sizeof("collapse")];
    char stringpool_str132[sizeof("dynamic")];
    char stringpool_str133[sizeof("beige")];
    char stringpool_str134[sizeof("hanging")];
    char stringpool_str135[sizeof("listitem")];
    char stringpool_str136[sizeof("orange")];
    char stringpool_str137[sizeof("clone")];
    char stringpool_str138[sizeof("a3")];
    char stringpool_str139[sizeof("s-resize")];
    char stringpool_str140[sizeof("no-drag")];
    char stringpool_str141[sizeof("repeat")];
    char stringpool_str142[sizeof("separate")];
    char stringpool_str143[sizeof("maroon")];
    char stringpool_str144[sizeof("a5")];
    char stringpool_str145[sizeof("saturation")];
    char stringpool_str146[sizeof("a4")];
    char stringpool_str147[sizeof("bengali")];
    char stringpool_str148[sizeof("spell-out")];
    char stringpool_str149[sizeof("forestgreen")];
    char stringpool_str150[sizeof("orangered")];
    char stringpool_str151[sizeof("mongolian")];
    char stringpool_str152[sizeof("source-in")];
    char stringpool_str153[sizeof("bottom")];
    char stringpool_str154[sizeof("logical")];
    char stringpool_str155[sizeof("slice")];
    char stringpool_str156[sizeof("aqua")];
    char stringpool_str157[sizeof("decimal")];
    char stringpool_str158[sizeof("moccasin")];
    char stringpool_str159[sizeof("azure")];
    char stringpool_str160[sizeof("safe")];
    char stringpool_str161[sizeof("help")];
    char stringpool_str162[sizeof("table")];
    char stringpool_str163[sizeof("transparent")];
    char stringpool_str164[sizeof("sepia")];
    char stringpool_str165[sizeof("double")];
    char stringpool_str166[sizeof("scale")];
    char stringpool_str167[sizeof("screen")];
    char stringpool_str168[sizeof("ease")];
    char stringpool_str169[sizeof("crimson")];
    char stringpool_str170[sizeof("tibetan")];
    char stringpool_str171[sizeof("srgb")];
    char stringpool_str172[sizeof("scale3d")];
    char stringpool_str173[sizeof("list-item")];
    char stringpool_str174[sizeof("crosshair")];
    char stringpool_str175[sizeof("cadetblue")];
    char stringpool_str176[sizeof("keep-all")];
    char stringpool_str177[sizeof("brightness")];
    char stringpool_str178[sizeof("col-resize")];
    char stringpool_str179[sizeof("minimal-ui")];
    char stringpool_str180[sizeof("springgreen")];
    char stringpool_str181[sizeof("hard-light")];
    char stringpool_str182[sizeof("after")];
    char stringpool_str183[sizeof("georgian")];
    char stringpool_str184[sizeof("min-content")];
    char stringpool_str185[sizeof("source-out")];
    char stringpool_str186[sizeof("highlight")];
    char stringpool_str187[sizeof("zoom-in")];
    char stringpool_str188[sizeof("replaced")];
    char stringpool_str189[sizeof("min-intrinsic")];
    char stringpool_str190[sizeof("se-resize")];
    char stringpool_str191[sizeof("clip")];
    char stringpool_str192[sizeof("n-resize")];
    char stringpool_str193[sizeof("dodgerblue")];
    char stringpool_str194[sizeof("default")];
    char stringpool_str195[sizeof("landscape")];
    char stringpool_str196[sizeof("all")];
    char stringpool_str197[sizeof("crispedges")];
    char stringpool_str198[sizeof("exact")];
    char stringpool_str199[sizeof("left")];
    char stringpool_str200[sizeof("nonzero")];
    char stringpool_str201[sizeof("no-change")];
    char stringpool_str202[sizeof("caption")];
    char stringpool_str203[sizeof("lightcoral")];
    char stringpool_str204[sizeof("orchid")];
    char stringpool_str205[sizeof("no-repeat")];
    char stringpool_str206[sizeof("serif")];
    char stringpool_str207[sizeof("oldlace")];
    char stringpool_str208[sizeof("linearrgb")];
    char stringpool_str209[sizeof("reset-size")];
    char stringpool_str210[sizeof("lr-tb")];
    char stringpool_str211[sizeof("distribute")];
    char stringpool_str212[sizeof("zoom-out")];
    char stringpool_str213[sizeof("scroll")];
    char stringpool_str214[sizeof("alternate")];
    char stringpool_str215[sizeof("self-end")];
    char stringpool_str216[sizeof("scalez")];
    char stringpool_str217[sizeof("monospace")];
    char stringpool_str218[sizeof("horizontal")];
    char stringpool_str219[sizeof("armenian")];
    char stringpool_str220[sizeof("slateblue")];
    char stringpool_str221[sizeof("sw-resize")];
    char stringpool_str222[sizeof("absolute")];
    char stringpool_str223[sizeof("line-through")];
    char stringpool_str224[sizeof("aquamarine")];
    char stringpool_str225[sizeof("lightblue")];
    char stringpool_str226[sizeof("ease-in")];
    char stringpool_str227[sizeof("self-start")];
    char stringpool_str228[sizeof("smooth")];
    char stringpool_str229[sizeof("rl-tb")];
    char stringpool_str230[sizeof("alias")];
    char stringpool_str231[sizeof("ne-resize")];
    char stringpool_str232[sizeof("table-cell")];
    char stringpool_str233[sizeof("element")];
    char stringpool_str234[sizeof("space-around")];
    char stringpool_str235[sizeof("after-edge")];
    char stringpool_str236[sizeof("closest-side")];
    char stringpool_str237[sizeof("capitalize")];
    char stringpool_str238[sizeof("skew")];
    char stringpool_str239[sizeof("expanded")];
    char stringpool_str240[sizeof("no-drop")];
    char stringpool_str241[sizeof("darken")];
    char stringpool_str242[sizeof("row")];
    char stringpool_str243[sizeof("sans-serif")];
    char stringpool_str244[sizeof("start")];
    char stringpool_str245[sizeof("rtl")];
    char stringpool_str246[sizeof("darkred")];
    char stringpool_str247[sizeof("color-burn")];
    char stringpool_str248[sizeof("mediumblue")];
    char stringpool_str249[sizeof("lightsalmon")];
    char stringpool_str250[sizeof("off")];
    char stringpool_str251[sizeof("markers")];
    char stringpool_str252[sizeof("katakana")];
    char stringpool_str253[sizeof("e-resize")];
    char stringpool_str254[sizeof("ease-out")];
    char stringpool_str255[sizeof("before")];
    char stringpool_str256[sizeof("hiragana-iroha")];
    char stringpool_str257[sizeof("ltr")];
    char stringpool_str258[sizeof("color-stop")];
    char stringpool_str259[sizeof("visual")];
    char stringpool_str260[sizeof("soft-light")];
    char stringpool_str261[sizeof("cambodian")];
    char stringpool_str262[sizeof("khmer")];
    char stringpool_str263[sizeof("nw-resize")];
    char stringpool_str264[sizeof("darkgreen")];
    char stringpool_str265[sizeof("chartreuse")];
    char stringpool_str266[sizeof("thin")];
    char stringpool_str267[sizeof("small-caps")];
    char stringpool_str268[sizeof("thai")];
    char stringpool_str269[sizeof("manipulation")];
    char stringpool_str270[sizeof("searchfield")];
    char stringpool_str271[sizeof("forwards")];
    char stringpool_str272[sizeof("mathematical")];
    char stringpool_str273[sizeof("each-line")];
    char stringpool_str274[sizeof("alpha")];
    char stringpool_str275[sizeof("cornsilk")];
    char stringpool_str276[sizeof("linear-gradient")];
    char stringpool_str277[sizeof("thistle")];
    char stringpool_str278[sizeof("media-slider")];
    char stringpool_str279[sizeof("mediumseagreen")];
    char stringpool_str280[sizeof("farthest-corner")];
    char stringpool_str281[sizeof("table-column")];
    char stringpool_str282[sizeof("lawngreen")];
    char stringpool_str283[sizeof("exclusion")];
    char stringpool_str284[sizeof("violet")];
    char stringpool_str285[sizeof("stretch")];
    char stringpool_str286[sizeof("radial-gradient")];
    char stringpool_str287[sizeof("new")];
    char stringpool_str288[sizeof("blanchedalmond")];
    char stringpool_str289[sizeof("strict")];
    char stringpool_str290[sizeof("static")];
    char stringpool_str291[sizeof("gainsboro")];
    char stringpool_str292[sizeof("mediumpurple")];
    char stringpool_str293[sizeof("standalone")];
    char stringpool_str294[sizeof("lightseagreen")];
    char stringpool_str295[sizeof("rotatex")];
    char stringpool_str296[sizeof("color-dodge")];
    char stringpool_str297[sizeof("square-button")];
    char stringpool_str298[sizeof("ellipse")];
    char stringpool_str299[sizeof("fixed")];
    char stringpool_str300[sizeof("horizontal-tb")];
    char stringpool_str301[sizeof("darkorange")];
    char stringpool_str302[sizeof("lowercase")];
    char stringpool_str303[sizeof("steps")];
    char stringpool_str304[sizeof("text")];
    char stringpool_str305[sizeof("difference")];
    char stringpool_str306[sizeof("break-all")];
    char stringpool_str307[sizeof("hue")];
    char stringpool_str308[sizeof("ease-in-out")];
    char stringpool_str309[sizeof("contextual")];
    char stringpool_str310[sizeof("scrollbar")];
    char stringpool_str311[sizeof("wait")];
    char stringpool_str312[sizeof("brown")];
    char stringpool_str313[sizeof("lower-greek")];
    char stringpool_str314[sizeof("minmax")];
    char stringpool_str315[sizeof("all-scroll")];
    char stringpool_str316[sizeof("butt")];
    char stringpool_str317[sizeof("vertical")];
    char stringpool_str318[sizeof("ew-resize")];
    char stringpool_str319[sizeof("embed")];
    char stringpool_str320[sizeof("matrix")];
    char stringpool_str321[sizeof("menutext")];
    char stringpool_str322[sizeof("hangul-consonant")];
    char stringpool_str323[sizeof("textarea")];
    char stringpool_str324[sizeof("darksalmon")];
    char stringpool_str325[sizeof("at")];
    char stringpool_str326[sizeof("read-write")];
    char stringpool_str327[sizeof("translatex")];
    char stringpool_str328[sizeof("below")];
    char stringpool_str329[sizeof("mediumspringgreen")];
    char stringpool_str330[sizeof("matrix3d")];
    char stringpool_str331[sizeof("browser")];
    char stringpool_str332[sizeof("move")];
    char stringpool_str333[sizeof("pre")];
    char stringpool_str334[sizeof("attr")];
    char stringpool_str335[sizeof("cover")];
    char stringpool_str336[sizeof("url")];
    char stringpool_str337[sizeof("darkslategrey")];
    char stringpool_str338[sizeof("blink")];
    char stringpool_str339[sizeof("darkslategray")];
    char stringpool_str340[sizeof("accumulate")];
    char stringpool_str341[sizeof("step-end")];
    char stringpool_str342[sizeof("silver")];
    char stringpool_str343[sizeof("black")];
    char stringpool_str344[sizeof("urdu")];
    char stringpool_str345[sizeof("destination-out")];
    char stringpool_str346[sizeof("ellipsis")];
    char stringpool_str347[sizeof("path")];
    char stringpool_str348[sizeof("running")];
    char stringpool_str349[sizeof("paused")];
    char stringpool_str350[sizeof("flex")];
    char stringpool_str351[sizeof("darkblue")];
    char stringpool_str352[sizeof("peru")];
    char stringpool_str353[sizeof("yellow")];
    char stringpool_str354[sizeof("nowrap")];
    char stringpool_str355[sizeof("step-start")];
    char stringpool_str356[sizeof("no-punctuation")];
    char stringpool_str357[sizeof("hsl")];
    char stringpool_str358[sizeof("steelblue")];
    char stringpool_str359[sizeof("super")];
    char stringpool_str360[sizeof("hotpink")];
    char stringpool_str361[sizeof("hsla")];
    char stringpool_str362[sizeof("palegreen")];
    char stringpool_str363[sizeof("hover")];
    char stringpool_str364[sizeof("wrap")];
    char stringpool_str365[sizeof("fullscreen")];
    char stringpool_str366[sizeof("vertical-lr")];
    char stringpool_str367[sizeof("row-resize")];
    char stringpool_str368[sizeof("pointer")];
    char stringpool_str369[sizeof("painted")];
    char stringpool_str370[sizeof("lavender")];
    char stringpool_str371[sizeof("darkorchid")];
    char stringpool_str372[sizeof("outset")];
    char stringpool_str373[sizeof("portrait")];
    char stringpool_str374[sizeof("chocolate")];
    char stringpool_str375[sizeof("reverse")];
    char stringpool_str376[sizeof("aliceblue")];
    char stringpool_str377[sizeof("repeating-radial-gradient")];
    char stringpool_str378[sizeof("darkgoldenrod")];
    char stringpool_str379[sizeof("double-circle")];
    char stringpool_str380[sizeof("button")];
    char stringpool_str381[sizeof("literal-punctuation")];
    char stringpool_str382[sizeof("not-allowed")];
    char stringpool_str383[sizeof("background")];
    char stringpool_str384[sizeof("lower-latin")];
    char stringpool_str385[sizeof("vertical-rl")];
    char stringpool_str386[sizeof("visible")];
    char stringpool_str387[sizeof("bisque")];
    char stringpool_str388[sizeof("nwse-resize")];
    char stringpool_str389[sizeof("padding")];
    char stringpool_str390[sizeof("hebrew")];
    char stringpool_str391[sizeof("bevel")];
    char stringpool_str392[sizeof("horizontal-bt")];
    char stringpool_str393[sizeof("w-resize")];
    char stringpool_str394[sizeof("korean-hangul-formal")];
    char stringpool_str395[sizeof("darkseagreen")];
    char stringpool_str396[sizeof("block")];
    char stringpool_str397[sizeof("sub")];
    char stringpool_str398[sizeof("flex-end")];
    char stringpool_str399[sizeof("persian")];
    char stringpool_str400[sizeof("on")];
    char stringpool_str401[sizeof("darkmagenta")];
    char stringpool_str402[sizeof("start-touch")];
    char stringpool_str403[sizeof("status-bar")];
    char stringpool_str404[sizeof("lightpink")];
    char stringpool_str405[sizeof("fuchsia")];
    char stringpool_str406[sizeof("threedface")];
    char stringpool_str407[sizeof("mediumslateblue")];
    char stringpool_str408[sizeof("relative")];
    char stringpool_str409[sizeof("closest-corner")];
    char stringpool_str410[sizeof("katakana-iroha")];
    char stringpool_str411[sizeof("multiple")];
    char stringpool_str412[sizeof("outside")];
    char stringpool_str413[sizeof("no-historical-ligatures")];
    char stringpool_str414[sizeof("xor")];
    char stringpool_str415[sizeof("flex-start")];
    char stringpool_str416[sizeof("pre-line")];
    char stringpool_str417[sizeof("extra-condensed")];
    char stringpool_str418[sizeof("plum")];
    char stringpool_str419[sizeof("semi-condensed")];
    char stringpool_str420[sizeof("upright")];
    char stringpool_str421[sizeof("scalex")];
    char stringpool_str422[sizeof("hue-rotate")];
    char stringpool_str423[sizeof("luminance")];
    char stringpool_str424[sizeof("devanagari")];
    char stringpool_str425[sizeof("context-menu")];
    char stringpool_str426[sizeof("before-edge")];
    char stringpool_str427[sizeof("rgb")];
    char stringpool_str428[sizeof("lower-roman")];
    char stringpool_str429[sizeof("repeat-x")];
    char stringpool_str430[sizeof("deeppink")];
    char stringpool_str431[sizeof("firebrick")];
    char stringpool_str432[sizeof("max-content")];
    char stringpool_str433[sizeof("rgba")];
    char stringpool_str434[sizeof("progress")];
    char stringpool_str435[sizeof("rotatey")];
    char stringpool_str436[sizeof("threedhighlight")];
    char stringpool_str437[sizeof("geometricprecision")];
    char stringpool_str438[sizeof("grey")];
    char stringpool_str439[sizeof("yellowgreen")];
    char stringpool_str440[sizeof("gray")];
    char stringpool_str441[sizeof("table-row")];
    char stringpool_str442[sizeof("pan-right")];
    char stringpool_str443[sizeof("repeating-linear-gradient")];
    char stringpool_str444[sizeof("x-large")];
    char stringpool_str445[sizeof("no-common-ligatures")];
    char stringpool_str446[sizeof("pan-up")];
    char stringpool_str447[sizeof("scale-down")];
    char stringpool_str448[sizeof("end")];
    char stringpool_str449[sizeof("auto")];
    char stringpool_str450[sizeof("fantasy")];
    char stringpool_str451[sizeof("trad-chinese-formal")];
    char stringpool_str452[sizeof("oblique")];
    char stringpool_str453[sizeof("discretionary-ligatures")];
    char stringpool_str454[sizeof("translatey")];
    char stringpool_str455[sizeof("media-sliderthumb")];
    char stringpool_str456[sizeof("x-small")];
    char stringpool_str457[sizeof("malayalam")];
    char stringpool_str458[sizeof("khaki")];
    char stringpool_str459[sizeof("saddlebrown")];
    char stringpool_str460[sizeof("alphabetic")];
    char stringpool_str461[sizeof("groove")];
    char stringpool_str462[sizeof("optimizespeed")];
    char stringpool_str463[sizeof("dimgrey")];
    char stringpool_str464[sizeof("dimgray")];
    char stringpool_str465[sizeof("captiontext")];
    char stringpool_str466[sizeof("lightsteelblue")];
    char stringpool_str467[sizeof("currentcolor")];
    char stringpool_str468[sizeof("uppercase")];
    char stringpool_str469[sizeof("source-atop")];
    char stringpool_str470[sizeof("alternate-reverse")];
    char stringpool_str471[sizeof("text-top")];
    char stringpool_str472[sizeof("on-demand")];
    char stringpool_str473[sizeof("media-enter-fullscreen-button")];
    char stringpool_str474[sizeof("xx-large")];
    char stringpool_str475[sizeof("textfield")];
    char stringpool_str476[sizeof("show")];
    char stringpool_str477[sizeof("icon")];
    char stringpool_str478[sizeof("mandatory")];
    char stringpool_str479[sizeof("sliderthumb-horizontal")];
    char stringpool_str480[sizeof("source-over")];
    char stringpool_str481[sizeof("trad-chinese-informal")];
    char stringpool_str482[sizeof("over")];
    char stringpool_str483[sizeof("copy")];
    char stringpool_str484[sizeof("stroke")];
    char stringpool_str485[sizeof("palegoldenrod")];
    char stringpool_str486[sizeof("olive")];
    char stringpool_str487[sizeof("searchfield-decoration")];
    char stringpool_str488[sizeof("listbox")];
    char stringpool_str489[sizeof("semi-expanded")];
    char stringpool_str490[sizeof("pan-left")];
    char stringpool_str491[sizeof("xx-small")];
    char stringpool_str492[sizeof("mediumaquamarine")];
    char stringpool_str493[sizeof("wheat")];
    char stringpool_str494[sizeof("mistyrose")];
    char stringpool_str495[sizeof("above")];
    char stringpool_str496[sizeof("slategrey")];
    char stringpool_str497[sizeof("grayscale")];
    char stringpool_str498[sizeof("slategray")];
    char stringpool_str499[sizeof("skewx")];
    char stringpool_str500[sizeof("thick")];
    char stringpool_str501[sizeof("lower-alpha")];
    char stringpool_str502[sizeof("destination-atop")];
    char stringpool_str503[sizeof("lightgrey")];
    char stringpool_str504[sizeof("lightgray")];
    char stringpool_str505[sizeof("cjk-earthly-branch")];
    char stringpool_str506[sizeof("table-column-group")];
    char stringpool_str507[sizeof("legacy")];
    char stringpool_str508[sizeof("non-scaling-stroke")];
    char stringpool_str509[sizeof("open-quote")];
    char stringpool_str510[sizeof("slider-vertical")];
    char stringpool_str511[sizeof("ns-resize")];
    char stringpool_str512[sizeof("mediumorchid")];
    char stringpool_str513[sizeof("darkslateblue")];
    char stringpool_str514[sizeof("backwards")];
    char stringpool_str515[sizeof("white")];
    char stringpool_str516[sizeof("lightcyan")];
    char stringpool_str517[sizeof("blueviolet")];
    char stringpool_str518[sizeof("oriya")];
    char stringpool_str519[sizeof("step-middle")];
    char stringpool_str520[sizeof("lower-armenian")];
    char stringpool_str521[sizeof("border-box")];
    char stringpool_str522[sizeof("overline")];
    char stringpool_str523[sizeof("floralwhite")];
    char stringpool_str524[sizeof("menulist-text")];
    char stringpool_str525[sizeof("antialiased")];
    char stringpool_str526[sizeof("simp-chinese-formal")];
    char stringpool_str527[sizeof("read-only")];
    char stringpool_str528[sizeof("sliderthumb-vertical")];
    char stringpool_str529[sizeof("pink")];
    char stringpool_str530[sizeof("midnightblue")];
    char stringpool_str531[sizeof("ideographic")];
    char stringpool_str532[sizeof("farthest-side")];
    char stringpool_str533[sizeof("margin-box")];
    char stringpool_str534[sizeof("scaley")];
    char stringpool_str535[sizeof("skyblue")];
    char stringpool_str536[sizeof("drop-shadow")];
    char stringpool_str537[sizeof("buttonface")];
    char stringpool_str538[sizeof("column-reverse")];
    char stringpool_str539[sizeof("upper-latin")];
    char stringpool_str540[sizeof("-webkit-left")];
    char stringpool_str541[sizeof("lemonchiffon")];
    char stringpool_str542[sizeof("window")];
    char stringpool_str543[sizeof("repeat-y")];
    char stringpool_str544[sizeof("table-header-group")];
    char stringpool_str545[sizeof("royalblue")];
    char stringpool_str546[sizeof("destination-in")];
    char stringpool_str547[sizeof("nesw-resize")];
    char stringpool_str548[sizeof("threedshadow")];
    char stringpool_str549[sizeof("searchfield-cancel-button")];
    char stringpool_str550[sizeof("opacity")];
    char stringpool_str551[sizeof("break-word")];
    char stringpool_str552[sizeof("highlighttext")];
    char stringpool_str553[sizeof("simp-chinese-informal")];
    char stringpool_str554[sizeof("ghostwhite")];
    char stringpool_str555[sizeof("text-bottom")];
    char stringpool_str556[sizeof("visiblefill")];
    char stringpool_str557[sizeof("text-after-edge")];
    char stringpool_str558[sizeof("searchfield-results-decoration")];
    char stringpool_str559[sizeof("buttonhighlight")];
    char stringpool_str560[sizeof("korean-hanja-formal")];
    char stringpool_str561[sizeof("glyphs")];
    char stringpool_str562[sizeof("snow")];
    char stringpool_str563[sizeof("upper-roman")];
    char stringpool_str564[sizeof("italic")];
    char stringpool_str565[sizeof("evenodd")];
    char stringpool_str566[sizeof("-webkit-auto")];
    char stringpool_str567[sizeof("gurmukhi")];
    char stringpool_str568[sizeof("-webkit-right")];
    char stringpool_str569[sizeof("space-between")];
    char stringpool_str570[sizeof("avoid")];
    char stringpool_str571[sizeof("scroll-event")];
    char stringpool_str572[sizeof("bounding-box")];
    char stringpool_str573[sizeof("olivedrab")];
    char stringpool_str574[sizeof("historical-ligatures")];
    char stringpool_str575[sizeof("korean-hanja-informal")];
    char stringpool_str576[sizeof("media-volume-slider")];
    char stringpool_str577[sizeof("pixelated")];
    char stringpool_str578[sizeof("skewy")];
    char stringpool_str579[sizeof("-webkit-calc")];
    char stringpool_str580[sizeof("content-box")];
    char stringpool_str581[sizeof("menulist-button")];
    char stringpool_str582[sizeof("pan-down")];
    char stringpool_str583[sizeof("pan-x")];
    char stringpool_str584[sizeof("plus-lighter")];
    char stringpool_str585[sizeof("inset")];
    char stringpool_str586[sizeof("threedlightshadow")];
    char stringpool_str587[sizeof("darkviolet")];
    char stringpool_str588[sizeof("last-baseline")];
    char stringpool_str589[sizeof("gujarati")];
    char stringpool_str590[sizeof("lightslategray")];
    char stringpool_str591[sizeof("sideways")];
    char stringpool_str592[sizeof("darkgrey")];
    char stringpool_str593[sizeof("darkgray")];
    char stringpool_str594[sizeof("cursive")];
    char stringpool_str595[sizeof("under")];
    char stringpool_str596[sizeof("lightslategrey")];
    char stringpool_str597[sizeof("ultra-condensed")];
    char stringpool_str598[sizeof("small-caption")];
    char stringpool_str599[sizeof("table-row-group")];
    char stringpool_str600[sizeof("mediumvioletred")];
    char stringpool_str601[sizeof("unset")];
    char stringpool_str602[sizeof("slider-horizontal")];
    char stringpool_str603[sizeof("destination-over")];
    char stringpool_str604[sizeof("pre-wrap")];
    char stringpool_str605[sizeof("buttontext")];
    char stringpool_str606[sizeof("inline")];
    char stringpool_str607[sizeof("honeydew")];
    char stringpool_str608[sizeof("close-quote")];
    char stringpool_str609[sizeof("darkcyan")];
    char stringpool_str610[sizeof("common-ligatures")];
    char stringpool_str611[sizeof("upper-alpha")];
    char stringpool_str612[sizeof("inside")];
    char stringpool_str613[sizeof("indianred")];
    char stringpool_str614[sizeof("message-box")];
    char stringpool_str615[sizeof("powderblue")];
    char stringpool_str616[sizeof("interlace")];
    char stringpool_str617[sizeof("purple")];
    char stringpool_str618[sizeof("visiblepainted")];
    char stringpool_str619[sizeof("inherit")];
    char stringpool_str620[sizeof("cjk-ideographic")];
    char stringpool_str621[sizeof("isolate")];
    char stringpool_str622[sizeof("upper-armenian")];
    char stringpool_str623[sizeof("progress-bar")];
    char stringpool_str624[sizeof("darkkhaki")];
    char stringpool_str625[sizeof("underline")];
    char stringpool_str626[sizeof("table-caption")];
    char stringpool_str627[sizeof("initial")];
    char stringpool_str628[sizeof("menulist-textfield")];
    char stringpool_str629[sizeof("buttonshadow")];
    char stringpool_str630[sizeof("media-mute-button")];
    char stringpool_str631[sizeof("vertical-right")];
    char stringpool_str632[sizeof("preserve-3d")];
    char stringpool_str633[sizeof("extra-expanded")];
    char stringpool_str634[sizeof("turquoise")];
    char stringpool_str635[sizeof("graytext")];
    char stringpool_str636[sizeof("arabic-indic")];
    char stringpool_str637[sizeof("row-reverse")];
    char stringpool_str638[sizeof("indigo")];
    char stringpool_str639[sizeof("peachpuff")];
    char stringpool_str640[sizeof("-webkit-control")];
    char stringpool_str641[sizeof("wrap-reverse")];
    char stringpool_str642[sizeof("-webkit-min-content")];
    char stringpool_str643[sizeof("after-white-space")];
    char stringpool_str644[sizeof("-webkit-center")];
    char stringpool_str645[sizeof("intrinsic")];
    char stringpool_str646[sizeof("block-axis")];
    char stringpool_str647[sizeof("inline-grid")];
    char stringpool_str648[sizeof("table-footer-group")];
    char stringpool_str649[sizeof("-webkit-match-parent")];
    char stringpool_str650[sizeof("pan-y")];
    char stringpool_str651[sizeof("decimal-leading-zero")];
    char stringpool_str652[sizeof("vertical-text")];
    char stringpool_str653[sizeof("rebeccapurple")];
    char stringpool_str654[sizeof("-webkit-isolate")];
    char stringpool_str655[sizeof("palevioletred")];
    char stringpool_str656[sizeof("-webkit-grab")];
    char stringpool_str657[sizeof("appworkspace")];
    char stringpool_str658[sizeof("navy")];
    char stringpool_str659[sizeof("cornflowerblue")];
    char stringpool_str660[sizeof("greenyellow")];
    char stringpool_str661[sizeof("economy")];
    char stringpool_str662[sizeof("-webkit-radial-gradient")];
    char stringpool_str663[sizeof("media-controls-background")];
    char stringpool_str664[sizeof("no-open-quote")];
    char stringpool_str665[sizeof("-webkit-fit-content")];
    char stringpool_str666[sizeof("media-volume-sliderthumb")];
    char stringpool_str667[sizeof("darkolivegreen")];
    char stringpool_str668[sizeof("media-volume-slider-container")];
    char stringpool_str669[sizeof("always")];
    char stringpool_str670[sizeof("multiply")];
    char stringpool_str671[sizeof("sandybrown")];
    char stringpool_str672[sizeof("polygon")];
    char stringpool_str673[sizeof("rating-level-indicator")];
    char stringpool_str674[sizeof("visiblestroke")];
    char stringpool_str675[sizeof("rosybrown")];
    char stringpool_str676[sizeof("-webkit-cross-fade")];
    char stringpool_str677[sizeof("-internal-media-cast-off-button")];
    char stringpool_str678[sizeof("-webkit-link")];
    char stringpool_str679[sizeof("infinite")];
    char stringpool_str680[sizeof("lightyellow")];
    char stringpool_str681[sizeof("-webkit-baseline-middle")];
    char stringpool_str682[sizeof("optimizelegibility")];
    char stringpool_str683[sizeof("no-contextual")];
    char stringpool_str684[sizeof("cubic-bezier")];
    char stringpool_str685[sizeof("whitesmoke")];
    char stringpool_str686[sizeof("windowframe")];
    char stringpool_str687[sizeof("-webkit-zoom-in")];
    char stringpool_str688[sizeof("-webkit-pictograph")];
    char stringpool_str689[sizeof("-webkit-linear-gradient")];
    char stringpool_str690[sizeof("windowtext")];
    char stringpool_str691[sizeof("use-script")];
    char stringpool_str692[sizeof("media-fullscreen-volume-slider")];
    char stringpool_str693[sizeof("inline-table")];
    char stringpool_str694[sizeof("space-evenly")];
    char stringpool_str695[sizeof("lavenderblush")];
    char stringpool_str696[sizeof("-internal-extend-to-zoom")];
    char stringpool_str697[sizeof("threeddarkshadow")];
    char stringpool_str698[sizeof("-webkit-grabbing")];
    char stringpool_str699[sizeof("-webkit-zoom-out")];
    char stringpool_str700[sizeof("wheel-event")];
    char stringpool_str701[sizeof("luminosity")];
    char stringpool_str702[sizeof("media-toggle-closed-captions-button")];
    char stringpool_str703[sizeof("-webkit-image-set")];
    char stringpool_str704[sizeof("-webkit-mini-control")];
    char stringpool_str705[sizeof("push-button")];
    char stringpool_str706[sizeof("overlay")];
    char stringpool_str707[sizeof("-webkit-small-control")];
    char stringpool_str708[sizeof("activeborder")];
    char stringpool_str709[sizeof("checkbox")];
    char stringpool_str710[sizeof("caps-lock-indicator")];
    char stringpool_str711[sizeof("progressive")];
    char stringpool_str712[sizeof("bidi-override")];
    char stringpool_str713[sizeof("-webkit-text")];
    char stringpool_str714[sizeof("-webkit-paged-x")];
    char stringpool_str715[sizeof("inter-word")];
    char stringpool_str716[sizeof("no-close-quote")];
    char stringpool_str717[sizeof("-webkit-optimize-contrast")];
    char stringpool_str718[sizeof("ethiopic-halehame")];
    char stringpool_str719[sizeof("text-before-edge")];
    char stringpool_str720[sizeof("invert")];
    char stringpool_str721[sizeof("perspective")];
    char stringpool_str722[sizeof("media-current-time-display")];
    char stringpool_str723[sizeof("ultra-expanded")];
    char stringpool_str724[sizeof("-webkit-gradient")];
    char stringpool_str725[sizeof("button-bevel")];
    char stringpool_str726[sizeof("deepskyblue")];
    char stringpool_str727[sizeof("lightgoldenrodyellow")];
    char stringpool_str728[sizeof("-webkit-focus-ring-color")];
    char stringpool_str729[sizeof("padding-box")];
    char stringpool_str730[sizeof("wavy")];
    char stringpool_str731[sizeof("ethiopic-halehame-am")];
    char stringpool_str732[sizeof("inline-axis")];
    char stringpool_str733[sizeof("media-controls-fullscreen-background")];
    char stringpool_str734[sizeof("scroll-position")];
    char stringpool_str735[sizeof("sideways-right")];
    char stringpool_str736[sizeof("-webkit-flex")];
    char stringpool_str737[sizeof("-webkit-nowrap")];
    char stringpool_str738[sizeof("burlywood")];
    char stringpool_str739[sizeof("ethiopic-halehame-ti-er")];
    char stringpool_str740[sizeof("ethiopic-halehame-ti-et")];
    char stringpool_str741[sizeof("media-time-remaining-display")];
    char stringpool_str742[sizeof("darkturquoise")];
    char stringpool_str743[sizeof("media-fullscreen-volume-slider-thumb")];
    char stringpool_str744[sizeof("justify")];
    char stringpool_str745[sizeof("-webkit-max-content")];
    char stringpool_str746[sizeof("-webkit-canvas")];
    char stringpool_str747[sizeof("-webkit-paged-y")];
    char stringpool_str748[sizeof("-webkit-activelink")];
    char stringpool_str749[sizeof("infotext")];
    char stringpool_str750[sizeof("inline-block")];
    char stringpool_str751[sizeof("antiquewhite")];
    char stringpool_str752[sizeof("-webkit-plaintext")];
    char stringpool_str753[sizeof("-webkit-box")];
    char stringpool_str754[sizeof("-webkit-body")];
    char stringpool_str755[sizeof("progress-bar-value")];
    char stringpool_str756[sizeof("-webkit-inline-flex")];
    char stringpool_str757[sizeof("mediumturquoise")];
    char stringpool_str758[sizeof("no-discretionary-ligatures")];
    char stringpool_str759[sizeof("activecaption")];
    char stringpool_str760[sizeof("navajowhite")];
    char stringpool_str761[sizeof("inline-box")];
    char stringpool_str762[sizeof("lightskyblue")];
    char stringpool_str763[sizeof("-webkit-inline-box")];
    char stringpool_str764[sizeof("media-play-button")];
    char stringpool_str765[sizeof("papayawhip")];
    char stringpool_str766[sizeof("proximity")];
    char stringpool_str767[sizeof("ivory")];
    char stringpool_str768[sizeof("paleturquoise")];
    char stringpool_str769[sizeof("cjk-heavenly-stem")];
    char stringpool_str770[sizeof("-webkit-repeating-linear-gradient")];
    char stringpool_str771[sizeof("-webkit-repeating-radial-gradient")];
    char stringpool_str772[sizeof("media-exit-fullscreen-button")];
    char stringpool_str773[sizeof("-webkit-isolate-override")];
    char stringpool_str774[sizeof("inline-flex")];
    char stringpool_str775[sizeof("subpixel-antialiased")];
    char stringpool_str776[sizeof("-webkit-fill-available")];
    char stringpool_str777[sizeof("relevancy-level-indicator")];
    char stringpool_str778[sizeof("media-overlay-play-button")];
    char stringpool_str779[sizeof("read-write-plaintext-only")];
    char stringpool_str780[sizeof("infobackground")];
    char stringpool_str781[sizeof("inactiveborder")];
    char stringpool_str782[sizeof("inner-spin-button")];
    char stringpool_str783[sizeof("-internal-active-list-box-selection")];
    char stringpool_str784[sizeof("optimizequality")];
    char stringpool_str785[sizeof("continuous-capacity-level-indicator")];
    char stringpool_str786[sizeof("discrete-capacity-level-indicator")];
    char stringpool_str787[sizeof("-webkit-xxx-large")];
    char stringpool_str788[sizeof("-internal-media-overlay-cast-off-button")];
    char stringpool_str789[sizeof("-internal-inactive-list-box-selection")];
    char stringpool_str790[sizeof("-internal-active-list-box-selection-text")];
    char stringpool_str791[sizeof("inactivecaption")];
    char stringpool_str792[sizeof("-internal-inactive-list-box-selection-text")];
    char stringpool_str793[sizeof("inactivecaptiontext")];
  };
static const struct stringpool_t stringpool_contents =
  {
    "dot",
    "to",
    "tb",
    "500",
    "900",
    "800",
    "400",
    "200",
    "cyan",
    "300",
    "100",
    "700",
    "tan",
    "600",
    "caret",
    "miter",
    "coral",
    "dotted",
    "span",
    "sienna",
    "middle",
    "content",
    "kannada",
    "square",
    "coarse",
    "rotate",
    "font",
    "counter",
    "round",
    "fine",
    "true",
    "gold",
    "fill",
    "cell",
    "meter",
    "red",
    "saturate",
    "contents",
    "contrast",
    "drag",
    "menu",
    "lr",
    "rotate3d",
    "center",
    "teal",
    "condensed",
    "single",
    "green",
    "filled",
    "counters",
    "magenta",
    "dense",
    "ridge",
    "linen",
    "central",
    "hand",
    "b5",
    "hide",
    "b4",
    "dashed",
    "linear",
    "solid",
    "bold",
    "reset",
    "calc",
    "myanmar",
    "hidden",
    "tb-rl",
    "column",
    "translate",
    "border",
    "contain",
    "bolder",
    "disc",
    "circle",
    "large",
    "right",
    "space",
    "radial",
    "larger",
    "seagreen",
    "format",
    "translate3d",
    "letter",
    "none",
    "rl",
    "lime",
    "both",
    "grid",
    "discard",
    "clear",
    "color",
    "sesame",
    "rect",
    "seashell",
    "telugu",
    "hangul",
    "digits",
    "hiragana",
    "legal",
    "local",
    "ledger",
    "flat",
    "lighter",
    "lighten",
    "rotatez",
    "lao",
    "triangle",
    "top",
    "menulist",
    "zoom",
    "mintcream",
    "document",
    "small",
    "open",
    "balance",
    "normal",
    "salmon",
    "limegreen",
    "medium",
    "loose",
    "goldenrod",
    "baseline",
    "smaller",
    "tomato",
    "blur",
    "blue",
    "radio",
    "translatez",
    "lightgreen",
    "from",
    "collapse",
    "dynamic",
    "beige",
    "hanging",
    "listitem",
    "orange",
    "clone",
    "a3",
    "s-resize",
    "no-drag",
    "repeat",
    "separate",
    "maroon",
    "a5",
    "saturation",
    "a4",
    "bengali",
    "spell-out",
    "forestgreen",
    "orangered",
    "mongolian",
    "source-in",
    "bottom",
    "logical",
    "slice",
    "aqua",
    "decimal",
    "moccasin",
    "azure",
    "safe",
    "help",
    "table",
    "transparent",
    "sepia",
    "double",
    "scale",
    "screen",
    "ease",
    "crimson",
    "tibetan",
    "srgb",
    "scale3d",
    "list-item",
    "crosshair",
    "cadetblue",
    "keep-all",
    "brightness",
    "col-resize",
    "minimal-ui",
    "springgreen",
    "hard-light",
    "after",
    "georgian",
    "min-content",
    "source-out",
    "highlight",
    "zoom-in",
    "replaced",
    "min-intrinsic",
    "se-resize",
    "clip",
    "n-resize",
    "dodgerblue",
    "default",
    "landscape",
    "all",
    "crispedges",
    "exact",
    "left",
    "nonzero",
    "no-change",
    "caption",
    "lightcoral",
    "orchid",
    "no-repeat",
    "serif",
    "oldlace",
    "linearrgb",
    "reset-size",
    "lr-tb",
    "distribute",
    "zoom-out",
    "scroll",
    "alternate",
    "self-end",
    "scalez",
    "monospace",
    "horizontal",
    "armenian",
    "slateblue",
    "sw-resize",
    "absolute",
    "line-through",
    "aquamarine",
    "lightblue",
    "ease-in",
    "self-start",
    "smooth",
    "rl-tb",
    "alias",
    "ne-resize",
    "table-cell",
    "element",
    "space-around",
    "after-edge",
    "closest-side",
    "capitalize",
    "skew",
    "expanded",
    "no-drop",
    "darken",
    "row",
    "sans-serif",
    "start",
    "rtl",
    "darkred",
    "color-burn",
    "mediumblue",
    "lightsalmon",
    "off",
    "markers",
    "katakana",
    "e-resize",
    "ease-out",
    "before",
    "hiragana-iroha",
    "ltr",
    "color-stop",
    "visual",
    "soft-light",
    "cambodian",
    "khmer",
    "nw-resize",
    "darkgreen",
    "chartreuse",
    "thin",
    "small-caps",
    "thai",
    "manipulation",
    "searchfield",
    "forwards",
    "mathematical",
    "each-line",
    "alpha",
    "cornsilk",
    "linear-gradient",
    "thistle",
    "media-slider",
    "mediumseagreen",
    "farthest-corner",
    "table-column",
    "lawngreen",
    "exclusion",
    "violet",
    "stretch",
    "radial-gradient",
    "new",
    "blanchedalmond",
    "strict",
    "static",
    "gainsboro",
    "mediumpurple",
    "standalone",
    "lightseagreen",
    "rotatex",
    "color-dodge",
    "square-button",
    "ellipse",
    "fixed",
    "horizontal-tb",
    "darkorange",
    "lowercase",
    "steps",
    "text",
    "difference",
    "break-all",
    "hue",
    "ease-in-out",
    "contextual",
    "scrollbar",
    "wait",
    "brown",
    "lower-greek",
    "minmax",
    "all-scroll",
    "butt",
    "vertical",
    "ew-resize",
    "embed",
    "matrix",
    "menutext",
    "hangul-consonant",
    "textarea",
    "darksalmon",
    "at",
    "read-write",
    "translatex",
    "below",
    "mediumspringgreen",
    "matrix3d",
    "browser",
    "move",
    "pre",
    "attr",
    "cover",
    "url",
    "darkslategrey",
    "blink",
    "darkslategray",
    "accumulate",
    "step-end",
    "silver",
    "black",
    "urdu",
    "destination-out",
    "ellipsis",
    "path",
    "running",
    "paused",
    "flex",
    "darkblue",
    "peru",
    "yellow",
    "nowrap",
    "step-start",
    "no-punctuation",
    "hsl",
    "steelblue",
    "super",
    "hotpink",
    "hsla",
    "palegreen",
    "hover",
    "wrap",
    "fullscreen",
    "vertical-lr",
    "row-resize",
    "pointer",
    "painted",
    "lavender",
    "darkorchid",
    "outset",
    "portrait",
    "chocolate",
    "reverse",
    "aliceblue",
    "repeating-radial-gradient",
    "darkgoldenrod",
    "double-circle",
    "button",
    "literal-punctuation",
    "not-allowed",
    "background",
    "lower-latin",
    "vertical-rl",
    "visible",
    "bisque",
    "nwse-resize",
    "padding",
    "hebrew",
    "bevel",
    "horizontal-bt",
    "w-resize",
    "korean-hangul-formal",
    "darkseagreen",
    "block",
    "sub",
    "flex-end",
    "persian",
    "on",
    "darkmagenta",
    "start-touch",
    "status-bar",
    "lightpink",
    "fuchsia",
    "threedface",
    "mediumslateblue",
    "relative",
    "closest-corner",
    "katakana-iroha",
    "multiple",
    "outside",
    "no-historical-ligatures",
    "xor",
    "flex-start",
    "pre-line",
    "extra-condensed",
    "plum",
    "semi-condensed",
    "upright",
    "scalex",
    "hue-rotate",
    "luminance",
    "devanagari",
    "context-menu",
    "before-edge",
    "rgb",
    "lower-roman",
    "repeat-x",
    "deeppink",
    "firebrick",
    "max-content",
    "rgba",
    "progress",
    "rotatey",
    "threedhighlight",
    "geometricprecision",
    "grey",
    "yellowgreen",
    "gray",
    "table-row",
    "pan-right",
    "repeating-linear-gradient",
    "x-large",
    "no-common-ligatures",
    "pan-up",
    "scale-down",
    "end",
    "auto",
    "fantasy",
    "trad-chinese-formal",
    "oblique",
    "discretionary-ligatures",
    "translatey",
    "media-sliderthumb",
    "x-small",
    "malayalam",
    "khaki",
    "saddlebrown",
    "alphabetic",
    "groove",
    "optimizespeed",
    "dimgrey",
    "dimgray",
    "captiontext",
    "lightsteelblue",
    "currentcolor",
    "uppercase",
    "source-atop",
    "alternate-reverse",
    "text-top",
    "on-demand",
    "media-enter-fullscreen-button",
    "xx-large",
    "textfield",
    "show",
    "icon",
    "mandatory",
    "sliderthumb-horizontal",
    "source-over",
    "trad-chinese-informal",
    "over",
    "copy",
    "stroke",
    "palegoldenrod",
    "olive",
    "searchfield-decoration",
    "listbox",
    "semi-expanded",
    "pan-left",
    "xx-small",
    "mediumaquamarine",
    "wheat",
    "mistyrose",
    "above",
    "slategrey",
    "grayscale",
    "slategray",
    "skewx",
    "thick",
    "lower-alpha",
    "destination-atop",
    "lightgrey",
    "lightgray",
    "cjk-earthly-branch",
    "table-column-group",
    "legacy",
    "non-scaling-stroke",
    "open-quote",
    "slider-vertical",
    "ns-resize",
    "mediumorchid",
    "darkslateblue",
    "backwards",
    "white",
    "lightcyan",
    "blueviolet",
    "oriya",
    "step-middle",
    "lower-armenian",
    "border-box",
    "overline",
    "floralwhite",
    "menulist-text",
    "antialiased",
    "simp-chinese-formal",
    "read-only",
    "sliderthumb-vertical",
    "pink",
    "midnightblue",
    "ideographic",
    "farthest-side",
    "margin-box",
    "scaley",
    "skyblue",
    "drop-shadow",
    "buttonface",
    "column-reverse",
    "upper-latin",
    "-webkit-left",
    "lemonchiffon",
    "window",
    "repeat-y",
    "table-header-group",
    "royalblue",
    "destination-in",
    "nesw-resize",
    "threedshadow",
    "searchfield-cancel-button",
    "opacity",
    "break-word",
    "highlighttext",
    "simp-chinese-informal",
    "ghostwhite",
    "text-bottom",
    "visiblefill",
    "text-after-edge",
    "searchfield-results-decoration",
    "buttonhighlight",
    "korean-hanja-formal",
    "glyphs",
    "snow",
    "upper-roman",
    "italic",
    "evenodd",
    "-webkit-auto",
    "gurmukhi",
    "-webkit-right",
    "space-between",
    "avoid",
    "scroll-event",
    "bounding-box",
    "olivedrab",
    "historical-ligatures",
    "korean-hanja-informal",
    "media-volume-slider",
    "pixelated",
    "skewy",
    "-webkit-calc",
    "content-box",
    "menulist-button",
    "pan-down",
    "pan-x",
    "plus-lighter",
    "inset",
    "threedlightshadow",
    "darkviolet",
    "last-baseline",
    "gujarati",
    "lightslategray",
    "sideways",
    "darkgrey",
    "darkgray",
    "cursive",
    "under",
    "lightslategrey",
    "ultra-condensed",
    "small-caption",
    "table-row-group",
    "mediumvioletred",
    "unset",
    "slider-horizontal",
    "destination-over",
    "pre-wrap",
    "buttontext",
    "inline",
    "honeydew",
    "close-quote",
    "darkcyan",
    "common-ligatures",
    "upper-alpha",
    "inside",
    "indianred",
    "message-box",
    "powderblue",
    "interlace",
    "purple",
    "visiblepainted",
    "inherit",
    "cjk-ideographic",
    "isolate",
    "upper-armenian",
    "progress-bar",
    "darkkhaki",
    "underline",
    "table-caption",
    "initial",
    "menulist-textfield",
    "buttonshadow",
    "media-mute-button",
    "vertical-right",
    "preserve-3d",
    "extra-expanded",
    "turquoise",
    "graytext",
    "arabic-indic",
    "row-reverse",
    "indigo",
    "peachpuff",
    "-webkit-control",
    "wrap-reverse",
    "-webkit-min-content",
    "after-white-space",
    "-webkit-center",
    "intrinsic",
    "block-axis",
    "inline-grid",
    "table-footer-group",
    "-webkit-match-parent",
    "pan-y",
    "decimal-leading-zero",
    "vertical-text",
    "rebeccapurple",
    "-webkit-isolate",
    "palevioletred",
    "-webkit-grab",
    "appworkspace",
    "navy",
    "cornflowerblue",
    "greenyellow",
    "economy",
    "-webkit-radial-gradient",
    "media-controls-background",
    "no-open-quote",
    "-webkit-fit-content",
    "media-volume-sliderthumb",
    "darkolivegreen",
    "media-volume-slider-container",
    "always",
    "multiply",
    "sandybrown",
    "polygon",
    "rating-level-indicator",
    "visiblestroke",
    "rosybrown",
    "-webkit-cross-fade",
    "-internal-media-cast-off-button",
    "-webkit-link",
    "infinite",
    "lightyellow",
    "-webkit-baseline-middle",
    "optimizelegibility",
    "no-contextual",
    "cubic-bezier",
    "whitesmoke",
    "windowframe",
    "-webkit-zoom-in",
    "-webkit-pictograph",
    "-webkit-linear-gradient",
    "windowtext",
    "use-script",
    "media-fullscreen-volume-slider",
    "inline-table",
    "space-evenly",
    "lavenderblush",
    "-internal-extend-to-zoom",
    "threeddarkshadow",
    "-webkit-grabbing",
    "-webkit-zoom-out",
    "wheel-event",
    "luminosity",
    "media-toggle-closed-captions-button",
    "-webkit-image-set",
    "-webkit-mini-control",
    "push-button",
    "overlay",
    "-webkit-small-control",
    "activeborder",
    "checkbox",
    "caps-lock-indicator",
    "progressive",
    "bidi-override",
    "-webkit-text",
    "-webkit-paged-x",
    "inter-word",
    "no-close-quote",
    "-webkit-optimize-contrast",
    "ethiopic-halehame",
    "text-before-edge",
    "invert",
    "perspective",
    "media-current-time-display",
    "ultra-expanded",
    "-webkit-gradient",
    "button-bevel",
    "deepskyblue",
    "lightgoldenrodyellow",
    "-webkit-focus-ring-color",
    "padding-box",
    "wavy",
    "ethiopic-halehame-am",
    "inline-axis",
    "media-controls-fullscreen-background",
    "scroll-position",
    "sideways-right",
    "-webkit-flex",
    "-webkit-nowrap",
    "burlywood",
    "ethiopic-halehame-ti-er",
    "ethiopic-halehame-ti-et",
    "media-time-remaining-display",
    "darkturquoise",
    "media-fullscreen-volume-slider-thumb",
    "justify",
    "-webkit-max-content",
    "-webkit-canvas",
    "-webkit-paged-y",
    "-webkit-activelink",
    "infotext",
    "inline-block",
    "antiquewhite",
    "-webkit-plaintext",
    "-webkit-box",
    "-webkit-body",
    "progress-bar-value",
    "-webkit-inline-flex",
    "mediumturquoise",
    "no-discretionary-ligatures",
    "activecaption",
    "navajowhite",
    "inline-box",
    "lightskyblue",
    "-webkit-inline-box",
    "media-play-button",
    "papayawhip",
    "proximity",
    "ivory",
    "paleturquoise",
    "cjk-heavenly-stem",
    "-webkit-repeating-linear-gradient",
    "-webkit-repeating-radial-gradient",
    "media-exit-fullscreen-button",
    "-webkit-isolate-override",
    "inline-flex",
    "subpixel-antialiased",
    "-webkit-fill-available",
    "relevancy-level-indicator",
    "media-overlay-play-button",
    "read-write-plaintext-only",
    "infobackground",
    "inactiveborder",
    "inner-spin-button",
    "-internal-active-list-box-selection",
    "optimizequality",
    "continuous-capacity-level-indicator",
    "discrete-capacity-level-indicator",
    "-webkit-xxx-large",
    "-internal-media-overlay-cast-off-button",
    "-internal-inactive-list-box-selection",
    "-internal-active-list-box-selection-text",
    "inactivecaption",
    "-internal-inactive-list-box-selection-text",
    "inactivecaptiontext"
  };
#define stringpool ((const char *) &stringpool_contents)
const struct Value *
CSSValueKeywordsHash::findValueImpl (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 794,
      MIN_WORD_LENGTH = 2,
      MAX_WORD_LENGTH = 42,
      MIN_HASH_VALUE = 33,
      MAX_HASH_VALUE = 6369
    };

  static const struct Value value_word_list[] =
    {
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str0, CSSValueDot},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1, CSSValueTo},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str2, CSSValueTb},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str3, CSSValue500},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str4, CSSValue900},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str5, CSSValue800},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str6, CSSValue400},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str7, CSSValue200},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str8, CSSValueCyan},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str9, CSSValue300},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str10, CSSValue100},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str11, CSSValue700},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str12, CSSValueTan},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str13, CSSValue600},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str14, CSSValueCaret},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str15, CSSValueMiter},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str16, CSSValueCoral},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str17, CSSValueDotted},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str18, CSSValueSpan},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str19, CSSValueSienna},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str20, CSSValueMiddle},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str21, CSSValueContent},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str22, CSSValueKannada},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str23, CSSValueSquare},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str24, CSSValueCoarse},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str25, CSSValueRotate},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str26, CSSValueFont},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str27, CSSValueCounter},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str28, CSSValueRound},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str29, CSSValueFine},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str30, CSSValueTrue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str31, CSSValueGold},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str32, CSSValueFill},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str33, CSSValueCell},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str34, CSSValueMeter},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str35, CSSValueRed},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str36, CSSValueSaturate},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str37, CSSValueContents},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str38, CSSValueContrast},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str39, CSSValueDrag},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str40, CSSValueMenu},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str41, CSSValueLr},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str42, CSSValueRotate3d},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str43, CSSValueCenter},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str44, CSSValueTeal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str45, CSSValueCondensed},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str46, CSSValueSingle},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str47, CSSValueGreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str48, CSSValueFilled},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str49, CSSValueCounters},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str50, CSSValueMagenta},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str51, CSSValueDense},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str52, CSSValueRidge},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str53, CSSValueLinen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str54, CSSValueCentral},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str55, CSSValueHand},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str56, CSSValueB5},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str57, CSSValueHide},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str58, CSSValueB4},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str59, CSSValueDashed},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str60, CSSValueLinear},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str61, CSSValueSolid},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str62, CSSValueBold},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str63, CSSValueReset},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str64, CSSValueCalc},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str65, CSSValueMyanmar},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str66, CSSValueHidden},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str67, CSSValueTbRl},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str68, CSSValueColumn},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str69, CSSValueTranslate},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str70, CSSValueBorder},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str71, CSSValueContain},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str72, CSSValueBolder},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str73, CSSValueDisc},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str74, CSSValueCircle},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str75, CSSValueLarge},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str76, CSSValueRight},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str77, CSSValueSpace},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str78, CSSValueRadial},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str79, CSSValueLarger},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str80, CSSValueSeagreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str81, CSSValueFormat},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str82, CSSValueTranslate3d},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str83, CSSValueLetter},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str84, CSSValueNone},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str85, CSSValueRl},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str86, CSSValueLime},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str87, CSSValueBoth},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str88, CSSValueGrid},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str89, CSSValueDiscard},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str90, CSSValueClear},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str91, CSSValueColor},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str92, CSSValueSesame},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str93, CSSValueRect},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str94, CSSValueSeashell},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str95, CSSValueTelugu},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str96, CSSValueHangul},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str97, CSSValueDigits},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str98, CSSValueHiragana},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str99, CSSValueLegal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str100, CSSValueLocal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str101, CSSValueLedger},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str102, CSSValueFlat},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str103, CSSValueLighter},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str104, CSSValueLighten},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str105, CSSValueRotateZ},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str106, CSSValueLao},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str107, CSSValueTriangle},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str108, CSSValueTop},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str109, CSSValueMenulist},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str110, CSSValueZoom},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str111, CSSValueMintcream},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str112, CSSValueDocument},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str113, CSSValueSmall},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str114, CSSValueOpen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str115, CSSValueBalance},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str116, CSSValueNormal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str117, CSSValueSalmon},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str118, CSSValueLimegreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str119, CSSValueMedium},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str120, CSSValueLoose},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str121, CSSValueGoldenrod},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str122, CSSValueBaseline},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str123, CSSValueSmaller},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str124, CSSValueTomato},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str125, CSSValueBlur},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str126, CSSValueBlue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str127, CSSValueRadio},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str128, CSSValueTranslateZ},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str129, CSSValueLightgreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str130, CSSValueFrom},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str131, CSSValueCollapse},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str132, CSSValueDynamic},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str133, CSSValueBeige},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str134, CSSValueHanging},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str135, CSSValueListitem},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str136, CSSValueOrange},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str137, CSSValueClone},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str138, CSSValueA3},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str139, CSSValueSResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str140, CSSValueNoDrag},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str141, CSSValueRepeat},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str142, CSSValueSeparate},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str143, CSSValueMaroon},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str144, CSSValueA5},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str145, CSSValueSaturation},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str146, CSSValueA4},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str147, CSSValueBengali},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str148, CSSValueSpellOut},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str149, CSSValueForestgreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str150, CSSValueOrangered},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str151, CSSValueMongolian},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str152, CSSValueSourceIn},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str153, CSSValueBottom},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str154, CSSValueLogical},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str155, CSSValueSlice},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str156, CSSValueAqua},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str157, CSSValueDecimal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str158, CSSValueMoccasin},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str159, CSSValueAzure},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str160, CSSValueSafe},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str161, CSSValueHelp},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str162, CSSValueTable},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str163, CSSValueTransparent},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str164, CSSValueSepia},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str165, CSSValueDouble},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str166, CSSValueScale},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str167, CSSValueScreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str168, CSSValueEase},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str169, CSSValueCrimson},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str170, CSSValueTibetan},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str171, CSSValueSRGB},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str172, CSSValueScale3d},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str173, CSSValueListItem},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str174, CSSValueCrosshair},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str175, CSSValueCadetblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str176, CSSValueKeepAll},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str177, CSSValueBrightness},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str178, CSSValueColResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str179, CSSValueMinimalUi},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str180, CSSValueSpringgreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str181, CSSValueHardLight},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str182, CSSValueAfter},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str183, CSSValueGeorgian},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str184, CSSValueMinContent},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str185, CSSValueSourceOut},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str186, CSSValueHighlight},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str187, CSSValueZoomIn},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str188, CSSValueReplaced},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str189, CSSValueMinIntrinsic},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str190, CSSValueSeResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str191, CSSValueClip},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str192, CSSValueNResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str193, CSSValueDodgerblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str194, CSSValueDefault},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str195, CSSValueLandscape},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str196, CSSValueAll},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str197, CSSValueCrispEdges},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str198, CSSValueExact},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str199, CSSValueLeft},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str200, CSSValueNonzero},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str201, CSSValueNoChange},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str202, CSSValueCaption},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str203, CSSValueLightcoral},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str204, CSSValueOrchid},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str205, CSSValueNoRepeat},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str206, CSSValueSerif},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str207, CSSValueOldlace},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str208, CSSValueLinearRGB},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str209, CSSValueResetSize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str210, CSSValueLrTb},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str211, CSSValueDistribute},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str212, CSSValueZoomOut},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str213, CSSValueScroll},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str214, CSSValueAlternate},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str215, CSSValueSelfEnd},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str216, CSSValueScaleZ},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str217, CSSValueMonospace},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str218, CSSValueHorizontal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str219, CSSValueArmenian},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str220, CSSValueSlateblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str221, CSSValueSwResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str222, CSSValueAbsolute},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str223, CSSValueLineThrough},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str224, CSSValueAquamarine},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str225, CSSValueLightblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str226, CSSValueEaseIn},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str227, CSSValueSelfStart},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str228, CSSValueSmooth},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str229, CSSValueRlTb},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str230, CSSValueAlias},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str231, CSSValueNeResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str232, CSSValueTableCell},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str233, CSSValueElement},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str234, CSSValueSpaceAround},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str235, CSSValueAfterEdge},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str236, CSSValueClosestSide},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str237, CSSValueCapitalize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str238, CSSValueSkew},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str239, CSSValueExpanded},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str240, CSSValueNoDrop},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str241, CSSValueDarken},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str242, CSSValueRow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str243, CSSValueSansSerif},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str244, CSSValueStart},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str245, CSSValueRtl},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str246, CSSValueDarkred},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str247, CSSValueColorBurn},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str248, CSSValueMediumblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str249, CSSValueLightsalmon},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str250, CSSValueOff},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str251, CSSValueMarkers},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str252, CSSValueKatakana},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str253, CSSValueEResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str254, CSSValueEaseOut},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str255, CSSValueBefore},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str256, CSSValueHiraganaIroha},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str257, CSSValueLtr},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str258, CSSValueColorStop},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str259, CSSValueVisual},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str260, CSSValueSoftLight},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str261, CSSValueCambodian},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str262, CSSValueKhmer},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str263, CSSValueNwResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str264, CSSValueDarkgreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str265, CSSValueChartreuse},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str266, CSSValueThin},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str267, CSSValueSmallCaps},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str268, CSSValueThai},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str269, CSSValueManipulation},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str270, CSSValueSearchfield},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str271, CSSValueForwards},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str272, CSSValueMathematical},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str273, CSSValueEachLine},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str274, CSSValueAlpha},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str275, CSSValueCornsilk},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str276, CSSValueLinearGradient},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str277, CSSValueThistle},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str278, CSSValueMediaSlider},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str279, CSSValueMediumseagreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str280, CSSValueFarthestCorner},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str281, CSSValueTableColumn},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str282, CSSValueLawngreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str283, CSSValueExclusion},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str284, CSSValueViolet},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str285, CSSValueStretch},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str286, CSSValueRadialGradient},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str287, CSSValueNew},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str288, CSSValueBlanchedalmond},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str289, CSSValueStrict},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str290, CSSValueStatic},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str291, CSSValueGainsboro},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str292, CSSValueMediumpurple},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str293, CSSValueStandalone},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str294, CSSValueLightseagreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str295, CSSValueRotateX},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str296, CSSValueColorDodge},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str297, CSSValueSquareButton},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str298, CSSValueEllipse},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str299, CSSValueFixed},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str300, CSSValueHorizontalTb},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str301, CSSValueDarkorange},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str302, CSSValueLowercase},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str303, CSSValueSteps},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str304, CSSValueText},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str305, CSSValueDifference},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str306, CSSValueBreakAll},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str307, CSSValueHue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str308, CSSValueEaseInOut},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str309, CSSValueContextual},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str310, CSSValueScrollbar},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str311, CSSValueWait},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str312, CSSValueBrown},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str313, CSSValueLowerGreek},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str314, CSSValueMinmax},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str315, CSSValueAllScroll},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str316, CSSValueButt},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str317, CSSValueVertical},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str318, CSSValueEwResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str319, CSSValueEmbed},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str320, CSSValueMatrix},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str321, CSSValueMenutext},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str322, CSSValueHangulConsonant},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str323, CSSValueTextarea},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str324, CSSValueDarksalmon},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str325, CSSValueAt},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str326, CSSValueReadWrite},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str327, CSSValueTranslateX},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str328, CSSValueBelow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str329, CSSValueMediumspringgreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str330, CSSValueMatrix3d},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str331, CSSValueBrowser},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str332, CSSValueMove},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str333, CSSValuePre},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str334, CSSValueAttr},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str335, CSSValueCover},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str336, CSSValueUrl},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str337, CSSValueDarkslategrey},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str338, CSSValueBlink},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str339, CSSValueDarkslategray},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str340, CSSValueAccumulate},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str341, CSSValueStepEnd},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str342, CSSValueSilver},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str343, CSSValueBlack},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str344, CSSValueUrdu},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str345, CSSValueDestinationOut},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str346, CSSValueEllipsis},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str347, CSSValuePath},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str348, CSSValueRunning},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str349, CSSValuePaused},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str350, CSSValueFlex},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str351, CSSValueDarkblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str352, CSSValuePeru},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str353, CSSValueYellow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str354, CSSValueNowrap},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str355, CSSValueStepStart},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str356, CSSValueNoPunctuation},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str357, CSSValueHsl},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str358, CSSValueSteelblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str359, CSSValueSuper},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str360, CSSValueHotpink},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str361, CSSValueHsla},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str362, CSSValuePalegreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str363, CSSValueHover},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str364, CSSValueWrap},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str365, CSSValueFullscreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str366, CSSValueVerticalLr},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str367, CSSValueRowResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str368, CSSValuePointer},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str369, CSSValuePainted},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str370, CSSValueLavender},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str371, CSSValueDarkorchid},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str372, CSSValueOutset},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str373, CSSValuePortrait},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str374, CSSValueChocolate},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str375, CSSValueReverse},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str376, CSSValueAliceblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str377, CSSValueRepeatingRadialGradient},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str378, CSSValueDarkgoldenrod},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str379, CSSValueDoubleCircle},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str380, CSSValueButton},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str381, CSSValueLiteralPunctuation},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str382, CSSValueNotAllowed},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str383, CSSValueBackground},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str384, CSSValueLowerLatin},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str385, CSSValueVerticalRl},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str386, CSSValueVisible},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str387, CSSValueBisque},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str388, CSSValueNwseResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str389, CSSValuePadding},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str390, CSSValueHebrew},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str391, CSSValueBevel},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str392, CSSValueHorizontalBt},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str393, CSSValueWResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str394, CSSValueKoreanHangulFormal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str395, CSSValueDarkseagreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str396, CSSValueBlock},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str397, CSSValueSub},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str398, CSSValueFlexEnd},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str399, CSSValuePersian},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str400, CSSValueOn},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str401, CSSValueDarkmagenta},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str402, CSSValueStartTouch},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str403, CSSValueStatusBar},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str404, CSSValueLightpink},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str405, CSSValueFuchsia},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str406, CSSValueThreedface},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str407, CSSValueMediumslateblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str408, CSSValueRelative},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str409, CSSValueClosestCorner},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str410, CSSValueKatakanaIroha},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str411, CSSValueMultiple},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str412, CSSValueOutside},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str413, CSSValueNoHistoricalLigatures},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str414, CSSValueXor},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str415, CSSValueFlexStart},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str416, CSSValuePreLine},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str417, CSSValueExtraCondensed},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str418, CSSValuePlum},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str419, CSSValueSemiCondensed},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str420, CSSValueUpright},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str421, CSSValueScaleX},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str422, CSSValueHueRotate},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str423, CSSValueLuminance},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str424, CSSValueDevanagari},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str425, CSSValueContextMenu},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str426, CSSValueBeforeEdge},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str427, CSSValueRgb},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str428, CSSValueLowerRoman},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str429, CSSValueRepeatX},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str430, CSSValueDeeppink},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str431, CSSValueFirebrick},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str432, CSSValueMaxContent},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str433, CSSValueRgba},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str434, CSSValueProgress},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str435, CSSValueRotateY},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str436, CSSValueThreedhighlight},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str437, CSSValueGeometricPrecision},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str438, CSSValueGrey},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str439, CSSValueYellowgreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str440, CSSValueGray},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str441, CSSValueTableRow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str442, CSSValuePanRight},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str443, CSSValueRepeatingLinearGradient},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str444, CSSValueXLarge},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str445, CSSValueNoCommonLigatures},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str446, CSSValuePanUp},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str447, CSSValueScaleDown},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str448, CSSValueEnd},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str449, CSSValueAuto},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str450, CSSValueFantasy},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str451, CSSValueTradChineseFormal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str452, CSSValueOblique},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str453, CSSValueDiscretionaryLigatures},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str454, CSSValueTranslateY},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str455, CSSValueMediaSliderthumb},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str456, CSSValueXSmall},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str457, CSSValueMalayalam},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str458, CSSValueKhaki},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str459, CSSValueSaddlebrown},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str460, CSSValueAlphabetic},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str461, CSSValueGroove},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str462, CSSValueOptimizeSpeed},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str463, CSSValueDimgrey},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str464, CSSValueDimgray},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str465, CSSValueCaptiontext},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str466, CSSValueLightsteelblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str467, CSSValueCurrentcolor},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str468, CSSValueUppercase},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str469, CSSValueSourceAtop},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str470, CSSValueAlternateReverse},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str471, CSSValueTextTop},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str472, CSSValueOnDemand},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str473, CSSValueMediaEnterFullscreenButton},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str474, CSSValueXxLarge},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str475, CSSValueTextfield},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str476, CSSValueShow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str477, CSSValueIcon},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str478, CSSValueMandatory},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str479, CSSValueSliderthumbHorizontal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str480, CSSValueSourceOver},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str481, CSSValueTradChineseInformal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str482, CSSValueOver},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str483, CSSValueCopy},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str484, CSSValueStroke},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str485, CSSValuePalegoldenrod},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str486, CSSValueOlive},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str487, CSSValueSearchfieldDecoration},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str488, CSSValueListbox},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str489, CSSValueSemiExpanded},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str490, CSSValuePanLeft},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str491, CSSValueXxSmall},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str492, CSSValueMediumaquamarine},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str493, CSSValueWheat},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str494, CSSValueMistyrose},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str495, CSSValueAbove},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str496, CSSValueSlategrey},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str497, CSSValueGrayscale},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str498, CSSValueSlategray},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str499, CSSValueSkewX},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str500, CSSValueThick},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str501, CSSValueLowerAlpha},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str502, CSSValueDestinationAtop},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str503, CSSValueLightgrey},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str504, CSSValueLightgray},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str505, CSSValueCjkEarthlyBranch},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str506, CSSValueTableColumnGroup},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str507, CSSValueLegacy},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str508, CSSValueNonScalingStroke},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str509, CSSValueOpenQuote},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str510, CSSValueSliderVertical},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str511, CSSValueNsResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str512, CSSValueMediumorchid},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str513, CSSValueDarkslateblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str514, CSSValueBackwards},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str515, CSSValueWhite},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str516, CSSValueLightcyan},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str517, CSSValueBlueviolet},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str518, CSSValueOriya},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str519, CSSValueStepMiddle},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str520, CSSValueLowerArmenian},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str521, CSSValueBorderBox},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str522, CSSValueOverline},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str523, CSSValueFloralwhite},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str524, CSSValueMenulistText},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str525, CSSValueAntialiased},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str526, CSSValueSimpChineseFormal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str527, CSSValueReadOnly},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str528, CSSValueSliderthumbVertical},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str529, CSSValuePink},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str530, CSSValueMidnightblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str531, CSSValueIdeographic},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str532, CSSValueFarthestSide},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str533, CSSValueMarginBox},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str534, CSSValueScaleY},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str535, CSSValueSkyblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str536, CSSValueDropShadow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str537, CSSValueButtonface},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str538, CSSValueColumnReverse},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str539, CSSValueUpperLatin},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str540, CSSValueWebkitLeft},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str541, CSSValueLemonchiffon},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str542, CSSValueWindow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str543, CSSValueRepeatY},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str544, CSSValueTableHeaderGroup},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str545, CSSValueRoyalblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str546, CSSValueDestinationIn},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str547, CSSValueNeswResize},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str548, CSSValueThreedshadow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str549, CSSValueSearchfieldCancelButton},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str550, CSSValueOpacity},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str551, CSSValueBreakWord},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str552, CSSValueHighlighttext},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str553, CSSValueSimpChineseInformal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str554, CSSValueGhostwhite},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str555, CSSValueTextBottom},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str556, CSSValueVisibleFill},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str557, CSSValueTextAfterEdge},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str558, CSSValueSearchfieldResultsDecoration},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str559, CSSValueButtonhighlight},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str560, CSSValueKoreanHanjaFormal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str561, CSSValueGlyphs},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str562, CSSValueSnow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str563, CSSValueUpperRoman},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str564, CSSValueItalic},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str565, CSSValueEvenodd},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str566, CSSValueWebkitAuto},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str567, CSSValueGurmukhi},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str568, CSSValueWebkitRight},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str569, CSSValueSpaceBetween},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str570, CSSValueAvoid},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str571, CSSValueScrollEvent},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str572, CSSValueBoundingBox},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str573, CSSValueOlivedrab},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str574, CSSValueHistoricalLigatures},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str575, CSSValueKoreanHanjaInformal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str576, CSSValueMediaVolumeSlider},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str577, CSSValuePixelated},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str578, CSSValueSkewY},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str579, CSSValueWebkitCalc},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str580, CSSValueContentBox},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str581, CSSValueMenulistButton},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str582, CSSValuePanDown},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str583, CSSValuePanX},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str584, CSSValuePlusLighter},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str585, CSSValueInset},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str586, CSSValueThreedlightshadow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str587, CSSValueDarkviolet},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str588, CSSValueLastBaseline},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str589, CSSValueGujarati},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str590, CSSValueLightslategray},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str591, CSSValueSideways},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str592, CSSValueDarkgrey},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str593, CSSValueDarkgray},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str594, CSSValueCursive},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str595, CSSValueUnder},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str596, CSSValueLightslategrey},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str597, CSSValueUltraCondensed},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str598, CSSValueSmallCaption},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str599, CSSValueTableRowGroup},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str600, CSSValueMediumvioletred},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str601, CSSValueUnset},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str602, CSSValueSliderHorizontal},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str603, CSSValueDestinationOver},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str604, CSSValuePreWrap},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str605, CSSValueButtontext},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str606, CSSValueInline},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str607, CSSValueHoneydew},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str608, CSSValueCloseQuote},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str609, CSSValueDarkcyan},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str610, CSSValueCommonLigatures},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str611, CSSValueUpperAlpha},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str612, CSSValueInside},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str613, CSSValueIndianred},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str614, CSSValueMessageBox},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str615, CSSValuePowderblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str616, CSSValueInterlace},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str617, CSSValuePurple},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str618, CSSValueVisiblePainted},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str619, CSSValueInherit},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str620, CSSValueCjkIdeographic},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str621, CSSValueIsolate},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str622, CSSValueUpperArmenian},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str623, CSSValueProgressBar},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str624, CSSValueDarkkhaki},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str625, CSSValueUnderline},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str626, CSSValueTableCaption},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str627, CSSValueInitial},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str628, CSSValueMenulistTextfield},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str629, CSSValueButtonshadow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str630, CSSValueMediaMuteButton},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str631, CSSValueVerticalRight},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str632, CSSValuePreserve3d},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str633, CSSValueExtraExpanded},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str634, CSSValueTurquoise},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str635, CSSValueGraytext},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str636, CSSValueArabicIndic},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str637, CSSValueRowReverse},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str638, CSSValueIndigo},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str639, CSSValuePeachpuff},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str640, CSSValueWebkitControl},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str641, CSSValueWrapReverse},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str642, CSSValueWebkitMinContent},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str643, CSSValueAfterWhiteSpace},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str644, CSSValueWebkitCenter},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str645, CSSValueIntrinsic},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str646, CSSValueBlockAxis},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str647, CSSValueInlineGrid},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str648, CSSValueTableFooterGroup},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str649, CSSValueWebkitMatchParent},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str650, CSSValuePanY},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str651, CSSValueDecimalLeadingZero},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str652, CSSValueVerticalText},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str653, CSSValueRebeccapurple},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str654, CSSValueWebkitIsolate},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str655, CSSValuePalevioletred},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str656, CSSValueWebkitGrab},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str657, CSSValueAppworkspace},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str658, CSSValueNavy},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str659, CSSValueCornflowerblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str660, CSSValueGreenyellow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str661, CSSValueEconomy},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str662, CSSValueWebkitRadialGradient},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str663, CSSValueMediaControlsBackground},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str664, CSSValueNoOpenQuote},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str665, CSSValueWebkitFitContent},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str666, CSSValueMediaVolumeSliderthumb},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str667, CSSValueDarkolivegreen},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str668, CSSValueMediaVolumeSliderContainer},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str669, CSSValueAlways},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str670, CSSValueMultiply},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str671, CSSValueSandybrown},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str672, CSSValuePolygon},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str673, CSSValueRatingLevelIndicator},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str674, CSSValueVisibleStroke},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str675, CSSValueRosybrown},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str676, CSSValueWebkitCrossFade},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str677, CSSValueInternalMediaCastOffButton},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str678, CSSValueWebkitLink},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str679, CSSValueInfinite},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str680, CSSValueLightyellow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str681, CSSValueWebkitBaselineMiddle},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str682, CSSValueOptimizeLegibility},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str683, CSSValueNoContextual},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str684, CSSValueCubicBezier},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str685, CSSValueWhitesmoke},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str686, CSSValueWindowframe},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str687, CSSValueWebkitZoomIn},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str688, CSSValueWebkitPictograph},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str689, CSSValueWebkitLinearGradient},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str690, CSSValueWindowtext},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str691, CSSValueUseScript},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str692, CSSValueMediaFullscreenVolumeSlider},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str693, CSSValueInlineTable},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str694, CSSValueSpaceEvenly},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str695, CSSValueLavenderblush},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str696, CSSValueInternalExtendToZoom},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str697, CSSValueThreeddarkshadow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str698, CSSValueWebkitGrabbing},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str699, CSSValueWebkitZoomOut},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str700, CSSValueWheelEvent},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str701, CSSValueLuminosity},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str702, CSSValueMediaToggleClosedCaptionsButton},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str703, CSSValueWebkitImageSet},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str704, CSSValueWebkitMiniControl},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str705, CSSValuePushButton},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str706, CSSValueOverlay},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str707, CSSValueWebkitSmallControl},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str708, CSSValueActiveborder},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str709, CSSValueCheckbox},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str710, CSSValueCapsLockIndicator},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str711, CSSValueProgressive},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str712, CSSValueBidiOverride},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str713, CSSValueWebkitText},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str714, CSSValueWebkitPagedX},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str715, CSSValueInterWord},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str716, CSSValueNoCloseQuote},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str717, CSSValueWebkitOptimizeContrast},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str718, CSSValueEthiopicHalehame},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str719, CSSValueTextBeforeEdge},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str720, CSSValueInvert},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str721, CSSValuePerspective},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str722, CSSValueMediaCurrentTimeDisplay},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str723, CSSValueUltraExpanded},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str724, CSSValueWebkitGradient},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str725, CSSValueButtonBevel},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str726, CSSValueDeepskyblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str727, CSSValueLightgoldenrodyellow},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str728, CSSValueWebkitFocusRingColor},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str729, CSSValuePaddingBox},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str730, CSSValueWavy},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str731, CSSValueEthiopicHalehameAm},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str732, CSSValueInlineAxis},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str733, CSSValueMediaControlsFullscreenBackground},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str734, CSSValueScrollPosition},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str735, CSSValueSidewaysRight},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str736, CSSValueWebkitFlex},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str737, CSSValueWebkitNowrap},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str738, CSSValueBurlywood},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str739, CSSValueEthiopicHalehameTiEr},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str740, CSSValueEthiopicHalehameTiEt},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str741, CSSValueMediaTimeRemainingDisplay},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str742, CSSValueDarkturquoise},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str743, CSSValueMediaFullscreenVolumeSliderThumb},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str744, CSSValueJustify},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str745, CSSValueWebkitMaxContent},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str746, CSSValueWebkitCanvas},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str747, CSSValueWebkitPagedY},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str748, CSSValueWebkitActivelink},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str749, CSSValueInfotext},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str750, CSSValueInlineBlock},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str751, CSSValueAntiquewhite},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str752, CSSValueWebkitPlaintext},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str753, CSSValueWebkitBox},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str754, CSSValueWebkitBody},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str755, CSSValueProgressBarValue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str756, CSSValueWebkitInlineFlex},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str757, CSSValueMediumturquoise},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str758, CSSValueNoDiscretionaryLigatures},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str759, CSSValueActivecaption},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str760, CSSValueNavajowhite},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str761, CSSValueInlineBox},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str762, CSSValueLightskyblue},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str763, CSSValueWebkitInlineBox},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str764, CSSValueMediaPlayButton},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str765, CSSValuePapayawhip},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str766, CSSValueProximity},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str767, CSSValueIvory},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str768, CSSValuePaleturquoise},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str769, CSSValueCjkHeavenlyStem},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str770, CSSValueWebkitRepeatingLinearGradient},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str771, CSSValueWebkitRepeatingRadialGradient},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str772, CSSValueMediaExitFullscreenButton},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str773, CSSValueWebkitIsolateOverride},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str774, CSSValueInlineFlex},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str775, CSSValueSubpixelAntialiased},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str776, CSSValueWebkitFillAvailable},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str777, CSSValueRelevancyLevelIndicator},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str778, CSSValueMediaOverlayPlayButton},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str779, CSSValueReadWritePlaintextOnly},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str780, CSSValueInfobackground},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str781, CSSValueInactiveborder},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str782, CSSValueInnerSpinButton},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str783, CSSValueInternalActiveListBoxSelection},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str784, CSSValueOptimizeQuality},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str785, CSSValueContinuousCapacityLevelIndicator},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str786, CSSValueDiscreteCapacityLevelIndicator},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str787, CSSValueWebkitXxxLarge},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str788, CSSValueInternalMediaOverlayCastOffButton},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str789, CSSValueInternalInactiveListBoxSelection},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str790, CSSValueInternalActiveListBoxSelectionText},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str791, CSSValueInactivecaption},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str792, CSSValueInternalInactiveListBoxSelectionText},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str793, CSSValueInactivecaptiontext}
    };

  static const short lookup[] =
    {
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,   0,  -1,  -1,  -1,  -1,  -1,  -1,
        1,   2,   3,   4,   5,   6,   7,   8,   9,  10,
       -1,  -1,  11,  12,  -1,  -1,  13,  14,  -1,  15,
       16,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  17,  -1,
       18,  -1,  -1,  19,  -1,  -1,  20,  -1,  -1,  21,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  22,  23,  -1,
       24,  -1,  25,  -1,  26,  -1,  27,  28,  29,  -1,
       -1,  30,  31,  -1,  -1,  -1,  32,  33,  -1,  34,
       -1,  35,  36,  37,  38,  39,  -1,  40,  -1,  41,
       42,  43,  -1,  44,  45,  46,  -1,  -1,  47,  48,
       49,  50,  51,  52,  -1,  -1,  -1,  53,  54,  55,
       56,  -1,  57,  -1,  58,  59,  -1,  -1,  -1,  60,
       -1,  -1,  -1,  61,  -1,  62,  63,  64,  65,  -1,
       -1,  -1,  -1,  -1,  -1,  66,  67,  68,  -1,  69,
       -1,  70,  -1,  71,  -1,  -1,  72,  -1,  73,  -1,
       -1,  74,  -1,  75,  -1,  -1,  -1,  76,  77,  -1,
       78,  -1,  -1,  79,  80,  81,  82,  -1,  -1,  83,
       84,  -1,  85,  -1,  86,  87,  -1,  -1,  -1,  -1,
       -1,  88,  -1,  89,  90,  91,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  92,  -1,  -1,  -1,  -1,  93,
       94,  95,  -1,  96,  -1,  -1,  -1,  97,  -1,  -1,
       98,  -1,  -1,  99,  -1, 100,  -1,  -1, 101,  -1,
       -1,  -1,  -1,  -1, 102,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 103, 104,  -1,  -1,  -1, 105, 106,
       -1,  -1,  -1,  -1,  -1, 107,  -1,  -1,  -1,  -1,
       -1, 108, 109,  -1,  -1,  -1,  -1,  -1,  -1, 110,
      111,  -1, 112,  -1,  -1,  -1, 113, 114, 115,  -1,
       -1,  -1,  -1,  -1,  -1, 116,  -1, 117, 118,  -1,
       -1, 119, 120,  -1, 121,  -1, 122, 123,  -1,  -1,
       -1,  -1, 124,  -1,  -1,  -1, 125, 126,  -1,  -1,
      127,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 128, 129,  -1, 130,  -1,
       -1, 131,  -1,  -1,  -1,  -1,  -1, 132,  -1, 133,
       -1,  -1,  -1,  -1, 134,  -1, 135, 136,  -1,  -1,
      137, 138, 139,  -1, 140,  -1, 141, 142,  -1,  -1,
      143, 144,  -1,  -1, 145, 146,  -1, 147,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 148,  -1, 149,
      150,  -1,  -1,  -1, 151,  -1, 152,  -1,  -1, 153,
      154,  -1,  -1, 155, 156, 157,  -1,  -1,  -1, 158,
      159, 160,  -1,  -1,  -1, 161,  -1, 162,  -1,  -1,
       -1,  -1,  -1,  -1, 163, 164, 165, 166,  -1,  -1,
       -1, 167,  -1, 168,  -1,  -1,  -1,  -1, 169,  -1,
       -1, 170,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 171,  -1,  -1, 172, 173,  -1,  -1,  -1,
      174,  -1,  -1,  -1, 175, 176,  -1,  -1,  -1,  -1,
      177,  -1,  -1, 178,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 179, 180,  -1, 181, 182, 183, 184,  -1,
       -1, 185, 186,  -1, 187,  -1,  -1,  -1,  -1, 188,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 189,  -1,  -1,
       -1,  -1,  -1, 190,  -1, 191,  -1,  -1, 192,  -1,
      193,  -1, 194,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 195, 196,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      197, 198, 199,  -1, 200,  -1,  -1, 201,  -1,  -1,
      202,  -1,  -1,  -1,  -1,  -1, 203,  -1,  -1,  -1,
      204,  -1,  -1,  -1,  -1,  -1, 205, 206, 207, 208,
       -1, 209,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      210,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 211, 212,
       -1,  -1,  -1,  -1,  -1,  -1, 213, 214,  -1,  -1,
       -1,  -1, 215, 216,  -1,  -1,  -1, 217,  -1,  -1,
       -1,  -1,  -1, 218,  -1,  -1,  -1,  -1, 219,  -1,
       -1, 220,  -1,  -1, 221,  -1,  -1,  -1,  -1,  -1,
      222, 223,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 224,  -1,  -1,  -1,  -1, 225, 226, 227,
       -1,  -1,  -1,  -1,  -1,  -1, 228,  -1,  -1,  -1,
       -1,  -1,  -1, 229,  -1,  -1,  -1,  -1, 230, 231,
       -1,  -1,  -1,  -1,  -1,  -1, 232,  -1, 233, 234,
       -1, 235,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 236,
      237,  -1, 238,  -1,  -1,  -1,  -1,  -1, 239, 240,
       -1, 241, 242,  -1, 243, 244,  -1, 245,  -1,  -1,
       -1,  -1, 246,  -1,  -1, 247, 248,  -1,  -1, 249,
       -1, 250,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 251, 252, 253,  -1,  -1,  -1,
       -1,  -1,  -1, 254,  -1,  -1,  -1,  -1, 255,  -1,
       -1,  -1,  -1, 256,  -1,  -1,  -1, 257,  -1,  -1,
      258,  -1,  -1,  -1,  -1, 259,  -1, 260,  -1,  -1,
      261, 262,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      263,  -1,  -1, 264,  -1,  -1,  -1,  -1,  -1, 265,
       -1, 266, 267, 268,  -1, 269,  -1,  -1,  -1,  -1,
      270,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 271,  -1,
       -1, 272, 273,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 274,  -1,  -1,  -1,
       -1,  -1, 275,  -1,  -1,  -1,  -1, 276,  -1,  -1,
       -1,  -1, 277,  -1,  -1,  -1, 278,  -1,  -1,  -1,
       -1,  -1, 279,  -1,  -1, 280,  -1,  -1,  -1,  -1,
      281,  -1,  -1,  -1, 282,  -1,  -1,  -1, 283, 284,
       -1,  -1,  -1,  -1,  -1,  -1, 285,  -1, 286,  -1,
       -1, 287,  -1, 288,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 289,  -1,  -1, 290,  -1,  -1,
       -1, 291,  -1, 292, 293,  -1, 294,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 295, 296, 297,  -1,  -1,  -1, 298,
      299, 300,  -1, 301, 302,  -1, 303,  -1, 304,  -1,
      305, 306,  -1,  -1,  -1,  -1, 307,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 308, 309, 310, 311,
       -1,  -1,  -1,  -1,  -1, 312, 313,  -1,  -1,  -1,
      314,  -1, 315, 316,  -1, 317,  -1,  -1, 318, 319,
      320,  -1,  -1, 321, 322, 323,  -1,  -1,  -1, 324,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 325,  -1, 326,
      327,  -1,  -1,  -1, 328,  -1,  -1, 329, 330, 331,
       -1, 332, 333,  -1,  -1,  -1,  -1,  -1,  -1, 334,
       -1,  -1, 335,  -1, 336,  -1, 337, 338, 339,  -1,
       -1,  -1,  -1, 340,  -1,  -1, 341,  -1,  -1,  -1,
       -1,  -1, 342, 343,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 344,  -1,  -1,  -1, 345, 346,  -1,  -1,  -1,
      347,  -1,  -1, 348,  -1,  -1,  -1,  -1,  -1,  -1,
      349, 350,  -1,  -1, 351,  -1, 352, 353,  -1,  -1,
      354,  -1,  -1, 355,  -1,  -1,  -1,  -1,  -1, 356,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 357,  -1,
       -1,  -1, 358,  -1,  -1, 359,  -1,  -1, 360,  -1,
       -1, 361,  -1, 362, 363, 364,  -1, 365,  -1,  -1,
       -1,  -1,  -1,  -1, 366,  -1,  -1,  -1, 367, 368,
       -1,  -1,  -1, 369,  -1, 370, 371,  -1,  -1,  -1,
       -1, 372, 373, 374, 375, 376, 377,  -1,  -1,  -1,
      378, 379, 380,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      381, 382,  -1,  -1,  -1,  -1, 383,  -1,  -1,  -1,
      384,  -1,  -1,  -1, 385,  -1, 386,  -1,  -1, 387,
      388,  -1,  -1, 389,  -1,  -1,  -1, 390, 391,  -1,
      392, 393,  -1,  -1, 394,  -1, 395,  -1, 396,  -1,
       -1,  -1, 397,  -1,  -1, 398,  -1,  -1, 399,  -1,
       -1,  -1, 400,  -1,  -1, 401,  -1, 402,  -1,  -1,
       -1, 403,  -1,  -1, 404,  -1, 405,  -1, 406,  -1,
      407,  -1,  -1, 408,  -1,  -1, 409,  -1, 410,  -1,
      411,  -1,  -1,  -1,  -1, 412,  -1,  -1,  -1, 413,
       -1, 414, 415, 416, 417,  -1,  -1,  -1, 418,  -1,
      419,  -1,  -1,  -1,  -1, 420,  -1,  -1, 421,  -1,
       -1,  -1, 422,  -1,  -1,  -1,  -1, 423,  -1,  -1,
      424,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 425, 426,  -1,
       -1,  -1,  -1,  -1, 427,  -1, 428, 429,  -1,  -1,
      430,  -1,  -1,  -1, 431, 432,  -1, 433,  -1, 434,
       -1,  -1,  -1, 435,  -1,  -1,  -1,  -1,  -1,  -1,
      436, 437,  -1,  -1,  -1,  -1,  -1, 438, 439, 440,
       -1,  -1,  -1, 441,  -1,  -1, 442,  -1,  -1,  -1,
       -1, 443,  -1,  -1,  -1, 444,  -1,  -1,  -1, 445,
       -1,  -1,  -1,  -1,  -1, 446, 447,  -1,  -1, 448,
      449,  -1,  -1,  -1, 450,  -1, 451,  -1,  -1,  -1,
       -1,  -1,  -1, 452,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 453,
      454,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 455,
       -1, 456, 457,  -1, 458,  -1,  -1, 459,  -1,  -1,
      460, 461,  -1,  -1, 462,  -1,  -1,  -1, 463,  -1,
      464,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 465,  -1,  -1,  -1, 466, 467,
       -1, 468,  -1,  -1,  -1, 469, 470,  -1,  -1, 471,
      472,  -1, 473, 474,  -1,  -1, 475,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 476,  -1, 477,  -1,  -1,  -1,
      478,  -1, 479, 480,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 481,  -1, 482, 483, 484,  -1,  -1,  -1,  -1,
      485,  -1,  -1, 486,  -1,  -1, 487,  -1, 488,  -1,
      489,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 490, 491,
       -1,  -1, 492, 493,  -1,  -1, 494,  -1, 495, 496,
      497, 498,  -1, 499, 500,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 501, 502,  -1,  -1, 503,  -1, 504,  -1,  -1,
       -1,  -1,  -1,  -1, 505,  -1,  -1,  -1, 506,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 507,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 508, 509,
       -1, 510,  -1, 511,  -1, 512,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 513,  -1,
       -1, 514,  -1,  -1, 515, 516, 517,  -1,  -1, 518,
      519,  -1,  -1, 520,  -1,  -1,  -1,  -1,  -1, 521,
       -1,  -1,  -1,  -1, 522,  -1,  -1,  -1,  -1,  -1,
      523, 524,  -1,  -1,  -1,  -1,  -1, 525, 526, 527,
       -1,  -1, 528, 529,  -1, 530,  -1, 531,  -1,  -1,
      532,  -1,  -1,  -1,  -1,  -1,  -1, 533, 534, 535,
       -1,  -1,  -1, 536,  -1,  -1,  -1,  -1,  -1,  -1,
      537,  -1,  -1,  -1,  -1,  -1, 538, 539,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 540,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 541,  -1, 542,  -1, 543,  -1,  -1,
       -1,  -1, 544,  -1,  -1,  -1,  -1,  -1, 545,  -1,
       -1,  -1,  -1,  -1, 546,  -1, 547,  -1,  -1,  -1,
       -1,  -1,  -1, 548,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 549, 550,  -1,  -1, 551,  -1,  -1,  -1,
       -1, 552,  -1, 553,  -1, 554,  -1,  -1, 555,  -1,
      556,  -1, 557,  -1,  -1,  -1,  -1, 558,  -1,  -1,
       -1,  -1, 559,  -1,  -1,  -1,  -1, 560,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 561,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 562,
       -1,  -1,  -1, 563,  -1,  -1,  -1,  -1,  -1, 564,
       -1, 565,  -1,  -1,  -1,  -1,  -1, 566,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      567,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      568,  -1,  -1, 569, 570,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 571, 572,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 573, 574,  -1,
       -1,  -1, 575,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 576,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 577,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 578, 579,  -1,  -1, 580,  -1,  -1,
       -1, 581,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 582,  -1, 583, 584,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 585,
       -1,  -1,  -1,  -1, 586,  -1,  -1,  -1, 587,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 588,  -1, 589,
       -1,  -1, 590,  -1,  -1,  -1, 591,  -1,  -1,  -1,
       -1,  -1, 592,  -1, 593,  -1,  -1,  -1,  -1,  -1,
       -1, 594,  -1, 595,  -1,  -1,  -1,  -1,  -1, 596,
       -1,  -1,  -1, 597,  -1,  -1,  -1, 598,  -1,  -1,
      599,  -1, 600,  -1,  -1,  -1,  -1, 601,  -1, 602,
       -1,  -1,  -1, 603, 604,  -1,  -1,  -1, 605,  -1,
       -1,  -1,  -1, 606,  -1,  -1,  -1,  -1, 607,  -1,
      608,  -1, 609,  -1, 610,  -1,  -1,  -1, 611,  -1,
       -1,  -1,  -1, 612,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 613,  -1, 614,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 615,  -1,  -1,
       -1,  -1, 616,  -1,  -1,  -1,  -1,  -1,  -1, 617,
       -1,  -1, 618,  -1, 619,  -1,  -1,  -1,  -1, 620,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 621,
      622,  -1,  -1,  -1,  -1,  -1,  -1, 623,  -1,  -1,
       -1,  -1,  -1, 624, 625,  -1,  -1,  -1, 626,  -1,
      627,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 628,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 629,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 630,  -1,  -1,  -1,
       -1,  -1,  -1, 631,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 632,  -1,  -1, 633,  -1, 634,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 635,  -1,  -1,  -1, 636,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 637,  -1,  -1,  -1, 638,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 639,  -1,  -1,  -1, 640,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 641,
       -1,  -1,  -1,  -1, 642,  -1, 643,  -1,  -1,  -1,
       -1, 644,  -1,  -1,  -1, 645,  -1,  -1,  -1,  -1,
      646,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 647,
       -1,  -1,  -1,  -1,  -1, 648,  -1,  -1, 649,  -1,
       -1,  -1,  -1,  -1,  -1, 650,  -1,  -1,  -1,  -1,
      651,  -1,  -1,  -1, 652,  -1,  -1,  -1,  -1,  -1,
       -1, 653, 654,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 655, 656,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 657,  -1,  -1,
      658,  -1,  -1,  -1, 659,  -1,  -1,  -1,  -1, 660,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 661,
       -1,  -1, 662,  -1,  -1,  -1,  -1, 663,  -1, 664,
       -1,  -1,  -1, 665,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 666, 667,
       -1, 668,  -1, 669,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      670,  -1,  -1,  -1, 671,  -1,  -1, 672,  -1,  -1,
      673,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 674,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 675,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 676,  -1, 677,  -1,  -1,  -1, 678,
      679,  -1,  -1, 680,  -1,  -1,  -1,  -1,  -1, 681,
      682,  -1, 683,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      684,  -1, 685,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 686,
       -1,  -1,  -1, 687,  -1,  -1, 688, 689,  -1,  -1,
       -1, 690,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 691,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 692,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 693,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      694,  -1,  -1,  -1,  -1,  -1, 695,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 696,  -1,
       -1,  -1,  -1,  -1,  -1, 697,  -1,  -1,  -1, 698,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 699,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 700,  -1,  -1,  -1,
       -1,  -1,  -1, 701,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 702,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 703,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 704,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      705,  -1, 706,  -1, 707,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 708,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 709,  -1,  -1, 710,  -1,  -1,  -1, 711,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 712,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 713,  -1,  -1,  -1,  -1,  -1, 714,  -1,
       -1,  -1,  -1, 715,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 716,  -1,  -1,  -1,  -1,  -1, 717,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 718, 719,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 720,  -1,  -1,  -1,  -1,  -1,
       -1, 721,  -1,  -1,  -1,  -1, 722,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 723,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 724,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 725,  -1,  -1,  -1,  -1,  -1,  -1, 726,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 727,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 728,
       -1, 729,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 730,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 731,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 732,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 733,  -1,
       -1,  -1, 734,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 735,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 736,  -1, 737,  -1,  -1,  -1,
       -1, 738,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 739,  -1, 740,  -1, 741,  -1,
       -1, 742,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 743,  -1,  -1, 744,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 745, 746, 747,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 748,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 749, 750,  -1,  -1, 751,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 752,
       -1,  -1,  -1,  -1, 753,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 754,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 755,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 756,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 757,  -1,  -1,
      758,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 759,  -1,  -1,  -1, 760,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 761,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 762,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 763,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 764,  -1,  -1,  -1,  -1, 765,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      766,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 767,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 768,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 769,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
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
       -1,  -1,  -1,  -1, 770,  -1,  -1, 771,  -1,  -1,
       -1,  -1,  -1, 772,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 773,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 774,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      775,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      776,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 777,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 778,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 779,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 780,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 781,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 782,  -1,  -1,  -1,  -1,  -1,  -1,
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
       -1,  -1,  -1,  -1, 783,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      784,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
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
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 785,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 786,  -1,  -1,
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
       -1,  -1,  -1, 787,  -1,  -1,  -1,  -1,  -1,  -1,
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
       -1,  -1, 788,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
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
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 789,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      790,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 791,  -1,  -1,  -1,  -1,  -1,  -1,
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
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 792,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 793
    };

    if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH) {
        register int key = value_hash_function(str, len);

        if (key <= MAX_HASH_VALUE && key >= 0) {
            register int index = lookup[key];

            if (index >= 0) {
                register const char *s = value_word_list[index].nameOffset + stringpool;

                if (*str == *s && !strncmp(str + 1, s + 1, len - 1) && s[len] == '\0')
                    return &value_word_list[index];
            }
        }
    }
    if (0 == strncmp(str, "fit-content", len)) {
        static Value fitContentStatic = { 0, CSSValueWebkitFitContent };
        return &fitContentStatic;
    } else if (0 == strncmp(str, "var", len)) {
		static Value varStatic = { 0, CSSValueVar };
		return &varStatic;
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
    if (CSSValueFitContent == id)
        return "fit-content";
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
            return isUASheetBehavior(mode);
        case CSSValueWebkitFocusRingColor:
    case CSSValueWebkitText:
            return isUASheetBehavior(mode) || isQuirksModeBehavior(mode);
        default:
            return true;
    }
}

} // namespace blink
