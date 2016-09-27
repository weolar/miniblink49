/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef AudioListener_h
#define AudioListener_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "platform/geometry/FloatPoint3D.h"
#include "platform/heap/Handle.h"
#include "wtf/Vector.h"

namespace blink {

class HRTFDatabaseLoader;
class PannerHandler;

// AudioListener maintains the state of the listener in the audio scene as defined in the OpenAL specification.

class AudioListener : public GarbageCollectedFinalized<AudioListener>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static AudioListener* create()
    {
        return new AudioListener();
    }
    virtual ~AudioListener();

    // Position
    void setPosition(float x, float y, float z) { setPosition(FloatPoint3D(x, y, z)); }
    const FloatPoint3D& position() const { return m_position; }

    // Orientation and Up-vector
    void setOrientation(float x, float y, float z, float upX, float upY, float upZ)
    {
        setOrientation(FloatPoint3D(x, y, z));
        setUpVector(FloatPoint3D(upX, upY, upZ));
    }
    const FloatPoint3D& orientation() const { return m_orientation; }
    const FloatPoint3D& upVector() const { return m_upVector; }

    // Velocity
    void setVelocity(float x, float y, float z) { setVelocity(FloatPoint3D(x, y, z)); }
    const FloatPoint3D& velocity() const { return m_velocity; }

    // Doppler factor
    void setDopplerFactor(double);
    double dopplerFactor() const { return m_dopplerFactor; }

    // Speed of sound
    void setSpeedOfSound(double);
    double speedOfSound() const { return m_speedOfSound; }

    Mutex& listenerLock() { return m_listenerLock; }
    void addPanner(PannerHandler&);
    void removePanner(PannerHandler&);

    // HRTF DB loader
    HRTFDatabaseLoader* hrtfDatabaseLoader() { return m_hrtfDatabaseLoader.get(); }
    void createAndLoadHRTFDatabaseLoader(float);
    bool isHRTFDatabaseLoaded();
    void waitForHRTFDatabaseLoaderThreadCompletion();

    DECLARE_TRACE();

private:
    AudioListener();

    void setPosition(const FloatPoint3D&);
    void setOrientation(const FloatPoint3D&);
    void setUpVector(const FloatPoint3D&);
    void setVelocity(const FloatPoint3D&);

    void markPannersAsDirty(unsigned);

    FloatPoint3D m_position;
    FloatPoint3D m_orientation;
    FloatPoint3D m_upVector;
    FloatPoint3D m_velocity;
    double m_dopplerFactor;
    double m_speedOfSound;

    // Synchronize a panner's process() with setting of the state of the listener.
    mutable Mutex m_listenerLock;
    // List for pannerNodes in context. This is updated only in the main thread,
    // and can be referred in audio thread.
    // These raw pointers are safe because PannerHandler::uninitialize()
    // unregisters it from m_panners.
    HashSet<PannerHandler*> m_panners;
    // HRTF DB loader for panner node.
    RefPtr<HRTFDatabaseLoader> m_hrtfDatabaseLoader;
};

} // namespace blink

#endif // AudioListener_h
