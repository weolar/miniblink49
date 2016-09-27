/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef Platform_h
#define Platform_h

#ifdef WIN32
#include <windows.h>
#endif

#include "WebAudioDevice.h"
#include "WebBatteryStatusListener.h"
#include "WebCommon.h"
#include "WebData.h"
#include "WebDeviceLightListener.h"
#include "WebGamepadListener.h"
#include "WebGamepads.h"
#include "WebGestureDevice.h"
#include "WebGraphicsContext3D.h"
#include "WebLocalizedString.h"
#include "WebPlatformEventType.h"
#include "WebSize.h"
#include "WebSpeechSynthesizer.h"
#include "WebStorageQuotaCallbacks.h"
#include "WebStorageQuotaType.h"
#include "WebString.h"
#include "WebURLError.h"
#include "WebVector.h"
#include "WebWaitableEvent.h"

#include <vector>

class GrContext;

namespace blink {

class WebAudioBus;
class WebBlobRegistry;
class WebBluetooth;
class WebClipboard;
class WebCompositorSupport;
class WebConvertableToTraceFormat;
class WebCookieJar;
class WebCrypto;
class WebDatabaseObserver;
class WebDiscardableMemory;
class WebPlatformEventListener;
class WebFallbackThemeEngine;
class WebFileSystem;
class WebFileUtilities;
class WebFlingAnimator;
class WebGeofencingProvider;
class WebGestureCurve;
class WebGraphicsContext3DProvider;
class WebIDBFactory;
class WebMIDIAccessor;
class WebMIDIAccessorClient;
class WebMediaStreamCenter;
class WebMediaStreamCenterClient;
class WebMemoryDumpProvider;
class WebMessagePortChannel;
class WebMimeRegistry;
class WebNavigatorConnectProvider;
class WebNotificationManager;
class WebPermissionClient;
class WebPluginListBuilder;
class WebPrescientNetworking;
class WebProcessMemoryDump;
class WebPublicSuffixList;
class WebPushProvider;
class WebRTCPeerConnectionHandler;
class WebRTCPeerConnectionHandlerClient;
class WebSandboxSupport;
class WebScrollbarBehavior;
class WebSecurityOrigin;
class WebServicePortProvider;
class WebServicePortProviderClient;
class WebServiceWorkerCacheStorage;
class WebSocketHandle;
class WebSpeechSynthesizer;
class WebSpeechSynthesizerClient;
class WebStorageNamespace;
class WebSyncProvider;
struct WebFloatPoint;
class WebThemeEngine;
class WebThread;
class WebURL;
class WebURLLoader;
class WebUnitTestSupport;
struct WebLocalizedString;
struct WebSize;

class Platform {
public:
    // HTML5 Database ------------------------------------------------------

#ifdef WIN32
    typedef HANDLE FileHandle;
#else
    typedef int FileHandle;
#endif

    BLINK_PLATFORM_EXPORT static void initialize(Platform*);
    BLINK_PLATFORM_EXPORT static void shutdown();
    BLINK_PLATFORM_EXPORT static Platform* current();

    // May return null.
    virtual WebCookieJar* cookieJar() { return nullptr; }

    // Must return non-null.
    virtual WebClipboard* clipboard() { return nullptr; }

    // Must return non-null.
    virtual WebFileUtilities* fileUtilities() { return nullptr; }

    // Must return non-null.
    virtual WebMimeRegistry* mimeRegistry() { return nullptr; }

    // May return null if sandbox support is not necessary
    virtual WebSandboxSupport* sandboxSupport() { return nullptr; }

    // May return null on some platforms.
    virtual WebThemeEngine* themeEngine() { return nullptr; }

    virtual WebFallbackThemeEngine* fallbackThemeEngine() { return nullptr; }

    // May return null.
    virtual WebSpeechSynthesizer* createSpeechSynthesizer(WebSpeechSynthesizerClient*) { return nullptr; }


    // Audio --------------------------------------------------------------

    virtual double audioHardwareSampleRate() { return 0; }
    virtual size_t audioHardwareBufferSize() { return 0; }
    virtual unsigned audioHardwareOutputChannels() { return 0; }

