WebInspector.CookiesTable=function(expandable,refreshCallback,selectedCallback)
{WebInspector.DataGridContainerWidget.call(this);var readOnly=expandable;this._refreshCallback=refreshCallback;var columns=[{id:"name",title:WebInspector.UIString("Name"),sortable:true,disclosure:expandable,sort:WebInspector.DataGrid.Order.Ascending,longText:true,weight:24},{id:"value",title:WebInspector.UIString("Value"),sortable:true,longText:true,weight:34},{id:"domain",title:WebInspector.UIString("Domain"),sortable:true,weight:7},{id:"path",title:WebInspector.UIString("Path"),sortable:true,weight:7},{id:"expires",title:WebInspector.UIString("Expires / Max-Age"),sortable:true,weight:7},{id:"size",title:WebInspector.UIString("Size"),sortable:true,align:WebInspector.DataGrid.Align.Right,weight:7},{id:"httpOnly",title:WebInspector.UIString("HTTP"),sortable:true,align:WebInspector.DataGrid.Align.Center,weight:7},{id:"secure",title:WebInspector.UIString("Secure"),sortable:true,align:WebInspector.DataGrid.Align.Center,weight:7},{id:"firstPartyOnly",title:WebInspector.UIString("First-Party"),sortable:true,align:WebInspector.DataGrid.Align.Center,weight:7}];if(readOnly)
this._dataGrid=new WebInspector.DataGrid(columns);else
this._dataGrid=new WebInspector.DataGrid(columns,undefined,this._onDeleteCookie.bind(this),refreshCallback,this._onContextMenu.bind(this));this._dataGrid.setName("cookiesTable");this._dataGrid.addEventListener(WebInspector.DataGrid.Events.SortingChanged,this._rebuildTable,this);if(selectedCallback)
this._dataGrid.addEventListener(WebInspector.DataGrid.Events.SelectedNode,selectedCallback,this);this._nextSelectedCookie=(null);this.appendDataGrid(this._dataGrid);this._data=[];}
WebInspector.CookiesTable.prototype={_clearAndRefresh:function(domain)
{this.clear(domain);this._refresh();},_onContextMenu:function(contextMenu,node)
{if(node===this._dataGrid.creationNode)
return;var cookie=node.cookie;var domain=cookie.domain();if(domain)
contextMenu.appendItem(WebInspector.UIString.capitalize("Clear ^all from \"%s\"",domain),this._clearAndRefresh.bind(this,domain));contextMenu.appendItem(WebInspector.UIString.capitalize("Clear ^all"),this._clearAndRefresh.bind(this,null));},setCookies:function(cookies)
{this.setCookieFolders([{cookies:cookies}]);},setCookieFolders:function(cookieFolders)
{this._data=cookieFolders;this._rebuildTable();},selectedCookie:function()
{var node=this._dataGrid.selectedNode;return node?node.cookie:null;},clear:function(domain)
{for(var i=0,length=this._data.length;i<length;++i){var cookies=this._data[i].cookies;for(var j=0,cookieCount=cookies.length;j<cookieCount;++j){if(!domain||cookies[j].domain()===domain)
cookies[j].remove();}}},_rebuildTable:function()
{var selectedCookie=this._nextSelectedCookie||this.selectedCookie();this._nextSelectedCookie=null;this._dataGrid.rootNode().removeChildren();for(var i=0;i<this._data.length;++i){var item=this._data[i];if(item.folderName){var groupData={name:item.folderName,value:"",domain:"",path:"",expires:"",size:this._totalSize(item.cookies),httpOnly:"",secure:"",firstPartyOnly:""};var groupNode=new WebInspector.DataGridNode(groupData);groupNode.selectable=true;this._dataGrid.rootNode().appendChild(groupNode);groupNode.element().classList.add("row-group");this._populateNode(groupNode,item.cookies,selectedCookie);groupNode.expand();}else
this._populateNode(this._dataGrid.rootNode(),item.cookies,selectedCookie);}},_populateNode:function(parentNode,cookies,selectedCookie)
{parentNode.removeChildren();if(!cookies)
return;this._sortCookies(cookies);for(var i=0;i<cookies.length;++i){var cookie=cookies[i];var cookieNode=this._createGridNode(cookie);parentNode.appendChild(cookieNode);if(selectedCookie&&selectedCookie.name()===cookie.name()&&selectedCookie.domain()===cookie.domain()&&selectedCookie.path()===cookie.path())
cookieNode.select();}},_totalSize:function(cookies)
{var totalSize=0;for(var i=0;cookies&&i<cookies.length;++i)
totalSize+=cookies[i].size();return totalSize;},_sortCookies:function(cookies)
{var sortDirection=this._dataGrid.isSortOrderAscending()?1:-1;function compareTo(getter,cookie1,cookie2)
{return sortDirection*(getter.apply(cookie1)+"").compareTo(getter.apply(cookie2)+"");}
function numberCompare(getter,cookie1,cookie2)
{return sortDirection*(getter.apply(cookie1)-getter.apply(cookie2));}
function expiresCompare(cookie1,cookie2)
{if(cookie1.session()!==cookie2.session())
return sortDirection*(cookie1.session()?1:-1);if(cookie1.session())
return 0;if(cookie1.maxAge()&&cookie2.maxAge())
return sortDirection*(cookie1.maxAge()-cookie2.maxAge());if(cookie1.expires()&&cookie2.expires())
return sortDirection*(cookie1.expires()-cookie2.expires());return sortDirection*(cookie1.expires()?1:-1);}
var comparator;switch(this._dataGrid.sortColumnIdentifier()){case"name":comparator=compareTo.bind(null,WebInspector.Cookie.prototype.name);break;case"value":comparator=compareTo.bind(null,WebInspector.Cookie.prototype.value);break;case"domain":comparator=compareTo.bind(null,WebInspector.Cookie.prototype.domain);break;case"path":comparator=compareTo.bind(null,WebInspector.Cookie.prototype.path);break;case"expires":comparator=expiresCompare;break;case"size":comparator=numberCompare.bind(null,WebInspector.Cookie.prototype.size);break;case"httpOnly":comparator=compareTo.bind(null,WebInspector.Cookie.prototype.httpOnly);break;case"secure":comparator=compareTo.bind(null,WebInspector.Cookie.prototype.secure);break;case"firstPartyOnly":comparator=compareTo.bind(null,WebInspector.Cookie.prototype.firstPartyOnly);break;default:compareTo.bind(null,WebInspector.Cookie.prototype.name);}
cookies.sort(comparator);},_createGridNode:function(cookie)
{var data={};data.name=cookie.name();data.value=cookie.value();if(cookie.type()===WebInspector.Cookie.Type.Request){data.domain=WebInspector.UIString("N/A");data.path=WebInspector.UIString("N/A");data.expires=WebInspector.UIString("N/A");}else{data.domain=cookie.domain()||"";data.path=cookie.path()||"";if(cookie.maxAge())
data.expires=Number.secondsToString(parseInt(cookie.maxAge(),10));else if(cookie.expires())
data.expires=new Date(cookie.expires()).toISOString();else
data.expires=WebInspector.UIString("Session");}
data.size=cookie.size();const checkmark="\u2713";data.httpOnly=(cookie.httpOnly()?checkmark:"");data.secure=(cookie.secure()?checkmark:"");data.firstPartyOnly=(cookie.firstPartyOnly()?checkmark:"");var node=new WebInspector.DataGridNode(data);node.cookie=cookie;node.selectable=true;return node;},_onDeleteCookie:function(node)
{var cookie=node.cookie;var neighbour=node.traverseNextNode()||node.traversePreviousNode();if(neighbour)
this._nextSelectedCookie=neighbour.cookie;cookie.remove();this._refresh();},_refresh:function()
{if(this._refreshCallback)
this._refreshCallback();},__proto__:WebInspector.DataGridContainerWidget.prototype};WebInspector.FilmStripModel=function(tracingModel,zeroTime)
{this._tracingModel=tracingModel;this._zeroTime=zeroTime||tracingModel.minimumRecordTime();this._frames=[];var browserProcess=tracingModel.processByName("Browser");if(!browserProcess)
return;var mainThread=browserProcess.threadByName("CrBrowserMain");if(!mainThread)
return;var events=mainThread.events();for(var i=0;i<events.length;++i){var event=events[i];if(event.startTime<this._zeroTime)
continue;if(!event.hasCategory(WebInspector.FilmStripModel._category))
continue;if(event.name===WebInspector.FilmStripModel.TraceEvents.CaptureFrame){var data=event.args["data"];if(data)
this._frames.push(WebInspector.FilmStripModel.Frame._fromEvent(this,event,this._frames.length));}else if(event.name===WebInspector.FilmStripModel.TraceEvents.Screenshot){this._frames.push(WebInspector.FilmStripModel.Frame._fromSnapshot(this,(event),this._frames.length));}}}
WebInspector.FilmStripModel._category="disabled-by-default-devtools.screenshot";WebInspector.FilmStripModel.TraceEvents={CaptureFrame:"CaptureFrame",Screenshot:"Screenshot"}
WebInspector.FilmStripModel.prototype={frames:function()
{return this._frames;},zeroTime:function()
{return this._zeroTime;},frameByTimestamp:function(timestamp)
{function comparator(timestamp,frame)
{return timestamp-frame.timestamp;}
var index=this._frames.lowerBound(timestamp,comparator);return index<this._frames.length?this._frames[index]:null;}}
WebInspector.FilmStripModel.Frame=function(model,timestamp,index)
{this._model=model;this.timestamp=timestamp;this.index=index;this._imageData=null;this._snapshot=null;}
WebInspector.FilmStripModel.Frame._fromEvent=function(model,event,index)
{var frame=new WebInspector.FilmStripModel.Frame(model,event.startTime,index);frame._imageData=event.args["data"];return frame;}
WebInspector.FilmStripModel.Frame._fromSnapshot=function(model,snapshot,index)
{var frame=new WebInspector.FilmStripModel.Frame(model,snapshot.startTime,index);frame._snapshot=snapshot;return frame;}
WebInspector.FilmStripModel.Frame.prototype={model:function()
{return this._model;},imageDataPromise:function()
{if(this._imageData||!this._snapshot)
return Promise.resolve(this._imageData);return(this._snapshot.objectPromise());}};WebInspector.FilmStripView=function()
{WebInspector.HBox.call(this,true);this.registerRequiredCSS("components_lazy/filmStripView.css");this.contentElement.classList.add("film-strip-view");this._statusLabel=this.contentElement.createChild("div","label");this.reset();this.setMode(WebInspector.FilmStripView.Modes.TimeBased);}
WebInspector.FilmStripView.Events={FrameSelected:"FrameSelected",FrameEnter:"FrameEnter",FrameExit:"FrameExit",}
WebInspector.FilmStripView.Modes={TimeBased:"TimeBased",FrameBased:"FrameBased"}
WebInspector.FilmStripView.prototype={setMode:function(mode)
{this._mode=mode;this.contentElement.classList.toggle("time-based",mode===WebInspector.FilmStripView.Modes.TimeBased);this.update();},setModel:function(filmStripModel,zeroTime,spanTime)
{this._model=filmStripModel;this._zeroTime=zeroTime;this._spanTime=spanTime;var frames=filmStripModel.frames();if(!frames.length){this.reset();return;}
this.update();},createFrameElement:function(frame)
{var time=frame.timestamp;var element=createElementWithClass("div","frame");element.title=WebInspector.UIString("Doubleclick to zoom image. Click to view preceding requests.");element.createChild("div","time").textContent=Number.millisToString(time-this._zeroTime);var imageElement=element.createChild("div","thumbnail").createChild("img");element.addEventListener("mousedown",this._onMouseEvent.bind(this,WebInspector.FilmStripView.Events.FrameSelected,time),false);element.addEventListener("mouseenter",this._onMouseEvent.bind(this,WebInspector.FilmStripView.Events.FrameEnter,time),false);element.addEventListener("mouseout",this._onMouseEvent.bind(this,WebInspector.FilmStripView.Events.FrameExit,time),false);element.addEventListener("dblclick",this._onDoubleClick.bind(this,frame),false);return frame.imageDataPromise().then(WebInspector.FilmStripView._setImageData.bind(null,imageElement)).then(returnElement);function returnElement()
{return element;}},frameByTime:function(time)
{function comparator(time,frame)
{return time-frame.timestamp;}
var frames=this._model.frames();var index=Math.max(frames.upperBound(time,comparator)-1,0);return frames[index];},update:function()
{if(!this._model)
return;var frames=this._model.frames();if(!frames.length)
return;if(this._mode===WebInspector.FilmStripView.Modes.FrameBased){Promise.all(frames.map(this.createFrameElement.bind(this))).then(appendElements.bind(this));return;}
var width=this.contentElement.clientWidth;var scale=this._spanTime/width;this.createFrameElement(frames[0]).then(continueWhenFrameImageLoaded.bind(this));function continueWhenFrameImageLoaded(element0)
{var frameWidth=Math.ceil(WebInspector.measurePreferredSize(element0,this.contentElement).width);if(!frameWidth)
return;var promises=[];for(var pos=frameWidth;pos<width;pos+=frameWidth){var time=pos*scale+this._zeroTime;promises.push(this.createFrameElement(this.frameByTime(time)).then(fixWidth));}
Promise.all(promises).then(appendElements.bind(this));function fixWidth(element)
{element.style.width=frameWidth+"px";return element;}}
function appendElements(elements)
{this.contentElement.removeChildren();for(var i=0;i<elements.length;++i)
this.contentElement.appendChild(elements[i]);}},onResize:function()
{if(this._mode===WebInspector.FilmStripView.Modes.FrameBased)
return;this.update();},_onMouseEvent:function(eventName,timestamp)
{this.dispatchEventToListeners(eventName,timestamp);},_onDoubleClick:function(filmStripFrame)
{new WebInspector.FilmStripView.Dialog(filmStripFrame,this._zeroTime);},reset:function()
{this._zeroTime=0;this.contentElement.removeChildren();this.contentElement.appendChild(this._statusLabel);},setStatusText:function(text)
{this._statusLabel.textContent=text;},__proto__:WebInspector.HBox.prototype}
WebInspector.FilmStripView._setImageData=function(imageElement,data)
{if(data)
imageElement.src="data:image/jpg;base64,"+data;}
WebInspector.FilmStripView.Dialog=function(filmStripFrame,zeroTime)
{WebInspector.VBox.call(this,true);this.registerRequiredCSS("components_lazy/filmStripDialog.css");this.contentElement.classList.add("filmstrip-dialog");this.contentElement.tabIndex=0;this._frames=filmStripFrame.model().frames();this._index=filmStripFrame.index;this._zeroTime=zeroTime||filmStripFrame.model().zeroTime();this._imageElement=this.contentElement.createChild("img");var footerElement=this.contentElement.createChild("div","filmstrip-dialog-footer");footerElement.createChild("div","flex-auto");var prevButton=createTextButton("\u25C0",this._onPrevFrame.bind(this),undefined,WebInspector.UIString("Previous frame"));footerElement.appendChild(prevButton);this._timeLabel=footerElement.createChild("div","filmstrip-dialog-label");var nextButton=createTextButton("\u25B6",this._onNextFrame.bind(this),undefined,WebInspector.UIString("Next frame"));footerElement.appendChild(nextButton);footerElement.createChild("div","flex-auto");this.contentElement.addEventListener("keydown",this._keyDown.bind(this),false);this.setDefaultFocusedElement(this.contentElement);this._render();}
WebInspector.FilmStripView.Dialog.prototype={_resize:function()
{if(!this._dialog){this._dialog=new WebInspector.Dialog();this.show(this._dialog.element);this._dialog.setWrapsContent(true);this._dialog.show();}
this._dialog.contentResized();},_keyDown:function(event)
{switch(event.keyIdentifier){case"Left":if(WebInspector.isMac()&&event.metaKey)
this._onFirstFrame();else
this._onPrevFrame();break;case"Right":if(WebInspector.isMac()&&event.metaKey)
this._onLastFrame();else
this._onNextFrame();break;case"Home":this._onFirstFrame();break;case"End":this._onLastFrame();break;}},_onPrevFrame:function()
{if(this._index>0)
--this._index;this._render();},_onNextFrame:function()
{if(this._index<this._frames.length-1)
++this._index;this._render();},_onFirstFrame:function()
{this._index=0;this._render();},_onLastFrame:function()
{this._index=this._frames.length-1;this._render();},_render:function()
{var frame=this._frames[this._index];this._timeLabel.textContent=Number.millisToString(frame.timestamp-this._zeroTime);return frame.imageDataPromise().then(WebInspector.FilmStripView._setImageData.bind(null,this._imageElement)).then(this._resize.bind(this));},__proto__:WebInspector.VBox.prototype};Runtime.cachedResources["components_lazy/filmStripDialog.css"]="/*\n * Copyright (c) 2015 The Chromium Authors. All rights reserved.\n * Use of this source code is governed by a BSD-style license that can be\n * found in the LICENSE file.\n */\n\n:host {\n    flex: none !important;\n}\n\n.filmstrip-dialog {\n    margin: 12px;\n}\n\n.filmstrip-dialog > img {\n    border: 1px solid #ddd;\n}\n\n.filmstrip-dialog-footer {\n    display: flex;\n    align-items: center;\n    margin-top: 10px;\n}\n\n.filmstrip-dialog-label {\n    margin: 8px 8px;\n}\n\n/*# sourceURL=components_lazy/filmStripDialog.css */";Runtime.cachedResources["components_lazy/filmStripView.css"]="/*\n * Copyright (c) 2015 The Chromium Authors. All rights reserved.\n * Use of this source code is governed by a BSD-style license that can be\n * found in the LICENSE file.\n */\n\n.film-strip-view {\n    overflow-x: auto;\n    overflow-y: hidden;\n    align-content: flex-start;\n    min-height: 81px;\n}\n\n.film-strip-view.time-based .frame .time {\n    display: none;\n}\n\n.film-strip-view .label {\n    margin: auto;\n    font-size: 18px;\n    color: #999;\n}\n\n.film-strip-view .frame {\n    display: flex;\n    flex-direction: column;\n    align-items: center;\n    padding: 4px;\n    flex: none;\n    cursor: pointer;\n}\n\n.film-strip-view .frame-limit-reached {\n    font-size: 24px;\n    color: #888;\n    justify-content: center;\n    display: inline-flex;\n    flex-direction: column;\n    flex: none;\n}\n\n.film-strip-view .frame .thumbnail {\n    min-width: 24px;\n    display: flex;\n    flex-direction: row;\n    align-items: center;\n    pointer-events: none;\n    margin: 4px 0 2px;\n    border: 2px solid transparent;\n}\n\n.film-strip-view .frame:hover .thumbnail {\n    border-color: #FBCA46;\n}\n\n.film-strip-view .frame .thumbnail img {\n    height: auto;\n    width: auto;\n    max-width: 80px;\n    max-height: 50px;\n    pointer-events: none;\n    box-shadow: 0 0 3px #bbb;\n    flex: 0 0 auto;\n}\n\n.film-strip-view .frame:hover .thumbnail img {\n    box-shadow: none;\n}\n\n.film-strip-view .frame .time {\n    font-size: 10px;\n    margin-top: 2px;\n}\n\n/*# sourceURL=components_lazy/filmStripView.css */";