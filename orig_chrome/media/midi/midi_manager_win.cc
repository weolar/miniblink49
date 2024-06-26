// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/midi/midi_manager_win.h"

#include <ks.h>
#include <ksmedia.h>
#include <mmreg.h>
#include <windows.h>
// Prevent unnecessary functions from being included from <mmsystem.h>
#define MMNODRV
#define MMNOSOUND
#define MMNOWAVE
#define MMNOAUX
#define MMNOMIXER
#define MMNOTIMER
#define MMNOJOY
#define MMNOMCI
#define MMNOMMIO
#include <mmsystem.h>

#include <algorithm>
#include <functional>
#include <queue>
#include <string>

#include "base/bind.h"
#include "base/containers/hash_tables.h"
#include "base/macros.h"
#include "base/message_loop/message_loop.h"
#include "base/strings/string16.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_piece.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "base/system_monitor/system_monitor.h"
#include "base/threading/thread_checker.h"
#include "base/timer/timer.h"
#include "base/win/message_window.h"
#include "device/usb/usb_ids.h"
#include "media/midi/midi_message_queue.h"
#include "media/midi/midi_message_util.h"
#include "media/midi/midi_port_info.h"

namespace media {
namespace midi {
    namespace {

        static const size_t kBufferLength = 32 * 1024;

        // We assume that nullpter represents an invalid MIDI handle.
        const HMIDIIN kInvalidMidiInHandle = nullptr;
        const HMIDIOUT kInvalidMidiOutHandle = nullptr;

        std::string GetInErrorMessage(MMRESULT result)
        {
            wchar_t text[MAXERRORLENGTH];
            MMRESULT get_result = midiInGetErrorText(result, text, arraysize(text));
            if (get_result != MMSYSERR_NOERROR) {
                DLOG(ERROR) << "Failed to get error message."
                            << " original error: " << result
                            << " midiInGetErrorText error: " << get_result;
                return std::string();
            }
            return base::WideToUTF8(text);
        }

        std::string GetOutErrorMessage(MMRESULT result)
        {
            wchar_t text[MAXERRORLENGTH];
            MMRESULT get_result = midiOutGetErrorText(result, text, arraysize(text));
            if (get_result != MMSYSERR_NOERROR) {
                DLOG(ERROR) << "Failed to get error message."
                            << " original error: " << result
                            << " midiOutGetErrorText error: " << get_result;
                return std::string();
            }
            return base::WideToUTF8(text);
        }

        std::string MmversionToString(MMVERSION version)
        {
            return base::StringPrintf("%d.%d", HIBYTE(version), LOBYTE(version));
        }

        class MIDIHDRDeleter {
        public:
            void operator()(MIDIHDR* header)
            {
                if (!header)
                    return;
                delete[] static_cast<char*>(header->lpData);
                header->lpData = NULL;
                header->dwBufferLength = 0;
                delete header;
            }
        };

        typedef scoped_ptr<MIDIHDR, MIDIHDRDeleter> ScopedMIDIHDR;

        ScopedMIDIHDR CreateMIDIHDR(size_t size)
        {
            ScopedMIDIHDR header(new MIDIHDR);
            ZeroMemory(header.get(), sizeof(*header));
            header->lpData = new char[size];
            header->dwBufferLength = static_cast<DWORD>(size);
            return header.Pass();
        }

        void SendShortMidiMessageInternal(HMIDIOUT midi_out_handle,
            const std::vector<uint8>& message)
        {
            DCHECK_LE(message.size(), static_cast<size_t>(3))
                << "A short MIDI message should be up to 3 bytes.";

            DWORD packed_message = 0;
            for (size_t i = 0; i < message.size(); ++i)
                packed_message |= (static_cast<uint32>(message[i]) << (i * 8));
            MMRESULT result = midiOutShortMsg(midi_out_handle, packed_message);
            DLOG_IF(ERROR, result != MMSYSERR_NOERROR)
                << "Failed to output short message: " << GetOutErrorMessage(result);
        }

        void SendLongMidiMessageInternal(HMIDIOUT midi_out_handle,
            const std::vector<uint8>& message)
        {
            // Implementation note:
            // Sending a long MIDI message can be performed synchronously or
            // asynchronously depending on the driver. There are 2 options to support both
            // cases:
            // 1) Call midiOutLongMsg() API and wait for its completion within this
            //   function. In this approach, we can avoid memory copy by directly pointing
            //   |message| as the data buffer to be sent.
            // 2) Allocate a buffer and copy |message| to it, then call midiOutLongMsg()
            //   API. The buffer will be freed in the MOM_DONE event hander, which tells
            //   us that the task of midiOutLongMsg() API is completed.
            // Here we choose option 2) in favor of asynchronous design.

            // Note for built-in USB-MIDI driver:
            // From an observation on Windows 7/8.1 with a USB-MIDI keyboard,
            // midiOutLongMsg() will be always blocked. Sending 64 bytes or less data
            // takes roughly 300 usecs. Sending 2048 bytes or more data takes roughly
            // |message.size() / (75 * 1024)| secs in practice. Here we put 60 KB size
            // limit on SysEx message, with hoping that midiOutLongMsg will be blocked at
            // most 1 sec or so with a typical USB-MIDI device.
            const size_t kSysExSizeLimit = 60 * 1024;
            if (message.size() >= kSysExSizeLimit) {
                DVLOG(1) << "Ingnoreing SysEx message due to the size limit"
                         << ", size = " << message.size();
                return;
            }

            ScopedMIDIHDR midi_header(CreateMIDIHDR(message.size()));
            std::copy(message.begin(), message.end(), midi_header->lpData);

            MMRESULT result = midiOutPrepareHeader(midi_out_handle, midi_header.get(),
                sizeof(*midi_header));
            if (result != MMSYSERR_NOERROR) {
                DLOG(ERROR) << "Failed to prepare output buffer: "
                            << GetOutErrorMessage(result);
                return;
            }

            result = midiOutLongMsg(midi_out_handle, midi_header.get(), sizeof(*midi_header));
            if (result != MMSYSERR_NOERROR) {
                DLOG(ERROR) << "Failed to output long message: "
                            << GetOutErrorMessage(result);
                result = midiOutUnprepareHeader(midi_out_handle, midi_header.get(),
                    sizeof(*midi_header));
                DLOG_IF(ERROR, result != MMSYSERR_NOERROR)
                    << "Failed to uninitialize output buffer: "
                    << GetOutErrorMessage(result);
                return;
            }

            // The ownership of |midi_header| is moved to MOM_DONE event handler.
            ignore_result(midi_header.release());
        }