    // Creates a device for audio I/O.
    // Pass in (numberOfInputChannels > 0) if live/local audio input is desired.
    virtual WebAudioDevice* createAudioDevice(size_t bufferSize, unsigned numberOfInputChannels, unsigned numberOfChannels, double sampleRate, WebAudioDevice::RenderCallback*, const WebString& deviceId) { return nullptr; }


    // MIDI ----------------------------------------------------------------

    // Creates a platform dependent WebMIDIAccessor. MIDIAccessor under platform
    // creates and owns it.
    virtual WebMIDIAccessor* createMIDIAccessor(WebMIDIAccessorClient*) { return nullptr; }


    // Blob ----------------------------------------------------------------

    // Must return non-null.
    virtual WebBlobRegistry* blobRegistry() { return nullptr; }

    // Database ------------------------------------------------------------

    // Opens a database file; dirHandle should be 0 if the caller does not need
    // a handle to the directory containing this file
    virtual FileHandle databaseOpenFile(const WebString& vfsFileName, int desiredFlags) { return FileHandle(); }

    // Deletes a database file and returns the error code
    virtual int databaseDeleteFile(const WebString& vfsFileName, bool syncDir) { return 0; }

    // Returns the attributes of the given database file
    virtual long databaseGetFileAttributes(const WebString& vfsFileName) { return 0; }

    // Returns the size of the given database file
    virtual long long databaseGetFileSize(const WebString& vfsFileName) { return 0; }

    // Returns the space available for the given origin
    virtual long long databaseGetSpaceAvailableForOrigin(const WebString& originIdentifier) { return 0; }

    // Set the size of the given database file
    virtual bool databaseSetFileSize(const WebString& vfsFileName, long long size) { return false; }


    // DOM Storage --------------------------------------------------

    // Return a LocalStorage namespace
    virtual WebStorageNamespace* createLocalStorageNamespace() { return nullptr; }


    // FileSystem ----------------------------------------------------------

    // Must return non-null.
    virtual WebFileSystem* fileSystem() { return nullptr; }


    // IDN conversion ------------------------------------------------------

    virtual WebString convertIDNToUnicode(const WebString& host, const WebString& languages) { return host; }


    // IndexedDB ----------------------------------------------------------

    // Must return non-null.
    virtual WebIDBFactory* idbFactory() { return nullptr; }


    // Cache Storage ----------------------------------------------------------

    // The caller is responsible for deleting the returned object.
    virtual WebServiceWorkerCacheStorage* cacheStorage(const WebString& originIdentifier) { return nullptr; }

    // Gamepad -------------------------------------------------------------

    virtual void sampleGamepads(WebGamepads& into) { into.length = 0; }

    // History -------------------------------------------------------------

    // Returns the hash for the given canonicalized URL for use in visited
    // link coloring.
    virtual unsigned long long visitedLinkHash(
        const char* canonicalURL, size_t length) { return 0; }

    // Returns whether the given link hash is in the user's history. The
    // hash must have been generated by calling VisitedLinkHash().
    virtual bool isLinkVisited(unsigned long long linkHash) { return false; }


    // Keygen --------------------------------------------------------------

    // Handle the <keygen> tag for generating client certificates
    // Returns a base64 encoded signed copy of a public key from a newly
    // generated key pair and the supplied challenge string. keySizeindex
    // specifies the strength of the key.
    virtual WebString signedPublicKeyAndChallengeString(unsigned keySizeIndex,
                                                        const WebString& challenge,
                                                        const WebURL& url) { return WebString(); }


    // Memory --------------------------------------------------------------

    // Returns the current space allocated for the pagefile, in MB.
    // That is committed size for Windows and virtual memory size for POSIX
    virtual size_t memoryUsageMB() { return 0; }

    // Same as above, but always returns actual value, without any caches.
    virtual size_t actualMemoryUsageMB() { return 0; }

    // Return the physical memory of the current machine, in MB.
    virtual size_t physicalMemoryMB() { return 0; }

    // Return the available virtual memory of the current machine, in MB. Or
    // zero, if there is no limit.
    virtual size_t virtualMemoryLimitMB() { return 0; }

    // Return the number of of processors of the current machine.
    virtual size_t numberOfProcessors() { return 0; }

