WebInspector.DataGrid=function(columnsArray,editCallback,deleteCallback,refreshCallback,contextMenuCallback)
{this.element=createElementWithClass("div","data-grid");this.element.appendChild(WebInspector.Widget.createStyleElement("ui_lazy/dataGrid.css"));this.element.tabIndex=0;this.element.addEventListener("keydown",this._keyDown.bind(this),false);var headerContainer=createElementWithClass("div","header-container");this._headerTable=headerContainer.createChild("table","header");this._headerTableHeaders={};this._scrollContainer=createElementWithClass("div","data-container");this._dataTable=this._scrollContainer.createChild("table","data");this._dataTable.addEventListener("mousedown",this._mouseDownInDataTable.bind(this));this._dataTable.addEventListener("click",this._clickInDataTable.bind(this),true);this._dataTable.addEventListener("contextmenu",this._contextMenuInDataTable.bind(this),true);if(editCallback)
this._dataTable.addEventListener("dblclick",this._ondblclick.bind(this),false);this._editCallback=editCallback;this._deleteCallback=deleteCallback;this._refreshCallback=refreshCallback;this._contextMenuCallback=contextMenuCallback;this.element.appendChild(headerContainer);this.element.appendChild(this._scrollContainer);this._headerRow=createElement("tr");this._headerTableColumnGroup=createElement("colgroup");this._dataTableColumnGroup=createElement("colgroup");this._topFillerRow=createElementWithClass("tr","data-grid-filler-row revealed");this._bottomFillerRow=createElementWithClass("tr","data-grid-filler-row revealed");this.setVerticalPadding(0,0);this._inline=false;this._columnsArray=columnsArray;this._visibleColumnsArray=columnsArray;this._columns={};this._cellClass=null;for(var i=0;i<columnsArray.length;++i){var column=columnsArray[i];var columnIdentifier=column.identifier=column.id||i;this._columns[columnIdentifier]=column;if(column.disclosure)
this.disclosureColumnIdentifier=columnIdentifier;var cell=createElement("th");cell.className=columnIdentifier+"-column";cell.columnIdentifier=String(columnIdentifier);this._headerTableHeaders[columnIdentifier]=cell;var div=createElement("div");if(column.titleDOMFragment)
div.appendChild(column.titleDOMFragment);else
div.textContent=column.title;cell.appendChild(div);if(column.sort){cell.classList.add(column.sort);this._sortColumnCell=cell;}
if(column.sortable){cell.addEventListener("click",this._clickInHeaderCell.bind(this),false);cell.classList.add("sortable");cell.createChild("div","sort-order-icon-container").createChild("div","sort-order-icon");}}
this._headerTable.appendChild(this._headerTableColumnGroup);this.headerTableBody.appendChild(this._headerRow);this._dataTable.appendChild(this._dataTableColumnGroup);this.dataTableBody.appendChild(this._topFillerRow);this.dataTableBody.appendChild(this._bottomFillerRow);this._refreshHeader();this._editing=false;this.selectedNode=null;this.expandNodesWhenArrowing=false;this.setRootNode(new WebInspector.DataGridNode());this.indentWidth=15;this._resizers=[];this._columnWidthsInitialized=false;this._cornerWidth=WebInspector.DataGrid.CornerWidth;this._resizeMethod=WebInspector.DataGrid.ResizeMethod.Nearest;}
WebInspector.DataGrid.CornerWidth=14;WebInspector.DataGrid.ColumnDescriptor;WebInspector.DataGrid.Events={SelectedNode:"SelectedNode",DeselectedNode:"DeselectedNode",SortingChanged:"SortingChanged",ColumnsResized:"ColumnsResized"}
WebInspector.DataGrid.Order={Ascending:"sort-ascending",Descending:"sort-descending"}
WebInspector.DataGrid.Align={Center:"center",Right:"right"}
WebInspector.DataGrid.prototype={setCellClass:function(cellClass)
{this._cellClass=cellClass;},_refreshHeader:function()
{this._headerTableColumnGroup.removeChildren();this._dataTableColumnGroup.removeChildren();this._headerRow.removeChildren();this._topFillerRow.removeChildren();this._bottomFillerRow.removeChildren();for(var i=0;i<this._visibleColumnsArray.length;++i){var column=this._visibleColumnsArray[i];var columnIdentifier=column.identifier;var headerColumn=this._headerTableColumnGroup.createChild("col");var dataColumn=this._dataTableColumnGroup.createChild("col");if(column.width){headerColumn.style.width=column.width;dataColumn.style.width=column.width;}
this._headerRow.appendChild(this._headerTableHeaders[columnIdentifier]);this._topFillerRow.createChild("td","top-filler-td");this._bottomFillerRow.createChild("td","bottom-filler-td").columnIdentifier_=columnIdentifier;}
this._headerRow.createChild("th","corner");this._topFillerRow.createChild("td","corner").classList.add("top-filler-td");this._bottomFillerRow.createChild("td","corner").classList.add("bottom-filler-td");this._headerTableColumnGroup.createChild("col","corner");this._dataTableColumnGroup.createChild("col","corner");},setVerticalPadding:function(top,bottom)
{this._topFillerRow.style.height=top+"px";if(top||bottom)
this._bottomFillerRow.style.height=bottom+"px";else
this._bottomFillerRow.style.height="auto";},setRootNode:function(rootNode)
{if(this._rootNode){this._rootNode.removeChildren();this._rootNode.dataGrid=null;this._rootNode._isRoot=false;}
this._rootNode=rootNode;rootNode._isRoot=true;rootNode.hasChildren=false;rootNode._expanded=true;rootNode._revealed=true;rootNode.dataGrid=this;},rootNode:function()
{return this._rootNode;},_ondblclick:function(event)
{if(this._editing||this._editingNode)
return;var columnIdentifier=this.columnIdentifierFromNode(event.target);if(!columnIdentifier||!this._columns[columnIdentifier].editable)
return;this._startEditing(event.target);},_startEditingColumnOfDataGridNode:function(node,cellIndex)
{this._editing=true;this._editingNode=node;this._editingNode.select();var element=this._editingNode._element.children[cellIndex];WebInspector.InplaceEditor.startEditing(element,this._startEditingConfig(element));element.getComponentSelection().setBaseAndExtent(element,0,element,1);},_startEditing:function(target)
{var element=target.enclosingNodeOrSelfWithNodeName("td");if(!element)
return;this._editingNode=this.dataGridNodeFromNode(target);if(!this._editingNode){if(!this.creationNode)
return;this._editingNode=this.creationNode;}
if(this._editingNode.isCreationNode)
return this._startEditingColumnOfDataGridNode(this._editingNode,this._nextEditableColumn(-1));this._editing=true;WebInspector.InplaceEditor.startEditing(element,this._startEditingConfig(element));element.getComponentSelection().setBaseAndExtent(element,0,element,1);},renderInline:function()
{this.element.classList.add("inline");this._cornerWidth=0;this._inline=true;this.updateWidths();},_startEditingConfig:function(element)
{return new WebInspector.InplaceEditor.Config(this._editingCommitted.bind(this),this._editingCancelled.bind(this),element.textContent);},_editingCommitted:function(element,newText,oldText,context,moveDirection)
{var columnIdentifier=this.columnIdentifierFromNode(element);if(!columnIdentifier){this._editingCancelled(element);return;}
var column=this._columns[columnIdentifier];var cellIndex=this._visibleColumnsArray.indexOf(column);var textBeforeEditing=this._editingNode.data[columnIdentifier];var currentEditingNode=this._editingNode;function moveToNextIfNeeded(wasChange){if(!moveDirection)
return;if(moveDirection==="forward"){var firstEditableColumn=this._nextEditableColumn(-1);if(currentEditingNode.isCreationNode&&cellIndex===firstEditableColumn&&!wasChange)
return;var nextEditableColumn=this._nextEditableColumn(cellIndex);if(nextEditableColumn!==-1)
return this._startEditingColumnOfDataGridNode(currentEditingNode,nextEditableColumn);var nextDataGridNode=currentEditingNode.traverseNextNode(true,null,true);if(nextDataGridNode)
return this._startEditingColumnOfDataGridNode(nextDataGridNode,firstEditableColumn);if(currentEditingNode.isCreationNode&&wasChange){this.addCreationNode(false);return this._startEditingColumnOfDataGridNode(this.creationNode,firstEditableColumn);}
return;}
if(moveDirection==="backward"){var prevEditableColumn=this._nextEditableColumn(cellIndex,true);if(prevEditableColumn!==-1)
return this._startEditingColumnOfDataGridNode(currentEditingNode,prevEditableColumn);var lastEditableColumn=this._nextEditableColumn(this._visibleColumnsArray.length,true);var nextDataGridNode=currentEditingNode.traversePreviousNode(true,true);if(nextDataGridNode)
return this._startEditingColumnOfDataGridNode(nextDataGridNode,lastEditableColumn);return;}}
if(textBeforeEditing==newText){this._editingCancelled(element);moveToNextIfNeeded.call(this,false);return;}
this._editingNode.data[columnIdentifier]=newText;this._editCallback(this._editingNode,columnIdentifier,textBeforeEditing,newText);if(this._editingNode.isCreationNode)
this.addCreationNode(false);this._editingCancelled(element);moveToNextIfNeeded.call(this,true);},_editingCancelled:function(element)
{this._editing=false;this._editingNode=null;},_nextEditableColumn:function(cellIndex,moveBackward)
{var increment=moveBackward?-1:1;var columns=this._visibleColumnsArray;for(var i=cellIndex+increment;(i>=0)&&(i<columns.length);i+=increment){if(columns[i].editable)
return i;}
return-1;},sortColumnIdentifier:function()
{if(!this._sortColumnCell)
return null;return this._sortColumnCell.columnIdentifier;},sortOrder:function()
{if(!this._sortColumnCell||this._sortColumnCell.classList.contains(WebInspector.DataGrid.Order.Ascending))
return WebInspector.DataGrid.Order.Ascending;if(this._sortColumnCell.classList.contains(WebInspector.DataGrid.Order.Descending))
return WebInspector.DataGrid.Order.Descending;return null;},isSortOrderAscending:function()
{return!this._sortColumnCell||this._sortColumnCell.classList.contains(WebInspector.DataGrid.Order.Ascending);},get headerTableBody()
{if("_headerTableBody"in this)
return this._headerTableBody;this._headerTableBody=this._headerTable.getElementsByTagName("tbody")[0];if(!this._headerTableBody){this._headerTableBody=this.element.ownerDocument.createElement("tbody");this._headerTable.insertBefore(this._headerTableBody,this._headerTable.tFoot);}
return this._headerTableBody;},get dataTableBody()
{if("_dataTableBody"in this)
return this._dataTableBody;this._dataTableBody=this._dataTable.getElementsByTagName("tbody")[0];if(!this._dataTableBody){this._dataTableBody=this.element.ownerDocument.createElement("tbody");this._dataTable.insertBefore(this._dataTableBody,this._dataTable.tFoot);}
return this._dataTableBody;},_autoSizeWidths:function(widths,minPercent,maxPercent)
{if(minPercent)
minPercent=Math.min(minPercent,Math.floor(100/widths.length));var totalWidth=0;for(var i=0;i<widths.length;++i)
totalWidth+=widths[i];var totalPercentWidth=0;for(var i=0;i<widths.length;++i){var width=Math.round(100*widths[i]/totalWidth);if(minPercent&&width<minPercent)
width=minPercent;else if(maxPercent&&width>maxPercent)
width=maxPercent;totalPercentWidth+=width;widths[i]=width;}
var recoupPercent=totalPercentWidth-100;while(minPercent&&recoupPercent>0){for(var i=0;i<widths.length;++i){if(widths[i]>minPercent){--widths[i];--recoupPercent;if(!recoupPercent)
break;}}}
while(maxPercent&&recoupPercent<0){for(var i=0;i<widths.length;++i){if(widths[i]<maxPercent){++widths[i];++recoupPercent;if(!recoupPercent)
break;}}}
return widths;},autoSizeColumns:function(minPercent,maxPercent,maxDescentLevel)
{var widths=[];for(var i=0;i<this._columnsArray.length;++i)
widths.push((this._columnsArray[i].title||"").length);maxDescentLevel=maxDescentLevel||0;var children=this._enumerateChildren(this._rootNode,[],maxDescentLevel+1);for(var i=0;i<children.length;++i){var node=children[i];for(var j=0;j<this._columnsArray.length;++j){var text=node.data[this._columnsArray[j].identifier]||"";if(text.length>widths[j])
widths[j]=text.length;}}
widths=this._autoSizeWidths(widths,minPercent,maxPercent);for(var i=0;i<this._columnsArray.length;++i)
this._columnsArray[i].weight=widths[i];this._columnWidthsInitialized=false;this.updateWidths();},_enumerateChildren:function(rootNode,result,maxLevel)
{if(!rootNode._isRoot)
result.push(rootNode);if(!maxLevel)
return;for(var i=0;i<rootNode.children.length;++i)
this._enumerateChildren(rootNode.children[i],result,maxLevel-1);return result;},onResize:function()
{this.updateWidths();},updateWidths:function()
{var headerTableColumns=this._headerTableColumnGroup.children;var tableWidth=this.element.offsetWidth-this._cornerWidth;var numColumns=headerTableColumns.length-1;if(!this._columnWidthsInitialized&&this.element.offsetWidth){for(var i=0;i<numColumns;i++){var columnWidth=this.headerTableBody.rows[0].cells[i].offsetWidth;var column=this._visibleColumnsArray[i];if(!column.weight)
column.weight=100*columnWidth/tableWidth;}
this._columnWidthsInitialized=true;}
this._applyColumnWeights();},setName:function(name)
{this._columnWeightsSetting=WebInspector.settings.createSetting("dataGrid-"+name+"-columnWeights",{});this._loadColumnWeights();},_loadColumnWeights:function()
{if(!this._columnWeightsSetting)
return;var weights=this._columnWeightsSetting.get();for(var i=0;i<this._columnsArray.length;++i){var column=this._columnsArray[i];var weight=weights[column.identifier];if(weight)
column.weight=weight;}
this._applyColumnWeights();},_saveColumnWeights:function()
{if(!this._columnWeightsSetting)
return;var weights={};for(var i=0;i<this._columnsArray.length;++i){var column=this._columnsArray[i];weights[column.identifier]=column.weight;}
this._columnWeightsSetting.set(weights);},wasShown:function()
{this._loadColumnWeights();},willHide:function()
{},_applyColumnWeights:function()
{var tableWidth=this.element.offsetWidth-this._cornerWidth;if(tableWidth<=0)
return;var sumOfWeights=0.0;for(var i=0;i<this._visibleColumnsArray.length;++i)
sumOfWeights+=this._visibleColumnsArray[i].weight;var sum=0;var lastOffset=0;for(var i=0;i<this._visibleColumnsArray.length;++i){sum+=this._visibleColumnsArray[i].weight;var offset=(sum*tableWidth/sumOfWeights)|0;var width=(offset-lastOffset)+"px";this._headerTableColumnGroup.children[i].style.width=width;this._dataTableColumnGroup.children[i].style.width=width;lastOffset=offset;}
this._positionResizers();this.dispatchEventToListeners(WebInspector.DataGrid.Events.ColumnsResized);},setColumnsVisiblity:function(columnsVisibility)
{this._visibleColumnsArray=[];for(var i=0;i<this._columnsArray.length;++i){var column=this._columnsArray[i];if(columnsVisibility[column.identifier])
this._visibleColumnsArray.push(column);}
this._refreshHeader();this._applyColumnWeights();var nodes=this._enumerateChildren(this.rootNode(),[],-1);for(var i=0;i<nodes.length;++i)
nodes[i].refresh();},get scrollContainer()
{return this._scrollContainer;},_positionResizers:function()
{var headerTableColumns=this._headerTableColumnGroup.children;var numColumns=headerTableColumns.length-1;var left=[];var resizers=this._resizers;while(resizers.length>numColumns-1)
resizers.pop().remove();for(var i=0;i<numColumns-1;i++){left[i]=(left[i-1]||0)+this.headerTableBody.rows[0].cells[i].offsetWidth;}
for(var i=0;i<numColumns-1;i++){var resizer=resizers[i];if(!resizer){resizer=createElement("div");resizer.__index=i;resizer.classList.add("data-grid-resizer");WebInspector.installDragHandle(resizer,this._startResizerDragging.bind(this),this._resizerDragging.bind(this),this._endResizerDragging.bind(this),"col-resize");this.element.appendChild(resizer);resizers.push(resizer);}
if(resizer.__position!==left[i]){resizer.__position=left[i];resizer.style.left=left[i]+"px";}}},addCreationNode:function(hasChildren)
{if(this.creationNode)
this.creationNode.makeNormal();var emptyData={};for(var column in this._columns)
emptyData[column]=null;this.creationNode=new WebInspector.CreationDataGridNode(emptyData,hasChildren);this.rootNode().appendChild(this.creationNode);},_keyDown:function(event)
{if(!this.selectedNode||event.shiftKey||event.metaKey||event.ctrlKey||this._editing)
return;var handled=false;var nextSelectedNode;if(event.keyIdentifier==="Up"&&!event.altKey){nextSelectedNode=this.selectedNode.traversePreviousNode(true);while(nextSelectedNode&&!nextSelectedNode.selectable)
nextSelectedNode=nextSelectedNode.traversePreviousNode(true);handled=nextSelectedNode?true:false;}else if(event.keyIdentifier==="Down"&&!event.altKey){nextSelectedNode=this.selectedNode.traverseNextNode(true);while(nextSelectedNode&&!nextSelectedNode.selectable)
nextSelectedNode=nextSelectedNode.traverseNextNode(true);handled=nextSelectedNode?true:false;}else if(event.keyIdentifier==="Left"){if(this.selectedNode.expanded){if(event.altKey)
this.selectedNode.collapseRecursively();else
this.selectedNode.collapse();handled=true;}else if(this.selectedNode.parent&&!this.selectedNode.parent._isRoot){handled=true;if(this.selectedNode.parent.selectable){nextSelectedNode=this.selectedNode.parent;handled=nextSelectedNode?true:false;}else if(this.selectedNode.parent)
this.selectedNode.parent.collapse();}}else if(event.keyIdentifier==="Right"){if(!this.selectedNode.revealed){this.selectedNode.reveal();handled=true;}else if(this.selectedNode.hasChildren){handled=true;if(this.selectedNode.expanded){nextSelectedNode=this.selectedNode.children[0];handled=nextSelectedNode?true:false;}else{if(event.altKey)
this.selectedNode.expandRecursively();else
this.selectedNode.expand();}}}else if(event.keyCode===8||event.keyCode===46){if(this._deleteCallback){handled=true;this._deleteCallback(this.selectedNode);this.changeNodeAfterDeletion();}}else if(isEnterKey(event)){if(this._editCallback){handled=true;this._startEditing(this.selectedNode._element.children[this._nextEditableColumn(-1)]);}}
if(nextSelectedNode){nextSelectedNode.reveal();nextSelectedNode.select();}
if(handled)
event.consume(true);},changeNodeAfterDeletion:function()
{var nextSelectedNode=this.selectedNode.traverseNextNode(true);while(nextSelectedNode&&!nextSelectedNode.selectable)
nextSelectedNode=nextSelectedNode.traverseNextNode(true);if(!nextSelectedNode||nextSelectedNode.isCreationNode){nextSelectedNode=this.selectedNode.traversePreviousNode(true);while(nextSelectedNode&&!nextSelectedNode.selectable)
nextSelectedNode=nextSelectedNode.traversePreviousNode(true);}
if(nextSelectedNode){nextSelectedNode.reveal();nextSelectedNode.select();}},dataGridNodeFromNode:function(target)
{var rowElement=target.enclosingNodeOrSelfWithNodeName("tr");return rowElement&&rowElement._dataGridNode;},columnIdentifierFromNode:function(target)
{var cellElement=target.enclosingNodeOrSelfWithNodeName("td");return cellElement&&cellElement.columnIdentifier_;},_clickInHeaderCell:function(event)
{var cell=event.target.enclosingNodeOrSelfWithNodeName("th");if(!cell||(cell.columnIdentifier===undefined)||!cell.classList.contains("sortable"))
return;var sortOrder=WebInspector.DataGrid.Order.Ascending;if((cell===this._sortColumnCell)&&this.isSortOrderAscending())
sortOrder=WebInspector.DataGrid.Order.Descending;if(this._sortColumnCell)
this._sortColumnCell.classList.remove(WebInspector.DataGrid.Order.Ascending,WebInspector.DataGrid.Order.Descending);this._sortColumnCell=cell;cell.classList.add(sortOrder);this.dispatchEventToListeners(WebInspector.DataGrid.Events.SortingChanged);},markColumnAsSortedBy:function(columnIdentifier,sortOrder)
{if(this._sortColumnCell)
this._sortColumnCell.classList.remove(WebInspector.DataGrid.Order.Ascending,WebInspector.DataGrid.Order.Descending);this._sortColumnCell=this._headerTableHeaders[columnIdentifier];this._sortColumnCell.classList.add(sortOrder);},headerTableHeader:function(columnIdentifier)
{return this._headerTableHeaders[columnIdentifier];},_mouseDownInDataTable:function(event)
{var gridNode=this.dataGridNodeFromNode(event.target);if(!gridNode||!gridNode.selectable)
return;if(gridNode.isEventWithinDisclosureTriangle(event))
return;var columnIdentifier=this.columnIdentifierFromNode(event.target);if(columnIdentifier&&this._columns[columnIdentifier].nonSelectable)
return;if(event.metaKey){if(gridNode.selected)
gridNode.deselect();else
gridNode.select();}else
gridNode.select();},_contextMenuInDataTable:function(event)
{var contextMenu=new WebInspector.ContextMenu(event);var gridNode=this.dataGridNodeFromNode(event.target);if(this._refreshCallback&&(!gridNode||gridNode!==this.creationNode))
contextMenu.appendItem(WebInspector.UIString("Refresh"),this._refreshCallback.bind(this));if(gridNode&&gridNode.selectable&&!gridNode.isEventWithinDisclosureTriangle(event)){if(this._editCallback){if(gridNode===this.creationNode)
contextMenu.appendItem(WebInspector.UIString.capitalize("Add ^new"),this._startEditing.bind(this,event.target));else{var columnIdentifier=this.columnIdentifierFromNode(event.target);if(columnIdentifier&&this._columns[columnIdentifier].editable)
contextMenu.appendItem(WebInspector.UIString("Edit \"%s\"",this._columns[columnIdentifier].title),this._startEditing.bind(this,event.target));}}
if(this._deleteCallback&&gridNode!==this.creationNode)
contextMenu.appendItem(WebInspector.UIString.capitalize("Delete"),this._deleteCallback.bind(this,gridNode));if(this._contextMenuCallback)
this._contextMenuCallback(contextMenu,gridNode);}
contextMenu.show();},_clickInDataTable:function(event)
{var gridNode=this.dataGridNodeFromNode(event.target);if(!gridNode||!gridNode.hasChildren)
return;if(!gridNode.isEventWithinDisclosureTriangle(event))
return;if(gridNode.expanded){if(event.altKey)
gridNode.collapseRecursively();else
gridNode.collapse();}else{if(event.altKey)
gridNode.expandRecursively();else
gridNode.expand();}},setResizeMethod:function(method)
{this._resizeMethod=method;},_startResizerDragging:function(event)
{this._currentResizer=event.target;return true;},_resizerDragging:function(event)
{var resizer=this._currentResizer;if(!resizer)
return;var dragPoint=event.clientX-this.element.totalOffsetLeft();var firstRowCells=this.headerTableBody.rows[0].cells;var leftEdgeOfPreviousColumn=0;var leftCellIndex=resizer.__index;var rightCellIndex=leftCellIndex+1;for(var i=0;i<leftCellIndex;i++)
leftEdgeOfPreviousColumn+=firstRowCells[i].offsetWidth;if(this._resizeMethod===WebInspector.DataGrid.ResizeMethod.Last){rightCellIndex=this._resizers.length;}else if(this._resizeMethod===WebInspector.DataGrid.ResizeMethod.First){leftEdgeOfPreviousColumn+=firstRowCells[leftCellIndex].offsetWidth-firstRowCells[0].offsetWidth;leftCellIndex=0;}
var rightEdgeOfNextColumn=leftEdgeOfPreviousColumn+firstRowCells[leftCellIndex].offsetWidth+firstRowCells[rightCellIndex].offsetWidth;var leftMinimum=leftEdgeOfPreviousColumn+this.ColumnResizePadding;var rightMaximum=rightEdgeOfNextColumn-this.ColumnResizePadding;if(leftMinimum>rightMaximum)
return;dragPoint=Number.constrain(dragPoint,leftMinimum,rightMaximum);var position=(dragPoint-this.CenterResizerOverBorderAdjustment);resizer.__position=position;resizer.style.left=position+"px";var pxLeftColumn=(dragPoint-leftEdgeOfPreviousColumn)+"px";this._headerTableColumnGroup.children[leftCellIndex].style.width=pxLeftColumn;this._dataTableColumnGroup.children[leftCellIndex].style.width=pxLeftColumn;var pxRightColumn=(rightEdgeOfNextColumn-dragPoint)+"px";this._headerTableColumnGroup.children[rightCellIndex].style.width=pxRightColumn;this._dataTableColumnGroup.children[rightCellIndex].style.width=pxRightColumn;var leftColumn=this._visibleColumnsArray[leftCellIndex];var rightColumn=this._visibleColumnsArray[rightCellIndex];if(leftColumn.weight||rightColumn.weight){var sumOfWeights=leftColumn.weight+rightColumn.weight;var delta=rightEdgeOfNextColumn-leftEdgeOfPreviousColumn;leftColumn.weight=(dragPoint-leftEdgeOfPreviousColumn)*sumOfWeights/delta;rightColumn.weight=(rightEdgeOfNextColumn-dragPoint)*sumOfWeights/delta;}
this._positionResizers();event.preventDefault();this.dispatchEventToListeners(WebInspector.DataGrid.Events.ColumnsResized);},columnOffset:function(columnId)
{if(!this.element.offsetWidth)
return 0;for(var i=1;i<this._visibleColumnsArray.length;++i){if(columnId===this._visibleColumnsArray[i].identifier){if(this._resizers[i-1])
return this._resizers[i-1].__position;}}
return 0;},_endResizerDragging:function(event)
{this._currentResizer=null;this._saveColumnWeights();this.dispatchEventToListeners(WebInspector.DataGrid.Events.ColumnsResized);},ColumnResizePadding:24,CenterResizerOverBorderAdjustment:3,__proto__:WebInspector.Object.prototype}
WebInspector.DataGrid.ResizeMethod={Nearest:"nearest",First:"first",Last:"last"}
WebInspector.DataGridNode=function(data,hasChildren)
{this._element=null;this._expanded=false;this._selected=false;this._depth;this._revealed;this._attached=false;this._savedPosition=null;this._shouldRefreshChildren=true;this._data=data||{};this.hasChildren=hasChildren||false;this.children=[];this.dataGrid=null;this.parent=null;this.previousSibling=null;this.nextSibling=null;this.disclosureToggleWidth=10;}
WebInspector.DataGridNode.prototype={selectable:true,_isRoot:false,element:function()
{if(!this._element){this.createElement();this.createCells();}
return(this._element);},createElement:function()
{this._element=createElement("tr");this._element._dataGridNode=this;if(this.hasChildren)
this._element.classList.add("parent");if(this.expanded)
this._element.classList.add("expanded");if(this.selected)
this._element.classList.add("selected");if(this.revealed)
this._element.classList.add("revealed");},createCells:function()
{this._element.removeChildren();var columnsArray=this.dataGrid._visibleColumnsArray;for(var i=0;i<columnsArray.length;++i)
this._element.appendChild(this.createCell(columnsArray[i].identifier));this._element.appendChild(this._createTDWithClass("corner"));},get data()
{return this._data;},set data(x)
{this._data=x||{};this.refresh();},get revealed()
{if(this._revealed!==undefined)
return this._revealed;var currentAncestor=this.parent;while(currentAncestor&&!currentAncestor._isRoot){if(!currentAncestor.expanded){this._revealed=false;return false;}
currentAncestor=currentAncestor.parent;}
this._revealed=true;return true;},set hasChildren(x)
{if(this._hasChildren===x)
return;this._hasChildren=x;if(!this._element)
return;this._element.classList.toggle("parent",this._hasChildren);this._element.classList.toggle("expanded",this._hasChildren&&this.expanded);},get hasChildren()
{return this._hasChildren;},set revealed(x)
{if(this._revealed===x)
return;this._revealed=x;if(this._element)
this._element.classList.toggle("revealed",this._revealed);for(var i=0;i<this.children.length;++i)
this.children[i].revealed=x&&this.expanded;},get depth()
{if(this._depth!==undefined)
return this._depth;if(this.parent&&!this.parent._isRoot)
this._depth=this.parent.depth+1;else
this._depth=0;return this._depth;},get leftPadding()
{return this.depth*this.dataGrid.indentWidth;},get shouldRefreshChildren()
{return this._shouldRefreshChildren;},set shouldRefreshChildren(x)
{this._shouldRefreshChildren=x;if(x&&this.expanded)
this.expand();},get selected()
{return this._selected;},set selected(x)
{if(x)
this.select();else
this.deselect();},get expanded()
{return this._expanded;},set expanded(x)
{if(x)
this.expand();else
this.collapse();},refresh:function()
{if(!this.dataGrid)
this._element=null;if(!this._element)
return;this.createCells();},_createTDWithClass:function(className)
{var cell=createElementWithClass("td",className);var cellClass=this.dataGrid._cellClass;if(cellClass)
cell.classList.add(cellClass);return cell;},createTD:function(columnIdentifier)
{var cell=this._createTDWithClass(columnIdentifier+"-column");cell.columnIdentifier_=columnIdentifier;var alignment=this.dataGrid._columns[columnIdentifier].align;if(alignment)
cell.classList.add(alignment);if(columnIdentifier===this.dataGrid.disclosureColumnIdentifier){cell.classList.add("disclosure");if(this.leftPadding)
cell.style.setProperty("padding-left",this.leftPadding+"px");}
return cell;},createCell:function(columnIdentifier)
{var cell=this.createTD(columnIdentifier);var data=this.data[columnIdentifier];if(data instanceof Node){cell.appendChild(data);}else{cell.textContent=data;if(this.dataGrid._columns[columnIdentifier].longText)
cell.title=data;}
return cell;},nodeSelfHeight:function()
{return 16;},appendChild:function(child)
{this.insertChild(child,this.children.length);},insertChild:function(child,index)
{if(!child)
throw("insertChild: Node can't be undefined or null.");if(child.parent===this){var currentIndex=this.children.indexOf(child);if(currentIndex<0)
console.assert(false,"Inconsistent DataGrid state");if(currentIndex===index)
return;if(currentIndex<index)
--index;}
child.remove();this.children.splice(index,0,child);this.hasChildren=true;child.parent=this;child.dataGrid=this.dataGrid;child.recalculateSiblings(index);child._depth=undefined;child._revealed=undefined;child._attached=false;child._shouldRefreshChildren=true;var current=child.children[0];while(current){current.dataGrid=this.dataGrid;current._depth=undefined;current._revealed=undefined;current._attached=false;current._shouldRefreshChildren=true;current=current.traverseNextNode(false,child,true);}
if(this.expanded)
child._attach();if(!this.revealed)
child.revealed=false;},remove:function()
{if(this.parent)
this.parent.removeChild(this);},removeChild:function(child)
{if(!child)
throw("removeChild: Node can't be undefined or null.");if(child.parent!==this)
throw("removeChild: Node is not a child of this node.");child.deselect();child._detach();this.children.remove(child,true);if(child.previousSibling)
child.previousSibling.nextSibling=child.nextSibling;if(child.nextSibling)
child.nextSibling.previousSibling=child.previousSibling;child.dataGrid=null;child.parent=null;child.nextSibling=null;child.previousSibling=null;if(this.children.length<=0)
this.hasChildren=false;},removeChildren:function()
{for(var i=0;i<this.children.length;++i){var child=this.children[i];child.deselect();child._detach();child.dataGrid=null;child.parent=null;child.nextSibling=null;child.previousSibling=null;}
this.children=[];this.hasChildren=false;},recalculateSiblings:function(myIndex)
{if(!this.parent)
return;var previousChild=this.parent.children[myIndex-1]||null;if(previousChild)
previousChild.nextSibling=this;this.previousSibling=previousChild;var nextChild=this.parent.children[myIndex+1]||null;if(nextChild)
nextChild.previousSibling=this;this.nextSibling=nextChild;},collapse:function()
{if(this._isRoot)
return;if(this._element)
this._element.classList.remove("expanded");this._expanded=false;for(var i=0;i<this.children.length;++i)
this.children[i].revealed=false;},collapseRecursively:function()
{var item=this;while(item){if(item.expanded)
item.collapse();item=item.traverseNextNode(false,this,true);}},populate:function(){},expand:function()
{if(!this.hasChildren||this.expanded)
return;if(this._isRoot)
return;if(this.revealed&&!this._shouldRefreshChildren)
for(var i=0;i<this.children.length;++i)
this.children[i].revealed=true;if(this._shouldRefreshChildren){for(var i=0;i<this.children.length;++i)
this.children[i]._detach();this.populate();if(this._attached){for(var i=0;i<this.children.length;++i){var child=this.children[i];if(this.revealed)
child.revealed=true;child._attach();}}
this._shouldRefreshChildren=false;}
if(this._element)
this._element.classList.add("expanded");this._expanded=true;},expandRecursively:function()
{var item=this;while(item){item.expand();item=item.traverseNextNode(false,this);}},reveal:function()
{if(this._isRoot)
return;var currentAncestor=this.parent;while(currentAncestor&&!currentAncestor._isRoot){if(!currentAncestor.expanded)
currentAncestor.expand();currentAncestor=currentAncestor.parent;}
this.element().scrollIntoViewIfNeeded(false);},select:function(supressSelectedEvent)
{if(!this.dataGrid||!this.selectable||this.selected)
return;if(this.dataGrid.selectedNode)
this.dataGrid.selectedNode.deselect();this._selected=true;this.dataGrid.selectedNode=this;if(this._element)
this._element.classList.add("selected");if(!supressSelectedEvent)
this.dataGrid.dispatchEventToListeners(WebInspector.DataGrid.Events.SelectedNode);},revealAndSelect:function()
{if(this._isRoot)
return;this.reveal();this.select();},deselect:function(supressDeselectedEvent)
{if(!this.dataGrid||this.dataGrid.selectedNode!==this||!this.selected)
return;this._selected=false;this.dataGrid.selectedNode=null;if(this._element)
this._element.classList.remove("selected");if(!supressDeselectedEvent)
this.dataGrid.dispatchEventToListeners(WebInspector.DataGrid.Events.DeselectedNode);},traverseNextNode:function(skipHidden,stayWithin,dontPopulate,info)
{if(!dontPopulate&&this.hasChildren)
this.populate();if(info)
info.depthChange=0;var node=(!skipHidden||this.revealed)?this.children[0]:null;if(node&&(!skipHidden||this.expanded)){if(info)
info.depthChange=1;return node;}
if(this===stayWithin)
return null;node=(!skipHidden||this.revealed)?this.nextSibling:null;if(node)
return node;node=this;while(node&&!node._isRoot&&!((!skipHidden||node.revealed)?node.nextSibling:null)&&node.parent!==stayWithin){if(info)
info.depthChange-=1;node=node.parent;}
if(!node)
return null;return(!skipHidden||node.revealed)?node.nextSibling:null;},traversePreviousNode:function(skipHidden,dontPopulate)
{var node=(!skipHidden||this.revealed)?this.previousSibling:null;if(!dontPopulate&&node&&node.hasChildren)
node.populate();while(node&&((!skipHidden||(node.revealed&&node.expanded))?node.children[node.children.length-1]:null)){if(!dontPopulate&&node.hasChildren)
node.populate();node=((!skipHidden||(node.revealed&&node.expanded))?node.children[node.children.length-1]:null);}
if(node)
return node;if(!this.parent||this.parent._isRoot)
return null;return this.parent;},isEventWithinDisclosureTriangle:function(event)
{if(!this.hasChildren)
return false;var cell=event.target.enclosingNodeOrSelfWithNodeName("td");if(!cell||!cell.classList.contains("disclosure"))
return false;var left=cell.totalOffsetLeft()+this.leftPadding;return event.pageX>=left&&event.pageX<=left+this.disclosureToggleWidth;},_attach:function()
{if(!this.dataGrid||this._attached)
return;this._attached=true;var previousNode=this.traversePreviousNode(true,true);var previousElement=previousNode?previousNode.element():this.dataGrid._topFillerRow;this.dataGrid.dataTableBody.insertBefore(this.element(),previousElement.nextSibling);if(this.expanded)
for(var i=0;i<this.children.length;++i)
this.children[i]._attach();},_detach:function()
{if(!this._attached)
return;this._attached=false;if(this._element)
this._element.remove();for(var i=0;i<this.children.length;++i)
this.children[i]._detach();this.wasDetached();},wasDetached:function()
{},savePosition:function()
{if(this._savedPosition)
return;if(!this.parent)
throw("savePosition: Node must have a parent.");this._savedPosition={parent:this.parent,index:this.parent.children.indexOf(this)};},restorePosition:function()
{if(!this._savedPosition)
return;if(this.parent!==this._savedPosition.parent)
this._savedPosition.parent.insertChild(this,this._savedPosition.index);this._savedPosition=null;},__proto__:WebInspector.Object.prototype}
WebInspector.CreationDataGridNode=function(data,hasChildren)
{WebInspector.DataGridNode.call(this,data,hasChildren);this.isCreationNode=true;}
WebInspector.CreationDataGridNode.prototype={makeNormal:function()
{this.isCreationNode=false;},__proto__:WebInspector.DataGridNode.prototype}
WebInspector.DataGridContainerWidget=function()
{WebInspector.VBox.call(this);this._dataGrids=[];}
WebInspector.DataGridContainerWidget.prototype={appendDataGrid:function(dataGrid)
{this._dataGrids.push(dataGrid);this.element.appendChild(dataGrid.element);},removeDataGrid:function(dataGrid)
{this._dataGrids.remove(dataGrid);this.element.removeChild(dataGrid.element);},wasShown:function()
{for(var dataGrid of this._dataGrids)
dataGrid.wasShown();},willHide:function()
{for(var dataGrid of this._dataGrids)
dataGrid.willHide();},onResize:function()
{for(var dataGrid of this._dataGrids)
dataGrid.onResize();},elementsToRestoreScrollPositionsFor:function()
{var result=[];for(var dataGrid of this._dataGrids){if(!dataGrid._inline)
result.push(dataGrid._scrollContainer);}
return result;},detachChildWidgets:function()
{WebInspector.Widget.prototype.detachChildWidgets.call(this);for(var dataGrid of this._dataGrids)
this.element.removeChild(dataGrid.element);this._dataGrids=[];},__proto__:WebInspector.VBox.prototype};WebInspector.ViewportDataGrid=function(columnsArray,editCallback,deleteCallback,refreshCallback,contextMenuCallback)
{WebInspector.DataGrid.call(this,columnsArray,editCallback,deleteCallback,refreshCallback,contextMenuCallback);this._scrollContainer.addEventListener("scroll",this._onScroll.bind(this),true);this._scrollContainer.addEventListener("mousewheel",this._onWheel.bind(this),true);this._visibleNodes=[];this._flatNodes=null;this._inline=false;this._wheelTarget=null;this._hiddenWheelTarget=null;this._stickToBottom=false;this._atBottom=true;this._lastScrollTop=0;this.setRootNode(new WebInspector.ViewportDataGridNode());}
WebInspector.ViewportDataGrid.prototype={onResize:function()
{if(this._stickToBottom&&this._atBottom)
this._scrollContainer.scrollTop=this._scrollContainer.scrollHeight-this._scrollContainer.clientHeight;this.scheduleUpdate();WebInspector.DataGrid.prototype.onResize.call(this);},setStickToBottom:function(stick)
{this._stickToBottom=stick;},_onWheel:function(event)
{this._wheelTarget=event.target?event.target.enclosingNodeOrSelfWithNodeName("tr"):null;},_onScroll:function(event)
{this._atBottom=this._scrollContainer.isScrolledToBottom();if(this._lastScrollTop!==this._scrollContainer.scrollTop)
this.scheduleUpdate();},scheduleUpdateStructure:function()
{this._flatNodes=null;this.scheduleUpdate();},scheduleUpdate:function()
{if(this._updateAnimationFrameId)
return;this._updateAnimationFrameId=this.element.window().requestAnimationFrame(this._update.bind(this));},updateInstantlyForTests:function()
{if(!this._updateAnimationFrameId)
return;this.element.window().cancelAnimationFrame(this._updateAnimationFrameId);this._update();},renderInline:function()
{this._inline=true;WebInspector.DataGrid.prototype.renderInline.call(this);this._update();},_flatNodesList:function()
{if(this._flatNodes)
return this._flatNodes;var flatNodes=[];var children=[this._rootNode.children];var counters=[0];var depth=0;while(depth>=0){var node=children[depth][counters[depth]++];if(!node){depth--;continue;}
flatNodes.push(node);node.setDepth(depth);if(node._expanded&&node.children.length){depth++;children[depth]=node.children;counters[depth]=0;}}
this._flatNodes=flatNodes;return this._flatNodes;},_calculateVisibleNodes:function(clientHeight,scrollTop)
{var nodes=this._flatNodesList();if(this._inline)
return{topPadding:0,bottomPadding:0,contentHeight:0,visibleNodes:nodes,offset:0};var size=nodes.length;var i=0;var y=0;for(;i<size&&y+nodes[i].nodeSelfHeight()<scrollTop;++i)
y+=nodes[i].nodeSelfHeight();var start=i;var topPadding=y;for(;i<size&&y<scrollTop+clientHeight;++i)
y+=nodes[i].nodeSelfHeight();var end=i;var bottomPadding=0;for(;i<size;++i)
bottomPadding+=nodes[i].nodeSelfHeight();return{topPadding:topPadding,bottomPadding:bottomPadding,contentHeight:y-topPadding,visibleNodes:nodes.slice(start,end),offset:start};},_contentHeight:function()
{var nodes=this._flatNodesList();var result=0;for(var i=0,size=nodes.length;i<size;++i)
result+=nodes[i].nodeSelfHeight();return result;},_update:function()
{delete this._updateAnimationFrameId;var clientHeight=this._scrollContainer.clientHeight;var scrollTop=this._scrollContainer.scrollTop;var currentScrollTop=scrollTop;var maxScrollTop=Math.max(0,this._contentHeight()-clientHeight);if(this._stickToBottom&&this._atBottom)
scrollTop=maxScrollTop;scrollTop=Math.min(maxScrollTop,scrollTop);this._atBottom=scrollTop===maxScrollTop;var viewportState=this._calculateVisibleNodes(clientHeight,scrollTop);var visibleNodes=viewportState.visibleNodes;var visibleNodesSet=new Set(visibleNodes);if(this._hiddenWheelTarget&&this._hiddenWheelTarget!==this._wheelTarget){this._hiddenWheelTarget.remove();this._hiddenWheelTarget=null;}
for(var i=0;i<this._visibleNodes.length;++i){var oldNode=this._visibleNodes[i];if(!visibleNodesSet.has(oldNode)&&oldNode.attached()){var element=oldNode._element;if(element===this._wheelTarget)
this._hiddenWheelTarget=oldNode.abandonElement();else
element.remove();oldNode.wasDetached();}}
var previousElement=this._topFillerRow;if(previousElement.nextSibling===this._hiddenWheelTarget)
previousElement=this._hiddenWheelTarget;var tBody=this.dataTableBody;var offset=viewportState.offset;for(var i=0;i<visibleNodes.length;++i){var node=visibleNodes[i];var element=node.element();node.willAttach();element.classList.toggle("odd",(offset+i)%2===0);tBody.insertBefore(element,previousElement.nextSibling);previousElement=element;}
this.setVerticalPadding(viewportState.topPadding,viewportState.bottomPadding);var contentFits=viewportState.contentHeight<=clientHeight;this.element.classList.toggle("data-grid-fits-viewport",contentFits&&viewportState.topPadding+viewportState.bottomPadding===0);this._lastScrollTop=scrollTop;if(scrollTop!==currentScrollTop)
this._scrollContainer.scrollTop=scrollTop;this._visibleNodes=visibleNodes;},_revealViewportNode:function(node)
{var nodes=this._flatNodesList();var index=nodes.indexOf(node);if(index===-1)
return;var fromY=0;for(var i=0;i<index;++i)
fromY+=nodes[i].nodeSelfHeight();var toY=fromY+node.nodeSelfHeight();var scrollTop=this._scrollContainer.scrollTop;if(scrollTop>fromY){scrollTop=fromY;this._atBottom=false;}else if(scrollTop+this._scrollContainer.offsetHeight<toY){scrollTop=toY-this._scrollContainer.offsetHeight;}
this._scrollContainer.scrollTop=scrollTop;},__proto__:WebInspector.DataGrid.prototype}
WebInspector.ViewportDataGridNode=function(data,hasChildren)
{WebInspector.DataGridNode.call(this,data,hasChildren);this._stale=false;}
WebInspector.ViewportDataGridNode.prototype={element:function()
{if(!this._element){this.createElement();this.createCells();this._stale=false;}
if(this._stale){this.createCells();this._stale=false;}
return(this._element);},setDepth:function(depth)
{this._depth=depth;},insertChild:function(child,index)
{if(child.parent===this){var currentIndex=this.children.indexOf(child);if(currentIndex<0)
console.assert(false,"Inconsistent DataGrid state");if(currentIndex===index)
return;if(currentIndex<index)
--index;}
child.remove();child.parent=this;child.dataGrid=this.dataGrid;if(!this.children.length)
this.hasChildren=true;this.children.splice(index,0,child);child.recalculateSiblings(index);if(this._expanded)
this.dataGrid.scheduleUpdateStructure();},removeChild:function(child)
{if(child.previousSibling)
child.previousSibling.nextSibling=child.nextSibling;if(child.nextSibling)
child.nextSibling.previousSibling=child.previousSibling;if(child.parent!==this)
throw("removeChild: Node is not a child of this node.");child._unlink();this.children.remove(child,true);if(!this.children.length)
this.hasChildren=false;if(this._expanded)
this.dataGrid.scheduleUpdateStructure();},removeChildren:function()
{for(var i=0;i<this.children.length;++i)
this.children[i]._unlink();this.children=[];if(this._expanded)
this.dataGrid.scheduleUpdateStructure();},_unlink:function()
{if(this.attached()){this._element.remove();this.wasDetached();}
this.deselect();this.dataGrid=null;this.parent=null;this.nextSibling=null;this.previousSibling=null;},collapse:function()
{if(!this._expanded)
return;this._expanded=false;if(this._element)
this._element.classList.remove("expanded");this.dataGrid.scheduleUpdateStructure();},expand:function()
{if(this._expanded)
return;WebInspector.DataGridNode.prototype.expand.call(this);this.dataGrid.scheduleUpdateStructure();},willAttach:function(){},attached:function()
{return!!(this.dataGrid&&this._element&&this._element.parentElement);},refresh:function()
{if(this.attached()){this._stale=true;this.dataGrid.scheduleUpdate();}else{this._element=null;}},abandonElement:function()
{var result=this._element;if(result)
result.style.display="none";this._element=null;return result;},reveal:function()
{this.dataGrid._revealViewportNode(this);},__proto__:WebInspector.DataGridNode.prototype};WebInspector.SortableDataGrid=function(columnsArray,editCallback,deleteCallback,refreshCallback,contextMenuCallback)
{WebInspector.ViewportDataGrid.call(this,columnsArray,editCallback,deleteCallback,refreshCallback,contextMenuCallback);this._sortingFunction=WebInspector.SortableDataGrid.TrivialComparator;this.setRootNode(new WebInspector.SortableDataGridNode());}
WebInspector.SortableDataGrid.NodeComparator;WebInspector.SortableDataGrid.TrivialComparator=function(a,b)
{return 0;}
WebInspector.SortableDataGrid.NumericComparator=function(columnIdentifier,a,b)
{var aValue=a.data[columnIdentifier];var bValue=b.data[columnIdentifier];var aNumber=Number(aValue instanceof Node?aValue.textContent:aValue);var bNumber=Number(bValue instanceof Node?bValue.textContent:bValue);return aNumber<bNumber?-1:(aNumber>bNumber?1:0);}
WebInspector.SortableDataGrid.StringComparator=function(columnIdentifier,a,b)
{var aValue=a.data[columnIdentifier];var bValue=b.data[columnIdentifier];var aString=aValue instanceof Node?aValue.textContent:String(aValue);var bString=bValue instanceof Node?bValue.textContent:String(bValue);return aString<bString?-1:(aString>bString?1:0);}
WebInspector.SortableDataGrid.Comparator=function(comparator,reverseMode,a,b)
{return reverseMode?comparator(b,a):comparator(a,b);}
WebInspector.SortableDataGrid.create=function(columnNames,values)
{var numColumns=columnNames.length;if(!numColumns)
return null;var columns=[];for(var i=0;i<columnNames.length;++i)
columns.push({title:columnNames[i],width:columnNames[i].length,sortable:true});var nodes=[];for(var i=0;i<values.length/numColumns;++i){var data={};for(var j=0;j<columnNames.length;++j)
data[j]=values[numColumns*i+j];var node=new WebInspector.SortableDataGridNode(data);node.selectable=false;nodes.push(node);}
var dataGrid=new WebInspector.SortableDataGrid(columns);var length=nodes.length;var rootNode=dataGrid.rootNode();for(var i=0;i<length;++i)
rootNode.appendChild(nodes[i]);dataGrid.addEventListener(WebInspector.DataGrid.Events.SortingChanged,sortDataGrid);function sortDataGrid()
{var nodes=dataGrid.rootNode().children;var sortColumnIdentifier=dataGrid.sortColumnIdentifier();if(!sortColumnIdentifier)
return;var columnIsNumeric=true;for(var i=0;i<nodes.length;i++){var value=nodes[i].data[sortColumnIdentifier];if(isNaN(value instanceof Node?value.textContent:value)){columnIsNumeric=false;break;}}
var comparator=columnIsNumeric?WebInspector.SortableDataGrid.NumericComparator:WebInspector.SortableDataGrid.StringComparator;dataGrid.sortNodes(comparator.bind(null,sortColumnIdentifier),!dataGrid.isSortOrderAscending());}
return dataGrid;}
WebInspector.SortableDataGrid.prototype={insertChild:function(node)
{var root=(this.rootNode());root.insertChildOrdered(node);},sortNodes:function(comparator,reverseMode)
{this._sortingFunction=WebInspector.SortableDataGrid.Comparator.bind(null,comparator,reverseMode);this._rootNode._sortChildren(reverseMode);this.scheduleUpdateStructure();},__proto__:WebInspector.ViewportDataGrid.prototype}
WebInspector.SortableDataGridNode=function(data,hasChildren)
{WebInspector.ViewportDataGridNode.call(this,data,hasChildren);}
WebInspector.SortableDataGridNode.prototype={insertChildOrdered:function(node)
{this.insertChild(node,this.children.upperBound(node,this.dataGrid._sortingFunction));},_sortChildren:function()
{this.children.sort(this.dataGrid._sortingFunction);for(var i=0;i<this.children.length;++i)
this.children[i].recalculateSiblings(i);for(var child of this.children)
child._sortChildren();},__proto__:WebInspector.ViewportDataGridNode.prototype};WebInspector.ShowMoreDataGridNode=function(callback,startPosition,endPosition,chunkSize)
{WebInspector.DataGridNode.call(this,{summaryRow:true},false);this._callback=callback;this._startPosition=startPosition;this._endPosition=endPosition;this._chunkSize=chunkSize;this.showNext=createElement("button");this.showNext.setAttribute("type","button");this.showNext.addEventListener("click",this._showNextChunk.bind(this),false);this.showNext.textContent=WebInspector.UIString("Show %d before",this._chunkSize);this.showAll=createElement("button");this.showAll.setAttribute("type","button");this.showAll.addEventListener("click",this._showAll.bind(this),false);this.showLast=createElement("button");this.showLast.setAttribute("type","button");this.showLast.addEventListener("click",this._showLastChunk.bind(this),false);this.showLast.textContent=WebInspector.UIString("Show %d after",this._chunkSize);this._updateLabels();this.selectable=false;}
WebInspector.ShowMoreDataGridNode.prototype={_showNextChunk:function()
{this._callback(this._startPosition,this._startPosition+this._chunkSize);},_showAll:function()
{this._callback(this._startPosition,this._endPosition);},_showLastChunk:function()
{this._callback(this._endPosition-this._chunkSize,this._endPosition);},_updateLabels:function()
{var totalSize=this._endPosition-this._startPosition;if(totalSize>this._chunkSize){this.showNext.classList.remove("hidden");this.showLast.classList.remove("hidden");}else{this.showNext.classList.add("hidden");this.showLast.classList.add("hidden");}
this.showAll.textContent=WebInspector.UIString("Show all %d",totalSize);},createCells:function()
{this._hasCells=false;WebInspector.DataGridNode.prototype.createCells.call(this);},createCell:function(columnIdentifier)
{var cell=this.createTD(columnIdentifier);if(!this._hasCells){this._hasCells=true;if(this.depth)
cell.style.setProperty("padding-left",(this.depth*this.dataGrid.indentWidth)+"px");cell.appendChild(this.showNext);cell.appendChild(this.showAll);cell.appendChild(this.showLast);}
return cell;},setStartPosition:function(from)
{this._startPosition=from;this._updateLabels();},setEndPosition:function(to)
{this._endPosition=to;this._updateLabels();},nodeSelfHeight:function()
{return 32;},dispose:function()
{},__proto__:WebInspector.DataGridNode.prototype};WebInspector.FlameChartDelegate=function(){}
WebInspector.FlameChartDelegate.prototype={requestWindowTimes:function(startTime,endTime){},updateRangeSelection:function(startTime,endTime){},endRangeSelection:function(){}}
WebInspector.FlameChart=function(dataProvider,flameChartDelegate,isTopDown)
{WebInspector.HBox.call(this,true);this.registerRequiredCSS("ui_lazy/flameChart.css");this.contentElement.classList.add("flame-chart-main-pane");this._flameChartDelegate=flameChartDelegate;this._isTopDown=isTopDown;this._calculator=new WebInspector.FlameChart.Calculator();this._canvas=this.contentElement.createChild("canvas");this._canvas.tabIndex=1;this.setDefaultFocusedElement(this._canvas);this._canvas.addEventListener("mousemove",this._onMouseMove.bind(this),false);this._canvas.addEventListener("mouseout",this._onMouseOut.bind(this),false);this._canvas.addEventListener("mousewheel",this._onMouseWheel.bind(this),false);this._canvas.addEventListener("click",this._onClick.bind(this),false);this._canvas.addEventListener("keydown",this._onKeyDown.bind(this),false);WebInspector.installDragHandle(this._canvas,this._startCanvasDragging.bind(this),this._canvasDragging.bind(this),this._endCanvasDragging.bind(this),"-webkit-grabbing",null);WebInspector.installDragHandle(this._canvas,this._startRangeSelection.bind(this),this._rangeSelectionDragging.bind(this),this._endRangeSelection.bind(this),"text",null);this._vScrollElement=this.contentElement.createChild("div","flame-chart-v-scroll");this._vScrollContent=this._vScrollElement.createChild("div");this._vScrollElement.addEventListener("scroll",this._onScroll.bind(this),false);this._scrollTop=0;this._entryInfo=this.contentElement.createChild("div","flame-chart-entry-info");this._markerHighlighElement=this.contentElement.createChild("div","flame-chart-marker-highlight-element");this._highlightElement=this.contentElement.createChild("div","flame-chart-highlight-element");this._selectedElement=this.contentElement.createChild("div","flame-chart-selected-element");this._selectionOverlay=this.contentElement.createChild("div","flame-chart-selection-overlay hidden");this._selectedTimeSpanLabel=this._selectionOverlay.createChild("div","time-span");this._dataProvider=dataProvider;this._windowLeft=0.0;this._windowRight=1.0;this._windowWidth=1.0;this._timeWindowLeft=0;this._timeWindowRight=Infinity;this._barHeight=dataProvider.barHeight();this._barHeightDelta=this._isTopDown?-this._barHeight:this._barHeight;this._paddingLeft=this._dataProvider.paddingLeft();this._markerPadding=2;this._markerRadius=this._barHeight/2-this._markerPadding;this._highlightedMarkerIndex=-1;this._highlightedEntryIndex=-1;this._selectedEntryIndex=-1;this._rawTimelineDataLength=0;this._textWidth={};this._lastMouseOffsetX=0;}
WebInspector.FlameChart.DividersBarHeight=18;WebInspector.FlameChart.MinimalTimeWindowMs=0.01;WebInspector.FlameChartDataProvider=function()
{}
WebInspector.FlameChart.TimelineData=function(entryLevels,entryTotalTimes,entryStartTimes)
{this.entryLevels=entryLevels;this.entryTotalTimes=entryTotalTimes;this.entryStartTimes=entryStartTimes;this.markers=[];this.flowStartTimes=[];this.flowStartLevels=[];this.flowEndTimes=[];this.flowEndLevels=[];}
WebInspector.FlameChartDataProvider.prototype={barHeight:function(){},dividerOffsets:function(startTime,endTime){},minimumBoundary:function(){},totalTime:function(){},maxStackDepth:function(){},timelineData:function(){},prepareHighlightedEntryInfo:function(entryIndex){},canJumpToEntry:function(entryIndex){},entryTitle:function(entryIndex){},entryFont:function(entryIndex){},entryColor:function(entryIndex){},decorateEntry:function(entryIndex,context,text,barX,barY,barWidth,barHeight){},forceDecoration:function(entryIndex){},textColor:function(entryIndex){},textBaseline:function(){},textPadding:function(){},highlightTimeRange:function(entryIndex){},paddingLeft:function(){},}
WebInspector.FlameChartMarker=function()
{}
WebInspector.FlameChartMarker.prototype={startTime:function(){},color:function(){},title:function(){},draw:function(context,x,height,pixelsPerMillisecond){},}
WebInspector.FlameChart.Events={EntrySelected:"EntrySelected"}
WebInspector.FlameChart.ColorGenerator=function(hueSpace,satSpace,lightnessSpace,alphaSpace)
{this._hueSpace=hueSpace||{min:0,max:360,count:20};this._satSpace=satSpace||67;this._lightnessSpace=lightnessSpace||80;this._alphaSpace=alphaSpace||1;this._colors={};}
WebInspector.FlameChart.ColorGenerator.prototype={setColorForID:function(id,color)
{this._colors[id]=color;},colorForID:function(id)
{var color=this._colors[id];if(!color){color=this._generateColorForID(id);this._colors[id]=color;}
return color;},_generateColorForID:function(id)
{var hash=Math.abs(String.hashCode(id));var h=this._indexToValueInSpace(hash,this._hueSpace);var s=this._indexToValueInSpace(hash,this._satSpace);var l=this._indexToValueInSpace(hash,this._lightnessSpace);var a=this._indexToValueInSpace(hash,this._alphaSpace);return"hsla("+h+", "+s+"%, "+l+"%, "+a+")";},_indexToValueInSpace:function(index,space)
{if(typeof space==="number")
return space;index%=space.count;return space.min+Math.floor(index/(space.count-1)*(space.max-space.min));}}
WebInspector.FlameChart.Calculator=function()
{this._paddingLeft=0;}
WebInspector.FlameChart.Calculator.prototype={paddingLeft:function()
{return this._paddingLeft;},_updateBoundaries:function(mainPane)
{this._totalTime=mainPane._dataProvider.totalTime();this._zeroTime=mainPane._dataProvider.minimumBoundary();this._minimumBoundaries=this._zeroTime+mainPane._windowLeft*this._totalTime;this._maximumBoundaries=this._zeroTime+mainPane._windowRight*this._totalTime;this._paddingLeft=mainPane._paddingLeft;this._width=mainPane._canvas.width/window.devicePixelRatio-this._paddingLeft;this._timeToPixel=this._width/this.boundarySpan();},computePosition:function(time)
{return Math.round((time-this._minimumBoundaries)*this._timeToPixel+this._paddingLeft);},formatTime:function(value,precision)
{return Number.preciseMillisToString(value-this._zeroTime,precision);},maximumBoundary:function()
{return this._maximumBoundaries;},minimumBoundary:function()
{return this._minimumBoundaries;},zeroTime:function()
{return this._zeroTime;},boundarySpan:function()
{return this._maximumBoundaries-this._minimumBoundaries;}}
WebInspector.FlameChart.prototype={_resetCanvas:function()
{var ratio=window.devicePixelRatio;this._canvas.width=this._offsetWidth*ratio;this._canvas.height=this._offsetHeight*ratio;this._canvas.style.width=this._offsetWidth+"px";this._canvas.style.height=this._offsetHeight+"px";},_timelineData:function()
{var timelineData=this._dataProvider.timelineData();if(timelineData!==this._rawTimelineData||timelineData.entryStartTimes.length!==this._rawTimelineDataLength)
this._processTimelineData(timelineData);return this._rawTimelineData;},_cancelAnimation:function()
{if(this._cancelWindowTimesAnimation){this._timeWindowLeft=this._pendingAnimationTimeLeft;this._timeWindowRight=this._pendingAnimationTimeRight;this._cancelWindowTimesAnimation();delete this._cancelWindowTimesAnimation;}},_revealEntry:function(entryIndex)
{var timelineData=this._timelineData();if(!timelineData)
return;var timeLeft=this._cancelWindowTimesAnimation?this._pendingAnimationTimeLeft:this._timeWindowLeft;var timeRight=this._cancelWindowTimesAnimation?this._pendingAnimationTimeRight:this._timeWindowRight;var entryStartTime=timelineData.entryStartTimes[entryIndex];var entryTotalTime=timelineData.entryTotalTimes[entryIndex];var entryEndTime=entryStartTime+entryTotalTime;var minEntryTimeWindow=Math.min(entryTotalTime,timeRight-timeLeft);var y=this._levelToHeight(timelineData.entryLevels[entryIndex]);if(y<this._vScrollElement.scrollTop)
this._vScrollElement.scrollTop=y;else if(y>this._vScrollElement.scrollTop+this._offsetHeight+this._barHeightDelta)
this._vScrollElement.scrollTop=y-this._offsetHeight-this._barHeightDelta;if(timeLeft>entryEndTime){var delta=timeLeft-entryEndTime+minEntryTimeWindow;this._flameChartDelegate.requestWindowTimes(timeLeft-delta,timeRight-delta);}else if(timeRight<entryStartTime){var delta=entryStartTime-timeRight+minEntryTimeWindow;this._flameChartDelegate.requestWindowTimes(timeRight+delta,timeRight+delta);}},setWindowTimes:function(startTime,endTime)
{if(this._muteAnimation||this._timeWindowLeft===0||this._timeWindowRight===Infinity||(startTime===0&&endTime===Infinity)||(startTime===Infinity&&endTime===Infinity)){this._timeWindowLeft=startTime;this._timeWindowRight=endTime;this.scheduleUpdate();return;}
this._cancelAnimation();this._cancelWindowTimesAnimation=WebInspector.animateFunction(this.element.window(),this._animateWindowTimes.bind(this),[{from:this._timeWindowLeft,to:startTime},{from:this._timeWindowRight,to:endTime}],5,this._animationCompleted.bind(this));this._pendingAnimationTimeLeft=startTime;this._pendingAnimationTimeRight=endTime;},_animateWindowTimes:function(startTime,endTime)
{this._timeWindowLeft=startTime;this._timeWindowRight=endTime;this.update();},_animationCompleted:function()
{delete this._cancelWindowTimesAnimation;},_initMaxDragOffset:function(event)
{this._maxDragOffsetSquared=0;this._dragStartX=event.pageX;this._dragStartY=event.pageY;},_updateMaxDragOffset:function(event)
{var dx=event.pageX-this._dragStartX;var dy=event.pageY-this._dragStartY;var dragOffsetSquared=dx*dx+dy*dy;this._maxDragOffsetSquared=Math.max(this._maxDragOffsetSquared,dragOffsetSquared);},_maxDragOffset:function()
{return Math.sqrt(this._maxDragOffsetSquared);},_startCanvasDragging:function(event)
{if(event.shiftKey)
return false;if(!this._timelineData()||this._timeWindowRight===Infinity)
return false;this._isDragging=true;this._initMaxDragOffset(event);this._dragStartPointX=event.pageX;this._dragStartPointY=event.pageY;this._dragStartScrollTop=this._vScrollElement.scrollTop;this._dragStartWindowLeft=this._timeWindowLeft;this._dragStartWindowRight=this._timeWindowRight;this._canvas.style.cursor="";return true;},_canvasDragging:function(event)
{var pixelShift=this._dragStartPointX-event.pageX;this._dragStartPointX=event.pageX;this._muteAnimation=true;this._handlePanGesture(pixelShift*this._pixelToTime);this._muteAnimation=false;var pixelScroll=this._dragStartPointY-event.pageY;this._vScrollElement.scrollTop=this._dragStartScrollTop+pixelScroll;this._updateMaxDragOffset(event);},_endCanvasDragging:function()
{this._isDragging=false;},_startRangeSelection:function(event)
{if(!event.shiftKey)
return false;this._isDragging=true;this._initMaxDragOffset(event);this._selectionOffsetShiftX=event.offsetX-event.pageX;this._selectionOffsetShiftY=event.offsetY-event.pageY;this._selectionStartX=event.offsetX;var style=this._selectionOverlay.style;style.left=this._selectionStartX+"px";style.width="1px";this._selectedTimeSpanLabel.textContent="";this._selectionOverlay.classList.remove("hidden");return true;},_endRangeSelection:function()
{this._isDragging=false;this._flameChartDelegate.endRangeSelection();},_hideRangeSelection:function()
{this._selectionOverlay.classList.add("hidden");},_rangeSelectionDragging:function(event)
{this._updateMaxDragOffset(event);var x=Number.constrain(event.pageX+this._selectionOffsetShiftX,0,this._offsetWidth);var start=this._cursorTime(this._selectionStartX);var end=this._cursorTime(x);this._rangeSelectionStart=Math.min(start,end);this._rangeSelectionEnd=Math.max(start,end);this._updateRangeSelectionOverlay();this._flameChartDelegate.updateRangeSelection(this._rangeSelectionStart,this._rangeSelectionEnd);},_updateRangeSelectionOverlay:function()
{var margin=100;var left=Number.constrain(this._timeToPosition(this._rangeSelectionStart),-margin,this._offsetWidth+margin);var right=Number.constrain(this._timeToPosition(this._rangeSelectionEnd),-margin,this._offsetWidth+margin);var style=this._selectionOverlay.style;style.left=left+"px";style.width=(right-left)+"px";var timeSpan=this._rangeSelectionEnd-this._rangeSelectionStart;this._selectedTimeSpanLabel.textContent=Number.preciseMillisToString(timeSpan,2);},_onMouseMove:function(event)
{this._lastMouseOffsetX=event.offsetX;if(!this._enabled())
return;if(this._isDragging)
return;var inDividersBar=event.offsetY<WebInspector.FlameChart.DividersBarHeight;this._highlightedMarkerIndex=inDividersBar?this._markerIndexAtPosition(event.offsetX):-1;this._updateMarkerHighlight();this._highlightEntry(this._coordinatesToEntryIndex(event.offsetX,event.offsetY));},_onMouseOut:function()
{this._highlightEntry(-1);},_highlightEntry:function(entryIndex)
{if(this._highlightedEntryIndex===entryIndex)
return;if(entryIndex===-1||!this._dataProvider.canJumpToEntry(entryIndex))
this._canvas.style.cursor="default";else
this._canvas.style.cursor="pointer";this._highlightedEntryIndex=entryIndex;this._updateElementPosition(this._highlightElement,this._highlightedEntryIndex);this._entryInfo.removeChildren();if(this._highlightedEntryIndex===-1)
return;if(!this._isDragging){var entryInfo=this._dataProvider.prepareHighlightedEntryInfo(this._highlightedEntryIndex);if(entryInfo)
this._entryInfo.appendChild(this._buildEntryInfo(entryInfo));}},_onClick:function()
{this.focus();const clickThreshold=5;if(this._maxDragOffset()>clickThreshold)
return;this._hideRangeSelection();this.dispatchEventToListeners(WebInspector.FlameChart.Events.EntrySelected,this._highlightedEntryIndex);},_onMouseWheel:function(e)
{if(!this._enabled())
return;var panVertically=e.shiftKey&&(e.wheelDeltaY||Math.abs(e.wheelDeltaX)===120);var panHorizontally=Math.abs(e.wheelDeltaX)>Math.abs(e.wheelDeltaY)&&!e.shiftKey;if(panVertically){this._vScrollElement.scrollTop-=(e.wheelDeltaY||e.wheelDeltaX)/120*this._offsetHeight/8;}else if(panHorizontally){var shift=-e.wheelDeltaX*this._pixelToTime;this._muteAnimation=true;this._handlePanGesture(shift);this._muteAnimation=false;}else{const mouseWheelZoomSpeed=1/120;this._handleZoomGesture(Math.pow(1.2,-(e.wheelDeltaY||e.wheelDeltaX)*mouseWheelZoomSpeed)-1);}
e.consume(true);},_onKeyDown:function(e)
{this._handleZoomPanKeys(e);this._handleSelectionNavigation(e);},_handleSelectionNavigation:function(e)
{if(!WebInspector.KeyboardShortcut.hasNoModifiers(e))
return;if(this._selectedEntryIndex===-1)
return;var timelineData=this._timelineData();if(!timelineData)
return;function timeComparator(time,entryIndex)
{return time-timelineData.entryStartTimes[entryIndex];}
function entriesIntersect(entry1,entry2)
{var start1=timelineData.entryStartTimes[entry1];var start2=timelineData.entryStartTimes[entry2];var end1=start1+timelineData.entryTotalTimes[entry1];var end2=start2+timelineData.entryTotalTimes[entry2];return start1<end2&&start2<end1;}
var keys=WebInspector.KeyboardShortcut.Keys;if(e.keyCode===keys.Left.code||e.keyCode===keys.Right.code){var level=timelineData.entryLevels[this._selectedEntryIndex];var levelIndexes=this._timelineLevels[level];var indexOnLevel=levelIndexes.lowerBound(this._selectedEntryIndex);indexOnLevel+=e.keyCode===keys.Left.code?-1:1;e.consume(true);if(indexOnLevel>=0&&indexOnLevel<levelIndexes.length)
this.dispatchEventToListeners(WebInspector.FlameChart.Events.EntrySelected,levelIndexes[indexOnLevel]);return;}
if(e.keyCode===keys.Up.code||e.keyCode===keys.Down.code){var level=timelineData.entryLevels[this._selectedEntryIndex];var delta=e.keyCode===keys.Up.code?1:-1;e.consume(true);if(this._isTopDown)
delta=-delta;level+=delta;if(level<0||level>=this._timelineLevels.length)
return;var entryTime=timelineData.entryStartTimes[this._selectedEntryIndex]+timelineData.entryTotalTimes[this._selectedEntryIndex]/2;var levelIndexes=this._timelineLevels[level];var indexOnLevel=levelIndexes.upperBound(entryTime,timeComparator)-1;if(!entriesIntersect(this._selectedEntryIndex,levelIndexes[indexOnLevel])){++indexOnLevel;if(indexOnLevel>=levelIndexes.length||!entriesIntersect(this._selectedEntryIndex,levelIndexes[indexOnLevel]))
return;}
this.dispatchEventToListeners(WebInspector.FlameChart.Events.EntrySelected,levelIndexes[indexOnLevel]);}},_handleZoomPanKeys:function(e)
{if(!WebInspector.KeyboardShortcut.hasNoModifiers(e))
return;var zoomMultiplier=e.shiftKey?0.8:0.3;var panMultiplier=e.shiftKey?320:80;if(e.keyCode==="A".charCodeAt(0)){this._handlePanGesture(-panMultiplier*this._pixelToTime);e.consume(true);}else if(e.keyCode==="D".charCodeAt(0)){this._handlePanGesture(panMultiplier*this._pixelToTime);e.consume(true);}else if(e.keyCode==="W".charCodeAt(0)){this._handleZoomGesture(-zoomMultiplier);e.consume(true);}else if(e.keyCode==="S".charCodeAt(0)){this._handleZoomGesture(zoomMultiplier);e.consume(true);}},_handleZoomGesture:function(zoom)
{this._cancelAnimation();var bounds=this._windowForGesture();var cursorTime=this._cursorTime(this._lastMouseOffsetX);bounds.left+=(bounds.left-cursorTime)*zoom;bounds.right+=(bounds.right-cursorTime)*zoom;this._requestWindowTimes(bounds);},_handlePanGesture:function(shift)
{this._cancelAnimation();var bounds=this._windowForGesture();shift=Number.constrain(shift,this._minimumBoundary-bounds.left,this._totalTime+this._minimumBoundary-bounds.right);bounds.left+=shift;bounds.right+=shift;this._requestWindowTimes(bounds);},_windowForGesture:function()
{var windowLeft=this._timeWindowLeft?this._timeWindowLeft:this._dataProvider.minimumBoundary();var windowRight=this._timeWindowRight!==Infinity?this._timeWindowRight:this._dataProvider.minimumBoundary()+this._dataProvider.totalTime();return{left:windowLeft,right:windowRight};},_requestWindowTimes:function(bounds)
{bounds.left=Number.constrain(bounds.left,this._minimumBoundary,this._totalTime+this._minimumBoundary);bounds.right=Number.constrain(bounds.right,this._minimumBoundary,this._totalTime+this._minimumBoundary);if(bounds.right-bounds.left<WebInspector.FlameChart.MinimalTimeWindowMs)
return;this._flameChartDelegate.requestWindowTimes(bounds.left,bounds.right);},_cursorTime:function(x)
{return(x+this._pixelWindowLeft-this._paddingLeft)*this._pixelToTime+this._minimumBoundary;},_coordinatesToEntryIndex:function(x,y)
{y+=this._scrollTop;var timelineData=this._timelineData();if(!timelineData)
return-1;var cursorTime=this._cursorTime(x);var cursorLevel;var offsetFromLevel;if(this._isTopDown){cursorLevel=Math.floor((y-WebInspector.FlameChart.DividersBarHeight)/this._barHeight);offsetFromLevel=y-WebInspector.FlameChart.DividersBarHeight-cursorLevel*this._barHeight;}else{cursorLevel=Math.floor((this._canvas.height/window.devicePixelRatio-y)/this._barHeight);offsetFromLevel=this._canvas.height/window.devicePixelRatio-cursorLevel*this._barHeight;}
var entryStartTimes=timelineData.entryStartTimes;var entryTotalTimes=timelineData.entryTotalTimes;var entryIndexes=this._timelineLevels[cursorLevel];if(!entryIndexes||!entryIndexes.length)
return-1;function comparator(time,entryIndex)
{return time-entryStartTimes[entryIndex];}
var indexOnLevel=Math.max(entryIndexes.upperBound(cursorTime,comparator)-1,0);function checkEntryHit(entryIndex)
{if(entryIndex===undefined)
return false;var startTime=entryStartTimes[entryIndex];var duration=entryTotalTimes[entryIndex];if(isNaN(duration)){var dx=(startTime-cursorTime)/this._pixelToTime;var dy=this._barHeight/2-offsetFromLevel;return dx*dx+dy*dy<this._markerRadius*this._markerRadius;}
var endTime=startTime+duration;var barThreshold=3*this._pixelToTime;return startTime-barThreshold<cursorTime&&cursorTime<endTime+barThreshold;}
var entryIndex=entryIndexes[indexOnLevel];if(checkEntryHit.call(this,entryIndex))
return entryIndex;entryIndex=entryIndexes[indexOnLevel+1];if(checkEntryHit.call(this,entryIndex))
return entryIndex;return-1;},_markerIndexAtPosition:function(x)
{var markers=this._timelineData().markers;if(!markers)
return-1;var accurracyOffsetPx=1;var time=this._cursorTime(x);var leftTime=this._cursorTime(x-accurracyOffsetPx);var rightTime=this._cursorTime(x+accurracyOffsetPx);var left=this._markerIndexBeforeTime(leftTime);var markerIndex=-1;var distance=Infinity;for(var i=left;i<markers.length&&markers[i].startTime()<rightTime;i++){var nextDistance=Math.abs(markers[i].startTime()-time);if(nextDistance<distance){markerIndex=i;distance=nextDistance;}}
return markerIndex;},_markerIndexBeforeTime:function(time)
{function comparator(markerTimestamp,marker)
{return markerTimestamp-marker.startTime();}
return this._timelineData().markers.lowerBound(time,comparator);},_draw:function(width,height)
{var timelineData=this._timelineData();if(!timelineData)
return;var context=this._canvas.getContext("2d");context.save();var ratio=window.devicePixelRatio;context.scale(ratio,ratio);var timeWindowRight=this._timeWindowRight;var timeWindowLeft=this._timeWindowLeft-this._paddingLeftTime;var entryTotalTimes=timelineData.entryTotalTimes;var entryStartTimes=timelineData.entryStartTimes;var entryLevels=timelineData.entryLevels;var titleIndices=new Uint32Array(entryTotalTimes.length);var nextTitleIndex=0;var markerIndices=new Uint32Array(entryTotalTimes.length);var nextMarkerIndex=0;var textPadding=this._dataProvider.textPadding();this._minTextWidth=2*textPadding+this._measureWidth(context,"\u2026");var minTextWidth=this._minTextWidth;var unclippedWidth=width-(WebInspector.isMac()?0:this._vScrollElement.offsetWidth);var barHeight=this._barHeight;var textBaseHeight=this._baseHeight+barHeight-this._dataProvider.textBaseline();var colorBuckets={};var minVisibleBarLevel=Math.max(Math.floor((this._scrollTop-this._baseHeight)/barHeight),0);var maxVisibleBarLevel=Math.min(Math.floor((this._scrollTop-this._baseHeight+height)/barHeight),this._dataProvider.maxStackDepth());context.translate(0,-this._scrollTop);function comparator(time,entryIndex)
{return time-entryStartTimes[entryIndex];}
for(var level=minVisibleBarLevel;level<=maxVisibleBarLevel;++level){var levelIndexes=this._timelineLevels[level];var rightIndexOnLevel=levelIndexes.lowerBound(timeWindowRight,comparator)-1;var lastDrawOffset=Infinity;for(var entryIndexOnLevel=rightIndexOnLevel;entryIndexOnLevel>=0;--entryIndexOnLevel){var entryIndex=levelIndexes[entryIndexOnLevel];var entryStartTime=entryStartTimes[entryIndex];var entryOffsetRight=entryStartTime+(isNaN(entryTotalTimes[entryIndex])?0:entryTotalTimes[entryIndex]);if(entryOffsetRight<=timeWindowLeft)
break;var barX=this._timeToPositionClipped(entryStartTime);if(barX>=lastDrawOffset)
continue;lastDrawOffset=barX;var color=this._dataProvider.entryColor(entryIndex);var bucket=colorBuckets[color];if(!bucket){bucket=[];colorBuckets[color]=bucket;}
bucket.push(entryIndex);}}
var colors=Object.keys(colorBuckets);for(var c=0;c<colors.length;++c){var color=colors[c];context.fillStyle=color;context.strokeStyle=color;var indexes=colorBuckets[color];context.beginPath();for(var i=0;i<indexes.length;++i){var entryIndex=indexes[i];var entryStartTime=entryStartTimes[entryIndex];var barX=this._timeToPositionClipped(entryStartTime);var barRight=this._timeToPositionClipped(entryStartTime+entryTotalTimes[entryIndex]);var barWidth=Math.max(barRight-barX,1);var barLevel=entryLevels[entryIndex];var barY=this._levelToHeight(barLevel);if(isNaN(entryTotalTimes[entryIndex])){context.moveTo(barX+this._markerRadius,barY+barHeight/2);context.arc(barX,barY+barHeight/2,this._markerRadius,0,Math.PI*2);markerIndices[nextMarkerIndex++]=entryIndex;}else{context.rect(barX,barY,barWidth-0.4,barHeight-1);if(barWidth>minTextWidth||this._dataProvider.forceDecoration(entryIndex))
titleIndices[nextTitleIndex++]=entryIndex;}}
context.fill();}
context.strokeStyle="rgb(0, 0, 0)";context.beginPath();for(var m=0;m<nextMarkerIndex;++m){var entryIndex=markerIndices[m];var entryStartTime=entryStartTimes[entryIndex];var barX=this._timeToPositionClipped(entryStartTime);var barLevel=entryLevels[entryIndex];var barY=this._levelToHeight(barLevel);context.moveTo(barX+this._markerRadius,barY+barHeight/2);context.arc(barX,barY+barHeight/2,this._markerRadius,0,Math.PI*2);}
context.stroke();context.textBaseline="alphabetic";for(var i=0;i<nextTitleIndex;++i){var entryIndex=titleIndices[i];var entryStartTime=entryStartTimes[entryIndex];var barX=this._timeToPositionClipped(entryStartTime);var barRight=Math.min(this._timeToPositionClipped(entryStartTime+entryTotalTimes[entryIndex]),unclippedWidth)+1;var barWidth=barRight-barX;var barLevel=entryLevels[entryIndex];var barY=this._levelToHeight(barLevel);var text=this._dataProvider.entryTitle(entryIndex);if(text&&text.length){context.font=this._dataProvider.entryFont(entryIndex);text=this._prepareText(context,text,barWidth-2*textPadding);}
if(this._dataProvider.decorateEntry(entryIndex,context,text,barX,barY,barWidth,barHeight))
continue;if(!text||!text.length)
continue;context.fillStyle=this._dataProvider.textColor(entryIndex);context.fillText(text,barX+textPadding,textBaseHeight-barLevel*this._barHeightDelta);}
this._drawFlowEvents(context,width,height);context.restore();var offsets=this._dataProvider.dividerOffsets(this._calculator.minimumBoundary(),this._calculator.maximumBoundary());WebInspector.TimelineGrid.drawCanvasGrid(this._canvas,this._calculator,offsets);this._drawMarkers();this._updateElementPosition(this._highlightElement,this._highlightedEntryIndex);this._updateElementPosition(this._selectedElement,this._selectedEntryIndex);this._updateMarkerHighlight();this._updateRangeSelectionOverlay();},_drawFlowEvents:function(context,width,height)
{var timelineData=this._timelineData();var timeWindowRight=this._timeWindowRight;var timeWindowLeft=this._timeWindowLeft;var flowStartTimes=timelineData.flowStartTimes;var flowEndTimes=timelineData.flowEndTimes;var flowStartLevels=timelineData.flowStartLevels;var flowEndLevels=timelineData.flowEndLevels;var flowCount=flowStartTimes.length;var endIndex=flowStartTimes.lowerBound(timeWindowRight);var color=[];var fadeColorsCount=8;for(var i=0;i<=fadeColorsCount;++i)
color[i]="rgba(128, 0, 0, "+i/fadeColorsCount+")";var fadeColorsRange=color.length;var minimumFlowDistancePx=15;var flowArcHeight=4*this._barHeight;var colorIndex=0;context.lineWidth=0.5;for(var i=0;i<endIndex;++i){if(flowEndTimes[i]<timeWindowLeft)
continue;var startX=this._timeToPosition(flowStartTimes[i]);var endX=this._timeToPosition(flowEndTimes[i]);if(endX-startX<minimumFlowDistancePx)
continue;if(startX<-minimumFlowDistancePx&&endX>width+minimumFlowDistancePx)
continue;if(endX-startX<minimumFlowDistancePx+fadeColorsRange||colorIndex!==color.length-1){colorIndex=Math.min(fadeColorsRange-1,Math.floor(endX-startX-minimumFlowDistancePx));context.strokeStyle=color[colorIndex];}
var startY=this._levelToHeight(flowStartLevels[i])+this._barHeight;var endY=this._levelToHeight(flowEndLevels[i]);context.beginPath();context.moveTo(startX,startY);var arcHeight=Math.max(Math.sqrt(Math.abs(startY-endY)),flowArcHeight)+5;context.bezierCurveTo(startX,startY+arcHeight,endX,endY+arcHeight,endX,endY+this._barHeight);context.stroke();}},_drawMarkers:function()
{var markers=this._timelineData().markers;var left=this._markerIndexBeforeTime(this._calculator.minimumBoundary());var rightBoundary=this._calculator.maximumBoundary();var context=this._canvas.getContext("2d");context.save();var ratio=window.devicePixelRatio;context.scale(ratio,ratio);var height=WebInspector.FlameChart.DividersBarHeight-1;for(var i=left;i<markers.length;i++){var timestamp=markers[i].startTime();if(timestamp>rightBoundary)
break;markers[i].draw(context,this._calculator.computePosition(timestamp),height,this._timeToPixel);}
context.restore();},_updateMarkerHighlight:function()
{var element=this._markerHighlighElement;if(element.parentElement)
element.remove();var markerIndex=this._highlightedMarkerIndex;if(markerIndex===-1)
return;var marker=this._timelineData().markers[markerIndex];var barX=this._timeToPositionClipped(marker.startTime());element.title=marker.title();var style=element.style;style.left=barX+"px";style.backgroundColor=marker.color();this.contentElement.appendChild(element);},_processTimelineData:function(timelineData)
{if(!timelineData){this._timelineLevels=null;this._rawTimelineData=null;this._rawTimelineDataLength=0;return;}
var entryCounters=new Uint32Array(this._dataProvider.maxStackDepth()+1);for(var i=0;i<timelineData.entryLevels.length;++i)
++entryCounters[timelineData.entryLevels[i]];var levelIndexes=new Array(entryCounters.length);for(var i=0;i<levelIndexes.length;++i){levelIndexes[i]=new Uint32Array(entryCounters[i]);entryCounters[i]=0;}
for(var i=0;i<timelineData.entryLevels.length;++i){var level=timelineData.entryLevels[i];levelIndexes[level][entryCounters[level]++]=i;}
this._timelineLevels=levelIndexes;this._rawTimelineData=timelineData;this._rawTimelineDataLength=timelineData.entryStartTimes.length;},setSelectedEntry:function(entryIndex)
{if(entryIndex===-1&&!this._isDragging)
this._hideRangeSelection();if(this._selectedEntryIndex===entryIndex)
return;this._selectedEntryIndex=entryIndex;this._revealEntry(entryIndex);this._updateElementPosition(this._selectedElement,this._selectedEntryIndex);},_updateElementPosition:function(element,entryIndex)
{var elementMinWidth=2;if(element.parentElement)
element.remove();if(entryIndex===-1)
return;var timeRange=this._dataProvider.highlightTimeRange(entryIndex);if(!timeRange)
return;var timelineData=this._timelineData();var barX=this._timeToPositionClipped(timeRange.startTime);var barRight=this._timeToPositionClipped(timeRange.endTime);if(barRight===0||barX===this._canvas.width)
return;var barWidth=barRight-barX;var barCenter=barX+barWidth/2;barWidth=Math.max(barWidth,elementMinWidth);barX=barCenter-barWidth/2;var barY=this._levelToHeight(timelineData.entryLevels[entryIndex])-this._scrollTop;var style=element.style;style.left=barX+"px";style.top=barY+"px";style.width=barWidth+"px";style.height=this._barHeight-1+"px";this.contentElement.appendChild(element);},_timeToPositionClipped:function(time)
{return Number.constrain(this._timeToPosition(time),0,this._canvas.width);},_timeToPosition:function(time)
{return Math.floor((time-this._minimumBoundary)*this._timeToPixel)-this._pixelWindowLeft+this._paddingLeft;},_levelToHeight:function(level)
{return this._baseHeight-level*this._barHeightDelta;},_buildEntryInfo:function(entryInfo)
{var infoTable=createElementWithClass("table","info-table");for(var entry of entryInfo){var row=infoTable.createChild("tr");row.createChild("td","title").textContent=entry.title;row.createChild("td").textContent=typeof entry.value==="string"?entry.value:entry.value.textContent;}
return infoTable;},_prepareText:function(context,title,maxSize)
{var titleWidth=this._measureWidth(context,title);if(maxSize>=titleWidth)
return title;var l=2;var r=title.length;while(l<r){var m=(l+r)>>1;if(this._measureWidth(context,title.trimMiddle(m))<=maxSize)
l=m+1;else
r=m;}
title=title.trimMiddle(r-1);return title!=="\u2026"?title:"";},_measureWidth:function(context,text)
{if(text.length>20)
return context.measureText(text).width;var font=context.font;var textWidths=this._textWidth[font];if(!textWidths){textWidths={};this._textWidth[font]=textWidths;}
var width=textWidths[text];if(!width){width=context.measureText(text).width;textWidths[text]=width;}
return width;},_updateBoundaries:function()
{this._totalTime=this._dataProvider.totalTime();this._minimumBoundary=this._dataProvider.minimumBoundary();if(this._timeWindowRight!==Infinity){this._windowLeft=(this._timeWindowLeft-this._minimumBoundary)/this._totalTime;this._windowRight=(this._timeWindowRight-this._minimumBoundary)/this._totalTime;this._windowWidth=this._windowRight-this._windowLeft;}else if(this._timeWindowLeft===Infinity){this._windowLeft=Infinity;this._windowRight=Infinity;this._windowWidth=1;}else{this._windowLeft=0;this._windowRight=1;this._windowWidth=1;}
this._pixelWindowWidth=this._offsetWidth-this._paddingLeft;this._totalPixels=Math.floor(this._pixelWindowWidth/this._windowWidth);this._pixelWindowLeft=Math.floor(this._totalPixels*this._windowLeft);this._timeToPixel=this._totalPixels/this._totalTime;this._pixelToTime=this._totalTime/this._totalPixels;this._paddingLeftTime=this._paddingLeft/this._timeToPixel;this._baseHeight=this._isTopDown?WebInspector.FlameChart.DividersBarHeight:this._offsetHeight-this._barHeight;this._totalHeight=this._levelToHeight(this._dataProvider.maxStackDepth());this._vScrollContent.style.height=this._totalHeight+"px";this._updateScrollBar();},onResize:function()
{this._updateScrollBar();this._updateContentElementSize();this.scheduleUpdate();},_updateScrollBar:function()
{var showScroll=this._totalHeight>this._offsetHeight;if(this._vScrollElement.classList.contains("hidden")===showScroll){this._vScrollElement.classList.toggle("hidden",!showScroll);this._updateContentElementSize();}},_updateContentElementSize:function()
{this._offsetWidth=this.contentElement.offsetWidth;this._offsetHeight=this.contentElement.offsetHeight;},_onScroll:function()
{this._scrollTop=this._vScrollElement.scrollTop;this.scheduleUpdate();},scheduleUpdate:function()
{if(this._updateTimerId||this._cancelWindowTimesAnimation)
return;this._updateTimerId=this.element.window().requestAnimationFrame(this.update.bind(this));},update:function()
{this._updateTimerId=0;if(!this._timelineData())
return;this._resetCanvas();this._updateBoundaries();this._calculator._updateBoundaries(this);this._draw(this._offsetWidth,this._offsetHeight);},reset:function()
{this._vScrollElement.scrollTop=0;this._highlightedMarkerIndex=-1;this._highlightedEntryIndex=-1;this._selectedEntryIndex=-1;this._textWidth={};this.update();},_enabled:function()
{return this._rawTimelineDataLength!==0;},__proto__:WebInspector.HBox.prototype};WebInspector.OverviewGrid=function(prefix)
{this.element=createElement("div");this.element.id=prefix+"-overview-container";this._grid=new WebInspector.TimelineGrid();this._grid.element.id=prefix+"-overview-grid";this._grid.setScrollTop(0);this.element.appendChild(this._grid.element);this._window=new WebInspector.OverviewGrid.Window(this.element,this._grid.dividersLabelBarElement);this._window.addEventListener(WebInspector.OverviewGrid.Events.WindowChanged,this._onWindowChanged,this);}
WebInspector.OverviewGrid.prototype={clientWidth:function()
{return this.element.clientWidth;},updateDividers:function(calculator)
{this._grid.updateDividers(calculator);},addEventDividers:function(dividers)
{this._grid.addEventDividers(dividers);},removeEventDividers:function()
{this._grid.removeEventDividers();},reset:function()
{this._window.reset();},windowLeft:function()
{return this._window.windowLeft;},windowRight:function()
{return this._window.windowRight;},setWindow:function(left,right)
{this._window._setWindow(left,right);},addEventListener:function(eventType,listener,thisObject)
{return this._window.addEventListener(eventType,listener,thisObject);},zoom:function(zoomFactor,referencePoint)
{this._window._zoom(zoomFactor,referencePoint);},setResizeEnabled:function(enabled)
{this._window.setEnabled(enabled);},_onWindowChanged:function()
{this._grid.showCurtains(this.windowLeft(),this.windowRight());}}
WebInspector.OverviewGrid.MinSelectableSize=14;WebInspector.OverviewGrid.WindowScrollSpeedFactor=.3;WebInspector.OverviewGrid.ResizerOffset=3.5;WebInspector.OverviewGrid.Window=function(parentElement,dividersLabelBarElement)
{this._parentElement=parentElement;WebInspector.installDragHandle(this._parentElement,this._startWindowSelectorDragging.bind(this),this._windowSelectorDragging.bind(this),this._endWindowSelectorDragging.bind(this),"text",null);if(dividersLabelBarElement)
WebInspector.installDragHandle(dividersLabelBarElement,this._startWindowDragging.bind(this),this._windowDragging.bind(this),null,"-webkit-grabbing","-webkit-grab");this.windowLeft=0.0;this.windowRight=1.0;this._parentElement.addEventListener("mousewheel",this._onMouseWheel.bind(this),true);this._parentElement.addEventListener("dblclick",this._resizeWindowMaximum.bind(this),true);this._parentElement.appendChild(WebInspector.Widget.createStyleElement("ui_lazy/overviewGrid.css"));this._leftResizeElement=parentElement.createChild("div","overview-grid-window-resizer");this._leftResizeElement.style.left=0;WebInspector.installDragHandle(this._leftResizeElement,this._resizerElementStartDragging.bind(this),this._leftResizeElementDragging.bind(this),null,"ew-resize");this._rightResizeElement=parentElement.createChild("div","overview-grid-window-resizer");this._rightResizeElement.style.right=0;WebInspector.installDragHandle(this._rightResizeElement,this._resizerElementStartDragging.bind(this),this._rightResizeElementDragging.bind(this),null,"ew-resize");this.setEnabled(true);}
WebInspector.OverviewGrid.Events={WindowChanged:"WindowChanged",Click:"Click"}
WebInspector.OverviewGrid.Window.prototype={reset:function()
{this.windowLeft=0.0;this.windowRight=1.0;this._leftResizeElement.style.left="0%";this._rightResizeElement.style.left="100%";this.setEnabled(true);},setEnabled:function(enabled)
{this._enabled=enabled;},_resizerElementStartDragging:function(event)
{if(!this._enabled)
return false;this._resizerParentOffsetLeft=event.pageX-event.offsetX-event.target.offsetLeft;event.preventDefault();return true;},_leftResizeElementDragging:function(event)
{this._resizeWindowLeft(event.pageX-this._resizerParentOffsetLeft);event.preventDefault();},_rightResizeElementDragging:function(event)
{this._resizeWindowRight(event.pageX-this._resizerParentOffsetLeft);event.preventDefault();},_startWindowSelectorDragging:function(event)
{if(!this._enabled)
return false;this._offsetLeft=this._parentElement.totalOffsetLeft();var position=event.x-this._offsetLeft;this._overviewWindowSelector=new WebInspector.OverviewGrid.WindowSelector(this._parentElement,position);return true;},_windowSelectorDragging:function(event)
{this._overviewWindowSelector._updatePosition(event.x-this._offsetLeft);event.preventDefault();},_endWindowSelectorDragging:function(event)
{var window=this._overviewWindowSelector._close(event.x-this._offsetLeft);delete this._overviewWindowSelector;var clickThreshold=3;if(window.end-window.start<clickThreshold){if(this.dispatchEventToListeners(WebInspector.OverviewGrid.Events.Click,event))
return;var middle=window.end;window.start=Math.max(0,middle-WebInspector.OverviewGrid.MinSelectableSize/2);window.end=Math.min(this._parentElement.clientWidth,middle+WebInspector.OverviewGrid.MinSelectableSize/2);}else if(window.end-window.start<WebInspector.OverviewGrid.MinSelectableSize){if(this._parentElement.clientWidth-window.end>WebInspector.OverviewGrid.MinSelectableSize)
window.end=window.start+WebInspector.OverviewGrid.MinSelectableSize;else
window.start=window.end-WebInspector.OverviewGrid.MinSelectableSize;}
this._setWindowPosition(window.start,window.end);},_startWindowDragging:function(event)
{this._dragStartPoint=event.pageX;this._dragStartLeft=this.windowLeft;this._dragStartRight=this.windowRight;return true;},_windowDragging:function(event)
{event.preventDefault();var delta=(event.pageX-this._dragStartPoint)/this._parentElement.clientWidth;if(this._dragStartLeft+delta<0)
delta=-this._dragStartLeft;if(this._dragStartRight+delta>1)
delta=1-this._dragStartRight;this._setWindow(this._dragStartLeft+delta,this._dragStartRight+delta);},_resizeWindowLeft:function(start)
{if(start<10)
start=0;else if(start>this._rightResizeElement.offsetLeft-4)
start=this._rightResizeElement.offsetLeft-4;this._setWindowPosition(start,null);},_resizeWindowRight:function(end)
{if(end>this._parentElement.clientWidth-10)
end=this._parentElement.clientWidth;else if(end<this._leftResizeElement.offsetLeft+WebInspector.OverviewGrid.MinSelectableSize)
end=this._leftResizeElement.offsetLeft+WebInspector.OverviewGrid.MinSelectableSize;this._setWindowPosition(null,end);},_resizeWindowMaximum:function()
{this._setWindowPosition(0,this._parentElement.clientWidth);},_setWindow:function(windowLeft,windowRight)
{var left=windowLeft;var right=windowRight;var width=windowRight-windowLeft;var widthInPixels=width*this._parentElement.clientWidth;var minWidthInPixels=WebInspector.OverviewGrid.MinSelectableSize/2;if(widthInPixels<minWidthInPixels){var factor=minWidthInPixels/widthInPixels;left=((windowRight+windowLeft)-width*factor)/2;right=((windowRight+windowLeft)+width*factor)/2;}
this.windowLeft=windowLeft;this._leftResizeElement.style.left=left*100+"%";this.windowRight=windowRight;this._rightResizeElement.style.left=right*100+"%";this.dispatchEventToListeners(WebInspector.OverviewGrid.Events.WindowChanged);},_setWindowPosition:function(start,end)
{var clientWidth=this._parentElement.clientWidth;var windowLeft=typeof start==="number"?start/clientWidth:this.windowLeft;var windowRight=typeof end==="number"?end/clientWidth:this.windowRight;this._setWindow(windowLeft,windowRight);},_onMouseWheel:function(event)
{if(!this._enabled)
return;if(typeof event.wheelDeltaY==="number"&&event.wheelDeltaY){const zoomFactor=1.1;const mouseWheelZoomSpeed=1/120;var reference=event.offsetX/event.target.clientWidth;this._zoom(Math.pow(zoomFactor,-event.wheelDeltaY*mouseWheelZoomSpeed),reference);}
if(typeof event.wheelDeltaX==="number"&&event.wheelDeltaX){var offset=Math.round(event.wheelDeltaX*WebInspector.OverviewGrid.WindowScrollSpeedFactor);var windowLeft=this._leftResizeElement.offsetLeft+WebInspector.OverviewGrid.ResizerOffset;var windowRight=this._rightResizeElement.offsetLeft+WebInspector.OverviewGrid.ResizerOffset;if(windowLeft-offset<0)
offset=windowLeft;if(windowRight-offset>this._parentElement.clientWidth)
offset=windowRight-this._parentElement.clientWidth;this._setWindowPosition(windowLeft-offset,windowRight-offset);event.preventDefault();}},_zoom:function(factor,reference)
{var left=this.windowLeft;var right=this.windowRight;var windowSize=right-left;var newWindowSize=factor*windowSize;if(newWindowSize>1){newWindowSize=1;factor=newWindowSize/windowSize;}
left=reference+(left-reference)*factor;left=Number.constrain(left,0,1-newWindowSize);right=reference+(right-reference)*factor;right=Number.constrain(right,newWindowSize,1);this._setWindow(left,right);},__proto__:WebInspector.Object.prototype}
WebInspector.OverviewGrid.WindowSelector=function(parent,position)
{this._startPosition=position;this._width=parent.offsetWidth;this._windowSelector=createElement("div");this._windowSelector.className="overview-grid-window-selector";this._windowSelector.style.left=this._startPosition+"px";this._windowSelector.style.right=this._width-this._startPosition+"px";parent.appendChild(this._windowSelector);}
WebInspector.OverviewGrid.WindowSelector.prototype={_close:function(position)
{position=Math.max(0,Math.min(position,this._width));this._windowSelector.remove();return this._startPosition<position?{start:this._startPosition,end:position}:{start:position,end:this._startPosition};},_updatePosition:function(position)
{position=Math.max(0,Math.min(position,this._width));if(position<this._startPosition){this._windowSelector.style.left=position+"px";this._windowSelector.style.right=this._width-this._startPosition+"px";}else{this._windowSelector.style.left=this._startPosition+"px";this._windowSelector.style.right=this._width-position+"px";}}};WebInspector.PieChart=function(size,formatter,showTotal)
{var shadowSize=WebInspector.PieChart._ShadowSizePercent;this.element=createElement("div");this._shadowRoot=WebInspector.createShadowRootWithCoreStyles(this.element);this._shadowRoot.appendChild(WebInspector.Widget.createStyleElement("ui_lazy/pieChart.css"));var root=this._shadowRoot.createChild("div","root");var svg=this._createSVGChild(root,"svg");svg.setAttribute("width",(100*(1+2*shadowSize))+"%");svg.setAttribute("height",(100*(1+2*shadowSize))+"%");this._group=this._createSVGChild(svg,"g");var shadow=this._createSVGChild(this._group,"circle");shadow.setAttribute("r",1+shadowSize);shadow.setAttribute("cy",shadowSize);shadow.setAttribute("fill","hsl(0,0%,70%)");var background=this._createSVGChild(this._group,"circle");background.setAttribute("r",1);background.setAttribute("fill","hsl(0,0%,92%)");this._foregroundElement=root.createChild("div","pie-chart-foreground");if(showTotal)
this._totalElement=this._foregroundElement.createChild("div","pie-chart-total");this._formatter=formatter;this._slices=[];this._lastAngle=-Math.PI/2;this._setSize(size);}
WebInspector.PieChart._ShadowSizePercent=0.02;WebInspector.PieChart.prototype={setTotal:function(totalValue)
{for(var i=0;i<this._slices.length;++i)
this._slices[i].remove();this._slices=[];this._totalValue=totalValue;var totalString;if(totalValue)
totalString=this._formatter?this._formatter(totalValue):totalValue;else
totalString="";if(this._totalElement)
this._totalElement.textContent=totalString;},_setSize:function(value)
{this._group.setAttribute("transform","scale("+(value/2)+") translate("+(1+WebInspector.PieChart._ShadowSizePercent)+",1)");var size=value+"px";this.element.style.width=size;this.element.style.height=size;},addSlice:function(value,color)
{var sliceAngle=value/this._totalValue*2*Math.PI;if(!isFinite(sliceAngle))
return;sliceAngle=Math.min(sliceAngle,2*Math.PI*0.9999);var path=this._createSVGChild(this._group,"path");var x1=Math.cos(this._lastAngle);var y1=Math.sin(this._lastAngle);this._lastAngle+=sliceAngle;var x2=Math.cos(this._lastAngle);var y2=Math.sin(this._lastAngle);var largeArc=sliceAngle>Math.PI?1:0;path.setAttribute("d","M0,0 L"+x1+","+y1+" A1,1,0,"+largeArc+",1,"+x2+","+y2+" Z");path.setAttribute("fill",color);this._slices.push(path);},_createSVGChild:function(parent,childType)
{var child=parent.ownerDocument.createElementNS("http://www.w3.org/2000/svg",childType);parent.appendChild(child);return child;}};WebInspector.TimelineGrid=function()
{this.element=createElement("div");this.element.appendChild(WebInspector.Widget.createStyleElement("ui_lazy/timelineGrid.css"));this._dividersElement=this.element.createChild("div","resources-dividers");this._gridHeaderElement=createElement("div");this._gridHeaderElement.classList.add("timeline-grid-header");this._eventDividersElement=this._gridHeaderElement.createChild("div","resources-event-dividers");this._dividersLabelBarElement=this._gridHeaderElement.createChild("div","resources-dividers-label-bar");this.element.appendChild(this._gridHeaderElement);this._leftCurtainElement=this.element.createChild("div","timeline-curtain-left");this._rightCurtainElement=this.element.createChild("div","timeline-curtain-right");}
WebInspector.TimelineGrid.calculateDividerOffsets=function(calculator,freeZoneAtLeft)
{var minGridSlicePx=64;var clientWidth=calculator.computePosition(calculator.maximumBoundary());var dividersCount=clientWidth/minGridSlicePx;var gridSliceTime=calculator.boundarySpan()/dividersCount;var pixelsPerTime=clientWidth/calculator.boundarySpan();var logGridSliceTime=Math.ceil(Math.log(gridSliceTime)/Math.LN10);gridSliceTime=Math.pow(10,logGridSliceTime);if(gridSliceTime*pixelsPerTime>=5*minGridSlicePx)
gridSliceTime=gridSliceTime/5;if(gridSliceTime*pixelsPerTime>=2*minGridSlicePx)
gridSliceTime=gridSliceTime/2;var leftBoundaryTime=calculator.minimumBoundary()-calculator.paddingLeft()/pixelsPerTime;var firstDividerTime=Math.ceil((leftBoundaryTime-calculator.zeroTime())/gridSliceTime)*gridSliceTime+calculator.zeroTime();var lastDividerTime=calculator.maximumBoundary();lastDividerTime+=minGridSlicePx/pixelsPerTime;dividersCount=Math.ceil((lastDividerTime-firstDividerTime)/gridSliceTime);if(!gridSliceTime)
dividersCount=0;var offsets=[];for(var i=0;i<dividersCount;++i){var time=firstDividerTime+gridSliceTime*i;if(calculator.computePosition(time)<freeZoneAtLeft)
continue;offsets.push(time);}
return{offsets:offsets,precision:Math.max(0,-Math.floor(Math.log(gridSliceTime*1.01)/Math.LN10))};}
WebInspector.TimelineGrid.drawCanvasGrid=function(canvas,calculator,dividerOffsets)
{var context=canvas.getContext("2d");context.save();var ratio=window.devicePixelRatio;context.scale(ratio,ratio);var printDeltas=!!dividerOffsets;var width=canvas.width/window.devicePixelRatio;var height=canvas.height/window.devicePixelRatio;var precision=0;if(!dividerOffsets){var dividersData=WebInspector.TimelineGrid.calculateDividerOffsets(calculator);dividerOffsets=dividersData.offsets;precision=dividersData.precision;}
context.fillStyle="rgba(255, 255, 255, 0.5)";context.fillRect(0,0,width,15);context.fillStyle="#333";context.strokeStyle="rgba(0, 0, 0, 0.1)";context.textBaseline="hanging";context.font=(printDeltas?"italic bold 11px ":" 11px ")+WebInspector.fontFamily();context.lineWidth=1;context.translate(0.5,0.5);const minWidthForTitle=60;var lastPosition=0;var time=0;var lastTime=0;var paddingRight=4;var paddingTop=3;for(var i=0;i<dividerOffsets.length;++i){time=dividerOffsets[i];var position=calculator.computePosition(time);context.beginPath();if(!printDeltas||i!==0&&position-lastPosition>minWidthForTitle){var text=printDeltas?calculator.formatTime(calculator.zeroTime()+time-lastTime):calculator.formatTime(time,precision);var textWidth=context.measureText(text).width;var textPosition=printDeltas?(position+lastPosition-textWidth)/2:position-textWidth-paddingRight;context.fillText(text,textPosition,paddingTop);}
context.moveTo(position,0);context.lineTo(position,height);context.stroke();lastTime=time;lastPosition=position;}
context.restore();}
WebInspector.TimelineGrid.prototype={get dividersElement()
{return this._dividersElement;},get dividersLabelBarElement()
{return this._dividersLabelBarElement;},removeDividers:function()
{this._dividersElement.removeChildren();this._dividersLabelBarElement.removeChildren();},updateDividers:function(calculator,freeZoneAtLeft)
{var dividersData=WebInspector.TimelineGrid.calculateDividerOffsets(calculator,freeZoneAtLeft);var dividerOffsets=dividersData.offsets;var precision=dividersData.precision;var dividersElementClientWidth=this._dividersElement.clientWidth;var divider=(this._dividersElement.firstChild);var dividerLabelBar=(this._dividersLabelBarElement.firstChild);for(var i=0;i<dividerOffsets.length;++i){if(!divider){divider=createElement("div");divider.className="resources-divider";this._dividersElement.appendChild(divider);dividerLabelBar=createElement("div");dividerLabelBar.className="resources-divider";var label=createElement("div");label.className="resources-divider-label";dividerLabelBar._labelElement=label;dividerLabelBar.appendChild(label);this._dividersLabelBarElement.appendChild(dividerLabelBar);}
var time=dividerOffsets[i];var position=calculator.computePosition(time);dividerLabelBar._labelElement.textContent=calculator.formatTime(time,precision);var percentLeft=100*position/dividersElementClientWidth;divider.style.left=percentLeft+"%";dividerLabelBar.style.left=percentLeft+"%";divider=(divider.nextSibling);dividerLabelBar=(dividerLabelBar.nextSibling);}
while(divider){var nextDivider=divider.nextSibling;this._dividersElement.removeChild(divider);divider=nextDivider;}
while(dividerLabelBar){var nextDivider=dividerLabelBar.nextSibling;this._dividersLabelBarElement.removeChild(dividerLabelBar);dividerLabelBar=nextDivider;}
return true;},addEventDivider:function(divider)
{this._eventDividersElement.appendChild(divider);},addEventDividers:function(dividers)
{this._gridHeaderElement.removeChild(this._eventDividersElement);for(var divider of dividers)
this._eventDividersElement.appendChild(divider);this._gridHeaderElement.appendChild(this._eventDividersElement);},removeEventDividers:function()
{this._eventDividersElement.removeChildren();},hideEventDividers:function()
{this._eventDividersElement.classList.add("hidden");},showEventDividers:function()
{this._eventDividersElement.classList.remove("hidden");},hideDividers:function()
{this._dividersElement.classList.add("hidden");},showDividers:function()
{this._dividersElement.classList.remove("hidden");},hideCurtains:function()
{this._leftCurtainElement.classList.add("hidden");this._rightCurtainElement.classList.add("hidden");},showCurtains:function(left,right)
{this._leftCurtainElement.style.width=(100*left).toFixed(2)+"%";this._leftCurtainElement.classList.remove("hidden");this._rightCurtainElement.style.width=(100*(1-right)).toFixed(2)+"%";this._rightCurtainElement.classList.remove("hidden");},setScrollTop:function(scrollTop)
{this._dividersLabelBarElement.style.top=scrollTop+"px";this._eventDividersElement.style.top=scrollTop+"px";this._leftCurtainElement.style.top=scrollTop+"px";this._rightCurtainElement.style.top=scrollTop+"px";}}
WebInspector.TimelineGrid.Calculator=function(){}
WebInspector.TimelineGrid.Calculator.prototype={paddingLeft:function(){},computePosition:function(time){},formatTime:function(time,precision){},minimumBoundary:function(){},zeroTime:function(){},maximumBoundary:function(){},boundarySpan:function(){}};WebInspector.TimelineOverviewPane=function(prefix)
{WebInspector.VBox.call(this);this.element.id=prefix+"-overview-pane";this._overviewCalculator=new WebInspector.TimelineOverviewCalculator();this._overviewGrid=new WebInspector.OverviewGrid(prefix);this.element.appendChild(this._overviewGrid.element);this._cursorArea=this._overviewGrid.element.createChild("div","overview-grid-cursor-area");this._cursorElement=this._overviewGrid.element.createChild("div","overview-grid-cursor-position");this._cursorArea.addEventListener("mousemove",this._onMouseMove.bind(this),true);this._cursorArea.addEventListener("mouseleave",this._hideCursor.bind(this),true);this._overviewGrid.setResizeEnabled(false);this._overviewGrid.addEventListener(WebInspector.OverviewGrid.Events.WindowChanged,this._onWindowChanged,this);this._overviewGrid.addEventListener(WebInspector.OverviewGrid.Events.Click,this._onClick,this);this._overviewControls=[];this._markers=new Map();this._popoverHelper=new WebInspector.PopoverHelper(this._cursorArea,this._getPopoverAnchor.bind(this),this._showPopover.bind(this),this._onHidePopover.bind(this));this._popoverHelper.setTimeout(0);this._updateThrottler=new WebInspector.Throttler(100);this._cursorEnabled=false;this._cursorPosition=0;this._lastWidth=0;}
WebInspector.TimelineOverviewPane.Events={WindowChanged:"WindowChanged"};WebInspector.TimelineOverviewPane.prototype={_getPopoverAnchor:function(element,event)
{return this._cursorArea;},_showPopover:function(anchor,popover)
{this._buildPopoverContents().then(maybeShowPopover.bind(this));function maybeShowPopover(fragment)
{if(!fragment.firstChild)
return;var content=new WebInspector.TimelineOverviewPane.PopoverContents();this._popoverContents=content.contentElement.createChild("div");this._popoverContents.appendChild(fragment);this._popover=popover;popover.showView(content,this._cursorElement);}},_onHidePopover:function()
{this._popover=null;this._popoverContents=null;},_onMouseMove:function(event)
{if(!this._cursorEnabled)
return;this._cursorPosition=event.offsetX+event.target.offsetLeft;this._cursorElement.style.left=this._cursorPosition+"px";this._cursorElement.style.visibility="visible";if(!this._popover)
return;this._buildPopoverContents().then(updatePopover.bind(this));this._popover.positionElement(this._cursorElement);function updatePopover(fragment)
{if(!this._popoverContents)
return;this._popoverContents.removeChildren();this._popoverContents.appendChild(fragment);}},_buildPopoverContents:function()
{var document=this.element.ownerDocument;var x=this._cursorPosition;var promises=this._overviewControls.map(mapToPopover);function mapToPopover(control)
{return control.popoverElementPromise(x)}
return Promise.all(promises).then(buildFragment);function buildFragment(elements)
{var fragment=document.createDocumentFragment();elements.remove(null);fragment.appendChildren.apply(fragment,elements);return fragment;}},_hideCursor:function()
{this._cursorElement.style.visibility="hidden";},wasShown:function()
{this._update();},onResize:function()
{var width=this.element.offsetWidth;if(width===this._lastWidth)
return;this._lastWidth=width;this.scheduleUpdate();},setOverviewControls:function(overviewControls)
{for(var i=0;i<this._overviewControls.length;++i)
this._overviewControls[i].dispose();for(var i=0;i<overviewControls.length;++i){overviewControls[i].setCalculator(this._overviewCalculator);overviewControls[i].show(this._overviewGrid.element);}
this._overviewControls=overviewControls;this._update();},setBounds:function(minimumBoundary,maximumBoundary)
{this._overviewCalculator.setBounds(minimumBoundary,maximumBoundary);this._overviewGrid.setResizeEnabled(true);this._cursorEnabled=true;},scheduleUpdate:function()
{this._updateThrottler.schedule(process.bind(this));function process()
{this._update();return Promise.resolve();}},_update:function()
{if(!this.isShowing())
return;this._overviewCalculator.setDisplayWindow(this._overviewGrid.clientWidth());for(var i=0;i<this._overviewControls.length;++i)
this._overviewControls[i].update();this._overviewGrid.updateDividers(this._overviewCalculator);this._updateMarkers();this._updateWindow();},setMarkers:function(markers)
{this._markers=markers;this._updateMarkers();},_updateMarkers:function()
{var filteredMarkers=new Map();for(var time of this._markers.keys()){var marker=this._markers.get(time);var position=Math.round(this._overviewCalculator.computePosition(time));if(filteredMarkers.has(position))
continue;filteredMarkers.set(position,marker);marker.style.left=position+"px";}
this._overviewGrid.removeEventDividers();this._overviewGrid.addEventDividers(filteredMarkers.valuesArray());},reset:function()
{this._windowStartTime=0;this._windowEndTime=Infinity;this._overviewCalculator.reset();this._overviewGrid.reset();this._overviewGrid.setResizeEnabled(false);this._overviewGrid.updateDividers(this._overviewCalculator);this._cursorEnabled=false;this._hideCursor();this._markers=new Map();for(var i=0;i<this._overviewControls.length;++i)
this._overviewControls[i].reset();this._popoverHelper.hidePopover();this._update();},_onClick:function(event)
{var domEvent=(event.data);for(var overviewControl of this._overviewControls){if(overviewControl.onClick(domEvent)){event.preventDefault();return;}}},_onWindowChanged:function(event)
{if(this._muteOnWindowChanged)
return;if(!this._overviewControls.length)
return;var windowTimes=this._overviewControls[0].windowTimes(this._overviewGrid.windowLeft(),this._overviewGrid.windowRight());this._windowStartTime=windowTimes.startTime;this._windowEndTime=windowTimes.endTime;this.dispatchEventToListeners(WebInspector.TimelineOverviewPane.Events.WindowChanged,windowTimes);},requestWindowTimes:function(startTime,endTime)
{if(startTime===this._windowStartTime&&endTime===this._windowEndTime)
return;this._windowStartTime=startTime;this._windowEndTime=endTime;this._updateWindow();this.dispatchEventToListeners(WebInspector.TimelineOverviewPane.Events.WindowChanged,{startTime:startTime,endTime:endTime});},_updateWindow:function()
{if(!this._overviewControls.length)
return;var windowBoundaries=this._overviewControls[0].windowBoundaries(this._windowStartTime,this._windowEndTime);this._muteOnWindowChanged=true;this._overviewGrid.setWindow(windowBoundaries.left,windowBoundaries.right);this._muteOnWindowChanged=false;},__proto__:WebInspector.VBox.prototype}
WebInspector.TimelineOverviewPane.PopoverContents=function()
{WebInspector.VBox.call(this,true);this.contentElement.classList.add("timeline-overview-popover");}
WebInspector.TimelineOverviewPane.PopoverContents.prototype={__proto__:WebInspector.VBox.prototype}
WebInspector.TimelineOverviewCalculator=function()
{this.reset();}
WebInspector.TimelineOverviewCalculator.prototype={paddingLeft:function()
{return this._paddingLeft;},computePosition:function(time)
{return(time-this._minimumBoundary)/this.boundarySpan()*this._workingArea+this._paddingLeft;},positionToTime:function(position)
{return(position-this._paddingLeft)/this._workingArea*this.boundarySpan()+this._minimumBoundary;},setBounds:function(minimumBoundary,maximumBoundary)
{this._minimumBoundary=minimumBoundary;this._maximumBoundary=maximumBoundary;},setDisplayWindow:function(clientWidth,paddingLeft)
{this._paddingLeft=paddingLeft||0;this._workingArea=clientWidth-this._paddingLeft;},reset:function()
{this.setBounds(0,1000);},formatTime:function(value,precision)
{return Number.preciseMillisToString(value-this.zeroTime(),precision);},maximumBoundary:function()
{return this._maximumBoundary;},minimumBoundary:function()
{return this._minimumBoundary;},zeroTime:function()
{return this._minimumBoundary;},boundarySpan:function()
{return this._maximumBoundary-this._minimumBoundary;}}
WebInspector.TimelineOverview=function()
{}
WebInspector.TimelineOverview.prototype={show:function(parentElement,insertBefore){},update:function(){},dispose:function(){},reset:function(){},popoverElementPromise:function(x){},onClick:function(event){},windowTimes:function(windowLeft,windowRight){},windowBoundaries:function(startTime,endTime){},timelineStarted:function(){},timelineStopped:function(){},}
WebInspector.TimelineOverviewBase=function()
{WebInspector.VBox.call(this);this._calculator=null;this._canvas=this.element.createChild("canvas","fill");this._context=this._canvas.getContext("2d");}
WebInspector.TimelineOverviewBase.prototype={update:function()
{this.resetCanvas();},dispose:function()
{this.detach();},reset:function()
{},popoverElementPromise:function(x)
{return Promise.resolve((null));},timelineStarted:function()
{},timelineStopped:function()
{},setCalculator:function(calculator)
{this._calculator=calculator;},onClick:function(event)
{return false;},windowTimes:function(windowLeft,windowRight)
{var absoluteMin=this._calculator.minimumBoundary();var timeSpan=this._calculator.maximumBoundary()-absoluteMin;return{startTime:absoluteMin+timeSpan*windowLeft,endTime:absoluteMin+timeSpan*windowRight};},windowBoundaries:function(startTime,endTime)
{var absoluteMin=this._calculator.minimumBoundary();var timeSpan=this._calculator.maximumBoundary()-absoluteMin;var haveRecords=absoluteMin>0;return{left:haveRecords&&startTime?Math.min((startTime-absoluteMin)/timeSpan,1):0,right:haveRecords&&endTime<Infinity?(endTime-absoluteMin)/timeSpan:1};},resetCanvas:function()
{this._canvas.width=this.element.clientWidth*window.devicePixelRatio;this._canvas.height=this.element.clientHeight*window.devicePixelRatio;},__proto__:WebInspector.VBox.prototype};Runtime.cachedResources["ui_lazy/dataGrid.css"]=".data-grid {\n    position: relative;\n    border: 1px solid #aaa;\n    font-size: 11px;\n    line-height: 120%;\n}\n\n.data-grid table {\n    table-layout: fixed;\n    border-spacing: 0;\n    border-collapse: separate;\n    height: 100%;\n    width: 100%;\n}\n\n.data-grid .header-container,\n.data-grid .data-container {\n    position: absolute;\n    left: 0;\n    right: 0;\n    overflow-x: hidden;\n}\n\n.data-grid .header-container {\n    top: 0;\n    height: 17px;\n }\n\n.data-grid .data-container {\n    top: 17px;\n    bottom: 0;\n    overflow-y: overlay;\n    transform: translateZ(0);\n}\n\n.data-grid.inline .header-container,\n.data-grid.inline .data-container {\n    position: static;\n}\n\n.data-grid.inline .corner {\n    display: none;\n}\n\n.platform-mac .data-grid .corner,\n.data-grid.data-grid-fits-viewport .corner {\n    display: none;\n}\n\n.data-grid .corner {\n    width: 14px;\n    padding-right: 0;\n    padding-left: 0;\n    border-left: 0 none transparent !important;\n}\n\n.data-grid .top-filler-td,\n.data-grid .bottom-filler-td {\n    height: auto !important;\n    padding: 0 !important;\n}\n\n.data-grid table.data {\n    position: absolute;\n    left: 0;\n    top: 0;\n    right: 0;\n    bottom: 0;\n    border-top: 0 none transparent;\n    background-image: linear-gradient(to bottom, white, white 50%, rgb(234, 243, 255) 50%, rgb(234, 243, 255));\n    background-size: 128px 32px;\n    table-layout: fixed;\n}\n\n.data-grid.inline table.data {\n    position: static;\n}\n\n.data-grid table.data tr {\n    display: none;\n}\n\n.data-grid table.data tr.revealed {\n    display: table-row;\n}\n\n.data-grid td,\n.data-grid th {\n    white-space: nowrap;\n    text-overflow: ellipsis;\n    overflow: hidden;\n    line-height: 14px;\n    border-left: 1px solid #aaa;\n}\n\n.data-grid th:first-child,\n.data-grid td:first-child {\n    border-left: none !important;\n}\n\n.data-grid td {\n    height: 16px; /* Keep in sync with .data-grid table.data @ background-size */\n    vertical-align: top;\n    padding: 1px 4px;\n    -webkit-user-select: text;\n}\n\n.data-grid th {\n    height: auto;\n    text-align: left;\n    background-color: #eee;\n    border-bottom: 1px solid #aaa;\n    font-weight: normal;\n    vertical-align: middle;\n    padding: 0 4px;\n}\n\n.data-grid td > div,\n.data-grid th > div {\n    white-space: nowrap;\n    text-overflow: ellipsis;\n    overflow: hidden;\n}\n\n.data-grid td.editing > div {\n    text-overflow: clip;\n}\n\n.data-grid .center {\n    text-align: center;\n}\n\n.data-grid .right {\n    text-align: right;\n}\n\n.data-grid th.sortable {\n    position: relative;\n}\n\n.data-grid th.sortable:active {\n    background-color: rgba(0, 0, 0, 0.15);\n}\n\n.data-grid th .sort-order-icon-container {\n    position: absolute;\n    top: 1px;\n    right: 0;\n    bottom: 1px;\n    display: flex;\n    align-items: center;\n}\n\n.data-grid th .sort-order-icon {\n    margin-right: 4px;\n    background-image: url(Images/toolbarButtonGlyphs.png);\n    background-size: 352px 168px;\n    opacity: 0.5;\n    width: 8px;\n    height: 7px;\n    display: none;\n}\n\n@media (-webkit-min-device-pixel-ratio: 1.5) {\n.data-grid th .sort-order-icon {\n    background-image: url(Images/toolbarButtonGlyphs_2x.png);\n}\n} /* media */\n\n.data-grid th.sort-ascending .sort-order-icon {\n    display: block;\n    background-position: -4px -111px;\n}\n\n.data-grid th.sort-descending .sort-order-icon {\n    display: block;\n    background-position: -20px -99px;\n}\n\n.data-grid th:hover {\n    background-color: hsla(0, 0%, 90%, 1);\n}\n\n.data-grid button {\n    line-height: 18px;\n    color: inherit;\n}\n\n.data-grid td.disclosure::before {\n    -webkit-user-select: none;\n    -webkit-mask-image: url(Images/toolbarButtonGlyphs.png);\n    -webkit-mask-position: -4px -96px;\n    -webkit-mask-size: 352px 168px;\n    float: left;\n    width: 8px;\n    height: 12px;\n    margin-right: 2px;\n    content: \"a\";\n    color: transparent;\n    position: relative;\n    top: 1px;\n    background-color: rgb(110, 110, 110);\n}\n\n.data-grid tr:not(.parent) td.disclosure::before {\n    background-color: transparent;\n}\n\n@media (-webkit-min-device-pixel-ratio: 1.5) {\n.data-grid tr.parent td.disclosure::before {\n    -webkit-mask-image: url(Images/toolbarButtonGlyphs_2x.png);\n}\n} /* media */\n\n.data-grid tr.expanded td.disclosure::before {\n    -webkit-mask-position: -20px -96px;\n}\n\n.data-grid tr.selected {\n    background-color: rgb(212, 212, 212);\n    color: inherit;\n}\n\n.data-grid:focus tr.selected {\n    background-color: rgb(56, 121, 217);\n    color: white;\n}\n\n.data-grid:focus tr.selected a {\n    color: white;\n}\n\n.data-grid:focus tr.parent.selected td.disclosure::before {\n    background-color: white;\n    -webkit-mask-position: -4px -96px;\n}\n\n.data-grid:focus tr.expanded.selected td.disclosure::before {\n    background-color: white;\n    -webkit-mask-position: -20px -96px;\n}\n\n.data-grid-resizer {\n    position: absolute;\n    top: 0;\n    bottom: 0;\n    width: 5px;\n    z-index: 500;\n}\n\n/*# sourceURL=ui_lazy/dataGrid.css */";Runtime.cachedResources["ui_lazy/flameChart.css"]=".flame-chart-main-pane {\n    overflow: hidden;\n}\n\n.flame-chart-marker-highlight-element {\n    position: absolute;\n    top: 0;\n    height: 20px;\n    width: 4px;\n    margin: 0 -2px;\n    content: \"\";\n    display: block;\n}\n\n.flame-chart-highlight-element {\n    background-color: black;\n    position: absolute;\n    opacity: 0.2;\n    pointer-events: none;\n}\n\n.flame-chart-selected-element {\n    position: absolute;\n    pointer-events: none;\n    border-color: rgb(56, 121, 217);\n    border-width: 1px;\n    border-style: solid;\n    background-color: rgba(56, 121, 217, 0.2);\n}\n\n.flame-chart-v-scroll {\n    position: absolute;\n    width: 14px;\n    top: 0;\n    right: 0;\n    bottom: 0;\n    overflow-x: hidden;\n    z-index: 200;\n}\n\n.flame-chart-selection-overlay {\n    position: absolute;\n    z-index: 100;\n    background-color: rgba(56, 121, 217, 0.3);\n    border-color: rgb(16, 81, 177);\n    border-width: 0 1px;\n    border-style: solid;\n    pointer-events: none;\n    top: 0;\n    bottom: 0;\n    text-align: center;\n}\n\n.flame-chart-selection-overlay .time-span {\n    white-space: nowrap;\n    position: absolute;\n    left: 0;\n    right: 0;\n    bottom: 0;\n}\n\n.flame-chart-entry-info {\n    position: absolute;\n    bottom: 6px;\n    left: 12px;\n    background-color: rgba(255, 255, 255, 0.7);\n    pointer-events: none;\n}\n\n.flame-chart-entry-info table tr td:first-child {\n    font-weight: bold;\n    padding-right: 8px;\n}\n\n/*# sourceURL=ui_lazy/flameChart.css */";Runtime.cachedResources["ui_lazy/overviewGrid.css"]="/*\n * Copyright (c) 2014 The Chromium Authors. All rights reserved.\n * Use of this source code is governed by a BSD-style license that can be\n * found in the LICENSE file.\n */\n\n.overview-grid-window-selector {\n    position: absolute;\n    top: 0;\n    bottom: 0;\n    background-color: rgba(125, 173, 217, 0.5);\n    z-index: 250;\n    pointer-events: none;\n}\n\n.overview-grid-window-resizer {\n    position: absolute;\n    top: -1px;\n    height: 20px;\n    width: 6px;\n    margin-left: -3px;\n    background-color: rgb(153, 153, 153);\n    border: 1px solid white;\n    z-index: 500;\n}\n\n.overview-grid-cursor-area {\n    position: absolute;\n    left: 0;\n    right: 0;\n    top: 20px;\n    bottom: 0;\n    z-index: 500;\n    cursor: text;\n}\n\n.overview-grid-cursor-position {\n    position: absolute;\n    top: 0;\n    bottom: 0;\n    width: 2px;\n    background-color: hsla(220, 95%, 50%, 0.7);\n    z-index: 500;\n    pointer-events: none;\n    visibility: hidden;\n    overflow: hidden;\n}\n\n/*# sourceURL=ui_lazy/overviewGrid.css */";Runtime.cachedResources["ui_lazy/pieChart.css"]="/*\n * Copyright (c) 2014 The Chromium Authors. All rights reserved.\n * Use of this source code is governed by a BSD-style license that can be\n * found in the LICENSE file.\n */\n\n.root {\n    position: relative;\n    width: 100%;\n    height: 100%;\n}\n\n.pie-chart-foreground {\n    position: absolute;\n    width: 100%;\n    height: 100%;\n    z-index: 10;\n    top: 0;\n    display: flex;\n}\n\n.pie-chart-total {\n    margin: auto;\n    padding: 2px 5px;\n    background-color: rgba(255, 255, 255, 0.6);\n}\n\n/*# sourceURL=ui_lazy/pieChart.css */";Runtime.cachedResources["ui_lazy/timelineGrid.css"]="/*\n * Copyright (c) 2015 The Chromium Authors. All rights reserved.\n * Use of this source code is governed by a BSD-style license that can be\n * found in the LICENSE file.\n */\n\n.resources-dividers {\n    position: absolute;\n    left: 0;\n    right: 0;\n    top: 0;\n    z-index: -100;\n    bottom: 0;\n}\n\n.resources-event-dividers {\n    position: absolute;\n    left: 0;\n    right: 0;\n    height: 100%;\n    top: 0;\n    z-index: 300;\n    pointer-events: none;\n}\n\n.resources-dividers-label-bar {\n    position: absolute;\n    top: 0;\n    left: 0;\n    right: 0;\n    background-color: rgba(255, 255, 255, 0.85);\n    background-clip: padding-box;\n    height: 20px;\n    z-index: 200;\n    pointer-events: none;\n    overflow: hidden;\n}\n\n.resources-divider {\n    position: absolute;\n    width: 1px;\n    top: 0;\n    bottom: 0;\n    background-color: rgba(0, 0, 0, 0.1);\n}\n\n.resources-event-divider {\n    position: absolute;\n    width: 2px;\n    top: 0;\n    bottom: 0;\n    z-index: 300;\n}\n\n.resources-divider-label {\n    position: absolute;\n    top: 4px;\n    right: 3px;\n    font-size: 80%;\n    white-space: nowrap;\n    pointer-events: none;\n}\n\n.timeline-grid-header {\n    height: 20px;\n    pointer-events: none;\n}\n\n.timeline-curtain-left, .timeline-curtain-right {\n    background-color: hsla(0, 0%, 80%, 0.5);\n    position: absolute;\n    top: 0;\n    height: 100%;\n    z-index: 300;\n    pointer-events: none;\n}\n\n.timeline-curtain-left {\n    left: 0;\n}\n\n.timeline-curtain-right {\n    right: 0;\n}\n\n/*# sourceURL=ui_lazy/timelineGrid.css */";