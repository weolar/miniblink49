WebInspector.LayerPaintProfilerView=function(showImageForLayerCallback)
{WebInspector.SplitWidget.call(this,true,false);this._showImageForLayerCallback=showImageForLayerCallback;this._logTreeView=new WebInspector.PaintProfilerCommandLogView();this.setSidebarWidget(this._logTreeView);this._paintProfilerView=new WebInspector.PaintProfilerView(this._showImage.bind(this));this.setMainWidget(this._paintProfilerView);this._paintProfilerView.addEventListener(WebInspector.PaintProfilerView.Events.WindowChanged,this._onWindowChanged,this);}
WebInspector.LayerPaintProfilerView.prototype={profileLayer:function(layer)
{this._logTreeView.setCommandLog(null,[]);this._paintProfilerView.setSnapshotAndLog(null,[],null);(layer).requestSnapshot(onSnapshotDone.bind(this));function onSnapshotDone(snapshot)
{this._layer=layer;snapshot.commandLog(onCommandLogDone.bind(this,snapshot));}
function onCommandLogDone(snapshot,log)
{this._logTreeView.setCommandLog(snapshot.target(),log||[]);this._paintProfilerView.setSnapshotAndLog(snapshot||null,log||[],null);}},_onWindowChanged:function()
{var window=this._paintProfilerView.windowBoundaries();this._logTreeView.updateWindow(window.left,window.right);},_showImage:function(imageURL)
{this._showImageForLayerCallback(this._layer,imageURL);},__proto__:WebInspector.SplitWidget.prototype};;WebInspector.LayersPanel=function()
{WebInspector.PanelWithSidebar.call(this,"layers",225);this.registerRequiredCSS("timeline/timelinePanel.css");this._model=null;WebInspector.targetManager.observeTargets(this);this._layerViewHost=new WebInspector.LayerViewHost();this._layerTreeOutline=new WebInspector.LayerTreeOutline(this._layerViewHost);this.panelSidebarElement().appendChild(this._layerTreeOutline.element);this.setDefaultFocusedElement(this._layerTreeOutline.element);this._rightSplitWidget=new WebInspector.SplitWidget(false,true,"layerDetailsSplitViewState");this.splitWidget().setMainWidget(this._rightSplitWidget);this._layers3DView=new WebInspector.Layers3DView(this._layerViewHost);this._rightSplitWidget.setMainWidget(this._layers3DView);this._layers3DView.addEventListener(WebInspector.Layers3DView.Events.LayerSnapshotRequested,this._onSnapshotRequested,this);this._tabbedPane=new WebInspector.TabbedPane();this._rightSplitWidget.setSidebarWidget(this._tabbedPane);this._layerDetailsView=new WebInspector.LayerDetailsView(this._layerViewHost);this._tabbedPane.appendTab(WebInspector.LayersPanel.DetailsViewTabs.Details,WebInspector.UIString("Details"),this._layerDetailsView);this._paintProfilerView=new WebInspector.LayerPaintProfilerView(this._layers3DView.showImageForLayer.bind(this._layers3DView));this._tabbedPane.appendTab(WebInspector.LayersPanel.DetailsViewTabs.Profiler,WebInspector.UIString("Profiler"),this._paintProfilerView);}
WebInspector.LayersPanel.DetailsViewTabs={Details:"details",Profiler:"profiler"};WebInspector.LayersPanel.prototype={focus:function()
{this._layerTreeOutline.focus();},wasShown:function()
{WebInspector.Panel.prototype.wasShown.call(this);if(this._model)
this._model.enable();this._layerTreeOutline.focus();},willHide:function()
{if(this._model)
this._model.disable();WebInspector.Panel.prototype.willHide.call(this);},targetAdded:function(target)
{if(this._model)
return;this._model=WebInspector.LayerTreeModel.fromTarget(target);if(!this._model)
return;this._model.addEventListener(WebInspector.LayerTreeModel.Events.LayerTreeChanged,this._onLayerTreeUpdated,this);this._model.addEventListener(WebInspector.LayerTreeModel.Events.LayerPainted,this._onLayerPainted,this);if(this.isShowing())
this._model.enable();},targetRemoved:function(target)
{if(!this._model||this._model.target()!==target)
return;this._model.removeEventListener(WebInspector.LayerTreeModel.Events.LayerTreeChanged,this._onLayerTreeUpdated,this);this._model.removeEventListener(WebInspector.LayerTreeModel.Events.LayerPainted,this._onLayerPainted,this);this._model.disable();this._model=null;},_showLayerTree:function(deferredLayerTree)
{deferredLayerTree.resolve(this._layerViewHost.setLayerTree.bind(this._layerViewHost));},_onLayerTreeUpdated:function()
{if(this._model)
this._layerViewHost.setLayerTree(this._model.layerTree());},_onLayerPainted:function(event)
{if(!this._model)
return;this._layers3DView.setLayerTree(this._model.layerTree());if(this._layerViewHost.selection()&&this._layerViewHost.selection().layer()===event.data)
this._layerDetailsView.update();},_onSnapshotRequested:function(event)
{var layer=(event.data);this._tabbedPane.selectTab(WebInspector.LayersPanel.DetailsViewTabs.Profiler);this._paintProfilerView.profileLayer(layer);},__proto__:WebInspector.PanelWithSidebar.prototype}
WebInspector.LayersPanel.LayerTreeRevealer=function()
{}
WebInspector.LayersPanel.LayerTreeRevealer.prototype={reveal:function(snapshotData)
{if(!(snapshotData instanceof WebInspector.DeferredLayerTree))
return Promise.reject(new Error("Internal error: not a WebInspector.DeferredLayerTree"));var panel=WebInspector.LayersPanel._instance();WebInspector.inspectorView.setCurrentPanel(panel);panel._showLayerTree((snapshotData));return Promise.resolve();}}
WebInspector.LayersPanel._instance=function()
{if(!WebInspector.LayersPanel._instanceObject)
WebInspector.LayersPanel._instanceObject=new WebInspector.LayersPanel();return WebInspector.LayersPanel._instanceObject;}
WebInspector.LayersPanelFactory=function()
{}
WebInspector.LayersPanelFactory.prototype={createPanel:function()
{return WebInspector.LayersPanel._instance();}};