    // Returns private and shared usage, in bytes. Private bytes is the amount of
    // memory currently allocated to this process that cannot be shared. Returns
    // false on platform specific error conditions.
    virtual bool processMemorySizesInBytes(size_t* privateBytes, size_t* sharedBytes) { return false; }

    // Reports number of bytes used by memory allocator for internal needs.
    // Returns true if the size has been reported, or false otherwise.
    virtual bool memoryAllocatorWasteInBytes(size_t*) { return false; }

    // Allocates discardable memory. May return nullptr, even if the platform supports
    // discardable memory. If nonzero, however, then the WebDiscardableMmeory is
    // returned in an locked state. You may use its underlying data() member
    // directly, taking care to unlock it when you are ready to let it become
    // discardable.
    virtual WebDiscardableMemory* allocateAndLockDiscardableMemory(size_t bytes) { return nullptr; }

    static const size_t noDecodedImageByteLimit = static_cast<size_t>(-1);

    // Returns the maximum amount of memory a decoded image should be allowed.
    // See comments on ImageDecoder::m_maxDecodedBytes.
    virtual size_t maxDecodedImageBytes() { return noDecodedImageByteLimit; }

    // Process -------------------------------------------------------------

    // Returns a unique identifier for a process. This may not necessarily be
    // the process's process ID.
    virtual uint32_t getUniqueIdForProcess() { return 0; }


    // Message Ports -------------------------------------------------------

    // Creates a Message Port Channel pair. This can be called on any thread.
    // The returned objects should only be used on the thread they were created on.
    virtual void createMessageChannel(WebMessagePortChannel** channel1, WebMessagePortChannel** channel2) { *channel1 = 0; *channel2 = 0; }


    // Network -------------------------------------------------------------

    // Returns a new WebURLLoader instance.
    virtual WebURLLoader* createURLLoader() { return nullptr; }

    // May return null.
    virtual WebPrescientNetworking* prescientNetworking() { return nullptr; }

    // Returns a new WebSocketHandle instance.
    virtual WebSocketHandle* createWebSocketHandle() { return nullptr; }

    // Returns the User-Agent string.
    virtual WebString userAgent() { return WebString(); }

    // A suggestion to cache this metadata in association with this URL.
    virtual void cacheMetadata(const WebURL&, int64 responseTime, const char* data, size_t dataSize) { }

    // Returns the decoded data url if url had a supported mimetype and parsing was successful.
    virtual WebData parseDataURL(const WebURL&, WebString& mimetype, WebString& charset) { return WebData(); }

    virtual WebURLError cancelledError(const WebURL&) const { return WebURLError(); }

    virtual bool isReservedIPAddress(const WebString& host) const { return false; }

    virtual bool portAllowed(const WebURL&) const { return false; }

    // Plugins -------------------------------------------------------------

    // If refresh is true, then cached information should not be used to
    // satisfy this call.
    virtual void getPluginList(bool refresh, WebPluginListBuilder*) { }


    // Public Suffix List --------------------------------------------------

    // May return null on some platforms.
    virtual WebPublicSuffixList* publicSuffixList() { return nullptr; }


    // Resources -----------------------------------------------------------

    // Returns a localized string resource (with substitution parameters).
    virtual WebString queryLocalizedString(WebLocalizedString::Name) { return WebString(); }
    virtual WebString queryLocalizedString(WebLocalizedString::Name, const WebString& parameter) { return WebString(); }
    virtual WebString queryLocalizedString(WebLocalizedString::Name, const WebString& parameter1, const WebString& parameter2) { return WebString(); }


    // Threads -------------------------------------------------------

    // Creates an embedder-defined thread.
    virtual WebThread* createThread(const char* name) { return nullptr; }

    // Returns an interface to the current thread. This is owned by the
    // embedder.
    virtual WebThread* currentThread() { return nullptr; }

    // Yield the current thread so another thread can be scheduled.
    virtual void yieldCurrentThread() { }

    // WaitableEvent -------------------------------------------------------

