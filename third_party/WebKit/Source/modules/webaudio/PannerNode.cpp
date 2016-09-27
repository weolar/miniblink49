/*
 * Copyright (C) 2010, Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#if ENABLE(WEB_AUDIO)
#include "modules/webaudio/PannerNode.h"

#include "bindings/core/v8/ExceptionMessages.h"
#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/ExecutionContext.h"
#include "modules/webaudio/AudioBufferSourceNode.h"
#include "modules/webaudio/AudioContext.h"
#include "modules/webaudio/AudioNodeInput.h"
#include "modules/webaudio/AudioNodeOutput.h"
#include "platform/audio/HRTFPanner.h"
#include "wtf/MathExtras.h"

namespace blink {

static void fixNANs(double& x)
{
    if (std::isnan(x) || std::isinf(x))
        x = 0.0;
}

PannerHandler::PannerHandler(AudioNode& node, float sampleRate)
    : AudioHandler(NodeTypePanner, node, sampleRate)
    , m_listener(node.context()->listener())
    , m_panningModel(Panner::PanningModelEqualPower)
    , m_distanceModel(DistanceEffect::ModelInverse)
    , m_position(0, 0, 0)
    , m_orientation(1, 0, 0)
    , m_velocity(0, 0, 0)
    , m_isAzimuthElevationDirty(true)
    , m_isDistanceConeGainDirty(true)
    , m_isDopplerRateDirty(true)
    , m_lastGain(-1.0)
    , m_cachedAzimuth(0)
    , m_cachedElevation(0)
    , m_cachedDistanceConeGain(1.0f)
    , m_cachedDopplerRate(1)
{
    // Load the HRTF database asynchronously so we don't block the Javascript thread while creating the HRTF database.
    // The HRTF panner will return zeroes until the database is loaded.
    listener()->createAndLoadHRTFDatabaseLoader(node.context()->sampleRate());

    addInput();
    addOutput(2);

    // Node-specific default mixing rules.
    m_channelCount = 2;
    m_channelCountMode = ClampedMax;
    m_channelInterpretation = AudioBus::Speakers;

    initialize();
}

PassRefPtr<PannerHandler> PannerHandler::create(AudioNode& node, float sampleRate)
{
    return adoptRef(new PannerHandler(node, sampleRate));
}

PannerHandler::~PannerHandler()
{
    uninitialize();
}

void PannerHandler::process(size_t framesToProcess)
{
    AudioBus* destination = output(0).bus();

    if (!isInitialized() || !input(0).isConnected() || !m_panner.get()) {
        destination->zero();
        return;
    }

    AudioBus* source = input(0).bus();
    if (!source) {
        destination->zero();
        return;
    }

    // The audio thread can't block on this lock, so we call tryLock() instead.
    MutexTryLocker tryLocker(m_processLock);
    MutexTryLocker tryListenerLocker(listener()->listenerLock());

    if (tryLocker.locked() && tryListenerLocker.locked()) {
        // HRTFDatabase should be loaded before proceeding for offline audio context when the panning model is HRTF.
        if (m_panningModel == Panner::PanningModelHRTF && !listener()->isHRTFDatabaseLoaded()) {
            if (context()->isOfflineContext()) {
                listener()->waitForHRTFDatabaseLoaderThreadCompletion();
            } else {
                destination->zero();
                return;
            }
        }

        // Apply the panning effect.
        double azimuth;
        double elevation;
        azimuthElevation(&azimuth, &elevation);

        m_panner->pan(azimuth, elevation, source, destination, framesToProcess);

        // Get the distance and cone gain.
        float totalGain = distanceConeGain();

        // Snap to desired gain at the beginning.
        if (m_lastGain == -1.0)
            m_lastGain = totalGain;

        // Apply gain in-place with de-zippering.
        destination->copyWithGainFrom(*destination, &m_lastGain, totalGain);
    } else {
        // Too bad - The tryLock() failed.
        // We must be in the middle of changing the properties of the panner or the listener.
        destination->zero();
    }
}

void PannerHandler::initialize()
{
    if (isInitialized())
        return;

    m_panner = Panner::create(m_panningModel, sampleRate(), listener()->hrtfDatabaseLoader());
    listener()->addPanner(*this);

    AudioHandler::initialize();
}

void PannerHandler::uninitialize()
{
    if (!isInitialized())
        return;

    m_panner.clear();
    listener()->removePanner(*this);

    AudioHandler::uninitialize();
}

AudioListener* PannerHandler::listener()
{
    return m_listener;
}

String PannerHandler::panningModel() const
{
    switch (m_panningModel) {
    case Panner::PanningModelEqualPower:
        return "equalpower";
    case Panner::PanningModelHRTF:
        return "HRTF";
    default:
        ASSERT_NOT_REACHED();
        return "equalpower";
    }
}

void PannerHandler::setPanningModel(const String& model)
{
    if (model == "equalpower")
        setPanningModel(Panner::PanningModelEqualPower);
    else if (model == "HRTF")
        setPanningModel(Panner::PanningModelHRTF);
}

bool PannerHandler::setPanningModel(unsigned model)
{
    switch (model) {
    case Panner::PanningModelEqualPower:
    case Panner::PanningModelHRTF:
        if (!m_panner.get() || model != m_panningModel) {
            // This synchronizes with process().
            MutexLocker processLocker(m_processLock);
            m_panner = Panner::create(model, sampleRate(), listener()->hrtfDatabaseLoader());
            m_panningModel = model;
        }
        break;
    default:
        ASSERT_NOT_REACHED();
        return false;
    }

    return true;
}

String PannerHandler::distanceModel() const
{
    switch (const_cast<PannerHandler*>(this)->m_distanceEffect.model()) {
    case DistanceEffect::ModelLinear:
        return "linear";
    case DistanceEffect::ModelInverse:
        return "inverse";
    case DistanceEffect::ModelExponential:
        return "exponential";
    default:
        ASSERT_NOT_REACHED();
        return "inverse";
    }
}

void PannerHandler::setDistanceModel(const String& model)
{
    if (model == "linear")
        setDistanceModel(DistanceEffect::ModelLinear);
    else if (model == "inverse")
        setDistanceModel(DistanceEffect::ModelInverse);
    else if (model == "exponential")
        setDistanceModel(DistanceEffect::ModelExponential);
}

bool PannerHandler::setDistanceModel(unsigned model)
{
    switch (model) {
    case DistanceEffect::ModelLinear:
    case DistanceEffect::ModelInverse:
    case DistanceEffect::ModelExponential:
        if (model != m_distanceModel) {
            // This synchronizes with process().
            MutexLocker processLocker(m_processLock);
            m_distanceEffect.setModel(static_cast<DistanceEffect::ModelType>(model), true);
            m_distanceModel = model;
        }
        break;
    default:
        ASSERT_NOT_REACHED();
        return false;
    }

    return true;
}

void PannerHandler::setRefDistance(double distance)
{
    if (refDistance() == distance)
        return;

    // This synchronizes with process().
    MutexLocker processLocker(m_processLock);
    m_distanceEffect.setRefDistance(distance);
    markPannerAsDirty(PannerHandler::DistanceConeGainDirty);
}

void PannerHandler::setMaxDistance(double distance)
{
    if (maxDistance() == distance)
        return;

    // This synchronizes with process().
    MutexLocker processLocker(m_processLock);
    m_distanceEffect.setMaxDistance(distance);
    markPannerAsDirty(PannerHandler::DistanceConeGainDirty);
}

void PannerHandler::setRolloffFactor(double factor)
{
    if (rolloffFactor() == factor)
        return;

    // This synchronizes with process().
    MutexLocker processLocker(m_processLock);
    m_distanceEffect.setRolloffFactor(factor);
    markPannerAsDirty(PannerHandler::DistanceConeGainDirty);
}

void PannerHandler::setConeInnerAngle(double angle)
{
    if (coneInnerAngle() == angle)
        return;

    // This synchronizes with process().
    MutexLocker processLocker(m_processLock);
    m_coneEffect.setInnerAngle(angle);
    markPannerAsDirty(PannerHandler::DistanceConeGainDirty);
}

void PannerHandler::setConeOuterAngle(double angle)
{
    if (coneOuterAngle() == angle)
        return;

    // This synchronizes with process().
    MutexLocker processLocker(m_processLock);
    m_coneEffect.setOuterAngle(angle);
    markPannerAsDirty(PannerHandler::DistanceConeGainDirty);
}

void PannerHandler::setConeOuterGain(double angle)
{
    if (coneOuterGain() == angle)
        return;

    // This synchronizes with process().
    MutexLocker processLocker(m_processLock);
    m_coneEffect.setOuterGain(angle);
    markPannerAsDirty(PannerHandler::DistanceConeGainDirty);
}

void PannerHandler::setPosition(float x, float y, float z)
{
    FloatPoint3D position = FloatPoint3D(x, y, z);

    if (m_position == position)
        return;

    // This synchronizes with process().
    MutexLocker processLocker(m_processLock);
    m_position = position;
    markPannerAsDirty(PannerHandler::AzimuthElevationDirty | PannerHandler::DistanceConeGainDirty | PannerHandler::DopplerRateDirty);
}

void PannerHandler::setOrientation(float x, float y, float z)
{
    FloatPoint3D orientation = FloatPoint3D(x, y, z);

    if (m_orientation == orientation)
        return;

    // This synchronizes with process().
    MutexLocker processLocker(m_processLock);
    m_orientation = orientation;
    markPannerAsDirty(PannerHandler::DistanceConeGainDirty);
}

void PannerHandler::setVelocity(float x, float y, float z)
{
    FloatPoint3D velocity = FloatPoint3D(x, y, z);

    if (m_velocity == velocity)
        return;

    // This synchronizes with process().
    MutexLocker processLocker(m_processLock);
    m_velocity = velocity;
    markPannerAsDirty(PannerHandler::DopplerRateDirty);
}

void PannerHandler::calculateAzimuthElevation(double* outAzimuth, double* outElevation)
{
    double azimuth = 0.0;

    // Calculate the source-listener vector
    FloatPoint3D listenerPosition = listener()->position();
    FloatPoint3D sourceListener = m_position - listenerPosition;

    // normalize() does nothing if the length of |sourceListener| is zero.
    sourceListener.normalize();

    // Align axes
    FloatPoint3D listenerFront = listener()->orientation();
    FloatPoint3D listenerUp = listener()->upVector();
    FloatPoint3D listenerRight = listenerFront.cross(listenerUp);
    listenerRight.normalize();

    FloatPoint3D listenerFrontNorm = listenerFront;
    listenerFrontNorm.normalize();

    FloatPoint3D up = listenerRight.cross(listenerFrontNorm);

    float upProjection = sourceListener.dot(up);

    FloatPoint3D projectedSource = sourceListener - upProjection * up;

    azimuth = rad2deg(projectedSource.angleBetween(listenerRight));
    fixNANs(azimuth); // avoid illegal values

    // Source  in front or behind the listener
    double frontBack = projectedSource.dot(listenerFrontNorm);
    if (frontBack < 0.0)
        azimuth = 360.0 - azimuth;

    // Make azimuth relative to "front" and not "right" listener vector
    if ((azimuth >= 0.0) && (azimuth <= 270.0))
        azimuth = 90.0 - azimuth;
    else
        azimuth = 450.0 - azimuth;

    // Elevation
    double elevation = 90 - rad2deg(sourceListener.angleBetween(up));
    fixNANs(elevation); // avoid illegal values

    if (elevation > 90.0)
        elevation = 180.0 - elevation;
    else if (elevation < -90.0)
        elevation = -180.0 - elevation;

    if (outAzimuth)
        *outAzimuth = azimuth;
    if (outElevation)
        *outElevation = elevation;
}

double PannerHandler::calculateDopplerRate()
{
    double dopplerShift = 1.0;
    double dopplerFactor = listener()->dopplerFactor();

    if (dopplerFactor > 0.0) {
        double speedOfSound = listener()->speedOfSound();

        const FloatPoint3D& sourceVelocity = m_velocity;
        const FloatPoint3D& listenerVelocity = listener()->velocity();

        // Don't bother if both source and listener have no velocity
        bool sourceHasVelocity = !sourceVelocity.isZero();
        bool listenerHasVelocity = !listenerVelocity.isZero();

        if (sourceHasVelocity || listenerHasVelocity) {
            // Calculate the source to listener vector
            FloatPoint3D listenerPosition = listener()->position();
            FloatPoint3D sourceToListener = m_position - listenerPosition;

            double sourceListenerMagnitude = sourceToListener.length();

            if (!sourceListenerMagnitude) {
                // Source and listener are at the same position. Skip the computation of the doppler
                // shift, and just return the cached value.
                dopplerShift = m_cachedDopplerRate;
            } else {
                double listenerProjection = sourceToListener.dot(listenerVelocity) / sourceListenerMagnitude;
                double sourceProjection = sourceToListener.dot(sourceVelocity) / sourceListenerMagnitude;

                listenerProjection = -listenerProjection;
                sourceProjection = -sourceProjection;

                double scaledSpeedOfSound = speedOfSound / dopplerFactor;
                listenerProjection = std::min(listenerProjection, scaledSpeedOfSound);
                sourceProjection = std::min(sourceProjection, scaledSpeedOfSound);

                dopplerShift = ((speedOfSound - dopplerFactor * listenerProjection) / (speedOfSound - dopplerFactor * sourceProjection));
                fixNANs(dopplerShift); // avoid illegal values

                // Limit the pitch shifting to 4 octaves up and 3 octaves down.
                if (dopplerShift > 16.0)
                    dopplerShift = 16.0;
                else if (dopplerShift < 0.125)
                    dopplerShift = 0.125;
            }
        }
    }

    return dopplerShift;
}

float PannerHandler::calculateDistanceConeGain()
{
    FloatPoint3D listenerPosition = listener()->position();

    double listenerDistance = m_position.distanceTo(listenerPosition);
    double distanceGain = m_distanceEffect.gain(listenerDistance);
    double coneGain = m_coneEffect.gain(m_position, m_orientation, listenerPosition);

    return float(distanceGain * coneGain);
}

void PannerHandler::azimuthElevation(double* outAzimuth, double* outElevation)
{
    ASSERT(context()->isAudioThread());

    if (isAzimuthElevationDirty()) {
        calculateAzimuthElevation(&m_cachedAzimuth, &m_cachedElevation);
        m_isAzimuthElevationDirty = false;
    }

    *outAzimuth = m_cachedAzimuth;
    *outElevation = m_cachedElevation;
}

double PannerHandler::dopplerRate()
{
    ASSERT(context()->isAudioThread());

    if (isDopplerRateDirty()) {
        m_cachedDopplerRate = calculateDopplerRate();
        m_isDopplerRateDirty = false;
    }

    return m_cachedDopplerRate;
}

float PannerHandler::distanceConeGain()
{
    ASSERT(context()->isAudioThread());

    if (isDistanceConeGainDirty()) {
        m_cachedDistanceConeGain = calculateDistanceConeGain();
        m_isDistanceConeGainDirty = false;
    }

    return m_cachedDistanceConeGain;
}

void PannerHandler::markPannerAsDirty(unsigned dirty)
{
    if (dirty & PannerHandler::AzimuthElevationDirty)
        m_isAzimuthElevationDirty = true;

    if (dirty & PannerHandler::DistanceConeGainDirty)
        m_isDistanceConeGainDirty = true;

    if (dirty & PannerHandler::DopplerRateDirty)
        m_isDopplerRateDirty = true;
}

void PannerHandler::setChannelCount(unsigned long channelCount, ExceptionState& exceptionState)
{
    ASSERT(isMainThread());
    AudioContext::AutoLocker locker(context());

    // A PannerNode only supports 1 or 2 channels
    if (channelCount > 0 && channelCount <= 2) {
        if (m_channelCount != channelCount) {
            m_channelCount = channelCount;
            if (m_channelCountMode != Max)
                updateChannelsForInputs();
        }
    } else {
        exceptionState.throwDOMException(
            NotSupportedError,
            ExceptionMessages::indexOutsideRange<unsigned long>(
                "channelCount",
                channelCount,
                1,
                ExceptionMessages::InclusiveBound,
                2,
                ExceptionMessages::InclusiveBound));
    }
}

void PannerHandler::setChannelCountMode(const String& mode, ExceptionState& exceptionState)
{
    ASSERT(isMainThread());
    AudioContext::AutoLocker locker(context());

    ChannelCountMode oldMode = m_channelCountMode;

    if (mode == "clamped-max") {
        m_newChannelCountMode = ClampedMax;
    } else if (mode == "explicit") {
        m_newChannelCountMode = Explicit;
    } else if (mode == "max") {
        // This is not supported for a PannerNode, which can only handle 1 or 2 channels.
        exceptionState.throwDOMException(
            NotSupportedError,
                "Panner: 'max' is not allowed");
        m_newChannelCountMode = oldMode;
    } else {
        // Do nothing for other invalid values.
        m_newChannelCountMode = oldMode;
    }

    if (m_newChannelCountMode != oldMode)
        context()->deferredTaskHandler().addChangedChannelCountMode(this);
}

// ----------------------------------------------------------------

PannerNode::PannerNode(AudioContext& context, float sampelRate)
    : AudioNode(context)
{
    setHandler(PannerHandler::create(*this, sampelRate));
}

PannerNode* PannerNode::create(AudioContext& context, float sampleRate)
{
    return new PannerNode(context, sampleRate);
}

PannerHandler& PannerNode::pannerHandler() const
{
    return static_cast<PannerHandler&>(handler());
}

String PannerNode::panningModel() const
{
    return pannerHandler().panningModel();
}

void PannerNode::setPanningModel(const String& model)
{
    pannerHandler().setPanningModel(model);
}

void PannerNode::setPosition(float x, float y, float z)
{
    pannerHandler().setPosition(x, y, z);
}

void PannerNode::setOrientation(float x, float y, float z)
{
    pannerHandler().setOrientation(x, y, z);
}

void PannerNode::setVelocity(float x, float y, float z)
{
    pannerHandler().setVelocity(x, y, z);
}

String PannerNode::distanceModel() const
{
    return pannerHandler().distanceModel();
}

void PannerNode::setDistanceModel(const String& model)
{
    pannerHandler().setDistanceModel(model);
}

double PannerNode::refDistance() const
{
    return pannerHandler().refDistance();
}

void PannerNode::setRefDistance(double distance)
{
    pannerHandler().setRefDistance(distance);
}

double PannerNode::maxDistance() const
{
    return pannerHandler().maxDistance();
}

void PannerNode::setMaxDistance(double distance)
{
    pannerHandler().setMaxDistance(distance);
}

double PannerNode::rolloffFactor() const
{
    return pannerHandler().rolloffFactor();
}

void PannerNode::setRolloffFactor(double factor)
{
    pannerHandler().setRolloffFactor(factor);
}

double PannerNode::coneInnerAngle() const
{
    return pannerHandler().coneInnerAngle();
}

void PannerNode::setConeInnerAngle(double angle)
{
    pannerHandler().setConeInnerAngle(angle);
}

double PannerNode::coneOuterAngle() const
{
    return pannerHandler().coneOuterAngle();
}

void PannerNode::setConeOuterAngle(double angle)
{
    pannerHandler().setConeOuterAngle(angle);
}

double PannerNode::coneOuterGain() const
{
    return pannerHandler().coneOuterGain();
}

void PannerNode::setConeOuterGain(double gain)
{
    pannerHandler().setConeOuterGain(gain);
}

} // namespace blink

#endif // ENABLE(WEB_AUDIO)