        template <size_t array_size>
        base::string16 AsString16(const wchar_t (&buffer)[array_size])
        {
            size_t len = 0;
            for (len = 0; len < array_size; ++len) {
                if (buffer[len] == L'\0')
                    break;
            }
            return base::string16(buffer, len);
        }

        struct MidiDeviceInfo final {
            explicit MidiDeviceInfo(const MIDIINCAPS2W& caps)
                : manufacturer_id(caps.wMid)
                , product_id(caps.wPid)
                , driver_version(caps.vDriverVersion)
                , product_name(AsString16(caps.szPname))
                , usb_vendor_id(ExtractUsbVendorIdIfExists(caps))
                , usb_product_id(ExtractUsbProductIdIfExists(caps))
                , is_usb_device(IsUsbDevice(caps))
                , is_software_synth(false)
            {
            }
            explicit MidiDeviceInfo(const MIDIOUTCAPS2W& caps)
                : manufacturer_id(caps.wMid)
                , product_id(caps.wPid)
                , driver_version(caps.vDriverVersion)
                , product_name(AsString16(caps.szPname))
                , usb_vendor_id(ExtractUsbVendorIdIfExists(caps))
                , usb_product_id(ExtractUsbProductIdIfExists(caps))
                , is_usb_device(IsUsbDevice(caps))
                , is_software_synth(IsSoftwareSynth(caps))
            {
            }
            explicit MidiDeviceInfo(const MidiDeviceInfo& info)
                : manufacturer_id(info.manufacturer_id)
                , product_id(info.product_id)
                , driver_version(info.driver_version)
                , product_name(info.product_name)
                , usb_vendor_id(info.usb_vendor_id)
                , usb_product_id(info.usb_product_id)
                , is_usb_device(info.is_usb_device)
                , is_software_synth(info.is_software_synth)
            {
            }
            // Currently only following entities are considered when testing the equality
            // of two MIDI devices.
            // TODO(toyoshim): Consider to calculate MIDIPort.id here and use it as the
            // key. See crbug.com/467448.  Then optimize the data for |MidiPortInfo|.
            const uint16 manufacturer_id;
            const uint16 product_id;
            const uint32 driver_version;
            const base::string16 product_name;
            const uint16 usb_vendor_id;
            const uint16 usb_product_id;
            const bool is_usb_device;
            const bool is_software_synth;

            // Required to be used as the key of base::hash_map.
            bool operator==(const MidiDeviceInfo& that) const
            {
                return manufacturer_id == that.manufacturer_id && product_id == that.product_id && driver_version == that.driver_version && product_name == that.product_name && is_usb_device == that.is_usb_device && (is_usb_device && usb_vendor_id == that.usb_vendor_id && usb_product_id == that.usb_product_id);
            }

            // Hash function to be used in base::hash_map.
            struct Hasher {
                size_t operator()(const MidiDeviceInfo& info) const
                {
                    size_t hash = info.manufacturer_id;
                    hash *= 131;
                    hash += info.product_id;
                    hash *= 131;
                    hash += info.driver_version;
                    hash *= 131;
                    hash += info.product_name.size();
                    hash *= 131;
                    if (!info.product_name.empty()) {
                        hash += info.product_name[0];
                    }
                    hash *= 131;
                    hash += info.usb_vendor_id;
                    hash *= 131;
                    hash += info.usb_product_id;
                    return hash;
                }
            };

        private:
            static bool IsUsbDevice(const MIDIINCAPS2W& caps)
            {
                return IS_COMPATIBLE_USBAUDIO_MID(&caps.ManufacturerGuid) && IS_COMPATIBLE_USBAUDIO_PID(&caps.ProductGuid);
            }
            static bool IsUsbDevice(const MIDIOUTCAPS2W& caps)
            {
                return IS_COMPATIBLE_USBAUDIO_MID(&caps.ManufacturerGuid) && IS_COMPATIBLE_USBAUDIO_PID(&caps.ProductGuid);
            }
            static bool IsSoftwareSynth(const MIDIOUTCAPS2W& caps)
            {
                return caps.wTechnology == MOD_SWSYNTH;
            }
            static uint16 ExtractUsbVendorIdIfExists(const MIDIINCAPS2W& caps)
            {
                if (!IS_COMPATIBLE_USBAUDIO_MID(&caps.ManufacturerGuid))
                    return 0;
                return EXTRACT_USBAUDIO_MID(&caps.ManufacturerGuid);
            }
            static uint16 ExtractUsbVendorIdIfExists(const MIDIOUTCAPS2W& caps)
            {
                if (!IS_COMPATIBLE_USBAUDIO_MID(&caps.ManufacturerGuid))
                    return 0;
                return EXTRACT_USBAUDIO_MID(&caps.ManufacturerGuid);
            }
            static uint16 ExtractUsbProductIdIfExists(const MIDIINCAPS2W& caps)
            {
                if (!IS_COMPATIBLE_USBAUDIO_PID(&caps.ProductGuid))
                    return 0;
                return EXTRACT_USBAUDIO_PID(&caps.ProductGuid);
            }
            static uint16 ExtractUsbProductIdIfExists(const MIDIOUTCAPS2W& caps)
            {
                if (!IS_COMPATIBLE_USBAUDIO_PID(&caps.ProductGuid))
                    return 0;
                return EXTRACT_USBAUDIO_PID(&caps.ProductGuid);
            }
        };

        std::string GetManufacturerName(const MidiDeviceInfo& info)
        {
            if (info.is_usb_device) {
                const char* name = device::UsbIds::GetVendorName(info.usb_vendor_id);
                return std::string(name ? name : "");
            }

            switch (info.manufacturer_id) {
            case MM_MICROSOFT:
                return "Microsoft Corporation";
            default:
                // TODO(toyoshim): Support other manufacture IDs.  crbug.com/472341.
                return "";
            }
        }