    // Creates an embedder-defined waitable event object.
    // TODO(toyoshim): Remove "virtual" from the no arguments version API once
    // the embedder drops the override implementation.
    virtual WebWaitableEvent* createWaitableEvent() { return createWaitableEvent(WebWaitableEvent::ResetPolicy::Auto, WebWaitableEvent::InitialState::NonSignaled); }
    virtual WebWaitableEvent* createWaitableEvent(WebWaitableEvent::ResetPolicy, WebWaitableEvent::InitialState) { return nullptr; }

    // Waits on multiple events and returns the event object that has been
    // signaled. This may return nullptr if it fails to wait events.
    // Any event objects given to this method must not deleted while this
    // wait is happening.
    virtual WebWaitableEvent* waitMultipleEvents(const WebVector<WebWaitableEvent*>& events) { return nullptr; }


    // Profiling -----------------------------------------------------------

    virtual void decrementStatsCounter(const char* name) { }
    virtual void incrementStatsCounter(const char* name) { }


    // Resources -----------------------------------------------------------

    // Returns a blob of data corresponding to the named resource.
    virtual WebData loadResource(const char* name) { return WebData(); }

    // Decodes the in-memory audio file data and returns the linear PCM audio data in the destinationBus.
    // A sample-rate conversion to sampleRate will occur if the file data is at a different sample-rate.
    // Returns true on success.
    virtual bool loadAudioResource(WebAudioBus* destinationBus, const char* audioFileData, size_t dataSize) { return false; }

    // Screen -------------------------------------------------------------

    // Supplies the system monitor color profile.
    virtual void screenColorProfile(WebVector<char>* profile) { }


    // Scrollbar ----------------------------------------------------------

    // Must return non-null.
    virtual WebScrollbarBehavior* scrollbarBehavior() { return nullptr; }


    // Sudden Termination --------------------------------------------------

    // Disable/Enable sudden termination on a process level. When possible, it
    // is preferable to disable sudden termination on a per-frame level via
    // WebFrameClient::suddenTerminationDisablerChanged.
    virtual void suddenTerminationChanged(bool enabled) { }


    // System --------------------------------------------------------------

    // Returns a value such as "en-US".
    virtual WebString defaultLocale() { return WebString(); }

    // Wall clock time in seconds since the epoch.
    virtual double currentTime() { return 0; }

    // Monotonically increasing time in seconds from an arbitrary fixed point in the past.
    // This function is expected to return at least millisecond-precision values. For this reason,
    // it is recommended that the fixed point be no further in the past than the epoch.
    virtual double monotonicallyIncreasingTime() { return 0; }

    // System trace time in seconds. For example, on Chrome OS, this timestamp should be
    // synchronized with ftrace timestamps.
    virtual double systemTraceTime() { return 0; }

    // WebKit clients must implement this funcion if they use cryptographic randomness.
    virtual void cryptographicallyRandomValues(unsigned char* buffer, size_t length) = 0;

    // Returns an interface to the main thread. Can be null if blink was initialized on a thread without a message loop.
    BLINK_PLATFORM_EXPORT WebThread* mainThread() const;

    // Vibration -----------------------------------------------------------

    // Starts a vibration for the given duration in milliseconds. If there is currently an active
    // vibration it will be cancelled before the new one is started.
    virtual void vibrate(unsigned time) { }

    // Cancels the current vibration, if there is one.
    virtual void cancelVibration() { }


    // Testing -------------------------------------------------------------

    // Get a pointer to testing support interfaces. Will not be available in production builds.
    virtual WebUnitTestSupport* unitTestSupport() { return nullptr; }


    // Tracing -------------------------------------------------------------

    // Get a pointer to the enabled state of the given trace category. The
    // embedder can dynamically change the enabled state as trace event
    // recording is started and stopped by the application. Only long-lived
    // literal strings should be given as the category name. The implementation
    // expects the returned pointer to be held permanently in a local static. If
    // the unsigned char is non-zero, tracing is enabled. If tracing is enabled,
    // addTraceEvent is expected to be called by the trace event macros.
    virtual const unsigned char* getTraceCategoryEnabledFlag(const char* categoryName) { return nullptr; }

    typedef intptr_t TraceEventAPIAtomicWord;

    // Get a pointer to a global state of the given thread. An embedder is
    // expected to update the global state as the state of the embedder changes.
    // A sampling thread in the Chromium side reads the global state periodically
    // and reflects the sampling profiled results into about:tracing.
    virtual TraceEventAPIAtomicWord* getTraceSamplingState(const unsigned bucketName) { return nullptr; }

