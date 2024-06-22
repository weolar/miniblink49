
#include "net/cookies/CookieMonster.h"

#include "net/cookies/CookieUtil.h"
#include "net/cookies/CanonicalCookie.h"

#include "third_party/WebKit/Source/wtf/text/WTFStringUtil.h"

namespace net {

CookieMonster::CookieMonster()
{
	persist_session_cookies_ = false;
	keep_expired_cookies_ = false;
}

CookieMonster::~CookieMonster()
{
}

std::string CookieMonster::GetKey(const std::string& domain) const {
	std::string effective_domain(cookie_util::GetDomainAndRegistry(domain, cookie_util::INCLUDE_PRIVATE_REGISTRIES));
	if (effective_domain.empty())
		effective_domain = domain;

	if (!effective_domain.empty() && effective_domain[0] == '.')
		return effective_domain.substr(1);
	return effective_domain;
}

void CookieMonster::FindCookiesForKey(const std::string& key,
	const blink::KURL& url,
	const CookieOptions& options,
	const double& current,
	bool update_access_time,
	std::vector<CanonicalCookie*>* cookies) {
	//lock_.AssertAcquired();

	for (CookieMapItPair its = cookies_.equal_range(key); its.first != its.second; ) {
		CookieMap::iterator curit = its.first;
		CanonicalCookie* cc = curit->second;
		++its.first;

		// If the cookie is expired, delete it.
		if (cc->IsExpired(current) && !keep_expired_cookies_) {
			InternalDeleteCookie(curit, true, DELETE_COOKIE_EXPIRED);
			continue;
		}

		// Filter out cookies that should not be included for a request to the
		// given |url|. HTTP only cookies are filtered depending on the passed
		// cookie |options|.
		if (!cc->IncludeForRequestURL(url, options))
			continue;

		// Add this cookie to the set of matching cookies. Update the access
		// time if we've been requested to do so.
		if (update_access_time) {
			//InternalUpdateCookieAccessTime(cc, current);
		}
		cookies->push_back(cc);
	}
}

void CookieMonster::FindCookiesForHostAndDomain(
	const blink::KURL& url,
	const CookieOptions& options,
	bool update_access_time,
	std::vector<CanonicalCookie*>* cookies) {
	//lock_.AssertAcquired();

	const double current_time(WTF::currentTime());

	// Probe to save statistics relatively frequently.  We do it here rather
	// than in the set path as many websites won't set cookies, and we
	// want to collect statistics whenever the browser's being used.
	//RecordPeriodicStats(current_time);

	// Can just dispatch to FindCookiesForKey
	const std::string key(GetKey(WTF::WTFStringToStdString(url.host())));
	FindCookiesForKey(key, url, options, current_time, update_access_time, cookies);
}

// Mozilla sorts on the path length (longest first), and then it
// sorts by creation time (oldest first).
// The RFC says the sort order for the domain attribute is undefined.
bool CookieSorter(CanonicalCookie* cc1, CanonicalCookie* cc2) {
	if (cc1->Path().length() == cc2->Path().length())
		return cc1->CreationDate() < cc2->CreationDate();
	return cc1->Path().length() > cc2->Path().length();
}

CookieList CookieMonster::GetAllCookiesForURLWithOptions(const blink::KURL& url, const CookieOptions& options) {
	//base::AutoLock autolock(lock_);

	std::vector<CanonicalCookie*> cookie_ptrs;
	FindCookiesForHostAndDomain(url, options, false, &cookie_ptrs);
	std::sort(cookie_ptrs.begin(), cookie_ptrs.end(), CookieSorter);

	CookieList cookies;
	for (std::vector<CanonicalCookie*>::const_iterator it = cookie_ptrs.begin(); it != cookie_ptrs.end(); it++)
		cookies.push_back(**it);

	return cookies;
}

bool CookieMonster::DeleteAnyEquivalentCookie(const std::string& key, const CanonicalCookie& ecc, bool skip_httponly, bool already_expired) {
	//lock_.AssertAcquired();

	bool found_equivalent_cookie = false;
	bool skipped_httponly = false;
	for (CookieMapItPair its = cookies_.equal_range(key); its.first != its.second; ) {
		CookieMap::iterator curit = its.first;
		CanonicalCookie* cc = curit->second;
		++its.first;

		if (ecc.IsEquivalent(*cc)) {
			// We should never have more than one equivalent cookie, since they should
			// overwrite each other.
			ASSERT(!found_equivalent_cookie);// << "Duplicate equivalent cookies found, cookie store is corrupted.";
			if (skip_httponly && cc->IsHttpOnly()) {
				skipped_httponly = true;
			} else {
				InternalDeleteCookie(curit, true, already_expired ? DELETE_COOKIE_EXPIRED_OVERWRITE : DELETE_COOKIE_OVERWRITE);
			}
			found_equivalent_cookie = true;
		}
	}
	return skipped_httponly;
}

bool CookieMonster::SetCanonicalCookie(CanonicalCookie* cc, const double& creation_time, const CookieOptions& options) {
	const std::string key(GetKey((cc)->Domain()));
	bool already_expired = (cc)->IsExpired(creation_time);
	if (DeleteAnyEquivalentCookie(key, *cc, options.exclude_httponly(), already_expired)) {
		//VLOG(kVlogSetCookies) << "SetCookie() not clobbering httponly cookie";
		return false;
	}

	//VLOG(kVlogSetCookies) << "SetCookie() key: " << key << " cc: " << (*cc)->DebugString();

	// Realize that we might be setting an expired cookie, and the only point
	// was to delete the cookie which we've already done.
	if (!already_expired || keep_expired_cookies_) {
		// See InitializeHistograms() for details.
		if ((cc)->IsPersistent()) {
// 			histogram_expiration_duration_minutes_->Add(((*cc)->ExpiryDate() - creation_time).InMinutes());
		}

		InternalInsertCookie(key, cc, true);
	} else {
		//VLOG(kVlogSetCookies) << "SetCookie() not storing already expired cookie.";
	}

	// We assume that hopefully setting a cookie will be less common than
	// querying a cookie.  Since setting a cookie can put us over our limits,
	// make sure that we garbage collect...  We can also make the assumption that
	// if a cookie was set, in the common case it will be used soon after,
	// and we will purge the expired cookies in GetCookies().
	GarbageCollect(creation_time, key);

	return true;
}

void CookieMonster::UpdataCookie(const blink::KURL& url, const std::string& name, const std::string& value) {
	const std::string key(GetKey(WTF::WTFStringToStdString(url.host())));

	bool find = false;
	for (CookieMapItPair its = cookies_.equal_range(key); its.first != its.second; ) {
		
		CookieMap::iterator curit = its.first;
		CanonicalCookie* cc = curit->second;
		++its.first;

		if (cc->Name() != name)
			continue;

		find = true;
		cc->SetValue(value);
	}

	if (!find) {
		CanonicalCookie* cc = new CanonicalCookie(url, name, value,
			WTF::WTFStringToStdString(url.host()), "/",
			WTF::currentTime(), 0, 0, false, false,
			COOKIE_PRIORITY_DEFAULT);
		InternalInsertCookie(key, cc, false);
	}
}

// InternalDeleteCookies must not invalidate iterators other than the one being
// deleted.
void CookieMonster::InternalDeleteCookie(CookieMap::iterator it, bool sync_to_store, DeletionCause deletion_cause) {
	//lock_.AssertAcquired();

	// Ideally, this would be asserted up where we define ChangeCauseMapping,
	// but DeletionCause's visibility (or lack thereof) forces us to make
	// this check here.
	//COMPILE_ASSERT(arraysize(ChangeCauseMapping) == DELETE_COOKIE_LAST_ENTRY + 1, ChangeCauseMapping_size_not_eq_DeletionCause_enum_size);

	// See InitializeHistograms() for details.
// 	if (deletion_cause != DELETE_COOKIE_DONT_RECORD)
// 		histogram_cookie_deletion_cause_->Add(deletion_cause);

	CanonicalCookie* cc = it->second;
	//VLOG(kVlogSetCookies) << "InternalDeleteCookie() cc: " << cc->DebugString();

// 	if ((cc->IsPersistent() || persist_session_cookies_) && store_.get() &&	sync_to_store)
// 		store_->DeleteCookie(*cc);

// 	if (delegate_.get()) {
// 		ChangeCausePair mapping = ChangeCauseMapping[deletion_cause];
// 
// 		if (mapping.notify)
// 			delegate_->OnCookieChanged(*cc, true, mapping.cause);
// 	}
	cookies_.erase(it);
	delete cc;
}


CookieMonster::CookieMap::iterator CookieMonster::InternalInsertCookie(const std::string& key, CanonicalCookie* cc, bool sync_to_store) {
	//lock_.AssertAcquired();

// 	if ((cc->IsPersistent() || persist_session_cookies_) && store_.get() &&	sync_to_store)
// 		store_->AddCookie(*cc);

	CookieMap::iterator inserted = cookies_.insert(CookieMap::value_type(key, cc));
// 	if (delegate_.get()) {
// 		delegate_->OnCookieChanged(
// 			*cc, false, CookieMonsterDelegate::CHANGE_COOKIE_EXPLICIT);
// 	}

	return inserted;
}

// Domain expiry behavior is unchanged by key/expiry scheme (the
// meaning of the key is different, but that's not visible to this routine).
int CookieMonster::GarbageCollect(const double& current, const std::string& key) {
	return 0;
}

}