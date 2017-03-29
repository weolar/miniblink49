const IntlCollator = process.binding('atom_common_intl_collator').IntlCollator;

function Collator(langArray, resolvedOptions) {
	this.m_resolvedOptions = resolvedOptions;
}

Collator.prototype.resolvedOptions = function () {
	return this.m_resolvedOptions;
}

Object.setPrototypeOf(Collator.prototype, IntlCollator.prototype)

exports.Collator = Collator;