    typedef uint64_t TraceEventHandle;

    // Add a trace event to the platform tracing system. Depending on the actual
    // enabled state, this event may be recorded or dropped.
    // - phase specifies the type of event:
    //   - BEGIN ('B'): Marks the beginning of a scoped event.
    //   - END ('E'): Marks the end of a scoped event.
    //   - COMPLETE ('X'): Marks the beginning of a scoped event, but doesn't
    //     need a matching END event. Instead, at the end of the scope,
    //     updateTraceEventDuration() must be called with the TraceEventHandle
    //     returned from addTraceEvent().
    //   - INSTANT ('I'): Standalone, instantaneous event.
    //   - START ('S'): Marks the beginning of an asynchronous event (the end
    //     event can occur in a different scope or thread). The id parameter is
    //     used to match START/FINISH pairs.
    //   - FINISH ('F'): Marks the end of an asynchronous event.
    //   - COUNTER ('C'): Used to trace integer quantities that change over
    //     time. The argument values are expected to be of type int.
    //   - METADATA ('M'): Reserved for internal use.
    // - categoryEnabled is the pointer returned by getTraceCategoryEnabledFlag.
    // - name is the name of the event. Also used to match BEGIN/END and
    //   START/FINISH pairs.
    // - id optionally allows events of the same name to be distinguished from
    //   each other. For example, to trace the consutruction and destruction of
    //   objects, specify the pointer as the id parameter.
    // - numArgs specifies the number of elements in argNames, argTypes, and
    //   argValues.
    // - argNames is the array of argument names. Use long-lived literal strings
    //   or specify the COPY flag.
    // - argTypes is the array of argument types:
    //   - BOOL (1): bool
    //   - UINT (2): unsigned long long
    //   - INT (3): long long
    //   - DOUBLE (4): double
    //   - POINTER (5): void*
    //   - STRING (6): char* (long-lived null-terminated char* string)
    //   - COPY_STRING (7): char* (temporary null-terminated char* string)
    //   - CONVERTABLE (8): WebConvertableToTraceFormat
    // - argValues is the array of argument values. Each value is the unsigned
    //   long long member of a union of all supported types.
    // - convertableValues is the array of WebConvertableToTraceFormat classes
    //   that may be converted to trace format by calling asTraceFormat method.
    //   ConvertableToTraceFormat interface.
    //   convertableValues can be moved to another object by
    //   WebConvertableToTraceFormat::moveFrom() in addTraceEvent(), and thus
    //   should not be dereferenced (e.g. asTraceFormat() should not be called)
    //   after return from addTraceEvent().
    // - thresholdBeginId optionally specifies the value returned by a previous
    //   call to addTraceEvent with a BEGIN phase.
    // - threshold is used on an END phase event in conjunction with the
    //   thresholdBeginId of a prior BEGIN event. The threshold is the minimum
    //   number of microseconds that must have passed since the BEGIN event. If
    //   less than threshold microseconds has passed, the BEGIN/END pair is
    //   dropped.
    // - flags can be 0 or one or more of the following, ORed together:
    //   - COPY (0x1): treat all strings (name, argNames and argValues of type
    //     string) as temporary so that they will be copied by addTraceEvent.
    //   - HAS_ID (0x2): use the id argument to uniquely identify the event for
    //     matching with other events of the same name.
    //   - MANGLE_ID (0x4): specify this flag if the id parameter is the value
    //     of a pointer.
    virtual TraceEventHandle addTraceEvent(
        char phase,
        const unsigned char* categoryEnabledFlag,
        const char* name,
        unsigned long long id,
        double timestamp,
        int numArgs,
        const char** argNames,
        const unsigned char* argTypes,
        const unsigned long long* argValues,
        WebConvertableToTraceFormat* convertableValues,
        unsigned char flags)
    {
        return 0;
    }

    // Set the duration field of a COMPLETE trace event.
    virtual void updateTraceEventDuration(const unsigned char* categoryEnabledFlag, const char* name, TraceEventHandle) { }

