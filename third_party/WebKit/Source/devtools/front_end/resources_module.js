WebInspector.ApplicationCacheItemsView=function(model,frameId)
{WebInspector.DataGridContainerWidget.call(this);this._model=model;this.element.classList.add("storage-view","table");this._deleteButton=new WebInspector.ToolbarButton(WebInspector.UIString("Delete"),"delete-toolbar-item");this._deleteButton.setVisible(false);this._deleteButton.addEventListener("click",this._deleteButtonClicked,this);this._connectivityIcon=createElement("label","dt-icon-label");this._connectivityIcon.style.margin="0 2px 0 5px";this._statusIcon=createElement("label","dt-icon-label");this._statusIcon.style.margin="0 2px 0 5px";this._frameId=frameId;this._emptyWidget=new WebInspector.EmptyWidget(WebInspector.UIString("No Application Cache information available."));this._emptyWidget.show(this.element);this._markDirty();var status=this._model.frameManifestStatus(frameId);this.updateStatus(status);this.updateNetworkState(this._model.onLine);this._deleteButton.element.style.display="none";}
WebInspector.ApplicationCacheItemsView.prototype={toolbarItems:function()
{return[this._deleteButton,new WebInspector.ToolbarItem(this._connectivityIcon),new WebInspector.ToolbarSeparator(),new WebInspector.ToolbarItem(this._statusIcon)];},wasShown:function()
{this._maybeUpdate();},willHide:function()
{this._deleteButton.setVisible(false);},_maybeUpdate:function()
{if(!this.isShowing()||!this._viewDirty)
return;this._update();this._viewDirty=false;},_markDirty:function()
{this._viewDirty=true;},updateStatus:function(status)
{var oldStatus=this._status;this._status=status;var statusInformation={};statusInformation[applicationCache.UNCACHED]={type:"red-ball",text:"UNCACHED"};statusInformation[applicationCache.IDLE]={type:"green-ball",text:"IDLE"};statusInformation[applicationCache.CHECKING]={type:"orange-ball",text:"CHECKING"};statusInformation[applicationCache.DOWNLOADING]={type:"orange-ball",text:"DOWNLOADING"};statusInformation[applicationCache.UPDATEREADY]={type:"green-ball",text:"UPDATEREADY"};statusInformation[applicationCache.OBSOLETE]={type:"red-ball",text:"OBSOLETE"};var info=statusInformation[status]||statusInformation[applicationCache.UNCACHED];this._statusIcon.type=info.type;this._statusIcon.textContent=info.text;if(this.isShowing()&&this._status===applicationCache.IDLE&&(oldStatus===applicationCache.UPDATEREADY||!this._resources))
this._markDirty();this._maybeUpdate();},updateNetworkState:function(isNowOnline)
{if(isNowOnline){this._connectivityIcon.type="green-ball";this._connectivityIcon.textContent=WebInspector.UIString("Online");}else{this._connectivityIcon.type="red-ball";this._connectivityIcon.textContent=WebInspector.UIString("Offline");}},_update:function()
{this._model.requestApplicationCache(this._frameId,this._updateCallback.bind(this));},_updateCallback:function(applicationCache)
{if(!applicationCache||!applicationCache.manifestURL){delete this._manifest;delete this._creationTime;delete this._updateTime;delete this._size;delete this._resources;this._emptyWidget.show(this.element);this._deleteButton.setVisible(false);if(this._dataGrid)
this._dataGrid.element.classList.add("hidden");return;}
this._manifest=applicationCache.manifestURL;this._creationTime=applicationCache.creationTime;this._updateTime=applicationCache.updateTime;this._size=applicationCache.size;this._resources=applicationCache.resources;if(!this._dataGrid)
this._createDataGrid();this._populateDataGrid();this._dataGrid.autoSizeColumns(20,80);this._dataGrid.element.classList.remove("hidden");this._emptyWidget.detach();this._deleteButton.setVisible(true);},_createDataGrid:function()
{var columns=[{title:WebInspector.UIString("Resource"),sort:WebInspector.DataGrid.Order.Ascending,sortable:true},{title:WebInspector.UIString("Type"),sortable:true},{title:WebInspector.UIString("Size"),align:WebInspector.DataGrid.Align.Right,sortable:true}];this._dataGrid=new WebInspector.DataGrid(columns);this.appendDataGrid(this._dataGrid);this._dataGrid.addEventListener(WebInspector.DataGrid.Events.SortingChanged,this._populateDataGrid,this);},_populateDataGrid:function()
{var selectedResource=this._dataGrid.selectedNode?this._dataGrid.selectedNode.resource:null;var sortDirection=this._dataGrid.isSortOrderAscending()?1:-1;function numberCompare(field,resource1,resource2)
{return sortDirection*(resource1[field]-resource2[field]);}
function localeCompare(field,resource1,resource2)
{return sortDirection*(resource1[field]+"").localeCompare(resource2[field]+"");}
var comparator;switch(parseInt(this._dataGrid.sortColumnIdentifier(),10)){case 0:comparator=localeCompare.bind(null,"name");break;case 1:comparator=localeCompare.bind(null,"type");break;case 2:comparator=numberCompare.bind(null,"size");break;default:localeCompare.bind(null,"resource");}
this._resources.sort(comparator);this._dataGrid.rootNode().removeChildren();var nodeToSelect;for(var i=0;i<this._resources.length;++i){var data={};var resource=this._resources[i];data[0]=resource.url;data[1]=resource.type;data[2]=Number.bytesToString(resource.size);var node=new WebInspector.DataGridNode(data);node.resource=resource;node.selectable=true;this._dataGrid.rootNode().appendChild(node);if(resource===selectedResource){nodeToSelect=node;nodeToSelect.selected=true;}}
if(!nodeToSelect&&this._dataGrid.rootNode().children.length)
this._dataGrid.rootNode().children[0].selected=true;},_deleteButtonClicked:function(event)
{if(!this._dataGrid||!this._dataGrid.selectedNode)
return;this._deleteCallback(this._dataGrid.selectedNode);},_deleteCallback:function(node)
{},__proto__:WebInspector.DataGridContainerWidget.prototype};WebInspector.CookieItemsView=function(treeElement,cookieDomain)
{WebInspector.VBox.call(this);this.element.classList.add("storage-view");this._deleteButton=new WebInspector.ToolbarButton(WebInspector.UIString("Delete"),"delete-toolbar-item");this._deleteButton.setVisible(false);this._deleteButton.addEventListener("click",this._deleteButtonClicked,this);this._clearButton=new WebInspector.ToolbarButton(WebInspector.UIString("Clear"),"clear-toolbar-item");this._clearButton.setVisible(false);this._clearButton.addEventListener("click",this._clearButtonClicked,this);this._refreshButton=new WebInspector.ToolbarButton(WebInspector.UIString("Refresh"),"refresh-toolbar-item");this._refreshButton.addEventListener("click",this._refreshButtonClicked,this);this._treeElement=treeElement;this._cookieDomain=cookieDomain;this._emptyWidget=new WebInspector.EmptyWidget(cookieDomain?WebInspector.UIString("This site has no cookies."):WebInspector.UIString("By default cookies are disabled for local files.\nYou could override this by starting the browser with --enable-file-cookies command line flag."));this._emptyWidget.show(this.element);this.element.addEventListener("contextmenu",this._contextMenu.bind(this),true);}
WebInspector.CookieItemsView.prototype={toolbarItems:function()
{return[this._refreshButton,this._clearButton,this._deleteButton];},wasShown:function()
{this._update();},willHide:function()
{this._deleteButton.setVisible(false);},_update:function()
{WebInspector.Cookies.getCookiesAsync(this._updateWithCookies.bind(this));},_updateWithCookies:function(allCookies)
{this._cookies=this._filterCookiesForDomain(allCookies);if(!this._cookies.length){this._emptyWidget.show(this.element);this._clearButton.setVisible(false);this._deleteButton.setVisible(false);if(this._cookiesTable)
this._cookiesTable.detach();return;}
if(!this._cookiesTable)
this._cookiesTable=new WebInspector.CookiesTable(false,this._update.bind(this),this._showDeleteButton.bind(this));this._cookiesTable.setCookies(this._cookies);this._emptyWidget.detach();this._cookiesTable.show(this.element);this._treeElement.subtitle=String.sprintf(WebInspector.UIString("%d cookies (%s)"),this._cookies.length,Number.bytesToString(this._totalSize));this._clearButton.setVisible(true);this._deleteButton.setVisible(!!this._cookiesTable.selectedCookie());},_filterCookiesForDomain:function(allCookies)
{var cookies=[];var resourceURLsForDocumentURL=[];this._totalSize=0;function populateResourcesForDocuments(resource)
{var url=resource.documentURL.asParsedURL();if(url&&url.host==this._cookieDomain)
resourceURLsForDocumentURL.push(resource.url);}
WebInspector.forAllResources(populateResourcesForDocuments.bind(this));for(var i=0;i<allCookies.length;++i){var pushed=false;var size=allCookies[i].size();for(var j=0;j<resourceURLsForDocumentURL.length;++j){var resourceURL=resourceURLsForDocumentURL[j];if(WebInspector.Cookies.cookieMatchesResourceURL(allCookies[i],resourceURL)){this._totalSize+=size;if(!pushed){pushed=true;cookies.push(allCookies[i]);}}}}
return cookies;},clear:function()
{this._cookiesTable.clear();this._update();},_clearButtonClicked:function()
{this.clear();},_showDeleteButton:function()
{this._deleteButton.setVisible(true);},_deleteButtonClicked:function()
{var selectedCookie=this._cookiesTable.selectedCookie();if(selectedCookie){selectedCookie.remove();this._update();}},_refreshButtonClicked:function(event)
{this._update();},_contextMenu:function(event)
{if(!this._cookies.length){var contextMenu=new WebInspector.ContextMenu(event);contextMenu.appendItem(WebInspector.UIString("Refresh"),this._update.bind(this));contextMenu.show();}},__proto__:WebInspector.VBox.prototype};WebInspector.Database=function(model,id,domain,name,version)
{this._model=model;this._id=id;this._domain=domain;this._name=name;this._version=version;}
WebInspector.Database.prototype={get id()
{return this._id;},get name()
{return this._name;},set name(x)
{this._name=x;},get version()
{return this._version;},set version(x)
{this._version=x;},get domain()
{return this._domain;},set domain(x)
{this._domain=x;},getTableNames:function(callback)
{function sortingCallback(error,names)
{if(!error)
callback(names.sort());}
this._model._agent.getDatabaseTableNames(this._id,sortingCallback);},executeSql:function(query,onSuccess,onError)
{function callback(error,columnNames,values,errorObj)
{if(error){onError(error);return;}
if(errorObj){var message;if(errorObj.message)
message=errorObj.message;else if(errorObj.code==2)
message=WebInspector.UIString("Database no longer has expected version.");else
message=WebInspector.UIString("An unexpected error %s occurred.",errorObj.code);onError(message);return;}
onSuccess(columnNames,values);}
this._model._agent.executeSQL(this._id,query,callback);}}
WebInspector.DatabaseModel=function(target)
{WebInspector.SDKModel.call(this,WebInspector.DatabaseModel,target);this._databases=[];this._agent=target.databaseAgent();}
WebInspector.DatabaseModel.Events={DatabaseAdded:"DatabaseAdded"}
WebInspector.DatabaseModel.prototype={enable:function()
{if(this._enabled)
return;this.target().registerDatabaseDispatcher(new WebInspector.DatabaseDispatcher(this));this._agent.enable();this._enabled=true;},databases:function()
{var result=[];for(var databaseId in this._databases)
result.push(this._databases[databaseId]);return result;},_addDatabase:function(database)
{this._databases.push(database);this.dispatchEventToListeners(WebInspector.DatabaseModel.Events.DatabaseAdded,database);},__proto__:WebInspector.SDKModel.prototype}
WebInspector.DatabaseDispatcher=function(model)
{this._model=model;}
WebInspector.DatabaseDispatcher.prototype={addDatabase:function(payload)
{this._model._addDatabase(new WebInspector.Database(this._model,payload.id,payload.domain,payload.name,payload.version));}}
WebInspector.DatabaseModel._symbol=Symbol("DatabaseModel");WebInspector.DatabaseModel.fromTarget=function(target)
{if(!target[WebInspector.DatabaseModel._symbol])
target[WebInspector.DatabaseModel._symbol]=new WebInspector.DatabaseModel(target);return target[WebInspector.DatabaseModel._symbol];};WebInspector.DOMStorage=function(model,securityOrigin,isLocalStorage)
{this._model=model;this._securityOrigin=securityOrigin;this._isLocalStorage=isLocalStorage;}
WebInspector.DOMStorage.storageId=function(securityOrigin,isLocalStorage)
{return{securityOrigin:securityOrigin,isLocalStorage:isLocalStorage};}
WebInspector.DOMStorage.Events={DOMStorageItemsCleared:"DOMStorageItemsCleared",DOMStorageItemRemoved:"DOMStorageItemRemoved",DOMStorageItemAdded:"DOMStorageItemAdded",DOMStorageItemUpdated:"DOMStorageItemUpdated"}
WebInspector.DOMStorage.prototype={get id()
{return WebInspector.DOMStorage.storageId(this._securityOrigin,this._isLocalStorage);},get securityOrigin()
{return this._securityOrigin;},get isLocalStorage()
{return this._isLocalStorage;},getItems:function(callback)
{this._model._agent.getDOMStorageItems(this.id,callback);},setItem:function(key,value)
{this._model._agent.setDOMStorageItem(this.id,key,value);},removeItem:function(key)
{this._model._agent.removeDOMStorageItem(this.id,key);},__proto__:WebInspector.Object.prototype}
WebInspector.DOMStorageModel=function(target)
{WebInspector.SDKModel.call(this,WebInspector.DOMStorageModel,target);this._storages={};this._agent=target.domstorageAgent();}
WebInspector.DOMStorageModel.Events={DOMStorageAdded:"DOMStorageAdded",DOMStorageRemoved:"DOMStorageRemoved"}
WebInspector.DOMStorageModel.prototype={enable:function()
{if(this._enabled)
return;this.target().registerDOMStorageDispatcher(new WebInspector.DOMStorageDispatcher(this));this.target().resourceTreeModel.addEventListener(WebInspector.ResourceTreeModel.EventTypes.SecurityOriginAdded,this._securityOriginAdded,this);this.target().resourceTreeModel.addEventListener(WebInspector.ResourceTreeModel.EventTypes.SecurityOriginRemoved,this._securityOriginRemoved,this);this._agent.enable();var securityOrigins=this.target().resourceTreeModel.securityOrigins();for(var i=0;i<securityOrigins.length;++i)
this._addOrigin(securityOrigins[i]);this._enabled=true;},_securityOriginAdded:function(event)
{this._addOrigin((event.data));},_addOrigin:function(securityOrigin)
{var localStorageKey=this._storageKey(securityOrigin,true);console.assert(!this._storages[localStorageKey]);var localStorage=new WebInspector.DOMStorage(this,securityOrigin,true);this._storages[localStorageKey]=localStorage;this.dispatchEventToListeners(WebInspector.DOMStorageModel.Events.DOMStorageAdded,localStorage);var sessionStorageKey=this._storageKey(securityOrigin,false);console.assert(!this._storages[sessionStorageKey]);var sessionStorage=new WebInspector.DOMStorage(this,securityOrigin,false);this._storages[sessionStorageKey]=sessionStorage;this.dispatchEventToListeners(WebInspector.DOMStorageModel.Events.DOMStorageAdded,sessionStorage);},_securityOriginRemoved:function(event)
{var securityOrigin=(event.data);var localStorageKey=this._storageKey(securityOrigin,true);var localStorage=this._storages[localStorageKey];console.assert(localStorage);delete this._storages[localStorageKey];this.dispatchEventToListeners(WebInspector.DOMStorageModel.Events.DOMStorageRemoved,localStorage);var sessionStorageKey=this._storageKey(securityOrigin,false);var sessionStorage=this._storages[sessionStorageKey];console.assert(sessionStorage);delete this._storages[sessionStorageKey];this.dispatchEventToListeners(WebInspector.DOMStorageModel.Events.DOMStorageRemoved,sessionStorage);},_storageKey:function(securityOrigin,isLocalStorage)
{return JSON.stringify(WebInspector.DOMStorage.storageId(securityOrigin,isLocalStorage));},_domStorageItemsCleared:function(storageId)
{var domStorage=this.storageForId(storageId);if(!domStorage)
return;var eventData={};domStorage.dispatchEventToListeners(WebInspector.DOMStorage.Events.DOMStorageItemsCleared,eventData);},_domStorageItemRemoved:function(storageId,key)
{var domStorage=this.storageForId(storageId);if(!domStorage)
return;var eventData={key:key};domStorage.dispatchEventToListeners(WebInspector.DOMStorage.Events.DOMStorageItemRemoved,eventData);},_domStorageItemAdded:function(storageId,key,value)
{var domStorage=this.storageForId(storageId);if(!domStorage)
return;var eventData={key:key,value:value};domStorage.dispatchEventToListeners(WebInspector.DOMStorage.Events.DOMStorageItemAdded,eventData);},_domStorageItemUpdated:function(storageId,key,oldValue,value)
{var domStorage=this.storageForId(storageId);if(!domStorage)
return;var eventData={key:key,oldValue:oldValue,value:value};domStorage.dispatchEventToListeners(WebInspector.DOMStorage.Events.DOMStorageItemUpdated,eventData);},storageForId:function(storageId)
{return this._storages[JSON.stringify(storageId)];},storages:function()
{var result=[];for(var id in this._storages)
result.push(this._storages[id]);return result;},__proto__:WebInspector.SDKModel.prototype}
WebInspector.DOMStorageDispatcher=function(model)
{this._model=model;}
WebInspector.DOMStorageDispatcher.prototype={domStorageItemsCleared:function(storageId)
{this._model._domStorageItemsCleared(storageId);},domStorageItemRemoved:function(storageId,key)
{this._model._domStorageItemRemoved(storageId,key);},domStorageItemAdded:function(storageId,key,value)
{this._model._domStorageItemAdded(storageId,key,value);},domStorageItemUpdated:function(storageId,key,oldValue,value)
{this._model._domStorageItemUpdated(storageId,key,oldValue,value);},}
WebInspector.DOMStorageModel._symbol=Symbol("DomStorage");WebInspector.DOMStorageModel.fromTarget=function(target)
{if(!target[WebInspector.DOMStorageModel._symbol])
target[WebInspector.DOMStorageModel._symbol]=new WebInspector.DOMStorageModel(target);return target[WebInspector.DOMStorageModel._symbol];};WebInspector.DOMStorageItemsView=function(domStorage)
{WebInspector.DataGridContainerWidget.call(this);this.domStorage=domStorage;this.element.classList.add("storage-view","table");this.deleteButton=new WebInspector.ToolbarButton(WebInspector.UIString("Delete"),"delete-toolbar-item");this.deleteButton.setVisible(false);this.deleteButton.addEventListener("click",this._deleteButtonClicked,this);this.refreshButton=new WebInspector.ToolbarButton(WebInspector.UIString("Refresh"),"refresh-toolbar-item");this.refreshButton.addEventListener("click",this._refreshButtonClicked,this);this.domStorage.addEventListener(WebInspector.DOMStorage.Events.DOMStorageItemsCleared,this._domStorageItemsCleared,this);this.domStorage.addEventListener(WebInspector.DOMStorage.Events.DOMStorageItemRemoved,this._domStorageItemRemoved,this);this.domStorage.addEventListener(WebInspector.DOMStorage.Events.DOMStorageItemAdded,this._domStorageItemAdded,this);this.domStorage.addEventListener(WebInspector.DOMStorage.Events.DOMStorageItemUpdated,this._domStorageItemUpdated,this);}
WebInspector.DOMStorageItemsView.prototype={toolbarItems:function()
{return[this.refreshButton,this.deleteButton];},wasShown:function()
{this._update();},willHide:function()
{this.deleteButton.setVisible(false);},_domStorageItemsCleared:function(event)
{if(!this.isShowing()||!this._dataGrid)
return;this._dataGrid.rootNode().removeChildren();this._dataGrid.addCreationNode(false);this.deleteButton.setVisible(false);event.consume(true);},_domStorageItemRemoved:function(event)
{if(!this.isShowing()||!this._dataGrid)
return;var storageData=event.data;var rootNode=this._dataGrid.rootNode();var children=rootNode.children;event.consume(true);for(var i=0;i<children.length;++i){var childNode=children[i];if(childNode.data.key===storageData.key){rootNode.removeChild(childNode);this.deleteButton.setVisible(children.length>1);return;}}},_domStorageItemAdded:function(event)
{if(!this.isShowing()||!this._dataGrid)
return;var storageData=event.data;var rootNode=this._dataGrid.rootNode();var children=rootNode.children;event.consume(true);this.deleteButton.setVisible(true);for(var i=0;i<children.length;++i)
if(children[i].data.key===storageData.key)
return;var childNode=new WebInspector.DataGridNode({key:storageData.key,value:storageData.value},false);rootNode.insertChild(childNode,children.length-1);},_domStorageItemUpdated:function(event)
{if(!this.isShowing()||!this._dataGrid)
return;var storageData=event.data;var rootNode=this._dataGrid.rootNode();var children=rootNode.children;event.consume(true);var keyFound=false;for(var i=0;i<children.length;++i){var childNode=children[i];if(childNode.data.key===storageData.key){if(keyFound){rootNode.removeChild(childNode);return;}
keyFound=true;if(childNode.data.value!==storageData.value){childNode.data.value=storageData.value;childNode.refresh();childNode.select();childNode.reveal();}
this.deleteButton.setVisible(true);}}},_update:function()
{this.detachChildWidgets();this.domStorage.getItems(this._showDOMStorageItems.bind(this));},_showDOMStorageItems:function(error,items)
{if(error)
return;this._dataGrid=this._dataGridForDOMStorageItems(items);this.appendDataGrid(this._dataGrid);this.deleteButton.setVisible(this._dataGrid.rootNode().children.length>1);},_dataGridForDOMStorageItems:function(items)
{var columns=[{id:"key",title:WebInspector.UIString("Key"),editable:true,weight:50},{id:"value",title:WebInspector.UIString("Value"),editable:true,weight:50}];var nodes=[];var keys=[];var length=items.length;for(var i=0;i<items.length;i++){var key=items[i][0];var value=items[i][1];var node=new WebInspector.DataGridNode({key:key,value:value},false);node.selectable=true;nodes.push(node);keys.push(key);}
var dataGrid=new WebInspector.DataGrid(columns,this._editingCallback.bind(this),this._deleteCallback.bind(this));dataGrid.setName("DOMStorageItemsView");length=nodes.length;for(var i=0;i<length;++i)
dataGrid.rootNode().appendChild(nodes[i]);dataGrid.addCreationNode(false);if(length>0)
nodes[0].selected=true;return dataGrid;},_deleteButtonClicked:function(event)
{if(!this._dataGrid||!this._dataGrid.selectedNode)
return;this._deleteCallback(this._dataGrid.selectedNode);this._dataGrid.changeNodeAfterDeletion();},_refreshButtonClicked:function(event)
{this._update();},_editingCallback:function(editingNode,columnIdentifier,oldText,newText)
{var domStorage=this.domStorage;if("key"===columnIdentifier){if(typeof oldText==="string")
domStorage.removeItem(oldText);domStorage.setItem(newText,editingNode.data.value||'');this._removeDupes(editingNode);}else
domStorage.setItem(editingNode.data.key||'',newText);},_removeDupes:function(masterNode)
{var rootNode=this._dataGrid.rootNode();var children=rootNode.children;for(var i=children.length-1;i>=0;--i){var childNode=children[i];if((childNode.data.key===masterNode.data.key)&&(masterNode!==childNode))
rootNode.removeChild(childNode);}},_deleteCallback:function(node)
{if(!node||node.isCreationNode)
return;if(this.domStorage)
this.domStorage.removeItem(node.data.key);},__proto__:WebInspector.DataGridContainerWidget.prototype};WebInspector.DatabaseQueryView=function(database)
{WebInspector.VBox.call(this);this.database=database;this.element.classList.add("storage-view","query","monospace");this.element.addEventListener("selectstart",this._selectStart.bind(this),false);this._promptElement=createElement("div");this._promptElement.className="database-query-prompt";this._promptElement.appendChild(createElement("br"));this._promptElement.addEventListener("keydown",this._promptKeyDown.bind(this),true);this.element.appendChild(this._promptElement);this._prompt=new WebInspector.TextPromptWithHistory(this.completions.bind(this)," ");this._proxyElement=this._prompt.attach(this._promptElement);this.element.addEventListener("click",this._messagesClicked.bind(this),true);}
WebInspector.DatabaseQueryView.Events={SchemaUpdated:"SchemaUpdated"}
WebInspector.DatabaseQueryView.prototype={toolbarItems:function()
{return[];},_messagesClicked:function()
{if(!this._prompt.isCaretInsidePrompt()&&this.element.isComponentSelectionCollapsed())
this._prompt.moveCaretToEndOfPrompt();},completions:function(proxyElement,text,cursorOffset,wordRange,force,completionsReadyCallback)
{var prefix=wordRange.toString().toLowerCase();if(!prefix)
return;var results=[];function accumulateMatches(textArray)
{for(var i=0;i<textArray.length;++i){var text=textArray[i].toLowerCase();if(text.length<prefix.length)
continue;if(!text.startsWith(prefix))
continue;results.push(textArray[i]);}}
function tableNamesCallback(tableNames)
{accumulateMatches(tableNames.map(function(name){return name+" ";}));accumulateMatches(["SELECT ","FROM ","WHERE ","LIMIT ","DELETE FROM ","CREATE ","DROP ","TABLE ","INDEX ","UPDATE ","INSERT INTO ","VALUES ("]);completionsReadyCallback(results);}
this.database.getTableNames(tableNamesCallback);},_selectStart:function(event)
{if(this._selectionTimeout)
clearTimeout(this._selectionTimeout);this._prompt.clearAutoComplete();function moveBackIfOutside()
{delete this._selectionTimeout;if(!this._prompt.isCaretInsidePrompt()&&this.element.isComponentSelectionCollapsed())
this._prompt.moveCaretToEndOfPrompt();this._prompt.autoCompleteSoon();}
this._selectionTimeout=setTimeout(moveBackIfOutside.bind(this),100);},_promptKeyDown:function(event)
{if(isEnterKey(event)){this._enterKeyPressed(event);return;}},_enterKeyPressed:function(event)
{event.consume(true);this._prompt.clearAutoComplete(true);var query=this._prompt.text();if(!query.length)
return;this._prompt.pushHistoryItem(query);this._prompt.setText("");this.database.executeSql(query,this._queryFinished.bind(this,query),this._queryError.bind(this,query));},_queryFinished:function(query,columnNames,values)
{var dataGrid=WebInspector.SortableDataGrid.create(columnNames,values);var trimmedQuery=query.trim();if(dataGrid){dataGrid.renderInline();var dataGridContainer=new WebInspector.DataGridContainerWidget();dataGridContainer.appendDataGrid(dataGrid);this._appendViewQueryResult(trimmedQuery,dataGridContainer);dataGrid.autoSizeColumns(5);}
if(trimmedQuery.match(/^create /i)||trimmedQuery.match(/^drop table /i))
this.dispatchEventToListeners(WebInspector.DatabaseQueryView.Events.SchemaUpdated,this.database);},_queryError:function(query,errorMessage)
{this._appendErrorQueryResult(query,errorMessage);},_appendViewQueryResult:function(query,view)
{var resultElement=this._appendQueryResult(query);view.show(resultElement);this._promptElement.scrollIntoView(false);},_appendErrorQueryResult:function(query,errorText)
{var resultElement=this._appendQueryResult(query);resultElement.classList.add("error");resultElement.textContent=errorText;this._promptElement.scrollIntoView(false);},_appendQueryResult:function(query)
{var element=createElement("div");element.className="database-user-query";this.element.insertBefore(element,this._proxyElement);var commandTextElement=createElement("span");commandTextElement.className="database-query-text";commandTextElement.textContent=query;element.appendChild(commandTextElement);var resultElement=createElement("div");resultElement.className="database-query-result";element.appendChild(resultElement);return resultElement;},__proto__:WebInspector.VBox.prototype};WebInspector.DatabaseTableView=function(database,tableName)
{WebInspector.DataGridContainerWidget.call(this);this.database=database;this.tableName=tableName;this.element.classList.add("storage-view","table");this._visibleColumnsSetting=WebInspector.settings.createSetting("databaseTableViewVisibleColumns",{});this.refreshButton=new WebInspector.ToolbarButton(WebInspector.UIString("Refresh"),"refresh-toolbar-item");this.refreshButton.addEventListener("click",this._refreshButtonClicked,this);this._visibleColumnsInput=new WebInspector.ToolbarInput(WebInspector.UIString("Visible columns"),1);this._visibleColumnsInput.addEventListener(WebInspector.ToolbarInput.Event.TextChanged,this._onVisibleColumnsChanged,this);}
WebInspector.DatabaseTableView.prototype={wasShown:function()
{this.update();},toolbarItems:function()
{return[this.refreshButton,this._visibleColumnsInput];},_escapeTableName:function(tableName)
{return tableName.replace(/\"/g,"\"\"");},update:function()
{this.database.executeSql("SELECT rowid, * FROM \""+this._escapeTableName(this.tableName)+"\"",this._queryFinished.bind(this),this._queryError.bind(this));},_queryFinished:function(columnNames,values)
{this.detachChildWidgets();this.element.removeChildren();this._dataGrid=WebInspector.SortableDataGrid.create(columnNames,values);this._visibleColumnsInput.setVisible(!!this._dataGrid);if(!this._dataGrid){this._emptyWidget=new WebInspector.EmptyWidget(WebInspector.UIString("The “%s”\ntable is empty.",this.tableName));this._emptyWidget.show(this.element);return;}
this.appendDataGrid(this._dataGrid);this._dataGrid.autoSizeColumns(5);this._columnsMap=new Map();for(var i=1;i<columnNames.length;++i)
this._columnsMap.set(columnNames[i],String(i));this._lastVisibleColumns="";var visibleColumnsText=this._visibleColumnsSetting.get()[this.tableName]||"";this._visibleColumnsInput.setValue(visibleColumnsText);this._onVisibleColumnsChanged();},_onVisibleColumnsChanged:function()
{if(!this._dataGrid)
return;var text=this._visibleColumnsInput.value();var parts=text.split(/[\s,]+/);var matches=new Set();var columnsVisibility={};columnsVisibility["0"]=true;for(var i=0;i<parts.length;++i){var part=parts[i];if(this._columnsMap.has(part)){matches.add(part);columnsVisibility[this._columnsMap.get(part)]=true;}}
var newVisibleColumns=matches.valuesArray().sort().join(", ");if(newVisibleColumns.length===0){for(var v of this._columnsMap.values())
columnsVisibility[v]=true;}
if(newVisibleColumns===this._lastVisibleColumns)
return;var visibleColumnsRegistry=this._visibleColumnsSetting.get();visibleColumnsRegistry[this.tableName]=text;this._visibleColumnsSetting.set(visibleColumnsRegistry);this._dataGrid.setColumnsVisiblity(columnsVisibility);this._lastVisibleColumns=newVisibleColumns;},_queryError:function(error)
{this.detachChildWidgets();this.element.removeChildren();var errorMsgElement=createElement("div");errorMsgElement.className="storage-table-error";errorMsgElement.textContent=WebInspector.UIString("An error occurred trying to\nread the “%s” table.",this.tableName);this.element.appendChild(errorMsgElement);},_refreshButtonClicked:function(event)
{this.update();},__proto__:WebInspector.DataGridContainerWidget.prototype};WebInspector.DirectoryContentView=function()
{const indexes=WebInspector.DirectoryContentView.columnIndexes;var columns=[{id:indexes.Name,title:WebInspector.UIString("Name"),sortable:true,sort:WebInspector.DataGrid.Order.Ascending,width:"20%"},{id:indexes.URL,title:WebInspector.UIString("URL"),sortable:true,width:"20%"},{id:indexes.Type,title:WebInspector.UIString("Type"),sortable:true,width:"15%"},{id:indexes.Size,title:WebInspector.UIString("Size"),sortable:true,width:"10%"},{id:indexes.ModificationTime,title:WebInspector.UIString("Modification Time"),sortable:true,width:"25%"}];WebInspector.SortableDataGrid.call(this,columns);this.addEventListener(WebInspector.DataGrid.Events.SortingChanged,this._sort,this);}
WebInspector.DirectoryContentView.columnIndexes={Name:"0",URL:"1",Type:"2",Size:"3",ModificationTime:"4"}
WebInspector.DirectoryContentView.prototype={showEntries:function(entries)
{const indexes=WebInspector.DirectoryContentView.columnIndexes;this.rootNode().removeChildren();for(var i=0;i<entries.length;++i)
this.rootNode().appendChild(new WebInspector.DirectoryContentView.Node(entries[i]));},_sort:function()
{var column=this.sortColumnIdentifier();if(!column)
return;this.sortNodes(WebInspector.DirectoryContentView.Node.comparator(column),!this.isSortOrderAscending());},__proto__:WebInspector.SortableDataGrid.prototype}
WebInspector.DirectoryContentView.Node=function(entry)
{const indexes=WebInspector.DirectoryContentView.columnIndexes;var data={};data[indexes.Name]=entry.name;data[indexes.URL]=entry.url;data[indexes.Type]=entry.isDirectory?WebInspector.UIString("Directory"):entry.mimeType;data[indexes.Size]="";data[indexes.ModificationTime]="";WebInspector.SortableDataGridNode.call(this,data);this._entry=entry;this._metadata=null;this._entry.requestMetadata(this._metadataReceived.bind(this));}
WebInspector.DirectoryContentView.Node.comparator=function(column)
{const indexes=WebInspector.DirectoryContentView.columnIndexes;switch(column){case indexes.Name:case indexes.URL:return function(x,y)
{return isDirectoryCompare(x,y)||nameCompare(x,y);};case indexes.Type:return function(x,y)
{return isDirectoryCompare(x,y)||typeCompare(x,y)||nameCompare(x,y);};case indexes.Size:return function(x,y)
{return isDirectoryCompare(x,y)||sizeCompare(x,y)||nameCompare(x,y);};case indexes.ModificationTime:return function(x,y)
{return isDirectoryCompare(x,y)||modificationTimeCompare(x,y)||nameCompare(x,y);};default:return WebInspector.SortableDataGrid.TrivialComparator;}
function isDirectoryCompare(x,y)
{if(x._entry.isDirectory!=y._entry.isDirectory)
return y._entry.isDirectory?1:-1;return 0;}
function nameCompare(x,y)
{return x._entry.name.compareTo(y._entry.name);}
function typeCompare(x,y)
{return(x._entry.mimeType||"").compareTo(y._entry.mimeType||"");}
function sizeCompare(x,y)
{return((x._metadata?x._metadata.size:0)-(y._metadata?y._metadata.size:0));}
function modificationTimeCompare(x,y)
{return((x._metadata?x._metadata.modificationTime:0)-(y._metadata?y._metadata.modificationTime:0));}}
WebInspector.DirectoryContentView.Node.prototype={_metadataReceived:function(errorCode,metadata)
{const indexes=WebInspector.DirectoryContentView.columnIndexes;if(errorCode!==0)
return;this._metadata=metadata;var data=this.data;if(this._entry.isDirectory)
data[indexes.Size]=WebInspector.UIString("-");else
data[indexes.Size]=Number.bytesToString(metadata.size);data[indexes.ModificationTime]=new Date(metadata.modificationTime).toISOString();this.data=data;},__proto__:WebInspector.SortableDataGridNode.prototype};WebInspector.IndexedDBModel=function(target)
{WebInspector.SDKModel.call(this,WebInspector.IndexedDBModel,target);this._agent=target.indexedDBAgent();this._databases=new Map();this._databaseNamesBySecurityOrigin={};}
WebInspector.IndexedDBModel.KeyTypes={NumberType:"number",StringType:"string",DateType:"date",ArrayType:"array"};WebInspector.IndexedDBModel.KeyPathTypes={NullType:"null",StringType:"string",ArrayType:"array"};WebInspector.IndexedDBModel.keyFromIDBKey=function(idbKey)
{if(typeof(idbKey)==="undefined"||idbKey===null)
return null;var key={};switch(typeof(idbKey)){case"number":key.number=idbKey;key.type=WebInspector.IndexedDBModel.KeyTypes.NumberType;break;case"string":key.string=idbKey;key.type=WebInspector.IndexedDBModel.KeyTypes.StringType;break;case"object":if(idbKey instanceof Date){key.date=idbKey.getTime();key.type=WebInspector.IndexedDBModel.KeyTypes.DateType;}else if(Array.isArray(idbKey)){key.array=[];for(var i=0;i<idbKey.length;++i)
key.array.push(WebInspector.IndexedDBModel.keyFromIDBKey(idbKey[i]));key.type=WebInspector.IndexedDBModel.KeyTypes.ArrayType;}
break;default:return null;}
return key;}
WebInspector.IndexedDBModel.keyRangeFromIDBKeyRange=function(idbKeyRange)
{if(typeof idbKeyRange==="undefined"||idbKeyRange===null)
return null;var keyRange={};keyRange.lower=WebInspector.IndexedDBModel.keyFromIDBKey(idbKeyRange.lower);keyRange.upper=WebInspector.IndexedDBModel.keyFromIDBKey(idbKeyRange.upper);keyRange.lowerOpen=idbKeyRange.lowerOpen;keyRange.upperOpen=idbKeyRange.upperOpen;return keyRange;}
WebInspector.IndexedDBModel.idbKeyPathFromKeyPath=function(keyPath)
{var idbKeyPath;switch(keyPath.type){case WebInspector.IndexedDBModel.KeyPathTypes.NullType:idbKeyPath=null;break;case WebInspector.IndexedDBModel.KeyPathTypes.StringType:idbKeyPath=keyPath.string;break;case WebInspector.IndexedDBModel.KeyPathTypes.ArrayType:idbKeyPath=keyPath.array;break;}
return idbKeyPath;}
WebInspector.IndexedDBModel.keyPathStringFromIDBKeyPath=function(idbKeyPath)
{if(typeof idbKeyPath==="string")
return"\""+idbKeyPath+"\"";if(idbKeyPath instanceof Array)
return"[\""+idbKeyPath.join("\", \"")+"\"]";return null;}
WebInspector.IndexedDBModel.EventTypes={DatabaseAdded:"DatabaseAdded",DatabaseRemoved:"DatabaseRemoved",DatabaseLoaded:"DatabaseLoaded"}
WebInspector.IndexedDBModel.prototype={enable:function()
{if(this._enabled)
return;this._agent.enable();this.target().resourceTreeModel.addEventListener(WebInspector.ResourceTreeModel.EventTypes.SecurityOriginAdded,this._securityOriginAdded,this);this.target().resourceTreeModel.addEventListener(WebInspector.ResourceTreeModel.EventTypes.SecurityOriginRemoved,this._securityOriginRemoved,this);var securityOrigins=this.target().resourceTreeModel.securityOrigins();for(var i=0;i<securityOrigins.length;++i)
this._addOrigin(securityOrigins[i]);this._enabled=true;},refreshDatabaseNames:function()
{for(var securityOrigin in this._databaseNamesBySecurityOrigin)
this._loadDatabaseNames(securityOrigin);},refreshDatabase:function(databaseId)
{this._loadDatabase(databaseId);},clearObjectStore:function(databaseId,objectStoreName,callback)
{this._agent.clearObjectStore(databaseId.securityOrigin,databaseId.name,objectStoreName,callback);},_securityOriginAdded:function(event)
{var securityOrigin=(event.data);this._addOrigin(securityOrigin);},_securityOriginRemoved:function(event)
{var securityOrigin=(event.data);this._removeOrigin(securityOrigin);},_addOrigin:function(securityOrigin)
{console.assert(!this._databaseNamesBySecurityOrigin[securityOrigin]);this._databaseNamesBySecurityOrigin[securityOrigin]=[];this._loadDatabaseNames(securityOrigin);},_removeOrigin:function(securityOrigin)
{console.assert(this._databaseNamesBySecurityOrigin[securityOrigin]);for(var i=0;i<this._databaseNamesBySecurityOrigin[securityOrigin].length;++i)
this._databaseRemoved(securityOrigin,this._databaseNamesBySecurityOrigin[securityOrigin][i]);delete this._databaseNamesBySecurityOrigin[securityOrigin];},_updateOriginDatabaseNames:function(securityOrigin,databaseNames)
{var newDatabaseNames=databaseNames.keySet();var oldDatabaseNames=this._databaseNamesBySecurityOrigin[securityOrigin].keySet();this._databaseNamesBySecurityOrigin[securityOrigin]=databaseNames;for(var databaseName in oldDatabaseNames){if(!newDatabaseNames[databaseName])
this._databaseRemoved(securityOrigin,databaseName);}
for(var databaseName in newDatabaseNames){if(!oldDatabaseNames[databaseName])
this._databaseAdded(securityOrigin,databaseName);}},databases:function()
{var result=[];for(var securityOrigin in this._databaseNamesBySecurityOrigin){var databaseNames=this._databaseNamesBySecurityOrigin[securityOrigin];for(var i=0;i<databaseNames.length;++i){result.push(new WebInspector.IndexedDBModel.DatabaseId(securityOrigin,databaseNames[i]));}}
return result;},_databaseAdded:function(securityOrigin,databaseName)
{var databaseId=new WebInspector.IndexedDBModel.DatabaseId(securityOrigin,databaseName);this.dispatchEventToListeners(WebInspector.IndexedDBModel.EventTypes.DatabaseAdded,databaseId);},_databaseRemoved:function(securityOrigin,databaseName)
{var databaseId=new WebInspector.IndexedDBModel.DatabaseId(securityOrigin,databaseName);this.dispatchEventToListeners(WebInspector.IndexedDBModel.EventTypes.DatabaseRemoved,databaseId);},_loadDatabaseNames:function(securityOrigin)
{function callback(error,databaseNames)
{if(error){console.error("IndexedDBAgent error: "+error);return;}
if(!this._databaseNamesBySecurityOrigin[securityOrigin])
return;this._updateOriginDatabaseNames(securityOrigin,databaseNames);}
this._agent.requestDatabaseNames(securityOrigin,callback.bind(this));},_loadDatabase:function(databaseId)
{function callback(error,databaseWithObjectStores)
{if(error){console.error("IndexedDBAgent error: "+error);return;}
if(!this._databaseNamesBySecurityOrigin[databaseId.securityOrigin])
return;var databaseModel=new WebInspector.IndexedDBModel.Database(databaseId,databaseWithObjectStores.version,databaseWithObjectStores.intVersion);this._databases.set(databaseId,databaseModel);for(var i=0;i<databaseWithObjectStores.objectStores.length;++i){var objectStore=databaseWithObjectStores.objectStores[i];var objectStoreIDBKeyPath=WebInspector.IndexedDBModel.idbKeyPathFromKeyPath(objectStore.keyPath);var objectStoreModel=new WebInspector.IndexedDBModel.ObjectStore(objectStore.name,objectStoreIDBKeyPath,objectStore.autoIncrement);for(var j=0;j<objectStore.indexes.length;++j){var index=objectStore.indexes[j];var indexIDBKeyPath=WebInspector.IndexedDBModel.idbKeyPathFromKeyPath(index.keyPath);var indexModel=new WebInspector.IndexedDBModel.Index(index.name,indexIDBKeyPath,index.unique,index.multiEntry);objectStoreModel.indexes[indexModel.name]=indexModel;}
databaseModel.objectStores[objectStoreModel.name]=objectStoreModel;}
this.dispatchEventToListeners(WebInspector.IndexedDBModel.EventTypes.DatabaseLoaded,databaseModel);}
this._agent.requestDatabase(databaseId.securityOrigin,databaseId.name,callback.bind(this));},loadObjectStoreData:function(databaseId,objectStoreName,idbKeyRange,skipCount,pageSize,callback)
{this._requestData(databaseId,databaseId.name,objectStoreName,"",idbKeyRange,skipCount,pageSize,callback);},loadIndexData:function(databaseId,objectStoreName,indexName,idbKeyRange,skipCount,pageSize,callback)
{this._requestData(databaseId,databaseId.name,objectStoreName,indexName,idbKeyRange,skipCount,pageSize,callback);},_requestData:function(databaseId,databaseName,objectStoreName,indexName,idbKeyRange,skipCount,pageSize,callback)
{function innerCallback(error,dataEntries,hasMore)
{if(error){console.error("IndexedDBAgent error: "+error);return;}
if(!this._databaseNamesBySecurityOrigin[databaseId.securityOrigin])
return;var entries=[];for(var i=0;i<dataEntries.length;++i){var key=WebInspector.RemoteObject.fromLocalObject(JSON.parse(dataEntries[i].key));var primaryKey=WebInspector.RemoteObject.fromLocalObject(JSON.parse(dataEntries[i].primaryKey));var value=WebInspector.RemoteObject.fromLocalObject(JSON.parse(dataEntries[i].value));entries.push(new WebInspector.IndexedDBModel.Entry(key,primaryKey,value));}
callback(entries,hasMore);}
var keyRange=WebInspector.IndexedDBModel.keyRangeFromIDBKeyRange(idbKeyRange);this._agent.requestData(databaseId.securityOrigin,databaseName,objectStoreName,indexName,skipCount,pageSize,keyRange?keyRange:undefined,innerCallback.bind(this));},__proto__:WebInspector.SDKModel.prototype}
WebInspector.IndexedDBModel.Entry=function(key,primaryKey,value)
{this.key=key;this.primaryKey=primaryKey;this.value=value;}
WebInspector.IndexedDBModel.DatabaseId=function(securityOrigin,name)
{this.securityOrigin=securityOrigin;this.name=name;}
WebInspector.IndexedDBModel.DatabaseId.prototype={equals:function(databaseId)
{return this.name===databaseId.name&&this.securityOrigin===databaseId.securityOrigin;},}
WebInspector.IndexedDBModel.Database=function(databaseId,version,intVersion)
{this.databaseId=databaseId;this.version=version;this.intVersion=intVersion;this.objectStores={};}
WebInspector.IndexedDBModel.ObjectStore=function(name,keyPath,autoIncrement)
{this.name=name;this.keyPath=keyPath;this.autoIncrement=autoIncrement;this.indexes={};}
WebInspector.IndexedDBModel.ObjectStore.prototype={get keyPathString()
{return WebInspector.IndexedDBModel.keyPathStringFromIDBKeyPath(this.keyPath);}}
WebInspector.IndexedDBModel.Index=function(name,keyPath,unique,multiEntry)
{this.name=name;this.keyPath=keyPath;this.unique=unique;this.multiEntry=multiEntry;}
WebInspector.IndexedDBModel.Index.prototype={get keyPathString()
{return WebInspector.IndexedDBModel.keyPathStringFromIDBKeyPath(this.keyPath);}}
WebInspector.IndexedDBModel.fromTarget=function(target)
{var model=(target.model(WebInspector.IndexedDBModel));if(!model)
model=new WebInspector.IndexedDBModel(target);return model;};WebInspector.IDBDatabaseView=function(database)
{WebInspector.VBox.call(this);this.registerRequiredCSS("resources/indexedDBViews.css");this.element.classList.add("indexed-db-database-view");this.element.classList.add("storage-view");this._headersTreeOutline=new TreeOutline();this._headersTreeOutline.element.classList.add("outline-disclosure");this.element.appendChild(this._headersTreeOutline.element);this._headersTreeOutline.expandTreeElementsWhenArrowing=true;this._securityOriginTreeElement=new TreeElement();this._securityOriginTreeElement.selectable=false;this._headersTreeOutline.appendChild(this._securityOriginTreeElement);this._nameTreeElement=new TreeElement();this._nameTreeElement.selectable=false;this._headersTreeOutline.appendChild(this._nameTreeElement);this._intVersionTreeElement=new TreeElement();this._intVersionTreeElement.selectable=false;this._headersTreeOutline.appendChild(this._intVersionTreeElement);this._stringVersionTreeElement=new TreeElement();this._stringVersionTreeElement.selectable=false;this._headersTreeOutline.appendChild(this._stringVersionTreeElement);this.update(database);}
WebInspector.IDBDatabaseView.prototype={toolbarItems:function()
{return[];},_formatHeader:function(name,value)
{var fragment=createDocumentFragment();fragment.createChild("div","attribute-name").textContent=name+":";fragment.createChild("div","attribute-value source-code").textContent=value;return fragment;},_refreshDatabase:function()
{this._securityOriginTreeElement.title=this._formatHeader(WebInspector.UIString("Security origin"),this._database.databaseId.securityOrigin);this._nameTreeElement.title=this._formatHeader(WebInspector.UIString("Name"),this._database.databaseId.name);this._stringVersionTreeElement.title=this._formatHeader(WebInspector.UIString("String Version"),this._database.version);this._intVersionTreeElement.title=this._formatHeader(WebInspector.UIString("Integer Version"),this._database.intVersion);},update:function(database)
{this._database=database;this._refreshDatabase();},__proto__:WebInspector.VBox.prototype}
WebInspector.IDBDataView=function(model,databaseId,objectStore,index)
{WebInspector.DataGridContainerWidget.call(this);this.registerRequiredCSS("resources/indexedDBViews.css");this._model=model;this._databaseId=databaseId;this._isIndex=!!index;this.element.classList.add("indexed-db-data-view");this._createEditorToolbar();this._refreshButton=new WebInspector.ToolbarButton(WebInspector.UIString("Refresh"),"refresh-toolbar-item");this._refreshButton.addEventListener("click",this._refreshButtonClicked,this);this._clearButton=new WebInspector.ToolbarButton(WebInspector.UIString("Clear object store"),"clear-toolbar-item");this._clearButton.addEventListener("click",this._clearButtonClicked,this);this._pageSize=50;this._skipCount=0;this.update(objectStore,index);this._entries=[];}
WebInspector.IDBDataView.prototype={_createDataGrid:function()
{var keyPath=this._isIndex?this._index.keyPath:this._objectStore.keyPath;var columns=[];columns.push({id:"number",title:WebInspector.UIString("#"),width:"50px"});columns.push({id:"key",titleDOMFragment:this._keyColumnHeaderFragment(WebInspector.UIString("Key"),keyPath)});if(this._isIndex)
columns.push({id:"primaryKey",titleDOMFragment:this._keyColumnHeaderFragment(WebInspector.UIString("Primary key"),this._objectStore.keyPath)});columns.push({id:"value",title:WebInspector.UIString("Value")});var dataGrid=new WebInspector.DataGrid(columns);return dataGrid;},_keyColumnHeaderFragment:function(prefix,keyPath)
{var keyColumnHeaderFragment=createDocumentFragment();keyColumnHeaderFragment.createTextChild(prefix);if(keyPath===null)
return keyColumnHeaderFragment;keyColumnHeaderFragment.createTextChild(" ("+WebInspector.UIString("Key path: "));if(Array.isArray(keyPath)){keyColumnHeaderFragment.createTextChild("[");for(var i=0;i<keyPath.length;++i){if(i!=0)
keyColumnHeaderFragment.createTextChild(", ");keyColumnHeaderFragment.appendChild(this._keyPathStringFragment(keyPath[i]));}
keyColumnHeaderFragment.createTextChild("]");}else{var keyPathString=(keyPath);keyColumnHeaderFragment.appendChild(this._keyPathStringFragment(keyPathString));}
keyColumnHeaderFragment.createTextChild(")");return keyColumnHeaderFragment;},_keyPathStringFragment:function(keyPathString)
{var keyPathStringFragment=createDocumentFragment();keyPathStringFragment.createTextChild("\"");var keyPathSpan=keyPathStringFragment.createChild("span","source-code indexed-db-key-path");keyPathSpan.textContent=keyPathString;keyPathStringFragment.createTextChild("\"");return keyPathStringFragment;},_createEditorToolbar:function()
{var editorToolbar=new WebInspector.Toolbar(this.element);editorToolbar.element.classList.add("data-view-toolbar");this._pageBackButton=new WebInspector.ToolbarButton(WebInspector.UIString("Show previous page"),"play-backwards-toolbar-item");this._pageBackButton.addEventListener("click",this._pageBackButtonClicked,this);editorToolbar.appendToolbarItem(this._pageBackButton);this._pageForwardButton=new WebInspector.ToolbarButton(WebInspector.UIString("Show next page"),"play-toolbar-item");this._pageForwardButton.setEnabled(false);this._pageForwardButton.addEventListener("click",this._pageForwardButtonClicked,this);editorToolbar.appendToolbarItem(this._pageForwardButton);this._keyInputElement=editorToolbar.element.createChild("input","key-input");this._keyInputElement.placeholder=WebInspector.UIString("Start from key");this._keyInputElement.addEventListener("paste",this._keyInputChanged.bind(this),false);this._keyInputElement.addEventListener("cut",this._keyInputChanged.bind(this),false);this._keyInputElement.addEventListener("keypress",this._keyInputChanged.bind(this),false);this._keyInputElement.addEventListener("keydown",this._keyInputChanged.bind(this),false);},_pageBackButtonClicked:function()
{this._skipCount=Math.max(0,this._skipCount-this._pageSize);this._updateData(false);},_pageForwardButtonClicked:function()
{this._skipCount=this._skipCount+this._pageSize;this._updateData(false);},_keyInputChanged:function()
{window.setTimeout(this._updateData.bind(this,false),0);},update:function(objectStore,index)
{this._objectStore=objectStore;this._index=index;if(this._dataGrid)
this.removeDataGrid(this._dataGrid);this._dataGrid=this._createDataGrid();this.appendDataGrid(this._dataGrid);this._skipCount=0;this._updateData(true);},_parseKey:function(keyString)
{var result;try{result=JSON.parse(keyString);}catch(e){result=keyString;}
return result;},_updateData:function(force)
{var key=this._parseKey(this._keyInputElement.value);var pageSize=this._pageSize;var skipCount=this._skipCount;this._refreshButton.setEnabled(false);this._clearButton.setEnabled(!this._isIndex);if(!force&&this._lastKey===key&&this._lastPageSize===pageSize&&this._lastSkipCount===skipCount)
return;if(this._lastKey!==key||this._lastPageSize!==pageSize){skipCount=0;this._skipCount=0;}
this._lastKey=key;this._lastPageSize=pageSize;this._lastSkipCount=skipCount;function callback(entries,hasMore)
{this._refreshButton.setEnabled(true);this.clear();this._entries=entries;for(var i=0;i<entries.length;++i){var data={};data["number"]=i+skipCount;data["key"]=entries[i].key;data["primaryKey"]=entries[i].primaryKey;data["value"]=entries[i].value;var node=new WebInspector.IDBDataGridNode(data);this._dataGrid.rootNode().appendChild(node);}
this._pageBackButton.setEnabled(!!skipCount);this._pageForwardButton.setEnabled(hasMore);}
var idbKeyRange=key?window.IDBKeyRange.lowerBound(key):null;if(this._isIndex)
this._model.loadIndexData(this._databaseId,this._objectStore.name,this._index.name,idbKeyRange,skipCount,pageSize,callback.bind(this));else
this._model.loadObjectStoreData(this._databaseId,this._objectStore.name,idbKeyRange,skipCount,pageSize,callback.bind(this));},_refreshButtonClicked:function(event)
{this._updateData(true);},_clearButtonClicked:function(event)
{function cleared(){this._clearButton.setEnabled(true);this._updateData(true);}
this._clearButton.setEnabled(false);this._model.clearObjectStore(this._databaseId,this._objectStore.name,cleared.bind(this));},toolbarItems:function()
{return[this._refreshButton,this._clearButton];},clear:function()
{this._dataGrid.rootNode().removeChildren();this._entries=[];},__proto__:WebInspector.DataGridContainerWidget.prototype}
WebInspector.IDBDataGridNode=function(data)
{WebInspector.DataGridNode.call(this,data,false);this.selectable=false;}
WebInspector.IDBDataGridNode.prototype={createCell:function(columnIdentifier)
{var cell=WebInspector.DataGridNode.prototype.createCell.call(this,columnIdentifier);var value=this.data[columnIdentifier];switch(columnIdentifier){case"value":case"key":case"primaryKey":cell.removeChildren();var objectElement=WebInspector.ObjectPropertiesSection.defaultObjectPresentation(value,true);cell.appendChild(objectElement);break;default:}
return cell;},__proto__:WebInspector.DataGridNode.prototype};WebInspector.FileContentView=function(file)
{WebInspector.VBox.call(this);this._innerView=(null);this._file=file;this._content=null;}
WebInspector.FileContentView.prototype={wasShown:function()
{if(!this._innerView){if(this._file.isTextFile)
this._innerView=new WebInspector.EmptyWidget("");else
this._innerView=new WebInspector.EmptyWidget(WebInspector.UIString("Binary File"));this.refresh();}
this._innerView.show(this.element);},_metadataReceived:function(errorCode,metadata)
{if(errorCode||!metadata)
return;if(this._content){if(!this._content.updateMetadata(metadata))
return;var sourceFrame=(this._innerView);this._content.requestContent(sourceFrame.setContent.bind(sourceFrame));}else{this._innerView.detach();this._content=new WebInspector.FileContentView.FileContentProvider(this._file,metadata);var sourceFrame=new WebInspector.SourceFrame(this._content);sourceFrame.setHighlighterType(this._file.resourceType.canonicalMimeType());this._innerView=sourceFrame;this._innerView.show(this.element);}},refresh:function()
{if(!this._innerView)
return;if(this._file.isTextFile)
this._file.requestMetadata(this._metadataReceived.bind(this));},__proto__:WebInspector.VBox.prototype}
WebInspector.FileContentView.FileContentProvider=function(file,metadata)
{this._file=file;this._metadata=metadata;}
WebInspector.FileContentView.FileContentProvider.prototype={contentURL:function()
{return this._file.url;},contentType:function()
{return this._file.resourceType;},requestContent:function(callback)
{var size=(this._metadata.size);this._file.requestFileContent(true,0,size,this._charset||"",this._fileContentReceived.bind(this,callback));},_fileContentReceived:function(callback,errorCode,content,base64Encoded,charset)
{if(errorCode||!content){callback(null);return;}
this._charset=charset;callback(content);},searchInContent:function(query,caseSensitive,isRegex,callback)
{setTimeout(callback.bind(null,[]),0);},updateMetadata:function(metadata)
{if(this._metadata.modificationTime>=metadata.modificationTime)
return false;this._metadata=metadata.modificationTime;return true;}};WebInspector.FileSystemModel=function(target)
{WebInspector.SDKObject.call(this,target);this._fileSystemsForOrigin={};target.resourceTreeModel.addEventListener(WebInspector.ResourceTreeModel.EventTypes.SecurityOriginAdded,this._securityOriginAdded,this);target.resourceTreeModel.addEventListener(WebInspector.ResourceTreeModel.EventTypes.SecurityOriginRemoved,this._securityOriginRemoved,this);this._agent=target.fileSystemAgent();this._agent.enable();this._reset();}
WebInspector.FileSystemModel.prototype={_reset:function()
{for(var securityOrigin in this._fileSystemsForOrigin)
this._removeOrigin(securityOrigin);var securityOrigins=this.target().resourceTreeModel.securityOrigins();for(var i=0;i<securityOrigins.length;++i)
this._addOrigin(securityOrigins[i]);},_securityOriginAdded:function(event)
{var securityOrigin=(event.data);this._addOrigin(securityOrigin);},_securityOriginRemoved:function(event)
{var securityOrigin=(event.data);this._removeOrigin(securityOrigin);},_addOrigin:function(securityOrigin)
{this._fileSystemsForOrigin[securityOrigin]={};var types=["persistent","temporary"];for(var i=0;i<types.length;++i)
this._requestFileSystemRoot(securityOrigin,types[i],this._fileSystemRootReceived.bind(this,securityOrigin,types[i],this._fileSystemsForOrigin[securityOrigin]));},_removeOrigin:function(securityOrigin)
{for(var type in this._fileSystemsForOrigin[securityOrigin]){var fileSystem=this._fileSystemsForOrigin[securityOrigin][type];delete this._fileSystemsForOrigin[securityOrigin][type];this._fileSystemRemoved(fileSystem);}
delete this._fileSystemsForOrigin[securityOrigin];},_requestFileSystemRoot:function(origin,type,callback)
{function innerCallback(error,errorCode,backendRootEntry)
{if(error){callback(FileError.SECURITY_ERR);return;}
callback(errorCode,backendRootEntry);}
this._agent.requestFileSystemRoot(origin,type,innerCallback);},_fileSystemAdded:function(fileSystem)
{this.dispatchEventToListeners(WebInspector.FileSystemModel.EventTypes.FileSystemAdded,fileSystem);},_fileSystemRemoved:function(fileSystem)
{this.dispatchEventToListeners(WebInspector.FileSystemModel.EventTypes.FileSystemRemoved,fileSystem);},refreshFileSystemList:function()
{this._reset();},_fileSystemRootReceived:function(origin,type,store,errorCode,backendRootEntry)
{if(!errorCode&&backendRootEntry&&this._fileSystemsForOrigin[origin]===store){var fileSystem=new WebInspector.FileSystemModel.FileSystem(this,origin,type,backendRootEntry);store[type]=fileSystem;this._fileSystemAdded(fileSystem);}},requestDirectoryContent:function(directory,callback)
{this._requestDirectoryContent(directory.url,this._directoryContentReceived.bind(this,directory,callback));},_requestDirectoryContent:function(url,callback)
{function innerCallback(error,errorCode,backendEntries)
{if(error){callback(FileError.SECURITY_ERR);return;}
if(errorCode!==0){callback(errorCode);return;}
callback(errorCode,backendEntries);}
this._agent.requestDirectoryContent(url,innerCallback);},_directoryContentReceived:function(parentDirectory,callback,errorCode,backendEntries)
{if(!backendEntries){callback(errorCode);return;}
var entries=[];for(var i=0;i<backendEntries.length;++i){if(backendEntries[i].isDirectory)
entries.push(new WebInspector.FileSystemModel.Directory(this,parentDirectory.fileSystem,backendEntries[i]));else
entries.push(new WebInspector.FileSystemModel.File(this,parentDirectory.fileSystem,backendEntries[i]));}
callback(errorCode,entries);},requestMetadata:function(entry,callback)
{function innerCallback(error,errorCode,metadata)
{if(error){callback(FileError.SECURITY_ERR);return;}
callback(errorCode,metadata);}
this._agent.requestMetadata(entry.url,innerCallback);},requestFileContent:function(file,readAsText,start,end,charset,callback)
{this._requestFileContent(file.url,readAsText,start,end,charset,callback);},_requestFileContent:function(url,readAsText,start,end,charset,callback)
{function innerCallback(error,errorCode,content,charset)
{if(error){if(callback)
callback(FileError.SECURITY_ERR);return;}
if(callback)
callback(errorCode,content,charset);}
this._agent.requestFileContent(url,readAsText,start,end,charset,innerCallback);},deleteEntry:function(entry,callback)
{var fileSystemModel=this;if(entry===entry.fileSystem.root)
this._deleteEntry(entry.url,hookFileSystemDeletion);else
this._deleteEntry(entry.url,callback);function hookFileSystemDeletion(errorCode)
{callback(errorCode);if(!errorCode)
fileSystemModel._removeFileSystem(entry.fileSystem);}},_deleteEntry:function(url,callback)
{function innerCallback(error,errorCode)
{if(error){if(callback)
callback(FileError.SECURITY_ERR);return;}
if(callback)
callback(errorCode);}
this._agent.deleteEntry(url,innerCallback);},_removeFileSystem:function(fileSystem)
{var origin=fileSystem.origin;var type=fileSystem.type;if(this._fileSystemsForOrigin[origin]&&this._fileSystemsForOrigin[origin][type]){delete this._fileSystemsForOrigin[origin][type];this._fileSystemRemoved(fileSystem);if(Object.isEmpty(this._fileSystemsForOrigin[origin]))
delete this._fileSystemsForOrigin[origin];}},__proto__:WebInspector.SDKObject.prototype}
WebInspector.FileSystemModel.EventTypes={FileSystemAdded:"FileSystemAdded",FileSystemRemoved:"FileSystemRemoved"}
WebInspector.FileSystemModel.FileSystem=function(fileSystemModel,origin,type,backendRootEntry)
{this.origin=origin;this.type=type;this.root=new WebInspector.FileSystemModel.Directory(fileSystemModel,this,backendRootEntry);}
WebInspector.FileSystemModel.FileSystem.prototype={get name()
{return"filesystem:"+this.origin+"/"+this.type;}}
WebInspector.FileSystemModel.Entry=function(fileSystemModel,fileSystem,backendEntry)
{this._fileSystemModel=fileSystemModel;this._fileSystem=fileSystem;this._url=backendEntry.url;this._name=backendEntry.name;this._isDirectory=backendEntry.isDirectory;}
WebInspector.FileSystemModel.Entry.compare=function(x,y)
{if(x.isDirectory!=y.isDirectory)
return y.isDirectory?1:-1;return x.name.compareTo(y.name);}
WebInspector.FileSystemModel.Entry.prototype={get fileSystemModel()
{return this._fileSystemModel;},get fileSystem()
{return this._fileSystem;},get url()
{return this._url;},get name()
{return this._name;},get isDirectory()
{return this._isDirectory;},requestMetadata:function(callback)
{this.fileSystemModel.requestMetadata(this,callback);},deleteEntry:function(callback)
{this.fileSystemModel.deleteEntry(this,callback);}}
WebInspector.FileSystemModel.Directory=function(fileSystemModel,fileSystem,backendEntry)
{WebInspector.FileSystemModel.Entry.call(this,fileSystemModel,fileSystem,backendEntry);}
WebInspector.FileSystemModel.Directory.prototype={requestDirectoryContent:function(callback)
{this.fileSystemModel.requestDirectoryContent(this,callback);},__proto__:WebInspector.FileSystemModel.Entry.prototype}
WebInspector.FileSystemModel.File=function(fileSystemModel,fileSystem,backendEntry)
{WebInspector.FileSystemModel.Entry.call(this,fileSystemModel,fileSystem,backendEntry);this._mimeType=backendEntry.mimeType;this._resourceType=WebInspector.resourceTypes[backendEntry.resourceType];this._isTextFile=backendEntry.isTextFile;}
WebInspector.FileSystemModel.File.prototype={get mimeType()
{return this._mimeType;},get resourceType()
{return this._resourceType;},get isTextFile()
{return this._isTextFile;},requestFileContent:function(readAsText,start,end,charset,callback)
{this.fileSystemModel.requestFileContent(this,readAsText,start,end,charset,callback);},__proto__:WebInspector.FileSystemModel.Entry.prototype};WebInspector.FileSystemView=function(fileSystem)
{WebInspector.SplitWidget.call(this,true,false,"fileSystemViewSplitViewState");this.element.classList.add("file-system-view","storage-view");var vbox=new WebInspector.VBox();vbox.element.classList.add("sidebar");this._directoryTree=new TreeOutline();this._directoryTree.element.classList.add("outline-disclosure","filesystem-directory-tree");vbox.element.appendChild(this._directoryTree.element);this.setSidebarWidget(vbox);var rootItem=new WebInspector.FileSystemView.EntryTreeElement(this,fileSystem.root);rootItem.expanded=true;this._directoryTree.appendChild(rootItem);this._visibleView=null;this._refreshButton=new WebInspector.ToolbarButton(WebInspector.UIString("Refresh"),"refresh-toolbar-item");this._refreshButton.setVisible(true);this._refreshButton.addEventListener("click",this._refresh,this);this._deleteButton=new WebInspector.ToolbarButton(WebInspector.UIString("Delete"),"delete-toolbar-item");this._deleteButton.setVisible(true);this._deleteButton.addEventListener("click",this._confirmDelete,this);}
WebInspector.FileSystemView.prototype={toolbarItems:function()
{return[this._refreshButton,this._deleteButton];},get visibleView()
{return this._visibleView;},showView:function(view)
{if(this._visibleView===view)
return;if(this._visibleView)
this._visibleView.detach();this._visibleView=view;this.setMainWidget(view);},_refresh:function()
{this._directoryTree.firstChild().refresh();},_confirmDelete:function()
{if(confirm(WebInspector.UIString("Are you sure you want to delete the selected entry?")))
this._delete();},_delete:function()
{this._directoryTree.selectedTreeElement.deleteEntry();},__proto__:WebInspector.SplitWidget.prototype}
WebInspector.FileSystemView.EntryTreeElement=function(fileSystemView,entry)
{TreeElement.call(this,entry.name,entry.isDirectory);this._entry=entry;this._fileSystemView=fileSystemView;}
WebInspector.FileSystemView.EntryTreeElement.prototype={onattach:function()
{var selection=this.listItemElement.createChild("div","selection fill");this.listItemElement.insertBefore(selection,this.listItemElement.firstChild);},onselect:function()
{if(!this._view){if(this._entry.isDirectory)
this._view=new WebInspector.DirectoryContentView();else{var file=(this._entry);this._view=new WebInspector.FileContentView(file);}}
this._fileSystemView.showView(this._view);this.refresh();return false;},onpopulate:function()
{this.refresh();},_directoryContentReceived:function(errorCode,entries)
{WebInspector.userMetrics.actionTaken(WebInspector.UserMetrics.Action.FileSystemDirectoryContentReceived);if(errorCode===FileError.NOT_FOUND_ERR){if(this.parent)
this.parent.refresh();return;}
if(errorCode!==0||!entries){console.error("Failed to read directory: "+errorCode);return;}
entries.sort(WebInspector.FileSystemModel.Entry.compare);if(this._view)
this._view.showEntries(entries);var oldChildren=this.children().slice(0);var newEntryIndex=0;var oldChildIndex=0;var currentTreeItem=0;while(newEntryIndex<entries.length&&oldChildIndex<oldChildren.length){var newEntry=entries[newEntryIndex];var oldChild=oldChildren[oldChildIndex];var order=newEntry.name.compareTo(oldChild._entry.name);if(order===0){if(oldChild._entry.isDirectory)
oldChild.invalidateChildren();else
oldChild.refresh();++newEntryIndex;++oldChildIndex;++currentTreeItem;continue;}
if(order<0){this.insertChild(new WebInspector.FileSystemView.EntryTreeElement(this._fileSystemView,newEntry),currentTreeItem);++newEntryIndex;++currentTreeItem;continue;}
this.removeChildAtIndex(currentTreeItem);++oldChildIndex;}
for(;newEntryIndex<entries.length;++newEntryIndex)
this.appendChild(new WebInspector.FileSystemView.EntryTreeElement(this._fileSystemView,entries[newEntryIndex]));for(;oldChildIndex<oldChildren.length;++oldChildIndex)
this.removeChild(oldChildren[oldChildIndex]);},refresh:function()
{if(!this._entry.isDirectory){if(this._view&&this._view===this._fileSystemView.visibleView){var fileContentView=(this._view);fileContentView.refresh();}}else
this._entry.requestDirectoryContent(this._directoryContentReceived.bind(this));},deleteEntry:function()
{this._entry.deleteEntry(this._deletionCompleted.bind(this));},_deletionCompleted:function()
{if(this._entry!=this._entry.fileSystem.root)
this.parent.refresh();},__proto__:TreeElement.prototype};WebInspector.ResourcesPanel=function()
{WebInspector.PanelWithSidebar.call(this,"resources");this.registerRequiredCSS("resources/resourcesPanel.css");this._resourcesLastSelectedItemSetting=WebInspector.settings.createSetting("resourcesLastSelectedItem",{});this._sidebarTree=new TreeOutline();this._sidebarTree.element.classList.add("filter-all","children","small","outline-disclosure");this.panelSidebarElement().appendChild(this._sidebarTree.element);this.setDefaultFocusedElement(this._sidebarTree.element);this.resourcesListTreeElement=new WebInspector.StorageCategoryTreeElement(this,WebInspector.UIString("Frames"),"Frames",["frame-storage-tree-item"]);this._sidebarTree.appendChild(this.resourcesListTreeElement);this.databasesListTreeElement=new WebInspector.StorageCategoryTreeElement(this,WebInspector.UIString("Web SQL"),"Databases",["database-storage-tree-item"]);this._sidebarTree.appendChild(this.databasesListTreeElement);this.indexedDBListTreeElement=new WebInspector.IndexedDBTreeElement(this);this._sidebarTree.appendChild(this.indexedDBListTreeElement);this.localStorageListTreeElement=new WebInspector.StorageCategoryTreeElement(this,WebInspector.UIString("Local Storage"),"LocalStorage",["domstorage-storage-tree-item","local-storage"]);this._sidebarTree.appendChild(this.localStorageListTreeElement);this.sessionStorageListTreeElement=new WebInspector.StorageCategoryTreeElement(this,WebInspector.UIString("Session Storage"),"SessionStorage",["domstorage-storage-tree-item","session-storage"]);this._sidebarTree.appendChild(this.sessionStorageListTreeElement);this.cookieListTreeElement=new WebInspector.StorageCategoryTreeElement(this,WebInspector.UIString("Cookies"),"Cookies",["cookie-storage-tree-item"]);this._sidebarTree.appendChild(this.cookieListTreeElement);this.applicationCacheListTreeElement=new WebInspector.StorageCategoryTreeElement(this,WebInspector.UIString("Application Cache"),"ApplicationCache",["application-cache-storage-tree-item"]);this._sidebarTree.appendChild(this.applicationCacheListTreeElement);this.cacheStorageListTreeElement=new WebInspector.ServiceWorkerCacheTreeElement(this);this._sidebarTree.appendChild(this.cacheStorageListTreeElement);if(Runtime.experiments.isEnabled("fileSystemInspection")){this.fileSystemListTreeElement=new WebInspector.FileSystemListTreeElement(this);this._sidebarTree.appendChild(this.fileSystemListTreeElement);}
var mainContainer=new WebInspector.VBox();this.storageViews=mainContainer.element.createChild("div","vbox flex-auto");this._storageViewToolbar=new WebInspector.Toolbar(mainContainer.element);this._storageViewToolbar.element.classList.add("resources-toolbar");this.splitWidget().setMainWidget(mainContainer);this._databaseTableViews=new Map();this._databaseQueryViews=new Map();this._databaseTreeElements=new Map();this._domStorageViews=new Map();this._domStorageTreeElements=new Map();this._cookieViews={};this._domains={};this.panelSidebarElement().addEventListener("mousemove",this._onmousemove.bind(this),false);this.panelSidebarElement().addEventListener("mouseleave",this._onmouseleave.bind(this),false);WebInspector.targetManager.observeTargets(this);}
WebInspector.ResourcesPanel.prototype={targetAdded:function(target)
{if(this._target)
return;this._target=target;if(target.serviceWorkerManager){this.serviceWorkersTreeElement=new WebInspector.ServiceWorkersTreeElement(this);this._sidebarTree.appendChild(this.serviceWorkersTreeElement);}
this._databaseModel=WebInspector.DatabaseModel.fromTarget(target);this._domStorageModel=WebInspector.DOMStorageModel.fromTarget(target);if(target.resourceTreeModel.cachedResourcesLoaded())
this._initialize();target.resourceTreeModel.addEventListener(WebInspector.ResourceTreeModel.EventTypes.Load,this._loadEventFired,this);target.resourceTreeModel.addEventListener(WebInspector.ResourceTreeModel.EventTypes.CachedResourcesLoaded,this._initialize,this);target.resourceTreeModel.addEventListener(WebInspector.ResourceTreeModel.EventTypes.WillLoadCachedResources,this._resetWithFrames,this);this._databaseModel.addEventListener(WebInspector.DatabaseModel.Events.DatabaseAdded,this._databaseAdded,this);},targetRemoved:function(target)
{if(target!==this._target)
return;delete this._target;target.resourceTreeModel.removeEventListener(WebInspector.ResourceTreeModel.EventTypes.Load,this._loadEventFired,this);target.resourceTreeModel.removeEventListener(WebInspector.ResourceTreeModel.EventTypes.CachedResourcesLoaded,this._initialize,this);target.resourceTreeModel.removeEventListener(WebInspector.ResourceTreeModel.EventTypes.WillLoadCachedResources,this._resetWithFrames,this);this._databaseModel.removeEventListener(WebInspector.DatabaseModel.Events.DatabaseAdded,this._databaseAdded,this);this._resetWithFrames();},_initialize:function()
{this._databaseModel.enable();this._domStorageModel.enable();var indexedDBModel=WebInspector.IndexedDBModel.fromTarget(this._target);if(indexedDBModel)
indexedDBModel.enable();var cacheStorageModel=WebInspector.ServiceWorkerCacheModel.fromTarget(this._target);if(cacheStorageModel)
cacheStorageModel.enable();if(this._target.isPage())
this._populateResourceTree();this._populateDOMStorageTree();this._populateApplicationCacheTree();this.indexedDBListTreeElement._initialize();this.cacheStorageListTreeElement._initialize();if(Runtime.experiments.isEnabled("fileSystemInspection"))
this.fileSystemListTreeElement._initialize();this._initDefaultSelection();this._initialized=true;},_loadEventFired:function()
{this._initDefaultSelection();},_initDefaultSelection:function()
{if(!this._initialized)
return;var itemURL=this._resourcesLastSelectedItemSetting.get();if(itemURL){var rootElement=this._sidebarTree.rootElement();for(var treeElement=rootElement.firstChild();treeElement;treeElement=treeElement.traverseNextTreeElement(false,rootElement,true)){if(treeElement.itemURL===itemURL){treeElement.revealAndSelect(true);return;}}}
var mainResource=this._target.resourceTreeModel.inspectedPageURL()&&this.resourcesListTreeElement&&this.resourcesListTreeElement.expanded?this._target.resourceTreeModel.resourceForURL(this._target.resourceTreeModel.inspectedPageURL()):null;if(mainResource)
this.showResource(mainResource);},_resetWithFrames:function()
{this.resourcesListTreeElement.removeChildren();this._treeElementForFrameId={};this._reset();},_reset:function()
{this._domains={};var queryViews=this._databaseQueryViews.valuesArray();for(var i=0;i<queryViews.length;++i)
queryViews[i].removeEventListener(WebInspector.DatabaseQueryView.Events.SchemaUpdated,this._updateDatabaseTables,this);this._databaseTableViews.clear();this._databaseQueryViews.clear();this._databaseTreeElements.clear();this._domStorageViews.clear();this._domStorageTreeElements.clear();this._cookieViews={};this.databasesListTreeElement.removeChildren();this.localStorageListTreeElement.removeChildren();this.sessionStorageListTreeElement.removeChildren();this.cookieListTreeElement.removeChildren();this.cacheStorageListTreeElement.removeChildren();if(this.visibleView&&!(this.visibleView instanceof WebInspector.StorageCategoryView))
this.visibleView.detach();this._storageViewToolbar.removeToolbarItems();if(this._sidebarTree.selectedTreeElement)
this._sidebarTree.selectedTreeElement.deselect();},_populateResourceTree:function()
{this._treeElementForFrameId={};this._target.resourceTreeModel.addEventListener(WebInspector.ResourceTreeModel.EventTypes.FrameAdded,this._frameAdded,this);this._target.resourceTreeModel.addEventListener(WebInspector.ResourceTreeModel.EventTypes.FrameNavigated,this._frameNavigated,this);this._target.resourceTreeModel.addEventListener(WebInspector.ResourceTreeModel.EventTypes.FrameDetached,this._frameDetached,this);this._target.resourceTreeModel.addEventListener(WebInspector.ResourceTreeModel.EventTypes.ResourceAdded,this._resourceAdded,this);function populateFrame(frame)
{this._frameAdded({data:frame});for(var i=0;i<frame.childFrames.length;++i)
populateFrame.call(this,frame.childFrames[i]);var resources=frame.resources();for(var i=0;i<resources.length;++i)
this._resourceAdded({data:resources[i]});}
populateFrame.call(this,this._target.resourceTreeModel.mainFrame);},_frameAdded:function(event)
{var frame=event.data;var parentFrame=frame.parentFrame;var parentTreeElement=parentFrame?this._treeElementForFrameId[parentFrame.id]:this.resourcesListTreeElement;if(!parentTreeElement){console.warn("No frame to route "+frame.url+" to.");return;}
var frameTreeElement=new WebInspector.FrameTreeElement(this,frame);this._treeElementForFrameId[frame.id]=frameTreeElement;parentTreeElement.appendChild(frameTreeElement);},_frameDetached:function(event)
{var frame=event.data;var frameTreeElement=this._treeElementForFrameId[frame.id];if(!frameTreeElement)
return;delete this._treeElementForFrameId[frame.id];if(frameTreeElement.parent)
frameTreeElement.parent.removeChild(frameTreeElement);},_resourceAdded:function(event)
{var resource=event.data;var frameId=resource.frameId;if(resource.statusCode>=301&&resource.statusCode<=303)
return;var frameTreeElement=this._treeElementForFrameId[frameId];if(!frameTreeElement){return;}
frameTreeElement.appendResource(resource);},_frameNavigated:function(event)
{var frame=event.data;if(!frame.parentFrame)
this._reset();var frameId=frame.id;var frameTreeElement=this._treeElementForFrameId[frameId];if(frameTreeElement)
frameTreeElement.frameNavigated(frame);var applicationCacheFrameTreeElement=this._applicationCacheFrameElements[frameId];if(applicationCacheFrameTreeElement)
applicationCacheFrameTreeElement.frameNavigated(frame);},_databaseAdded:function(event)
{var database=(event.data);this._addDatabase(database);},_addDatabase:function(database)
{var databaseTreeElement=new WebInspector.DatabaseTreeElement(this,database);this._databaseTreeElements.set(database,databaseTreeElement);this.databasesListTreeElement.appendChild(databaseTreeElement);},addDocumentURL:function(url)
{var parsedURL=url.asParsedURL();if(!parsedURL)
return;var domain=parsedURL.host;if(!this._domains[domain]){this._domains[domain]=true;var cookieDomainTreeElement=new WebInspector.CookieTreeElement(this,domain);this.cookieListTreeElement.appendChild(cookieDomainTreeElement);}},_domStorageAdded:function(event)
{var domStorage=(event.data);this._addDOMStorage(domStorage);},_addDOMStorage:function(domStorage)
{console.assert(!this._domStorageTreeElements.get(domStorage));var domStorageTreeElement=new WebInspector.DOMStorageTreeElement(this,domStorage,(domStorage.isLocalStorage?"local-storage":"session-storage"));this._domStorageTreeElements.set(domStorage,domStorageTreeElement);if(domStorage.isLocalStorage)
this.localStorageListTreeElement.appendChild(domStorageTreeElement);else
this.sessionStorageListTreeElement.appendChild(domStorageTreeElement);},_domStorageRemoved:function(event)
{var domStorage=(event.data);this._removeDOMStorage(domStorage);},_removeDOMStorage:function(domStorage)
{var treeElement=this._domStorageTreeElements.get(domStorage);if(!treeElement)
return;var wasSelected=treeElement.selected;var parentListTreeElement=treeElement.parent;parentListTreeElement.removeChild(treeElement);if(wasSelected)
parentListTreeElement.select();this._domStorageTreeElements.remove(domStorage);this._domStorageViews.remove(domStorage);},selectDatabase:function(database)
{if(database){this._showDatabase(database);this._databaseTreeElements.get(database).select();}},selectDOMStorage:function(domStorage)
{if(domStorage){this._showDOMStorage(domStorage);this._domStorageTreeElements.get(domStorage).select();}},showResource:function(resource,line,column)
{var resourceTreeElement=this._findTreeElementForResource(resource);if(resourceTreeElement)
resourceTreeElement.revealAndSelect(true);if(typeof line==="number"){var resourceSourceFrame=this._resourceSourceFrameViewForResource(resource);if(resourceSourceFrame)
resourceSourceFrame.revealPosition(line,column,true);}
return true;},_showResourceView:function(resource)
{var view=this._resourceViewForResource(resource);if(!view){this.visibleView.detach();return;}
this._innerShowView(view);},_resourceViewForResource:function(resource)
{if(resource.hasTextContent()){var treeElement=this._findTreeElementForResource(resource);if(!treeElement)
return null;return treeElement.sourceView();}
switch(resource.resourceType()){case WebInspector.resourceTypes.Image:return new WebInspector.ImageView(resource.url,resource.mimeType,resource);case WebInspector.resourceTypes.Font:return new WebInspector.FontView(resource.url,resource.mimeType,resource);default:return new WebInspector.EmptyWidget(resource.url);}},_resourceSourceFrameViewForResource:function(resource)
{var resourceView=this._resourceViewForResource(resource);if(resourceView&&resourceView instanceof WebInspector.ResourceSourceFrame)
return(resourceView);return null;},_showDatabase:function(database,tableName)
{if(!database)
return;var view;if(tableName){var tableViews=this._databaseTableViews.get(database);if(!tableViews){tableViews=({});this._databaseTableViews.set(database,tableViews);}
view=tableViews[tableName];if(!view){view=new WebInspector.DatabaseTableView(database,tableName);tableViews[tableName]=view;}}else{view=this._databaseQueryViews.get(database);if(!view){view=new WebInspector.DatabaseQueryView(database);this._databaseQueryViews.set(database,view);view.addEventListener(WebInspector.DatabaseQueryView.Events.SchemaUpdated,this._updateDatabaseTables,this);}}
this._innerShowView(view);},showIndexedDB:function(view)
{this._innerShowView(view);},showServiceWorkerCache:function(view)
{this._innerShowView(view);},showServiceWorkersView:function(view)
{this._innerShowView(view);},_showDOMStorage:function(domStorage)
{if(!domStorage)
return;var view;view=this._domStorageViews.get(domStorage);if(!view){view=new WebInspector.DOMStorageItemsView(domStorage);this._domStorageViews.set(domStorage,view);}
this._innerShowView(view);},showCookies:function(treeElement,cookieDomain)
{var view=this._cookieViews[cookieDomain];if(!view){view=new WebInspector.CookieItemsView(treeElement,cookieDomain);this._cookieViews[cookieDomain]=view;}
this._innerShowView(view);},clearCookies:function(cookieDomain)
{this._cookieViews[cookieDomain].clear();},showApplicationCache:function(frameId)
{if(!this._applicationCacheViews[frameId])
this._applicationCacheViews[frameId]=new WebInspector.ApplicationCacheItemsView(this._applicationCacheModel,frameId);this._innerShowView(this._applicationCacheViews[frameId]);},showFileSystem:function(view)
{this._innerShowView(view);},showCategoryView:function(categoryName)
{if(!this._categoryView)
this._categoryView=new WebInspector.StorageCategoryView();this._categoryView.setText(categoryName);this._innerShowView(this._categoryView);},_innerShowView:function(view)
{if(this.visibleView===view)
return;if(this.visibleView)
this.visibleView.detach();view.show(this.storageViews);this.visibleView=view;this._storageViewToolbar.removeToolbarItems();var toolbarItems=view.toolbarItems?view.toolbarItems():null;for(var i=0;toolbarItems&&i<toolbarItems.length;++i)
this._storageViewToolbar.appendToolbarItem(toolbarItems[i]);},closeVisibleView:function()
{if(!this.visibleView)
return;this.visibleView.detach();delete this.visibleView;},_updateDatabaseTables:function(event)
{var database=event.data;if(!database)
return;var databasesTreeElement=this._databaseTreeElements.get(database);if(!databasesTreeElement)
return;databasesTreeElement.invalidateChildren();var tableViews=this._databaseTableViews.get(database);if(!tableViews)
return;var tableNamesHash={};var self=this;function tableNamesCallback(tableNames)
{var tableNamesLength=tableNames.length;for(var i=0;i<tableNamesLength;++i)
tableNamesHash[tableNames[i]]=true;for(var tableName in tableViews){if(!(tableName in tableNamesHash)){if(self.visibleView===tableViews[tableName])
self.closeVisibleView();delete tableViews[tableName];}}}
database.getTableNames(tableNamesCallback);},_populateDOMStorageTree:function()
{this._domStorageModel.storages().forEach(this._addDOMStorage.bind(this));this._domStorageModel.addEventListener(WebInspector.DOMStorageModel.Events.DOMStorageAdded,this._domStorageAdded,this);this._domStorageModel.addEventListener(WebInspector.DOMStorageModel.Events.DOMStorageRemoved,this._domStorageRemoved,this);},_populateApplicationCacheTree:function()
{this._applicationCacheModel=new WebInspector.ApplicationCacheModel(this._target);this._applicationCacheViews={};this._applicationCacheFrameElements={};this._applicationCacheManifestElements={};this._applicationCacheModel.addEventListener(WebInspector.ApplicationCacheModel.EventTypes.FrameManifestAdded,this._applicationCacheFrameManifestAdded,this);this._applicationCacheModel.addEventListener(WebInspector.ApplicationCacheModel.EventTypes.FrameManifestRemoved,this._applicationCacheFrameManifestRemoved,this);this._applicationCacheModel.addEventListener(WebInspector.ApplicationCacheModel.EventTypes.FrameManifestStatusUpdated,this._applicationCacheFrameManifestStatusChanged,this);this._applicationCacheModel.addEventListener(WebInspector.ApplicationCacheModel.EventTypes.NetworkStateChanged,this._applicationCacheNetworkStateChanged,this);},_applicationCacheFrameManifestAdded:function(event)
{var frameId=event.data;var manifestURL=this._applicationCacheModel.frameManifestURL(frameId);var manifestTreeElement=this._applicationCacheManifestElements[manifestURL];if(!manifestTreeElement){manifestTreeElement=new WebInspector.ApplicationCacheManifestTreeElement(this,manifestURL);this.applicationCacheListTreeElement.appendChild(manifestTreeElement);this._applicationCacheManifestElements[manifestURL]=manifestTreeElement;}
var frameTreeElement=new WebInspector.ApplicationCacheFrameTreeElement(this,frameId,manifestURL);manifestTreeElement.appendChild(frameTreeElement);manifestTreeElement.expand();this._applicationCacheFrameElements[frameId]=frameTreeElement;},_applicationCacheFrameManifestRemoved:function(event)
{var frameId=event.data;var frameTreeElement=this._applicationCacheFrameElements[frameId];if(!frameTreeElement)
return;var manifestURL=frameTreeElement.manifestURL;delete this._applicationCacheFrameElements[frameId];delete this._applicationCacheViews[frameId];frameTreeElement.parent.removeChild(frameTreeElement);var manifestTreeElement=this._applicationCacheManifestElements[manifestURL];if(manifestTreeElement.childCount())
return;delete this._applicationCacheManifestElements[manifestURL];manifestTreeElement.parent.removeChild(manifestTreeElement);},_applicationCacheFrameManifestStatusChanged:function(event)
{var frameId=event.data;var status=this._applicationCacheModel.frameManifestStatus(frameId);if(this._applicationCacheViews[frameId])
this._applicationCacheViews[frameId].updateStatus(status);},_applicationCacheNetworkStateChanged:function(event)
{var isNowOnline=event.data;for(var manifestURL in this._applicationCacheViews)
this._applicationCacheViews[manifestURL].updateNetworkState(isNowOnline);},_findTreeElementForResource:function(resource)
{return resource[WebInspector.FrameResourceTreeElement._symbol];},showView:function(view)
{if(view)
this.showResource(view.resource);},_onmousemove:function(event)
{var nodeUnderMouse=event.target;if(!nodeUnderMouse)
return;var listNode=nodeUnderMouse.enclosingNodeOrSelfWithNodeName("li");if(!listNode)
return;var element=listNode.treeElement;if(this._previousHoveredElement===element)
return;if(this._previousHoveredElement){this._previousHoveredElement.hovered=false;delete this._previousHoveredElement;}
if(element instanceof WebInspector.FrameTreeElement){this._previousHoveredElement=element;element.hovered=true;}},_onmouseleave:function(event)
{if(this._previousHoveredElement){this._previousHoveredElement.hovered=false;delete this._previousHoveredElement;}},__proto__:WebInspector.PanelWithSidebar.prototype}
WebInspector.ResourcesPanel.ResourceRevealer=function()
{}
WebInspector.ResourcesPanel.ResourceRevealer.prototype={reveal:function(resource,lineNumber)
{if(!(resource instanceof WebInspector.Resource))
return Promise.reject(new Error("Internal error: not a resource"));var panel=WebInspector.ResourcesPanel._instance();WebInspector.inspectorView.setCurrentPanel(panel);panel.showResource(resource,lineNumber);return Promise.resolve();}}
WebInspector.BaseStorageTreeElement=function(storagePanel,title,iconClasses,expandable,noIcon)
{TreeElement.call(this,"",expandable);this._storagePanel=storagePanel;this._titleText=title;this._iconClasses=iconClasses;this._noIcon=noIcon;}
WebInspector.BaseStorageTreeElement.prototype={onattach:function()
{this.listItemElement.removeChildren();if(this._iconClasses){for(var i=0;i<this._iconClasses.length;++i)
this.listItemElement.classList.add(this._iconClasses[i]);}
this.listItemElement.createChild("div","selection fill");if(!this._noIcon)
this.imageElement=this.listItemElement.createChild("img","icon");this.titleElement=this.listItemElement.createChild("div","base-storage-tree-element-title");this._titleTextNode=this.titleElement.createTextChild("");this._updateTitle();this._updateSubtitle();},get displayName()
{return this._displayName;},_updateDisplayName:function()
{this._displayName=this._titleText||"";if(this._subtitleText)
this._displayName+=" ("+this._subtitleText+")";},_updateTitle:function()
{this._updateDisplayName();if(!this.titleElement)
return;this._titleTextNode.textContent=this._titleText||"";},_updateSubtitle:function()
{this._updateDisplayName();if(!this.titleElement)
return;if(this._subtitleText){if(!this._subtitleElement)
this._subtitleElement=this.titleElement.createChild("span","base-storage-tree-element-subtitle");this._subtitleElement.textContent="("+this._subtitleText+")";}else if(this._subtitleElement){this._subtitleElement.remove();delete this._subtitleElement;}},onselect:function(selectedByUser)
{if(!selectedByUser)
return false;var itemURL=this.itemURL;if(itemURL)
this._storagePanel._resourcesLastSelectedItemSetting.set(itemURL);return false;},onreveal:function()
{if(this.listItemElement)
this.listItemElement.scrollIntoViewIfNeeded(false);},get titleText()
{return this._titleText;},set titleText(titleText)
{this._titleText=titleText;this._updateTitle();},get subtitleText()
{return this._subtitleText;},set subtitleText(subtitleText)
{this._subtitleText=subtitleText;this._updateSubtitle();},__proto__:TreeElement.prototype}
WebInspector.StorageCategoryTreeElement=function(storagePanel,categoryName,settingsKey,iconClasses,noIcon)
{WebInspector.BaseStorageTreeElement.call(this,storagePanel,categoryName,iconClasses,false,noIcon);this._expandedSetting=WebInspector.settings.createSetting("resources"+settingsKey+"Expanded",settingsKey==="Frames");this._categoryName=categoryName;}
WebInspector.StorageCategoryTreeElement.prototype={target:function()
{return this._storagePanel._target;},get itemURL()
{return"category://"+this._categoryName;},onselect:function(selectedByUser)
{WebInspector.BaseStorageTreeElement.prototype.onselect.call(this,selectedByUser);this._storagePanel.showCategoryView(this._categoryName);return false;},onattach:function()
{WebInspector.BaseStorageTreeElement.prototype.onattach.call(this);if(this._expandedSetting.get())
this.expand();},onexpand:function()
{this._expandedSetting.set(true);},oncollapse:function()
{this._expandedSetting.set(false);},__proto__:WebInspector.BaseStorageTreeElement.prototype}
WebInspector.FrameTreeElement=function(storagePanel,frame)
{WebInspector.BaseStorageTreeElement.call(this,storagePanel,"",["frame-storage-tree-item"]);this._frame=frame;this.frameNavigated(frame);}
WebInspector.FrameTreeElement.prototype={frameNavigated:function(frame)
{this.removeChildren();this._frameId=frame.id;this.titleText=frame.name;this.subtitleText=new WebInspector.ParsedURL(frame.url).displayName;this._categoryElements={};this._treeElementForResource={};this._storagePanel.addDocumentURL(frame.url);},get itemURL()
{return"frame://"+encodeURI(this.displayName);},onselect:function(selectedByUser)
{WebInspector.BaseStorageTreeElement.prototype.onselect.call(this,selectedByUser);this._storagePanel.showCategoryView(this.displayName);this.listItemElement.classList.remove("hovered");WebInspector.DOMModel.hideDOMNodeHighlight();return false;},set hovered(hovered)
{if(hovered){this.listItemElement.classList.add("hovered");var domModel=WebInspector.DOMModel.fromTarget(this._frame.target());if(domModel)
domModel.highlightFrame(this._frameId);}else{this.listItemElement.classList.remove("hovered");WebInspector.DOMModel.hideDOMNodeHighlight();}},appendResource:function(resource)
{if(resource.isHidden())
return;var resourceType=resource.resourceType();var categoryName=resourceType.name();var categoryElement=resourceType===WebInspector.resourceTypes.Document?this:this._categoryElements[categoryName];if(!categoryElement){categoryElement=new WebInspector.StorageCategoryTreeElement(this._storagePanel,resource.resourceType().category().title,categoryName,null,true);this._categoryElements[resourceType.name()]=categoryElement;this._insertInPresentationOrder(this,categoryElement);}
var resourceTreeElement=new WebInspector.FrameResourceTreeElement(this._storagePanel,resource);this._insertInPresentationOrder(categoryElement,resourceTreeElement);this._treeElementForResource[resource.url]=resourceTreeElement;},resourceByURL:function(url)
{var treeElement=this._treeElementForResource[url];return treeElement?treeElement._resource:null;},appendChild:function(treeElement)
{this._insertInPresentationOrder(this,treeElement);},_insertInPresentationOrder:function(parentTreeElement,childTreeElement)
{function typeWeight(treeElement)
{if(treeElement instanceof WebInspector.StorageCategoryTreeElement)
return 2;if(treeElement instanceof WebInspector.FrameTreeElement)
return 1;return 3;}
function compare(treeElement1,treeElement2)
{var typeWeight1=typeWeight(treeElement1);var typeWeight2=typeWeight(treeElement2);var result;if(typeWeight1>typeWeight2)
result=1;else if(typeWeight1<typeWeight2)
result=-1;else{var title1=treeElement1.displayName||treeElement1.titleText;var title2=treeElement2.displayName||treeElement2.titleText;result=title1.localeCompare(title2);}
return result;}
var childCount=parentTreeElement.childCount();var i;for(i=0;i<childCount;++i){if(compare(childTreeElement,parentTreeElement.childAt(i))<0)
break;}
parentTreeElement.insertChild(childTreeElement,i);},__proto__:WebInspector.BaseStorageTreeElement.prototype}
WebInspector.FrameResourceTreeElement=function(storagePanel,resource)
{WebInspector.BaseStorageTreeElement.call(this,storagePanel,resource.displayName,["resource-sidebar-tree-item","resources-type-"+resource.resourceType().name()]);this._resource=resource;this.tooltip=resource.url;this._resource[WebInspector.FrameResourceTreeElement._symbol]=this;}
WebInspector.FrameResourceTreeElement._symbol=Symbol("treeElement");WebInspector.FrameResourceTreeElement.prototype={get itemURL()
{return this._resource.url;},onselect:function(selectedByUser)
{WebInspector.BaseStorageTreeElement.prototype.onselect.call(this,selectedByUser);this._storagePanel._showResourceView(this._resource);return false;},ondblclick:function(event)
{InspectorFrontendHost.openInNewTab(this._resource.url);return false;},onattach:function()
{WebInspector.BaseStorageTreeElement.prototype.onattach.call(this);if(this._resource.resourceType()===WebInspector.resourceTypes.Image){var iconElement=createElementWithClass("div","icon");var previewImage=iconElement.createChild("img","image-resource-icon-preview");this._resource.populateImageSource(previewImage);this.listItemElement.replaceChild(iconElement,this.imageElement);}
this._statusElement=createElementWithClass("div","status");this.listItemElement.insertBefore(this._statusElement,this.titleElement);this.listItemElement.draggable=true;this.listItemElement.addEventListener("dragstart",this._ondragstart.bind(this),false);this.listItemElement.addEventListener("contextmenu",this._handleContextMenuEvent.bind(this),true);},_ondragstart:function(event)
{event.dataTransfer.setData("text/plain",this._resource.content||"");event.dataTransfer.effectAllowed="copy";return true;},_handleContextMenuEvent:function(event)
{var contextMenu=new WebInspector.ContextMenu(event);contextMenu.appendApplicableItems(this._resource);contextMenu.show();},sourceView:function()
{if(!this._sourceView){var sourceFrame=new WebInspector.ResourceSourceFrame(this._resource);sourceFrame.setHighlighterType(this._resource.canonicalMimeType());this._sourceView=sourceFrame;}
return this._sourceView;},__proto__:WebInspector.BaseStorageTreeElement.prototype}
WebInspector.DatabaseTreeElement=function(storagePanel,database)
{WebInspector.BaseStorageTreeElement.call(this,storagePanel,database.name,["database-storage-tree-item"],true);this._database=database;}
WebInspector.DatabaseTreeElement.prototype={get itemURL()
{return"database://"+encodeURI(this._database.name);},onselect:function(selectedByUser)
{WebInspector.BaseStorageTreeElement.prototype.onselect.call(this,selectedByUser);this._storagePanel._showDatabase(this._database);return false;},onexpand:function()
{this._updateChildren();},_updateChildren:function()
{this.removeChildren();function tableNamesCallback(tableNames)
{var tableNamesLength=tableNames.length;for(var i=0;i<tableNamesLength;++i)
this.appendChild(new WebInspector.DatabaseTableTreeElement(this._storagePanel,this._database,tableNames[i]));}
this._database.getTableNames(tableNamesCallback.bind(this));},__proto__:WebInspector.BaseStorageTreeElement.prototype}
WebInspector.DatabaseTableTreeElement=function(storagePanel,database,tableName)
{WebInspector.BaseStorageTreeElement.call(this,storagePanel,tableName,["database-table-storage-tree-item"]);this._database=database;this._tableName=tableName;}
WebInspector.DatabaseTableTreeElement.prototype={get itemURL()
{return"database://"+encodeURI(this._database.name)+"/"+encodeURI(this._tableName);},onselect:function(selectedByUser)
{WebInspector.BaseStorageTreeElement.prototype.onselect.call(this,selectedByUser);this._storagePanel._showDatabase(this._database,this._tableName);return false;},__proto__:WebInspector.BaseStorageTreeElement.prototype}
WebInspector.ServiceWorkerCacheTreeElement=function(storagePanel)
{WebInspector.StorageCategoryTreeElement.call(this,storagePanel,WebInspector.UIString("Cache Storage"),"CacheStorage",["service-worker-cache-storage-tree-item"]);}
WebInspector.ServiceWorkerCacheTreeElement.prototype={_initialize:function()
{this._swCacheTreeElements=[];var target=this._storagePanel._target;if(target){var model=WebInspector.ServiceWorkerCacheModel.fromTarget(target);var caches=model.caches();for(var cache of caches)
this._addCache(model,cache);}
WebInspector.targetManager.addModelListener(WebInspector.ServiceWorkerCacheModel,WebInspector.ServiceWorkerCacheModel.EventTypes.CacheAdded,this._cacheAdded,this);WebInspector.targetManager.addModelListener(WebInspector.ServiceWorkerCacheModel,WebInspector.ServiceWorkerCacheModel.EventTypes.CacheRemoved,this._cacheRemoved,this);},onattach:function()
{WebInspector.StorageCategoryTreeElement.prototype.onattach.call(this);this.listItemElement.addEventListener("contextmenu",this._handleContextMenuEvent.bind(this),true);},_handleContextMenuEvent:function(event)
{var contextMenu=new WebInspector.ContextMenu(event);contextMenu.appendItem(WebInspector.UIString("Refresh Caches"),this._refreshCaches.bind(this));contextMenu.show();},_refreshCaches:function()
{var target=this._storagePanel._target;if(target){var model=WebInspector.ServiceWorkerCacheModel.fromTarget(target);model.refreshCacheNames();}},_cacheAdded:function(event)
{var cache=(event.data);var model=(event.target);this._addCache(model,cache);},_addCache:function(model,cache)
{var swCacheTreeElement=new WebInspector.SWCacheTreeElement(this._storagePanel,model,cache);this._swCacheTreeElements.push(swCacheTreeElement);this.appendChild(swCacheTreeElement);},_cacheRemoved:function(event)
{var cache=(event.data);var model=(event.target);var swCacheTreeElement=this._cacheTreeElement(model,cache);if(!swCacheTreeElement)
return;swCacheTreeElement.clear();this.removeChild(swCacheTreeElement);this._swCacheTreeElements.remove(swCacheTreeElement);},_cacheTreeElement:function(model,cache)
{var index=-1;for(var i=0;i<this._swCacheTreeElements.length;++i){if(this._swCacheTreeElements[i]._cache.equals(cache)&&this._swCacheTreeElements[i]._model===model){index=i;break;}}
if(index!==-1)
return this._swCacheTreeElements[i];return null;},__proto__:WebInspector.StorageCategoryTreeElement.prototype}
WebInspector.SWCacheTreeElement=function(storagePanel,model,cache)
{WebInspector.BaseStorageTreeElement.call(this,storagePanel,cache.cacheName+" - "+cache.securityOrigin,["service-worker-cache-tree-item"]);this._model=model;this._cache=cache;}
WebInspector.SWCacheTreeElement.prototype={get itemURL()
{return"cache://"+this._cache.cacheId;},onattach:function()
{WebInspector.BaseStorageTreeElement.prototype.onattach.call(this);this.listItemElement.addEventListener("contextmenu",this._handleContextMenuEvent.bind(this),true);},_handleContextMenuEvent:function(event)
{var contextMenu=new WebInspector.ContextMenu(event);contextMenu.appendItem(WebInspector.UIString("Delete"),this._clearCache.bind(this));contextMenu.show();},_clearCache:function()
{this._model.deleteCache(this._cache);},update:function(cache)
{this._cache=cache;if(this._view)
this._view.update(cache);},onselect:function(selectedByUser)
{WebInspector.BaseStorageTreeElement.prototype.onselect.call(this,selectedByUser);if(!this._view)
this._view=new WebInspector.ServiceWorkerCacheView(this._model,this._cache);this._storagePanel.showServiceWorkerCache(this._view);return false;},clear:function()
{if(this._view)
this._view.clear();},__proto__:WebInspector.BaseStorageTreeElement.prototype}
WebInspector.ServiceWorkersTreeElement=function(storagePanel)
{WebInspector.StorageCategoryTreeElement.call(this,storagePanel,WebInspector.UIString("Service Workers"),"Service Workers",["service-workers-tree-item"]);}
WebInspector.ServiceWorkersTreeElement.prototype={onselect:function(selectedByUser)
{WebInspector.StorageCategoryTreeElement.prototype.onselect.call(this,selectedByUser);if(!this._view)
this._view=new WebInspector.ServiceWorkersView();this._storagePanel.showServiceWorkersView(this._view);return false;},__proto__:WebInspector.StorageCategoryTreeElement.prototype}
WebInspector.IndexedDBTreeElement=function(storagePanel)
{WebInspector.StorageCategoryTreeElement.call(this,storagePanel,WebInspector.UIString("IndexedDB"),"IndexedDB",["indexed-db-storage-tree-item"]);}
WebInspector.IndexedDBTreeElement.prototype={_initialize:function()
{WebInspector.targetManager.addModelListener(WebInspector.IndexedDBModel,WebInspector.IndexedDBModel.EventTypes.DatabaseAdded,this._indexedDBAdded,this);WebInspector.targetManager.addModelListener(WebInspector.IndexedDBModel,WebInspector.IndexedDBModel.EventTypes.DatabaseRemoved,this._indexedDBRemoved,this);WebInspector.targetManager.addModelListener(WebInspector.IndexedDBModel,WebInspector.IndexedDBModel.EventTypes.DatabaseLoaded,this._indexedDBLoaded,this);this._idbDatabaseTreeElements=[];var targets=WebInspector.targetManager.targets();for(var i=0;i<targets.length;++i){var indexedDBModel=WebInspector.IndexedDBModel.fromTarget(targets[i]);var databases=indexedDBModel.databases();for(var j=0;j<databases.length;++j)
this._addIndexedDB(indexedDBModel,databases[j]);}},onattach:function()
{WebInspector.StorageCategoryTreeElement.prototype.onattach.call(this);this.listItemElement.addEventListener("contextmenu",this._handleContextMenuEvent.bind(this),true);},_handleContextMenuEvent:function(event)
{var contextMenu=new WebInspector.ContextMenu(event);contextMenu.appendItem(WebInspector.UIString("Refresh IndexedDB"),this.refreshIndexedDB.bind(this));contextMenu.show();},refreshIndexedDB:function()
{var targets=WebInspector.targetManager.targets();for(var i=0;i<targets.length;++i)
WebInspector.IndexedDBModel.fromTarget(targets[i]).refreshDatabaseNames();},_indexedDBAdded:function(event)
{var databaseId=(event.data);var model=(event.target);this._addIndexedDB(model,databaseId);},_addIndexedDB:function(model,databaseId)
{var idbDatabaseTreeElement=new WebInspector.IDBDatabaseTreeElement(this._storagePanel,model,databaseId);this._idbDatabaseTreeElements.push(idbDatabaseTreeElement);this.appendChild(idbDatabaseTreeElement);model.refreshDatabase(databaseId);},_indexedDBRemoved:function(event)
{var databaseId=(event.data);var model=(event.target);var idbDatabaseTreeElement=this._idbDatabaseTreeElement(model,databaseId)
if(!idbDatabaseTreeElement)
return;idbDatabaseTreeElement.clear();this.removeChild(idbDatabaseTreeElement);this._idbDatabaseTreeElements.remove(idbDatabaseTreeElement);},_indexedDBLoaded:function(event)
{var database=(event.data);var model=(event.target);var idbDatabaseTreeElement=this._idbDatabaseTreeElement(model,database.databaseId);if(!idbDatabaseTreeElement)
return;idbDatabaseTreeElement.update(database);},_idbDatabaseTreeElement:function(model,databaseId)
{var index=-1;for(var i=0;i<this._idbDatabaseTreeElements.length;++i){if(this._idbDatabaseTreeElements[i]._databaseId.equals(databaseId)&&this._idbDatabaseTreeElements[i]._model===model){index=i;break;}}
if(index!==-1)
return this._idbDatabaseTreeElements[i];return null;},__proto__:WebInspector.StorageCategoryTreeElement.prototype}
WebInspector.FileSystemListTreeElement=function(storagePanel)
{WebInspector.StorageCategoryTreeElement.call(this,storagePanel,WebInspector.UIString("FileSystem"),"FileSystem",["file-system-storage-tree-item"]);}
WebInspector.FileSystemListTreeElement.prototype={_initialize:function()
{this._refreshFileSystem();},onattach:function()
{WebInspector.StorageCategoryTreeElement.prototype.onattach.call(this);this.listItemElement.addEventListener("contextmenu",this._handleContextMenuEvent.bind(this),true);},_handleContextMenuEvent:function(event)
{var contextMenu=new WebInspector.ContextMenu(event);contextMenu.appendItem(WebInspector.UIString.capitalize("Refresh FileSystem ^list"),this._refreshFileSystem.bind(this));contextMenu.show();},_fileSystemAdded:function(event)
{var fileSystem=(event.data);var fileSystemTreeElement=new WebInspector.FileSystemTreeElement(this._storagePanel,fileSystem);this.appendChild(fileSystemTreeElement);},_fileSystemRemoved:function(event)
{var fileSystem=(event.data);var fileSystemTreeElement=this._fileSystemTreeElementByName(fileSystem.name);if(!fileSystemTreeElement)
return;fileSystemTreeElement.clear();this.removeChild(fileSystemTreeElement);},_fileSystemTreeElementByName:function(fileSystemName)
{for(var child of this.children()){var fschild=(child);if(fschild.fileSystemName===fileSystemName)
return fschild;}
return null;},_refreshFileSystem:function()
{if(!this._fileSystemModel){this._fileSystemModel=new WebInspector.FileSystemModel(this.target());this._fileSystemModel.addEventListener(WebInspector.FileSystemModel.EventTypes.FileSystemAdded,this._fileSystemAdded,this);this._fileSystemModel.addEventListener(WebInspector.FileSystemModel.EventTypes.FileSystemRemoved,this._fileSystemRemoved,this);}
this._fileSystemModel.refreshFileSystemList();},__proto__:WebInspector.StorageCategoryTreeElement.prototype}
WebInspector.IDBDatabaseTreeElement=function(storagePanel,model,databaseId)
{WebInspector.BaseStorageTreeElement.call(this,storagePanel,databaseId.name+" - "+databaseId.securityOrigin,["indexed-db-storage-tree-item"]);this._model=model;this._databaseId=databaseId;this._idbObjectStoreTreeElements={};}
WebInspector.IDBDatabaseTreeElement.prototype={get itemURL()
{return"indexedDB://"+this._databaseId.securityOrigin+"/"+this._databaseId.name;},onattach:function()
{WebInspector.BaseStorageTreeElement.prototype.onattach.call(this);this.listItemElement.addEventListener("contextmenu",this._handleContextMenuEvent.bind(this),true);},_handleContextMenuEvent:function(event)
{var contextMenu=new WebInspector.ContextMenu(event);contextMenu.appendItem(WebInspector.UIString("Refresh IndexedDB"),this._refreshIndexedDB.bind(this));contextMenu.show();},_refreshIndexedDB:function()
{this._model.refreshDatabaseNames();},update:function(database)
{this._database=database;var objectStoreNames={};for(var objectStoreName in this._database.objectStores){var objectStore=this._database.objectStores[objectStoreName];objectStoreNames[objectStore.name]=true;if(!this._idbObjectStoreTreeElements[objectStore.name]){var idbObjectStoreTreeElement=new WebInspector.IDBObjectStoreTreeElement(this._storagePanel,this._model,this._databaseId,objectStore);this._idbObjectStoreTreeElements[objectStore.name]=idbObjectStoreTreeElement;this.appendChild(idbObjectStoreTreeElement);}
this._idbObjectStoreTreeElements[objectStore.name].update(objectStore);}
for(var objectStoreName in this._idbObjectStoreTreeElements){if(!objectStoreNames[objectStoreName])
this._objectStoreRemoved(objectStoreName);}
if(this._view)
this._view.update(database);this._updateTooltip();},_updateTooltip:function()
{this.tooltip=WebInspector.UIString("Version")+": "+this._database.version;},onselect:function(selectedByUser)
{WebInspector.BaseStorageTreeElement.prototype.onselect.call(this,selectedByUser);if(!this._view)
this._view=new WebInspector.IDBDatabaseView(this._database);this._storagePanel.showIndexedDB(this._view);return false;},_objectStoreRemoved:function(objectStoreName)
{var objectStoreTreeElement=this._idbObjectStoreTreeElements[objectStoreName];objectStoreTreeElement.clear();this.removeChild(objectStoreTreeElement);delete this._idbObjectStoreTreeElements[objectStoreName];},clear:function()
{for(var objectStoreName in this._idbObjectStoreTreeElements)
this._objectStoreRemoved(objectStoreName);},__proto__:WebInspector.BaseStorageTreeElement.prototype}
WebInspector.IDBObjectStoreTreeElement=function(storagePanel,model,databaseId,objectStore)
{WebInspector.BaseStorageTreeElement.call(this,storagePanel,objectStore.name,["indexed-db-object-store-storage-tree-item"]);this._model=model;this._databaseId=databaseId;this._idbIndexTreeElements={};}
WebInspector.IDBObjectStoreTreeElement.prototype={get itemURL()
{return"indexedDB://"+this._databaseId.securityOrigin+"/"+this._databaseId.name+"/"+this._objectStore.name;},onattach:function()
{WebInspector.BaseStorageTreeElement.prototype.onattach.call(this);this.listItemElement.addEventListener("contextmenu",this._handleContextMenuEvent.bind(this),true);},_handleContextMenuEvent:function(event)
{var contextMenu=new WebInspector.ContextMenu(event);contextMenu.appendItem(WebInspector.UIString("Clear"),this._clearObjectStore.bind(this));contextMenu.show();},_clearObjectStore:function()
{function callback(){this.update(this._objectStore);}
this._model.clearObjectStore(this._databaseId,this._objectStore.name,callback.bind(this));},update:function(objectStore)
{this._objectStore=objectStore;var indexNames={};for(var indexName in this._objectStore.indexes){var index=this._objectStore.indexes[indexName];indexNames[index.name]=true;if(!this._idbIndexTreeElements[index.name]){var idbIndexTreeElement=new WebInspector.IDBIndexTreeElement(this._storagePanel,this._model,this._databaseId,this._objectStore,index);this._idbIndexTreeElements[index.name]=idbIndexTreeElement;this.appendChild(idbIndexTreeElement);}
this._idbIndexTreeElements[index.name].update(index);}
for(var indexName in this._idbIndexTreeElements){if(!indexNames[indexName])
this._indexRemoved(indexName);}
for(var indexName in this._idbIndexTreeElements){if(!indexNames[indexName]){this.removeChild(this._idbIndexTreeElements[indexName]);delete this._idbIndexTreeElements[indexName];}}
if(this.childCount())
this.expand();if(this._view)
this._view.update(this._objectStore);this._updateTooltip();},_updateTooltip:function()
{var keyPathString=this._objectStore.keyPathString;var tooltipString=keyPathString!==null?(WebInspector.UIString("Key path: ")+keyPathString):"";if(this._objectStore.autoIncrement)
tooltipString+="\n"+WebInspector.UIString("autoIncrement");this.tooltip=tooltipString;},onselect:function(selectedByUser)
{WebInspector.BaseStorageTreeElement.prototype.onselect.call(this,selectedByUser);if(!this._view)
this._view=new WebInspector.IDBDataView(this._model,this._databaseId,this._objectStore,null);this._storagePanel.showIndexedDB(this._view);return false;},_indexRemoved:function(indexName)
{var indexTreeElement=this._idbIndexTreeElements[indexName];indexTreeElement.clear();this.removeChild(indexTreeElement);delete this._idbIndexTreeElements[indexName];},clear:function()
{for(var indexName in this._idbIndexTreeElements)
this._indexRemoved(indexName);if(this._view)
this._view.clear();},__proto__:WebInspector.BaseStorageTreeElement.prototype}
WebInspector.IDBIndexTreeElement=function(storagePanel,model,databaseId,objectStore,index)
{WebInspector.BaseStorageTreeElement.call(this,storagePanel,index.name,["indexed-db-index-storage-tree-item"]);this._model=model;this._databaseId=databaseId;this._objectStore=objectStore;this._index=index;}
WebInspector.IDBIndexTreeElement.prototype={get itemURL()
{return"indexedDB://"+this._databaseId.securityOrigin+"/"+this._databaseId.name+"/"+this._objectStore.name+"/"+this._index.name;},update:function(index)
{this._index=index;if(this._view)
this._view.update(this._index);this._updateTooltip();},_updateTooltip:function()
{var tooltipLines=[];var keyPathString=this._index.keyPathString;tooltipLines.push(WebInspector.UIString("Key path: ")+keyPathString);if(this._index.unique)
tooltipLines.push(WebInspector.UIString("unique"));if(this._index.multiEntry)
tooltipLines.push(WebInspector.UIString("multiEntry"));this.tooltip=tooltipLines.join("\n");},onselect:function(selectedByUser)
{WebInspector.BaseStorageTreeElement.prototype.onselect.call(this,selectedByUser);if(!this._view)
this._view=new WebInspector.IDBDataView(this._model,this._databaseId,this._objectStore,this._index);this._storagePanel.showIndexedDB(this._view);return false;},clear:function()
{if(this._view)
this._view.clear();},__proto__:WebInspector.BaseStorageTreeElement.prototype}
WebInspector.DOMStorageTreeElement=function(storagePanel,domStorage,className)
{WebInspector.BaseStorageTreeElement.call(this,storagePanel,domStorage.securityOrigin?domStorage.securityOrigin:WebInspector.UIString("Local Files"),["domstorage-storage-tree-item",className]);this._domStorage=domStorage;}
WebInspector.DOMStorageTreeElement.prototype={get itemURL()
{return"storage://"+this._domStorage.securityOrigin+"/"+(this._domStorage.isLocalStorage?"local":"session");},onselect:function(selectedByUser)
{WebInspector.BaseStorageTreeElement.prototype.onselect.call(this,selectedByUser);this._storagePanel._showDOMStorage(this._domStorage);return false;},__proto__:WebInspector.BaseStorageTreeElement.prototype}
WebInspector.CookieTreeElement=function(storagePanel,cookieDomain)
{WebInspector.BaseStorageTreeElement.call(this,storagePanel,cookieDomain?cookieDomain:WebInspector.UIString("Local Files"),["cookie-storage-tree-item"]);this._cookieDomain=cookieDomain;}
WebInspector.CookieTreeElement.prototype={get itemURL()
{return"cookies://"+this._cookieDomain;},onattach:function()
{WebInspector.BaseStorageTreeElement.prototype.onattach.call(this);this.listItemElement.addEventListener("contextmenu",this._handleContextMenuEvent.bind(this),true);},_handleContextMenuEvent:function(event)
{var contextMenu=new WebInspector.ContextMenu(event);contextMenu.appendItem(WebInspector.UIString("Clear"),this._clearCookies.bind(this));contextMenu.show();},_clearCookies:function(domain)
{this._storagePanel.clearCookies(this._cookieDomain);},onselect:function(selectedByUser)
{WebInspector.BaseStorageTreeElement.prototype.onselect.call(this,selectedByUser);this._storagePanel.showCookies(this,this._cookieDomain);return false;},__proto__:WebInspector.BaseStorageTreeElement.prototype}
WebInspector.ApplicationCacheManifestTreeElement=function(storagePanel,manifestURL)
{var title=new WebInspector.ParsedURL(manifestURL).displayName;WebInspector.BaseStorageTreeElement.call(this,storagePanel,title,["application-cache-storage-tree-item"]);this.tooltip=manifestURL;this._manifestURL=manifestURL;}
WebInspector.ApplicationCacheManifestTreeElement.prototype={get itemURL()
{return"appcache://"+this._manifestURL;},get manifestURL()
{return this._manifestURL;},onselect:function(selectedByUser)
{WebInspector.BaseStorageTreeElement.prototype.onselect.call(this,selectedByUser);this._storagePanel.showCategoryView(this._manifestURL);return false;},__proto__:WebInspector.BaseStorageTreeElement.prototype}
WebInspector.ApplicationCacheFrameTreeElement=function(storagePanel,frameId,manifestURL)
{WebInspector.BaseStorageTreeElement.call(this,storagePanel,"",["frame-storage-tree-item"]);this._frameId=frameId;this._manifestURL=manifestURL;this._refreshTitles();}
WebInspector.ApplicationCacheFrameTreeElement.prototype={get itemURL()
{return"appcache://"+this._manifestURL+"/"+encodeURI(this.displayName);},get frameId()
{return this._frameId;},get manifestURL()
{return this._manifestURL;},_refreshTitles:function()
{var frame=this._storagePanel._target.resourceTreeModel.frameForId(this._frameId);if(!frame){this.subtitleText=WebInspector.UIString("new frame");return;}
this.titleText=frame.name;this.subtitleText=new WebInspector.ParsedURL(frame.url).displayName;},frameNavigated:function()
{this._refreshTitles();},onselect:function(selectedByUser)
{WebInspector.BaseStorageTreeElement.prototype.onselect.call(this,selectedByUser);this._storagePanel.showApplicationCache(this._frameId);return false;},__proto__:WebInspector.BaseStorageTreeElement.prototype}
WebInspector.FileSystemTreeElement=function(storagePanel,fileSystem)
{var displayName=fileSystem.type+" - "+fileSystem.origin;WebInspector.BaseStorageTreeElement.call(this,storagePanel,displayName,["file-system-storage-tree-item"]);this._fileSystem=fileSystem;}
WebInspector.FileSystemTreeElement.prototype={get fileSystemName()
{return this._fileSystem.name;},get itemURL()
{return"filesystem://"+this._fileSystem.name;},onselect:function(selectedByUser)
{WebInspector.BaseStorageTreeElement.prototype.onselect.call(this,selectedByUser);this._fileSystemView=new WebInspector.FileSystemView(this._fileSystem);this._storagePanel.showFileSystem(this._fileSystemView);return false;},clear:function()
{if(this.fileSystemView&&this._storagePanel.visibleView===this.fileSystemView)
this._storagePanel.closeVisibleView();},__proto__:WebInspector.BaseStorageTreeElement.prototype}
WebInspector.StorageCategoryView=function()
{WebInspector.VBox.call(this);this.element.classList.add("storage-view");this._emptyWidget=new WebInspector.EmptyWidget("");this._emptyWidget.show(this.element);}
WebInspector.StorageCategoryView.prototype={toolbarItems:function()
{return[];},setText:function(text)
{this._emptyWidget.text=text;},__proto__:WebInspector.VBox.prototype}
WebInspector.ResourcesPanel.show=function()
{WebInspector.inspectorView.setCurrentPanel(WebInspector.ResourcesPanel._instance());}
WebInspector.ResourcesPanel._instance=function()
{if(!WebInspector.ResourcesPanel._instanceObject)
WebInspector.ResourcesPanel._instanceObject=new WebInspector.ResourcesPanel();return WebInspector.ResourcesPanel._instanceObject;}
WebInspector.ResourcesPanelFactory=function()
{}
WebInspector.ResourcesPanelFactory.prototype={createPanel:function()
{return WebInspector.ResourcesPanel._instance();}};WebInspector.ServiceWorkerCacheView=function(model,cache)
{WebInspector.DataGridContainerWidget.call(this);this.registerRequiredCSS("resources/serviceWorkerCacheViews.css");this._model=model;this.element.classList.add("service-worker-cache-data-view");this.element.classList.add("storage-view");this._createEditorToolbar();this._refreshButton=new WebInspector.ToolbarButton(WebInspector.UIString("Refresh"),"refresh-toolbar-item");this._refreshButton.addEventListener("click",this._refreshButtonClicked,this);this._pageSize=50;this._skipCount=0;this.update(cache);this._entries=[];}
WebInspector.ServiceWorkerCacheView.prototype={_createDataGrid:function()
{var columns=[];columns.push({id:"number",title:WebInspector.UIString("#"),width:"50px"});columns.push({id:"request",title:WebInspector.UIString("Request")});columns.push({id:"response",title:WebInspector.UIString("Response")});var dataGrid=new WebInspector.DataGrid(columns,undefined,this._deleteButtonClicked.bind(this),this._updateData.bind(this,true));return dataGrid;},_createEditorToolbar:function()
{var editorToolbar=new WebInspector.Toolbar(this.element);editorToolbar.element.classList.add("data-view-toolbar");this._pageBackButton=new WebInspector.ToolbarButton(WebInspector.UIString("Show previous page"),"play-backwards-toolbar-item");this._pageBackButton.addEventListener("click",this._pageBackButtonClicked,this);editorToolbar.appendToolbarItem(this._pageBackButton);this._pageForwardButton=new WebInspector.ToolbarButton(WebInspector.UIString("Show next page"),"play-toolbar-item");this._pageForwardButton.setEnabled(false);this._pageForwardButton.addEventListener("click",this._pageForwardButtonClicked,this);editorToolbar.appendToolbarItem(this._pageForwardButton);},_pageBackButtonClicked:function()
{this._skipCount=Math.max(0,this._skipCount-this._pageSize);this._updateData(false);},_pageForwardButtonClicked:function()
{this._skipCount=this._skipCount+this._pageSize;this._updateData(false);},_deleteButtonClicked:function(node)
{this._model.deleteCacheEntry(this._cache,node.data["request"],node.remove.bind(node));},update:function(cache)
{this._cache=cache;if(this._dataGrid)
this.removeDataGrid(this._dataGrid);this._dataGrid=this._createDataGrid();this.appendDataGrid(this._dataGrid);this._skipCount=0;this._updateData(true);},_updateDataCallback(skipCount,entries,hasMore)
{this._refreshButton.setEnabled(true);this.clear();this._entries=entries;for(var i=0;i<entries.length;++i){var data={};data["number"]=i+skipCount;data["request"]=entries[i].request;data["response"]=entries[i].response;var node=new WebInspector.DataGridNode(data);node.selectable=true;this._dataGrid.rootNode().appendChild(node);}
this._pageBackButton.setEnabled(!!skipCount);this._pageForwardButton.setEnabled(hasMore);},_updateData:function(force)
{var pageSize=this._pageSize;var skipCount=this._skipCount;this._refreshButton.setEnabled(false);if(!force&&this._lastPageSize===pageSize&&this._lastSkipCount===skipCount)
return;if(this._lastPageSize!==pageSize){skipCount=0;this._skipCount=0;}
this._lastPageSize=pageSize;this._lastSkipCount=skipCount;this._model.loadCacheData(this._cache,skipCount,pageSize,this._updateDataCallback.bind(this,skipCount));},_refreshButtonClicked:function(event)
{this._updateData(true);},toolbarItems:function()
{return[this._refreshButton];},clear:function()
{this._dataGrid.rootNode().removeChildren();this._entries=[];},__proto__:WebInspector.DataGridContainerWidget.prototype};WebInspector.ServiceWorkersView=function()
{WebInspector.VBox.call(this,true);this.registerRequiredCSS("resources/serviceWorkersView.css");this.contentElement.classList.add("service-workers-view");this._securityOriginHosts=new Set();this._originHostToOriginElementMap=new Map();this._registrationIdToOriginElementMap=new Map();var settingsDiv=createElementWithClass("div","service-workers-settings");var debugOnStartCheckboxLabel=createCheckboxLabel(WebInspector.UIString("Open DevTools window and pause JavaScript execution on Service Worker startup for debugging."));this._debugOnStartCheckbox=debugOnStartCheckboxLabel.checkboxElement;this._debugOnStartCheckbox.addEventListener("change",this._debugOnStartCheckboxChanged.bind(this),false)
this._debugOnStartCheckbox.disabled=true
settingsDiv.appendChild(debugOnStartCheckboxLabel);this.contentElement.appendChild(settingsDiv);this._root=this.contentElement.createChild("ol");this._root.classList.add("service-workers-root");WebInspector.targetManager.observeTargets(this);}
WebInspector.ServiceWorkersView.prototype={targetAdded:function(target)
{if(this._target)
return;this._target=target;this._manager=this._target.serviceWorkerManager;this._debugOnStartCheckbox.disabled=false;this._debugOnStartCheckbox.checked=this._manager.debugOnStart();for(var registration of this._manager.registrations().values())
this._updateRegistration(registration);this._manager.addEventListener(WebInspector.ServiceWorkerManager.Events.RegistrationUpdated,this._registrationUpdated,this);this._manager.addEventListener(WebInspector.ServiceWorkerManager.Events.RegistrationDeleted,this._registrationDeleted,this);this._manager.addEventListener(WebInspector.ServiceWorkerManager.Events.DebugOnStartUpdated,this._debugOnStartUpdated,this);this._target.resourceTreeModel.addEventListener(WebInspector.ResourceTreeModel.EventTypes.SecurityOriginAdded,this._securityOriginAdded,this);this._target.resourceTreeModel.addEventListener(WebInspector.ResourceTreeModel.EventTypes.SecurityOriginRemoved,this._securityOriginRemoved,this);var securityOrigins=this._target.resourceTreeModel.securityOrigins();for(var i=0;i<securityOrigins.length;++i)
this._addOrigin(securityOrigins[i]);},targetRemoved:function(target)
{if(target!==this._target)
return;delete this._target;},_registrationUpdated:function(event)
{var registration=(event.data);this._updateRegistration(registration);},_updateRegistration:function(registration)
{var parsedURL=registration.scopeURL.asParsedURL();if(!parsedURL)
return;var originHost=parsedURL.host;var originElement=this._originHostToOriginElementMap.get(originHost);if(!originElement){originElement=new WebInspector.ServiceWorkerOriginElement(this._manager,originHost);if(this._securityOriginHosts.has(originHost))
this._appendOriginNode(originElement);this._originHostToOriginElementMap.set(originHost,originElement);}
this._registrationIdToOriginElementMap.set(registration.id,originElement);originElement._updateRegistration(registration);},_registrationDeleted:function(event)
{var registration=(event.data);var registrationId=registration.id;var originElement=this._registrationIdToOriginElementMap.get(registrationId);if(!originElement)
return;this._registrationIdToOriginElementMap.delete(registrationId);originElement._deleteRegistration(registrationId);if(originElement._hasRegistration())
return;if(this._securityOriginHosts.has(originElement._originHost))
this._removeOriginNode(originElement);this._originHostToOriginElementMap.delete(originElement._originHost);},_debugOnStartUpdated:function(event)
{var debugOnStart=(event.data);this._debugOnStartCheckbox.checked=debugOnStart;},_securityOriginAdded:function(event)
{this._addOrigin((event.data));},_addOrigin:function(securityOrigin)
{var parsedURL=securityOrigin.asParsedURL();if(!parsedURL)
return;var originHost=parsedURL.host;if(this._securityOriginHosts.has(originHost))
return;this._securityOriginHosts.add(originHost);var originElement=this._originHostToOriginElementMap.get(originHost);if(!originElement)
return;this._appendOriginNode(originElement);},_securityOriginRemoved:function(event)
{var securityOrigin=(event.data);var parsedURL=securityOrigin.asParsedURL();if(!parsedURL)
return;var originHost=parsedURL.host;if(!this._securityOriginHosts.has(originHost))
return;this._securityOriginHosts.delete(originHost);var originElement=this._originHostToOriginElementMap.get(originHost);if(!originElement)
return;this._removeOriginNode(originElement);},_appendOriginNode:function(originElement)
{this._root.appendChild(originElement._element);},_removeOriginNode:function(originElement)
{this._root.removeChild(originElement._element);},_debugOnStartCheckboxChanged:function()
{if(!this._manager)
return;this._manager.setDebugOnStart(this._debugOnStartCheckbox.checked);this._debugOnStartCheckbox.checked=this._manager.debugOnStart();},__proto__:WebInspector.VBox.prototype}
WebInspector.ServiceWorkerOriginElement=function(manager,originHost)
{this._manager=manager;this._registrationElements=new Map();this._originHost=originHost;this._element=createElementWithClass("div","service-workers-origin");this._listItemNode=this._element.createChild("li","service-workers-origin-title");this._listItemNode.createChild("div").createTextChild(originHost);this._childrenListNode=this._element.createChild("ol");}
WebInspector.ServiceWorkerOriginElement.prototype={_hasRegistration:function()
{return this._registrationElements.size!=0;},_updateRegistration:function(registration)
{var swRegistrationElement=this._registrationElements.get(registration.id);if(swRegistrationElement){swRegistrationElement._updateRegistration(registration);return;}
swRegistrationElement=new WebInspector.SWRegistrationElement(this._manager,this,registration);this._registrationElements.set(registration.id,swRegistrationElement);this._childrenListNode.appendChild(swRegistrationElement._element);},_deleteRegistration:function(registrationId)
{var swRegistrationElement=this._registrationElements.get(registrationId);if(!swRegistrationElement)
return;this._registrationElements.delete(registrationId);this._childrenListNode.removeChild(swRegistrationElement._element);},_visible:function()
{return!!this._element.parentElement;},}
WebInspector.SWRegistrationElement=function(manager,originElement,registration)
{this._manager=manager;this._originElement=originElement;this._registration=registration;this._element=createElementWithClass("div","service-workers-registration");var headerNode=this._element.createChild("div","service-workers-registration-header");this._titleNode=headerNode.createChild("div","service-workers-registration-title");var buttonsNode=headerNode.createChild("div","service-workers-registration-buttons");this._updateButton=buttonsNode.createChild("button","service-workers-button service-workers-update-button");this._updateButton.addEventListener("click",this._updateButtonClicked.bind(this),false);this._updateButton.title=WebInspector.UIString("Update");this._updateButton.disabled=true
this._pushButton=buttonsNode.createChild("button","service-workers-button service-workers-push-button");this._pushButton.addEventListener("click",this._pushButtonClicked.bind(this),false);this._pushButton.title=WebInspector.UIString("Emulate push event");this._pushButton.disabled=true
this._deleteButton=buttonsNode.createChild("button","service-workers-button service-workers-delete-button");this._deleteButton.addEventListener("click",this._deleteButtonClicked.bind(this),false);this._deleteButton.title=WebInspector.UIString("Delete");this._childrenListNode=this._element.createChild("div","service-workers-registration-content");this._categorizedVersions={};for(var mode in WebInspector.ServiceWorkerVersion.Modes)
this._categorizedVersions[WebInspector.ServiceWorkerVersion.Modes[mode]]=[];this._selectedMode=WebInspector.ServiceWorkerVersion.Modes.Active;this._versionElements=[];this._updateRegistration(registration);}
WebInspector.SWRegistrationElement.prototype={_updateRegistration:function(registration)
{this._registration=registration;this._titleNode.textContent=WebInspector.UIString(registration.isDeleted?"Scope: %s - deleted":"Scope: %s",registration.scopeURL.asParsedURL().path);this._updateButton.disabled=!!registration.isDeleted;this._deleteButton.disabled=!!registration.isDeleted;var lastFocusedVersionId=undefined;if(this._categorizedVersions[this._selectedMode].length)
lastFocusedVersionId=this._categorizedVersions[this._selectedMode][0].id;for(var mode in WebInspector.ServiceWorkerVersion.Modes)
this._categorizedVersions[WebInspector.ServiceWorkerVersion.Modes[mode]]=[];for(var version of registration.versions.valuesArray()){if(version.isStoppedAndRedundant()&&!version.errorMessages.length)
continue;var mode=version.mode();this._categorizedVersions[mode].push(version);if(version.id===lastFocusedVersionId)
this._selectedMode=mode;}
if(!this._categorizedVersions[this._selectedMode].length){for(var mode of[WebInspector.ServiceWorkerVersion.Modes.Active,WebInspector.ServiceWorkerVersion.Modes.Waiting,WebInspector.ServiceWorkerVersion.Modes.Installing,WebInspector.ServiceWorkerVersion.Modes.Redundant]){if(this._categorizedVersions[mode].length){this._selectedMode=mode;break;}}}
this._pushButton.disabled=!this._categorizedVersions[WebInspector.ServiceWorkerVersion.Modes.Active].length||!!this._registration.isDeleted;this._updateVersionList();},_updateVersionList:function()
{var fragment=createDocumentFragment();var modeTabList=createElementWithClass("div","service-workers-versions-mode-tab-list");modeTabList.appendChild(this._createVersionModeTab(WebInspector.ServiceWorkerVersion.Modes.Installing));modeTabList.appendChild(this._createVersionModeTab(WebInspector.ServiceWorkerVersion.Modes.Waiting));modeTabList.appendChild(this._createVersionModeTab(WebInspector.ServiceWorkerVersion.Modes.Active));modeTabList.appendChild(this._createVersionModeTab(WebInspector.ServiceWorkerVersion.Modes.Redundant));fragment.appendChild(modeTabList);fragment.appendChild(this._createSelectedModeVersionsPanel(this._selectedMode));this._childrenListNode.removeChildren();this._childrenListNode.appendChild(fragment);},_createVersionModeTab:function(mode)
{var versions=this._categorizedVersions[mode];var modeTitle=WebInspector.UIString(mode);var selected=this._selectedMode==mode;var modeTab=createElementWithClass("div","service-workers-versions-mode-tab");for(var version of versions){var icon=modeTab.createChild("div","service-workers-versions-mode-tab-icon service-workers-color-"+(version.id%10));icon.title=WebInspector.UIString("ID: %s",version.id);}
var modeTabText=modeTab.createChild("div","service-workers-versions-mode-tab-text");modeTabText.createTextChild(WebInspector.UIString(modeTitle));if(selected){modeTab.classList.add("service-workers-versions-mode-tab-selected");modeTabText.classList.add("service-workers-versions-mode-tab-text-selected");}
if(versions.length){modeTab.addEventListener("click",this._modeTabClicked.bind(this,mode),false);}else{modeTab.classList.add("service-workers-versions-mode-tab-disabled");modeTabText.classList.add("service-workers-versions-mode-tab-text-disabled");}
return modeTab;},_createSelectedModeVersionsPanel:function(mode)
{var versions=this._categorizedVersions[mode];var panelContainer=createElementWithClass("div","service-workers-versions-panel-container");var index=0;var versionElement;for(var i=0;i<versions.length;++i){if(i<this._versionElements.length){versionElement=this._versionElements[i];versionElement._updateVersion(versions[i]);}else{versionElement=new WebInspector.SWVersionElement(this._manager,this._registration.scopeURL,versions[i]);this._versionElements.push(versionElement);}
panelContainer.appendChild(versionElement._element);}
this._versionElements.splice(versions.length);return panelContainer;},_modeTabClicked:function(mode)
{if(this._selectedMode==mode)
return;this._selectedMode=mode;this._updateVersionList();},_deleteButtonClicked:function(event)
{this._manager.deleteRegistration(this._registration.id);},_updateButtonClicked:function(event)
{this._manager.updateRegistration(this._registration.id);},_pushButtonClicked:function(event)
{var data="Test push message from DevTools."
this._manager.deliverPushMessage(this._registration.id,data);},_visible:function()
{return this._originElement._visible();},}
WebInspector.SWVersionElement=function(manager,scopeURL,version)
{this._manager=manager;this._scopeURL=scopeURL;this._version=version;this._element=createElementWithClass("div","service-workers-version");this._clientInfoCache={};this._createElements();this._updateVersion(version);}
WebInspector.SWVersionElement.prototype={_createElements:function()
{var panel=createElementWithClass("div","service-workers-versions-panel");var leftPanel=panel.createChild("div","service-workers-versions-panel-left");var rightPanel=panel.createChild("div","service-workers-versions-panel-right");this._stateCell=this._addTableRow(leftPanel,WebInspector.UIString("State"));this._workerCell=this._addTableRow(leftPanel,WebInspector.UIString("Worker"));this._scriptCell=this._addTableRow(leftPanel,WebInspector.UIString("Script URL"));this._updatedCell=this._addTableRow(leftPanel,WebInspector.UIString("Updated"));this._updatedCell.classList.add("service-worker-script-response-time");this._scriptLastModifiedCell=this._addTableRow(leftPanel,WebInspector.UIString("Last-Modified"));this._scriptLastModifiedCell.classList.add("service-worker-script-last-modified");rightPanel.createChild("div","service-workers-versions-table-messages-title").createTextChild(WebInspector.UIString("Recent messages"));this._messagesPanel=rightPanel.createChild("div","service-workers-versions-table-messages-content");this._clientsTitle=rightPanel.createChild("div","service-workers-versions-table-clients-title");this._clientsTitle.createTextChild(WebInspector.UIString("Controlled clients"));this._clientsPanel=rightPanel.createChild("div","service-workers-versions-table-clients-content");this._element.appendChild(panel);},_updateVersion:function(version)
{this._stateCell.removeChildren();this._stateCell.createTextChild(version.status);this._workerCell.removeChildren();if(version.isRunning()||version.isStarting()||version.isStartable()){var runningStatusCell=this._workerCell.createChild("div","service-workers-versions-table-worker-running-status-cell");var runningStatusLeftCell=runningStatusCell.createChild("div","service-workers-versions-table-running-status-left-cell");var runningStatusRightCell=runningStatusCell.createChild("div","service-workers-versions-table-running-status-right-cell");if(version.isRunning()||version.isStarting()){var stopButton=runningStatusLeftCell.createChild("button","service-workers-button service-workers-stop-button");stopButton.addEventListener("click",this._stopButtonClicked.bind(this,version.id),false);stopButton.title=WebInspector.UIString("Stop");}else if(version.isStartable()){var startButton=runningStatusLeftCell.createChild("button","service-workers-button service-workers-start-button");startButton.addEventListener("click",this._startButtonClicked.bind(this),false);startButton.title=WebInspector.UIString("Start");}
runningStatusRightCell.createTextChild(version.runningStatus);if(version.isRunning()||version.isStarting()){var inspectButton=runningStatusRightCell.createChild("div","service-workers-versions-table-running-status-inspect");inspectButton.createTextChild(WebInspector.UIString("inspect"));inspectButton.addEventListener("click",this._inspectButtonClicked.bind(this,version.id),false);}}else{this._workerCell.createTextChild(version.runningStatus);}
this._scriptCell.removeChildren();this._scriptCell.createTextChild(version.scriptURL.asParsedURL().path);this._updatedCell.removeChildren();if(version.scriptResponseTime)
this._updatedCell.createTextChild((new Date(version.scriptResponseTime*1000)).toConsoleTime());this._scriptLastModifiedCell.removeChildren();if(version.scriptLastModified)
this._scriptLastModifiedCell.createTextChild((new Date(version.scriptLastModified*1000)).toConsoleTime());this._messagesPanel.removeChildren();if(version.scriptLastModified){var scriptLastModifiedLabel=this._messagesPanel.createChild("label"," service-workers-info service-worker-script-last-modified","dt-icon-label");scriptLastModifiedLabel.type="info-icon";scriptLastModifiedLabel.createTextChild(WebInspector.UIString("Last-Modified: %s",(new Date(version.scriptLastModified*1000)).toConsoleTime()));}
if(version.scriptResponseTime){var scriptResponseTimeDiv=this._messagesPanel.createChild("label"," service-workers-info service-worker-script-response-time","dt-icon-label");scriptResponseTimeDiv.type="info-icon";scriptResponseTimeDiv.createTextChild(WebInspector.UIString("Server response time: %s",(new Date(version.scriptResponseTime*1000)).toConsoleTime()));}
var errorMessages=version.errorMessages;for(var index=0;index<errorMessages.length;++index){var errorDiv=this._messagesPanel.createChild("div","service-workers-error");errorDiv.createChild("label","","dt-icon-label").type="error-icon";errorDiv.createChild("div","service-workers-error-message").createTextChild(errorMessages[index].errorMessage);var script_path=errorMessages[index].sourceURL;var script_url;if(script_url=script_path.asParsedURL())
script_path=script_url.displayName;if(script_path.length&&errorMessages[index].lineNumber!=-1)
script_path=String.sprintf("(%s:%d)",script_path,errorMessages[index].lineNumber);errorDiv.createChild("div","service-workers-error-line").createTextChild(script_path);}
this._clientsTitle.classList.toggle("hidden",version.controlledClients.length==0);this._clientsPanel.removeChildren();for(var i=0;i<version.controlledClients.length;++i){var client=version.controlledClients[i];var clientLabelText=this._clientsPanel.createChild("div","service-worker-client");if(this._clientInfoCache[client]){this._updateClientInfo(clientLabelText,this._clientInfoCache[client]);}
this._manager.getTargetInfo(client,this._onClientInfo.bind(this,clientLabelText));}},_addTableRow:function(tableElement,title)
{var rowElement=tableElement.createChild("div","service-workers-versions-table-row");rowElement.createChild("div","service-workers-versions-table-row-title").createTextChild(title);return rowElement.createChild("div","service-workers-versions-table-row-content");},_onClientInfo:function(element,targetInfo)
{if(!targetInfo)
return;this._clientInfoCache[targetInfo.id]=targetInfo;this._updateClientInfo(element,targetInfo);},_updateClientInfo:function(element,targetInfo)
{if(!(targetInfo.isWebContents()||targetInfo.isFrame())){element.createTextChild(WebInspector.UIString("Worker: %s",targetInfo.url));return;}
element.removeChildren();element.createTextChild(WebInspector.UIString("Tab: %s",targetInfo.url));var focusLabel=element.createChild("label","service-worker-client-focus");focusLabel.createTextChild("focus");focusLabel.addEventListener("click",this._activateTarget.bind(this,targetInfo.id),true);},_activateTarget:function(targetId)
{this._manager.activateTarget(targetId);},_startButtonClicked:function(event)
{this._manager.startWorker(this._scopeURL);},_stopButtonClicked:function(versionId,event)
{this._manager.stopWorker(versionId);},_inspectButtonClicked:function(versionId,event)
{this._manager.inspectWorker(versionId);},};Runtime.cachedResources["resources/indexedDBViews.css"]="/*\n * Copyright (C) 2012 Google Inc. All rights reserved.\n *\n * Redistribution and use in source and binary forms, with or without\n * modification, are permitted provided that the following conditions are\n * met:\n *\n *     * Redistributions of source code must retain the above copyright\n * notice, this list of conditions and the following disclaimer.\n *     * Redistributions in binary form must reproduce the above\n * copyright notice, this list of conditions and the following disclaimer\n * in the documentation and/or other materials provided with the\n * distribution.\n *     * Neither the name of Google Inc. nor the names of its\n * contributors may be used to endorse or promote products derived from\n * this software without specific prior written permission.\n *\n * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS\n * \"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT\n * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR\n * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT\n * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,\n * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT\n * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,\n * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY\n * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT\n * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE\n * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n */\n\n.indexed-db-database-view {\n    -webkit-user-select: text;\n    margin-top: 5px;\n}\n\n.indexed-db-database-view .outline-disclosure {\n    padding-left: 0;\n}\n\n.indexed-db-database-view .outline-disclosure li {\n    white-space: nowrap;\n}\n\n.indexed-db-database-view .outline-disclosure .attribute-name {\n    color: rgb(33%, 33%, 33%);\n    display: inline-block;\n    margin-right: 0.5em;\n    font-weight: bold;\n    vertical-align: top;\n}\n\n.indexed-db-database-view .outline-disclosure .attribute-value {\n    display: inline;\n    margin-top: 1px;\n}\n\n.indexed-db-data-view .data-view-toolbar {\n    position: relative;\n    background-color: #eee;\n    border-bottom: 1px solid #ccc;\n}\n\n.indexed-db-data-view .data-view-toolbar .key-input {\n    font-size: 11px;\n    margin: auto 0;\n    width: 200px;\n}\n\n.indexed-db-data-view .data-grid {\n    flex: auto;\n}\n\n.indexed-db-data-view .data-grid .data-container tr:nth-child(even) {\n    background-color: white;\n}\n\n.indexed-db-data-view .data-grid .data-container tr:nth-child(odd) {\n    background-color: #EAF3FF;\n}\n\n.indexed-db-data-view .data-grid .data-container tr:nth-last-child(1) {\n    background-color: white;\n}\n\n.indexed-db-data-view .data-grid .data-container tr:nth-last-child(1) td {\n    border: 0;\n}\n\n.indexed-db-data-view .data-grid .data-container tr:nth-last-child(2) td {\n    border-bottom: 1px solid #aaa;\n}\n\n.indexed-db-data-view .section,\n.indexed-db-data-view .section > .header,\n.indexed-db-data-view .section > .header .title {\n    margin: 0;\n    min-height: inherit;\n    line-height: inherit;\n}\n\n.indexed-db-data-view .primitive-value {\n    padding-top: 1px;\n}\n\n.indexed-db-data-view .data-grid .data-container td .section .header .title {\n    white-space: nowrap;\n    text-overflow: ellipsis;\n    overflow: hidden;\n}\n\n.indexed-db-key-path {\n    color: rgb(196, 26, 22);\n    white-space: pre-wrap;\n    unicode-bidi: -webkit-isolate;\n}\n\n/*# sourceURL=resources/indexedDBViews.css */";Runtime.cachedResources["resources/resourcesPanel.css"]="/*\n * Copyright (C) 2006, 2007, 2008 Apple Inc.  All rights reserved.\n * Copyright (C) 2009 Anthony Ricaud <rik@webkit.org>\n *\n * Redistribution and use in source and binary forms, with or without\n * modification, are permitted provided that the following conditions\n * are met:\n *\n * 1.  Redistributions of source code must retain the above copyright\n *     notice, this list of conditions and the following disclaimer.\n * 2.  Redistributions in binary form must reproduce the above copyright\n *     notice, this list of conditions and the following disclaimer in the\n *     documentation and/or other materials provided with the distribution.\n * 3.  Neither the name of Apple Computer, Inc. (\"Apple\") nor the names of\n *     its contributors may be used to endorse or promote products derived\n *     from this software without specific prior written permission.\n *\n * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS \"AS IS\" AND ANY\n * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED\n * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE\n * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY\n * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES\n * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;\n * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND\n * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT\n * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF\n * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n */\n\n.resources.panel .sidebar {\n    padding-left: 0;\n    z-index: 10;\n    display: block;\n}\n\n.resources.panel .sidebar li {\n    height: 18px;\n    white-space: nowrap;\n}\n\n.resources.panel .sidebar li.selected {\n    color: white;\n    text-shadow: rgba(0, 0, 0, 0.33) 1px 1px 0;\n}\n\n.resources.panel .sidebar li.selected .selection {\n    background-image: linear-gradient(to bottom, rgb(162, 177, 207), rgb(120, 138, 177));\n    border-top: 1px solid #979797;\n    height: 18px;\n}\n\n.resources.panel .sidebar :focus li.selected .selection {\n    background-image: linear-gradient(to bottom, rgb(92, 147, 213), rgb(21, 83, 170));\n    border-top: 1px solid rgb(68, 128, 200);\n}\n\nbody.inactive .resources.panel .sidebar li.selected .selection {\n    background-image: linear-gradient(to bottom, rgb(180, 180, 180), rgb(138, 138, 138));\n    border-top: 1px solid rgb(151, 151, 151);\n}\n\n.resources.panel .sidebar .icon {\n    width: 16px;\n    height: 16px;\n    float: left;\n}\n\n.resources.panel .base-storage-tree-element-title {\n    overflow: hidden;\n    position: relative;\n    text-overflow: ellipsis;\n    padding-left: 2px;\n    top: 1px;\n}\n\n.resources-toolbar {\n    border-top: 1px solid #ccc;\n    background-color: #eee;\n}\n\nli.selected .base-storage-tree-element-subtitle {\n    color: white;\n}\n\n.base-storage-tree-element-subtitle {\n    padding-left: 2px;\n    color: rgb(80, 80, 80);\n    text-shadow: none;\n}\n\n.resources.panel .status {\n    float: right;\n    height: 16px;\n    margin-top: 1px;\n    margin-left: 4px;\n    line-height: 1em;\n}\n\n.storage-view {\n    display: flex;\n    overflow: hidden;\n}\n\n.storage-view {\n    overflow: hidden;\n}\n\n.storage-view .data-grid:not(.inline) {\n    border: none;\n    flex: auto;\n}\n\n.storage-view .storage-table-error {\n    color: rgb(66%, 33%, 33%);\n    font-size: 24px;\n    font-weight: bold;\n    padding: 10px;\n    display: flex;\n    align-items: center;\n    justify-content: center;\n}\n\n.storage-view.query {\n    padding: 2px 0;\n    overflow-y: overlay;\n    overflow-x: hidden;\n}\n\n.database-query-prompt {\n    position: relative;\n    padding: 1px 22px 1px 24px;\n    min-height: 16px;\n    white-space: pre-wrap;\n    -webkit-user-modify: read-write-plaintext-only;\n    -webkit-user-select: text;\n}\n\n.database-user-query::before,\n.database-query-prompt::before,\n.database-query-result::before {\n    position: absolute;\n    display: block;\n    content: \"\";\n    left: 7px;\n    top: 0.8em;\n    width: 10px;\n    height: 10px;\n    margin-top: -7px;\n    -webkit-user-select: none;\n    background-image: url(Images/toolbarButtonGlyphs.png);\n    background-size: 352px 168px;\n}\n\n@media (-webkit-min-device-pixel-ratio: 1.5) {\n.database-user-query::before,\n.database-query-prompt::before,\n.database-query-result::before {\n    background-image: url(Images/toolbarButtonGlyphs_2x.png);\n}\n} /* media */\n\n.database-query-prompt::before {\n    background-position: -192px -96px;\n}\n\n.database-user-query {\n    position: relative;\n    border-bottom: 1px solid rgb(245, 245, 245);\n    padding: 1px 22px 1px 24px;\n    min-height: 16px;\n    flex-shrink: 0;\n}\n\n.database-user-query::before {\n    background-position: -192px -107px;\n}\n\n.database-query-text {\n    color: rgb(0, 128, 255);\n    -webkit-user-select: text;\n}\n\n.database-query-result {\n    position: relative;\n    padding: 1px 22px 1px 24px;\n    min-height: 16px;\n    margin-left: -24px;\n    padding-right: 0;\n}\n\n.database-query-result.error {\n    color: red;\n    -webkit-user-select: text;\n}\n\n.database-query-result.error::before {\n    background-position: -213px -96px;\n}\n\n.resource-sidebar-tree-item .icon {\n    content: url(Images/resourcePlainIcon.png);\n}\n\n.children.small .resource-sidebar-tree-item .icon {\n    content: url(Images/resourcePlainIconSmall.png);\n}\n\n.resource-sidebar-tree-item.resources-type-image .icon {\n    position: relative;\n    background-image: url(Images/resourcePlainIcon.png);\n    background-repeat: no-repeat;\n    content: \"\";\n}\n\n.resources-type-image .image-resource-icon-preview {\n    position: absolute;\n    margin: auto;\n    top: 3px;\n    bottom: 4px;\n    left: 5px;\n    right: 5px;\n    max-width: 18px;\n    max-height: 21px;\n    min-width: 1px;\n    min-height: 1px;\n}\n\n.children.small .resource-sidebar-tree-item.resources-type-image .icon {\n    background-image: url(Images/resourcePlainIconSmall.png);\n    content: \"\";\n}\n\n.children.small .resources-type-image .image-resource-icon-preview {\n    top: 2px;\n    bottom: 1px;\n    left: 3px;\n    right: 3px;\n    max-width: 8px;\n    max-height: 11px;\n    overflow: hidden;\n}\n\n.resource-sidebar-tree-item.resources-type-document .icon {\n    content: url(Images/resourceDocumentIcon.png);\n}\n\n.children.small .resource-sidebar-tree-item.resources-type-document .icon {\n    content: url(Images/resourceDocumentIconSmall.png);\n}\n\n.resource-sidebar-tree-item.resources-type-stylesheet .icon {\n    content: url(Images/resourceCSSIcon.png);\n}\n\n.children.small .resource-sidebar-tree-item.resources-type-stylesheet .icon {\n    content: url(Images/resourceDocumentIconSmall.png);\n}\n\n.resource-sidebar-tree-item.resources-type-image .icon {\n    position: relative;\n    background-image: url(Images/resourcePlainIcon.png);\n    background-repeat: no-repeat;\n    content: \"\";\n}\n\n.children.small .resource-sidebar-tree-item.resources-type-image .icon {\n    background-image: url(Images/resourcePlainIconSmall.png);\n    content: \"\";\n}\n\n.resource-sidebar-tree-item.resources-type-font .icon {\n    content: url(Images/resourcePlainIcon.png);\n}\n\n.children.small .resource-sidebar-tree-item.resources-type-font .icon {\n    content: url(Images/resourcePlainIconSmall.png);\n}\n\n.resource-sidebar-tree-item.resources-type-script .icon {\n    content: url(Images/resourceJSIcon.png);\n}\n\n.children.small .resource-sidebar-tree-item.resources-type-script .icon {\n    content: url(Images/resourceDocumentIconSmall.png);\n}\n\n.resource-sidebar-tree-item.resources-type-xhr .icon {\n    content: url(Images/resourcePlainIcon.png);\n}\n\n.children.small .resource-sidebar-tree-item.resources-type-xhr .icon {\n    content: url(Images/resourceDocumentIconSmall.png);\n}\n\n.frame-storage-tree-item .icon {\n    content: url(Images/frame.png);\n}\n\n.database-storage-tree-item .icon {\n    content: url(Images/database.png);\n}\n\n.database-table-storage-tree-item .icon {\n    content: url(Images/databaseTable.png);\n}\n\n.indexed-db-storage-tree-item .icon {\n    content: url(Images/indexedDB.png);\n}\n\n.indexed-db-object-store-storage-tree-item .icon {\n    content: url(Images/indexedDBObjectStore.png);\n}\n\n.indexed-db-index-storage-tree-item .icon {\n    content: url(Images/indexedDBIndex.png);\n}\n\n.service-worker-cache-tree-item .icon {\n    content: url(Images/indexedDBObjectStore.png);\n}\n\n.service-worker-cache-storage-tree-item .icon {\n    content: url(Images/indexedDB.png);\n}\n\n.service-workers-tree-item .icon {\n    content: url(Images/serviceWorker.svg);\n}\n\n.domstorage-storage-tree-item.local-storage .icon {\n    content: url(Images/localStorage.png);\n}\n\n.domstorage-storage-tree-item.session-storage .icon {\n    content: url(Images/sessionStorage.png);\n}\n\n.cookie-storage-tree-item .icon {\n    content: url(Images/cookie.png);\n}\n\n.application-cache-storage-tree-item .icon {\n    content: url(Images/applicationCache.png);\n}\n\n.file-system-storage-tree-item .icon {\n    content: url(Images/fileSystem.png);\n}\n\n/*# sourceURL=resources/resourcesPanel.css */";Runtime.cachedResources["resources/serviceWorkerCacheViews.css"]="/*\n * Copyright 2014 The Chromium Authors. All rights reserved.\n * Use of this source code is governed by a BSD-style license that can be\n * found in the LICENSE file.\n */\n\n.service-worker-cache-data-view .data-view-toolbar {\n    position: relative;\n    background-color: #eee;\n    border-bottom: 1px solid #ccc;\n}\n\n.service-worker-cache-data-view .data-view-toolbar .key-input {\n    font-size: 11px;\n    margin: auto 0;\n    width: 200px;\n}\n\n.service-worker-cache-data-view .data-grid {\n    flex: auto;\n}\n\n.service-worker-cache-data-view .data-grid .data-container tr:nth-child(even) {\n    background-color: white;\n}\n\n.service-worker-cache-data-view .data-grid .data-container tr:nth-child(odd) {\n    background-color: #EAF3FF;\n}\n\n.service-worker-cache-data-view .data-grid .data-container tr:nth-last-child(1) {\n    background-color: white;\n}\n\n.service-worker-cache-data-view .data-grid .data-container tr:nth-last-child(1) td {\n    border: 0;\n}\n\n.service-worker-cache-data-view .data-grid .data-container tr:nth-last-child(2) td {\n    border-bottom: 1px solid #aaa;\n}\n\n.service-worker-cache-data-view .data-grid .data-container tr.selected {\n    background-color: rgb(212, 212, 212);\n    color: inherit;\n}\n\n.service-worker-cache-data-view .data-grid:focus .data-container tr.selected {\n    background-color: rgb(56, 121, 217);\n    color: white;\n}\n\n.service-worker-cache-data-view .section,\n.service-worker-cache-data-view .section > .header,\n.service-worker-cache-data-view .section > .header .title {\n    margin: 0;\n    min-height: inherit;\n    line-height: inherit;\n}\n\n.service-worker-cache-data-view .primitive-value {\n    padding-top: 1px;\n}\n\n.service-worker-cache-data-view .data-grid .data-container td .section .header .title {\n    white-space: nowrap;\n    text-overflow: ellipsis;\n    overflow: hidden;\n}\n\n/*# sourceURL=resources/serviceWorkerCacheViews.css */";Runtime.cachedResources["resources/serviceWorkersView.css"]="/*\n * Copyright 2015 The Chromium Authors. All rights reserved.\n * Use of this source code is governed by a BSD-style license that can be\n * found in the LICENSE file.\n */\n\n.service-workers-view {\n    overflow: auto;\n}\n\n.service-workers-settings {\n    border-bottom: 1px solid #e1e1e1;\n    min-height: 26px;\n    overflow: hidden;\n    padding: 5px;\n    white-space: nowrap;\n}\n\n.service-workers-root {\n    margin: 0;\n    padding: 5px;\n}\n\n.service-workers-root ol {\n    list-style-type: none;\n    padding-left: 12px;\n}\n\n.service-workers-origin {\n    padding: 5px;\n}\n\n.service-workers-origin-title {\n    font-size: 20px;\n}\n\n.service-workers-registration {\n    border-right: 1px solid #e1e1e1;\n    margin-top: 5px;\n    margin-bottom: 10px;\n    padding: 0;\n}\n\n.service-workers-registration-header {\n    border-left: 1px solid #e1e1e1;\n    border-top: 1px solid #e1e1e1;\n    display: flex;\n    flex-wrap: wrap;\n    overflow: hidden;\n    position: relative;\n}\n\n.service-workers-registration-title {\n    flex: none;\n    font-size: 15px;\n    padding: 4px;\n}\n\n.service-workers-registration-buttons {\n    flex: 1 1 0;\n    padding: 1px;\n}\n\n.service-workers-registration-content {\n    border: 0;\n    padding: 0;\n}\n\n.service-workers-versions-mode-tab-list {\n    display: flex;\n}\n\n.service-workers-versions-mode-tab {\n    border-bottom: 1px solid #e1e1e1;\n    border-left: 1px solid #e1e1e1;\n    border-top: 1px solid #e1e1e1;\n    display: flex;\n    flex: 1 1 0;\n    flex-wrap: wrap;\n    overflow: hidden;\n    padding: 3px;\n}\n\n.service-workers-versions-mode-tab-disabled {\n    background-color: #eee;\n}\n\n.service-workers-versions-mode-tab-selected {\n    border-bottom: 0;\n}\n\n.service-workers-versions-mode-tab-icon {\n    height: 16px;\n    margin: 1px;\n    -webkit-mask-image: url(Images/serviceWorker.svg);\n    -webkit-mask-size: 16px 16px;\n    width: 16px;\n}\n\n.service-workers-versions-mode-tab-text {\n    margin: 2px;\n}\n\n.service-workers-versions-mode-tab-text-disabled {\n    color: #888;\n}\n\n.service-workers-versions-mode-tab-text-selected {\n    font-weight: bold;\n}\n\n.service-workers-versions-panel {\n    border-bottom: 1px solid #e1e1e1;\n    border-left: 1px solid #e1e1e1;\n    display: flex;\n}\n\n.service-workers-versions-panel-left {\n    flex: 3 3 0;\n    overflow: hidden;\n    padding: 3px;\n}\n.service-workers-versions-panel-right {\n    flex: 5 5 0;\n    padding: 3px;\n}\n\n.service-workers-versions-table-row {\n    display: flex;\n}\n\n.service-workers-versions-table-row-title {\n    color: #888;\n    flex: 1 1 0;\n    overflow: hidden;\n    padding: 8px;\n}\n\n.service-workers-versions-table-row-content {\n    flex: 2 2 0;\n    padding: 8px;\n    text-overflow: ellipsis;\n}\n\n.service-workers-versions-table-messages-title {\n    color: #888;\n    padding: 8px 8px 3px 8px;\n}\n\n.service-workers-versions-table-messages-content {\n    overflow: auto;\n    padding: 3px 3px 3px 10px;\n}\n\n.service-workers-versions-table-clients-title {\n    color: #888;\n    padding: 8px 8px 3px 8px;\n}\n\n.service-workers-versions-table-clients-content {\n    overflow: auto;\n    padding: 3px 3px 3px 10px;\n}\n\n.service-workers-versions-table-worker-running-status-cell {\n    display: flex;\n}\n\n.service-workers-versions-table-running-status-right-cell {\n    padding: 3px;\n}\n\n.service-workers-versions-table-running-status-inspect {\n    color: #00e;\n    cursor: pointer;\n    text-decoration: underline;\n}\n\n.service-workers-button {\n    -webkit-mask-image: url(Images/toolbarButtonGlyphs.png);\n    -webkit-mask-size: 352px 168px;\n    background-color: rgba(0, 0, 0, 0.75);\n    height: 24px;\n    opacity: 0.8;\n    width: 32px;\n    border: 0;\n    padding: 0;\n}\n\n@media (-webkit-min-device-pixel-ratio: 1.5) {\n.service-workers-button {\n    -webkit-mask-image: url(Images/toolbarButtonGlyphs_2x.png);\n}\n} /* media */\n\n.service-workers-button:hover {\n    opacity: 1;\n}\n\n.service-workers-button:active {\n    opacity: 0.8;\n}\n\n.service-workers-button:disabled {\n    opacity: 0.5;\n}\n\n.service-workers-start-button {\n    -webkit-mask-position: -64px -48px;\n}\n\n.service-workers-stop-button {\n    -webkit-mask-position: -96px -48px;\n}\n\n.service-workers-delete-button {\n    float: right;\n    -webkit-mask-position: -128px -24px;\n    position: relative;\n}\n\n.service-workers-update-button {\n    -webkit-mask-image: url(Images/updateServiceWorker.svg);\n    -webkit-mask-position: 7px 3px;\n    -webkit-mask-repeat: no-repeat;\n    -webkit-mask-size: 18px 18px;\n}\n\n.service-workers-push-button {\n    -webkit-mask-image: url(Images/notifications.svg);\n    -webkit-mask-position: 7px 3px;\n    -webkit-mask-repeat: no-repeat;\n    -webkit-mask-size: 18px 18px;\n}\n\n.service-workers-versions-option-panel {\n    border-bottom: 1px solid #e1e1e1;\n    border-left: 1px solid #e1e1e1;\n    display: flex;\n}\n\n.service-workers-error {\n    display: flex;\n}\n\n.service-workers-error-message {\n    font-weight: bold;\n}\n\n.service-workers-info {\n    display: flex;\n}\n\n.service-worker-client-focus {\n   color: #00e;\n   cursor: pointer;\n   padding-left: 4px;\n   text-decoration: underline;\n}\n\n.service-workers-color-0 {\n    background-color: #003366;\n}\n\n.service-workers-color-1 {\n    background-color: #663300;\n}\n\n.service-workers-color-2 {\n    background-color: #660066;\n}\n\n.service-workers-color-3 {\n    background-color: #003300;\n}\n\n.service-workers-color-4 {\n    background-color: #993333;\n}\n\n.service-workers-color-5 {\n    background-color: #000066;\n}\n\n.service-workers-color-6 {\n    background-color: #006666;\n}\n\n.service-workers-color-7 {\n    background-color: #996600;\n}\n\n.service-workers-color-8 {\n    background-color: #9900CC;\n}\n\n.service-workers-color-9 {\n    background-color: #336600;\n}\n\n/*# sourceURL=resources/serviceWorkersView.css */";