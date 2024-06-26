// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_FORMATS_MP2T_TS_SECTION_PMT_H_
#define MEDIA_FORMATS_MP2T_TS_SECTION_PMT_H_

#include "base/callback.h"
#include "base/compiler_specific.h"
#include "media/formats/mp2t/ts_section_psi.h"

namespace media {
namespace mp2t {

    class TsSectionPmt : public TsSectionPsi {
    public:
        // RegisterPesCb::Run(int pes_pid, int stream_type);
        // Stream type is defined in
        // "Table 2-34 – Stream type assignments" in H.222
        // TODO(damienv): add the program number.
        typedef base::Callback<void(int, int)> RegisterPesCb;

        explicit TsSectionPmt(const RegisterPesCb& register_pes_cb);
        ~TsSectionPmt() override;

        // Mpeg2TsPsiParser implementation.
        bool ParsePsiSection(BitReader* bit_reader) override;
        void ResetPsiSection() override;

    private:
        RegisterPesCb register_pes_cb_;

        DISALLOW_COPY_AND_ASSIGN(TsSectionPmt);
    };

} // namespace mp2t
} // namespace media

#endif
