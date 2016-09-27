// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

"use strict";

var xmlTreeViewerCSS = privateScriptController.import("DocumentXMLTreeViewer.css");

privateScriptController.installClass("Document", function(DocumentPrototype) {
    var nodeParentPairs = [];
    var tree;

    function prepareWebKitXMLViewer(noStyleMessage)
    {
        var html = createHTMLElement('html');
        var head = createHTMLElement('head');
        html.appendChild(head);
        var style = createHTMLElement('style');
        style.id = 'xml-viewer-style';
        style.appendChild(document.createTextNode(xmlTreeViewerCSS));
        head.appendChild(style);
        var body = createHTMLElement('body');
        html.appendChild(body);
        var sourceXML = createHTMLElement('div');
        sourceXML.id = 'webkit-xml-viewer-source-xml';
        body.appendChild(sourceXML);

        var child;
        while (child = document.firstChild) {
            document.removeChild(child);
            if (child.nodeType != Node.DOCUMENT_TYPE_NODE)
                sourceXML.appendChild(child);
        }
        document.appendChild(html);

        var header = createHTMLElement('div');
        body.appendChild(header);
        header.classList.add('header');
        var headerSpan = createHTMLElement('span');
        header.appendChild(headerSpan);
        headerSpan.textContent = noStyleMessage;
        header.appendChild(createHTMLElement('br'));

        tree = createHTMLElement('div');
        body.appendChild(tree);
        tree.classList.add('pretty-print');
        window.onload = sourceXMLLoaded;
    }

    function sourceXMLLoaded()
    {
        var sourceXML = document.getElementById('webkit-xml-viewer-source-xml');
        if (!sourceXML)
            return; // Stop if some XML tree extension is already processing this document

        for (var child = sourceXML.firstChild; child; child = child.nextSibling)
            nodeParentPairs.push({parentElement: tree, node: child});

        for (var i = 0; i < nodeParentPairs.length; i++)
            processNode(nodeParentPairs[i].parentElement, nodeParentPairs[i].node);

        drawArrows();
        initButtons();

        return false;
    }

    // Tree processing.

    function processNode(parentElement, node)
    {
        var map = processNode.processorsMap;
        if (!map) {
            map = {};
            processNode.processorsMap = map;
            map[Node.PROCESSING_INSTRUCTION_NODE] = processProcessingInstruction;
            map[Node.ELEMENT_NODE] = processElement;
            map[Node.COMMENT_NODE] = processComment;
            map[Node.TEXT_NODE] = processText;
            map[Node.CDATA_SECTION_NODE] = processCDATA;
        }
        if (processNode.processorsMap[node.nodeType])
            processNode.processorsMap[node.nodeType].call(this, parentElement, node);
    }

    function processElement(parentElement, node)
    {
        if (!node.firstChild)
            processEmptyElement(parentElement, node);
        else {
            var child = node.firstChild;
            if (child.nodeType == Node.TEXT_NODE && isShort(child.nodeValue) && !child.nextSibling)
                processShortTextOnlyElement(parentElement, node);
            else
                processComplexElement(parentElement, node);
        }
    }

    function processEmptyElement(parentElement, node)
    {
        var line = createLine();
        line.appendChild(createTag(node, false, true));
        parentElement.appendChild(line);
    }

    function processShortTextOnlyElement(parentElement, node)
    {
        var line = createLine();
        line.appendChild(createTag(node, false, false));
        for (var child = node.firstChild; child; child = child.nextSibling)
            line.appendChild(createText(child.nodeValue));
        line.appendChild(createTag(node, true, false));
        parentElement.appendChild(line);
    }

    function processComplexElement(parentElement, node)
    {
        var collapsible = createCollapsible();

        collapsible.expanded.start.appendChild(createTag(node, false, false));
        for (var child = node.firstChild; child; child = child.nextSibling)
            nodeParentPairs.push({parentElement: collapsible.expanded.content, node: child});
        collapsible.expanded.end.appendChild(createTag(node, true, false));

        collapsible.collapsed.content.appendChild(createTag(node, false, false));
        collapsible.collapsed.content.appendChild(createText('...'));
        collapsible.collapsed.content.appendChild(createTag(node, true, false));
        parentElement.appendChild(collapsible);
    }

    function processComment(parentElement, node)
    {
        if (isShort(node.nodeValue)) {
            var line = createLine();
            line.appendChild(createComment('<!-- ' + node.nodeValue + ' -->'));
            parentElement.appendChild(line);
        } else {
            var collapsible = createCollapsible();

            collapsible.expanded.start.appendChild(createComment('<!--'));
            collapsible.expanded.content.appendChild(createComment(node.nodeValue));
            collapsible.expanded.end.appendChild(createComment('-->'));

            collapsible.collapsed.content.appendChild(createComment('<!--'));
            collapsible.collapsed.content.appendChild(createComment('...'));
            collapsible.collapsed.content.appendChild(createComment('-->'));
            parentElement.appendChild(collapsible);
        }
    }

    function processCDATA(parentElement, node)
    {
        if (isShort(node.nodeValue)) {
            var line = createLine();
            line.appendChild(createText('<![CDATA[ ' + node.nodeValue + ' ]]>'));
            parentElement.appendChild(line);
        } else {
            var collapsible = createCollapsible();

            collapsible.expanded.start.appendChild(createText('<![CDATA['));
            collapsible.expanded.content.appendChild(createText(node.nodeValue));
            collapsible.expanded.end.appendChild(createText(']]>'));

            collapsible.collapsed.content.appendChild(createText('<![CDATA['));
            collapsible.collapsed.content.appendChild(createText('...'));
            collapsible.collapsed.content.appendChild(createText(']]>'));
            parentElement.appendChild(collapsible);
        }
    }

    function processProcessingInstruction(parentElement, node)
    {
        if (isShort(node.nodeValue)) {
            var line = createLine();
            line.appendChild(createComment('<?' + node.nodeName + ' ' + node.nodeValue + '?>'));
            parentElement.appendChild(line);
        } else {
            var collapsible = createCollapsible();

            collapsible.expanded.start.appendChild(createComment('<?' + node.nodeName));
            collapsible.expanded.content.appendChild(createComment(node.nodeValue));
            collapsible.expanded.end.appendChild(createComment('?>'));

            collapsible.collapsed.content.appendChild(createComment('<?' + node.nodeName));
            collapsible.collapsed.content.appendChild(createComment('...'));
            collapsible.collapsed.content.appendChild(createComment('?>'));
            parentElement.appendChild(collapsible);
        }
    }

    function processText(parentElement, node)
    {
        parentElement.appendChild(createText(node.nodeValue));
    }

    // Processing utils.

    function trim(value)
    {
        return value.replace(/^\s\s*/, '').replace(/\s\s*$/, '');
    }

    function isShort(value)
    {
        return trim(value).length <= 50;
    }

    // Tree rendering.

    function createHTMLElement(elementName)
    {
        return document.createElementNS('http://www.w3.org/1999/xhtml', elementName)
    }

    function createCollapsible()
    {
        var collapsible = createHTMLElement('div');
        collapsible.classList.add('collapsible');
        collapsible.expanded = createHTMLElement('div');
        collapsible.expanded.classList.add('expanded');
        collapsible.appendChild(collapsible.expanded);

        collapsible.expanded.start = createLine();
        collapsible.expanded.start.appendChild(createCollapseButton());
        collapsible.expanded.appendChild(collapsible.expanded.start);

        collapsible.expanded.content = createHTMLElement('div');
        collapsible.expanded.content.classList.add('collapsible-content');
        collapsible.expanded.appendChild(collapsible.expanded.content);

        collapsible.expanded.end = createLine();
        collapsible.expanded.appendChild(collapsible.expanded.end);

        collapsible.collapsed = createHTMLElement('div');
        collapsible.collapsed.classList.add('collapsed');
        collapsible.collapsed.classList.add('hidden');
        collapsible.appendChild(collapsible.collapsed);
        collapsible.collapsed.content = createLine();
        collapsible.collapsed.content.appendChild(createExpandButton());
        collapsible.collapsed.appendChild(collapsible.collapsed.content);

        return collapsible;
    }

    function createButton()
    {
        var button = createHTMLElement('span');
        button.classList.add('button');
        return button;
    }

    function createCollapseButton(str)
    {
        var button = createButton();
        button.classList.add('collapse-button');
        return button;
    }

    function createExpandButton(str)
    {
        var button = createButton();
        button.classList.add('expand-button');
        return button;
    }

    function createComment(commentString)
    {
        var comment = createHTMLElement('span');
        comment.classList.add('comment');
        comment.classList.add('html-comment');
        comment.textContent = commentString;
        return comment;
    }

    function createText(value)
    {
        var text = createHTMLElement('span');
        text.textContent = trim(value);
        text.classList.add('text');
        return text;
    }

    function createLine()
    {
        var line = createHTMLElement('div');
        line.classList.add('line');
        return line;
    }

    function createTag(node, isClosing, isEmpty)
    {
        var tag = createHTMLElement('span');
        tag.classList.add('html-tag');

        var stringBeforeAttrs = '<';
        if (isClosing)
            stringBeforeAttrs += '/';
        stringBeforeAttrs += node.nodeName;
        var textBeforeAttrs = document.createTextNode(stringBeforeAttrs);
        tag.appendChild(textBeforeAttrs);

        if (!isClosing) {
            for (var i = 0; i < node.attributes.length; i++)
                tag.appendChild(createAttribute(node.attributes[i]));
        }

        var stringAfterAttrs = '';
        if (isEmpty)
            stringAfterAttrs += '/';
        stringAfterAttrs += '>';
        var textAfterAttrs = document.createTextNode(stringAfterAttrs);
        tag.appendChild(textAfterAttrs);

        return tag;
    }

    function createAttribute(attributeNode)
    {
        var attribute = createHTMLElement('span');
        attribute.classList.add('html-attribute');

        var attributeName = createHTMLElement('span');
        attributeName.classList.add('html-attribute-name');
        attributeName.textContent = attributeNode.name;

        var textBefore = document.createTextNode(' ');
        var textBetween = document.createTextNode('="');

        var attributeValue = createHTMLElement('span');
        attributeValue.classList.add('html-attribute-value');
        attributeValue.textContent = attributeNode.value;

        var textAfter = document.createTextNode('"');

        attribute.appendChild(textBefore);
        attribute.appendChild(attributeName);
        attribute.appendChild(textBetween);
        attribute.appendChild(attributeValue);
        attribute.appendChild(textAfter);
        return attribute;
    }

    // Tree behaviour.

    function drawArrows()
    {
        var ctx = document.getCSSCanvasContext("2d", "arrowRight", 10, 11);

        ctx.fillStyle = "rgb(90,90,90)";
        ctx.beginPath();
        ctx.moveTo(0, 0);
        ctx.lineTo(0, 8);
        ctx.lineTo(7, 4);
        ctx.lineTo(0, 0);
        ctx.fill();
        ctx.closePath();

        var ctx = document.getCSSCanvasContext("2d", "arrowDown", 10, 10);

        ctx.fillStyle = "rgb(90,90,90)";
        ctx.beginPath();
        ctx.moveTo(0, 0);
        ctx.lineTo(8, 0);
        ctx.lineTo(4, 7);
        ctx.lineTo(0, 0);
        ctx.fill();
        ctx.closePath();
    }

    function expandFunction(sectionId)
    {
        return function()
        {
            document.querySelector('#' + sectionId + ' > .expanded').className = 'expanded';
            document.querySelector('#' + sectionId + ' > .collapsed').className = 'collapsed hidden';
        };
    }

    function collapseFunction(sectionId)
    {
        return function()
        {
            document.querySelector('#' + sectionId + ' > .expanded').className = 'expanded hidden';
            document.querySelector('#' + sectionId + ' > .collapsed').className = 'collapsed';
        };
    }

    function initButtons()
    {
        var sections = document.querySelectorAll('.collapsible');
        for (var i = 0; i < sections.length; i++) {
            var sectionId = 'collapsible' + i;
            sections[i].id = sectionId;

            var expandedPart = sections[i].querySelector('#' + sectionId + ' > .expanded');
            var collapseButton = expandedPart.querySelector('.collapse-button');
            collapseButton.onclick = collapseFunction(sectionId);
            collapseButton.onmousedown = handleButtonMouseDown;

            var collapsedPart = sections[i].querySelector('#' + sectionId + ' > .collapsed');
            var expandButton = collapsedPart.querySelector('.expand-button');
            expandButton.onclick = expandFunction(sectionId);
            expandButton.onmousedown = handleButtonMouseDown;
        }

    }

    function handleButtonMouseDown(e)
    {
       // To prevent selection on double click
       e.preventDefault();
    }

    DocumentPrototype.transformDocumentToTreeView = function(noStyleMessage) {
        prepareWebKitXMLViewer(noStyleMessage);
    }
});