    // Callbacks for reporting histogram data.
    // CustomCounts histogram has exponential bucket sizes, so that min=1, max=1000000, bucketCount=50 would do.
    virtual void histogramCustomCounts(const char* name, int sample, int min, int max, int bucketCount) { }
    // Enumeration histogram buckets are linear, boundaryValue should be larger than any possible sample value.
    virtual void histogramEnumeration(const char* name, int sample, int boundaryValue) { }
    // Unlike enumeration histograms, sparse histograms only allocate memory for non-empty buckets.
    virtual void histogramSparse(const char* name, int sample) { }

    // Record to a RAPPOR privacy-preserving metric, see: https://www.chromium.org/developers/design-documents/rappor.
    // recordRappor records a sample string, while recordRapporURL records the domain and registry of a url.
    virtual void recordRappor(const char* metric, const WebString& sample) { }
    virtual void recordRapporURL(const char* metric, const blink::WebURL& url) { }

    // Registers a memory dump provider. The WebMemoryDumpProvider::onMemoryDump
    // method will be called on the same thread that called the
    // registerMemoryDumpProvider() method.
    // See crbug.com/458295 for design docs.
    virtual void registerMemoryDumpProvider(blink::WebMemoryDumpProvider*) { }

    // Must be called on the thread that called registerMemoryDumpProvider().
    virtual void unregisterMemoryDumpProvider(blink::WebMemoryDumpProvider*) { }

    // Returns a newly allocated WebProcessMemoryDump instance.
    virtual blink::WebProcessMemoryDump* createProcessMemoryDump() { return nullptr; }

    typedef uint64_t WebMemoryAllocatorDumpGuid;

    // Returns guid corresponding to the given string (the hash value) for
    // creating a WebMemoryAllocatorDump.
    virtual WebMemoryAllocatorDumpGuid createWebMemoryAllocatorDumpGuid(const WebString& guidStr) { return 0; }

    // GPU ----------------------------------------------------------------
    //
    // May return null if GPU is not supported.
    // Returns newly allocated and initialized offscreen WebGraphicsContext3D instance.
    // Passing an existing context to shareContext will create the new context in the same share group as the passed context.
    virtual WebGraphicsContext3D* createOffscreenGraphicsContext3D(const WebGraphicsContext3D::Attributes&, WebGraphicsContext3D* shareContext) { return nullptr; }
    virtual WebGraphicsContext3D* createOffscreenGraphicsContext3D(const WebGraphicsContext3D::Attributes&, WebGraphicsContext3D* shareContext, WebGLInfo* glInfo) { return nullptr; }
    virtual WebGraphicsContext3D* createOffscreenGraphicsContext3D(const WebGraphicsContext3D::Attributes&) { return nullptr; }

    // Returns a newly allocated and initialized offscreen context provider. The provider may return a null
    // graphics context if GPU is not supported.
    virtual WebGraphicsContext3DProvider* createSharedOffscreenGraphicsContext3DProvider() { return nullptr; }

    // Returns true if the platform is capable of producing an offscreen context suitable for accelerating 2d canvas.
    // This will return false if the platform cannot promise that contexts will be preserved across operations like
    // locking the screen or if the platform cannot provide a context with suitable performance characteristics.
    //
    // This value must be checked again after a context loss event as the platform's capabilities may have changed.
    virtual bool canAccelerate2dCanvas() { return false; }

    virtual bool isThreadedCompositingEnabled() { return false; }

    virtual WebCompositorSupport* compositorSupport() { return nullptr; }

    virtual WebFlingAnimator* createFlingAnimator() { return nullptr; }

    // Creates a new fling animation curve instance for device |deviceSource|
    // with |velocity| and already scrolled |cumulativeScroll| pixels.
    virtual WebGestureCurve* createFlingAnimationCurve(WebGestureDevice deviceSource, const WebFloatPoint& velocity, const WebSize& cumulativeScroll) { return nullptr; }

    // WebRTC ----------------------------------------------------------

    // Creates an WebRTCPeerConnectionHandler for RTCPeerConnection.
    // May return null if WebRTC functionality is not avaliable or out of resources.
    virtual WebRTCPeerConnectionHandler* createRTCPeerConnectionHandler(WebRTCPeerConnectionHandlerClient*) { return nullptr; }

