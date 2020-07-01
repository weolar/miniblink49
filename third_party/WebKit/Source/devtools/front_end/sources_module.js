WebInspector.UIList=function()
{WebInspector.VBox.call(this,true);this.registerRequiredCSS("sources/uiList.css");this._items=[];}
WebInspector.UIList._Key=Symbol("ownerList");WebInspector.UIList.prototype={addItem:function(item,beforeItem)
{item[WebInspector.UIList._Key]=this;var beforeElement=beforeItem?beforeItem.element:null;this.contentElement.insertBefore(item.element,beforeElement);var index=beforeItem?this._items.indexOf(beforeItem):this._items.length;console.assert(index>=0,"Anchor item not found in UIList");this._items.splice(index,0,item);},removeItem:function(item)
{var index=this._items.indexOf(item);console.assert(index>=0);this._items.splice(index,1);item.element.remove();},clear:function()
{this.contentElement.removeChildren();this._items=[];},__proto__:WebInspector.VBox.prototype}
WebInspector.UIList.Item=function(title,subtitle,isLabel)
{this.element=createElementWithClass("div","list-item");if(isLabel)
this.element.classList.add("label");this.subtitleElement=this.element.createChild("div","subtitle");this.titleElement=this.element.createChild("div","title");this._hidden=false;this._isLabel=!!isLabel;this.setTitle(title);this.setSubtitle(subtitle);this.setSelected(false);}
WebInspector.UIList.Item.prototype={nextSibling:function()
{var list=this[WebInspector.UIList._Key];var index=list._items.indexOf(this);console.assert(index>=0);return list._items[index+1]||null;},title:function()
{return this._title;},setTitle:function(x)
{if(this._title===x)
return;this._title=x;this.titleElement.textContent=x;},subtitle:function()
{return this._subtitle;},setSubtitle:function(x)
{if(this._subtitle===x)
return;this._subtitle=x;this.subtitleElement.textContent=x;},isSelected:function()
{return this._selected;},setSelected:function(x)
{if(x)
this.select();else
this.deselect();},select:function()
{if(this._selected)
return;this._selected=true;this.element.classList.add("selected");},deselect:function()
{if(!this._selected)
return;this._selected=false;this.element.classList.remove("selected");},toggleSelected:function()
{this.setSelected(!this.isSelected());},isHidden:function()
{return this._hidden;},setHidden:function(x)
{if(this._hidden===x)
return;this._hidden=x;this.element.classList.toggle("hidden",x);},isLabel:function()
{return this._isLabel;},setDimmed:function(x)
{this.element.classList.toggle("dimmed",x);},discard:function()
{}};WebInspector.AddSourceMapURLDialog=function(callback)
{WebInspector.HBox.call(this,true);this.registerRequiredCSS("sources/addSourceMapURLDialog.css");this.contentElement.createChild("label").textContent=WebInspector.UIString("Source map URL: ");this._input=this.contentElement.createChild("input");this._input.setAttribute("type","text");this._input.addEventListener("keydown",this._onKeyDown.bind(this),false);var addButton=this.contentElement.createChild("button");addButton.textContent=WebInspector.UIString("Add");addButton.addEventListener("click",this._apply.bind(this),false);this.setDefaultFocusedElement(this._input);this._callback=callback;this.contentElement.tabIndex=0;}
WebInspector.AddSourceMapURLDialog.show=function(callback)
{var dialog=new WebInspector.Dialog();var addSourceMapURLDialog=new WebInspector.AddSourceMapURLDialog(done);addSourceMapURLDialog.show(dialog.element);dialog.setWrapsContent(true);dialog.show();function done(value)
{dialog.detach();callback(value);}}
WebInspector.AddSourceMapURLDialog.prototype={_apply:function()
{this._callback(this._input.value);},_onKeyDown:function(event)
{if(event.keyCode===WebInspector.KeyboardShortcut.Keys.Enter.code){event.preventDefault();this._apply();}},__proto__:WebInspector.HBox.prototype};WebInspector.AsyncOperationsSidebarPane=function()
{WebInspector.BreakpointsSidebarPaneBase.call(this,WebInspector.UIString("Async Operation Breakpoints"));this.element.classList.add("async-operations");this._updateEmptyElement();var refreshButton=new WebInspector.ToolbarButton(WebInspector.UIString("Refresh"),"refresh-toolbar-item");refreshButton.addEventListener("click",this._refreshButtonClicked.bind(this));this.toolbar().appendToolbarItem(refreshButton);this._asyncOperationsByTarget=new Map();this._operationIdToElement=new Map();this._revealBlackboxedCallFrames=false;this._linkifier=new WebInspector.Linkifier(new WebInspector.Linkifier.DefaultFormatter(30));this._popoverHelper=new WebInspector.PopoverHelper(this.element,this._getPopoverAnchor.bind(this),this._showPopover.bind(this));this._popoverHelper.setTimeout(250,250);this.element.addEventListener("click",this._hidePopover.bind(this),true);WebInspector.targetManager.addModelListener(WebInspector.DebuggerModel,WebInspector.DebuggerModel.Events.AsyncOperationStarted,this._onAsyncOperationStarted,this);WebInspector.targetManager.addModelListener(WebInspector.DebuggerModel,WebInspector.DebuggerModel.Events.AsyncOperationCompleted,this._onAsyncOperationCompleted,this);WebInspector.targetManager.addModelListener(WebInspector.DebuggerModel,WebInspector.DebuggerModel.Events.DebuggerResumed,this._debuggerResumed,this);WebInspector.targetManager.addModelListener(WebInspector.DebuggerModel,WebInspector.DebuggerModel.Events.GlobalObjectCleared,this._debuggerReset,this);WebInspector.context.addFlavorChangeListener(WebInspector.Target,this._targetChanged,this);WebInspector.moduleSetting("skipStackFramesPattern").addChangeListener(this._refresh,this);WebInspector.moduleSetting("enableAsyncStackTraces").addChangeListener(this._asyncStackTracesStateChanged,this);WebInspector.targetManager.observeTargets(this);}
WebInspector.AsyncOperationsSidebarPane.prototype={_operationIdSymbol:Symbol("operationId"),_checkedSymbol:Symbol("checked"),targetAdded:function(target)
{},targetRemoved:function(target)
{this._asyncOperationsByTarget.delete(target);if(this._target===target){this._clear();delete this._target;}},_targetChanged:function(event)
{var target=(event.data);if(this._target===target)
return;this._target=target;this._refresh();},operationById:function(target,operationId)
{if(!target)
return null;var operationsMap=this._asyncOperationsByTarget.get(target);if(!operationsMap)
return null;return operationsMap.get(operationId)||null;},_asyncStackTracesStateChanged:function()
{var enabled=WebInspector.moduleSetting("enableAsyncStackTraces").get();if(enabled){this._target=WebInspector.context.flavor(WebInspector.Target);}else if(this._target){this._asyncOperationsByTarget.delete(this._target);delete this._target;}
this._updateEmptyElement();this._refresh();},_updateEmptyElement:function()
{var enabled=WebInspector.moduleSetting("enableAsyncStackTraces").get();if(enabled){this.emptyElement.textContent=WebInspector.UIString("No Async Operations");}else{this.emptyElement.textContent=WebInspector.UIString("Async stack traces are disabled.");this.emptyElement.createTextChild(" ");var enableLink=this.emptyElement.createChild("span","link");enableLink.textContent=WebInspector.UIString("Enable");enableLink.addEventListener("click",enableAsyncStackTraces,true);}
function enableAsyncStackTraces()
{WebInspector.moduleSetting("enableAsyncStackTraces").set(true);}},wasShown:function()
{if(!this._target&&WebInspector.moduleSetting("enableAsyncStackTraces").get()){this._target=WebInspector.context.flavor(WebInspector.Target);this._refresh();}},willHide:function()
{this._hidePopover();},onResize:function()
{this._hidePopover();},revealHiddenCallFrames:function(target)
{if(this._target!==target||this._revealBlackboxedCallFrames)
return;this._revealBlackboxedCallFrames=true;this._refresh();},highlightBreakpoint:function(operationId)
{this._breakpointHitId=operationId;var element=this._operationIdToElement.get(operationId);if(!element)
return;this.expand();element.classList.add("breakpoint-hit");},clearBreakpointHighlight:function()
{if(!this._breakpointHitId)
return;var element=this._operationIdToElement.get(this._breakpointHitId);if(element)
element.classList.remove("breakpoint-hit");delete this._breakpointHitId;},_debuggerResumed:function(event)
{var target=(event.target.target());if(this._target!==target||!this._revealBlackboxedCallFrames)
return;this._revealBlackboxedCallFrames=false;this._refresh();},_debuggerReset:function(event)
{var target=(event.target.target());this._asyncOperationsByTarget.delete(target);if(this._target===target)
this._clear();},_refreshButtonClicked:function(event)
{event.consume();this.expand();var debuggerModel=WebInspector.DebuggerModel.fromTarget(this._target);if(debuggerModel)
debuggerModel.flushAsyncOperationEvents();},_onAsyncOperationStarted:function(event)
{var target=(event.target.target());var operation=(event.data);var operationsMap=this._asyncOperationsByTarget.get(target);if(!operationsMap){operationsMap=new Map();this._asyncOperationsByTarget.set(target,operationsMap)}
operationsMap.set(operation.id,operation);if(this._target===target)
this._createAsyncOperationItem(operation);},_onAsyncOperationCompleted:function(event)
{var target=(event.target.target());var operationId=(event.data);var operationsMap=this._asyncOperationsByTarget.get(target);if(operationsMap)
operationsMap.delete(operationId);if(this._target===target){var element=this._operationIdToElement.get(operationId);if(element)
this.removeListElement(element);this._operationIdToElement.delete(operationId);if(!this._operationIdToElement.size)
this._clear();}},_refresh:function()
{this._clear();if(!this._target)
return;var operationsMap=this._asyncOperationsByTarget.get(this._target);if(!operationsMap||!operationsMap.size)
return;for(var pair of operationsMap){var operation=(pair[1]);this._createAsyncOperationItem(operation);}},_createAsyncOperationItem:function(operation)
{var element=createElementWithClass("li","async-operation");var title=operation.description||WebInspector.UIString("Async Operation");var label=createCheckboxLabel(title,operation[this._checkedSymbol]);label.checkboxElement.addEventListener("click",this._checkboxClicked.bind(this,operation.id),false);element.appendChild(label);var debuggerModel=WebInspector.DebuggerModel.fromTarget(this._target);var callFrame=WebInspector.DebuggerPresentationUtils.callFrameAnchorFromStackTrace(debuggerModel,operation.stackTrace,operation.asyncStackTrace,this._revealBlackboxedCallFrames);if(callFrame)
element.createChild("div").appendChild(this._linkifier.linkifyConsoleCallFrame(this._target,callFrame));element[this._operationIdSymbol]=operation.id;this._operationIdToElement.set(operation.id,element);this.addListElement(element,this.listElement.firstChild);if(operation.id===this._breakpointHitId){element.classList.add("breakpoint-hit");this.expand();}},_checkboxClicked:function(operationId,event)
{var operation=this.operationById(this._target,operationId);if(!operation)
return;var debuggerModel=WebInspector.DebuggerModel.fromTarget(this._target);if(!debuggerModel)
return;operation[this._checkedSymbol]=event.target.checked;if(event.target.checked)
debuggerModel.setAsyncOperationBreakpoint(operationId);else
debuggerModel.removeAsyncOperationBreakpoint(operationId);},_clear:function()
{this._hidePopover();this.reset();this._operationIdToElement.clear();this._linkifier.reset();},_hidePopover:function()
{this._popoverHelper.hidePopover();},_getPopoverAnchor:function(element,event)
{var anchor=(element.enclosingNodeOrSelfWithNodeName("a"));if(!anchor)
return undefined;var operation=this._operationForPopover(anchor);return operation?anchor:undefined;},_showPopover:function(anchor,popover)
{var operation=this._operationForPopover(anchor);if(!operation)
return;var content=WebInspector.DOMPresentationUtils.buildStackTracePreviewContents(this._target,this._linkifier,operation.stackTrace,operation.asyncStackTrace);popover.setCanShrink(true);popover.showForAnchor(content,anchor);},_operationForPopover:function(element)
{var asyncOperations=this._target&&this._asyncOperationsByTarget.get(this._target);if(!asyncOperations)
return null;var anchor=element.enclosingNodeOrSelfWithClass("async-operation");if(!anchor)
return null;var operationId=anchor[this._operationIdSymbol];var operation=operationId&&asyncOperations.get(operationId);if(!operation||!operation.stackTrace)
return null;return operation;},__proto__:WebInspector.BreakpointsSidebarPaneBase.prototype};WebInspector.CallStackSidebarPane=function()
{WebInspector.SidebarPane.call(this,WebInspector.UIString("Call Stack"));this.element.addEventListener("keydown",this._keyDown.bind(this),true);this.element.tabIndex=0;this.callFrameList=new WebInspector.UIList();this.callFrameList.show(this.element);WebInspector.moduleSetting("enableAsyncStackTraces").addChangeListener(this._asyncStackTracesStateChanged,this);WebInspector.moduleSetting("skipStackFramesPattern").addChangeListener(this._blackboxingStateChanged,this);}
WebInspector.CallStackSidebarPane.Events={CallFrameSelected:"CallFrameSelected",RevealHiddenCallFrames:"RevealHiddenCallFrames"}
WebInspector.CallStackSidebarPane.prototype={update:function(details)
{this.callFrameList.detach();this.callFrameList.clear();this.element.removeChildren();if(!details){var infoElement=this.element.createChild("div","callstack-info");infoElement.textContent=WebInspector.UIString("Not Paused");return;}
this.callFrameList.show(this.element);this._debuggerModel=details.debuggerModel;var callFrames=details.callFrames;var asyncStackTrace=details.asyncStackTrace;delete this._statusMessageElement;delete this._hiddenCallFramesMessageElement;this.callFrames=[];this._hiddenCallFrames=0;this._appendSidebarCallFrames(callFrames);var topStackHidden=(this._hiddenCallFrames===this.callFrames.length);while(asyncStackTrace){var title=WebInspector.asyncStackTraceLabel(asyncStackTrace.description);var asyncCallFrame=new WebInspector.UIList.Item(title,"",true);asyncCallFrame.element.addEventListener("click",this._selectNextVisibleCallFrame.bind(this,this.callFrames.length,false),false);asyncCallFrame.element.addEventListener("contextmenu",this._asyncCallFrameContextMenu.bind(this,this.callFrames.length),true);this._appendSidebarCallFrames(asyncStackTrace.callFrames,asyncCallFrame);asyncStackTrace=asyncStackTrace.asyncStackTrace;}
if(topStackHidden)
this._revealHiddenCallFrames();if(this._hiddenCallFrames){var element=createElementWithClass("div","hidden-callframes-message");if(this._hiddenCallFrames===1)
element.textContent=WebInspector.UIString("1 stack frame is hidden (black-boxed).");else
element.textContent=WebInspector.UIString("%d stack frames are hidden (black-boxed).",this._hiddenCallFrames);element.createTextChild(" ");var showAllLink=element.createChild("span","link");showAllLink.textContent=WebInspector.UIString("Show");showAllLink.addEventListener("click",this._revealHiddenCallFrames.bind(this),false);this.element.insertBefore(element,this.element.firstChild);this._hiddenCallFramesMessageElement=element;}},_appendSidebarCallFrames:function(callFrames,asyncCallFrameItem)
{if(asyncCallFrameItem)
this.callFrameList.addItem(asyncCallFrameItem);var allCallFramesHidden=true;for(var i=0,n=callFrames.length;i<n;++i){var callFrame=callFrames[i];var callFrameItem=new WebInspector.CallStackSidebarPane.CallFrame(callFrame,asyncCallFrameItem);callFrameItem.element.addEventListener("click",this._callFrameSelected.bind(this,callFrameItem),false);callFrameItem.element.addEventListener("contextmenu",this._callFrameContextMenu.bind(this,callFrameItem),true);this.callFrames.push(callFrameItem);if(WebInspector.BlackboxSupport.isBlackboxed(callFrame.script.sourceURL,callFrame.script.isContentScript())){callFrameItem.setHidden(true);callFrameItem.setDimmed(true);++this._hiddenCallFrames;}else{this.callFrameList.addItem(callFrameItem);allCallFramesHidden=false;}}
if(allCallFramesHidden&&asyncCallFrameItem){asyncCallFrameItem.setHidden(true);asyncCallFrameItem.element.remove();}},_revealHiddenCallFrames:function()
{if(!this._hiddenCallFrames)
return;this._hiddenCallFrames=0;this.callFrameList.clear();for(var i=0;i<this.callFrames.length;++i){var callFrame=this.callFrames[i];if(callFrame._asyncCallFrame){callFrame._asyncCallFrame.setHidden(false);if(i&&callFrame._asyncCallFrame!==this.callFrames[i-1]._asyncCallFrame)
this.callFrameList.addItem(callFrame._asyncCallFrame);}
callFrame.setHidden(false);this.callFrameList.addItem(callFrame);}
if(this._hiddenCallFramesMessageElement){this._hiddenCallFramesMessageElement.remove();delete this._hiddenCallFramesMessageElement;}
this.dispatchEventToListeners(WebInspector.CallStackSidebarPane.Events.RevealHiddenCallFrames);},_callFrameContextMenu:function(callFrame,event)
{var contextMenu=new WebInspector.ContextMenu(event);if(!callFrame._callFrame.isAsync())
contextMenu.appendItem(WebInspector.UIString.capitalize("Restart ^frame"),this._restartFrame.bind(this,callFrame));contextMenu.appendItem(WebInspector.UIString.capitalize("Copy ^stack ^trace"),this._copyStackTrace.bind(this));var script=callFrame._callFrame.script;this.appendBlackboxURLContextMenuItems(contextMenu,script.sourceURL,script.isContentScript());contextMenu.show();},_asyncCallFrameContextMenu:function(index,event)
{for(;index<this.callFrames.length;++index){var callFrame=this.callFrames[index];if(!callFrame.isHidden()){this._callFrameContextMenu(callFrame,event);break;}}},appendBlackboxURLContextMenuItems:function(contextMenu,url,isContentScript)
{var blackboxed=WebInspector.BlackboxSupport.isBlackboxed(url,isContentScript);var canBlackBox=WebInspector.BlackboxSupport.canBlackboxURL(url);if(!blackboxed&&!isContentScript&&!canBlackBox)
return;if(blackboxed){contextMenu.appendItem(WebInspector.UIString.capitalize("Stop ^blackboxing"),this._handleContextMenuBlackboxURL.bind(this,url,isContentScript,false));}else{if(canBlackBox)
contextMenu.appendItem(WebInspector.UIString.capitalize("Blackbox ^script"),this._handleContextMenuBlackboxURL.bind(this,url,false,true));if(isContentScript)
contextMenu.appendItem(WebInspector.UIString.capitalize("Blackbox ^all ^content ^scripts"),this._handleContextMenuBlackboxURL.bind(this,url,true,true));}},_handleContextMenuBlackboxURL:function(url,isContentScript,blackbox)
{if(blackbox){if(isContentScript)
WebInspector.moduleSetting("skipContentScripts").set(true);else
WebInspector.BlackboxSupport.blackboxURL(url);}else{WebInspector.BlackboxSupport.unblackbox(url,isContentScript);}},_blackboxingStateChanged:function()
{if(!this._debuggerModel)
return;var details=this._debuggerModel.debuggerPausedDetails();if(!details)
return;this.update(details);var selectedCallFrame=this._debuggerModel.selectedCallFrame();if(selectedCallFrame)
this.setSelectedCallFrame(selectedCallFrame);},_restartFrame:function(callFrame)
{callFrame._callFrame.restart();},_asyncStackTracesStateChanged:function()
{var enabled=WebInspector.moduleSetting("enableAsyncStackTraces").get();if(!enabled&&this.callFrames)
this._removeAsyncCallFrames();},_removeAsyncCallFrames:function()
{var shouldSelectTopFrame=false;var lastSyncCallFrameIndex=-1;for(var i=0;i<this.callFrames.length;++i){var callFrame=this.callFrames[i];if(callFrame._asyncCallFrame){if(callFrame.isSelected())
shouldSelectTopFrame=true;callFrame._asyncCallFrame.element.remove();callFrame.element.remove();}else{lastSyncCallFrameIndex=i;}}
this.callFrames.length=lastSyncCallFrameIndex+1;if(shouldSelectTopFrame)
this._selectNextVisibleCallFrame(0);},setSelectedCallFrame:function(x)
{for(var i=0;i<this.callFrames.length;++i){var callFrame=this.callFrames[i];callFrame.setSelected(callFrame._callFrame===x);if(callFrame.isSelected()&&callFrame.isHidden())
this._revealHiddenCallFrames();}},_selectNextCallFrameOnStack:function()
{var index=this._selectedCallFrameIndex();if(index===-1)
return false;return this._selectNextVisibleCallFrame(index+1);},_selectPreviousCallFrameOnStack:function()
{var index=this._selectedCallFrameIndex();if(index===-1)
return false;return this._selectNextVisibleCallFrame(index-1,true);},_selectNextVisibleCallFrame:function(index,backward)
{while(0<=index&&index<this.callFrames.length){var callFrame=this.callFrames[index];if(!callFrame.isHidden()&&!callFrame.isLabel()){this._callFrameSelected(callFrame);return true;}
index+=backward?-1:1;}
return false;},_selectedCallFrameIndex:function()
{if(!this._debuggerModel)
return-1;var selectedCallFrame=this._debuggerModel.selectedCallFrame();if(!selectedCallFrame)
return-1;for(var i=0;i<this.callFrames.length;++i){var callFrame=this.callFrames[i];if(callFrame._callFrame===selectedCallFrame)
return i;}
return-1;},_callFrameSelected:function(callFrame)
{callFrame.element.scrollIntoViewIfNeeded();this.dispatchEventToListeners(WebInspector.CallStackSidebarPane.Events.CallFrameSelected,callFrame._callFrame);},_copyStackTrace:function()
{var text="";var lastCallFrame=null;for(var i=0;i<this.callFrames.length;++i){var callFrame=this.callFrames[i];if(callFrame.isHidden())
continue;if(lastCallFrame&&callFrame._asyncCallFrame!==lastCallFrame._asyncCallFrame)
text+=callFrame._asyncCallFrame.title()+"\n";text+=callFrame.title()+" ("+callFrame.subtitle()+")\n";lastCallFrame=callFrame;}
InspectorFrontendHost.copyText(text);},registerShortcuts:function(registerShortcutDelegate)
{registerShortcutDelegate(WebInspector.ShortcutsScreen.SourcesPanelShortcuts.NextCallFrame,this._selectNextCallFrameOnStack.bind(this));registerShortcutDelegate(WebInspector.ShortcutsScreen.SourcesPanelShortcuts.PrevCallFrame,this._selectPreviousCallFrameOnStack.bind(this));},setStatus:function(status)
{if(!this._statusMessageElement)
this._statusMessageElement=this.element.createChild("div","callstack-info status");if(typeof status==="string"){this._statusMessageElement.textContent=status;}else{this._statusMessageElement.removeChildren();this._statusMessageElement.appendChild(status);}},_keyDown:function(event)
{if(event.altKey||event.shiftKey||event.metaKey||event.ctrlKey)
return;if(event.keyIdentifier==="Up"&&this._selectPreviousCallFrameOnStack()||event.keyIdentifier==="Down"&&this._selectNextCallFrameOnStack())
event.consume(true);},__proto__:WebInspector.SidebarPane.prototype}
WebInspector.CallStackSidebarPane.CallFrame=function(callFrame,asyncCallFrame)
{WebInspector.UIList.Item.call(this,WebInspector.beautifyFunctionName(callFrame.functionName),"");WebInspector.debuggerWorkspaceBinding.createCallFrameLiveLocation(callFrame,this._update.bind(this));this._callFrame=callFrame;this._asyncCallFrame=asyncCallFrame;}
WebInspector.CallStackSidebarPane.CallFrame.prototype={_update:function(uiLocation)
{var text=uiLocation.linkText();this.setSubtitle(text.trimMiddle(30));this.subtitleElement.title=text;},__proto__:WebInspector.UIList.Item.prototype};WebInspector.HistoryEntry=function(){}
WebInspector.HistoryEntry.prototype={valid:function(){},reveal:function(){}};WebInspector.SimpleHistoryManager=function(historyDepth)
{this._entries=[];this._activeEntryIndex=-1;this._coalescingReadonly=0;this._historyDepth=historyDepth;}
WebInspector.SimpleHistoryManager.prototype={readOnlyLock:function()
{++this._coalescingReadonly;},releaseReadOnlyLock:function()
{--this._coalescingReadonly;},readOnly:function()
{return!!this._coalescingReadonly;},filterOut:function(filterOutCallback)
{if(this.readOnly())
return;var filteredEntries=[];var removedBeforeActiveEntry=0;for(var i=0;i<this._entries.length;++i){if(!filterOutCallback(this._entries[i])){filteredEntries.push(this._entries[i]);}else if(i<=this._activeEntryIndex)
++removedBeforeActiveEntry;}
this._entries=filteredEntries;this._activeEntryIndex=Math.max(0,this._activeEntryIndex-removedBeforeActiveEntry);},empty:function()
{return!this._entries.length;},active:function()
{return this.empty()?null:this._entries[this._activeEntryIndex];},push:function(entry)
{if(this.readOnly())
return;if(!this.empty())
this._entries.splice(this._activeEntryIndex+1);this._entries.push(entry);if(this._entries.length>this._historyDepth)
this._entries.shift();this._activeEntryIndex=this._entries.length-1;},rollback:function()
{if(this.empty())
return false;var revealIndex=this._activeEntryIndex-1;while(revealIndex>=0&&!this._entries[revealIndex].valid())
--revealIndex;if(revealIndex<0)
return false;this.readOnlyLock();this._entries[revealIndex].reveal();this.releaseReadOnlyLock();this._activeEntryIndex=revealIndex;return true;},rollover:function()
{var revealIndex=this._activeEntryIndex+1;while(revealIndex<this._entries.length&&!this._entries[revealIndex].valid())
++revealIndex;if(revealIndex>=this._entries.length)
return false;this.readOnlyLock();this._entries[revealIndex].reveal();this.releaseReadOnlyLock();this._activeEntryIndex=revealIndex;return true;},};;WebInspector.EditingLocationHistoryManager=function(sourcesView,currentSourceFrameCallback)
{this._sourcesView=sourcesView;this._historyManager=new WebInspector.SimpleHistoryManager(WebInspector.EditingLocationHistoryManager.HistoryDepth);this._currentSourceFrameCallback=currentSourceFrameCallback;}
WebInspector.EditingLocationHistoryManager.HistoryDepth=20;WebInspector.EditingLocationHistoryManager.prototype={trackSourceFrameCursorJumps:function(sourceFrame)
{sourceFrame.addEventListener(WebInspector.SourceFrame.Events.JumpHappened,this._onJumpHappened.bind(this));},_onJumpHappened:function(event)
{if(event.data.from)
this._updateActiveState(event.data.from);if(event.data.to)
this._pushActiveState(event.data.to);},rollback:function()
{this._historyManager.rollback();},rollover:function()
{this._historyManager.rollover();},updateCurrentState:function()
{var sourceFrame=this._currentSourceFrameCallback();if(!sourceFrame)
return;this._updateActiveState(sourceFrame.textEditor.selection());},pushNewState:function()
{var sourceFrame=this._currentSourceFrameCallback();if(!sourceFrame)
return;this._pushActiveState(sourceFrame.textEditor.selection());},_updateActiveState:function(selection)
{var active=this._historyManager.active();if(!active)
return;var sourceFrame=this._currentSourceFrameCallback();if(!sourceFrame)
return;var entry=new WebInspector.EditingLocationHistoryEntry(this._sourcesView,this,sourceFrame,selection);active.merge(entry);},_pushActiveState:function(selection)
{var sourceFrame=this._currentSourceFrameCallback();if(!sourceFrame)
return;var entry=new WebInspector.EditingLocationHistoryEntry(this._sourcesView,this,sourceFrame,selection);this._historyManager.push(entry);},removeHistoryForSourceCode:function(uiSourceCode)
{function filterOut(entry)
{return entry._projectId===uiSourceCode.project().id()&&entry._path===uiSourceCode.path();}
this._historyManager.filterOut(filterOut);},}
WebInspector.EditingLocationHistoryEntry=function(sourcesView,editingLocationManager,sourceFrame,selection)
{this._sourcesView=sourcesView;this._editingLocationManager=editingLocationManager;var uiSourceCode=sourceFrame.uiSourceCode();this._projectId=uiSourceCode.project().id();this._path=uiSourceCode.path();var position=this._positionFromSelection(selection);this._positionHandle=sourceFrame.textEditor.textEditorPositionHandle(position.lineNumber,position.columnNumber);}
WebInspector.EditingLocationHistoryEntry.prototype={merge:function(entry)
{if(this._projectId!==entry._projectId||this._path!==entry._path)
return;this._positionHandle=entry._positionHandle;},_positionFromSelection:function(selection)
{return{lineNumber:selection.endLine,columnNumber:selection.endColumn};},valid:function()
{var position=this._positionHandle.resolve();var uiSourceCode=WebInspector.workspace.project(this._projectId).uiSourceCode(this._path);return!!(position&&uiSourceCode);},reveal:function()
{var position=this._positionHandle.resolve();var uiSourceCode=WebInspector.workspace.project(this._projectId).uiSourceCode(this._path);if(!position||!uiSourceCode)
return;this._editingLocationManager.updateCurrentState();this._sourcesView.showSourceLocation(uiSourceCode,position.lineNumber,position.columnNumber);}};WebInspector.EventListenerBreakpointsSidebarPane=function()
{WebInspector.SidebarPane.call(this,WebInspector.UIString("Event Listener Breakpoints"));this.registerRequiredCSS("components/breakpointsList.css");this._eventListenerBreakpointsSetting=WebInspector.settings.createLocalSetting("eventListenerBreakpoints",[]);this._categoriesTreeOutline=new TreeOutline();this._categoriesTreeOutline.element.tabIndex=0;this._categoriesTreeOutline.element.classList.add("event-listener-breakpoints");this.element.appendChild(this._categoriesTreeOutline.element);this._categoryItems=[];this._createCategory(WebInspector.UIString("Animation"),["requestAnimationFrame","cancelAnimationFrame","animationFrameFired"],true);this._createCategory(WebInspector.UIString("Clipboard"),["copy","cut","paste","beforecopy","beforecut","beforepaste"]);this._createCategory(WebInspector.UIString("Control"),["resize","scroll","zoom","focus","blur","select","change","submit","reset"]);this._createCategory(WebInspector.UIString("Device"),["deviceorientation","devicemotion"]);this._createCategory(WebInspector.UIString("DOM Mutation"),["DOMActivate","DOMFocusIn","DOMFocusOut","DOMAttrModified","DOMCharacterDataModified","DOMNodeInserted","DOMNodeInsertedIntoDocument","DOMNodeRemoved","DOMNodeRemovedFromDocument","DOMSubtreeModified","DOMContentLoaded"]);this._createCategory(WebInspector.UIString("Drag / drop"),["dragenter","dragover","dragleave","drop"]);this._createCategory(WebInspector.UIString("Keyboard"),["keydown","keyup","keypress","input"]);this._createCategory(WebInspector.UIString("Load"),["load","beforeunload","unload","abort","error","hashchange","popstate"]);this._createCategory(WebInspector.UIString("Media"),["play","pause","playing","canplay","canplaythrough","seeking","seeked","timeupdate","ended","ratechange","durationchange","volumechange","loadstart","progress","suspend","abort","error","emptied","stalled","loadedmetadata","loadeddata","waiting"],false,["audio","video"]);this._createCategory(WebInspector.UIString("Mouse"),["click","dblclick","mousedown","mouseup","mouseover","mousemove","mouseout","mouseenter","mouseleave","mousewheel","wheel","contextmenu"]);this._createCategory(WebInspector.UIString("Parse"),["setInnerHTML"],true);this._createCategory(WebInspector.UIString("Pointer"),["pointerover","pointerout","pointerenter","pointerleave","pointerdown","pointerup","pointermove","pointercancel","gotpointercapture","lostpointercapture"],true);this._createCategory(WebInspector.UIString("Script"),["scriptFirstStatement"],true);this._createCategory(WebInspector.UIString("Timer"),["setTimer","clearTimer","timerFired"],true);this._createCategory(WebInspector.UIString("Touch"),["touchstart","touchmove","touchend","touchcancel"]);this._createCategory(WebInspector.UIString("WebGL"),["webglErrorFired","webglWarningFired"],true);this._createCategory(WebInspector.UIString("Window"),["close"],true);this._createCategory(WebInspector.UIString("XHR"),["readystatechange","load","loadstart","loadend","abort","error","progress","timeout"],false,["XMLHttpRequest","XMLHttpRequestUpload"]);WebInspector.targetManager.observeTargets(this,WebInspector.Target.Type.Page);}
WebInspector.EventListenerBreakpointsSidebarPane.categoryListener="listener:";WebInspector.EventListenerBreakpointsSidebarPane.categoryInstrumentation="instrumentation:";WebInspector.EventListenerBreakpointsSidebarPane.eventTargetAny="*";WebInspector.EventListenerBreakpointsSidebarPane.eventNameForUI=function(eventName,auxData)
{if(!WebInspector.EventListenerBreakpointsSidebarPane._eventNamesForUI){WebInspector.EventListenerBreakpointsSidebarPane._eventNamesForUI={"instrumentation:setTimer":WebInspector.UIString("Set Timer"),"instrumentation:clearTimer":WebInspector.UIString("Clear Timer"),"instrumentation:timerFired":WebInspector.UIString("Timer Fired"),"instrumentation:scriptFirstStatement":WebInspector.UIString("Script First Statement"),"instrumentation:requestAnimationFrame":WebInspector.UIString("Request Animation Frame"),"instrumentation:cancelAnimationFrame":WebInspector.UIString("Cancel Animation Frame"),"instrumentation:animationFrameFired":WebInspector.UIString("Animation Frame Fired"),"instrumentation:webglErrorFired":WebInspector.UIString("WebGL Error Fired"),"instrumentation:webglWarningFired":WebInspector.UIString("WebGL Warning Fired"),"instrumentation:setInnerHTML":WebInspector.UIString("Set innerHTML"),};}
if(auxData){if(eventName==="instrumentation:webglErrorFired"&&auxData["webglErrorName"]){var errorName=auxData["webglErrorName"];errorName=errorName.replace(/^.*(0x[0-9a-f]+).*$/i,"$1");return WebInspector.UIString("WebGL Error Fired (%s)",errorName);}}
return WebInspector.EventListenerBreakpointsSidebarPane._eventNamesForUI[eventName]||eventName.substring(eventName.indexOf(":")+1);}
WebInspector.EventListenerBreakpointsSidebarPane.prototype={targetAdded:function(target)
{this._restoreBreakpoints(target);},targetRemoved:function(target){},_createCategory:function(name,eventNames,isInstrumentationEvent,targetNames)
{var labelNode=createCheckboxLabel(name);var categoryItem={};categoryItem.element=new TreeElement(labelNode);this._categoriesTreeOutline.appendChild(categoryItem.element);categoryItem.element.listItemElement.classList.add("event-category");categoryItem.element.selectable=true;categoryItem.checkbox=labelNode.checkboxElement;categoryItem.checkbox.addEventListener("click",this._categoryCheckboxClicked.bind(this,categoryItem),true);categoryItem.targetNames=this._stringArrayToLowerCase(targetNames||[WebInspector.EventListenerBreakpointsSidebarPane.eventTargetAny]);categoryItem.children={};var category=(isInstrumentationEvent?WebInspector.EventListenerBreakpointsSidebarPane.categoryInstrumentation:WebInspector.EventListenerBreakpointsSidebarPane.categoryListener);for(var i=0;i<eventNames.length;++i){var eventName=category+eventNames[i];var breakpointItem={};var title=WebInspector.EventListenerBreakpointsSidebarPane.eventNameForUI(eventName);labelNode=createCheckboxLabel(title);labelNode.classList.add("source-code");breakpointItem.element=new TreeElement(labelNode);categoryItem.element.appendChild(breakpointItem.element);breakpointItem.element.listItemElement.createChild("div","breakpoint-hit-marker");breakpointItem.element.selectable=false;breakpointItem.checkbox=labelNode.checkboxElement;breakpointItem.checkbox.addEventListener("click",this._breakpointCheckboxClicked.bind(this,eventName,categoryItem.targetNames),true);breakpointItem.parent=categoryItem;categoryItem.children[eventName]=breakpointItem;}
this._categoryItems.push(categoryItem);},_stringArrayToLowerCase:function(array)
{return array.map(function(value){return value.toLowerCase();});},_categoryCheckboxClicked:function(categoryItem)
{var checked=categoryItem.checkbox.checked;for(var eventName in categoryItem.children){var breakpointItem=categoryItem.children[eventName];if(breakpointItem.checkbox.checked===checked)
continue;if(checked)
this._setBreakpoint(eventName,categoryItem.targetNames);else
this._removeBreakpoint(eventName,categoryItem.targetNames);}
this._saveBreakpoints();},_breakpointCheckboxClicked:function(eventName,targetNames,event)
{if(event.target.checked)
this._setBreakpoint(eventName,targetNames);else
this._removeBreakpoint(eventName,targetNames);this._saveBreakpoints();},_setBreakpoint:function(eventName,eventTargetNames,target)
{eventTargetNames=eventTargetNames||[WebInspector.EventListenerBreakpointsSidebarPane.eventTargetAny];for(var i=0;i<eventTargetNames.length;++i){var eventTargetName=eventTargetNames[i];var breakpointItem=this._findBreakpointItem(eventName,eventTargetName);if(!breakpointItem)
continue;breakpointItem.checkbox.checked=true;breakpointItem.parent.dirtyCheckbox=true;this._updateBreakpointOnTarget(eventName,eventTargetName,true,target);}
this._updateCategoryCheckboxes();},_removeBreakpoint:function(eventName,eventTargetNames,target)
{eventTargetNames=eventTargetNames||[WebInspector.EventListenerBreakpointsSidebarPane.eventTargetAny];for(var i=0;i<eventTargetNames.length;++i){var eventTargetName=eventTargetNames[i];var breakpointItem=this._findBreakpointItem(eventName,eventTargetName);if(!breakpointItem)
continue;breakpointItem.checkbox.checked=false;breakpointItem.parent.dirtyCheckbox=true;this._updateBreakpointOnTarget(eventName,eventTargetName,false,target);}
this._updateCategoryCheckboxes();},_updateBreakpointOnTarget:function(eventName,eventTargetName,enable,target)
{var targets=target?[target]:WebInspector.targetManager.targets(WebInspector.Target.Type.Page);for(var i=0;i<targets.length;++i){if(eventName.startsWith(WebInspector.EventListenerBreakpointsSidebarPane.categoryListener)){var protocolEventName=eventName.substring(WebInspector.EventListenerBreakpointsSidebarPane.categoryListener.length);if(enable)
targets[i].domdebuggerAgent().setEventListenerBreakpoint(protocolEventName,eventTargetName);else
targets[i].domdebuggerAgent().removeEventListenerBreakpoint(protocolEventName,eventTargetName);}else if(eventName.startsWith(WebInspector.EventListenerBreakpointsSidebarPane.categoryInstrumentation)){var protocolEventName=eventName.substring(WebInspector.EventListenerBreakpointsSidebarPane.categoryInstrumentation.length);if(enable)
targets[i].domdebuggerAgent().setInstrumentationBreakpoint(protocolEventName);else
targets[i].domdebuggerAgent().removeInstrumentationBreakpoint(protocolEventName);}}},_updateCategoryCheckboxes:function()
{for(var i=0;i<this._categoryItems.length;++i){var categoryItem=this._categoryItems[i];if(!categoryItem.dirtyCheckbox)
continue;categoryItem.dirtyCheckbox=false;var hasEnabled=false;var hasDisabled=false;for(var eventName in categoryItem.children){var breakpointItem=categoryItem.children[eventName];if(breakpointItem.checkbox.checked)
hasEnabled=true;else
hasDisabled=true;}
categoryItem.checkbox.checked=hasEnabled;categoryItem.checkbox.indeterminate=hasEnabled&&hasDisabled;}},_findBreakpointItem:function(eventName,targetName)
{targetName=(targetName||WebInspector.EventListenerBreakpointsSidebarPane.eventTargetAny).toLowerCase();for(var i=0;i<this._categoryItems.length;++i){var categoryItem=this._categoryItems[i];if(categoryItem.targetNames.indexOf(targetName)===-1)
continue;var breakpointItem=categoryItem.children[eventName];if(breakpointItem)
return breakpointItem;}
return null;},highlightBreakpoint:function(eventName,targetName)
{var breakpointItem=this._findBreakpointItem(eventName,targetName);if(!breakpointItem||!breakpointItem.checkbox.checked)
breakpointItem=this._findBreakpointItem(eventName,WebInspector.EventListenerBreakpointsSidebarPane.eventTargetAny);if(!breakpointItem)
return;this.expand();breakpointItem.parent.element.expand();breakpointItem.element.listItemElement.classList.add("breakpoint-hit");this._highlightedElement=breakpointItem.element.listItemElement;},clearBreakpointHighlight:function()
{if(this._highlightedElement){this._highlightedElement.classList.remove("breakpoint-hit");delete this._highlightedElement;}},_saveBreakpoints:function()
{var breakpoints=[];for(var i=0;i<this._categoryItems.length;++i){var categoryItem=this._categoryItems[i];for(var eventName in categoryItem.children){var breakpointItem=categoryItem.children[eventName];if(breakpointItem.checkbox.checked)
breakpoints.push({eventName:eventName,targetNames:categoryItem.targetNames});}}
this._eventListenerBreakpointsSetting.set(breakpoints);},_restoreBreakpoints:function(target)
{var breakpoints=this._eventListenerBreakpointsSetting.get();for(var i=0;i<breakpoints.length;++i){var breakpoint=breakpoints[i];if(breakpoint&&typeof breakpoint.eventName==="string")
this._setBreakpoint(breakpoint.eventName,breakpoint.targetNames,target);}},__proto__:WebInspector.SidebarPane.prototype};WebInspector.FilePathScoreFunction=function(query)
{this._query=query;this._queryUpperCase=query.toUpperCase();this._score=null;this._sequence=null;this._dataUpperCase="";this._fileNameIndex=0;}
WebInspector.FilePathScoreFunction.filterRegex=function(query)
{const toEscape=String.regexSpecialCharacters();var regexString="";for(var i=0;i<query.length;++i){var c=query.charAt(i);if(toEscape.indexOf(c)!==-1)
c="\\"+c;if(i)
regexString+="[^"+c+"]*";regexString+=c;}
return new RegExp(regexString,"i");}
WebInspector.FilePathScoreFunction.prototype={score:function(data,matchIndexes)
{if(!data||!this._query)
return 0;var n=this._query.length;var m=data.length;if(!this._score||this._score.length<n*m){this._score=new Int32Array(n*m*2);this._sequence=new Int32Array(n*m*2);}
var score=this._score;var sequence=(this._sequence);this._dataUpperCase=data.toUpperCase();this._fileNameIndex=data.lastIndexOf("/");for(var i=0;i<n;++i){for(var j=0;j<m;++j){var skipCharScore=j===0?0:score[i*m+j-1];var prevCharScore=i===0||j===0?0:score[(i-1)*m+j-1];var consecutiveMatch=i===0||j===0?0:sequence[(i-1)*m+j-1];var pickCharScore=this._match(this._query,data,i,j,consecutiveMatch);if(pickCharScore&&prevCharScore+pickCharScore>=skipCharScore){sequence[i*m+j]=consecutiveMatch+1;score[i*m+j]=(prevCharScore+pickCharScore);}else{sequence[i*m+j]=0;score[i*m+j]=skipCharScore;}}}
if(matchIndexes)
this._restoreMatchIndexes(sequence,n,m,matchIndexes);return score[n*m-1];},_testWordStart:function(data,j)
{var prevChar=data.charAt(j-1);return j===0||prevChar==="_"||prevChar==="-"||prevChar==="/"||(data[j-1]!==this._dataUpperCase[j-1]&&data[j]===this._dataUpperCase[j]);},_restoreMatchIndexes:function(sequence,n,m,out)
{var i=n-1,j=m-1;while(i>=0&&j>=0){switch(sequence[i*m+j]){case 0:--j;break;default:out.push(j);--i;--j;break;}}
out.reverse();},_singleCharScore:function(query,data,i,j)
{var isWordStart=this._testWordStart(data,j);var isFileName=j>this._fileNameIndex;var isPathTokenStart=j===0||data[j-1]==="/";var isCapsMatch=query[i]===data[j]&&query[i]==this._queryUpperCase[i];var score=10;if(isPathTokenStart)
score+=4;if(isWordStart)
score+=2;if(isCapsMatch)
score+=6;if(isFileName)
score+=4;if(j===this._fileNameIndex+1&&i===0)
score+=5;if(isFileName&&isWordStart)
score+=3;return score;},_sequenceCharScore:function(query,data,i,j,sequenceLength)
{var isFileName=j>this._fileNameIndex;var isPathTokenStart=j===0||data[j-1]==="/";var score=10;if(isFileName)
score+=4;if(isPathTokenStart)
score+=5;score+=sequenceLength*4;return score;},_match:function(query,data,i,j,consecutiveMatch)
{if(this._queryUpperCase[i]!==this._dataUpperCase[j])
return 0;if(!consecutiveMatch)
return this._singleCharScore(query,data,i,j);else
return this._sequenceCharScore(query,data,i,j-consecutiveMatch,consecutiveMatch);}};WebInspector.FilteredItemSelectionDialog=function(delegate,renderAsTwoRows)
{WebInspector.VBox.call(this);this._renderAsTwoRows=renderAsTwoRows;this.element.classList.add("filtered-item-list-dialog");this.element.addEventListener("keydown",this._onKeyDown.bind(this),false);this.element.appendChild(WebInspector.Widget.createStyleElement("sources/filteredItemSelectionDialog.css"));this._promptElement=this.element.createChild("input","monospace");this._promptElement.addEventListener("input",this._onInput.bind(this),false);this._promptElement.type="text";this._promptElement.setAttribute("spellcheck","false");this._filteredItems=[];this._viewportControl=new WebInspector.ViewportControl(this);this._itemElementsContainer=this._viewportControl.element;this._itemElementsContainer.classList.add("container");this._itemElementsContainer.classList.add("monospace");this._itemElementsContainer.addEventListener("click",this._onClick.bind(this),false);this.element.appendChild(this._itemElementsContainer);this.setDefaultFocusedElement(this._promptElement);this._delegate=delegate;this._delegate.setRefreshCallback(this._itemsLoaded.bind(this));this._itemsLoaded();this._updateShowMatchingItems();this._viewportControl.refresh();this._dialog=new WebInspector.Dialog();this._dialog.setMaxSize(new Size(504,600));this.show(this._dialog.element);this._dialog.show();}
WebInspector.FilteredItemSelectionDialog.prototype={willHide:function()
{this._delegate.dispose();if(this._filterTimer)
clearTimeout(this._filterTimer);},_onEnter:function(event)
{if(!this._delegate.itemCount())
return;event.preventDefault();var selectedIndex=this._shouldShowMatchingItems()&&this._selectedIndexInFiltered<this._filteredItems.length?this._filteredItems[this._selectedIndexInFiltered]:null;this._delegate.selectItem(selectedIndex,this._promptElement.value.trim());this._dialog.detach();},_itemsLoaded:function()
{if(this._loadTimeout)
return;this._loadTimeout=setTimeout(this._updateAfterItemsLoaded.bind(this),0);},_updateAfterItemsLoaded:function()
{delete this._loadTimeout;this._filterItems();},_createItemElement:function(index)
{var itemElement=createElement("div");itemElement.className="filtered-item-list-dialog-item "+(this._renderAsTwoRows?"two-rows":"one-row");itemElement._titleElement=itemElement.createChild("div","filtered-item-list-dialog-title");itemElement._subtitleElement=itemElement.createChild("div","filtered-item-list-dialog-subtitle");itemElement._subtitleElement.textContent="\u200B";itemElement._index=index;this._delegate.renderItem(index,this._promptElement.value.trim(),itemElement._titleElement,itemElement._subtitleElement);return itemElement;},setQuery:function(query)
{this._promptElement.value=query;this._scheduleFilter();},_filterItems:function()
{delete this._filterTimer;if(this._scoringTimer){clearTimeout(this._scoringTimer);delete this._scoringTimer;}
var query=this._delegate.rewriteQuery(this._promptElement.value.trim());this._query=query;var filterRegex=query?WebInspector.FilePathScoreFunction.filterRegex(query):null;var oldSelectedAbsoluteIndex=this._selectedIndexInFiltered?this._filteredItems[this._selectedIndexInFiltered]:null;var filteredItems=[];this._selectedIndexInFiltered=0;var bestScores=[];var bestItems=[];var bestItemsToCollect=100;var minBestScore=0;var overflowItems=[];scoreItems.call(this,0);function compareIntegers(a,b)
{return b-a;}
function scoreItems(fromIndex)
{var maxWorkItems=1000;var workDone=0;for(var i=fromIndex;i<this._delegate.itemCount()&&workDone<maxWorkItems;++i){if(filterRegex&&!filterRegex.test(this._delegate.itemKeyAt(i)))
continue;var score=this._delegate.itemScoreAt(i,query);if(query)
workDone++;if(score>minBestScore||bestScores.length<bestItemsToCollect){var index=insertionIndexForObjectInListSortedByFunction(score,bestScores,compareIntegers,true);bestScores.splice(index,0,score);bestItems.splice(index,0,i);if(bestScores.length>bestItemsToCollect){overflowItems.push(bestItems.peekLast());bestScores.length=bestItemsToCollect;bestItems.length=bestItemsToCollect;}
minBestScore=bestScores.peekLast();}else
filteredItems.push(i);}
if(i<this._delegate.itemCount()){this._scoringTimer=setTimeout(scoreItems.bind(this,i),0);return;}
delete this._scoringTimer;this._filteredItems=bestItems.concat(overflowItems).concat(filteredItems);for(var i=0;i<this._filteredItems.length;++i){if(this._filteredItems[i]===oldSelectedAbsoluteIndex){this._selectedIndexInFiltered=i;break;}}
this._viewportControl.invalidate();if(!query)
this._selectedIndexInFiltered=0;this._updateSelection(this._selectedIndexInFiltered,false);}},_shouldShowMatchingItems:function()
{return this._delegate.shouldShowMatchingItems(this._promptElement.value);},_onInput:function(event)
{this._updateShowMatchingItems();this._scheduleFilter();},_updateShowMatchingItems:function()
{var shouldShowMatchingItems=this._shouldShowMatchingItems();this._itemElementsContainer.classList.toggle("hidden",!shouldShowMatchingItems);},_rowsPerViewport:function()
{return Math.floor(this._viewportControl.element.clientHeight/this._rowHeight);},_onKeyDown:function(event)
{var newSelectedIndex=this._selectedIndexInFiltered;switch(event.keyCode){case WebInspector.KeyboardShortcut.Keys.Down.code:if(++newSelectedIndex>=this._filteredItems.length)
newSelectedIndex=this._filteredItems.length-1;this._updateSelection(newSelectedIndex,true);event.consume(true);break;case WebInspector.KeyboardShortcut.Keys.Up.code:if(--newSelectedIndex<0)
newSelectedIndex=0;this._updateSelection(newSelectedIndex,false);event.consume(true);break;case WebInspector.KeyboardShortcut.Keys.PageDown.code:newSelectedIndex=Math.min(newSelectedIndex+this._rowsPerViewport(),this._filteredItems.length-1);this._updateSelection(newSelectedIndex,true);event.consume(true);break;case WebInspector.KeyboardShortcut.Keys.PageUp.code:newSelectedIndex=Math.max(newSelectedIndex-this._rowsPerViewport(),0);this._updateSelection(newSelectedIndex,false);event.consume(true);break;case WebInspector.KeyboardShortcut.Keys.Enter.code:this._onEnter(event);break;default:}},_scheduleFilter:function()
{if(this._filterTimer)
return;this._filterTimer=setTimeout(this._filterItems.bind(this),0);},_updateSelection:function(index,makeLast)
{if(!this._filteredItems.length)
return;if(this._selectedElement)
this._selectedElement.classList.remove("selected");this._viewportControl.scrollItemIntoView(index,makeLast);this._selectedIndexInFiltered=index;this._selectedElement=this._viewportControl.renderedElementAt(index);if(this._selectedElement)
this._selectedElement.classList.add("selected");},_onClick:function(event)
{var itemElement=event.target.enclosingNodeOrSelfWithClass("filtered-item-list-dialog-item");if(!itemElement)
return;this._delegate.selectItem(itemElement._index,this._promptElement.value.trim());this._dialog.detach();},itemCount:function()
{return this._filteredItems.length;},fastHeight:function(index)
{if(!this._rowHeight){var delegateIndex=this._filteredItems[index];var element=this._createItemElement(delegateIndex);this._rowHeight=WebInspector.measurePreferredSize(element,this._viewportControl.contentElement()).height;}
return this._rowHeight;},itemElement:function(index)
{var delegateIndex=this._filteredItems[index];var element=this._createItemElement(delegateIndex);return new WebInspector.StaticViewportElement(element);},minimumRowHeight:function()
{return this.fastHeight(0);},__proto__:WebInspector.VBox.prototype}
WebInspector.SelectionDialogContentProvider=function()
{}
WebInspector.SelectionDialogContentProvider.prototype={setRefreshCallback:function(refreshCallback)
{this._refreshCallback=refreshCallback;},shouldShowMatchingItems:function(query)
{return true;},itemCount:function()
{return 0;},itemKeyAt:function(itemIndex)
{return"";},itemScoreAt:function(itemIndex,query)
{return 1;},renderItem:function(itemIndex,query,titleElement,subtitleElement)
{},highlightRanges:function(element,query)
{if(!query)
return false;function rangesForMatch(text,query)
{var opcodes=WebInspector.Diff.charDiff(query,text);var offset=0;var ranges=[];for(var i=0;i<opcodes.length;++i){var opcode=opcodes[i];if(opcode[0]===WebInspector.Diff.Operation.Equal)
ranges.push(new WebInspector.SourceRange(offset,opcode[1].length));else if(opcode[0]!==WebInspector.Diff.Operation.Insert)
return null;offset+=opcode[1].length;}
return ranges;}
var text=element.textContent;var ranges=rangesForMatch(text,query);if(!ranges)
ranges=rangesForMatch(text.toUpperCase(),query.toUpperCase());if(ranges){WebInspector.highlightRangesWithStyleClass(element,ranges,"highlight");return true;}
return false;},selectItem:function(itemIndex,promptValue)
{},refresh:function()
{this._refreshCallback();},rewriteQuery:function(query)
{return query;},dispose:function()
{}}
WebInspector.JavaScriptOutlineDialog=function(uiSourceCode,selectItemCallback)
{WebInspector.SelectionDialogContentProvider.call(this);this._functionItems=[];this._selectItemCallback=selectItemCallback;this._outlineWorker=new WorkerRuntime.Worker("script_formatter_worker");this._outlineWorker.onmessage=this._didBuildOutlineChunk.bind(this);this._outlineWorker.postMessage({method:"javaScriptOutline",params:{content:uiSourceCode.workingCopy()}});}
WebInspector.JavaScriptOutlineDialog.show=function(uiSourceCode,selectItemCallback)
{new WebInspector.FilteredItemSelectionDialog(new WebInspector.JavaScriptOutlineDialog(uiSourceCode,selectItemCallback),false);}
WebInspector.JavaScriptOutlineDialog.prototype={_didBuildOutlineChunk:function(event)
{var data=(event.data);var chunk=data.chunk;for(var i=0;i<chunk.length;++i)
this._functionItems.push(chunk[i]);if(data.isLastChunk)
this.dispose();this.refresh();},itemCount:function()
{return this._functionItems.length;},itemKeyAt:function(itemIndex)
{var item=this._functionItems[itemIndex];return item.name+(item.arguments?item.arguments:"");},itemScoreAt:function(itemIndex,query)
{var item=this._functionItems[itemIndex];return-item.line;},renderItem:function(itemIndex,query,titleElement,subtitleElement)
{var item=this._functionItems[itemIndex];titleElement.textContent=item.name+(item.arguments?item.arguments:"");this.highlightRanges(titleElement,query);subtitleElement.textContent=":"+(item.line+1);},selectItem:function(itemIndex,promptValue)
{if(itemIndex===null)
return;var lineNumber=this._functionItems[itemIndex].line;if(!isNaN(lineNumber)&&lineNumber>=0)
this._selectItemCallback(lineNumber,this._functionItems[itemIndex].column);},dispose:function()
{if(this._outlineWorker){this._outlineWorker.terminate();delete this._outlineWorker;}},__proto__:WebInspector.SelectionDialogContentProvider.prototype}
WebInspector.SelectUISourceCodeDialog=function(defaultScores)
{WebInspector.SelectionDialogContentProvider.call(this);this._populate();this._defaultScores=defaultScores;this._scorer=new WebInspector.FilePathScoreFunction("");WebInspector.workspace.addEventListener(WebInspector.Workspace.Events.UISourceCodeAdded,this._uiSourceCodeAdded,this);WebInspector.workspace.addEventListener(WebInspector.Workspace.Events.ProjectRemoved,this._projectRemoved,this);}
WebInspector.SelectUISourceCodeDialog.prototype={_projectRemoved:function(event)
{var project=(event.data);this._populate(project);this.refresh();},_populate:function(skipProject)
{this._uiSourceCodes=[];var projects=WebInspector.workspace.projects().filter(this.filterProject.bind(this));for(var i=0;i<projects.length;++i){if(skipProject&&projects[i]===skipProject)
continue;this._uiSourceCodes=this._uiSourceCodes.concat(projects[i].uiSourceCodes());}},uiSourceCodeSelected:function(uiSourceCode,lineNumber,columnNumber)
{},filterProject:function(project)
{return true;},itemCount:function()
{return this._uiSourceCodes.length;},itemKeyAt:function(itemIndex)
{return this._uiSourceCodes[itemIndex].fullDisplayName();},itemScoreAt:function(itemIndex,query)
{var uiSourceCode=this._uiSourceCodes[itemIndex];var score=this._defaultScores?(this._defaultScores.get(uiSourceCode)||0):0;if(!query||query.length<2)
return score;if(this._query!==query){this._query=query;this._scorer=new WebInspector.FilePathScoreFunction(query);}
var path=uiSourceCode.fullDisplayName();return score+10*this._scorer.score(path,null);},renderItem:function(itemIndex,query,titleElement,subtitleElement)
{query=this.rewriteQuery(query);var uiSourceCode=this._uiSourceCodes[itemIndex];var fullDisplayName=uiSourceCode.fullDisplayName();var indexes=[];var score=new WebInspector.FilePathScoreFunction(query).score(fullDisplayName,indexes);var fileNameIndex=fullDisplayName.lastIndexOf("/");titleElement.textContent=uiSourceCode.displayName()+(this._queryLineNumberAndColumnNumber||"");subtitleElement.textContent=fullDisplayName.trimEnd(100);subtitleElement.title=fullDisplayName;var ranges=[];for(var i=0;i<indexes.length;++i)
ranges.push({offset:indexes[i],length:1});if(indexes[0]>fileNameIndex){for(var i=0;i<ranges.length;++i)
ranges[i].offset-=fileNameIndex+1;WebInspector.highlightRangesWithStyleClass(titleElement,ranges,"highlight");}else{WebInspector.highlightRangesWithStyleClass(subtitleElement,ranges,"highlight");}},selectItem:function(itemIndex,promptValue)
{var parsedExpression=promptValue.trim().match(/^([^:]*)(:\d+)?(:\d+)?$/);if(!parsedExpression)
return;var lineNumber;var columnNumber;if(parsedExpression[2])
lineNumber=parseInt(parsedExpression[2].substr(1),10)-1;if(parsedExpression[3])
columnNumber=parseInt(parsedExpression[3].substr(1),10)-1;var uiSourceCode=itemIndex!==null?this._uiSourceCodes[itemIndex]:null;this.uiSourceCodeSelected(uiSourceCode,lineNumber,columnNumber);},rewriteQuery:function(query)
{if(!query)
return query;query=query.trim();var lineNumberMatch=query.match(/^([^:]+)((?::[^:]*){0,2})$/);this._queryLineNumberAndColumnNumber=lineNumberMatch?lineNumberMatch[2]:"";return lineNumberMatch?lineNumberMatch[1]:query;},_uiSourceCodeAdded:function(event)
{var uiSourceCode=(event.data);if(!this.filterProject(uiSourceCode.project()))
return;this._uiSourceCodes.push(uiSourceCode);this.refresh();},dispose:function()
{WebInspector.workspace.removeEventListener(WebInspector.Workspace.Events.UISourceCodeAdded,this._uiSourceCodeAdded,this);WebInspector.workspace.removeEventListener(WebInspector.Workspace.Events.ProjectRemoved,this._projectRemoved,this);},__proto__:WebInspector.SelectionDialogContentProvider.prototype}
WebInspector.OpenResourceDialog=function(sourcesView,defaultScores)
{WebInspector.SelectUISourceCodeDialog.call(this,defaultScores);this._sourcesView=sourcesView;}
WebInspector.OpenResourceDialog.prototype={uiSourceCodeSelected:function(uiSourceCode,lineNumber,columnNumber)
{if(!uiSourceCode)
uiSourceCode=this._sourcesView.currentUISourceCode();if(!uiSourceCode)
return;this._sourcesView.showSourceLocation(uiSourceCode,lineNumber,columnNumber);},shouldShowMatchingItems:function(query)
{return!query.startsWith(":");},filterProject:function(project)
{return!project.isServiceProject();},__proto__:WebInspector.SelectUISourceCodeDialog.prototype}
WebInspector.OpenResourceDialog.show=function(sourcesView,query,defaultScores)
{var filteredItemSelectionDialog=new WebInspector.FilteredItemSelectionDialog(new WebInspector.OpenResourceDialog(sourcesView,defaultScores),true);if(query)
filteredItemSelectionDialog.setQuery(query);}
WebInspector.SelectUISourceCodeForProjectTypesDialog=function(types,callback)
{this._types=types;WebInspector.SelectUISourceCodeDialog.call(this);this._callback=callback;}
WebInspector.SelectUISourceCodeForProjectTypesDialog.prototype={uiSourceCodeSelected:function(uiSourceCode,lineNumber,columnNumber)
{this._callback(uiSourceCode);},filterProject:function(project)
{return this._types.indexOf(project.type())!==-1;},__proto__:WebInspector.SelectUISourceCodeDialog.prototype}
WebInspector.SelectUISourceCodeForProjectTypesDialog.show=function(name,types,callback)
{var filteredItemSelectionDialog=new WebInspector.FilteredItemSelectionDialog(new WebInspector.SelectUISourceCodeForProjectTypesDialog(types,callback),true);filteredItemSelectionDialog.setQuery(name);}
WebInspector.JavaScriptOutlineDialog.MessageEventData;;WebInspector.UISourceCodeFrame=function(uiSourceCode)
{this._uiSourceCode=uiSourceCode;WebInspector.SourceFrame.call(this,this._uiSourceCode);this.textEditor.setAutocompleteDelegate(new WebInspector.SimpleAutocompleteDelegate());this._rowMessageBuckets={};this._uiSourceCode.addEventListener(WebInspector.UISourceCode.Events.WorkingCopyChanged,this._onWorkingCopyChanged,this);this._uiSourceCode.addEventListener(WebInspector.UISourceCode.Events.WorkingCopyCommitted,this._onWorkingCopyCommitted,this);this._uiSourceCode.addEventListener(WebInspector.UISourceCode.Events.MessageAdded,this._onMessageAdded,this);this._uiSourceCode.addEventListener(WebInspector.UISourceCode.Events.MessageRemoved,this._onMessageRemoved,this);this._updateStyle();this._errorPopoverHelper=new WebInspector.PopoverHelper(this.element,this._getErrorAnchor.bind(this),this._showErrorPopover.bind(this));this._errorPopoverHelper.setTimeout(100,100);}
WebInspector.UISourceCodeFrame.prototype={uiSourceCode:function()
{return this._uiSourceCode;},wasShown:function()
{WebInspector.SourceFrame.prototype.wasShown.call(this);this._boundWindowFocused=this._windowFocused.bind(this);this.element.ownerDocument.defaultView.addEventListener("focus",this._boundWindowFocused,false);this._checkContentUpdated();setImmediate(this._updateBucketDecorations.bind(this));},willHide:function()
{WebInspector.SourceFrame.prototype.willHide.call(this);this.element.ownerDocument.defaultView.removeEventListener("focus",this._boundWindowFocused,false);delete this._boundWindowFocused;this._uiSourceCode.removeWorkingCopyGetter();},canEditSource:function()
{var projectType=this._uiSourceCode.project().type();if(projectType===WebInspector.projectTypes.Service||projectType===WebInspector.projectTypes.Debugger||projectType===WebInspector.projectTypes.Formatter)
return false;if(projectType===WebInspector.projectTypes.Network&&this._uiSourceCode.contentType()===WebInspector.resourceTypes.Document)
return false;return true;},_windowFocused:function(event)
{this._checkContentUpdated();},_checkContentUpdated:function()
{if(!this.loaded||!this.isShowing())
return;this._uiSourceCode.checkContentUpdated(true);},commitEditing:function()
{if(!this._uiSourceCode.isDirty())
return;this._muteSourceCodeEvents=true;this._uiSourceCode.commitWorkingCopy();delete this._muteSourceCodeEvents;},onTextEditorContentLoaded:function()
{WebInspector.SourceFrame.prototype.onTextEditorContentLoaded.call(this);for(var message of this._uiSourceCode.messages())
this._addMessageToSource(message);},onTextChanged:function(oldRange,newRange)
{WebInspector.SourceFrame.prototype.onTextChanged.call(this,oldRange,newRange);this._clearMessages();if(this._isSettingContent)
return;this._muteSourceCodeEvents=true;if(this._textEditor.isClean())
this._uiSourceCode.resetWorkingCopy();else
this._uiSourceCode.setWorkingCopyGetter(this._textEditor.text.bind(this._textEditor));delete this._muteSourceCodeEvents;},_onWorkingCopyChanged:function(event)
{if(this._muteSourceCodeEvents)
return;this._innerSetContent(this._uiSourceCode.workingCopy());this.onUISourceCodeContentChanged();},_onWorkingCopyCommitted:function(event)
{if(!this._muteSourceCodeEvents){this._innerSetContent(this._uiSourceCode.workingCopy());this.onUISourceCodeContentChanged();}
this._textEditor.markClean();this._updateStyle();},_updateStyle:function()
{this.element.classList.toggle("source-frame-unsaved-committed-changes",this._uiSourceCode.hasUnsavedCommittedChanges());},onUISourceCodeContentChanged:function()
{},_innerSetContent:function(content)
{this._isSettingContent=true;this.setContent(content);delete this._isSettingContent;},populateTextAreaContextMenu:function(contextMenu,lineNumber,columnNumber)
{function appendItems()
{contextMenu.appendApplicableItems(this._uiSourceCode);contextMenu.appendApplicableItems(new WebInspector.UILocation(this._uiSourceCode,lineNumber,columnNumber));contextMenu.appendSeparator();}
return WebInspector.SourceFrame.prototype.populateTextAreaContextMenu.call(this,contextMenu,lineNumber,columnNumber).then(appendItems.bind(this));},attachInfobars:function(infobars)
{for(var i=infobars.length-1;i>=0;--i){var infobar=infobars[i];if(!infobar)
continue;this.element.insertBefore(infobar.element,this.element.children[0]);infobar._attached(this);}
this.doResize();},dispose:function()
{this._textEditor.dispose();this.detach();},_onMessageAdded:function(event)
{if(!this.loaded)
return;var message=(event.data);this._addMessageToSource(message);},_addMessageToSource:function(message)
{var lineNumber=message.lineNumber();if(lineNumber>=this._textEditor.linesCount)
lineNumber=this._textEditor.linesCount-1;if(lineNumber<0)
lineNumber=0;if(!this._rowMessageBuckets[lineNumber])
this._rowMessageBuckets[lineNumber]=new WebInspector.UISourceCodeFrame.RowMessageBucket(this,this._textEditor,lineNumber);var messageBucket=this._rowMessageBuckets[lineNumber];messageBucket.addMessage(message);},_onMessageRemoved:function(event)
{if(!this.loaded)
return;var message=(event.data);this._removeMessageFromSource(message);},_removeMessageFromSource:function(message)
{var lineNumber=message.lineNumber();if(lineNumber>=this._textEditor.linesCount)
lineNumber=this._textEditor.linesCount-1;if(lineNumber<0)
lineNumber=0;var messageBucket=this._rowMessageBuckets[lineNumber];if(!messageBucket)
return;messageBucket.removeMessage(message);if(!messageBucket.uniqueMessagesCount()){messageBucket.detachFromEditor();delete this._rowMessageBuckets[lineNumber];}},_clearMessages:function()
{for(var line in this._rowMessageBuckets){var bubble=this._rowMessageBuckets[line];bubble.detachFromEditor();}
this._rowMessageBuckets={};this._errorPopoverHelper.hidePopover();this._uiSourceCode.removeAllMessages();},_getErrorAnchor:function(target,event)
{var element=target.enclosingNodeOrSelfWithClass("text-editor-line-decoration-icon")||target.enclosingNodeOrSelfWithClass("text-editor-line-decoration-wave");if(!element)
return;this._errorWavePopoverAnchor=new AnchorBox(event.clientX,event.clientY,1,1);return element;},_showErrorPopover:function(anchor,popover)
{var messageBucket=anchor.enclosingNodeOrSelfWithClass("text-editor-line-decoration")._messageBucket;var messagesOutline=messageBucket.messagesDescription();var popoverAnchor=anchor.enclosingNodeOrSelfWithClass("text-editor-line-decoration-icon")?anchor:this._errorWavePopoverAnchor;popover.showForAnchor(messagesOutline,popoverAnchor);},_updateBucketDecorations:function()
{for(var line in this._rowMessageBuckets){var bucket=this._rowMessageBuckets[line];bucket._updateDecoration();}},__proto__:WebInspector.SourceFrame.prototype}
WebInspector.UISourceCodeFrame.Infobar=function(type,message,disableSetting)
{WebInspector.Infobar.call(this,type,disableSetting);this.setCloseCallback(this.dispose.bind(this));this.element.classList.add("source-frame-infobar");this._rows=this.element.createChild("div","source-frame-infobar-rows");this._mainRow=this._rows.createChild("div","source-frame-infobar-main-row");this._mainRow.createChild("span","source-frame-infobar-row-message").textContent=message;this._toggleElement=this._mainRow.createChild("div","source-frame-infobar-toggle link");this._toggleElement.addEventListener("click",this._onToggleDetails.bind(this),false);this._detailsContainer=this._rows.createChild("div","source-frame-infobar-details-container");this._updateToggleElement();}
WebInspector.UISourceCodeFrame.Infobar.prototype={_onResize:function()
{if(this._uiSourceCodeFrame)
this._uiSourceCodeFrame.doResize();},_onToggleDetails:function()
{this._toggled=!this._toggled;this._updateToggleElement();this._onResize();},_updateToggleElement:function()
{this._toggleElement.textContent=this._toggled?WebInspector.UIString("less"):WebInspector.UIString("more");this._detailsContainer.classList.toggle("hidden",!this._toggled);},_attached:function(uiSourceCodeFrame)
{this._uiSourceCodeFrame=uiSourceCodeFrame;this.setVisible(true);},createDetailsRowMessage:function(message)
{var infobarDetailsRow=this._detailsContainer.createChild("div","source-frame-infobar-details-row");var detailsRowMessage=infobarDetailsRow.createChild("span","source-frame-infobar-row-message");detailsRowMessage.textContent=message||"";return detailsRowMessage;},dispose:function()
{this.element.remove();this._onResize();delete this._uiSourceCodeFrame;},__proto__:WebInspector.Infobar.prototype}
WebInspector.UISourceCodeFrame._iconClassPerLevel={};WebInspector.UISourceCodeFrame._iconClassPerLevel[WebInspector.UISourceCode.Message.Level.Error]="error-icon";WebInspector.UISourceCodeFrame._iconClassPerLevel[WebInspector.UISourceCode.Message.Level.Warning]="warning-icon";WebInspector.UISourceCodeFrame._lineClassPerLevel={};WebInspector.UISourceCodeFrame._lineClassPerLevel[WebInspector.UISourceCode.Message.Level.Error]="text-editor-line-with-error";WebInspector.UISourceCodeFrame._lineClassPerLevel[WebInspector.UISourceCode.Message.Level.Warning]="text-editor-line-with-warning";WebInspector.UISourceCodeFrame.RowMessage=function(message)
{this._message=message;this._repeatCount=1;this.element=createElementWithClass("div","text-editor-row-message");this._icon=this.element.createChild("label","","dt-icon-label");this._icon.type=WebInspector.UISourceCodeFrame._iconClassPerLevel[message.level()];this._repeatCountElement=this.element.createChild("span","bubble-repeat-count hidden error");var linesContainer=this.element.createChild("div","text-editor-row-message-lines");var lines=this._message.text().split("\n");for(var i=0;i<lines.length;++i){var messageLine=linesContainer.createChild("div");messageLine.textContent=lines[i];}}
WebInspector.UISourceCodeFrame.RowMessage.prototype={message:function()
{return this._message;},repeatCount:function()
{return this._repeatCount;},setRepeatCount:function(repeatCount)
{if(this._repeatCount===repeatCount)
return;this._repeatCount=repeatCount;this._updateMessageRepeatCount();},_updateMessageRepeatCount:function()
{this._repeatCountElement.textContent=this._repeatCount;var showRepeatCount=this._repeatCount>1;this._repeatCountElement.classList.toggle("hidden",!showRepeatCount);this._icon.classList.toggle("hidden",showRepeatCount);}}
WebInspector.UISourceCodeFrame.RowMessageBucket=function(sourceFrame,textEditor,lineNumber)
{this._sourceFrame=sourceFrame;this._textEditor=textEditor;this._lineHandle=textEditor.textEditorPositionHandle(lineNumber,0);this._decoration=createElementWithClass("div","text-editor-line-decoration");this._decoration._messageBucket=this;this._wave=this._decoration.createChild("div","text-editor-line-decoration-wave");this._icon=this._wave.createChild("label","text-editor-line-decoration-icon","dt-icon-label");this._textEditor.addDecoration(lineNumber,this._decoration);this._messagesDescriptionElement=createElementWithClass("div","text-editor-messages-description-container");this._messages=[];this._level=null;}
WebInspector.UISourceCodeFrame.RowMessageBucket.prototype={_updateWavePosition:function(lineNumber,columnNumber)
{lineNumber=Math.min(lineNumber,this._textEditor.linesCount-1);var lineText=this._textEditor.line(lineNumber);columnNumber=Math.min(columnNumber,lineText.length);var lineIndent=WebInspector.TextUtils.lineIndent(lineText).length;var base=this._textEditor.cursorPositionToCoordinates(lineNumber,0);var start=this._textEditor.cursorPositionToCoordinates(lineNumber,Math.max(columnNumber-1,lineIndent));var end=this._textEditor.cursorPositionToCoordinates(lineNumber,lineText.length);var codeMirrorLinesLeftPadding=4;this._wave.style.left=(start.x-base.x+codeMirrorLinesLeftPadding)+"px";this._wave.style.width=(end.x-start.x)+"px";},messagesDescription:function()
{this._messagesDescriptionElement.removeChildren();for(var i=0;i<this._messages.length;++i){this._messagesDescriptionElement.appendChild(this._messages[i].element);}
return this._messagesDescriptionElement;},detachFromEditor:function()
{var position=this._lineHandle.resolve();if(!position)
return;var lineNumber=position.lineNumber;if(this._level)
this._textEditor.toggleLineClass(lineNumber,WebInspector.UISourceCodeFrame._lineClassPerLevel[this._level],false);this._textEditor.removeDecoration(lineNumber,this._decoration);},uniqueMessagesCount:function()
{return this._messages.length;},addMessage:function(message)
{for(var i=0;i<this._messages.length;++i){var rowMessage=this._messages[i];if(rowMessage.message().isEqual(message)){rowMessage.setRepeatCount(rowMessage.repeatCount()+1);return;}}
var rowMessage=new WebInspector.UISourceCodeFrame.RowMessage(message);this._messages.push(rowMessage);this._updateDecoration();},removeMessage:function(message)
{for(var i=0;i<this._messages.length;++i){var rowMessage=this._messages[i];if(!rowMessage.message().isEqual(message))
continue;rowMessage.setRepeatCount(rowMessage.repeatCount()-1);if(!rowMessage.repeatCount())
this._messages.splice(i,1);this._updateDecoration();return;}},_updateDecoration:function()
{if(!this._sourceFrame.isEditorShowing())
return;if(!this._messages.length)
return;var position=this._lineHandle.resolve();if(!position)
return;var lineNumber=position.lineNumber;var columnNumber=Number.MAX_VALUE;var maxMessage=null;for(var i=0;i<this._messages.length;++i){var message=this._messages[i].message();columnNumber=Math.min(columnNumber,message.columnNumber());if(!maxMessage||WebInspector.UISourceCode.Message.messageLevelComparator(maxMessage,message)<0)
maxMessage=message;}
this._updateWavePosition(lineNumber,columnNumber);if(this._level){this._textEditor.toggleLineClass(lineNumber,WebInspector.UISourceCodeFrame._lineClassPerLevel[this._level],false);this._icon.type="";}
this._level=maxMessage.level();if(!this._level)
return;this._textEditor.toggleLineClass(lineNumber,WebInspector.UISourceCodeFrame._lineClassPerLevel[this._level],true);this._icon.type=WebInspector.UISourceCodeFrame._iconClassPerLevel[this._level];}}
WebInspector.UISourceCode.Message._messageLevelPriority={"Warning":3,"Error":4};WebInspector.UISourceCode.Message.messageLevelComparator=function(a,b)
{return WebInspector.UISourceCode.Message._messageLevelPriority[a.level()]-WebInspector.UISourceCode.Message._messageLevelPriority[b.level()];};WebInspector.JavaScriptBreakpointsSidebarPane=function(breakpointManager,showSourceLineDelegate)
{WebInspector.SidebarPane.call(this,WebInspector.UIString("Breakpoints"));this.registerRequiredCSS("components/breakpointsList.css");this._breakpointManager=breakpointManager;this._showSourceLineDelegate=showSourceLineDelegate;this.listElement=createElementWithClass("ol","breakpoint-list");this.emptyElement=this.element.createChild("div","info");this.emptyElement.textContent=WebInspector.UIString("No Breakpoints");this._items=new Map();var breakpointLocations=this._breakpointManager.allBreakpointLocations();for(var i=0;i<breakpointLocations.length;++i)
this._addBreakpoint(breakpointLocations[i].breakpoint,breakpointLocations[i].uiLocation);this._breakpointManager.addEventListener(WebInspector.BreakpointManager.Events.BreakpointAdded,this._breakpointAdded,this);this._breakpointManager.addEventListener(WebInspector.BreakpointManager.Events.BreakpointRemoved,this._breakpointRemoved,this);this.emptyElement.addEventListener("contextmenu",this._emptyElementContextMenu.bind(this),true);}
WebInspector.JavaScriptBreakpointsSidebarPane.prototype={_emptyElementContextMenu:function(event)
{var contextMenu=new WebInspector.ContextMenu(event);this._appendBreakpointActiveItem(contextMenu);contextMenu.show();},_appendBreakpointActiveItem:function(contextMenu)
{var breakpointActive=this._breakpointManager.breakpointsActive();var breakpointActiveTitle=breakpointActive?WebInspector.UIString.capitalize("Deactivate ^breakpoints"):WebInspector.UIString.capitalize("Activate ^breakpoints");contextMenu.appendItem(breakpointActiveTitle,this._breakpointManager.setBreakpointsActive.bind(this._breakpointManager,!breakpointActive));},_breakpointAdded:function(event)
{this._breakpointRemoved(event);var breakpoint=(event.data.breakpoint);var uiLocation=(event.data.uiLocation);this._addBreakpoint(breakpoint,uiLocation);},_addBreakpoint:function(breakpoint,uiLocation)
{var element=createElementWithClass("li","cursor-pointer");element.addEventListener("contextmenu",this._breakpointContextMenu.bind(this,breakpoint),true);element.addEventListener("click",this._breakpointClicked.bind(this,uiLocation),false);var checkboxLabel=createCheckboxLabel(uiLocation.linkText(),breakpoint.enabled());element.appendChild(checkboxLabel);checkboxLabel.addEventListener("click",this._breakpointCheckboxClicked.bind(this,breakpoint),false);var snippetElement=element.createChild("div","source-text monospace");function didRequestContent(content)
{var lineNumber=uiLocation.lineNumber
var columnNumber=uiLocation.columnNumber;var contentString=new String(content);if(lineNumber<contentString.lineCount()){var lineText=contentString.lineAt(lineNumber);var maxSnippetLength=200;var snippetStartIndex=columnNumber>100?columnNumber:0;snippetElement.textContent=lineText.substr(snippetStartIndex).trimEnd(maxSnippetLength);}
this.didReceiveBreakpointLineForTest(uiLocation.uiSourceCode);}
uiLocation.uiSourceCode.requestContent(didRequestContent.bind(this));element._data=uiLocation;var currentElement=this.listElement.firstChild;while(currentElement){if(currentElement._data&&this._compareBreakpoints(currentElement._data,element._data)>0)
break;currentElement=currentElement.nextSibling;}
this._addListElement(element,currentElement);var breakpointItem={element:element,checkbox:checkboxLabel.checkboxElement};this._items.set(breakpoint,breakpointItem);this.expand();},didReceiveBreakpointLineForTest:function(uiSourceCode)
{},_breakpointRemoved:function(event)
{var breakpoint=(event.data.breakpoint);var breakpointItem=this._items.get(breakpoint);if(!breakpointItem)
return;this._items.remove(breakpoint);this._removeListElement(breakpointItem.element);},highlightBreakpoint:function(breakpoint)
{var breakpointItem=this._items.get(breakpoint);if(!breakpointItem)
return;breakpointItem.element.classList.add("breakpoint-hit");this._highlightedBreakpointItem=breakpointItem;},clearBreakpointHighlight:function()
{if(this._highlightedBreakpointItem){this._highlightedBreakpointItem.element.classList.remove("breakpoint-hit");delete this._highlightedBreakpointItem;}},_breakpointClicked:function(uiLocation,event)
{this._showSourceLineDelegate(uiLocation.uiSourceCode,uiLocation.lineNumber);},_breakpointCheckboxClicked:function(breakpoint,event)
{event.consume();breakpoint.setEnabled(event.target.checkboxElement.checked);},_breakpointContextMenu:function(breakpoint,event)
{var breakpoints=this._items.valuesArray();var contextMenu=new WebInspector.ContextMenu(event);contextMenu.appendItem(WebInspector.UIString.capitalize("Remove ^breakpoint"),breakpoint.remove.bind(breakpoint));if(breakpoints.length>1){var removeAllTitle=WebInspector.UIString.capitalize("Remove ^all ^breakpoints");contextMenu.appendItem(removeAllTitle,this._breakpointManager.removeAllBreakpoints.bind(this._breakpointManager));}
contextMenu.appendSeparator();this._appendBreakpointActiveItem(contextMenu);function enabledBreakpointCount(breakpoints)
{var count=0;for(var i=0;i<breakpoints.length;++i){if(breakpoints[i].checkbox.checked)
count++;}
return count;}
if(breakpoints.length>1){var enableBreakpointCount=enabledBreakpointCount(breakpoints);var enableTitle=WebInspector.UIString.capitalize("Enable ^all ^breakpoints");var disableTitle=WebInspector.UIString.capitalize("Disable ^all ^breakpoints");contextMenu.appendSeparator();contextMenu.appendItem(enableTitle,this._breakpointManager.toggleAllBreakpoints.bind(this._breakpointManager,true),!(enableBreakpointCount!=breakpoints.length));contextMenu.appendItem(disableTitle,this._breakpointManager.toggleAllBreakpoints.bind(this._breakpointManager,false),!(enableBreakpointCount>1));}
contextMenu.show();},_addListElement:function(element,beforeElement)
{if(beforeElement)
this.listElement.insertBefore(element,beforeElement);else{if(!this.listElement.firstChild){this.element.removeChild(this.emptyElement);this.element.appendChild(this.listElement);}
this.listElement.appendChild(element);}},_removeListElement:function(element)
{this.listElement.removeChild(element);if(!this.listElement.firstChild){this.element.removeChild(this.listElement);this.element.appendChild(this.emptyElement);}},_compare:function(x,y)
{if(x!==y)
return x<y?-1:1;return 0;},_compareBreakpoints:function(b1,b2)
{return this._compare(b1.uiSourceCode.originURL(),b2.uiSourceCode.originURL())||this._compare(b1.lineNumber,b2.lineNumber);},reset:function()
{this.listElement.removeChildren();if(this.listElement.parentElement){this.element.removeChild(this.listElement);this.element.appendChild(this.emptyElement);}
this._items.clear();},__proto__:WebInspector.SidebarPane.prototype};WebInspector.JavaScriptSourceFrame=function(scriptsPanel,uiSourceCode)
{this._scriptsPanel=scriptsPanel;this._breakpointManager=WebInspector.breakpointManager;this._uiSourceCode=uiSourceCode;WebInspector.UISourceCodeFrame.call(this,uiSourceCode);if(uiSourceCode.project().type()===WebInspector.projectTypes.Debugger)
this.element.classList.add("source-frame-debugger-script");this._popoverHelper=new WebInspector.ObjectPopoverHelper(scriptsPanel.element,this._getPopoverAnchor.bind(this),this._resolveObjectForPopover.bind(this),this._onHidePopover.bind(this),true);this.textEditor.element.addEventListener("keydown",this._onKeyDown.bind(this),true);this.textEditor.addEventListener(WebInspector.CodeMirrorTextEditor.Events.GutterClick,this._handleGutterClick.bind(this),this);this._breakpointManager.addEventListener(WebInspector.BreakpointManager.Events.BreakpointAdded,this._breakpointAdded,this);this._breakpointManager.addEventListener(WebInspector.BreakpointManager.Events.BreakpointRemoved,this._breakpointRemoved,this);this._uiSourceCode.addEventListener(WebInspector.UISourceCode.Events.SourceMappingChanged,this._onSourceMappingChanged,this);this._uiSourceCode.addEventListener(WebInspector.UISourceCode.Events.WorkingCopyChanged,this._workingCopyChanged,this);this._uiSourceCode.addEventListener(WebInspector.UISourceCode.Events.WorkingCopyCommitted,this._workingCopyCommitted,this);this._uiSourceCode.addEventListener(WebInspector.UISourceCode.Events.TitleChanged,this._showBlackboxInfobarIfNeeded,this);this._scriptFileForTarget=new Map();this._registerShortcuts();var targets=WebInspector.targetManager.targets();for(var i=0;i<targets.length;++i){var scriptFile=WebInspector.debuggerWorkspaceBinding.scriptFile(uiSourceCode,targets[i]);if(scriptFile)
this._updateScriptFile(targets[i]);}
if(this._scriptFileForTarget.size||uiSourceCode.extension()==="js")
this._compiler=new WebInspector.JavaScriptCompiler(this);WebInspector.moduleSetting("skipStackFramesPattern").addChangeListener(this._showBlackboxInfobarIfNeeded,this);WebInspector.moduleSetting("skipContentScripts").addChangeListener(this._showBlackboxInfobarIfNeeded,this);this._showBlackboxInfobarIfNeeded();this._valueWidgets=new Map();}
WebInspector.JavaScriptSourceFrame.prototype={_updateInfobars:function()
{this.attachInfobars([this._blackboxInfobar,this._divergedInfobar]);},_showDivergedInfobar:function()
{if(this._uiSourceCode.contentType()!==WebInspector.resourceTypes.Script)
return;if(this._divergedInfobar)
this._divergedInfobar.dispose();var infobar=new WebInspector.UISourceCodeFrame.Infobar(WebInspector.Infobar.Type.Warning,WebInspector.UIString("Workspace mapping mismatch"));this._divergedInfobar=infobar;var fileURL=this._uiSourceCode.originURL();infobar.createDetailsRowMessage(WebInspector.UIString("The content of this file on the file system:\u00a0")).appendChild(WebInspector.linkifyURLAsNode(fileURL,fileURL,"source-frame-infobar-details-url",true));var scriptURL=WebInspector.networkMapping.networkURL(this._uiSourceCode);infobar.createDetailsRowMessage(WebInspector.UIString("does not match the loaded script:\u00a0")).appendChild(WebInspector.linkifyURLAsNode(scriptURL,scriptURL,"source-frame-infobar-details-url",true));infobar.createDetailsRowMessage();infobar.createDetailsRowMessage(WebInspector.UIString("Possible solutions are:"));if(WebInspector.moduleSetting("cacheDisabled").get())
infobar.createDetailsRowMessage(" - ").createTextChild(WebInspector.UIString("Reload inspected page"));else
infobar.createDetailsRowMessage(" - ").createTextChild(WebInspector.UIString("Check \"Disable cache\" in settings and reload inspected page (recommended setup for authoring and debugging)"));infobar.createDetailsRowMessage(" - ").createTextChild(WebInspector.UIString("Check that your file and script are both loaded from the correct source and their contents match"));this._updateInfobars();},_hideDivergedInfobar:function()
{if(!this._divergedInfobar)
return;this._divergedInfobar.dispose();delete this._divergedInfobar;},_showBlackboxInfobarIfNeeded:function()
{var contentType=this._uiSourceCode.contentType();if(contentType!==WebInspector.resourceTypes.Script&&contentType!==WebInspector.resourceTypes.Document)
return;var projectType=this._uiSourceCode.project().type();if(projectType===WebInspector.projectTypes.Snippets)
return;var networkURL=WebInspector.networkMapping.networkURL(this._uiSourceCode);var url=projectType===WebInspector.projectTypes.Formatter?this._uiSourceCode.originURL():networkURL;var isContentScript=projectType===WebInspector.projectTypes.ContentScripts;if(!WebInspector.BlackboxSupport.isBlackboxed(url,isContentScript)){this._hideBlackboxInfobar();return;}
if(this._blackboxInfobar)
this._blackboxInfobar.dispose();var infobar=new WebInspector.UISourceCodeFrame.Infobar(WebInspector.Infobar.Type.Warning,WebInspector.UIString("This script is blackboxed in debugger"));this._blackboxInfobar=infobar;infobar.createDetailsRowMessage(WebInspector.UIString("Debugger will skip stepping through this script, and will not stop on exceptions"));infobar.createDetailsRowMessage();infobar.createDetailsRowMessage(WebInspector.UIString("Possible ways to cancel this behavior are:"));infobar.createDetailsRowMessage(" - ").createTextChild(WebInspector.UIString("Go to \"%s\" tab in settings",WebInspector.manageBlackboxingSettingsTabLabel()));var unblackboxLink=infobar.createDetailsRowMessage(" - ").createChild("span","link");unblackboxLink.textContent=WebInspector.UIString("Unblackbox this script");unblackboxLink.addEventListener("click",unblackbox,false);function unblackbox()
{WebInspector.BlackboxSupport.unblackbox(url,isContentScript);}
this._updateInfobars();},_hideBlackboxInfobar:function()
{if(!this._blackboxInfobar)
return;this._blackboxInfobar.dispose();delete this._blackboxInfobar;},_registerShortcuts:function()
{var shortcutKeys=WebInspector.ShortcutsScreen.SourcesPanelShortcuts;for(var i=0;i<shortcutKeys.EvaluateSelectionInConsole.length;++i){var keyDescriptor=shortcutKeys.EvaluateSelectionInConsole[i];this.addShortcut(keyDescriptor.key,this._evaluateSelectionInConsole.bind(this));}
for(var i=0;i<shortcutKeys.AddSelectionToWatch.length;++i){var keyDescriptor=shortcutKeys.AddSelectionToWatch[i];this.addShortcut(keyDescriptor.key,this._addCurrentSelectionToWatch.bind(this));}},_addCurrentSelectionToWatch:function()
{var textSelection=this.textEditor.selection();if(textSelection&&!textSelection.isEmpty())
this._innerAddToWatch(this.textEditor.copyRange(textSelection));return true;},_innerAddToWatch:function(expression)
{this._scriptsPanel.addToWatch(expression);},_evaluateSelectionInConsole:function()
{var selection=this.textEditor.selection();if(!selection||selection.isEmpty())
return true;this._evaluateInConsole(this.textEditor.copyRange(selection));return true;},_evaluateInConsole:function(expression)
{var currentExecutionContext=WebInspector.context.flavor(WebInspector.ExecutionContext);if(currentExecutionContext)
WebInspector.ConsoleModel.evaluateCommandInConsole(currentExecutionContext,expression);},wasShown:function()
{WebInspector.UISourceCodeFrame.prototype.wasShown.call(this);if(this._executionLocation&&this.loaded){setImmediate(this._generateValuesInSource.bind(this));}},willHide:function()
{WebInspector.UISourceCodeFrame.prototype.willHide.call(this);this._popoverHelper.hidePopover();},onUISourceCodeContentChanged:function()
{this._removeAllBreakpoints();WebInspector.UISourceCodeFrame.prototype.onUISourceCodeContentChanged.call(this);},onTextChanged:function(oldRange,newRange)
{this._scriptsPanel.setIgnoreExecutionLineEvents(true);WebInspector.UISourceCodeFrame.prototype.onTextChanged.call(this,oldRange,newRange);this._scriptsPanel.setIgnoreExecutionLineEvents(false);if(this._compiler)
this._compiler.scheduleCompile();},populateLineGutterContextMenu:function(contextMenu,lineNumber)
{function populate(resolve,reject)
{var uiLocation=new WebInspector.UILocation(this._uiSourceCode,lineNumber,0);this._scriptsPanel.appendUILocationItems(contextMenu,uiLocation);var breakpoint=this._breakpointManager.findBreakpointOnLine(this._uiSourceCode,lineNumber);if(!breakpoint){contextMenu.appendItem(WebInspector.UIString.capitalize("Add ^breakpoint"),this._createNewBreakpoint.bind(this,lineNumber,0,"",true));contextMenu.appendItem(WebInspector.UIString.capitalize("Add ^conditional ^breakpoint…"),this._editBreakpointCondition.bind(this,lineNumber));}else{contextMenu.appendItem(WebInspector.UIString.capitalize("Remove ^breakpoint"),breakpoint.remove.bind(breakpoint));contextMenu.appendItem(WebInspector.UIString.capitalize("Edit ^breakpoint…"),this._editBreakpointCondition.bind(this,lineNumber,breakpoint));if(breakpoint.enabled())
contextMenu.appendItem(WebInspector.UIString.capitalize("Disable ^breakpoint"),breakpoint.setEnabled.bind(breakpoint,false));else
contextMenu.appendItem(WebInspector.UIString.capitalize("Enable ^breakpoint"),breakpoint.setEnabled.bind(breakpoint,true));}
resolve();}
return new Promise(populate.bind(this));},populateTextAreaContextMenu:function(contextMenu,lineNumber,columnNumber)
{var textSelection=this.textEditor.selection();if(textSelection&&!textSelection.isEmpty()){var selection=this.textEditor.copyRange(textSelection);var addToWatchLabel=WebInspector.UIString.capitalize("Add to ^watch");contextMenu.appendItem(addToWatchLabel,this._innerAddToWatch.bind(this,selection));var evaluateLabel=WebInspector.UIString.capitalize("Evaluate in ^console");contextMenu.appendItem(evaluateLabel,this._evaluateInConsole.bind(this,selection));contextMenu.appendSeparator();}
function addSourceMapURL(scriptFile)
{WebInspector.AddSourceMapURLDialog.show(addSourceMapURLDialogCallback.bind(null,scriptFile));}
function addSourceMapURLDialogCallback(scriptFile,url)
{if(!url)
return;scriptFile.addSourceMapURL(url);}
function populateSourceMapMembers()
{if(this._uiSourceCode.project().type()===WebInspector.projectTypes.Network&&WebInspector.moduleSetting("jsSourceMapsEnabled").get()){if(this._scriptFileForTarget.size){var scriptFile=this._scriptFileForTarget.valuesArray()[0];var addSourceMapURLLabel=WebInspector.UIString.capitalize("Add ^source ^map\u2026");contextMenu.appendItem(addSourceMapURLLabel,addSourceMapURL.bind(null,scriptFile));contextMenu.appendSeparator();}}}
return WebInspector.UISourceCodeFrame.prototype.populateTextAreaContextMenu.call(this,contextMenu,lineNumber,columnNumber).then(populateSourceMapMembers.bind(this));},_workingCopyChanged:function(event)
{if(this._supportsEnabledBreakpointsWhileEditing()||this._scriptFileForTarget.size)
return;if(this._uiSourceCode.isDirty())
this._muteBreakpointsWhileEditing();else
this._restoreBreakpointsAfterEditing();},_workingCopyCommitted:function(event)
{if(this._supportsEnabledBreakpointsWhileEditing())
return;if(!this._scriptFileForTarget.size){this._restoreBreakpointsAfterEditing();return;}
this._scriptsPanel.setIgnoreExecutionLineEvents(true);},_didMergeToVM:function()
{this._scriptsPanel.setIgnoreExecutionLineEvents(false);if(this._supportsEnabledBreakpointsWhileEditing())
return;this._updateDivergedInfobar();this._restoreBreakpointsIfConsistentScripts();},_didDivergeFromVM:function()
{this._scriptsPanel.setIgnoreExecutionLineEvents(false);if(this._supportsEnabledBreakpointsWhileEditing())
return;this._updateDivergedInfobar();this._muteBreakpointsWhileEditing();},_muteBreakpointsWhileEditing:function()
{if(this._muted)
return;for(var lineNumber=0;lineNumber<this._textEditor.linesCount;++lineNumber){var breakpointDecoration=this._textEditor.getAttribute(lineNumber,"breakpoint");if(!breakpointDecoration)
continue;this._removeBreakpointDecoration(lineNumber);this._addBreakpointDecoration(lineNumber,breakpointDecoration.columnNumber,breakpointDecoration.condition,breakpointDecoration.enabled,true);}
this._muted=true;},_updateDivergedInfobar:function()
{if(this._uiSourceCode.project().type()!==WebInspector.projectTypes.FileSystem){this._hideDivergedInfobar();return;}
var scriptFiles=this._scriptFileForTarget.valuesArray();var hasDivergedScript=false;for(var i=0;i<scriptFiles.length;++i)
hasDivergedScript=hasDivergedScript||scriptFiles[i].hasDivergedFromVM();if(this._divergedInfobar){if(!hasDivergedScript)
this._hideDivergedInfobar();}else{if(hasDivergedScript&&!this._uiSourceCode.isDirty())
this._showDivergedInfobar();}},_supportsEnabledBreakpointsWhileEditing:function()
{return this._uiSourceCode.project().type()===WebInspector.projectTypes.Snippets;},_restoreBreakpointsIfConsistentScripts:function()
{var scriptFiles=this._scriptFileForTarget.valuesArray();for(var i=0;i<scriptFiles.length;++i)
if(scriptFiles[i].hasDivergedFromVM()||scriptFiles[i].isMergingToVM())
return;this._restoreBreakpointsAfterEditing();},_restoreBreakpointsAfterEditing:function()
{delete this._muted;var breakpoints={};for(var lineNumber=0;lineNumber<this._textEditor.linesCount;++lineNumber){var breakpointDecoration=this._textEditor.getAttribute(lineNumber,"breakpoint");if(breakpointDecoration){breakpoints[lineNumber]=breakpointDecoration;this._removeBreakpointDecoration(lineNumber);}}
this._removeAllBreakpoints();for(var lineNumberString in breakpoints){var lineNumber=parseInt(lineNumberString,10);if(isNaN(lineNumber))
continue;var breakpointDecoration=breakpoints[lineNumberString];this._setBreakpoint(lineNumber,breakpointDecoration.columnNumber,breakpointDecoration.condition,breakpointDecoration.enabled);}},_removeAllBreakpoints:function()
{var breakpoints=this._breakpointManager.breakpointsForUISourceCode(this._uiSourceCode);for(var i=0;i<breakpoints.length;++i)
breakpoints[i].remove();},_isIdentifier:function(tokenType)
{return tokenType.startsWith("js-variable")||tokenType.startsWith("js-property")||tokenType=="js-def";},_getPopoverAnchor:function(element,event)
{var target=WebInspector.context.flavor(WebInspector.Target);var debuggerModel=WebInspector.DebuggerModel.fromTarget(target);if(!debuggerModel||!debuggerModel.isPaused())
return;var textPosition=this.textEditor.coordinatesToCursorPosition(event.x,event.y);if(!textPosition)
return;var mouseLine=textPosition.startLine;var mouseColumn=textPosition.startColumn;var textSelection=this.textEditor.selection().normalize();if(textSelection&&!textSelection.isEmpty()){if(textSelection.startLine!==textSelection.endLine||textSelection.startLine!==mouseLine||mouseColumn<textSelection.startColumn||mouseColumn>textSelection.endColumn)
return;var leftCorner=this.textEditor.cursorPositionToCoordinates(textSelection.startLine,textSelection.startColumn);var rightCorner=this.textEditor.cursorPositionToCoordinates(textSelection.endLine,textSelection.endColumn);var anchorBox=new AnchorBox(leftCorner.x,leftCorner.y,rightCorner.x-leftCorner.x,leftCorner.height);anchorBox.highlight={lineNumber:textSelection.startLine,startColumn:textSelection.startColumn,endColumn:textSelection.endColumn-1};anchorBox.forSelection=true;return anchorBox;}
var token=this.textEditor.tokenAtTextPosition(textPosition.startLine,textPosition.startColumn);if(!token||!token.type)
return;var lineNumber=textPosition.startLine;var line=this.textEditor.line(lineNumber);var tokenContent=line.substring(token.startColumn,token.endColumn);var isIdentifier=this._isIdentifier(token.type);if(!isIdentifier&&(token.type!=="js-keyword"||tokenContent!=="this"))
return;var leftCorner=this.textEditor.cursorPositionToCoordinates(lineNumber,token.startColumn);var rightCorner=this.textEditor.cursorPositionToCoordinates(lineNumber,token.endColumn-1);var anchorBox=new AnchorBox(leftCorner.x,leftCorner.y,rightCorner.x-leftCorner.x,leftCorner.height);anchorBox.highlight={lineNumber:lineNumber,startColumn:token.startColumn,endColumn:token.endColumn-1};return anchorBox;},_resolveObjectForPopover:function(anchorBox,showCallback,objectGroupName)
{var target=WebInspector.context.flavor(WebInspector.Target);var debuggerModel=WebInspector.DebuggerModel.fromTarget(target);if(!debuggerModel||!debuggerModel.isPaused()){this._popoverHelper.hidePopover();return;}
var lineNumber=anchorBox.highlight.lineNumber;var startHighlight=anchorBox.highlight.startColumn;var endHighlight=anchorBox.highlight.endColumn;var line=this.textEditor.line(lineNumber);if(!anchorBox.forSelection){while(startHighlight>1&&line.charAt(startHighlight-1)==='.'){var token=this.textEditor.tokenAtTextPosition(lineNumber,startHighlight-2);if(!token||!token.type){this._popoverHelper.hidePopover();return;}
startHighlight=token.startColumn;}}
var evaluationText=line.substring(startHighlight,endHighlight+1);var selectedCallFrame=debuggerModel.selectedCallFrame();selectedCallFrame.evaluate(evaluationText,objectGroupName,false,true,false,false,showObjectPopover.bind(this));function showObjectPopover(result,wasThrown)
{var target=WebInspector.context.flavor(WebInspector.Target);if(selectedCallFrame.target()!=target||!debuggerModel.isPaused()||!result){this._popoverHelper.hidePopover();return;}
this._popoverAnchorBox=anchorBox;showCallback(target.runtimeModel.createRemoteObject(result),wasThrown,this._popoverAnchorBox);if(this._popoverAnchorBox){var highlightRange=new WebInspector.TextRange(lineNumber,startHighlight,lineNumber,endHighlight);this._popoverAnchorBox._highlightDescriptor=this.textEditor.highlightRange(highlightRange,"source-frame-eval-expression");}}},_onHidePopover:function()
{if(!this._popoverAnchorBox)
return;if(this._popoverAnchorBox._highlightDescriptor)
this.textEditor.removeHighlight(this._popoverAnchorBox._highlightDescriptor);delete this._popoverAnchorBox;},_addBreakpointDecoration:function(lineNumber,columnNumber,condition,enabled,mutedWhileEditing)
{var breakpoint={condition:condition,enabled:enabled,columnNumber:columnNumber};this.textEditor.setAttribute(lineNumber,"breakpoint",breakpoint);var disabled=!enabled||mutedWhileEditing;this.textEditor.addBreakpoint(lineNumber,disabled,!!condition);},_removeBreakpointDecoration:function(lineNumber)
{this.textEditor.removeAttribute(lineNumber,"breakpoint");this.textEditor.removeBreakpoint(lineNumber);},_onKeyDown:function(event)
{if(event.keyIdentifier==="U+001B"){if(this._popoverHelper.isPopoverVisible()){this._popoverHelper.hidePopover();event.consume();}}},_editBreakpointCondition:function(lineNumber,breakpoint)
{this._conditionElement=this._createConditionElement(lineNumber);this.textEditor.addDecoration(lineNumber,this._conditionElement);function finishEditing(committed,element,newText)
{this.textEditor.removeDecoration(lineNumber,this._conditionElement);delete this._conditionEditorElement;delete this._conditionElement;if(!committed)
return;if(breakpoint)
breakpoint.setCondition(newText);else
this._createNewBreakpoint(lineNumber,0,newText,true);}
var config=new WebInspector.InplaceEditor.Config(finishEditing.bind(this,true),finishEditing.bind(this,false));WebInspector.InplaceEditor.startEditing(this._conditionEditorElement,config);this._conditionEditorElement.value=breakpoint?breakpoint.condition():"";this._conditionEditorElement.select();},_createConditionElement:function(lineNumber)
{var conditionElement=createElementWithClass("div","source-frame-breakpoint-condition");var labelElement=conditionElement.createChild("label","source-frame-breakpoint-message");labelElement.htmlFor="source-frame-breakpoint-condition";labelElement.createTextChild(WebInspector.UIString("The breakpoint on line %d will stop only if this expression is true:",lineNumber+1));var editorElement=conditionElement.createChild("input","monospace");editorElement.id="source-frame-breakpoint-condition";editorElement.type="text";this._conditionEditorElement=editorElement;return conditionElement;},setExecutionLocation:function(uiLocation)
{this._executionLocation=uiLocation;if(!this.loaded)
return;this.textEditor.setExecutionLocation(uiLocation.lineNumber,uiLocation.columnNumber);if(this.isShowing()){setImmediate(this._generateValuesInSource.bind(this));}},_generateValuesInSource:function()
{if(!WebInspector.moduleSetting("inlineVariableValues").get())
return;var executionContext=WebInspector.context.flavor(WebInspector.ExecutionContext);if(!executionContext)
return;var callFrame=executionContext.debuggerModel.selectedCallFrame();if(!callFrame)
return;var localScope=callFrame.localScope();var functionLocation=callFrame.functionLocation();if(localScope&&functionLocation)
localScope.object().getAllProperties(false,this._prepareScopeVariables.bind(this,callFrame));if(this._clearValueWidgetsTimer){clearTimeout(this._clearValueWidgetsTimer);delete this._clearValueWidgetsTimer;}},_prepareScopeVariables:function(callFrame,properties,internalProperties)
{if(!properties||!properties.length||properties.length>500){this._clearValueWidgets();return;}
var functionUILocation=WebInspector.debuggerWorkspaceBinding.rawLocationToUILocation((callFrame.functionLocation()));var executionUILocation=WebInspector.debuggerWorkspaceBinding.rawLocationToUILocation(callFrame.location());if(functionUILocation.uiSourceCode!==this._uiSourceCode||executionUILocation.uiSourceCode!==this._uiSourceCode){this._clearValueWidgets();return;}
var fromLine=functionUILocation.lineNumber;var fromColumn=functionUILocation.columnNumber;var toLine=executionUILocation.lineNumber;if(this._valueWidgets){for(var line of this._valueWidgets.keys())
toLine=Math.max(toLine,line+1);}
if(fromLine>=toLine||toLine-fromLine>500){this._clearValueWidgets();return;}
var valuesMap=new Map();for(var property of properties)
valuesMap.set(property.name,property.value);var namesPerLine=new Map();var tokenizer=new WebInspector.CodeMirrorUtils.TokenizerFactory().createTokenizer("text/javascript");tokenizer(this.textEditor.line(fromLine).substring(fromColumn),processToken.bind(this,fromLine));for(var i=fromLine+1;i<toLine;++i)
tokenizer(this.textEditor.line(i),processToken.bind(this,i));function processToken(lineNumber,tokenValue,tokenType,column,newColumn)
{if(tokenType&&this._isIdentifier(tokenType)&&valuesMap.get(tokenValue)){var names=namesPerLine.get(lineNumber);if(!names){names=new Set();namesPerLine.set(lineNumber,names);}
names.add(tokenValue);}}
this.textEditor.operation(this._renderDecorations.bind(this,valuesMap,namesPerLine,fromLine,toLine));},_renderDecorations:function(valuesMap,namesPerLine,fromLine,toLine)
{var formatter=new WebInspector.RemoteObjectPreviewFormatter();for(var i=fromLine;i<toLine;++i){var names=namesPerLine.get(i);var oldWidget=this._valueWidgets.get(i);if(!names){if(oldWidget){this._valueWidgets.delete(i);this.textEditor.removeDecoration(i,oldWidget);}
continue;}
var widget=createElementWithClass("div","text-editor-value-decoration");var base=this.textEditor.cursorPositionToCoordinates(i,0);var offset=this.textEditor.cursorPositionToCoordinates(i,this.textEditor.line(i).length);var codeMirrorLinesLeftPadding=4;var left=offset.x-base.x+codeMirrorLinesLeftPadding;widget.style.left=left+"px";widget.__nameToToken=new Map();widget.__lineNumber=i;var renderedNameCount=0;for(var name of names){if(renderedNameCount>10)
break;if(namesPerLine.get(i-1)&&namesPerLine.get(i-1).has(name))
continue;if(renderedNameCount)
widget.createTextChild(", ");var nameValuePair=widget.createChild("span");widget.__nameToToken.set(name,nameValuePair);nameValuePair.createTextChild(name+" = ");var value=valuesMap.get(name);var propertyCount=value.preview?value.preview.properties.length:0;var entryCount=value.preview&&value.preview.entries?value.preview.entries.length:0;if(value.preview&&propertyCount+entryCount<10)
formatter.appendObjectPreview(nameValuePair,value.preview);else
nameValuePair.appendChild(WebInspector.ObjectPropertiesSection.createValueElement(value,false));++renderedNameCount;}
var widgetChanged=true;if(oldWidget){widgetChanged=false;for(var name of widget.__nameToToken.keys()){var oldText=oldWidget.__nameToToken.get(name)?oldWidget.__nameToToken.get(name).textContent:"";var newText=widget.__nameToToken.get(name)?widget.__nameToToken.get(name).textContent:"";if(newText!==oldText){widgetChanged=true;WebInspector.runCSSAnimationOnce((widget.__nameToToken.get(name)),"source-frame-value-update-highlight");}}
if(widgetChanged){this._valueWidgets.delete(i);this.textEditor.removeDecoration(i,oldWidget);}}
if(widgetChanged){this._valueWidgets.set(i,widget);this.textEditor.addDecoration(i,widget);}}},clearExecutionLine:function()
{if(this.loaded&&this._executionLocation)
this.textEditor.clearExecutionLine();delete this._executionLocation;this._clearValueWidgetsTimer=setTimeout(this._clearValueWidgets.bind(this),1000);},_clearValueWidgets:function()
{delete this._clearValueWidgetsTimer;for(var line of this._valueWidgets.keys())
this.textEditor.removeDecoration(line,this._valueWidgets.get(line));this._valueWidgets.clear();},_shouldIgnoreExternalBreakpointEvents:function()
{if(this._supportsEnabledBreakpointsWhileEditing())
return false;if(this._muted)
return true;var scriptFiles=this._scriptFileForTarget.valuesArray();for(var i=0;i<scriptFiles.length;++i){if(scriptFiles[i].isDivergingFromVM()||scriptFiles[i].isMergingToVM())
return true;}
return false;},_breakpointAdded:function(event)
{var uiLocation=(event.data.uiLocation);if(uiLocation.uiSourceCode!==this._uiSourceCode)
return;if(this._shouldIgnoreExternalBreakpointEvents())
return;var breakpoint=(event.data.breakpoint);if(this.loaded)
this._addBreakpointDecoration(uiLocation.lineNumber,uiLocation.columnNumber,breakpoint.condition(),breakpoint.enabled(),false);},_breakpointRemoved:function(event)
{var uiLocation=(event.data.uiLocation);if(uiLocation.uiSourceCode!==this._uiSourceCode)
return;if(this._shouldIgnoreExternalBreakpointEvents())
return;var remainingBreakpoint=this._breakpointManager.findBreakpointOnLine(this._uiSourceCode,uiLocation.lineNumber);if(!remainingBreakpoint&&this.loaded)
this._removeBreakpointDecoration(uiLocation.lineNumber);},_onSourceMappingChanged:function(event)
{var data=(event.data);this._updateScriptFile(data.target);this._updateLinesWithoutMappingHighlight();},_updateLinesWithoutMappingHighlight:function()
{var linesCount=this.textEditor.linesCount;for(var i=0;i<linesCount;++i){var lineHasMapping=WebInspector.debuggerWorkspaceBinding.uiLineHasMapping(this._uiSourceCode,i);if(!lineHasMapping)
this._hasLineWithoutMapping=true;if(this._hasLineWithoutMapping)
this.textEditor.toggleLineClass(i,"cm-line-without-source-mapping",!lineHasMapping);}},_updateScriptFile:function(target)
{var oldScriptFile=this._scriptFileForTarget.get(target);var newScriptFile=WebInspector.debuggerWorkspaceBinding.scriptFile(this._uiSourceCode,target);this._scriptFileForTarget.remove(target);if(oldScriptFile){oldScriptFile.removeEventListener(WebInspector.ResourceScriptFile.Events.DidMergeToVM,this._didMergeToVM,this);oldScriptFile.removeEventListener(WebInspector.ResourceScriptFile.Events.DidDivergeFromVM,this._didDivergeFromVM,this);if(this._muted&&!this._uiSourceCode.isDirty())
this._restoreBreakpointsIfConsistentScripts();}
if(newScriptFile)
this._scriptFileForTarget.set(target,newScriptFile);this._updateDivergedInfobar();if(newScriptFile){newScriptFile.addEventListener(WebInspector.ResourceScriptFile.Events.DidMergeToVM,this._didMergeToVM,this);newScriptFile.addEventListener(WebInspector.ResourceScriptFile.Events.DidDivergeFromVM,this._didDivergeFromVM,this);if(this.loaded)
newScriptFile.checkMapping();}},onTextEditorContentLoaded:function()
{WebInspector.UISourceCodeFrame.prototype.onTextEditorContentLoaded.call(this);if(this._executionLocation)
this.setExecutionLocation(this._executionLocation);var breakpointLocations=this._breakpointManager.breakpointLocationsForUISourceCode(this._uiSourceCode);for(var i=0;i<breakpointLocations.length;++i)
this._breakpointAdded({data:breakpointLocations[i]});var scriptFiles=this._scriptFileForTarget.valuesArray();for(var i=0;i<scriptFiles.length;++i)
scriptFiles[i].checkMapping();this._updateLinesWithoutMappingHighlight();},_handleGutterClick:function(event)
{if(this._muted)
return;var eventData=(event.data);var lineNumber=eventData.lineNumber;var eventObject=eventData.event;if(eventObject.button!=0||eventObject.altKey||eventObject.ctrlKey||eventObject.metaKey)
return;this._toggleBreakpoint(lineNumber,eventObject.shiftKey);eventObject.consume(true);},_toggleBreakpoint:function(lineNumber,onlyDisable)
{var breakpoint=this._breakpointManager.findBreakpointOnLine(this._uiSourceCode,lineNumber);if(breakpoint){if(onlyDisable)
breakpoint.setEnabled(!breakpoint.enabled());else
breakpoint.remove();}else
this._createNewBreakpoint(lineNumber,0,"",true);},_createNewBreakpoint:function(lineNumber,columnNumber,condition,enabled)
{this._setBreakpoint(lineNumber,columnNumber,condition,enabled);WebInspector.userMetrics.actionTaken(WebInspector.UserMetrics.Action.ScriptsBreakpointSet);},toggleBreakpointOnCurrentLine:function()
{if(this._muted)
return;var selection=this.textEditor.selection();if(!selection)
return;this._toggleBreakpoint(selection.startLine,false);},_setBreakpoint:function(lineNumber,columnNumber,condition,enabled)
{this._breakpointManager.setBreakpoint(this._uiSourceCode,lineNumber,columnNumber,condition,enabled);},dispose:function()
{this._breakpointManager.removeEventListener(WebInspector.BreakpointManager.Events.BreakpointAdded,this._breakpointAdded,this);this._breakpointManager.removeEventListener(WebInspector.BreakpointManager.Events.BreakpointRemoved,this._breakpointRemoved,this);this._uiSourceCode.removeEventListener(WebInspector.UISourceCode.Events.SourceMappingChanged,this._onSourceMappingChanged,this);this._uiSourceCode.removeEventListener(WebInspector.UISourceCode.Events.WorkingCopyChanged,this._workingCopyChanged,this);this._uiSourceCode.removeEventListener(WebInspector.UISourceCode.Events.WorkingCopyCommitted,this._workingCopyCommitted,this);this._uiSourceCode.removeEventListener(WebInspector.UISourceCode.Events.TitleChanged,this._showBlackboxInfobarIfNeeded,this);WebInspector.moduleSetting("skipStackFramesPattern").removeChangeListener(this._showBlackboxInfobarIfNeeded,this);WebInspector.moduleSetting("skipContentScripts").removeChangeListener(this._showBlackboxInfobarIfNeeded,this);WebInspector.UISourceCodeFrame.prototype.dispose.call(this);},__proto__:WebInspector.UISourceCodeFrame.prototype};WebInspector.CSSSourceFrame=function(uiSourceCode)
{WebInspector.UISourceCodeFrame.call(this,uiSourceCode);this.textEditor.setAutocompleteDelegate(new WebInspector.CSSSourceFrame.AutocompleteDelegate());this._registerShortcuts();}
WebInspector.CSSSourceFrame.prototype={_registerShortcuts:function()
{var shortcutKeys=WebInspector.ShortcutsScreen.SourcesPanelShortcuts;for(var i=0;i<shortcutKeys.IncreaseCSSUnitByOne.length;++i)
this.addShortcut(shortcutKeys.IncreaseCSSUnitByOne[i].key,this._handleUnitModification.bind(this,1));for(var i=0;i<shortcutKeys.DecreaseCSSUnitByOne.length;++i)
this.addShortcut(shortcutKeys.DecreaseCSSUnitByOne[i].key,this._handleUnitModification.bind(this,-1));for(var i=0;i<shortcutKeys.IncreaseCSSUnitByTen.length;++i)
this.addShortcut(shortcutKeys.IncreaseCSSUnitByTen[i].key,this._handleUnitModification.bind(this,10));for(var i=0;i<shortcutKeys.DecreaseCSSUnitByTen.length;++i)
this.addShortcut(shortcutKeys.DecreaseCSSUnitByTen[i].key,this._handleUnitModification.bind(this,-10));},_modifyUnit:function(unit,change)
{var unitValue=parseInt(unit,10);if(isNaN(unitValue))
return null;var tail=unit.substring((unitValue).toString().length);return String.sprintf("%d%s",unitValue+change,tail);},_handleUnitModification:function(change)
{var selection=this.textEditor.selection().normalize();var token=this.textEditor.tokenAtTextPosition(selection.startLine,selection.startColumn);if(!token){if(selection.startColumn>0)
token=this.textEditor.tokenAtTextPosition(selection.startLine,selection.startColumn-1);if(!token)
return false;}
if(token.type!=="css-number")
return false;var cssUnitRange=new WebInspector.TextRange(selection.startLine,token.startColumn,selection.startLine,token.endColumn);var cssUnitText=this.textEditor.copyRange(cssUnitRange);var newUnitText=this._modifyUnit(cssUnitText,change);if(!newUnitText)
return false;this.textEditor.editRange(cssUnitRange,newUnitText);selection.startColumn=token.startColumn;selection.endColumn=selection.startColumn+newUnitText.length;this.textEditor.setSelection(selection);return true;},__proto__:WebInspector.UISourceCodeFrame.prototype}
WebInspector.CSSSourceFrame.AutocompleteDelegate=function()
{this._simpleDelegate=new WebInspector.SimpleAutocompleteDelegate(".-$");}
WebInspector.CSSSourceFrame._backtrackDepth=10;WebInspector.CSSSourceFrame.AutocompleteDelegate.prototype={initialize:function(editor)
{this._simpleDelegate.initialize(editor);},dispose:function()
{this._simpleDelegate.dispose();},substituteRange:function(editor,lineNumber,columnNumber)
{return this._simpleDelegate.substituteRange(editor,lineNumber,columnNumber);},_backtrackPropertyToken:function(editor,lineNumber,columnNumber)
{var tokenPosition=columnNumber;var line=editor.line(lineNumber);var seenColumn=false;for(var i=0;i<WebInspector.CSSSourceFrame._backtrackDepth&&tokenPosition>=0;++i){var token=editor.tokenAtTextPosition(lineNumber,tokenPosition);if(!token)
return null;if(token.type==="css-property")
return seenColumn?token:null;if(token.type&&!(token.type.indexOf("whitespace")!==-1||token.type.startsWith("css-comment")))
return null;if(!token.type&&line.substring(token.startColumn,token.endColumn)===":"){if(!seenColumn)
seenColumn=true;else
return null;}
tokenPosition=token.startColumn-1;}
return null;},wordsWithPrefix:function(editor,prefixRange,substituteRange)
{var prefix=editor.copyRange(prefixRange);if(prefix.startsWith("$"))
return this._simpleDelegate.wordsWithPrefix(editor,prefixRange,substituteRange);var propertyToken=this._backtrackPropertyToken(editor,prefixRange.startLine,prefixRange.startColumn-1);if(!propertyToken)
return this._simpleDelegate.wordsWithPrefix(editor,prefixRange,substituteRange);var line=editor.line(prefixRange.startLine);var tokenContent=line.substring(propertyToken.startColumn,propertyToken.endColumn);var keywords=WebInspector.CSSMetadata.keywordsForProperty(tokenContent);return keywords.startsWith(prefix);},};WebInspector.NavigatorView=function()
{WebInspector.VBox.call(this);this.registerRequiredCSS("sources/navigatorView.css");this.element.classList.add("navigator-container");var scriptsOutlineElement=this.element.createChild("div","navigator");this._scriptsTree=new TreeOutline();this._scriptsTree.setComparator(WebInspector.NavigatorView._treeElementsCompare);this._scriptsTree.element.classList.add("outline-disclosure");scriptsOutlineElement.appendChild(this._scriptsTree.element);this.setDefaultFocusedElement(this._scriptsTree.element);this._uiSourceCodeNodes=new Map();this._subfolderNodes=new Map();this._rootNode=new WebInspector.NavigatorRootTreeNode(this);this._rootNode.populate();this.element.addEventListener("contextmenu",this.handleContextMenu.bind(this),false);}
WebInspector.NavigatorView.Events={ItemSelected:"ItemSelected",ItemRenamed:"ItemRenamed",}
WebInspector.NavigatorView.Types={Root:"Root",Domain:"Domain",Folder:"Folder",UISourceCode:"UISourceCode",FileSystem:"FileSystem"}
WebInspector.NavigatorView.iconClassForType=function(type)
{if(type===WebInspector.NavigatorView.Types.Domain)
return"navigator-domain-tree-item";if(type===WebInspector.NavigatorView.Types.FileSystem)
return"navigator-folder-tree-item";return"navigator-folder-tree-item";}
WebInspector.NavigatorView.appendAddFolderItem=function(contextMenu)
{function addFolder()
{WebInspector.isolatedFileSystemManager.addFileSystem("");}
var addFolderLabel=WebInspector.UIString.capitalize("Add ^folder to ^workspace");contextMenu.appendItem(addFolderLabel,addFolder);}
WebInspector.NavigatorView.prototype={setWorkspace:function(workspace)
{this._workspace=workspace;this._workspace.addEventListener(WebInspector.Workspace.Events.UISourceCodeAdded,this._uiSourceCodeAdded,this);this._workspace.addEventListener(WebInspector.Workspace.Events.UISourceCodeRemoved,this._uiSourceCodeRemoved,this);this._workspace.addEventListener(WebInspector.Workspace.Events.ProjectRemoved,this._projectRemoved.bind(this),this);},wasShown:function()
{if(this._loaded)
return;this._loaded=true;this._workspace.uiSourceCodes().forEach(this._addUISourceCode.bind(this));},accept:function(uiSourceCode)
{return!uiSourceCode.project().isServiceProject();},_addUISourceCode:function(uiSourceCode)
{if(!this.accept(uiSourceCode))
return;var projectNode=this._projectNode(uiSourceCode.project());var folderNode=this._folderNode(projectNode,uiSourceCode.parentPath());var uiSourceCodeNode=new WebInspector.NavigatorUISourceCodeTreeNode(this,uiSourceCode);this._uiSourceCodeNodes.set(uiSourceCode,uiSourceCodeNode);folderNode.appendChild(uiSourceCodeNode);},_uiSourceCodeAdded:function(event)
{var uiSourceCode=(event.data);this._addUISourceCode(uiSourceCode);},_uiSourceCodeRemoved:function(event)
{var uiSourceCode=(event.data);this._removeUISourceCode(uiSourceCode);},_projectRemoved:function(event)
{var project=(event.data);project.removeEventListener(WebInspector.Project.Events.DisplayNameUpdated,this._updateProjectNodeTitle,this);var uiSourceCodes=project.uiSourceCodes();for(var i=0;i<uiSourceCodes.length;++i)
this._removeUISourceCode(uiSourceCodes[i]);},_projectNode:function(project)
{if(!project.displayName())
return this._rootNode;var projectNode=this._rootNode.child(project.id());if(!projectNode){projectNode=this._createProjectNode(project);this._rootNode.appendChild(projectNode);}
return projectNode;},_createProjectNode:function(project)
{var type=project.type()===WebInspector.projectTypes.FileSystem?WebInspector.NavigatorView.Types.FileSystem:WebInspector.NavigatorView.Types.Domain;var projectNode=new WebInspector.NavigatorFolderTreeNode(this,project,project.id(),type,"",project.displayName());project.addEventListener(WebInspector.Project.Events.DisplayNameUpdated,this._updateProjectNodeTitle,this);return projectNode;},_updateProjectNodeTitle:function(event)
{var project=(event.target);var projectNode=this._rootNode.child(project.id());if(!projectNode)
return;projectNode.treeNode().titleText=project.displayName();},_folderNode:function(projectNode,folderPath)
{if(!folderPath)
return projectNode;var subfolderNodes=this._subfolderNodes.get(projectNode);if(!subfolderNodes){subfolderNodes=(new Map());this._subfolderNodes.set(projectNode,subfolderNodes);}
var folderNode=subfolderNodes.get(folderPath);if(folderNode)
return folderNode;var parentNode=projectNode;var index=folderPath.lastIndexOf("/");if(index!==-1)
parentNode=this._folderNode(projectNode,folderPath.substring(0,index));var name=folderPath.substring(index+1);folderNode=new WebInspector.NavigatorFolderTreeNode(this,null,name,WebInspector.NavigatorView.Types.Folder,folderPath,name);subfolderNodes.set(folderPath,folderNode);parentNode.appendChild(folderNode);return folderNode;},revealUISourceCode:function(uiSourceCode,select)
{var node=this._uiSourceCodeNodes.get(uiSourceCode);if(!node)
return;if(this._scriptsTree.selectedTreeElement)
this._scriptsTree.selectedTreeElement.deselect();this._lastSelectedUISourceCode=uiSourceCode;node.reveal(select);},_sourceSelected:function(uiSourceCode,focusSource)
{this._lastSelectedUISourceCode=uiSourceCode;var data={uiSourceCode:uiSourceCode,focusSource:focusSource};this.dispatchEventToListeners(WebInspector.NavigatorView.Events.ItemSelected,data);},sourceDeleted:function(uiSourceCode)
{},_removeUISourceCode:function(uiSourceCode)
{var node=this._uiSourceCodeNodes.get(uiSourceCode);if(!node)
return;var projectNode=this._projectNode(uiSourceCode.project());var subfolderNodes=this._subfolderNodes.get(projectNode);var parentNode=node.parent;this._uiSourceCodeNodes.remove(uiSourceCode);parentNode.removeChild(node);node=parentNode;while(node){parentNode=node.parent;if(!parentNode||!node.isEmpty())
break;if(subfolderNodes)
subfolderNodes.remove(node._folderPath);parentNode.removeChild(node);node=parentNode;}},_updateIcon:function(uiSourceCode)
{var node=this._uiSourceCodeNodes.get(uiSourceCode);node.updateIcon();},reset:function()
{var nodes=this._uiSourceCodeNodes.valuesArray();for(var i=0;i<nodes.length;++i)
nodes[i].dispose();this._scriptsTree.removeChildren();this._uiSourceCodeNodes.clear();this._subfolderNodes.clear();this._rootNode.reset();},handleContextMenu:function(event)
{var contextMenu=new WebInspector.ContextMenu(event);WebInspector.NavigatorView.appendAddFolderItem(contextMenu);contextMenu.show();},_handleContextMenuRefresh:function(project,path)
{project.refresh(path);},_handleContextMenuCreate:function(project,path,uiSourceCode)
{this.create(project,path,uiSourceCode);},_handleContextMenuRename:function(uiSourceCode)
{this.rename(uiSourceCode,false);},_handleContextMenuExclude:function(project,path)
{var shouldExclude=window.confirm(WebInspector.UIString("Are you sure you want to exclude this folder?"));if(shouldExclude){WebInspector.startBatchUpdate();project.excludeFolder(path);WebInspector.endBatchUpdate();}},_handleContextMenuDelete:function(uiSourceCode)
{var shouldDelete=window.confirm(WebInspector.UIString("Are you sure you want to delete this file?"));if(shouldDelete)
uiSourceCode.project().deleteFile(uiSourceCode.path());},handleFileContextMenu:function(event,uiSourceCode)
{var contextMenu=new WebInspector.ContextMenu(event);contextMenu.appendApplicableItems(uiSourceCode);contextMenu.appendSeparator();var project=uiSourceCode.project();if(project.type()===WebInspector.projectTypes.FileSystem){var path=uiSourceCode.parentPath();contextMenu.appendItem(WebInspector.UIString.capitalize("Rename\u2026"),this._handleContextMenuRename.bind(this,uiSourceCode));contextMenu.appendItem(WebInspector.UIString.capitalize("Make a ^copy\u2026"),this._handleContextMenuCreate.bind(this,project,path,uiSourceCode));contextMenu.appendItem(WebInspector.UIString.capitalize("Delete"),this._handleContextMenuDelete.bind(this,uiSourceCode));contextMenu.appendSeparator();}
contextMenu.show();},handleFolderContextMenu:function(event,node)
{var contextMenu=new WebInspector.ContextMenu(event);var path="/";var projectNode=node;while(projectNode.parent!==this._rootNode){path="/"+projectNode.id+path;projectNode=projectNode.parent;}
var project=projectNode._project;if(project.type()===WebInspector.projectTypes.FileSystem){contextMenu.appendItem(WebInspector.UIString.capitalize("Refresh"),this._handleContextMenuRefresh.bind(this,project,path));contextMenu.appendItem(WebInspector.UIString.capitalize("New ^file"),this._handleContextMenuCreate.bind(this,project,path));contextMenu.appendItem(WebInspector.UIString.capitalize("Exclude ^folder"),this._handleContextMenuExclude.bind(this,project,path));}
contextMenu.appendSeparator();WebInspector.NavigatorView.appendAddFolderItem(contextMenu);function removeFolder()
{var shouldRemove=window.confirm(WebInspector.UIString("Are you sure you want to remove this folder?"));if(shouldRemove)
project.remove();}
if(project.type()===WebInspector.projectTypes.FileSystem&&node===projectNode){var removeFolderLabel=WebInspector.UIString.capitalize("Remove ^folder from ^workspace");contextMenu.appendItem(removeFolderLabel,removeFolder);}
contextMenu.show();},rename:function(uiSourceCode,deleteIfCanceled)
{var node=this._uiSourceCodeNodes.get(uiSourceCode);console.assert(node);node.rename(callback.bind(this));function callback(committed)
{if(!committed){if(deleteIfCanceled)
uiSourceCode.remove();return;}
this.dispatchEventToListeners(WebInspector.NavigatorView.Events.ItemRenamed,uiSourceCode);this._updateIcon(uiSourceCode);this._sourceSelected(uiSourceCode,true);}},create:function(project,path,uiSourceCodeToCopy)
{var filePath;var uiSourceCode;function contentLoaded(content)
{createFile.call(this,content||"");}
if(uiSourceCodeToCopy)
uiSourceCodeToCopy.requestContent(contentLoaded.bind(this));else
createFile.call(this);function createFile(content)
{project.createFile(path,null,content||"",fileCreated.bind(this));}
function fileCreated(path)
{if(!path)
return;filePath=path;uiSourceCode=project.uiSourceCode(filePath);if(!uiSourceCode){console.assert(uiSourceCode);return;}
this._sourceSelected(uiSourceCode,false);this.revealUISourceCode(uiSourceCode,true);this.rename(uiSourceCode,true);}},__proto__:WebInspector.VBox.prototype}
WebInspector.SourcesNavigatorView=function()
{WebInspector.NavigatorView.call(this);WebInspector.targetManager.addEventListener(WebInspector.TargetManager.Events.InspectedURLChanged,this._inspectedURLChanged,this);}
WebInspector.SourcesNavigatorView.prototype={accept:function(uiSourceCode)
{if(!WebInspector.NavigatorView.prototype.accept(uiSourceCode))
return false;return uiSourceCode.project().type()!==WebInspector.projectTypes.ContentScripts&&uiSourceCode.project().type()!==WebInspector.projectTypes.Snippets;},_inspectedURLChanged:function(event)
{var nodes=this._uiSourceCodeNodes.valuesArray();for(var i=0;i<nodes.length;++i){var uiSourceCode=nodes[i].uiSourceCode();var inspectedPageURL=WebInspector.targetManager.inspectedPageURL();if(inspectedPageURL&&WebInspector.networkMapping.networkURL(uiSourceCode)===inspectedPageURL)
this.revealUISourceCode(uiSourceCode,true);}},_addUISourceCode:function(uiSourceCode)
{WebInspector.NavigatorView.prototype._addUISourceCode.call(this,uiSourceCode);var inspectedPageURL=WebInspector.targetManager.inspectedPageURL();if(inspectedPageURL&&WebInspector.networkMapping.networkURL(uiSourceCode)===inspectedPageURL)
this.revealUISourceCode(uiSourceCode,true);},__proto__:WebInspector.NavigatorView.prototype}
WebInspector.ContentScriptsNavigatorView=function()
{WebInspector.NavigatorView.call(this);}
WebInspector.ContentScriptsNavigatorView.prototype={accept:function(uiSourceCode)
{if(!WebInspector.NavigatorView.prototype.accept(uiSourceCode))
return false;return uiSourceCode.project().type()===WebInspector.projectTypes.ContentScripts;},__proto__:WebInspector.NavigatorView.prototype}
WebInspector.NavigatorView._treeElementsCompare=function compare(treeElement1,treeElement2)
{function typeWeight(treeElement)
{var type=treeElement.type();if(type===WebInspector.NavigatorView.Types.Domain){if(treeElement.titleText===WebInspector.targetManager.inspectedPageDomain())
return 1;return 2;}
if(type===WebInspector.NavigatorView.Types.FileSystem)
return 3;if(type===WebInspector.NavigatorView.Types.Folder)
return 4;return 5;}
var typeWeight1=typeWeight(treeElement1);var typeWeight2=typeWeight(treeElement2);var result;if(typeWeight1>typeWeight2)
result=1;else if(typeWeight1<typeWeight2)
result=-1;else{var title1=treeElement1.titleText;var title2=treeElement2.titleText;result=title1.compareTo(title2);}
return result;}
WebInspector.BaseNavigatorTreeElement=function(type,title,iconClasses,expandable,noIcon)
{this._type=type;TreeElement.call(this,"",expandable);this._titleText=title;this._iconClasses=iconClasses;this._noIcon=noIcon;}
WebInspector.BaseNavigatorTreeElement.prototype={onattach:function()
{this.listItemElement.removeChildren();if(this._iconClasses){for(var i=0;i<this._iconClasses.length;++i)
this.listItemElement.classList.add(this._iconClasses[i]);}
this.listItemElement.createChild("div","selection fill");if(!this._noIcon)
this.imageElement=this.listItemElement.createChild("img","icon");this.titleElement=this.listItemElement.createChild("div","base-navigator-tree-element-title");this.titleElement.textContent=this._titleText;},updateIconClasses:function(iconClasses)
{for(var i=0;i<this._iconClasses.length;++i)
this.listItemElement.classList.remove(this._iconClasses[i]);this._iconClasses=iconClasses;for(var i=0;i<this._iconClasses.length;++i)
this.listItemElement.classList.add(this._iconClasses[i]);},onreveal:function()
{if(this.listItemElement)
this.listItemElement.scrollIntoViewIfNeeded(true);},get titleText()
{return this._titleText;},set titleText(titleText)
{if(this._titleText===titleText)
return;this._titleText=titleText||"";if(this.titleElement){this.titleElement.textContent=this._titleText;this.titleElement.title=this._titleText;}},type:function()
{return this._type;},__proto__:TreeElement.prototype}
WebInspector.NavigatorFolderTreeElement=function(navigatorView,type,title)
{var iconClass=WebInspector.NavigatorView.iconClassForType(type);WebInspector.BaseNavigatorTreeElement.call(this,type,title,[iconClass],true);this._navigatorView=navigatorView;}
WebInspector.NavigatorFolderTreeElement.prototype={onpopulate:function()
{this._node.populate();},onattach:function()
{WebInspector.BaseNavigatorTreeElement.prototype.onattach.call(this);this.collapse();this.listItemElement.addEventListener("contextmenu",this._handleContextMenuEvent.bind(this),false);},setNode:function(node)
{this._node=node;var paths=[];while(node&&!node.isRoot()){paths.push(node._title);node=node.parent;}
paths.reverse();this.tooltip=paths.join("/");},_handleContextMenuEvent:function(event)
{if(!this._node)
return;this.select();this._navigatorView.handleFolderContextMenu(event,this._node);},__proto__:WebInspector.BaseNavigatorTreeElement.prototype}
WebInspector.NavigatorSourceTreeElement=function(navigatorView,uiSourceCode,title)
{this._navigatorView=navigatorView;this._uiSourceCode=uiSourceCode;WebInspector.BaseNavigatorTreeElement.call(this,WebInspector.NavigatorView.Types.UISourceCode,title,this._calculateIconClasses(),false);this.tooltip=uiSourceCode.originURL();}
WebInspector.NavigatorSourceTreeElement.prototype={get uiSourceCode()
{return this._uiSourceCode;},_calculateIconClasses:function()
{return["navigator-"+this._uiSourceCode.contentType().name()+"-tree-item"];},updateIcon:function()
{this.updateIconClasses(this._calculateIconClasses());},onattach:function()
{WebInspector.BaseNavigatorTreeElement.prototype.onattach.call(this);this.listItemElement.draggable=true;this.listItemElement.addEventListener("click",this._onclick.bind(this),false);this.listItemElement.addEventListener("contextmenu",this._handleContextMenuEvent.bind(this),false);this.listItemElement.addEventListener("mousedown",this._onmousedown.bind(this),false);this.listItemElement.addEventListener("dragstart",this._ondragstart.bind(this),false);},_onmousedown:function(event)
{if(event.which===1)
this._uiSourceCode.requestContent(callback.bind(this));function callback(content)
{this._warmedUpContent=content;}},_shouldRenameOnMouseDown:function()
{if(!this._uiSourceCode.canRename())
return false;var isSelected=this===this.treeOutline.selectedTreeElement;var document=this.treeOutline.element.ownerDocument;var isFocused=this.treeOutline.element.isSelfOrAncestor(document.activeElement);return isSelected&&isFocused&&!WebInspector.isBeingEdited(this.treeOutline.element);},selectOnMouseDown:function(event)
{if(event.which!==1||!this._shouldRenameOnMouseDown()){TreeElement.prototype.selectOnMouseDown.call(this,event);return;}
setTimeout(rename.bind(this),300);function rename()
{if(this._shouldRenameOnMouseDown())
this._navigatorView.rename(this.uiSourceCode,false);}},_ondragstart:function(event)
{event.dataTransfer.setData("text/plain",this._warmedUpContent);event.dataTransfer.effectAllowed="copy";return true;},onspace:function()
{this._navigatorView._sourceSelected(this.uiSourceCode,true);return true;},_onclick:function(event)
{this._navigatorView._sourceSelected(this.uiSourceCode,false);},ondblclick:function(event)
{var middleClick=event.button===1;this._navigatorView._sourceSelected(this.uiSourceCode,!middleClick);return false;},onenter:function()
{this._navigatorView._sourceSelected(this.uiSourceCode,true);return true;},ondelete:function()
{this._navigatorView.sourceDeleted(this.uiSourceCode);return true;},_handleContextMenuEvent:function(event)
{this.select();this._navigatorView.handleFileContextMenu(event,this._uiSourceCode);},__proto__:WebInspector.BaseNavigatorTreeElement.prototype}
WebInspector.NavigatorTreeNode=function(id)
{this.id=id;this._children=new Map();}
WebInspector.NavigatorTreeNode.prototype={treeNode:function(){throw"Not implemented";},dispose:function(){},isRoot:function()
{return false;},hasChildren:function()
{return true;},populate:function()
{if(this.isPopulated())
return;if(this.parent)
this.parent.populate();this._populated=true;this.wasPopulated();},wasPopulated:function()
{var children=this.children();for(var i=0;i<children.length;++i)
this.treeNode().appendChild((children[i].treeNode()));},didAddChild:function(node)
{if(this.isPopulated())
this.treeNode().appendChild((node.treeNode()));},willRemoveChild:function(node)
{if(this.isPopulated())
this.treeNode().removeChild((node.treeNode()));},isPopulated:function()
{return this._populated;},isEmpty:function()
{return!this._children.size;},child:function(id)
{return this._children.get(id)||null;},children:function()
{return this._children.valuesArray();},appendChild:function(node)
{this._children.set(node.id,node);node.parent=this;this.didAddChild(node);},removeChild:function(node)
{this.willRemoveChild(node);this._children.remove(node.id);delete node.parent;node.dispose();},reset:function()
{this._children.clear();}}
WebInspector.NavigatorRootTreeNode=function(navigatorView)
{WebInspector.NavigatorTreeNode.call(this,"");this._navigatorView=navigatorView;}
WebInspector.NavigatorRootTreeNode.prototype={isRoot:function()
{return true;},treeNode:function()
{return this._navigatorView._scriptsTree.rootElement();},__proto__:WebInspector.NavigatorTreeNode.prototype}
WebInspector.NavigatorUISourceCodeTreeNode=function(navigatorView,uiSourceCode)
{WebInspector.NavigatorTreeNode.call(this,uiSourceCode.name());this._navigatorView=navigatorView;this._uiSourceCode=uiSourceCode;this._treeElement=null;}
WebInspector.NavigatorUISourceCodeTreeNode.prototype={uiSourceCode:function()
{return this._uiSourceCode;},updateIcon:function()
{if(this._treeElement)
this._treeElement.updateIcon();},treeNode:function()
{if(this._treeElement)
return this._treeElement;this._treeElement=new WebInspector.NavigatorSourceTreeElement(this._navigatorView,this._uiSourceCode,"");this.updateTitle();this._uiSourceCode.addEventListener(WebInspector.UISourceCode.Events.TitleChanged,this._titleChanged,this);this._uiSourceCode.addEventListener(WebInspector.UISourceCode.Events.WorkingCopyChanged,this._workingCopyChanged,this);this._uiSourceCode.addEventListener(WebInspector.UISourceCode.Events.WorkingCopyCommitted,this._workingCopyCommitted,this);return this._treeElement;},updateTitle:function(ignoreIsDirty)
{if(!this._treeElement)
return;var titleText=this._uiSourceCode.displayName();if(!ignoreIsDirty&&(this._uiSourceCode.isDirty()||this._uiSourceCode.hasUnsavedCommittedChanges()))
titleText="*"+titleText;this._treeElement.titleText=titleText;},hasChildren:function()
{return false;},dispose:function()
{if(!this._treeElement)
return;this._uiSourceCode.removeEventListener(WebInspector.UISourceCode.Events.TitleChanged,this._titleChanged,this);this._uiSourceCode.removeEventListener(WebInspector.UISourceCode.Events.WorkingCopyChanged,this._workingCopyChanged,this);this._uiSourceCode.removeEventListener(WebInspector.UISourceCode.Events.WorkingCopyCommitted,this._workingCopyCommitted,this);},_titleChanged:function(event)
{this.updateTitle();},_workingCopyChanged:function(event)
{this.updateTitle();},_workingCopyCommitted:function(event)
{this.updateTitle();},reveal:function(select)
{this.parent.populate();this.parent.treeNode().expand();this._treeElement.reveal();if(select)
this._treeElement.select(true);},rename:function(callback)
{if(!this._treeElement)
return;var treeOutlineElement=this._treeElement.treeOutline.element;WebInspector.markBeingEdited(treeOutlineElement,true);function commitHandler(element,newTitle,oldTitle)
{if(newTitle!==oldTitle){this._treeElement.titleText=newTitle;this._uiSourceCode.rename(newTitle,renameCallback.bind(this));return;}
afterEditing.call(this,true);}
function renameCallback(success)
{if(!success){WebInspector.markBeingEdited(treeOutlineElement,false);this.updateTitle();this.rename(callback);return;}
afterEditing.call(this,true);}
function cancelHandler()
{afterEditing.call(this,false);}
function afterEditing(committed)
{WebInspector.markBeingEdited(treeOutlineElement,false);this.updateTitle();this._treeElement.treeOutline.focus();if(callback)
callback(committed);}
var editingConfig=new WebInspector.InplaceEditor.Config(commitHandler.bind(this),cancelHandler.bind(this));this.updateTitle(true);WebInspector.InplaceEditor.startEditing(this._treeElement.titleElement,editingConfig);treeOutlineElement.getComponentSelection().setBaseAndExtent(this._treeElement.titleElement,0,this._treeElement.titleElement,1);},__proto__:WebInspector.NavigatorTreeNode.prototype}
WebInspector.NavigatorFolderTreeNode=function(navigatorView,project,id,type,folderPath,title)
{WebInspector.NavigatorTreeNode.call(this,id);this._navigatorView=navigatorView;this._project=project;this._type=type;this._folderPath=folderPath;this._title=title;}
WebInspector.NavigatorFolderTreeNode.prototype={treeNode:function()
{if(this._treeElement)
return this._treeElement;this._treeElement=this._createTreeElement(this._title,this);return this._treeElement;},_createTreeElement:function(title,node)
{var treeElement=new WebInspector.NavigatorFolderTreeElement(this._navigatorView,this._type,title);treeElement.setNode(node);return treeElement;},wasPopulated:function()
{if(!this._treeElement||this._treeElement._node!==this)
return;this._addChildrenRecursive();},_addChildrenRecursive:function()
{var children=this.children();for(var i=0;i<children.length;++i){var child=children[i];this.didAddChild(child);if(child instanceof WebInspector.NavigatorFolderTreeNode)
child._addChildrenRecursive();}},_shouldMerge:function(node)
{return this._type!==WebInspector.NavigatorView.Types.Domain&&node instanceof WebInspector.NavigatorFolderTreeNode;},didAddChild:function(node)
{function titleForNode(node)
{return node._title;}
if(!this._treeElement)
return;var children=this.children();if(children.length===1&&this._shouldMerge(node)){node._isMerged=true;this._treeElement.titleText=this._treeElement.titleText+"/"+node._title;node._treeElement=this._treeElement;this._treeElement.setNode(node);return;}
var oldNode;if(children.length===2)
oldNode=children[0]!==node?children[0]:children[1];if(oldNode&&oldNode._isMerged){delete oldNode._isMerged;var mergedToNodes=[];mergedToNodes.push(this);var treeNode=this;while(treeNode._isMerged){treeNode=treeNode.parent;mergedToNodes.push(treeNode);}
mergedToNodes.reverse();var titleText=mergedToNodes.map(titleForNode).join("/");var nodes=[];treeNode=oldNode;do{nodes.push(treeNode);children=treeNode.children();treeNode=children.length===1?children[0]:null;}while(treeNode&&treeNode._isMerged);if(!this.isPopulated()){this._treeElement.titleText=titleText;this._treeElement.setNode(this);for(var i=0;i<nodes.length;++i){delete nodes[i]._treeElement;delete nodes[i]._isMerged;}
return;}
var oldTreeElement=this._treeElement;var treeElement=this._createTreeElement(titleText,this);for(var i=0;i<mergedToNodes.length;++i)
mergedToNodes[i]._treeElement=treeElement;oldTreeElement.parent.appendChild(treeElement);oldTreeElement.setNode(nodes[nodes.length-1]);oldTreeElement.titleText=nodes.map(titleForNode).join("/");oldTreeElement.parent.removeChild(oldTreeElement);this._treeElement.appendChild(oldTreeElement);if(oldTreeElement.expanded)
treeElement.expand();}
if(this.isPopulated())
this._treeElement.appendChild(node.treeNode());},willRemoveChild:function(node)
{if(node._isMerged||!this.isPopulated())
return;this._treeElement.removeChild(node._treeElement);},__proto__:WebInspector.NavigatorTreeNode.prototype};WebInspector.RevisionHistoryView=function()
{WebInspector.VBox.call(this);this.registerRequiredCSS("sources/revisionHistory.css");this.element.classList.add("revision-history-drawer");this._uiSourceCodeItems=new Map();this._treeOutline=new TreeOutline();this._treeOutline.element.classList.add("outline-disclosure");this.element.appendChild(this._treeOutline.element);function populateRevisions(uiSourceCode)
{if(uiSourceCode.history.length)
this._createUISourceCodeItem(uiSourceCode);}
WebInspector.workspace.uiSourceCodes().forEach(populateRevisions.bind(this));WebInspector.workspace.addEventListener(WebInspector.Workspace.Events.WorkingCopyCommittedByUser,this._revisionAdded,this);WebInspector.workspace.addEventListener(WebInspector.Workspace.Events.UISourceCodeRemoved,this._uiSourceCodeRemoved,this);WebInspector.workspace.addEventListener(WebInspector.Workspace.Events.ProjectRemoved,this._projectRemoved,this);}
WebInspector.RevisionHistoryView.showHistory=function(uiSourceCode)
{function revealSource(view)
{console.assert(view&&view instanceof WebInspector.RevisionHistoryView);var historyView=(view);historyView._revealUISourceCode(uiSourceCode);}
WebInspector.inspectorView.showViewInDrawer("sources.history").then(revealSource);}
WebInspector.RevisionHistoryView.prototype={_createUISourceCodeItem:function(uiSourceCode)
{var uiSourceCodeItem=new TreeElement(uiSourceCode.displayName(),true);uiSourceCodeItem.selectable=false;var rootElement=this._treeOutline.rootElement();for(var i=0;i<rootElement.childCount();++i){if(rootElement.childAt(i).title.localeCompare(uiSourceCode.displayName())>0){rootElement.insertChild(uiSourceCodeItem,i);break;}}
if(i===rootElement.childCount())
rootElement.appendChild(uiSourceCodeItem);this._uiSourceCodeItems.set(uiSourceCode,uiSourceCodeItem);var revisionCount=uiSourceCode.history.length;for(var i=revisionCount-1;i>=0;--i){var revision=uiSourceCode.history[i];var historyItem=new WebInspector.RevisionHistoryTreeElement(revision,uiSourceCode.history[i-1],i!==revisionCount-1);uiSourceCodeItem.appendChild(historyItem);}
var linkItem=new TreeElement();linkItem.selectable=false;uiSourceCodeItem.appendChild(linkItem);var revertToOriginal=linkItem.listItemElement.createChild("span","revision-history-link revision-history-link-row");revertToOriginal.textContent=WebInspector.UIString("apply original content");revertToOriginal.addEventListener("click",this._revertToOriginal.bind(this,uiSourceCode));var clearHistoryElement=uiSourceCodeItem.listItemElement.createChild("span","revision-history-link");clearHistoryElement.textContent=WebInspector.UIString("revert");clearHistoryElement.addEventListener("click",this._clearHistory.bind(this,uiSourceCode));return uiSourceCodeItem;},_revertToOriginal:function(uiSourceCode)
{uiSourceCode.revertToOriginal();},_clearHistory:function(uiSourceCode)
{uiSourceCode.revertAndClearHistory(this._removeUISourceCode.bind(this));},_revisionAdded:function(event)
{var uiSourceCode=(event.data.uiSourceCode);var uiSourceCodeItem=this._uiSourceCodeItems.get(uiSourceCode);if(!uiSourceCodeItem){uiSourceCodeItem=this._createUISourceCodeItem(uiSourceCode);return;}
var historyLength=uiSourceCode.history.length;var historyItem=new WebInspector.RevisionHistoryTreeElement(uiSourceCode.history[historyLength-1],uiSourceCode.history[historyLength-2],false);if(uiSourceCodeItem.firstChild())
uiSourceCodeItem.firstChild().allowRevert();uiSourceCodeItem.insertChild(historyItem,0);},_revealUISourceCode:function(uiSourceCode)
{var uiSourceCodeItem=this._uiSourceCodeItems.get(uiSourceCode);if(uiSourceCodeItem){uiSourceCodeItem.reveal();uiSourceCodeItem.expand();}},_uiSourceCodeRemoved:function(event)
{var uiSourceCode=(event.data);this._removeUISourceCode(uiSourceCode);},_removeUISourceCode:function(uiSourceCode)
{var uiSourceCodeItem=this._uiSourceCodeItems.get(uiSourceCode);if(!uiSourceCodeItem)
return;this._treeOutline.removeChild(uiSourceCodeItem);this._uiSourceCodeItems.remove(uiSourceCode);},_projectRemoved:function(event)
{var project=event.data;project.uiSourceCodes().forEach(this._removeUISourceCode.bind(this));},__proto__:WebInspector.VBox.prototype}
WebInspector.RevisionHistoryTreeElement=function(revision,baseRevision,allowRevert)
{TreeElement.call(this,revision.timestamp.toLocaleTimeString(),true);this.selectable=false;this._revision=revision;this._baseRevision=baseRevision;this._revertElement=createElement("span");this._revertElement.className="revision-history-link";this._revertElement.textContent=WebInspector.UIString("apply revision content");this._revertElement.addEventListener("click",this._revision.revertToThis.bind(this._revision),false);if(!allowRevert)
this._revertElement.classList.add("hidden");}
WebInspector.RevisionHistoryTreeElement.prototype={onattach:function()
{this.listItemElement.classList.add("revision-history-revision");},onpopulate:function()
{this.listItemElement.appendChild(this._revertElement);this.childrenListElement.classList.add("source-code");if(this._baseRevision)
this._baseRevision.requestContent(step1.bind(this));else
this._revision.uiSourceCode.requestOriginalContent(step1.bind(this));function step1(baseContent)
{this._revision.requestContent(step2.bind(this,baseContent));}
function step2(baseContent,newContent)
{var baseLines=baseContent.split("\n");var newLines=newContent.split("\n");var opcodes=WebInspector.Diff.lineDiff(baseLines,newLines);var lastWasSeparator=false;var baseLineNumber=0;var newLineNumber=0;for(var idx=0;idx<opcodes.length;idx++){var code=opcodes[idx][0];var rowCount=opcodes[idx][1].length;if(code===WebInspector.Diff.Operation.Equal){baseLineNumber+=rowCount;newLineNumber+=rowCount;if(!lastWasSeparator)
this._createLine(null,null,"    \u2026","separator");lastWasSeparator=true;}else if(code===WebInspector.Diff.Operation.Delete){lastWasSeparator=false;for(var i=0;i<rowCount;++i)
this._createLine(baseLineNumber+i,null,baseLines[baseLineNumber+i],"removed");baseLineNumber+=rowCount;}else if(code===WebInspector.Diff.Operation.Insert){lastWasSeparator=false;for(var i=0;i<rowCount;++i)
this._createLine(null,newLineNumber+i,newLines[newLineNumber+i],"added");newLineNumber+=rowCount;}}}},oncollapse:function()
{this._revertElement.remove();},_createLine:function(baseLineNumber,newLineNumber,lineContent,changeType)
{var child=new TreeElement();child.selectable=false;this.appendChild(child);function appendLineNumber(lineNumber)
{var numberString=lineNumber!==null?numberToStringWithSpacesPadding(lineNumber+1,4):spacesPadding(4);var lineNumberSpan=createElement("span");lineNumberSpan.classList.add("webkit-line-number");lineNumberSpan.textContent=numberString;child.listItemElement.appendChild(lineNumberSpan);}
appendLineNumber(baseLineNumber);appendLineNumber(newLineNumber);var contentSpan=createElement("span");contentSpan.textContent=lineContent;child.listItemElement.appendChild(contentSpan);child.listItemElement.classList.add("revision-history-line");contentSpan.classList.add("revision-history-line-"+changeType);},allowRevert:function()
{this._revertElement.classList.remove("hidden");},__proto__:TreeElement.prototype};WebInspector.ScopeChainSidebarPane=function()
{WebInspector.SidebarPane.call(this,WebInspector.UIString("Scope"));this._sections=[];this._expandedSections=new Set();this._expandedProperties=new Set();}
WebInspector.ScopeChainSidebarPane._pathSymbol=Symbol("path");WebInspector.ScopeChainSidebarPane.prototype={update:function(callFrame)
{this.element.removeChildren();if(!callFrame){var infoElement=createElement("div");infoElement.className="info";infoElement.textContent=WebInspector.UIString("Not Paused");this.element.appendChild(infoElement);return;}
for(var i=0;i<this._sections.length;++i){var section=this._sections[i];if(!section.title)
continue;if(section.expanded)
this._expandedSections.add(section.title);else
this._expandedSections.delete(section.title);}
this._sections=[];var foundLocalScope=false;var scopeChain=callFrame.scopeChain();for(var i=0;i<scopeChain.length;++i){var scope=scopeChain[i];var title=null;var emptyPlaceholder=null;var extraProperties=[];switch(scope.type()){case DebuggerAgent.ScopeType.Local:foundLocalScope=true;title=WebInspector.UIString("Local");emptyPlaceholder=WebInspector.UIString("No Variables");var thisObject=callFrame.thisObject();if(thisObject)
extraProperties.push(new WebInspector.RemoteObjectProperty("this",thisObject));if(i==0){var details=callFrame.debuggerModel.debuggerPausedDetails();if(!callFrame.isAsync()){var exception=details.exception();if(exception)
extraProperties.push(new WebInspector.RemoteObjectProperty(WebInspector.UIString.capitalize("Exception"),exception,undefined,undefined,undefined,undefined,undefined,true));}
var returnValue=callFrame.returnValue();if(returnValue)
extraProperties.push(new WebInspector.RemoteObjectProperty(WebInspector.UIString.capitalize("Return ^value"),returnValue,undefined,undefined,undefined,undefined,undefined,true));}
break;case DebuggerAgent.ScopeType.Closure:var scopeName=scope.name();if(scopeName)
title=WebInspector.UIString("Closure (%s)",WebInspector.beautifyFunctionName(scopeName));else
title=WebInspector.UIString("Closure");emptyPlaceholder=WebInspector.UIString("No Variables");break;case DebuggerAgent.ScopeType.Catch:title=WebInspector.UIString("Catch");break;case DebuggerAgent.ScopeType.Block:title=WebInspector.UIString("Block");break;case DebuggerAgent.ScopeType.Script:title=WebInspector.UIString("Script");break;case DebuggerAgent.ScopeType.With:title=WebInspector.UIString("With Block");break;case DebuggerAgent.ScopeType.Global:title=WebInspector.UIString("Global");break;}
var subtitle=scope.description();if(!title||title===subtitle)
subtitle=undefined;var titleElement=createElementWithClass("div");titleElement.createChild("div","scope-chain-sidebar-pane-section-subtitle").textContent=subtitle;titleElement.createChild("div","scope-chain-sidebar-pane-section-title").textContent=title;var section=new WebInspector.ObjectPropertiesSection(scope.object(),titleElement,emptyPlaceholder,true,extraProperties);section[WebInspector.ScopeChainSidebarPane._pathSymbol]=title+":"+(subtitle?subtitle+":":"");section.addEventListener(TreeOutline.Events.ElementAttached,this._elementAttached,this);section.addEventListener(TreeOutline.Events.ElementExpanded,this._elementExpanded,this);section.addEventListener(TreeOutline.Events.ElementCollapsed,this._elementCollapsed,this);if(scope.type()===DebuggerAgent.ScopeType.Global)
section.objectTreeElement().collapse();else if(!foundLocalScope||scope.type()===DebuggerAgent.ScopeType.Local||this._expandedSections.has(title))
section.objectTreeElement().expand();section.element.classList.add("scope-chain-sidebar-pane-section");this._sections.push(section);this.element.appendChild(section.element);}},_elementAttached:function(event)
{var element=(event.data);if(element.isExpandable()&&this._expandedProperties.has(this._propertyPath(element)))
element.expand();},_elementExpanded:function(event)
{var element=(event.data);this._expandedProperties.add(this._propertyPath(element));},_elementCollapsed:function(event)
{var element=(event.data);this._expandedProperties.delete(this._propertyPath(element));},_propertyPath:function(treeElement)
{return treeElement.treeOutline[WebInspector.ScopeChainSidebarPane._pathSymbol]+WebInspector.ObjectPropertyTreeElement.prototype.propertyPath.call(treeElement);},__proto__:WebInspector.SidebarPane.prototype};WebInspector.ServiceWorkersSidebarPane=function()
{WebInspector.SidebarPane.call(this,WebInspector.UIString("\u2699 Service Workers"));this.registerRequiredCSS("sources/serviceWorkersSidebar.css");this._bodyElement=this.element.createChild("div","vbox");this.setVisible(false);this._manager=null;WebInspector.targetManager.observeTargets(this,WebInspector.Target.Type.Page);this._placeholderElement=createElementWithClass("div","info");this._placeholderElement.textContent=WebInspector.UIString("No service workers control this page");this._versionIdCheckBoxMap=new Map();}
WebInspector.ServiceWorkersSidebarPane.prototype={targetAdded:function(target)
{if(this._manager)
return;this._manager=target.serviceWorkerManager;this._updateVisibility();target.serviceWorkerManager.addEventListener(WebInspector.ServiceWorkerManager.Events.WorkersUpdated,this._update,this);target.serviceWorkerManager.addEventListener(WebInspector.ServiceWorkerManager.Events.RegistrationUpdated,this._registrationUpdated,this);},targetRemoved:function(target)
{target.serviceWorkerManager.removeEventListener(WebInspector.ServiceWorkerManager.Events.WorkersUpdated,this._update,this);target.serviceWorkerManager.removeEventListener(WebInspector.ServiceWorkerManager.Events.RegistrationUpdated,this._registrationUpdated,this);this._updateVisibility();},_update:function()
{this._updateVisibility();this._bodyElement.removeChildren();this._versionIdCheckBoxMap.clear();if(!this.isShowing()||!this._manager)
return;if(!this._manager.hasWorkers()){this._bodyElement.appendChild(this._placeholderElement);return;}
for(var worker of this._manager.workers()){var workerElement=this._bodyElement.createChild("div","service-worker");var leftBox=workerElement.createChild("div","vbox flex-auto");leftBox.appendChild(WebInspector.linkifyURLAsNode(worker.url(),worker.name()));var scopeElement=leftBox.createChild("span","service-worker-scope");scopeElement.textContent=worker.scope();scopeElement.title=worker.scope();var forceUpdateOnPageLoadCheckboxLabel=createCheckboxLabel(WebInspector.UIString("Force update on page load"));var forceUpdateOnPageLoadCheckbox=forceUpdateOnPageLoadCheckboxLabel.checkboxElement;this._versionIdCheckBoxMap.set(worker.versionId(),forceUpdateOnPageLoadCheckbox);forceUpdateOnPageLoadCheckbox.addEventListener("click",this._forceUpdateOnPageLoadCheckboxClicked.bind(this,forceUpdateOnPageLoadCheckbox,worker.versionId()),false);var version=this._manager.findVersion(worker.versionId());if(version)
forceUpdateOnPageLoadCheckbox.checked=version.registration.forceUpdateOnPageLoad;leftBox.appendChild(forceUpdateOnPageLoadCheckboxLabel);workerElement.appendChild(createTextButton(WebInspector.UIString("Unregister"),worker.stop.bind(worker)));}},_registrationUpdated:function(event)
{var registration=(event.data);for(var version of registration.versions.values()){var checkBox=this._versionIdCheckBoxMap.get(version.id);if(checkBox)
checkBox.checked=registration.forceUpdateOnPageLoad;}},_forceUpdateOnPageLoadCheckboxClicked:function(checkbox,versionId,event)
{event.preventDefault()
var version=this._manager.findVersion(versionId);if(!version)
return;this._manager.setForceUpdateOnPageLoad(version.registration.id,checkbox.checked);},_updateVisibility:function()
{this._wasVisibleAtLeastOnce=this._wasVisibleAtLeastOnce||!!this._manager&&this._manager.hasWorkers();this.setVisible(this._wasVisibleAtLeastOnce);},wasShown:function()
{this._update();},__proto__:WebInspector.SidebarPane.prototype};WebInspector.SourcesNavigator=function(workspace)
{WebInspector.Object.call(this);this._workspace=workspace;this._tabbedPane=new WebInspector.TabbedPane();this._tabbedPane.setShrinkableTabs(true);this._tabbedPane.element.classList.add("navigator-tabbed-pane");this._tabbedPaneController=new WebInspector.ExtensibleTabbedPaneController(this._tabbedPane,"navigator-view",this._navigatorViewCreated.bind(this));this._navigatorViews=new Map();}
WebInspector.SourcesNavigator.Events={SourceSelected:"SourceSelected",SourceRenamed:"SourceRenamed"}
WebInspector.SourcesNavigator.prototype={_navigatorViewCreated:function(id,view)
{var navigatorView=(view);navigatorView.addEventListener(WebInspector.NavigatorView.Events.ItemSelected,this._sourceSelected,this);navigatorView.addEventListener(WebInspector.NavigatorView.Events.ItemRenamed,this._sourceRenamed,this);this._navigatorViews.set(id,navigatorView);navigatorView.setWorkspace(this._workspace);},get view()
{return this._tabbedPane;},revealUISourceCode:function(uiSourceCode)
{var ids=this._tabbedPaneController.viewIds();var promises=[];for(var i=0;i<ids.length;++i)
promises.push(this._tabbedPaneController.viewForId(ids[i]));Promise.all(promises).then(filterNavigators.bind(this));function filterNavigators(objects)
{for(var i=0;i<objects.length;++i){var navigatorView=(objects[i]);if(navigatorView.accept(uiSourceCode)){this._tabbedPane.selectTab(ids[i]);navigatorView.revealUISourceCode(uiSourceCode,true);}}}},_sourceSelected:function(event)
{this.dispatchEventToListeners(WebInspector.SourcesNavigator.Events.SourceSelected,event.data);},_sourceRenamed:function(event)
{this.dispatchEventToListeners(WebInspector.SourcesNavigator.Events.SourceRenamed,event.data);},__proto__:WebInspector.Object.prototype}
WebInspector.SnippetsNavigatorView=function()
{WebInspector.NavigatorView.call(this);}
WebInspector.SnippetsNavigatorView.prototype={accept:function(uiSourceCode)
{if(!WebInspector.NavigatorView.prototype.accept(uiSourceCode))
return false;return uiSourceCode.project().type()===WebInspector.projectTypes.Snippets;},handleContextMenu:function(event)
{var contextMenu=new WebInspector.ContextMenu(event);contextMenu.appendItem(WebInspector.UIString("New"),this._handleCreateSnippet.bind(this));contextMenu.show();},handleFileContextMenu:function(event,uiSourceCode)
{var contextMenu=new WebInspector.ContextMenu(event);contextMenu.appendItem(WebInspector.UIString("Run"),this._handleEvaluateSnippet.bind(this,uiSourceCode));contextMenu.appendItem(WebInspector.UIString("Rename"),this.rename.bind(this,uiSourceCode));contextMenu.appendItem(WebInspector.UIString("Remove"),this._handleRemoveSnippet.bind(this,uiSourceCode));contextMenu.appendSeparator();contextMenu.appendItem(WebInspector.UIString("New"),this._handleCreateSnippet.bind(this));contextMenu.show();},_handleEvaluateSnippet:function(uiSourceCode)
{var executionContext=WebInspector.context.flavor(WebInspector.ExecutionContext);if(uiSourceCode.project().type()!==WebInspector.projectTypes.Snippets||!executionContext)
return;WebInspector.scriptSnippetModel.evaluateScriptSnippet(executionContext,uiSourceCode);},_handleRemoveSnippet:function(uiSourceCode)
{if(uiSourceCode.project().type()!==WebInspector.projectTypes.Snippets)
return;uiSourceCode.remove();},_handleCreateSnippet:function()
{this.create(WebInspector.scriptSnippetModel.project(),"");},sourceDeleted:function(uiSourceCode)
{this._handleRemoveSnippet(uiSourceCode);},__proto__:WebInspector.NavigatorView.prototype};WebInspector.StyleSheetOutlineDialog=function(uiSourceCode,selectItemCallback)
{WebInspector.SelectionDialogContentProvider.call(this);this._selectItemCallback=selectItemCallback;this._cssParser=new WebInspector.CSSParser();this._cssParser.addEventListener(WebInspector.CSSParser.Events.RulesParsed,this.refresh.bind(this));this._cssParser.parse(uiSourceCode.workingCopy());}
WebInspector.StyleSheetOutlineDialog.show=function(uiSourceCode,selectItemCallback)
{WebInspector.StyleSheetOutlineDialog._instanceForTests=new WebInspector.StyleSheetOutlineDialog(uiSourceCode,selectItemCallback);new WebInspector.FilteredItemSelectionDialog(WebInspector.StyleSheetOutlineDialog._instanceForTests,false);}
WebInspector.StyleSheetOutlineDialog.prototype={itemCount:function()
{return this._cssParser.rules().length;},itemKeyAt:function(itemIndex)
{var rule=this._cssParser.rules()[itemIndex];return rule.selectorText||rule.atRule;},itemScoreAt:function(itemIndex,query)
{var rule=this._cssParser.rules()[itemIndex];return-rule.lineNumber;},renderItem:function(itemIndex,query,titleElement,subtitleElement)
{var rule=this._cssParser.rules()[itemIndex];titleElement.textContent=rule.selectorText||rule.atRule;this.highlightRanges(titleElement,query);subtitleElement.textContent=":"+(rule.lineNumber+1);},selectItem:function(itemIndex,promptValue)
{var rule=this._cssParser.rules()[itemIndex];var lineNumber=rule.lineNumber;if(!isNaN(lineNumber)&&lineNumber>=0)
this._selectItemCallback(lineNumber,rule.columnNumber);},dispose:function()
{this._cssParser.dispose();},__proto__:WebInspector.SelectionDialogContentProvider.prototype};WebInspector.TabbedEditorContainerDelegate=function(){}
WebInspector.TabbedEditorContainerDelegate.prototype={viewForFile:function(uiSourceCode){},}
WebInspector.TabbedEditorContainer=function(delegate,setting,placeholderText)
{WebInspector.Object.call(this);this._delegate=delegate;this._tabbedPane=new WebInspector.TabbedPane();this._tabbedPane.setPlaceholderText(placeholderText);this._tabbedPane.setTabDelegate(new WebInspector.EditorContainerTabDelegate(this));this._tabbedPane.setCloseableTabs(true);this._tabbedPane.setAllowTabReorder(true,true);this._tabbedPane.insertBeforeTabStrip(createElementWithClass("div","sources-editor-tabstrip-left"));this._tabbedPane.appendAfterTabStrip(createElementWithClass("div","sources-editor-tabstrip-right"));this._tabbedPane.addEventListener(WebInspector.TabbedPane.EventTypes.TabClosed,this._tabClosed,this);this._tabbedPane.addEventListener(WebInspector.TabbedPane.EventTypes.TabSelected,this._tabSelected,this);this._tabIds=new Map();this._files={};this._previouslyViewedFilesSetting=setting;this._history=WebInspector.TabbedEditorContainer.History.fromObject(this._previouslyViewedFilesSetting.get());}
WebInspector.TabbedEditorContainer.Events={EditorSelected:"EditorSelected",EditorClosed:"EditorClosed"}
WebInspector.TabbedEditorContainer._tabId=0;WebInspector.TabbedEditorContainer.maximalPreviouslyViewedFilesCount=30;WebInspector.TabbedEditorContainer.prototype={get view()
{return this._tabbedPane;},get visibleView()
{return this._tabbedPane.visibleView;},fileViews:function()
{return(this._tabbedPane.tabViews());},show:function(parentElement)
{this._tabbedPane.show(parentElement);},showFile:function(uiSourceCode)
{this._innerShowFile(uiSourceCode,true);},closeFile:function(uiSourceCode)
{var tabId=this._tabIds.get(uiSourceCode);if(!tabId)
return;this._closeTabs([tabId]);},historyUISourceCodes:function()
{var uriToUISourceCode={};for(var id in this._files){var uiSourceCode=this._files[id];uriToUISourceCode[uiSourceCode.uri()]=uiSourceCode;}
var result=[];var uris=this._history._urls();for(var i=0;i<uris.length;++i){var uiSourceCode=uriToUISourceCode[uris[i]];if(uiSourceCode)
result.push(uiSourceCode);}
return result;},_addViewListeners:function()
{if(!this._currentView)
return;this._currentView.addEventListener(WebInspector.SourceFrame.Events.ScrollChanged,this._scrollChanged,this);this._currentView.addEventListener(WebInspector.SourceFrame.Events.SelectionChanged,this._selectionChanged,this);},_removeViewListeners:function()
{if(!this._currentView)
return;this._currentView.removeEventListener(WebInspector.SourceFrame.Events.ScrollChanged,this._scrollChanged,this);this._currentView.removeEventListener(WebInspector.SourceFrame.Events.SelectionChanged,this._selectionChanged,this);},_scrollChanged:function(event)
{var lineNumber=(event.data);this._history.updateScrollLineNumber(this._currentFile.uri(),lineNumber);this._history.save(this._previouslyViewedFilesSetting);},_selectionChanged:function(event)
{var range=(event.data);this._history.updateSelectionRange(this._currentFile.uri(),range);this._history.save(this._previouslyViewedFilesSetting);},_innerShowFile:function(uiSourceCode,userGesture)
{if(this._currentFile===uiSourceCode)
return;this._removeViewListeners();this._currentFile=uiSourceCode;var tabId=this._tabIds.get(uiSourceCode)||this._appendFileTab(uiSourceCode,userGesture);this._tabbedPane.selectTab(tabId,userGesture);if(userGesture)
this._editorSelectedByUserAction();this._currentView=this.visibleView;this._addViewListeners();var eventData={currentFile:this._currentFile,userGesture:userGesture};this.dispatchEventToListeners(WebInspector.TabbedEditorContainer.Events.EditorSelected,eventData);},_titleForFile:function(uiSourceCode)
{var maxDisplayNameLength=30;var title=uiSourceCode.displayName(true).trimMiddle(maxDisplayNameLength);if(uiSourceCode.isDirty()||uiSourceCode.hasUnsavedCommittedChanges())
title+="*";return title;},_maybeCloseTab:function(id,nextTabId)
{var uiSourceCode=this._files[id];var shouldPrompt=uiSourceCode.isDirty()&&uiSourceCode.project().canSetFileContent();if(!shouldPrompt||confirm(WebInspector.UIString("Are you sure you want to close unsaved file: %s?",uiSourceCode.name()))){uiSourceCode.resetWorkingCopy();if(nextTabId)
this._tabbedPane.selectTab(nextTabId,true);this._tabbedPane.closeTab(id,true);return true;}
return false;},_closeTabs:function(ids)
{var dirtyTabs=[];var cleanTabs=[];for(var i=0;i<ids.length;++i){var id=ids[i];var uiSourceCode=this._files[id];if(uiSourceCode.isDirty())
dirtyTabs.push(id);else
cleanTabs.push(id);}
if(dirtyTabs.length)
this._tabbedPane.selectTab(dirtyTabs[0],true);this._tabbedPane.closeTabs(cleanTabs,true);for(var i=0;i<dirtyTabs.length;++i){var nextTabId=i+1<dirtyTabs.length?dirtyTabs[i+1]:null;if(!this._maybeCloseTab(dirtyTabs[i],nextTabId))
break;}},_onContextMenu:function(tabId,contextMenu)
{var uiSourceCode=this._files[tabId];if(uiSourceCode)
contextMenu.appendApplicableItems(uiSourceCode);},addUISourceCode:function(uiSourceCode)
{var uri=uiSourceCode.uri();var index=this._history.index(uri);if(index===-1)
return;if(!this._tabIds.has(uiSourceCode))
this._appendFileTab(uiSourceCode,false);if(!index){this._innerShowFile(uiSourceCode,false);return;}
if(!this._currentFile)
return;var currentProjectType=this._currentFile.project().type();var addedProjectType=uiSourceCode.project().type();var snippetsProjectType=WebInspector.projectTypes.Snippets;if(this._history.index(this._currentFile.uri())&&currentProjectType===snippetsProjectType&&addedProjectType!==snippetsProjectType)
this._innerShowFile(uiSourceCode,false);},removeUISourceCode:function(uiSourceCode)
{this.removeUISourceCodes([uiSourceCode]);},removeUISourceCodes:function(uiSourceCodes)
{var tabIds=[];for(var i=0;i<uiSourceCodes.length;++i){var uiSourceCode=uiSourceCodes[i];var tabId=this._tabIds.get(uiSourceCode);if(tabId)
tabIds.push(tabId);}
this._tabbedPane.closeTabs(tabIds);},_editorClosedByUserAction:function(uiSourceCode)
{this._history.remove(uiSourceCode.uri());this._updateHistory();},_editorSelectedByUserAction:function()
{this._updateHistory();},_updateHistory:function()
{var tabIds=this._tabbedPane.lastOpenedTabIds(WebInspector.TabbedEditorContainer.maximalPreviouslyViewedFilesCount);function tabIdToURI(tabId)
{return this._files[tabId].uri();}
this._history.update(tabIds.map(tabIdToURI.bind(this)));this._history.save(this._previouslyViewedFilesSetting);},_tooltipForFile:function(uiSourceCode)
{return uiSourceCode.originURL();},_appendFileTab:function(uiSourceCode,userGesture)
{var view=this._delegate.viewForFile(uiSourceCode);var title=this._titleForFile(uiSourceCode);var tooltip=this._tooltipForFile(uiSourceCode);var tabId=this._generateTabId();this._tabIds.set(uiSourceCode,tabId);this._files[tabId]=uiSourceCode;var savedSelectionRange=this._history.selectionRange(uiSourceCode.uri());if(savedSelectionRange)
view.setSelection(savedSelectionRange);var savedScrollLineNumber=this._history.scrollLineNumber(uiSourceCode.uri());if(savedScrollLineNumber)
view.scrollToLine(savedScrollLineNumber);this._tabbedPane.appendTab(tabId,title,view,tooltip,userGesture);this._updateFileTitle(uiSourceCode);this._addUISourceCodeListeners(uiSourceCode);return tabId;},_tabClosed:function(event)
{var tabId=(event.data.tabId);var userGesture=(event.data.isUserGesture);var uiSourceCode=this._files[tabId];if(this._currentFile===uiSourceCode){this._removeViewListeners();delete this._currentView;delete this._currentFile;}
this._tabIds.remove(uiSourceCode);delete this._files[tabId];this._removeUISourceCodeListeners(uiSourceCode);this.dispatchEventToListeners(WebInspector.TabbedEditorContainer.Events.EditorClosed,uiSourceCode);if(userGesture)
this._editorClosedByUserAction(uiSourceCode);},_tabSelected:function(event)
{var tabId=(event.data.tabId);var userGesture=(event.data.isUserGesture);var uiSourceCode=this._files[tabId];this._innerShowFile(uiSourceCode,userGesture);},_addUISourceCodeListeners:function(uiSourceCode)
{uiSourceCode.addEventListener(WebInspector.UISourceCode.Events.TitleChanged,this._uiSourceCodeTitleChanged,this);uiSourceCode.addEventListener(WebInspector.UISourceCode.Events.WorkingCopyChanged,this._uiSourceCodeWorkingCopyChanged,this);uiSourceCode.addEventListener(WebInspector.UISourceCode.Events.WorkingCopyCommitted,this._uiSourceCodeWorkingCopyCommitted,this);},_removeUISourceCodeListeners:function(uiSourceCode)
{uiSourceCode.removeEventListener(WebInspector.UISourceCode.Events.TitleChanged,this._uiSourceCodeTitleChanged,this);uiSourceCode.removeEventListener(WebInspector.UISourceCode.Events.WorkingCopyChanged,this._uiSourceCodeWorkingCopyChanged,this);uiSourceCode.removeEventListener(WebInspector.UISourceCode.Events.WorkingCopyCommitted,this._uiSourceCodeWorkingCopyCommitted,this);},_updateFileTitle:function(uiSourceCode)
{var tabId=this._tabIds.get(uiSourceCode);if(tabId){var title=this._titleForFile(uiSourceCode);this._tabbedPane.changeTabTitle(tabId,title);if(uiSourceCode.hasUnsavedCommittedChanges())
this._tabbedPane.setTabIcon(tabId,"warning-icon",WebInspector.UIString("Changes to this file were not saved to file system."));else
this._tabbedPane.setTabIcon(tabId,"");}},_uiSourceCodeTitleChanged:function(event)
{var uiSourceCode=(event.target);this._updateFileTitle(uiSourceCode);this._updateHistory();},_uiSourceCodeWorkingCopyChanged:function(event)
{var uiSourceCode=(event.target);this._updateFileTitle(uiSourceCode);},_uiSourceCodeWorkingCopyCommitted:function(event)
{var uiSourceCode=(event.target);this._updateFileTitle(uiSourceCode);},_generateTabId:function()
{return"tab_"+(WebInspector.TabbedEditorContainer._tabId++);},currentFile:function()
{return this._currentFile;},__proto__:WebInspector.Object.prototype}
WebInspector.TabbedEditorContainer.HistoryItem=function(url,selectionRange,scrollLineNumber)
{this.url=url;this._isSerializable=url.length<WebInspector.TabbedEditorContainer.HistoryItem.serializableUrlLengthLimit;this.selectionRange=selectionRange;this.scrollLineNumber=scrollLineNumber;}
WebInspector.TabbedEditorContainer.HistoryItem.serializableUrlLengthLimit=4096;WebInspector.TabbedEditorContainer.HistoryItem.fromObject=function(serializedHistoryItem)
{var selectionRange=serializedHistoryItem.selectionRange?WebInspector.TextRange.fromObject(serializedHistoryItem.selectionRange):undefined;return new WebInspector.TabbedEditorContainer.HistoryItem(serializedHistoryItem.url,selectionRange,serializedHistoryItem.scrollLineNumber);}
WebInspector.TabbedEditorContainer.HistoryItem.prototype={serializeToObject:function()
{if(!this._isSerializable)
return null;var serializedHistoryItem={};serializedHistoryItem.url=this.url;serializedHistoryItem.selectionRange=this.selectionRange;serializedHistoryItem.scrollLineNumber=this.scrollLineNumber;return serializedHistoryItem;}}
WebInspector.TabbedEditorContainer.History=function(items)
{this._items=items;this._rebuildItemIndex();}
WebInspector.TabbedEditorContainer.History.fromObject=function(serializedHistory)
{var items=[];for(var i=0;i<serializedHistory.length;++i)
items.push(WebInspector.TabbedEditorContainer.HistoryItem.fromObject(serializedHistory[i]));return new WebInspector.TabbedEditorContainer.History(items);}
WebInspector.TabbedEditorContainer.History.prototype={index:function(url)
{var index=this._itemsIndex[url];if(typeof index==="number")
return index;return-1;},_rebuildItemIndex:function()
{this._itemsIndex={};for(var i=0;i<this._items.length;++i){console.assert(!this._itemsIndex.hasOwnProperty(this._items[i].url));this._itemsIndex[this._items[i].url]=i;}},selectionRange:function(url)
{var index=this.index(url);return index!==-1?this._items[index].selectionRange:undefined;},updateSelectionRange:function(url,selectionRange)
{if(!selectionRange)
return;var index=this.index(url);if(index===-1)
return;this._items[index].selectionRange=selectionRange;},scrollLineNumber:function(url)
{var index=this.index(url);return index!==-1?this._items[index].scrollLineNumber:undefined;},updateScrollLineNumber:function(url,scrollLineNumber)
{var index=this.index(url);if(index===-1)
return;this._items[index].scrollLineNumber=scrollLineNumber;},update:function(urls)
{for(var i=urls.length-1;i>=0;--i){var index=this.index(urls[i]);var item;if(index!==-1){item=this._items[index];this._items.splice(index,1);}else
item=new WebInspector.TabbedEditorContainer.HistoryItem(urls[i]);this._items.unshift(item);this._rebuildItemIndex();}},remove:function(url)
{var index=this.index(url);if(index!==-1){this._items.splice(index,1);this._rebuildItemIndex();}},save:function(setting)
{setting.set(this._serializeToObject());},_serializeToObject:function()
{var serializedHistory=[];for(var i=0;i<this._items.length;++i){var serializedItem=this._items[i].serializeToObject();if(serializedItem)
serializedHistory.push(serializedItem);if(serializedHistory.length===WebInspector.TabbedEditorContainer.maximalPreviouslyViewedFilesCount)
break;}
return serializedHistory;},_urls:function()
{var result=[];for(var i=0;i<this._items.length;++i)
result.push(this._items[i].url);return result;}}
WebInspector.EditorContainerTabDelegate=function(editorContainer)
{this._editorContainer=editorContainer;}
WebInspector.EditorContainerTabDelegate.prototype={closeTabs:function(tabbedPane,ids)
{this._editorContainer._closeTabs(ids);},onContextMenu:function(tabId,contextMenu)
{this._editorContainer._onContextMenu(tabId,contextMenu);}};WebInspector.WatchExpressionsSidebarPane=function()
{WebInspector.SidebarPane.call(this,WebInspector.UIString("Watch"));this.registerRequiredCSS("components/objectValue.css");this._requiresUpdate=true;this._watchExpressions=[];this._watchExpressionsSetting=WebInspector.settings.createLocalSetting("watchExpressions",[]);var addButton=new WebInspector.ToolbarButton(WebInspector.UIString("Add expression"),"add-toolbar-item");addButton.addEventListener("click",this._addButtonClicked.bind(this));this.toolbar().appendToolbarItem(addButton);var refreshButton=new WebInspector.ToolbarButton(WebInspector.UIString("Refresh"),"refresh-toolbar-item");refreshButton.addEventListener("click",this._refreshButtonClicked.bind(this));this.toolbar().appendToolbarItem(refreshButton);this._bodyElement=this.element.createChild("div","vbox watch-expressions");this._bodyElement.addEventListener("contextmenu",this._contextMenu.bind(this),false);WebInspector.context.addFlavorChangeListener(WebInspector.ExecutionContext,this.refreshExpressions,this);}
WebInspector.WatchExpressionsSidebarPane.prototype={wasShown:function()
{this._refreshExpressionsIfNeeded();},refreshExpressions:function()
{this._requiresUpdate=true;this._refreshExpressionsIfNeeded();},addExpression:function(expressionString)
{this.expand();if(this._requiresUpdate){this._rebuildWatchExpressions();delete this._requiresUpdate;}
this._createWatchExpression(expressionString);this._saveExpressions();},expandIfNecessary:function()
{if(this._watchExpressionsSetting.get().length)
this.expand();},_saveExpressions:function()
{var toSave=[];for(var i=0;i<this._watchExpressions.length;i++)
if(this._watchExpressions[i].expression())
toSave.push(this._watchExpressions[i].expression());this._watchExpressionsSetting.set(toSave);},_refreshExpressionsIfNeeded:function()
{if(this._requiresUpdate&&this.isShowing()){this._rebuildWatchExpressions();delete this._requiresUpdate;}else
this._requiresUpdate=true;},_addButtonClicked:function(event)
{if(event)
event.consume(true);this.expand();this._createWatchExpression(null).startEditing();},_refreshButtonClicked:function(event)
{event.consume();this.refreshExpressions();},_rebuildWatchExpressions:function()
{this._bodyElement.removeChildren();this._watchExpressions=[];this._emptyElement=this._bodyElement.createChild("div","info");this._emptyElement.textContent=WebInspector.UIString("No Watch Expressions");var watchExpressionStrings=this._watchExpressionsSetting.get();for(var i=0;i<watchExpressionStrings.length;++i){var expression=watchExpressionStrings[i];if(!expression)
continue;this._createWatchExpression(expression);}},_createWatchExpression:function(expression)
{this._emptyElement.classList.add("hidden");var watchExpression=new WebInspector.WatchExpression(expression);watchExpression.addEventListener(WebInspector.WatchExpression.Events.ExpressionUpdated,this._watchExpressionUpdated.bind(this));this._bodyElement.appendChild(watchExpression.element());this._watchExpressions.push(watchExpression);return watchExpression;},_watchExpressionUpdated:function(event)
{var watchExpression=(event.target);if(!watchExpression.expression()){this._watchExpressions.remove(watchExpression);this._bodyElement.removeChild(watchExpression.element());this._emptyElement.classList.toggle("hidden",!!this._watchExpressions.length);}
this._saveExpressions();},_contextMenu:function(event)
{var contextMenu=new WebInspector.ContextMenu(event);this._populateContextMenu(contextMenu,event);contextMenu.show();},_populateContextMenu:function(contextMenu,event)
{var isEditing=false;for(var watchExpression of this._watchExpressions)
isEditing|=watchExpression.isEditing();if(!isEditing)
contextMenu.appendItem(WebInspector.UIString.capitalize("Add ^watch ^expression"),this._addButtonClicked.bind(this));if(this._watchExpressions.length>1)
contextMenu.appendItem(WebInspector.UIString.capitalize("Delete ^all ^watch ^expressions"),this._deleteAllButtonClicked.bind(this));for(var watchExpression of this._watchExpressions)
if(watchExpression.element().containsEventPoint(event))
watchExpression._populateContextMenu(contextMenu,event);},_deleteAllButtonClicked:function()
{this._watchExpressions=[];this._saveExpressions();this._rebuildWatchExpressions();},__proto__:WebInspector.SidebarPane.prototype}
WebInspector.WatchExpression=function(expression)
{this._expression=expression;this._element=createElementWithClass("div","watch-expression monospace");this._editing=false;this._createWatchExpression(null,false);this.update();}
WebInspector.WatchExpression._watchObjectGroupId="watch-group";WebInspector.WatchExpression.Events={ExpressionUpdated:"ExpressionUpdated"}
WebInspector.WatchExpression.prototype={element:function()
{return this._element;},expression:function()
{return this._expression;},update:function()
{var currentExecutionContext=WebInspector.context.flavor(WebInspector.ExecutionContext);if(currentExecutionContext&&this._expression)
currentExecutionContext.evaluate(this._expression,WebInspector.WatchExpression._watchObjectGroupId,false,true,false,false,this._createWatchExpression.bind(this));},startEditing:function()
{this._editing=true;this._element.removeChild(this._objectPresentationElement);var newDiv=this._element.createChild("div");newDiv.textContent=this._nameElement.textContent;this._textPrompt=new WebInspector.ObjectPropertyPrompt();this._textPrompt.renderAsBlock();var proxyElement=this._textPrompt.attachAndStartEditing(newDiv,this._finishEditing.bind(this));proxyElement.classList.add("watch-expression-text-prompt-proxy");proxyElement.addEventListener("keydown",this._promptKeyDown.bind(this),false);this._element.getComponentSelection().setBaseAndExtent(newDiv,0,newDiv,1);},isEditing:function()
{return!!this._editing;},_finishEditing:function(event,canceled)
{if(event)
event.consume(true);this._editing=false;this._textPrompt.detach();var newExpression=canceled?this._expression:this._textPrompt.text();delete this._textPrompt;this._element.removeChildren();this._element.appendChild(this._objectPresentationElement);this._updateExpression(newExpression);},_dblClickOnWatchExpression:function(event)
{event.consume();if(!this.isEditing())
this.startEditing();},_updateExpression:function(newExpression)
{this._expression=newExpression;this.update();this.dispatchEventToListeners(WebInspector.WatchExpression.Events.ExpressionUpdated);},_deleteWatchExpression:function(event)
{event.consume(true);this._updateExpression(null);},_createWatchExpression:function(result,wasThrown)
{this._result=result;var headerElement=createElementWithClass("div","watch-expression-header");var deleteButton=headerElement.createChild("button","watch-expression-delete-button");deleteButton.title=WebInspector.UIString("Delete watch expression");deleteButton.addEventListener("click",this._deleteWatchExpression.bind(this),false);var titleElement=headerElement.createChild("div","watch-expression-title");this._nameElement=WebInspector.ObjectPropertiesSection.createNameElement(this._expression);if(wasThrown||!result){this._valueElement=createElementWithClass("span","error-message value");titleElement.classList.add("dimmed");this._valueElement.textContent=WebInspector.UIString("<not available>");}else{this._valueElement=WebInspector.ObjectPropertiesSection.createValueElementWithCustomSupport(result,wasThrown,titleElement);}
var separatorElement=createElementWithClass("span","watch-expressions-separator");separatorElement.textContent=": ";titleElement.appendChildren(this._nameElement,separatorElement,this._valueElement);this._element.removeChildren();this._objectPropertiesSection=null;if(!wasThrown&&result&&result.hasChildren&&!result.customPreview()){this._objectPropertiesSection=new WebInspector.ObjectPropertiesSection(result,headerElement);this._objectPresentationElement=this._objectPropertiesSection.element;var objectTreeElement=this._objectPropertiesSection.objectTreeElement();objectTreeElement.toggleOnClick=false;objectTreeElement.listItemElement.addEventListener("click",this._onSectionClick.bind(this),false);objectTreeElement.listItemElement.addEventListener("dblclick",this._dblClickOnWatchExpression.bind(this));}else{this._objectPresentationElement=headerElement;this._objectPresentationElement.addEventListener("dblclick",this._dblClickOnWatchExpression.bind(this));}
this._element.appendChild(this._objectPresentationElement);},_onSectionClick:function(event)
{event.consume(true);if(event.detail==1){this._preventClickTimeout=setTimeout(handleClick.bind(this),333);}else{clearTimeout(this._preventClickTimeout);delete this._preventClickTimeout;}
function handleClick()
{if(!this._objectPropertiesSection)
return;var objectTreeElement=this._objectPropertiesSection.objectTreeElement();if(objectTreeElement.expanded)
objectTreeElement.collapse();else
objectTreeElement.expand();}},_promptKeyDown:function(event)
{if(isEnterKey(event)||isEscKey(event))
this._finishEditing(event,isEscKey(event));},_populateContextMenu:function(contextMenu,event)
{if(!this.isEditing())
contextMenu.appendItem(WebInspector.UIString.capitalize("Delete ^watch ^expression"),this._updateExpression.bind(this,null));if(!this.isEditing()&&this._result&&(this._result.type==="number"||this._result.type==="string"))
contextMenu.appendItem(WebInspector.UIString.capitalize("Copy ^value"),this._copyValueButtonClicked.bind(this));if(this._valueElement.containsEventPoint(event))
contextMenu.appendApplicableItems(this._result);},_copyValueButtonClicked:function()
{InspectorFrontendHost.copyText(this._valueElement.textContent);},__proto__:WebInspector.Object.prototype};WebInspector.ThreadsSidebarPane=function()
{WebInspector.SidebarPane.call(this,WebInspector.UIString("Threads"));this.setVisible(false);this._debuggerModelToListItems=new Map();this._listItemsToTargets=new Map();this._selectedListItem=null;this.threadList=new WebInspector.UIList();this.threadList.show(this.element);WebInspector.targetManager.addModelListener(WebInspector.DebuggerModel,WebInspector.DebuggerModel.Events.DebuggerPaused,this._onDebuggerStateChanged,this);WebInspector.targetManager.addModelListener(WebInspector.DebuggerModel,WebInspector.DebuggerModel.Events.DebuggerResumed,this._onDebuggerStateChanged,this);WebInspector.context.addFlavorChangeListener(WebInspector.Target,this._targetChanged,this);WebInspector.targetManager.observeTargets(this);}
WebInspector.ThreadsSidebarPane.prototype={targetAdded:function(target)
{var debuggerModel=WebInspector.DebuggerModel.fromTarget(target)
if(!debuggerModel){this._updateVisibility();return;}
var listItem=new WebInspector.UIList.Item(target.name(),"");listItem.element.addEventListener("click",this._onListItemClick.bind(this,listItem),false);var currentTarget=WebInspector.context.flavor(WebInspector.Target);if(currentTarget===target)
this._selectListItem(listItem);this._debuggerModelToListItems.set(debuggerModel,listItem);this._listItemsToTargets.set(listItem,target);this.threadList.addItem(listItem);this._updateDebuggerState(debuggerModel);this._updateVisibility();},_updateVisibility:function()
{this._wasVisibleAtLeastOnce=this._wasVisibleAtLeastOnce||this._debuggerModelToListItems.size>1;this.setVisible(this._wasVisibleAtLeastOnce);},targetRemoved:function(target)
{var debuggerModel=WebInspector.DebuggerModel.fromTarget(target)
if(!debuggerModel)
return;var listItem=this._debuggerModelToListItems.remove(debuggerModel);if(listItem){this._listItemsToTargets.remove(listItem);this.threadList.removeItem(listItem);}
this._updateVisibility();},_targetChanged:function(event)
{var newTarget=(event.data);var debuggerModel=WebInspector.DebuggerModel.fromTarget(newTarget)
if(!debuggerModel)
return;var listItem=(this._debuggerModelToListItems.get(debuggerModel));this._selectListItem(listItem);},_onDebuggerStateChanged:function(event)
{var debuggerModel=(event.target);this._updateDebuggerState(debuggerModel);},_updateDebuggerState:function(debuggerModel)
{var listItem=this._debuggerModelToListItems.get(debuggerModel);listItem.setSubtitle(WebInspector.UIString(debuggerModel.isPaused()?"paused":""));},_selectListItem:function(listItem)
{if(listItem===this._selectedListItem)
return;if(this._selectedListItem)
this._selectedListItem.setSelected(false);this._selectedListItem=listItem;listItem.setSelected(true);},_onListItemClick:function(listItem)
{WebInspector.context.setFlavor(WebInspector.Target,this._listItemsToTargets.get(listItem));listItem.element.scrollIntoViewIfNeeded();},__proto__:WebInspector.SidebarPane.prototype};WebInspector.FormatterScriptMapping=function(debuggerModel,editorAction)
{this._debuggerModel=debuggerModel;this._editorAction=editorAction;}
WebInspector.FormatterScriptMapping.prototype={rawLocationToUILocation:function(rawLocation)
{var debuggerModelLocation=(rawLocation);var script=debuggerModelLocation.script();var uiSourceCode=this._editorAction._uiSourceCodes.get(script);if(!uiSourceCode)
return null;var formatData=this._editorAction._formatData.get(uiSourceCode);if(!formatData)
return null;var mapping=formatData.mapping;var lineNumber=debuggerModelLocation.lineNumber;var columnNumber=debuggerModelLocation.columnNumber||0;var formattedLocation=mapping.originalToFormatted(lineNumber,columnNumber);return uiSourceCode.uiLocation(formattedLocation[0],formattedLocation[1]);},uiLocationToRawLocation:function(uiSourceCode,lineNumber,columnNumber)
{var formatData=this._editorAction._formatData.get(uiSourceCode);if(!formatData)
return null;var originalLocation=formatData.mapping.formattedToOriginal(lineNumber,columnNumber);for(var i=0;i<formatData.scripts.length;++i){if(formatData.scripts[i].debuggerModel===this._debuggerModel)
return this._debuggerModel.createRawLocation(formatData.scripts[i],originalLocation[0],originalLocation[1]);}
return null;},isIdentity:function()
{return false;},uiLineHasMapping:function(uiSourceCode,lineNumber)
{return true;}}
WebInspector.FormatterScriptMapping.FormatData=function(projectId,path,mapping,scripts)
{this.projectId=projectId;this.path=path;this.mapping=mapping;this.scripts=scripts;}
WebInspector.FormatterProjectDelegate=function(workspace,id)
{WebInspector.ContentProviderBasedProjectDelegate.call(this,workspace,id,WebInspector.projectTypes.Formatter);}
WebInspector.FormatterProjectDelegate.prototype={displayName:function()
{return"formatter";},_addFormatted:function(name,sourceURL,contentType,content)
{var contentProvider=new WebInspector.StaticContentProvider(contentType,content);return this.addContentProvider(sourceURL,name+":formatted",sourceURL,contentProvider);},_removeFormatted:function(path)
{this.removeFile(path);},__proto__:WebInspector.ContentProviderBasedProjectDelegate.prototype}
WebInspector.ScriptFormatterEditorAction=function()
{this._projectId="formatter:";this._projectDelegate=new WebInspector.FormatterProjectDelegate(WebInspector.workspace,this._projectId);this._uiSourceCodes=new Map();this._formattedPaths=new Map();this._formatData=new Map();this._pathsToFormatOnLoad=new Set();this._scriptMappingByTarget=new Map();this._workspace=WebInspector.workspace;WebInspector.targetManager.observeTargets(this);}
WebInspector.ScriptFormatterEditorAction.prototype={targetAdded:function(target)
{var debuggerModel=WebInspector.DebuggerModel.fromTarget(target);if(!debuggerModel)
return;this._scriptMappingByTarget.set(target,new WebInspector.FormatterScriptMapping(debuggerModel,this));debuggerModel.addEventListener(WebInspector.DebuggerModel.Events.GlobalObjectCleared,this._debuggerReset,this);},targetRemoved:function(target)
{var debuggerModel=WebInspector.DebuggerModel.fromTarget(target);if(!debuggerModel)
return;this._scriptMappingByTarget.remove(target);this._cleanForTarget(target);debuggerModel.removeEventListener(WebInspector.DebuggerModel.Events.GlobalObjectCleared,this._debuggerReset,this);},_editorSelected:function(event)
{var uiSourceCode=(event.data);this._updateButton(uiSourceCode);var path=uiSourceCode.project().id()+":"+uiSourceCode.path();var networkURL=WebInspector.networkMapping.networkURL(uiSourceCode);if(this._isFormatableScript(uiSourceCode)&&networkURL&&this._pathsToFormatOnLoad.has(path)&&!this._formattedPaths.get(path))
this._formatUISourceCodeScript(uiSourceCode);},_editorClosed:function(event)
{var uiSourceCode=(event.data.uiSourceCode);var wasSelected=(event.data.wasSelected);if(wasSelected)
this._updateButton(null);this._discardFormattedUISourceCodeScript(uiSourceCode);},_updateButton:function(uiSourceCode)
{this._button.element.classList.toggle("hidden",!this._isFormatableScript(uiSourceCode));},button:function(sourcesView)
{if(this._button)
return this._button;this._sourcesView=sourcesView;this._sourcesView.addEventListener(WebInspector.SourcesView.Events.EditorSelected,this._editorSelected.bind(this));this._sourcesView.addEventListener(WebInspector.SourcesView.Events.EditorClosed,this._editorClosed.bind(this));this._button=new WebInspector.ToolbarButton(WebInspector.UIString("Pretty print"),"format-toolbar-item");this._button.setToggled(false);this._button.addEventListener("click",this._toggleFormatScriptSource,this);this._updateButton(sourcesView.currentUISourceCode());return this._button;},_isFormatableScript:function(uiSourceCode)
{if(!uiSourceCode)
return false;var supportedProjectTypes=[WebInspector.projectTypes.Network,WebInspector.projectTypes.Debugger,WebInspector.projectTypes.ContentScripts];if(supportedProjectTypes.indexOf(uiSourceCode.project().type())===-1)
return false;var contentType=uiSourceCode.contentType();return contentType===WebInspector.resourceTypes.Script||contentType===WebInspector.resourceTypes.Document;},_toggleFormatScriptSource:function()
{var uiSourceCode=this._sourcesView.currentUISourceCode();if(!this._isFormatableScript(uiSourceCode))
return;this._formatUISourceCodeScript(uiSourceCode);},_showIfNeeded:function(uiSourceCode,formattedUISourceCode,mapping)
{if(uiSourceCode!==this._sourcesView.currentUISourceCode())
return;var sourceFrame=this._sourcesView.viewForFile(uiSourceCode);var start=[0,0];if(sourceFrame){var selection=sourceFrame.selection();start=mapping.originalToFormatted(selection.startLine,selection.startColumn);}
this._sourcesView.showSourceLocation(formattedUISourceCode,start[0],start[1]);this._updateButton(formattedUISourceCode);},_discardFormattedUISourceCodeScript:function(formattedUISourceCode)
{var formatData=this._formatData.get(formattedUISourceCode);if(!formatData)
return;this._formatData.remove(formattedUISourceCode);var path=formatData.projectId+":"+formatData.path;this._formattedPaths.remove(path);this._pathsToFormatOnLoad.delete(path);for(var i=0;i<formatData.scripts.length;++i){this._uiSourceCodes.remove(formatData.scripts[i]);WebInspector.debuggerWorkspaceBinding.popSourceMapping(formatData.scripts[i]);}
this._projectDelegate._removeFormatted(formattedUISourceCode.path());},_cleanForTarget:function(target)
{var uiSourceCodes=this._formatData.keysArray();for(var i=0;i<uiSourceCodes.length;++i){WebInspector.debuggerWorkspaceBinding.setSourceMapping(target,uiSourceCodes[i],null);var formatData=this._formatData.get(uiSourceCodes[i]);var scripts=[];for(var j=0;j<formatData.scripts.length;++j){if(formatData.scripts[j].target()===target)
this._uiSourceCodes.remove(formatData.scripts[j]);else
scripts.push(formatData.scripts[j]);}
if(scripts.length)
formatData.scripts=scripts;else{this._formattedPaths.remove(formatData.projectId+":"+formatData.path);this._formatData.remove(uiSourceCodes[i]);this._projectDelegate._removeFormatted(uiSourceCodes[i].path());}}},_debuggerReset:function(event)
{var debuggerModel=(event.target);this._cleanForTarget(debuggerModel.target());},_scriptsForUISourceCode:function(uiSourceCode)
{function isInlineScript(script)
{return script.isInlineScript()&&!script.hasSourceURL;}
if(uiSourceCode.contentType()===WebInspector.resourceTypes.Document){var scripts=[];var debuggerModels=WebInspector.DebuggerModel.instances();for(var i=0;i<debuggerModels.length;++i){var networkURL=WebInspector.networkMapping.networkURL(uiSourceCode);scripts.pushAll(debuggerModels[i].scriptsForSourceURL(networkURL));}
return scripts.filter(isInlineScript);}
if(uiSourceCode.contentType()===WebInspector.resourceTypes.Script){var rawLocations=WebInspector.debuggerWorkspaceBinding.uiLocationToRawLocations(uiSourceCode,0,0);return rawLocations.map(function(rawLocation){return rawLocation.script();});}
return[];},_formatUISourceCodeScript:function(uiSourceCode)
{var formattedPath=this._formattedPaths.get(uiSourceCode.project().id()+":"+uiSourceCode.path());if(formattedPath){var uiSourceCodePath=formattedPath;var formattedUISourceCode=this._workspace.uiSourceCode(this._projectId,uiSourceCodePath);var formatData=formattedUISourceCode?this._formatData.get(formattedUISourceCode):null;if(formatData)
this._showIfNeeded(uiSourceCode,(formattedUISourceCode),formatData.mapping);return;}
uiSourceCode.requestContent(contentLoaded.bind(this));function contentLoaded(content)
{var highlighterType=WebInspector.SourcesView.uiSourceCodeHighlighterType(uiSourceCode);WebInspector.Formatter.format(uiSourceCode.contentType(),highlighterType,content||"",innerCallback.bind(this));}
function innerCallback(formattedContent,formatterMapping)
{var scripts=this._scriptsForUISourceCode(uiSourceCode);var name;if(uiSourceCode.contentType()===WebInspector.resourceTypes.Document)
name=uiSourceCode.displayName();else
name=uiSourceCode.name()||(scripts.length?scripts[0].scriptId:"");var networkURL=WebInspector.networkMapping.networkURL(uiSourceCode);formattedPath=this._projectDelegate._addFormatted(name,networkURL,uiSourceCode.contentType(),formattedContent);var formattedUISourceCode=(this._workspace.uiSourceCode(this._projectId,formattedPath));var formatData=new WebInspector.FormatterScriptMapping.FormatData(uiSourceCode.project().id(),uiSourceCode.path(),formatterMapping,scripts);this._formatData.set(formattedUISourceCode,formatData);var path=uiSourceCode.project().id()+":"+uiSourceCode.path();this._formattedPaths.set(path,formattedPath);this._pathsToFormatOnLoad.add(path);for(var i=0;i<scripts.length;++i){this._uiSourceCodes.set(scripts[i],formattedUISourceCode);var scriptMapping=(this._scriptMappingByTarget.get(scripts[i].target()));WebInspector.debuggerWorkspaceBinding.pushSourceMapping(scripts[i],scriptMapping);}
var targets=WebInspector.targetManager.targets();for(var i=0;i<targets.length;++i){var scriptMapping=(this._scriptMappingByTarget.get(targets[i]));WebInspector.debuggerWorkspaceBinding.setSourceMapping(targets[i],formattedUISourceCode,scriptMapping);}
this._showIfNeeded(uiSourceCode,formattedUISourceCode,formatterMapping);}}};WebInspector.InplaceFormatterEditorAction=function()
{}
WebInspector.InplaceFormatterEditorAction.prototype={_editorSelected:function(event)
{var uiSourceCode=(event.data);this._updateButton(uiSourceCode);},_editorClosed:function(event)
{var wasSelected=(event.data.wasSelected);if(wasSelected)
this._updateButton(null);},_updateButton:function(uiSourceCode)
{this._button.element.classList.toggle("hidden",!this._isFormattable(uiSourceCode));},button:function(sourcesView)
{if(this._button)
return this._button;this._sourcesView=sourcesView;this._sourcesView.addEventListener(WebInspector.SourcesView.Events.EditorSelected,this._editorSelected.bind(this));this._sourcesView.addEventListener(WebInspector.SourcesView.Events.EditorClosed,this._editorClosed.bind(this));this._button=new WebInspector.ToolbarButton(WebInspector.UIString("Format"),"format-toolbar-item");this._button.setToggled(false);this._button.addEventListener("click",this._formatSourceInPlace,this);this._updateButton(sourcesView.currentUISourceCode());return this._button;},_isFormattable:function(uiSourceCode)
{if(!uiSourceCode)
return false;if(uiSourceCode.project().type()===WebInspector.projectTypes.FileSystem)
return true;return uiSourceCode.contentType()===WebInspector.resourceTypes.Stylesheet||uiSourceCode.project().type()===WebInspector.projectTypes.Snippets;},_formatSourceInPlace:function()
{var uiSourceCode=this._sourcesView.currentUISourceCode();if(!this._isFormattable(uiSourceCode))
return;if(uiSourceCode.isDirty())
contentLoaded.call(this,uiSourceCode.workingCopy());else
uiSourceCode.requestContent(contentLoaded.bind(this));function contentLoaded(content)
{var highlighterType=WebInspector.SourcesView.uiSourceCodeHighlighterType(uiSourceCode);WebInspector.Formatter.format(uiSourceCode.contentType(),highlighterType,content||"",innerCallback.bind(this));}
function innerCallback(formattedContent,formatterMapping)
{if(uiSourceCode.workingCopy()===formattedContent)
return;var sourceFrame=this._sourcesView.viewForFile(uiSourceCode);var start=[0,0];if(sourceFrame){var selection=sourceFrame.selection();start=formatterMapping.originalToFormatted(selection.startLine,selection.startColumn);}
uiSourceCode.setWorkingCopy(formattedContent);this._sourcesView.showSourceLocation(uiSourceCode,start[0],start[1]);}},};WebInspector.Formatter=function()
{}
WebInspector.Formatter.format=function(contentType,mimeType,content,callback)
{if(contentType===WebInspector.resourceTypes.Script||contentType===WebInspector.resourceTypes.Document||contentType===WebInspector.resourceTypes.Stylesheet)
new WebInspector.ScriptFormatter(mimeType,content,callback);else
new WebInspector.IdentityFormatter(mimeType,content,callback);}
WebInspector.Formatter.locationToPosition=function(lineEndings,lineNumber,columnNumber)
{var position=lineNumber?lineEndings[lineNumber-1]+1:0;return position+columnNumber;}
WebInspector.Formatter.positionToLocation=function(lineEndings,position)
{var lineNumber=lineEndings.upperBound(position-1);if(!lineNumber)
var columnNumber=position;else
var columnNumber=position-lineEndings[lineNumber-1]-1;return[lineNumber,columnNumber];}
WebInspector.ScriptFormatter=function(mimeType,content,callback)
{content=content.replace(/\r\n?|[\n\u2028\u2029]/g,"\n").replace(/^\uFEFF/,'');this._callback=callback;this._originalContent=content;this._worker=new WorkerRuntime.Worker("script_formatter_worker");this._worker.onmessage=this._didFormatContent.bind(this);var parameters={mimeType:mimeType,content:content,indentString:WebInspector.moduleSetting("textEditorIndent").get()};this._worker.postMessage({method:"format",params:parameters});}
WebInspector.ScriptFormatter.prototype={_didFormatContent:function(event)
{this._worker.terminate();var originalContent=this._originalContent;var formattedContent=event.data.content;var mapping=event.data["mapping"];var sourceMapping=new WebInspector.FormatterSourceMappingImpl(originalContent.lineEndings(),formattedContent.lineEndings(),mapping);this._callback(formattedContent,sourceMapping);}}
WebInspector.IdentityFormatter=function(mimeType,content,callback)
{callback(content,new WebInspector.IdentityFormatterSourceMapping());}
WebInspector.FormatterMappingPayload;WebInspector.FormatterSourceMapping=function()
{}
WebInspector.FormatterSourceMapping.prototype={originalToFormatted:function(lineNumber,columnNumber){},formattedToOriginal:function(lineNumber,columnNumber){}}
WebInspector.IdentityFormatterSourceMapping=function()
{}
WebInspector.IdentityFormatterSourceMapping.prototype={originalToFormatted:function(lineNumber,columnNumber)
{return[lineNumber,columnNumber||0];},formattedToOriginal:function(lineNumber,columnNumber)
{return[lineNumber,columnNumber||0];}}
WebInspector.FormatterSourceMappingImpl=function(originalLineEndings,formattedLineEndings,mapping)
{this._originalLineEndings=originalLineEndings;this._formattedLineEndings=formattedLineEndings;this._mapping=mapping;}
WebInspector.FormatterSourceMappingImpl.prototype={originalToFormatted:function(lineNumber,columnNumber)
{var originalPosition=WebInspector.Formatter.locationToPosition(this._originalLineEndings,lineNumber,columnNumber||0);var formattedPosition=this._convertPosition(this._mapping.original,this._mapping.formatted,originalPosition||0);return WebInspector.Formatter.positionToLocation(this._formattedLineEndings,formattedPosition);},formattedToOriginal:function(lineNumber,columnNumber)
{var formattedPosition=WebInspector.Formatter.locationToPosition(this._formattedLineEndings,lineNumber,columnNumber||0);var originalPosition=this._convertPosition(this._mapping.formatted,this._mapping.original,formattedPosition);return WebInspector.Formatter.positionToLocation(this._originalLineEndings,originalPosition||0);},_convertPosition:function(positions1,positions2,position)
{var index=positions1.upperBound(position)-1;var convertedPosition=positions2[index]+position-positions1[index];if(index<positions2.length-1&&convertedPosition>positions2[index+1])
convertedPosition=positions2[index+1];return convertedPosition;}};WebInspector.SourcesView=function(workspace,sourcesPanel)
{WebInspector.VBox.call(this);this.registerRequiredCSS("sources/sourcesView.css");this.element.id="sources-panel-sources-view";this.setMinimumAndPreferredSizes(50,52,150,100);this._workspace=workspace;this._sourcesPanel=sourcesPanel;this._searchableView=new WebInspector.SearchableView(this,"sourcesViewSearchConfig");this._searchableView.setMinimalSearchQuerySize(0);this._searchableView.show(this.element);this._sourceFramesByUISourceCode=new Map();var tabbedEditorPlaceholderText=WebInspector.isMac()?WebInspector.UIString("Hit Cmd+P to open a file"):WebInspector.UIString("Hit Ctrl+P to open a file");this._editorContainer=new WebInspector.TabbedEditorContainer(this,WebInspector.settings.createLocalSetting("previouslyViewedFiles",[]),tabbedEditorPlaceholderText);this._editorContainer.show(this._searchableView.element);this._editorContainer.addEventListener(WebInspector.TabbedEditorContainer.Events.EditorSelected,this._editorSelected,this);this._editorContainer.addEventListener(WebInspector.TabbedEditorContainer.Events.EditorClosed,this._editorClosed,this);this._historyManager=new WebInspector.EditingLocationHistoryManager(this,this.currentSourceFrame.bind(this));this._toolbarContainerElement=this.element.createChild("div","sources-toolbar");this._toolbarEditorActions=new WebInspector.Toolbar(this._toolbarContainerElement);self.runtime.instancesPromise(WebInspector.SourcesView.EditorAction).then(appendButtonsForExtensions.bind(this));function appendButtonsForExtensions(actions)
{for(var i=0;i<actions.length;++i)
this._toolbarEditorActions.appendToolbarItem(actions[i].button(this));}
this._scriptViewToolbarText=new WebInspector.Toolbar(this._toolbarContainerElement);WebInspector.startBatchUpdate();this._workspace.uiSourceCodes().forEach(this._addUISourceCode.bind(this));WebInspector.endBatchUpdate();this._workspace.addEventListener(WebInspector.Workspace.Events.UISourceCodeAdded,this._uiSourceCodeAdded,this);this._workspace.addEventListener(WebInspector.Workspace.Events.UISourceCodeRemoved,this._uiSourceCodeRemoved,this);this._workspace.addEventListener(WebInspector.Workspace.Events.ProjectRemoved,this._projectRemoved.bind(this),this);function handleBeforeUnload(event)
{if(event.returnValue)
return;var unsavedSourceCodes=WebInspector.workspace.unsavedSourceCodes();if(!unsavedSourceCodes.length)
return;event.returnValue=WebInspector.UIString("DevTools have unsaved changes that will be permanently lost.");WebInspector.inspectorView.setCurrentPanel(WebInspector.SourcesPanel.instance());for(var i=0;i<unsavedSourceCodes.length;++i)
WebInspector.Revealer.reveal(unsavedSourceCodes[i]);}
if(!window.opener)
window.addEventListener("beforeunload",handleBeforeUnload,true);this._shortcuts={};this.element.addEventListener("keydown",this._handleKeyDown.bind(this),false);}
WebInspector.SourcesView.Events={EditorClosed:"EditorClosed",EditorSelected:"EditorSelected",}
WebInspector.SourcesView.uiSourceCodeHighlighterType=function(uiSourceCode)
{var networkContentType=WebInspector.NetworkProject.uiSourceCodeContentType(uiSourceCode);if(networkContentType)
return networkContentType.canonicalMimeType();var mimeType=WebInspector.ResourceType.mimeTypesForExtensions[uiSourceCode.extension().toLowerCase()];return mimeType||uiSourceCode.contentType().canonicalMimeType();}
WebInspector.SourcesView.prototype={registerShortcuts:function(registerShortcutDelegate)
{function registerShortcut(shortcuts,handler)
{registerShortcutDelegate(shortcuts,handler);this._registerShortcuts(shortcuts,handler);}
registerShortcut.call(this,WebInspector.ShortcutsScreen.SourcesPanelShortcuts.JumpToPreviousLocation,this._onJumpToPreviousLocation.bind(this));registerShortcut.call(this,WebInspector.ShortcutsScreen.SourcesPanelShortcuts.JumpToNextLocation,this._onJumpToNextLocation.bind(this));registerShortcut.call(this,WebInspector.ShortcutsScreen.SourcesPanelShortcuts.CloseEditorTab,this._onCloseEditorTab.bind(this));registerShortcut.call(this,WebInspector.ShortcutsScreen.SourcesPanelShortcuts.GoToLine,this._showGoToLineDialog.bind(this));registerShortcut.call(this,WebInspector.ShortcutsScreen.SourcesPanelShortcuts.GoToMember,this._showOutlineDialog.bind(this));registerShortcut.call(this,[WebInspector.KeyboardShortcut.makeDescriptor("o",WebInspector.KeyboardShortcut.Modifiers.CtrlOrMeta|WebInspector.KeyboardShortcut.Modifiers.Shift)],this._showOutlineDialog.bind(this));registerShortcut.call(this,WebInspector.ShortcutsScreen.SourcesPanelShortcuts.ToggleBreakpoint,this._toggleBreakpoint.bind(this));registerShortcut.call(this,WebInspector.ShortcutsScreen.SourcesPanelShortcuts.Save,this._save.bind(this));registerShortcut.call(this,WebInspector.ShortcutsScreen.SourcesPanelShortcuts.SaveAll,this._saveAll.bind(this));},_registerShortcuts:function(keys,handler)
{for(var i=0;i<keys.length;++i)
this._shortcuts[keys[i].key]=handler;},_handleKeyDown:function(event)
{var shortcutKey=WebInspector.KeyboardShortcut.makeKeyFromEvent(event);var handler=this._shortcuts[shortcutKey];if(handler&&handler())
event.consume(true);},wasShown:function()
{WebInspector.VBox.prototype.wasShown.call(this);WebInspector.context.setFlavor(WebInspector.SourcesView,this);},willHide:function()
{WebInspector.context.setFlavor(WebInspector.SourcesView,null);WebInspector.VBox.prototype.willHide.call(this);},toolbarContainerElement:function()
{return this._toolbarContainerElement;},defaultFocusedElement:function()
{return this._editorContainer.view.defaultFocusedElement();},searchableView:function()
{return this._searchableView;},visibleView:function()
{return this._editorContainer.visibleView;},currentSourceFrame:function()
{var view=this.visibleView();if(!(view instanceof WebInspector.SourceFrame))
return null;return(view);},currentUISourceCode:function()
{return this._currentUISourceCode;},_onCloseEditorTab:function(event)
{var uiSourceCode=this.currentUISourceCode();if(!uiSourceCode)
return false;this._editorContainer.closeFile(uiSourceCode);return true;},_onJumpToPreviousLocation:function(event)
{this._historyManager.rollback();return true;},_onJumpToNextLocation:function(event)
{this._historyManager.rollover();return true;},_uiSourceCodeAdded:function(event)
{var uiSourceCode=(event.data);this._addUISourceCode(uiSourceCode);},_addUISourceCode:function(uiSourceCode)
{if(uiSourceCode.project().isServiceProject())
return;this._editorContainer.addUISourceCode(uiSourceCode);var currentUISourceCode=this._currentUISourceCode;if(!currentUISourceCode)
return;var networkURL=WebInspector.networkMapping.networkURL(uiSourceCode);var currentNetworkURL=WebInspector.networkMapping.networkURL(currentUISourceCode);if(currentUISourceCode.project().isServiceProject()&&currentUISourceCode!==uiSourceCode&&currentNetworkURL===networkURL&&networkURL){this._showFile(uiSourceCode);this._editorContainer.removeUISourceCode(currentUISourceCode);}},_uiSourceCodeRemoved:function(event)
{var uiSourceCode=(event.data);this._removeUISourceCodes([uiSourceCode]);},_removeUISourceCodes:function(uiSourceCodes)
{this._editorContainer.removeUISourceCodes(uiSourceCodes);for(var i=0;i<uiSourceCodes.length;++i){this._removeSourceFrame(uiSourceCodes[i]);this._historyManager.removeHistoryForSourceCode(uiSourceCodes[i]);}},_projectRemoved:function(event)
{var project=event.data;var uiSourceCodes=project.uiSourceCodes();this._removeUISourceCodes(uiSourceCodes);},_updateScriptViewToolbarItems:function()
{this._scriptViewToolbarText.removeToolbarItems();var sourceFrame=this.currentSourceFrame();if(!sourceFrame)
return;var toolbarText=sourceFrame.toolbarText();this._scriptViewToolbarText.appendToolbarItem(toolbarText);},showSourceLocation:function(uiSourceCode,lineNumber,columnNumber,omitFocus,omitHighlight)
{this._historyManager.updateCurrentState();var sourceFrame=this._showFile(uiSourceCode);if(typeof lineNumber==="number")
sourceFrame.revealPosition(lineNumber,columnNumber,!omitHighlight);this._historyManager.pushNewState();if(!omitFocus)
sourceFrame.focus();},_showFile:function(uiSourceCode)
{var sourceFrame=this._getOrCreateSourceFrame(uiSourceCode);if(this._currentUISourceCode===uiSourceCode)
return sourceFrame;this._currentUISourceCode=uiSourceCode;this._editorContainer.showFile(uiSourceCode);this._updateScriptViewToolbarItems();return sourceFrame;},_createSourceFrame:function(uiSourceCode)
{var sourceFrame;switch(uiSourceCode.contentType()){case WebInspector.resourceTypes.Script:sourceFrame=new WebInspector.JavaScriptSourceFrame(this._sourcesPanel,uiSourceCode);break;case WebInspector.resourceTypes.Document:sourceFrame=new WebInspector.JavaScriptSourceFrame(this._sourcesPanel,uiSourceCode);break;case WebInspector.resourceTypes.Stylesheet:sourceFrame=new WebInspector.CSSSourceFrame(uiSourceCode);break;default:sourceFrame=new WebInspector.UISourceCodeFrame(uiSourceCode);break;}
sourceFrame.setHighlighterType(WebInspector.SourcesView.uiSourceCodeHighlighterType(uiSourceCode));this._sourceFramesByUISourceCode.set(uiSourceCode,sourceFrame);this._historyManager.trackSourceFrameCursorJumps(sourceFrame);return sourceFrame;},_getOrCreateSourceFrame:function(uiSourceCode)
{return this._sourceFramesByUISourceCode.get(uiSourceCode)||this._createSourceFrame(uiSourceCode);},_sourceFrameMatchesUISourceCode:function(sourceFrame,uiSourceCode)
{switch(uiSourceCode.contentType()){case WebInspector.resourceTypes.Script:case WebInspector.resourceTypes.Document:return sourceFrame instanceof WebInspector.JavaScriptSourceFrame;case WebInspector.resourceTypes.Stylesheet:return sourceFrame instanceof WebInspector.CSSSourceFrame;default:return!(sourceFrame instanceof WebInspector.JavaScriptSourceFrame);}},_recreateSourceFrameIfNeeded:function(uiSourceCode)
{var oldSourceFrame=this._sourceFramesByUISourceCode.get(uiSourceCode);if(!oldSourceFrame)
return;if(this._sourceFrameMatchesUISourceCode(oldSourceFrame,uiSourceCode)){oldSourceFrame.setHighlighterType(WebInspector.SourcesView.uiSourceCodeHighlighterType(uiSourceCode));}else{this._editorContainer.removeUISourceCode(uiSourceCode);this._removeSourceFrame(uiSourceCode);}},viewForFile:function(uiSourceCode)
{return this._getOrCreateSourceFrame(uiSourceCode);},_removeSourceFrame:function(uiSourceCode)
{var sourceFrame=this._sourceFramesByUISourceCode.get(uiSourceCode);if(!sourceFrame)
return;this._sourceFramesByUISourceCode.remove(uiSourceCode);sourceFrame.dispose();},clearCurrentExecutionLine:function()
{if(this._executionSourceFrame)
this._executionSourceFrame.clearExecutionLine();delete this._executionSourceFrame;},setExecutionLocation:function(uiLocation)
{var sourceFrame=this._getOrCreateSourceFrame(uiLocation.uiSourceCode);sourceFrame.setExecutionLocation(uiLocation);this._executionSourceFrame=sourceFrame;},_editorClosed:function(event)
{var uiSourceCode=(event.data);this._historyManager.removeHistoryForSourceCode(uiSourceCode);var wasSelected=false;if(this._currentUISourceCode===uiSourceCode){delete this._currentUISourceCode;wasSelected=true;}
this._updateScriptViewToolbarItems();this._searchableView.resetSearch();var data={};data.uiSourceCode=uiSourceCode;data.wasSelected=wasSelected;this.dispatchEventToListeners(WebInspector.SourcesView.Events.EditorClosed,data);},_editorSelected:function(event)
{var uiSourceCode=(event.data.currentFile);var shouldUseHistoryManager=uiSourceCode!==this._currentUISourceCode&&event.data.userGesture;if(shouldUseHistoryManager)
this._historyManager.updateCurrentState();var sourceFrame=this._showFile(uiSourceCode);if(shouldUseHistoryManager)
this._historyManager.pushNewState();this._searchableView.setReplaceable(!!sourceFrame&&sourceFrame.canEditSource());this._searchableView.refreshSearch();this.dispatchEventToListeners(WebInspector.SourcesView.Events.EditorSelected,uiSourceCode);},sourceRenamed:function(uiSourceCode)
{this._recreateSourceFrameIfNeeded(uiSourceCode);},searchCanceled:function()
{if(this._searchView)
this._searchView.searchCanceled();delete this._searchView;delete this._searchConfig;},performSearch:function(searchConfig,shouldJump,jumpBackwards)
{this._searchableView.updateSearchMatchesCount(0);var sourceFrame=this.currentSourceFrame();if(!sourceFrame)
return;this._searchView=sourceFrame;this._searchConfig=searchConfig;function finishedCallback(view,searchMatches)
{if(!searchMatches)
return;this._searchableView.updateSearchMatchesCount(searchMatches);}
function currentMatchChanged(currentMatchIndex)
{this._searchableView.updateCurrentMatchIndex(currentMatchIndex);}
function searchResultsChanged()
{this.performSearch(this._searchConfig,false,false);}
this._searchView.performSearch(this._searchConfig,shouldJump,!!jumpBackwards,finishedCallback.bind(this),currentMatchChanged.bind(this),searchResultsChanged.bind(this));},jumpToNextSearchResult:function()
{if(!this._searchView)
return;if(this._searchView!==this.currentSourceFrame()){this.performSearch(this._searchConfig,true);return;}
this._searchView.jumpToNextSearchResult();},jumpToPreviousSearchResult:function()
{if(!this._searchView)
return;if(this._searchView!==this.currentSourceFrame()){this.performSearch(this._searchConfig,true);if(this._searchView)
this._searchView.jumpToLastSearchResult();return;}
this._searchView.jumpToPreviousSearchResult();},supportsCaseSensitiveSearch:function()
{return true;},supportsRegexSearch:function()
{return true;},replaceSelectionWith:function(searchConfig,replacement)
{var sourceFrame=this.currentSourceFrame();if(!sourceFrame){console.assert(sourceFrame);return;}
sourceFrame.replaceSelectionWith(searchConfig,replacement);},replaceAllWith:function(searchConfig,replacement)
{var sourceFrame=this.currentSourceFrame();if(!sourceFrame){console.assert(sourceFrame);return;}
sourceFrame.replaceAllWith(searchConfig,replacement);},_showOutlineDialog:function(event)
{var uiSourceCode=this._editorContainer.currentFile();if(!uiSourceCode)
return false;switch(uiSourceCode.contentType()){case WebInspector.resourceTypes.Document:case WebInspector.resourceTypes.Script:WebInspector.JavaScriptOutlineDialog.show(uiSourceCode,this.showSourceLocation.bind(this,uiSourceCode));return true;case WebInspector.resourceTypes.Stylesheet:WebInspector.StyleSheetOutlineDialog.show(uiSourceCode,this.showSourceLocation.bind(this,uiSourceCode));return true;default:return true;}},showOpenResourceDialog:function(query)
{var uiSourceCodes=this._editorContainer.historyUISourceCodes();var defaultScores=new Map();for(var i=1;i<uiSourceCodes.length;++i)
defaultScores.set(uiSourceCodes[i],uiSourceCodes.length-i);WebInspector.OpenResourceDialog.show(this,query,defaultScores);},_showGoToLineDialog:function(event)
{if(this._currentUISourceCode)
this.showOpenResourceDialog(":");return true;},_save:function()
{this._saveSourceFrame(this.currentSourceFrame());return true;},_saveAll:function()
{var sourceFrames=this._editorContainer.fileViews();sourceFrames.forEach(this._saveSourceFrame.bind(this));return true;},_saveSourceFrame:function(sourceFrame)
{if(!sourceFrame)
return;if(!(sourceFrame instanceof WebInspector.UISourceCodeFrame))
return;var uiSourceCodeFrame=(sourceFrame);uiSourceCodeFrame.commitEditing();},_toggleBreakpoint:function()
{var sourceFrame=this.currentSourceFrame();if(!sourceFrame)
return false;if(sourceFrame instanceof WebInspector.JavaScriptSourceFrame){var javaScriptSourceFrame=(sourceFrame);javaScriptSourceFrame.toggleBreakpointOnCurrentLine();return true;}
return false;},toggleBreakpointsActiveState:function(active)
{this._editorContainer.view.element.classList.toggle("breakpoints-deactivated",!active);},__proto__:WebInspector.VBox.prototype}
WebInspector.SourcesView.EditorAction=function()
{}
WebInspector.SourcesView.EditorAction.prototype={button:function(sourcesView){}}
WebInspector.SourcesView.SwitchFileActionDelegate=function()
{}
WebInspector.SourcesView.SwitchFileActionDelegate._nextFile=function(currentUISourceCode)
{function fileNamePrefix(name)
{var lastDotIndex=name.lastIndexOf(".");var namePrefix=name.substr(0,lastDotIndex!==-1?lastDotIndex:name.length);return namePrefix.toLowerCase();}
var uiSourceCodes=currentUISourceCode.project().uiSourceCodes();var candidates=[];var path=currentUISourceCode.parentPath();var name=currentUISourceCode.name();var namePrefix=fileNamePrefix(name);for(var i=0;i<uiSourceCodes.length;++i){var uiSourceCode=uiSourceCodes[i];if(path!==uiSourceCode.parentPath())
continue;if(fileNamePrefix(uiSourceCode.name())===namePrefix)
candidates.push(uiSourceCode.name());}
candidates.sort(String.naturalOrderComparator);var index=mod(candidates.indexOf(name)+1,candidates.length);var fullPath=(path?path+"/":"")+candidates[index];var nextUISourceCode=currentUISourceCode.project().uiSourceCode(fullPath);return nextUISourceCode!==currentUISourceCode?nextUISourceCode:null;}
WebInspector.SourcesView.SwitchFileActionDelegate.prototype={handleAction:function(context,actionId)
{var sourcesView=WebInspector.context.flavor(WebInspector.SourcesView);var currentUISourceCode=sourcesView.currentUISourceCode();if(!currentUISourceCode)
return false;var nextUISourceCode=WebInspector.SourcesView.SwitchFileActionDelegate._nextFile(currentUISourceCode);if(!nextUISourceCode)
return false;sourcesView.showSourceLocation(nextUISourceCode);return true;}};WebInspector.AdvancedSearchView=function()
{WebInspector.VBox.call(this,true);this.setMinimumSize(0,40);this.registerRequiredCSS("sources/sourcesSearch.css");this._searchId=0;this.contentElement.classList.add("search-view");this._searchPanelElement=this.contentElement.createChild("div","search-drawer-header");this._searchPanelElement.addEventListener("keydown",this._onKeyDown.bind(this),false);this._searchResultsElement=this.contentElement.createChild("div");this._searchResultsElement.className="search-results";this._search=WebInspector.HistoryInput.create();this._searchPanelElement.appendChild(this._search);this._search.placeholder=WebInspector.UIString("Enter query, use `file:` to filter by path");this._search.setAttribute("type","text");this._search.classList.add("search-config-search");this._search.setAttribute("results","0");this._search.setAttribute("size",30);this._ignoreCaseLabel=createCheckboxLabel(WebInspector.UIString("Ignore case"));this._ignoreCaseLabel.classList.add("search-config-label");this._searchPanelElement.appendChild(this._ignoreCaseLabel);this._ignoreCaseCheckbox=this._ignoreCaseLabel.checkboxElement;this._ignoreCaseCheckbox.classList.add("search-config-checkbox");this._regexLabel=createCheckboxLabel(WebInspector.UIString("Regular expression"));this._regexLabel.classList.add("search-config-label");this._searchPanelElement.appendChild(this._regexLabel);this._regexCheckbox=this._regexLabel.checkboxElement;this._regexCheckbox.classList.add("search-config-checkbox");this._searchToolbarElement=this.contentElement.createChild("div","search-toolbar-summary");this._searchMessageElement=this._searchToolbarElement.createChild("div","search-message");this._searchProgressPlaceholderElement=this._searchToolbarElement.createChild("div","flex-centered");this._searchToolbarElement.createChild("div","search-message-spacer");this._searchResultsMessageElement=this._searchToolbarElement.createChild("div","search-message");this._advancedSearchConfig=WebInspector.settings.createLocalSetting("advancedSearchConfig",new WebInspector.SearchConfig("",true,false).toPlainObject());this._load();this._searchScope=new WebInspector.SourcesSearchScope();}
WebInspector.AdvancedSearchView.prototype={_buildSearchConfig:function()
{return new WebInspector.SearchConfig(this._search.value,this._ignoreCaseCheckbox.checked,this._regexCheckbox.checked);},_toggle:function(queryCandidate)
{if(queryCandidate)
this._search.value=queryCandidate;this.focus();this._startIndexing();},_onIndexingFinished:function()
{var finished=!this._progressIndicator.isCanceled();this._progressIndicator.done();delete this._progressIndicator;delete this._isIndexing;this._indexingFinished(finished);if(!finished)
delete this._pendingSearchConfig;if(!this._pendingSearchConfig)
return;var searchConfig=this._pendingSearchConfig;delete this._pendingSearchConfig;this._innerStartSearch(searchConfig);},_startIndexing:function()
{this._isIndexing=true;if(this._progressIndicator)
this._progressIndicator.done();this._progressIndicator=new WebInspector.ProgressIndicator();this._searchMessageElement.textContent=WebInspector.UIString("Indexing\u2026");this._progressIndicator.show(this._searchProgressPlaceholderElement);this._searchScope.performIndexing(new WebInspector.ProgressProxy(this._progressIndicator,this._onIndexingFinished.bind(this)));},_onSearchResult:function(searchId,searchResult)
{if(searchId!==this._searchId||!this._progressIndicator)
return;if(this._progressIndicator&&this._progressIndicator.isCanceled()){this._onIndexingFinished();return;}
this._addSearchResult(searchResult);if(!searchResult.searchMatches.length)
return;if(!this._searchResultsPane)
this._searchResultsPane=this._searchScope.createSearchResultsPane(this._searchConfig);this._resetResults();this._searchResultsElement.appendChild(this._searchResultsPane.element);this._searchResultsPane.addSearchResult(searchResult);},_onSearchFinished:function(searchId,finished)
{if(searchId!==this._searchId||!this._progressIndicator)
return;if(!this._searchResultsPane)
this._nothingFound();this._searchFinished(finished);delete this._searchConfig;},_startSearch:function(searchConfig)
{this._resetSearch();++this._searchId;if(!this._isIndexing)
this._startIndexing();this._pendingSearchConfig=searchConfig;},_innerStartSearch:function(searchConfig)
{this._searchConfig=searchConfig;if(this._progressIndicator)
this._progressIndicator.done();this._progressIndicator=new WebInspector.ProgressIndicator();this._searchStarted(this._progressIndicator);this._searchScope.performSearch(searchConfig,this._progressIndicator,this._onSearchResult.bind(this,this._searchId),this._onSearchFinished.bind(this,this._searchId));},_resetSearch:function()
{this._stopSearch();if(this._searchResultsPane){this._resetResults();delete this._searchResultsPane;}},_stopSearch:function()
{if(this._progressIndicator&&!this._isIndexing)
this._progressIndicator.cancel();if(this._searchScope)
this._searchScope.stopSearch();delete this._searchConfig;},_searchStarted:function(progressIndicator)
{this._resetResults();this._resetCounters();this._searchMessageElement.textContent=WebInspector.UIString("Searching\u2026");progressIndicator.show(this._searchProgressPlaceholderElement);this._updateSearchResultsMessage();if(!this._searchingView)
this._searchingView=new WebInspector.EmptyWidget(WebInspector.UIString("Searching\u2026"));this._searchingView.show(this._searchResultsElement);},_indexingFinished:function(finished)
{this._searchMessageElement.textContent=finished?"":WebInspector.UIString("Indexing interrupted.");},_updateSearchResultsMessage:function()
{if(this._searchMatchesCount&&this._searchResultsCount)
this._searchResultsMessageElement.textContent=WebInspector.UIString("Found %d matches in %d files.",this._searchMatchesCount,this._nonEmptySearchResultsCount);else
this._searchResultsMessageElement.textContent="";},_resetResults:function()
{if(this._searchingView)
this._searchingView.detach();if(this._notFoundView)
this._notFoundView.detach();this._searchResultsElement.removeChildren();},_resetCounters:function()
{this._searchMatchesCount=0;this._searchResultsCount=0;this._nonEmptySearchResultsCount=0;},_nothingFound:function()
{this._resetResults();if(!this._notFoundView)
this._notFoundView=new WebInspector.EmptyWidget(WebInspector.UIString("No matches found."));this._notFoundView.show(this._searchResultsElement);this._searchResultsMessageElement.textContent=WebInspector.UIString("No matches found.");},_addSearchResult:function(searchResult)
{this._searchMatchesCount+=searchResult.searchMatches.length;this._searchResultsCount++;if(searchResult.searchMatches.length)
this._nonEmptySearchResultsCount++;this._updateSearchResultsMessage();},_searchFinished:function(finished)
{this._searchMessageElement.textContent=finished?WebInspector.UIString("Search finished."):WebInspector.UIString("Search interrupted.");},focus:function()
{WebInspector.setCurrentFocusElement(this._search);this._search.select();},willHide:function()
{this._stopSearch();},_onKeyDown:function(event)
{switch(event.keyCode){case WebInspector.KeyboardShortcut.Keys.Enter.code:this._onAction();break;}},_save:function()
{this._advancedSearchConfig.set(this._buildSearchConfig().toPlainObject());},_load:function()
{var searchConfig=WebInspector.SearchConfig.fromPlainObject(this._advancedSearchConfig.get());this._search.value=searchConfig.query();this._ignoreCaseCheckbox.checked=searchConfig.ignoreCase();this._regexCheckbox.checked=searchConfig.isRegex();},_onAction:function()
{var searchConfig=this._buildSearchConfig();if(!searchConfig.query()||!searchConfig.query().length)
return;this._save();this._startSearch(searchConfig);},__proto__:WebInspector.VBox.prototype}
WebInspector.SearchResultsPane=function(searchConfig)
{this._searchConfig=searchConfig;this.element=createElement("div");}
WebInspector.SearchResultsPane.prototype={get searchConfig()
{return this._searchConfig;},addSearchResult:function(searchResult){}}
WebInspector.AdvancedSearchView.ActionDelegate=function()
{}
WebInspector.AdvancedSearchView.ActionDelegate.prototype={handleAction:function(context,actionId)
{this._showSearch();return true;},_showSearch:function()
{function updateSearchBox(view)
{console.assert(view&&view instanceof WebInspector.AdvancedSearchView);var searchView=(view);if(searchView._search!==searchView.element.window().document.activeElement){WebInspector.inspectorView.setCurrentPanel(WebInspector.SourcesPanel.instance());searchView._toggle(queryCandidate);searchView.focus();}
return searchView;}
var selection=WebInspector.inspectorView.element.getDeepSelection();var queryCandidate="";if(selection.rangeCount)
queryCandidate=selection.toString().replace(/\r?\n.*/,"");return WebInspector.inspectorView.showViewInDrawer("sources.search").then(updateSearchBox);}}
WebInspector.FileBasedSearchResult=function(uiSourceCode,searchMatches){this.uiSourceCode=uiSourceCode;this.searchMatches=searchMatches;}
WebInspector.SearchScope=function()
{}
WebInspector.SearchScope.prototype={performSearch:function(searchConfig,progress,searchResultCallback,searchFinishedCallback){},performIndexing:function(progress){},stopSearch:function(){},createSearchResultsPane:function(searchConfig){}};WebInspector.FileBasedSearchResultsPane=function(searchConfig)
{WebInspector.SearchResultsPane.call(this,searchConfig);this._searchResults=[];this.element.id="search-results-pane-file-based";this._treeOutline=new TreeOutline();this._treeOutline.element.classList.add("search-results-outline-disclosure");this.element.appendChild(this._treeOutline.element);this._matchesExpandedCount=0;}
WebInspector.FileBasedSearchResultsPane.matchesExpandedByDefaultCount=20;WebInspector.FileBasedSearchResultsPane.fileMatchesShownAtOnce=20;WebInspector.FileBasedSearchResultsPane.prototype={addSearchResult:function(searchResult)
{this._searchResults.push(searchResult);var uiSourceCode=searchResult.uiSourceCode;if(!uiSourceCode)
return;this._addFileTreeElement(searchResult);},_addFileTreeElement:function(searchResult)
{var fileTreeElement=new WebInspector.FileBasedSearchResultsPane.FileTreeElement(this._searchConfig,searchResult);this._treeOutline.appendChild(fileTreeElement);if(this._matchesExpandedCount<WebInspector.FileBasedSearchResultsPane.matchesExpandedByDefaultCount)
fileTreeElement.expand();this._matchesExpandedCount+=searchResult.searchMatches.length;},__proto__:WebInspector.SearchResultsPane.prototype}
WebInspector.FileBasedSearchResultsPane.FileTreeElement=function(searchConfig,searchResult)
{TreeElement.call(this,"",true);this._searchConfig=searchConfig;this._searchResult=searchResult;this.toggleOnClick=true;this.selectable=false;}
WebInspector.FileBasedSearchResultsPane.FileTreeElement.prototype={onexpand:function()
{if(this._initialized)
return;this._updateMatchesUI();this._initialized=true;},_updateMatchesUI:function()
{this.removeChildren();var toIndex=Math.min(this._searchResult.searchMatches.length,WebInspector.FileBasedSearchResultsPane.fileMatchesShownAtOnce);if(toIndex<this._searchResult.searchMatches.length){this._appendSearchMatches(0,toIndex-1);this._appendShowMoreMatchesElement(toIndex-1);}else{this._appendSearchMatches(0,toIndex);}},onattach:function()
{this._updateSearchMatches();},_updateSearchMatches:function()
{this.listItemElement.classList.add("search-result");var fileNameSpan=createElement("span");fileNameSpan.className="search-result-file-name";fileNameSpan.textContent=this._searchResult.uiSourceCode.fullDisplayName();this.listItemElement.appendChild(fileNameSpan);var matchesCountSpan=createElement("span");matchesCountSpan.className="search-result-matches-count";var searchMatchesCount=this._searchResult.searchMatches.length;if(searchMatchesCount===1)
matchesCountSpan.textContent=WebInspector.UIString("(%d match)",searchMatchesCount);else
matchesCountSpan.textContent=WebInspector.UIString("(%d matches)",searchMatchesCount);this.listItemElement.appendChild(matchesCountSpan);if(this.expanded)
this._updateMatchesUI();},_appendSearchMatches:function(fromIndex,toIndex)
{var searchResult=this._searchResult;var uiSourceCode=searchResult.uiSourceCode;var searchMatches=searchResult.searchMatches;var queries=this._searchConfig.queries();var regexes=[];for(var i=0;i<queries.length;++i)
regexes.push(createSearchRegex(queries[i],!this._searchConfig.ignoreCase(),this._searchConfig.isRegex()));for(var i=fromIndex;i<toIndex;++i){var lineNumber=searchMatches[i].lineNumber;var lineContent=searchMatches[i].lineContent;var matchRanges=[];for(var j=0;j<regexes.length;++j)
matchRanges=matchRanges.concat(this._regexMatchRanges(lineContent,regexes[j]));var anchor=this._createAnchor(uiSourceCode,lineNumber,matchRanges[0].offset);var numberString=numberToStringWithSpacesPadding(lineNumber+1,4);var lineNumberSpan=createElement("span");lineNumberSpan.classList.add("search-match-line-number");lineNumberSpan.textContent=numberString;anchor.appendChild(lineNumberSpan);var contentSpan=this._createContentSpan(lineContent,matchRanges);anchor.appendChild(contentSpan);var searchMatchElement=new TreeElement();searchMatchElement.selectable=false;this.appendChild(searchMatchElement);searchMatchElement.listItemElement.className="search-match source-code";searchMatchElement.listItemElement.appendChild(anchor);}},_appendShowMoreMatchesElement:function(startMatchIndex)
{var matchesLeftCount=this._searchResult.searchMatches.length-startMatchIndex;var showMoreMatchesText=WebInspector.UIString("Show all matches (%d more).",matchesLeftCount);this._showMoreMatchesTreeElement=new TreeElement(showMoreMatchesText);this.appendChild(this._showMoreMatchesTreeElement);this._showMoreMatchesTreeElement.listItemElement.classList.add("show-more-matches");this._showMoreMatchesTreeElement.onselect=this._showMoreMatchesElementSelected.bind(this,startMatchIndex);},_createAnchor:function(uiSourceCode,lineNumber,columnNumber)
{return WebInspector.Linkifier.linkifyUsingRevealer(uiSourceCode.uiLocation(lineNumber,columnNumber),"");},_createContentSpan:function(lineContent,matchRanges)
{var contentSpan=createElement("span");contentSpan.className="search-match-content";contentSpan.textContent=lineContent;WebInspector.highlightRangesWithStyleClass(contentSpan,matchRanges,"highlighted-match");return contentSpan;},_regexMatchRanges:function(lineContent,regex)
{regex.lastIndex=0;var match;var matchRanges=[];while((regex.lastIndex<lineContent.length)&&(match=regex.exec(lineContent)))
matchRanges.push(new WebInspector.SourceRange(match.index,match[0].length));return matchRanges;},_showMoreMatchesElementSelected:function(startMatchIndex)
{this.removeChild(this._showMoreMatchesTreeElement);this._appendSearchMatches(startMatchIndex,this._searchResult.searchMatches.length);return false;},__proto__:TreeElement.prototype};WebInspector.SourcesSearchScope=function()
{this._searchId=0;}
WebInspector.SourcesSearchScope._filesComparator=function(uiSourceCode1,uiSourceCode2)
{if(uiSourceCode1.isDirty()&&!uiSourceCode2.isDirty())
return-1;if(!uiSourceCode1.isDirty()&&uiSourceCode2.isDirty())
return 1;var networkURL1=WebInspector.networkMapping.networkURL(uiSourceCode1);var networkURL2=WebInspector.networkMapping.networkURL(uiSourceCode2);if(networkURL1&&!networkURL2)
return-1;if(!networkURL1&&networkURL2)
return 1;return String.naturalOrderComparator(uiSourceCode1.fullDisplayName(),uiSourceCode2.fullDisplayName());}
WebInspector.SourcesSearchScope.prototype={performIndexing:function(progress)
{this.stopSearch();var projects=this._projects();var compositeProgress=new WebInspector.CompositeProgress(progress);for(var i=0;i<projects.length;++i){var project=projects[i];var projectProgress=compositeProgress.createSubProgress(project.uiSourceCodes().length);project.indexContent(projectProgress);}},_projects:function()
{function filterOutServiceProjects(project)
{return!project.isServiceProject()||project.type()===WebInspector.projectTypes.Formatter;}
function filterOutContentScriptsIfNeeded(project)
{return WebInspector.moduleSetting("searchInContentScripts").get()||project.type()!==WebInspector.projectTypes.ContentScripts;}
return WebInspector.workspace.projects().filter(filterOutServiceProjects).filter(filterOutContentScriptsIfNeeded);},performSearch:function(searchConfig,progress,searchResultCallback,searchFinishedCallback)
{this.stopSearch();this._searchResultCandidates=[];this._searchResultCallback=searchResultCallback;this._searchFinishedCallback=searchFinishedCallback;this._searchConfig=searchConfig;var projects=this._projects();var barrier=new CallbackBarrier();var compositeProgress=new WebInspector.CompositeProgress(progress);var searchContentProgress=compositeProgress.createSubProgress();var findMatchingFilesProgress=new WebInspector.CompositeProgress(compositeProgress.createSubProgress());for(var i=0;i<projects.length;++i){var project=projects[i];var weight=project.uiSourceCodes().length;var findMatchingFilesInProjectProgress=findMatchingFilesProgress.createSubProgress(weight);var barrierCallback=barrier.createCallback();var filesMathingFileQuery=this._projectFilesMatchingFileQuery(project,searchConfig);var callback=this._processMatchingFilesForProject.bind(this,this._searchId,project,filesMathingFileQuery,barrierCallback);project.findFilesMatchingSearchRequest(searchConfig,filesMathingFileQuery,findMatchingFilesInProjectProgress,callback);}
barrier.callWhenDone(this._processMatchingFiles.bind(this,this._searchId,searchContentProgress,this._searchFinishedCallback.bind(this,true)));},_projectFilesMatchingFileQuery:function(project,searchConfig,dirtyOnly)
{var result=[];var uiSourceCodes=project.uiSourceCodes();for(var i=0;i<uiSourceCodes.length;++i){var uiSourceCode=uiSourceCodes[i];if(dirtyOnly&&!uiSourceCode.isDirty())
continue;if(this._searchConfig.filePathMatchesFileQuery(uiSourceCode.fullDisplayName()))
result.push(uiSourceCode.path());}
result.sort(String.naturalOrderComparator);return result;},_processMatchingFilesForProject:function(searchId,project,filesMathingFileQuery,callback,files)
{if(searchId!==this._searchId){this._searchFinishedCallback(false);return;}
files.sort(String.naturalOrderComparator);files=files.intersectOrdered(filesMathingFileQuery,String.naturalOrderComparator);var dirtyFiles=this._projectFilesMatchingFileQuery(project,this._searchConfig,true);files=files.mergeOrdered(dirtyFiles,String.naturalOrderComparator);var uiSourceCodes=[];for(var i=0;i<files.length;++i){var uiSourceCode=project.uiSourceCode(files[i]);if(uiSourceCode)
uiSourceCodes.push(uiSourceCode);}
uiSourceCodes.sort(WebInspector.SourcesSearchScope._filesComparator);this._searchResultCandidates=this._searchResultCandidates.mergeOrdered(uiSourceCodes,WebInspector.SourcesSearchScope._filesComparator);callback();},_processMatchingFiles:function(searchId,progress,callback)
{if(searchId!==this._searchId){this._searchFinishedCallback(false);return;}
var files=this._searchResultCandidates;if(!files.length){progress.done();callback();return;}
progress.setTotalWork(files.length);var fileIndex=0;var maxFileContentRequests=20;var callbacksLeft=0;for(var i=0;i<maxFileContentRequests&&i<files.length;++i)
scheduleSearchInNextFileOrFinish.call(this);function searchInNextFile(uiSourceCode)
{if(uiSourceCode.isDirty())
contentLoaded.call(this,uiSourceCode,uiSourceCode.workingCopy());else
uiSourceCode.checkContentUpdated(true,contentUpdated.bind(this,uiSourceCode));}
function contentUpdated(uiSourceCode)
{uiSourceCode.requestContent(contentLoaded.bind(this,uiSourceCode));}
function scheduleSearchInNextFileOrFinish()
{if(fileIndex>=files.length){if(!callbacksLeft){progress.done();callback();return;}
return;}
++callbacksLeft;var uiSourceCode=files[fileIndex++];setTimeout(searchInNextFile.bind(this,uiSourceCode),0);}
function contentLoaded(uiSourceCode,content)
{function matchesComparator(a,b)
{return a.lineNumber-b.lineNumber;}
progress.worked(1);var matches=[];var queries=this._searchConfig.queries();if(content!==null){for(var i=0;i<queries.length;++i){var nextMatches=WebInspector.ContentProvider.performSearchInContent(content,queries[i],!this._searchConfig.ignoreCase(),this._searchConfig.isRegex());matches=matches.mergeOrdered(nextMatches,matchesComparator);}}
if(matches){var searchResult=new WebInspector.FileBasedSearchResult(uiSourceCode,matches);this._searchResultCallback(searchResult);}
--callbacksLeft;scheduleSearchInNextFileOrFinish.call(this);}},stopSearch:function()
{++this._searchId;},createSearchResultsPane:function(searchConfig)
{return new WebInspector.FileBasedSearchResultsPane(searchConfig);}};WebInspector.SourcesPanel=function(workspaceForTest)
{WebInspector.Panel.call(this,"sources");this.registerRequiredCSS("sources/sourcesPanel.css");new WebInspector.DropTarget(this.element,[WebInspector.DropTarget.Types.Files],WebInspector.UIString("Drop workspace folder here"),this._handleDrop.bind(this));this._workspace=workspaceForTest||WebInspector.workspace;this._networkMapping=WebInspector.networkMapping;this._debugToolbar=this._createDebugToolbar();this._debugToolbarDrawer=this._createDebugToolbarDrawer();const initialDebugSidebarWidth=225;this._splitWidget=new WebInspector.SplitWidget(true,true,"sourcesPanelSplitViewState",initialDebugSidebarWidth);this._splitWidget.enableShowModeSaving();this._splitWidget.show(this.element);const initialNavigatorWidth=225;this.editorView=new WebInspector.SplitWidget(true,false,"sourcesPanelNavigatorSplitViewState",initialNavigatorWidth);this.editorView.enableShowModeSaving();this.editorView.element.tabIndex=0;this._splitWidget.setMainWidget(this.editorView);this._navigator=new WebInspector.SourcesNavigator(this._workspace);this._navigator.view.setMinimumSize(100,25);this.editorView.setSidebarWidget(this._navigator.view);this._navigator.addEventListener(WebInspector.SourcesNavigator.Events.SourceSelected,this._sourceSelected,this);this._navigator.addEventListener(WebInspector.SourcesNavigator.Events.SourceRenamed,this._sourceRenamed,this);this._sourcesView=new WebInspector.SourcesView(this._workspace,this);this._sourcesView.addEventListener(WebInspector.SourcesView.Events.EditorSelected,this._editorSelected.bind(this));this._sourcesView.addEventListener(WebInspector.SourcesView.Events.EditorClosed,this._editorClosed.bind(this));this._sourcesView.registerShortcuts(this.registerShortcuts.bind(this));this.editorView.setMainWidget(this._sourcesView);this.sidebarPanes={};this.sidebarPanes.threads=new WebInspector.ThreadsSidebarPane();this.sidebarPanes.watchExpressions=new WebInspector.WatchExpressionsSidebarPane();this.sidebarPanes.callstack=new WebInspector.CallStackSidebarPane();this.sidebarPanes.callstack.addEventListener(WebInspector.CallStackSidebarPane.Events.CallFrameSelected,this._callFrameSelectedInSidebar.bind(this));this.sidebarPanes.callstack.addEventListener(WebInspector.CallStackSidebarPane.Events.RevealHiddenCallFrames,this._hiddenCallFramesRevealedInSidebar.bind(this));this.sidebarPanes.callstack.registerShortcuts(this.registerShortcuts.bind(this));this.sidebarPanes.scopechain=new WebInspector.ScopeChainSidebarPane();this.sidebarPanes.serviceWorkers=new WebInspector.ServiceWorkersSidebarPane();this.sidebarPanes.jsBreakpoints=new WebInspector.JavaScriptBreakpointsSidebarPane(WebInspector.breakpointManager,this.showUISourceCode.bind(this));this.sidebarPanes.domBreakpoints=WebInspector.domBreakpointsSidebarPane.createProxy(this);this.sidebarPanes.xhrBreakpoints=new WebInspector.XHRBreakpointsSidebarPane();this.sidebarPanes.eventListenerBreakpoints=new WebInspector.EventListenerBreakpointsSidebarPane();this.sidebarPanes.objectEventListeners=new WebInspector.ObjectEventListenersSidebarPane();if(Runtime.experiments.isEnabled("stepIntoAsync"))
this.sidebarPanes.asyncOperationBreakpoints=new WebInspector.AsyncOperationsSidebarPane();this._lastSelectedTabSetting=WebInspector.settings.createLocalSetting("lastSelectedSourcesSidebarPaneTab",this.sidebarPanes.scopechain.title());this._installDebuggerSidebarController();WebInspector.moduleSetting("sidebarPosition").addChangeListener(this._updateSidebarPosition.bind(this));this._updateSidebarPosition();this._updateDebuggerButtons();this._pauseOnExceptionEnabledChanged();WebInspector.moduleSetting("pauseOnExceptionEnabled").addChangeListener(this._pauseOnExceptionEnabledChanged,this);this._setTarget(WebInspector.context.flavor(WebInspector.Target));WebInspector.breakpointManager.addEventListener(WebInspector.BreakpointManager.Events.BreakpointsActiveStateChanged,this._breakpointsActiveStateChanged,this);WebInspector.context.addFlavorChangeListener(WebInspector.Target,this._onCurrentTargetChanged,this);WebInspector.targetManager.addModelListener(WebInspector.DebuggerModel,WebInspector.DebuggerModel.Events.DebuggerWasEnabled,this._debuggerWasEnabled,this);WebInspector.targetManager.addModelListener(WebInspector.DebuggerModel,WebInspector.DebuggerModel.Events.DebuggerPaused,this._debuggerPaused,this);WebInspector.targetManager.addModelListener(WebInspector.DebuggerModel,WebInspector.DebuggerModel.Events.DebuggerResumed,this._debuggerResumed,this);WebInspector.targetManager.addModelListener(WebInspector.DebuggerModel,WebInspector.DebuggerModel.Events.CallFrameSelected,this._callFrameSelected,this);WebInspector.targetManager.addModelListener(WebInspector.DebuggerModel,WebInspector.DebuggerModel.Events.ConsoleCommandEvaluatedInSelectedCallFrame,this._consoleCommandEvaluatedInSelectedCallFrame,this);WebInspector.targetManager.addModelListener(WebInspector.DebuggerModel,WebInspector.DebuggerModel.Events.GlobalObjectCleared,this._debuggerReset,this);new WebInspector.WorkspaceMappingTip(this,this._workspace);WebInspector.extensionServer.addEventListener(WebInspector.ExtensionServer.Events.SidebarPaneAdded,this._extensionSidebarPaneAdded,this);WebInspector.DataSaverInfobar.maybeShowInPanel(this);}
WebInspector.SourcesPanel.minToolbarWidth=215;WebInspector.SourcesPanel.prototype={_setTarget:function(target)
{if(!target)
return;var debuggerModel=WebInspector.DebuggerModel.fromTarget(target);if(!debuggerModel)
return;if(debuggerModel.isPaused()){this._showDebuggerPausedDetails((debuggerModel.debuggerPausedDetails()));var callFrame=debuggerModel.selectedCallFrame();if(callFrame)
this._selectCallFrame(callFrame);}else{this._paused=false;this._clearInterface();this._toggleDebuggerSidebarButton.disabled=false;}},_onCurrentTargetChanged:function(event)
{var target=(event.data);this._setTarget(target);},defaultFocusedElement:function()
{return this._sourcesView.defaultFocusedElement();},paused:function()
{return this._paused;},wasShown:function()
{WebInspector.context.setFlavor(WebInspector.SourcesPanel,this);WebInspector.Panel.prototype.wasShown.call(this);},willHide:function()
{WebInspector.Panel.prototype.willHide.call(this);WebInspector.context.setFlavor(WebInspector.SourcesPanel,null);},onResize:function()
{if(WebInspector.moduleSetting("sidebarPosition").get()==="auto")
this.element.window().requestAnimationFrame(this._updateSidebarPosition.bind(this));},searchableView:function()
{return this._sourcesView.searchableView();},_consoleCommandEvaluatedInSelectedCallFrame:function(event)
{var debuggerModel=(event.target);var target=debuggerModel.target();if(WebInspector.context.flavor(WebInspector.Target)!==target)
return;this.sidebarPanes.scopechain.update(debuggerModel.selectedCallFrame());},_debuggerPaused:function(event)
{var details=(event.data);if(!this._paused)
WebInspector.inspectorView.setCurrentPanel(this);if(WebInspector.context.flavor(WebInspector.Target)===details.target())
this._showDebuggerPausedDetails(details);else if(!this._paused)
WebInspector.context.setFlavor(WebInspector.Target,details.target());},_showDebuggerPausedDetails:function(details)
{this._paused=true;this._updateDebuggerButtons();this.sidebarPanes.callstack.update(details);function didCreateBreakpointHitStatusMessage(element)
{this.sidebarPanes.callstack.setStatus(element);}
function didGetUILocation(uiLocation)
{var breakpoint=WebInspector.breakpointManager.findBreakpointOnLine(uiLocation.uiSourceCode,uiLocation.lineNumber);if(!breakpoint)
return;this.sidebarPanes.jsBreakpoints.highlightBreakpoint(breakpoint);this.sidebarPanes.callstack.setStatus(WebInspector.UIString("Paused on a JavaScript breakpoint."));}
if(details.reason===WebInspector.DebuggerModel.BreakReason.DOM){WebInspector.domBreakpointsSidebarPane.highlightBreakpoint(details.auxData);WebInspector.domBreakpointsSidebarPane.createBreakpointHitStatusMessage(details,didCreateBreakpointHitStatusMessage.bind(this));}else if(details.reason===WebInspector.DebuggerModel.BreakReason.EventListener){var eventName=details.auxData["eventName"];var targetName=details.auxData["targetName"];this.sidebarPanes.eventListenerBreakpoints.highlightBreakpoint(eventName,targetName);var eventNameForUI=WebInspector.EventListenerBreakpointsSidebarPane.eventNameForUI(eventName,details.auxData);this.sidebarPanes.callstack.setStatus(WebInspector.UIString("Paused on a \"%s\" Event Listener.",eventNameForUI));}else if(details.reason===WebInspector.DebuggerModel.BreakReason.XHR){this.sidebarPanes.xhrBreakpoints.highlightBreakpoint(details.auxData["breakpointURL"]);this.sidebarPanes.callstack.setStatus(WebInspector.UIString("Paused on a XMLHttpRequest."));}else if(details.reason===WebInspector.DebuggerModel.BreakReason.Exception){var description=details.auxData["description"]||"";this.sidebarPanes.callstack.setStatus(WebInspector.UIString("Paused on exception: '%s'.",description.split("\n",1)[0]));}else if(details.reason===WebInspector.DebuggerModel.BreakReason.PromiseRejection){var description=details.auxData["description"]||"";this.sidebarPanes.callstack.setStatus(WebInspector.UIString("Paused on promise rejection: '%s'.",description.split("\n",1)[0]));}else if(details.reason===WebInspector.DebuggerModel.BreakReason.Assert){this.sidebarPanes.callstack.setStatus(WebInspector.UIString("Paused on assertion."));}else if(details.reason===WebInspector.DebuggerModel.BreakReason.CSPViolation){this.sidebarPanes.callstack.setStatus(WebInspector.UIString("Paused on a script blocked due to Content Security Policy directive: \"%s\".",details.auxData["directiveText"]));}else if(details.reason===WebInspector.DebuggerModel.BreakReason.DebugCommand){this.sidebarPanes.callstack.setStatus(WebInspector.UIString("Paused on a debugged function."));}else if(details.reason===WebInspector.DebuggerModel.BreakReason.AsyncOperation){if(Runtime.experiments.isEnabled("stepIntoAsync")){var operationId=details.auxData["operationId"];var operation=this.sidebarPanes.asyncOperationBreakpoints.operationById(details.target(),operationId);var description=(operation&&operation.description)||WebInspector.UIString("<unknown>");this.sidebarPanes.callstack.setStatus(WebInspector.UIString("Paused on a \"%s\" async operation.",description));this.sidebarPanes.asyncOperationBreakpoints.highlightBreakpoint(operationId);}}else{if(details.callFrames.length)
WebInspector.debuggerWorkspaceBinding.createCallFrameLiveLocation(details.callFrames[0],didGetUILocation.bind(this));else
console.warn("ScriptsPanel paused, but callFrames.length is zero.");}
this._splitWidget.showBoth(true);this._toggleDebuggerSidebarButton.disabled=true;window.focus();InspectorFrontendHost.bringToFront();},_debuggerResumed:function(event)
{var debuggerModel=(event.target);var target=debuggerModel.target();if(WebInspector.context.flavor(WebInspector.Target)!==target)
return;this._paused=false;this._clearInterface();this._toggleDebuggerSidebarButton.disabled=false;this._switchToPausedTargetTimeout=setTimeout(this._switchToPausedTarget.bind(this,debuggerModel),500);},_debuggerWasEnabled:function(event)
{var target=(event.target.target());if(WebInspector.context.flavor(WebInspector.Target)!==target)
return;this._updateDebuggerButtons();},_debuggerReset:function(event)
{this._debuggerResumed(event);},get visibleView()
{return this._sourcesView.visibleView();},showUISourceCode:function(uiSourceCode,lineNumber,columnNumber)
{this._showEditor();this._sourcesView.showSourceLocation(uiSourceCode,lineNumber,columnNumber);},_showEditor:function()
{WebInspector.inspectorView.setCurrentPanel(this);},showUILocation:function(uiLocation)
{this.showUISourceCode(uiLocation.uiSourceCode,uiLocation.lineNumber,uiLocation.columnNumber);},_revealInNavigator:function(uiSourceCode)
{this._navigator.revealUISourceCode(uiSourceCode);},setIgnoreExecutionLineEvents:function(ignoreExecutionLineEvents)
{this._ignoreExecutionLineEvents=ignoreExecutionLineEvents;},_executionLineChanged:function(uiLocation)
{this._sourcesView.clearCurrentExecutionLine();this._sourcesView.setExecutionLocation(uiLocation);if(this._ignoreExecutionLineEvents)
return;this._sourcesView.showSourceLocation(uiLocation.uiSourceCode,uiLocation.lineNumber,uiLocation.columnNumber,undefined,true);},_callFrameSelected:function(event)
{var callFrame=(event.data);if(!callFrame||callFrame.target()!==WebInspector.context.flavor(WebInspector.Target))
return;this._selectCallFrame(callFrame);},_selectCallFrame:function(callFrame)
{this.sidebarPanes.scopechain.update(callFrame);this.sidebarPanes.watchExpressions.refreshExpressions();this.sidebarPanes.callstack.setSelectedCallFrame(callFrame);WebInspector.debuggerWorkspaceBinding.createCallFrameLiveLocation(callFrame,this._executionLineChanged.bind(this));},_sourceSelected:function(event)
{var uiSourceCode=(event.data.uiSourceCode);this._sourcesView.showSourceLocation(uiSourceCode,undefined,undefined,!event.data.focusSource)},_sourceRenamed:function(event)
{var uiSourceCode=(event.data);this._sourcesView.sourceRenamed(uiSourceCode);},_pauseOnExceptionEnabledChanged:function()
{var enabled=WebInspector.moduleSetting("pauseOnExceptionEnabled").get();this._pauseOnExceptionButton.setToggled(enabled);this._pauseOnExceptionButton.setTitle(WebInspector.UIString(enabled?"Don't pause on exceptions":"Pause on exceptions"));this._debugToolbarDrawer.classList.toggle("expanded",enabled);},_updateDebuggerButtons:function()
{var currentTarget=WebInspector.context.flavor(WebInspector.Target);var currentDebuggerModel=WebInspector.DebuggerModel.fromTarget(currentTarget);if(!currentDebuggerModel){this._pauseButton.setEnabled(false);this._stepOverButton.setEnabled(false);this._stepIntoButton.setEnabled(false);this._stepOutButton.setEnabled(false);}else if(this._paused){this._pauseButton.setTitle(WebInspector.UIString("Resume script execution"));this._pauseButton.setToggled(true);this._pauseButton.setLongClickOptionsEnabled((function(){return[this._longResumeButton];}).bind(this));this._pauseButton.setEnabled(true);this._stepOverButton.setEnabled(true);this._stepIntoButton.setEnabled(true);this._stepOutButton.setEnabled(true);}else{this._pauseButton.setTitle(WebInspector.UIString("Pause script execution"));this._pauseButton.setToggled(false);this._pauseButton.setLongClickOptionsEnabled(null);this._pauseButton.setEnabled(!currentDebuggerModel.isPausing());this._stepOverButton.setEnabled(false);this._stepIntoButton.setEnabled(false);this._stepOutButton.setEnabled(false);}},_clearInterface:function()
{this.sidebarPanes.callstack.update(null);this.sidebarPanes.scopechain.update(null);this.sidebarPanes.jsBreakpoints.clearBreakpointHighlight();WebInspector.domBreakpointsSidebarPane.clearBreakpointHighlight();this.sidebarPanes.eventListenerBreakpoints.clearBreakpointHighlight();this.sidebarPanes.xhrBreakpoints.clearBreakpointHighlight();if(this.sidebarPanes.asyncOperationBreakpoints)
this.sidebarPanes.asyncOperationBreakpoints.clearBreakpointHighlight();this._sourcesView.clearCurrentExecutionLine();this._updateDebuggerButtons();if(this._switchToPausedTargetTimeout)
clearTimeout(this._switchToPausedTargetTimeout);},_switchToPausedTarget:function(debuggerModel)
{delete this._switchToPausedTargetTimeout;if(this._paused)
return;var target=WebInspector.context.flavor(WebInspector.Target);if(debuggerModel.isPaused())
return;var debuggerModels=WebInspector.DebuggerModel.instances();for(var i=0;i<debuggerModels.length;++i){if(debuggerModels[i].isPaused()){WebInspector.context.setFlavor(WebInspector.Target,debuggerModels[i].target());break;}}},_togglePauseOnExceptions:function()
{WebInspector.moduleSetting("pauseOnExceptionEnabled").set(!this._pauseOnExceptionButton.toggled());},_runSnippet:function()
{var uiSourceCode=this._sourcesView.currentUISourceCode();if(uiSourceCode.project().type()!==WebInspector.projectTypes.Snippets)
return false;var currentExecutionContext=WebInspector.context.flavor(WebInspector.ExecutionContext);if(!currentExecutionContext)
return false;WebInspector.scriptSnippetModel.evaluateScriptSnippet(currentExecutionContext,uiSourceCode);return true;},_editorSelected:function(event)
{var uiSourceCode=(event.data);this._editorChanged(uiSourceCode);},_editorClosed:function(event)
{var wasSelected=(event.data.wasSelected);if(wasSelected)
this._editorChanged(null);},_editorChanged:function(uiSourceCode)
{var isSnippet=uiSourceCode&&uiSourceCode.project().type()===WebInspector.projectTypes.Snippets;this._runSnippetButton.element.classList.toggle("hidden",!isSnippet);},togglePause:function()
{var target=WebInspector.context.flavor(WebInspector.Target);if(!target)
return true;var debuggerModel=WebInspector.DebuggerModel.fromTarget(target);if(!debuggerModel)
return true;if(this._paused){this._paused=false;debuggerModel.resume();}else{debuggerModel.pause();}
this._clearInterface();return true;},_prepareToResume:function()
{if(!this._paused)
return null;this._paused=false;this._clearInterface();var target=WebInspector.context.flavor(WebInspector.Target);return target?WebInspector.DebuggerModel.fromTarget(target):null;},_longResume:function()
{var debuggerModel=this._prepareToResume();if(!debuggerModel)
return true;debuggerModel.skipAllPausesUntilReloadOrTimeout(500);debuggerModel.resume();return true;},_stepOverClicked:function()
{var debuggerModel=this._prepareToResume();if(!debuggerModel)
return true;debuggerModel.stepOver();return true;},_stepIntoClicked:function()
{var debuggerModel=this._prepareToResume();if(!debuggerModel)
return true;debuggerModel.stepInto();return true;},_stepIntoAsyncClicked:function()
{var debuggerModel=this._prepareToResume();if(!debuggerModel)
return true;debuggerModel.stepIntoAsync();return true;},_stepOutClicked:function()
{var debuggerModel=this._prepareToResume();if(!debuggerModel)
return true;debuggerModel.stepOut();return true;},_callFrameSelectedInSidebar:function(event)
{var callFrame=(event.data);callFrame.debuggerModel.setSelectedCallFrame(callFrame);},_hiddenCallFramesRevealedInSidebar:function()
{if(Runtime.experiments.isEnabled("stepIntoAsync"))
this.sidebarPanes.asyncOperationBreakpoints.revealHiddenCallFrames((WebInspector.context.flavor(WebInspector.Target)));},_continueToLocation:function(uiLocation)
{var executionContext=WebInspector.context.flavor(WebInspector.ExecutionContext);if(!executionContext)
return;var rawLocation=WebInspector.debuggerWorkspaceBinding.uiLocationToRawLocation(executionContext.target(),uiLocation.uiSourceCode,uiLocation.lineNumber,0);if(!rawLocation)
return;if(!this._prepareToResume())
return;rawLocation.continueToLocation();},_toggleBreakpointsActive:function()
{WebInspector.breakpointManager.setBreakpointsActive(!WebInspector.breakpointManager.breakpointsActive());},_breakpointsActiveStateChanged:function(event)
{var active=event.data;this._toggleBreakpointsButton.setToggled(!active);this.sidebarPanes.jsBreakpoints.listElement.classList.toggle("breakpoints-list-deactivated",!active);this._sourcesView.toggleBreakpointsActiveState(active);if(active)
this._toggleBreakpointsButton.setTitle(WebInspector.UIString("Deactivate breakpoints"));else
this._toggleBreakpointsButton.setTitle(WebInspector.UIString("Activate breakpoints"));},_createDebugToolbar:function()
{var debugToolbar=new WebInspector.Toolbar();debugToolbar.element.classList.add("scripts-debug-toolbar");var title,handler;title=WebInspector.UIString("Run snippet");handler=this._runSnippet.bind(this);this._runSnippetButton=WebInspector.ToolbarButton.createActionButton("debugger.run-snippet");debugToolbar.appendToolbarItem(this._runSnippetButton);this._runSnippetButton.element.classList.add("hidden");this._pauseButton=WebInspector.ToolbarButton.createActionButton("debugger.toggle-pause");debugToolbar.appendToolbarItem(this._pauseButton);title=WebInspector.UIString("Resume with all pauses blocked for 500 ms");this._longResumeButton=new WebInspector.ToolbarButton(title,"play-toolbar-item");this._longResumeButton.addEventListener("click",this._longResume.bind(this),this);title=WebInspector.UIString("Step over next function call");this._stepOverButton=WebInspector.ToolbarButton.createActionButton("debugger.step-over");debugToolbar.appendToolbarItem(this._stepOverButton);title=WebInspector.UIString("Step into next function call");this._stepIntoButton=WebInspector.ToolbarButton.createActionButton("debugger.step-into");debugToolbar.appendToolbarItem(this._stepIntoButton);title=WebInspector.UIString("Step out of current function");this._stepOutButton=WebInspector.ToolbarButton.createActionButton("debugger.step-out");debugToolbar.appendToolbarItem(this._stepOutButton);debugToolbar.appendSeparator();this._toggleBreakpointsButton=WebInspector.ToolbarButton.createActionButton("debugger.toggle-breakpoints-active");this._toggleBreakpointsButton.setToggled(false);this._toggleBreakpointsButton.setTitle(WebInspector.UIString("Deactivate breakpoints"));debugToolbar.appendToolbarItem(this._toggleBreakpointsButton);this._pauseOnExceptionButton=new WebInspector.ToolbarButton("","pause-on-exceptions-toolbar-item");this._pauseOnExceptionButton.addEventListener("click",this._togglePauseOnExceptions,this);debugToolbar.appendToolbarItem(this._pauseOnExceptionButton);debugToolbar.appendSeparator();debugToolbar.appendToolbarItem(new WebInspector.ToolbarCheckbox(WebInspector.UIString("Async"),WebInspector.UIString("Capture async stack traces"),WebInspector.moduleSetting("enableAsyncStackTraces")));return debugToolbar;},_createDebugToolbarDrawer:function()
{var debugToolbarDrawer=createElementWithClass("div","scripts-debug-toolbar-drawer");var label=WebInspector.UIString("Pause On Caught Exceptions");var setting=WebInspector.moduleSetting("pauseOnCaughtException");debugToolbarDrawer.appendChild(WebInspector.SettingsUI.createSettingCheckbox(label,setting,true));return debugToolbarDrawer;},addToWatch:function(expression)
{this.sidebarPanes.watchExpressions.addExpression(expression);},_installDebuggerSidebarController:function()
{this.editorView.displayShowHideSidebarButton("navigator");this._toggleDebuggerSidebarButton=this._splitWidget.displayShowHideSidebarButton("debugger","scripts-debugger-show-hide-button");},_showLocalHistory:function(uiSourceCode)
{WebInspector.RevisionHistoryView.showHistory(uiSourceCode);},appendApplicableItems:function(event,contextMenu,target)
{this._appendUISourceCodeItems(event,contextMenu,target);this.appendUILocationItems(contextMenu,target);this._appendRemoteObjectItems(contextMenu,target);this._appendNetworkRequestItems(contextMenu,target);},mapFileSystemToNetwork:function(uiSourceCode)
{WebInspector.SelectUISourceCodeForProjectTypesDialog.show(uiSourceCode.name(),[WebInspector.projectTypes.Network,WebInspector.projectTypes.ContentScripts],mapFileSystemToNetwork.bind(this));function mapFileSystemToNetwork(networkUISourceCode)
{if(!networkUISourceCode)
return;this._networkMapping.addMapping(networkUISourceCode,uiSourceCode);}},mapNetworkToFileSystem:function(networkUISourceCode)
{WebInspector.SelectUISourceCodeForProjectTypesDialog.show(networkUISourceCode.name(),[WebInspector.projectTypes.FileSystem],mapNetworkToFileSystem.bind(this));function mapNetworkToFileSystem(uiSourceCode)
{if(!uiSourceCode)
return;this._networkMapping.addMapping(networkUISourceCode,uiSourceCode);}},_removeNetworkMapping:function(uiSourceCode)
{this._networkMapping.removeMapping(uiSourceCode);},_appendUISourceCodeMappingItems:function(contextMenu,uiSourceCode)
{WebInspector.NavigatorView.appendAddFolderItem(contextMenu);if(uiSourceCode.project().type()===WebInspector.projectTypes.FileSystem){var hasMappings=!!this._networkMapping.networkURL(uiSourceCode);if(!hasMappings)
contextMenu.appendItem(WebInspector.UIString.capitalize("Map to ^network ^resource\u2026"),this.mapFileSystemToNetwork.bind(this,uiSourceCode));else
contextMenu.appendItem(WebInspector.UIString.capitalize("Remove ^network ^mapping"),this._removeNetworkMapping.bind(this,uiSourceCode));}
function filterProject(project)
{return project.type()===WebInspector.projectTypes.FileSystem;}
if(uiSourceCode.project().type()===WebInspector.projectTypes.Network||uiSourceCode.project().type()===WebInspector.projectTypes.ContentScripts){if(!this._workspace.projects().filter(filterProject).length)
return;var networkURL=this._networkMapping.networkURL(uiSourceCode);if(this._networkMapping.uiSourceCodeForURLForAnyTarget(networkURL)===uiSourceCode)
contextMenu.appendItem(WebInspector.UIString.capitalize("Map to ^file ^system ^resource\u2026"),this.mapNetworkToFileSystem.bind(this,uiSourceCode));}},_appendUISourceCodeItems:function(event,contextMenu,target)
{if(!(target instanceof WebInspector.UISourceCode))
return;var uiSourceCode=(target);var projectType=uiSourceCode.project().type();if(projectType!==WebInspector.projectTypes.Debugger&&!event.target.isSelfOrDescendant(this._navigator.view.element)){contextMenu.appendItem(WebInspector.UIString.capitalize("Reveal in ^navigator"),this._handleContextMenuReveal.bind(this,uiSourceCode));contextMenu.appendSeparator();}
this._appendUISourceCodeMappingItems(contextMenu,uiSourceCode);if(projectType!==WebInspector.projectTypes.FileSystem)
contextMenu.appendItem(WebInspector.UIString.capitalize("Local ^modifications\u2026"),this._showLocalHistory.bind(this,uiSourceCode));},appendUILocationItems:function(contextMenu,object)
{if(!(object instanceof WebInspector.UILocation))
return;var uiLocation=(object);var uiSourceCode=uiLocation.uiSourceCode;var projectType=uiSourceCode.project().type();var contentType=uiSourceCode.contentType();if(contentType===WebInspector.resourceTypes.Script||contentType===WebInspector.resourceTypes.Document){var target=WebInspector.context.flavor(WebInspector.Target);var debuggerModel=WebInspector.DebuggerModel.fromTarget(target);if(debuggerModel&&debuggerModel.isPaused())
contextMenu.appendItem(WebInspector.UIString.capitalize("Continue to ^here"),this._continueToLocation.bind(this,uiLocation));}
if((contentType===WebInspector.resourceTypes.Script||contentType===WebInspector.resourceTypes.Document)&&projectType!==WebInspector.projectTypes.Snippets){var networkURL=this._networkMapping.networkURL(uiSourceCode);var url=projectType===WebInspector.projectTypes.Formatter?uiSourceCode.originURL():networkURL;this.sidebarPanes.callstack.appendBlackboxURLContextMenuItems(contextMenu,url,projectType===WebInspector.projectTypes.ContentScripts);}},_handleContextMenuReveal:function(uiSourceCode)
{this.editorView.showBoth();this._revealInNavigator(uiSourceCode);},_appendRemoteObjectItems:function(contextMenu,target)
{if(!(target instanceof WebInspector.RemoteObject))
return;var remoteObject=(target);contextMenu.appendItem(WebInspector.UIString.capitalize("Store as ^global ^variable"),this._saveToTempVariable.bind(this,remoteObject));if(remoteObject.type==="function")
contextMenu.appendItem(WebInspector.UIString.capitalize("Show ^function ^definition"),this._showFunctionDefinition.bind(this,remoteObject));if(remoteObject.subtype==="generator")
contextMenu.appendItem(WebInspector.UIString.capitalize("Show ^generator ^location"),this._showGeneratorLocation.bind(this,remoteObject));},_appendNetworkRequestItems:function(contextMenu,target)
{if(!(target instanceof WebInspector.NetworkRequest))
return;var request=(target);var uiSourceCode=this._networkMapping.uiSourceCodeForURLForAnyTarget(request.url);if(!uiSourceCode)
return;var openText=WebInspector.UIString.capitalize("Open in Sources ^panel");contextMenu.appendItem(openText,this.showUILocation.bind(this,uiSourceCode.uiLocation(0,0)));},_saveToTempVariable:function(remoteObject)
{var currentExecutionContext=WebInspector.context.flavor(WebInspector.ExecutionContext);if(!currentExecutionContext)
return;currentExecutionContext.globalObject("",false,false,didGetGlobalObject);function didGetGlobalObject(global,wasThrown)
{function remoteFunction(value)
{var prefix="temp";var index=1;while((prefix+index)in this)
++index;var name=prefix+index;this[name]=value;return name;}
if(wasThrown||!global)
failedToSave(global);else
global.callFunction(remoteFunction,[WebInspector.RemoteObject.toCallArgument(remoteObject)],didSave.bind(null,global));}
function didSave(global,result,wasThrown)
{global.release();if(wasThrown||!result||result.type!=="string")
failedToSave(result);else
WebInspector.ConsoleModel.evaluateCommandInConsole((currentExecutionContext),result.value);}
function failedToSave(result)
{var message=WebInspector.UIString("Failed to save to temp variable.");if(result){message+=" "+result.description;result.release();}
WebInspector.console.error(message);}},_showFunctionDefinition:function(remoteObject)
{remoteObject.debuggerModel().functionDetails(remoteObject,this._didGetFunctionOrGeneratorObjectDetails.bind(this));},_showGeneratorLocation:function(remoteObject)
{remoteObject.debuggerModel().generatorObjectDetails(remoteObject,this._didGetFunctionOrGeneratorObjectDetails.bind(this));},_didGetFunctionOrGeneratorObjectDetails:function(response)
{if(!response||!response.location)
return;var location=response.location;if(!location)
return;var uiLocation=WebInspector.debuggerWorkspaceBinding.rawLocationToUILocation(location);if(uiLocation)
this.showUILocation(uiLocation);},showGoToSourceDialog:function()
{this._sourcesView.showOpenResourceDialog();},_updateSidebarPosition:function()
{var vertically;var position=WebInspector.moduleSetting("sidebarPosition").get();if(position==="right")
vertically=false;else if(position==="bottom")
vertically=true;else
vertically=WebInspector.inspectorView.element.offsetWidth<680;if(this.sidebarPaneView&&vertically===!this._splitWidget.isVertical())
return;if(this.sidebarPaneView&&this.sidebarPaneView.shouldHideOnDetach())
return;if(this.sidebarPaneView)
this.sidebarPaneView.detach();this._splitWidget.setVertical(!vertically);this._splitWidget.element.classList.toggle("sources-split-view-vertical",vertically);if(!vertically)
this._splitWidget.uninstallResizer(this._sourcesView.toolbarContainerElement());else
this._splitWidget.installResizer(this._sourcesView.toolbarContainerElement());var vbox=new WebInspector.VBox();vbox.element.appendChild(this._debugToolbarDrawer);vbox.setMinimumAndPreferredSizes(25,25,WebInspector.SourcesPanel.minToolbarWidth,100);var sidebarPaneStack=new WebInspector.SidebarPaneStack();sidebarPaneStack.element.classList.add("flex-auto");sidebarPaneStack.show(vbox.element);vbox.element.appendChild(this._debugToolbar.element);if(!vertically){for(var pane in this.sidebarPanes)
sidebarPaneStack.addPane(this.sidebarPanes[pane]);this._extensionSidebarPanesContainer=sidebarPaneStack;this.sidebarPaneView=vbox;this.sidebarPanes.scopechain.expand();this.sidebarPanes.watchExpressions.expandIfNecessary();}else{var splitWidget=new WebInspector.SplitWidget(true,true,"sourcesPanelDebuggerSidebarSplitViewState",0.5);splitWidget.setMainWidget(vbox);sidebarPaneStack.addPane(this.sidebarPanes.threads);sidebarPaneStack.addPane(this.sidebarPanes.callstack);sidebarPaneStack.addPane(this.sidebarPanes.jsBreakpoints);sidebarPaneStack.addPane(this.sidebarPanes.domBreakpoints);sidebarPaneStack.addPane(this.sidebarPanes.xhrBreakpoints);sidebarPaneStack.addPane(this.sidebarPanes.eventListenerBreakpoints);sidebarPaneStack.addPane(this.sidebarPanes.objectEventListeners);if(Runtime.experiments.isEnabled("stepIntoAsync"))
sidebarPaneStack.addPane(this.sidebarPanes.asyncOperationBreakpoints);var tabbedPane=new WebInspector.SidebarTabbedPane();splitWidget.setSidebarWidget(tabbedPane);tabbedPane.addPane(this.sidebarPanes.scopechain);tabbedPane.addPane(this.sidebarPanes.watchExpressions);if(this.sidebarPanes.serviceWorkers)
tabbedPane.addPane(this.sidebarPanes.serviceWorkers);tabbedPane.selectTab(this._lastSelectedTabSetting.get());tabbedPane.addEventListener(WebInspector.TabbedPane.EventTypes.TabSelected,this._tabSelected,this);this._extensionSidebarPanesContainer=tabbedPane;this.sidebarPaneView=splitWidget;}
var extensionSidebarPanes=WebInspector.extensionServer.sidebarPanes();for(var i=0;i<extensionSidebarPanes.length;++i)
this._addExtensionSidebarPane(extensionSidebarPanes[i]);this._splitWidget.setSidebarWidget(this.sidebarPaneView);this.sidebarPanes.threads.expand();this.sidebarPanes.jsBreakpoints.expand();this.sidebarPanes.callstack.expand();},_tabSelected:function(event)
{this._lastSelectedTabSetting.set(event.data.tabId);},_extensionSidebarPaneAdded:function(event)
{var pane=(event.data);this._addExtensionSidebarPane(pane);},_addExtensionSidebarPane:function(pane)
{if(pane.panelName()===this.name)
this._extensionSidebarPanesContainer.addPane(pane);},sourcesView:function()
{return this._sourcesView;},_handleDrop:function(dataTransfer)
{var items=dataTransfer.items;if(!items.length)
return;var entry=items[0].webkitGetAsEntry();if(!entry.isDirectory)
return;InspectorFrontendHost.upgradeDraggedFileSystemPermissions(entry.filesystem);},__proto__:WebInspector.Panel.prototype}
WebInspector.SourcesPanel.ContextMenuProvider=function()
{}
WebInspector.SourcesPanel.ContextMenuProvider.prototype={appendApplicableItems:function(event,contextMenu,target)
{WebInspector.SourcesPanel.instance().appendApplicableItems(event,contextMenu,target);}}
WebInspector.SourcesPanel.UILocationRevealer=function()
{}
WebInspector.SourcesPanel.UILocationRevealer.prototype={reveal:function(uiLocation)
{if(!(uiLocation instanceof WebInspector.UILocation))
return Promise.reject(new Error("Internal error: not a ui location"));WebInspector.SourcesPanel.instance().showUILocation(uiLocation);return Promise.resolve();}}
WebInspector.SourcesPanel.DebuggerLocationRevealer=function()
{}
WebInspector.SourcesPanel.DebuggerLocationRevealer.prototype={reveal:function(rawLocation)
{if(!(rawLocation instanceof WebInspector.DebuggerModel.Location))
return Promise.reject(new Error("Internal error: not a debugger location"));WebInspector.SourcesPanel.instance().showUILocation(WebInspector.debuggerWorkspaceBinding.rawLocationToUILocation(rawLocation));return Promise.resolve();}}
WebInspector.SourcesPanel.UISourceCodeRevealer=function()
{}
WebInspector.SourcesPanel.UISourceCodeRevealer.prototype={reveal:function(uiSourceCode)
{if(!(uiSourceCode instanceof WebInspector.UISourceCode))
return Promise.reject(new Error("Internal error: not a ui source code"));WebInspector.SourcesPanel.instance().showUISourceCode(uiSourceCode);return Promise.resolve();}}
WebInspector.SourcesPanel.DebuggerPausedDetailsRevealer=function()
{}
WebInspector.SourcesPanel.DebuggerPausedDetailsRevealer.prototype={reveal:function(object)
{WebInspector.inspectorView.setCurrentPanel(WebInspector.SourcesPanel.instance());return Promise.resolve();}}
WebInspector.SourcesPanel.RevealingActionDelegate=function(){}
WebInspector.SourcesPanel.RevealingActionDelegate.prototype={handleAction:function(context,actionId)
{var panel=WebInspector.SourcesPanel.instance();WebInspector.inspectorView.setCurrentPanel(panel);switch(actionId){case"debugger.toggle-pause":panel.togglePause();return true;case"sources.go-to-source":panel.showGoToSourceDialog();return true;}
return false;}}
WebInspector.SourcesPanel.DebuggingActionDelegate=function()
{}
WebInspector.SourcesPanel.DebuggingActionDelegate.prototype={handleAction:function(context,actionId)
{var panel=WebInspector.SourcesPanel.instance();switch(actionId){case"debugger.step-over":panel._stepOverClicked();return true;case"debugger.step-into":panel._stepIntoClicked();return true;case"debugger.step-into-async":panel._stepIntoAsyncClicked();return true;case"debugger.step-out":panel._stepOutClicked();return true;case"debugger.run-snippet":panel._runSnippet();return true;case"debugger.toggle-breakpoints-active":panel._toggleBreakpointsActive();return true;}
return false;}}
WebInspector.SourcesPanel.show=function()
{WebInspector.inspectorView.setCurrentPanel(WebInspector.SourcesPanel.instance());}
WebInspector.SourcesPanel.instance=function()
{if(!WebInspector.SourcesPanel._instanceObject)
WebInspector.SourcesPanel._instanceObject=new WebInspector.SourcesPanel();return WebInspector.SourcesPanel._instanceObject;}
WebInspector.SourcesPanelFactory=function()
{}
WebInspector.SourcesPanelFactory.prototype={createPanel:function()
{return WebInspector.SourcesPanel.instance();}};WebInspector.WorkspaceMappingTip=function(sourcesPanel,workspace)
{this._sourcesPanel=sourcesPanel;this._workspace=workspace;this._sourcesView=this._sourcesPanel.sourcesView();this._workspaceInfobarDisabledSetting=WebInspector.settings.createSetting("workspaceInfobarDisabled",false);this._workspaceMappingInfobarDisabledSetting=WebInspector.settings.createSetting("workspaceMappingInfobarDisabled",false);if(this._workspaceInfobarDisabledSetting.get()&&this._workspaceMappingInfobarDisabledSetting.get())
return;this._sourcesView.addEventListener(WebInspector.SourcesView.Events.EditorSelected,this._editorSelected.bind(this));}
WebInspector.WorkspaceMappingTip._infobarSymbol=Symbol("infobar");WebInspector.WorkspaceMappingTip.prototype={_editorSelected:function(event)
{var uiSourceCode=(event.data);if(this._editorSelectedTimer)
clearTimeout(this._editorSelectedTimer);this._editorSelectedTimer=setTimeout(this._updateSuggestedMappingInfobar.bind(this,uiSourceCode),3000);},_updateSuggestedMappingInfobar:function(uiSourceCode)
{var uiSourceCodeFrame=this._sourcesView.viewForFile(uiSourceCode);if(!uiSourceCodeFrame.isShowing())
return;if(uiSourceCode[WebInspector.WorkspaceMappingTip._infobarSymbol])
return;if(!this._workspaceMappingInfobarDisabledSetting.get()&&uiSourceCode.project().type()===WebInspector.projectTypes.FileSystem){var networkURL=WebInspector.networkMapping.networkURL(uiSourceCode);var hasMappings=!!networkURL;if(hasMappings)
return;var networkProjects=this._workspace.projectsForType(WebInspector.projectTypes.Network);networkProjects=networkProjects.concat(this._workspace.projectsForType(WebInspector.projectTypes.ContentScripts));for(var i=0;i<networkProjects.length;++i){if(!this._isLocalHost(networkProjects[i].url()))
continue;var name=uiSourceCode.name();var networkUiSourceCodes=networkProjects[i].uiSourceCodes();for(var j=0;j<networkUiSourceCodes.length;++j){if(networkUiSourceCodes[j].name()===name){this._showMappingInfobar(uiSourceCode,false);return;}}}}
if(uiSourceCode.project().type()===WebInspector.projectTypes.Network||uiSourceCode.project().type()===WebInspector.projectTypes.ContentScripts){if(!this._isLocalHost(uiSourceCode.originURL()))
return;var networkURL=WebInspector.networkMapping.networkURL(uiSourceCode);if(WebInspector.networkMapping.uiSourceCodeForURLForAnyTarget(networkURL)!==uiSourceCode)
return;var filesystemProjects=this._workspace.projectsForType(WebInspector.projectTypes.FileSystem);for(var i=0;i<filesystemProjects.length;++i){var name=uiSourceCode.name();var fsUiSourceCodes=filesystemProjects[i].uiSourceCodes();for(var j=0;j<fsUiSourceCodes.length;++j){if(fsUiSourceCodes[j].name()===name){if(!this._workspaceMappingInfobarDisabledSetting.get())
this._showMappingInfobar(uiSourceCode,true);return;}}}
if(!this._workspaceInfobarDisabledSetting.get())
this._showWorkspaceInfobar(uiSourceCode);}},_isLocalHost:function(url)
{var parsedURL=url.asParsedURL();return!!parsedURL&&parsedURL.host==="localhost";},_showWorkspaceInfobar:function(uiSourceCode)
{var infobar=new WebInspector.UISourceCodeFrame.Infobar(WebInspector.Infobar.Type.Info,WebInspector.UIString("Serving from the file system? Add your files into the workspace."),this._workspaceInfobarDisabledSetting);infobar.createDetailsRowMessage(WebInspector.UIString("If you add files into your DevTools workspace, your changes will be persisted to disk."));infobar.createDetailsRowMessage(WebInspector.UIString("To add a folder into the workspace, drag and drop it into the Sources panel."));this._appendInfobar(uiSourceCode,infobar);},_showMappingInfobar:function(uiSourceCode,isNetwork)
{var title;if(isNetwork)
title=WebInspector.UIString("Map network resource '%s' to workspace?",uiSourceCode.originURL());else
title=WebInspector.UIString("Map workspace resource '%s' to network?",uiSourceCode.path());var infobar=new WebInspector.UISourceCodeFrame.Infobar(WebInspector.Infobar.Type.Info,title,this._workspaceMappingInfobarDisabledSetting);infobar.createDetailsRowMessage(WebInspector.UIString("You can map files in your workspace to the ones loaded over the network. As a result, changes made in DevTools will be persisted to disk."));infobar.createDetailsRowMessage(WebInspector.UIString("Use context menu to establish the mapping at any time."));var anchor=createElementWithClass("a","link");anchor.textContent=WebInspector.UIString("Establish the mapping now...");anchor.addEventListener("click",this._establishTheMapping.bind(this,uiSourceCode),false);infobar.createDetailsRowMessage("").appendChild(anchor);this._appendInfobar(uiSourceCode,infobar);},_establishTheMapping:function(uiSourceCode,event)
{event.consume(true);if(uiSourceCode.project().type()===WebInspector.projectTypes.FileSystem)
this._sourcesPanel.mapFileSystemToNetwork(uiSourceCode);else
this._sourcesPanel.mapNetworkToFileSystem(uiSourceCode);},_appendInfobar:function(uiSourceCode,infobar)
{var uiSourceCodeFrame=this._sourcesView.viewForFile(uiSourceCode);var rowElement=infobar.createDetailsRowMessage(WebInspector.UIString("For more information on workspaces, refer to the "));rowElement.appendChild(WebInspector.linkifyDocumentationURLAsNode("../setup/setup-workflow",WebInspector.UIString("workspaces documentation")));rowElement.createTextChild(".");uiSourceCode[WebInspector.WorkspaceMappingTip._infobarSymbol]=infobar;uiSourceCodeFrame.attachInfobars([infobar]);WebInspector.runCSSAnimationOnce(infobar.element,"source-frame-infobar-animation");}};WebInspector.XHRBreakpointsSidebarPane=function()
{WebInspector.BreakpointsSidebarPaneBase.call(this,WebInspector.UIString("XHR Breakpoints"));this._xhrBreakpointsSetting=WebInspector.settings.createLocalSetting("xhrBreakpoints",[]);this._breakpointElements=new Map();var addButton=new WebInspector.ToolbarButton(WebInspector.UIString("Add breakpoint"),"add-toolbar-item");addButton.addEventListener("click",this._addButtonClicked.bind(this));this.toolbar().appendToolbarItem(addButton);this.emptyElement.addEventListener("contextmenu",this._emptyElementContextMenu.bind(this),true);WebInspector.targetManager.observeTargets(this,WebInspector.Target.Type.Page);}
WebInspector.XHRBreakpointsSidebarPane.prototype={targetAdded:function(target)
{this._restoreBreakpoints(target);},targetRemoved:function(target){},_emptyElementContextMenu:function(event)
{var contextMenu=new WebInspector.ContextMenu(event);contextMenu.appendItem(WebInspector.UIString.capitalize("Add ^breakpoint"),this._addButtonClicked.bind(this));contextMenu.show();},_addButtonClicked:function(event)
{if(event)
event.consume();this.expand();var inputElementContainer=createElementWithClass("p","breakpoint-condition");inputElementContainer.textContent=WebInspector.UIString("Break when URL contains:");var inputElement=inputElementContainer.createChild("span","editing");inputElement.id="breakpoint-condition-input";this.addListElement(inputElementContainer,(this.listElement.firstChild));function finishEditing(accept,e,text)
{this.removeListElement(inputElementContainer);if(accept){this._setBreakpoint(text,true);this._saveBreakpoints();}}
var config=new WebInspector.InplaceEditor.Config(finishEditing.bind(this,true),finishEditing.bind(this,false));WebInspector.InplaceEditor.startEditing(inputElement,config);},_setBreakpoint:function(url,enabled,target)
{if(enabled)
this._updateBreakpointOnTarget(url,true,target);if(this._breakpointElements.has(url))
return;var element=createElement("li");element._url=url;element.addEventListener("contextmenu",this._contextMenu.bind(this,url),true);var title=url?WebInspector.UIString("URL contains \"%s\"",url):WebInspector.UIString("Any XHR");var label=createCheckboxLabel(title,enabled);element.appendChild(label);label.checkboxElement.addEventListener("click",this._checkboxClicked.bind(this,url),false);element._checkboxElement=label.checkboxElement;label.textElement.classList.add("cursor-auto");label.textElement.addEventListener("dblclick",this._labelClicked.bind(this,url),false);var currentElement=(this.listElement.firstChild);while(currentElement){if(currentElement._url&&currentElement._url<element._url)
break;currentElement=(currentElement.nextSibling);}
this.addListElement(element,currentElement);this._breakpointElements.set(url,element);},_removeBreakpoint:function(url,target)
{var element=this._breakpointElements.get(url);if(!element)
return;this.removeListElement(element);this._breakpointElements.delete(url);if(element._checkboxElement.checked)
this._updateBreakpointOnTarget(url,false,target);},_updateBreakpointOnTarget:function(url,enable,target)
{var targets=target?[target]:WebInspector.targetManager.targets(WebInspector.Target.Type.Page);for(var i=0;i<targets.length;++i){if(enable)
targets[i].domdebuggerAgent().setXHRBreakpoint(url);else
targets[i].domdebuggerAgent().removeXHRBreakpoint(url);}},_contextMenu:function(url,event)
{var contextMenu=new WebInspector.ContextMenu(event);function removeBreakpoint()
{this._removeBreakpoint(url);this._saveBreakpoints();}
function removeAllBreakpoints()
{for(var url of this._breakpointElements.keys())
this._removeBreakpoint(url);this._saveBreakpoints();}
var removeAllTitle=WebInspector.UIString.capitalize("Remove ^all ^breakpoints");contextMenu.appendItem(WebInspector.UIString.capitalize("Add ^breakpoint"),this._addButtonClicked.bind(this));contextMenu.appendItem(WebInspector.UIString.capitalize("Remove ^breakpoint"),removeBreakpoint.bind(this));contextMenu.appendItem(removeAllTitle,removeAllBreakpoints.bind(this));contextMenu.show();},_checkboxClicked:function(url,event)
{this._updateBreakpointOnTarget(url,event.target.checked);this._saveBreakpoints();},_labelClicked:function(url)
{var element=this._breakpointElements.get(url)||null;var inputElement=createElementWithClass("span","breakpoint-condition editing");inputElement.textContent=url;this.listElement.insertBefore(inputElement,element);element.classList.add("hidden");function finishEditing(accept,e,text)
{this.removeListElement(inputElement);if(accept){this._removeBreakpoint(url);this._setBreakpoint(text,element._checkboxElement.checked);this._saveBreakpoints();}else
element.classList.remove("hidden");}
WebInspector.InplaceEditor.startEditing(inputElement,new WebInspector.InplaceEditor.Config(finishEditing.bind(this,true),finishEditing.bind(this,false)));},highlightBreakpoint:function(url)
{var element=this._breakpointElements.get(url);if(!element)
return;this.expand();element.classList.add("breakpoint-hit");this._highlightedElement=element;},clearBreakpointHighlight:function()
{if(this._highlightedElement){this._highlightedElement.classList.remove("breakpoint-hit");delete this._highlightedElement;}},_saveBreakpoints:function()
{var breakpoints=[];for(var url of this._breakpointElements.keys())
breakpoints.push({url:url,enabled:this._breakpointElements.get(url)._checkboxElement.checked});this._xhrBreakpointsSetting.set(breakpoints);},_restoreBreakpoints:function(target)
{var breakpoints=this._xhrBreakpointsSetting.get();for(var i=0;i<breakpoints.length;++i){var breakpoint=breakpoints[i];if(breakpoint&&typeof breakpoint.url==="string")
this._setBreakpoint(breakpoint.url,breakpoint.enabled,target);}},__proto__:WebInspector.BreakpointsSidebarPaneBase.prototype};WebInspector.JavaScriptCompiler=function(sourceFrame)
{this._sourceFrame=sourceFrame;this._compiling=false;}
WebInspector.JavaScriptCompiler.CompileDelay=1000;WebInspector.JavaScriptCompiler.prototype={scheduleCompile:function()
{if(this._compiling){this._recompileScheduled=true;return;}
if(this._timeout)
clearTimeout(this._timeout);this._timeout=setTimeout(this._compile.bind(this),WebInspector.JavaScriptCompiler.CompileDelay);},_findTarget:function()
{var targets=WebInspector.targetManager.targets();var sourceCode=this._sourceFrame.uiSourceCode();for(var i=0;i<targets.length;++i){var scriptFile=WebInspector.debuggerWorkspaceBinding.scriptFile(sourceCode,targets[i]);if(scriptFile)
return targets[i];}
return WebInspector.targetManager.mainTarget();},_compile:function()
{var target=this._findTarget();if(!target)
return;var debuggerModel=WebInspector.DebuggerModel.fromTarget(target);if(!debuggerModel)
return;var currentExecutionContext=WebInspector.context.flavor(WebInspector.ExecutionContext);if(!currentExecutionContext)
return;this._compiling=true;var code=this._sourceFrame.textEditor.text();debuggerModel.compileScript(code,"",false,currentExecutionContext.id,compileCallback.bind(this,target));function compileCallback(target,scriptId,exceptionDetails)
{this._compiling=false;if(this._recompileScheduled){delete this._recompileScheduled;this.scheduleCompile();return;}
if(!exceptionDetails)
return;this._sourceFrame.uiSourceCode().addMessage(WebInspector.UISourceCode.Message.Level.Error,exceptionDetails.text,exceptionDetails.line-1,exceptionDetails.column+1);this._compilationFinishedForTest();}},_compilationFinishedForTest:function(){}};WebInspector.ObjectEventListenersSidebarPane=function()
{WebInspector.SidebarPane.call(this,"Event Listeners");this.element.classList.add("event-listeners-sidebar-pane");this._refreshButton=new WebInspector.ToolbarButton(WebInspector.UIString("Refresh"),"refresh-toolbar-item");this._refreshButton.addEventListener("click",this._refreshClick.bind(this));this._refreshButton.setEnabled(false);this.toolbar().appendToolbarItem(this._refreshButton);this._eventListenersView=new WebInspector.EventListenersView(this.element);}
WebInspector.ObjectEventListenersSidebarPane._objectGroupName="object-event-listeners-sidebar-pane";WebInspector.ObjectEventListenersSidebarPane.prototype={update:function()
{if(this._lastRequestedContext){this._lastRequestedContext.target().runtimeAgent().releaseObjectGroup(WebInspector.ObjectEventListenersSidebarPane._objectGroupName);delete this._lastRequestedContext;}
var executionContext=WebInspector.context.flavor(WebInspector.ExecutionContext);if(!executionContext){this._eventListenersView.reset();this._eventListenersView.addEmptyHolderIfNeeded();return;}
this._lastRequestedContext=executionContext;Promise.all([this._windowObjectInContext(executionContext)]).then(this._eventListenersView.addObjects.bind(this._eventListenersView));},wasShown:function()
{WebInspector.SidebarPane.prototype.wasShown.call(this);WebInspector.context.addFlavorChangeListener(WebInspector.ExecutionContext,this.update,this);this._refreshButton.setEnabled(true);this.update();},willHide:function()
{WebInspector.SidebarPane.prototype.willHide.call(this);WebInspector.context.removeFlavorChangeListener(WebInspector.ExecutionContext,this.update,this);this._refreshButton.setEnabled(false);},_windowObjectInContext:function(executionContext)
{return new Promise(windowObjectInContext);function windowObjectInContext(fulfill,reject)
{executionContext.evaluate("self",WebInspector.ObjectEventListenersSidebarPane._objectGroupName,false,true,false,false,mycallback);function mycallback(object)
{if(object)
fulfill(object);else
reject(null);}}},_refreshClick:function(event)
{event.consume();this.update();},__proto__:WebInspector.SidebarPane.prototype};Runtime.cachedResources["sources/addSourceMapURLDialog.css"]="/*\n * Copyright (c) 2015 The Chromium Authors. All rights reserved.\n * Use of this source code is governed by a BSD-style license that can be\n * found in the LICENSE file.\n */\n\n:host {\n    font-family: 'Lucida Grande', sans-serif;\n    font-size: inherit;\n    padding: 10px;\n}\n\n.widget {\n    align-items: center;\n}\n\nlabel {\n    white-space: nowrap;\n}\n\ninput[type=text] {\n    font-size: inherit;\n    height: 24px;\n    padding-left: 2px;\n    margin: 0 5px;\n}\n\ninput[type=\"search\"]:focus,\ninput[type=\"text\"]:focus {\n    outline: none;\n}\n\nbutton {\n    background-image: linear-gradient(hsl(0, 0%, 93%), hsl(0, 0%, 93%) 38%, hsl(0, 0%, 87%));\n    border: 1px solid hsla(0, 0%, 0%, 0.25);\n    border-radius: 2px;\n    box-shadow: 0 1px 0 hsla(0, 0%, 0%, 0.08), inset 0 1px 2px hsla(0, 100%, 100%, 0.75);\n    color: hsl(0, 0%, 27%);\n    font-size: 12px;\n    margin: 0 6px 0 6px;\n    text-shadow: 0 1px 0 hsl(0, 0%, 94%);\n    min-height: 2em;\n    padding-left: 10px;\n    padding-right: 10px;\n}\n\nbutton:active {\n    background-color: rgb(215, 215, 215);\n    background-image: linear-gradient(to bottom, rgb(194, 194, 194), rgb(239, 239, 239));\n}\n\n/*# sourceURL=sources/addSourceMapURLDialog.css */";Runtime.cachedResources["sources/filteredItemSelectionDialog.css"]="/*\n * Copyright (c) 2015 The Chromium Authors. All rights reserved.\n * Use of this source code is governed by a BSD-style license that can be\n * found in the LICENSE file.\n */\n\n.filtered-item-list-dialog {\n    display: flex;\n    flex-direction: column;\n    flex: auto;\n}\n\n.filtered-item-list-dialog > input[type=text] {\n    flex: 0 0 36px;\n    border: 0;\n    box-shadow: rgba(140, 140, 140, 0.5) 0 4px 16px;\n    margin: 0;\n    padding-left: 6px;\n    z-index: 1;\n    font-size: inherit;\n}\n\n.filtered-item-list-dialog > input[type=text]:focus {\n    outline: none;\n}\n\n.filtered-item-list-dialog > div.container {\n    flex: auto;\n    overflow-y: auto;\n    background: white;\n}\n\n.filtered-item-list-dialog-item {\n    padding: 6px;\n    white-space: nowrap;\n    text-overflow: ellipsis;\n    overflow: hidden;\n    color: rgb(95, 95, 95);\n}\n\n.filtered-item-list-dialog-item.selected {\n    background-color: #eee;\n}\n\n.filtered-item-list-dialog-item span.highlight {\n    color: #222;\n    font-weight: bold;\n}\n\n.filtered-item-list-dialog-item .filtered-item-list-dialog-title {\n    flex: auto;\n    overflow: hidden;\n    text-overflow: ellipsis;\n}\n\n.filtered-item-list-dialog-item .filtered-item-list-dialog-subtitle {\n    flex: none;\n    overflow: hidden;\n    text-overflow: ellipsis;\n    color: rgb(155, 155, 155);\n}\n\n.filtered-item-list-dialog-item.one-row {\n    display: flex;\n}\n\n.filtered-item-list-dialog-item.two-rows {\n    border-bottom: 1px solid rgb(235, 235, 235);\n}\n\n/*# sourceURL=sources/filteredItemSelectionDialog.css */";Runtime.cachedResources["sources/uiList.css"]="/*\n * Copyright 2015 The Chromium Authors. All rights reserved.\n * Use of this source code is governed by a BSD-style license that can be\n * found in the LICENSE file.\n */\n\n.list-item {\n    padding: 2px 8px 3px 8px;\n    position: relative;\n    min-height: 18px;\n    white-space: nowrap;\n}\n\n.list-item:nth-of-type(2n) {\n    background-color: #f8f8f8;\n}\n\n.list-item.selected {\n    background-color: #dadada;\n}\n\n.list-item.selected > .title {\n    font-weight: bold;\n}\n\n.list-item:hover {\n    background-color: #eee;\n}\n\n.list-item > .title {\n    font-weight: normal;\n    word-wrap: break-word;\n    white-space: normal;\n}\n\n.list-item > .subtitle {\n    margin-left: 5px;\n    color: rgba(0, 0, 0, 0.7);\n    text-overflow: ellipsis;\n    overflow: hidden;\n    float: right;\n}\n\n.list-item > .subtitle a {\n    color: inherit;\n}\n\n.list-item.label {\n    text-align: center;\n}\n\n.list-item.label .title,\n.list-item.label .subtitle {\n    font-style: italic;\n    font-weight: bold;\n    color: #999;\n}\n\n.list-item.dimmed {\n    opacity: 0.6;\n    font-style: italic;\n}\n\n/*# sourceURL=sources/uiList.css */";Runtime.cachedResources["sources/navigatorView.css"]="/*\n * Copyright (C) 2006, 2007, 2008 Apple Inc.  All rights reserved.\n * Copyright (C) 2009 Anthony Ricaud <rik@webkit.org>\n *\n * Redistribution and use in source and binary forms, with or without\n * modification, are permitted provided that the following conditions\n * are met:\n *\n * 1.  Redistributions of source code must retain the above copyright\n *     notice, this list of conditions and the following disclaimer.\n * 2.  Redistributions in binary form must reproduce the above copyright\n *     notice, this list of conditions and the following disclaimer in the\n *     documentation and/or other materials provided with the distribution.\n * 3.  Neither the name of Apple Computer, Inc. (\"Apple\") nor the names of\n *     its contributors may be used to endorse or promote products derived\n *     from this software without specific prior written permission.\n *\n * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS \"AS IS\" AND ANY\n * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED\n * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE\n * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY\n * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES\n * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;\n * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND\n * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT\n * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF\n * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n */\n\n.navigator-domain-tree-item .icon {\n    content: url(Images/domain.png);\n}\n\n.navigator-folder-tree-item .icon {\n    opacity: 0.7;\n    content: url(Images/frame.png);\n}\n\n.navigator-script-tree-item .icon {\n    content: url(Images/resourceJSIcon.png);\n}\n\n.navigator-stylesheet-tree-item .icon {\n    content: url(Images/resourceCSSIcon.png);\n}\n\n.navigator-document-tree-item .icon {\n    content: url(Images/resourceDocumentIcon.png);\n}\n\n.navigator-other-tree-item .icon {\n    content: url(Images/resourcePlainIcon.png);\n}\n\n.navigator > ol {\n    min-width: 100%;\n    min-height: 100%;\n    display: inline-block;\n    position: relative\n}\n\n.navigator li {\n    height: 18px;\n    line-height: 17px;\n    white-space: nowrap;\n}\n\n.navigator :focus li.selected {\n    color: white;\n}\n\n.navigator > ol.being-edited li.selected .selection {\n    background-color: rgb(56, 121, 217);\n}\n\n.navigator > ol.being-edited li.selected {\n    overflow: visible;\n}\n\n.navigator .icon {\n    width: 16px;\n    height: 16px;\n    float: left;\n}\n\n.navigator .base-navigator-tree-element-title {\n    display: inline-block;\n    position: relative;\n    padding-left: 2px;\n}\n\n.navigator .base-navigator-tree-element-title.editing {\n    margin: auto;\n}\n\n.navigator-tabbed-pane {\n    flex: auto;\n}\n\n.navigator-tabbed-pane .navigator {\n    padding-left: 0;\n    transform: translateZ(0);\n}\n\n/*# sourceURL=sources/navigatorView.css */";Runtime.cachedResources["sources/revisionHistory.css"]="/*\n * Copyright (C) 2012 Google Inc.  All rights reserved.\n *\n * Redistribution and use in source and binary forms, with or without\n * modification, are permitted provided that the following conditions\n * are met:\n *\n * 1.  Redistributions of source code must retain the above copyright\n *     notice, this list of conditions and the following disclaimer.\n * 2.  Redistributions in binary form must reproduce the above copyright\n *     notice, this list of conditions and the following disclaimer in the\n *     documentation and/or other materials provided with the distribution.\n * 3.  Neither the name of Apple Computer, Inc. (\"Apple\") nor the names of\n *     its contributors may be used to endorse or promote products derived\n *     from this software without specific prior written permission.\n *\n * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS \"AS IS\" AND ANY\n * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED\n * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE\n * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY\n * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES\n * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;\n * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND\n * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT\n * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF\n * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n */\n\n.outline-disclosure.revision-history-drawer {\n    -webkit-padding-start: 0;\n    overflow: auto;\n}\n\n.outline-disclosure.revision-history-drawer ol {\n    margin-top: 2px;\n    -webkit-padding-start: 0;\n    padding-left: 0 !important;\n}\n\n.outline-disclosure.revision-history-drawer > ol {\n    padding-left: 0;\n}\n\n.outline-disclosure.revision-history-drawer li {\n    padding-left: 6px;\n    margin-top: 0;\n    margin-bottom: 0;\n    height: 13px;\n}\n\n.outline-disclosure.revision-history-drawer li.parent {\n    margin-left: 4px;\n}\n\n.revision-history-link {\n    text-decoration: underline;\n    cursor: pointer;\n    color: #00e;\n    padding: 0 4px;\n}\n\n.revision-history-link-row {\n    padding-left: 16px;\n}\n\n.outline-disclosure.revision-history-drawer .revision-history-line {\n    padding-left: 0;\n    -webkit-user-select: text;\n}\n\n.revision-history-drawer .webkit-line-number {\n    border-right: 1px solid #BBB;\n    background-color: #F0F0F0;\n}\n\n.revision-history-drawer li.revision-history-revision {\n    padding-left: 16px;\n}\n\n.revision-history-line-added {\n    background-color: rgb(153, 238, 153);\n}\n\n.revision-history-line-removed {\n    background-color: rgb(255, 221, 221);\n}\n\n.revision-history-line-separator .webkit-line-number {\n    color: transparent;\n}\n\n.revision-history-line {\n    white-space: nowrap;\n}\n\n/*# sourceURL=sources/revisionHistory.css */";Runtime.cachedResources["sources/serviceWorkersSidebar.css"]="/*\n * Copyright 2015 The Chromium Authors. All rights reserved.\n * Use of this source code is governed by a BSD-style license that can be\n * found in the LICENSE file.\n */\n\n.service-worker {\n    padding: 5px 5px 5px 8px;\n    border-bottom: 1px solid #aaa;\n    display: flex;\n    align-items: center;\n}\n\n.service-worker:last-child {\n    border-bottom: none;\n}\n\n.service-worker-scope {\n    color: #777;\n    flex: auto;\n    margin: 5px 5px 0 0;\n    white-space: nowrap;\n    overflow: hidden;\n    text-overflow: ellipsis;\n}\n\n/*# sourceURL=sources/serviceWorkersSidebar.css */";Runtime.cachedResources["sources/sourcesPanel.css"]="/*\n * Copyright (C) 2006, 2007, 2008 Apple Inc.  All rights reserved.\n * Copyright (C) 2009 Anthony Ricaud <rik@webkit.org>\n *\n * Redistribution and use in source and binary forms, with or without\n * modification, are permitted provided that the following conditions\n * are met:\n *\n * 1.  Redistributions of source code must retain the above copyright\n *     notice, this list of conditions and the following disclaimer.\n * 2.  Redistributions in binary form must reproduce the above copyright\n *     notice, this list of conditions and the following disclaimer in the\n *     documentation and/or other materials provided with the distribution.\n * 3.  Neither the name of Apple Computer, Inc. (\"Apple\") nor the names of\n *     its contributors may be used to endorse or promote products derived\n *     from this software without specific prior written permission.\n *\n * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS \"AS IS\" AND ANY\n * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED\n * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE\n * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY\n * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES\n * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;\n * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND\n * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT\n * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF\n * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n */\n\n.scripts-debug-toolbar {\n    position: absolute;\n    top: 0;\n    width: 100%;\n    background-color: #f3f3f3;\n    border-bottom: 1px solid #ccc;\n    overflow: hidden;\n}\n\n.scripts-debug-toolbar-drawer {\n    flex: 0 0 52px;\n    -webkit-transition: margin-top 0.1s ease-in-out;\n    margin-top: -26px;\n    padding-top: 25px;\n    background-color: white;\n    overflow: hidden;\n}\n\n.material .scripts-debug-toolbar-drawer {\n    flex: 0 0 54px;\n    margin-top: -28px;\n    padding-top: 26px;\n}\n\n.scripts-debug-toolbar-drawer.expanded {\n    margin-top: 0;\n}\n\n.scripts-debug-toolbar-drawer > label {\n    display: flex;\n    padding-left: 3px;\n    height: 28px;\n}\n\n.sources-editor-tabstrip-left {\n    width: 22px;\n}\n\n.sources-editor-tabstrip-right {\n    width: 36px;\n}\n\n.function-location-link {\n    float: right;\n    margin-left: 10px;\n}\n\n.function-popover-title {\n    border-bottom: 1px solid #AAA;\n    margin-bottom: 3px;\n    padding-bottom: 2px;\n}\n\n.function-popover-title .function-name {\n    font-weight: bold;\n}\n\n.panel.sources .sidebar-pane-stack {\n    overflow: auto;\n}\n\n.source-frame-breakpoint-condition {\n    z-index: 30;\n    padding: 4px;\n    background-color: rgb(203, 226, 255);\n    border-radius: 7px;\n    border: 2px solid rgb(169, 172, 203);\n    width: 90%;\n    pointer-events: auto;\n}\n\n.source-frame-breakpoint-message {\n    background-color: transparent;\n    font-weight: normal;\n    font-size: 11px;\n    text-align: left;\n    text-shadow: none;\n    color: rgb(85, 85, 85);\n    cursor: default;\n    margin: 0 0 2px 0;\n}\n\n#source-frame-breakpoint-condition {\n    margin: 0;\n    border: 1px inset rgb(190, 190, 190) !important;\n    width: 100%;\n    box-shadow: none !important;\n    outline: none !important;\n    -webkit-user-modify: read-write;\n}\n\n.cursor-pointer {\n    cursor: pointer;\n}\n\n.cursor-auto {\n    cursor: auto;\n}\n\n.callstack-info {\n    text-align: center;\n    font-style: italic;\n    font-size: 90%;\n    padding: 6px;\n    color: #888;\n    pointer-events: none;\n}\n\n.callstack-info.status {\n    border-top: 1px solid rgb(189, 189, 189);\n    background-color: rgb(255, 255, 194);\n}\n\n.watch-expression-delete-button {\n    width: 10px;\n    height: 10px;\n    background-image: url(Images/deleteIcon.png);\n    background-position: 0 0;\n    background-color: transparent;\n    background-repeat: no-repeat;\n    border: 0 none transparent;\n    position: absolute;\n    right: 3px;\n    display: none;\n}\n\n.watch-expression-header:hover .watch-expression-delete-button {\n    display: inline;\n}\n\n.watch-expressions {\n    overflow-x: hidden;\n    padding: 3px 6px 6px 0px;\n}\n\n.watch-expressions .dimmed {\n    opacity: 0.6;\n}\n\n.watch-expression-title {\n    white-space: nowrap;\n    text-overflow: ellipsis;\n    overflow: hidden;\n    line-height: 12px;\n    margin-left: 11px;\n}\n\n.watch-expression {\n    position: relative;\n    padding: 1px 0px 1px 6px;\n}\n\n.watch-expressions .name {\n    color: rgb(136, 19, 145);\n    flex: none;\n    white-space: nowrap;\n    text-overflow: ellipsis ;\n    overflow: hidden;\n}\n\n.watch-expressions-separator {\n    flex: none;\n}\n\n.watch-expressions .value {\n    white-space: nowrap;\n    display: inline;\n}\n\n.watch-expression .text-prompt {\n    text-overflow: clip;\n    overflow: hidden;\n    white-space: nowrap;\n    padding-left: 4px;\n    -webkit-user-select: text;\n}\n\n.watch-expression-text-prompt-proxy {\n    margin-left: 12px;\n}\n\n.watch-expression-header:hover {\n    background-color: #F0F0F0;\n    padding-right: 14px;\n}\n\n.sidebar-pane-stack .watch-expressions {\n    margin-top: 0px;\n}\n\n.scope-chain-sidebar-pane-section-subtitle {\n    float: right;\n    margin-left: 5px;\n    max-width: 55%;\n    text-overflow: ellipsis;\n    overflow: hidden;\n}\n\n.scope-chain-sidebar-pane-section-title {\n    font-weight: normal;\n    word-wrap: break-word;\n    white-space: normal;\n}\n\n.scope-chain-sidebar-pane-section {\n    padding: 2px 4px 2px 4px;\n}\n\n.scope-chain-sidebar-pane-section-subtitle {\n    float: right;\n    margin-left: 5px;\n    max-width: 55%;\n    text-overflow: ellipsis;\n    overflow: hidden;\n}\n\n.scope-chain-sidebar-pane-section-title {\n    font-weight: normal;\n    word-wrap: break-word;\n    white-space: normal;\n}\n\n.scope-chain-sidebar-pane-section {\n    padding: 2px 4px 2px 4px;\n}\n/*# sourceURL=sources/sourcesPanel.css */";Runtime.cachedResources["sources/sourcesSearch.css"]="/*\n * Copyright 2014 The Chromium Authors. All rights reserved.\n * Use of this source code is governed by a BSD-style license that can be\n * found in the LICENSE file.\n */\n\n.search-drawer-header {\n    flex: none;\n    padding: 4px;\n    display: flex;\n}\n\n.search-drawer-header input[type=\"text\"].search-config-search {\n    -webkit-appearance: none;\n    padding: 0 3px;\n    margin: 0;\n    border: 1px solid rgb(163, 163, 163);\n    height: 20px;\n    border-radius: 2px;\n    color: #303030;\n}\n\n.search-drawer-header input[type=\"search\"].search-config-search:focus {\n    border: 1px solid rgb(190, 190, 190);\n    outline: none;\n}\n\n:host-context(.platform-mac) .search-drawer-header input[type=\"search\"].search-config-search {\n    top: 1px;\n}\n\n.search-drawer-header label.search-config-label {\n    margin: auto 0;\n    margin-left: 8px;\n    color: #303030;\n    display: flex;\n}\n\n.search-toolbar-summary {\n    background-color: #eee;\n    border-top: 1px solid #ccc;\n    padding-left: 5px;\n    flex: 0 0 19px;\n    display: flex;\n    padding-right: 5px;\n}\n\n.search-toolbar-summary .search-message {\n    padding-top: 2px;\n}\n\n.search-toolbar-summary .search-message-spacer {\n    flex-grow: 1;\n}\n\n#search-results-pane-file-based li {\n    list-style: none;\n}\n\n#search-results-pane-file-based ol {\n    -webkit-padding-start: 0;\n    margin-top: 0;\n}\n\n#search-results-pane-file-based ol.children {\n    display: none;\n}\n\n#search-results-pane-file-based ol.children.expanded {\n    display: block;\n}\n\n#search-results-pane-file-based li.parent::before {\n    -webkit-user-select: none;\n    background-image: url(Images/toolbarButtonGlyphs.png);\n    background-size: 352px 168px;\n    opacity: 0.5;\n    width: 12px;\n    content: \"a\";\n    color: transparent;\n    margin-left: -5px;\n    padding-right: 4px;\n    display: inline-block;\n    box-sizing: border-box;\n}\n\n@media (-webkit-min-device-pixel-ratio: 1.5) {\n#search-results-pane-file-based li.parent::before {\n    background-image: url(Images/toolbarButtonGlyphs_2x.png);\n}\n} /* media */\n\n#search-results-pane-file-based li.parent::before {\n    background-position: -4px -96px;\n}\n\n#search-results-pane-file-based li.parent.expanded::before {\n    background-position: -20px -96px;\n}\n\n#search-results-pane-file-based .search-result {\n    font-size: 11px;\n    padding: 2px 0 2px 10px;\n    word-wrap: normal;\n    white-space: pre;\n    cursor: pointer;\n}\n\n#search-results-pane-file-based .search-result:hover {\n    background-color: rgba(121, 121, 121, 0.1);\n}\n\n#search-results-pane-file-based .search-result .search-result-file-name {\n    font-weight: bold;\n    color: #222;\n}\n\n#search-results-pane-file-based .search-result .search-result-matches-count {\n    margin-left: 5px;\n    color: #222;\n}\n\n#search-results-pane-file-based .show-more-matches {\n    padding: 4px 0;\n    color: #222;\n    cursor: pointer;\n    font-size: 11px;\n    margin-left: 20px;\n}\n\n#search-results-pane-file-based .show-more-matches:hover {\n    text-decoration: underline;\n}\n\n#search-results-pane-file-based .search-match {\n    word-wrap: normal;\n    white-space: pre;\n}\n\n#search-results-pane-file-based .search-match .search-match-line-number {\n    color: rgb(128, 128, 128);\n    text-align: right;\n    vertical-align: top;\n    word-break: normal;\n    padding-right: 4px;\n    padding-left: 6px;\n    margin-right: 5px;\n    border-right: 1px solid #BBB;\n}\n\n#search-results-pane-file-based .search-match:not(:hover) .search-match-line-number {\n    background-color: #F0F0F0;\n}\n\n#search-results-pane-file-based .search-match:hover {\n    background-color: rgba(56, 121, 217, 0.1);\n}\n\n#search-results-pane-file-based .search-match .highlighted-match {\n    background-color: #F1EA00;\n}\n\n#search-results-pane-file-based a {\n    text-decoration: none;\n    display: block;\n}\n\n#search-results-pane-file-based .search-match .search-match-content {\n    color: #000;\n}\n\n.search-view .search-results {\n    overflow-y: auto;\n    display: flex;\n    flex: auto;\n}\n\n.search-results .empty-view {\n    pointer-events: none;\n}\n\n.empty-view {\n    font-size: 24px;\n    color: rgb(75%, 75%, 75%);\n    font-weight: bold;\n    padding: 10px;\n    display: flex;\n    align-items: center;\n    justify-content: center;\n}\n\n/*# sourceURL=sources/sourcesSearch.css */";Runtime.cachedResources["sources/sourcesView.css"]="/*\n * Copyright (C) 2013 Google Inc. All rights reserved.\n *\n * Redistribution and use in source and binary forms, with or without\n * modification, are permitted provided that the following conditions are\n * met:\n *\n *     * Redistributions of source code must retain the above copyright\n * notice, this list of conditions and the following disclaimer.\n *     * Redistributions in binary form must reproduce the above\n * copyright notice, this list of conditions and the following disclaimer\n * in the documentation and/or other materials provided with the\n * distribution.\n *     * Neither the name of Google Inc. nor the names of its\n * contributors may be used to endorse or promote products derived from\n * this software without specific prior written permission.\n *\n * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS\n * \"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT\n * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR\n * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT\n * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,\n * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT\n * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,\n * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY\n * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT\n * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE\n * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n */\n\n#sources-panel-sources-view {\n    flex: auto;\n    position: relative;\n}\n\n#sources-panel-sources-view .sources-toolbar {\n    display: flex;\n    flex: 0 0 27px;\n    background-color: #f3f3f3;\n    border-top: 1px solid #dadada;\n    overflow: hidden;\n}\n\n.sources-toolbar .toolbar {\n    cursor: default;\n}\n\n.source-frame-debugger-script {\n    background-color: rgba(255, 255, 194, 0.5);\n}\n\n.source-frame-unsaved-committed-changes {\n    background-color: rgba(255, 225, 205, 0.40);\n}\n\n.source-frame-infobar {\n    flex: none;\n    display: flex;\n    -webkit-user-select: text;\n}\n\n.source-frame-infobar-animation {\n    -webkit-animation: source-frame-infobar-reveal 0.1s;\n}\n@-webkit-keyframes source-frame-infobar-reveal {\n    from { margin-top: -24px; }\n    to { margin-top: 0; }\n}\n\n.source-frame-infobar-main-row {\n    display: flex;\n    flex-direction: row;\n    flex: auto;\n    overflow: hidden;\n}\n\n.source-frame-infobar-details-container {\n    display: flex;\n    flex-direction: column;\n}\n\n.source-frame-infobar-details-row {\n    display: flex;\n    flex: none;\n    margin-top: 2px;\n}\n\n.source-frame-infobar-row-message {\n    overflow: hidden;\n}\n\n.source-frame-infobar-toggle {\n    flex: 1 0 auto;\n    padding: 0 4px;\n    -webkit-user-select: none;\n}\n\n.source-frame-infobar-details-url {\n    flex: 0 1000 auto;\n}\n\n@-webkit-keyframes source-frame-value-update-highlight-animation {\n    from {\n        background-color: inherit;\n        color: inherit;\n    }\n    10% {\n        background-color: rgb(158, 54, 153);\n        color: white;\n    }\n    to {\n        background-color: inherit;\n        color: inherit;\n    }\n}\n\n.source-frame-value-update-highlight {\n    -webkit-animation: source-frame-value-update-highlight-animation 0.8s 1 cubic-bezier(0, 0, 0.2, 1);\n    border-radius: 2px;\n}\n\n/*# sourceURL=sources/sourcesView.css */";