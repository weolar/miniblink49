WebInspector.InspectElementModeController=function()
{this._toggleSearchAction=WebInspector.actionRegistry.action("elements.toggle-element-search");if(Runtime.experiments.isEnabled("layoutEditor")){this._layoutEditorButton=new WebInspector.ToolbarButton(WebInspector.UIString("Toggle Layout Editor"),"layout-editor-toolbar-item");this._layoutEditorButton.addEventListener("click",this._toggleLayoutEditor,this);}
this._mode=DOMAgent.InspectMode.None;WebInspector.targetManager.addEventListener(WebInspector.TargetManager.Events.SuspendStateChanged,this._suspendStateChanged,this);WebInspector.targetManager.observeTargets(this,WebInspector.Target.Type.Page);}
WebInspector.InspectElementModeController.prototype={targetAdded:function(target)
{if(this._mode===DOMAgent.InspectMode.None)
return;var domModel=WebInspector.DOMModel.fromTarget(target);domModel.setInspectMode(this._mode);},targetRemoved:function(target)
{},isInInspectElementMode:function()
{return this._mode===DOMAgent.InspectMode.SearchForNode||this._mode===DOMAgent.InspectMode.SearchForUAShadowDOM;},isInLayoutEditorMode:function()
{return this._mode===DOMAgent.InspectMode.ShowLayoutEditor;},stopInspection:function()
{if(this._mode&&this._mode!==DOMAgent.InspectMode.None)
this._toggleInspectMode();},_toggleLayoutEditor:function()
{var mode=this.isInLayoutEditorMode()?DOMAgent.InspectMode.None:DOMAgent.InspectMode.ShowLayoutEditor;this._setMode(mode);},_toggleInspectMode:function()
{if(WebInspector.targetManager.allTargetsSuspended())
return;var mode;if(this.isInInspectElementMode())
mode=DOMAgent.InspectMode.None;else
mode=WebInspector.moduleSetting("showUAShadowDOM").get()?DOMAgent.InspectMode.SearchForUAShadowDOM:DOMAgent.InspectMode.SearchForNode;this._setMode(mode);},_setMode:function(mode)
{this._mode=mode;for(var domModel of WebInspector.DOMModel.instances())
domModel.setInspectMode(mode);if(this._layoutEditorButton){this._layoutEditorButton.setEnabled(!this.isInInspectElementMode());this._layoutEditorButton.setToggled(this.isInLayoutEditorMode());}
this._toggleSearchAction.setEnabled(!this.isInLayoutEditorMode());this._toggleSearchAction.setToggled(this.isInInspectElementMode());},_suspendStateChanged:function()
{if(!WebInspector.targetManager.allTargetsSuspended())
return;this._mode=DOMAgent.InspectMode.None;this._toggleSearchAction.setToggled(false);if(this._layoutEditorButton)
this._layoutEditorButton.setToggled(false);}}
WebInspector.InspectElementModeController.ToggleSearchActionDelegate=function()
{}
WebInspector.InspectElementModeController.ToggleSearchActionDelegate.prototype={handleAction:function(context,actionId)
{if(!WebInspector.inspectElementModeController)
return false;WebInspector.inspectElementModeController._toggleInspectMode();return true;}}
WebInspector.InspectElementModeController.LayoutEditorButtonProvider=function()
{}
WebInspector.InspectElementModeController.LayoutEditorButtonProvider.prototype={item:function()
{if(!WebInspector.inspectElementModeController)
return null;return WebInspector.inspectElementModeController._layoutEditorButton;}}
WebInspector.inspectElementModeController=Runtime.queryParam("isSharedWorker")?null:new WebInspector.InspectElementModeController();;WebInspector.BezierUI=function(width,height,marginTop,controlPointRadius,linearLine)
{this.width=width;this.height=height;this.marginTop=marginTop;this.radius=controlPointRadius;this.linearLine=linearLine;}
WebInspector.BezierUI.prototype={curveWidth:function()
{return this.width-this.radius*2;},curveHeight:function()
{return this.height-this.radius*2-this.marginTop*2;},_drawLine:function(parentElement,className,x1,y1,x2,y2)
{var line=parentElement.createSVGChild("line",className);line.setAttribute("x1",x1+this.radius);line.setAttribute("y1",y1+this.radius+this.marginTop);line.setAttribute("x2",x2+this.radius);line.setAttribute("y2",y2+this.radius+this.marginTop);},_drawControlPoints:function(parentElement,startX,startY,controlX,controlY)
{this._drawLine(parentElement,"bezier-control-line",startX,startY,controlX,controlY);var circle=parentElement.createSVGChild("circle","bezier-control-circle");circle.setAttribute("cx",controlX+this.radius);circle.setAttribute("cy",controlY+this.radius+this.marginTop);circle.setAttribute("r",this.radius);},drawCurve:function(bezier,svg)
{if(!bezier)
return;var width=this.curveWidth();var height=this.curveHeight();svg.setAttribute("width",this.width);svg.setAttribute("height",this.height);svg.removeChildren();var group=svg.createSVGChild("g");if(this.linearLine)
this._drawLine(group,"linear-line",0,height,width,0);var curve=group.createSVGChild("path","bezier-path");var curvePoints=[new WebInspector.Geometry.Point(bezier.controlPoints[0].x*width+this.radius,(1-bezier.controlPoints[0].y)*height+this.radius+this.marginTop),new WebInspector.Geometry.Point(bezier.controlPoints[1].x*width+this.radius,(1-bezier.controlPoints[1].y)*height+this.radius+this.marginTop),new WebInspector.Geometry.Point(width+this.radius,this.marginTop+this.radius)];curve.setAttribute("d","M"+this.radius+","+(height+this.radius+this.marginTop)+" C"+curvePoints.join(" "));this._drawControlPoints(group,0,height,bezier.controlPoints[0].x*width,(1-bezier.controlPoints[0].y)*height);this._drawControlPoints(group,width,0,bezier.controlPoints[1].x*width,(1-bezier.controlPoints[1].y)*height);}}
WebInspector.BezierUI.Height=26;WebInspector.BezierUI.drawVelocityChart=function(bezier,path,width)
{var height=WebInspector.BezierUI.Height;var pathBuilder=["M",0,height];const sampleSize=1/40;var prev=bezier.evaluateAt(0);for(var t=sampleSize;t<1+sampleSize;t+=sampleSize){var current=bezier.evaluateAt(t);var slope=(current.y-prev.y)/(current.x-prev.x);var weightedX=prev.x*(1-t)+current.x*t;slope=Math.tanh(slope/1.5);pathBuilder=pathBuilder.concat(["L",(weightedX*width).toFixed(2),(height-slope*height).toFixed(2)]);prev=current;}
pathBuilder=pathBuilder.concat(["L",width.toFixed(2),height,"Z"]);path.setAttribute("d",pathBuilder.join(" "));};WebInspector.StylesPopoverHelper=function()
{this._popover=new WebInspector.Popover();this._popover.setCanShrink(false);this._popover.setNoMargins(true);this._popover.element.addEventListener("mousedown",consumeEvent,false);this._hideProxy=this.hide.bind(this,true);this._boundOnKeyDown=this._onKeyDown.bind(this);this._repositionBound=this.reposition.bind(this);this._boundFocusOut=this._onFocusOut.bind(this);}
WebInspector.StylesPopoverHelper.prototype={_onFocusOut:function(event)
{if(!event.relatedTarget||event.relatedTarget.isSelfOrDescendant(this._view.contentElement))
return;this._hideProxy();},isShowing:function()
{return this._popover.isShowing();},show:function(view,anchorElement,hiddenCallback)
{if(this._popover.isShowing()){if(this._anchorElement===anchorElement)
return;this.hide(true);}
delete this._isHidden;this._anchorElement=anchorElement;this._view=view;this._hiddenCallback=hiddenCallback;this.reposition();var document=this._popover.element.ownerDocument;document.addEventListener("mousedown",this._hideProxy,false);document.defaultView.addEventListener("resize",this._hideProxy,false);this._view.contentElement.addEventListener("keydown",this._boundOnKeyDown,false);this._scrollerElement=anchorElement.enclosingNodeOrSelfWithClass("style-panes-wrapper");if(this._scrollerElement)
this._scrollerElement.addEventListener("scroll",this._repositionBound,false);},reposition:function(event)
{if(!this._previousFocusElement)
this._previousFocusElement=WebInspector.currentFocusElement();this._view.contentElement.removeEventListener("focusout",this._boundFocusOut,false);this._popover.showView(this._view,this._anchorElement);this._view.contentElement.addEventListener("focusout",this._boundFocusOut,false);WebInspector.setCurrentFocusElement(this._view.contentElement);},hide:function(commitEdit)
{if(this._isHidden)
return;var document=this._popover.element.ownerDocument;this._isHidden=true;this._popover.hide();if(this._scrollerElement)
this._scrollerElement.removeEventListener("scroll",this._repositionBound,false);document.removeEventListener("mousedown",this._hideProxy,false);document.defaultView.removeEventListener("resize",this._hideProxy,false);if(this._hiddenCallback)
this._hiddenCallback.call(null,!!commitEdit);WebInspector.setCurrentFocusElement(this._previousFocusElement);delete this._previousFocusElement;delete this._anchorElement;if(this._view){this._view.detach();this._view.contentElement.removeEventListener("keydown",this._boundOnKeyDown,false);this._view.contentElement.removeEventListener("focusout",this._boundFocusOut,false);delete this._view;}},_onKeyDown:function(event)
{if(event.keyIdentifier==="Enter"){this.hide(true);event.consume(true);return;}
if(event.keyIdentifier==="U+001B"){this.hide(false);event.consume(true);}},__proto__:WebInspector.Object.prototype}
WebInspector.BezierPopoverIcon=function(treeElement,stylesPopoverHelper,text)
{this._treeElement=treeElement;this._stylesPopoverHelper=stylesPopoverHelper;this._createDOM(text);this._boundBezierChanged=this._bezierChanged.bind(this);}
WebInspector.BezierPopoverIcon.prototype={element:function()
{return this._element;},_createDOM:function(text)
{this._element=createElement("nobr");this._element.title=WebInspector.UIString("Open cubic bezier editor");this._iconElement=this._element.createSVGChild("svg","popover-icon bezier-icon");this._iconElement.setAttribute("height",10);this._iconElement.setAttribute("width",10);this._iconElement.addEventListener("click",this._iconClick.bind(this),false);var g=this._iconElement.createSVGChild("g");var path=g.createSVGChild("path");path.setAttribute("d","M2,8 C2,3 8,7 8,2");this._bezierValueElement=this._element.createChild("span");this._bezierValueElement.textContent=text;},_iconClick:function(event)
{event.consume(true);if(this._stylesPopoverHelper.isShowing()){this._stylesPopoverHelper.hide(true);return;}
this._bezierEditor=new WebInspector.BezierEditor();var geometry=WebInspector.Geometry.CubicBezier.parse(this._bezierValueElement.textContent);this._bezierEditor.setBezier(geometry);this._bezierEditor.addEventListener(WebInspector.BezierEditor.Events.BezierChanged,this._boundBezierChanged);this._stylesPopoverHelper.show(this._bezierEditor,this._iconElement,this._onPopoverHidden.bind(this));this._originalPropertyText=this._treeElement.property.propertyText;this._treeElement.parentPane().setEditingStyle(true);},_bezierChanged:function(event)
{this._bezierValueElement.textContent=(event.data);this._treeElement.applyStyleText(this._treeElement.renderedPropertyText(),false);},_onPopoverHidden:function(commitEdit)
{this._bezierEditor.removeEventListener(WebInspector.BezierEditor.Events.BezierChanged,this._boundBezierChanged);delete this._bezierEditor;var propertyText=commitEdit?this._treeElement.renderedPropertyText():this._originalPropertyText;this._treeElement.applyStyleText(propertyText,true);this._treeElement.parentPane().setEditingStyle(false);delete this._originalPropertyText;}}
WebInspector.ColorSwatchPopoverIcon=function(treeElement,stylesPopoverHelper,colorText)
{this._treeElement=treeElement;this._treeElement[WebInspector.ColorSwatchPopoverIcon._treeElementSymbol]=this;this._stylesPopoverHelper=stylesPopoverHelper;this._swatch=WebInspector.ColorSwatch.create();this._swatch.setColorText(colorText);this._swatch.setFormat(WebInspector.ColorSwatchPopoverIcon._colorFormat(this._swatch.color()));var shiftClickMessage=WebInspector.UIString("Shift + Click to change color format.");this._swatch.iconElement().title=WebInspector.UIString("Open color picker. %s",shiftClickMessage);this._swatch.iconElement().addEventListener("click",this._iconClick.bind(this));this._contrastColor=null;this._boundSpectrumChanged=this._spectrumChanged.bind(this);}
WebInspector.ColorSwatchPopoverIcon._treeElementSymbol=Symbol("WebInspector.ColorSwatchPopoverIcon._treeElementSymbol");WebInspector.ColorSwatchPopoverIcon.forTreeElement=function(treeElement)
{return treeElement[WebInspector.ColorSwatchPopoverIcon._treeElementSymbol]||null;}
WebInspector.ColorSwatchPopoverIcon._colorFormat=function(color)
{const cf=WebInspector.Color.Format;var format;var formatSetting=WebInspector.moduleSetting("colorFormat").get();if(formatSetting===cf.Original)
format=cf.Original;else if(formatSetting===cf.RGB)
format=(color.hasAlpha()?cf.RGBA:cf.RGB);else if(formatSetting===cf.HSL)
format=(color.hasAlpha()?cf.HSLA:cf.HSL);else if(!color.hasAlpha())
format=(color.canBeShortHex()?cf.ShortHEX:cf.HEX);else
format=cf.RGBA;return format;}
WebInspector.ColorSwatchPopoverIcon.prototype={element:function()
{return this._swatch;},setContrastColor:function(color)
{this._contrastColor=color;if(this._spectrum)
this._spectrum.setContrastColor(this._contrastColor);},_iconClick:function(event)
{event.consume(true);this.showPopover();},showPopover:function()
{if(this._stylesPopoverHelper.isShowing()){this._stylesPopoverHelper.hide(true);return;}
var color=this._swatch.color();var format=this._swatch.format();if(format===WebInspector.Color.Format.Original)
format=color.format();this._spectrum=new WebInspector.Spectrum();this._spectrum.setColor(color,format);if(this._contrastColor)
this._spectrum.setContrastColor(this._contrastColor);this._spectrum.addEventListener(WebInspector.Spectrum.Events.SizeChanged,this._spectrumResized,this);this._spectrum.addEventListener(WebInspector.Spectrum.Events.ColorChanged,this._boundSpectrumChanged);this._stylesPopoverHelper.show(this._spectrum,this._swatch.iconElement(),this._onPopoverHidden.bind(this));this._originalPropertyText=this._treeElement.property.propertyText;this._treeElement.parentPane().setEditingStyle(true);},_spectrumResized:function(event)
{this._stylesPopoverHelper.reposition();},_spectrumChanged:function(event)
{var colorString=(event.data);this._swatch.setColorText(colorString);this._treeElement.applyStyleText(this._treeElement.renderedPropertyText(),false);},_onPopoverHidden:function(commitEdit)
{this._spectrum.removeEventListener(WebInspector.Spectrum.Events.ColorChanged,this._boundSpectrumChanged);delete this._spectrum;var propertyText=commitEdit?this._treeElement.renderedPropertyText():this._originalPropertyText;this._treeElement.applyStyleText(propertyText,true);this._treeElement.parentPane().setEditingStyle(false);delete this._originalPropertyText;}};WebInspector.BezierEditor=function()
{WebInspector.VBox.call(this,true);this.registerRequiredCSS("elements/bezierEditor.css");this.contentElement.tabIndex=0;this._previewElement=this.contentElement.createChild("div","bezier-preview-container");this._previewElement.createChild("div","bezier-preview-animation");this._previewElement.addEventListener("click",this._startPreviewAnimation.bind(this));this._previewOnion=this.contentElement.createChild("div","bezier-preview-onion");this._previewOnion.addEventListener("click",this._startPreviewAnimation.bind(this));this._outerContainer=this.contentElement.createChild("div","bezier-container");this._presetsContainer=this._outerContainer.createChild("div","bezier-presets");this._presetUI=new WebInspector.BezierUI(40,40,0,2,false);this._presetCategories=[];for(var i=0;i<WebInspector.BezierEditor.Presets.length;i++){this._presetCategories[i]=this._createCategory(WebInspector.BezierEditor.Presets[i]);this._presetsContainer.appendChild(this._presetCategories[i].icon);}
this._curveUI=new WebInspector.BezierUI(150,250,50,7,true);this._curve=this._outerContainer.createSVGChild("svg","bezier-curve");WebInspector.installDragHandle(this._curve,this._dragStart.bind(this),this._dragMove.bind(this),this._dragEnd.bind(this),"default");this._header=this.contentElement.createChild("div","bezier-header");var minus=this._createPresetModifyIcon(this._header,"bezier-preset-minus","M 12 6 L 8 10 L 12 14");var plus=this._createPresetModifyIcon(this._header,"bezier-preset-plus","M 8 6 L 12 10 L 8 14");minus.addEventListener("click",this._presetModifyClicked.bind(this,false));plus.addEventListener("click",this._presetModifyClicked.bind(this,true));this._label=this._header.createChild("span","source-code bezier-display-value");}
WebInspector.BezierEditor.Events={BezierChanged:"BezierChanged"}
WebInspector.BezierEditor.Presets=[[{name:"ease-in-out",value:"ease-in-out"},{name:"In Out · Sine",value:"cubic-bezier(0.45, 0.05, 0.55, 0.95)"},{name:"In Out · Quadratic",value:"cubic-bezier(0.46, 0.03, 0.52, 0.96)"},{name:"In Out · Cubic",value:"cubic-bezier(0.65, 0.05, 0.36, 1)"},{name:"Fast Out, Slow In",value:"cubic-bezier(0.4, 0, 0.2, 1)"},{name:"In Out · Back",value:"cubic-bezier(0.68, -0.55, 0.27, 1.55)"}],[{name:"Fast Out, Linear In",value:"cubic-bezier(0.4, 0, 1, 1)"},{name:"ease-in",value:"ease-in"},{name:"In · Sine",value:"cubic-bezier(0.47, 0, 0.75, 0.72)"},{name:"In · Quadratic",value:"cubic-bezier(0.55, 0.09, 0.68, 0.53)"},{name:"In · Cubic",value:"cubic-bezier(0.55, 0.06, 0.68, 0.19)"},{name:"In · Back",value:"cubic-bezier(0.6, -0.28, 0.74, 0.05)"}],[{name:"ease-out",value:"ease-out"},{name:"Out · Sine",value:"cubic-bezier(0.39, 0.58, 0.57, 1)"},{name:"Out · Quadratic",value:"cubic-bezier(0.25, 0.46, 0.45, 0.94)"},{name:"Out · Cubic",value:"cubic-bezier(0.22, 0.61, 0.36, 1)"},{name:"Linear Out, Slow In",value:"cubic-bezier(0, 0, 0.2, 1)"},{name:"Out · Back",value:"cubic-bezier(0.18, 0.89, 0.32, 1.28)"}]]
WebInspector.BezierEditor.PresetCategory;WebInspector.BezierEditor.prototype={setBezier:function(bezier)
{if(!bezier)
return;this._bezier=bezier;this._updateUI();},bezier:function()
{return this._bezier;},wasShown:function()
{this._unselectPresets();for(var category of this._presetCategories){for(var i=0;i<category.presets.length;i++){if(this._bezier.asCSSText()===category.presets[i].value){category.presetIndex=i;this._presetCategorySelected(category);}}}
this._updateUI();this._startPreviewAnimation();},_onchange:function()
{this._updateUI();this.dispatchEventToListeners(WebInspector.BezierEditor.Events.BezierChanged,this._bezier.asCSSText());},_updateUI:function()
{var labelText=this._selectedCategory?this._selectedCategory.presets[this._selectedCategory.presetIndex].name:this._bezier.asCSSText().replace(/\s(-\d\.\d)/g,"$1");this._label.textContent=WebInspector.UIString(labelText);this._curveUI.drawCurve(this._bezier,this._curve);this._previewOnion.removeChildren();},_dragStart:function(event)
{this._mouseDownPosition=new WebInspector.Geometry.Point(event.x,event.y);var ui=this._curveUI;this._controlPosition=new WebInspector.Geometry.Point(Number.constrain((event.offsetX-ui.radius)/ui.curveWidth(),0,1),(ui.curveHeight()+ui.marginTop+ui.radius-event.offsetY)/ui.curveHeight());var firstControlPointIsCloser=this._controlPosition.distanceTo(this._bezier.controlPoints[0])<this._controlPosition.distanceTo(this._bezier.controlPoints[1]);this._selectedPoint=firstControlPointIsCloser?0:1;this._bezier.controlPoints[this._selectedPoint]=this._controlPosition;this._unselectPresets();this._onchange();event.consume(true);return true;},_updateControlPosition:function(mouseX,mouseY)
{var deltaX=(mouseX-this._mouseDownPosition.x)/this._curveUI.curveWidth();var deltaY=(mouseY-this._mouseDownPosition.y)/this._curveUI.curveHeight();var newPosition=new WebInspector.Geometry.Point(Number.constrain(this._controlPosition.x+deltaX,0,1),this._controlPosition.y-deltaY);this._bezier.controlPoints[this._selectedPoint]=newPosition;},_dragMove:function(event)
{this._updateControlPosition(event.x,event.y);this._onchange();},_dragEnd:function(event)
{this._updateControlPosition(event.x,event.y);this._onchange();this._startPreviewAnimation();},_createCategory:function(presetGroup)
{var presetElement=createElementWithClass("div","bezier-preset-category");var iconElement=presetElement.createSVGChild("svg","bezier-preset monospace");var category={presets:presetGroup,presetIndex:0,icon:presetElement};this._presetUI.drawCurve(WebInspector.Geometry.CubicBezier.parse(category.presets[0].value),iconElement);iconElement.addEventListener("click",this._presetCategorySelected.bind(this,category));return category;},_createPresetModifyIcon:function(parentElement,className,drawPath)
{var icon=parentElement.createSVGChild("svg","bezier-preset-modify "+className);icon.setAttribute("width",20);icon.setAttribute("height",20);var path=icon.createSVGChild("path");path.setAttribute("d",drawPath);return icon;},_unselectPresets:function()
{for(var category of this._presetCategories)
category.icon.classList.remove("bezier-preset-selected");delete this._selectedCategory;this._header.classList.remove("bezier-header-active");},_presetCategorySelected:function(category,event)
{if(this._selectedCategory===category)
return;this._unselectPresets();this._header.classList.add("bezier-header-active");this._selectedCategory=category;this._selectedCategory.icon.classList.add("bezier-preset-selected");this.setBezier(WebInspector.Geometry.CubicBezier.parse(category.presets[category.presetIndex].value));this._onchange();this._startPreviewAnimation();if(event)
event.consume(true);},_presetModifyClicked:function(intensify,event)
{if(!this._selectedCategory)
return;var length=this._selectedCategory.presets.length;this._selectedCategory.presetIndex=(this._selectedCategory.presetIndex+(intensify?1:-1)+length)%length;this.setBezier(WebInspector.Geometry.CubicBezier.parse(this._selectedCategory.presets[this._selectedCategory.presetIndex].value));this._onchange();this._startPreviewAnimation();},_startPreviewAnimation:function()
{if(this._previewAnimation)
this._previewAnimation.cancel();const animationDuration=1600;const numberOnionSlices=20;var keyframes=[{offset:0,transform:"translateX(0px)",easing:this._bezier.asCSSText(),opacity:1},{offset:0.9,transform:"translateX(218px)",opacity:1},{offset:1,transform:"translateX(218px)",opacity:0}];this._previewAnimation=this._previewElement.animate(keyframes,animationDuration);this._previewOnion.removeChildren();for(var i=0;i<=numberOnionSlices;i++){var slice=this._previewOnion.createChild("div","bezier-preview-animation");var player=slice.animate([{transform:"translateX(0px)",easing:this._bezier.asCSSText()},{transform:"translateX(218px)"}],{duration:animationDuration,fill:"forwards"});player.pause();player.currentTime=animationDuration*i/numberOnionSlices;}},__proto__:WebInspector.VBox.prototype};WebInspector.Spectrum=function()
{function appendSwitcherIcon(parentElement)
{var icon=parentElement.createSVGChild("svg");icon.setAttribute("height",16);icon.setAttribute("width",16);var path=icon.createSVGChild("path");path.setAttribute("d","M5,6 L11,6 L8,2 Z M5,10 L11,10 L8,14 Z");return icon;}
WebInspector.VBox.call(this,true);this.registerRequiredCSS("elements/spectrum.css");this.contentElement.tabIndex=0;this._colorElement=this.contentElement.createChild("div","spectrum-color");this._colorDragElement=this._colorElement.createChild("div","spectrum-sat fill").createChild("div","spectrum-val fill").createChild("div","spectrum-dragger");var contrastRatioSVG=this._colorElement.createSVGChild("svg","spectrum-contrast-container fill");this._contrastRatioLine=contrastRatioSVG.createSVGChild("path","spectrum-contrast-line");var toolbar=new WebInspector.Toolbar(this.contentElement);toolbar.element.classList.add("spectrum-eye-dropper");this._colorPickerButton=new WebInspector.ToolbarButton(WebInspector.UIString("Toggle color picker"),"eyedropper-toolbar-item");this._colorPickerButton.setToggled(true);this._colorPickerButton.addEventListener("click",this._toggleColorPicker.bind(this,undefined));toolbar.appendToolbarItem(this._colorPickerButton);var swatchElement=this.contentElement.createChild("span","swatch");this._swatchInnerElement=swatchElement.createChild("span","swatch-inner");this._hueElement=this.contentElement.createChild("div","spectrum-hue");this._hueSlider=this._hueElement.createChild("div","spectrum-slider");this._alphaElement=this.contentElement.createChild("div","spectrum-alpha");this._alphaElementBackground=this._alphaElement.createChild("div","spectrum-alpha-background");this._alphaSlider=this._alphaElement.createChild("div","spectrum-slider");var displaySwitcher=this.contentElement.createChild("div","spectrum-display-switcher spectrum-switcher");appendSwitcherIcon(displaySwitcher);displaySwitcher.addEventListener("click",this._formatViewSwitch.bind(this));this._displayContainer=this.contentElement.createChild("div","spectrum-text source-code");this._textValues=[];for(var i=0;i<4;++i){var inputValue=this._displayContainer.createChild("input","spectrum-text-value");inputValue.maxLength=4;this._textValues.push(inputValue);inputValue.addEventListener("keydown",this._inputChanged.bind(this),false);inputValue.addEventListener("input",this._inputChanged.bind(this),false);inputValue.addEventListener("mousewheel",this._inputChanged.bind(this),false);}
this._textLabels=this._displayContainer.createChild("div","spectrum-text-label");this._hexContainer=this.contentElement.createChild("div","spectrum-text spectrum-text-hex source-code");this._hexValue=this._hexContainer.createChild("input","spectrum-text-value");this._hexValue.maxLength=7;this._hexValue.addEventListener("keydown",this._inputChanged.bind(this),false);this._hexValue.addEventListener("input",this._inputChanged.bind(this),false);this._hexValue.addEventListener("mousewheel",this._inputChanged.bind(this),false);var label=this._hexContainer.createChild("div","spectrum-text-label");label.textContent="HEX";WebInspector.installDragHandle(this._hueElement,dragStart.bind(this,positionHue.bind(this)),positionHue.bind(this),null,"default");WebInspector.installDragHandle(this._alphaElement,dragStart.bind(this,positionAlpha.bind(this)),positionAlpha.bind(this),null,"default");WebInspector.installDragHandle(this._colorElement,dragStart.bind(this,positionColor.bind(this)),positionColor.bind(this),null,"default");this.element.classList.add("palettes-enabled");this._palettes=new Map();this._palettePanel=this.contentElement.createChild("div","palette-panel");this._palettePanelShowing=false;this._paletteContainer=this.contentElement.createChild("div","spectrum-palette");this._paletteContainer.addEventListener("contextmenu",this._showPaletteColorContextMenu.bind(this,-1));this._shadesContainer=this.contentElement.createChild("div","palette-color-shades hidden");WebInspector.installDragHandle(this._paletteContainer,this._paletteDragStart.bind(this),this._paletteDrag.bind(this),this._paletteDragEnd.bind(this),"default");var paletteSwitcher=this.contentElement.createChild("div","spectrum-palette-switcher spectrum-switcher");appendSwitcherIcon(paletteSwitcher);paletteSwitcher.addEventListener("click",this._togglePalettePanel.bind(this,true));this._deleteIconToolbar=new WebInspector.Toolbar();this._deleteIconToolbar.element.classList.add("delete-color-toolbar");this._deleteButton=new WebInspector.ToolbarButton("","garbage-collect-toolbar-item");this._deleteIconToolbar.appendToolbarItem(this._deleteButton);var overlay=this.contentElement.createChild("div","spectrum-overlay fill");overlay.addEventListener("click",this._togglePalettePanel.bind(this,false));this._addColorToolbar=new WebInspector.Toolbar();this._addColorToolbar.element.classList.add("add-color-toolbar");var addColorButton=new WebInspector.ToolbarButton(WebInspector.UIString("Add to palette"),"add-toolbar-item");addColorButton.addEventListener("click",this._addColorToCustomPalette.bind(this));this._addColorToolbar.appendToolbarItem(addColorButton);this._loadPalettes();new WebInspector.Spectrum.PaletteGenerator(this._generatedPaletteLoaded.bind(this));function dragStart(callback,event)
{this._hueAlphaLeft=this._hueElement.totalOffsetLeft();this._colorOffset=this._colorElement.totalOffset();callback(event);return true;}
function positionHue(event)
{var hsva=this._hsv.slice();hsva[0]=Number.constrain(1-(event.x-this._hueAlphaLeft)/this._hueAlphaWidth,0,1);this._innerSetColor(hsva,"",undefined,WebInspector.Spectrum._ChangeSource.Other);}
function positionAlpha(event)
{var newAlpha=Math.round((event.x-this._hueAlphaLeft)/this._hueAlphaWidth*100)/100;var hsva=this._hsv.slice();hsva[3]=Number.constrain(newAlpha,0,1);var colorFormat=undefined;if(hsva[3]!==1&&(this._colorFormat===WebInspector.Color.Format.ShortHEX||this._colorFormat===WebInspector.Color.Format.HEX||this._colorFormat===WebInspector.Color.Format.Nickname))
colorFormat=WebInspector.Color.Format.RGB;this._innerSetColor(hsva,"",colorFormat,WebInspector.Spectrum._ChangeSource.Other);}
function positionColor(event)
{var hsva=this._hsv.slice();hsva[1]=Number.constrain((event.x-this._colorOffset.left)/this.dragWidth,0,1);hsva[2]=Number.constrain(1-(event.y-this._colorOffset.top)/this.dragHeight,0,1);this._innerSetColor(hsva,"",undefined,WebInspector.Spectrum._ChangeSource.Other);}}
WebInspector.Spectrum._ChangeSource={Input:"Input",Model:"Model",Other:"Other"}
WebInspector.Spectrum.Events={ColorChanged:"ColorChanged",SizeChanged:"SizeChanged"};WebInspector.Spectrum._colorChipSize=24;WebInspector.Spectrum._itemsPerPaletteRow=8;WebInspector.Spectrum.prototype={_updatePalettePanel:function()
{this._palettePanel.removeChildren();var title=this._palettePanel.createChild("div","palette-title");title.textContent=WebInspector.UIString("Color Palettes");var toolbar=new WebInspector.Toolbar(this._palettePanel);var closeButton=new WebInspector.ToolbarButton("Return to color picker","delete-toolbar-item");closeButton.addEventListener("click",this._togglePalettePanel.bind(this,false));toolbar.appendToolbarItem(closeButton);for(var palette of this._palettes.values())
this._palettePanel.appendChild(this._createPreviewPaletteElement(palette));},_togglePalettePanel:function(show)
{if(this._palettePanelShowing===show)
return;if(show)
this._updatePalettePanel();this._focus();this._palettePanelShowing=show;this.contentElement.classList.toggle("palette-panel-showing",show);},_focus:function()
{if(WebInspector.currentFocusElement()!==this.contentElement)
WebInspector.setCurrentFocusElement(this.contentElement);},_createPaletteColor:function(colorText,animationDelay)
{var element=createElementWithClass("div","spectrum-palette-color");element.style.background=String.sprintf("linear-gradient(%s, %s), url(Images/checker.png)",colorText,colorText);if(animationDelay)
element.animate([{opacity:0},{opacity:1}],{duration:100,delay:animationDelay,fill:"backwards"});element.title=colorText;return element;},_showPalette:function(palette,animate,event)
{this._resizeForSelectedPalette();this._paletteContainer.removeChildren();for(var i=0;i<palette.colors.length;i++){var animationDelay=animate?i*100/palette.colors.length:0;var colorElement=this._createPaletteColor(palette.colors[i],animationDelay);colorElement.addEventListener("mousedown",this._paletteColorSelected.bind(this,palette.colors[i],palette.matchUserFormat));if(palette.mutable){colorElement.__mutable=true;colorElement.__color=palette.colors[i];colorElement.addEventListener("contextmenu",this._showPaletteColorContextMenu.bind(this,i));}else if(palette===WebInspector.Spectrum.MaterialPalette){colorElement.classList.add("has-material-shades");var shadow=colorElement.createChild("div","spectrum-palette-color spectrum-palette-color-shadow");shadow.style.background=palette.colors[i];shadow=colorElement.createChild("div","spectrum-palette-color spectrum-palette-color-shadow");shadow.style.background=palette.colors[i];var controller=new WebInspector.LongClickController(colorElement);controller.enable();controller.addEventListener(WebInspector.LongClickController.Events.LongClick,this._showLightnessShades.bind(this,colorElement,palette.colors[i]));}
this._paletteContainer.appendChild(colorElement);}
this._paletteContainerMutable=palette.mutable;var numItems=palette.colors.length;if(palette.mutable)
numItems++;if(palette.mutable){this._paletteContainer.appendChild(this._addColorToolbar.element);this._paletteContainer.appendChild(this._deleteIconToolbar.element);}else{this._addColorToolbar.element.remove();this._deleteIconToolbar.element.remove();}
this._togglePalettePanel(false);this._focus();},_showLightnessShades:function(colorElement,colorText,event)
{function closeLightnessShades(element)
{this._shadesContainer.classList.add("hidden");element.classList.remove("spectrum-shades-shown");this._shadesContainer.ownerDocument.removeEventListener("mousedown",this._shadesCloseHandler,true);delete this._shadesCloseHandler;}
if(this._shadesCloseHandler)
this._shadesCloseHandler();this._shadesContainer.classList.remove("hidden");this._shadesContainer.removeChildren();this._shadesContainer.animate([{transform:"scaleY(0)",opacity:"0"},{transform:"scaleY(1)",opacity:"1"}],{duration:200,easing:"cubic-bezier(0.4, 0, 0.2, 1)"});var anchorBox=colorElement.boxInWindow();this._shadesContainer.style.top=colorElement.offsetTop+"px";this._shadesContainer.style.left=colorElement.offsetLeft+"px";colorElement.classList.add("spectrum-shades-shown");var shades=WebInspector.Spectrum.MaterialPaletteShades[colorText];for(var i=shades.length-1;i>=0;i--){var shadeElement=this._createPaletteColor(shades[i],i*200/shades.length+100);shadeElement.addEventListener("mousedown",this._paletteColorSelected.bind(this,shades[i],false));this._shadesContainer.appendChild(shadeElement);}
WebInspector.setCurrentFocusElement(this._shadesContainer);this._shadesCloseHandler=closeLightnessShades.bind(this,colorElement);this._shadesContainer.ownerDocument.addEventListener("mousedown",this._shadesCloseHandler,true);},_slotIndexForEvent:function(e)
{var localX=e.pageX-this._paletteContainer.totalOffsetLeft();var localY=e.pageY-this._paletteContainer.totalOffsetTop();var col=Math.min(localX/WebInspector.Spectrum._colorChipSize|0,WebInspector.Spectrum._itemsPerPaletteRow-1);var row=(localY/WebInspector.Spectrum._colorChipSize)|0;return Math.min(row*WebInspector.Spectrum._itemsPerPaletteRow+col,this._customPaletteSetting.get().colors.length-1);},_isDraggingToBin:function(e)
{return e.pageX>this._deleteIconToolbar.element.totalOffsetLeft();},_paletteDragStart:function(e)
{var element=e.deepElementFromPoint();if(!element||!element.__mutable)
return false;var index=this._slotIndexForEvent(e);this._dragElement=element;this._dragHotSpotX=e.pageX-(index%WebInspector.Spectrum._itemsPerPaletteRow)*WebInspector.Spectrum._colorChipSize;this._dragHotSpotY=e.pageY-(index/WebInspector.Spectrum._itemsPerPaletteRow|0)*WebInspector.Spectrum._colorChipSize;this._deleteIconToolbar.element.classList.add("dragging");return true;},_paletteDrag:function(e)
{if(e.pageX<this._paletteContainer.totalOffsetLeft()||e.pageY<this._paletteContainer.totalOffsetTop())
return;var newIndex=this._slotIndexForEvent(e);var offsetX=e.pageX-(newIndex%WebInspector.Spectrum._itemsPerPaletteRow)*WebInspector.Spectrum._colorChipSize;var offsetY=e.pageY-(newIndex/WebInspector.Spectrum._itemsPerPaletteRow|0)*WebInspector.Spectrum._colorChipSize;var isDeleting=this._isDraggingToBin(e);this._deleteIconToolbar.element.classList.toggle("delete-color-toolbar-active",isDeleting);var dragElementTransform="translateX("+(offsetX-this._dragHotSpotX)+"px) translateY("+(offsetY-this._dragHotSpotY)+"px)";this._dragElement.style.transform=isDeleting?dragElementTransform+" scale(0.8)":dragElementTransform;var children=Array.prototype.slice.call(this._paletteContainer.children);var index=children.indexOf(this._dragElement);var swatchOffsets=new Map();for(var swatch of children)
swatchOffsets.set(swatch,swatch.totalOffset());if(index!==newIndex)
this._paletteContainer.insertBefore(this._dragElement,children[newIndex>index?newIndex+1:newIndex]);for(var swatch of children){if(swatch===this._dragElement)
continue;var before=swatchOffsets.get(swatch);var after=swatch.totalOffset();if(before.left!==after.left||before.top!==after.top){swatch.animate([{transform:"translateX("+(before.left-after.left)+"px) translateY("+(before.top-after.top)+"px)"},{transform:"none"}],{duration:100,easing:"cubic-bezier(0, 0, 0.2, 1)"});}}},_paletteDragEnd:function(e)
{if(this._isDraggingToBin(e))
this._dragElement.remove();this._dragElement.style.removeProperty("transform");var children=this._paletteContainer.children;var colors=[];for(var i=0;i<children.length;++i){if(children[i].__color)
colors.push(children[i].__color);}
var palette=this._customPaletteSetting.get();palette.colors=colors;this._customPaletteSetting.set(palette);this._showPalette(this._customPaletteSetting.get(),false);this._deleteIconToolbar.element.classList.remove("dragging");this._deleteIconToolbar.element.classList.remove("delete-color-toolbar-active");this._deleteButton.setToggled(false);},_loadPalettes:function()
{this._palettes.set(WebInspector.Spectrum.MaterialPalette.title,WebInspector.Spectrum.MaterialPalette);var defaultCustomPalette={title:"Custom",colors:[],mutable:true};this._customPaletteSetting=WebInspector.settings.createSetting("customColorPalette",defaultCustomPalette);this._palettes.set(this._customPaletteSetting.get().title,this._customPaletteSetting.get());this._selectedColorPalette=WebInspector.settings.createSetting("selectedColorPalette",WebInspector.Spectrum.GeneratedPaletteTitle);var palette=this._palettes.get(this._selectedColorPalette.get());if(palette)
this._showPalette(palette,true);},_generatedPaletteLoaded:function(generatedPalette)
{if(generatedPalette.colors.length)
this._palettes.set(generatedPalette.title,generatedPalette);if(this._selectedColorPalette.get()!==generatedPalette.title){return;}else if(!generatedPalette.colors.length){this._paletteSelected(WebInspector.Spectrum.MaterialPalette);return;}
this._showPalette(generatedPalette,true);},_createPreviewPaletteElement:function(palette)
{var colorsPerPreviewRow=5;var previewElement=createElementWithClass("div","palette-preview");var titleElement=previewElement.createChild("div","palette-preview-title");titleElement.textContent=palette.title;for(var i=0;i<colorsPerPreviewRow&&i<palette.colors.length;i++)
previewElement.appendChild(this._createPaletteColor(palette.colors[i]));for(;i<colorsPerPreviewRow;i++)
previewElement.createChild("div","spectrum-palette-color empty-color");previewElement.addEventListener("click",this._paletteSelected.bind(this,palette));return previewElement;},_paletteSelected:function(palette)
{this._selectedColorPalette.set(palette.title);this._showPalette(palette,true);},_resizeForSelectedPalette:function()
{var palette=this._palettes.get(this._selectedColorPalette.get());if(!palette)
return;var numColors=palette.colors.length;if(palette===this._customPaletteSetting.get())
numColors++;var rowsNeeded=Math.max(1,Math.ceil(numColors/WebInspector.Spectrum._itemsPerPaletteRow));if(this._numPaletteRowsShown===rowsNeeded)
return;this._numPaletteRowsShown=rowsNeeded;var paletteColorHeight=12;var paletteMargin=12;var paletteTop=235;this.element.style.height=(paletteTop+paletteMargin+(paletteColorHeight+paletteMargin)*rowsNeeded)+"px";this.dispatchEventToListeners(WebInspector.Spectrum.Events.SizeChanged);},_paletteColorSelected:function(colorText,matchUserFormat)
{var color=WebInspector.Color.parse(colorText);if(!color)
return;this._innerSetColor(color.hsva(),colorText,matchUserFormat?this._colorFormat:color.format(),WebInspector.Spectrum._ChangeSource.Other);},_addColorToCustomPalette:function()
{var palette=this._customPaletteSetting.get();palette.colors.push(this.colorString());this._customPaletteSetting.set(palette);this._showPalette(this._customPaletteSetting.get(),false);},_showPaletteColorContextMenu:function(colorIndex,event)
{if(!this._paletteContainerMutable)
return;var contextMenu=new WebInspector.ContextMenu(event);if(colorIndex!==-1){contextMenu.appendItem(WebInspector.UIString("Remove color"),this._deletePaletteColors.bind(this,colorIndex,false));contextMenu.appendItem(WebInspector.UIString("Remove all to the right"),this._deletePaletteColors.bind(this,colorIndex,true));}
contextMenu.appendItem(WebInspector.UIString("Clear palette"),this._deletePaletteColors.bind(this,-1,true));contextMenu.show();},_deletePaletteColors:function(colorIndex,toRight)
{var palette=this._customPaletteSetting.get();if(toRight)
palette.colors.splice(colorIndex+1,palette.colors.length-colorIndex-1);else
palette.colors.splice(colorIndex,1);this._customPaletteSetting.set(palette);this._showPalette(this._customPaletteSetting.get(),false);},setColor:function(color,colorFormat)
{this._originalFormat=colorFormat;this._innerSetColor(color.hsva(),"",colorFormat,WebInspector.Spectrum._ChangeSource.Model);},_innerSetColor:function(hsva,colorString,colorFormat,changeSource)
{if(hsva!==undefined)
this._hsv=hsva;if(colorString!==undefined)
this._colorString=colorString;if(colorFormat!==undefined){console.assert(colorFormat!==WebInspector.Color.Format.Original,"Spectrum's color format cannot be Original");if(colorFormat===WebInspector.Color.Format.RGBA)
colorFormat=WebInspector.Color.Format.RGB;else if(colorFormat===WebInspector.Color.Format.HSLA)
colorFormat=WebInspector.Color.Format.HSL;this._colorFormat=colorFormat;}
this._updateHelperLocations();this._updateUI();if(changeSource!==WebInspector.Spectrum._ChangeSource.Input)
this._updateInput();if(changeSource!==WebInspector.Spectrum._ChangeSource.Model)
this.dispatchEventToListeners(WebInspector.Spectrum.Events.ColorChanged,this.colorString());},setContrastColor:function(color)
{this._contrastColor=color;this._updateUI();},_color:function()
{return WebInspector.Color.fromHSVA(this._hsv);},colorString:function()
{if(this._colorString)
return this._colorString;var cf=WebInspector.Color.Format;var color=this._color();var colorString=color.asString(this._colorFormat);if(colorString)
return colorString;if(this._colorFormat===cf.Nickname||this._colorFormat===cf.ShortHEX){colorString=color.asString(cf.HEX);if(colorString)
return colorString;}
console.assert(color.hasAlpha());return this._colorFormat===cf.HSL?(color.asString(cf.HSLA)):(color.asString(cf.RGBA));},_updateHelperLocations:function()
{var h=this._hsv[0];var s=this._hsv[1];var v=this._hsv[2];var alpha=this._hsv[3];var dragX=s*this.dragWidth;var dragY=this.dragHeight-(v*this.dragHeight);dragX=Math.max(-this._colorDragElementHeight,Math.min(this.dragWidth-this._colorDragElementHeight,dragX-this._colorDragElementHeight));dragY=Math.max(-this._colorDragElementHeight,Math.min(this.dragHeight-this._colorDragElementHeight,dragY-this._colorDragElementHeight));this._colorDragElement.positionAt(dragX,dragY);var hueSlideX=(1-h)*this._hueAlphaWidth-this.slideHelperWidth;this._hueSlider.style.left=hueSlideX+"px";var alphaSlideX=alpha*this._hueAlphaWidth-this.slideHelperWidth;this._alphaSlider.style.left=alphaSlideX+"px";},_updateInput:function()
{var cf=WebInspector.Color.Format;if(this._colorFormat===cf.HEX||this._colorFormat===cf.ShortHEX||this._colorFormat===cf.Nickname){this._hexContainer.hidden=false;this._displayContainer.hidden=true;if(this._colorFormat===cf.ShortHEX&&this._color().canBeShortHex())
this._hexValue.value=this._color().asString(cf.ShortHEX);else
this._hexValue.value=this._color().asString(cf.HEX);}else{this._hexContainer.hidden=true;this._displayContainer.hidden=false;var isRgb=this._colorFormat===cf.RGB;this._textLabels.textContent=isRgb?"RGBA":"HSLA";var colorValues=isRgb?this._color().canonicalRGBA():this._color().canonicalHSLA();for(var i=0;i<3;++i){this._textValues[i].value=colorValues[i];if(!isRgb&&(i===1||i===2))
this._textValues[i].value+="%";}
this._textValues[3].value=Math.round(colorValues[3]*100)/100;}},_drawContrastRatioLine:function(requiredContrast)
{if(!this._contrastColor||!this.dragWidth||!this.dragHeight)
return;var width=this.dragWidth;var height=this.dragHeight;var dS=0.02;var epsilon=0.002;var H=0;var S=1;var V=2;var A=3;var fgRGBA=[];WebInspector.Color.hsva2rgba(this._hsv,fgRGBA);var fgLuminance=WebInspector.Color.luminance(fgRGBA);var bgRGBA=this._contrastColor.rgba();var bgLuminance=WebInspector.Color.luminance(bgRGBA);var fgIsLighter=fgLuminance>bgLuminance;var desiredLuminance=WebInspector.Color.desiredLuminance(bgLuminance,requiredContrast,fgIsLighter);var lastV=this._hsv[V];var currentSlope=0;var candidateHSVA=[this._hsv[H],0,0,this._hsv[A]];var pathBuilder=[];var candidateRGBA=[];WebInspector.Color.hsva2rgba(candidateHSVA,candidateRGBA);var blendedRGBA=[];WebInspector.Color.blendColors(candidateRGBA,bgRGBA,blendedRGBA);function approach(index,x,onAxis)
{while(0<=x&&x<=1){candidateHSVA[index]=x;WebInspector.Color.hsva2rgba(candidateHSVA,candidateRGBA);WebInspector.Color.blendColors(candidateRGBA,bgRGBA,blendedRGBA);var fgLuminance=WebInspector.Color.luminance(blendedRGBA);var dLuminance=fgLuminance-desiredLuminance;if(Math.abs(dLuminance)<(onAxis?epsilon/10:epsilon))
return x;else
x+=(index===V?-dLuminance:dLuminance);}
return null;}
for(var s=0;s<1+dS;s+=dS){s=Math.min(1,s);candidateHSVA[S]=s;var v=lastV;v=lastV+currentSlope*dS;v=approach(V,v,s==0);if(v===null)
break;currentSlope=(v-lastV)/dS;pathBuilder.push(pathBuilder.length?"L":"M");pathBuilder.push(s*width);pathBuilder.push((1-v)*height);}
if(s<1+dS){s-=dS;candidateHSVA[V]=1;s=approach(S,s,true);if(s!==null)
pathBuilder=pathBuilder.concat(["L",s*width,-1])}
this._contrastRatioLine.setAttribute("d",pathBuilder.join(" "));},_updateUI:function()
{var h=WebInspector.Color.fromHSVA([this._hsv[0],1,1,1]);this._colorElement.style.backgroundColor=(h.asString(WebInspector.Color.Format.RGB));if(Runtime.experiments.isEnabled("colorContrastRatio")){this._drawContrastRatioLine(4.5);}
this._swatchInnerElement.style.backgroundColor=(this._color().asString(WebInspector.Color.Format.RGBA));this._swatchInnerElement.classList.toggle("swatch-inner-white",this._color().hsla()[2]>0.9);this._colorDragElement.style.backgroundColor=(this._color().asString(WebInspector.Color.Format.RGBA));var noAlpha=WebInspector.Color.fromHSVA(this._hsv.slice(0,3).concat(1));this._alphaElementBackground.style.backgroundImage=String.sprintf("linear-gradient(to right, rgba(0,0,0,0), %s)",noAlpha.asString(WebInspector.Color.Format.RGB));},_formatViewSwitch:function()
{var cf=WebInspector.Color.Format;var format=cf.RGB;if(this._colorFormat===cf.RGB)
format=cf.HSL;else if(this._colorFormat===cf.HSL&&!this._color().hasAlpha())
format=this._originalFormat===cf.ShortHEX?cf.ShortHEX:cf.HEX;this._innerSetColor(undefined,"",format,WebInspector.Spectrum._ChangeSource.Other);},_inputChanged:function(event)
{function elementValue(element)
{return element.value;}
var inputElement=(event.currentTarget);var arrowKeyOrMouseWheelEvent=(event.keyIdentifier==="Up"||event.keyIdentifier==="Down"||event.type==="mousewheel");var pageKeyPressed=(event.keyIdentifier==="PageUp"||event.keyIdentifier==="PageDown");if(arrowKeyOrMouseWheelEvent||pageKeyPressed){var newValue=WebInspector.createReplacementString(inputElement.value,event);if(newValue){inputElement.value=newValue;inputElement.selectionStart=0;inputElement.selectionEnd=newValue.length;}
event.consume(true);}
const cf=WebInspector.Color.Format;var colorString;if(this._colorFormat===cf.HEX||this._colorFormat===cf.ShortHEX){colorString=this._hexValue.value;}else{var format=this._colorFormat===cf.RGB?"rgba":"hsla";var values=this._textValues.map(elementValue).join(",");colorString=String.sprintf("%s(%s)",format,values);}
var color=WebInspector.Color.parse(colorString);if(!color)
return;var hsv=color.hsva();if(this._colorFormat===cf.HEX||this._colorFormat===cf.ShortHEX)
this._colorFormat=color.canBeShortHex()?cf.ShortHEX:cf.HEX;this._innerSetColor(hsv,colorString,undefined,WebInspector.Spectrum._ChangeSource.Input);},wasShown:function()
{this._hueAlphaWidth=this._hueElement.offsetWidth;this.slideHelperWidth=this._hueSlider.offsetWidth/2;this.dragWidth=this._colorElement.offsetWidth;this.dragHeight=this._colorElement.offsetHeight;this._colorDragElementHeight=this._colorDragElement.offsetHeight/2;this._innerSetColor(undefined,undefined,undefined,WebInspector.Spectrum._ChangeSource.Model);this._toggleColorPicker(true);WebInspector.targetManager.addModelListener(WebInspector.ResourceTreeModel,WebInspector.ResourceTreeModel.EventTypes.ColorPicked,this._colorPicked,this);},willHide:function()
{this._toggleColorPicker(false);WebInspector.targetManager.removeModelListener(WebInspector.ResourceTreeModel,WebInspector.ResourceTreeModel.EventTypes.ColorPicked,this._colorPicked,this);},_toggleColorPicker:function(enabled,event)
{if(enabled===undefined)
enabled=!this._colorPickerButton.toggled();this._colorPickerButton.setToggled(enabled);for(var target of WebInspector.targetManager.targets())
target.pageAgent().setColorPickerEnabled(enabled);},_colorPicked:function(event)
{var rgbColor=(event.data);var rgba=[rgbColor.r,rgbColor.g,rgbColor.b,(rgbColor.a/2.55|0)/100];var color=WebInspector.Color.fromRGBA(rgba);this._innerSetColor(color.hsva(),"",undefined,WebInspector.Spectrum._ChangeSource.Other);InspectorFrontendHost.bringToFront();},__proto__:WebInspector.VBox.prototype}
WebInspector.Spectrum.Palette;WebInspector.Spectrum.GeneratedPaletteTitle="Page colors";WebInspector.Spectrum.PaletteGenerator=function(callback)
{this._callback=callback;this._frequencyMap=new Map();var stylesheetPromises=[];for(var target of WebInspector.targetManager.targets(WebInspector.Target.Type.Page)){var cssModel=WebInspector.CSSStyleModel.fromTarget(target);for(var stylesheet of cssModel.allStyleSheets())
stylesheetPromises.push(new Promise(this._processStylesheet.bind(this,stylesheet)));}
Promise.all(stylesheetPromises).catchException(null).then(this._finish.bind(this));}
WebInspector.Spectrum.PaletteGenerator.prototype={_frequencyComparator:function(a,b)
{return this._frequencyMap.get(b)-this._frequencyMap.get(a);},_finish:function()
{function hueComparator(a,b)
{var hsva=paletteColors.get(a).hsva();var hsvb=paletteColors.get(b).hsva();if(hsvb[1]<0.12&&hsva[1]<0.12)
return hsvb[2]*hsvb[3]-hsva[2]*hsva[3];if(hsvb[1]<0.12)
return-1;if(hsva[1]<0.12)
return 1;if(hsvb[0]===hsva[0])
return hsvb[1]*hsvb[3]-hsva[1]*hsva[3];return(hsvb[0]+0.94)%1-(hsva[0]+0.94)%1;}
var colors=this._frequencyMap.keysArray();colors=colors.sort(this._frequencyComparator.bind(this));var paletteColors=new Map();var colorsPerRow=24;while(paletteColors.size<colorsPerRow&&colors.length){var colorText=colors.shift();var color=WebInspector.Color.parse(colorText);if(!color||color.nickname()==="white"||color.nickname()==="black")
continue;paletteColors.set(colorText,color);}
this._callback({title:WebInspector.Spectrum.GeneratedPaletteTitle,colors:paletteColors.keysArray().sort(hueComparator),mutable:false});},_processStylesheet:function(stylesheet,resolve)
{function parseContent(text)
{text=text.toLowerCase();var regexResult=text.match(/((?:rgb|hsl)a?\([^)]+\)|#[0-9a-f]{6}|#[0-9a-f]{3})/g)||[];for(var c of regexResult){var frequency=this._frequencyMap.get(c)||0;this._frequencyMap.set(c,++frequency);}
resolve(null);}
stylesheet.requestContent(parseContent.bind(this));}}
WebInspector.Spectrum.MaterialPaletteShades={"#F44336":["#FFEBEE","#FFCDD2","#EF9A9A","#E57373","#EF5350","#F44336","#E53935","#D32F2F","#C62828","#B71C1C"],"#E91E63":["#FCE4EC","#F8BBD0","#F48FB1","#F06292","#EC407A","#E91E63","#D81B60","#C2185B","#AD1457","#880E4F"],"#9C27B0":["#F3E5F5","#E1BEE7","#CE93D8","#BA68C8","#AB47BC","#9C27B0","#8E24AA","#7B1FA2","#6A1B9A","#4A148C"],"#673AB7":["#EDE7F6","#D1C4E9","#B39DDB","#9575CD","#7E57C2","#673AB7","#5E35B1","#512DA8","#4527A0","#311B92"],"#3F51B5":["#E8EAF6","#C5CAE9","#9FA8DA","#7986CB","#5C6BC0","#3F51B5","#3949AB","#303F9F","#283593","#1A237E"],"#2196F3":["#E3F2FD","#BBDEFB","#90CAF9","#64B5F6","#42A5F5","#2196F3","#1E88E5","#1976D2","#1565C0","#0D47A1"],"#03A9F4":["#E1F5FE","#B3E5FC","#81D4FA","#4FC3F7","#29B6F6","#03A9F4","#039BE5","#0288D1","#0277BD","#01579B"],"#00BCD4":["#E0F7FA","#B2EBF2","#80DEEA","#4DD0E1","#26C6DA","#00BCD4","#00ACC1","#0097A7","#00838F","#006064"],"#009688":["#E0F2F1","#B2DFDB","#80CBC4","#4DB6AC","#26A69A","#009688","#00897B","#00796B","#00695C","#004D40"],"#4CAF50":["#E8F5E9","#C8E6C9","#A5D6A7","#81C784","#66BB6A","#4CAF50","#43A047","#388E3C","#2E7D32","#1B5E20"],"#8BC34A":["#F1F8E9","#DCEDC8","#C5E1A5","#AED581","#9CCC65","#8BC34A","#7CB342","#689F38","#558B2F","#33691E"],"#CDDC39":["#F9FBE7","#F0F4C3","#E6EE9C","#DCE775","#D4E157","#CDDC39","#C0CA33","#AFB42B","#9E9D24","#827717"],"#FFEB3B":["#FFFDE7","#FFF9C4","#FFF59D","#FFF176","#FFEE58","#FFEB3B","#FDD835","#FBC02D","#F9A825","#F57F17"],"#FFC107":["#FFF8E1","#FFECB3","#FFE082","#FFD54F","#FFCA28","#FFC107","#FFB300","#FFA000","#FF8F00","#FF6F00"],"#FF9800":["#FFF3E0","#FFE0B2","#FFCC80","#FFB74D","#FFA726","#FF9800","#FB8C00","#F57C00","#EF6C00","#E65100"],"#FF5722":["#FBE9E7","#FFCCBC","#FFAB91","#FF8A65","#FF7043","#FF5722","#F4511E","#E64A19","#D84315","#BF360C"],"#795548":["#EFEBE9","#D7CCC8","#BCAAA4","#A1887F","#8D6E63","#795548","#6D4C41","#5D4037","#4E342E","#3E2723"],"#9E9E9E":["#FAFAFA","#F5F5F5","#EEEEEE","#E0E0E0","#BDBDBD","#9E9E9E","#757575","#616161","#424242","#212121"],"#607D8B":["#ECEFF1","#CFD8DC","#B0BEC5","#90A4AE","#78909C","#607D8B","#546E7A","#455A64","#37474F","#263238"]};WebInspector.Spectrum.MaterialPalette={title:"Material",mutable:false,matchUserFormat:true,colors:Object.keys(WebInspector.Spectrum.MaterialPaletteShades)};;WebInspector.ElementsBreadcrumbs=function()
{WebInspector.HBox.call(this,true);this.registerRequiredCSS("elements/breadcrumbs.css");this.crumbsElement=this.contentElement.createChild("div","crumbs");this.crumbsElement.addEventListener("mousemove",this._mouseMovedInCrumbs.bind(this),false);this.crumbsElement.addEventListener("mouseleave",this._mouseMovedOutOfCrumbs.bind(this),false);this._nodeSymbol=Symbol("node");}
WebInspector.ElementsBreadcrumbs.Events={NodeSelected:"NodeSelected"}
WebInspector.ElementsBreadcrumbs.prototype={wasShown:function()
{this.update();},updateNodes:function(nodes)
{if(!nodes.length)
return;var crumbs=this.crumbsElement;for(var crumb=crumbs.firstChild;crumb;crumb=crumb.nextSibling){if(nodes.indexOf(crumb[this._nodeSymbol])!==-1){this.update(true);return;}}},setSelectedNode:function(node)
{this._currentDOMNode=node;this.update();},_mouseMovedInCrumbs:function(event)
{var nodeUnderMouse=event.target;var crumbElement=nodeUnderMouse.enclosingNodeOrSelfWithClass("crumb");var node=(crumbElement?crumbElement[this._nodeSymbol]:null);if(node)
node.highlight();},_mouseMovedOutOfCrumbs:function(event)
{if(this._currentDOMNode)
WebInspector.DOMModel.hideDOMNodeHighlight();},update:function(force)
{if(!this.isShowing())
return;var currentDOMNode=this._currentDOMNode;var crumbs=this.crumbsElement;var handled=false;var crumb=crumbs.firstChild;while(crumb){if(crumb[this._nodeSymbol]===currentDOMNode){crumb.classList.add("selected");handled=true;}else{crumb.classList.remove("selected");}
crumb=crumb.nextSibling;}
if(handled&&!force){this.updateSizes();return;}
crumbs.removeChildren();var panel=this;function selectCrumb(event)
{event.preventDefault();var crumb=(event.currentTarget);if(!crumb.classList.contains("collapsed")){this.dispatchEventToListeners(WebInspector.ElementsBreadcrumbs.Events.NodeSelected,crumb[this._nodeSymbol]);return;}
if(crumb===panel.crumbsElement.firstChild){var currentCrumb=crumb;while(currentCrumb){var hidden=currentCrumb.classList.contains("hidden");var collapsed=currentCrumb.classList.contains("collapsed");if(!hidden&&!collapsed)
break;crumb=currentCrumb;currentCrumb=currentCrumb.nextSiblingElement;}}
this.updateSizes(crumb);}
var boundSelectCrumb=selectCrumb.bind(this);for(var current=currentDOMNode;current;current=current.parentNode){if(current.nodeType()===Node.DOCUMENT_NODE)
continue;crumb=createElementWithClass("span","crumb");crumb[this._nodeSymbol]=current;crumb.addEventListener("mousedown",boundSelectCrumb,false);var crumbTitle="";switch(current.nodeType()){case Node.ELEMENT_NODE:if(current.pseudoType())
crumbTitle="::"+current.pseudoType();else
WebInspector.DOMPresentationUtils.decorateNodeLabel(current,crumb);break;case Node.TEXT_NODE:crumbTitle=WebInspector.UIString("(text)");break;case Node.COMMENT_NODE:crumbTitle="<!-->";break;case Node.DOCUMENT_TYPE_NODE:crumbTitle="<!DOCTYPE>";break;case Node.DOCUMENT_FRAGMENT_NODE:crumbTitle=current.shadowRootType()?"#shadow-root":current.nodeNameInCorrectCase();break;default:crumbTitle=current.nodeNameInCorrectCase();}
if(!crumb.childNodes.length){var nameElement=createElement("span");nameElement.textContent=crumbTitle;crumb.appendChild(nameElement);crumb.title=crumbTitle;}
if(current===currentDOMNode)
crumb.classList.add("selected");crumbs.insertBefore(crumb,crumbs.firstChild);}
this.updateSizes();},updateSizes:function(focusedCrumb)
{if(!this.isShowing())
return;var crumbs=this.crumbsElement;if(!crumbs.firstChild)
return;var selectedIndex=0;var focusedIndex=0;var selectedCrumb;for(var i=0;i<crumbs.childNodes.length;++i){var crumb=crumbs.childNodes[i];if(!selectedCrumb&&crumb.classList.contains("selected")){selectedCrumb=crumb;selectedIndex=i;}
if(crumb===focusedCrumb)
focusedIndex=i;crumb.classList.remove("compact","collapsed","hidden");}
var contentElementWidth=this.contentElement.offsetWidth;var normalSizes=[];for(var i=0;i<crumbs.childNodes.length;++i){var crumb=crumbs.childNodes[i];normalSizes[i]=crumb.offsetWidth;}
var compactSizes=[];for(var i=0;i<crumbs.childNodes.length;++i){var crumb=crumbs.childNodes[i];crumb.classList.add("compact");}
for(var i=0;i<crumbs.childNodes.length;++i){var crumb=crumbs.childNodes[i];compactSizes[i]=crumb.offsetWidth;}
crumbs.firstChild.classList.add("collapsed");var collapsedSize=crumbs.firstChild.offsetWidth;for(var i=0;i<crumbs.childNodes.length;++i){var crumb=crumbs.childNodes[i];crumb.classList.remove("compact","collapsed");}
function crumbsAreSmallerThanContainer()
{var totalSize=0;for(var i=0;i<crumbs.childNodes.length;++i){var crumb=crumbs.childNodes[i];if(crumb.classList.contains("hidden"))
continue;if(crumb.classList.contains("collapsed")){totalSize+=collapsedSize;continue;}
totalSize+=crumb.classList.contains("compact")?compactSizes[i]:normalSizes[i];}
const rightPadding=10;return totalSize+rightPadding<contentElementWidth;}
if(crumbsAreSmallerThanContainer())
return;var BothSides=0;var AncestorSide=-1;var ChildSide=1;function makeCrumbsSmaller(shrinkingFunction,direction)
{var significantCrumb=focusedCrumb||selectedCrumb;var significantIndex=significantCrumb===selectedCrumb?selectedIndex:focusedIndex;function shrinkCrumbAtIndex(index)
{var shrinkCrumb=crumbs.childNodes[index];if(shrinkCrumb&&shrinkCrumb!==significantCrumb)
shrinkingFunction(shrinkCrumb);if(crumbsAreSmallerThanContainer())
return true;return false;}
if(direction){var index=(direction>0?0:crumbs.childNodes.length-1);while(index!==significantIndex){if(shrinkCrumbAtIndex(index))
return true;index+=(direction>0?1:-1);}}else{var startIndex=0;var endIndex=crumbs.childNodes.length-1;while(startIndex!=significantIndex||endIndex!=significantIndex){var startDistance=significantIndex-startIndex;var endDistance=endIndex-significantIndex;if(startDistance>=endDistance)
var index=startIndex++;else
var index=endIndex--;if(shrinkCrumbAtIndex(index))
return true;}}
return false;}
function coalesceCollapsedCrumbs()
{var crumb=crumbs.firstChild;var collapsedRun=false;var newStartNeeded=false;var newEndNeeded=false;while(crumb){var hidden=crumb.classList.contains("hidden");if(!hidden){var collapsed=crumb.classList.contains("collapsed");if(collapsedRun&&collapsed){crumb.classList.add("hidden");crumb.classList.remove("compact");crumb.classList.remove("collapsed");if(crumb.classList.contains("start")){crumb.classList.remove("start");newStartNeeded=true;}
if(crumb.classList.contains("end")){crumb.classList.remove("end");newEndNeeded=true;}
continue;}
collapsedRun=collapsed;if(newEndNeeded){newEndNeeded=false;crumb.classList.add("end");}}else{collapsedRun=true;}
crumb=crumb.nextSibling;}
if(newStartNeeded){crumb=crumbs.lastChild;while(crumb){if(!crumb.classList.contains("hidden")){crumb.classList.add("start");break;}
crumb=crumb.previousSibling;}}}
function compact(crumb)
{if(crumb.classList.contains("hidden"))
return;crumb.classList.add("compact");}
function collapse(crumb,dontCoalesce)
{if(crumb.classList.contains("hidden"))
return;crumb.classList.add("collapsed");crumb.classList.remove("compact");if(!dontCoalesce)
coalesceCollapsedCrumbs();}
if(!focusedCrumb){if(makeCrumbsSmaller(compact,ChildSide))
return;if(makeCrumbsSmaller(collapse,ChildSide))
return;}
if(makeCrumbsSmaller(compact,focusedCrumb?BothSides:AncestorSide))
return;if(makeCrumbsSmaller(collapse,focusedCrumb?BothSides:AncestorSide))
return;if(!selectedCrumb)
return;compact(selectedCrumb);if(crumbsAreSmallerThanContainer())
return;collapse(selectedCrumb,true);},__proto__:WebInspector.HBox.prototype};WebInspector.ElementsSidebarPane=function(title)
{WebInspector.SidebarPane.call(this,title);this._node=null;this._updateController=new WebInspector.ElementsSidebarPane._UpdateController(this,this.doUpdate.bind(this));WebInspector.context.addFlavorChangeListener(WebInspector.DOMNode,this._nodeChanged,this);}
WebInspector.ElementsSidebarPane.prototype={node:function()
{return this._node;},cssModel:function()
{return this._cssModel&&this._cssModel.isEnabled()?this._cssModel:null;},_nodeChanged:function(event)
{this.setNode((event.data));},setNode:function(node)
{this._node=node;this._updateTarget(node?node.target():null);this.update();},doUpdate:function()
{return Promise.resolve();},update:function()
{this._updateController.update();},wasShown:function()
{WebInspector.SidebarPane.prototype.wasShown.call(this);this._updateController.viewWasShown();},_updateTarget:function(target)
{if(this._target===target)
return;if(this._targetEvents){WebInspector.EventTarget.removeEventListeners(this._targetEvents);this._targetEvents=null;}
this._target=target;var domModel=null;var resourceTreeModel=null;if(target){this._cssModel=WebInspector.CSSStyleModel.fromTarget(target);domModel=WebInspector.DOMModel.fromTarget(target);resourceTreeModel=target.resourceTreeModel;}
if(this._cssModel&&domModel&&resourceTreeModel){this._targetEvents=[this._cssModel.addEventListener(WebInspector.CSSStyleModel.Events.StyleSheetAdded,this.onCSSModelChanged,this),this._cssModel.addEventListener(WebInspector.CSSStyleModel.Events.StyleSheetRemoved,this.onCSSModelChanged,this),this._cssModel.addEventListener(WebInspector.CSSStyleModel.Events.StyleSheetChanged,this.onCSSModelChanged,this),this._cssModel.addEventListener(WebInspector.CSSStyleModel.Events.MediaQueryResultChanged,this.onCSSModelChanged,this),this._cssModel.addEventListener(WebInspector.CSSStyleModel.Events.PseudoStateForced,this.onCSSModelChanged,this),this._cssModel.addEventListener(WebInspector.CSSStyleModel.Events.ModelWasEnabled,this.onCSSModelChanged,this),domModel.addEventListener(WebInspector.DOMModel.Events.DOMMutated,this._domModelChanged,this),resourceTreeModel.addEventListener(WebInspector.ResourceTreeModel.EventTypes.FrameResized,this._onFrameResized,this),];}},_onFrameResized:function(event)
{function refreshContents()
{this.onFrameResizedThrottled();delete this._frameResizedTimer;}
if(this._frameResizedTimer)
clearTimeout(this._frameResizedTimer);this._frameResizedTimer=setTimeout(refreshContents.bind(this),100);},_domModelChanged:function(event)
{var node=(event.data);this.onDOMModelChanged(node)},onDOMModelChanged:function(node){},onCSSModelChanged:function(){},onFrameResizedThrottled:function(){},__proto__:WebInspector.SidebarPane.prototype}
WebInspector.ElementsSidebarPane._UpdateController=function(view,doUpdate)
{this._view=view;this._updateThrottler=new WebInspector.Throttler(100);this._updateWhenVisible=false;this._doUpdate=doUpdate;}
WebInspector.ElementsSidebarPane._UpdateController.prototype={update:function()
{this._updateWhenVisible=!this._view.isShowing();if(this._updateWhenVisible)
return;this._updateThrottler.schedule(innerUpdate.bind(this));function innerUpdate()
{return this._view.isShowing()?this._doUpdate.call(null):Promise.resolve();}},viewWasShown:function()
{if(this._updateWhenVisible)
this.update();}};WebInspector.ElementsSidebarViewWrapperPane=function(title,widget)
{WebInspector.SidebarPane.call(this,title);widget.show(this.element);}
WebInspector.ElementsSidebarViewWrapperPane.prototype={__proto__:WebInspector.SidebarPane.prototype};WebInspector.ElementsTreeElement=function(node,elementCloseTag)
{TreeElement.call(this);this._node=node;this._gutterContainer=this.listItemElement.createChild("div","gutter-container");this._gutterContainer.addEventListener("click",this._showContextMenu.bind(this));this._decorationsElement=this._gutterContainer.createChild("div","hidden");this._elementCloseTag=elementCloseTag;if(this._node.nodeType()==Node.ELEMENT_NODE&&!elementCloseTag)
this._canAddAttributes=true;this._searchQuery=null;this._expandedChildrenLimit=WebInspector.ElementsTreeElement.InitialChildrenLimit;}
WebInspector.ElementsTreeElement.InitialChildrenLimit=500;WebInspector.ElementsTreeElement.ForbiddenClosingTagElements=["area","base","basefont","br","canvas","col","command","embed","frame","hr","img","input","keygen","link","menuitem","meta","param","source","track","wbr"].keySet();WebInspector.ElementsTreeElement.EditTagBlacklist=["html","head","body"].keySet();WebInspector.ElementsTreeElement.animateOnDOMUpdate=function(treeElement)
{var tagName=treeElement.listItemElement.querySelector(".webkit-html-tag-name");WebInspector.runCSSAnimationOnce(tagName||treeElement.listItemElement,"dom-update-highlight");}
WebInspector.ElementsTreeElement.visibleShadowRoots=function(node)
{var roots=node.shadowRoots();if(roots.length&&!WebInspector.moduleSetting("showUAShadowDOM").get())
roots=roots.filter(filter);function filter(root)
{return root.shadowRootType()!==WebInspector.DOMNode.ShadowRootTypes.UserAgent;}
return roots;}
WebInspector.ElementsTreeElement.canShowInlineText=function(node)
{if(node.importedDocument()||node.templateContent()||WebInspector.ElementsTreeElement.visibleShadowRoots(node).length||node.hasPseudoElements())
return false;if(node.nodeType()!==Node.ELEMENT_NODE)
return false;if(!node.firstChild||node.firstChild!==node.lastChild||node.firstChild.nodeType()!==Node.TEXT_NODE)
return false;var textChild=node.firstChild;var maxInlineTextChildLength=80;if(textChild.nodeValue().length<maxInlineTextChildLength)
return true;return false;}
WebInspector.ElementsTreeElement.populateForcedPseudoStateItems=function(subMenu,node)
{const pseudoClasses=["active","hover","focus","visited"];var forcedPseudoState=WebInspector.CSSStyleModel.fromNode(node).pseudoState(node);for(var i=0;i<pseudoClasses.length;++i){var pseudoClassForced=forcedPseudoState.indexOf(pseudoClasses[i])>=0;subMenu.appendCheckboxItem(":"+pseudoClasses[i],setPseudoStateCallback.bind(null,pseudoClasses[i],!pseudoClassForced),pseudoClassForced,false);}
function setPseudoStateCallback(pseudoState,enabled)
{WebInspector.CSSStyleModel.fromNode(node).forcePseudoState(node,pseudoState,enabled);}}
WebInspector.ElementsTreeElement.prototype={isClosingTag:function()
{return!!this._elementCloseTag;},node:function()
{return this._node;},isEditing:function()
{return!!this._editing;},highlightSearchResults:function(searchQuery)
{if(this._searchQuery!==searchQuery)
this._hideSearchHighlight();this._searchQuery=searchQuery;this._searchHighlightsVisible=true;this.updateTitle(null,true);},hideSearchHighlights:function()
{delete this._searchHighlightsVisible;this._hideSearchHighlight();},_hideSearchHighlight:function()
{if(!this._highlightResult)
return;function updateEntryHide(entry)
{switch(entry.type){case"added":entry.node.remove();break;case"changed":entry.node.textContent=entry.oldText;break;}}
for(var i=(this._highlightResult.length-1);i>=0;--i)
updateEntryHide(this._highlightResult[i]);delete this._highlightResult;},setInClipboard:function(inClipboard)
{if(this._inClipboard===inClipboard)
return;this._inClipboard=inClipboard;this.listItemElement.classList.toggle("in-clipboard",inClipboard);},get hovered()
{return this._hovered;},set hovered(x)
{if(this._hovered===x)
return;this._hovered=x;if(this.listItemElement){if(x){this.updateSelection();this.listItemElement.classList.add("hovered");}else{this.listItemElement.classList.remove("hovered");}}},expandedChildrenLimit:function()
{return this._expandedChildrenLimit;},setExpandedChildrenLimit:function(expandedChildrenLimit)
{this._expandedChildrenLimit=expandedChildrenLimit;},updateSelection:function()
{var listItemElement=this.listItemElement;if(!listItemElement)
return;if(!this.selectionElement){this.selectionElement=createElement("div");this.selectionElement.className="selection fill";listItemElement.insertBefore(this.selectionElement,listItemElement.firstChild);}},onbind:function()
{if(!this._elementCloseTag)
this._node[this.treeOutline.treeElementSymbol()]=this;},onunbind:function()
{if(this._node[this.treeOutline.treeElementSymbol()]===this)
this._node[this.treeOutline.treeElementSymbol()]=null;},onattach:function()
{if(this._hovered){this.updateSelection();this.listItemElement.classList.add("hovered");}
this.updateTitle();this._preventFollowingLinksOnDoubleClick();this.listItemElement.draggable=true;},_preventFollowingLinksOnDoubleClick:function()
{var links=this.listItemElement.querySelectorAll("li .webkit-html-tag > .webkit-html-attribute > .webkit-html-external-link, li .webkit-html-tag > .webkit-html-attribute > .webkit-html-resource-link");if(!links)
return;for(var i=0;i<links.length;++i)
links[i].preventFollowOnDoubleClick=true;},onpopulate:function()
{this.populated=true;this.treeOutline.populateTreeElement(this);},expandRecursively:function()
{function callback()
{TreeElement.prototype.expandRecursively.call(this,Number.MAX_VALUE);}
this._node.getSubtree(-1,callback.bind(this));},onexpand:function()
{if(this._elementCloseTag)
return;this.updateTitle();this.treeOutline.updateSelection();},oncollapse:function()
{if(this._elementCloseTag)
return;this.updateTitle();this.treeOutline.updateSelection();},onreveal:function()
{if(this.listItemElement){var tagSpans=this.listItemElement.getElementsByClassName("webkit-html-tag-name");if(tagSpans.length)
tagSpans[0].scrollIntoViewIfNeeded(true);else
this.listItemElement.scrollIntoViewIfNeeded(true);}},select:function(omitFocus,selectedByUser)
{if(this._editing)
return false;if(selectedByUser&&this.treeOutline.handlePickNode(this.title,this._node))
return true;return TreeElement.prototype.select.call(this,omitFocus,selectedByUser);},onselect:function(selectedByUser)
{this.treeOutline.suppressRevealAndSelect=true;this.treeOutline.selectDOMNode(this._node,selectedByUser);if(selectedByUser)
this._node.highlight();this.updateSelection();this.treeOutline.suppressRevealAndSelect=false;return true;},ondelete:function()
{var startTagTreeElement=this.treeOutline.findTreeElement(this._node);startTagTreeElement?startTagTreeElement.remove():this.remove();return true;},onenter:function()
{if(this._editing)
return false;this._startEditing();return true;},selectOnMouseDown:function(event)
{TreeElement.prototype.selectOnMouseDown.call(this,event);if(this._editing)
return;if(event.detail>=2)
event.preventDefault();},ondblclick:function(event)
{if(this._editing||this._elementCloseTag)
return false;if(this._startEditingTarget((event.target)))
return false;if(this.isExpandable()&&!this.expanded)
this.expand();return false;},hasEditableNode:function()
{return!this._node.isShadowRoot()&&!this._node.ancestorUserAgentShadowRoot();},_insertInLastAttributePosition:function(tag,node)
{if(tag.getElementsByClassName("webkit-html-attribute").length>0)
tag.insertBefore(node,tag.lastChild);else{var nodeName=tag.textContent.match(/^<(.*?)>$/)[1];tag.textContent='';tag.createTextChild('<'+nodeName);tag.appendChild(node);tag.createTextChild('>');}
this.updateSelection();},_startEditingTarget:function(eventTarget)
{if(this.treeOutline.selectedDOMNode()!=this._node)
return false;if(this._node.nodeType()!=Node.ELEMENT_NODE&&this._node.nodeType()!=Node.TEXT_NODE)
return false;if(this.treeOutline.pickNodeMode())
return false;var textNode=eventTarget.enclosingNodeOrSelfWithClass("webkit-html-text-node");if(textNode)
return this._startEditingTextNode(textNode);var attribute=eventTarget.enclosingNodeOrSelfWithClass("webkit-html-attribute");if(attribute)
return this._startEditingAttribute(attribute,eventTarget);var tagName=eventTarget.enclosingNodeOrSelfWithClass("webkit-html-tag-name");if(tagName)
return this._startEditingTagName(tagName);var newAttribute=eventTarget.enclosingNodeOrSelfWithClass("add-attribute");if(newAttribute)
return this._addNewAttribute();return false;},_showContextMenu:function(event)
{this.treeOutline.showContextMenu(this,event);},populateTagContextMenu:function(contextMenu,event)
{var treeElement=this._elementCloseTag?this.treeOutline.findTreeElement(this._node):this;contextMenu.appendItem(WebInspector.UIString.capitalize("Add ^attribute"),treeElement._addNewAttribute.bind(treeElement));var attribute=event.target.enclosingNodeOrSelfWithClass("webkit-html-attribute");var newAttribute=event.target.enclosingNodeOrSelfWithClass("add-attribute");if(attribute&&!newAttribute)
contextMenu.appendItem(WebInspector.UIString.capitalize("Edit ^attribute"),this._startEditingAttribute.bind(this,attribute,event.target));this.populateNodeContextMenu(contextMenu);WebInspector.ElementsTreeElement.populateForcedPseudoStateItems(contextMenu,treeElement.node());contextMenu.appendSeparator();this.populateScrollIntoView(contextMenu);},populateScrollIntoView:function(contextMenu)
{contextMenu.appendItem(WebInspector.UIString.capitalize("Scroll into ^view"),this._scrollIntoView.bind(this));},populateTextContextMenu:function(contextMenu,textNode)
{if(!this._editing)
contextMenu.appendItem(WebInspector.UIString.capitalize("Edit ^text"),this._startEditingTextNode.bind(this,textNode));this.populateNodeContextMenu(contextMenu);},populateNodeContextMenu:function(contextMenu)
{var openTagElement=this._node[this.treeOutline.treeElementSymbol()]||this;var isEditable=this.hasEditableNode();if(isEditable&&!this._editing)
contextMenu.appendAction("elements.edit-as-html",WebInspector.UIString("Edit as HTML"));var isShadowRoot=this._node.isShadowRoot();var copyMenu=contextMenu.appendSubMenuItem(WebInspector.UIString("Copy"));var createShortcut=WebInspector.KeyboardShortcut.shortcutToString;var modifier=WebInspector.KeyboardShortcut.Modifiers.CtrlOrMeta;var menuItem;if(!isShadowRoot)
menuItem=copyMenu.appendItem(WebInspector.UIString("Copy outerHTML"),this.treeOutline.performCopyOrCut.bind(this.treeOutline,false,this._node));menuItem.setShortcut(createShortcut("V",modifier));if(this._node.nodeType()===Node.ELEMENT_NODE)
copyMenu.appendItem(WebInspector.UIString.capitalize("Copy selector"),this._copyCSSPath.bind(this));if(!isShadowRoot)
copyMenu.appendItem(WebInspector.UIString("Copy XPath"),this._copyXPath.bind(this));if(!isShadowRoot){var treeOutline=this.treeOutline;menuItem=copyMenu.appendItem(WebInspector.UIString("Cut element"),treeOutline.performCopyOrCut.bind(treeOutline,true,this._node),!this.hasEditableNode());menuItem.setShortcut(createShortcut("X",modifier));menuItem=copyMenu.appendItem(WebInspector.UIString("Copy element"),treeOutline.performCopyOrCut.bind(treeOutline,false,this._node));menuItem.setShortcut(createShortcut("C",modifier));menuItem=copyMenu.appendItem(WebInspector.UIString("Paste element"),treeOutline.pasteNode.bind(treeOutline,this._node),!treeOutline.canPaste(this._node));menuItem.setShortcut(createShortcut("V",modifier));}
contextMenu.appendSeparator();menuItem=contextMenu.appendCheckboxItem(WebInspector.UIString("Hide element"),this.treeOutline.toggleHideElement.bind(this.treeOutline,this._node),this.treeOutline.isToggledToHidden(this._node));menuItem.setShortcut(WebInspector.shortcutRegistry.shortcutTitleForAction("elements.hide-element"));if(isEditable)
contextMenu.appendItem(WebInspector.UIString("Delete element"),this.remove.bind(this));contextMenu.appendSeparator();},_startEditing:function()
{if(this.treeOutline.selectedDOMNode()!==this._node)
return;var listItem=this._listItemNode;if(this._canAddAttributes){var attribute=listItem.getElementsByClassName("webkit-html-attribute")[0];if(attribute)
return this._startEditingAttribute(attribute,attribute.getElementsByClassName("webkit-html-attribute-value")[0]);return this._addNewAttribute();}
if(this._node.nodeType()===Node.TEXT_NODE){var textNode=listItem.getElementsByClassName("webkit-html-text-node")[0];if(textNode)
return this._startEditingTextNode(textNode);return;}},_addNewAttribute:function()
{var container=createElement("span");this._buildAttributeDOM(container," ","",null);var attr=container.firstElementChild;attr.style.marginLeft="2px";attr.style.marginRight="2px";var tag=this.listItemElement.getElementsByClassName("webkit-html-tag")[0];this._insertInLastAttributePosition(tag,attr);attr.scrollIntoViewIfNeeded(true);return this._startEditingAttribute(attr,attr);},_triggerEditAttribute:function(attributeName)
{var attributeElements=this.listItemElement.getElementsByClassName("webkit-html-attribute-name");for(var i=0,len=attributeElements.length;i<len;++i){if(attributeElements[i].textContent===attributeName){for(var elem=attributeElements[i].nextSibling;elem;elem=elem.nextSibling){if(elem.nodeType!==Node.ELEMENT_NODE)
continue;if(elem.classList.contains("webkit-html-attribute-value"))
return this._startEditingAttribute(elem.parentNode,elem);}}}},_startEditingAttribute:function(attribute,elementForSelection)
{console.assert(this.listItemElement.isAncestor(attribute));if(WebInspector.isBeingEdited(attribute))
return true;var attributeNameElement=attribute.getElementsByClassName("webkit-html-attribute-name")[0];if(!attributeNameElement)
return false;var attributeName=attributeNameElement.textContent;var attributeValueElement=attribute.getElementsByClassName("webkit-html-attribute-value")[0];elementForSelection=attributeValueElement.isAncestor(elementForSelection)?attributeValueElement:elementForSelection;function removeZeroWidthSpaceRecursive(node)
{if(node.nodeType===Node.TEXT_NODE){node.nodeValue=node.nodeValue.replace(/\u200B/g,"");return;}
if(node.nodeType!==Node.ELEMENT_NODE)
return;for(var child=node.firstChild;child;child=child.nextSibling)
removeZeroWidthSpaceRecursive(child);}
var attributeValue=attributeName&&attributeValueElement?this._node.getAttribute(attributeName):undefined;if(attributeValue!==undefined)
attributeValueElement.setTextContentTruncatedIfNeeded(attributeValue,WebInspector.UIString("<value is too large to edit>"));removeZeroWidthSpaceRecursive(attribute);var config=new WebInspector.InplaceEditor.Config(this._attributeEditingCommitted.bind(this),this._editingCancelled.bind(this),attributeName);function postKeyDownFinishHandler(event)
{WebInspector.handleElementValueModifications(event,attribute);return"";}
config.setPostKeydownFinishHandler(postKeyDownFinishHandler);this._editing=WebInspector.InplaceEditor.startEditing(attribute,config);this.listItemElement.getComponentSelection().setBaseAndExtent(elementForSelection,0,elementForSelection,1);return true;},_startEditingTextNode:function(textNodeElement)
{if(WebInspector.isBeingEdited(textNodeElement))
return true;var textNode=this._node;if(textNode.nodeType()===Node.ELEMENT_NODE&&textNode.firstChild)
textNode=textNode.firstChild;var container=textNodeElement.enclosingNodeOrSelfWithClass("webkit-html-text-node");if(container)
container.textContent=textNode.nodeValue();var config=new WebInspector.InplaceEditor.Config(this._textNodeEditingCommitted.bind(this,textNode),this._editingCancelled.bind(this));this._editing=WebInspector.InplaceEditor.startEditing(textNodeElement,config);this.listItemElement.getComponentSelection().setBaseAndExtent(textNodeElement,0,textNodeElement,1);return true;},_startEditingTagName:function(tagNameElement)
{if(!tagNameElement){tagNameElement=this.listItemElement.getElementsByClassName("webkit-html-tag-name")[0];if(!tagNameElement)
return false;}
var tagName=tagNameElement.textContent;if(WebInspector.ElementsTreeElement.EditTagBlacklist[tagName.toLowerCase()])
return false;if(WebInspector.isBeingEdited(tagNameElement))
return true;var closingTagElement=this._distinctClosingTagElement();function keyupListener(event)
{if(closingTagElement)
closingTagElement.textContent="</"+tagNameElement.textContent+">";}
function editingComitted(element,newTagName)
{tagNameElement.removeEventListener('keyup',keyupListener,false);this._tagNameEditingCommitted.apply(this,arguments);}
function editingCancelled()
{tagNameElement.removeEventListener('keyup',keyupListener,false);this._editingCancelled.apply(this,arguments);}
tagNameElement.addEventListener('keyup',keyupListener,false);var config=new WebInspector.InplaceEditor.Config(editingComitted.bind(this),editingCancelled.bind(this),tagName);this._editing=WebInspector.InplaceEditor.startEditing(tagNameElement,config);this.listItemElement.getComponentSelection().setBaseAndExtent(tagNameElement,0,tagNameElement,1);return true;},_startEditingAsHTML:function(commitCallback,disposeCallback,error,initialValue)
{if(error)
return;if(this._editing)
return;function consume(event)
{if(event.eventPhase===Event.AT_TARGET)
event.consume(true);}
initialValue=this._convertWhitespaceToEntities(initialValue).text;this._htmlEditElement=createElement("div");this._htmlEditElement.className="source-code elements-tree-editor";var child=this.listItemElement.firstChild;while(child){child.style.display="none";child=child.nextSibling;}
if(this._childrenListNode)
this._childrenListNode.style.display="none";this.listItemElement.appendChild(this._htmlEditElement);this.treeOutline.element.addEventListener("mousedown",consume,false);this.updateSelection();function commit(element,newValue)
{commitCallback(initialValue,newValue);dispose.call(this);}
function dispose()
{disposeCallback();delete this._editing;this.treeOutline.setMultilineEditing(null);this.listItemElement.removeChild(this._htmlEditElement);delete this._htmlEditElement;if(this._childrenListNode)
this._childrenListNode.style.removeProperty("display");var child=this.listItemElement.firstChild;while(child){child.style.removeProperty("display");child=child.nextSibling;}
this.treeOutline.element.removeEventListener("mousedown",consume,false);this.updateSelection();this.treeOutline.focus();}
var config=new WebInspector.InplaceEditor.Config(commit.bind(this),dispose.bind(this));config.setMultilineOptions(initialValue,{name:"xml",htmlMode:true},"web-inspector-html",WebInspector.moduleSetting("domWordWrap").get(),true);WebInspector.InplaceEditor.startMultilineEditing(this._htmlEditElement,config).then(markAsBeingEdited.bind(this));function markAsBeingEdited(controller)
{this._editing=(controller);this._editing.setWidth(this.treeOutline.visibleWidth());this.treeOutline.setMultilineEditing(this._editing);}},_attributeEditingCommitted:function(element,newText,oldText,attributeName,moveDirection)
{delete this._editing;var treeOutline=this.treeOutline;function moveToNextAttributeIfNeeded(error)
{if(error)
this._editingCancelled(element,attributeName);if(!moveDirection)
return;treeOutline.runPendingUpdates();var attributes=this._node.attributes();for(var i=0;i<attributes.length;++i){if(attributes[i].name!==attributeName)
continue;if(moveDirection==="backward"){if(i===0)
this._startEditingTagName();else
this._triggerEditAttribute(attributes[i-1].name);}else{if(i===attributes.length-1)
this._addNewAttribute();else
this._triggerEditAttribute(attributes[i+1].name);}
return;}
if(moveDirection==="backward"){if(newText===" "){if(attributes.length>0)
this._triggerEditAttribute(attributes[attributes.length-1].name);}else{if(attributes.length>1)
this._triggerEditAttribute(attributes[attributes.length-2].name);}}else if(moveDirection==="forward"){if(!/^\s*$/.test(newText))
this._addNewAttribute();else
this._startEditingTagName();}}
if((attributeName.trim()||newText.trim())&&oldText!==newText){this._node.setAttribute(attributeName,newText,moveToNextAttributeIfNeeded.bind(this));return;}
this.updateTitle();moveToNextAttributeIfNeeded.call(this);},_tagNameEditingCommitted:function(element,newText,oldText,tagName,moveDirection)
{delete this._editing;var self=this;function cancel()
{var closingTagElement=self._distinctClosingTagElement();if(closingTagElement)
closingTagElement.textContent="</"+tagName+">";self._editingCancelled(element,tagName);moveToNextAttributeIfNeeded.call(self);}
function moveToNextAttributeIfNeeded()
{if(moveDirection!=="forward"){this._addNewAttribute();return;}
var attributes=this._node.attributes();if(attributes.length>0)
this._triggerEditAttribute(attributes[0].name);else
this._addNewAttribute();}
newText=newText.trim();if(newText===oldText){cancel();return;}
var treeOutline=this.treeOutline;var wasExpanded=this.expanded;function changeTagNameCallback(error,nodeId)
{if(error||!nodeId){cancel();return;}
var newTreeItem=treeOutline.selectNodeAfterEdit(wasExpanded,error,nodeId);moveToNextAttributeIfNeeded.call(newTreeItem);}
this._node.setNodeName(newText,changeTagNameCallback);},_textNodeEditingCommitted:function(textNode,element,newText)
{delete this._editing;function callback()
{this.updateTitle();}
textNode.setNodeValue(newText,callback.bind(this));},_editingCancelled:function(element,context)
{delete this._editing;this.updateTitle();},_distinctClosingTagElement:function()
{if(this.expanded){var closers=this._childrenListNode.querySelectorAll(".close");return closers[closers.length-1];}
var tags=this.listItemElement.getElementsByClassName("webkit-html-tag");return(tags.length===1?null:tags[tags.length-1]);},updateTitle:function(updateRecord,onlySearchQueryChanged)
{if(this._editing)
return;if(onlySearchQueryChanged){this._hideSearchHighlight();}else{var nodeInfo=this._nodeTitleInfo(updateRecord||null);if(this._node.nodeType()===Node.DOCUMENT_FRAGMENT_NODE&&this._node.isInShadowTree()&&this._node.shadowRootType()){this.childrenListElement.classList.add("shadow-root");var depth=4;for(var node=this._node;depth&&node;node=node.parentNode){if(node.nodeType()===Node.DOCUMENT_FRAGMENT_NODE)
depth--;}
if(!depth)
this.childrenListElement.classList.add("shadow-root-deep");else
this.childrenListElement.classList.add("shadow-root-depth-"+depth);}
var highlightElement=createElement("span");highlightElement.className="highlight";highlightElement.appendChild(nodeInfo);this.title=highlightElement;this.updateDecorations();this.listItemElement.insertBefore(this._gutterContainer,this.listItemElement.firstChild);delete this._highlightResult;}
delete this.selectionElement;if(this.selected)
this.updateSelection();this._preventFollowingLinksOnDoubleClick();this._highlightSearchResults();},updateDecorations:function()
{var treeElement=this.parent;var depth=0;while(treeElement!=null){depth++;treeElement=treeElement.parent;}
this._gutterContainer.style.left=(-12*(depth-2)-(this.isExpandable()?1:12))+"px";if(this.isClosingTag())
return;var node=this._node;if(node.nodeType()!==Node.ELEMENT_NODE)
return;if(!this.treeOutline._decoratorExtensions)
this.treeOutline._decoratorExtensions=runtime.extensions(WebInspector.DOMPresentationUtils.MarkerDecorator);var markerToExtension=new Map();for(var i=0;i<this.treeOutline._decoratorExtensions.length;++i)
markerToExtension.set(this.treeOutline._decoratorExtensions[i].descriptor()["marker"],this.treeOutline._decoratorExtensions[i]);var promises=[];var decorations=[];var descendantDecorations=[];node.traverseMarkers(visitor);function visitor(n,marker)
{var extension=markerToExtension.get(marker);if(!extension)
return;promises.push(extension.instancePromise().then(collectDecoration.bind(null,n)));}
function collectDecoration(n,decorator)
{var decoration=decorator.decorate(n);if(!decoration)
return;(n===node?decorations:descendantDecorations).push(decoration);}
Promise.all(promises).then(updateDecorationsUI.bind(this));function updateDecorationsUI()
{this._decorationsElement.removeChildren();this._decorationsElement.classList.add("hidden");this._gutterContainer.classList.toggle("has-decorations",decorations.length||descendantDecorations.length);if(!decorations.length&&!descendantDecorations.length)
return;var colors=new Set();var titles=createElement("div");for(var decoration of decorations){var titleElement=titles.createChild("div");titleElement.textContent=decoration.title;colors.add(decoration.color);}
if(this.expanded&&!decorations.length)
return;var descendantColors=new Set();if(descendantDecorations.length){var element=titles.createChild("div");element.textContent=WebInspector.UIString("Children:");for(var decoration of descendantDecorations){element=titles.createChild("div");element.style.marginLeft="15px";element.textContent=decoration.title;descendantColors.add(decoration.color);}}
var offset=0;processColors.call(this,colors,"elements-gutter-decoration");if(!this.expanded)
processColors.call(this,descendantColors,"elements-gutter-decoration elements-has-decorated-children");WebInspector.Tooltip.install(this._decorationsElement,titles);function processColors(colors,className)
{for(var color of colors){var child=this._decorationsElement.createChild("div",className);this._decorationsElement.classList.remove("hidden");child.style.backgroundColor=color;child.style.borderColor=color;if(offset)
child.style.marginLeft=offset+"px";offset+=3;}}}},_buildAttributeDOM:function(parentElement,name,value,updateRecord,forceValue,node)
{var closingPunctuationRegex=/[\/;:\)\]\}]/g;var highlightIndex=0;var highlightCount;var additionalHighlightOffset=0;var result;function replacer(match,replaceOffset){while(highlightIndex<highlightCount&&result.entityRanges[highlightIndex].offset<replaceOffset){result.entityRanges[highlightIndex].offset+=additionalHighlightOffset;++highlightIndex;}
additionalHighlightOffset+=1;return match+"\u200B";}
function setValueWithEntities(element,value)
{result=this._convertWhitespaceToEntities(value);highlightCount=result.entityRanges.length;value=result.text.replace(closingPunctuationRegex,replacer);while(highlightIndex<highlightCount){result.entityRanges[highlightIndex].offset+=additionalHighlightOffset;++highlightIndex;}
element.setTextContentTruncatedIfNeeded(value);WebInspector.highlightRangesWithStyleClass(element,result.entityRanges,"webkit-html-entity-value");}
var hasText=(forceValue||value.length>0);var attrSpanElement=parentElement.createChild("span","webkit-html-attribute");var attrNameElement=attrSpanElement.createChild("span","webkit-html-attribute-name");attrNameElement.textContent=name;if(hasText)
attrSpanElement.createTextChild("=\u200B\"");var attrValueElement=attrSpanElement.createChild("span","webkit-html-attribute-value");if(updateRecord&&updateRecord.isAttributeModified(name))
WebInspector.runCSSAnimationOnce(hasText?attrValueElement:attrNameElement,"dom-update-highlight");function linkifyValue(value)
{var rewrittenHref=node.resolveURL(value);if(rewrittenHref===null){var span=createElement("span");setValueWithEntities.call(this,span,value);return span;}
value=value.replace(closingPunctuationRegex,"$&\u200B");if(value.startsWith("data:"))
value=value.trimMiddle(60);var anchor=WebInspector.linkifyURLAsNode(rewrittenHref,value,"",node.nodeName().toLowerCase()==="a");anchor.preventFollow=true;return anchor;}
if(node&&name==="src"||name==="href"){attrValueElement.appendChild(linkifyValue.call(this,value));}else if(node&&node.nodeName().toLowerCase()==="img"&&name==="srcset"){var sources=value.split(",");for(var i=0;i<sources.length;++i){if(i>0)
attrValueElement.createTextChild(", ");var source=sources[i].trim();var indexOfSpace=source.indexOf(" ");var url=source.substring(0,indexOfSpace);var tail=source.substring(indexOfSpace);attrValueElement.appendChild(linkifyValue.call(this,url));attrValueElement.createTextChild(tail);}}else{setValueWithEntities.call(this,attrValueElement,value);}
if(hasText)
attrSpanElement.createTextChild("\"");},_buildPseudoElementDOM:function(parentElement,pseudoElementName)
{var pseudoElement=parentElement.createChild("span","webkit-html-pseudo-element");pseudoElement.textContent="::"+pseudoElementName;parentElement.createTextChild("\u200B");},_buildTagDOM:function(parentElement,tagName,isClosingTag,isDistinctTreeElement,updateRecord)
{var node=this._node;var classes=["webkit-html-tag"];if(isClosingTag&&isDistinctTreeElement)
classes.push("close");var tagElement=parentElement.createChild("span",classes.join(" "));tagElement.createTextChild("<");var tagNameElement=tagElement.createChild("span",isClosingTag?"webkit-html-close-tag-name":"webkit-html-tag-name");tagNameElement.textContent=(isClosingTag?"/":"")+tagName;if(!isClosingTag){if(node.hasAttributes()){var attributes=node.attributes();for(var i=0;i<attributes.length;++i){var attr=attributes[i];tagElement.createTextChild(" ");this._buildAttributeDOM(tagElement,attr.name,attr.value,updateRecord,false,node);}}
if(updateRecord){var hasUpdates=updateRecord.hasRemovedAttributes()||updateRecord.hasRemovedChildren();hasUpdates|=!this.expanded&&updateRecord.hasChangedChildren();if(hasUpdates)
WebInspector.runCSSAnimationOnce(tagNameElement,"dom-update-highlight");}}
tagElement.createTextChild(">");parentElement.createTextChild("\u200B");},_convertWhitespaceToEntities:function(text)
{var result="";var lastIndexAfterEntity=0;var entityRanges=[];var charToEntity=WebInspector.ElementsTreeOutline.MappedCharToEntity;for(var i=0,size=text.length;i<size;++i){var char=text.charAt(i);if(charToEntity[char]){result+=text.substring(lastIndexAfterEntity,i);var entityValue="&"+charToEntity[char]+";";entityRanges.push({offset:result.length,length:entityValue.length});result+=entityValue;lastIndexAfterEntity=i+1;}}
if(result)
result+=text.substring(lastIndexAfterEntity);return{text:result||text,entityRanges:entityRanges};},_nodeTitleInfo:function(updateRecord)
{var node=this._node;var titleDOM=createDocumentFragment();switch(node.nodeType()){case Node.ATTRIBUTE_NODE:this._buildAttributeDOM(titleDOM,(node.name),(node.value),updateRecord,true);break;case Node.ELEMENT_NODE:var pseudoType=node.pseudoType();if(pseudoType){this._buildPseudoElementDOM(titleDOM,pseudoType);break;}
var tagName=node.nodeNameInCorrectCase();if(this._elementCloseTag){this._buildTagDOM(titleDOM,tagName,true,true,updateRecord);break;}
this._buildTagDOM(titleDOM,tagName,false,false,updateRecord);if(this.isExpandable()){if(!this.expanded){var textNodeElement=titleDOM.createChild("span","webkit-html-text-node bogus");textNodeElement.textContent="\u2026";titleDOM.createTextChild("\u200B");this._buildTagDOM(titleDOM,tagName,true,false,updateRecord);}
break;}
if(WebInspector.ElementsTreeElement.canShowInlineText(node)){var textNodeElement=titleDOM.createChild("span","webkit-html-text-node");var result=this._convertWhitespaceToEntities(node.firstChild.nodeValue());textNodeElement.textContent=result.text;WebInspector.highlightRangesWithStyleClass(textNodeElement,result.entityRanges,"webkit-html-entity-value");titleDOM.createTextChild("\u200B");this._buildTagDOM(titleDOM,tagName,true,false,updateRecord);if(updateRecord&&updateRecord.hasChangedChildren())
WebInspector.runCSSAnimationOnce(textNodeElement,"dom-update-highlight");if(updateRecord&&updateRecord.isCharDataModified())
WebInspector.runCSSAnimationOnce(textNodeElement,"dom-update-highlight");break;}
if(this.treeOutline.isXMLMimeType||!WebInspector.ElementsTreeElement.ForbiddenClosingTagElements[tagName])
this._buildTagDOM(titleDOM,tagName,true,false,updateRecord);break;case Node.TEXT_NODE:if(node.parentNode&&node.parentNode.nodeName().toLowerCase()==="script"){var newNode=titleDOM.createChild("span","webkit-html-text-node webkit-html-js-node");newNode.textContent=node.nodeValue();var javascriptSyntaxHighlighter=new WebInspector.DOMSyntaxHighlighter("text/javascript",true);javascriptSyntaxHighlighter.syntaxHighlightNode(newNode).then(updateSearchHighlight.bind(this));}else if(node.parentNode&&node.parentNode.nodeName().toLowerCase()==="style"){var newNode=titleDOM.createChild("span","webkit-html-text-node webkit-html-css-node");newNode.textContent=node.nodeValue();var cssSyntaxHighlighter=new WebInspector.DOMSyntaxHighlighter("text/css",true);cssSyntaxHighlighter.syntaxHighlightNode(newNode).then(updateSearchHighlight.bind(this));}else{titleDOM.createTextChild("\"");var textNodeElement=titleDOM.createChild("span","webkit-html-text-node");var result=this._convertWhitespaceToEntities(node.nodeValue());textNodeElement.textContent=result.text;WebInspector.highlightRangesWithStyleClass(textNodeElement,result.entityRanges,"webkit-html-entity-value");titleDOM.createTextChild("\"");if(updateRecord&&updateRecord.isCharDataModified())
WebInspector.runCSSAnimationOnce(textNodeElement,"dom-update-highlight");}
break;case Node.COMMENT_NODE:var commentElement=titleDOM.createChild("span","webkit-html-comment");commentElement.createTextChild("<!--"+node.nodeValue()+"-->");break;case Node.DOCUMENT_TYPE_NODE:var docTypeElement=titleDOM.createChild("span","webkit-html-doctype");docTypeElement.createTextChild("<!DOCTYPE "+node.nodeName());if(node.publicId){docTypeElement.createTextChild(" PUBLIC \""+node.publicId+"\"");if(node.systemId)
docTypeElement.createTextChild(" \""+node.systemId+"\"");}else if(node.systemId)
docTypeElement.createTextChild(" SYSTEM \""+node.systemId+"\"");if(node.internalSubset)
docTypeElement.createTextChild(" ["+node.internalSubset+"]");docTypeElement.createTextChild(">");break;case Node.CDATA_SECTION_NODE:var cdataElement=titleDOM.createChild("span","webkit-html-text-node");cdataElement.createTextChild("<![CDATA["+node.nodeValue()+"]]>");break;case Node.DOCUMENT_FRAGMENT_NODE:var fragmentElement=titleDOM.createChild("span","webkit-html-fragment");fragmentElement.textContent=node.nodeNameInCorrectCase().collapseWhitespace();break;default:titleDOM.createTextChild(node.nodeNameInCorrectCase().collapseWhitespace());}
function updateSearchHighlight()
{delete this._highlightResult;this._highlightSearchResults();}
return titleDOM;},remove:function()
{if(this._node.pseudoType())
return;var parentElement=this.parent;if(!parentElement)
return;if(!this._node.parentNode||this._node.parentNode.nodeType()===Node.DOCUMENT_NODE)
return;this._node.removeNode();},toggleEditAsHTML:function(callback,startEditing)
{if(this._editing&&this._htmlEditElement&&WebInspector.isBeingEdited(this._htmlEditElement)){this._editing.commit();return;}
if(startEditing===false)
return;function selectNode(error)
{if(callback)
callback(!error);}
function commitChange(initialValue,value)
{if(initialValue!==value)
node.setOuterHTML(value,selectNode);}
function disposeCallback()
{if(callback)
callback(false);}
var node=this._node;node.getOuterHTML(this._startEditingAsHTML.bind(this,commitChange,disposeCallback));},_copyCSSPath:function()
{InspectorFrontendHost.copyText(WebInspector.DOMPresentationUtils.cssPath(this._node,true));},_copyXPath:function()
{InspectorFrontendHost.copyText(WebInspector.DOMPresentationUtils.xPath(this._node,true));},_highlightSearchResults:function()
{if(!this._searchQuery||!this._searchHighlightsVisible)
return;this._hideSearchHighlight();var text=this.listItemElement.textContent;var regexObject=createPlainTextSearchRegex(this._searchQuery,"gi");var match=regexObject.exec(text);var matchRanges=[];while(match){matchRanges.push(new WebInspector.SourceRange(match.index,match[0].length));match=regexObject.exec(text);}
if(!matchRanges.length)
matchRanges.push(new WebInspector.SourceRange(0,text.length));this._highlightResult=[];WebInspector.highlightSearchResults(this.listItemElement,matchRanges,this._highlightResult);},_scrollIntoView:function()
{function scrollIntoViewCallback(object)
{function scrollIntoView()
{this.scrollIntoViewIfNeeded(true);}
if(object)
object.callFunction(scrollIntoView);}
this._node.resolveToObject("",scrollIntoViewCallback);},__proto__:TreeElement.prototype};WebInspector.ElementsTreeOutline=function(domModel,omitRootDOMNode,selectEnabled)
{this._domModel=domModel;this._treeElementSymbol=Symbol("treeElement");var element=createElement("div");this._shadowRoot=WebInspector.createShadowRootWithCoreStyles(element);this._shadowRoot.appendChild(WebInspector.Widget.createStyleElement("elements/elementsTreeOutline.css"));var outlineDisclosureElement=this._shadowRoot.createChild("div","elements-disclosure");TreeOutline.call(this);this._element=this.element;this._element.classList.add("elements-tree-outline","source-code");this._element.addEventListener("mousedown",this._onmousedown.bind(this),false);this._element.addEventListener("mousemove",this._onmousemove.bind(this),false);this._element.addEventListener("mouseleave",this._onmouseleave.bind(this),false);this._element.addEventListener("dragstart",this._ondragstart.bind(this),false);this._element.addEventListener("dragover",this._ondragover.bind(this),false);this._element.addEventListener("dragleave",this._ondragleave.bind(this),false);this._element.addEventListener("drop",this._ondrop.bind(this),false);this._element.addEventListener("dragend",this._ondragend.bind(this),false);this._element.addEventListener("webkitAnimationEnd",this._onAnimationEnd.bind(this),false);this._element.addEventListener("contextmenu",this._contextMenuEventFired.bind(this),false);outlineDisclosureElement.appendChild(this._element);this.element=element;this._includeRootDOMNode=!omitRootDOMNode;this._selectEnabled=selectEnabled;this._rootDOMNode=null;this._selectedDOMNode=null;this._visible=false;this._pickNodeMode=false;this._popoverHelper=new WebInspector.PopoverHelper(this._element,this._getPopoverAnchor.bind(this),this._showPopover.bind(this));this._popoverHelper.setTimeout(0);this._updateRecords=new Map();this._treeElementsBeingUpdated=new Set();this._domModel.addEventListener(WebInspector.DOMModel.Events.MarkersChanged,this._markersChanged,this);}
WebInspector.ElementsTreeOutline.ClipboardData;WebInspector.ElementsTreeOutline.Events={NodePicked:"NodePicked",SelectedNodeChanged:"SelectedNodeChanged",ElementsTreeUpdated:"ElementsTreeUpdated"}
WebInspector.ElementsTreeOutline.MappedCharToEntity={"\u00a0":"nbsp","\u0093":"#147","\u00ad":"shy","\u2002":"ensp","\u2003":"emsp","\u2009":"thinsp","\u200a":"#8202","\u200b":"#8203","\u200c":"zwnj","\u200d":"zwj","\u200e":"lrm","\u200f":"rlm","\u202a":"#8234","\u202b":"#8235","\u202c":"#8236","\u202d":"#8237","\u202e":"#8238","\ufeff":"#65279"}
WebInspector.ElementsTreeOutline.prototype={treeElementSymbol:function()
{return this._treeElementSymbol;},focus:function()
{this._element.focus();},hasFocus:function()
{return this._element===WebInspector.currentFocusElement();},setWordWrap:function(wrap)
{this._element.classList.toggle("elements-tree-nowrap",!wrap);},_onAnimationEnd:function(event)
{event.target.classList.remove("elements-tree-element-pick-node-1");event.target.classList.remove("elements-tree-element-pick-node-2");},pickNodeMode:function()
{return this._pickNodeMode;},setPickNodeMode:function(value)
{this._pickNodeMode=value;this._element.classList.toggle("pick-node-mode",value);},handlePickNode:function(element,node)
{if(!this._pickNodeMode)
return false;this.dispatchEventToListeners(WebInspector.ElementsTreeOutline.Events.NodePicked,node);var hasRunningAnimation=element.classList.contains("elements-tree-element-pick-node-1")||element.classList.contains("elements-tree-element-pick-node-2");element.classList.toggle("elements-tree-element-pick-node-1");if(hasRunningAnimation)
element.classList.toggle("elements-tree-element-pick-node-2");return true;},domModel:function()
{return this._domModel;},setMultilineEditing:function(multilineEditing)
{this._multilineEditing=multilineEditing;},visibleWidth:function()
{return this._visibleWidth;},setVisibleWidth:function(width)
{this._visibleWidth=width;if(this._multilineEditing)
this._multilineEditing.setWidth(this._visibleWidth);},_setClipboardData:function(data)
{if(this._clipboardNodeData){var treeElement=this.findTreeElement(this._clipboardNodeData.node);if(treeElement)
treeElement.setInClipboard(false);delete this._clipboardNodeData;}
if(data){var treeElement=this.findTreeElement(data.node);if(treeElement)
treeElement.setInClipboard(true);this._clipboardNodeData=data;}},resetClipboardIfNeeded:function(removedNode)
{if(this._clipboardNodeData&&this._clipboardNodeData.node===removedNode)
this._setClipboardData(null);},handleCopyOrCutKeyboardEvent:function(isCut,event)
{this._setClipboardData(null);if(!event.target.isComponentSelectionCollapsed())
return;if(WebInspector.isEditing())
return;var targetNode=this.selectedDOMNode();if(!targetNode)
return;event.clipboardData.clearData();event.preventDefault();this.performCopyOrCut(isCut,targetNode);},performCopyOrCut:function(isCut,node)
{if(isCut&&(node.isShadowRoot()||node.ancestorUserAgentShadowRoot()))
return;node.copyNode();this._setClipboardData({node:node,isCut:isCut});},canPaste:function(targetNode)
{if(targetNode.isShadowRoot()||targetNode.ancestorUserAgentShadowRoot())
return false;if(!this._clipboardNodeData)
return false;var node=this._clipboardNodeData.node;if(this._clipboardNodeData.isCut&&(node===targetNode||node.isAncestor(targetNode)))
return false;if(targetNode.target()!==node.target())
return false;return true;},pasteNode:function(targetNode)
{if(this.canPaste(targetNode))
this._performPaste(targetNode);},handlePasteKeyboardEvent:function(event)
{if(WebInspector.isEditing())
return;var targetNode=this.selectedDOMNode();if(!targetNode||!this.canPaste(targetNode))
return;event.preventDefault();this._performPaste(targetNode);},_performPaste:function(targetNode)
{if(this._clipboardNodeData.isCut){this._clipboardNodeData.node.moveTo(targetNode,null,expandCallback.bind(this));this._setClipboardData(null);}else{this._clipboardNodeData.node.copyTo(targetNode,null,expandCallback.bind(this));}
function expandCallback(error,nodeId)
{if(error)
return;var pastedNode=this._domModel.nodeForId(nodeId);if(!pastedNode)
return;this.selectDOMNode(pastedNode);}},setVisible:function(visible)
{this._visible=visible;if(!this._visible){this._popoverHelper.hidePopover();if(this._multilineEditing)
this._multilineEditing.cancel();return;}
this.runPendingUpdates();if(this._selectedDOMNode)
this._revealAndSelectNode(this._selectedDOMNode,false);},get rootDOMNode()
{return this._rootDOMNode;},set rootDOMNode(x)
{if(this._rootDOMNode===x)
return;this._rootDOMNode=x;this._isXMLMimeType=x&&x.isXMLNode();this.update();},get isXMLMimeType()
{return this._isXMLMimeType;},selectedDOMNode:function()
{return this._selectedDOMNode;},selectDOMNode:function(node,focus)
{if(this._selectedDOMNode===node){this._revealAndSelectNode(node,!focus);return;}
this._selectedDOMNode=node;this._revealAndSelectNode(node,!focus);if(this._selectedDOMNode===node)
this._selectedNodeChanged();},editing:function()
{var node=this.selectedDOMNode();if(!node)
return false;var treeElement=this.findTreeElement(node);if(!treeElement)
return false;return treeElement.isEditing()||false;},update:function()
{var selectedTreeElement=this.selectedTreeElement;if(!(selectedTreeElement instanceof WebInspector.ElementsTreeElement))
selectedTreeElement=null;var selectedNode=selectedTreeElement?selectedTreeElement.node():null;this.removeChildren();if(!this.rootDOMNode)
return;var treeElement;if(this._includeRootDOMNode){treeElement=this._createElementTreeElement(this.rootDOMNode);this.appendChild(treeElement);}else{var node=this.rootDOMNode.firstChild;while(node){treeElement=this._createElementTreeElement(node);this.appendChild(treeElement);node=node.nextSibling;}}
if(selectedNode)
this._revealAndSelectNode(selectedNode,true);},updateSelection:function()
{if(!this.selectedTreeElement)
return;var element=this.selectedTreeElement;element.updateSelection();},_selectedNodeChanged:function()
{this.dispatchEventToListeners(WebInspector.ElementsTreeOutline.Events.SelectedNodeChanged,this._selectedDOMNode);},_fireElementsTreeUpdated:function(nodes)
{this.dispatchEventToListeners(WebInspector.ElementsTreeOutline.Events.ElementsTreeUpdated,nodes);},findTreeElement:function(node)
{var treeElement=this._lookUpTreeElement(node);if(!treeElement&&node.nodeType()===Node.TEXT_NODE){treeElement=this._lookUpTreeElement(node.parentNode);}
return(treeElement);},_lookUpTreeElement:function(node)
{if(!node)
return null;var cachedElement=node[this._treeElementSymbol];if(cachedElement)
return cachedElement;var ancestors=[];for(var currentNode=node.parentNode;currentNode;currentNode=currentNode.parentNode){ancestors.push(currentNode);if(currentNode[this._treeElementSymbol])
break;}
if(!currentNode)
return null;for(var i=ancestors.length-1;i>=0;--i){var treeElement=ancestors[i][this._treeElementSymbol];if(treeElement)
treeElement.onpopulate();}
return node[this._treeElementSymbol];},createTreeElementFor:function(node)
{var treeElement=this.findTreeElement(node);if(treeElement)
return treeElement;if(!node.parentNode)
return null;treeElement=this.createTreeElementFor(node.parentNode);return treeElement?this._showChild(treeElement,node):null;},set suppressRevealAndSelect(x)
{if(this._suppressRevealAndSelect===x)
return;this._suppressRevealAndSelect=x;},_revealAndSelectNode:function(node,omitFocus)
{if(this._suppressRevealAndSelect)
return;if(!this._includeRootDOMNode&&node===this.rootDOMNode&&this.rootDOMNode)
node=this.rootDOMNode.firstChild;if(!node)
return;var treeElement=this.createTreeElementFor(node);if(!treeElement)
return;treeElement.revealAndSelect(omitFocus);},_treeElementFromEvent:function(event)
{var scrollContainer=this.element.parentElement;var x=scrollContainer.totalOffsetLeft()+scrollContainer.offsetWidth-36;var y=event.pageY;var elementUnderMouse=this.treeElementFromPoint(x,y);var elementAboveMouse=this.treeElementFromPoint(x,y-2);var element;if(elementUnderMouse===elementAboveMouse)
element=elementUnderMouse;else
element=this.treeElementFromPoint(x,y+2);return element;},_getPopoverAnchor:function(element,event)
{var anchor=element.enclosingNodeOrSelfWithClass("webkit-html-resource-link");if(!anchor||!anchor.href)
return;return anchor;},_loadDimensionsForNode:function(node,callback)
{if(!node.nodeName()||node.nodeName().toLowerCase()!=="img"){callback();return;}
node.resolveToObject("",resolvedNode);function resolvedNode(object)
{if(!object){callback();return;}
object.callFunctionJSON(features,undefined,callback);object.release();function features()
{return{offsetWidth:this.offsetWidth,offsetHeight:this.offsetHeight,naturalWidth:this.naturalWidth,naturalHeight:this.naturalHeight,currentSrc:this.currentSrc};}}},_showPopover:function(anchor,popover)
{var listItem=anchor.enclosingNodeOrSelfWithNodeName("li");var node=(listItem.treeElement).node();this._loadDimensionsForNode(node,WebInspector.DOMPresentationUtils.buildImagePreviewContents.bind(WebInspector.DOMPresentationUtils,node.target(),anchor.href,true,showPopover));function showPopover(contents)
{if(!contents)
return;popover.setCanShrink(false);popover.showForAnchor(contents,anchor);}},_onmousedown:function(event)
{var element=this._treeElementFromEvent(event);if(!element||element.isEventWithinDisclosureTriangle(event))
return;element.select();},setHoverEffect:function(treeElement)
{if(this._previousHoveredElement===treeElement)
return;if(this._previousHoveredElement){this._previousHoveredElement.hovered=false;delete this._previousHoveredElement;}
if(treeElement){treeElement.hovered=true;this._previousHoveredElement=treeElement;}},_onmousemove:function(event)
{var element=this._treeElementFromEvent(event);if(element&&this._previousHoveredElement===element)
return;this.setHoverEffect(element);if(element instanceof WebInspector.ElementsTreeElement){this._domModel.highlightDOMNodeWithConfig(element.node().id,{mode:"all",showInfo:!WebInspector.KeyboardShortcut.eventHasCtrlOrMeta(event)});return;}
if(element instanceof WebInspector.ElementsTreeOutline.ShortcutTreeElement)
this._domModel.highlightDOMNodeWithConfig(undefined,{mode:"all",showInfo:!WebInspector.KeyboardShortcut.eventHasCtrlOrMeta(event)},element.backendNodeId());},_onmouseleave:function(event)
{this.setHoverEffect(null);WebInspector.DOMModel.hideDOMNodeHighlight();},_ondragstart:function(event)
{if(!event.target.isComponentSelectionCollapsed())
return false;if(event.target.nodeName==="A")
return false;var treeElement=this._treeElementFromEvent(event);if(!this._isValidDragSourceOrTarget(treeElement))
return false;if(treeElement.node().nodeName()==="BODY"||treeElement.node().nodeName()==="HEAD")
return false;event.dataTransfer.setData("text/plain",treeElement.listItemElement.textContent.replace(/\u200b/g,""));event.dataTransfer.effectAllowed="copyMove";this._treeElementBeingDragged=treeElement;WebInspector.DOMModel.hideDOMNodeHighlight();return true;},_ondragover:function(event)
{if(!this._treeElementBeingDragged)
return false;var treeElement=this._treeElementFromEvent(event);if(!this._isValidDragSourceOrTarget(treeElement))
return false;var node=treeElement.node();while(node){if(node===this._treeElementBeingDragged._node)
return false;node=node.parentNode;}
treeElement.updateSelection();treeElement.listItemElement.classList.add("elements-drag-over");this._dragOverTreeElement=treeElement;event.preventDefault();event.dataTransfer.dropEffect='move';return false;},_ondragleave:function(event)
{this._clearDragOverTreeElementMarker();event.preventDefault();return false;},_isValidDragSourceOrTarget:function(treeElement)
{if(!treeElement)
return false;if(!(treeElement instanceof WebInspector.ElementsTreeElement))
return false;var elementsTreeElement=(treeElement);var node=elementsTreeElement.node();if(!node.parentNode||node.parentNode.nodeType()!==Node.ELEMENT_NODE)
return false;return true;},_ondrop:function(event)
{event.preventDefault();var treeElement=this._treeElementFromEvent(event);if(treeElement)
this._doMove(treeElement);},_doMove:function(treeElement)
{if(!this._treeElementBeingDragged)
return;var parentNode;var anchorNode;if(treeElement.isClosingTag()){parentNode=treeElement.node();}else{var dragTargetNode=treeElement.node();parentNode=dragTargetNode.parentNode;anchorNode=dragTargetNode;}
var wasExpanded=this._treeElementBeingDragged.expanded;this._treeElementBeingDragged._node.moveTo(parentNode,anchorNode,this.selectNodeAfterEdit.bind(this,wasExpanded));delete this._treeElementBeingDragged;},_ondragend:function(event)
{event.preventDefault();this._clearDragOverTreeElementMarker();delete this._treeElementBeingDragged;},_clearDragOverTreeElementMarker:function()
{if(this._dragOverTreeElement){this._dragOverTreeElement.updateSelection();this._dragOverTreeElement.listItemElement.classList.remove("elements-drag-over");delete this._dragOverTreeElement;}},_contextMenuEventFired:function(event)
{var treeElement=this._treeElementFromEvent(event);if(treeElement instanceof WebInspector.ElementsTreeElement)
this.showContextMenu(treeElement,event);},showContextMenu:function(treeElement,event)
{if(WebInspector.isEditing())
return;var contextMenu=new WebInspector.ContextMenu(event);var isPseudoElement=!!treeElement.node().pseudoType();var isTag=treeElement.node().nodeType()===Node.ELEMENT_NODE&&!isPseudoElement;var textNode=event.target.enclosingNodeOrSelfWithClass("webkit-html-text-node");if(textNode&&textNode.classList.contains("bogus"))
textNode=null;var commentNode=event.target.enclosingNodeOrSelfWithClass("webkit-html-comment");contextMenu.appendApplicableItems(event.target);if(textNode){contextMenu.appendSeparator();treeElement.populateTextContextMenu(contextMenu,textNode);}else if(isTag){contextMenu.appendSeparator();treeElement.populateTagContextMenu(contextMenu,event);}else if(commentNode){contextMenu.appendSeparator();treeElement.populateNodeContextMenu(contextMenu);}else if(isPseudoElement){treeElement.populateScrollIntoView(contextMenu);}
contextMenu.appendApplicableItems(treeElement.node());contextMenu.show();},runPendingUpdates:function()
{this._updateModifiedNodes();},handleShortcut:function(event)
{var node=this.selectedDOMNode();if(!node)
return;var treeElement=node[this._treeElementSymbol];if(!treeElement)
return;if(WebInspector.KeyboardShortcut.eventHasCtrlOrMeta(event)&&node.parentNode){if(event.keyIdentifier==="Up"&&node.previousSibling){node.moveTo(node.parentNode,node.previousSibling,this.selectNodeAfterEdit.bind(this,treeElement.expanded));event.handled=true;return;}
if(event.keyIdentifier==="Down"&&node.nextSibling){node.moveTo(node.parentNode,node.nextSibling.nextSibling,this.selectNodeAfterEdit.bind(this,treeElement.expanded));event.handled=true;return;}}},toggleEditAsHTML:function(node,startEditing,callback)
{var treeElement=node[this._treeElementSymbol];if(!treeElement||!treeElement.hasEditableNode())
return;if(node.pseudoType())
return;var parentNode=node.parentNode;var index=node.index;var wasExpanded=treeElement.expanded;treeElement.toggleEditAsHTML(editingFinished.bind(this),startEditing);function editingFinished(success)
{if(callback)
callback();if(!success)
return;this.runPendingUpdates();var newNode=parentNode?parentNode.children()[index]||parentNode:null;if(!newNode)
return;this.selectDOMNode(newNode,true);if(wasExpanded){var newTreeItem=this.findTreeElement(newNode);if(newTreeItem)
newTreeItem.expand();}}},selectNodeAfterEdit:function(wasExpanded,error,nodeId)
{if(error)
return null;this.runPendingUpdates();var newNode=nodeId?this._domModel.nodeForId(nodeId):null;if(!newNode)
return null;this.selectDOMNode(newNode,true);var newTreeItem=this.findTreeElement(newNode);if(wasExpanded){if(newTreeItem)
newTreeItem.expand();}
return newTreeItem;},toggleHideElement:function(node,userCallback)
{var pseudoType=node.pseudoType();var effectiveNode=pseudoType?node.parentNode:node;if(!effectiveNode)
return;var hidden=node.marker("hidden-marker");function resolvedNode(object)
{if(!object)
return;function toggleClassAndInjectStyleRule(pseudoType,hidden)
{const classNamePrefix="__web-inspector-hide";const classNameSuffix="-shortcut__";const styleTagId="__web-inspector-hide-shortcut-style__";var selectors=[];selectors.push(".__web-inspector-hide-shortcut__");selectors.push(".__web-inspector-hide-shortcut__ *");selectors.push(".__web-inspector-hidebefore-shortcut__::before");selectors.push(".__web-inspector-hideafter-shortcut__::after");var selector=selectors.join(", ");var ruleBody="    visibility: hidden !important;";var rule="\n"+selector+"\n{\n"+ruleBody+"\n}\n";var className=classNamePrefix+(pseudoType||"")+classNameSuffix;this.classList.toggle(className,hidden);var localRoot=this;while(localRoot.parentNode)
localRoot=localRoot.parentNode;if(localRoot.nodeType===Node.DOCUMENT_NODE)
localRoot=document.head;var style=localRoot.querySelector("style#"+styleTagId);if(style)
return;style=document.createElement("style");style.id=styleTagId;style.type="text/css";style.textContent=rule;localRoot.appendChild(style);}
object.callFunction(toggleClassAndInjectStyleRule,[{value:pseudoType},{value:!hidden}],userCallback);object.release();node.setMarker("hidden-marker",hidden?null:true);}
effectiveNode.resolveToObject("",resolvedNode);},isToggledToHidden:function(node)
{return!!node.marker("hidden-marker");},_reset:function()
{this.rootDOMNode=null;this.selectDOMNode(null,false);this._popoverHelper.hidePopover();delete this._clipboardNodeData;WebInspector.DOMModel.hideDOMNodeHighlight();this._updateRecords.clear();},wireToDOMModel:function()
{this._domModel.addEventListener(WebInspector.DOMModel.Events.NodeInserted,this._nodeInserted,this);this._domModel.addEventListener(WebInspector.DOMModel.Events.NodeRemoved,this._nodeRemoved,this);this._domModel.addEventListener(WebInspector.DOMModel.Events.AttrModified,this._attributeModified,this);this._domModel.addEventListener(WebInspector.DOMModel.Events.AttrRemoved,this._attributeRemoved,this);this._domModel.addEventListener(WebInspector.DOMModel.Events.CharacterDataModified,this._characterDataModified,this);this._domModel.addEventListener(WebInspector.DOMModel.Events.DocumentUpdated,this._documentUpdated,this);this._domModel.addEventListener(WebInspector.DOMModel.Events.ChildNodeCountUpdated,this._childNodeCountUpdated,this);this._domModel.addEventListener(WebInspector.DOMModel.Events.DistributedNodesChanged,this._distributedNodesChanged,this);},unwireFromDOMModel:function()
{this._domModel.removeEventListener(WebInspector.DOMModel.Events.NodeInserted,this._nodeInserted,this);this._domModel.removeEventListener(WebInspector.DOMModel.Events.NodeRemoved,this._nodeRemoved,this);this._domModel.removeEventListener(WebInspector.DOMModel.Events.AttrModified,this._attributeModified,this);this._domModel.removeEventListener(WebInspector.DOMModel.Events.AttrRemoved,this._attributeRemoved,this);this._domModel.removeEventListener(WebInspector.DOMModel.Events.CharacterDataModified,this._characterDataModified,this);this._domModel.removeEventListener(WebInspector.DOMModel.Events.DocumentUpdated,this._documentUpdated,this);this._domModel.removeEventListener(WebInspector.DOMModel.Events.ChildNodeCountUpdated,this._childNodeCountUpdated,this);this._domModel.removeEventListener(WebInspector.DOMModel.Events.DistributedNodesChanged,this._distributedNodesChanged,this);},_addUpdateRecord:function(node)
{var record=this._updateRecords.get(node);if(!record){record=new WebInspector.ElementsTreeOutline.UpdateRecord();this._updateRecords.set(node,record);}
return record;},_updateRecordForHighlight:function(node)
{if(!this._visible)
return null;return this._updateRecords.get(node)||null;},_documentUpdated:function(event)
{var inspectedRootDocument=event.data;this._reset();if(!inspectedRootDocument)
return;this.rootDOMNode=inspectedRootDocument;},_attributeModified:function(event)
{var node=(event.data.node);this._addUpdateRecord(node).attributeModified(event.data.name);this._updateModifiedNodesSoon();},_attributeRemoved:function(event)
{var node=(event.data.node);this._addUpdateRecord(node).attributeRemoved(event.data.name);this._updateModifiedNodesSoon();},_characterDataModified:function(event)
{var node=(event.data);this._addUpdateRecord(node).charDataModified();if(node.parentNode&&node.parentNode.firstChild===node.parentNode.lastChild)
this._addUpdateRecord(node.parentNode).childrenModified();this._updateModifiedNodesSoon();},_nodeInserted:function(event)
{var node=(event.data);this._addUpdateRecord((node.parentNode)).nodeInserted(node);this._updateModifiedNodesSoon();},_nodeRemoved:function(event)
{var node=(event.data.node);var parentNode=(event.data.parent);this.resetClipboardIfNeeded(node);this._addUpdateRecord(parentNode).nodeRemoved(node);this._updateModifiedNodesSoon();},_childNodeCountUpdated:function(event)
{var node=(event.data);this._addUpdateRecord(node).childrenModified();this._updateModifiedNodesSoon();},_distributedNodesChanged:function(event)
{var node=(event.data);this._addUpdateRecord(node).childrenModified();this._updateModifiedNodesSoon();},_updateModifiedNodesSoon:function()
{if(!this._updateRecords.size)
return;if(this._updateModifiedNodesTimeout)
return;this._updateModifiedNodesTimeout=setTimeout(this._updateModifiedNodes.bind(this),50);},_updateModifiedNodes:function()
{if(this._updateModifiedNodesTimeout){clearTimeout(this._updateModifiedNodesTimeout);delete this._updateModifiedNodesTimeout;}
var updatedNodes=this._updateRecords.keysArray();var hidePanelWhileUpdating=updatedNodes.length>10;if(hidePanelWhileUpdating){var treeOutlineContainerElement=this.element.parentNode;var originalScrollTop=treeOutlineContainerElement?treeOutlineContainerElement.scrollTop:0;this._element.classList.add("hidden");}
if(this._rootDOMNode&&this._updateRecords.get(this._rootDOMNode)&&this._updateRecords.get(this._rootDOMNode).hasChangedChildren()){this.update();}else{for(var node of this._updateRecords.keys()){if(this._updateRecords.get(node).hasChangedChildren())
this._updateModifiedParentNode(node);else
this._updateModifiedNode(node);}}
if(hidePanelWhileUpdating){this._element.classList.remove("hidden");if(originalScrollTop)
treeOutlineContainerElement.scrollTop=originalScrollTop;this.updateSelection();}
this._updateRecords.clear();this._fireElementsTreeUpdated(updatedNodes);},_updateModifiedNode:function(node)
{var treeElement=this.findTreeElement(node);if(treeElement)
treeElement.updateTitle(this._updateRecordForHighlight(node));},_updateModifiedParentNode:function(node)
{var parentTreeElement=this.findTreeElement(node);if(parentTreeElement){parentTreeElement.setExpandable(this._hasVisibleChildren(node));parentTreeElement.updateTitle(this._updateRecordForHighlight(node));if(parentTreeElement.populated)
this._updateChildren(parentTreeElement);}},populateTreeElement:function(treeElement)
{if(treeElement.childCount()||!treeElement.isExpandable())
return;this._updateModifiedParentNode(treeElement.node());},_createElementTreeElement:function(node,closingTag)
{var treeElement=new WebInspector.ElementsTreeElement(node,closingTag);treeElement.setExpandable(!closingTag&&this._hasVisibleChildren(node));if(node.nodeType()===Node.ELEMENT_NODE&&node.parentNode&&node.parentNode.nodeType()===Node.DOCUMENT_NODE&&!node.parentNode.parentNode)
treeElement.setCollapsible(false);treeElement.selectable=this._selectEnabled;return treeElement;},_showChild:function(treeElement,child)
{if(treeElement.isClosingTag())
return null;var index=this._visibleChildren(treeElement.node()).indexOf(child);if(index===-1)
return null;if(index>=treeElement.expandedChildrenLimit())
this.setExpandedChildrenLimit(treeElement,index+1);return(treeElement.childAt(index));},_visibleChildren:function(node)
{var visibleChildren=WebInspector.ElementsTreeElement.visibleShadowRoots(node);if(node.importedDocument())
visibleChildren.push(node.importedDocument());if(node.templateContent())
visibleChildren.push(node.templateContent());var beforePseudoElement=node.beforePseudoElement();if(beforePseudoElement)
visibleChildren.push(beforePseudoElement);if(node.childNodeCount())
visibleChildren=visibleChildren.concat(node.children());var afterPseudoElement=node.afterPseudoElement();if(afterPseudoElement)
visibleChildren.push(afterPseudoElement);return visibleChildren;},_hasVisibleChildren:function(node)
{if(node.importedDocument())
return true;if(node.templateContent())
return true;if(WebInspector.ElementsTreeElement.visibleShadowRoots(node).length)
return true;if(node.hasPseudoElements())
return true;if(node.isInsertionPoint())
return true;return!!node.childNodeCount()&&!WebInspector.ElementsTreeElement.canShowInlineText(node);},_createExpandAllButtonTreeElement:function(treeElement)
{var button=createTextButton("",handleLoadAllChildren.bind(this));button.value="";var expandAllButtonElement=new TreeElement(button);expandAllButtonElement.selectable=false;expandAllButtonElement.expandAllButton=true;expandAllButtonElement.button=button;return expandAllButtonElement;function handleLoadAllChildren(event)
{var visibleChildCount=this._visibleChildren(treeElement.node()).length;this.setExpandedChildrenLimit(treeElement,Math.max(visibleChildCount,treeElement.expandedChildrenLimit()+WebInspector.ElementsTreeElement.InitialChildrenLimit));event.consume();}},setExpandedChildrenLimit:function(treeElement,expandedChildrenLimit)
{if(treeElement.expandedChildrenLimit()===expandedChildrenLimit)
return;treeElement.setExpandedChildrenLimit(expandedChildrenLimit);if(treeElement.treeOutline&&!this._treeElementsBeingUpdated.has(treeElement))
this._updateModifiedParentNode(treeElement.node());},_updateChildren:function(treeElement)
{if(!treeElement.isExpandable()){var selectedTreeElement=treeElement.treeOutline.selectedTreeElement;if(selectedTreeElement&&selectedTreeElement.hasAncestor(treeElement))
treeElement.select(true);treeElement.removeChildren();return;}
console.assert(!treeElement.isClosingTag());treeElement.node().getChildNodes(childNodesLoaded.bind(this));function childNodesLoaded(children)
{if(!children)
return;this._innerUpdateChildren(treeElement);}},insertChildElement:function(treeElement,child,index,closingTag)
{var newElement=this._createElementTreeElement(child,closingTag);treeElement.insertChild(newElement,index);return newElement;},_moveChild:function(treeElement,child,targetIndex)
{if(treeElement.indexOfChild(child)===targetIndex)
return;var wasSelected=child.selected;if(child.parent)
child.parent.removeChild(child);treeElement.insertChild(child,targetIndex);if(wasSelected)
child.select();},_innerUpdateChildren:function(treeElement)
{if(this._treeElementsBeingUpdated.has(treeElement))
return;this._treeElementsBeingUpdated.add(treeElement);var node=treeElement.node();var visibleChildren=this._visibleChildren(node);var visibleChildrenSet=new Set(visibleChildren);var existingTreeElements=new Map();for(var i=treeElement.childCount()-1;i>=0;--i){var existingTreeElement=treeElement.childAt(i);if(!(existingTreeElement instanceof WebInspector.ElementsTreeElement)){treeElement.removeChildAtIndex(i);continue;}
var elementsTreeElement=(existingTreeElement);var existingNode=elementsTreeElement.node();if(visibleChildrenSet.has(existingNode)){existingTreeElements.set(existingNode,existingTreeElement);continue;}
treeElement.removeChildAtIndex(i);}
for(var i=0;i<visibleChildren.length&&i<treeElement.expandedChildrenLimit();++i){var child=visibleChildren[i];var existingTreeElement=existingTreeElements.get(child)||this.findTreeElement(child);if(existingTreeElement&&existingTreeElement!==treeElement){this._moveChild(treeElement,existingTreeElement,i);}else{var newElement=this.insertChildElement(treeElement,child,i);if(this._updateRecordForHighlight(node)&&treeElement.expanded)
WebInspector.ElementsTreeElement.animateOnDOMUpdate(newElement);if(treeElement.childCount()>treeElement.expandedChildrenLimit())
this.setExpandedChildrenLimit(treeElement,treeElement.expandedChildrenLimit()+1);}}
var expandedChildCount=treeElement.childCount();if(visibleChildren.length>expandedChildCount){var targetButtonIndex=expandedChildCount;if(!treeElement.expandAllButtonElement)
treeElement.expandAllButtonElement=this._createExpandAllButtonTreeElement(treeElement);treeElement.insertChild(treeElement.expandAllButtonElement,targetButtonIndex);treeElement.expandAllButtonElement.button.textContent=WebInspector.UIString("Show All Nodes (%d More)",visibleChildren.length-expandedChildCount);}else if(treeElement.expandAllButtonElement){delete treeElement.expandAllButtonElement;}
if(node.isInsertionPoint()){for(var distributedNode of node.distributedNodes())
treeElement.appendChild(new WebInspector.ElementsTreeOutline.ShortcutTreeElement(distributedNode));}
if(node.nodeType()===Node.ELEMENT_NODE&&treeElement.isExpandable())
this.insertChildElement(treeElement,node,treeElement.childCount(),true);this._treeElementsBeingUpdated.delete(treeElement);},_markersChanged:function(event)
{var node=(event.data);var treeElement=node[this._treeElementSymbol];if(treeElement)
treeElement.updateDecorations();},__proto__:TreeOutline.prototype}
WebInspector.ElementsTreeOutline.UpdateRecord=function()
{}
WebInspector.ElementsTreeOutline.UpdateRecord.prototype={attributeModified:function(attrName)
{if(this._removedAttributes&&this._removedAttributes.has(attrName))
this._removedAttributes.delete(attrName);if(!this._modifiedAttributes)
this._modifiedAttributes=(new Set());this._modifiedAttributes.add(attrName);},attributeRemoved:function(attrName)
{if(this._modifiedAttributes&&this._modifiedAttributes.has(attrName))
this._modifiedAttributes.delete(attrName);if(!this._removedAttributes)
this._removedAttributes=(new Set());this._removedAttributes.add(attrName);},nodeInserted:function(node)
{this._hasChangedChildren=true;},nodeRemoved:function(node)
{this._hasChangedChildren=true;this._hasRemovedChildren=true;},charDataModified:function()
{this._charDataModified=true;},childrenModified:function()
{this._hasChangedChildren=true;},isAttributeModified:function(attributeName)
{return this._modifiedAttributes&&this._modifiedAttributes.has(attributeName);},hasRemovedAttributes:function()
{return!!this._removedAttributes&&!!this._removedAttributes.size;},isCharDataModified:function()
{return!!this._charDataModified;},hasChangedChildren:function()
{return!!this._hasChangedChildren;},hasRemovedChildren:function()
{return!!this._hasRemovedChildren;}}
WebInspector.ElementsTreeOutline.Renderer=function()
{}
WebInspector.ElementsTreeOutline.Renderer.prototype={render:function(object)
{return new Promise(renderPromise);function renderPromise(resolve,reject)
{if(object instanceof WebInspector.DOMNode){onNodeResolved((object));}else if(object instanceof WebInspector.DeferredDOMNode){((object)).resolve(onNodeResolved);}else if(object instanceof WebInspector.RemoteObject){var domModel=WebInspector.DOMModel.fromTarget(((object)).target());if(domModel)
domModel.pushObjectAsNodeToFrontend(object,onNodeResolved);else
reject(new Error("No dom model for given JS object target found."));}else{reject(new Error("Can't reveal not a node."));}
function onNodeResolved(node)
{if(!node){reject(new Error("Could not resolve node."));return;}
var treeOutline=new WebInspector.ElementsTreeOutline(node.domModel(),false,false);treeOutline.rootDOMNode=node;if(!treeOutline.firstChild().isExpandable())
treeOutline._element.classList.add("single-node");treeOutline.setVisible(true);treeOutline.element.treeElementForTest=treeOutline.firstChild();resolve(treeOutline.element);}}}}
WebInspector.ElementsTreeOutline.ShortcutTreeElement=function(nodeShortcut)
{TreeElement.call(this,"");this.listItemElement.createChild("div","selection fill");var title=this.listItemElement.createChild("span","elements-tree-shortcut-title");var text=nodeShortcut.nodeName.toLowerCase();if(nodeShortcut.nodeType===Node.ELEMENT_NODE)
text="<"+text+">";title.textContent="\u21AA "+text;var link=WebInspector.DOMPresentationUtils.linkifyDeferredNodeReference(nodeShortcut.deferredNode);this.listItemElement.createTextChild(" ");link.classList.add("elements-tree-shortcut-link");link.textContent=WebInspector.UIString("reveal");this.listItemElement.appendChild(link);this._nodeShortcut=nodeShortcut;}
WebInspector.ElementsTreeOutline.ShortcutTreeElement.prototype={get hovered()
{return this._hovered;},set hovered(x)
{if(this._hovered===x)
return;this._hovered=x;this.listItemElement.classList.toggle("hovered",x);},updateSelection:function()
{},backendNodeId:function()
{return this._nodeShortcut.deferredNode.backendNodeId();},onselect:function(selectedByUser)
{if(!selectedByUser)
return true;this._nodeShortcut.deferredNode.highlight();this._nodeShortcut.deferredNode.resolve(resolved.bind(this));function resolved(node)
{if(node){this.treeOutline._selectedDOMNode=node;this.treeOutline._selectedNodeChanged();}}
return true;},__proto__:TreeElement.prototype};WebInspector.SharedSidebarModel=function()
{WebInspector.Object.call(this);this._node=WebInspector.context.flavor(WebInspector.DOMNode);WebInspector.context.addFlavorChangeListener(WebInspector.DOMNode,this._onNodeChanged,this);}
WebInspector.SharedSidebarModel.elementNode=function(node)
{if(node&&node.nodeType()===Node.TEXT_NODE&&node.parentNode)
node=node.parentNode;if(node&&node.nodeType()!==Node.ELEMENT_NODE)
node=null;return node;}
WebInspector.SharedSidebarModel.Events={ComputedStyleChanged:"ComputedStyleChanged"}
WebInspector.SharedSidebarModel.prototype={node:function()
{return this._node;},cssModel:function()
{return this._cssModel;},_onNodeChanged:function(event)
{this._node=(event.data);this._updateTarget(this._node?this._node.target():null);this._onComputedStyleChanged();},_updateTarget:function(target)
{if(this._target===target)
return;if(this._targetEvents){WebInspector.EventTarget.removeEventListeners(this._targetEvents);this._targetEvents=null;}
this._target=target;var domModel=null;if(target){this._cssModel=WebInspector.CSSStyleModel.fromTarget(target);domModel=WebInspector.DOMModel.fromTarget(target);}
if(domModel&&this._cssModel){this._targetEvents=[this._cssModel.addEventListener(WebInspector.CSSStyleModel.Events.StyleSheetAdded,this._onComputedStyleChanged,this),this._cssModel.addEventListener(WebInspector.CSSStyleModel.Events.StyleSheetRemoved,this._onComputedStyleChanged,this),this._cssModel.addEventListener(WebInspector.CSSStyleModel.Events.StyleSheetChanged,this._onComputedStyleChanged,this),this._cssModel.addEventListener(WebInspector.CSSStyleModel.Events.MediaQueryResultChanged,this._onComputedStyleChanged,this),this._cssModel.addEventListener(WebInspector.CSSStyleModel.Events.PseudoStateForced,this._onComputedStyleChanged,this),this._cssModel.addEventListener(WebInspector.CSSStyleModel.Events.ModelWasEnabled,this._onComputedStyleChanged,this),domModel.addEventListener(WebInspector.DOMModel.Events.DOMMutated,this._onComputedStyleChanged,this)];}},_elementNode:function()
{return WebInspector.SharedSidebarModel.elementNode(this.node());},fetchComputedStyle:function()
{var elementNode=this._elementNode();var cssModel=this.cssModel();if(!elementNode||!cssModel)
return Promise.resolve((null));if(!this._computedStylePromise)
this._computedStylePromise=cssModel.computedStylePromise(elementNode.id).then(verifyOutdated.bind(this,elementNode));return this._computedStylePromise;function verifyOutdated(elementNode,style)
{return elementNode===this._elementNode()&&style?new WebInspector.SharedSidebarModel.ComputedStyle(elementNode,style):(null);}},_onComputedStyleChanged:function()
{delete this._computedStylePromise;this.dispatchEventToListeners(WebInspector.SharedSidebarModel.Events.ComputedStyleChanged);},__proto__:WebInspector.Object.prototype}
WebInspector.SharedSidebarModel.ComputedStyle=function(node,computedStyle)
{this.node=node;this.computedStyle=computedStyle;};WebInspector.EventListenersWidget=function()
{WebInspector.ThrottledWidget.call(this);this.element.classList.add("events-pane");this._showForAncestorsSetting=WebInspector.settings.createSetting("showEventListenersForAncestors",true);this._showForAncestorsSetting.addChangeListener(this.update.bind(this));this._showFrameworkListenersSetting=WebInspector.settings.createSetting("showFrameowkrListeners",true);this._showFrameworkListenersSetting.addChangeListener(this._showFrameworkListenersChanged.bind(this));this._eventListenersView=new WebInspector.EventListenersView(this.element);WebInspector.context.addFlavorChangeListener(WebInspector.DOMNode,this.update,this);}
WebInspector.EventListenersWidget.createSidebarWrapper=function()
{var widget=new WebInspector.EventListenersWidget();var result=new WebInspector.ElementsSidebarViewWrapperPane(WebInspector.UIString("Event Listeners"),widget);var refreshButton=new WebInspector.ToolbarButton(WebInspector.UIString("Refresh"),"refresh-toolbar-item");refreshButton.addEventListener("click",widget.update.bind(widget));result.toolbar().appendToolbarItem(refreshButton);result.toolbar().appendToolbarItem(new WebInspector.ToolbarCheckbox(WebInspector.UIString("Ancestors"),WebInspector.UIString("Show listeners on the ancestors"),widget._showForAncestorsSetting));result.toolbar().appendToolbarItem(new WebInspector.ToolbarCheckbox(WebInspector.UIString("Framework listeners"),WebInspector.UIString("Resolve event listeners bound with framework"),widget._showFrameworkListenersSetting));return result;}
WebInspector.EventListenersWidget._objectGroupName="event-listeners-panel";WebInspector.EventListenersWidget.prototype={doUpdate:function()
{if(this._lastRequestedNode){this._lastRequestedNode.target().runtimeAgent().releaseObjectGroup(WebInspector.EventListenersWidget._objectGroupName);delete this._lastRequestedNode;}
var node=WebInspector.context.flavor(WebInspector.DOMNode);if(!node){this._eventListenersView.reset();this._eventListenersView.addEmptyHolderIfNeeded();return Promise.resolve();}
this._lastRequestedNode=node;var selectedNodeOnly=!this._showForAncestorsSetting.get();var promises=[];var listenersView=this._eventListenersView;promises.push(node.resolveToObjectPromise(WebInspector.EventListenersWidget._objectGroupName));if(!selectedNodeOnly){var currentNode=node.parentNode;while(currentNode){promises.push(currentNode.resolveToObjectPromise(WebInspector.EventListenersWidget._objectGroupName));currentNode=currentNode.parentNode;}
promises.push(this._windowObjectInNodeContext(node));}
return Promise.all(promises).then(this._eventListenersView.addObjects.bind(this._eventListenersView)).then(this._showFrameworkListenersChanged.bind(this));},_showFrameworkListenersChanged:function()
{this._eventListenersView.showFrameworkListeners(this._showFrameworkListenersSetting.get());},_windowObjectInNodeContext:function(node)
{return new Promise(windowObjectInNodeContext);function windowObjectInNodeContext(fulfill,reject)
{var executionContexts=node.target().runtimeModel.executionContexts();var context=null;if(node.frameId()){for(var i=0;i<executionContexts.length;++i){var executionContext=executionContexts[i];if(executionContext.frameId===node.frameId()&&executionContext.isMainWorldContext)
context=executionContext;}}else{context=executionContexts[0];}
context.evaluate("self",WebInspector.EventListenersWidget._objectGroupName,false,true,false,false,fulfill);}},_eventListenersArrivedForTest:function()
{},__proto__:WebInspector.ThrottledWidget.prototype};WebInspector.MetricsSidebarPane=function()
{WebInspector.ElementsSidebarPane.call(this,WebInspector.UIString("Metrics"));}
WebInspector.MetricsSidebarPane.prototype={doUpdate:function()
{if(this._isEditingMetrics)
return Promise.resolve();var node=this.node();var cssModel=this.cssModel();if(!node||node.nodeType()!==Node.ELEMENT_NODE||!cssModel){this.element.removeChildren();return Promise.resolve();}
function callback(style)
{if(!style||this.node()!==node)
return;this._updateMetrics(style);}
function inlineStyleCallback(inlineStyleResult)
{if(inlineStyleResult&&this.node()===node)
this.inlineStyle=inlineStyleResult.inlineStyle;}
var promises=[cssModel.computedStylePromise(node.id).then(callback.bind(this)),cssModel.inlineStylesPromise(node.id).then(inlineStyleCallback.bind(this))];return Promise.all(promises);},onDOMModelChanged:function()
{this.update();},onCSSModelChanged:function()
{this.update();},onFrameResizedThrottled:function()
{this.update();},_getPropertyValueAsPx:function(style,propertyName)
{return Number(style.get(propertyName).replace(/px$/,"")||0);},_getBox:function(computedStyle,componentName)
{var suffix=componentName==="border"?"-width":"";var left=this._getPropertyValueAsPx(computedStyle,componentName+"-left"+suffix);var top=this._getPropertyValueAsPx(computedStyle,componentName+"-top"+suffix);var right=this._getPropertyValueAsPx(computedStyle,componentName+"-right"+suffix);var bottom=this._getPropertyValueAsPx(computedStyle,componentName+"-bottom"+suffix);return{left:left,top:top,right:right,bottom:bottom};},_highlightDOMNode:function(showHighlight,mode,event)
{event.consume();if(showHighlight&&this.node()){if(this._highlightMode===mode)
return;this._highlightMode=mode;this.node().highlight(mode);}else{delete this._highlightMode;WebInspector.DOMModel.hideDOMNodeHighlight();}
for(var i=0;this._boxElements&&i<this._boxElements.length;++i){var element=this._boxElements[i];if(!this.node()||mode==="all"||element._name===mode)
element.style.backgroundColor=element._backgroundColor;else
element.style.backgroundColor="";}},_updateMetrics:function(style)
{var metricsElement=createElement("div");metricsElement.className="metrics";var self=this;function createBoxPartElement(style,name,side,suffix)
{var propertyName=(name!=="position"?name+"-":"")+side+suffix;var value=style.get(propertyName);if(value===""||(name!=="position"&&value==="0px"))
value="\u2012";else if(name==="position"&&value==="auto")
value="\u2012";value=value.replace(/px$/,"");value=Number.toFixedIfFloating(value);var element=createElement("div");element.className=side;element.textContent=value;element.addEventListener("dblclick",this.startEditing.bind(this,element,name,propertyName,style),false);return element;}
function getContentAreaWidthPx(style)
{var width=style.get("width").replace(/px$/,"");if(!isNaN(width)&&style.get("box-sizing")==="border-box"){var borderBox=self._getBox(style,"border");var paddingBox=self._getBox(style,"padding");width=width-borderBox.left-borderBox.right-paddingBox.left-paddingBox.right;}
return Number.toFixedIfFloating(width.toString());}
function getContentAreaHeightPx(style)
{var height=style.get("height").replace(/px$/,"");if(!isNaN(height)&&style.get("box-sizing")==="border-box"){var borderBox=self._getBox(style,"border");var paddingBox=self._getBox(style,"padding");height=height-borderBox.top-borderBox.bottom-paddingBox.top-paddingBox.bottom;}
return Number.toFixedIfFloating(height.toString());}
var noMarginDisplayType={"table-cell":true,"table-column":true,"table-column-group":true,"table-footer-group":true,"table-header-group":true,"table-row":true,"table-row-group":true};var noPaddingDisplayType={"table-column":true,"table-column-group":true,"table-footer-group":true,"table-header-group":true,"table-row":true,"table-row-group":true};var noPositionType={"static":true};var boxes=["content","padding","border","margin","position"];var boxColors=[WebInspector.Color.PageHighlight.Content,WebInspector.Color.PageHighlight.Padding,WebInspector.Color.PageHighlight.Border,WebInspector.Color.PageHighlight.Margin,WebInspector.Color.fromRGBA([0,0,0,0])];var boxLabels=[WebInspector.UIString("content"),WebInspector.UIString("padding"),WebInspector.UIString("border"),WebInspector.UIString("margin"),WebInspector.UIString("position")];var previousBox=null;this._boxElements=[];for(var i=0;i<boxes.length;++i){var name=boxes[i];if(name==="margin"&&noMarginDisplayType[style.get("display")])
continue;if(name==="padding"&&noPaddingDisplayType[style.get("display")])
continue;if(name==="position"&&noPositionType[style.get("position")])
continue;var boxElement=createElement("div");boxElement.className=name;boxElement._backgroundColor=boxColors[i].asString(WebInspector.Color.Format.RGBA);boxElement._name=name;boxElement.style.backgroundColor=boxElement._backgroundColor;boxElement.addEventListener("mouseover",this._highlightDOMNode.bind(this,true,name==="position"?"all":name),false);this._boxElements.push(boxElement);if(name==="content"){var widthElement=createElement("span");widthElement.textContent=getContentAreaWidthPx(style);widthElement.addEventListener("dblclick",this.startEditing.bind(this,widthElement,"width","width",style),false);var heightElement=createElement("span");heightElement.textContent=getContentAreaHeightPx(style);heightElement.addEventListener("dblclick",this.startEditing.bind(this,heightElement,"height","height",style),false);boxElement.appendChild(widthElement);boxElement.createTextChild(" \u00D7 ");boxElement.appendChild(heightElement);}else{var suffix=(name==="border"?"-width":"");var labelElement=createElement("div");labelElement.className="label";labelElement.textContent=boxLabels[i];boxElement.appendChild(labelElement);boxElement.appendChild(createBoxPartElement.call(this,style,name,"top",suffix));boxElement.appendChild(createElement("br"));boxElement.appendChild(createBoxPartElement.call(this,style,name,"left",suffix));if(previousBox)
boxElement.appendChild(previousBox);boxElement.appendChild(createBoxPartElement.call(this,style,name,"right",suffix));boxElement.appendChild(createElement("br"));boxElement.appendChild(createBoxPartElement.call(this,style,name,"bottom",suffix));}
previousBox=boxElement;}
metricsElement.appendChild(previousBox);metricsElement.addEventListener("mouseover",this._highlightDOMNode.bind(this,false,"all"),false);this.element.removeChildren();this.element.appendChild(metricsElement);},startEditing:function(targetElement,box,styleProperty,computedStyle)
{if(WebInspector.isBeingEdited(targetElement))
return;var context={box:box,styleProperty:styleProperty,computedStyle:computedStyle};var boundKeyDown=this._handleKeyDown.bind(this,context,styleProperty);context.keyDownHandler=boundKeyDown;targetElement.addEventListener("keydown",boundKeyDown,false);this._isEditingMetrics=true;var config=new WebInspector.InplaceEditor.Config(this.editingCommitted.bind(this),this.editingCancelled.bind(this),context);WebInspector.InplaceEditor.startEditing(targetElement,config);targetElement.getComponentSelection().setBaseAndExtent(targetElement,0,targetElement,1);},_handleKeyDown:function(context,styleProperty,event)
{var element=event.currentTarget;function finishHandler(originalValue,replacementString)
{this._applyUserInput(element,replacementString,originalValue,context,false);}
function customNumberHandler(prefix,number,suffix)
{if(styleProperty!=="margin"&&number<0)
number=0;return prefix+number+suffix;}
WebInspector.handleElementValueModifications(event,element,finishHandler.bind(this),undefined,customNumberHandler);},editingEnded:function(element,context)
{delete this.originalPropertyData;delete this.previousPropertyDataCandidate;element.removeEventListener("keydown",context.keyDownHandler,false);delete this._isEditingMetrics;},editingCancelled:function(element,context)
{if("originalPropertyData"in this&&this.inlineStyle){if(!this.originalPropertyData){var pastLastSourcePropertyIndex=this.inlineStyle.pastLastSourcePropertyIndex();if(pastLastSourcePropertyIndex)
this.inlineStyle.allProperties[pastLastSourcePropertyIndex-1].setText("",false);}else
this.inlineStyle.allProperties[this.originalPropertyData.index].setText(this.originalPropertyData.propertyText,false);}
this.editingEnded(element,context);this.update();},_applyUserInput:function(element,userInput,previousContent,context,commitEditor)
{if(!this.inlineStyle){return this.editingCancelled(element,context);}
if(commitEditor&&userInput===previousContent)
return this.editingCancelled(element,context);if(context.box!=="position"&&(!userInput||userInput==="\u2012"))
userInput="0px";else if(context.box==="position"&&(!userInput||userInput==="\u2012"))
userInput="auto";userInput=userInput.toLowerCase();if(/^\d+$/.test(userInput))
userInput+="px";var styleProperty=context.styleProperty;var computedStyle=context.computedStyle;if(computedStyle.get("box-sizing")==="border-box"&&(styleProperty==="width"||styleProperty==="height")){if(!userInput.match(/px$/)){WebInspector.console.error("For elements with box-sizing: border-box, only absolute content area dimensions can be applied");return;}
var borderBox=this._getBox(computedStyle,"border");var paddingBox=this._getBox(computedStyle,"padding");var userValuePx=Number(userInput.replace(/px$/,""));if(isNaN(userValuePx))
return;if(styleProperty==="width")
userValuePx+=borderBox.left+borderBox.right+paddingBox.left+paddingBox.right;else
userValuePx+=borderBox.top+borderBox.bottom+paddingBox.top+paddingBox.bottom;userInput=userValuePx+"px";}
this.previousPropertyDataCandidate=null;var allProperties=this.inlineStyle.allProperties;for(var i=0;i<allProperties.length;++i){var property=allProperties[i];if(property.name!==context.styleProperty||!property.activeInStyle())
continue;this.previousPropertyDataCandidate=property;property.setValue(userInput,commitEditor,true,callback.bind(this));return;}
this.inlineStyle.appendProperty(context.styleProperty,userInput,callback.bind(this));function callback(success)
{if(!success)
return;if(!("originalPropertyData"in this))
this.originalPropertyData=this.previousPropertyDataCandidate;if(typeof this._highlightMode!=="undefined")
this._node.highlight(this._highlightMode);if(commitEditor)
this.update();}},editingCommitted:function(element,userInput,previousContent,context)
{this.editingEnded(element,context);this._applyUserInput(element,userInput,previousContent,context,true);},__proto__:WebInspector.ElementsSidebarPane.prototype};WebInspector.PlatformFontsWidget=function(sharedModel)
{WebInspector.ThrottledWidget.call(this);this.element.classList.add("platform-fonts");this._sharedModel=sharedModel;this._sharedModel.addEventListener(WebInspector.SharedSidebarModel.Events.ComputedStyleChanged,this.update,this);this._sectionTitle=createElementWithClass("div","sidebar-separator");this.element.appendChild(this._sectionTitle);this._sectionTitle.textContent=WebInspector.UIString("Rendered Fonts");this._fontStatsSection=this.element.createChild("div","stats-section");}
WebInspector.PlatformFontsWidget.createSidebarWrapper=function(sharedModel)
{var widget=new WebInspector.PlatformFontsWidget(sharedModel);return new WebInspector.ElementsSidebarViewWrapperPane(WebInspector.UIString("Fonts"),widget)}
WebInspector.PlatformFontsWidget.prototype={doUpdate:function()
{var cssModel=this._sharedModel.cssModel();var node=this._sharedModel.node();if(!node||!cssModel)
return Promise.resolve();return cssModel.platformFontsPromise(node.id).then(this._refreshUI.bind(this,node))},_refreshUI:function(node,platformFonts)
{if(this._sharedModel.node()!==node)
return;this._fontStatsSection.removeChildren();var isEmptySection=!platformFonts||!platformFonts.length;this._sectionTitle.classList.toggle("hidden",isEmptySection);if(isEmptySection)
return;platformFonts.sort(function(a,b){return b.glyphCount-a.glyphCount;});for(var i=0;i<platformFonts.length;++i){var fontStatElement=this._fontStatsSection.createChild("div","font-stats-item");var fontNameElement=fontStatElement.createChild("span","font-name");fontNameElement.textContent=platformFonts[i].familyName;var fontDelimeterElement=fontStatElement.createChild("span","delimeter");fontDelimeterElement.textContent="\u2014";var fontUsageElement=fontStatElement.createChild("span","font-usage");var usage=platformFonts[i].glyphCount;fontUsageElement.textContent=usage===1?WebInspector.UIString("%d glyph",usage):WebInspector.UIString("%d glyphs",usage);}},__proto__:WebInspector.ThrottledWidget.prototype};WebInspector.PropertiesWidget=function()
{WebInspector.ThrottledWidget.call(this);WebInspector.targetManager.addModelListener(WebInspector.DOMModel,WebInspector.DOMModel.Events.AttrModified,this._onNodeChange,this);WebInspector.targetManager.addModelListener(WebInspector.DOMModel,WebInspector.DOMModel.Events.AttrRemoved,this._onNodeChange,this);WebInspector.targetManager.addModelListener(WebInspector.DOMModel,WebInspector.DOMModel.Events.CharacterDataModified,this._onNodeChange,this);WebInspector.targetManager.addModelListener(WebInspector.DOMModel,WebInspector.DOMModel.Events.ChildNodeCountUpdated,this._onNodeChange,this);WebInspector.context.addFlavorChangeListener(WebInspector.DOMNode,this._setNode,this);}
WebInspector.PropertiesWidget.createSidebarWrapper=function()
{return new WebInspector.ElementsSidebarViewWrapperPane(WebInspector.UIString("Properties"),new WebInspector.PropertiesWidget());}
WebInspector.PropertiesWidget._objectGroupName="properties-sidebar-pane";WebInspector.PropertiesWidget.prototype={_setNode:function(event)
{this._node=(event.data);this.update();},doUpdate:function()
{if(this._lastRequestedNode){this._lastRequestedNode.target().runtimeAgent().releaseObjectGroup(WebInspector.PropertiesWidget._objectGroupName);delete this._lastRequestedNode;}
if(!this._node){this.element.removeChildren();this.sections=[];return Promise.resolve();}
this._lastRequestedNode=this._node;return this._node.resolveToObjectPromise(WebInspector.PropertiesWidget._objectGroupName).then(nodeResolved.bind(this))
function nodeResolved(object)
{if(!object)
return;function protoList()
{var proto=this;var result={__proto__:null};var counter=1;while(proto){result[counter++]=proto;proto=proto.__proto__;}
return result;}
var promise=object.callFunctionPromise(protoList).then(nodePrototypesReady.bind(this));object.release();return promise;}
function nodePrototypesReady(result)
{if(!result.object||result.wasThrown)
return;var promise=result.object.getOwnPropertiesPromise().then(fillSection.bind(this));result.object.release();return promise;}
function fillSection(result)
{if(!result||!result.properties)
return;var properties=result.properties;var expanded=[];var sections=this.sections||[];for(var i=0;i<sections.length;++i)
expanded.push(sections[i].expanded);this.element.removeChildren();this.sections=[];for(var i=0;i<properties.length;++i){if(!parseInt(properties[i].name,10))
continue;var property=properties[i].value;var title=property.description;title=title.replace(/Prototype$/,"");var section=new WebInspector.ObjectPropertiesSection(property,title);section.element.classList.add("properties-widget-section");this.sections.push(section);this.element.appendChild(section.element);if(expanded[this.sections.length-1])
section.expand();}}},_onNodeChange:function(event)
{if(!this._node)
return;var data=event.data;var node=(data instanceof WebInspector.DOMNode?data:data.node);if(this._node!==node)
return;this.update();},__proto__:WebInspector.ThrottledWidget.prototype};WebInspector.PropertyChangeHighlighter=function(ssp)
{this._styleSidebarPane=ssp;WebInspector.targetManager.addModelListener(WebInspector.CSSStyleModel,WebInspector.CSSStyleModel.Events.LayoutEditorChange,this._onLayoutEditorChange,this);this._animationDuration=1400;this._requestAnimationFrame=ssp.element.window().requestAnimationFrame;}
WebInspector.PropertyChangeHighlighter.prototype={_onLayoutEditorChange:function(event)
{this._target=event.target.target();this._styleSheetId=event.data.id;this._changeRange=event.data.range;delete this._animationStart;if(!this._nextAnimation)
this._nextAnimation=this._requestAnimationFrame.call(null,this.update.bind(this));},update:function(now)
{delete this._nextAnimation;if(!this._styleSheetId)
return;var node=this._styleSidebarPane.node();if(!node||this._target!==node.target()){this._clear();return;}
var sectionBlocks=this._styleSidebarPane.sectionBlocks();var foundSection=null;for(var block of sectionBlocks){for(var section of block.sections){var declaration=section.style();if(declaration.styleSheetId!==this._styleSheetId)
continue;if(this._checkRanges(declaration.range,this._changeRange)){foundSection=section;break;}}
if(foundSection)
break;}
if(!foundSection){this._clear();return;}
var treeElement=foundSection.propertiesTreeOutline.firstChild();var foundTreeElement=null;while(treeElement){if(treeElement.property.range&&this._checkRanges(treeElement.property.range,this._changeRange)){foundTreeElement=treeElement;break;}
treeElement=treeElement.traverseNextTreeElement(false,null,true);}
if(!foundTreeElement){this._clear();return;}
if(!this._animationStart)
this._animationStart=now;var animationProgress=(now-this._animationStart)/this._animationDuration;var valueElement=foundTreeElement.valueElement;valueElement.classList.toggle("css-update-highlight",animationProgress<1);valueElement.classList.toggle("first-part",animationProgress<0.2);if(animationProgress>1){this._clear();delete valueElement.style.backgroundColor;return;}
valueElement.style.backgroundColor="rgba(158, 54, 153, "+(1-animationProgress)+")";this._nextAnimation=this._requestAnimationFrame.call(null,this.update.bind(this));},_clear:function()
{delete this._styleSheetId;delete this._changeRange;delete this._target;delete this._animationStart;},_checkRanges:function(outterRange,innerRange)
{var startsBefore=outterRange.startLine<innerRange.startLine||(outterRange.startLine===innerRange.startLine&&outterRange.startColumn<=innerRange.startColumn);var eps=5;var endsAfter=outterRange.endLine>innerRange.endLine||(outterRange.endLine===innerRange.endLine&&outterRange.endColumn+eps>=innerRange.endColumn);return startsBefore&&endsAfter;}};WebInspector.StylesSidebarPane=function(toolbarPaneElement)
{WebInspector.ElementsSidebarPane.call(this,WebInspector.UIString("Styles"));this.setMinimumSize(96,26);WebInspector.moduleSetting("colorFormat").addChangeListener(this.update.bind(this));WebInspector.moduleSetting("textEditorIndent").addChangeListener(this.update.bind(this));var hbox=this.element.createChild("div","hbox styles-sidebar-pane-toolbar");var filterContainerElement=hbox.createChild("div","styles-sidebar-pane-filter-box");this._filterInput=WebInspector.StylesSidebarPane.createPropertyFilterElement(WebInspector.UIString("Filter"),hbox,this._onFilterChanged.bind(this));filterContainerElement.appendChild(this._filterInput);var toolbar=new WebInspector.ExtensibleToolbar("styles-sidebarpane-toolbar",hbox);toolbar.appendToolbarItem(WebInspector.StylesSidebarPane.createAddNewRuleButton(this));toolbar.element.classList.add("styles-pane-toolbar","toolbar-gray-toggled");var toolbarPaneContainer=this.element.createChild("div","styles-sidebar-toolbar-pane-container");this._toolbarPaneElement=toolbarPaneElement;toolbarPaneContainer.appendChild(toolbarPaneElement);this._sectionsContainer=this.element.createChild("div");this._stylesPopoverHelper=new WebInspector.StylesPopoverHelper();this._linkifier=new WebInspector.Linkifier(new WebInspector.Linkifier.DefaultCSSFormatter());this.element.classList.add("styles-pane");this.element.addEventListener("mousemove",this._mouseMovedOverElement.bind(this),false);this._keyDownBound=this._keyDown.bind(this);this._keyUpBound=this._keyUp.bind(this);new WebInspector.PropertyChangeHighlighter(this);}
WebInspector.StylesSidebarPane.Events={SelectorEditingStarted:"SelectorEditingStarted",SelectorEditingEnded:"SelectorEditingEnded"};WebInspector.StylesSidebarPane.createExclamationMark=function(property)
{var exclamationElement=createElement("label","dt-icon-label");exclamationElement.className="exclamation-mark";if(!WebInspector.StylesSidebarPane.ignoreErrorsForProperty(property))
exclamationElement.type="warning-icon";exclamationElement.title=WebInspector.CSSMetadata.cssPropertiesMetainfo.keySet()[property.name.toLowerCase()]?WebInspector.UIString("Invalid property value"):WebInspector.UIString("Unknown property name");return exclamationElement;}
WebInspector.StylesSidebarPane.ignoreErrorsForProperty=function(property){function hasUnknownVendorPrefix(string)
{return!string.startsWith("-webkit-")&&/^[-_][\w\d]+-\w/.test(string);}
var name=property.name.toLowerCase();if(name.charAt(0)==="_")
return true;if(name==="filter")
return true;if(name.startsWith("scrollbar-"))
return true;if(hasUnknownVendorPrefix(name))
return true;var value=property.value.toLowerCase();if(value.endsWith("\9"))
return true;if(hasUnknownVendorPrefix(value))
return true;return false;}
WebInspector.StylesSidebarPane.prototype={onUndoOrRedoHappened:function()
{this.setNode(this.node());},_onAddButtonLongClick:function(event)
{var cssModel=this.cssModel();if(!cssModel)
return;var headers=cssModel.styleSheetHeaders().filter(styleSheetResourceHeader);var contextMenuDescriptors=[];for(var i=0;i<headers.length;++i){var header=headers[i];var handler=this._createNewRuleInStyleSheet.bind(this,header);contextMenuDescriptors.push({text:WebInspector.displayNameForURL(header.resourceURL()),handler:handler});}
contextMenuDescriptors.sort(compareDescriptors);var contextMenu=new WebInspector.ContextMenu((event.data));for(var i=0;i<contextMenuDescriptors.length;++i){var descriptor=contextMenuDescriptors[i];contextMenu.appendItem(descriptor.text,descriptor.handler);}
if(!contextMenu.isEmpty())
contextMenu.appendSeparator();contextMenu.appendItem("inspector-stylesheet",this._createNewRuleInViaInspectorStyleSheet.bind(this));contextMenu.show();function compareDescriptors(descriptor1,descriptor2)
{return String.naturalOrderComparator(descriptor1.text,descriptor2.text);}
function styleSheetResourceHeader(header)
{return!header.isViaInspector()&&!header.isInline&&!!header.resourceURL();}},updateEditingSelectorForNode:function(node)
{var selectorText=WebInspector.DOMPresentationUtils.simpleSelector(node);if(!selectorText)
return;this._editingSelectorSection.setSelectorText(selectorText);},isEditingSelector:function()
{return!!this._editingSelectorSection;},_startEditingSelector:function(section)
{this._editingSelectorSection=section;this.dispatchEventToListeners(WebInspector.StylesSidebarPane.Events.SelectorEditingStarted);},_finishEditingSelector:function()
{delete this._editingSelectorSection;this.dispatchEventToListeners(WebInspector.StylesSidebarPane.Events.SelectorEditingEnded);},_styleSheetRuleEdited:function(editedRule,oldRange,newRange)
{if(!editedRule.styleSheetId)
return;for(var block of this._sectionBlocks){for(var section of block.sections)
section._styleSheetRuleEdited(editedRule,oldRange,newRange);}},_styleSheetMediaEdited:function(oldMedia,newMedia)
{if(!oldMedia.parentStyleSheetId)
return;for(var block of this._sectionBlocks){for(var section of block.sections)
section._styleSheetMediaEdited(oldMedia,newMedia);}},_onFilterChanged:function(regex)
{this._filterRegex=regex;this._updateFilter();},setNode:function(node)
{this._stylesPopoverHelper.hide();node=WebInspector.SharedSidebarModel.elementNode(node);this._resetCache();WebInspector.ElementsSidebarPane.prototype.setNode.call(this,node);},_refreshUpdate:function(editedSection)
{var node=this.node();if(!node)
return;for(var block of this._sectionBlocks){for(var section of block.sections){if(section.isBlank)
continue;section.update(section===editedSection);}}
if(this._filterRegex)
this._updateFilter();this._nodeStylesUpdatedForTest(node,false);},doUpdate:function()
{this._discardElementUnderMouse();return this.fetchMatchedCascade().then(this._innerRebuildUpdate.bind(this));},_resetCache:function()
{delete this._matchedCascadePromise;},fetchMatchedCascade:function()
{var node=this.node();if(!node)
return Promise.resolve((null));if(!this._matchedCascadePromise)
this._matchedCascadePromise=this._matchedStylesForNode(node).then(validateStyles.bind(this));return this._matchedCascadePromise;function validateStyles(matchedStyles)
{return matchedStyles&&matchedStyles.node()===this.node()?matchedStyles:null;}},_matchedStylesForNode:function(node)
{var cssModel=this.cssModel();if(!cssModel)
return Promise.resolve((null));return cssModel.matchedStylesPromise(node.id)},setEditingStyle:function(editing)
{if(this._isEditingStyle===editing)
return;this.element.classList.toggle("is-editing-style",editing);this._isEditingStyle=editing;},onCSSModelChanged:function()
{if(this._userOperation||this._isEditingStyle)
return;this._resetCache();this.update();},onFrameResizedThrottled:function()
{this.onCSSModelChanged();},onDOMModelChanged:function(node)
{if(this._isEditingStyle||this._userOperation)
return;if(!this._canAffectCurrentStyles(node))
return;this._resetCache();this.update();},_canAffectCurrentStyles:function(node)
{var currentNode=this.node();return currentNode&&(currentNode===node||node.parentNode===currentNode.parentNode||node.isAncestor(currentNode));},_innerRebuildUpdate:function(matchedStyles)
{this._linkifier.reset();this._sectionsContainer.removeChildren();this._sectionBlocks=[];var node=this.node();if(!matchedStyles||!node)
return;this._sectionBlocks=this._rebuildSectionsForMatchedStyleRules(matchedStyles);var pseudoTypes=[];var keys=new Set(matchedStyles.pseudoStyles().keys());if(keys.delete(DOMAgent.PseudoType.Before))
pseudoTypes.push(DOMAgent.PseudoType.Before);pseudoTypes=pseudoTypes.concat(keys.valuesArray().sort());for(var pseudoType of pseudoTypes){var block=WebInspector.SectionBlock.createPseudoTypeBlock(pseudoType);var styles=(matchedStyles.pseudoStyles().get(pseudoType));for(var style of styles){var section=new WebInspector.StylePropertiesSection(this,matchedStyles,style);block.sections.push(section);}
this._sectionBlocks.push(block);}
for(var block of this._sectionBlocks){var titleElement=block.titleElement();if(titleElement)
this._sectionsContainer.appendChild(titleElement);for(var section of block.sections)
this._sectionsContainer.appendChild(section.element);}
if(this._filterRegex)
this._updateFilter();this._nodeStylesUpdatedForTest(node,true);},_nodeStylesUpdatedForTest:function(node,rebuild)
{},_rebuildSectionsForMatchedStyleRules:function(matchedStyles)
{var blocks=[new WebInspector.SectionBlock(null)];var lastParentNode=null;for(var style of matchedStyles.nodeStyles()){var parentNode=matchedStyles.isInherited(style)?matchedStyles.nodeForStyle(style):null;if(parentNode&&parentNode!==lastParentNode){lastParentNode=parentNode;var block=WebInspector.SectionBlock.createInheritedNodeBlock(lastParentNode);blocks.push(block);}
var section=new WebInspector.StylePropertiesSection(this,matchedStyles,style);blocks.peekLast().sections.push(section);}
return blocks;},_createNewRuleInViaInspectorStyleSheet:function()
{var cssModel=this.cssModel();var node=this.node();if(!cssModel||!node)
return;this._userOperation=true;cssModel.requestViaInspectorStylesheet(node,onViaInspectorStyleSheet.bind(this));function onViaInspectorStyleSheet(styleSheetHeader)
{delete this._userOperation;this._createNewRuleInStyleSheet(styleSheetHeader);}},_createNewRuleInStyleSheet:function(styleSheetHeader)
{if(!styleSheetHeader)
return;styleSheetHeader.requestContent(onStyleSheetContent.bind(this,styleSheetHeader.id));function onStyleSheetContent(styleSheetId,text)
{var lines=text.split("\n");var range=WebInspector.TextRange.createFromLocation(lines.length-1,lines[lines.length-1].length);this._addBlankSection(this._sectionBlocks[0].sections[0],styleSheetId,range);}},_addBlankSection:function(insertAfterSection,styleSheetId,ruleLocation)
{this.expand();var node=this.node();var blankSection=new WebInspector.BlankStylePropertiesSection(this,insertAfterSection._matchedStyles,node?WebInspector.DOMPresentationUtils.simpleSelector(node):"",styleSheetId,ruleLocation,insertAfterSection._style);this._sectionsContainer.insertBefore(blankSection.element,insertAfterSection.element.nextSibling);for(var block of this._sectionBlocks){var index=block.sections.indexOf(insertAfterSection);if(index===-1)
continue;block.sections.splice(index+1,0,blankSection);blankSection.startEditingSelector();}},removeSection:function(section)
{for(var block of this._sectionBlocks){var index=block.sections.indexOf(section);if(index===-1)
continue;block.sections.splice(index,1);section.element.remove();}},filterRegex:function()
{return this._filterRegex;},_updateFilter:function()
{for(var block of this._sectionBlocks)
block.updateFilter();},wasShown:function()
{WebInspector.ElementsSidebarPane.prototype.wasShown.call(this);this.element.ownerDocument.body.addEventListener("keydown",this._keyDownBound,false);this.element.ownerDocument.body.addEventListener("keyup",this._keyUpBound,false);},willHide:function()
{this.element.ownerDocument.body.removeEventListener("keydown",this._keyDownBound,false);this.element.ownerDocument.body.removeEventListener("keyup",this._keyUpBound,false);this._stylesPopoverHelper.hide();this._discardElementUnderMouse();WebInspector.ElementsSidebarPane.prototype.willHide.call(this);},_discardElementUnderMouse:function()
{if(this._elementUnderMouse)
this._elementUnderMouse.classList.remove("styles-panel-hovered");delete this._elementUnderMouse;},_mouseMovedOverElement:function(event)
{if(this._elementUnderMouse&&event.target!==this._elementUnderMouse)
this._discardElementUnderMouse();this._elementUnderMouse=event.target;if(WebInspector.KeyboardShortcut.eventHasCtrlOrMeta((event)))
this._elementUnderMouse.classList.add("styles-panel-hovered");},_keyDown:function(event)
{if((!WebInspector.isMac()&&event.keyCode===WebInspector.KeyboardShortcut.Keys.Ctrl.code)||(WebInspector.isMac()&&event.keyCode===WebInspector.KeyboardShortcut.Keys.Meta.code)){if(this._elementUnderMouse)
this._elementUnderMouse.classList.add("styles-panel-hovered");}},_keyUp:function(event)
{if((!WebInspector.isMac()&&event.keyCode===WebInspector.KeyboardShortcut.Keys.Ctrl.code)||(WebInspector.isMac()&&event.keyCode===WebInspector.KeyboardShortcut.Keys.Meta.code)){this._discardElementUnderMouse();}},sectionBlocks:function()
{return this._sectionBlocks||[];},__proto__:WebInspector.ElementsSidebarPane.prototype}
WebInspector.StylesSidebarPane.createPropertyFilterElement=function(placeholder,container,filterCallback)
{var input=createElement("input");input.placeholder=placeholder;function searchHandler()
{var regex=input.value?new RegExp(input.value.escapeForRegExp(),"i"):null;filterCallback(regex);container.classList.toggle("styles-filter-engaged",!!input.value);}
input.addEventListener("input",searchHandler,false);function keydownHandler(event)
{var Esc="U+001B";if(event.keyIdentifier!==Esc||!input.value)
return;event.consume(true);input.value="";searchHandler();}
input.addEventListener("keydown",keydownHandler,false);input.setFilterValue=setFilterValue;function setFilterValue(value)
{input.value=value;input.focus();searchHandler();}
return input;}
WebInspector.SectionBlock=function(titleElement)
{this._titleElement=titleElement;this.sections=[];}
WebInspector.SectionBlock.createPseudoTypeBlock=function(pseudoType)
{var separatorElement=createElement("div");separatorElement.className="sidebar-separator";separatorElement.textContent=WebInspector.UIString("Pseudo ::%s element",pseudoType);return new WebInspector.SectionBlock(separatorElement);}
WebInspector.SectionBlock.createInheritedNodeBlock=function(node)
{var separatorElement=createElement("div");separatorElement.className="sidebar-separator";var link=WebInspector.DOMPresentationUtils.linkifyNodeReference(node);separatorElement.createTextChild(WebInspector.UIString("Inherited from")+" ");separatorElement.appendChild(link);return new WebInspector.SectionBlock(separatorElement);}
WebInspector.SectionBlock.prototype={updateFilter:function()
{var hasAnyVisibleSection=false;for(var section of this.sections)
hasAnyVisibleSection|=section._updateFilter();if(this._titleElement)
this._titleElement.classList.toggle("hidden",!hasAnyVisibleSection);},titleElement:function()
{return this._titleElement;}}
WebInspector.StylePropertiesSection=function(parentPane,matchedStyles,style)
{this._parentPane=parentPane;this._style=style;this._matchedStyles=matchedStyles;this.editable=!!(style.styleSheetId&&style.range);var rule=style.parentRule;this.element=createElementWithClass("div","styles-section matched-styles monospace");this.element._section=this;this._titleElement=this.element.createChild("div","styles-section-title "+(rule?"styles-selector":""));this.propertiesTreeOutline=new TreeOutline();this.propertiesTreeOutline.element.classList.add("style-properties","monospace");this.propertiesTreeOutline.section=this;this.element.appendChild(this.propertiesTreeOutline.element);var selectorContainer=createElement("div");this._selectorElement=createElementWithClass("span","selector");this._selectorElement.textContent=this._selectorText();selectorContainer.appendChild(this._selectorElement);this._selectorElement.addEventListener("mouseenter",this._onMouseEnterSelector.bind(this),false);this._selectorElement.addEventListener("mouseleave",this._onMouseOutSelector.bind(this),false);var openBrace=createElement("span");openBrace.textContent=" {";selectorContainer.appendChild(openBrace);selectorContainer.addEventListener("mousedown",this._handleEmptySpaceMouseDown.bind(this),false);selectorContainer.addEventListener("click",this._handleSelectorContainerClick.bind(this),false);var closeBrace=this.element.createChild("div","sidebar-pane-closing-brace");closeBrace.textContent="}";if(this.editable){var items=[];var colorButton=new WebInspector.ToolbarButton(WebInspector.UIString("Add color"),"foreground-color-toolbar-item");colorButton.addEventListener("click",this._onInsertColorPropertyClick.bind(this));items.push(colorButton);var backgroundButton=new WebInspector.ToolbarButton(WebInspector.UIString("Add background-color"),"background-color-toolbar-item");backgroundButton.addEventListener("click",this._onInsertBackgroundColorPropertyClick.bind(this));items.push(backgroundButton);if(rule){var newRuleButton=new WebInspector.ToolbarButton(WebInspector.UIString("Insert Style Rule"),"add-toolbar-item");newRuleButton.addEventListener("click",this._onNewRuleClick.bind(this));items.push(newRuleButton);}
var menuButton=new WebInspector.ToolbarButton(WebInspector.UIString("More tools\u2026"),"menu-toolbar-item");items.push(menuButton);if(items.length){var sectionToolbar=new WebInspector.Toolbar();sectionToolbar.element.classList.add("sidebar-pane-section-toolbar");closeBrace.appendChild(sectionToolbar.element);for(var i=0;i<items.length;++i)
sectionToolbar.appendToolbarItem(items[i]);items.pop();function setItemsVisibility(items,value)
{for(var i=0;i<items.length;++i)
items[i].setVisible(value);menuButton.setVisible(!value);}
setItemsVisibility(items,false);sectionToolbar.element.addEventListener("mouseenter",setItemsVisibility.bind(null,items,true));sectionToolbar.element.addEventListener("mouseleave",setItemsVisibility.bind(null,items,false));}}
this._selectorElement.addEventListener("click",this._handleSelectorClick.bind(this),false);this.element.addEventListener("mousedown",this._handleEmptySpaceMouseDown.bind(this),false);this.element.addEventListener("click",this._handleEmptySpaceClick.bind(this),false);if(rule){if(rule.isUserAgent()||rule.isInjected()){this.editable=false;}else{if(rule.styleSheetId)
this.navigable=!!rule.resourceURL();}}
this._selectorRefElement=createElementWithClass("div","styles-section-subtitle");this._mediaListElement=this._titleElement.createChild("div","media-list media-matches");this._updateMediaList();this._updateRuleOrigin();selectorContainer.insertBefore(this._selectorRefElement,selectorContainer.firstChild);this._titleElement.appendChild(selectorContainer);this._selectorContainer=selectorContainer;if(this.navigable)
this.element.classList.add("navigable");if(!this.editable)
this.element.classList.add("read-only");this._markSelectorMatches();this.onpopulate();}
WebInspector.StylePropertiesSection.prototype={style:function()
{return this._style;},_selectorText:function()
{var node=this._matchedStyles.nodeForStyle(this._style);if(this._style.type===WebInspector.CSSStyleDeclaration.Type.Inline)
return this._matchedStyles.isInherited(this._style)?WebInspector.UIString("Style Attribute"):"element.style";if(this._style.type===WebInspector.CSSStyleDeclaration.Type.Attributes)
return node.nodeNameInCorrectCase()+"["+WebInspector.UIString("Attributes Style")+"]";return this._style.parentRule.selectorText();},_onMouseOutSelector:function()
{if(this._hoverTimer)
clearTimeout(this._hoverTimer);WebInspector.DOMModel.hideDOMNodeHighlight()},_onMouseEnterSelector:function()
{if(this._hoverTimer)
clearTimeout(this._hoverTimer);this._hoverTimer=setTimeout(this._highlight.bind(this),300);},_highlight:function()
{WebInspector.DOMModel.hideDOMNodeHighlight()
var node=this._parentPane.node();var domModel=node.domModel();var selectors=this._style.parentRule?this._style.parentRule.selectorText():undefined;domModel.highlightDOMNodeWithConfig(node.id,{mode:"all",showInfo:undefined,selectors:selectors});this._activeHighlightDOMModel=domModel;},firstSibling:function()
{var parent=this.element.parentElement;if(!parent)
return null;var childElement=parent.firstChild;while(childElement){if(childElement._section)
return childElement._section;childElement=childElement.nextSibling;}
return null;},lastSibling:function()
{var parent=this.element.parentElement;if(!parent)
return null;var childElement=parent.lastChild;while(childElement){if(childElement._section)
return childElement._section;childElement=childElement.previousSibling;}
return null;},nextSibling:function()
{var curElement=this.element;do{curElement=curElement.nextSibling;}while(curElement&&!curElement._section);return curElement?curElement._section:null;},previousSibling:function()
{var curElement=this.element;do{curElement=curElement.previousSibling;}while(curElement&&!curElement._section);return curElement?curElement._section:null;},_onNewRuleClick:function(event)
{event.consume();var rule=this._style.parentRule;var range=WebInspector.TextRange.createFromLocation(rule.style.range.endLine,rule.style.range.endColumn+1);this._parentPane._addBlankSection(this,(rule.styleSheetId),range);},_onInsertColorPropertyClick:function(event)
{event.consume(true);var treeElement=this.addNewBlankProperty();treeElement.property.name="color";treeElement.property.value="black";treeElement.updateTitle();var colorSwatch=WebInspector.ColorSwatchPopoverIcon.forTreeElement(treeElement);if(colorSwatch)
colorSwatch.showPopover();},_onInsertBackgroundColorPropertyClick:function(event)
{event.consume(true);var treeElement=this.addNewBlankProperty();treeElement.property.name="background-color";treeElement.property.value="white";treeElement.updateTitle();var colorSwatch=WebInspector.ColorSwatchPopoverIcon.forTreeElement(treeElement);if(colorSwatch)
colorSwatch.showPopover();},_styleSheetRuleEdited:function(editedRule,oldRange,newRange)
{var rule=this._style.parentRule;if(!rule||!rule.styleSheetId)
return;if(rule!==editedRule)
rule.sourceStyleSheetEdited((editedRule.styleSheetId),oldRange,newRange);this._updateMediaList();this._updateRuleOrigin();},_styleSheetMediaEdited:function(oldMedia,newMedia)
{var rule=this._style.parentRule;if(!rule||!rule.styleSheetId)
return;rule.mediaEdited(oldMedia,newMedia);this._updateMediaList();},_createMediaList:function(mediaRules)
{if(!mediaRules)
return;for(var i=mediaRules.length-1;i>=0;--i){var media=mediaRules[i];var mediaDataElement=this._mediaListElement.createChild("div","media");if(media.sourceURL){var anchor=this._parentPane._linkifier.linkifyMedia(media);anchor.classList.add("subtitle");mediaDataElement.appendChild(anchor);}
var mediaContainerElement=mediaDataElement.createChild("span");var mediaTextElement=mediaContainerElement.createChild("span","media-text");mediaTextElement.title=media.text;switch(media.source){case WebInspector.CSSMedia.Source.LINKED_SHEET:case WebInspector.CSSMedia.Source.INLINE_SHEET:mediaTextElement.textContent="media=\""+media.text+"\"";break;case WebInspector.CSSMedia.Source.MEDIA_RULE:var decoration=mediaContainerElement.createChild("span");mediaContainerElement.insertBefore(decoration,mediaTextElement);decoration.textContent="@media ";decoration.title=media.text;mediaTextElement.textContent=media.text;if(media.parentStyleSheetId){mediaDataElement.classList.add("editable-media");mediaTextElement.addEventListener("click",this._handleMediaRuleClick.bind(this,media,mediaTextElement),false);}
break;case WebInspector.CSSMedia.Source.IMPORT_RULE:mediaTextElement.textContent="@import "+media.text;break;}}},_updateMediaList:function()
{this._mediaListElement.removeChildren();this._createMediaList(this._style.parentRule?this._style.parentRule.media:null);},isPropertyInherited:function(propertyName)
{if(this._matchedStyles.isInherited(this._style)){return!WebInspector.CSSMetadata.isPropertyInherited(propertyName);}
return false;},nextEditableSibling:function()
{var curSection=this;do{curSection=curSection.nextSibling();}while(curSection&&!curSection.editable);if(!curSection){curSection=this.firstSibling();while(curSection&&!curSection.editable)
curSection=curSection.nextSibling();}
return(curSection&&curSection.editable)?curSection:null;},previousEditableSibling:function()
{var curSection=this;do{curSection=curSection.previousSibling();}while(curSection&&!curSection.editable);if(!curSection){curSection=this.lastSibling();while(curSection&&!curSection.editable)
curSection=curSection.previousSibling();}
return(curSection&&curSection.editable)?curSection:null;},update:function(full)
{this._selectorElement.textContent=this._selectorText();this._markSelectorMatches();if(full){this.propertiesTreeOutline.removeChildren();this.onpopulate();}else{var child=this.propertiesTreeOutline.firstChild();while(child){var overloaded=this._matchedStyles.propertyState(child.property)===WebInspector.CSSStyleModel.MatchedStyleResult.PropertyState.Overloaded;child.setOverloaded(overloaded);child=child.traverseNextTreeElement(false,null,true);}}
this.afterUpdate();},afterUpdate:function()
{if(this._afterUpdate){this._afterUpdate(this);delete this._afterUpdate;this._afterUpdateFinishedForTest();}},_afterUpdateFinishedForTest:function()
{},onpopulate:function()
{var style=this._style;for(var property of style.leadingProperties()){var isShorthand=!!WebInspector.CSSMetadata.cssPropertiesMetainfo.longhands(property.name);var inherited=this.isPropertyInherited(property.name);var overloaded=this._matchedStyles.propertyState(property)===WebInspector.CSSStyleModel.MatchedStyleResult.PropertyState.Overloaded;var item=new WebInspector.StylePropertyTreeElement(this._parentPane,this._matchedStyles,property,isShorthand,inherited,overloaded);this.propertiesTreeOutline.appendChild(item);}},_updateFilter:function()
{var hasMatchingChild=false;for(var child of this.propertiesTreeOutline.rootElement().children())
hasMatchingChild|=child._updateFilter();var regex=this._parentPane.filterRegex();var hideRule=!hasMatchingChild&&regex&&!regex.test(this.element.textContent);this.element.classList.toggle("hidden",hideRule);if(!hideRule&&this._style.parentRule)
this._markSelectorHighlights();return!hideRule;},_markSelectorMatches:function()
{var rule=this._style.parentRule;if(!rule)
return;this._mediaListElement.classList.toggle("media-matches",this._matchedStyles.mediaMatches(this._style));if(!this._matchedStyles.hasMatchingSelectors(this._style))
return;var selectors=rule.selectors;var fragment=createDocumentFragment();var currentMatch=0;var matchingSelectors=rule.matchingSelectors;for(var i=0;i<selectors.length;++i){if(i)
fragment.createTextChild(", ");var isSelectorMatching=matchingSelectors[currentMatch]===i;if(isSelectorMatching)
++currentMatch;var matchingSelectorClass=isSelectorMatching?" selector-matches":"";var selectorElement=createElement("span");selectorElement.className="simple-selector"+matchingSelectorClass;if(rule.styleSheetId)
selectorElement._selectorIndex=i;selectorElement.textContent=selectors[i].value;fragment.appendChild(selectorElement);}
this._selectorElement.removeChildren();this._selectorElement.appendChild(fragment);this._markSelectorHighlights();},_markSelectorHighlights:function()
{var selectors=this._selectorElement.getElementsByClassName("simple-selector");var regex=this._parentPane.filterRegex();for(var i=0;i<selectors.length;++i){var selectorMatchesFilter=regex&&regex.test(selectors[i].textContent);selectors[i].classList.toggle("filter-match",selectorMatchesFilter);}},_checkWillCancelEditing:function()
{var willCauseCancelEditing=this._willCauseCancelEditing;delete this._willCauseCancelEditing;return willCauseCancelEditing;},_handleSelectorContainerClick:function(event)
{if(this._checkWillCancelEditing()||!this.editable)
return;if(event.target===this._selectorContainer){this.addNewBlankProperty(0).startEditing();event.consume(true);}},addNewBlankProperty:function(index)
{var property=this._style.newBlankProperty(index);var item=new WebInspector.StylePropertyTreeElement(this._parentPane,this._matchedStyles,property,false,false,false);index=property.index;this.propertiesTreeOutline.insertChild(item,index);item.listItemElement.textContent="";item._newProperty=true;item.updateTitle();return item;},_handleEmptySpaceMouseDown:function()
{this._willCauseCancelEditing=this._parentPane._isEditingStyle;},_handleEmptySpaceClick:function(event)
{if(!this.editable)
return;if(!event.target.isComponentSelectionCollapsed())
return;if(this._checkWillCancelEditing())
return;if(event.target.enclosingNodeOrSelfWithNodeName("a"))
return;if(event.target.classList.contains("header")||this.element.classList.contains("read-only")||event.target.enclosingNodeOrSelfWithClass("media")){event.consume();return;}
this.addNewBlankProperty().startEditing();event.consume(true);},_handleMediaRuleClick:function(media,element,event)
{if(WebInspector.isBeingEdited(element))
return;var config=new WebInspector.InplaceEditor.Config(this._editingMediaCommitted.bind(this,media),this._editingMediaCancelled.bind(this,element),undefined,this._editingMediaBlurHandler.bind(this));WebInspector.InplaceEditor.startEditing(element,config);element.getComponentSelection().setBaseAndExtent(element,0,element,1);this._parentPane.setEditingStyle(true);var parentMediaElement=element.enclosingNodeOrSelfWithClass("media");parentMediaElement.classList.add("editing-media");event.consume(true);},_editingMediaFinished:function(element)
{this._parentPane.setEditingStyle(false);var parentMediaElement=element.enclosingNodeOrSelfWithClass("media");parentMediaElement.classList.remove("editing-media");},_editingMediaCancelled:function(element)
{this._editingMediaFinished(element);this._markSelectorMatches();element.getComponentSelection().collapse(element,0);},_editingMediaBlurHandler:function(editor,blurEvent)
{return true;},_editingMediaCommitted:function(media,element,newContent,oldContent,context,moveDirection)
{this._parentPane.setEditingStyle(false);this._editingMediaFinished(element);if(newContent)
newContent=newContent.trim();function userCallback(newMedia)
{if(newMedia){this._parentPane._styleSheetMediaEdited(media,newMedia);this._parentPane._refreshUpdate(this);}
delete this._parentPane._userOperation;this._editingMediaTextCommittedForTest();}
this._parentPane._userOperation=true;this._parentPane._cssModel.setMediaText(media,newContent,userCallback.bind(this));},_editingMediaTextCommittedForTest:function(){},_handleSelectorClick:function(event)
{if(WebInspector.KeyboardShortcut.eventHasCtrlOrMeta((event))&&this.navigable&&event.target.classList.contains("simple-selector")){var index=event.target._selectorIndex;var cssModel=this._parentPane._cssModel;var rule=this._style.parentRule;var rawLocation=new WebInspector.CSSLocation(cssModel,(rule.styleSheetId),rule.sourceURL,rule.lineNumberInSource(index),rule.columnNumberInSource(index));var uiLocation=WebInspector.cssWorkspaceBinding.rawLocationToUILocation(rawLocation);if(uiLocation)
WebInspector.Revealer.reveal(uiLocation);event.consume(true);return;}
this._startEditingOnMouseEvent();event.consume(true);},_startEditingOnMouseEvent:function()
{if(!this.editable)
return;var rule=this._style.parentRule;if(!rule&&!this.propertiesTreeOutline.rootElement().childCount()){this.addNewBlankProperty().startEditing();return;}
if(!rule)
return;this.startEditingSelector();},startEditingSelector:function()
{var element=this._selectorElement;if(WebInspector.isBeingEdited(element))
return;element.scrollIntoViewIfNeeded(false);element.textContent=element.textContent;var config=new WebInspector.InplaceEditor.Config(this.editingSelectorCommitted.bind(this),this.editingSelectorCancelled.bind(this),undefined,this._editingSelectorBlurHandler.bind(this));WebInspector.InplaceEditor.startEditing(this._selectorElement,config);element.getComponentSelection().setBaseAndExtent(element,0,element,1);this._parentPane.setEditingStyle(true);this._parentPane._startEditingSelector(this);},setSelectorText:function(text)
{this._selectorElement.textContent=text;this._selectorElement.getComponentSelection().setBaseAndExtent(this._selectorElement,0,this._selectorElement,1);},_editingSelectorBlurHandler:function(editor,blurEvent)
{if(!blurEvent.relatedTarget)
return true;var elementTreeOutline=blurEvent.relatedTarget.enclosingNodeOrSelfWithClass("elements-tree-outline");if(!elementTreeOutline)
return true;editor.focus();return false;},_moveEditorFromSelector:function(moveDirection)
{this._markSelectorMatches();if(!moveDirection)
return;if(moveDirection==="forward"){var firstChild=this.propertiesTreeOutline.firstChild();while(firstChild&&firstChild.inherited())
firstChild=firstChild.nextSibling;if(!firstChild)
this.addNewBlankProperty().startEditing();else
firstChild.startEditing(firstChild.nameElement);}else{var previousSection=this.previousEditableSibling();if(!previousSection)
return;previousSection.addNewBlankProperty().startEditing();}},editingSelectorCommitted:function(element,newContent,oldContent,context,moveDirection)
{this._editingSelectorEnded();if(newContent)
newContent=newContent.trim();if(newContent===oldContent){this._selectorElement.textContent=newContent;this._moveEditorFromSelector(moveDirection);return;}
var rule=this._style.parentRule;var oldSelectorRange=rule.selectorRange();if(!rule||!oldSelectorRange)
return;function finishCallback(rule,oldSelectorRange,success)
{if(success){var doesAffectSelectedNode=rule.matchingSelectors.length>0;this.element.classList.toggle("no-affect",!doesAffectSelectedNode);this._matchedStyles.resetActiveProperties();var newSelectorRange=(rule.selectorRange());rule.style.sourceStyleSheetEdited((rule.styleSheetId),oldSelectorRange,newSelectorRange);this._parentPane._styleSheetRuleEdited(rule,oldSelectorRange,newSelectorRange);this._parentPane._refreshUpdate(this);}
delete this._parentPane._userOperation;this._moveEditorFromSelector(moveDirection);this._editingSelectorCommittedForTest();}
this._parentPane._userOperation=true;var selectedNode=this._parentPane.node();rule.setSelectorText(selectedNode?selectedNode.id:0,newContent,finishCallback.bind(this,rule,oldSelectorRange));},_editingSelectorCommittedForTest:function(){},_updateRuleOrigin:function()
{this._selectorRefElement.removeChildren();this._selectorRefElement.appendChild(WebInspector.StylePropertiesSection.createRuleOriginNode(this._parentPane._cssModel,this._parentPane._linkifier,this._style.parentRule));},_editingSelectorEnded:function()
{this._parentPane.setEditingStyle(false);this._parentPane._finishEditingSelector();},editingSelectorCancelled:function()
{this._editingSelectorEnded();this._markSelectorMatches();}}
WebInspector.StylePropertiesSection.createRuleOriginNode=function(cssModel,linkifier,rule)
{if(!rule)
return createTextNode("");var firstMatchingIndex=rule.matchingSelectors&&rule.matchingSelectors.length?rule.matchingSelectors[0]:0;var ruleLocation=rule.selectors[firstMatchingIndex].range;var header=rule.styleSheetId?cssModel.styleSheetHeaderForId(rule.styleSheetId):null;if(ruleLocation&&rule.styleSheetId&&header&&header.resourceURL())
return WebInspector.StylePropertiesSection._linkifyRuleLocation(cssModel,linkifier,rule.styleSheetId,ruleLocation);if(rule.isUserAgent())
return createTextNode(WebInspector.UIString("user agent stylesheet"));if(rule.isInjected())
return createTextNode(WebInspector.UIString("injected stylesheet"));if(rule.isViaInspector())
return createTextNode(WebInspector.UIString("via inspector"));if(header&&header.ownerNode){var link=WebInspector.DOMPresentationUtils.linkifyDeferredNodeReference(header.ownerNode);link.textContent="<style>…</style>";return link;}
return createTextNode("");}
WebInspector.StylePropertiesSection._linkifyRuleLocation=function(cssModel,linkifier,styleSheetId,ruleLocation)
{var styleSheetHeader=cssModel.styleSheetHeaderForId(styleSheetId);var sourceURL=styleSheetHeader.resourceURL();var lineNumber=styleSheetHeader.lineNumberInSource(ruleLocation.startLine);var columnNumber=styleSheetHeader.columnNumberInSource(ruleLocation.startLine,ruleLocation.startColumn);var matchingSelectorLocation=new WebInspector.CSSLocation(cssModel,styleSheetId,sourceURL,lineNumber,columnNumber);return linkifier.linkifyCSSLocation(matchingSelectorLocation);}
WebInspector.BlankStylePropertiesSection=function(stylesPane,matchedStyles,defaultSelectorText,styleSheetId,ruleLocation,insertAfterStyle)
{var rule=WebInspector.CSSRule.createDummyRule(stylesPane._cssModel,defaultSelectorText);WebInspector.StylePropertiesSection.call(this,stylesPane,matchedStyles,rule.style);this._ruleLocation=ruleLocation;this._styleSheetId=styleSheetId;this._selectorRefElement.removeChildren();this._selectorRefElement.appendChild(WebInspector.StylePropertiesSection._linkifyRuleLocation(this._parentPane._cssModel,this._parentPane._linkifier,styleSheetId,this._actualRuleLocation()));if(insertAfterStyle&&insertAfterStyle.parentRule)
this._createMediaList(insertAfterStyle.parentRule.media);this.element.classList.add("blank-section");}
WebInspector.BlankStylePropertiesSection.prototype={_actualRuleLocation:function()
{var prefix=this._rulePrefix();var lines=prefix.split("\n");var editRange=new WebInspector.TextRange(0,0,lines.length-1,lines.peekLast().length);return this._ruleLocation.rebaseAfterTextEdit(WebInspector.TextRange.createFromLocation(0,0),editRange);},_rulePrefix:function()
{return this._ruleLocation.startLine===0&&this._ruleLocation.startColumn===0?"":"\n\n";},get isBlank()
{return!this._normal;},editingSelectorCommitted:function(element,newContent,oldContent,context,moveDirection)
{if(!this.isBlank){WebInspector.StylePropertiesSection.prototype.editingSelectorCommitted.call(this,element,newContent,oldContent,context,moveDirection);return;}
function userCallback(newRule)
{if(!newRule){this.editingSelectorCancelled();this._editingSelectorCommittedForTest();return;}
var doesSelectorAffectSelectedNode=newRule.matchingSelectors.length>0;this._makeNormal(newRule);if(!doesSelectorAffectSelectedNode)
this.element.classList.add("no-affect");var ruleTextLines=ruleText.split("\n");var startLine=this._ruleLocation.startLine;var startColumn=this._ruleLocation.startColumn;var newRange=new WebInspector.TextRange(startLine,startColumn,startLine+ruleTextLines.length-1,startColumn+ruleTextLines[ruleTextLines.length-1].length);this._parentPane._styleSheetRuleEdited(newRule,this._ruleLocation,newRange);this._updateRuleOrigin();if(this.element.parentElement)
this._moveEditorFromSelector(moveDirection);delete this._parentPane._userOperation;this._editingSelectorEnded();this._markSelectorMatches();this._editingSelectorCommittedForTest();}
if(newContent)
newContent=newContent.trim();this._parentPane._userOperation=true;var cssModel=this._parentPane._cssModel;var ruleText=this._rulePrefix()+newContent+" {}";cssModel.addRule(this._styleSheetId,this._parentPane.node(),ruleText,this._ruleLocation,userCallback.bind(this));},editingSelectorCancelled:function()
{delete this._parentPane._userOperation;if(!this.isBlank){WebInspector.StylePropertiesSection.prototype.editingSelectorCancelled.call(this);return;}
this._editingSelectorEnded();this._parentPane.removeSection(this);},_makeNormal:function(newRule)
{this.element.classList.remove("blank-section");this._style=newRule.style;this._normal=true;},__proto__:WebInspector.StylePropertiesSection.prototype}
WebInspector.StylePropertyTreeElement=function(stylesPane,matchedStyles,property,isShorthand,inherited,overloaded)
{TreeElement.call(this,"",isShorthand);this._style=property.ownerStyle;this._matchedStyles=matchedStyles;this.property=property;this._inherited=inherited;this._overloaded=overloaded;this.selectable=false;this._parentPane=stylesPane;this.isShorthand=isShorthand;this._applyStyleThrottler=new WebInspector.Throttler(0);}
WebInspector.StylePropertyTreeElement.Context;WebInspector.StylePropertyTreeElement.prototype={_editable:function()
{return this._style.styleSheetId&&this._style.range;},inherited:function()
{return this._inherited;},overloaded:function()
{return this._overloaded;},setOverloaded:function(x)
{if(x===this._overloaded)
return;this._overloaded=x;this._updateState();},get name()
{return this.property.name;},get value()
{return this.property.value;},_updateFilter:function()
{var regex=this._parentPane.filterRegex();var matches=!!regex&&(regex.test(this.property.name)||regex.test(this.property.value));this.listItemElement.classList.toggle("filter-match",matches);this.onpopulate();var hasMatchingChildren=false;for(var i=0;i<this.childCount();++i)
hasMatchingChildren|=this.childAt(i)._updateFilter();if(!regex){if(this._expandedDueToFilter)
this.collapse();this._expandedDueToFilter=false;}else if(hasMatchingChildren&&!this.expanded){this.expand();this._expandedDueToFilter=true;}else if(!hasMatchingChildren&&this.expanded&&this._expandedDueToFilter){this.collapse();this._expandedDueToFilter=false;}
return matches;},_processColor:function(text)
{var color=WebInspector.Color.parse(text);if(!color)
return createTextNode(text);if(!this._editable()){var swatch=WebInspector.ColorSwatch.create();swatch.setColorText(text);return swatch;}
var stylesPopoverHelper=this._parentPane._stylesPopoverHelper;var swatchIcon=new WebInspector.ColorSwatchPopoverIcon(this,stylesPopoverHelper,text);function computedCallback(backgroundColors)
{if(!backgroundColors||!backgroundColors.length)
return;var bgColorText=backgroundColors[0];var bgColor=WebInspector.Color.parse(bgColorText);if(!bgColor)
return;if(bgColor.hasAlpha){var blendedRGBA=[];WebInspector.Color.blendColors(bgColor.rgba(),color.rgba(),blendedRGBA);bgColor=new WebInspector.Color(blendedRGBA,WebInspector.Color.Format.RGBA);}
swatchIcon.setContrastColor(bgColor);}
if(this.property.name==="color"&&this._parentPane.cssModel()&&this.node()){var cssModel=this._parentPane.cssModel();cssModel.backgroundColorsPromise(this.node().id).then(computedCallback);}
return swatchIcon.element();},renderedPropertyText:function()
{return this.nameElement.textContent+": "+this.valueElement.textContent;},_processBezier:function(text)
{var geometry=WebInspector.Geometry.CubicBezier.parse(text);if(!geometry||!this._editable())
return createTextNode(text);var stylesPopoverHelper=this._parentPane._stylesPopoverHelper;return new WebInspector.BezierPopoverIcon(this,stylesPopoverHelper,text).element();},_updateState:function()
{if(!this.listItemElement)
return;if(this._style.isPropertyImplicit(this.name))
this.listItemElement.classList.add("implicit");else
this.listItemElement.classList.remove("implicit");var hasIgnorableError=!this.property.parsedOk&&WebInspector.StylesSidebarPane.ignoreErrorsForProperty(this.property);if(hasIgnorableError)
this.listItemElement.classList.add("has-ignorable-error");else
this.listItemElement.classList.remove("has-ignorable-error");if(this.inherited())
this.listItemElement.classList.add("inherited");else
this.listItemElement.classList.remove("inherited");if(this.overloaded())
this.listItemElement.classList.add("overloaded");else
this.listItemElement.classList.remove("overloaded");if(this.property.disabled)
this.listItemElement.classList.add("disabled");else
this.listItemElement.classList.remove("disabled");},node:function()
{return this._parentPane.node();},parentPane:function()
{return this._parentPane;},section:function()
{return this.treeOutline&&this.treeOutline.section;},_updatePane:function()
{var section=this.section();if(section&&section._parentPane)
section._parentPane._refreshUpdate(section);},_styleTextEdited:function(oldStyleRange)
{var newStyleRange=(this._style.range);this._matchedStyles.resetActiveProperties();if(this._style.parentRule)
this._parentPane._styleSheetRuleEdited(this._style.parentRule,oldStyleRange,newStyleRange);},_toggleEnabled:function(event)
{var disabled=!event.target.checked;var oldStyleRange=this._style.range;if(!oldStyleRange)
return;function callback(success)
{delete this._parentPane._userOperation;if(!success)
return;this._styleTextEdited(oldStyleRange);this._updatePane();this.styleTextAppliedForTest();}
event.consume();this._parentPane._userOperation=true;this.property.setDisabled(disabled).then(callback.bind(this));},onpopulate:function()
{if(this.childCount()||!this.isShorthand)
return;var longhandProperties=this._style.longhandProperties(this.name);for(var i=0;i<longhandProperties.length;++i){var name=longhandProperties[i].name;var inherited=false;var overloaded=false;var section=this.section();if(section){inherited=section.isPropertyInherited(name);overloaded=this._matchedStyles.propertyState(longhandProperties[i])===WebInspector.CSSStyleModel.MatchedStyleResult.PropertyState.Overloaded;}
var item=new WebInspector.StylePropertyTreeElement(this._parentPane,this._matchedStyles,longhandProperties[i],false,inherited,overloaded);this.appendChild(item);}},onattach:function()
{this.updateTitle();this.listItemElement.addEventListener("mousedown",this._mouseDown.bind(this));this.listItemElement.addEventListener("mouseup",this._resetMouseDownElement.bind(this));this.listItemElement.addEventListener("click",this._mouseClick.bind(this));},_mouseDown:function(event)
{if(this._parentPane){this._parentPane._mouseDownTreeElement=this;this._parentPane._mouseDownTreeElementIsName=this.nameElement&&this.nameElement.isSelfOrAncestor(event.target);this._parentPane._mouseDownTreeElementIsValue=this.valueElement&&this.valueElement.isSelfOrAncestor(event.target);}},_resetMouseDownElement:function()
{if(this._parentPane){delete this._parentPane._mouseDownTreeElement;delete this._parentPane._mouseDownTreeElementIsName;delete this._parentPane._mouseDownTreeElementIsValue;}},updateTitle:function()
{this._updateState();this._expandElement=createElement("span");this._expandElement.className="expand-element";var propertyRenderer=new WebInspector.StylesSidebarPropertyRenderer(this._style.parentRule,this.node(),this.name,this.value);if(this.property.parsedOk){propertyRenderer.setColorHandler(this._processColor.bind(this));propertyRenderer.setBezierHandler(this._processBezier.bind(this));}
this.listItemElement.removeChildren();this.nameElement=propertyRenderer.renderName();this.valueElement=propertyRenderer.renderValue();if(!this.treeOutline)
return;var indent=WebInspector.moduleSetting("textEditorIndent").get();this.listItemElement.createChild("span","styles-clipboard-only").createTextChild(indent+(this.property.disabled?"/* ":""));this.listItemElement.appendChild(this.nameElement);this.listItemElement.createTextChild(": ");this.listItemElement.appendChild(this._expandElement);this.listItemElement.appendChild(this.valueElement);this.listItemElement.createTextChild(";");if(this.property.disabled)
this.listItemElement.createChild("span","styles-clipboard-only").createTextChild(" */");if(!this.property.parsedOk){this.listItemElement.classList.add("not-parsed-ok");this.listItemElement.insertBefore(WebInspector.StylesSidebarPane.createExclamationMark(this.property),this.listItemElement.firstChild);}
if(!this.property.activeInStyle())
this.listItemElement.classList.add("inactive");this._updateFilter();if(this.property.parsedOk&&this.section()&&this.parent.root){var enabledCheckboxElement=createElement("input");enabledCheckboxElement.className="enabled-button";enabledCheckboxElement.type="checkbox";enabledCheckboxElement.checked=!this.property.disabled;enabledCheckboxElement.addEventListener("click",this._toggleEnabled.bind(this),false);this.listItemElement.insertBefore(enabledCheckboxElement,this.listItemElement.firstChild);}},_mouseClick:function(event)
{if(!event.target.isComponentSelectionCollapsed())
return;event.consume(true);if(event.target===this.listItemElement){var section=this.section();if(!section||!section.editable)
return;if(section._checkWillCancelEditing())
return;section.addNewBlankProperty(this.property.index+1).startEditing();return;}
if(WebInspector.KeyboardShortcut.eventHasCtrlOrMeta((event))&&this.section().navigable){this._navigateToSource((event.target));return;}
this.startEditing((event.target));},_navigateToSource:function(element)
{console.assert(this.section().navigable);var propertyNameClicked=element===this.nameElement;var uiLocation=WebInspector.cssWorkspaceBinding.propertyUILocation(this.property,propertyNameClicked);if(uiLocation)
WebInspector.Revealer.reveal(uiLocation);},startEditing:function(selectElement)
{if(this.parent.isShorthand)
return;if(selectElement===this._expandElement)
return;var section=this.section();if(section&&!section.editable)
return;if(!selectElement)
selectElement=this.nameElement;else
selectElement=selectElement.enclosingNodeOrSelfWithClass("webkit-css-property")||selectElement.enclosingNodeOrSelfWithClass("value");if(WebInspector.isBeingEdited(selectElement))
return;var isEditingName=selectElement===this.nameElement;if(!isEditingName)
this.valueElement.textContent=restoreURLs(this.valueElement.textContent,this.value);function restoreURLs(fieldValue,modelValue)
{const urlRegex=/\b(url\([^)]*\))/g;var splitFieldValue=fieldValue.split(urlRegex);if(splitFieldValue.length===1)
return fieldValue;var modelUrlRegex=new RegExp(urlRegex);for(var i=1;i<splitFieldValue.length;i+=2){var match=modelUrlRegex.exec(modelValue);if(match)
splitFieldValue[i]=match[0];}
return splitFieldValue.join("");}
var context={expanded:this.expanded,hasChildren:this.isExpandable(),isEditingName:isEditingName,previousContent:selectElement.textContent};this.setExpandable(false);if(selectElement.parentElement)
selectElement.parentElement.classList.add("child-editing");selectElement.textContent=selectElement.textContent;function pasteHandler(context,event)
{var data=event.clipboardData.getData("Text");if(!data)
return;var colonIdx=data.indexOf(":");if(colonIdx<0)
return;var name=data.substring(0,colonIdx).trim();var value=data.substring(colonIdx+1).trim();event.preventDefault();if(!("originalName"in context)){context.originalName=this.nameElement.textContent;context.originalValue=this.valueElement.textContent;}
this.property.name=name;this.property.value=value;this.nameElement.textContent=name;this.valueElement.textContent=value;this.nameElement.normalize();this.valueElement.normalize();this.editingCommitted(event.target.textContent,context,"forward");}
function blurListener(context,event)
{var treeElement=this._parentPane._mouseDownTreeElement;var moveDirection="";if(treeElement===this){if(isEditingName&&this._parentPane._mouseDownTreeElementIsValue)
moveDirection="forward";if(!isEditingName&&this._parentPane._mouseDownTreeElementIsName)
moveDirection="backward";}
this.editingCommitted(event.target.textContent,context,moveDirection);}
this._originalPropertyText=this.property.propertyText;this._parentPane.setEditingStyle(true);if(selectElement.parentElement)
selectElement.parentElement.scrollIntoViewIfNeeded(false);var applyItemCallback=!isEditingName?this._applyFreeFlowStyleTextEdit.bind(this):undefined;this._prompt=new WebInspector.StylesSidebarPane.CSSPropertyPrompt(isEditingName?WebInspector.CSSMetadata.cssPropertiesMetainfo:WebInspector.CSSMetadata.keywordsForProperty(this.nameElement.textContent),this,isEditingName);this._prompt.setAutocompletionTimeout(0);if(applyItemCallback){this._prompt.addEventListener(WebInspector.TextPrompt.Events.ItemApplied,applyItemCallback,this);this._prompt.addEventListener(WebInspector.TextPrompt.Events.ItemAccepted,applyItemCallback,this);}
var proxyElement=this._prompt.attachAndStartEditing(selectElement,blurListener.bind(this,context));proxyElement.addEventListener("keydown",this._editingNameValueKeyDown.bind(this,context),false);proxyElement.addEventListener("keypress",this._editingNameValueKeyPress.bind(this,context),false);proxyElement.addEventListener("input",this._editingNameValueInput.bind(this,context),false);if(isEditingName)
proxyElement.addEventListener("paste",pasteHandler.bind(this,context),false);selectElement.getComponentSelection().setBaseAndExtent(selectElement,0,selectElement,1);},_editingNameValueKeyDown:function(context,event)
{if(event.handled)
return;var result;if(isEnterKey(event)){event.preventDefault();result="forward";}else if(event.keyCode===WebInspector.KeyboardShortcut.Keys.Esc.code||event.keyIdentifier==="U+001B")
result="cancel";else if(!context.isEditingName&&this._newProperty&&event.keyCode===WebInspector.KeyboardShortcut.Keys.Backspace.code){var selection=event.target.getComponentSelection();if(selection.isCollapsed&&!selection.focusOffset){event.preventDefault();result="backward";}}else if(event.keyIdentifier==="U+0009"){result=event.shiftKey?"backward":"forward";event.preventDefault();}
if(result){switch(result){case"cancel":this.editingCancelled(null,context);break;case"forward":case"backward":this.editingCommitted(event.target.textContent,context,result);break;}
event.consume();return;}},_editingNameValueKeyPress:function(context,event)
{function shouldCommitValueSemicolon(text,cursorPosition)
{var openQuote="";for(var i=0;i<cursorPosition;++i){var ch=text[i];if(ch==="\\"&&openQuote!=="")
++i;else if(!openQuote&&(ch==="\""||ch==="'"))
openQuote=ch;else if(openQuote===ch)
openQuote="";}
return!openQuote;}
var keyChar=String.fromCharCode(event.charCode);var isFieldInputTerminated=(context.isEditingName?keyChar===":":keyChar===";"&&shouldCommitValueSemicolon(event.target.textContent,event.target.selectionLeftOffset()));if(isFieldInputTerminated){event.consume(true);this.editingCommitted(event.target.textContent,context,"forward");return;}},_editingNameValueInput:function(context,event)
{if(!context.isEditingName&&(!this._parentPane.node().pseudoType()||this.name!=="content"))
this._applyFreeFlowStyleTextEdit();},_applyFreeFlowStyleTextEdit:function()
{var valueText=this.valueElement.textContent;if(valueText.indexOf(";")===-1)
this.applyStyleText(this.nameElement.textContent+": "+valueText,false);},kickFreeFlowStyleEditForTest:function()
{this._applyFreeFlowStyleTextEdit();},editingEnded:function(context)
{this._resetMouseDownElement();this.setExpandable(context.hasChildren);if(context.expanded)
this.expand();var editedElement=context.isEditingName?this.nameElement:this.valueElement;if(editedElement.parentElement)
editedElement.parentElement.classList.remove("child-editing");this._parentPane.setEditingStyle(false);},editingCancelled:function(element,context)
{this._removePrompt();this._revertStyleUponEditingCanceled();this.editingEnded(context);},_revertStyleUponEditingCanceled:function()
{if(this._propertyHasBeenEditedIncrementally){this.applyStyleText(this._originalPropertyText,false);delete this._originalPropertyText;}else if(this._newProperty){this.treeOutline.removeChild(this);}else{this.updateTitle();}},_findSibling:function(moveDirection)
{var target=this;do{target=(moveDirection==="forward"?target.nextSibling:target.previousSibling);}while(target&&target.inherited());return target;},editingCommitted:function(userInput,context,moveDirection)
{this._removePrompt();this.editingEnded(context);var isEditingName=context.isEditingName;var createNewProperty,moveToPropertyName,moveToSelector;var isDataPasted="originalName"in context;var isDirtyViaPaste=isDataPasted&&(this.nameElement.textContent!==context.originalName||this.valueElement.textContent!==context.originalValue);var isPropertySplitPaste=isDataPasted&&isEditingName&&this.valueElement.textContent!==context.originalValue;var moveTo=this;var moveToOther=(isEditingName^(moveDirection==="forward"));var abandonNewProperty=this._newProperty&&!userInput&&(moveToOther||isEditingName);if(moveDirection==="forward"&&(!isEditingName||isPropertySplitPaste)||moveDirection==="backward"&&isEditingName){moveTo=moveTo._findSibling(moveDirection);if(moveTo)
moveToPropertyName=moveTo.name;else if(moveDirection==="forward"&&(!this._newProperty||userInput))
createNewProperty=true;else if(moveDirection==="backward")
moveToSelector=true;}
var moveToIndex=moveTo&&this.treeOutline?this.treeOutline.rootElement().indexOfChild(moveTo):-1;var blankInput=/^\s*$/.test(userInput);var shouldCommitNewProperty=this._newProperty&&(isPropertySplitPaste||moveToOther||(!moveDirection&&!isEditingName)||(isEditingName&&blankInput));var section=(this.section());if(((userInput!==context.previousContent||isDirtyViaPaste)&&!this._newProperty)||shouldCommitNewProperty){section._afterUpdate=moveToNextCallback.bind(this,this._newProperty,!blankInput,section);var propertyText;if(blankInput||(this._newProperty&&/^\s*$/.test(this.valueElement.textContent)))
propertyText="";else{if(isEditingName)
propertyText=userInput+": "+this.property.value;else
propertyText=this.property.name+": "+userInput;}
this.applyStyleText(propertyText,true);}else{if(isEditingName)
this.property.name=userInput;else
this.property.value=userInput;if(!isDataPasted&&!this._newProperty)
this.updateTitle();moveToNextCallback.call(this,this._newProperty,false,section);}
function moveToNextCallback(alreadyNew,valueChanged,section)
{if(!moveDirection)
return;if(moveTo&&moveTo.parent){moveTo.startEditing(!isEditingName?moveTo.nameElement:moveTo.valueElement);return;}
if(moveTo&&!moveTo.parent){var rootElement=section.propertiesTreeOutline.rootElement();if(moveDirection==="forward"&&blankInput&&!isEditingName)
--moveToIndex;if(moveToIndex>=rootElement.childCount()&&!this._newProperty)
createNewProperty=true;else{var treeElement=moveToIndex>=0?rootElement.childAt(moveToIndex):null;if(treeElement){var elementToEdit=!isEditingName||isPropertySplitPaste?treeElement.nameElement:treeElement.valueElement;if(alreadyNew&&blankInput)
elementToEdit=moveDirection==="forward"?treeElement.nameElement:treeElement.valueElement;treeElement.startEditing(elementToEdit);return;}else if(!alreadyNew)
moveToSelector=true;}}
if(createNewProperty){if(alreadyNew&&!valueChanged&&(isEditingName^(moveDirection==="backward")))
return;section.addNewBlankProperty().startEditing();return;}
if(abandonNewProperty){moveTo=this._findSibling(moveDirection);var sectionToEdit=(moveTo||moveDirection==="backward")?section:section.nextEditableSibling();if(sectionToEdit){if(sectionToEdit.style().parentRule)
sectionToEdit.startEditingSelector();else
sectionToEdit._moveEditorFromSelector(moveDirection);}
return;}
if(moveToSelector){if(section.style().parentRule)
section.startEditingSelector();else
section._moveEditorFromSelector(moveDirection);}}},_removePrompt:function()
{if(this._prompt){this._prompt.detach();delete this._prompt;}},styleTextAppliedForTest:function(){},applyStyleText:function(styleText,majorChange)
{this._applyStyleThrottler.schedule(this._innerApplyStyleText.bind(this,styleText,majorChange));},_innerApplyStyleText:function(styleText,majorChange)
{if(!this.treeOutline)
return Promise.resolve();var oldStyleRange=this._style.range;if(!oldStyleRange)
return Promise.resolve();styleText=styleText.replace(/\s/g," ").trim();if(!styleText.length&&majorChange&&this._newProperty&&!this._propertyHasBeenEditedIncrementally){var section=this.section();this.parent.removeChild(this);section.afterUpdate();return Promise.resolve();}
var currentNode=this._parentPane.node();this._parentPane._userOperation=true;function callback(success)
{delete this._parentPane._userOperation;if(!success){if(majorChange){this._revertStyleUponEditingCanceled();}
this.styleTextAppliedForTest();return;}
this._styleTextEdited(oldStyleRange);this._propertyHasBeenEditedIncrementally=true;this.property=this._style.propertyAt(this.property.index);if(!this._parentPane._isEditingStyle&&currentNode===this.node())
this._updatePane();this.styleTextAppliedForTest();}
if(styleText.length&&!/;\s*$/.test(styleText))
styleText+=";";var overwriteProperty=!this._newProperty||this._propertyHasBeenEditedIncrementally;return this.property.setText(styleText,majorChange,overwriteProperty).then(callback.bind(this));},ondblclick:function()
{return true;},isEventWithinDisclosureTriangle:function(event)
{return event.target===this._expandElement;},__proto__:TreeElement.prototype}
WebInspector.StylesSidebarPane.CSSPropertyPrompt=function(cssCompletions,treeElement,isEditingName)
{WebInspector.TextPrompt.call(this,this._buildPropertyCompletions.bind(this),WebInspector.StyleValueDelimiters);this.setSuggestBoxEnabled(true);this._cssCompletions=cssCompletions;this._treeElement=treeElement;this._isEditingName=isEditingName;if(!isEditingName)
this.disableDefaultSuggestionForEmptyInput();}
WebInspector.StylesSidebarPane.CSSPropertyPrompt.prototype={onKeyDown:function(event)
{switch(event.keyIdentifier){case"Up":case"Down":case"PageUp":case"PageDown":if(this._handleNameOrValueUpDown(event)){event.preventDefault();return;}
break;case"Enter":if(this.autoCompleteElement&&!this.autoCompleteElement.textContent.length){this.tabKeyPressed();return;}
break;}
WebInspector.TextPrompt.prototype.onKeyDown.call(this,event);},onMouseWheel:function(event)
{if(this._handleNameOrValueUpDown(event)){event.consume(true);return;}
WebInspector.TextPrompt.prototype.onMouseWheel.call(this,event);},tabKeyPressed:function()
{this.acceptAutoComplete();return false;},_handleNameOrValueUpDown:function(event)
{function finishHandler(originalValue,replacementString)
{this._treeElement.applyStyleText(this._treeElement.nameElement.textContent+": "+this._treeElement.valueElement.textContent,false);}
function customNumberHandler(prefix,number,suffix)
{if(number!==0&&!suffix.length&&WebInspector.CSSMetadata.isLengthProperty(this._treeElement.property.name))
suffix="px";return prefix+number+suffix;}
if(!this._isEditingName&&WebInspector.handleElementValueModifications(event,this._treeElement.valueElement,finishHandler.bind(this),this._isValueSuggestion.bind(this),customNumberHandler.bind(this)))
return true;return false;},_isValueSuggestion:function(word)
{if(!word)
return false;word=word.toLowerCase();return this._cssCompletions.keySet().hasOwnProperty(word);},_buildPropertyCompletions:function(proxyElement,text,cursorOffset,wordRange,force,completionsReadyCallback)
{var prefix=wordRange.toString().toLowerCase();if(!prefix&&!force&&(this._isEditingName||proxyElement.textContent.length)){completionsReadyCallback([]);return;}
var results=this._cssCompletions.startsWith(prefix);if(!this._isEditingName&&!results.length&&prefix.length>1&&"!important".startsWith(prefix))
results.push("!important");var userEnteredText=wordRange.toString().replace("-","");if(userEnteredText&&(userEnteredText===userEnteredText.toUpperCase())){for(var i=0;i<results.length;++i)
results[i]=results[i].toUpperCase();}
var selectedIndex=this._cssCompletions.mostUsedOf(results);completionsReadyCallback(results,selectedIndex);},__proto__:WebInspector.TextPrompt.prototype}
WebInspector.StylesSidebarPropertyRenderer=function(rule,node,name,value)
{this._rule=rule;this._node=node;this._propertyName=name;this._propertyValue=value;}
WebInspector.StylesSidebarPropertyRenderer._colorRegex=/((?:rgb|hsl)a?\([^)]+\)|#[0-9a-fA-F]{6}|#[0-9a-fA-F]{3}|\b\w+\b(?!-))/g;WebInspector.StylesSidebarPropertyRenderer._bezierRegex=/((cubic-bezier\([^)]+\))|\b(linear|ease-in-out|ease-in|ease-out|ease)\b)/g;WebInspector.StylesSidebarPropertyRenderer._urlRegex=function(value)
{if(/url\(\s*'.*\s*'\s*\)/.test(value))
return/url\(\s*('.+')\s*\)/g;if(/url\(\s*".*\s*"\s*\)/.test(value))
return/url\(\s*(".+")\s*\)/g;return/url\(\s*([^)]+)\s*\)/g;}
WebInspector.StylesSidebarPropertyRenderer.prototype={setColorHandler:function(handler)
{this._colorHandler=handler;},setBezierHandler:function(handler)
{this._bezierHandler=handler;},renderName:function()
{var nameElement=createElement("span");nameElement.className="webkit-css-property";nameElement.textContent=this._propertyName;nameElement.normalize();return nameElement;},renderValue:function()
{var valueElement=createElement("span");valueElement.className="value";if(!this._propertyValue)
return valueElement;var formatter=new WebInspector.StringFormatter();formatter.addProcessor(WebInspector.StylesSidebarPropertyRenderer._urlRegex(this._propertyValue),this._processURL.bind(this));if(this._bezierHandler&&WebInspector.CSSMetadata.isBezierAwareProperty(this._propertyName))
formatter.addProcessor(WebInspector.StylesSidebarPropertyRenderer._bezierRegex,this._bezierHandler);if(this._colorHandler&&WebInspector.CSSMetadata.isColorAwareProperty(this._propertyName))
formatter.addProcessor(WebInspector.StylesSidebarPropertyRenderer._colorRegex,this._colorHandler);valueElement.appendChild(formatter.formatText(this._propertyValue));valueElement.normalize();return valueElement;},_processURL:function(url)
{var hrefUrl=url;var match=hrefUrl.match(/['"]?([^'"]+)/);if(match)
hrefUrl=match[1];var container=createDocumentFragment();container.createTextChild("url(");if(this._rule&&this._rule.resourceURL())
hrefUrl=WebInspector.ParsedURL.completeURL(this._rule.resourceURL(),hrefUrl);else if(this._node)
hrefUrl=this._node.resolveURL(hrefUrl);var hasResource=hrefUrl&&!!WebInspector.resourceForURL(hrefUrl);container.appendChild(WebInspector.linkifyURLAsNode(hrefUrl||url,url,undefined,!hasResource));container.createTextChild(")");return container;}}
WebInspector.StylesSidebarPane.createAddNewRuleButton=function(stylesSidebarPane)
{var button=new WebInspector.ToolbarButton(WebInspector.UIString("New Style Rule"),"add-toolbar-item");button.makeLongClickEnabled();button.addEventListener("click",stylesSidebarPane._createNewRuleInViaInspectorStyleSheet,stylesSidebarPane);button.addEventListener("longClickDown",stylesSidebarPane._onAddButtonLongClick,stylesSidebarPane);WebInspector.context.addFlavorChangeListener(WebInspector.DOMNode,onNodeChanged);onNodeChanged();return button;function onNodeChanged()
{var node=WebInspector.context.flavor(WebInspector.DOMNode);button.setEnabled(!!node);}};WebInspector.ComputedStyleWidget=function(stylesSidebarPane,sharedModel)
{WebInspector.ThrottledWidget.call(this);this.element.classList.add("computed-style-sidebar-pane");this.registerRequiredCSS("elements/computedStyleSidebarPane.css");this._alwaysShowComputedProperties={"display":true,"height":true,"width":true};this._sharedModel=sharedModel;this._sharedModel.addEventListener(WebInspector.SharedSidebarModel.Events.ComputedStyleChanged,this.update,this);this._showInheritedComputedStylePropertiesSetting=WebInspector.settings.createSetting("showInheritedComputedStyleProperties",false);this._showInheritedComputedStylePropertiesSetting.addChangeListener(this._showInheritedComputedStyleChanged.bind(this));var hbox=this.element.createChild("div","hbox styles-sidebar-pane-toolbar");var filterContainerElement=hbox.createChild("div","styles-sidebar-pane-filter-box");var filterInput=WebInspector.StylesSidebarPane.createPropertyFilterElement(WebInspector.UIString("Filter"),hbox,filterCallback.bind(this));filterContainerElement.appendChild(filterInput);var toolbar=new WebInspector.Toolbar(hbox);toolbar.element.classList.add("styles-pane-toolbar");toolbar.appendToolbarItem(new WebInspector.ToolbarCheckbox(WebInspector.UIString("Show all"),undefined,this._showInheritedComputedStylePropertiesSetting));this._propertiesOutline=new TreeOutlineInShadow();this._propertiesOutline.registerRequiredCSS("elements/computedStyleSidebarPane.css");this._propertiesOutline.element.classList.add("monospace","computed-properties");this.element.appendChild(this._propertiesOutline.element);this._stylesSidebarPane=stylesSidebarPane;this._linkifier=new WebInspector.Linkifier(new WebInspector.Linkifier.DefaultCSSFormatter());function filterCallback(regex)
{this._filterRegex=regex;this._updateFilter(regex);}}
WebInspector.ComputedStyleWidget.createSidebarWrapper=function(stylesSidebarPane,sharedModel)
{var widget=new WebInspector.ComputedStyleWidget(stylesSidebarPane,sharedModel);return new WebInspector.ElementsSidebarViewWrapperPane(WebInspector.UIString("Computed Style"),widget)}
WebInspector.ComputedStyleWidget._propertySymbol=Symbol("property");WebInspector.ComputedStyleWidget.prototype={_showInheritedComputedStyleChanged:function()
{this.update();},doUpdate:function()
{var promises=[this._sharedModel.fetchComputedStyle(),this._stylesSidebarPane.fetchMatchedCascade()];return Promise.all(promises).spread(this._innerRebuildUpdate.bind(this));},_processColor:function(text)
{var color=WebInspector.Color.parse(text);if(!color)
return createTextNode(text);var swatch=WebInspector.ColorSwatch.create();swatch.setColorText(text);return swatch;},_innerRebuildUpdate:function(nodeStyle,matchedStyles)
{this._propertiesOutline.removeChildren();this._linkifier.reset();var cssModel=this._sharedModel.cssModel();if(!nodeStyle||!matchedStyles||!cssModel)
return;var uniqueProperties=nodeStyle.computedStyle.keysArray();uniqueProperties.sort(propertySorter);var propertyTraces=this._computePropertyTraces(matchedStyles);var inhertiedProperties=this._computeInheritedProperties(matchedStyles);var showInherited=this._showInheritedComputedStylePropertiesSetting.get();for(var i=0;i<uniqueProperties.length;++i){var propertyName=uniqueProperties[i];var propertyValue=nodeStyle.computedStyle.get(propertyName);var canonicalName=WebInspector.CSSMetadata.canonicalPropertyName(propertyName);var inherited=!inhertiedProperties.has(canonicalName);if(!showInherited&&inherited&&!(propertyName in this._alwaysShowComputedProperties))
continue;if(propertyName!==canonicalName&&propertyValue===nodeStyle.computedStyle.get(canonicalName))
continue;var propertyElement=createElement("div");propertyElement.classList.add("computed-style-property");propertyElement.classList.toggle("computed-style-property-inherited",inherited);var renderer=new WebInspector.StylesSidebarPropertyRenderer(null,nodeStyle.node,propertyName,(propertyValue));renderer.setColorHandler(this._processColor.bind(this));var propertyNameElement=renderer.renderName();propertyNameElement.classList.add("property-name");propertyElement.appendChild(propertyNameElement);var propertyValueElement=renderer.renderValue();propertyValueElement.classList.add("property-value");propertyElement.appendChild(propertyValueElement);var treeElement=new TreeElement();treeElement.selectable=false;treeElement.title=propertyElement;treeElement[WebInspector.ComputedStyleWidget._propertySymbol]={name:propertyName,value:propertyValue};var isOdd=this._propertiesOutline.rootElement().children().length%2===0;treeElement.listItemElement.classList.toggle("odd-row",isOdd);this._propertiesOutline.appendChild(treeElement);var trace=propertyTraces.get(propertyName);if(trace){this._renderPropertyTrace(cssModel,matchedStyles,nodeStyle.node,treeElement,trace);treeElement.listItemElement.addEventListener("mousedown",consumeEvent,false);treeElement.listItemElement.addEventListener("dblclick",consumeEvent,false);treeElement.listItemElement.addEventListener("click",handleClick.bind(null,treeElement),false);}}
this._updateFilter(this._filterRegex);function propertySorter(a,b)
{if(a.startsWith("-webkit")^b.startsWith("-webkit"))
return a.startsWith("-webkit")?1:-1;var canonicalName=WebInspector.CSSMetadata.canonicalPropertyName;return canonicalName(a).compareTo(canonicalName(b));}
function handleClick(treeElement,event)
{if(!treeElement.expanded)
treeElement.expand();else
treeElement.collapse();consumeEvent(event);}},_renderPropertyTrace:function(cssModel,matchedStyles,node,rootTreeElement,tracedProperties)
{for(var property of tracedProperties){var trace=createElement("div");trace.classList.add("property-trace");if(matchedStyles.propertyState(property)===WebInspector.CSSStyleModel.MatchedStyleResult.PropertyState.Overloaded)
trace.classList.add("property-trace-inactive");var renderer=new WebInspector.StylesSidebarPropertyRenderer(null,node,property.name,(property.value));renderer.setColorHandler(this._processColor.bind(this));var valueElement=renderer.renderValue();valueElement.classList.add("property-trace-value");trace.appendChild(valueElement);var rule=property.ownerStyle.parentRule;if(rule){var linkSpan=trace.createChild("span","trace-link");linkSpan.appendChild(WebInspector.StylePropertiesSection.createRuleOriginNode(cssModel,this._linkifier,rule));}
var selectorElement=trace.createChild("span","property-trace-selector");selectorElement.textContent=rule?rule.selectorText():"element.style";selectorElement.title=selectorElement.textContent;var traceTreeElement=new TreeElement();traceTreeElement.title=trace;traceTreeElement.selectable=false;rootTreeElement.appendChild(traceTreeElement);}},_computePropertyTraces:function(matchedStyles)
{var result=new Map();for(var style of matchedStyles.nodeStyles()){var allProperties=style.allProperties;for(var property of allProperties){if(!property.activeInStyle()||!matchedStyles.propertyState(property))
continue;if(!result.has(property.name))
result.set(property.name,[]);result.get(property.name).push(property);}}
return result;},_computeInheritedProperties:function(matchedStyles)
{var result=new Set();for(var style of matchedStyles.nodeStyles()){for(var property of style.allProperties){if(!matchedStyles.propertyState(property))
continue;result.add(WebInspector.CSSMetadata.canonicalPropertyName(property.name));}}
return result;},_updateFilter:function(regex)
{var children=this._propertiesOutline.rootElement().children();for(var child of children){var property=child[WebInspector.ComputedStyleWidget._propertySymbol];var matched=!regex||regex.test(property.name)||regex.test(property.value);child.hidden=!matched;}},__proto__:WebInspector.ThrottledWidget.prototype};WebInspector.ElementsPanel=function()
{WebInspector.Panel.call(this,"elements");this.registerRequiredCSS("elements/elementsPanel.css");this._splitWidget=new WebInspector.SplitWidget(true,true,"elementsPanelSplitViewState",325,325);this._splitWidget.addEventListener(WebInspector.SplitWidget.Events.SidebarSizeChanged,this._updateTreeOutlineVisibleWidth.bind(this));this._splitWidget.show(this.element);this._searchableView=new WebInspector.SearchableView(this);this._searchableView.setMinimumSize(25,28);this._searchableView.setPlaceholder(WebInspector.UIString("Find by string, selector, or XPath"));var stackElement=this._searchableView.element;this._contentElement=createElement("div");var crumbsContainer=createElement("div");stackElement.appendChild(this._contentElement);stackElement.appendChild(crumbsContainer);this._treeOutlineSplit=new WebInspector.SplitWidget(false,true,"treeOutlineAnimationTimelineWidget",300,300);this._treeOutlineSplit.hideSidebar();this._treeOutlineSplit.setMainWidget(this._searchableView);this._splitWidget.setMainWidget(this._treeOutlineSplit);this._contentElement.id="elements-content";if(WebInspector.moduleSetting("domWordWrap").get())
this._contentElement.classList.add("elements-wrap");WebInspector.moduleSetting("domWordWrap").addChangeListener(this._domWordWrapSettingChanged.bind(this));crumbsContainer.id="elements-crumbs";this._breadcrumbs=new WebInspector.ElementsBreadcrumbs();this._breadcrumbs.show(crumbsContainer);this._breadcrumbs.addEventListener(WebInspector.ElementsBreadcrumbs.Events.NodeSelected,this._crumbNodeSelected,this);this.sidebarPanes={};this._elementsSidebarViewWrappers=[];this._currentToolbarPane=null;this._toolbarPaneElement=createElementWithClass("div","styles-sidebar-toolbar-pane");var sharedSidebarModel=new WebInspector.SharedSidebarModel();this.sidebarPanes.platformFonts=WebInspector.PlatformFontsWidget.createSidebarWrapper(sharedSidebarModel);this.sidebarPanes.styles=new WebInspector.StylesSidebarPane(this._toolbarPaneElement);this.sidebarPanes.computedStyle=WebInspector.ComputedStyleWidget.createSidebarWrapper(this.sidebarPanes.styles,sharedSidebarModel);this.sidebarPanes.styles.addEventListener(WebInspector.StylesSidebarPane.Events.SelectorEditingStarted,this._onEditingSelectorStarted.bind(this));this.sidebarPanes.styles.addEventListener(WebInspector.StylesSidebarPane.Events.SelectorEditingEnded,this._onEditingSelectorEnded.bind(this));this.sidebarPanes.metrics=new WebInspector.MetricsSidebarPane();this.sidebarPanes.properties=WebInspector.PropertiesWidget.createSidebarWrapper();this.sidebarPanes.domBreakpoints=WebInspector.domBreakpointsSidebarPane.createProxy(this);this.sidebarPanes.eventListeners=WebInspector.EventListenersWidget.createSidebarWrapper();WebInspector.moduleSetting("sidebarPosition").addChangeListener(this._updateSidebarPosition.bind(this));this._updateSidebarPosition();this._loadSidebarViews();this._treeOutlines=[];this._modelToTreeOutline=new Map();WebInspector.targetManager.observeTargets(this);WebInspector.moduleSetting("showUAShadowDOM").addChangeListener(this._showUAShadowDOMChanged.bind(this));WebInspector.targetManager.addModelListener(WebInspector.DOMModel,WebInspector.DOMModel.Events.DocumentUpdated,this._documentUpdatedEvent,this);WebInspector.extensionServer.addEventListener(WebInspector.ExtensionServer.Events.SidebarPaneAdded,this._extensionSidebarPaneAdded,this);}
WebInspector.ElementsPanel._elementsSidebarViewTitleSymbol=Symbol("title");WebInspector.ElementsPanel.prototype={showToolbarPane:function(widget)
{if(this._animatedToolbarPane!==undefined)
this._pendingWidget=widget;else
this._startToolbarPaneAnimation(widget);},_startToolbarPaneAnimation:function(widget)
{if(widget===this._currentToolbarPane)
return;if(widget&&this._currentToolbarPane){this._currentToolbarPane.detach();widget.show(this._toolbarPaneElement);this._currentToolbarPane=widget;return;}
this._animatedToolbarPane=widget;if(this._currentToolbarPane)
this._toolbarPaneElement.style.animationName='styles-element-state-pane-slideout';else if(widget)
this._toolbarPaneElement.style.animationName='styles-element-state-pane-slidein';if(widget)
widget.show(this._toolbarPaneElement);var listener=onAnimationEnd.bind(this);this._toolbarPaneElement.addEventListener("animationend",listener,false);function onAnimationEnd()
{this._toolbarPaneElement.style.removeProperty('animation-name');this._toolbarPaneElement.removeEventListener("animationend",listener,false);if(this._currentToolbarPane)
this._currentToolbarPane.detach();this._currentToolbarPane=this._animatedToolbarPane;delete this._animatedToolbarPane;if(this._pendingWidget!==undefined){this._startToolbarPaneAnimation(this._pendingWidget);delete this._pendingWidget;}}},_toggleHideElement:function()
{var node=this.selectedDOMNode();var treeOutline=this._treeOutlineForNode(node);if(!node||!treeOutline)
return;treeOutline.toggleHideElement(node);},_toggleEditAsHTML:function()
{var node=this.selectedDOMNode();var treeOutline=this._treeOutlineForNode(node);if(!node||!treeOutline)
return;treeOutline.toggleEditAsHTML(node);},_loadSidebarViews:function()
{var extensions=self.runtime.extensions("@WebInspector.Widget");for(var i=0;i<extensions.length;++i){var descriptor=extensions[i].descriptor();if(descriptor["location"]!=="elements-panel")
continue;var title=WebInspector.UIString(descriptor["title"]);extensions[i].instancePromise().then(addSidebarView.bind(this,title));}
function addSidebarView(title,object)
{var widget=(object);var elementsSidebarViewWrapperPane=new WebInspector.ElementsSidebarViewWrapperPane(title,widget);this._elementsSidebarViewWrappers.push(elementsSidebarViewWrapperPane);if(this.sidebarPaneView)
this.sidebarPaneView.addPane(elementsSidebarViewWrapperPane);}},_onEditingSelectorStarted:function()
{for(var i=0;i<this._treeOutlines.length;++i)
this._treeOutlines[i].setPickNodeMode(true);},_onEditingSelectorEnded:function()
{for(var i=0;i<this._treeOutlines.length;++i)
this._treeOutlines[i].setPickNodeMode(false);},targetAdded:function(target)
{var domModel=WebInspector.DOMModel.fromTarget(target);if(!domModel)
return;var treeOutline=new WebInspector.ElementsTreeOutline(domModel,true,true);treeOutline.setWordWrap(WebInspector.moduleSetting("domWordWrap").get());treeOutline.wireToDOMModel();treeOutline.addEventListener(WebInspector.ElementsTreeOutline.Events.SelectedNodeChanged,this._selectedNodeChanged,this);treeOutline.addEventListener(WebInspector.ElementsTreeOutline.Events.NodePicked,this._onNodePicked,this);treeOutline.addEventListener(WebInspector.ElementsTreeOutline.Events.ElementsTreeUpdated,this._updateBreadcrumbIfNeeded,this);new WebInspector.ElementsTreeElementHighlighter(treeOutline);this._treeOutlines.push(treeOutline);this._modelToTreeOutline.set(domModel,treeOutline);if(this.isShowing())
this.wasShown();},targetRemoved:function(target)
{var domModel=WebInspector.DOMModel.fromTarget(target);if(!domModel)
return;var treeOutline=this._modelToTreeOutline.remove(domModel);treeOutline.unwireFromDOMModel();this._treeOutlines.remove(treeOutline);treeOutline.element.remove();},_updateTreeOutlineVisibleWidth:function()
{if(!this._treeOutlines.length)
return;var width=this._splitWidget.element.offsetWidth;if(this._splitWidget.isVertical())
width-=this._splitWidget.sidebarSize();for(var i=0;i<this._treeOutlines.length;++i){this._treeOutlines[i].setVisibleWidth(width);this._treeOutlines[i].updateSelection();}
this._breadcrumbs.updateSizes();},defaultFocusedElement:function()
{return this._treeOutlines.length?this._treeOutlines[0].element:this.element;},searchableView:function()
{return this._searchableView;},wasShown:function()
{WebInspector.context.setFlavor(WebInspector.ElementsPanel,this);for(var i=0;i<this._treeOutlines.length;++i){var treeOutline=this._treeOutlines[i];if(treeOutline.element.parentElement!==this._contentElement)
this._contentElement.appendChild(treeOutline.element);}
WebInspector.Panel.prototype.wasShown.call(this);this._breadcrumbs.update();for(var i=0;i<this._treeOutlines.length;++i){var treeOutline=this._treeOutlines[i];treeOutline.updateSelection();treeOutline.setVisible(true);if(!treeOutline.rootDOMNode)
if(treeOutline.domModel().existingDocument())
this._documentUpdated(treeOutline.domModel(),treeOutline.domModel().existingDocument());else
treeOutline.domModel().requestDocument();}},willHide:function()
{WebInspector.context.setFlavor(WebInspector.ElementsPanel,null);WebInspector.DOMModel.hideDOMNodeHighlight();for(var i=0;i<this._treeOutlines.length;++i){var treeOutline=this._treeOutlines[i];treeOutline.setVisible(false);this._contentElement.removeChild(treeOutline.element);}
if(this._popoverHelper)
this._popoverHelper.hidePopover();WebInspector.Panel.prototype.willHide.call(this);},onResize:function()
{if(WebInspector.moduleSetting("sidebarPosition").get()==="auto")
this.element.window().requestAnimationFrame(this._updateSidebarPosition.bind(this));this._updateTreeOutlineVisibleWidth();},_onNodePicked:function(event)
{if(!this.sidebarPanes.styles.isEditingSelector())
return;this.sidebarPanes.styles.updateEditingSelectorForNode((event.data));},_selectedNodeChanged:function(event)
{var selectedNode=(event.data);for(var i=0;i<this._treeOutlines.length;++i){if(!selectedNode||selectedNode.domModel()!==this._treeOutlines[i].domModel())
this._treeOutlines[i].selectDOMNode(null);}
if(!selectedNode&&this._lastValidSelectedNode)
this._selectedPathOnReset=this._lastValidSelectedNode.path();this._breadcrumbs.setSelectedNode(selectedNode);WebInspector.context.setFlavor(WebInspector.DOMNode,selectedNode);if(selectedNode){selectedNode.setAsInspectedNode();this._lastValidSelectedNode=selectedNode;}
WebInspector.notifications.dispatchEventToListeners(WebInspector.NotificationService.Events.SelectedNodeChanged);this._selectedNodeChangedForTest();},_selectedNodeChangedForTest:function(){},_reset:function()
{delete this.currentQuery;},_documentUpdatedEvent:function(event)
{this._documentUpdated((event.target),(event.data));},_documentUpdated:function(domModel,inspectedRootDocument)
{this._reset();this.searchCanceled();var treeOutline=this._modelToTreeOutline.get(domModel);treeOutline.rootDOMNode=inspectedRootDocument;if(!inspectedRootDocument){if(this.isShowing())
domModel.requestDocument();return;}
WebInspector.domBreakpointsSidebarPane.restoreBreakpoints(domModel);function selectNode(candidateFocusNode)
{if(!candidateFocusNode)
candidateFocusNode=inspectedRootDocument.body||inspectedRootDocument.documentElement;if(!candidateFocusNode)
return;if(!this._pendingNodeReveal){this.selectDOMNode(candidateFocusNode);if(treeOutline.selectedTreeElement)
treeOutline.selectedTreeElement.expand();}}
function selectLastSelectedNode(nodeId)
{if(this.selectedDOMNode()){return;}
var node=nodeId?domModel.nodeForId(nodeId):null;selectNode.call(this,node);}
if(this._omitDefaultSelection)
return;if(this._selectedPathOnReset)
domModel.pushNodeByPathToFrontend(this._selectedPathOnReset,selectLastSelectedNode.bind(this));else
selectNode.call(this,null);delete this._selectedPathOnReset;},searchCanceled:function()
{delete this._searchQuery;this._hideSearchHighlights();this._searchableView.updateSearchMatchesCount(0);delete this._currentSearchResultIndex;delete this._searchResults;WebInspector.DOMModel.cancelSearch();},performSearch:function(searchConfig,shouldJump,jumpBackwards)
{var query=searchConfig.query;this.searchCanceled();const whitespaceTrimmedQuery=query.trim();if(!whitespaceTrimmedQuery.length)
return;this._searchQuery=query;var promises=[];var domModels=WebInspector.DOMModel.instances();for(var domModel of domModels)
promises.push(domModel.performSearchPromise(whitespaceTrimmedQuery,WebInspector.moduleSetting("showUAShadowDOM").get()));Promise.all(promises).then(resultCountCallback.bind(this));function resultCountCallback(resultCounts)
{this._searchResults=[];for(var i=0;i<resultCounts.length;++i){var resultCount=resultCounts[i];for(var j=0;j<resultCount;++j)
this._searchResults.push({domModel:domModels[i],index:j,node:undefined});}
this._searchableView.updateSearchMatchesCount(this._searchResults.length);if(!this._searchResults.length)
return;this._currentSearchResultIndex=-1;if(shouldJump)
this._jumpToSearchResult(jumpBackwards?-1:0);}},_domWordWrapSettingChanged:function(event)
{this._contentElement.classList.toggle("elements-wrap",event.data);for(var i=0;i<this._treeOutlines.length;++i)
this._treeOutlines[i].setWordWrap((event.data));var selectedNode=this.selectedDOMNode();if(!selectedNode)
return;var treeElement=this._treeElementForNode(selectedNode);if(treeElement)
treeElement.updateSelection();},switchToAndFocus:function(node)
{this._searchableView.cancelSearch();WebInspector.inspectorView.setCurrentPanel(this);this.selectDOMNode(node,true);},_getPopoverAnchor:function(element,event)
{var anchor=element.enclosingNodeOrSelfWithClass("webkit-html-resource-link");if(!anchor||!anchor.href)
return;return anchor;},_showPopover:function(anchor,popover)
{var node=this.selectedDOMNode();if(node)
WebInspector.DOMPresentationUtils.buildImagePreviewContents(node.target(),anchor.href,true,showPopover);function showPopover(contents)
{if(!contents)
return;popover.setCanShrink(false);popover.showForAnchor(contents,anchor);}},_jumpToSearchResult:function(index)
{this._hideSearchHighlights();this._currentSearchResultIndex=(index+this._searchResults.length)%this._searchResults.length;this._highlightCurrentSearchResult();},jumpToNextSearchResult:function()
{if(!this._searchResults)
return;this._jumpToSearchResult(this._currentSearchResultIndex+1);},jumpToPreviousSearchResult:function()
{if(!this._searchResults)
return;this._jumpToSearchResult(this._currentSearchResultIndex-1);},supportsCaseSensitiveSearch:function()
{return false;},supportsRegexSearch:function()
{return false;},_highlightCurrentSearchResult:function()
{var index=this._currentSearchResultIndex;var searchResults=this._searchResults;var searchResult=searchResults[index];if(searchResult.node===null){this._searchableView.updateCurrentMatchIndex(index);return;}
function searchCallback(node)
{searchResult.node=node;this._highlightCurrentSearchResult();}
if(typeof searchResult.node==="undefined"){searchResult.domModel.searchResult(searchResult.index,searchCallback.bind(this));return;}
this._searchableView.updateCurrentMatchIndex(index);var treeElement=this._treeElementForNode(searchResult.node);if(treeElement){treeElement.highlightSearchResults(this._searchQuery);treeElement.reveal();var matches=treeElement.listItemElement.getElementsByClassName(WebInspector.highlightedSearchResultClassName);if(matches.length)
matches[0].scrollIntoViewIfNeeded();}},_hideSearchHighlights:function()
{if(!this._searchResults||!this._searchResults.length||this._currentSearchResultIndex<0)
return;var searchResult=this._searchResults[this._currentSearchResultIndex];if(!searchResult.node)
return;var treeOutline=this._modelToTreeOutline.get(searchResult.node.domModel());var treeElement=treeOutline.findTreeElement(searchResult.node);if(treeElement)
treeElement.hideSearchHighlights();},selectedDOMNode:function()
{for(var i=0;i<this._treeOutlines.length;++i){var treeOutline=this._treeOutlines[i];if(treeOutline.selectedDOMNode())
return treeOutline.selectedDOMNode();}
return null;},selectDOMNode:function(node,focus)
{for(var i=0;i<this._treeOutlines.length;++i){var treeOutline=this._treeOutlines[i];if(treeOutline.domModel()===node.domModel())
treeOutline.selectDOMNode(node,focus);else
treeOutline.selectDOMNode(null);}},_updateBreadcrumbIfNeeded:function(event)
{var nodes=(event.data);this._breadcrumbs.updateNodes(nodes);},_crumbNodeSelected:function(event)
{var node=(event.data);this.selectDOMNode(node,true);},handleShortcut:function(event)
{function handleUndoRedo(treeOutline)
{if(WebInspector.KeyboardShortcut.eventHasCtrlOrMeta(event)&&!event.shiftKey&&event.keyIdentifier==="U+005A"){treeOutline.domModel().undo();event.handled=true;return;}
var isRedoKey=WebInspector.isMac()?event.metaKey&&event.shiftKey&&event.keyIdentifier==="U+005A":event.ctrlKey&&event.keyIdentifier==="U+0059";if(isRedoKey){treeOutline.domModel().redo();event.handled=true;}}
if(WebInspector.isEditing()&&event.keyCode!==WebInspector.KeyboardShortcut.Keys.F2.code)
return;var treeOutline=null;for(var i=0;i<this._treeOutlines.length;++i){if(this._treeOutlines[i].selectedDOMNode()===this._lastValidSelectedNode)
treeOutline=this._treeOutlines[i];}
if(!treeOutline)
return;if(!treeOutline.editing()){handleUndoRedo.call(null,treeOutline);if(event.handled){this.sidebarPanes.styles.onUndoOrRedoHappened();return;}}
treeOutline.handleShortcut(event);if(event.handled)
return;WebInspector.Panel.prototype.handleShortcut.call(this,event);},_treeOutlineForNode:function(node)
{if(!node)
return null;return this._modelToTreeOutline.get(node.domModel())||null;},_focusedTreeOutline:function()
{for(var i=0;i<this._treeOutlines.length;++i){if(this._treeOutlines[i].hasFocus())
return this._treeOutlines[i];}
return null;},_treeElementForNode:function(node)
{var treeOutline=this._treeOutlineForNode(node);return(treeOutline.findTreeElement(node));},handleCopyEvent:function(event)
{var treeOutline=this._focusedTreeOutline();if(treeOutline)
treeOutline.handleCopyOrCutKeyboardEvent(false,event);},handleCutEvent:function(event)
{var treeOutline=this._focusedTreeOutline();if(treeOutline)
treeOutline.handleCopyOrCutKeyboardEvent(true,event);},handlePasteEvent:function(event)
{var treeOutline=this._focusedTreeOutline();if(treeOutline)
treeOutline.handlePasteKeyboardEvent(event);},_leaveUserAgentShadowDOM:function(node)
{var userAgentShadowRoot=node.ancestorUserAgentShadowRoot();return userAgentShadowRoot?(userAgentShadowRoot.parentNode):node;},revealAndSelectNode:function(node)
{if(WebInspector.inspectElementModeController&&WebInspector.inspectElementModeController.isInInspectElementMode())
WebInspector.inspectElementModeController.stopInspection();this._omitDefaultSelection=true;var showLayoutEditor=!!WebInspector.inspectElementModeController&&WebInspector.inspectElementModeController.isInLayoutEditorMode();WebInspector.inspectorView.setCurrentPanel(this,showLayoutEditor);node=WebInspector.moduleSetting("showUAShadowDOM").get()?node:this._leaveUserAgentShadowDOM(node);if(!showLayoutEditor)
node.highlightForTwoSeconds();this.selectDOMNode(node,true);delete this._omitDefaultSelection;if(!this._notFirstInspectElement)
InspectorFrontendHost.inspectElementCompleted();this._notFirstInspectElement=true;},appendApplicableItems:function(event,contextMenu,object)
{if(!(object instanceof WebInspector.RemoteObject&&((object)).isNode())&&!(object instanceof WebInspector.DOMNode)&&!(object instanceof WebInspector.DeferredDOMNode)){return;}
if(object instanceof WebInspector.DOMNode){contextMenu.appendSeparator();WebInspector.domBreakpointsSidebarPane.populateNodeContextMenu(object,contextMenu,true);}
if(this.element.isAncestor((event.target)))
return;var commandCallback=WebInspector.Revealer.reveal.bind(WebInspector.Revealer,object);contextMenu.appendItem(WebInspector.UIString.capitalize("Reveal in Elements ^panel"),commandCallback);},_sidebarContextMenuEventFired:function(event)
{var contextMenu=new WebInspector.ContextMenu(event);contextMenu.appendApplicableItems((event.deepElementFromPoint()));contextMenu.show();},_showUAShadowDOMChanged:function()
{for(var i=0;i<this._treeOutlines.length;++i)
this._treeOutlines[i].update();},_updateSidebarPosition:function()
{var vertically;var position=WebInspector.moduleSetting("sidebarPosition").get();if(position==="right")
vertically=false;else if(position==="bottom")
vertically=true;else
vertically=WebInspector.inspectorView.element.offsetWidth<680;if(this.sidebarPaneView&&vertically===!this._splitWidget.isVertical())
return;if(this.sidebarPaneView&&this.sidebarPaneView.shouldHideOnDetach())
return;var selectedTabId=this.sidebarPaneView?this.sidebarPaneView.selectedTabId:null;var extensionSidebarPanes=WebInspector.extensionServer.sidebarPanes();if(this.sidebarPaneView){this.sidebarPaneView.detach();this._splitWidget.uninstallResizer(this.sidebarPaneView.headerElement());}
this._splitWidget.setVertical(!vertically);var computedPane=new WebInspector.SidebarPane(WebInspector.UIString("Computed"));computedPane.element.classList.add("composite");computedPane.element.classList.add("fill");computedPane.element.classList.add("metrics-and-computed");var matchedStylePanesWrapper=new WebInspector.VBox();matchedStylePanesWrapper.element.classList.add("style-panes-wrapper");var computedStylePanesWrapper=new WebInspector.VBox();computedStylePanesWrapper.element.classList.add("style-panes-wrapper");function showMetrics(inComputedStyle)
{if(inComputedStyle)
this.sidebarPanes.metrics.show(computedStylePanesWrapper.element,this.sidebarPanes.computedStyle.element);else
this.sidebarPanes.metrics.show(matchedStylePanesWrapper.element);}
function tabSelected(event)
{var tabId=(event.data.tabId);if(tabId===computedPane.title())
showMetrics.call(this,true);else if(tabId===stylesPane.title())
showMetrics.call(this,false);}
this.sidebarPaneView=new WebInspector.SidebarTabbedPane();this.sidebarPaneView.element.addEventListener("contextmenu",this._sidebarContextMenuEventFired.bind(this),false);if(this._popoverHelper)
this._popoverHelper.hidePopover();this._popoverHelper=new WebInspector.PopoverHelper(this.sidebarPaneView.element,this._getPopoverAnchor.bind(this),this._showPopover.bind(this));this._popoverHelper.setTimeout(0);if(vertically){this._splitWidget.installResizer(this.sidebarPaneView.headerElement());var compositePane=new WebInspector.SidebarPane(this.sidebarPanes.styles.title());compositePane.element.classList.add("composite");compositePane.element.classList.add("fill");var splitWidget=new WebInspector.SplitWidget(true,true,"stylesPaneSplitViewState",215);splitWidget.show(compositePane.element);splitWidget.setMainWidget(matchedStylePanesWrapper);splitWidget.setSidebarWidget(computedStylePanesWrapper);computedPane.show(computedStylePanesWrapper.element);this.sidebarPaneView.addPane(compositePane);}else{var stylesPane=new WebInspector.SidebarPane(this.sidebarPanes.styles.title());stylesPane.element.classList.add("composite","fill","metrics-and-styles");matchedStylePanesWrapper.show(stylesPane.element);computedStylePanesWrapper.show(computedPane.element);this.sidebarPaneView.addEventListener(WebInspector.TabbedPane.EventTypes.TabSelected,tabSelected,this);this.sidebarPaneView.addPane(stylesPane);this.sidebarPaneView.addPane(computedPane);}
this.sidebarPanes.styles.show(matchedStylePanesWrapper.element);this.sidebarPanes.computedStyle.show(computedStylePanesWrapper.element);showMetrics.call(this,vertically);this.sidebarPanes.platformFonts.show(computedStylePanesWrapper.element);this.sidebarPaneView.addPane(this.sidebarPanes.eventListeners);this.sidebarPaneView.addPane(this.sidebarPanes.domBreakpoints);this.sidebarPaneView.addPane(this.sidebarPanes.properties);for(var sidebarViewWrapper of this._elementsSidebarViewWrappers)
this.sidebarPaneView.addPane(sidebarViewWrapper);this._extensionSidebarPanesContainer=this.sidebarPaneView;for(var i=0;i<extensionSidebarPanes.length;++i)
this._addExtensionSidebarPane(extensionSidebarPanes[i]);this._splitWidget.setSidebarWidget(this.sidebarPaneView);this.sidebarPanes.styles.expand();if(selectedTabId)
this.sidebarPaneView.selectTab(selectedTabId);},_extensionSidebarPaneAdded:function(event)
{var pane=(event.data);this._addExtensionSidebarPane(pane);},_addExtensionSidebarPane:function(pane)
{if(pane.panelName()===this.name)
this._extensionSidebarPanesContainer.addPane(pane);},setWidgetBelowDOM:function(widget)
{if(widget){this._treeOutlineSplit.setSidebarWidget(widget);this._treeOutlineSplit.showBoth(true);}else{this._treeOutlineSplit.hideSidebar(true);}},__proto__:WebInspector.Panel.prototype}
WebInspector.ElementsPanel.ContextMenuProvider=function()
{}
WebInspector.ElementsPanel.ContextMenuProvider.prototype={appendApplicableItems:function(event,contextMenu,target)
{WebInspector.ElementsPanel.instance().appendApplicableItems(event,contextMenu,target);}}
WebInspector.ElementsPanel.DOMNodeRevealer=function()
{}
WebInspector.ElementsPanel.DOMNodeRevealer.prototype={reveal:function(node)
{var panel=WebInspector.ElementsPanel.instance();panel._pendingNodeReveal=true;return new Promise(revealPromise);function revealPromise(resolve,reject)
{if(node instanceof WebInspector.DOMNode){onNodeResolved((node));}else if(node instanceof WebInspector.DeferredDOMNode){((node)).resolve(onNodeResolved);}else if(node instanceof WebInspector.RemoteObject){var domModel=WebInspector.DOMModel.fromTarget((node).target());if(domModel)
domModel.pushObjectAsNodeToFrontend(node,onNodeResolved);else
reject(new Error("Could not resolve a node to reveal."));}else{reject(new Error("Can't reveal a non-node."));panel._pendingNodeReveal=false;}
function onNodeResolved(resolvedNode)
{panel._pendingNodeReveal=false;if(resolvedNode){panel.revealAndSelectNode(resolvedNode);resolve(undefined);return;}
reject(new Error("Could not resolve node to reveal."));}}}}
WebInspector.ElementsPanel.show=function()
{WebInspector.inspectorView.setCurrentPanel(WebInspector.ElementsPanel.instance());}
WebInspector.ElementsPanel.instance=function()
{if(!WebInspector.ElementsPanel._instanceObject)
WebInspector.ElementsPanel._instanceObject=new WebInspector.ElementsPanel();return WebInspector.ElementsPanel._instanceObject;}
WebInspector.ElementsPanelFactory=function()
{}
WebInspector.ElementsPanelFactory.prototype={createPanel:function()
{return WebInspector.ElementsPanel.instance();}}
WebInspector.ElementsActionDelegate=function(){}
WebInspector.ElementsActionDelegate.prototype={handleAction:function(context,actionId)
{switch(actionId){case"elements.hide-element":WebInspector.ElementsPanel.instance()._toggleHideElement();return true;case"elements.edit-as-html":WebInspector.ElementsPanel.instance()._toggleEditAsHTML();return true;}
return false;}}
WebInspector.ElementsPanel.PseudoStateMarkerDecorator=function()
{}
WebInspector.ElementsPanel.PseudoStateMarkerDecorator.prototype={decorate:function(node)
{return{color:"orange",title:WebInspector.UIString("Element state: %s",":"+WebInspector.CSSStyleModel.fromNode(node).pseudoState(node).join(", :"))};}}
WebInspector.ElementsPanel.BaseToolbarPaneWidget=function(toolbarItem)
{WebInspector.Widget.call(this);this._toolbarItem=toolbarItem;WebInspector.context.addFlavorChangeListener(WebInspector.DOMNode,this._nodeChanged,this);}
WebInspector.ElementsPanel.BaseToolbarPaneWidget.prototype={_nodeChanged:function()
{if(!this.isShowing())
return;var elementNode=WebInspector.SharedSidebarModel.elementNode(WebInspector.context.flavor(WebInspector.DOMNode));this.onNodeChanged(elementNode);},onNodeChanged:function(newNode)
{},willHide:function()
{this._toolbarItem.setToggled(false);},wasShown:function()
{this._toolbarItem.setToggled(true);this._nodeChanged();},__proto__:WebInspector.Widget.prototype};WebInspector.ElementStatePaneWidget=function(toolbarItem)
{WebInspector.ElementsPanel.BaseToolbarPaneWidget.call(this,toolbarItem);this.element.className="styles-element-state-pane";this.element.createChild("div").createTextChild(WebInspector.UIString("Force element state"));var table=createElementWithClass("table","source-code");var inputs=[];this._inputs=inputs;function clickListener(event)
{var node=WebInspector.context.flavor(WebInspector.DOMNode);if(!node)
return;WebInspector.CSSStyleModel.fromNode(node).forcePseudoState(node,event.target.state,event.target.checked);}
function createCheckbox(state)
{var td=createElement("td");var label=createCheckboxLabel(":"+state);var input=label.checkboxElement;input.state=state;input.addEventListener("click",clickListener,false);inputs.push(input);td.appendChild(label);return td;}
var tr=table.createChild("tr");tr.appendChild(createCheckbox.call(null,"active"));tr.appendChild(createCheckbox.call(null,"hover"));tr=table.createChild("tr");tr.appendChild(createCheckbox.call(null,"focus"));tr.appendChild(createCheckbox.call(null,"visited"));this.element.appendChild(table);}
WebInspector.ElementStatePaneWidget.prototype={_updateTarget:function(target)
{if(this._target===target)
return;if(this._target){var cssModel=WebInspector.CSSStyleModel.fromTarget(this._target);cssModel.removeEventListener(WebInspector.CSSStyleModel.Events.PseudoStateForced,this._pseudoStateForced,this)}
this._target=target;if(target){var cssModel=WebInspector.CSSStyleModel.fromTarget(target);cssModel.addEventListener(WebInspector.CSSStyleModel.Events.PseudoStateForced,this._pseudoStateForced,this)}},_pseudoStateForced:function(event)
{var node=(event.data.node);if(node===WebInspector.context.flavor(WebInspector.DOMNode))
this._updateInputs(node);},onNodeChanged:function(newNode)
{this._updateTarget(newNode?newNode.target():null);if(newNode)
this._updateInputs(newNode);},_updateInputs:function(node)
{var nodePseudoState=WebInspector.CSSStyleModel.fromNode(node).pseudoState(node);var inputs=this._inputs;for(var i=0;i<inputs.length;++i){inputs[i].disabled=!!node.pseudoType();inputs[i].checked=nodePseudoState.indexOf(inputs[i].state)>=0;}},__proto__:WebInspector.ElementsPanel.BaseToolbarPaneWidget.prototype}
WebInspector.ElementStatePaneWidget.ButtonProvider=function()
{this._button=new WebInspector.ToolbarButton(WebInspector.UIString("Toggle Element State"),"pin-toolbar-item");this._button.addEventListener("click",this._clicked,this);this._view=new WebInspector.ElementStatePaneWidget(this.item());WebInspector.context.addFlavorChangeListener(WebInspector.DOMNode,this._nodeChanged,this);this._nodeChanged();}
WebInspector.ElementStatePaneWidget.ButtonProvider.prototype={_clicked:function()
{WebInspector.ElementsPanel.instance().showToolbarPane(!this._view.isShowing()?this._view:null);},item:function()
{return this._button;},_nodeChanged:function()
{var enabled=!!WebInspector.context.flavor(WebInspector.DOMNode);this._button.setEnabled(enabled);if(!enabled&&this._button.toggled())
WebInspector.ElementsPanel.instance().showToolbarPane(null);}};WebInspector.ElementsTreeElementHighlighter=function(treeOutline)
{this._throttler=new WebInspector.Throttler(100);this._treeOutline=treeOutline;this._treeOutline.addEventListener(TreeOutline.Events.ElementExpanded,this._clearState,this);this._treeOutline.addEventListener(TreeOutline.Events.ElementCollapsed,this._clearState,this);this._treeOutline.addEventListener(WebInspector.ElementsTreeOutline.Events.SelectedNodeChanged,this._clearState,this);WebInspector.targetManager.addModelListener(WebInspector.DOMModel,WebInspector.DOMModel.Events.NodeHighlightedInOverlay,this._highlightNode,this);this._treeOutline.domModel().addEventListener(WebInspector.DOMModel.Events.InspectModeWillBeToggled,this._clearState,this);}
WebInspector.ElementsTreeElementHighlighter.prototype={_highlightNode:function(event)
{var domNode=(event.data);this._throttler.schedule(callback.bind(this));this._pendingHighlightNode=this._treeOutline.domModel()===domNode.domModel()?domNode:null;function callback()
{this._highlightNodeInternal(this._pendingHighlightNode);delete this._pendingHighlightNode;return Promise.resolve();}},_highlightNodeInternal:function(node)
{this._isModifyingTreeOutline=true;var treeElement=null;if(this._currentHighlightedElement){var currentTreeElement=this._currentHighlightedElement;while(currentTreeElement!==this._alreadyExpandedParentElement){if(currentTreeElement.expanded)
currentTreeElement.collapse();currentTreeElement=currentTreeElement.parent;}}
delete this._currentHighlightedElement;delete this._alreadyExpandedParentElement;if(node){var deepestExpandedParent=node;var treeElementSymbol=this._treeOutline.treeElementSymbol();while(deepestExpandedParent&&(!deepestExpandedParent[treeElementSymbol]||!deepestExpandedParent[treeElementSymbol].expanded))
deepestExpandedParent=deepestExpandedParent.parentNode;this._alreadyExpandedParentElement=deepestExpandedParent?deepestExpandedParent[treeElementSymbol]:this._treeOutline.rootElement();treeElement=this._treeOutline.createTreeElementFor(node);}
this._currentHighlightedElement=treeElement;this._treeOutline.setHoverEffect(treeElement);if(treeElement)
treeElement.reveal();this._isModifyingTreeOutline=false;},_clearState:function()
{if(this._isModifyingTreeOutline)
return;delete this._currentHighlightedElement;delete this._alreadyExpandedParentElement;delete this._pendingHighlightNode;}};Runtime.cachedResources["elements/bezierEditor.css"]="/*\n * Copyright (c) 2015 The Chromium Authors. All rights reserved.\n * Use of this source code is governed by a BSD-style license that can be\n * found in the LICENSE file.\n */\n\n:host {\n    width: 270px;\n    height: 350px;\n    -webkit-user-select: none;\n    padding: 16px;\n    overflow: hidden;\n}\n\n.bezier-preset-selected > svg {\n    background-color: rgb(56, 121, 217);\n}\n\n.bezier-preset-label {\n    font-size: 10px;\n}\n\n.bezier-preset {\n    width: 50px;\n    height: 50px;\n    padding: 5px;\n    margin: auto;\n    background-color: #f5f5f5;\n    border-radius: 3px;\n}\n\n.bezier-preset line.bezier-control-line {\n    stroke: #666;\n    stroke-width: 1;\n    stroke-linecap: round;\n    fill: none;\n}\n\n.bezier-preset circle.bezier-control-circle {\n    fill: #666;\n}\n\n.bezier-preset path.bezier-path {\n    stroke: black;\n    stroke-width: 2;\n    stroke-linecap: round;\n    fill: none;\n}\n\n.bezier-preset-selected path.bezier-path, .bezier-preset-selected line.bezier-control-line {\n    stroke: white;\n}\n\n.bezier-preset-selected circle.bezier-control-circle {\n    fill: white;\n}\n\n.bezier-curve line.linear-line {\n    stroke: #eee;\n    stroke-width: 2;\n    stroke-linecap: round;\n    fill: none;\n}\n\n.bezier-curve line.bezier-control-line {\n    stroke: #9C27B0;\n    stroke-width: 2;\n    stroke-linecap: round;\n    fill: none;\n    opacity: 0.6;\n}\n\n.bezier-curve circle.bezier-control-circle {\n    fill: #9C27B0;\n    cursor: pointer;\n}\n\n.bezier-curve path.bezier-path {\n    stroke: black;\n    stroke-width: 3;\n    stroke-linecap: round;\n    fill: none;\n}\n\n.bezier-preview-container {\n    position: relative;\n    background-color: white;\n    overflow: hidden;\n    border-radius: 20px;\n    width: 200%;\n    height: 20px;\n    z-index: 2;\n    flex-shrink: 0;\n    opacity: 0;\n}\n\n.bezier-preview-animation {\n    background-color: #9C27B0;\n    width: 20px;\n    height: 20px;\n    border-radius: 20px;\n    position: absolute;\n}\n\n.bezier-preview-onion {\n    margin-top: -20px;\n    position: relative;\n    z-index: 1;\n}\n\n.bezier-preview-onion > .bezier-preview-animation {\n    opacity: 0.1;\n}\n\nsvg.bezier-preset-modify {\n    background-color: #f5f5f5;\n    border-radius: 35px;\n    display: inline-block;\n    visibility: hidden;\n    transition: transform 100ms cubic-bezier(0.4, 0, 0.2, 1);\n    cursor: pointer;\n    position: absolute;\n}\n\nsvg.bezier-preset-modify:hover, .bezier-preset:hover {\n    background-color: #999;\n}\n\n.bezier-preset-selected .bezier-preset:hover {\n    background-color: rgb(56, 121, 217);\n}\n\n.bezier-preset-modify path {\n    stroke-width: 2;\n    stroke: black;\n    fill: none;\n}\n\n.bezier-preset-selected .bezier-preset-modify {\n    opacity: 1;\n}\n\n.bezier-preset-category {\n    width: 50px;\n    margin: 20px 0;\n    cursor: pointer;\n    transition: transform 100ms cubic-bezier(0.4, 0, 0.2, 1);\n}\n\nspan.bezier-display-value {\n    width: 100%;\n    -webkit-user-select: text;\n    display: block;\n    text-align: center;\n    line-height: 20px;\n    height: 20px;\n    cursor: text;\n    white-space: nowrap !important;\n}\n\n.bezier-container {\n    display: flex;\n    margin-top: 38px;\n}\n\nsvg.bezier-curve {\n    margin-left: 32px;\n    margin-top: -8px;\n}\n\nsvg.bezier-preset-modify.bezier-preset-plus {\n    right: 0;\n}\n\n.bezier-header {\n    margin-top: 16px;\n}\n\nsvg.bezier-preset-modify:active {\n    transform: scale(1.1);\n    background-color: rgb(56, 121, 217);\n}\n\n.bezier-preset-category:active {\n    transform: scale(1.05);\n}\n\n.bezier-header-active > svg.bezier-preset-modify {\n    visibility: visible;\n}\n\n.bezier-preset-modify:active path {\n    stroke: white;\n}\n\n/*# sourceURL=elements/bezierEditor.css */";Runtime.cachedResources["elements/breadcrumbs.css"]="/*\n * Copyright 2014 The Chromium Authors. All rights reserved.\n * Use of this source code is governed by a BSD-style license that can be\n * found in the LICENSE file.\n */\n\n.crumbs {\n    display: inline-block;\n    pointer-events: auto;\n    cursor: default;\n    font-size: 11px;\n    line-height: 17px;\n    white-space: nowrap;\n}\n\n.crumbs .crumb {\n    display: inline-block;\n    padding: 0 7px;\n    height: 18px;\n    white-space: nowrap;\n}\n\n.crumbs .crumb.collapsed > * {\n    display: none;\n}\n\n.crumbs .crumb.collapsed::before {\n    content: \"\\2026\";\n    font-weight: bold;\n}\n\n.crumbs .crumb.compact .extra {\n    display: none;\n}\n\n.crumbs .crumb.selected, .crumbs .crumb.selected:hover {\n    background-color: rgb(56, 121, 217);\n    color: white;\n    text-shadow: rgba(255, 255, 255, 0.5) 0 0 0;\n}\n\n.crumbs .crumb:hover {\n    background-color: rgb(216, 216, 216);\n}\n\n/*# sourceURL=elements/breadcrumbs.css */";Runtime.cachedResources["elements/computedStyleSidebarPane.css"]="/*\n * Copyright (c) 2015 The Chromium Authors. All rights reserved.\n * Use of this source code is governed by a BSD-style license that can be\n * found in the LICENSE file.\n */\n\n.computed-properties {\n    -webkit-user-select: text;\n}\n\n.computed-style-property {\n    display: flex;\n    flex-wrap: wrap;\n}\n\n.computed-style-property .property-name {\n    width: 16em;\n    text-overflow: ellipsis;\n    overflow: hidden;\n}\n\n.computed-style-property .property-value {\n    min-width: 5em;\n}\n\n.computed-style-property-inherited {\n    opacity: 0.5;\n}\n\n.trace-link {\n    float: right;\n    padding-left: 1em;\n    position: relative;\n    z-index: 1;\n}\n\n.trace-link a::before {\n    content: attr(data-uncopyable);\n    text-decoration: underline;\n}\n\n.property-trace {\n    text-overflow: ellipsis;\n    overflow: hidden;\n    flex-grow: 1;\n}\n\n.property-trace-selector {\n    color: gray;\n    padding-left: 2em;\n}\n\n.property-trace-value {\n    position: relative;\n    display: inline-block;\n}\n\n.property-trace-inactive .property-trace-value::before {\n    position: absolute;\n    content: \".\";\n    border-bottom: 1px solid rgba(0, 0, 0, 0.35);\n    top: 0;\n    bottom: 5px;\n    left: 0;\n    right: 0;\n}\n\n.tree-outline li.odd-row {\n    position: relative;\n    background-color: #F5F5F5;\n}\n\n.tree-outline, .tree-outline ol {\n    padding-left: 0;\n}\n\n.tree-outline li:hover {\n    background-color: rgb(235, 242, 252);\n    cursor: pointer;\n}\n\n/* FIXME: Patch TreeElement styles to use flex for title layout. */\n\n.tree-outline li {\n    display: flex;\n    align-items: baseline;\n}\n\n.tree-outline li::before {\n    float: none;\n    flex: none;\n    -webkit-mask-position: -3px -97px;\n    background-color: rgb(110, 110, 110);\n    margin-left: 4px;\n}\n\n.tree-outline li.parent.expanded::before {\n    -webkit-mask-position: -19px -97px;\n}\n\n/*# sourceURL=elements/computedStyleSidebarPane.css */";Runtime.cachedResources["elements/elementsPanel.css"]="/*\n * Copyright (C) 2006, 2007, 2008 Apple Inc.  All rights reserved.\n * Copyright (C) 2009 Anthony Ricaud <rik@webkit.org>\n *\n * Redistribution and use in source and binary forms, with or without\n * modification, are permitted provided that the following conditions\n * are met:\n *\n * 1.  Redistributions of source code must retain the above copyright\n *     notice, this list of conditions and the following disclaimer.\n * 2.  Redistributions in binary form must reproduce the above copyright\n *     notice, this list of conditions and the following disclaimer in the\n *     documentation and/or other materials provided with the distribution.\n * 3.  Neither the name of Apple Computer, Inc. (\"Apple\") nor the names of\n *     its contributors may be used to endorse or promote products derived\n *     from this software without specific prior written permission.\n *\n * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS \"AS IS\" AND ANY\n * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED\n * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE\n * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY\n * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES\n * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;\n * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND\n * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT\n * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF\n * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n */\n\n#elements-content {\n    flex: 1 1;\n    overflow: auto;\n    padding: 2px 0 0 0;\n    transform: translateZ(0);\n}\n\n#elements-content:not(.elements-wrap) > div {\n    display: inline-block;\n    min-width: 100%;\n}\n\n#elements-content.elements-wrap {\n    overflow-x: hidden;\n}\n\n.elements-topbar {\n    border-bottom: 1px solid hsla(0, 0%, 0%, 0.1);\n    flex-shrink: 0;\n}\n\n#elements-crumbs {\n    flex: 0 0 19px;\n    background-color: white;\n    border-top: 1px solid #ccc;\n    overflow: hidden;\n    height: 19px;\n    width: 100%;\n}\n\n.metrics {\n    padding: 8px;\n    font-size: 10px;\n    text-align: center;\n    white-space: nowrap;\n}\n\n.metrics .label {\n    position: absolute;\n    font-size: 10px;\n    margin-left: 3px;\n    padding-left: 2px;\n    padding-right: 2px;\n}\n\n.metrics .position {\n    border: 1px rgb(66%, 66%, 66%) dotted;\n    background-color: white;\n    display: inline-block;\n    text-align: center;\n    padding: 3px;\n    margin: 3px;\n}\n\n.metrics .margin {\n    border: 1px dashed;\n    background-color: white;\n    display: inline-block;\n    text-align: center;\n    vertical-align: middle;\n    padding: 3px;\n    margin: 3px;\n}\n\n.metrics .border {\n    border: 1px black solid;\n    background-color: white;\n    display: inline-block;\n    text-align: center;\n    vertical-align: middle;\n    padding: 3px;\n    margin: 3px;\n}\n\n.metrics .padding {\n    border: 1px grey dashed;\n    background-color: white;\n    display: inline-block;\n    text-align: center;\n    vertical-align: middle;\n    padding: 3px;\n    margin: 3px;\n}\n\n.metrics .content {\n    position: static;\n    border: 1px gray solid;\n    background-color: white;\n    display: inline-block;\n    text-align: center;\n    vertical-align: middle;\n    padding: 3px;\n    margin: 3px;\n    min-width: 80px;\n    overflow: visible;\n}\n\n.metrics .content span {\n    display: inline-block;\n}\n\n.metrics .editing {\n    position: relative;\n    z-index: 100;\n    cursor: text;\n}\n\n.metrics .left {\n    display: inline-block;\n    vertical-align: middle;\n}\n\n.metrics .right {\n    display: inline-block;\n    vertical-align: middle;\n}\n\n.metrics .top {\n    display: inline-block;\n}\n\n.metrics .bottom {\n    display: inline-block;\n}\n\n.styles-section {\n    padding: 2px 2px 4px 4px;\n    min-height: 18px;\n    white-space: nowrap;\n    background-origin: padding;\n    background-clip: padding;\n    -webkit-user-select: text;\n    border-bottom: 1px solid #eee;\n    position: relative;\n    overflow: hidden;\n}\n\n.styles-section:last-child {\n    border-bottom: none;\n}\n\n.styles-pane .sidebar-separator {\n    border-top: 0 none;\n}\n\n.styles-section.read-only {\n    background-color: #eee;\n}\n\n.styles-section .style-properties li.not-parsed-ok {\n    margin-left: 0;\n}\n\n.styles-section .style-properties li.filter-match,\n.styles-section .simple-selector.filter-match {\n    background-color: rgba(255, 255, 0, 0.5);\n}\n\n.styles-section .style-properties li.overloaded.filter-match {\n    background-color: rgba(255, 255, 0, 0.25);\n}\n\n.styles-section .style-properties li .css-update-highlight {\n    border-radius: 2px;\n}\n\n.styles-section .style-properties li .css-update-highlight.first-part {\n    color: white !important;\n}\n\n.styles-section .style-properties li.not-parsed-ok .exclamation-mark {\n    display: inline-block;\n    position: relative;\n    width: 11px;\n    height: 10px;\n    margin: 0 7px 0 0;\n    top: 1px;\n    left: -36px; /* outdent to compensate for the top-level property indent */\n    -webkit-user-select: none;\n    cursor: default;\n    z-index: 1;\n}\n\n.styles-section .sidebar-pane-closing-brace {\n    clear: both;\n}\n\n.styles-section-title {\n    background-origin: padding;\n    background-clip: padding;\n    word-wrap: break-word;\n    white-space: normal;\n}\n\n.styles-section-title .media-list {\n    color: #888;\n}\n\n.styles-section-title .media-list.media-matches .media.editable-media {\n    color: #222;\n}\n\n.styles-section-title .media:not(.editing-media),\n.styles-section-title .media:not(.editing-media) .subtitle {\n    overflow: hidden;\n}\n\n.styles-section-title .media .subtitle {\n    float: right;\n    color: rgb(85, 85, 85);\n}\n\n.styles-section-subtitle {\n    color: rgb(85, 85, 85);\n    float: right;\n    margin-left: 5px;\n    max-width: 100%;\n    text-overflow: ellipsis;\n    overflow: hidden;\n    white-space: nowrap;\n}\n\n.styles-section .styles-section-subtitle a {\n    color: inherit;\n}\n\n.styles-section .selector {\n    color: #888;\n}\n\n.styles-section .simple-selector.selector-matches {\n    color: #222;\n}\n\n.styles-section a[data-uncopyable] {\n    display: inline-block;\n}\n\n.styles-section a[data-uncopyable]::before {\n    content: attr(data-uncopyable);\n    text-decoration: underline;\n}\n\n.styles-section .style-properties {\n    margin: 0;\n    padding: 2px 4px 0 0;\n    list-style: none;\n    clear: both;\n}\n\n.styles-section.matched-styles .style-properties {\n    padding-left: 0;\n}\n\n.no-affect .style-properties li {\n    opacity: 0.5;\n}\n\n.no-affect .style-properties li.editing {\n    opacity: 1.0;\n}\n\n.styles-section .style-properties li {\n    margin-left: 12px;\n    padding-left: 22px;\n    white-space: normal;\n    text-overflow: ellipsis;\n    overflow: hidden;\n    cursor: auto;\n}\n\n.styles-section .style-properties li .webkit-css-property {\n    margin-left: -22px; /* outdent the first line of longhand properties (in an expanded shorthand) to compensate for the \"padding-left\" shift in .styles-section .style-properties li */\n}\n\n.styles-section .style-properties > li {\n    padding-left: 38px;\n    clear: both;\n}\n\n.styles-section .style-properties > li .webkit-css-property {\n    margin-left: -38px; /* outdent the first line of the top-level properties to compensate for the \"padding-left\" shift in .styles-section .style-properties > li */\n}\n\n.styles-section .style-properties > li.child-editing {\n    padding-left: 8px;\n}\n\n.styles-section .style-properties > li.child-editing .webkit-css-property {\n    margin-left: 0;\n}\n\n.styles-section.matched-styles .style-properties li {\n    margin-left: 0 !important;\n}\n\n.styles-section .style-properties li.child-editing {\n    word-wrap: break-word !important;\n    white-space: normal !important;\n    padding-left: 0;\n}\n\n.styles-section .style-properties ol {\n    display: none;\n    margin: 0;\n    -webkit-padding-start: 12px;\n    list-style: none;\n}\n\n.styles-section .style-properties ol.expanded {\n    display: block;\n}\n\n.styles-section.matched-styles .style-properties li.parent .expand-element {\n    -webkit-user-select: none;\n    background-image: url(Images/toolbarButtonGlyphs.png);\n    background-size: 352px 168px;\n    margin-right: 2px;\n    margin-left: -6px;\n    opacity: 0.55;\n    width: 8px;\n    height: 10px;\n    display: inline-block;\n}\n\n@media (-webkit-min-device-pixel-ratio: 1.5) {\n.styles-section.matched-styles .style-properties li.parent .expand-element {\n    background-image: url(Images/toolbarButtonGlyphs_2x.png);\n}\n} /* media */\n\n.styles-section.matched-styles .style-properties li.parent .expand-element {\n    background-position: -4px -96px;\n}\n\n.styles-section.matched-styles .style-properties li.parent.expanded .expand-element {\n    background-position: -20px -96px;\n}\n\n.styles-section .style-properties li .info {\n    padding-top: 4px;\n    padding-bottom: 3px;\n}\n\n.styles-section.matched-styles:not(.read-only):hover .style-properties .enabled-button {\n    visibility: visible;\n}\n\n.styles-section.matched-styles:not(.read-only) .style-properties li.disabled .enabled-button {\n    visibility: visible;\n}\n\n.styles-section .style-properties .enabled-button {\n    visibility: hidden;\n    float: left;\n    font-size: 10px;\n    margin: 0;\n    vertical-align: top;\n    position: relative;\n    z-index: 1;\n    width: 18px;\n    left: -40px; /* original -2px + (-38px) to compensate for the first line outdent */\n    top: 1px;\n    height: 13px;\n}\n\n.styles-section.matched-styles .style-properties ol.expanded {\n    margin-left: 16px;\n}\n\n.styles-section .style-properties .overloaded:not(.has-ignorable-error),\n.styles-section .style-properties .inactive,\n.styles-section .style-properties .disabled,\n.styles-section .style-properties .not-parsed-ok:not(.has-ignorable-error) {\n    text-decoration: line-through;\n}\n\n.styles-section .style-properties .has-ignorable-error .webkit-css-property {\n    color: inherit;\n}\n\n.styles-section .style-properties .implicit,\n.styles-section .style-properties .inherited {\n    opacity: 0.5;\n}\n\n.styles-section .style-properties .has-ignorable-error {\n    color: gray;\n}\n\n.styles-element-state-pane {\n    overflow: hidden;\n    height: 60px;\n    padding-left: 2px;\n    border-bottom: 1px solid rgb(189, 189, 189);\n}\n\n@keyframes styles-element-state-pane-slidein {\n    from {\n        margin-top: -60px;\n    }\n    to {\n        margin-top: 0px;\n    }\n}\n\n@keyframes styles-element-state-pane-slideout {\n    from {\n        margin-top: 0px;\n    }\n    to {\n        margin-top: -60px;\n    }\n}\n\n.styles-sidebar-toolbar-pane {\n    position: relative;\n    animation-duration: 0.1s;\n    animation-direction: normal;\n}\n\n.styles-sidebar-toolbar-pane-container {\n    position: relative;\n    max-height: 60px;\n    overflow: hidden;\n}\n\n.styles-element-state-pane {\n    background-color: #f3f3f3;\n    border-bottom: 1px solid rgb(189, 189, 189);\n    margin-top: 0;\n}\n\n\n.styles-element-state-pane > div {\n    margin: 6px 4px 2px;\n}\n\n.styles-element-state-pane > table {\n    width: 100%;\n    border-spacing: 0;\n}\n\n.styles-element-state-pane td {\n    padding: 0;\n}\n\n.styles-animations-controls-pane > * {\n    margin: 6px 4px;\n}\n\n.styles-animations-controls-pane {\n    border-bottom: 1px solid rgb(189, 189, 189);\n    height: 60px;\n    overflow: hidden;\n    background-color: #eee;\n}\n\n.animations-controls {\n    width: 100%;\n    max-width: 200px;\n    display: flex;\n    align-items: center;\n}\n\n.animations-controls > .toolbar {\n    display: inline-block;\n}\n\n.animations-controls > input {\n    flex-grow: 1;\n    margin-right: 10px;\n}\n\n.animations-controls > .playback-label {\n    width: 35px;\n}\n\n.styles-selector {\n    cursor: text;\n}\n\n.image-preview-container {\n    background: transparent;\n    text-align: center;\n}\n\n.image-preview-container img {\n    margin: 2px auto;\n    max-width: 100px;\n    max-height: 100px;\n    background-image: url(Images/checker.png);\n    -webkit-user-select: text;\n    -webkit-user-drag: auto;\n}\n\n.sidebar-pane.composite {\n    position: absolute;\n}\n\n.metrics {\n    border-bottom: 1px solid #ccc;\n}\n\n.metrics-and-styles .metrics {\n    border-top: 1px solid #ccc;\n    border-bottom: none;\n}\n\n.sidebar-pane.composite .metrics {\n    height: 190px;\n    display: flex;\n    flex-direction: column;\n    -webkit-align-items: center;\n    -webkit-justify-content: center;\n}\n\n.sidebar-pane .metrics-and-styles,\n.sidebar-pane .metrics-and-computed {\n    display: flex !important;\n    flex-direction: column !important;\n    position: relative;\n}\n\n.sidebar-pane .style-panes-wrapper {\n    transform: translateZ(0);\n    flex: 1;\n    overflow-y: auto;\n    position: relative;\n}\n\n.styles-sidebar-pane-toolbar {\n    border-bottom: 1px solid #eee;\n}\n\n.styles-sidebar-pane-filter-box {\n    flex: auto;\n    display: flex;\n}\n\n.styles-sidebar-pane-filter-box > input {\n    outline: none !important;\n    border: none;\n    width: 100%;\n    background: transparent;\n    margin-left: 4px;\n}\n\n.styles-filter-engaged {\n    background-color: rgba(255, 255, 0, 0.5);\n}\n\n.sidebar-pane.composite .platform-fonts {\n    -webkit-user-select: text;\n}\n\n.sidebar-pane.composite .platform-fonts .sidebar-separator {\n    height: 24px;\n    background-color: #f1f1f1;\n    border-color: #ddd;\n    display: flex;\n    align-items: center;\n}\n\n.sidebar-pane.composite .platform-fonts .stats-section {\n    margin: 5px 0;\n}\n\n.sidebar-pane.composite .platform-fonts .font-stats-item {\n    padding-left: 1em;\n}\n\n.sidebar-pane.composite .platform-fonts .font-stats-item .delimeter {\n    margin: 0 1ex 0 1ex;\n}\n\n.sidebar-pane.composite .metrics-and-styles .metrics {\n    border-bottom: none;\n}\n\n.sidebar-pane > .split-widget {\n    position: absolute;\n    top: 0;\n    bottom: 0;\n    left: 0;\n    right: 0;\n}\n\n.styles-section:not(.read-only) .style-properties .webkit-css-property.styles-panel-hovered,\n.styles-section:not(.read-only) .style-properties .value .styles-panel-hovered,\n.styles-section:not(.read-only) .style-properties .value.styles-panel-hovered,\n.styles-section:not(.read-only) span.simple-selector.styles-panel-hovered {\n    text-decoration: underline;\n    cursor: default;\n}\n\n.styles-clipboard-only {\n    display: inline-block;\n    width: 0;\n    opacity: 0;\n    pointer-events: none;\n    white-space: pre;\n}\n\n.popover-icon {\n    margin-left: 1px;\n    margin-right: 2px;\n    width: 10px;\n    height: 10px;\n    position: relative;\n    top: 1px;\n    display: inline-block;\n    line-height: 1px;\n    -webkit-user-select: none;\n}\n\n.bezier-icon {\n    background-color: #9C27B0;\n    border-radius: 2px;\n}\n\n.bezier-icon path {\n    stroke: white;\n    stroke-width: 1.5;\n    stroke-linecap: square;\n    fill: none;\n}\n\n.swatch {\n    background-image: url(Images/checker.png);\n}\n\nli.child-editing .styles-clipboard-only {\n    display: none;\n}\n\nli.editing .swatch,\nli.editing .enabled-button {\n    display: none !important;\n}\n\n.sidebar-separator {\n    background-color: #ddd;\n    padding: 0 5px;\n    border-top: 1px solid #ccc;\n    border-bottom: 1px solid #ccc;\n    color: rgb(50, 50, 50);\n    white-space: nowrap;\n    text-overflow: ellipsis;\n    overflow: hidden;\n    line-height: 16px;\n}\n\n.sidebar-separator > span.monospace {\n    background: rgb(255, 255, 255);\n    padding: 0px 3px;\n    border-radius: 2px;\n    border: 1px solid #C1C1C1;\n}\n\n.swatch-inner {\n    width: 100%;\n    height: 100%;\n    display: inline-block;\n    border: 1px solid rgba(128, 128, 128, 0.6);\n}\n\n.swatch-inner:hover {\n    border: 1px solid rgba(64, 64, 64, 0.8);\n}\n\n.animation-section-body {\n    display: none;\n}\n\n.animation-section-body.expanded {\n    display: block;\n}\n\n.animation-section-body .section {\n    border-bottom: 1px solid rgb(191, 191, 191);\n}\n\n.animationsHeader {\n    padding-top: 23px;\n}\n\n.global-animations-toolbar {\n    position: absolute;\n    top: 0;\n    width: 100%;\n    background-color: #eee;\n    border-bottom: 1px solid rgb(163, 163, 163);\n    padding-left: 10px;\n}\n\n.events-pane .section:not(:first-of-type) {\n    border-top: 1px solid rgb(231, 231, 231);\n}\n\n.events-pane .section {\n    margin: 0;\n}\n\n.style-properties li.editing {\n    margin-left: 10px;\n    text-overflow: clip;\n}\n\n.style-properties li.editing-sub-part {\n    padding: 3px 6px 8px 18px;\n    margin: -1px -6px -8px -6px;\n    text-overflow: clip;\n}\n\n.properties-widget-section {\n    padding: 2px 0px 2px 5px;\n}\n\n.sidebar-pane-section-toolbar {\n    position: absolute;\n    right: 0;\n    bottom: 0;\n    visibility: hidden;\n    background-color: rgba(255, 255, 255, 0.9);\n}\n\n.styles-pane:not(.is-editing-style) .styles-section.matched-styles:not(.read-only):hover .sidebar-pane-section-toolbar {\n    visibility: visible;\n}\n\n/*# sourceURL=elements/elementsPanel.css */";Runtime.cachedResources["elements/elementsTreeOutline.css"]="/*\n * Copyright (c) 2014 The Chromium Authors. All rights reserved.\n * Use of this source code is governed by a BSD-style license that can be\n * found in the LICENSE file.\n */\n\n.elements-disclosure {\n    width: 100%;\n    display: inline-block;\n    line-height: normal;\n}\n\n.elements-disclosure li {\n    /** Keep margin-left & padding-left in sync with ElementsTreeElements.updateDecorators **/\n    padding: 0 0 0 14px;\n    margin-top: 1px;\n    margin-left: -2px;\n    word-wrap: break-word;\n    position: relative;\n}\n\n.elements-disclosure li.parent {\n    /** Keep it in sync with ElementsTreeElements.updateDecorators **/\n    margin-left: -13px;\n}\n\n.elements-disclosure li.parent::before {\n    float: left;\n    width: 10px;\n    box-sizing: border-box;\n}\n\n.elements-disclosure li.parent::before {\n    -webkit-user-select: none;\n    -webkit-mask-image: url(Images/toolbarButtonGlyphs.png);\n    -webkit-mask-size: 352px 168px;\n    content: \"a\";\n    color: transparent;\n    text-shadow: none;\n    margin-right: 1px;\n}\n\n.elements-disclosure li.always-parent::before {\n    visibility: hidden;\n}\n\n@media (-webkit-min-device-pixel-ratio: 1.5) {\n.elements-disclosure li.parent::before {\n    -webkit-mask-image: url(Images/toolbarButtonGlyphs_2x.png);\n}\n} /* media */\n\n.elements-disclosure li.parent::before {\n    -webkit-mask-position: -4px -96px;\n    background-color: rgb(110, 110, 110);\n}\n\n.elements-disclosure li .selection {\n    display: none;\n    z-index: -1;\n    margin-left: -10000px;\n}\n\n.elements-disclosure li.hovered:not(.selected) .selection {\n    display: block;\n    left: 3px;\n    right: 3px;\n    background-color: rgba(56, 121, 217, 0.1);\n    border-radius: 5px;\n}\n\n.elements-disclosure li.parent.expanded::before {\n    -webkit-mask-position: -20px -96px;\n}\n\n.elements-disclosure li.selected .selection {\n    display: block;\n    background-color: #dadada;\n}\n\n.elements-disclosure ol {\n    list-style-type: none;\n    /** Keep it in sync with ElementsTreeElements.updateDecorators **/\n    -webkit-padding-start: 12px;\n    margin: 0;\n}\n\n.elements-disclosure ol.children {\n    display: none;\n}\n\n.elements-disclosure ol.children.expanded {\n    display: block;\n}\n\n.elements-disclosure li .webkit-html-tag.close {\n    margin-left: -12px;\n}\n\n.elements-disclosure > ol {\n    position: relative;\n    margin: 0;\n    cursor: default;\n    min-width: 100%;\n    min-height: 100%;\n    -webkit-transform: translateZ(0);\n    padding-left: 2px;\n}\n\n.elements-disclosure ol:focus li.selected {\n    color: white;\n}\n\n.elements-disclosure ol:focus li.parent.selected::before {\n    background-color: white;\n}\n\n.elements-disclosure ol:focus li.selected * {\n    color: inherit;\n}\n\n.elements-disclosure ol:focus li.selected .selection {\n    background-color: rgb(56, 121, 217);\n}\n\n.elements-tree-outline ol.shadow-root {\n    margin-left: 5px;\n    padding-left: 5px;\n    border-left: 1px solid rgb(190, 190, 190);\n}\n\n.elements-tree-outline ol.shadow-root-depth-4 {\n    background-color: rgba(0, 0, 0, 0.04);\n}\n\n.elements-tree-outline ol.shadow-root-depth-3 {\n    background-color: rgba(0, 0, 0, 0.03);\n}\n\n.elements-tree-outline ol.shadow-root-depth-2 {\n    background-color: rgba(0, 0, 0, 0.02);\n}\n\n.elements-tree-outline ol.shadow-root-depth-1 {\n    background-color: rgba(0, 0, 0, 0.01);\n}\n\n.elements-tree-outline ol.shadow-root-deep {\n    background-color: transparent;\n}\n\n.elements-tree-editor {\n    -webkit-user-select: text;\n    -webkit-user-modify: read-write-plaintext-only;\n}\n\n.elements-disclosure li.elements-drag-over .selection {\n    display: block;\n    margin-top: -2px;\n    border-top: 2px solid rgb(56, 121, 217);\n}\n\n.elements-disclosure li.in-clipboard .highlight {\n    outline: 1px dotted darkgrey;\n}\n\n.CodeMirror {\n    /* Consistent with the .editing class in inspector.css */\n    box-shadow: rgba(0, 0, 0, .5) 3px 3px 4px;\n    outline: 1px solid rgb(66%, 66%, 66%) !important;\n    background-color: white;\n}\n\n.CodeMirror-lines {\n    padding: 0;\n}\n\n.CodeMirror pre {\n    padding: 0;\n}\n\nbutton, input, select {\n  font-family: inherit;\n  font-size: inherit;\n}\n\n.editing {\n    -webkit-user-select: text;\n    box-shadow: rgba(0, 0, 0, .5) 3px 3px 4px;\n    outline: 1px solid rgb(66%, 66%, 66%) !important;\n    background-color: white;\n    -webkit-user-modify: read-write-plaintext-only;\n    text-overflow: clip !important;\n    padding-left: 2px;\n    margin-left: -2px;\n    padding-right: 2px;\n    margin-right: -2px;\n    margin-bottom: -1px;\n    padding-bottom: 1px;\n    opacity: 1.0 !important;\n}\n\n.editing,\n.editing * {\n    color: #222 !important;\n    text-decoration: none !important;\n}\n\n.editing br {\n    display: none;\n}\n\n.elements-gutter-decoration {\n    position: absolute;\n    left: 2px;\n    margin-top: 2px;\n    height: 9px;\n    width: 9px;\n    border-radius: 5px;\n    border: 1px solid orange;\n    background-color: orange;\n    cursor: pointer;\n}\n\n.elements-gutter-decoration.elements-has-decorated-children {\n    opacity: 0.5;\n}\n\n.add-attribute {\n    margin-left: 1px;\n    margin-right: 1px;\n    white-space: nowrap;\n}\n\n.elements-tree-element-pick-node-1 {\n    border-radius: 3px;\n    padding: 1px 0 1px 0;\n    -webkit-animation: elements-tree-element-pick-node-animation-1 0.5s 1;\n}\n\n.elements-tree-element-pick-node-2 {\n    border-radius: 3px;\n    padding: 1px 0 1px 0;\n    -webkit-animation: elements-tree-element-pick-node-animation-2 0.5s 1;\n}\n\n@-webkit-keyframes elements-tree-element-pick-node-animation-1 {\n    from { background-color: rgb(255, 210, 126); }\n    to { background-color: inherit; }\n}\n\n@-webkit-keyframes elements-tree-element-pick-node-animation-2 {\n    from { background-color: rgb(255, 210, 126); }\n    to { background-color: inherit; }\n}\n\n.pick-node-mode {\n    cursor: pointer;\n}\n\n.webkit-html-attribute-value a {\n    cursor: default !important;\n}\n\n.elements-tree-nowrap, .elements-tree-nowrap .li {\n    white-space: pre !important;\n}\n\n.elements-disclosure .elements-tree-nowrap li {\n    word-wrap: normal;\n}\n\n/* DOM update highlight */\n@-webkit-keyframes dom-update-highlight-animation {\n    from {\n        background-color: rgb(158, 54, 153);\n        color: white;\n    }\n    80% {\n        background-color: rgb(245, 219, 244);\n        color: inherit;\n    }\n    to {\n        background-color: inherit;\n    }\n}\n\n.dom-update-highlight {\n    -webkit-animation: dom-update-highlight-animation 1.4s 1 cubic-bezier(0, 0, 0.2, 1);\n    border-radius: 2px;\n}\n\n.elements-disclosure.single-node li {\n    padding-left: 2px;\n}\n\n.elements-tree-shortcut-title {\n    color: rgb(87, 87, 87);\n}\n\nol:hover > li > .elements-tree-shortcut-link {\n    display: initial;\n}\n\n.elements-tree-shortcut-link {\n    color: rgb(87, 87, 87);\n    display: none;\n}\n\nol:focus li.selected .webkit-html-tag {\n    color: #a5a5a5;\n}\n\nol:focus li.selected .webkit-html-tag-name,\nol:focus li.selected .webkit-html-close-tag-name,\nol:focus li.selected .webkit-html-attribute-value,\nol:focus li.selected .webkit-html-external-link,\nol:focus li.selected .webkit-html-resource-link {\n    color: white;\n}\n\nol:focus li.selected .webkit-html-attribute-name {\n    color: #ccc;\n}\n\n.elements-disclosure .gutter-container {\n    position: absolute;\n}\n\n.elements-disclosure li.selected .gutter-container:not(.has-decorations) {\n    left: 0px;\n    width: 16.25px;\n    height: 18px;\n    transform: rotate(-90deg) translateX(-13px) scale(0.8);\n    transform-origin: 0% 0%;\n    -webkit-mask-image: url(Images/toolbarButtonGlyphs.png);\n    -webkit-mask-position: -201px -27px;\n    -webkit-mask-size: 352px 168px;\n    background-color: white;\n    cursor: pointer;\n}\n\n@media (-webkit-min-device-pixel-ratio: 1.5) {\n.elements-disclosure li.selected .gutter-container:not(.has-decorations) {\n    -webkit-mask-image: url(Images/toolbarButtonGlyphs_2x.png);\n}\n} /* media */\n\n/*# sourceURL=elements/elementsTreeOutline.css */";Runtime.cachedResources["elements/spectrum.css"]="/* https://github.com/bgrins/spectrum */\n:host {\n    width: 232px;\n    height: 240px;\n    -webkit-user-select: none;\n}\n\n:host(.palettes-enabled) {\n    height: 319px;\n}\n\n.spectrum-color {\n    position: relative;\n    width: 232px;\n    height: 124px;\n    border-radius: 2px 2px 0 0;\n    overflow: hidden;\n}\n\n.spectrum-display-value {\n    -webkit-user-select: text;\n    display: inline-block;\n    padding-left: 2px;\n}\n\n.spectrum-hue {\n    top: 140px;\n}\n\n.spectrum-alpha {\n    top: 159px;\n    background-image: url(Images/checker.png);\n    background-size: 12px 11px;\n}\n\n.spectrum-alpha-background {\n    height: 100%;\n    border-radius: 2px;\n}\n\n.spectrum-hue, .spectrum-alpha {\n    position: absolute;\n    right: 16px;\n    width: 130px;\n    height: 11px;\n    border-radius: 2px;\n}\n\n.spectrum-dragger,\n.spectrum-slider {\n    -webkit-user-select: none;\n}\n\n.spectrum-sat {\n    background-image: linear-gradient(to right, white, rgba(204, 154, 129, 0));\n}\n\n.spectrum-val {\n    background-image: linear-gradient(to top, black, rgba(204, 154, 129, 0));\n}\n\n.spectrum-hue {\n    background: linear-gradient(to left, #ff0000 0%, #ffff00 17%, #00ff00 33%, #00ffff 50%, #0000ff 67%, #ff00ff 83%, #ff0000 100%);\n}\n\n.spectrum-dragger {\n    border-radius: 12px;\n    height: 12px;\n    width: 12px;\n    border: 1px solid white;\n    cursor: pointer;\n    position: absolute;\n    top: 0;\n    left: 0;\n    background: black;\n    box-shadow: 0 0 2px 0px rgba(0, 0, 0, 0.24);\n}\n\n.spectrum-slider {\n    position: absolute;\n    top: -1px;\n    cursor: pointer;\n    width: 13px;\n    height: 13px;\n    border-radius: 13px;\n    background-color: rgb(248, 248, 248);\n    box-shadow: 0 1px 4px 0 rgba(0, 0, 0, 0.37);\n}\n\n.swatch {\n    width: 24px;\n    height: 24px;\n    margin: 0;\n    position: absolute;\n    top: 144px;\n    left: 47px;\n    background-image: url(Images/checker.png);\n    border-radius: 16px;\n}\n\n.swatch-inner {\n    width: 100%;\n    height: 100%;\n    display: inline-block;\n    border-radius: 16px;\n}\n\n.swatch-inner-white {\n    border: 1px solid #ddd;\n}\n\n.spectrum-text {\n    position: absolute;\n    top: 184px;\n    left: 16px;\n}\n\n.spectrum-text-value {\n    display: inline-block;\n    width: 40px;\n    overflow: hidden;\n    text-align: center;\n    border: 1px solid #dadada;\n    border-radius: 2px;\n    margin-right: 6px;\n    line-height: 20px;\n    font-size: 11px;\n    padding: 0;\n    color: #333;\n    white-space: nowrap;\n}\n\n.spectrum-text-label {\n    letter-spacing: 39.5px;\n    margin-top: 8px;\n    display: block;\n    color: #969696;\n    margin-left: 16px;\n    width: 174px;\n}\n\n.spectrum-text-hex > .spectrum-text-value {\n    width: 178px;\n}\n\n.spectrum-text-hex > .spectrum-text-label {\n    letter-spacing: normal;\n    margin-left: 0px;\n    text-align: center;\n}\n\n.spectrum-palette-value {\n    background-color: rgb(65, 75, 217);\n    border-radius: 2px;\n    margin-top: 12px;\n    margin-left: 12px;\n    width: 12px;\n    height: 12px;\n    display: inline-block;\n}\n\n.spectrum-switcher {\n    border-radius: 2px;\n    height: 20px;\n    width: 20px;\n    padding: 2px;\n}\n\n.spectrum-display-switcher {\n    top: 196px;\n    position: absolute;\n    right: 10px;\n}\n\n.spectrum-switcher:hover {\n    background-color: #EEEEEE;\n}\n\n.spectrum-eye-dropper {\n    width: 32px;\n    height: 24px;\n    position: absolute;\n    left: 12px;\n    top: 144px;\n    cursor: pointer;\n}\n\n.spectrum-palette {\n    border-top: 1px solid #dadada;\n    position: absolute;\n    top: 235px;\n    width: 100%;\n    padding: 6px 24px 6px 6px;\n    display: flex;\n    flex-wrap: wrap;\n}\n\n.spectrum-palette-color {\n    width: 12px;\n    height: 12px;\n    flex: 0 0 12px;\n    display: inline-block;\n    border-radius: 2px;\n    margin: 6px;\n    cursor: pointer;\n    position: relative;\n    border: 1px solid rgba(0, 0, 0, 0.1);\n    background-position: -1px !important;\n}\n\n.spectrum-palette-color:hover:not(.spectrum-shades-shown) > .spectrum-palette-color-shadow {\n    opacity: 0.2;\n}\n\n.spectrum-palette-color:hover:not(.spectrum-shades-shown) > .spectrum-palette-color-shadow:first-child {\n    opacity: 0.6;\n    top: -3px;\n    left: 1px;\n}\n\n.spectrum-palette-color-shadow {\n    position: absolute;\n    opacity: 0;\n    margin: 0;\n    top: -5px;\n    left: 3px;\n}\n\n.palette-color-shades {\n    position: absolute;\n    background-color: white;\n    height: 237px;\n    width: 28px;\n    box-shadow: 0 4px 5px 0 rgba(0, 0, 0, 0.14), 0 1px 10px 0 rgba(0, 0, 0, 0.12), 0 2px 4px -1px rgba(0, 0, 0, 0.4);\n    z-index: 14;\n    border-radius: 2px;\n    transform-origin: 0px 237px;\n    margin-top: 18px;\n    margin-left: -8px;\n}\n\n.spectrum-palette > .spectrum-palette-color.spectrum-shades-shown {\n    z-index: 15;\n}\n\n.palette-color-shades > .spectrum-palette-color {\n    margin: 8px 0 0 0;\n    margin-left: 8px;\n    width: 12px;\n}\n\n.spectrum-palette > .spectrum-palette-color {\n    transition: transform 100ms cubic-bezier(0, 0, 0.2, 1);\n    will-change: transform;\n    z-index: 13;\n}\n\n.spectrum-palette > .spectrum-palette-color.empty-color {\n    border-color: transparent;\n}\n\n.spectrum-palette > .spectrum-palette-color:not(.empty-color):not(.has-material-shades):hover,\n.palette-color-shades > .spectrum-palette-color:not(.empty-color):hover {\n    transform: scale(1.15);\n}\n\n.add-color-toolbar {\n    margin-left: -3px;\n    margin-top: -1px;\n}\n\n.spectrum-palette-switcher {\n    right: 10px;\n    top: 235px;\n    margin-top: 9px;\n    position: absolute;\n}\n\n.palette-panel {\n    width: 100%;\n    height: 100%;\n    position: absolute;\n    top: 100%;\n    display: flex;\n    flex-direction: column;\n    background-color: white;\n    z-index: 14;\n    transition: transform 200ms cubic-bezier(0, 0, 0.2, 1), visibility 0s 200ms;\n    border-top: 1px solid #dadada;\n    visibility: hidden;\n}\n\n.palette-panel-showing > .palette-panel {\n    transform: translateY(calc(-100% + 117px));\n    transition-delay: 0s;\n    visibility: visible;\n}\n\n.palette-panel > div.toolbar {\n    position: absolute;\n    right: 6px;\n    top: 6px;\n}\n\n.palette-panel > div:not(.toolbar) {\n    flex: 0 0 38px;\n    border-bottom: 1px solid #dadada;\n    padding: 12px;\n    line-height: 14px;\n    color: #333;\n}\n\n.palette-panel > div.palette-title {\n    font-size: 14px;\n    line-height: 16px;\n    color: #333;\n    flex-basis: 40px;\n}\n\ndiv.palette-preview {\n    display: flex;\n    cursor: pointer;\n}\n\n.palette-preview-title {\n    flex: 0 0 84px;\n}\n\n.palette-preview > .spectrum-palette-color {\n    margin-top: 1px;\n}\n\n.palette-preview:hover {\n    background-color: #eee;\n}\n\n.spectrum-overlay {\n    z-index: 13;\n    visibility: hidden;\n    background-color: hsla(0, 0%, 0%, 0.5);\n    opacity: 0;\n    transition: opacity 100ms cubic-bezier(0, 0, 0.2, 1), visibility 0s 100ms;\n}\n\n.palette-panel-showing > .spectrum-overlay {\n    transition-delay: 0s;\n    visibility: visible;\n    opacity: 1;\n}\n\n.spectrum-contrast-container {\n    width: 100%;\n    height: 100%;\n}\n\n.spectrum-contrast-line {\n    fill: none;\n    stroke: white;\n    opacity: 0.7;\n    stroke-width: 1.5px;\n}\n\n.delete-color-toolbar {\n    position: absolute;\n    right: 0;\n    top: 0;\n    height: 100%;\n    background-color: #EFEFEF;\n    visibility: hidden;\n    z-index: 3;\n    width: 36px;\n    display: flex;\n    align-items: center;\n    padding-left: 5px;\n}\n\n@keyframes showDeleteToolbar {\n    from {\n        opacity: 0;\n    }\n    to {\n        opacity: 1;\n    }\n}\n\n.delete-color-toolbar.dragging {\n    visibility: visible;\n    animation: showDeleteToolbar 100ms 150ms cubic-bezier(0, 0, 0.2, 1) backwards;\n}\n\n.delete-color-toolbar-active {\n    background-color: #ddd;\n    color: white;\n}\n\n/*# sourceURL=elements/spectrum.css */";