    // May return null if WebRTC functionality is not avaliable or out of resources.
    virtual WebMediaStreamCenter* createMediaStreamCenter(WebMediaStreamCenterClient*) { return nullptr; }


    // WebWorker ----------------------------------------------------------

    virtual void didStartWorkerRunLoop() { }
    virtual void didStopWorkerRunLoop() { }

    // WebCrypto ----------------------------------------------------------

    virtual WebCrypto* crypto() { return nullptr; }


    // Platform events -----------------------------------------------------
    // Device Orientation, Device Motion, Device Light, Battery, Gamepad.

    // Request the platform to start listening to the events of the specified
    // type and notify the given listener (if not null) when there is an update.
    virtual void startListening(WebPlatformEventType type, WebPlatformEventListener* listener) { }

    // Request the platform to stop listening to the specified event and no
    // longer notify the listener, if any.
    virtual void stopListening(WebPlatformEventType type) { }

    // This method converts from the supplied DOM code enum to the
    // embedder's DOM code value for the key pressed. |domCode| values are
    // based on the value defined in ui/events/keycodes/dom4/keycode_converter_data.h.
    // Returns null string, if DOM code value is not found.
    virtual WebString domCodeStringFromEnum(int domCode) { return WebString(); }

    // This method converts from the suppled DOM code value to the
    // embedder's DOM code enum for the key pressed. |codeString| is defined in
    // ui/events/keycodes/dom4/keycode_converter_data.h.
    // Returns 0, if DOM code enum is not found.
    virtual int domEnumFromCodeString(const WebString& codeString) { return 0; }

    // This method converts from the supplied DOM |key| enum to the
    // corresponding DOM |key| string value for the key pressed. |domKey| values are
    // based on the value defined in ui/events/keycodes/dom3/dom_key_data.h.
    // Returns empty string, if DOM key value is not found.
    virtual WebString domKeyStringFromEnum(int domKey) { return WebString(); }

    // This method converts from the suppled DOM |key| value to the
    // embedder's DOM |key| enum for the key pressed. |keyString| is defined in
    // ui/events/keycodes/dom3/dom_key_data.h.
    // Returns 0 if DOM key enum is not found.
    virtual int domKeyEnumFromString(const WebString& keyString) { return 0; }

    // Quota -----------------------------------------------------------

    // Queries the storage partition's storage usage and quota information.
    // WebStorageQuotaCallbacks::didQueryStorageUsageAndQuota will be called
    // with the current usage and quota information for the partition. When
    // an error occurs WebStorageQuotaCallbacks::didFail is called with an
    // error code.
    virtual void queryStorageUsageAndQuota(
        const WebURL& storagePartition,
        WebStorageQuotaType,
        WebStorageQuotaCallbacks) { }


    // WebDatabase --------------------------------------------------------

    virtual WebDatabaseObserver* databaseObserver() { return nullptr; }


    // Web Notifications --------------------------------------------------

    virtual WebNotificationManager* notificationManager() { return nullptr; }


    // Geofencing ---------------------------------------------------------

    virtual WebGeofencingProvider* geofencingProvider() { return nullptr; }


    // Bluetooth ----------------------------------------------------------

    // Returns pointer to client owned WebBluetooth implementation.
    virtual WebBluetooth* bluetooth() { return nullptr; }


    // Push API------------------------------------------------------------

    virtual WebPushProvider* pushProvider() { return nullptr; }


    // navigator.connect --------------------------------------------------

    virtual WebNavigatorConnectProvider* navigatorConnectProvider() { return nullptr; }

    // Returns pointer to a new blink owned WebServicePortProvider instance,
    // associated with a particular ServicePortCollection (identified by the
    // WebServicePortProviderClient passed in).
    virtual WebServicePortProvider* createServicePortProvider(WebServicePortProviderClient*) { return nullptr; }

    // Permissions --------------------------------------------------------

    virtual WebPermissionClient* permissionClient() { return nullptr; }


    // Background Sync API------------------------------------------------------------

    virtual WebSyncProvider* backgroundSyncProvider() { return nullptr; }

protected:
    BLINK_PLATFORM_EXPORT Platform();
    virtual ~Platform() { }

    WebThread* m_mainThread;
};

} // namespace blink

#endif