        bool IsUnsupportedDevice(const MidiDeviceInfo& info)
        {
            return info.is_software_synth && info.manufacturer_id == MM_MICROSOFT && (info.product_id == MM_MSFT_WDMAUDIO_MIDIOUT || info.product_id == MM_MSFT_GENERIC_MIDISYNTH);
        }

        using PortNumberCache = base::hash_map<
            MidiDeviceInfo,
            std::priority_queue<uint32, std::vector<uint32>, std::greater<uint32>>,
            MidiDeviceInfo::Hasher>;

        struct MidiInputDeviceState final
            : base::RefCountedThreadSafe<MidiInputDeviceState> {
            explicit MidiInputDeviceState(const MidiDeviceInfo& device_info)
                : device_info(device_info)
                , midi_handle(kInvalidMidiInHandle)
                , port_index(0)
                , port_age(0)
                , start_time_initialized(false)
            {
            }

            const MidiDeviceInfo device_info;
            HMIDIIN midi_handle;
            ScopedMIDIHDR midi_header;
            // Since Win32 multimedia system uses a relative time offset from when
            // |midiInStart| API is called, we need to record when it is called.
            base::TimeTicks start_time;
            // 0-based port index.  We will try to reuse the previous port index when the
            // MIDI device is closed then reopened.
            uint32 port_index;
            // A sequence number which represents how many times |port_index| is reused.
            // We can remove this field if we decide not to clear unsent events
            // when the device is disconnected.
            // See https://github.com/WebAudio/web-midi-api/issues/133
            uint64 port_age;
            // True if |start_time| is initialized. This field is not used so far, but
            // kept for the debugging purpose.
            bool start_time_initialized;

        private:
            friend class base::RefCountedThreadSafe<MidiInputDeviceState>;
            ~MidiInputDeviceState() { }
        };

        struct MidiOutputDeviceState final
            : base::RefCountedThreadSafe<MidiOutputDeviceState> {
            explicit MidiOutputDeviceState(const MidiDeviceInfo& device_info)
                : device_info(device_info)
                , midi_handle(kInvalidMidiOutHandle)
                , port_index(0)
                , port_age(0)
                , closed(false)
            {
            }

            const MidiDeviceInfo device_info;
            HMIDIOUT midi_handle;
            // 0-based port index.  We will try to reuse the previous port index when the
            // MIDI device is closed then reopened.
            uint32 port_index;
            // A sequence number which represents how many times |port_index| is reused.
            // We can remove this field if we decide not to clear unsent events
            // when the device is disconnected.
            // See https://github.com/WebAudio/web-midi-api/issues/133
            uint64 port_age;
            // True if the device is already closed and |midi_handle| is considered to be
            // invalid.
            // TODO(toyoshim): Use std::atomic<bool> when it is allowed in Chromium
            // project.
            volatile bool closed;

        private:
            friend class base::RefCountedThreadSafe<MidiOutputDeviceState>;
            ~MidiOutputDeviceState() { }
        };

