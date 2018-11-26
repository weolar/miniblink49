WebInspector.AuditsPanel=function()
{WebInspector.PanelWithSidebar.call(this,"audits");this.registerRequiredCSS("ui/panelEnablerView.css");this.registerRequiredCSS("audits/auditsPanel.css");var sidebarTree=new TreeOutline();sidebarTree.element.classList.add("sidebar-tree");this.panelSidebarElement().appendChild(sidebarTree.element);this.setDefaultFocusedElement(sidebarTree.element);this.auditsTreeElement=new WebInspector.SidebarSectionTreeElement("");sidebarTree.appendChild(this.auditsTreeElement);this.auditsTreeElement.listItemElement.classList.add("hidden");this.auditsItemTreeElement=new WebInspector.AuditsSidebarTreeElement(this);this.auditsTreeElement.appendChild(this.auditsItemTreeElement);this.auditResultsTreeElement=new WebInspector.SidebarSectionTreeElement(WebInspector.UIString("RESULTS"));sidebarTree.appendChild(this.auditResultsTreeElement);this.auditResultsTreeElement.expand();this._constructCategories();this._auditController=new WebInspector.AuditController(this);this._launcherView=new WebInspector.AuditLauncherView(this._auditController);for(var id in this.categoriesById)
this._launcherView.addCategory(this.categoriesById[id]);var extensionCategories=WebInspector.extensionServer.auditCategories();for(var i=0;i<extensionCategories.length;++i){var category=extensionCategories[i];this.addCategory(new WebInspector.AuditExtensionCategory(category.extensionOrigin,category.id,category.displayName,category.ruleCount));}
WebInspector.extensionServer.addEventListener(WebInspector.ExtensionServer.Events.AuditCategoryAdded,this._extensionAuditCategoryAdded,this);}
WebInspector.AuditsPanel.prototype={get categoriesById()
{return this._auditCategoriesById;},addCategory:function(category)
{this.categoriesById[category.id]=category;this._launcherView.addCategory(category);},getCategory:function(id)
{return this.categoriesById[id];},_constructCategories:function()
{this._auditCategoriesById={};for(var categoryCtorID in WebInspector.AuditCategories){var auditCategory=new WebInspector.AuditCategories[categoryCtorID]();auditCategory._id=categoryCtorID;this.categoriesById[categoryCtorID]=auditCategory;}},auditFinishedCallback:function(mainResourceURL,results)
{var ordinal=1;for(var child of this.auditResultsTreeElement.children()){if(child.mainResourceURL===mainResourceURL)
ordinal++;}
var resultTreeElement=new WebInspector.AuditResultSidebarTreeElement(this,results,mainResourceURL,ordinal);this.auditResultsTreeElement.appendChild(resultTreeElement);resultTreeElement.revealAndSelect();},showResults:function(categoryResults)
{if(!categoryResults._resultView)
categoryResults._resultView=new WebInspector.AuditResultView(categoryResults);this.visibleView=categoryResults._resultView;},showLauncherView:function()
{this.visibleView=this._launcherView;},get visibleView()
{return this._visibleView;},set visibleView(x)
{if(this._visibleView===x)
return;if(this._visibleView)
this._visibleView.detach();this._visibleView=x;if(x)
this.splitWidget().setMainWidget(x);},wasShown:function()
{WebInspector.Panel.prototype.wasShown.call(this);if(!this._visibleView)
this.auditsItemTreeElement.select();},clearResults:function()
{this.auditsItemTreeElement.revealAndSelect();this.auditResultsTreeElement.removeChildren();},_extensionAuditCategoryAdded:function(event)
{var category=(event.data);this.addCategory(new WebInspector.AuditExtensionCategory(category.extensionOrigin,category.id,category.displayName,category.ruleCount));},__proto__:WebInspector.PanelWithSidebar.prototype}
WebInspector.AuditCategoryImpl=function(displayName)
{this._displayName=displayName;this._rules=[];}
WebInspector.AuditCategoryImpl.prototype={get id()
{return this._id;},get displayName()
{return this._displayName;},addRule:function(rule,severity)
{rule.severity=severity;this._rules.push(rule);},run:function(target,requests,ruleResultCallback,progress)
{this._ensureInitialized();var remainingRulesCount=this._rules.length;progress.setTotalWork(remainingRulesCount);function callbackWrapper(result)
{ruleResultCallback(result);progress.worked();if(!--remainingRulesCount)
progress.done();}
for(var i=0;i<this._rules.length;++i){if(!progress.isCanceled())
this._rules[i].run(target,requests,callbackWrapper,progress);else
callbackWrapper(null);}},_ensureInitialized:function()
{if(!this._initialized){if("initialize"in this)
this.initialize();this._initialized=true;}}}
WebInspector.AuditRule=function(id,displayName)
{this._id=id;this._displayName=displayName;}
WebInspector.AuditRule.Severity={Info:"info",Warning:"warning",Severe:"severe"}
WebInspector.AuditRule.SeverityOrder={"info":3,"warning":2,"severe":1}
WebInspector.AuditRule.prototype={get id()
{return this._id;},get displayName()
{return this._displayName;},set severity(severity)
{this._severity=severity;},run:function(target,requests,callback,progress)
{if(progress.isCanceled())
return;var result=new WebInspector.AuditRuleResult(this.displayName);result.severity=this._severity;this.doRun(target,requests,result,callback,progress);},doRun:function(target,requests,result,callback,progress)
{throw new Error("doRun() not implemented");}}
WebInspector.AuditCategoryResult=function(category)
{this.title=category.displayName;this.ruleResults=[];}
WebInspector.AuditCategoryResult.prototype={addRuleResult:function(ruleResult)
{this.ruleResults.push(ruleResult);}}
WebInspector.AuditRuleResult=function(value,expanded,className)
{this.value=value;this.className=className;this.expanded=expanded;this.violationCount=0;this._formatters={r:WebInspector.AuditRuleResult.linkifyDisplayName};var standardFormatters=Object.keys(String.standardFormatters);for(var i=0;i<standardFormatters.length;++i)
this._formatters[standardFormatters[i]]=String.standardFormatters[standardFormatters[i]];}
WebInspector.AuditRuleResult.linkifyDisplayName=function(url)
{return WebInspector.linkifyURLAsNode(url,WebInspector.displayNameForURL(url));}
WebInspector.AuditRuleResult.resourceDomain=function(domain)
{return domain||WebInspector.UIString("[empty domain]");}
WebInspector.AuditRuleResult.prototype={addChild:function(value,expanded,className)
{if(!this.children)
this.children=[];var entry=new WebInspector.AuditRuleResult(value,expanded,className);this.children.push(entry);return entry;},addURL:function(url)
{this.addChild(WebInspector.AuditRuleResult.linkifyDisplayName(url));},addURLs:function(urls)
{for(var i=0;i<urls.length;++i)
this.addURL(urls[i]);},addSnippet:function(snippet)
{this.addChild(snippet,false,"source-code");},addFormatted:function(format,vararg)
{var substitutions=Array.prototype.slice.call(arguments,1);var fragment=createDocumentFragment();function append(a,b)
{if(!(b instanceof Node))
b=createTextNode(b);a.appendChild(b);return a;}
var formattedResult=String.format(format,substitutions,this._formatters,fragment,append).formattedResult;if(formattedResult instanceof Node)
formattedResult.normalize();return this.addChild(formattedResult);}}
WebInspector.AuditsSidebarTreeElement=function(panel)
{this._panel=panel;this.small=false;WebInspector.SidebarTreeElement.call(this,"audits-sidebar-tree-item",WebInspector.UIString("Audits"));}
WebInspector.AuditsSidebarTreeElement.prototype={onattach:function()
{WebInspector.SidebarTreeElement.prototype.onattach.call(this);},onselect:function()
{this._panel.showLauncherView();return true;},get selectable()
{return true;},refresh:function()
{this.refreshTitles();},__proto__:WebInspector.SidebarTreeElement.prototype}
WebInspector.AuditResultSidebarTreeElement=function(panel,results,mainResourceURL,ordinal)
{this._panel=panel;this.results=results;this.mainResourceURL=mainResourceURL;WebInspector.SidebarTreeElement.call(this,"audit-result-sidebar-tree-item",String.sprintf("%s (%d)",mainResourceURL,ordinal));}
WebInspector.AuditResultSidebarTreeElement.prototype={onselect:function()
{this._panel.showResults(this.results);return true;},get selectable()
{return true;},__proto__:WebInspector.SidebarTreeElement.prototype}
WebInspector.AuditsPanel.show=function()
{WebInspector.inspectorView.setCurrentPanel(WebInspector.AuditsPanel.instance());}
WebInspector.AuditsPanel.instance=function()
{if(!WebInspector.AuditsPanel._instanceObject)
WebInspector.AuditsPanel._instanceObject=new WebInspector.AuditsPanel();return WebInspector.AuditsPanel._instanceObject;}
WebInspector.AuditsPanelFactory=function()
{}
WebInspector.AuditsPanelFactory.prototype={createPanel:function()
{return WebInspector.AuditsPanel.instance();}}
WebInspector.AuditRules={};WebInspector.AuditCategories={};;WebInspector.AuditCategory=function()
{}
WebInspector.AuditCategory.prototype={get id()
{},get displayName()
{},run:function(target,requests,ruleResultCallback,progress)
{}};WebInspector.AuditCategories.PagePerformance=function(){WebInspector.AuditCategoryImpl.call(this,WebInspector.AuditCategories.PagePerformance.AuditCategoryName);}
WebInspector.AuditCategories.PagePerformance.AuditCategoryName=WebInspector.UIString("Web Page Performance");WebInspector.AuditCategories.PagePerformance.prototype={initialize:function()
{this.addRule(new WebInspector.AuditRules.UnusedCssRule(),WebInspector.AuditRule.Severity.Warning);this.addRule(new WebInspector.AuditRules.CssInHeadRule(),WebInspector.AuditRule.Severity.Severe);this.addRule(new WebInspector.AuditRules.StylesScriptsOrderRule(),WebInspector.AuditRule.Severity.Severe);this.addRule(new WebInspector.AuditRules.VendorPrefixedCSSProperties(),WebInspector.AuditRule.Severity.Warning);},__proto__:WebInspector.AuditCategoryImpl.prototype}
WebInspector.AuditCategories.NetworkUtilization=function(){WebInspector.AuditCategoryImpl.call(this,WebInspector.AuditCategories.NetworkUtilization.AuditCategoryName);}
WebInspector.AuditCategories.NetworkUtilization.AuditCategoryName=WebInspector.UIString("Network Utilization");WebInspector.AuditCategories.NetworkUtilization.prototype={initialize:function()
{this.addRule(new WebInspector.AuditRules.GzipRule(),WebInspector.AuditRule.Severity.Severe);this.addRule(new WebInspector.AuditRules.ImageDimensionsRule(),WebInspector.AuditRule.Severity.Warning);this.addRule(new WebInspector.AuditRules.CookieSizeRule(400),WebInspector.AuditRule.Severity.Warning);this.addRule(new WebInspector.AuditRules.StaticCookielessRule(5),WebInspector.AuditRule.Severity.Warning);this.addRule(new WebInspector.AuditRules.CombineJsResourcesRule(2),WebInspector.AuditRule.Severity.Severe);this.addRule(new WebInspector.AuditRules.CombineCssResourcesRule(2),WebInspector.AuditRule.Severity.Severe);this.addRule(new WebInspector.AuditRules.MinimizeDnsLookupsRule(4),WebInspector.AuditRule.Severity.Warning);this.addRule(new WebInspector.AuditRules.ParallelizeDownloadRule(4,10,0.5),WebInspector.AuditRule.Severity.Warning);this.addRule(new WebInspector.AuditRules.BrowserCacheControlRule(),WebInspector.AuditRule.Severity.Severe);},__proto__:WebInspector.AuditCategoryImpl.prototype};WebInspector.AuditController=function(auditsPanel)
{this._auditsPanel=auditsPanel;WebInspector.targetManager.addEventListener(WebInspector.TargetManager.Events.Load,this._didMainResourceLoad,this);WebInspector.targetManager.addModelListener(WebInspector.NetworkManager,WebInspector.NetworkManager.EventTypes.RequestFinished,this._didLoadResource,this);}
WebInspector.AuditController.prototype={_executeAudit:function(target,categories,resultCallback)
{this._progress.setTitle(WebInspector.UIString("Running audit"));function ruleResultReadyCallback(categoryResult,ruleResult)
{if(ruleResult&&ruleResult.children)
categoryResult.addRuleResult(ruleResult);}
var results=[];var mainResourceURL=target.resourceTreeModel.inspectedPageURL();var categoriesDone=0;function categoryDoneCallback()
{if(++categoriesDone!==categories.length)
return;resultCallback(mainResourceURL,results);}
var requests=target.networkLog.requests().slice();var compositeProgress=new WebInspector.CompositeProgress(this._progress);var subprogresses=[];for(var i=0;i<categories.length;++i)
subprogresses.push(new WebInspector.ProgressProxy(compositeProgress.createSubProgress(),categoryDoneCallback));for(var i=0;i<categories.length;++i){if(this._progress.isCanceled()){subprogresses[i].done();continue;}
var category=categories[i];var result=new WebInspector.AuditCategoryResult(category);results.push(result);category.run(target,requests,ruleResultReadyCallback.bind(null,result),subprogresses[i]);}},_auditFinishedCallback:function(mainResourceURL,results)
{if(!this._progress.isCanceled())
this._auditsPanel.auditFinishedCallback(mainResourceURL,results);this._progress.done();},initiateAudit:function(categoryIds,progress,runImmediately,startedCallback)
{var target=WebInspector.targetManager.mainTarget();if(!categoryIds||!categoryIds.length||!target)
return;this._progress=progress;var categories=[];for(var i=0;i<categoryIds.length;++i)
categories.push(this._auditsPanel.categoriesById[categoryIds[i]]);if(runImmediately)
this._startAuditWhenResourcesReady(target,categories,startedCallback);else
this._reloadResources(this._startAuditWhenResourcesReady.bind(this,target,categories,startedCallback));WebInspector.userMetrics.actionTaken(WebInspector.UserMetrics.Action.AuditsStarted);},_startAuditWhenResourcesReady:function(target,categories,startedCallback)
{if(this._progress.isCanceled()){this._progress.done();return;}
startedCallback();this._executeAudit(target,categories,this._auditFinishedCallback.bind(this));},_reloadResources:function(callback)
{this._pageReloadCallback=callback;WebInspector.targetManager.reloadPage();},_didLoadResource:function()
{if(this._pageReloadCallback&&this._progress&&this._progress.isCanceled())
this._pageReloadCallback();},_didMainResourceLoad:function()
{if(this._pageReloadCallback){var callback=this._pageReloadCallback;delete this._pageReloadCallback;callback();}},clearResults:function()
{this._auditsPanel.clearResults();}};WebInspector.AuditFormatters=function()
{}
WebInspector.AuditFormatters.Registry={text:function(text)
{return createTextNode(text);},snippet:function(snippetText)
{var div=createElement("div");div.textContent=snippetText;div.className="source-code";return div;},concat:function()
{var parent=createElement("span");for(var arg=0;arg<arguments.length;++arg)
parent.appendChild(WebInspector.auditFormatters.apply(arguments[arg]));return parent;},url:function(url,displayText)
{return WebInspector.linkifyURLAsNode(url,displayText,undefined,true);},resourceLink:function(url,line)
{return WebInspector.linkifyResourceAsNode(url,line,undefined,"resource-url webkit-html-resource-link");}};WebInspector.AuditFormatters.prototype={apply:function(value)
{var formatter;var type=typeof value;var args;switch(type){case"string":case"boolean":case"number":formatter=WebInspector.AuditFormatters.Registry.text;args=[value.toString()];break;case"object":if(value instanceof Node)
return value;if(Array.isArray(value)){formatter=WebInspector.AuditFormatters.Registry.concat;args=value;}else if(value.type&&value.arguments){formatter=WebInspector.AuditFormatters.Registry[value.type];args=value.arguments;}}
if(!formatter)
throw"Invalid value or formatter: "+type+JSON.stringify(value);return formatter.apply(null,args);},partiallyApply:function(formatters,thisArgument,value)
{if(Array.isArray(value))
return value.map(this.partiallyApply.bind(this,formatters,thisArgument));if(typeof value==="object"&&typeof formatters[value.type]==="function"&&value.arguments)
return formatters[value.type].apply(thisArgument,value.arguments);return value;}}
WebInspector.auditFormatters=new WebInspector.AuditFormatters();;WebInspector.AuditLauncherView=function(auditController)
{WebInspector.VBox.call(this);this.setMinimumSize(100,25);this._auditController=auditController;this._categoryIdPrefix="audit-category-item-";this._auditRunning=false;this.element.classList.add("audit-launcher-view");this.element.classList.add("panel-enabler-view");this._contentElement=createElement("div");this._contentElement.className="audit-launcher-view-content";this.element.appendChild(this._contentElement);this._boundCategoryClickListener=this._categoryClicked.bind(this);this._resetResourceCount();this._sortedCategories=[];this._headerElement=createElement("h1");this._headerElement.className="no-audits";this._headerElement.textContent=WebInspector.UIString("No audits to run");this._contentElement.appendChild(this._headerElement);WebInspector.targetManager.addModelListener(WebInspector.NetworkManager,WebInspector.NetworkManager.EventTypes.RequestStarted,this._onRequestStarted,this);WebInspector.targetManager.addModelListener(WebInspector.NetworkManager,WebInspector.NetworkManager.EventTypes.RequestFinished,this._onRequestFinished,this);var defaultSelectedAuditCategory={};defaultSelectedAuditCategory[WebInspector.AuditLauncherView.AllCategoriesKey]=true;this._selectedCategoriesSetting=WebInspector.settings.createSetting("selectedAuditCategories",defaultSelectedAuditCategory);}
WebInspector.AuditLauncherView.AllCategoriesKey="__AllCategories";WebInspector.AuditLauncherView.prototype={_resetResourceCount:function()
{this._loadedResources=0;this._totalResources=0;},_onRequestStarted:function(event)
{var request=(event.data);if(request.resourceType()===WebInspector.resourceTypes.WebSocket)
return;++this._totalResources;this._updateResourceProgress();},_onRequestFinished:function(event)
{var request=(event.data);if(request.resourceType()===WebInspector.resourceTypes.WebSocket)
return;++this._loadedResources;this._updateResourceProgress();},addCategory:function(category)
{if(!this._sortedCategories.length)
this._createLauncherUI();var selectedCategories=this._selectedCategoriesSetting.get();var categoryElement=this._createCategoryElement(category.displayName,category.id);category._checkboxElement=categoryElement.checkboxElement;if(this._selectAllCheckboxElement.checked||selectedCategories[category.displayName]){category._checkboxElement.checked=true;++this._currentCategoriesCount;}
function compareCategories(a,b)
{var aTitle=a.displayName||"";var bTitle=b.displayName||"";return aTitle.localeCompare(bTitle);}
var insertBefore=insertionIndexForObjectInListSortedByFunction(category,this._sortedCategories,compareCategories);this._categoriesElement.insertBefore(categoryElement,this._categoriesElement.children[insertBefore]);this._sortedCategories.splice(insertBefore,0,category);this._selectedCategoriesUpdated();},_startAudit:function()
{this._auditRunning=true;this._updateButton();this._toggleUIComponents(this._auditRunning);var catIds=[];for(var category=0;category<this._sortedCategories.length;++category){if(this._sortedCategories[category]._checkboxElement.checked)
catIds.push(this._sortedCategories[category].id);}
this._resetResourceCount();this._progressIndicator=new WebInspector.ProgressIndicator();this._buttonContainerElement.appendChild(this._progressIndicator.element);this._displayResourceLoadingProgress=true;function onAuditStarted()
{this._displayResourceLoadingProgress=false;}
this._auditController.initiateAudit(catIds,new WebInspector.ProgressProxy(this._progressIndicator,this._auditsDone.bind(this)),this._auditPresentStateElement.checked,onAuditStarted.bind(this));},_auditsDone:function()
{this._displayResourceLoadingProgress=false;delete this._progressIndicator;this._launchButton.disabled=false;this._auditRunning=false;this._updateButton();this._toggleUIComponents(this._auditRunning);},_toggleUIComponents:function(disable)
{this._selectAllCheckboxElement.disabled=disable;for(var child=this._categoriesElement.firstChild;child;child=child.nextSibling)
child.checkboxElement.disabled=disable;this._auditPresentStateElement.disabled=disable;this._auditReloadedStateElement.disabled=disable;},_launchButtonClicked:function(event)
{if(this._auditRunning){this._launchButton.disabled=true;this._progressIndicator.cancel();return;}
this._startAudit();},_clearButtonClicked:function()
{this._auditController.clearResults();},_selectAllClicked:function(checkCategories,userGesture)
{var childNodes=this._categoriesElement.childNodes;for(var i=0,length=childNodes.length;i<length;++i)
childNodes[i].checkboxElement.checked=checkCategories;this._currentCategoriesCount=checkCategories?this._sortedCategories.length:0;this._selectedCategoriesUpdated(userGesture);},_categoryClicked:function(event)
{this._currentCategoriesCount+=event.target.checked?1:-1;this._selectAllCheckboxElement.checked=this._currentCategoriesCount===this._sortedCategories.length;this._selectedCategoriesUpdated(true);},_createCategoryElement:function(title,id)
{var labelElement=createCheckboxLabel(title);if(id){labelElement.id=this._categoryIdPrefix+id;labelElement.checkboxElement.addEventListener("click",this._boundCategoryClickListener,false);}
labelElement.__displayName=title;return labelElement;},_createLauncherUI:function()
{this._headerElement=createElement("h1");this._headerElement.textContent=WebInspector.UIString("Select audits to run");this._contentElement.removeChildren();this._contentElement.appendChild(this._headerElement);function handleSelectAllClick(event)
{this._selectAllClicked(event.target.checked,true);}
var categoryElement=this._createCategoryElement(WebInspector.UIString("Select All"),"");categoryElement.id="audit-launcher-selectall";this._selectAllCheckboxElement=categoryElement.checkboxElement;this._selectAllCheckboxElement.checked=this._selectedCategoriesSetting.get()[WebInspector.AuditLauncherView.AllCategoriesKey];this._selectAllCheckboxElement.addEventListener("click",handleSelectAllClick.bind(this),false);this._contentElement.appendChild(categoryElement);this._categoriesElement=this._contentElement.createChild("fieldset","audit-categories-container");this._currentCategoriesCount=0;this._contentElement.createChild("div","flexible-space");this._buttonContainerElement=this._contentElement.createChild("div","button-container");var radio=createRadioLabel("audit-mode",WebInspector.UIString("Audit Present State"),true);this._buttonContainerElement.appendChild(radio);this._auditPresentStateElement=radio.radioElement;radio=createRadioLabel("audit-mode",WebInspector.UIString("Reload Page and Audit on Load"));this._buttonContainerElement.appendChild(radio);this._auditReloadedStateElement=radio.radioElement;this._launchButton=createTextButton(WebInspector.UIString("Run"),this._launchButtonClicked.bind(this));this._buttonContainerElement.appendChild(this._launchButton);this._clearButton=createTextButton(WebInspector.UIString("Clear"),this._clearButtonClicked.bind(this));this._buttonContainerElement.appendChild(this._clearButton);this._selectAllClicked(this._selectAllCheckboxElement.checked);},_updateResourceProgress:function()
{if(this._displayResourceLoadingProgress)
this._progressIndicator.setTitle(WebInspector.UIString("Loading (%d of %d)",this._loadedResources,this._totalResources));},_selectedCategoriesUpdated:function(userGesture)
{var selectedCategories=userGesture?{}:this._selectedCategoriesSetting.get();var childNodes=this._categoriesElement.childNodes;for(var i=0,length=childNodes.length;i<length;++i)
selectedCategories[childNodes[i].__displayName]=childNodes[i].checkboxElement.checked;selectedCategories[WebInspector.AuditLauncherView.AllCategoriesKey]=this._selectAllCheckboxElement.checked;this._selectedCategoriesSetting.set(selectedCategories);this._updateButton();},_updateButton:function()
{this._launchButton.textContent=this._auditRunning?WebInspector.UIString("Stop"):WebInspector.UIString("Run");this._launchButton.disabled=!this._currentCategoriesCount;},__proto__:WebInspector.VBox.prototype};WebInspector.AuditResultView=function(categoryResults)
{WebInspector.SidebarPaneStack.call(this);this.setMinimumSize(100,25);this.element.classList.add("audit-result-view");function categorySorter(a,b){return(a.title||"").localeCompare(b.title||"");}
categoryResults.sort(categorySorter);for(var i=0;i<categoryResults.length;++i)
this.addPane(new WebInspector.AuditCategoryResultPane(categoryResults[i]));}
WebInspector.AuditResultView.prototype={__proto__:WebInspector.SidebarPaneStack.prototype}
WebInspector.AuditCategoryResultPane=function(categoryResult)
{WebInspector.SidebarPane.call(this,categoryResult.title);this._treeOutline=new TreeOutlineInShadow();this._treeOutline.registerRequiredCSS("audits/auditResultTree.css");this._treeOutline.element.classList.add("audit-result-tree");this.element.appendChild(this._treeOutline.element);this._treeOutline.expandTreeElementsWhenArrowing=true;function ruleSorter(a,b)
{var result=WebInspector.AuditRule.SeverityOrder[a.severity||0]-WebInspector.AuditRule.SeverityOrder[b.severity||0];if(!result)
result=(a.value||"").localeCompare(b.value||"");return result;}
categoryResult.ruleResults.sort(ruleSorter);for(var i=0;i<categoryResult.ruleResults.length;++i){var ruleResult=categoryResult.ruleResults[i];var treeElement=this._appendResult(this._treeOutline.rootElement(),ruleResult,ruleResult.severity);treeElement.listItemElement.classList.add("audit-result");}
this.expand();}
WebInspector.AuditCategoryResultPane.prototype={_appendResult:function(parentTreeNode,result,severity)
{var title="";if(typeof result.value==="string"){title=result.value;if(result.violationCount)
title=String.sprintf("%s (%d)",title,result.violationCount);}
var titleFragment=createDocumentFragment();if(severity){var severityElement=createElement("div");severityElement.classList.add("severity",severity);titleFragment.appendChild(severityElement);}
titleFragment.createTextChild(title);var treeElement=new TreeElement(titleFragment,!!result.children);treeElement.selectable=false;parentTreeNode.appendChild(treeElement);if(result.className)
treeElement.listItemElement.classList.add(result.className);if(typeof result.value!=="string")
treeElement.listItemElement.appendChild(WebInspector.auditFormatters.apply(result.value));if(result.children){for(var i=0;i<result.children.length;++i)
this._appendResult(treeElement,result.children[i]);}
if(result.expanded){treeElement.listItemElement.classList.remove("parent");treeElement.listItemElement.classList.add("parent-expanded");treeElement.expand();}
return treeElement;},__proto__:WebInspector.SidebarPane.prototype};WebInspector.AuditRules.IPAddressRegexp=/^\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}$/;WebInspector.AuditRules.CacheableResponseCodes={200:true,203:true,206:true,300:true,301:true,410:true,304:true}
WebInspector.AuditRules.getDomainToResourcesMap=function(requests,types,needFullResources)
{var domainToResourcesMap={};for(var i=0,size=requests.length;i<size;++i){var request=requests[i];if(types&&types.indexOf(request.resourceType())===-1)
continue;var parsedURL=request.url.asParsedURL();if(!parsedURL)
continue;var domain=parsedURL.host;var domainResources=domainToResourcesMap[domain];if(domainResources===undefined){domainResources=[];domainToResourcesMap[domain]=domainResources;}
domainResources.push(needFullResources?request:request.url);}
return domainToResourcesMap;}
WebInspector.AuditRules.GzipRule=function()
{WebInspector.AuditRule.call(this,"network-gzip",WebInspector.UIString("Enable gzip compression"));}
WebInspector.AuditRules.GzipRule.prototype={doRun:function(target,requests,result,callback,progress)
{var totalSavings=0;var compressedSize=0;var candidateSize=0;var summary=result.addChild("",true);for(var i=0,length=requests.length;i<length;++i){var request=requests[i];if(request.cached()||request.statusCode===304)
continue;if(this._shouldCompress(request)){var size=request.resourceSize;candidateSize+=size;if(this._isCompressed(request)){compressedSize+=size;continue;}
var savings=2*size/3;totalSavings+=savings;summary.addFormatted("%r could save ~%s",request.url,Number.bytesToString(savings));result.violationCount++;}}
if(!totalSavings){callback(null);return;}
summary.value=WebInspector.UIString("Compressing the following resources with gzip could reduce their transfer size by about two thirds (~%s):",Number.bytesToString(totalSavings));callback(result);},_isCompressed:function(request)
{var encodingHeader=request.responseHeaderValue("Content-Encoding");if(!encodingHeader)
return false;return/\b(?:gzip|deflate)\b/.test(encodingHeader);},_shouldCompress:function(request)
{return request.resourceType().isTextType()&&request.parsedURL.host&&request.resourceSize!==undefined&&request.resourceSize>150;},__proto__:WebInspector.AuditRule.prototype}
WebInspector.AuditRules.CombineExternalResourcesRule=function(id,name,type,resourceTypeName,allowedPerDomain)
{WebInspector.AuditRule.call(this,id,name);this._type=type;this._resourceTypeName=resourceTypeName;this._allowedPerDomain=allowedPerDomain;}
WebInspector.AuditRules.CombineExternalResourcesRule.prototype={doRun:function(target,requests,result,callback,progress)
{var domainToResourcesMap=WebInspector.AuditRules.getDomainToResourcesMap(requests,[this._type],false);var penalizedResourceCount=0;var summary=result.addChild("",true);for(var domain in domainToResourcesMap){var domainResources=domainToResourcesMap[domain];var extraResourceCount=domainResources.length-this._allowedPerDomain;if(extraResourceCount<=0)
continue;penalizedResourceCount+=extraResourceCount-1;summary.addChild(WebInspector.UIString("%d %s resources served from %s.",domainResources.length,this._resourceTypeName,WebInspector.AuditRuleResult.resourceDomain(domain)));result.violationCount+=domainResources.length;}
if(!penalizedResourceCount){callback(null);return;}
summary.value=WebInspector.UIString("There are multiple resources served from same domain. Consider combining them into as few files as possible.");callback(result);},__proto__:WebInspector.AuditRule.prototype}
WebInspector.AuditRules.CombineJsResourcesRule=function(allowedPerDomain){WebInspector.AuditRules.CombineExternalResourcesRule.call(this,"page-externaljs",WebInspector.UIString("Combine external JavaScript"),WebInspector.resourceTypes.Script,"JavaScript",allowedPerDomain);}
WebInspector.AuditRules.CombineJsResourcesRule.prototype={__proto__:WebInspector.AuditRules.CombineExternalResourcesRule.prototype}
WebInspector.AuditRules.CombineCssResourcesRule=function(allowedPerDomain){WebInspector.AuditRules.CombineExternalResourcesRule.call(this,"page-externalcss",WebInspector.UIString("Combine external CSS"),WebInspector.resourceTypes.Stylesheet,"CSS",allowedPerDomain);}
WebInspector.AuditRules.CombineCssResourcesRule.prototype={__proto__:WebInspector.AuditRules.CombineExternalResourcesRule.prototype}
WebInspector.AuditRules.MinimizeDnsLookupsRule=function(hostCountThreshold){WebInspector.AuditRule.call(this,"network-minimizelookups",WebInspector.UIString("Minimize DNS lookups"));this._hostCountThreshold=hostCountThreshold;}
WebInspector.AuditRules.MinimizeDnsLookupsRule.prototype={doRun:function(target,requests,result,callback,progress)
{var summary=result.addChild("");var domainToResourcesMap=WebInspector.AuditRules.getDomainToResourcesMap(requests,null,false);for(var domain in domainToResourcesMap){if(domainToResourcesMap[domain].length>1)
continue;var parsedURL=domain.asParsedURL();if(!parsedURL)
continue;if(!parsedURL.host.search(WebInspector.AuditRules.IPAddressRegexp))
continue;summary.addSnippet(domain);result.violationCount++;}
if(!summary.children||summary.children.length<=this._hostCountThreshold){callback(null);return;}
summary.value=WebInspector.UIString("The following domains only serve one resource each. If possible, avoid the extra DNS lookups by serving these resources from existing domains.");callback(result);},__proto__:WebInspector.AuditRule.prototype}
WebInspector.AuditRules.ParallelizeDownloadRule=function(optimalHostnameCount,minRequestThreshold,minBalanceThreshold)
{WebInspector.AuditRule.call(this,"network-parallelizehosts",WebInspector.UIString("Parallelize downloads across hostnames"));this._optimalHostnameCount=optimalHostnameCount;this._minRequestThreshold=minRequestThreshold;this._minBalanceThreshold=minBalanceThreshold;}
WebInspector.AuditRules.ParallelizeDownloadRule.prototype={doRun:function(target,requests,result,callback,progress)
{function hostSorter(a,b)
{var aCount=domainToResourcesMap[a].length;var bCount=domainToResourcesMap[b].length;return(aCount<bCount)?1:(aCount===bCount)?0:-1;}
var domainToResourcesMap=WebInspector.AuditRules.getDomainToResourcesMap(requests,[WebInspector.resourceTypes.Stylesheet,WebInspector.resourceTypes.Image],true);var hosts=[];for(var url in domainToResourcesMap)
hosts.push(url);if(!hosts.length){callback(null);return;}
hosts.sort(hostSorter);var optimalHostnameCount=this._optimalHostnameCount;if(hosts.length>optimalHostnameCount)
hosts.splice(optimalHostnameCount);var busiestHostResourceCount=domainToResourcesMap[hosts[0]].length;var requestCountAboveThreshold=busiestHostResourceCount-this._minRequestThreshold;if(requestCountAboveThreshold<=0){callback(null);return;}
var avgResourcesPerHost=0;for(var i=0,size=hosts.length;i<size;++i)
avgResourcesPerHost+=domainToResourcesMap[hosts[i]].length;avgResourcesPerHost/=optimalHostnameCount;avgResourcesPerHost=Math.max(avgResourcesPerHost,1);var pctAboveAvg=(requestCountAboveThreshold/avgResourcesPerHost)-1.0;var minBalanceThreshold=this._minBalanceThreshold;if(pctAboveAvg<minBalanceThreshold){callback(null);return;}
var requestsOnBusiestHost=domainToResourcesMap[hosts[0]];var entry=result.addChild(WebInspector.UIString("This page makes %d parallelizable requests to %s. Increase download parallelization by distributing the following requests across multiple hostnames.",busiestHostResourceCount,hosts[0]),true);for(var i=0;i<requestsOnBusiestHost.length;++i)
entry.addURL(requestsOnBusiestHost[i].url);result.violationCount=requestsOnBusiestHost.length;callback(result);},__proto__:WebInspector.AuditRule.prototype}
WebInspector.AuditRules.UnusedCssRule=function()
{WebInspector.AuditRule.call(this,"page-unusedcss",WebInspector.UIString("Remove unused CSS rules"));}
WebInspector.AuditRules.UnusedCssRule.prototype={doRun:function(target,requests,result,callback,progress)
{var domModel=WebInspector.DOMModel.fromTarget(target);var cssModel=WebInspector.CSSStyleModel.fromTarget(target);if(!domModel||!cssModel){callback(null);return;}
function evalCallback(styleSheets){if(!styleSheets.length)
return callback(null);var selectors=[];var testedSelectors={};for(var i=0;i<styleSheets.length;++i){var styleSheet=styleSheets[i];for(var curRule=0;curRule<styleSheet.rules.length;++curRule){var selectorText=styleSheet.rules[curRule].selectorText;if(testedSelectors[selectorText])
continue;selectors.push(selectorText);testedSelectors[selectorText]=1;}}
var foundSelectors={};function selectorsCallback(styleSheets)
{if(progress.isCanceled()){callback(null);return;}
var inlineBlockOrdinal=0;var totalStylesheetSize=0;var totalUnusedStylesheetSize=0;var summary;for(var i=0;i<styleSheets.length;++i){var styleSheet=styleSheets[i];var unusedRules=[];for(var curRule=0;curRule<styleSheet.rules.length;++curRule){var rule=styleSheet.rules[curRule];if(!testedSelectors[rule.selectorText]||foundSelectors[rule.selectorText])
continue;unusedRules.push(rule.selectorText);}
totalStylesheetSize+=styleSheet.rules.length;totalUnusedStylesheetSize+=unusedRules.length;if(!unusedRules.length)
continue;var resource=WebInspector.resourceForURL(styleSheet.sourceURL);var isInlineBlock=resource&&resource.request&&resource.request.resourceType()===WebInspector.resourceTypes.Document;var url=!isInlineBlock?WebInspector.AuditRuleResult.linkifyDisplayName(styleSheet.sourceURL):WebInspector.UIString("Inline block #%d",++inlineBlockOrdinal);var pctUnused=Math.round(100*unusedRules.length/styleSheet.rules.length);if(!summary)
summary=result.addChild("",true);var entry=summary.addFormatted("%s: %d% is not used by the current page.",url,pctUnused);for(var j=0;j<unusedRules.length;++j)
entry.addSnippet(unusedRules[j]);result.violationCount+=unusedRules.length;}
if(!totalUnusedStylesheetSize)
return callback(null);var totalUnusedPercent=Math.round(100*totalUnusedStylesheetSize/totalStylesheetSize);summary.value=WebInspector.UIString("%s rules (%d%) of CSS not used by the current page.",totalUnusedStylesheetSize,totalUnusedPercent);callback(result);}
function queryCallback(boundSelectorsCallback,selector,nodeId)
{if(nodeId)
foundSelectors[selector]=true;if(boundSelectorsCallback)
boundSelectorsCallback();}
function documentLoaded(selectors,document){var pseudoSelectorRegexp=/::?(?:[\w-]+)(?:\(.*?\))?/g;if(!selectors.length){selectorsCallback([]);return;}
for(var i=0;i<selectors.length;++i){if(progress.isCanceled()){callback(null);return;}
var effectiveSelector=selectors[i].replace(pseudoSelectorRegexp,"");domModel.querySelector(document.id,effectiveSelector,queryCallback.bind(null,i===selectors.length-1?selectorsCallback.bind(null,styleSheets):null,selectors[i]));}}
domModel.requestDocument(documentLoaded.bind(null,selectors));}
var styleSheetInfos=cssModel.allStyleSheets();if(!styleSheetInfos||!styleSheetInfos.length){evalCallback([]);return;}
var styleSheetProcessor=new WebInspector.AuditRules.StyleSheetProcessor(styleSheetInfos,progress,evalCallback);styleSheetProcessor.run();},__proto__:WebInspector.AuditRule.prototype}
WebInspector.AuditRules.ParsedStyleSheet;WebInspector.AuditRules.StyleSheetProcessor=function(styleSheetHeaders,progress,styleSheetsParsedCallback)
{this._styleSheetHeaders=styleSheetHeaders;this._progress=progress;this._styleSheets=[];this._styleSheetsParsedCallback=styleSheetsParsedCallback;}
WebInspector.AuditRules.StyleSheetProcessor.prototype={run:function()
{this._parser=new WebInspector.CSSParser();this._processNextStyleSheet();},_terminateWorker:function()
{if(this._parser){this._parser.dispose();delete this._parser;}},_finish:function()
{this._terminateWorker();this._styleSheetsParsedCallback(this._styleSheets);},_processNextStyleSheet:function()
{if(!this._styleSheetHeaders.length){this._finish();return;}
this._currentStyleSheetHeader=this._styleSheetHeaders.shift();this._parser.fetchAndParse(this._currentStyleSheetHeader,this._onStyleSheetParsed.bind(this));},_onStyleSheetParsed:function(rules)
{if(this._progress.isCanceled()){this._finish();return;}
var styleRules=[];for(var i=0;i<rules.length;++i){var rule=rules[i];if(rule.selectorText)
styleRules.push(rule);}
this._styleSheets.push({sourceURL:this._currentStyleSheetHeader.sourceURL,rules:styleRules});this._processNextStyleSheet();},}
WebInspector.AuditRules.CacheControlRule=function(id,name)
{WebInspector.AuditRule.call(this,id,name);}
WebInspector.AuditRules.CacheControlRule.MillisPerMonth=1000*60*60*24*30;WebInspector.AuditRules.CacheControlRule.prototype={doRun:function(target,requests,result,callback,progress)
{var cacheableAndNonCacheableResources=this._cacheableAndNonCacheableResources(requests);if(cacheableAndNonCacheableResources[0].length)
this.runChecks(cacheableAndNonCacheableResources[0],result);this.handleNonCacheableResources(cacheableAndNonCacheableResources[1],result);callback(result);},handleNonCacheableResources:function(requests,result)
{},_cacheableAndNonCacheableResources:function(requests)
{var processedResources=[[],[]];for(var i=0;i<requests.length;++i){var request=requests[i];if(!this.isCacheableResource(request))
continue;if(this._isExplicitlyNonCacheable(request))
processedResources[1].push(request);else
processedResources[0].push(request);}
return processedResources;},execCheck:function(messageText,requestCheckFunction,requests,result)
{var requestCount=requests.length;var urls=[];for(var i=0;i<requestCount;++i){if(requestCheckFunction.call(this,requests[i]))
urls.push(requests[i].url);}
if(urls.length){var entry=result.addChild(messageText,true);entry.addURLs(urls);result.violationCount+=urls.length;}},freshnessLifetimeGreaterThan:function(request,timeMs)
{var dateHeader=this.responseHeader(request,"Date");if(!dateHeader)
return false;var dateHeaderMs=Date.parse(dateHeader);if(isNaN(dateHeaderMs))
return false;var freshnessLifetimeMs;var maxAgeMatch=this.responseHeaderMatch(request,"Cache-Control","max-age=(\\d+)");if(maxAgeMatch)
freshnessLifetimeMs=(maxAgeMatch[1])?1000*maxAgeMatch[1]:0;else{var expiresHeader=this.responseHeader(request,"Expires");if(expiresHeader){var expDate=Date.parse(expiresHeader);if(!isNaN(expDate))
freshnessLifetimeMs=expDate-dateHeaderMs;}}
return(isNaN(freshnessLifetimeMs))?false:freshnessLifetimeMs>timeMs;},responseHeader:function(request,header)
{return request.responseHeaderValue(header);},hasResponseHeader:function(request,header)
{return request.responseHeaderValue(header)!==undefined;},isCompressible:function(request)
{return request.resourceType().isTextType();},isPubliclyCacheable:function(request)
{if(this._isExplicitlyNonCacheable(request))
return false;if(this.responseHeaderMatch(request,"Cache-Control","public"))
return true;return request.url.indexOf("?")===-1&&!this.responseHeaderMatch(request,"Cache-Control","private");},responseHeaderMatch:function(request,header,regexp)
{return request.responseHeaderValue(header)?request.responseHeaderValue(header).match(new RegExp(regexp,"im")):null;},hasExplicitExpiration:function(request)
{return this.hasResponseHeader(request,"Date")&&(this.hasResponseHeader(request,"Expires")||!!this.responseHeaderMatch(request,"Cache-Control","max-age"));},_isExplicitlyNonCacheable:function(request)
{var hasExplicitExp=this.hasExplicitExpiration(request);return!!this.responseHeaderMatch(request,"Cache-Control","(no-cache|no-store)")||!!this.responseHeaderMatch(request,"Pragma","no-cache")||(hasExplicitExp&&!this.freshnessLifetimeGreaterThan(request,0))||(!hasExplicitExp&&!!request.url&&request.url.indexOf("?")>=0)||(!hasExplicitExp&&!this.isCacheableResource(request));},isCacheableResource:function(request)
{return request.statusCode!==undefined&&WebInspector.AuditRules.CacheableResponseCodes[request.statusCode];},__proto__:WebInspector.AuditRule.prototype}
WebInspector.AuditRules.BrowserCacheControlRule=function()
{WebInspector.AuditRules.CacheControlRule.call(this,"http-browsercache",WebInspector.UIString("Leverage browser caching"));}
WebInspector.AuditRules.BrowserCacheControlRule.prototype={handleNonCacheableResources:function(requests,result)
{if(requests.length){var entry=result.addChild(WebInspector.UIString("The following resources are explicitly non-cacheable. Consider making them cacheable if possible:"),true);result.violationCount+=requests.length;for(var i=0;i<requests.length;++i)
entry.addURL(requests[i].url);}},runChecks:function(requests,result,callback)
{this.execCheck(WebInspector.UIString("The following resources are missing a cache expiration. Resources that do not specify an expiration may not be cached by browsers:"),this._missingExpirationCheck,requests,result);this.execCheck(WebInspector.UIString("The following resources specify a \"Vary\" header that disables caching in most versions of Internet Explorer:"),this._varyCheck,requests,result);this.execCheck(WebInspector.UIString("The following cacheable resources have a short freshness lifetime:"),this._oneMonthExpirationCheck,requests,result);this.execCheck(WebInspector.UIString("To further improve cache hit rate, specify an expiration one year in the future for the following cacheable resources:"),this._oneYearExpirationCheck,requests,result);},_missingExpirationCheck:function(request)
{return this.isCacheableResource(request)&&!this.hasResponseHeader(request,"Set-Cookie")&&!this.hasExplicitExpiration(request);},_varyCheck:function(request)
{var varyHeader=this.responseHeader(request,"Vary");if(varyHeader){varyHeader=varyHeader.replace(/User-Agent/gi,"");varyHeader=varyHeader.replace(/Accept-Encoding/gi,"");varyHeader=varyHeader.replace(/[, ]*/g,"");}
return varyHeader&&varyHeader.length&&this.isCacheableResource(request)&&this.freshnessLifetimeGreaterThan(request,0);},_oneMonthExpirationCheck:function(request)
{return this.isCacheableResource(request)&&!this.hasResponseHeader(request,"Set-Cookie")&&!this.freshnessLifetimeGreaterThan(request,WebInspector.AuditRules.CacheControlRule.MillisPerMonth)&&this.freshnessLifetimeGreaterThan(request,0);},_oneYearExpirationCheck:function(request)
{return this.isCacheableResource(request)&&!this.hasResponseHeader(request,"Set-Cookie")&&!this.freshnessLifetimeGreaterThan(request,11*WebInspector.AuditRules.CacheControlRule.MillisPerMonth)&&this.freshnessLifetimeGreaterThan(request,WebInspector.AuditRules.CacheControlRule.MillisPerMonth);},__proto__:WebInspector.AuditRules.CacheControlRule.prototype}
WebInspector.AuditRules.ImageDimensionsRule=function()
{WebInspector.AuditRule.call(this,"page-imagedims",WebInspector.UIString("Specify image dimensions"));}
WebInspector.AuditRules.ImageDimensionsRule.prototype={doRun:function(target,requests,result,callback,progress)
{var domModel=WebInspector.DOMModel.fromTarget(target);var cssModel=WebInspector.CSSStyleModel.fromTarget(target);if(!domModel||!cssModel){callback(null);return;}
var urlToNoDimensionCount={};function doneCallback()
{for(var url in urlToNoDimensionCount){var entry=entry||result.addChild(WebInspector.UIString("A width and height should be specified for all images in order to speed up page display. The following image(s) are missing a width and/or height:"),true);var format="%r";if(urlToNoDimensionCount[url]>1)
format+=" (%d uses)";entry.addFormatted(format,url,urlToNoDimensionCount[url]);result.violationCount++;}
callback(entry?result:null);}
function imageStylesReady(imageId,styles)
{if(progress.isCanceled()){callback(null);return;}
const node=domModel.nodeForId(imageId);var src=node.getAttribute("src");if(!src.asParsedURL()){for(var frameOwnerCandidate=node;frameOwnerCandidate;frameOwnerCandidate=frameOwnerCandidate.parentNode){if(frameOwnerCandidate.baseURL){var completeSrc=WebInspector.ParsedURL.completeURL(frameOwnerCandidate.baseURL,src);break;}}}
if(completeSrc)
src=completeSrc;if(styles.computedStyle.get("position")==="absolute")
return;var widthFound=false;var heightFound=false;for(var i=0;!(widthFound&&heightFound)&&i<styles.nodeStyles.length;++i){var style=styles.nodeStyles[i];if(style.getPropertyValue("width")!=="")
widthFound=true;if(style.getPropertyValue("height")!=="")
heightFound=true;}
if(!widthFound||!heightFound){if(src in urlToNoDimensionCount)
++urlToNoDimensionCount[src];else
urlToNoDimensionCount[src]=1;}}
function getStyles(nodeIds)
{if(progress.isCanceled()){callback(null);return;}
var targetResult={};function matchedCallback(matchedStyleResult)
{if(!matchedStyleResult)
return;targetResult.nodeStyles=matchedStyleResult.nodeStyles();}
function computedCallback(computedStyle)
{targetResult.computedStyle=computedStyle;}
if(!nodeIds||!nodeIds.length)
doneCallback();var nodePromises=[];for(var i=0;nodeIds&&i<nodeIds.length;++i){var stylePromises=[cssModel.matchedStylesPromise(nodeIds[i]).then(matchedCallback),cssModel.computedStylePromise(nodeIds[i]).then(computedCallback)];var nodePromise=Promise.all(stylePromises).then(imageStylesReady.bind(null,nodeIds[i],targetResult));nodePromises.push(nodePromise);}
Promise.all(nodePromises).catchException(null).then(doneCallback);}
function onDocumentAvailable(root)
{if(progress.isCanceled()){callback(null);return;}
domModel.querySelectorAll(root.id,"img[src]",getStyles);}
if(progress.isCanceled()){callback(null);return;}
domModel.requestDocument(onDocumentAvailable);},__proto__:WebInspector.AuditRule.prototype}
WebInspector.AuditRules.CssInHeadRule=function()
{WebInspector.AuditRule.call(this,"page-cssinhead",WebInspector.UIString("Put CSS in the document head"));}
WebInspector.AuditRules.CssInHeadRule.prototype={doRun:function(target,requests,result,callback,progress)
{var domModel=WebInspector.DOMModel.fromTarget(target);if(!domModel){callback(null);return;}
function evalCallback(evalResult)
{if(progress.isCanceled()){callback(null);return;}
if(!evalResult)
return callback(null);var summary=result.addChild("");for(var url in evalResult){var urlViolations=evalResult[url];if(urlViolations[0]){result.addFormatted("%s style block(s) in the %r body should be moved to the document head.",urlViolations[0],url);result.violationCount+=urlViolations[0];}
for(var i=0;i<urlViolations[1].length;++i)
result.addFormatted("Link node %r should be moved to the document head in %r",urlViolations[1][i],url);result.violationCount+=urlViolations[1].length;}
summary.value=WebInspector.UIString("CSS in the document body adversely impacts rendering performance.");callback(result);}
function externalStylesheetsReceived(root,inlineStyleNodeIds,nodeIds)
{if(progress.isCanceled()){callback(null);return;}
if(!nodeIds)
return;var externalStylesheetNodeIds=nodeIds;var result=null;if(inlineStyleNodeIds.length||externalStylesheetNodeIds.length){var urlToViolationsArray={};var externalStylesheetHrefs=[];for(var j=0;j<externalStylesheetNodeIds.length;++j){var linkNode=domModel.nodeForId(externalStylesheetNodeIds[j]);var completeHref=WebInspector.ParsedURL.completeURL(linkNode.ownerDocument.baseURL,linkNode.getAttribute("href"));externalStylesheetHrefs.push(completeHref||"<empty>");}
urlToViolationsArray[root.documentURL]=[inlineStyleNodeIds.length,externalStylesheetHrefs];result=urlToViolationsArray;}
evalCallback(result);}
function inlineStylesReceived(root,nodeIds)
{if(progress.isCanceled()){callback(null);return;}
if(!nodeIds)
return;domModel.querySelectorAll(root.id,"body link[rel~='stylesheet'][href]",externalStylesheetsReceived.bind(null,root,nodeIds));}
function onDocumentAvailable(root)
{if(progress.isCanceled()){callback(null);return;}
domModel.querySelectorAll(root.id,"body style",inlineStylesReceived.bind(null,root));}
domModel.requestDocument(onDocumentAvailable);},__proto__:WebInspector.AuditRule.prototype}
WebInspector.AuditRules.StylesScriptsOrderRule=function()
{WebInspector.AuditRule.call(this,"page-stylescriptorder",WebInspector.UIString("Optimize the order of styles and scripts"));}
WebInspector.AuditRules.StylesScriptsOrderRule.prototype={doRun:function(target,requests,result,callback,progress)
{var domModel=WebInspector.DOMModel.fromTarget(target);if(!domModel){callback(null);return;}
function evalCallback(resultValue)
{if(progress.isCanceled()){callback(null);return;}
if(!resultValue)
return callback(null);var lateCssUrls=resultValue[0];var cssBeforeInlineCount=resultValue[1];if(lateCssUrls.length){var entry=result.addChild(WebInspector.UIString("The following external CSS files were included after an external JavaScript file in the document head. To ensure CSS files are downloaded in parallel, always include external CSS before external JavaScript."),true);entry.addURLs(lateCssUrls);result.violationCount+=lateCssUrls.length;}
if(cssBeforeInlineCount){result.addChild(WebInspector.UIString(" %d inline script block%s found in the head between an external CSS file and another resource. To allow parallel downloading, move the inline script before the external CSS file, or after the next resource.",cssBeforeInlineCount,cssBeforeInlineCount>1?"s were":" was"));result.violationCount+=cssBeforeInlineCount;}
callback(result);}
function cssBeforeInlineReceived(lateStyleIds,nodeIds)
{if(progress.isCanceled()){callback(null);return;}
if(!nodeIds)
return;var cssBeforeInlineCount=nodeIds.length;var result=null;if(lateStyleIds.length||cssBeforeInlineCount){var lateStyleUrls=[];for(var i=0;i<lateStyleIds.length;++i){var lateStyleNode=domModel.nodeForId(lateStyleIds[i]);var completeHref=WebInspector.ParsedURL.completeURL(lateStyleNode.ownerDocument.baseURL,lateStyleNode.getAttribute("href"));lateStyleUrls.push(completeHref||"<empty>");}
result=[lateStyleUrls,cssBeforeInlineCount];}
evalCallback(result);}
function lateStylesReceived(root,nodeIds)
{if(progress.isCanceled()){callback(null);return;}
if(!nodeIds)
return;domModel.querySelectorAll(root.id,"head link[rel~='stylesheet'][href] ~ script:not([src])",cssBeforeInlineReceived.bind(null,nodeIds));}
function onDocumentAvailable(root)
{if(progress.isCanceled()){callback(null);return;}
domModel.querySelectorAll(root.id,"head script[src] ~ link[rel~='stylesheet'][href]",lateStylesReceived.bind(null,root));}
domModel.requestDocument(onDocumentAvailable);},__proto__:WebInspector.AuditRule.prototype}
WebInspector.AuditRules.CSSRuleBase=function(id,name)
{WebInspector.AuditRule.call(this,id,name);}
WebInspector.AuditRules.CSSRuleBase.prototype={doRun:function(target,requests,result,callback,progress)
{var cssModel=WebInspector.CSSStyleModel.fromTarget(target);if(!cssModel){callback(null);return;}
var headers=cssModel.allStyleSheets();if(!headers.length){callback(null);return;}
var activeHeaders=[];for(var i=0;i<headers.length;++i){if(!headers[i].disabled)
activeHeaders.push(headers[i]);}
var styleSheetProcessor=new WebInspector.AuditRules.StyleSheetProcessor(activeHeaders,progress,this._styleSheetsLoaded.bind(this,result,callback,progress));styleSheetProcessor.run();},_styleSheetsLoaded:function(result,callback,progress,styleSheets)
{for(var i=0;i<styleSheets.length;++i)
this._visitStyleSheet(styleSheets[i],result);callback(result);},_visitStyleSheet:function(styleSheet,result)
{this.visitStyleSheet(styleSheet,result);for(var i=0;i<styleSheet.rules.length;++i)
this._visitRule(styleSheet,styleSheet.rules[i],result);this.didVisitStyleSheet(styleSheet,result);},_visitRule:function(styleSheet,rule,result)
{this.visitRule(styleSheet,rule,result);var allProperties=rule.properties;for(var i=0;i<allProperties.length;++i)
this.visitProperty(styleSheet,rule,allProperties[i],result);this.didVisitRule(styleSheet,rule,result);},visitStyleSheet:function(styleSheet,result)
{},didVisitStyleSheet:function(styleSheet,result)
{},visitRule:function(styleSheet,rule,result)
{},didVisitRule:function(styleSheet,rule,result)
{},visitProperty:function(styleSheet,rule,property,result)
{},__proto__:WebInspector.AuditRule.prototype}
WebInspector.AuditRules.VendorPrefixedCSSProperties=function()
{WebInspector.AuditRules.CSSRuleBase.call(this,"page-vendorprefixedcss",WebInspector.UIString("Use normal CSS property names instead of vendor-prefixed ones"));this._webkitPrefix="-webkit-";}
WebInspector.AuditRules.VendorPrefixedCSSProperties.supportedProperties=["background-clip","background-origin","background-size","border-radius","border-bottom-left-radius","border-bottom-right-radius","border-top-left-radius","border-top-right-radius","box-shadow","box-sizing","opacity","text-shadow"].keySet();WebInspector.AuditRules.VendorPrefixedCSSProperties.prototype={didVisitStyleSheet:function(styleSheet)
{delete this._styleSheetResult;},visitRule:function()
{this._mentionedProperties={};},didVisitRule:function()
{delete this._ruleResult;delete this._mentionedProperties;},visitProperty:function(styleSheet,rule,property,result)
{if(!property.name.startsWith(this._webkitPrefix))
return;var normalPropertyName=property.name.substring(this._webkitPrefix.length).toLowerCase();if(WebInspector.AuditRules.VendorPrefixedCSSProperties.supportedProperties[normalPropertyName]&&!this._mentionedProperties[normalPropertyName]){this._mentionedProperties[normalPropertyName]=true;if(!this._styleSheetResult)
this._styleSheetResult=result.addChild(styleSheet.sourceURL?WebInspector.linkifyResourceAsNode(styleSheet.sourceURL):WebInspector.UIString("<unknown>"));if(!this._ruleResult){var anchor=WebInspector.linkifyURLAsNode(styleSheet.sourceURL,rule.selectorText);anchor.lineNumber=rule.lineNumber;this._ruleResult=this._styleSheetResult.addChild(anchor);}
++result.violationCount;this._ruleResult.addSnippet(WebInspector.UIString("\"%s%s\" is used, but \"%s\" is supported.",this._webkitPrefix,normalPropertyName,normalPropertyName));}},__proto__:WebInspector.AuditRules.CSSRuleBase.prototype}
WebInspector.AuditRules.CookieRuleBase=function(id,name)
{WebInspector.AuditRule.call(this,id,name);}
WebInspector.AuditRules.CookieRuleBase.prototype={doRun:function(target,requests,result,callback,progress)
{var self=this;function resultCallback(receivedCookies)
{if(progress.isCanceled()){callback(result);return;}
self.processCookies(receivedCookies,requests,result);callback(result);}
WebInspector.Cookies.getCookiesAsync(resultCallback);},mapResourceCookies:function(requestsByDomain,allCookies,callback)
{for(var i=0;i<allCookies.length;++i){for(var requestDomain in requestsByDomain){if(WebInspector.Cookies.cookieDomainMatchesResourceDomain(allCookies[i].domain(),requestDomain))
this._callbackForResourceCookiePairs(requestsByDomain[requestDomain],allCookies[i],callback);}}},_callbackForResourceCookiePairs:function(requests,cookie,callback)
{if(!requests)
return;for(var i=0;i<requests.length;++i){if(WebInspector.Cookies.cookieMatchesResourceURL(cookie,requests[i].url))
callback(requests[i],cookie);}},__proto__:WebInspector.AuditRule.prototype}
WebInspector.AuditRules.CookieSizeRule=function(avgBytesThreshold)
{WebInspector.AuditRules.CookieRuleBase.call(this,"http-cookiesize",WebInspector.UIString("Minimize cookie size"));this._avgBytesThreshold=avgBytesThreshold;this._maxBytesThreshold=1000;}
WebInspector.AuditRules.CookieSizeRule.prototype={_average:function(cookieArray)
{var total=0;for(var i=0;i<cookieArray.length;++i)
total+=cookieArray[i].size();return cookieArray.length?Math.round(total/cookieArray.length):0;},_max:function(cookieArray)
{var result=0;for(var i=0;i<cookieArray.length;++i)
result=Math.max(cookieArray[i].size(),result);return result;},processCookies:function(allCookies,requests,result)
{function maxSizeSorter(a,b)
{return b.maxCookieSize-a.maxCookieSize;}
function avgSizeSorter(a,b)
{return b.avgCookieSize-a.avgCookieSize;}
var cookiesPerResourceDomain={};function collectorCallback(request,cookie)
{var cookies=cookiesPerResourceDomain[request.parsedURL.host];if(!cookies){cookies=[];cookiesPerResourceDomain[request.parsedURL.host]=cookies;}
cookies.push(cookie);}
if(!allCookies.length)
return;var sortedCookieSizes=[];var domainToResourcesMap=WebInspector.AuditRules.getDomainToResourcesMap(requests,null,true);this.mapResourceCookies(domainToResourcesMap,allCookies,collectorCallback);for(var requestDomain in cookiesPerResourceDomain){var cookies=cookiesPerResourceDomain[requestDomain];sortedCookieSizes.push({domain:requestDomain,avgCookieSize:this._average(cookies),maxCookieSize:this._max(cookies)});}
var avgAllCookiesSize=this._average(allCookies);var hugeCookieDomains=[];sortedCookieSizes.sort(maxSizeSorter);for(var i=0,len=sortedCookieSizes.length;i<len;++i){var maxCookieSize=sortedCookieSizes[i].maxCookieSize;if(maxCookieSize>this._maxBytesThreshold)
hugeCookieDomains.push(WebInspector.AuditRuleResult.resourceDomain(sortedCookieSizes[i].domain)+": "+Number.bytesToString(maxCookieSize));}
var bigAvgCookieDomains=[];sortedCookieSizes.sort(avgSizeSorter);for(var i=0,len=sortedCookieSizes.length;i<len;++i){var domain=sortedCookieSizes[i].domain;var avgCookieSize=sortedCookieSizes[i].avgCookieSize;if(avgCookieSize>this._avgBytesThreshold&&avgCookieSize<this._maxBytesThreshold)
bigAvgCookieDomains.push(WebInspector.AuditRuleResult.resourceDomain(domain)+": "+Number.bytesToString(avgCookieSize));}
result.addChild(WebInspector.UIString("The average cookie size for all requests on this page is %s",Number.bytesToString(avgAllCookiesSize)));if(hugeCookieDomains.length){var entry=result.addChild(WebInspector.UIString("The following domains have a cookie size in excess of 1KB. This is harmful because requests with cookies larger than 1KB typically cannot fit into a single network packet."),true);entry.addURLs(hugeCookieDomains);result.violationCount+=hugeCookieDomains.length;}
if(bigAvgCookieDomains.length){var entry=result.addChild(WebInspector.UIString("The following domains have an average cookie size in excess of %d bytes. Reducing the size of cookies for these domains can reduce the time it takes to send requests.",this._avgBytesThreshold),true);entry.addURLs(bigAvgCookieDomains);result.violationCount+=bigAvgCookieDomains.length;}},__proto__:WebInspector.AuditRules.CookieRuleBase.prototype}
WebInspector.AuditRules.StaticCookielessRule=function(minResources)
{WebInspector.AuditRules.CookieRuleBase.call(this,"http-staticcookieless",WebInspector.UIString("Serve static content from a cookieless domain"));this._minResources=minResources;}
WebInspector.AuditRules.StaticCookielessRule.prototype={processCookies:function(allCookies,requests,result)
{var domainToResourcesMap=WebInspector.AuditRules.getDomainToResourcesMap(requests,[WebInspector.resourceTypes.Stylesheet,WebInspector.resourceTypes.Image],true);var totalStaticResources=0;for(var domain in domainToResourcesMap)
totalStaticResources+=domainToResourcesMap[domain].length;if(totalStaticResources<this._minResources)
return;var matchingResourceData={};this.mapResourceCookies(domainToResourcesMap,allCookies,this._collectorCallback.bind(this,matchingResourceData));var badUrls=[];var cookieBytes=0;for(var url in matchingResourceData){badUrls.push(url);cookieBytes+=matchingResourceData[url];}
if(badUrls.length<this._minResources)
return;var entry=result.addChild(WebInspector.UIString("%s of cookies were sent with the following static resources. Serve these static resources from a domain that does not set cookies:",Number.bytesToString(cookieBytes)),true);entry.addURLs(badUrls);result.violationCount=badUrls.length;},_collectorCallback:function(matchingResourceData,request,cookie)
{matchingResourceData[request.url]=(matchingResourceData[request.url]||0)+cookie.size();},__proto__:WebInspector.AuditRules.CookieRuleBase.prototype};WebInspector.AuditExtensionCategory=function(extensionOrigin,id,displayName,ruleCount)
{this._extensionOrigin=extensionOrigin;this._id=id;this._displayName=displayName;this._ruleCount=ruleCount;}
WebInspector.AuditExtensionCategory.prototype={get id()
{return this._id;},get displayName()
{return this._displayName;},run:function(target,requests,ruleResultCallback,progress)
{var results=new WebInspector.AuditExtensionCategoryResults(this,target,ruleResultCallback,progress);WebInspector.extensionServer.startAuditRun(this.id,results);}}
WebInspector.AuditExtensionCategoryResults=function(category,target,ruleResultCallback,progress)
{this._target=target;this._category=category;this._ruleResultCallback=ruleResultCallback;this._progress=progress;this._progress.setTotalWork(1);this._expectedResults=category._ruleCount;this._actualResults=0;this._id=category.id+"-"+ ++WebInspector.AuditExtensionCategoryResults._lastId;}
WebInspector.AuditExtensionCategoryResults.prototype={id:function()
{return this._id;},done:function()
{WebInspector.extensionServer.stopAuditRun(this);this._progress.done();},addResult:function(displayName,description,severity,details)
{var result=new WebInspector.AuditRuleResult(displayName);if(description)
result.addChild(description);result.severity=severity;if(details)
this._addNode(result,details);this._addResult(result);},_addNode:function(parent,node)
{var contents=WebInspector.auditFormatters.partiallyApply(WebInspector.AuditExtensionFormatters,this,node.contents);var addedNode=parent.addChild(contents,node.expanded);if(node.children){for(var i=0;i<node.children.length;++i)
this._addNode(addedNode,node.children[i]);}},_addResult:function(result)
{this._ruleResultCallback(result);++this._actualResults;if(typeof this._expectedResults==="number"){this._progress.setWorked(this._actualResults/this._expectedResults);if(this._actualResults===this._expectedResults)
this.done();}},updateProgress:function(progress)
{this._progress.setWorked(progress);},evaluate:function(expression,evaluateOptions,callback)
{function onEvaluate(error,result,wasThrown)
{if(wasThrown)
return;var object=this._target.runtimeModel.createRemoteObject(result);callback(object);}
var evaluateCallback=(onEvaluate.bind(this));WebInspector.extensionServer.evaluate(expression,false,false,evaluateOptions,this._category._extensionOrigin,evaluateCallback);}}
WebInspector.AuditExtensionFormatters={object:function(expression,title,evaluateOptions)
{var parentElement=createElement("div");function onEvaluate(remoteObject)
{var section=new WebInspector.ObjectPropertiesSection(remoteObject,title);section.expand();section.editable=false;parentElement.appendChild(section.element);}
this.evaluate(expression,evaluateOptions,onEvaluate);return parentElement;},node:function(expression,evaluateOptions)
{var parentElement=createElement("div");this.evaluate(expression,evaluateOptions,onEvaluate);function onEvaluate(remoteObject)
{WebInspector.Renderer.renderPromise(remoteObject).then(appendRenderer).then(remoteObject.release.bind(remoteObject));function appendRenderer(element)
{parentElement.appendChild(element);}}
return parentElement;}}
WebInspector.AuditExtensionCategoryResults._lastId=0;;Runtime.cachedResources["audits/auditsPanel.css"]="/*\n * Copyright (C) 2008 Apple Inc.  All rights reserved.\n * Copyright (C) 2009 Google Inc. All rights reserved.\n *\n * Redistribution and use in source and binary forms, with or without\n * modification, are permitted provided that the following conditions are\n * met:\n *\n *     * Redistributions of source code must retain the above copyright\n * notice, this list of conditions and the following disclaimer.\n *     * Redistributions in binary form must reproduce the above\n * copyright notice, this list of conditions and the following disclaimer\n * in the documentation and/or other materials provided with the\n * distribution.\n *     * Neither the name of Google Inc. nor the names of its\n * contributors may be used to endorse or promote products derived from\n * this software without specific prior written permission.\n *\n * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS\n * \"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT\n * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR\n * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT\n * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,\n * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT\n * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,\n * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY\n * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT\n * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE\n * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n */\n\n.audits-sidebar-tree-item .icon {\n    content: url(Images/resourcesTimeGraphIcon.png);\n}\n\n.audit-result-sidebar-tree-item .icon {\n    content: url(Images/resourceDocumentIcon.png);\n}\n\n.audit-launcher-view .audit-launcher-view-content {\n    padding: 0 0 0 16px;\n    white-space: nowrap;\n    display: -webkit-flex;\n    text-align: left;\n    -webkit-flex-direction: column;\n    flex: auto;\n}\n\n.audit-launcher-view h1 {\n    padding-top: 15px;\n    -webkit-flex: none;\n}\n\n.audit-launcher-view h1.no-audits {\n    text-align: center;\n    font-style: italic;\n    position: relative;\n    left: -8px;\n}\n\n.audit-launcher-view div.button-container {\n    width: 100%;\n    padding: 16px 0;\n    -webkit-flex: none;\n}\n\n.audit-launcher-view div.button-container > button {\n    -webkit-align-self: flex-start;\n    margin-right: 10px;\n    margin-bottom: 5px;\n    margin-top: 5px;\n}\n\n.audit-launcher-view fieldset.audit-categories-container {\n    position: relative;\n    top: 11px;\n    left: 0;\n    width: 100%;\n    overflow-y: auto;\n    border: 0 none;\n    -webkit-flex: none;\n}\n\n.audit-launcher-view button {\n    margin: 0 5px 0 0;\n}\n\n.panel-enabler-view.audit-launcher-view label {\n    padding: 0 0 5px 0;\n    margin: 0;\n    display: flex;\n    flex-shrink: 0;\n}\n\n.panel-enabler-view.audit-launcher-view label.disabled {\n    color: rgb(130, 130, 130);\n}\n\n.audit-launcher-view input[type=\"checkbox\"] {\n    margin-left: 0;\n    height: 14px;\n    width: 14px;\n}\n\n.audit-result-view {\n    overflow: auto;\n    display: block;\n    flex: auto;\n}\n\n.audit-result-tree {\n    margin: 0 0 3px;\n}\n\n.audit-launcher-view .progress-indicator {\n    display: inline-block;\n}\n\n.resource-url {\n    float: right;\n    text-align: right;\n    max-width: 100%;\n    margin-left: 4px;\n}\n\n/*# sourceURL=audits/auditsPanel.css */";Runtime.cachedResources["audits/auditResultTree.css"]="/*\n * Copyright 2015 The Chromium Authors. All rights reserved.\n * Use of this source code is governed by a BSD-style license that can be\n * found in the LICENSE file.\n */\n\n.severity {\n    background-image: url(Images/toolbarButtonGlyphs.png);\n    background-size: 352px 168px;\n    display: inline-block;\n    width: 10px;\n    height: 10px;\n    position: relative;\n    top: 1px;\n    margin-right: 4px;\n}\n\nli {\n    -webkit-user-select: text;\n}\n\n@media (-webkit-min-device-pixel-ratio: 1.5) {\n.severity {\n    background-image: url(Images/toolbarButtonGlyphs_2x.png);\n}\n} /* media */\n\n.severity.severe {\n    background-position: -224px -96px;\n}\n\n.severity.warning {\n    background-position: -246px -96px;\n}\n\n.severity.info {\n    background-position: -235px -96px;\n}\n.audit-result {\n    font-weight: bold;\n}\n\n/*# sourceURL=audits/auditResultTree.css */";