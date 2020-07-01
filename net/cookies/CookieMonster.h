#include <vector>
#include <map>

#include "net/cookies/CanonicalCookie.h"

namespace blink {
class KURL;
}

namespace net {

class CookieMonster {
public:
	CookieMonster();
	~CookieMonster();

	// Terminology:
	//    * The 'top level domain' (TLD) of an internet domain name is
	//      the terminal "." free substring (e.g. "com" for google.com
	//      or world.std.com).
	//    * The 'effective top level domain' (eTLD) is the longest
	//      "." initiated terminal substring of an internet domain name
	//      that is controlled by a general domain registrar.
	//      (e.g. "co.uk" for news.bbc.co.uk).
	//    * The 'effective top level domain plus one' (eTLD+1) is the
	//      shortest "." delimited terminal substring of an internet
	//      domain name that is not controlled by a general domain
	//      registrar (e.g. "bbc.co.uk" for news.bbc.co.uk, or
	//      "google.com" for news.google.com).  The general assumption
	//      is that all hosts and domains under an eTLD+1 share some
	//      administrative control.

	// CookieMap is the central data structure of the CookieMonster.  It
	// is a map whose values are pointers to CanonicalCookie data
	// structures (the data structures are owned by the CookieMonster
	// and must be destroyed when removed from the map).  The key is based on the
	// effective domain of the cookies.  If the domain of the cookie has an
	// eTLD+1, that is the key for the map.  If the domain of the cookie does not
	// have an eTLD+1, the key of the map is the host the cookie applies to (it is
	// not legal to have domain cookies without an eTLD+1).  This rule
	// excludes cookies for, e.g, ".com", ".co.uk", or ".internalnetwork".
	// This behavior is the same as the behavior in Firefox v 3.6.10.

	// NOTE(deanm):
	// I benchmarked hash_multimap vs multimap.  We're going to be query-heavy
	// so it would seem like hashing would help.  However they were very
	// close, with multimap being a tiny bit faster.  I think this is because
	// our map is at max around 1000 entries, and the additional complexity
	// for the hashing might not overcome the O(log(1000)) for querying
	// a multimap.  Also, multimap is standard, another reason to use it.
	// TODO(rdsmith): This benchmark should be re-done now that we're allowing
	// subtantially more entries in the map.
	typedef std::multimap<std::string, CanonicalCookie*> CookieMap;
	typedef std::pair<CookieMap::iterator, CookieMap::iterator> CookieMapItPair;
	typedef std::vector<CookieMap::iterator> CookieItVector;

	CookieMap cookies_;

	// Internal reasons for deletion, used to populate informative histograms
	// and to provide a public cause for onCookieChange notifications.
	//
	// If you add or remove causes from this list, please be sure to also update
	// the CookieMonsterDelegate::ChangeCause mapping inside ChangeCauseMapping.
	// Moreover, these are used as array indexes, so avoid reordering to keep the
	// histogram buckets consistent. New items (if necessary) should be added
	// at the end of the list, just before DELETE_COOKIE_LAST_ENTRY.
	enum DeletionCause {
		DELETE_COOKIE_EXPLICIT = 0,
		DELETE_COOKIE_OVERWRITE,
		DELETE_COOKIE_EXPIRED,
		DELETE_COOKIE_EVICTED,
		DELETE_COOKIE_DUPLICATE_IN_BACKING_STORE,
		DELETE_COOKIE_DONT_RECORD,  // e.g. For final cleanup after flush to store.
		DELETE_COOKIE_EVICTED_DOMAIN,
		DELETE_COOKIE_EVICTED_GLOBAL,

		// Cookies evicted during domain level garbage collection that
		// were accessed longer ago than kSafeFromGlobalPurgeDays
		DELETE_COOKIE_EVICTED_DOMAIN_PRE_SAFE,

		// Cookies evicted during domain level garbage collection that
		// were accessed more recently than kSafeFromGlobalPurgeDays
		// (and thus would have been preserved by global garbage collection).
		DELETE_COOKIE_EVICTED_DOMAIN_POST_SAFE,

		// A common idiom is to remove a cookie by overwriting it with an
		// already-expired expiration date. This captures that case.
		DELETE_COOKIE_EXPIRED_OVERWRITE,

		// Cookies are not allowed to contain control characters in the name or
		// value. However, we used to allow them, so we are now evicting any such
		// cookies as we load them. See http://crbug.com/238041.
		DELETE_COOKIE_CONTROL_CHAR,

		DELETE_COOKIE_LAST_ENTRY
	};
	
	std::string GetKey(const std::string& domain) const;

	void FindCookiesForHostAndDomain(const blink::KURL& url,
		const CookieOptions& options,
		bool update_access_time,
		std::vector<CanonicalCookie*>* cookies);

	void FindCookiesForKey(const std::string& key,
		const blink::KURL& url,
		const CookieOptions& options,
		const double& current,
		bool update_access_time,
		std::vector<CanonicalCookie*>* cookies);

	bool SetCanonicalCookie(CanonicalCookie* cc, const double& creation_time, const CookieOptions& options);

	void UpdataCookie(const blink::KURL& url, const std::string& name, const std::string& value);

	CookieList GetAllCookiesForURLWithOptions(const blink::KURL& url, const CookieOptions& options);

	bool DeleteAnyEquivalentCookie(const std::string& key, const CanonicalCookie& ecc, bool skip_httponly, bool already_expired);

	void InternalDeleteCookie(CookieMap::iterator it, bool sync_to_store, DeletionCause deletion_cause);

	CookieMonster::CookieMap::iterator InternalInsertCookie(const std::string& key, CanonicalCookie* cc, bool sync_to_store);

	int GarbageCollect(const double& current, const std::string& key);

private:
	bool persist_session_cookies_;
	bool keep_expired_cookies_;
};

}