        // The core logic of MIDI device handling for Windows. Basically this class is
        // shared among following 4 threads:
        //  1. Chrome IO Thread
        //  2. OS Multimedia Thread
        //  3. Task Thread
        //  4. Sender Thread
        //
        // Chrome IO Thread:
        //  MidiManager runs on Chrome IO thread. Device change notification is
        //  delivered to the thread through the SystemMonitor service.
        //  OnDevicesChanged() callback is invoked to update the MIDI device list.
        //  Note that in the current implementation we will try to open all the
        //  existing devices in practice. This is OK because trying to reopen a MIDI
        //  device that is already opened would simply fail, and there is no unwilling
        //  side effect.
        //
        // OS Multimedia Thread:
        //  This thread is maintained by the OS as a part of MIDI runtime, and
        //  responsible for receiving all the system initiated events such as device
        //  close, and receiving data. For performance reasons, most of potentially
        //  blocking operations will be dispatched into Task Thread.
        //
        // Task Thread:
        //  This thread will be used to call back following methods of MidiManager.
        //  - MidiManager::CompleteInitialization
        //  - MidiManager::AddInputPort
        //  - MidiManager::AddOutputPort
        //  - MidiManager::SetInputPortState
        //  - MidiManager::SetOutputPortState
        //  - MidiManager::ReceiveMidiData
        //
        // Sender Thread:
        //  This thread will be used to call Win32 APIs to send MIDI message at the
        //  specified time. We don't want to call MIDI send APIs on Task Thread
        //  because those APIs could be performed synchronously, hence they could block
        //  the caller thread for a while. See the comment in
        //  SendLongMidiMessageInternal for details. Currently we expect that the
        //  blocking time would be less than 1 second.
        class MidiServiceWinImpl : public MidiServiceWin,
                                   public base::SystemMonitor::DevicesChangedObserver {
        public:
            MidiServiceWinImpl()
                : delegate_(nullptr)
                , sender_thread_("Windows MIDI sender thread")
                , task_thread_("Windows MIDI task thread")
                , destructor_started(false)
            {
            }

            ~MidiServiceWinImpl() final
            {
                // Start() and Stop() of the threads, and AddDevicesChangeObserver() and
                // RemoveDevicesChangeObserver() should be called on the same thread.
                DCHECK(thread_checker_.CalledOnValidThread());

                destructor_started = true;
                base::SystemMonitor::Get()->RemoveDevicesChangedObserver(this);
                {
                    std::vector<HMIDIIN> input_devices;
                    {
                        base::AutoLock auto_lock(input_ports_lock_);
                        for (auto it : input_device_map_)
                            input_devices.push_back(it.first);
                    }
                    {
                        for (const auto handle : input_devices) {
                            MMRESULT result = midiInClose(handle);
                            if (result == MIDIERR_STILLPLAYING) {
                                result = midiInReset(handle);
                                DLOG_IF(ERROR, result != MMSYSERR_NOERROR)
                                    << "midiInReset failed: " << GetInErrorMessage(result);
                                result = midiInClose(handle);
                            }
                            DLOG_IF(ERROR, result != MMSYSERR_NOERROR)
                                << "midiInClose failed: " << GetInErrorMessage(result);
                        }
                    }
                }
                {
                    std::vector<HMIDIOUT> output_devices;
                    {
                        base::AutoLock auto_lock(output_ports_lock_);
                        for (auto it : output_device_map_)
                            output_devices.push_back(it.first);
                    }
                    {
                        for (const auto handle : output_devices) {
                            MMRESULT result = midiOutClose(handle);
                            if (result == MIDIERR_STILLPLAYING) {
                                result = midiOutReset(handle);
                                DLOG_IF(ERROR, result != MMSYSERR_NOERROR)
                                    << "midiOutReset failed: " << GetOutErrorMessage(result);
                                result = midiOutClose(handle);
                            }
                            DLOG_IF(ERROR, result != MMSYSERR_NOERROR)
                                << "midiOutClose failed: " << GetOutErrorMessage(result);
                        }
                    }
                }
                sender_thread_.Stop();
                task_thread_.Stop();
            }

            // MidiServiceWin overrides:
            void InitializeAsync(MidiServiceWinDelegate* delegate) final
            {
                // Start() and Stop() of the threads, and AddDevicesChangeObserver() and
                // RemoveDevicesChangeObserver() should be called on the same thread.
                DCHECK(thread_checker_.CalledOnValidThread());

                delegate_ = delegate;

                sender_thread_.Start();
                task_thread_.Start();

                // Start monitoring device changes. This should start before the
                // following UpdateDeviceList() call not to miss the event happening
                // between the call and the observer registration.
                base::SystemMonitor::Get()->AddDevicesChangedObserver(this);

                UpdateDeviceList();

                task_thread_.message_loop()->PostTask(
                    FROM_HERE,
                    base::Bind(&MidiServiceWinImpl::CompleteInitializationOnTaskThread,
                        base::Unretained(this), Result::OK));
            }

            void SendMidiDataAsync(uint32 port_number,
                const std::vector<uint8>& data,
                base::TimeTicks time) final
            {
                if (destructor_started) {
                    LOG(ERROR) << "ThreadSafeSendData failed because MidiServiceWinImpl is "
                                  "being destructed.  port: "
                               << port_number;
                    return;
                }
                auto state = GetOutputDeviceFromPort(port_number);
                if (!state) {
                    LOG(ERROR) << "ThreadSafeSendData failed due to an invalid port number. "
                               << "port: " << port_number;
                    return;
                }
                if (state->closed) {
                    LOG(ERROR)
                        << "ThreadSafeSendData failed because target port is already closed."
                        << "port: " << port_number;
                    return;
                }
                const auto now = base::TimeTicks::Now();
                if (now < time) {
                    sender_thread_.message_loop()->PostDelayedTask(
                        FROM_HERE, base::Bind(&MidiServiceWinImpl::SendOnSenderThread, base::Unretained(this), port_number, state->port_age, data, time),
                        time - now);
                } else {
                    sender_thread_.message_loop()->PostTask(
                        FROM_HERE, base::Bind(&MidiServiceWinImpl::SendOnSenderThread, base::Unretained(this), port_number, state->port_age, data, time));
                }
            }

            // base::SystemMonitor::DevicesChangedObserver overrides:
            void OnDevicesChanged(base::SystemMonitor::DeviceType device_type) final
            {
                DCHECK(thread_checker_.CalledOnValidThread());
                if (destructor_started)
                    return;

                switch (device_type) {
                case base::SystemMonitor::DEVTYPE_AUDIO_CAPTURE:
                case base::SystemMonitor::DEVTYPE_VIDEO_CAPTURE:
                    // Add case of other unrelated device types here.
                    return;
                case base::SystemMonitor::DEVTYPE_UNKNOWN:
                    // Interested in MIDI devices. Try updating the device list.
                    UpdateDeviceList();
                    break;
                    // No default here to capture new DeviceType by compile time.
                }
            }

        private:
            scoped_refptr<MidiInputDeviceState> GetInputDeviceFromHandle(
                HMIDIIN midi_handle)
            {
                base::AutoLock auto_lock(input_ports_lock_);
                const auto it = input_device_map_.find(midi_handle);
                return (it != input_device_map_.end() ? it->second : nullptr);
            }

            scoped_refptr<MidiOutputDeviceState> GetOutputDeviceFromHandle(
                HMIDIOUT midi_handle)
            {
                base::AutoLock auto_lock(output_ports_lock_);
                const auto it = output_device_map_.find(midi_handle);
                return (it != output_device_map_.end() ? it->second : nullptr);
            }

            scoped_refptr<MidiOutputDeviceState> GetOutputDeviceFromPort(
                uint32 port_number)
            {
                base::AutoLock auto_lock(output_ports_lock_);
                if (output_ports_.size() <= port_number)
                    return nullptr;
                return output_ports_[port_number];
            }

            void UpdateDeviceList()
            {
                task_thread_.message_loop()->PostTask(
                    FROM_HERE, base::Bind(&MidiServiceWinImpl::UpdateDeviceListOnTaskThread, base::Unretained(this)));
            }

            /////////////////////////////////////////////////////////////////////////////
            // Callbacks on the OS multimedia thread.
            /////////////////////////////////////////////////////////////////////////////

            static void CALLBACK
            OnMidiInEventOnMainlyMultimediaThread(HMIDIIN midi_in_handle,
                UINT message,
                DWORD_PTR instance,
                DWORD_PTR param1,
                DWORD_PTR param2)
            {
                MidiServiceWinImpl* self = reinterpret_cast<MidiServiceWinImpl*>(instance);
                if (!self)
                    return;
                switch (message) {
                case MIM_OPEN:
                    self->OnMidiInOpen(midi_in_handle);
                    break;
                case MIM_DATA:
                    self->OnMidiInDataOnMultimediaThread(midi_in_handle, param1, param2);
                    break;
                case MIM_LONGDATA:
                    self->OnMidiInLongDataOnMultimediaThread(midi_in_handle, param1,
                        param2);
                    break;
                case MIM_CLOSE:
                    self->OnMidiInCloseOnMultimediaThread(midi_in_handle);
                    break;
                }
            }

            void OnMidiInOpen(HMIDIIN midi_in_handle)
            {
                UINT device_id = 0;
                MMRESULT result = midiInGetID(midi_in_handle, &device_id);
                if (result != MMSYSERR_NOERROR) {
                    DLOG(ERROR) << "midiInGetID failed: " << GetInErrorMessage(result);
                    return;
                }
                MIDIINCAPS2W caps = {};
                result = midiInGetDevCaps(device_id, reinterpret_cast<LPMIDIINCAPSW>(&caps),
                    sizeof(caps));
                if (result != MMSYSERR_NOERROR) {
                    DLOG(ERROR) << "midiInGetDevCaps failed: " << GetInErrorMessage(result);
                    return;
                }
                auto state = make_scoped_refptr(new MidiInputDeviceState(MidiDeviceInfo(caps)));
                state->midi_handle = midi_in_handle;
                state->midi_header = CreateMIDIHDR(kBufferLength);
                const auto& state_device_info = state->device_info;
                bool add_new_port = false;
                uint32 port_number = 0;
                {
                    base::AutoLock auto_lock(input_ports_lock_);
                    const auto it = unused_input_ports_.find(state_device_info);
                    if (it == unused_input_ports_.end()) {
                        port_number = static_cast<uint32>(input_ports_.size());
                        add_new_port = true;
                        input_ports_.push_back(nullptr);
                        input_ports_ages_.push_back(0);
                    } else {
                        port_number = it->second.top();
                        it->second.pop();
                        if (it->second.empty()) {
                            unused_input_ports_.erase(it);
                        }
                    }
                    input_ports_[port_number] = state;

                    input_ports_ages_[port_number] += 1;
                    input_device_map_[input_ports_[port_number]->midi_handle] = input_ports_[port_number];
                    input_ports_[port_number]->port_index = port_number;
                    input_ports_[port_number]->port_age = input_ports_ages_[port_number];
                }
                // Several initial startup tasks cannot be done in MIM_OPEN handler.
                task_thread_.message_loop()->PostTask(
                    FROM_HERE, base::Bind(&MidiServiceWinImpl::StartInputDeviceOnTaskThread, base::Unretained(this), midi_in_handle));
                if (add_new_port) {
                    const MidiPortInfo port_info(
                        // TODO(toyoshim): Use a hash ID insted crbug.com/467448
                        base::IntToString(static_cast<int>(port_number)),
                        GetManufacturerName(state_device_info),
                        base::WideToUTF8(state_device_info.product_name),
                        MmversionToString(state_device_info.driver_version),
                        MIDI_PORT_OPENED);
                    task_thread_.message_loop()->PostTask(
                        FROM_HERE, base::Bind(&MidiServiceWinImpl::AddInputPortOnTaskThread, base::Unretained(this), port_info));
                } else {
                    task_thread_.message_loop()->PostTask(
                        FROM_HERE,
                        base::Bind(&MidiServiceWinImpl::SetInputPortStateOnTaskThread,
                            base::Unretained(this), port_number,
                            MidiPortState::MIDI_PORT_CONNECTED));
                }
            }

            void OnMidiInDataOnMultimediaThread(HMIDIIN midi_in_handle,
                DWORD_PTR param1,
                DWORD_PTR param2)
            {
                auto state = GetInputDeviceFromHandle(midi_in_handle);
                if (!state)
                    return;
                const uint8 status_byte = static_cast<uint8>(param1 & 0xff);
                const uint8 first_data_byte = static_cast<uint8>((param1 >> 8) & 0xff);
                const uint8 second_data_byte = static_cast<uint8>((param1 >> 16) & 0xff);
                const DWORD elapsed_ms = param2;
                const size_t len = GetMidiMessageLength(status_byte);
                const uint8 kData[] = { status_byte, first_data_byte, second_data_byte };
                std::vector<uint8> data;
                data.assign(kData, kData + len);
                DCHECK_LE(len, arraysize(kData));
                // MIM_DATA/MIM_LONGDATA message treats the time when midiInStart() is
                // called as the origin of |elapsed_ms|.
                // http://msdn.microsoft.com/en-us/library/windows/desktop/dd757284.aspx
                // http://msdn.microsoft.com/en-us/library/windows/desktop/dd757286.aspx
                const base::TimeTicks event_time = state->start_time + base::TimeDelta::FromMilliseconds(elapsed_ms);
                task_thread_.message_loop()->PostTask(
                    FROM_HERE, base::Bind(&MidiServiceWinImpl::ReceiveMidiDataOnTaskThread, base::Unretained(this), state->port_index, data, event_time));
            }

            void OnMidiInLongDataOnMultimediaThread(HMIDIIN midi_in_handle,
                DWORD_PTR param1,
                DWORD_PTR param2)
            {
                auto state = GetInputDeviceFromHandle(midi_in_handle);
                if (!state)
                    return;
                MIDIHDR* header = reinterpret_cast<MIDIHDR*>(param1);
                const DWORD elapsed_ms = param2;
                MMRESULT result = MMSYSERR_NOERROR;
                if (destructor_started) {
                    if (state->midi_header && (state->midi_header->dwFlags & MHDR_PREPARED) == MHDR_PREPARED) {
                        result = midiInUnprepareHeader(state->midi_handle, state->midi_header.get(),
                            sizeof(*state->midi_header));
                        DLOG_IF(ERROR, result != MMSYSERR_NOERROR)
                            << "Failed to uninitialize input buffer: "
                            << GetInErrorMessage(result);
                    }
                    return;
                }
                if (header->dwBytesRecorded > 0) {
                    const uint8* src = reinterpret_cast<const uint8*>(header->lpData);
                    std::vector<uint8> data;
                    data.assign(src, src + header->dwBytesRecorded);
                    // MIM_DATA/MIM_LONGDATA message treats the time when midiInStart() is
                    // called as the origin of |elapsed_ms|.
                    // http://msdn.microsoft.com/en-us/library/windows/desktop/dd757284.aspx
                    // http://msdn.microsoft.com/en-us/library/windows/desktop/dd757286.aspx
                    const base::TimeTicks event_time = state->start_time + base::TimeDelta::FromMilliseconds(elapsed_ms);
                    task_thread_.message_loop()->PostTask(
                        FROM_HERE,
                        base::Bind(&MidiServiceWinImpl::ReceiveMidiDataOnTaskThread,
                            base::Unretained(this), state->port_index, data,
                            event_time));
                }
                result = midiInAddBuffer(state->midi_handle, header, sizeof(*header));
                DLOG_IF(ERROR, result != MMSYSERR_NOERROR)
                    << "Failed to attach input buffer: " << GetInErrorMessage(result)
                    << "port number:" << state->port_index;
            }

            void OnMidiInCloseOnMultimediaThread(HMIDIIN midi_in_handle)
            {
                auto state = GetInputDeviceFromHandle(midi_in_handle);
                if (!state)
                    return;
                const uint32 port_number = state->port_index;
                const auto device_info(state->device_info);
                {
                    base::AutoLock auto_lock(input_ports_lock_);
                    input_device_map_.erase(state->midi_handle);
                    input_ports_[port_number] = nullptr;
                    input_ports_ages_[port_number] += 1;
                    unused_input_ports_[device_info].push(port_number);
                }
                task_thread_.message_loop()->PostTask(
                    FROM_HERE,
                    base::Bind(&MidiServiceWinImpl::SetInputPortStateOnTaskThread,
                        base::Unretained(this), port_number,
                        MIDI_PORT_DISCONNECTED));
            }

            static void CALLBACK
            OnMidiOutEventOnMainlyMultimediaThread(HMIDIOUT midi_out_handle,
                UINT message,
                DWORD_PTR instance,
                DWORD_PTR param1,
                DWORD_PTR param2)
            {
                MidiServiceWinImpl* self = reinterpret_cast<MidiServiceWinImpl*>(instance);
                if (!self)
                    return;
                switch (message) {
                case MOM_OPEN:
                    self->OnMidiOutOpen(midi_out_handle, param1, param2);
                    break;
                case MOM_DONE:
                    self->OnMidiOutDoneOnMultimediaThread(midi_out_handle, param1);
                    break;
                case MOM_CLOSE:
                    self->OnMidiOutCloseOnMultimediaThread(midi_out_handle);
                    break;
                }
            }

            void OnMidiOutOpen(HMIDIOUT midi_out_handle,
                DWORD_PTR param1,
                DWORD_PTR param2)
            {
                UINT device_id = 0;
                MMRESULT result = midiOutGetID(midi_out_handle, &device_id);
                if (result != MMSYSERR_NOERROR) {
                    DLOG(ERROR) << "midiOutGetID failed: " << GetOutErrorMessage(result);
                    return;
                }
                MIDIOUTCAPS2W caps = {};
                result = midiOutGetDevCaps(
                    device_id, reinterpret_cast<LPMIDIOUTCAPSW>(&caps), sizeof(caps));
                if (result != MMSYSERR_NOERROR) {
                    DLOG(ERROR) << "midiInGetDevCaps failed: " << GetOutErrorMessage(result);
                    return;
                }
                auto state = make_scoped_refptr(new MidiOutputDeviceState(MidiDeviceInfo(caps)));
                state->midi_handle = midi_out_handle;
                const auto& state_device_info = state->device_info;
                if (IsUnsupportedDevice(state_device_info))
                    return;
                bool add_new_port = false;
                uint32 port_number = 0;
                {
                    base::AutoLock auto_lock(output_ports_lock_);
                    const auto it = unused_output_ports_.find(state_device_info);
                    if (it == unused_output_ports_.end()) {
                        port_number = static_cast<uint32>(output_ports_.size());
                        add_new_port = true;
                        output_ports_.push_back(nullptr);
                        output_ports_ages_.push_back(0);
                    } else {
                        port_number = it->second.top();
                        it->second.pop();
                        if (it->second.empty())
                            unused_output_ports_.erase(it);
                    }
                    output_ports_[port_number] = state;
                    output_ports_ages_[port_number] += 1;
                    output_device_map_[output_ports_[port_number]->midi_handle] = output_ports_[port_number];
                    output_ports_[port_number]->port_index = port_number;
                    output_ports_[port_number]->port_age = output_ports_ages_[port_number];
                }
                if (add_new_port) {
                    const MidiPortInfo port_info(
                        // TODO(toyoshim): Use a hash ID insted. crbug.com/467448
                        base::IntToString(static_cast<int>(port_number)),
                        GetManufacturerName(state_device_info),
                        base::WideToUTF8(state_device_info.product_name),
                        MmversionToString(state_device_info.driver_version),
                        MIDI_PORT_OPENED);
                    task_thread_.message_loop()->PostTask(
                        FROM_HERE, base::Bind(&MidiServiceWinImpl::AddOutputPortOnTaskThread, base::Unretained(this), port_info));
                } else {
                    task_thread_.message_loop()->PostTask(
                        FROM_HERE,
                        base::Bind(&MidiServiceWinImpl::SetOutputPortStateOnTaskThread,
                            base::Unretained(this), port_number, MIDI_PORT_CONNECTED));
                }
            }

            void OnMidiOutDoneOnMultimediaThread(HMIDIOUT midi_out_handle,
                DWORD_PTR param1)
            {
                auto state = GetOutputDeviceFromHandle(midi_out_handle);
                if (!state)
                    return;
                // Take ownership of the MIDIHDR object.
                ScopedMIDIHDR header(reinterpret_cast<MIDIHDR*>(param1));
                if (!header)
                    return;
                MMRESULT result = midiOutUnprepareHeader(state->midi_handle, header.get(),
                    sizeof(*header));
                DLOG_IF(ERROR, result != MMSYSERR_NOERROR)
                    << "Failed to uninitialize output buffer: "
                    << GetOutErrorMessage(result);
            }

            void OnMidiOutCloseOnMultimediaThread(HMIDIOUT midi_out_handle)
            {
                auto state = GetOutputDeviceFromHandle(midi_out_handle);
                if (!state)
                    return;
                const uint32 port_number = state->port_index;
                const auto device_info(state->device_info);
                {
                    base::AutoLock auto_lock(output_ports_lock_);
                    output_device_map_.erase(state->midi_handle);
                    output_ports_[port_number] = nullptr;
                    output_ports_ages_[port_number] += 1;
                    unused_output_ports_[device_info].push(port_number);
                    state->closed = true;
                }
                task_thread_.message_loop()->PostTask(
                    FROM_HERE,
                    base::Bind(&MidiServiceWinImpl::SetOutputPortStateOnTaskThread,
                        base::Unretained(this), port_number,
                        MIDI_PORT_DISCONNECTED));
            }

            /////////////////////////////////////////////////////////////////////////////
            // Callbacks on the sender thread.
            /////////////////////////////////////////////////////////////////////////////

            void AssertOnSenderThread()
            {
                DCHECK_EQ(sender_thread_.GetThreadId(), base::PlatformThread::CurrentId());
            }

            void SendOnSenderThread(uint32 port_number,
                uint64 port_age,
                const std::vector<uint8>& data,
                base::TimeTicks time)
            {
                AssertOnSenderThread();
                if (destructor_started) {
                    LOG(ERROR) << "ThreadSafeSendData failed because MidiServiceWinImpl is "
                                  "being destructed. port: "
                               << port_number;
                }
                auto state = GetOutputDeviceFromPort(port_number);
                if (!state) {
                    LOG(ERROR) << "ThreadSafeSendData failed due to an invalid port number. "
                               << "port: " << port_number;
                    return;
                }
                if (state->closed) {
                    LOG(ERROR)
                        << "ThreadSafeSendData failed because target port is already closed."
                        << "port: " << port_number;
                    return;
                }
                if (state->port_age != port_age) {
                    LOG(ERROR)
                        << "ThreadSafeSendData failed because target port is being closed."
                        << "port: " << port_number << "expected port age: " << port_age
                        << "actual port age: " << state->port_age;
                }

                // MIDI Running status must be filtered out.
                MidiMessageQueue message_queue(false);
                message_queue.Add(data);
                std::vector<uint8> message;
                while (true) {
                    if (destructor_started)
                        break;
                    if (state->closed)
                        break;
                    message_queue.Get(&message);
                    if (message.empty())
                        break;
                    // SendShortMidiMessageInternal can send a MIDI message up to 3 bytes.
                    if (message.size() <= 3)
                        SendShortMidiMessageInternal(state->midi_handle, message);
                    else
                        SendLongMidiMessageInternal(state->midi_handle, message);
                }
            }

            /////////////////////////////////////////////////////////////////////////////
            // Callbacks on the task thread.
            /////////////////////////////////////////////////////////////////////////////

            void AssertOnTaskThread()
            {
                DCHECK_EQ(task_thread_.GetThreadId(), base::PlatformThread::CurrentId());
            }

            void UpdateDeviceListOnTaskThread()
            {
                AssertOnTaskThread();
                const UINT num_in_devices = midiInGetNumDevs();
                for (UINT device_id = 0; device_id < num_in_devices; ++device_id) {
                    // Here we use |CALLBACK_FUNCTION| to subscribe MIM_DATA, MIM_LONGDATA,
                    // MIM_OPEN, and MIM_CLOSE events.
                    // - MIM_DATA: This is the only way to get a short MIDI message with
                    //     timestamp information.
                    // - MIM_LONGDATA: This is the only way to get a long MIDI message with
                    //     timestamp information.
                    // - MIM_OPEN: This event is sent the input device is opened. Note that
                    //     this message is called on the caller thread.
                    // - MIM_CLOSE: This event is sent when 1) midiInClose() is called, or 2)
                    //     the MIDI device becomes unavailable for some reasons, e.g., the
                    //     cable is disconnected. As for the former case, HMIDIOUT will be
                    //     invalidated soon after the callback is finished. As for the later
                    //     case, however, HMIDIOUT continues to be valid until midiInClose()
                    //     is called.
                    HMIDIIN midi_handle = kInvalidMidiInHandle;
                    const MMRESULT result = midiInOpen(
                        &midi_handle, device_id,
                        reinterpret_cast<DWORD_PTR>(&OnMidiInEventOnMainlyMultimediaThread),
                        reinterpret_cast<DWORD_PTR>(this), CALLBACK_FUNCTION);
                    DLOG_IF(ERROR, result != MMSYSERR_NOERROR && result != MMSYSERR_ALLOCATED)
                        << "Failed to open output device. "
                        << " id: " << device_id << " message: " << GetInErrorMessage(result);
                }

                const UINT num_out_devices = midiOutGetNumDevs();
                for (UINT device_id = 0; device_id < num_out_devices; ++device_id) {
                    // Here we use |CALLBACK_FUNCTION| to subscribe MOM_DONE, MOM_OPEN, and
                    // MOM_CLOSE events.
                    // - MOM_DONE: SendLongMidiMessageInternal() relies on this event to clean
                    //     up the backing store where a long MIDI message is stored.
                    // - MOM_OPEN: This event is sent the output device is opened. Note that
                    //     this message is called on the caller thread.
                    // - MOM_CLOSE: This event is sent when 1) midiOutClose() is called, or 2)
                    //     the MIDI device becomes unavailable for some reasons, e.g., the
                    //     cable is disconnected. As for the former case, HMIDIOUT will be
                    //     invalidated soon after the callback is finished. As for the later
                    //     case, however, HMIDIOUT continues to be valid until midiOutClose()
                    //     is called.
                    HMIDIOUT midi_handle = kInvalidMidiOutHandle;
                    const MMRESULT result = midiOutOpen(
                        &midi_handle, device_id,
                        reinterpret_cast<DWORD_PTR>(&OnMidiOutEventOnMainlyMultimediaThread),
                        reinterpret_cast<DWORD_PTR>(this), CALLBACK_FUNCTION);
                    DLOG_IF(ERROR, result != MMSYSERR_NOERROR && result != MMSYSERR_ALLOCATED)
                        << "Failed to open output device. "
                        << " id: " << device_id << " message: " << GetOutErrorMessage(result);
                }
            }

            void StartInputDeviceOnTaskThread(HMIDIIN midi_in_handle)
            {
                AssertOnTaskThread();
                auto state = GetInputDeviceFromHandle(midi_in_handle);
                if (!state)
                    return;
                MMRESULT result = midiInPrepareHeader(state->midi_handle, state->midi_header.get(),
                    sizeof(*state->midi_header));
                if (result != MMSYSERR_NOERROR) {
                    DLOG(ERROR) << "Failed to initialize input buffer: "
                                << GetInErrorMessage(result);
                    return;
                }
                result = midiInAddBuffer(state->midi_handle, state->midi_header.get(),
                    sizeof(*state->midi_header));
                if (result != MMSYSERR_NOERROR) {
                    DLOG(ERROR) << "Failed to attach input buffer: "
                                << GetInErrorMessage(result);
                    return;
                }
                result = midiInStart(state->midi_handle);
                if (result != MMSYSERR_NOERROR) {
                    DLOG(ERROR) << "Failed to start input port: "
                                << GetInErrorMessage(result);
                    return;
                }
                state->start_time = base::TimeTicks::Now();
                state->start_time_initialized = true;
            }

            void CompleteInitializationOnTaskThread(Result result)
            {
                AssertOnTaskThread();
                delegate_->OnCompleteInitialization(result);
            }

            void ReceiveMidiDataOnTaskThread(uint32 port_index,
                std::vector<uint8> data,
                base::TimeTicks time)
            {
                AssertOnTaskThread();
                delegate_->OnReceiveMidiData(port_index, data, time);
            }

            void AddInputPortOnTaskThread(MidiPortInfo info)
            {
                AssertOnTaskThread();
                delegate_->OnAddInputPort(info);
            }

            void AddOutputPortOnTaskThread(MidiPortInfo info)
            {
                AssertOnTaskThread();
                delegate_->OnAddOutputPort(info);
            }

            void SetInputPortStateOnTaskThread(uint32 port_index, MidiPortState state)
            {
                AssertOnTaskThread();
                delegate_->OnSetInputPortState(port_index, state);
            }

            void SetOutputPortStateOnTaskThread(uint32 port_index, MidiPortState state)
            {
                AssertOnTaskThread();
                delegate_->OnSetOutputPortState(port_index, state);
            }

            /////////////////////////////////////////////////////////////////////////////
            // Fields:
            /////////////////////////////////////////////////////////////////////////////

            // Does not take ownership.
            MidiServiceWinDelegate* delegate_;

            base::ThreadChecker thread_checker_;

            base::Thread sender_thread_;
            base::Thread task_thread_;

            base::Lock input_ports_lock_;
            base::hash_map<HMIDIIN, scoped_refptr<MidiInputDeviceState>>
                input_device_map_; // GUARDED_BY(input_ports_lock_)
            PortNumberCache unused_input_ports_; // GUARDED_BY(input_ports_lock_)
            std::vector<scoped_refptr<MidiInputDeviceState>>
                input_ports_; // GUARDED_BY(input_ports_lock_)
            std::vector<uint64> input_ports_ages_; // GUARDED_BY(input_ports_lock_)

            base::Lock output_ports_lock_;
            base::hash_map<HMIDIOUT, scoped_refptr<MidiOutputDeviceState>>
                output_device_map_; // GUARDED_BY(output_ports_lock_)
            PortNumberCache unused_output_ports_; // GUARDED_BY(output_ports_lock_)
            std::vector<scoped_refptr<MidiOutputDeviceState>>
                output_ports_; // GUARDED_BY(output_ports_lock_)
            std::vector<uint64> output_ports_ages_; // GUARDED_BY(output_ports_lock_)

            // True if one thread reached MidiServiceWinImpl::~MidiServiceWinImpl(). Note
            // that MidiServiceWinImpl::~MidiServiceWinImpl() is blocked until
            // |sender_thread_|, and |task_thread_| are stopped.
            // This flag can be used as the signal that when background tasks must be
            // interrupted.
            // TODO(toyoshim): Use std::atomic<bool> when it is allowed.
            volatile bool destructor_started;

            DISALLOW_COPY_AND_ASSIGN(MidiServiceWinImpl);
        };

    } // namespace

