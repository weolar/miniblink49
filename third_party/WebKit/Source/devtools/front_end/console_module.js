WebInspector.ConsoleViewMessage=function(consoleMessage,linkifier,nestingLevel)
{this._message=consoleMessage;this._linkifier=linkifier;this._repeatCount=1;this._closeGroupDecorationCount=0;this._nestingLevel=nestingLevel;this._dataGrids=[];this._customFormatters={"array":this._formatParameterAsArray,"error":this._formatParameterAsError,"function":this._formatParameterAsFunction,"generator":this._formatParameterAsObject,"iterator":this._formatParameterAsObject,"map":this._formatParameterAsObject,"node":this._formatParameterAsNode,"object":this._formatParameterAsObject,"set":this._formatParameterAsObject,"string":this._formatParameterAsString};this._previewFormatter=new WebInspector.RemoteObjectPreviewFormatter();this._searchRegex=null;}
WebInspector.ConsoleViewMessage.prototype={_target:function()
{return this.consoleMessage().target();},element:function()
{return this.toMessageElement();},wasShown:function()
{for(var i=0;this._dataGrids&&i<this._dataGrids.length;++i)
this._dataGrids[i].updateWidths();this._isVisible=true;},onResize:function()
{if(!this._isVisible)
return;for(var i=0;this._dataGrids&&i<this._dataGrids.length;++i)
this._dataGrids[i].onResize();},willHide:function()
{this._isVisible=false;this._cachedHeight=this.contentElement().offsetHeight;},fastHeight:function()
{if(this._cachedHeight)
return this._cachedHeight;const defaultConsoleRowHeight=18;if(this._message.type===WebInspector.ConsoleMessage.MessageType.Table){var table=this._message.parameters[0];if(table&&table.preview)
return defaultConsoleRowHeight*table.preview.properties.length;}
return defaultConsoleRowHeight;},consoleMessage:function()
{return this._message;},_formatMessage:function()
{this._formattedMessage=createElement("span");this._formattedMessage.appendChild(WebInspector.Widget.createStyleElement("components/objectValue.css"));this._formattedMessage.className="console-message-text source-code";function linkifyRequest(title)
{return WebInspector.Linkifier.linkifyUsingRevealer((this.request),title,this.request.url);}
var consoleMessage=this._message;if(!this._messageElement){if(consoleMessage.source===WebInspector.ConsoleMessage.MessageSource.ConsoleAPI){switch(consoleMessage.type){case WebInspector.ConsoleMessage.MessageType.Trace:this._messageElement=this._format(consoleMessage.parameters||["console.trace()"]);break;case WebInspector.ConsoleMessage.MessageType.Clear:this._messageElement=createTextNode(WebInspector.UIString("Console was cleared"));this._formattedMessage.classList.add("console-info");break;case WebInspector.ConsoleMessage.MessageType.Assert:var args=[WebInspector.UIString("Assertion failed:")];if(consoleMessage.parameters)
args=args.concat(consoleMessage.parameters);this._messageElement=this._format(args);break;case WebInspector.ConsoleMessage.MessageType.Dir:var obj=consoleMessage.parameters?consoleMessage.parameters[0]:undefined;var args=["%O",obj];this._messageElement=this._format(args);break;case WebInspector.ConsoleMessage.MessageType.Profile:case WebInspector.ConsoleMessage.MessageType.ProfileEnd:this._messageElement=this._format([consoleMessage.messageText]);break;default:if(consoleMessage.parameters&&consoleMessage.parameters.length===1&&consoleMessage.parameters[0].type==="string")
this._messageElement=this._tryFormatAsError((consoleMessage.parameters[0].value));var args=consoleMessage.parameters||[consoleMessage.messageText];this._messageElement=this._messageElement||this._format(args);}}else if(consoleMessage.source===WebInspector.ConsoleMessage.MessageSource.Network){if(consoleMessage.request){this._messageElement=createElement("span");if(consoleMessage.level===WebInspector.ConsoleMessage.MessageLevel.Error||consoleMessage.level===WebInspector.ConsoleMessage.MessageLevel.RevokedError){this._messageElement.createTextChildren(consoleMessage.request.requestMethod," ");this._messageElement.appendChild(WebInspector.Linkifier.linkifyUsingRevealer(consoleMessage.request,consoleMessage.request.url,consoleMessage.request.url));if(consoleMessage.request.failed)
this._messageElement.createTextChildren(" ",consoleMessage.request.localizedFailDescription);else
this._messageElement.createTextChildren(" ",String(consoleMessage.request.statusCode)," (",consoleMessage.request.statusText,")");}else{var fragment=WebInspector.linkifyStringAsFragmentWithCustomLinkifier(consoleMessage.messageText,linkifyRequest.bind(consoleMessage));this._messageElement.appendChild(fragment);}}else{var url=consoleMessage.url;if(url){var isExternal=!WebInspector.resourceForURL(url)&&!WebInspector.networkMapping.uiSourceCodeForURLForAnyTarget(url);this._anchorElement=WebInspector.linkifyURLAsNode(url,url,"console-message-url",isExternal);}
this._messageElement=this._format([consoleMessage.messageText]);}}else{var args=consoleMessage.parameters||[consoleMessage.messageText];this._messageElement=this._format(args);}}
if(consoleMessage.source!==WebInspector.ConsoleMessage.MessageSource.Network||consoleMessage.request){if(consoleMessage.scriptId){this._anchorElement=this._linkifyScriptId(consoleMessage.scriptId,consoleMessage.url||"",consoleMessage.line,consoleMessage.column);}else{var showBlackboxed=(consoleMessage.source!==WebInspector.ConsoleMessage.MessageSource.ConsoleAPI);var debuggerModel=WebInspector.DebuggerModel.fromTarget(this._target());var callFrame=WebInspector.DebuggerPresentationUtils.callFrameAnchorFromStackTrace(debuggerModel,consoleMessage.stackTrace,consoleMessage.asyncStackTrace,showBlackboxed);if(callFrame&&callFrame.scriptId)
this._anchorElement=this._linkifyCallFrame(callFrame);else if(consoleMessage.url&&consoleMessage.url!=="undefined")
this._anchorElement=this._linkifyLocation(consoleMessage.url,consoleMessage.line,consoleMessage.column);}}
this._formattedMessage.appendChild(this._messageElement);if(this._anchorElement){this._anchorElement.appendChild(createTextNode(" "));this._formattedMessage.insertBefore(this._anchorElement,this._formattedMessage.firstChild);}
var dumpStackTrace=(!!consoleMessage.stackTrace||!!consoleMessage.asyncStackTrace)&&(consoleMessage.source===WebInspector.ConsoleMessage.MessageSource.Network||consoleMessage.level===WebInspector.ConsoleMessage.MessageLevel.Error||consoleMessage.level===WebInspector.ConsoleMessage.MessageLevel.RevokedError||consoleMessage.type===WebInspector.ConsoleMessage.MessageType.Trace);if(dumpStackTrace){var treeOutline=new TreeOutline();treeOutline.element.classList.add("outline-disclosure","outline-disclosure-no-padding");var content=this._formattedMessage;var root=new TreeElement(content);root.toggleOnClick=true;root.selectable=false;content.treeElementForTest=root;treeOutline.appendChild(root);if(consoleMessage.type===WebInspector.ConsoleMessage.MessageType.Trace)
root.expand();this._populateStackTraceTreeElement(root);this._formattedMessage=treeOutline.element;}},formattedMessage:function()
{if(!this._formattedMessage)
this._formatMessage();return this._formattedMessage;},_linkifyLocation:function(url,lineNumber,columnNumber)
{var target=this._target();if(!target)
return null;lineNumber=lineNumber?lineNumber-1:0;columnNumber=columnNumber?columnNumber-1:0;return this._linkifier.linkifyScriptLocation(target,null,url,lineNumber,columnNumber,"console-message-url");},_linkifyCallFrame:function(callFrame)
{var target=this._target();return this._linkifier.linkifyConsoleCallFrame(target,callFrame,"console-message-url");},_linkifyScriptId:function(scriptId,url,lineNumber,columnNumber)
{var target=this._target();if(!target)
return null;lineNumber=lineNumber?lineNumber-1:0;columnNumber=columnNumber?columnNumber-1:0;return this._linkifier.linkifyScriptLocation(target,scriptId,url,lineNumber,columnNumber,"console-message-url");},_format:function(parameters)
{var formattedResult=createElement("span");if(!parameters.length)
return formattedResult;var target=this._target();for(var i=0;i<parameters.length;++i){if(parameters[i]instanceof WebInspector.RemoteObject)
continue;if(!target){parameters[i]=WebInspector.RemoteObject.fromLocalObject(parameters[i]);continue;}
if(typeof parameters[i]==="object")
parameters[i]=target.runtimeModel.createRemoteObject(parameters[i]);else
parameters[i]=target.runtimeModel.createRemoteObjectFromPrimitiveValue(parameters[i]);}
var shouldFormatMessage=WebInspector.RemoteObject.type(parameters[0])==="string"&&(this._message.type!==WebInspector.ConsoleMessage.MessageType.Result||this._message.level===WebInspector.ConsoleMessage.MessageLevel.Error||this._message.level===WebInspector.ConsoleMessage.MessageLevel.RevokedError);if(shouldFormatMessage){var result=this._formatWithSubstitutionString(parameters[0].description,parameters.slice(1),formattedResult);parameters=result.unusedSubstitutions;if(parameters.length)
formattedResult.createTextChild(" ");}
if(this._message.type===WebInspector.ConsoleMessage.MessageType.Table){formattedResult.appendChild(this._formatParameterAsTable(parameters));return formattedResult;}
for(var i=0;i<parameters.length;++i){if(shouldFormatMessage&&parameters[i].type==="string")
formattedResult.appendChild(WebInspector.linkifyStringAsFragment(parameters[i].description));else
formattedResult.appendChild(this._formatParameter(parameters[i],false,true));if(i<parameters.length-1)
formattedResult.createTextChild(" ");}
return formattedResult;},_formatParameter:function(output,forceObjectFormat,includePreview)
{if(output.customPreview()){return(new WebInspector.CustomPreviewComponent(output)).element;}
var type=forceObjectFormat?"object":(output.subtype||output.type);var formatter=this._customFormatters[type]||this._formatParameterAsValue;var span=createElement("span");span.className="object-value-"+type+" source-code";formatter.call(this,output,span,includePreview);return span;},_formatParameterAsValue:function(obj,elem)
{elem.createTextChild(obj.description||"");if(obj.objectId)
elem.addEventListener("contextmenu",this._contextMenuEventFired.bind(this,obj),false);},_formatParameterAsObject:function(obj,elem,includePreview)
{this._formatParameterAsArrayOrObject(obj,elem,includePreview);},_formatParameterAsArrayOrObject:function(obj,elem,includePreview)
{var titleElement=createElement("span");if(includePreview&&obj.preview){titleElement.classList.add("console-object-preview");var lossless=this._previewFormatter.appendObjectPreview(titleElement,obj.preview);if(lossless){elem.appendChild(titleElement);titleElement.addEventListener("contextmenu",this._contextMenuEventFired.bind(this,obj),false);return;}}else{if(obj.type==="function"){WebInspector.ObjectPropertiesSection.formatObjectAsFunction(obj,titleElement,false);titleElement.classList.add("object-value-function");}else{titleElement.createTextChild(obj.description||"");}}
var note=titleElement.createChild("span","object-info-state-note");note.title=WebInspector.UIString("Object value at left was snapshotted when logged, value below was evaluated just now.");var section=new WebInspector.ObjectPropertiesSection(obj,titleElement);section.enableContextMenu();elem.appendChild(section.element);section.element.classList.add("console-view-object-properties-section");},_formatParameterAsFunction:function(func,element,includePreview)
{WebInspector.RemoteFunction.objectAsFunction(func).targetFunction().then(formatTargetFunction.bind(this));function formatTargetFunction(targetFunction)
{var functionElement=createElement("span")
WebInspector.ObjectPropertiesSection.formatObjectAsFunction(targetFunction,functionElement,true,includePreview);element.appendChild(functionElement);if(targetFunction!==func){var note=element.createChild("span","object-info-state-note");note.title=WebInspector.UIString("Function was resolved from bound function.");}
element.addEventListener("contextmenu",this._contextMenuEventFired.bind(this,targetFunction),false);}},_contextMenuEventFired:function(obj,event)
{var contextMenu=new WebInspector.ContextMenu(event);contextMenu.appendApplicableItems(obj);contextMenu.show();},_renderPropertyPreviewOrAccessor:function(object,propertyPath)
{var property=propertyPath.peekLast();if(property.type==="accessor")
return this._formatAsAccessorProperty(object,propertyPath.select("name"),false);return this._previewFormatter.renderPropertyPreview(property.type,(property.subtype),property.value);},_formatParameterAsNode:function(object,elem)
{WebInspector.Renderer.renderPromise(object).then(appendRenderer,failedToRender.bind(this));function appendRenderer(rendererElement)
{elem.appendChild(rendererElement);}
function failedToRender()
{this._formatParameterAsObject(object,elem,false);}},useArrayPreviewInFormatter:function(array)
{return this._message.type!==WebInspector.ConsoleMessage.MessageType.DirXML;},_formatParameterAsArray:function(array,elem)
{var maxFlatArrayLength=100;if(this.useArrayPreviewInFormatter(array)||array.arrayLength()>maxFlatArrayLength)
this._formatParameterAsArrayOrObject(array,elem,this.useArrayPreviewInFormatter(array)||array.arrayLength()<=maxFlatArrayLength);else
array.getAllProperties(false,this._printArray.bind(this,array,elem));},_formatParameterAsTable:function(parameters)
{var element=createElementWithClass("div","console-message-formatted-table");var table=parameters[0];if(!table||!table.preview)
return element;var columnNames=[];var preview=table.preview;var rows=[];for(var i=0;i<preview.properties.length;++i){var rowProperty=preview.properties[i];var rowPreview=rowProperty.valuePreview;if(!rowPreview)
continue;var rowValue={};const maxColumnsToRender=20;for(var j=0;j<rowPreview.properties.length;++j){var cellProperty=rowPreview.properties[j];var columnRendered=columnNames.indexOf(cellProperty.name)!=-1;if(!columnRendered){if(columnNames.length===maxColumnsToRender)
continue;columnRendered=true;columnNames.push(cellProperty.name);}
if(columnRendered){var cellElement=this._renderPropertyPreviewOrAccessor(table,[rowProperty,cellProperty]);cellElement.classList.add("console-message-nowrap-below");rowValue[cellProperty.name]=cellElement;}}
rows.push([rowProperty.name,rowValue]);}
var flatValues=[];for(var i=0;i<rows.length;++i){var rowName=rows[i][0];var rowValue=rows[i][1];flatValues.push(rowName);for(var j=0;j<columnNames.length;++j)
flatValues.push(rowValue[columnNames[j]]);}
var dataGridContainer=element.createChild("span");if(!preview.lossless||!flatValues.length){element.appendChild(this._formatParameter(table,true,false));if(!flatValues.length)
return element;}
columnNames.unshift(WebInspector.UIString("(index)"));var dataGrid=WebInspector.SortableDataGrid.create(columnNames,flatValues);dataGrid.renderInline();dataGridContainer.appendChild(dataGrid.element);this._dataGrids.push(dataGrid);return element;},_formatParameterAsString:function(output,elem)
{var span=createElement("span");span.className="object-value-string source-code";span.appendChild(WebInspector.linkifyStringAsFragment(output.description||""));elem.classList.remove("object-value-string");elem.createTextChild("\"");elem.appendChild(span);elem.createTextChild("\"");},_formatParameterAsError:function(output,elem)
{var span=elem.createChild("span","object-value-error source-code");var text=output.description||"";var lines=text.split("\n",2);span.appendChild(WebInspector.linkifyStringAsFragment(lines[0]));if(lines.length>1){var detailedLink=elem.createChild("a");detailedLink.textContent="(\u2026)";function showDetailed(event)
{span.removeChildren();detailedLink.remove();span.appendChild(WebInspector.linkifyStringAsFragment(text));event.consume(true);}
detailedLink._showDetailedForTest=showDetailed.bind(null,new MouseEvent('click'));detailedLink.addEventListener("click",showDetailed,false);}},_printArray:function(array,elem,properties)
{if(!properties){this._formatParameterAsObject(array,elem,false);return;}
var elements=[];for(var i=0;i<properties.length;++i){var property=properties[i];var name=property.name;if(isNaN(name))
continue;if(property.getter)
elements[name]=this._formatAsAccessorProperty(array,[name],true);else if(property.value)
elements[name]=this._formatAsArrayEntry(property.value);}
elem.createTextChild("[");var lastNonEmptyIndex=-1;function appendUndefined(elem,index)
{if(index-lastNonEmptyIndex<=1)
return;var span=elem.createChild("span","object-value-undefined");span.textContent=WebInspector.UIString("undefined × %d",index-lastNonEmptyIndex-1);}
var length=array.arrayLength();for(var i=0;i<length;++i){var element=elements[i];if(!element)
continue;if(i-lastNonEmptyIndex>1){appendUndefined(elem,i);elem.createTextChild(", ");}
elem.appendChild(element);lastNonEmptyIndex=i;if(i<length-1)
elem.createTextChild(", ");}
appendUndefined(elem,length);elem.createTextChild("]");elem.addEventListener("contextmenu",this._contextMenuEventFired.bind(this,array),false);},_formatAsArrayEntry:function(output)
{return this._formatParameter(output,output.subtype==="array",false);},_formatAsAccessorProperty:function(object,propertyPath,isArrayEntry)
{var rootElement=WebInspector.ObjectPropertyTreeElement.createRemoteObjectAccessorPropertySpan(object,propertyPath,onInvokeGetterClick.bind(this));function onInvokeGetterClick(result,wasThrown)
{if(!result)
return;rootElement.removeChildren();if(wasThrown){var element=rootElement.createChild("span","error-message");element.textContent=WebInspector.UIString("<exception>");element.title=(result.description);}else if(isArrayEntry){rootElement.appendChild(this._formatAsArrayEntry(result));}else{const maxLength=100;var type=result.type;var subtype=result.subtype;var description="";if(type!=="function"&&result.description){if(type==="string"||subtype==="regexp")
description=result.description.trimMiddle(maxLength);else
description=result.description.trimEnd(maxLength);}
rootElement.appendChild(this._previewFormatter.renderPropertyPreview(type,subtype,description));}}
return rootElement;},_formatWithSubstitutionString:function(format,parameters,formattedResult)
{var formatters={};function parameterFormatter(force,obj)
{return this._formatParameter(obj,force,false);}
function stringFormatter(obj)
{return obj.description;}
function floatFormatter(obj)
{if(typeof obj.value!=="number")
return"NaN";return obj.value;}
function integerFormatter(obj)
{if(typeof obj.value!=="number")
return"NaN";return Math.floor(obj.value);}
function bypassFormatter(obj)
{return(obj instanceof Node)?obj:"";}
var currentStyle=null;function styleFormatter(obj)
{currentStyle={};var buffer=createElement("span");buffer.setAttribute("style",obj.description);for(var i=0;i<buffer.style.length;i++){var property=buffer.style[i];var value=buffer.style.getPropertyValue(property);if(!value.startsWith("url(")&&isWhitelistedProperty(property))
currentStyle[property]=buffer.style[property];}}
function isWhitelistedProperty(property)
{var prefixes=["background","border","color","font","line","margin","padding","text","-webkit-background","-webkit-border","-webkit-font","-webkit-margin","-webkit-padding","-webkit-text"];for(var i=0;i<prefixes.length;i++){if(property.startsWith(prefixes[i]))
return true;}
return false;}
formatters.o=parameterFormatter.bind(this,false);formatters.s=stringFormatter;formatters.f=floatFormatter;formatters.i=integerFormatter;formatters.d=integerFormatter;formatters.c=styleFormatter;formatters.O=parameterFormatter.bind(this,true);formatters._=bypassFormatter;function append(a,b)
{if(b instanceof Node)
a.appendChild(b);else if(typeof b!=="undefined"){var toAppend=WebInspector.linkifyStringAsFragment(String(b));if(currentStyle){var wrapper=createElement('span');wrapper.appendChild(toAppend);applyCurrentStyle(wrapper);for(var i=0;i<wrapper.children.length;++i)
applyCurrentStyle(wrapper.children[i]);toAppend=wrapper;}
a.appendChild(toAppend);}
return a;}
function applyCurrentStyle(element)
{for(var key in currentStyle)
element.style[key]=currentStyle[key];}
return String.format(format,parameters,formatters,formattedResult,append);},matchesFilterRegex:function(regexObject)
{regexObject.lastIndex=0;var text=this.searchableElement().deepTextContent();if(this._anchorElement)
text+=" "+this._anchorElement.textContent;return regexObject.test(text);},updateTimestamp:function(show)
{if(!this._formattedMessage)
return;if(show&&!this.timestampElement){this.timestampElement=createElementWithClass("span","console-timestamp");this.timestampElement.textContent=(new Date(this._message.timestamp)).toConsoleTime()+" ";var afterRepeatCountChild=this._repeatCountElement&&this._repeatCountElement.nextSibling;this._formattedMessage.insertBefore(this.timestampElement,this._formattedMessage.firstChild);return;}
if(!show&&this.timestampElement){this.timestampElement.remove();delete this.timestampElement;}},nestingLevel:function()
{return this._nestingLevel;},resetCloseGroupDecorationCount:function()
{if(!this._closeGroupDecorationCount)
return;this._closeGroupDecorationCount=0;this._updateCloseGroupDecorations();},incrementCloseGroupDecorationCount:function()
{++this._closeGroupDecorationCount;this._updateCloseGroupDecorations();},_updateCloseGroupDecorations:function()
{if(!this._nestingLevelMarkers)
return;for(var i=0,n=this._nestingLevelMarkers.length;i<n;++i){var marker=this._nestingLevelMarkers[i];marker.classList.toggle("group-closed",n-i<=this._closeGroupDecorationCount);}},contentElement:function()
{if(this._element)
return this._element;var element=createElementWithClass("div","console-message");this._element=element;if(this._message.type===WebInspector.ConsoleMessage.MessageType.StartGroup||this._message.type===WebInspector.ConsoleMessage.MessageType.StartGroupCollapsed)
element.classList.add("console-group-title");element.appendChild(this.formattedMessage());if(this._repeatCount>1)
this._showRepeatCountElement();this.updateTimestamp(WebInspector.moduleSetting("consoleTimestampsEnabled").get());return this._element;},toMessageElement:function()
{if(this._wrapperElement)
return this._wrapperElement;this._wrapperElement=createElement("div");this.updateMessageElement();return this._wrapperElement;},updateMessageElement:function()
{if(!this._wrapperElement)
return;this._wrapperElement.className="console-message-wrapper";this._wrapperElement.removeChildren();this._nestingLevelMarkers=[];for(var i=0;i<this._nestingLevel;++i)
this._nestingLevelMarkers.push(this._wrapperElement.createChild("div","nesting-level-marker"));this._updateCloseGroupDecorations();this._wrapperElement.message=this;switch(this._message.level){case WebInspector.ConsoleMessage.MessageLevel.Log:this._wrapperElement.classList.add("console-log-level");break;case WebInspector.ConsoleMessage.MessageLevel.Debug:this._wrapperElement.classList.add("console-debug-level");break;case WebInspector.ConsoleMessage.MessageLevel.Warning:this._wrapperElement.classList.add("console-warning-level");break;case WebInspector.ConsoleMessage.MessageLevel.Error:this._wrapperElement.classList.add("console-error-level");break;case WebInspector.ConsoleMessage.MessageLevel.RevokedError:this._wrapperElement.classList.add("console-revokedError-level");break;case WebInspector.ConsoleMessage.MessageLevel.Info:this._wrapperElement.classList.add("console-info-level");break;}
this._wrapperElement.appendChild(this.contentElement());},_populateStackTraceTreeElement:function(parentTreeElement)
{var target=this._target();if(!target)
return;var content=WebInspector.DOMPresentationUtils.buildStackTracePreviewContents(target,this._linkifier,this._message.stackTrace,this._message.asyncStackTrace);var treeElement=new TreeElement(content);treeElement.selectable=false;parentTreeElement.appendChild(treeElement);},resetIncrementRepeatCount:function()
{this._repeatCount=1;if(!this._repeatCountElement)
return;this._repeatCountElement.remove();delete this._repeatCountElement;},incrementRepeatCount:function()
{this._repeatCount++;this._showRepeatCountElement();},_showRepeatCountElement:function()
{if(!this._element)
return;if(!this._repeatCountElement){this._repeatCountElement=createElement("span");this._repeatCountElement.className="bubble-repeat-count";this._element.insertBefore(this._repeatCountElement,this._element.firstChild);this._element.classList.add("repeated-message");}
this._repeatCountElement.textContent=this._repeatCount;},toString:function()
{var sourceString;switch(this._message.source){case WebInspector.ConsoleMessage.MessageSource.XML:sourceString="XML";break;case WebInspector.ConsoleMessage.MessageSource.JS:sourceString="JavaScript";break;case WebInspector.ConsoleMessage.MessageSource.Network:sourceString="Network";break;case WebInspector.ConsoleMessage.MessageSource.ConsoleAPI:sourceString="ConsoleAPI";break;case WebInspector.ConsoleMessage.MessageSource.Storage:sourceString="Storage";break;case WebInspector.ConsoleMessage.MessageSource.AppCache:sourceString="AppCache";break;case WebInspector.ConsoleMessage.MessageSource.Rendering:sourceString="Rendering";break;case WebInspector.ConsoleMessage.MessageSource.CSS:sourceString="CSS";break;case WebInspector.ConsoleMessage.MessageSource.Security:sourceString="Security";break;case WebInspector.ConsoleMessage.MessageSource.Other:sourceString="Other";break;}
var typeString;switch(this._message.type){case WebInspector.ConsoleMessage.MessageType.Log:typeString="Log";break;case WebInspector.ConsoleMessage.MessageType.Dir:typeString="Dir";break;case WebInspector.ConsoleMessage.MessageType.DirXML:typeString="Dir XML";break;case WebInspector.ConsoleMessage.MessageType.Trace:typeString="Trace";break;case WebInspector.ConsoleMessage.MessageType.StartGroupCollapsed:case WebInspector.ConsoleMessage.MessageType.StartGroup:typeString="Start Group";break;case WebInspector.ConsoleMessage.MessageType.EndGroup:typeString="End Group";break;case WebInspector.ConsoleMessage.MessageType.Assert:typeString="Assert";break;case WebInspector.ConsoleMessage.MessageType.Result:typeString="Result";break;case WebInspector.ConsoleMessage.MessageType.Profile:case WebInspector.ConsoleMessage.MessageType.ProfileEnd:typeString="Profiling";break;}
var levelString;switch(this._message.level){case WebInspector.ConsoleMessage.MessageLevel.Log:levelString="Log";break;case WebInspector.ConsoleMessage.MessageLevel.Warning:levelString="Warning";break;case WebInspector.ConsoleMessage.MessageLevel.Debug:levelString="Debug";break;case WebInspector.ConsoleMessage.MessageLevel.Error:levelString="Error";break;case WebInspector.ConsoleMessage.MessageLevel.RevokedError:levelString="RevokedError";break;case WebInspector.ConsoleMessage.MessageLevel.Info:levelString="Info";break;}
return sourceString+" "+typeString+" "+levelString+": "+this.formattedMessage().textContent+"\n"+this._message.url+" line "+this._message.line;},get text()
{return this._message.messageText;},setSearchRegex:function(regex)
{if(this._searchHiglightNodeChanges&&this._searchHiglightNodeChanges.length)
WebInspector.revertDomChanges(this._searchHiglightNodeChanges);this._searchRegex=regex;this._searchHighlightNodes=[];this._searchHiglightNodeChanges=[];if(!this._searchRegex)
return;var text=this.searchableElement().deepTextContent();var match;this._searchRegex.lastIndex=0;var sourceRanges=[];while((match=this._searchRegex.exec(text))&&match[0])
sourceRanges.push(new WebInspector.SourceRange(match.index,match[0].length));if(sourceRanges.length&&this.searchableElement())
this._searchHighlightNodes=WebInspector.highlightSearchResults(this.searchableElement(),sourceRanges,this._searchHiglightNodeChanges);},searchRegex:function()
{return this._searchRegex;},searchCount:function()
{return this._searchHighlightNodes.length;},searchHighlightNode:function(index)
{return this._searchHighlightNodes[index];},searchableElement:function()
{this.formattedMessage();return this._messageElement;},_tryFormatAsError:function(string)
{function startsWith(prefix)
{return string.startsWith(prefix);}
var errorPrefixes=["EvalError","ReferenceError","SyntaxError","TypeError","RangeError","Error","URIError"];var target=this._target();if(!target||!errorPrefixes.some(startsWith))
return null;var debuggerModel=WebInspector.DebuggerModel.fromTarget(target);if(!debuggerModel)
return null;var lines=string.split("\n");var links=[];var position=0;for(var i=0;i<lines.length;++i){position+=i>0?lines[i-1].length+1:0;var isCallFrameLine=/^\s*at\s/.test(lines[i]);if(!isCallFrameLine&&links.length)
return null;if(!isCallFrameLine)
continue;var openBracketIndex=lines[i].indexOf("(");var closeBracketIndex=lines[i].indexOf(")");var hasOpenBracket=openBracketIndex!==-1;var hasCloseBracket=closeBracketIndex!==-1;if((openBracketIndex>closeBracketIndex)||(hasOpenBracket^hasCloseBracket))
return null;var left=hasOpenBracket?openBracketIndex+1:lines[i].indexOf("at")+3;var right=hasOpenBracket?closeBracketIndex:lines[i].length;var linkCandidate=lines[i].substring(left,right);var splitResult=WebInspector.ParsedURL.splitLineAndColumn(linkCandidate);if(!splitResult)
return null;var parsed=splitResult.url.asParsedURL();var url;if(parsed)
url=parsed.url;else if(debuggerModel.scriptsForSourceURL(splitResult.url).length)
url=splitResult.url;else if(splitResult.url==="<anonymous>")
continue;else
return null;links.push({url:url,positionLeft:position+left,positionRight:position+right,lineNumber:splitResult.lineNumber,columnNumber:splitResult.columnNumber});}
if(!links.length)
return null;var formattedResult=createElement("span");var start=0;for(var i=0;i<links.length;++i){formattedResult.appendChild(WebInspector.linkifyStringAsFragment(string.substring(start,links[i].positionLeft)));formattedResult.appendChild(this._linkifier.linkifyScriptLocation(target,null,links[i].url,links[i].lineNumber,links[i].columnNumber));start=links[i].positionRight;}
if(start!=string.length)
formattedResult.appendChild(WebInspector.linkifyStringAsFragment(string.substring(start)));return formattedResult;}}
WebInspector.ConsoleGroupViewMessage=function(consoleMessage,linkifier,nestingLevel)
{console.assert(consoleMessage.isGroupStartMessage());WebInspector.ConsoleViewMessage.call(this,consoleMessage,linkifier,nestingLevel);this.setCollapsed(consoleMessage.type===WebInspector.ConsoleMessage.MessageType.StartGroupCollapsed);}
WebInspector.ConsoleGroupViewMessage.prototype={setCollapsed:function(collapsed)
{this._collapsed=collapsed;if(this._wrapperElement)
this._wrapperElement.classList.toggle("collapsed",this._collapsed);},collapsed:function()
{return this._collapsed;},toMessageElement:function()
{if(!this._wrapperElement){WebInspector.ConsoleViewMessage.prototype.toMessageElement.call(this);this._wrapperElement.classList.toggle("collapsed",this._collapsed);}
return this._wrapperElement;},__proto__:WebInspector.ConsoleViewMessage.prototype};WebInspector.ConsoleView=function()
{WebInspector.VBox.call(this);this.setMinimumSize(0,35);this.registerRequiredCSS("ui/filter.css");this.registerRequiredCSS("console/consoleView.css");this._searchableView=new WebInspector.SearchableView(this);this._searchableView.setPlaceholder(WebInspector.UIString("Find string in logs"));this._searchableView.setMinimalSearchQuerySize(0);this._searchableView.show(this.element);this._contentsElement=this._searchableView.element;this._contentsElement.classList.add("console-view");this._visibleViewMessages=[];this._urlToMessageCount={};this._hiddenByFilterCount=0;this._regexMatchRanges=[];this._executionContextComboBox=new WebInspector.ToolbarComboBox(null,"console-context");this._executionContextComboBox.setMaxWidth(200);this._executionContextModel=new WebInspector.ExecutionContextModel(this._executionContextComboBox.selectElement());this._filter=new WebInspector.ConsoleViewFilter(this);this._filter.addEventListener(WebInspector.ConsoleViewFilter.Events.FilterChanged,this._updateMessageList.bind(this));this._filterBar=new WebInspector.FilterBar("consoleView");this._preserveLogCheckbox=new WebInspector.ToolbarCheckbox(WebInspector.UIString("Preserve log"),WebInspector.UIString("Do not clear log on page reload / navigation"),WebInspector.moduleSetting("preserveConsoleLog"));this._progressToolbarItem=new WebInspector.ToolbarItem(createElement("div"));var toolbar=new WebInspector.Toolbar(this._contentsElement);toolbar.appendToolbarItem(WebInspector.ToolbarButton.createActionButton("console.clear"));toolbar.appendToolbarItem(this._filterBar.filterButton());toolbar.appendToolbarItem(this._executionContextComboBox);toolbar.appendToolbarItem(this._preserveLogCheckbox);toolbar.appendToolbarItem(this._progressToolbarItem);this._contentsElement.appendChild(this._filterBar.filtersElement());this._filter.addFilters(this._filterBar);this._viewport=new WebInspector.ViewportControl(this);this._viewport.setStickToBottom(true);this._viewport.contentElement().classList.add("console-group","console-group-messages");this._contentsElement.appendChild(this._viewport.element);this._messagesElement=this._viewport.element;this._messagesElement.id="console-messages";this._messagesElement.classList.add("monospace");this._messagesElement.addEventListener("click",this._messagesClicked.bind(this),true);this._viewportThrottler=new WebInspector.Throttler(50);this._filterStatusMessageElement=createElementWithClass("div","console-message");this._messagesElement.insertBefore(this._filterStatusMessageElement,this._messagesElement.firstChild);this._filterStatusTextElement=this._filterStatusMessageElement.createChild("span","console-info");this._filterStatusMessageElement.createTextChild(" ");var resetFiltersLink=this._filterStatusMessageElement.createChild("span","console-info link");resetFiltersLink.textContent=WebInspector.UIString("Show all messages.");resetFiltersLink.addEventListener("click",this._filter.reset.bind(this._filter),true);this._topGroup=WebInspector.ConsoleGroup.createTopGroup();this._currentGroup=this._topGroup;this._promptElement=this._messagesElement.createChild("div","source-code");this._promptElement.id="console-prompt";this._promptElement.spellcheck=false;this._searchableView.setDefaultFocusedElement(this._promptElement);var selectAllFixer=this._messagesElement.createChild("div","console-view-fix-select-all");selectAllFixer.textContent=".";this._showAllMessagesCheckbox=new WebInspector.ToolbarCheckbox(WebInspector.UIString("Show all messages"));this._showAllMessagesCheckbox.inputElement.checked=true;this._showAllMessagesCheckbox.inputElement.addEventListener("change",this._updateMessageList.bind(this),false);this._showAllMessagesCheckbox.element.classList.add("hidden");toolbar.appendToolbarItem(this._showAllMessagesCheckbox);this._registerShortcuts();this._messagesElement.addEventListener("contextmenu",this._handleContextMenuEvent.bind(this),false);WebInspector.moduleSetting("monitoringXHREnabled").addChangeListener(this._monitoringXHREnabledSettingChanged,this);this._linkifier=new WebInspector.Linkifier();this._consoleMessages=[];this._viewMessageSymbol=Symbol("viewMessage");this._prompt=new WebInspector.TextPromptWithHistory(WebInspector.ExecutionContextSelector.completionsForTextPromptInCurrentContext);this._prompt.setSuggestBoxEnabled(true);this._prompt.setAutocompletionTimeout(0);this._prompt.renderAsBlock();var proxyElement=this._prompt.attach(this._promptElement);proxyElement.addEventListener("keydown",this._promptKeyDown.bind(this),false);this._consoleHistorySetting=WebInspector.settings.createLocalSetting("consoleHistory",[]);var historyData=this._consoleHistorySetting.get();this._prompt.setHistoryData(historyData);this._updateFilterStatus();WebInspector.moduleSetting("consoleTimestampsEnabled").addChangeListener(this._consoleTimestampsSettingChanged,this);this._registerWithMessageSink();WebInspector.targetManager.observeTargets(this);WebInspector.targetManager.addEventListener(WebInspector.TargetManager.Events.MainFrameNavigated,this._onMainFrameNavigated,this);this._initConsoleMessages();WebInspector.context.addFlavorChangeListener(WebInspector.ExecutionContext,this._executionContextChanged,this);}
WebInspector.ConsoleView.persistedHistorySize=300;WebInspector.ConsoleView.prototype={searchableView:function()
{return this._searchableView;},_onMainFrameNavigated:function(event)
{var frame=(event.data);WebInspector.console.log(WebInspector.UIString("Navigated to %s",frame.url));},_initConsoleMessages:function()
{var mainTarget=WebInspector.targetManager.mainTarget();if(!mainTarget||!mainTarget.resourceTreeModel.cachedResourcesLoaded()){WebInspector.targetManager.addModelListener(WebInspector.ResourceTreeModel,WebInspector.ResourceTreeModel.EventTypes.CachedResourcesLoaded,this._onResourceTreeModelLoaded,this);return;}
this._fetchMultitargetMessages();},_onResourceTreeModelLoaded:function(event)
{var resourceTreeModel=event.target;if(resourceTreeModel.target()!==WebInspector.targetManager.mainTarget())
return;WebInspector.targetManager.removeModelListener(WebInspector.ResourceTreeModel,WebInspector.ResourceTreeModel.EventTypes.CachedResourcesLoaded,this._onResourceTreeModelLoaded,this);this._fetchMultitargetMessages();},_fetchMultitargetMessages:function()
{WebInspector.multitargetConsoleModel.addEventListener(WebInspector.ConsoleModel.Events.ConsoleCleared,this._consoleCleared,this);WebInspector.multitargetConsoleModel.addEventListener(WebInspector.ConsoleModel.Events.MessageAdded,this._onConsoleMessageAdded,this);WebInspector.multitargetConsoleModel.addEventListener(WebInspector.ConsoleModel.Events.MessageUpdated,this._onConsoleMessageUpdated,this);WebInspector.multitargetConsoleModel.addEventListener(WebInspector.ConsoleModel.Events.CommandEvaluated,this._commandEvaluated,this);WebInspector.multitargetConsoleModel.messages().forEach(this._addConsoleMessage,this);},itemCount:function()
{return this._visibleViewMessages.length;},itemElement:function(index)
{return this._visibleViewMessages[index];},fastHeight:function(index)
{return this._visibleViewMessages[index].fastHeight();},minimumRowHeight:function()
{return 16;},targetAdded:function(target)
{this._viewport.invalidate();if(WebInspector.targetManager.targets().length>1&&WebInspector.targetManager.mainTarget().isPage())
this._showAllMessagesCheckbox.element.classList.toggle("hidden",false);},targetRemoved:function(target)
{},_registerWithMessageSink:function()
{WebInspector.console.messages().forEach(this._addSinkMessage,this);WebInspector.console.addEventListener(WebInspector.Console.Events.MessageAdded,messageAdded,this);function messageAdded(event)
{this._addSinkMessage((event.data));}},_addSinkMessage:function(message)
{var level=WebInspector.ConsoleMessage.MessageLevel.Debug;switch(message.level){case WebInspector.Console.MessageLevel.Error:level=WebInspector.ConsoleMessage.MessageLevel.Error;break;case WebInspector.Console.MessageLevel.Warning:level=WebInspector.ConsoleMessage.MessageLevel.Warning;break;}
var consoleMessage=new WebInspector.ConsoleMessage(null,WebInspector.ConsoleMessage.MessageSource.Other,level,message.text,undefined,undefined,undefined,undefined,undefined,undefined,undefined,message.timestamp);this._addConsoleMessage(consoleMessage);},_consoleTimestampsSettingChanged:function(event)
{var enabled=(event.data);this._updateMessageList();this._consoleMessages.forEach(function(viewMessage){viewMessage.updateTimestamp(enabled);});},defaultFocusedElement:function()
{return this._promptElement;},_executionContextChanged:function()
{this._prompt.clearAutoComplete(true);if(!this._showAllMessagesCheckbox.checked())
this._updateMessageList();},willHide:function()
{this._hidePromptSuggestBox();},wasShown:function()
{this._viewport.refresh();if(!this._prompt.isCaretInsidePrompt())
this._prompt.moveCaretToEndOfPrompt();},focus:function()
{if(this._promptElement===WebInspector.currentFocusElement())
return;WebInspector.setCurrentFocusElement(this._promptElement);this._prompt.moveCaretToEndOfPrompt();},restoreScrollPositions:function()
{if(this._viewport.scrolledToBottom())
this._immediatelyScrollToBottom();else
WebInspector.Widget.prototype.restoreScrollPositions.call(this);},onResize:function()
{this._scheduleViewportRefresh();this._hidePromptSuggestBox();if(this._viewport.scrolledToBottom())
this._immediatelyScrollToBottom();for(var i=0;i<this._visibleViewMessages.length;++i)
this._visibleViewMessages[i].onResize();},_hidePromptSuggestBox:function()
{this._prompt.hideSuggestBox();this._prompt.clearAutoComplete(true);},_scheduleViewportRefresh:function()
{function invalidateViewport()
{if(this._needsFullUpdate){this._updateMessageList();delete this._needsFullUpdate;}else{this._viewport.invalidate();}
return Promise.resolve();}
this._viewportThrottler.schedule(invalidateViewport.bind(this));},_immediatelyScrollToBottom:function()
{this._promptElement.scrollIntoView(true);},_updateFilterStatus:function()
{this._filterStatusTextElement.textContent=WebInspector.UIString(this._hiddenByFilterCount===1?"%d message is hidden by filters.":"%d messages are hidden by filters.",this._hiddenByFilterCount);this._filterStatusMessageElement.style.display=this._hiddenByFilterCount?"":"none";},_onConsoleMessageAdded:function(event)
{var message=(event.data);this._addConsoleMessage(message);},_addConsoleMessage:function(message)
{function compareTimestamps(viewMessage1,viewMessage2)
{return WebInspector.ConsoleMessage.timestampComparator(viewMessage1.consoleMessage(),viewMessage2.consoleMessage());}
if(message.type===WebInspector.ConsoleMessage.MessageType.Command||message.type===WebInspector.ConsoleMessage.MessageType.Result)
message.timestamp=this._consoleMessages.length?this._consoleMessages.peekLast().consoleMessage().timestamp:0;var viewMessage=this._createViewMessage(message);message[this._viewMessageSymbol]=viewMessage;var insertAt=insertionIndexForObjectInListSortedByFunction(viewMessage,this._consoleMessages,compareTimestamps,true);var insertedInMiddle=insertAt<this._consoleMessages.length;this._consoleMessages.splice(insertAt,0,viewMessage);if(this._urlToMessageCount[message.url])
++this._urlToMessageCount[message.url];else
this._urlToMessageCount[message.url]=1;if(!insertedInMiddle){this._appendMessageToEnd(viewMessage);this._updateFilterStatus();this._searchableView.updateSearchMatchesCount(this._regexMatchRanges.length);}else{this._needsFullUpdate=true;}
this._scheduleViewportRefresh();this._consoleMessageAddedForTest(viewMessage);},_onConsoleMessageUpdated:function(event)
{var message=(event.data);var viewMessage=message[this._viewMessageSymbol];if(viewMessage){viewMessage.updateMessageElement();this._updateMessageList();}},_consoleMessageAddedForTest:function(viewMessage){},_appendMessageToEnd:function(viewMessage)
{if(!this._filter.shouldBeVisible(viewMessage)){this._hiddenByFilterCount++;return;}
if(this._tryToCollapseMessages(viewMessage,this._visibleViewMessages.peekLast()))
return;var lastMessage=this._visibleViewMessages.peekLast();if(viewMessage.consoleMessage().type===WebInspector.ConsoleMessage.MessageType.EndGroup){if(lastMessage&&!this._currentGroup.messagesHidden())
lastMessage.incrementCloseGroupDecorationCount();this._currentGroup=this._currentGroup.parentGroup();return;}
if(!this._currentGroup.messagesHidden()){var originatingMessage=viewMessage.consoleMessage().originatingMessage();if(lastMessage&&originatingMessage&&lastMessage.consoleMessage()===originatingMessage)
lastMessage.toMessageElement().classList.add("console-adjacent-user-command-result");this._visibleViewMessages.push(viewMessage);this._searchMessage(this._visibleViewMessages.length-1);}
if(viewMessage.consoleMessage().isGroupStartMessage())
this._currentGroup=new WebInspector.ConsoleGroup(this._currentGroup,viewMessage);this._messageAppendedForTests();},_messageAppendedForTests:function()
{},_createViewMessage:function(message)
{var nestingLevel=this._currentGroup.nestingLevel();switch(message.type){case WebInspector.ConsoleMessage.MessageType.Command:return new WebInspector.ConsoleCommand(message,this._linkifier,nestingLevel);case WebInspector.ConsoleMessage.MessageType.Result:return new WebInspector.ConsoleCommandResult(message,this._linkifier,nestingLevel);case WebInspector.ConsoleMessage.MessageType.StartGroupCollapsed:case WebInspector.ConsoleMessage.MessageType.StartGroup:return new WebInspector.ConsoleGroupViewMessage(message,this._linkifier,nestingLevel);default:return new WebInspector.ConsoleViewMessage(message,this._linkifier,nestingLevel);}},_consoleCleared:function()
{this._currentMatchRangeIndex=-1;this._consoleMessages=[];this._updateMessageList();this._hidePromptSuggestBox();this._linkifier.reset();},_handleContextMenuEvent:function(event)
{if(event.target.enclosingNodeOrSelfWithNodeName("a"))
return;var contextMenu=new WebInspector.ContextMenu(event);if(event.target.isSelfOrDescendant(this._promptElement)){contextMenu.show()
return;}
function monitoringXHRItemAction()
{WebInspector.moduleSetting("monitoringXHREnabled").set(!WebInspector.moduleSetting("monitoringXHREnabled").get());}
contextMenu.appendCheckboxItem(WebInspector.UIString("Log XMLHttpRequests"),monitoringXHRItemAction,WebInspector.moduleSetting("monitoringXHREnabled").get());var sourceElement=event.target.enclosingNodeOrSelfWithClass("console-message-wrapper");var consoleMessage=sourceElement?sourceElement.message.consoleMessage():null;var filterSubMenu=contextMenu.appendSubMenuItem(WebInspector.UIString("Filter"));if(consoleMessage&&consoleMessage.url){var menuTitle=WebInspector.UIString.capitalize("Hide ^messages from %s",new WebInspector.ParsedURL(consoleMessage.url).displayName);filterSubMenu.appendItem(menuTitle,this._filter.addMessageURLFilter.bind(this._filter,consoleMessage.url));}
filterSubMenu.appendSeparator();var unhideAll=filterSubMenu.appendItem(WebInspector.UIString.capitalize("Unhide ^all"),this._filter.removeMessageURLFilter.bind(this._filter));filterSubMenu.appendSeparator();var hasFilters=false;for(var url in this._filter.messageURLFilters){filterSubMenu.appendCheckboxItem(String.sprintf("%s (%d)",new WebInspector.ParsedURL(url).displayName,this._urlToMessageCount[url]),this._filter.removeMessageURLFilter.bind(this._filter,url),true);hasFilters=true;}
filterSubMenu.setEnabled(hasFilters||(consoleMessage&&consoleMessage.url));unhideAll.setEnabled(hasFilters);contextMenu.appendSeparator();contextMenu.appendAction("console.clear");contextMenu.appendItem(WebInspector.UIString("Save as..."),this._saveConsole.bind(this));var request=consoleMessage?consoleMessage.request:null;if(request&&request.resourceType()===WebInspector.resourceTypes.XHR){contextMenu.appendSeparator();contextMenu.appendItem(WebInspector.UIString("Replay XHR"),request.replayXHR.bind(request));}
contextMenu.show();},_saveConsole:function()
{var filename=String.sprintf("%s-%d.log",WebInspector.targetManager.inspectedPageDomain(),Date.now());var stream=new WebInspector.FileOutputStream();var progressIndicator=new WebInspector.ProgressIndicator();progressIndicator.setTitle(WebInspector.UIString("Writing file…"));progressIndicator.setTotalWork(this.itemCount());var chunkSize=350;var messageIndex=0;stream.open(filename,openCallback.bind(this));function openCallback(accepted)
{if(!accepted)
return;this._progressToolbarItem.element.appendChild(progressIndicator.element);writeNextChunk.call(this,stream);}
function writeNextChunk(stream,error)
{if(messageIndex>=this.itemCount()||error){stream.close();progressIndicator.done();return;}
var lines=[];for(var i=0;i<chunkSize&&i+messageIndex<this.itemCount();++i){var message=this.itemElement(messageIndex+i);lines.push(message.searchableElement().deepTextContent());}
messageIndex+=i;stream.write(lines.join("\n")+"\n",writeNextChunk.bind(this));progressIndicator.setWorked(messageIndex);}},_tryToCollapseMessages:function(lastMessage,viewMessage)
{if(!WebInspector.moduleSetting("consoleTimestampsEnabled").get()&&viewMessage&&!lastMessage.consoleMessage().isGroupMessage()&&lastMessage.consoleMessage().isEqual(viewMessage.consoleMessage())){viewMessage.incrementRepeatCount();return true;}
return false;},_updateMessageList:function()
{this._topGroup=WebInspector.ConsoleGroup.createTopGroup();this._currentGroup=this._topGroup;this._regexMatchRanges=[];this._hiddenByFilterCount=0;for(var i=0;i<this._visibleViewMessages.length;++i){this._visibleViewMessages[i].resetCloseGroupDecorationCount();this._visibleViewMessages[i].resetIncrementRepeatCount();}
this._visibleViewMessages=[];for(var i=0;i<this._consoleMessages.length;++i)
this._appendMessageToEnd(this._consoleMessages[i]);this._updateFilterStatus();this._searchableView.updateSearchMatchesCount(this._regexMatchRanges.length);this._viewport.invalidate();},_monitoringXHREnabledSettingChanged:function(event)
{var enabled=(event.data);WebInspector.targetManager.targets().forEach(function(target){target.networkAgent().setMonitoringXHREnabled(enabled);});},_messagesClicked:function(event)
{var targetElement=event.deepElementFromPoint();if(!this._prompt.isCaretInsidePrompt()&&(!targetElement||targetElement.isComponentSelectionCollapsed()))
this._prompt.moveCaretToEndOfPrompt();var groupMessage=event.target.enclosingNodeOrSelfWithClass("console-group-title");if(!groupMessage)
return;var consoleGroupViewMessage=groupMessage.parentElement.message;consoleGroupViewMessage.setCollapsed(!consoleGroupViewMessage.collapsed());this._updateMessageList();},_registerShortcuts:function()
{this._shortcuts={};var shortcut=WebInspector.KeyboardShortcut;var section=WebInspector.shortcutsScreen.section(WebInspector.UIString("Console"));var shortcutL=shortcut.makeDescriptor("l",WebInspector.KeyboardShortcut.Modifiers.Ctrl);var keys=[shortcutL];if(WebInspector.isMac()){var shortcutK=shortcut.makeDescriptor("k",WebInspector.KeyboardShortcut.Modifiers.Meta);keys.unshift(shortcutK);}
section.addAlternateKeys(keys,WebInspector.UIString("Clear console"));section.addKey(shortcut.makeDescriptor(shortcut.Keys.Tab),WebInspector.UIString("Autocomplete common prefix"));section.addKey(shortcut.makeDescriptor(shortcut.Keys.Right),WebInspector.UIString("Accept suggestion"));var shortcutU=shortcut.makeDescriptor("u",WebInspector.KeyboardShortcut.Modifiers.Ctrl);this._shortcuts[shortcutU.key]=this._clearPromptBackwards.bind(this);section.addAlternateKeys([shortcutU],WebInspector.UIString("Clear console prompt"));keys=[shortcut.makeDescriptor(shortcut.Keys.Down),shortcut.makeDescriptor(shortcut.Keys.Up)];section.addRelatedKeys(keys,WebInspector.UIString("Next/previous line"));if(WebInspector.isMac()){keys=[shortcut.makeDescriptor("N",shortcut.Modifiers.Alt),shortcut.makeDescriptor("P",shortcut.Modifiers.Alt)];section.addRelatedKeys(keys,WebInspector.UIString("Next/previous command"));}
section.addKey(shortcut.makeDescriptor(shortcut.Keys.Enter),WebInspector.UIString("Execute command"));},_clearPromptBackwards:function()
{this._prompt.setText("");},_promptKeyDown:function(event)
{if(isEnterKey(event)){this._enterKeyPressed(event);return;}
var shortcut=WebInspector.KeyboardShortcut.makeKeyFromEvent(event);var handler=this._shortcuts[shortcut];if(handler){handler();event.preventDefault();}},_enterKeyPressed:function(event)
{if(event.altKey||event.ctrlKey||event.shiftKey)
return;event.consume(true);this._prompt.clearAutoComplete(true);var str=this._prompt.text();if(!str.length)
return;this._appendCommand(str,true);},_printResult:function(result,wasThrown,originatingConsoleMessage,exceptionDetails)
{if(!result)
return;var level=wasThrown?WebInspector.ConsoleMessage.MessageLevel.Error:WebInspector.ConsoleMessage.MessageLevel.Log;var message;if(!wasThrown)
message=new WebInspector.ConsoleMessage(result.target(),WebInspector.ConsoleMessage.MessageSource.JS,level,"",WebInspector.ConsoleMessage.MessageType.Result,undefined,undefined,undefined,undefined,[result]);else
message=new WebInspector.ConsoleMessage(result.target(),WebInspector.ConsoleMessage.MessageSource.JS,level,exceptionDetails.text,WebInspector.ConsoleMessage.MessageType.Result,exceptionDetails.url,exceptionDetails.line,exceptionDetails.column,undefined,[WebInspector.UIString("Uncaught"),result],exceptionDetails.stackTrace,undefined,undefined,undefined,exceptionDetails.scriptId);message.setOriginatingMessage(originatingConsoleMessage);result.target().consoleModel.addMessage(message);},_appendCommand:function(text,useCommandLineAPI)
{this._prompt.setText("");var currentExecutionContext=WebInspector.context.flavor(WebInspector.ExecutionContext);if(currentExecutionContext){WebInspector.ConsoleModel.evaluateCommandInConsole(currentExecutionContext,text,useCommandLineAPI);if(WebInspector.inspectorView.currentPanel()&&WebInspector.inspectorView.currentPanel().name==="console")
WebInspector.userMetrics.actionTaken(WebInspector.UserMetrics.Action.CommandEvaluatedInConsolePanel);}},_commandEvaluated:function(event)
{var data=(event.data);this._prompt.pushHistoryItem(data.text);this._consoleHistorySetting.set(this._prompt.historyData().slice(-WebInspector.ConsoleView.persistedHistorySize));this._printResult(data.result,data.wasThrown,data.commandMessage,data.exceptionDetails);},elementsToRestoreScrollPositionsFor:function()
{return[this._messagesElement];},searchCanceled:function()
{this._cleanupAfterSearch();for(var i=0;i<this._visibleViewMessages.length;++i){var message=this._visibleViewMessages[i];message.setSearchRegex(null);}
this._currentMatchRangeIndex=-1;this._regexMatchRanges=[];delete this._searchRegex;this._viewport.refresh();},performSearch:function(searchConfig,shouldJump,jumpBackwards)
{this.searchCanceled();this._searchableView.updateSearchMatchesCount(0);this._searchRegex=searchConfig.toSearchRegex(true);this._regexMatchRanges=[];this._currentMatchRangeIndex=-1;if(shouldJump)
this._searchShouldJumpBackwards=!!jumpBackwards;this._searchProgressIndicator=new WebInspector.ProgressIndicator();this._searchProgressIndicator.setTitle(WebInspector.UIString("Searching…"));this._searchProgressIndicator.setTotalWork(this._visibleViewMessages.length);this._progressToolbarItem.element.appendChild(this._searchProgressIndicator.element);this._innerSearch(0);},_cleanupAfterSearch:function()
{delete this._searchShouldJumpBackwards;if(this._innerSearchTimeoutId){clearTimeout(this._innerSearchTimeoutId);delete this._innerSearchTimeoutId;}
if(this._searchProgressIndicator){this._searchProgressIndicator.done();delete this._searchProgressIndicator;}},_searchFinishedForTests:function()
{},_innerSearch:function(index)
{delete this._innerSearchTimeoutId;if(this._searchProgressIndicator.isCanceled()){this._cleanupAfterSearch();return;}
var startTime=Date.now();for(;index<this._visibleViewMessages.length&&Date.now()-startTime<100;++index)
this._searchMessage(index);this._searchableView.updateSearchMatchesCount(this._regexMatchRanges.length);if(typeof this._searchShouldJumpBackwards!=="undefined"&&this._regexMatchRanges.length){this._jumpToMatch(this._searchShouldJumpBackwards?-1:0);delete this._searchShouldJumpBackwards;}
if(index===this._visibleViewMessages.length){this._cleanupAfterSearch();setTimeout(this._searchFinishedForTests.bind(this),0);return;}
this._innerSearchTimeoutId=setTimeout(this._innerSearch.bind(this,index),100);this._searchProgressIndicator.setWorked(index);},_searchMessage:function(index)
{var message=this._visibleViewMessages[index];message.setSearchRegex(this._searchRegex);for(var i=0;i<message.searchCount();++i){this._regexMatchRanges.push({messageIndex:index,matchIndex:i});}},jumpToNextSearchResult:function()
{this._jumpToMatch(this._currentMatchRangeIndex+1);},jumpToPreviousSearchResult:function()
{this._jumpToMatch(this._currentMatchRangeIndex-1);},supportsCaseSensitiveSearch:function()
{return true;},supportsRegexSearch:function()
{return true;},_jumpToMatch:function(index)
{if(!this._regexMatchRanges.length)
return;var currentSearchResultClassName="current-search-result";var matchRange;if(this._currentMatchRangeIndex>=0){matchRange=this._regexMatchRanges[this._currentMatchRangeIndex];var message=this._visibleViewMessages[matchRange.messageIndex];message.searchHighlightNode(matchRange.matchIndex).classList.remove(currentSearchResultClassName);}
index=mod(index,this._regexMatchRanges.length);this._currentMatchRangeIndex=index;this._searchableView.updateCurrentMatchIndex(index);matchRange=this._regexMatchRanges[index];var message=this._visibleViewMessages[matchRange.messageIndex];var highlightNode=message.searchHighlightNode(matchRange.matchIndex);highlightNode.classList.add(currentSearchResultClassName);this._viewport.scrollItemIntoView(matchRange.messageIndex);highlightNode.scrollIntoViewIfNeeded();},__proto__:WebInspector.VBox.prototype}
WebInspector.ConsoleViewFilter=function(view)
{this._messageURLFiltersSetting=WebInspector.settings.createSetting("messageURLFilters",{});this._messageLevelFiltersSetting=WebInspector.settings.createSetting("messageLevelFilters",{});this._view=view;this._messageURLFilters=this._messageURLFiltersSetting.get();this._filterChanged=this.dispatchEventToListeners.bind(this,WebInspector.ConsoleViewFilter.Events.FilterChanged);};WebInspector.ConsoleViewFilter.Events={FilterChanged:"FilterChanged"};WebInspector.ConsoleViewFilter.prototype={addFilters:function(filterBar)
{this._textFilterUI=new WebInspector.TextFilterUI(true);this._textFilterUI.addEventListener(WebInspector.FilterUI.Events.FilterChanged,this._textFilterChanged,this);filterBar.addFilter(this._textFilterUI);this._hideNetworkMessagesCheckbox=new WebInspector.CheckboxFilterUI("hide-network-messages",WebInspector.UIString("Hide network messages"),true,WebInspector.moduleSetting("hideNetworkMessages"));this._hideNetworkMessagesCheckbox.addEventListener(WebInspector.FilterUI.Events.FilterChanged,this._filterChanged.bind(this),this);filterBar.addFilter(this._hideNetworkMessagesCheckbox);var levels=[{name:WebInspector.ConsoleMessage.MessageLevel.Error,label:WebInspector.UIString("Errors")},{name:WebInspector.ConsoleMessage.MessageLevel.Warning,label:WebInspector.UIString("Warnings")},{name:WebInspector.ConsoleMessage.MessageLevel.Info,label:WebInspector.UIString("Info")},{name:WebInspector.ConsoleMessage.MessageLevel.Log,label:WebInspector.UIString("Logs")},{name:WebInspector.ConsoleMessage.MessageLevel.Debug,label:WebInspector.UIString("Debug")},{name:WebInspector.ConsoleMessage.MessageLevel.RevokedError,label:WebInspector.UIString("Handled")}];this._levelFilterUI=new WebInspector.NamedBitSetFilterUI(levels,this._messageLevelFiltersSetting);this._levelFilterUI.addEventListener(WebInspector.FilterUI.Events.FilterChanged,this._filterChanged,this);filterBar.addFilter(this._levelFilterUI);},_textFilterChanged:function(event)
{this._filterRegex=this._textFilterUI.regex();this._filterChanged();},addMessageURLFilter:function(url)
{this._messageURLFilters[url]=true;this._messageURLFiltersSetting.set(this._messageURLFilters);this._filterChanged();},removeMessageURLFilter:function(url)
{if(!url)
this._messageURLFilters={};else
delete this._messageURLFilters[url];this._messageURLFiltersSetting.set(this._messageURLFilters);this._filterChanged();},get messageURLFilters()
{return this._messageURLFilters;},shouldBeVisible:function(viewMessage)
{var message=viewMessage.consoleMessage();var executionContext=WebInspector.context.flavor(WebInspector.ExecutionContext);if(!message.target())
return true;if(!this._view._showAllMessagesCheckbox.checked()&&executionContext){if(message.target()!==executionContext.target())
return false;if(message.executionContextId&&message.executionContextId!==executionContext.id){return false;}}
if(WebInspector.moduleSetting("hideNetworkMessages").get()&&viewMessage.consoleMessage().source===WebInspector.ConsoleMessage.MessageSource.Network)
return false;if(viewMessage.consoleMessage().isGroupMessage())
return true;if(message.type===WebInspector.ConsoleMessage.MessageType.Result||message.type===WebInspector.ConsoleMessage.MessageType.Command)
return true;if(message.url&&this._messageURLFilters[message.url])
return false;if(message.level&&!this._levelFilterUI.accept(message.level))
return false;if(this._filterRegex){this._filterRegex.lastIndex=0;if(!viewMessage.matchesFilterRegex(this._filterRegex))
return false;}
return true;},reset:function()
{this._messageURLFilters={};this._messageURLFiltersSetting.set(this._messageURLFilters);this._messageLevelFiltersSetting.set({});this._view._showAllMessagesCheckbox.inputElement.checked=true;WebInspector.moduleSetting("hideNetworkMessages").set(false);this._textFilterUI.setValue("");this._filterChanged();},__proto__:WebInspector.Object.prototype};WebInspector.ConsoleCommand=function(message,linkifier,nestingLevel)
{WebInspector.ConsoleViewMessage.call(this,message,linkifier,nestingLevel);}
WebInspector.ConsoleCommand.prototype={searchableElement:function()
{return this.contentElement();},contentElement:function()
{if(!this._element){this._element=createElementWithClass("div","console-user-command");this._element.message=this;this._formattedCommand=createElementWithClass("span","console-message-text source-code");this._formattedCommand.textContent=this.text;this._element.appendChild(this._formattedCommand);var javascriptSyntaxHighlighter=new WebInspector.DOMSyntaxHighlighter("text/javascript",true);javascriptSyntaxHighlighter.syntaxHighlightNode(this._formattedCommand).then(this._updateSearch.bind(this))}
return this._element;},_updateSearch:function()
{this.setSearchRegex(this.searchRegex());},__proto__:WebInspector.ConsoleViewMessage.prototype}
WebInspector.ConsoleCommandResult=function(message,linkifier,nestingLevel)
{WebInspector.ConsoleViewMessage.call(this,message,linkifier,nestingLevel);}
WebInspector.ConsoleCommandResult.prototype={useArrayPreviewInFormatter:function(array)
{return false;},contentElement:function()
{var element=WebInspector.ConsoleViewMessage.prototype.contentElement.call(this);element.classList.add("console-user-command-result");this.updateTimestamp(false);return element;},__proto__:WebInspector.ConsoleViewMessage.prototype}
WebInspector.ConsoleGroup=function(parentGroup,groupMessage)
{this._parentGroup=parentGroup;this._nestingLevel=parentGroup?parentGroup.nestingLevel()+1:0;this._messagesHidden=groupMessage&&groupMessage.collapsed()||this._parentGroup&&this._parentGroup.messagesHidden();}
WebInspector.ConsoleGroup.createTopGroup=function()
{return new WebInspector.ConsoleGroup(null,null);}
WebInspector.ConsoleGroup.prototype={messagesHidden:function()
{return this._messagesHidden;},nestingLevel:function()
{return this._nestingLevel;},parentGroup:function()
{return this._parentGroup||this;},}
WebInspector.ConsoleView.ActionDelegate=function()
{}
WebInspector.ConsoleView.ActionDelegate.prototype={handleAction:function(context,actionId)
{switch(actionId){case"console.show":WebInspector.console.show();return true;case"console.clear":WebInspector.ConsoleModel.clearConsole();return true;}
return false;}}
WebInspector.ConsoleView.RegexMatchRange;;WebInspector.ConsolePanel=function()
{WebInspector.Panel.call(this,"console");this._view=WebInspector.ConsolePanel._view();}
WebInspector.ConsolePanel._view=function()
{if(!WebInspector.ConsolePanel._consoleView)
WebInspector.ConsolePanel._consoleView=new WebInspector.ConsoleView();return WebInspector.ConsolePanel._consoleView;}
WebInspector.ConsolePanel.prototype={defaultFocusedElement:function()
{return this._view.defaultFocusedElement();},wasShown:function()
{WebInspector.Panel.prototype.wasShown.call(this);this._view.show(this.element);},willHide:function()
{WebInspector.Panel.prototype.willHide.call(this);if(WebInspector.ConsolePanel.WrapperView._instance)
WebInspector.ConsolePanel.WrapperView._instance._showViewInWrapper();},searchableView:function()
{return WebInspector.ConsolePanel._view().searchableView();},__proto__:WebInspector.Panel.prototype}
WebInspector.ConsolePanel.WrapperView=function()
{WebInspector.VBox.call(this);this.element.classList.add("console-view-wrapper");WebInspector.ConsolePanel.WrapperView._instance=this;this._view=WebInspector.ConsolePanel._view();}
WebInspector.ConsolePanel.WrapperView.prototype={wasShown:function()
{if(!WebInspector.inspectorView.currentPanel()||WebInspector.inspectorView.currentPanel().name!=="console")
this._showViewInWrapper();},defaultFocusedElement:function()
{return this._view.defaultFocusedElement();},focus:function()
{this._view.focus();},_showViewInWrapper:function()
{this._view.show(this.element);},__proto__:WebInspector.VBox.prototype}
WebInspector.ConsolePanel.ConsoleRevealer=function()
{}
WebInspector.ConsolePanel.ConsoleRevealer.prototype={reveal:function(object)
{var consoleView=WebInspector.ConsolePanel._view();if(consoleView.isShowing()){consoleView.focus();return Promise.resolve();}
WebInspector.inspectorView.showViewInDrawer("console");return Promise.resolve();}}
WebInspector.ConsolePanel.show=function()
{WebInspector.inspectorView.setCurrentPanel(WebInspector.ConsolePanel._instance());}
WebInspector.ConsolePanel._instance=function()
{if(!WebInspector.ConsolePanel._instanceObject)
WebInspector.ConsolePanel._instanceObject=new WebInspector.ConsolePanel();return WebInspector.ConsolePanel._instanceObject;}
WebInspector.ConsolePanelFactory=function()
{}
WebInspector.ConsolePanelFactory.prototype={createPanel:function()
{return WebInspector.ConsolePanel._instance();}};Runtime.cachedResources["console/consoleView.css"]="/*\n * Copyright (C) 2006, 2007, 2008 Apple Inc.  All rights reserved.\n * Copyright (C) 2009 Anthony Ricaud <rik@webkit.org>\n *\n * Redistribution and use in source and binary forms, with or without\n * modification, are permitted provided that the following conditions\n * are met:\n *\n * 1.  Redistributions of source code must retain the above copyright\n *     notice, this list of conditions and the following disclaimer.\n * 2.  Redistributions in binary form must reproduce the above copyright\n *     notice, this list of conditions and the following disclaimer in the\n *     documentation and/or other materials provided with the distribution.\n * 3.  Neither the name of Apple Computer, Inc. (\"Apple\") nor the names of\n *     its contributors may be used to endorse or promote products derived\n *     from this software without specific prior written permission.\n *\n * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS \"AS IS\" AND ANY\n * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED\n * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE\n * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY\n * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES\n * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;\n * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND\n * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT\n * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF\n * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n */\n\n.console-view {\n    background-color: white;\n    overflow: hidden;\n}\n\n.console-view > .toolbar {\n    border-bottom: 1px solid #dadada;\n}\n\n.console-view-wrapper {\n    background-color: #eee;\n}\n\n.console-view-fix-select-all {\n    height: 0;\n    overflow: hidden;\n}\n\n#console-messages {\n    flex: 1 1;\n    padding: 2px 0;\n    overflow-y: auto;\n    word-wrap: break-word;\n    -webkit-user-select: text;\n    transform: translateZ(0);\n}\n\n#console-prompt {\n    clear: right;\n    position: relative;\n    padding: 3px 22px 1px 0;\n    margin-left: 24px;\n    min-height: 18px;  /* Sync with ConsoleViewMessage.js */\n    white-space: pre-wrap;\n    -webkit-user-modify: read-write-plaintext-only;\n}\n\n#console-prompt::before {\n    background-position: -192px -96px;\n}\n\n.console-log-level .console-user-command-result::before {\n    background-position: -202px -96px;\n}\n\n.console-message,\n.console-user-command {\n    clear: right;\n    position: relative;\n    padding: 3px 22px 1px 0;\n    margin-left: 24px;\n    min-height: 18px;  /* Sync with ConsoleViewMessage.js */\n    flex: auto;\n    display: flex;\n}\n\n.console-message > * {\n    flex: auto;\n}\n\n.console-adjacent-user-command-result + .console-user-command-result.console-log-level::before {\n    background-image: none;\n}\n\n.console-timestamp {\n    color: gray;\n    -webkit-user-select: none;\n}\n\n.console-message::before,\n.console-user-command::before,\n#console-prompt::before,\n.console-group-title::before {\n    position: absolute;\n    display: block;\n    content: \"\";\n    left: -17px;\n    top: 9px;\n    width: 10px;\n    height: 10px;\n    margin-top: -4px;\n    -webkit-user-select: none;\n    background-image: url(Images/toolbarButtonGlyphs.png);\n    background-size: 352px 168px;\n}\n\n@media (-webkit-min-device-pixel-ratio: 1.5) {\n.console-message::before,\n.console-user-command::before,\n#console-prompt::before,\n.console-group-title::before {\n    background-image: url(Images/toolbarButtonGlyphs_2x.png);\n}\n} /* media */\n\n.console-message > .outline-disclosure li.parent::before {\n    top: 0;\n}\n\n.bubble-repeat-count {\n    display: inline-block;\n    height: 14px;\n    background-color: rgb(128, 151, 189);\n    vertical-align: middle;\n    white-space: nowrap;\n    padding: 1px 4px;\n    text-align: left;\n    font-size: 11px;\n    line-height: normal;\n    font-weight: bold;\n    text-shadow: none;\n    color: white;\n    margin-top: -1px;\n    border-radius: 7px;\n    flex: none;\n}\n\n.console-message .bubble-repeat-count {\n    margin-right: 4px;\n    margin-left: -18px;\n}\n\n.console-error-level .repeated-message::before,\n.console-revokedError-level .repeated-message::before,\n.console-warning-level .repeated-message::before,\n.console-debug-level .repeated-message::before,\n.console-info-level .repeated-message::before {\n    visibility: hidden;\n}\n\n.repeated-message .outline-disclosure,\n.repeated-message > .console-message-text {\n    flex: 1;\n}\n\n.console-warning-level .repeated-message,\n.console-error-level .repeated-message,\n.console-revokedError-level .repeated-message,\n.console-log-level .repeated-message,\n.console-debug-level .repeated-message,\n.console-info-level .repeated-message {\n    display: flex;\n}\n\n.console-info {\n    color: rgb(128, 128, 128);\n    font-style: italic;\n}\n\n.console-group .console-group > .console-group-messages {\n    margin-left: 16px;\n}\n\n.console-group-title {\n    font-weight: bold;\n}\n\n.console-group-title::before {\n    -webkit-user-select: none;\n    -webkit-mask-image: url(Images/toolbarButtonGlyphs.png);\n    -webkit-mask-size: 352px 168px;\n    float: left;\n    width: 8px;\n    content: \"a\";\n    color: transparent;\n    text-shadow: none;\n    margin-left: 3px;\n    margin-top: -7px;\n}\n\n@media (-webkit-min-device-pixel-ratio: 1.5) {\n.console-group-title::before {\n    -webkit-mask-image: url(Images/toolbarButtonGlyphs_2x.png);\n}\n} /* media */\n\n.console-group .console-group-title::before {\n    -webkit-mask-position: -20px -96px;\n    background-color: rgb(110, 110, 110);\n}\n\n.console-message-wrapper.collapsed .console-group-title::before {\n    -webkit-mask-position: -4px -96px;\n}\n\n.console-group {\n    position: relative;\n}\n\n.console-message-wrapper {\n    display: flex;\n    border-bottom: 1px solid rgb(240, 240, 240);\n}\n\n.console-message-wrapper.console-adjacent-user-command-result {\n    border-bottom: none;\n}\n\n.console-message-wrapper.console-error-level {\n    border-top: 1px solid rgb(255, 216, 216);\n    border-bottom: 1px solid rgb(255, 216, 216);\n    margin-top: -1px;\n}\n\n.console-message-wrapper.console-warning-level {\n    border-top: 1px solid hsl(50, 100%, 88%);\n    border-bottom: 1px solid hsl(50, 100%, 88%);\n    margin-top: -1px;\n}\n\n.console-message-wrapper .nesting-level-marker {\n    width: 14px;\n    flex: 0 0 auto;\n    border-right: 1px solid #A3A3A3;\n    position: relative;\n    margin-bottom: -1px;\n}\n\n.console-message-wrapper:last-child .nesting-level-marker::before,\n.console-message-wrapper .nesting-level-marker.group-closed::before {\n    content: \"\";\n}\n\n.console-message-wrapper .nesting-level-marker::before {\n    border-bottom: 1px solid #A3A3A3;\n    position: absolute;\n    top: 0;\n    left: 0;\n    margin-left: 100%;\n    width: 3px;\n    height: 100%;\n    box-sizing: border-box;\n}\n\n.console-error-level {\n    background-color: rgb(255, 239, 239);\n}\n\n.console-warning-level {\n    background-color: hsl(50, 100%, 95%);\n}\n\n.console-warning-level .console-message-text {\n    color: hsl(39, 100%, 18%);\n}\n\n.console-error-level .console-message-text,\n.console-error-level .console-view-object-properties-section {\n    color: red !important;\n}\n\n.console-debug-level .console-message-text {\n    color: blue;\n}\n\n.console-message.console-warning-level {\n    background-color: rgb(255, 250, 224);\n}\n\n.console-error-level .console-message::before,\n.console-revokedError-level .console-message::before,\n.console-warning-level .console-message::before,\n.console-debug-level .console-message::before,\n.console-info-level .console-message::before {\n    background-image: url(Images/toolbarButtonGlyphs.png);\n    background-size: 352px 168px;\n    width: 10px;\n    height: 10px;\n}\n\n@media (-webkit-min-device-pixel-ratio: 1.5) {\n.console-error-level .console-message::before,\n.console-revokedError-level .console-message::before,\n.console-warning-level .console-message::before,\n.console-debug-level .console-message::before,\n.console-info-level .console-message::before {\n    background-image: url(Images/toolbarButtonGlyphs_2x.png);\n}\n} /* media */\n\n.console-warning-level .console-message::before {\n    background-position: -202px -107px;\n}\n\n.console-error-level .console-message::before {\n    background-position: -213px -96px;\n}\n\n.console-revokedError-level .console-message::before {\n    background-position: -245px -107px;\n}\n\n.console-info-level .console-message::before {\n    background-position: -213px -107px;\n}\n\n.console-user-command .console-message {\n    margin-left: -24px;\n    padding-right: 0;\n    border-bottom: none;\n}\n\n.console-user-command::before {\n    background-position: -192px -107px;\n}\n\n#console-messages .link {\n    text-decoration: underline;\n}\n\n#console-messages .link,\n#console-messages a {\n    color: rgb(33%, 33%, 33%);\n    cursor: pointer;\n}\n\n#console-messages .link:hover,\n#console-messages a:hover {\n    color: rgb(15%, 15%, 15%);\n}\n\n.console-group-messages .section {\n    margin: 0 0 0 12px !important;\n}\n\n.console-group-messages .section > .header {\n    padding: 0 8px 0 0;\n    background-image: none;\n    border: none;\n    min-height: 0;\n}\n\n.console-group-messages .section > .header::before {\n    margin-left: -12px;\n}\n\n.console-group-messages .section > .header .title {\n    color: #222;\n    font-weight: normal;\n    line-height: 13px;\n}\n\n.console-group-messages .section .properties li .info {\n    padding-top: 0;\n    padding-bottom: 0;\n    color: rgb(60%, 60%, 60%);\n}\n\n.console-object-preview {\n    font-style: italic;\n    white-space: normal;\n    word-wrap: break-word;\n}\n\n.console-object-preview .name {\n    /* Follows .section .properties .name, .event-properties .name */\n    color: rgb(136, 19, 145);\n    flex-shrink: 0;\n}\n\n.console-message-text {\n    white-space: pre-wrap;\n}\n\n.console-message-formatted-table {\n    clear: both;\n}\n\n.console-message-url {\n    float: right;\n    text-align: right;\n    max-width: 100%;\n    margin-left: 4px;\n}\n\n.console-message-nowrap-below,\n.console-message-nowrap-below div,\n.console-message-nowrap-below span {\n    white-space: nowrap !important;\n}\n\n.object-info-state-note {\n    display: inline-block;\n    width: 11px;\n    height: 11px;\n    background-color: rgb(179, 203, 247);\n    color: white;\n    text-align: center;\n    border-radius: 3px;\n    line-height: 13px;\n    margin: 0 6px;\n    font-size: 9px;\n}\n\n.object-info-state-note::before {\n    content: \"i\";\n}\n\n.console-view-object-properties-section:not(.expanded) .object-info-state-note {\n    display: none;\n}\n\n.object-info-state-note {\n    display: inline-block;\n    width: 11px;\n    height: 11px;\n    background-color: rgb(179, 203, 247);\n    color: white;\n    text-align: center;\n    border-radius: 3px;\n    line-height: 13px;\n    margin: 0 6px;\n    font-size: 9px;\n}\n\n.object-info-state-note::before {\n    content: \"i\";\n}\n\n.console-view-object-properties-section:not(.expanded) .object-info-state-note {\n    display: none;\n}\n\n/*# sourceURL=console/consoleView.css */";