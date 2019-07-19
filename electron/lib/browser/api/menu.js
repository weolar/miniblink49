'use strict';

const EventEmitter = require('events').EventEmitter;
const bindings = process.binding('atom_browser_menu');
const Menu = bindings.Menu;
Object.setPrototypeOf(Menu.prototype, EventEmitter.prototype);

const MenuItem = require('electron').MenuItem;

Menu.prototype._init = function () {
    if (("items" in this))
        return;

    this.items = [];
	
    this.on('click', function (event, click, focusedWindow, focusedWebContents) {
        click(event, focusedWindow, focusedWebContents);
	});
}

Menu.prototype.popup = function (options) {
	// windowObj, x, y, positioningItem
    this._init();
    if (options && ('x' in options) && 'y' in options)
		this._popup(options.x, options.y);
	else
		this._popup();
}

Menu.prototype.append = function (item) {
    //console.log('Menu.append:');
    this._init();
    this._insert(this.getItemCount(), item);
    this.items.push(item);
}

Menu.prototype.insert = function (pos, item) {
    this._init();
    this._insert(pos, item);
    this.items.splice(pos, 0, item)
}

Menu.prototype.clear = function () {
    this.items = [];
    this._clear();
}

Menu.buildFromTemplate = function (template) {
    var insertIndex, item, j, k, key, len, len1, menu, menuItem, positionedTemplate;
    if (!Array.isArray(template)) {
        throw new TypeError('Invalid template for Menu');
    }
    positionedTemplate = [];
    insertIndex = 0;
    for (j = 0, len = template.length; j < len; j++) {
        item = template[j];
        if (item.position) {
            insertIndex = indexToInsertByPosition(positionedTemplate, item.position);
        } else {
            // If no |position| is specified, insert after last item.
            insertIndex++;
        }
        positionedTemplate.splice(insertIndex, 0, item);
    }

    menu = new Menu();
    menu._init();

    for (k = 0, len1 = positionedTemplate.length; k < len1; k++) {
        item = positionedTemplate[k];
        if (typeof item !== 'object') {
            throw new TypeError('Invalid template for MenuItem');
        }

        menuItem = new MenuItem(item);
        for (key in item) {
            // Preserve extra fields specified by user
            if (!menuItem.hasOwnProperty(key)) {
                menuItem[key] = item[key];
            }
        }
        menu.append(menuItem);
    }
    return menu;
}

// Returns the index of where to insert the item according to |position|.
var indexToInsertByPosition = function (items, position) {
    var insertIndex;
    if (!position) {
        return items.length;
    }
    const positionSplit = position.split('=');
    const query = positionSplit[0];
    const id = positionSplit[1];

    insertIndex = indexOfItemById(items, id);
    if (insertIndex === -1 && query !== 'endof') {
        console.warn("Item with id '" + id + "' is not found");
        return items.length;
    }
    switch (query) {
        case 'after':
            insertIndex++;
            break
        case 'endof':

            // If the |id| doesn't exist, then create a new group with the |id|.
            if (insertIndex === -1) {
                items.push({
                    id: id,
                    type: 'separator'
                })
                insertIndex = items.length - 1;
            }

            // Find the end of the group.
            insertIndex++;
            while (insertIndex < items.length && items[insertIndex].type !== 'separator') {
                insertIndex++;
            }
    }
    return insertIndex;
}

var applicationMenu = null;
Menu.getApplicationMenu = function () {
    return applicationMenu;
}

Menu.setApplicationMenu = function (menu) {
	if (!(menu === null || menu.constructor === Menu)) {
		throw new TypeError('Invalid menu');
	}

	// Keep a reference.
	applicationMenu = menu;
	
	//menu._setApplicationMenu(); // weolar
}

module.exports = Menu;