    MidiManagerWin::MidiManagerWin()
    {
    }

    MidiManagerWin::~MidiManagerWin()
    {
    }

    void MidiManagerWin::StartInitialization()
    {
        midi_service_.reset(new MidiServiceWinImpl);
        // Note that |CompleteInitialization()| will be called from the callback.
        midi_service_->InitializeAsync(this);
    }

    void MidiManagerWin::Finalize()
    {
        midi_service_.reset();
    }

    void MidiManagerWin::DispatchSendMidiData(MidiManagerClient* client,
        uint32 port_index,
        const std::vector<uint8>& data,
        double timestamp)
    {
        if (!midi_service_)
            return;

        base::TimeTicks time_to_send = base::TimeTicks::Now();
        if (timestamp != 0.0) {
            time_to_send = base::TimeTicks() + base::TimeDelta::FromMicroseconds(timestamp * base::Time::kMicrosecondsPerSecond);
        }
        midi_service_->SendMidiDataAsync(port_index, data, time_to_send);

        // TOOD(toyoshim): This calculation should be done when the date is actually
        // sent.
        client->AccumulateMidiBytesSent(data.size());
    }

    void MidiManagerWin::OnCompleteInitialization(Result result)
    {
        CompleteInitialization(result);
    }

    void MidiManagerWin::OnAddInputPort(MidiPortInfo info)
    {
        AddInputPort(info);
    }

    void MidiManagerWin::OnAddOutputPort(MidiPortInfo info)
    {
        AddOutputPort(info);
    }

    void MidiManagerWin::OnSetInputPortState(uint32 port_index,
        MidiPortState state)
    {
        SetInputPortState(port_index, state);
    }

    void MidiManagerWin::OnSetOutputPortState(uint32 port_index,
        MidiPortState state)
    {
        SetOutputPortState(port_index, state);
    }

    void MidiManagerWin::OnReceiveMidiData(uint32 port_index,
        const std::vector<uint8>& data,
        base::TimeTicks time)
    {
        ReceiveMidiData(port_index, &data[0], data.size(), time);
    }

    MidiManager* MidiManager::Create()
    {
        return new MidiManagerWin();
    }

} // namespace midi
} // namespace media
