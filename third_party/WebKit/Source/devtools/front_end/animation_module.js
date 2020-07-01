WebInspector.AnimationModel=function(target)
{WebInspector.SDKModel.call(this,WebInspector.AnimationModel,target);this._agent=target.animationAgent();target.registerAnimationDispatcher(new WebInspector.AnimationDispatcher(this));this._animationsById=new Map();this._animationGroups=new Map();this._pendingAnimations=[];target.resourceTreeModel.addEventListener(WebInspector.ResourceTreeModel.EventTypes.MainFrameNavigated,this._mainFrameNavigated,this);}
WebInspector.AnimationModel.Events={AnimationGroupStarted:"AnimationGroupStarted"}
WebInspector.AnimationModel.prototype={_mainFrameNavigated:function()
{this._animationsById.clear();this._animationGroups.clear();this._pendingAnimations=[];},animationCreated:function(id)
{this._pendingAnimations.push(id);},_animationCanceled:function(id)
{this._pendingAnimations.remove(id);this._flushPendingAnimationsIfNeeded();},animationStarted:function(payload)
{var animation=WebInspector.AnimationModel.Animation.parsePayload(this.target(),payload);if(animation.type()==="WebAnimation"&&animation.source().keyframesRule().keyframes().length===0){this._pendingAnimations.remove(animation.id());}else{this._animationsById.set(animation.id(),animation);if(this._pendingAnimations.indexOf(animation.id())===-1)
this._pendingAnimations.push(animation.id());}
this._flushPendingAnimationsIfNeeded();},_flushPendingAnimationsIfNeeded:function()
{for(var id of this._pendingAnimations){if(!this._animationsById.get(id))
return;}
while(this._pendingAnimations.length)
this._matchExistingGroups(this._createGroupFromPendingAnimations());},_matchExistingGroups:function(incomingGroup)
{var matchedGroup=null;for(var group of this._animationGroups.values()){if(group._matches(incomingGroup)){matchedGroup=group;group._update(incomingGroup);break;}}
if(!matchedGroup)
this._animationGroups.set(incomingGroup.id(),incomingGroup);this.dispatchEventToListeners(WebInspector.AnimationModel.Events.AnimationGroupStarted,matchedGroup||incomingGroup);return!!matchedGroup;},_createGroupFromPendingAnimations:function()
{console.assert(this._pendingAnimations.length);var groupedAnimations=[this._animationsById.get(this._pendingAnimations.shift())];var remainingAnimations=[];for(var id of this._pendingAnimations){var anim=this._animationsById.get(id);if(anim.startTime()===groupedAnimations[0].startTime())
groupedAnimations.push(anim);else
remainingAnimations.push(id);}
this._pendingAnimations=remainingAnimations;return new WebInspector.AnimationModel.AnimationGroup(this.target(),groupedAnimations[0].id(),groupedAnimations);},playbackRatePromise:function()
{function callback(error,playbackRate)
{if(error)
return 1;return playbackRate;}
return this._agent.getPlaybackRate(callback).catchException(1);},setPlaybackRate:function(playbackRate)
{this._agent.setPlaybackRate(playbackRate);},ensureEnabled:function()
{if(this._enabled)
return;this._agent.enable();this._enabled=true;},__proto__:WebInspector.SDKModel.prototype}
WebInspector.AnimationModel._symbol=Symbol("AnimationModel");WebInspector.AnimationModel.fromTarget=function(target)
{if(!target[WebInspector.AnimationModel._symbol])
target[WebInspector.AnimationModel._symbol]=new WebInspector.AnimationModel(target);return target[WebInspector.AnimationModel._symbol];}
WebInspector.AnimationModel.Animation=function(target,payload)
{WebInspector.SDKObject.call(this,target);this._payload=payload;this._source=new WebInspector.AnimationModel.AnimationEffect(this.target(),this._payload.source);}
WebInspector.AnimationModel.Animation.parsePayload=function(target,payload)
{return new WebInspector.AnimationModel.Animation(target,payload);}
WebInspector.AnimationModel.Animation.Type={CSSTransition:"CSSTransition",CSSAnimation:"CSSAnimation",WebAnimation:"WebAnimation"}
WebInspector.AnimationModel.Animation.prototype={payload:function()
{return this._payload;},id:function()
{return this._payload.id;},name:function()
{return this.source().name();},paused:function()
{return this._payload.pausedState;},playState:function()
{return this._playState||this._payload.playState;},setPlayState:function(playState)
{this._playState=playState;},playbackRate:function()
{return this._payload.playbackRate;},startTime:function()
{return this._payload.startTime;},endTime:function()
{if(!this.source().iterations)
return Infinity;return this.startTime()+this.source().delay()+this.source().duration()*this.source().iterations()+this.source().endDelay();},currentTime:function()
{return this._payload.currentTime;},source:function()
{return this._source;},type:function()
{return(this._payload.type);},overlaps:function(animation)
{if(!this.source().iterations()||!animation.source().iterations())
return true;var firstAnimation=this.startTime()<animation.startTime()?this:animation;var secondAnimation=firstAnimation===this?animation:this;return firstAnimation.endTime()>=secondAnimation.startTime();},setTiming:function(duration,delay)
{this._source.node().then(this._updateNodeStyle.bind(this,duration,delay));this._source._duration=duration;this._source._delay=delay;if(this.type()!==WebInspector.AnimationModel.Animation.Type.CSSAnimation)
this.target().animationAgent().setTiming(this.id(),duration,delay);},_updateNodeStyle:function(duration,delay,node)
{var animationPrefix;if(this.type()==WebInspector.AnimationModel.Animation.Type.CSSTransition)
animationPrefix="transition-";else if(this.type()==WebInspector.AnimationModel.Animation.Type.CSSAnimation)
animationPrefix="animation-";else
return;var cssModel=WebInspector.CSSStyleModel.fromTarget(node.target());if(!cssModel)
return;cssModel.setEffectivePropertyValueForNode(node.id,animationPrefix+"duration",duration+"ms");cssModel.setEffectivePropertyValueForNode(node.id,animationPrefix+"delay",delay+"ms");},remoteObjectPromise:function()
{function callback(error,payload)
{return!error?this.target().runtimeModel.createRemoteObject(payload):null;}
return this.target().animationAgent().resolveAnimation(this.id(),callback.bind(this));},_cssId:function()
{return this._payload.cssId||"";},__proto__:WebInspector.SDKObject.prototype}
WebInspector.AnimationModel.AnimationEffect=function(target,payload)
{WebInspector.SDKObject.call(this,target);this._payload=payload;if(payload.keyframesRule)
this._keyframesRule=new WebInspector.AnimationModel.KeyframesRule(target,payload.keyframesRule);this._delay=this._payload.delay;this._duration=this._payload.duration;}
WebInspector.AnimationModel.AnimationEffect.prototype={delay:function()
{return this._delay;},endDelay:function()
{return this._payload.endDelay;},playbackRate:function()
{return this._payload.playbackRate;},iterationStart:function()
{return this._payload.iterationStart;},iterations:function()
{if(!this.delay()&&!this.endDelay()&&!this.duration())
return 0;return this._payload.iterations||Infinity;},duration:function()
{return this._duration;},direction:function()
{return this._payload.direction;},fill:function()
{return this._payload.fill;},name:function()
{return this._payload.name;},node:function()
{if(!this._deferredNode)
this._deferredNode=new WebInspector.DeferredDOMNode(this.target(),this.backendNodeId());return this._deferredNode.resolvePromise();},deferredNode:function()
{return new WebInspector.DeferredDOMNode(this.target(),this.backendNodeId());},backendNodeId:function()
{return this._payload.backendNodeId;},keyframesRule:function()
{return this._keyframesRule;},easing:function()
{return this._payload.easing;},__proto__:WebInspector.SDKObject.prototype}
WebInspector.AnimationModel.KeyframesRule=function(target,payload)
{WebInspector.SDKObject.call(this,target);this._payload=payload;this._keyframes=this._payload.keyframes.map(function(keyframeStyle){return new WebInspector.AnimationModel.KeyframeStyle(target,keyframeStyle);});}
WebInspector.AnimationModel.KeyframesRule.prototype={_setKeyframesPayload:function(payload)
{this._keyframes=payload.map(function(keyframeStyle){return new WebInspector.AnimationModel.KeyframeStyle(this._target,keyframeStyle);});},name:function()
{return this._payload.name;},keyframes:function()
{return this._keyframes;},__proto__:WebInspector.SDKObject.prototype}
WebInspector.AnimationModel.KeyframeStyle=function(target,payload)
{WebInspector.SDKObject.call(this,target);this._payload=payload;this._offset=this._payload.offset;}
WebInspector.AnimationModel.KeyframeStyle.prototype={offset:function()
{return this._offset;},setOffset:function(offset)
{this._offset=offset*100+"%";},offsetAsNumber:function()
{return parseFloat(this._offset)/100;},easing:function()
{return this._payload.easing;},__proto__:WebInspector.SDKObject.prototype}
WebInspector.AnimationModel.AnimationGroup=function(target,id,animations)
{WebInspector.SDKObject.call(this,target);this._id=id;this._animations=animations;this._paused=false;}
WebInspector.AnimationModel.AnimationGroup.prototype={id:function()
{return this._id;},animations:function()
{return this._animations;},_animationIds:function()
{function extractId(animation)
{return animation.id();}
return this._animations.map(extractId);},startTime:function()
{return this._animations[0].startTime();},seekTo:function(currentTime)
{this.target().animationAgent().seekAnimations(this._animationIds(),currentTime);},paused:function()
{return this._paused;},togglePause:function(paused)
{this._paused=paused;this.target().animationAgent().setPaused(this._animationIds(),paused);},currentTimePromise:function()
{function callback(error,currentTime)
{return!error?currentTime:0;}
return this.target().animationAgent().getCurrentTime(this._animations[0].id(),callback).catchException(0);},_matches:function(group)
{function extractId(anim)
{if(anim.type()===WebInspector.AnimationModel.Animation.Type.WebAnimation)
return anim.type()+anim.id();else
return anim._cssId();}
if(this._animations.length!==group._animations.length)
return false;var left=this._animations.map(extractId).sort();var right=group._animations.map(extractId).sort();for(var i=0;i<left.length;i++){if(left[i]!==right[i])
return false;}
return true;},_update:function(group)
{this._animations=group._animations;},__proto__:WebInspector.SDKObject.prototype}
WebInspector.AnimationDispatcher=function(animationModel)
{this._animationModel=animationModel;}
WebInspector.AnimationDispatcher.prototype={animationCreated:function(id)
{this._animationModel.animationCreated(id);},animationCanceled:function(id)
{this._animationModel._animationCanceled(id);},animationStarted:function(payload)
{this._animationModel.animationStarted(payload);}};WebInspector.AnimationGroupPreviewUI=function(model)
{this._model=model;this.element=createElementWithClass("div","animation-buffer-preview");this._replayOverlayElement=this.element.createChild("div","animation-buffer-preview-animation");this._svg=this.element.createSVGChild("svg");this._svg.setAttribute("width","100%");this._svg.setAttribute("preserveAspectRatio","none");this._svg.setAttribute("height","100%");this._viewBoxHeight=32;this._svg.setAttribute("viewBox","0 0 100 "+this._viewBoxHeight);this._svg.setAttribute("shape-rendering","crispEdges");this._render();}
WebInspector.AnimationGroupPreviewUI.prototype={_groupDuration:function()
{var duration=0;for(var anim of this._model.animations()){var animDuration=anim.source().delay()+anim.source().duration();if(animDuration>duration)
duration=animDuration;}
return duration;},replay:function()
{this._replayOverlayElement.animate([{offset:0,width:"0%",opacity:1},{offset:0.9,width:"100%",opacity:1},{offset:1,width:"100%",opacity:0}],{duration:200,easing:"cubic-bezier(0, 0, 0.2, 1)"});},_render:function()
{this._svg.removeChildren();var maxToShow=10;var numberOfAnimations=Math.min(this._model.animations().length,maxToShow);var timeToPixelRatio=100/Math.max(this._groupDuration(),300);for(var i=0;i<numberOfAnimations;i++){var effect=this._model.animations()[i].source();var line=this._svg.createSVGChild("line");line.setAttribute("x1",effect.delay()*timeToPixelRatio);line.setAttribute("x2",(effect.delay()+effect.duration())*timeToPixelRatio);var y=Math.floor(this._viewBoxHeight/Math.max(6,numberOfAnimations)*i+1);line.setAttribute("y1",y);line.setAttribute("y2",y);line.style.stroke=WebInspector.AnimationUI.Color(this._model.animations()[i]);}}};WebInspector.AnimationTimeline=function()
{WebInspector.VBox.call(this,true);this.registerRequiredCSS("animation/animationTimeline.css");this.element.classList.add("animations-timeline");this._grid=this.contentElement.createSVGChild("svg","animation-timeline-grid");this._underlyingPlaybackRate=1;this._createHeader();this._animationsContainer=this.contentElement.createChild("div","animation-timeline-rows");this._duration=this._defaultDuration();this._scrubberRadius=25;this._timelineControlsWidth=175;this._nodesMap=new Map();this._groupBuffer=[];this._groupBufferSize=8;this._previewMap=new Map();this._symbol=Symbol("animationTimeline");this._animationsMap=new Map();WebInspector.targetManager.addModelListener(WebInspector.ResourceTreeModel,WebInspector.ResourceTreeModel.EventTypes.MainFrameNavigated,this._mainFrameNavigated,this);WebInspector.targetManager.addModelListener(WebInspector.DOMModel,WebInspector.DOMModel.Events.NodeRemoved,this._nodeRemoved,this);WebInspector.targetManager.observeTargets(this,WebInspector.Target.Type.Page);}
WebInspector.AnimationTimeline.GlobalPlaybackRates=[0.1,0.25,0.5,1.0];WebInspector.AnimationTimeline.prototype={wasShown:function()
{for(var target of WebInspector.targetManager.targets(WebInspector.Target.Type.Page))
this._addEventListeners(target);},willHide:function()
{for(var target of WebInspector.targetManager.targets(WebInspector.Target.Type.Page))
this._removeEventListeners(target);},targetAdded:function(target)
{if(this.isShowing())
this._addEventListeners(target);},targetRemoved:function(target)
{this._removeEventListeners(target);},_addEventListeners:function(target)
{var animationModel=WebInspector.AnimationModel.fromTarget(target);animationModel.ensureEnabled();animationModel.addEventListener(WebInspector.AnimationModel.Events.AnimationGroupStarted,this._animationGroupStarted,this);},_removeEventListeners:function(target)
{var animationModel=WebInspector.AnimationModel.fromTarget(target);animationModel.removeEventListener(WebInspector.AnimationModel.Events.AnimationGroupStarted,this._animationGroupStarted,this);},setNode:function(node)
{for(var nodeUI of this._nodesMap.values())
nodeUI.setNode(node);},_createScrubber:function(){this._timelineScrubber=createElementWithClass("div","animation-scrubber hidden");this._timelineScrubber.createChild("div","animation-time-overlay");this._timelineScrubber.createChild("div","animation-scrubber-arrow");this._timelineScrubberHead=this._timelineScrubber.createChild("div","animation-scrubber-head");var timerContainer=this._timelineScrubber.createChild("div","animation-timeline-timer");this._timerSpinner=timerContainer.createChild("div","timer-spinner timer-hemisphere");this._timerFiller=timerContainer.createChild("div","timer-filler timer-hemisphere");this._timerMask=timerContainer.createChild("div","timer-mask");return this._timelineScrubber;},_createHeader:function()
{function playbackSliderInputHandler(event)
{this._underlyingPlaybackRate=WebInspector.AnimationTimeline.GlobalPlaybackRates[event.target.value];this._updatePlaybackControls();}
this._previewContainer=this.contentElement.createChild("div","animation-timeline-buffer");var container=this.contentElement.createChild("div","animation-timeline-header");var controls=container.createChild("div","animation-controls");var toolbar=new WebInspector.Toolbar(controls);toolbar.element.classList.add("animation-controls-toolbar");this._controlButton=new WebInspector.ToolbarButton(WebInspector.UIString("Replay timeline"),"replay-outline-toolbar-item");this._controlButton.addEventListener("click",this._controlButtonToggle.bind(this));toolbar.appendToolbarItem(this._controlButton);this._playbackLabel=controls.createChild("span","animation-playback-label");this._playbackLabel.createTextChild("1x");this._playbackLabel.addEventListener("keydown",this._playbackLabelInput.bind(this));this._playbackLabel.addEventListener("focusout",this._playbackLabelInput.bind(this));this._playbackSlider=controls.createChild("input","animation-playback-slider");this._playbackSlider.type="range";this._playbackSlider.min=0;this._playbackSlider.max=WebInspector.AnimationTimeline.GlobalPlaybackRates.length-1;this._playbackSlider.value=this._playbackSlider.max;this._playbackSlider.addEventListener("input",playbackSliderInputHandler.bind(this));this._updateAnimationsPlaybackRate();var gridHeader=container.createChild("div","animation-grid-header");WebInspector.installDragHandle(gridHeader,this._repositionScrubber.bind(this),this._scrubberDragMove.bind(this),this._scrubberDragEnd.bind(this),"pointer");container.appendChild(this._createScrubber());WebInspector.installDragHandle(this._timelineScrubberHead,this._scrubberDragStart.bind(this),this._scrubberDragMove.bind(this),this._scrubberDragEnd.bind(this),"move");this._timelineScrubberHead.textContent=WebInspector.UIString(Number.millisToString(0));return container;},_playbackLabelInput:function(event)
{var element=(event.currentTarget);if(event.type!=="focusout"&&!WebInspector.handleElementValueModifications(event,element)&&!isEnterKey(event))
return;var value=parseFloat(this._playbackLabel.textContent);if(!isNaN(value))
this._underlyingPlaybackRate=Math.max(0,value);this._updatePlaybackControls();event.consume(true);},_updatePlaybackControls:function()
{this._playbackLabel.textContent=this._underlyingPlaybackRate+"x";var playbackSliderValue=0;for(var rate of WebInspector.AnimationTimeline.GlobalPlaybackRates){if(this._underlyingPlaybackRate>rate)
playbackSliderValue++;}
this._playbackSlider.value=playbackSliderValue;var target=WebInspector.targetManager.mainTarget();if(target)
WebInspector.AnimationModel.fromTarget(target).setPlaybackRate(this._underlyingPlaybackRate);WebInspector.userMetrics.actionTaken(WebInspector.UserMetrics.Action.AnimationsPlaybackRateChanged);if(this._scrubberPlayer)
this._scrubberPlayer.playbackRate=this._effectivePlaybackRate();},_controlButtonToggle:function()
{if(this._controlButton.element.classList.contains("play-outline-toolbar-item"))
this._togglePause(false);else if(this._controlButton.element.classList.contains("replay-outline-toolbar-item"))
this._replay();else
this._togglePause(true);this._updateControlButton();},_updateControlButton:function()
{this._controlButton.setEnabled(!!this._selectedGroup);this._controlButton.element.classList.remove("play-outline-toolbar-item");this._controlButton.element.classList.remove("replay-outline-toolbar-item");this._controlButton.element.classList.remove("pause-outline-toolbar-item");if(this._selectedGroup&&this._selectedGroup.paused()){this._controlButton.element.classList.add("play-outline-toolbar-item");this._controlButton.setTitle(WebInspector.UIString("Play timeline"));this._controlButton.setToggled(true);}else if(!this._scrubberPlayer||this._scrubberPlayer.currentTime>=this.duration()-this._scrubberRadius/this.pixelMsRatio()){this._controlButton.element.classList.add("replay-outline-toolbar-item");this._controlButton.setTitle(WebInspector.UIString("Replay timeline"));this._controlButton.setToggled(true);}else{this._controlButton.element.classList.add("pause-outline-toolbar-item");this._controlButton.setTitle(WebInspector.UIString("Pause timeline"));this._controlButton.setToggled(false);}},_updateAnimationsPlaybackRate:function()
{function syncPlaybackRate(playbackRate)
{this._underlyingPlaybackRate=playbackRate;this._updatePlaybackControls();}
for(var target of WebInspector.targetManager.targets(WebInspector.Target.Type.Page))
WebInspector.AnimationModel.fromTarget(target).playbackRatePromise().then(syncPlaybackRate.bind(this));},_effectivePlaybackRate:function()
{return this._selectedGroup&&this._selectedGroup.paused()?0:this._underlyingPlaybackRate;},_togglePause:function(pause)
{this._selectedGroup.togglePause(pause);if(this._scrubberPlayer)
this._scrubberPlayer.playbackRate=this._effectivePlaybackRate();},_replay:function()
{if(!this._selectedGroup)
return;this._selectedGroup.seekTo(0);this._animateTime(0);this._updateControlButton();},_defaultDuration:function()
{return 100;},duration:function()
{return this._duration;},setDuration:function(duration)
{this._duration=duration;this.scheduleRedraw();},startTime:function()
{return this._startTime;},_reset:function()
{if(!this._nodesMap.size)
return;this._nodesMap.clear();this._animationsMap.clear();this._animationsContainer.removeChildren();this._duration=this._defaultDuration();delete this._startTime;},_mainFrameNavigated:function(event)
{this._reset();this._updateAnimationsPlaybackRate();if(this._underlyingPlaybackRate===0){this._underlyingPlaybackRate=1;this._updatePlaybackControls();}
if(this._scrubberPlayer)
this._scrubberPlayer.cancel();delete this._scrubberPlayer;this._timelineScrubberHead.textContent=WebInspector.UIString(Number.millisToString(0));this._updateControlButton();this._groupBuffer=[];this._previewMap.clear();this._previewContainer.removeChildren();},_animationGroupStarted:function(event)
{this._addAnimationGroup((event.data));},_addAnimationGroup:function(group)
{function startTimeComparator(left,right)
{return left.startTime()>right.startTime();}
if(this._previewMap.get(group)){if(this._selectedGroup===group)
this._syncScrubber();else
this._previewMap.get(group).replay();return;}
this._groupBuffer.push(group);this._groupBuffer.sort(startTimeComparator);var groupsToDiscard=[];while(this._groupBuffer.length>this._groupBufferSize){var toDiscard=this._groupBuffer.splice(this._groupBuffer[0]===this._selectedGroup?1:0,1);groupsToDiscard.push(toDiscard[0]);}
for(var g of groupsToDiscard){this._previewMap.get(g).element.remove();this._previewMap.delete(g);}
var preview=new WebInspector.AnimationGroupPreviewUI(group);this._previewMap.set(group,preview);this._previewContainer.appendChild(preview.element);preview.element.addEventListener("click",this._selectAnimationGroup.bind(this,group));},_selectAnimationGroup:function(group)
{function applySelectionClass(ui,group)
{ui.element.classList.toggle("selected",this._selectedGroup===group);}
if(this._selectedGroup===group){this._replay();return;}
this._selectedGroup=group;this._previewMap.forEach(applySelectionClass,this);this._reset();for(var anim of group.animations())
this._addAnimation(anim);this.scheduleRedraw();this._timelineScrubber.classList.remove("hidden");this._replay();},_addAnimation:function(animation)
{function nodeResolved(node)
{if(!node)
return;uiAnimation.setNode(node);node[this._symbol]=nodeUI;}
this._resizeWindow(animation);var nodeUI=this._nodesMap.get(animation.source().backendNodeId());if(!nodeUI){nodeUI=new WebInspector.AnimationTimeline.NodeUI(animation.source());this._animationsContainer.appendChild(nodeUI.element);this._nodesMap.set(animation.source().backendNodeId(),nodeUI);}
var nodeRow=nodeUI.findRow(animation);var uiAnimation=new WebInspector.AnimationUI(animation,this,nodeRow.element);animation.source().deferredNode().resolve(nodeResolved.bind(this));nodeRow.animations.push(uiAnimation);this._animationsMap.set(animation.id(),animation);},_nodeRemoved:function(event)
{var node=event.data.node;if(node[this._symbol])
node[this._symbol].nodeRemoved();},_renderGrid:function()
{const gridSize=250;this._grid.setAttribute("width",this.width());this._grid.setAttribute("height",this._animationsContainer.offsetHeight+30);this._grid.setAttribute("shape-rendering","crispEdges");this._grid.removeChildren();var lastDraw=undefined;for(var time=0;time<this.duration();time+=gridSize){var line=this._grid.createSVGChild("rect","animation-timeline-grid-line");line.setAttribute("x",time*this.pixelMsRatio());line.setAttribute("y",23);line.setAttribute("height","100%");line.setAttribute("width",1);}
for(var time=0;time<this.duration();time+=gridSize){var gridWidth=time*this.pixelMsRatio();if(!lastDraw||gridWidth-lastDraw>50){lastDraw=gridWidth;var label=this._grid.createSVGChild("text","animation-timeline-grid-label");label.setAttribute("x",gridWidth+5);label.setAttribute("y",16);label.textContent=WebInspector.UIString(Number.millisToString(time));}}},scheduleRedraw:function(){if(this._redrawing)
return;this._redrawing=true;this._animationsContainer.window().requestAnimationFrame(this._redraw.bind(this));},_redraw:function(timestamp)
{delete this._redrawing;for(var nodeUI of this._nodesMap.values())
nodeUI.redraw();this._renderGrid();},onResize:function()
{this._cachedTimelineWidth=Math.max(0,this._animationsContainer.offsetWidth-this._timelineControlsWidth)||0;this.scheduleRedraw();if(this._scrubberPlayer)
this._syncScrubber();},width:function()
{return this._cachedTimelineWidth||0;},_resizeWindow:function(animation)
{var resized=false;if(!this._startTime)
this._startTime=animation.startTime();var duration=animation.source().duration()*Math.min(3,animation.source().iterations());var requiredDuration=animation.startTime()+animation.source().delay()+duration+animation.source().endDelay()-this.startTime();if(requiredDuration>this._duration*0.8){resized=true;this._duration=requiredDuration*1.5;}
return resized;},_syncScrubber:function()
{if(!this._selectedGroup)
return;this._selectedGroup.currentTimePromise().then(this._animateTime.bind(this)).then(this._updateControlButton.bind(this));},_animateTime:function(currentTime)
{if(this._scrubberPlayer)
this._scrubberPlayer.cancel();var scrubberDuration=this.duration()-this._scrubberRadius/this.pixelMsRatio();this._scrubberPlayer=this._timelineScrubber.animate([{transform:"translateX(0px)"},{transform:"translateX("+(this.width()-this._scrubberRadius)+"px)"}],{duration:scrubberDuration,fill:"forwards"});this._scrubberPlayer.playbackRate=this._effectivePlaybackRate();this._scrubberPlayer.onfinish=this._updateControlButton.bind(this);this._scrubberPlayer.currentTime=currentTime;this._timelineScrubber.classList.remove("animation-timeline-end");this._timelineScrubberHead.window().requestAnimationFrame(this._updateScrubber.bind(this));},pixelMsRatio:function()
{return this.width()/this.duration()||0;},_updateScrubber:function(timestamp)
{if(!this._scrubberPlayer)
return;this._timelineScrubberHead.textContent=WebInspector.UIString(Number.millisToString(this._scrubberPlayer.currentTime));if(this._scrubberPlayer.playState==="pending"||this._scrubberPlayer.playState==="running"){this._timelineScrubberHead.window().requestAnimationFrame(this._updateScrubber.bind(this));}else if(this._scrubberPlayer.playState==="finished"){this._timelineScrubberHead.textContent=WebInspector.UIString(". . .");this._timelineScrubber.classList.add("animation-timeline-end");}},_repositionScrubber:function(event)
{if(!this._selectedGroup)
return false;if(!this._gridOffsetLeft)
this._gridOffsetLeft=this._grid.totalOffsetLeft();var seekTime=Math.max(0,event.x-this._gridOffsetLeft)/this.pixelMsRatio();this._selectedGroup.seekTo(seekTime);this._togglePause(true);this._animateTime(seekTime);this._updateControlButton();this._originalScrubberTime=seekTime;this._originalMousePosition=event.x;return true;},_scrubberDragStart:function(event)
{if(!this._scrubberPlayer||!this._selectedGroup)
return false;this._originalScrubberTime=this._scrubberPlayer.currentTime;this._timelineScrubber.classList.remove("animation-timeline-end");this._scrubberPlayer.pause();this._originalMousePosition=event.x;this._togglePause(true);this._updateControlButton();return true;},_scrubberDragMove:function(event)
{var delta=event.x-this._originalMousePosition;var currentTime=Math.max(0,Math.min(this._originalScrubberTime+delta/this.pixelMsRatio(),this.duration()-this._scrubberRadius/this.pixelMsRatio()));this._scrubberPlayer.currentTime=currentTime;this._timelineScrubberHead.textContent=WebInspector.UIString(Number.millisToString(Math.round(currentTime)));this._selectedGroup.seekTo(currentTime);},_scrubberDragEnd:function(event)
{var currentTime=Math.max(0,this._scrubberPlayer.currentTime);this._scrubberPlayer.play();this._scrubberPlayer.currentTime=currentTime;this._timelineScrubberHead.window().requestAnimationFrame(this._updateScrubber.bind(this));},__proto__:WebInspector.VBox.prototype}
WebInspector.AnimationTimeline.NodeUI=function(animationEffect)
{function nodeResolved(node)
{if(!node)
return;this._node=node;WebInspector.DOMPresentationUtils.decorateNodeLabel(node,this._description);this.element.addEventListener("click",WebInspector.Revealer.reveal.bind(WebInspector.Revealer,node,undefined),false);}
this._rows=[];this.element=createElementWithClass("div","animation-node-row");this._description=this.element.createChild("div","animation-node-description");animationEffect.deferredNode().resolve(nodeResolved.bind(this));this._timelineElement=this.element.createChild("div","animation-node-timeline");}
WebInspector.AnimationTimeline.NodeRow;WebInspector.AnimationTimeline.NodeUI.prototype={findRow:function(animation)
{var existingRow=this._collapsibleIntoRow(animation);if(existingRow)
return existingRow;var container=this._timelineElement.createChild("div","animation-timeline-row");var nodeRow={element:container,animations:[]};this._rows.push(nodeRow);return nodeRow;},redraw:function()
{for(var nodeRow of this._rows){for(var ui of nodeRow.animations)
ui.redraw();}},_collapsibleIntoRow:function(animation)
{if(animation.endTime()===Infinity)
return null;for(var nodeRow of this._rows){var overlap=false;for(var ui of nodeRow.animations)
overlap|=animation.overlaps(ui.animation());if(!overlap)
return nodeRow;}
return null;},nodeRemoved:function()
{this.element.classList.add("animation-node-removed");},setNode:function(node)
{this.element.classList.toggle("animation-node-selected",node===this._node);}}
WebInspector.AnimationTimeline.StepTimingFunction=function(steps,stepAtPosition)
{this.steps=steps;this.stepAtPosition=stepAtPosition;}
WebInspector.AnimationTimeline.StepTimingFunction.parse=function(text){var match=text.match(/^step-(start|middle|end)$/);if(match)
return new WebInspector.AnimationTimeline.StepTimingFunction(1,match[1]);match=text.match(/^steps\((\d+), (start|middle|end)\)$/);if(match)
return new WebInspector.AnimationTimeline.StepTimingFunction(parseInt(match[1],10),match[2]);return null;};WebInspector.AnimationUI=function(animation,timeline,parentElement){this._animation=animation;this._timeline=timeline;this._parentElement=parentElement;if(this._animation.source().keyframesRule())
this._keyframes=this._animation.source().keyframesRule().keyframes();this._nameElement=parentElement.createChild("div","animation-name");this._nameElement.textContent=this._animation.name();this._svg=parentElement.createSVGChild("svg","animation-ui");this._svg.setAttribute("height",WebInspector.AnimationUI.Options.AnimationSVGHeight);this._svg.style.marginLeft="-"+WebInspector.AnimationUI.Options.AnimationMargin+"px";this._svg.addEventListener("mousedown",this._mouseDown.bind(this,WebInspector.AnimationUI.MouseEvents.AnimationDrag,null));this._svg.addEventListener("contextmenu",this._onContextMenu.bind(this));this._activeIntervalGroup=this._svg.createSVGChild("g");this._cachedElements=[];this._movementInMs=0;this._color=WebInspector.AnimationUI.Color(this._animation);}
WebInspector.AnimationUI.MouseEvents={AnimationDrag:"AnimationDrag",KeyframeMove:"KeyframeMove",StartEndpointMove:"StartEndpointMove",FinishEndpointMove:"FinishEndpointMove"}
WebInspector.AnimationUI.prototype={animation:function()
{return this._animation;},setNode:function(node)
{this._node=node;},_createLine:function(parentElement,className)
{var line=parentElement.createSVGChild("line",className);line.setAttribute("x1",WebInspector.AnimationUI.Options.AnimationMargin);line.setAttribute("y1",WebInspector.AnimationUI.Options.AnimationHeight);line.setAttribute("y2",WebInspector.AnimationUI.Options.AnimationHeight);line.style.stroke=this._color;return line;},_drawAnimationLine:function(iteration,parentElement)
{var cache=this._cachedElements[iteration];if(!cache.animationLine)
cache.animationLine=this._createLine(parentElement,"animation-line");cache.animationLine.setAttribute("x2",(this._duration()*this._timeline.pixelMsRatio()+WebInspector.AnimationUI.Options.AnimationMargin).toFixed(2));},_drawDelayLine:function(parentElement)
{if(!this._delayLine){this._delayLine=this._createLine(parentElement,"animation-delay-line");this._endDelayLine=this._createLine(parentElement,"animation-delay-line");}
this._delayLine.setAttribute("x1",WebInspector.AnimationUI.Options.AnimationMargin);this._delayLine.setAttribute("x2",(this._delay()*this._timeline.pixelMsRatio()+WebInspector.AnimationUI.Options.AnimationMargin).toFixed(2));var leftMargin=(this._delay()+this._duration()*this._animation.source().iterations())*this._timeline.pixelMsRatio();this._endDelayLine.style.transform="translateX("+Math.min(leftMargin,this._timeline.width()).toFixed(2)+"px)";this._endDelayLine.setAttribute("x1",WebInspector.AnimationUI.Options.AnimationMargin);this._endDelayLine.setAttribute("x2",(this._animation.source().endDelay()*this._timeline.pixelMsRatio()+WebInspector.AnimationUI.Options.AnimationMargin).toFixed(2));},_drawPoint:function(iteration,parentElement,x,keyframeIndex,attachEvents)
{if(this._cachedElements[iteration].keyframePoints[keyframeIndex]){this._cachedElements[iteration].keyframePoints[keyframeIndex].setAttribute("cx",x.toFixed(2));return;}
var circle=parentElement.createSVGChild("circle",keyframeIndex<=0?"animation-endpoint":"animation-keyframe-point");circle.setAttribute("cx",x.toFixed(2));circle.setAttribute("cy",WebInspector.AnimationUI.Options.AnimationHeight);circle.style.stroke=this._color;circle.setAttribute("r",WebInspector.AnimationUI.Options.AnimationMargin/2);if(keyframeIndex<=0)
circle.style.fill=this._color;this._cachedElements[iteration].keyframePoints[keyframeIndex]=circle;if(!attachEvents)
return;if(keyframeIndex===0){circle.addEventListener("mousedown",this._mouseDown.bind(this,WebInspector.AnimationUI.MouseEvents.StartEndpointMove,keyframeIndex));}else if(keyframeIndex===-1){circle.addEventListener("mousedown",this._mouseDown.bind(this,WebInspector.AnimationUI.MouseEvents.FinishEndpointMove,keyframeIndex));}else{circle.addEventListener("mousedown",this._mouseDown.bind(this,WebInspector.AnimationUI.MouseEvents.KeyframeMove,keyframeIndex));}},_renderKeyframe:function(iteration,keyframeIndex,parentElement,leftDistance,width,easing)
{function createStepLine(parentElement,x,strokeColor)
{var line=parentElement.createSVGChild("line");line.setAttribute("x1",x);line.setAttribute("x2",x);line.setAttribute("y1",WebInspector.AnimationUI.Options.AnimationMargin);line.setAttribute("y2",WebInspector.AnimationUI.Options.AnimationHeight);line.style.stroke=strokeColor;}
var bezier=WebInspector.Geometry.CubicBezier.parse(easing);var cache=this._cachedElements[iteration].keyframeRender;if(!cache[keyframeIndex])
cache[keyframeIndex]=bezier?parentElement.createSVGChild("path","animation-keyframe"):parentElement.createSVGChild("g","animation-keyframe-step");var group=cache[keyframeIndex];group.style.transform="translateX("+leftDistance.toFixed(2)+"px)";if(bezier){group.style.fill=this._color;WebInspector.BezierUI.drawVelocityChart(bezier,group,width);}else{var stepFunction=WebInspector.AnimationTimeline.StepTimingFunction.parse(easing);group.removeChildren();const offsetMap={"start":0,"middle":0.5,"end":1};const offsetWeight=offsetMap[stepFunction.stepAtPosition];for(var i=0;i<stepFunction.steps;i++)
createStepLine(group,(i+offsetWeight)*width/stepFunction.steps,this._color);}},redraw:function()
{var durationWithDelay=this._delay()+this._duration()*this._animation.source().iterations()+this._animation.source().endDelay();var leftMargin=((this._animation.startTime()-this._timeline.startTime())*this._timeline.pixelMsRatio());var maxWidth=this._timeline.width()-WebInspector.AnimationUI.Options.AnimationMargin-leftMargin;var svgWidth=Math.min(maxWidth,durationWithDelay*this._timeline.pixelMsRatio());this._svg.classList.toggle("animation-ui-canceled",this._animation.playState()==="idle");this._svg.setAttribute("width",(svgWidth+2*WebInspector.AnimationUI.Options.AnimationMargin).toFixed(2));this._svg.style.transform="translateX("+leftMargin.toFixed(2)+"px)";this._activeIntervalGroup.style.transform="translateX("+(this._delay()*this._timeline.pixelMsRatio()).toFixed(2)+"px)";this._nameElement.style.transform="translateX("+(leftMargin+this._delay()*this._timeline.pixelMsRatio()+WebInspector.AnimationUI.Options.AnimationMargin).toFixed(2)+"px)";this._nameElement.style.width=(this._duration()*this._timeline.pixelMsRatio().toFixed(2))+"px";this._drawDelayLine(this._svg);if(this._animation.type()==="CSSTransition"){this._renderTransition();return;}
this._renderIteration(this._activeIntervalGroup,0);if(!this._tailGroup)
this._tailGroup=this._activeIntervalGroup.createSVGChild("g","animation-tail-iterations");var iterationWidth=this._duration()*this._timeline.pixelMsRatio();for(var iteration=1;iteration<this._animation.source().iterations()&&iterationWidth*(iteration-1)<this._timeline.width();iteration++)
this._renderIteration(this._tailGroup,iteration);while(iteration<this._cachedElements.length)
this._cachedElements.pop().group.remove();},_renderTransition:function()
{if(!this._cachedElements[0])
this._cachedElements[0]={animationLine:null,keyframePoints:{},keyframeRender:{},group:null};this._drawAnimationLine(0,this._activeIntervalGroup);this._renderKeyframe(0,0,this._activeIntervalGroup,WebInspector.AnimationUI.Options.AnimationMargin,this._duration()*this._timeline.pixelMsRatio(),this._animation.source().easing());this._drawPoint(0,this._activeIntervalGroup,WebInspector.AnimationUI.Options.AnimationMargin,0,true);this._drawPoint(0,this._activeIntervalGroup,this._duration()*this._timeline.pixelMsRatio()+WebInspector.AnimationUI.Options.AnimationMargin,-1,true);},_renderIteration:function(parentElement,iteration)
{if(!this._cachedElements[iteration])
this._cachedElements[iteration]={animationLine:null,keyframePoints:{},keyframeRender:{},group:parentElement.createSVGChild("g")};var group=this._cachedElements[iteration].group;group.style.transform="translateX("+(iteration*this._duration()*this._timeline.pixelMsRatio()).toFixed(2)+"px)";this._drawAnimationLine(iteration,group);console.assert(this._keyframes.length>1);for(var i=0;i<this._keyframes.length-1;i++){var leftDistance=this._offset(i)*this._duration()*this._timeline.pixelMsRatio()+WebInspector.AnimationUI.Options.AnimationMargin;var width=this._duration()*(this._offset(i+1)-this._offset(i))*this._timeline.pixelMsRatio();this._renderKeyframe(iteration,i,group,leftDistance,width,this._keyframes[i].easing());if(i||(!i&&iteration===0))
this._drawPoint(iteration,group,leftDistance,i,iteration===0);}
this._drawPoint(iteration,group,this._duration()*this._timeline.pixelMsRatio()+WebInspector.AnimationUI.Options.AnimationMargin,-1,iteration===0);},_delay:function()
{var delay=this._animation.source().delay();if(this._mouseEventType===WebInspector.AnimationUI.MouseEvents.AnimationDrag||this._mouseEventType===WebInspector.AnimationUI.MouseEvents.StartEndpointMove)
delay+=this._movementInMs;return Math.max(0,delay);},_duration:function()
{var duration=this._animation.source().duration();if(this._mouseEventType===WebInspector.AnimationUI.MouseEvents.FinishEndpointMove)
duration+=this._movementInMs;else if(this._mouseEventType===WebInspector.AnimationUI.MouseEvents.StartEndpointMove)
duration-=Math.max(this._movementInMs,-this._animation.source().delay());return Math.max(0,duration);},_offset:function(i)
{var offset=this._keyframes[i].offsetAsNumber();if(this._mouseEventType===WebInspector.AnimationUI.MouseEvents.KeyframeMove&&i===this._keyframeMoved){console.assert(i>0&&i<this._keyframes.length-1,"First and last keyframe cannot be moved");offset+=this._movementInMs/this._animation.source().duration();offset=Math.max(offset,this._keyframes[i-1].offsetAsNumber());offset=Math.min(offset,this._keyframes[i+1].offsetAsNumber());}
return offset;},_mouseDown:function(mouseEventType,keyframeIndex,event)
{if(event.buttons==2)
return;if(this._animation.playState()==="idle")
return;this._mouseEventType=mouseEventType;this._keyframeMoved=keyframeIndex;this._downMouseX=event.clientX;this._mouseMoveHandler=this._mouseMove.bind(this);this._mouseUpHandler=this._mouseUp.bind(this);this._parentElement.ownerDocument.addEventListener("mousemove",this._mouseMoveHandler);this._parentElement.ownerDocument.addEventListener("mouseup",this._mouseUpHandler);event.preventDefault();event.stopPropagation();if(this._node)
WebInspector.Revealer.reveal(this._node);},_mouseMove:function(event)
{this._movementInMs=(event.clientX-this._downMouseX)/this._timeline.pixelMsRatio();if(this._animation.startTime()+this._delay()+this._duration()-this._timeline.startTime()>this._timeline.duration()*0.8)
this._timeline.setDuration(this._timeline.duration()*1.2);this.redraw();},_mouseUp:function(event)
{this._movementInMs=(event.clientX-this._downMouseX)/this._timeline.pixelMsRatio();if(this._mouseEventType===WebInspector.AnimationUI.MouseEvents.KeyframeMove)
this._keyframes[this._keyframeMoved].setOffset(this._offset(this._keyframeMoved));else
this._animation.setTiming(this._duration(),this._delay());this._movementInMs=0;this.redraw();this._parentElement.ownerDocument.removeEventListener("mousemove",this._mouseMoveHandler);this._parentElement.ownerDocument.removeEventListener("mouseup",this._mouseUpHandler);delete this._mouseMoveHandler;delete this._mouseUpHandler;delete this._mouseEventType;delete this._downMouseX;delete this._keyframeMoved;},_onContextMenu:function(event)
{function showContextMenu(remoteObject)
{if(!remoteObject)
return;var contextMenu=new WebInspector.ContextMenu(event);contextMenu.appendApplicableItems(remoteObject);contextMenu.show();}
this._animation.remoteObjectPromise().then(showContextMenu);event.consume(true);}}
WebInspector.AnimationUI.Options={AnimationHeight:26,AnimationSVGHeight:50,AnimationMargin:7,EndpointsClickRegionSize:10,GridCanvasHeight:40}
WebInspector.AnimationUI.Colors={"Purple":WebInspector.Color.parse("#9C27B0"),"Light Blue":WebInspector.Color.parse("#03A9F4"),"Deep Orange":WebInspector.Color.parse("#FF5722"),"Blue":WebInspector.Color.parse("#5677FC"),"Lime":WebInspector.Color.parse("#CDDC39"),"Blue Grey":WebInspector.Color.parse("#607D8B"),"Pink":WebInspector.Color.parse("#E91E63"),"Green":WebInspector.Color.parse("#0F9D58"),"Brown":WebInspector.Color.parse("#795548"),"Cyan":WebInspector.Color.parse("#00BCD4")}
WebInspector.AnimationUI.Color=function(animation)
{function hash(string)
{var hash=0;for(var i=0;i<string.length;i++)
hash=(hash<<5)+hash+string.charCodeAt(i);return Math.abs(hash);}
var names=Object.keys(WebInspector.AnimationUI.Colors);var color=WebInspector.AnimationUI.Colors[names[hash(animation.name()||animation.id())%names.length]];return color.asString(WebInspector.Color.Format.RGB);};WebInspector.AnimationControlPane=function(toolbarItem)
{WebInspector.ElementsPanel.BaseToolbarPaneWidget.call(this,toolbarItem);this._animationsPaused=false;this._animationsPlaybackRate=1;this.element.className="styles-animations-controls-pane";this.element.createChild("div").createTextChild("Animations");var container=this.element.createChild("div","animations-controls");var toolbar=new WebInspector.Toolbar();this._animationsPauseButton=new WebInspector.ToolbarButton("","pause-toolbar-item");toolbar.appendToolbarItem(this._animationsPauseButton);this._animationsPauseButton.addEventListener("click",this._pauseButtonHandler.bind(this));container.appendChild(toolbar.element);this._animationsPlaybackSlider=container.createChild("input");this._animationsPlaybackSlider.type="range";this._animationsPlaybackSlider.min=0;this._animationsPlaybackSlider.max=WebInspector.AnimationTimeline.GlobalPlaybackRates.length-1;this._animationsPlaybackSlider.value=this._animationsPlaybackSlider.max;this._animationsPlaybackSlider.addEventListener("input",this._playbackSliderInputHandler.bind(this));this._animationsPlaybackLabel=container.createChild("div","playback-label");this._animationsPlaybackLabel.createTextChild("1x");}
WebInspector.AnimationControlPane.prototype={_playbackSliderInputHandler:function(event)
{this._animationsPlaybackRate=WebInspector.AnimationTimeline.GlobalPlaybackRates[event.target.value];WebInspector.AnimationModel.fromTarget(this._target).setPlaybackRate(this._animationsPaused?0:this._animationsPlaybackRate);this._animationsPlaybackLabel.textContent=this._animationsPlaybackRate+"x";WebInspector.userMetrics.actionTaken(WebInspector.UserMetrics.Action.AnimationsPlaybackRateChanged);},_pauseButtonHandler:function()
{this._animationsPaused=!this._animationsPaused;WebInspector.AnimationModel.fromTarget(this._target).setPlaybackRate(this._animationsPaused?0:this._animationsPlaybackRate);WebInspector.userMetrics.actionTaken(WebInspector.UserMetrics.Action.AnimationsPlaybackRateChanged);this._animationsPauseButton.element.classList.toggle("pause-toolbar-item");this._animationsPauseButton.element.classList.toggle("play-toolbar-item");},_updateAnimationsPlaybackRate:function(event)
{function setPlaybackRate(playbackRate)
{this._animationsPlaybackSlider.value=WebInspector.AnimationTimeline.GlobalPlaybackRates.indexOf(playbackRate);this._animationsPlaybackLabel.textContent=playbackRate+"x";}
if(this._target)
WebInspector.AnimationModel.fromTarget(this._target).playbackRatePromise().then(setPlaybackRate.bind(this));},onNodeChanged:function(node)
{if(!node)
return;if(this._target)
this._target.resourceTreeModel.removeEventListener(WebInspector.ResourceTreeModel.EventTypes.MainFrameNavigated,this._updateAnimationsPlaybackRate,this);this._target=node.target();this._target.resourceTreeModel.addEventListener(WebInspector.ResourceTreeModel.EventTypes.MainFrameNavigated,this._updateAnimationsPlaybackRate,this);this._updateAnimationsPlaybackRate();},__proto__:WebInspector.ElementsPanel.BaseToolbarPaneWidget.prototype}
WebInspector.AnimationControlPane.ButtonProvider=function()
{this._button=new WebInspector.ToolbarButton(WebInspector.UIString("Toggle animation controls"),"animation-toolbar-item");this._button.addEventListener("click",this._clicked,this);WebInspector.context.addFlavorChangeListener(WebInspector.DOMNode,this._nodeChanged,this);this._nodeChanged();}
WebInspector.AnimationControlPane.ButtonProvider.prototype={_toggleAnimationTimelineMode:function(toggleOn)
{if(!this._animationTimeline)
this._animationTimeline=new WebInspector.AnimationTimeline();this._button.setToggled(toggleOn);var elementsPanel=WebInspector.ElementsPanel.instance();elementsPanel.setWidgetBelowDOM(toggleOn?this._animationTimeline:null);},_toggleAnimationControlPaneMode:function(toggleOn)
{if(!this._animationsControlPane)
this._animationsControlPane=new WebInspector.AnimationControlPane(this.item());WebInspector.ElementsPanel.instance().showToolbarPane(toggleOn?this._animationsControlPane:null);},_clicked:function()
{if(Runtime.experiments.isEnabled("animationInspection"))
this._toggleAnimationTimelineMode(!this._button.toggled());else
this._toggleAnimationControlPaneMode(!this._button.toggled());},_nodeChanged:function()
{var node=WebInspector.context.flavor(WebInspector.DOMNode);if(Runtime.experiments.isEnabled("animationInspection")){if(this._animationTimeline)
this._animationTimeline.setNode(node);}else{this._button.setEnabled(!!node);if(!node)
this._toggleAnimationControlPaneMode(false);}},item:function()
{return this._button;}};Runtime.cachedResources["animation/animationTimeline.css"]="/*\n * Copyright (c) 2015 The Chromium Authors. All rights reserved.\n * Use of this source code is governed by a BSD-style license that can be\n * found in the LICENSE file.\n */\n\n.animation-node-row {\n    width: 100%;\n    display: flex;\n    border-bottom: 1px dashed hsla(0,0%,94%,1);\n}\n\n.animation-node-description {\n    width: 150px;\n    padding-left: 8px;\n    overflow: hidden;\n    position: relative;\n    transform-style: preserve-3d;\n    border-bottom: 1px solid hsl(0, 0%, 90%);\n    margin-bottom: -1px;\n    margin-right: 26px;\n    background-color: hsl(0, 0%, 98%);\n    display: flex;\n    align-items: center;\n    flex: 0 0 150px;\n}\n\n.animation-node-description > * {\n    flex: 0 0 auto;\n}\n\n.animation-timeline-row {\n    height: 32px;\n    position: relative;\n}\n\npath.animation-keyframe {\n    fill-opacity: 0.3;\n}\n\n.animation-node-selected path.animation-keyframe {\n    fill-opacity: 0.4;\n}\n\nline.animation-line {\n    stroke-width: 2px;\n    stroke-linecap: round;\n    fill: none;\n}\n\nline.animation-delay-line {\n    stroke-width: 2px;\n    stroke-dasharray: 6, 4;\n}\n\ncircle.animation-endpoint, circle.animation-keyframe-point {\n    stroke-width: 2px;\n    transition: transform 100ms cubic-bezier(0, 0, 0.2, 1);\n    transform: scale(1);\n    transform-origin: 50% 50%;\n}\n\n.animation-ui circle.animation-endpoint:hover, .animation-ui circle.animation-keyframe-point:hover {\n    transform: scale(1.2);\n}\n\ncircle.animation-endpoint:active, circle.animation-keyframe-point:active {\n    transform: scale(1);\n}\n\ncircle.animation-keyframe-point {\n    fill: white;\n}\n\n.animation-name {\n    position: absolute;\n    top: 8px;\n    color: #333;\n    text-align: center;\n    margin-left: -8px;\n    white-space: nowrap;\n    min-width: 50px;\n}\n\n.animation-timeline-header {\n    height: 30px;\n    border-bottom: 1px solid #ccc;\n    flex-shrink: 0;\n    display: flex;\n}\n\n.animation-timeline-header:after {\n    content: \"\";\n    height: calc(100% - 48px);\n    position: absolute;\n    width: 150px;\n    left: 0;\n    background-color: hsl(0, 0%, 98%);\n    z-index: -1;\n    border-right: 1px solid hsl(0, 0%, 90%);\n}\n\n.animation-controls {\n    flex: 0 0 150px;\n    padding: 1px;\n    height: 100%;\n    position: relative;\n}\n\n.animation-grid-header {\n    flex: 1 0 auto;\n}\n\n.animation-timeline-buffer {\n    height: 48px;\n    flex: 0 0 auto;\n    border-bottom: 1px solid #ccc;\n    display: flex;\n    padding: 0 2px;\n}\n\n.animation-time-overlay {\n    background-color: black;\n    opacity: 0.05;\n    position: absolute;\n    height: 100%;\n    width: 100%;\n}\n\n.animation-timeline-end > .animation-time-overlay {\n    visibility: hidden;\n}\n\ninput.animation-playback-slider {\n    width: 65px;\n    margin-left: 7px;\n    -webkit-appearance: none;\n    position: absolute;\n    top: 1px;\n    background: none;\n}\n\ninput[type=range].animation-playback-slider::-webkit-slider-runnable-track {\n    height: 4px;\n    margin: 10px 0;\n    border-radius: 2px;\n    cursor: pointer;\n    background: #c8c8c8;\n}\n\ninput[type=range].animation-playback-slider:focus::-webkit-slider-runnable-track {\n    background: #b8b8b8;\n}\n\ninput[type=range].animation-playback-slider::-webkit-slider-thumb {\n    -webkit-appearance: none;\n    cursor: pointer;\n    margin-top: -4px;\n    cursor: pointer;\n    width: 12px;\n    height: 12px;\n    border-radius: 12px;\n    background-color: rgb(248, 248, 248);\n    border: 1px solid #999;\n    box-shadow: 0 1px 1px 0 rgba(0, 0, 0, 0.24);\n}\n\n.animation-playback-label {\n    display: inline-block;\n    width: 40px;\n    height: 22px;\n    text-align: center;\n    border: 1px solid #dadada;\n    border-radius: 2px;\n    line-height: 20px;\n    font-size: 11px;\n    padding: 0;\n    color: #333;\n    white-space: nowrap;\n    overflow: hidden;\n    -webkit-user-modify: read-write-plaintext-only;\n    margin: 2px;\n}\n\n.animation-scrubber {\n    opacity: 1;\n    position: absolute;\n    left: 175px;\n    height: calc(100% - 78px);\n    width: calc(100% - 175px);\n    top: 78px;\n    border-left: 1px solid rgba(0,0,0,0.5);\n}\n\n.animation-scrubber.animation-timeline-end {\n    border: none;\n}\n\n.animation-scrubber-head {\n    background-color: rgba(0, 0, 0, 0.7);\n    width: 50px;\n    height: 23px;\n    color: white;\n    line-height: 23px;\n    text-align: center;\n    border-radius: 5px;\n    position: relative;\n    top: -29px;\n    left: -25px;\n    font-size: 10px;\n    visibility: visible;\n}\n\n.animation-scrubber-head:before, .animation-scrubber-head:after {\n    position: absolute;\n    color: #999;\n    font-size: 8px;\n}\n\n.animation-scrubber-head:before {\n    left: 3px;\n}\n\n.animation-scrubber-head:after {\n    right: 3px;\n}\n\n.animation-timeline-end > .animation-scrubber-head {\n    visibility: visible;\n}\n\n.animation-timeline-end > .animation-scrubber-arrow {\n    visibility: hidden;\n}\n\n.animation-scrubber-arrow {\n    width: 21px;\n    height: 25px;\n    position: absolute;\n    top: -6px;\n    left: -7px;\n    -webkit-clip-path: polygon(0 0, 6px 6px, 12px 0px, 0px 0px);\n    background-color: rgba(0, 0, 0, 0.7);\n}\n\n.animation-timeline-end > .animation-timeline-timer {\n    visibility: hidden;\n}\n\nsvg.animation-timeline-grid {\n    position: absolute;\n    left: 175px;\n    top: 48px;\n    z-index: -1;\n}\n\nrect.animation-timeline-grid-line {\n    fill: hsla(0,0%,93%,1);\n}\n\n.animation-timeline-row > svg.animation-ui {\n    position: absolute;\n}\n\n.animation-node-timeline {\n    flex-grow: 1;\n}\n\n.animation-node-description > div {\n    position: absolute;\n    top: 50%;\n    transform: translateY(-50%);\n    max-height: 100%;\n}\n\n.animation-node-row.animation-node-removed {\n    background-color: hsla(0, 100%, 50%, 0.1);\n}\n\nsvg.animation-ui g:first-child {\n    opacity: 1;\n}\n\n.animation-tail-iterations {\n    opacity: 0.5;\n}\n\n.animation-keyframe-step line {\n    stroke-width: 2;\n    stroke-opacity: 0.3;\n}\n\ntext.animation-timeline-grid-label {\n    font-size: 10px;\n    fill: #5a5a5a;\n}\n\n.animation-timeline-rows {\n    flex-grow: 1;\n    overflow-y: auto;\n    z-index: 1;\n    overflow-x: hidden;\n}\n\n.toolbar.animation-controls-toolbar {\n    float: right;\n}\n\n.animation-node-row.animation-node-selected {\n    background-color: hsla(216, 71%, 53%, 0.08);\n}\n\n.animation-node-selected > .animation-node-description {\n    background-color: #EFF4FD;\n}\n\n.animation-timeline-empty-message {\n    padding-left: 230px;\n    padding-right: 30px;\n    text-align: center;\n    position: absolute;\n    font-size: 20px;\n    line-height: 32px;\n    align-items: center;   justify-content: center;\n    width: 100%;\n    height: calc(100% - 44px);\n    display: flex;\n}\n\n.animation-buffer-preview {\n    height: 40px;\n    margin: 4px 2px;\n    background-color: #F3F3F3;\n    border-radius: 2px;\n    flex: 1 1;\n    padding: 4px;\n    max-width: 100px;\n    animation: newGroupAnim 200ms;\n    position: relative;\n}\n\n.animation-buffer-preview-animation {\n    width: 100%;\n    height: 100%;\n    border-radius: 2px 0 0 2px;\n    position: absolute;\n    top: 0;\n    left: 0;\n    background: hsla(219, 100%, 66%, 0.27);\n    opacity: 0;\n    border-right: 1px solid #A7A7A7;\n}\n\n.animation-buffer-preview:not(.selected):hover {\n    background-color: hsla(217,90%,92%,1);\n}\n\n.animation-buffer-preview.selected {\n    background-color: hsl(217, 89%, 61%);\n}\n\n.animation-buffer-preview.selected > svg > line {\n    stroke: white !important;\n}\n\n.animation-buffer-preview > svg > line {\n    stroke-width: 1px;\n}\n\n@keyframes newGroupAnim {\n    from {\n        -webkit-clip-path: polygon(0% 0%, 0% 100%, 0% 100%, 0% 0%);\n    }\n    to {\n        -webkit-clip-path: polygon(0% 0%, 0% 100%, 100% 100%, 100% 0%);\n    }\n}\n/*# sourceURL=animation/animationTimeline.css */";