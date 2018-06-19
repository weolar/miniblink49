const IntlCollator = process.binding('atom_common_intl_collator').IntlCollator;

/*
function Collator(langArray, resolvedOptions) {
	this.m_resolvedOptions = resolvedOptions;
}

Collator.prototype.resolvedOptions = function () {
	return this.m_resolvedOptions;
}



IntlCollator.prototype.compare = function(a, b) {
	console.log("this._compare:" + this._compare);
	return this._compare(a, b);
}


Object.setPrototypeOf(Collator.prototype, IntlCollator.prototype);
*/

function Collator(langArray, resolvedOptions) {
	this.m_resolvedOptions = resolvedOptions;
	this.intl = new IntlCollator(langArray, resolvedOptions);
}

Collator.prototype.resolvedOptions = function () {
	return this.m_resolvedOptions;
}

Collator.prototype.compare = function(a, b) {
	return this.intl.compare(a, b);
}

exports.Collator = Collator;