// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/texture_manager.h"

#include <algorithm>
#include <set>
#include <utility>

#include "base/bits.h"
#include "base/lazy_instance.h"
#include "base/strings/stringprintf.h"
#include "base/thread_task_runner_handle.h"
//#include "base/trace_event/memory_dump_manager.h"
#include "gpu/command_buffer/common/gles2_cmd_utils.h"
#include "gpu/command_buffer/service/context_state.h"
#include "gpu/command_buffer/service/error_state.h"
#include "gpu/command_buffer/service/feature_info.h"
#include "gpu/command_buffer/service/framebuffer_manager.h"
#include "gpu/command_buffer/service/gles2_cmd_decoder.h"
#include "gpu/command_buffer/service/mailbox_manager.h"
#include "gpu/command_buffer/service/memory_tracking.h"
#include "ui/gl/gl_implementation.h"
#include "ui/gl/gl_version_info.h"
#include "ui/gl/trace_util.h"

namespace gpu {
namespace gles2 {

    namespace {

        // This should contain everything to uniquely identify a Texture.
        const char TextureTag[] = "|Texture|";
        struct TextureSignature {
            GLenum target_;
            GLint level_;
            GLenum min_filter_;
            GLenum mag_filter_;
            GLenum wrap_r_;
            GLenum wrap_s_;
            GLenum wrap_t_;
            GLenum usage_;
            GLenum internal_format_;
            GLenum compare_func_;
            GLenum compare_mode_;
            GLsizei width_;
            GLsizei height_;
            GLsizei depth_;
            GLfloat max_lod_;
            GLfloat min_lod_;
            GLint base_level_;
            GLint border_;
            GLint max_level_;
            GLenum format_;
            GLenum type_;
            bool has_image_;
            bool can_render_;
            bool can_render_to_;
            bool npot_;

            // Since we will be hashing this signature structure, the padding must be
            // zero initialized. Although the C++11 specifications specify that this is
            // true, we will use a constructor with a memset to further enforce it instead
            // of relying on compilers adhering to this deep dark corner specification.
            TextureSignature(GLenum target,
                GLint level,
                GLenum min_filter,
                GLenum mag_filter,
                GLenum wrap_r,
                GLenum wrap_s,
                GLenum wrap_t,
                GLenum usage,
                GLenum internal_format,
                GLenum compare_func,
                GLenum compare_mode,
                GLsizei width,
                GLsizei height,
                GLsizei depth,
                GLfloat max_lod,
                GLfloat min_lod,
                GLint base_level,
                GLint border,
                GLint max_level,
                GLenum format,
                GLenum type,
                bool has_image,
                bool can_render,
                bool can_render_to,
                bool npot)
            {
                memset(this, 0, sizeof(TextureSignature));
                target_ = target;
                level_ = level;
                min_filter_ = min_filter;
                mag_filter_ = mag_filter;
                wrap_r_ = wrap_r;
                wrap_s_ = wrap_s;
                wrap_t_ = wrap_t;
                usage_ = usage;
                internal_format_ = internal_format;
                compare_func_ = compare_func;
                compare_mode_ = compare_mode;
                width_ = width;
                height_ = height;
                depth_ = depth;
                max_lod_ = max_lod;
                min_lod_ = min_lod;
                base_level_ = base_level;
                border_ = border;
                max_level_ = max_level;
                format_ = format;
                type_ = type;
                has_image_ = has_image;
                can_render_ = can_render;
                can_render_to_ = can_render_to;
                npot_ = npot;
            }
        };

        class FormatTypeValidator {
        public:
            FormatTypeValidator()
            {
                static const FormatType kSupportedFormatTypes[] = {
                    // ES2.
                    { GL_RGB, GL_RGB, GL_UNSIGNED_BYTE },
                    { GL_RGB, GL_RGB, GL_UNSIGNED_SHORT_5_6_5 },
                    { GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE },
                    { GL_RGBA, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4 },
                    { GL_RGBA, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1 },
                    { GL_LUMINANCE_ALPHA, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE },
                    { GL_LUMINANCE, GL_LUMINANCE, GL_UNSIGNED_BYTE },
                    { GL_ALPHA, GL_ALPHA, GL_UNSIGNED_BYTE },
                    // Exposed by GL_OES_texture_float and GL_OES_texture_half_float
                    { GL_RGB, GL_RGB, GL_FLOAT },
                    { GL_RGBA, GL_RGBA, GL_FLOAT },
                    { GL_LUMINANCE_ALPHA, GL_LUMINANCE_ALPHA, GL_FLOAT },
                    { GL_LUMINANCE, GL_LUMINANCE, GL_FLOAT },
                    { GL_ALPHA, GL_ALPHA, GL_FLOAT },
                    { GL_RGB, GL_RGB, GL_HALF_FLOAT_OES },
                    { GL_RGBA, GL_RGBA, GL_HALF_FLOAT_OES },
                    { GL_LUMINANCE_ALPHA, GL_LUMINANCE_ALPHA, GL_HALF_FLOAT_OES },
                    { GL_LUMINANCE, GL_LUMINANCE, GL_HALF_FLOAT_OES },
                    { GL_ALPHA, GL_ALPHA, GL_HALF_FLOAT_OES },
                    // Exposed by GL_ANGLE_depth_texture
                    { GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT },
                    { GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT },
                    { GL_DEPTH_STENCIL, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8 },
                    // Exposed by GL_EXT_sRGB
                    { GL_SRGB, GL_SRGB, GL_UNSIGNED_BYTE },
                    { GL_SRGB_ALPHA, GL_SRGB_ALPHA, GL_UNSIGNED_BYTE },
                    // Exposed by GL_EXT_texture_format_BGRA8888
                    { GL_BGRA_EXT, GL_BGRA_EXT, GL_UNSIGNED_BYTE },
                    // Exposed by GL_EXT_texture_rg
                    { GL_RED, GL_RED, GL_UNSIGNED_BYTE },
                    { GL_RG, GL_RG, GL_UNSIGNED_BYTE },
                    { GL_RED, GL_RED, GL_FLOAT },
                    { GL_RG, GL_RG, GL_FLOAT },
                    { GL_RED, GL_RED, GL_HALF_FLOAT_OES },
                    { GL_RG, GL_RG, GL_HALF_FLOAT_OES },

                    // ES3.
                    { GL_R8, GL_RED, GL_UNSIGNED_BYTE },
                    { GL_R8_SNORM, GL_RED, GL_BYTE },
                    { GL_R16F, GL_RED, GL_HALF_FLOAT },
                    { GL_R16F, GL_RED, GL_FLOAT },
                    { GL_R32F, GL_RED, GL_FLOAT },
                    { GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE },
                    { GL_R8I, GL_RED_INTEGER, GL_BYTE },
                    { GL_R16UI, GL_RED_INTEGER, GL_UNSIGNED_SHORT },
                    { GL_R16I, GL_RED_INTEGER, GL_SHORT },
                    { GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT },
                    { GL_R32I, GL_RED_INTEGER, GL_INT },
                    { GL_RG8, GL_RG, GL_UNSIGNED_BYTE },
                    { GL_RG8_SNORM, GL_RG, GL_BYTE },
                    { GL_RG16F, GL_RG, GL_HALF_FLOAT },
                    { GL_RG16F, GL_RG, GL_FLOAT },
                    { GL_RG32F, GL_RG, GL_FLOAT },
                    { GL_RG8UI, GL_RG_INTEGER, GL_UNSIGNED_BYTE },
                    { GL_RG8I, GL_RG_INTEGER, GL_BYTE },
                    { GL_RG16UI, GL_RG_INTEGER, GL_UNSIGNED_SHORT },
                    { GL_RG16I, GL_RG_INTEGER, GL_SHORT },
                    { GL_RG32UI, GL_RG_INTEGER, GL_UNSIGNED_INT },
                    { GL_RG32I, GL_RG_INTEGER, GL_INT },
                    { GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE },
                    { GL_SRGB8, GL_RGB, GL_UNSIGNED_BYTE },
                    {
                        GL_RGB565,
                        GL_RGB,
                        GL_UNSIGNED_BYTE,
                    },
                    { GL_RGB565, GL_RGB, GL_UNSIGNED_SHORT_5_6_5 },
                    { GL_RGB8_SNORM, GL_RGB, GL_BYTE },
                    { GL_R11F_G11F_B10F, GL_RGB, GL_UNSIGNED_INT_10F_11F_11F_REV },
                    { GL_R11F_G11F_B10F, GL_RGB, GL_HALF_FLOAT },
                    { GL_R11F_G11F_B10F, GL_RGB, GL_FLOAT },
                    { GL_RGB9_E5, GL_RGB, GL_UNSIGNED_INT_5_9_9_9_REV },
                    { GL_RGB9_E5, GL_RGB, GL_HALF_FLOAT },
                    { GL_RGB9_E5, GL_RGB, GL_FLOAT },
                    { GL_RGB16F, GL_RGB, GL_HALF_FLOAT },
                    { GL_RGB16F, GL_RGB, GL_FLOAT },
                    { GL_RGB32F, GL_RGB, GL_FLOAT },
                    { GL_RGB8UI, GL_RGB_INTEGER, GL_UNSIGNED_BYTE },
                    { GL_RGB8I, GL_RGB_INTEGER, GL_BYTE },
                    { GL_RGB16UI, GL_RGB_INTEGER, GL_UNSIGNED_SHORT },
                    { GL_RGB16I, GL_RGB_INTEGER, GL_SHORT },
                    { GL_RGB32UI, GL_RGB_INTEGER, GL_UNSIGNED_INT },
                    { GL_RGB32I, GL_RGB_INTEGER, GL_INT },
                    { GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE },
                    { GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE },
                    { GL_RGBA8_SNORM, GL_RGBA, GL_BYTE },
                    { GL_RGB5_A1, GL_RGBA, GL_UNSIGNED_BYTE },
                    { GL_RGB5_A1, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1 },
                    { GL_RGB5_A1, GL_RGBA, GL_UNSIGNED_INT_2_10_10_10_REV },
                    { GL_RGBA4, GL_RGBA, GL_UNSIGNED_BYTE },
                    { GL_RGBA4, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4 },
                    { GL_RGB10_A2, GL_RGBA, GL_UNSIGNED_INT_2_10_10_10_REV },
                    { GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT },
                    { GL_RGBA16F, GL_RGBA, GL_FLOAT },
                    { GL_RGBA32F, GL_RGBA, GL_FLOAT },
                    { GL_RGBA8UI, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE },
                    { GL_RGBA8I, GL_RGBA_INTEGER, GL_BYTE },
                    { GL_RGB10_A2UI, GL_RGBA_INTEGER, GL_UNSIGNED_INT_2_10_10_10_REV },
                    { GL_RGBA16UI, GL_RGBA_INTEGER, GL_UNSIGNED_SHORT },
                    { GL_RGBA16I, GL_RGBA_INTEGER, GL_SHORT },
                    { GL_RGBA32I, GL_RGBA_INTEGER, GL_INT },
                    { GL_RGBA32UI, GL_RGBA_INTEGER, GL_UNSIGNED_INT },
                    { GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT },
                    { GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT },
                    { GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT },
                    { GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT },
                    { GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8 },
                    { GL_DEPTH32F_STENCIL8, GL_DEPTH_STENCIL,
                        GL_FLOAT_32_UNSIGNED_INT_24_8_REV },
                };

                for (size_t ii = 0; ii < arraysize(kSupportedFormatTypes); ++ii) {
                    supported_combinations_.insert(kSupportedFormatTypes[ii]);
                }
            }

            // This may be accessed from multiple threads.
            bool IsValid(GLenum internal_format, GLenum format, GLenum type) const
            {
                FormatType query = { internal_format, format, type };
                return supported_combinations_.find(query) != supported_combinations_.end();
            }

        private:
            // TODO(zmo): once std::tuple is allowed, switch over to that.
            struct FormatType {
                GLenum internal_format;
                GLenum format;
                GLenum type;
            };

            struct FormatTypeCompare {
                bool operator()(const FormatType& lhs, const FormatType& rhs) const
                {
                    return (lhs.internal_format < rhs.internal_format || ((lhs.internal_format == rhs.internal_format) && (lhs.format < rhs.format)) || ((lhs.internal_format == rhs.internal_format) && (lhs.format == rhs.format) && (lhs.type < rhs.type)));
                }
            };

            // This class needs to be thread safe, so once supported_combinations_
            // are initialized in the constructor, it should never be modified later.
            std::set<FormatType, FormatTypeCompare> supported_combinations_;
        };

        base::LazyInstance<const FormatTypeValidator>::Leaky g_format_type_validator = LAZY_INSTANCE_INITIALIZER;

    } // namespace anonymous

    TextureManager::DestructionObserver::DestructionObserver() { }

    TextureManager::DestructionObserver::~DestructionObserver() { }

    TextureManager::~TextureManager()
    {
        for (unsigned int i = 0; i < destruction_observers_.size(); i++)
            destruction_observers_[i]->OnTextureManagerDestroying(this);

        DCHECK(textures_.empty());

        // If this triggers, that means something is keeping a reference to
        // a Texture belonging to this.
        CHECK_EQ(texture_count_, 0u);

        DCHECK_EQ(0, num_unrenderable_textures_);
        DCHECK_EQ(0, num_unsafe_textures_);
        DCHECK_EQ(0, num_uncleared_mips_);
        DCHECK_EQ(0, num_images_);

        //   base::trace_event::MemoryDumpManager::GetInstance()->UnregisterDumpProvider(
        //       this);
    }

    void TextureManager::Destroy(bool have_context)
    {
        have_context_ = have_context;
        textures_.clear();
        for (int ii = 0; ii < kNumDefaultTextures; ++ii) {
            default_textures_[ii] = NULL;
        }

        if (have_context) {
            glDeleteTextures(arraysize(black_texture_ids_), black_texture_ids_);
        }

        DCHECK_EQ(0u, memory_type_tracker_->GetMemRepresented());
    }

    Texture::Texture(GLuint service_id)
        : mailbox_manager_(NULL)
        , memory_tracking_ref_(NULL)
        , service_id_(service_id)
        , cleared_(true)
        , num_uncleared_mips_(0)
        , num_npot_faces_(0)
        , target_(0)
        , min_filter_(GL_NEAREST_MIPMAP_LINEAR)
        , mag_filter_(GL_LINEAR)
        , wrap_r_(GL_REPEAT)
        , wrap_s_(GL_REPEAT)
        , wrap_t_(GL_REPEAT)
        , usage_(GL_NONE)
        , compare_func_(GL_LEQUAL)
        , compare_mode_(GL_NONE)
        , max_lod_(1000.0f)
        , min_lod_(-1000.0f)
        , base_level_(0)
        , max_level_(1000)
        , max_level_set_(-1)
        , texture_complete_(false)
        , texture_mips_dirty_(false)
        , cube_complete_(false)
        , texture_level0_dirty_(false)
        , npot_(false)
        , has_been_bound_(false)
        , framebuffer_attachment_count_(0)
        , immutable_(false)
        , has_images_(false)
        , estimated_size_(0)
        , can_render_condition_(CAN_RENDER_ALWAYS)
        , texture_max_anisotropy_initialized_(false)
    {
    }

    Texture::~Texture()
    {
        if (mailbox_manager_)
            mailbox_manager_->TextureDeleted(this);
    }

    void Texture::AddTextureRef(TextureRef* ref)
    {
        DCHECK(refs_.find(ref) == refs_.end());
        refs_.insert(ref);
        if (!memory_tracking_ref_) {
            memory_tracking_ref_ = ref;
            GetMemTracker()->TrackMemAlloc(estimated_size());
        }
    }

    void Texture::RemoveTextureRef(TextureRef* ref, bool have_context)
    {
        if (memory_tracking_ref_ == ref) {
            GetMemTracker()->TrackMemFree(estimated_size());
            memory_tracking_ref_ = NULL;
        }
        size_t result = refs_.erase(ref);
        DCHECK_EQ(result, 1u);
        if (refs_.empty()) {
            if (have_context) {
                GLuint id = service_id();
                glDeleteTextures(1, &id);
            }
            delete this;
        } else if (memory_tracking_ref_ == NULL) {
            // TODO(piman): tune ownership semantics for cross-context group shared
            // textures.
            memory_tracking_ref_ = *refs_.begin();
            GetMemTracker()->TrackMemAlloc(estimated_size());
        }
    }

    MemoryTypeTracker* Texture::GetMemTracker()
    {
        DCHECK(memory_tracking_ref_);
        return memory_tracking_ref_->manager()->GetMemTracker();
    }

    Texture::LevelInfo::LevelInfo()
        : target(0)
        , level(-1)
        , internal_format(0)
        , width(0)
        , height(0)
        , depth(0)
        , border(0)
        , format(0)
        , type(0)
        , image_state(UNBOUND)
        , estimated_size(0)
    {
    }

    Texture::LevelInfo::LevelInfo(const LevelInfo& rhs)
        : cleared_rect(rhs.cleared_rect)
        , target(rhs.target)
        , level(rhs.level)
        , internal_format(rhs.internal_format)
        , width(rhs.width)
        , height(rhs.height)
        , depth(rhs.depth)
        , border(rhs.border)
        , format(rhs.format)
        , type(rhs.type)
        , image(rhs.image)
        , image_state(rhs.image_state)
        , estimated_size(rhs.estimated_size)
    {
    }

    Texture::LevelInfo::~LevelInfo()
    {
    }

    Texture::FaceInfo::FaceInfo()
        : num_mip_levels(0)
    {
    }

    Texture::FaceInfo::~FaceInfo()
    {
    }

    Texture::CanRenderCondition Texture::GetCanRenderCondition() const
    {
        if (target_ == 0)
            return CAN_RENDER_ALWAYS;

        if (target_ != GL_TEXTURE_EXTERNAL_OES) {
            if (face_infos_.empty()) {
                return CAN_RENDER_NEVER;
            }

            const Texture::LevelInfo& first_face = face_infos_[0].level_infos[0];
            if (first_face.width == 0 || first_face.height == 0 || first_face.depth == 0) {
                return CAN_RENDER_NEVER;
            }
        }

        bool needs_mips = NeedsMips();
        if (needs_mips) {
            if (!texture_complete())
                return CAN_RENDER_NEVER;
        }

        if (target_ == GL_TEXTURE_CUBE_MAP && !cube_complete())
            return CAN_RENDER_NEVER;

        bool is_npot_compatible = !needs_mips && wrap_s_ == GL_CLAMP_TO_EDGE && wrap_t_ == GL_CLAMP_TO_EDGE;

        if (!is_npot_compatible) {
            if (target_ == GL_TEXTURE_RECTANGLE_ARB)
                return CAN_RENDER_NEVER;
            else if (npot())
                return CAN_RENDER_ONLY_IF_NPOT;
        }

        return CAN_RENDER_ALWAYS;
    }

    bool Texture::CanRender(const FeatureInfo* feature_info) const
    {
        switch (can_render_condition_) {
        case CAN_RENDER_ALWAYS:
            return true;
        case CAN_RENDER_NEVER:
            return false;
        case CAN_RENDER_ONLY_IF_NPOT:
            break;
        }
        return feature_info->feature_flags().npot_ok;
    }

    void Texture::AddToSignature(
        const FeatureInfo* feature_info,
        GLenum target,
        GLint level,
        std::string* signature) const
    {
        DCHECK(feature_info);
        DCHECK(signature);
        DCHECK_GE(level, 0);
        size_t face_index = GLES2Util::GLTargetToFaceIndex(target);
        DCHECK_LT(static_cast<size_t>(face_index),
            face_infos_.size());
        DCHECK_LT(static_cast<size_t>(level),
            face_infos_[face_index].level_infos.size());

        const Texture::LevelInfo& info = face_infos_[face_index].level_infos[level];

        TextureSignature signature_data(target,
            level,
            min_filter_,
            mag_filter_,
            wrap_r_,
            wrap_s_,
            wrap_t_,
            usage_,
            info.internal_format,
            compare_func_,
            compare_mode_,
            info.width,
            info.height,
            info.depth,
            max_lod_,
            min_lod_,
            base_level_,
            info.border,
            max_level_,
            info.format,
            info.type,
            info.image.get() != NULL,
            CanRender(feature_info),
            CanRenderTo(),
            npot_);

        signature->append(TextureTag, sizeof(TextureTag));
        signature->append(reinterpret_cast<const char*>(&signature_data),
            sizeof(signature_data));
    }

    void Texture::SetMailboxManager(MailboxManager* mailbox_manager)
    {
        DCHECK(!mailbox_manager_ || mailbox_manager_ == mailbox_manager);
        mailbox_manager_ = mailbox_manager;
    }

    bool Texture::MarkMipmapsGenerated(
        const FeatureInfo* feature_info)
    {
        if (!CanGenerateMipmaps(feature_info)) {
            return false;
        }
        for (size_t ii = 0; ii < face_infos_.size(); ++ii) {
            const Texture::FaceInfo& face_info = face_infos_[ii];
            const Texture::LevelInfo& level0_info = face_info.level_infos[0];
            GLsizei width = level0_info.width;
            GLsizei height = level0_info.height;
            GLsizei depth = level0_info.depth;
            GLenum target = target_ == GL_TEXTURE_2D ? GL_TEXTURE_2D : GLES2Util::IndexToGLFaceTarget(ii);

            const GLsizei num_mips = face_info.num_mip_levels;
            for (GLsizei level = 1; level < num_mips; ++level) {
                width = std::max(1, width >> 1);
                height = std::max(1, height >> 1);
                depth = std::max(1, depth >> 1);
                SetLevelInfo(feature_info, target, level, level0_info.internal_format,
                    width, height, depth, level0_info.border, level0_info.format,
                    level0_info.type, gfx::Rect(width, height));
            }
        }

        return true;
    }

    void Texture::SetTarget(
        const FeatureInfo* feature_info, GLenum target, GLint max_levels)
    {
        DCHECK_EQ(0u, target_); // you can only set this once.
        target_ = target;
        size_t num_faces = (target == GL_TEXTURE_CUBE_MAP) ? 6 : 1;
        face_infos_.resize(num_faces);
        for (size_t ii = 0; ii < num_faces; ++ii) {
            face_infos_[ii].level_infos.resize(max_levels);
        }

        if (target == GL_TEXTURE_EXTERNAL_OES || target == GL_TEXTURE_RECTANGLE_ARB) {
            min_filter_ = GL_LINEAR;
            wrap_s_ = wrap_t_ = GL_CLAMP_TO_EDGE;
        }

        if (target == GL_TEXTURE_EXTERNAL_OES) {
            immutable_ = true;
        }
        Update(feature_info);
        UpdateCanRenderCondition();
    }

    bool Texture::CanGenerateMipmaps(
        const FeatureInfo* feature_info) const
    {
        if ((npot() && !feature_info->feature_flags().npot_ok) || face_infos_.empty() || target_ == GL_TEXTURE_EXTERNAL_OES || target_ == GL_TEXTURE_RECTANGLE_ARB) {
            return false;
        }

        if (static_cast<size_t>(base_level_) >= face_infos_[0].level_infos.size()) {
            return false;
        }

        // Can't generate mips for depth or stencil textures.
        const Texture::LevelInfo& base = face_infos_[0].level_infos[base_level_];
        uint32 channels = GLES2Util::GetChannelsForFormat(base.format);
        if (channels & (GLES2Util::kDepth | GLES2Util::kStencil)) {
            return false;
        }

        // TODO(gman): Check internal_format, format and type.
        for (size_t ii = 0; ii < face_infos_.size(); ++ii) {
            const LevelInfo& info = face_infos_[ii].level_infos[base_level_];
            if ((info.target == 0) || (info.width != base.width) || (info.height != base.height) || (info.depth != base.depth) || (info.format != base.format) || (info.internal_format != base.internal_format) || (info.type != base.type) || feature_info->validators()->compressed_texture_format.IsValid(info.internal_format) || info.image.get()) {
                return false;
            }
        }
        return true;
    }

    bool Texture::TextureIsNPOT(GLsizei width,
        GLsizei height,
        GLsizei depth)
    {
        return (GLES2Util::IsNPOT(width) || GLES2Util::IsNPOT(height) || GLES2Util::IsNPOT(depth));
    }

    bool Texture::TextureFaceComplete(const Texture::LevelInfo& first_face,
        size_t face_index,
        GLenum target,
        GLenum internal_format,
        GLsizei width,
        GLsizei height,
        GLsizei depth,
        GLenum format,
        GLenum type)
    {
        bool complete = (target != 0 && depth == 1);
        if (face_index != 0) {
            complete &= (width == first_face.width && height == first_face.height && internal_format == first_face.internal_format && format == first_face.format && type == first_face.type);
        }
        return complete;
    }

    bool Texture::TextureMipComplete(const Texture::LevelInfo& level0_face,
        GLenum target,
        GLint level,
        GLenum internal_format,
        GLsizei width,
        GLsizei height,
        GLsizei depth,
        GLenum format,
        GLenum type)
    {
        bool complete = (target != 0);
        if (level != 0) {
            const GLsizei mip_width = std::max(1, level0_face.width >> level);
            const GLsizei mip_height = std::max(1, level0_face.height >> level);
            const GLsizei mip_depth = std::max(1, level0_face.depth >> level);

            complete &= (width == mip_width && height == mip_height && depth == mip_depth && internal_format == level0_face.internal_format && format == level0_face.format && type == level0_face.type);
        }
        return complete;
    }

    void Texture::SetLevelClearedRect(GLenum target,
        GLint level,
        const gfx::Rect& cleared_rect)
    {
        DCHECK_GE(level, 0);
        size_t face_index = GLES2Util::GLTargetToFaceIndex(target);
        DCHECK_LT(static_cast<size_t>(face_index),
            face_infos_.size());
        DCHECK_LT(static_cast<size_t>(level),
            face_infos_[face_index].level_infos.size());
        Texture::LevelInfo& info = face_infos_[face_index].level_infos[level];
        UpdateMipCleared(&info, info.width, info.height, cleared_rect);
        UpdateCleared();
    }

    void Texture::SetLevelCleared(GLenum target, GLint level, bool cleared)
    {
        DCHECK_GE(level, 0);
        size_t face_index = GLES2Util::GLTargetToFaceIndex(target);
        DCHECK_LT(static_cast<size_t>(face_index), face_infos_.size());
        DCHECK_LT(static_cast<size_t>(level),
            face_infos_[face_index].level_infos.size());
        Texture::LevelInfo& info = face_infos_[face_index].level_infos[level];
        UpdateMipCleared(&info, info.width, info.height,
            cleared ? gfx::Rect(info.width, info.height) : gfx::Rect());
        UpdateCleared();
    }

    void Texture::UpdateCleared()
    {
        if (face_infos_.empty()) {
            return;
        }

        const bool cleared = (num_uncleared_mips_ == 0);

        // If texture is uncleared and is attached to a framebuffer,
        // that framebuffer must be marked possibly incomplete.
        if (!cleared && IsAttachedToFramebuffer()) {
            IncAllFramebufferStateChangeCount();
        }

        UpdateSafeToRenderFrom(cleared);
    }

    void Texture::UpdateSafeToRenderFrom(bool cleared)
    {
        if (cleared_ == cleared)
            return;
        cleared_ = cleared;
        int delta = cleared ? -1 : +1;
        for (RefSet::iterator it = refs_.begin(); it != refs_.end(); ++it)
            (*it)->manager()->UpdateSafeToRenderFrom(delta);
    }

    void Texture::UpdateMipCleared(LevelInfo* info,
        GLsizei width,
        GLsizei height,
        const gfx::Rect& cleared_rect)
    {
        bool was_cleared = info->cleared_rect == gfx::Rect(info->width, info->height);
        info->width = width;
        info->height = height;
        info->cleared_rect = cleared_rect;
        bool cleared = info->cleared_rect == gfx::Rect(info->width, info->height);
        if (cleared == was_cleared)
            return;
        int delta = cleared ? -1 : +1;
        num_uncleared_mips_ += delta;
        for (RefSet::iterator it = refs_.begin(); it != refs_.end(); ++it)
            (*it)->manager()->UpdateUnclearedMips(delta);
    }

    void Texture::UpdateCanRenderCondition()
    {
        CanRenderCondition can_render_condition = GetCanRenderCondition();
        if (can_render_condition_ == can_render_condition)
            return;
        for (RefSet::iterator it = refs_.begin(); it != refs_.end(); ++it)
            (*it)->manager()->UpdateCanRenderCondition(can_render_condition_,
                can_render_condition);
        can_render_condition_ = can_render_condition;
    }

    void Texture::UpdateHasImages()
    {
        if (face_infos_.empty())
            return;

        bool has_images = false;
        for (size_t ii = 0; ii < face_infos_.size(); ++ii) {
            for (size_t jj = 0; jj < face_infos_[ii].level_infos.size(); ++jj) {
                const Texture::LevelInfo& info = face_infos_[ii].level_infos[jj];
                if (info.image.get() != NULL) {
                    has_images = true;
                    break;
                }
            }
        }

        if (has_images_ == has_images)
            return;
        has_images_ = has_images;
        int delta = has_images ? +1 : -1;
        for (RefSet::iterator it = refs_.begin(); it != refs_.end(); ++it)
            (*it)->manager()->UpdateNumImages(delta);
    }

    void Texture::IncAllFramebufferStateChangeCount()
    {
        for (RefSet::iterator it = refs_.begin(); it != refs_.end(); ++it)
            (*it)->manager()->IncFramebufferStateChangeCount();
    }

    void Texture::SetLevelInfo(const FeatureInfo* feature_info,
        GLenum target,
        GLint level,
        GLenum internal_format,
        GLsizei width,
        GLsizei height,
        GLsizei depth,
        GLint border,
        GLenum format,
        GLenum type,
        const gfx::Rect& cleared_rect)
    {
        DCHECK_GE(level, 0);
        size_t face_index = GLES2Util::GLTargetToFaceIndex(target);
        DCHECK_LT(static_cast<size_t>(face_index),
            face_infos_.size());
        DCHECK_LT(static_cast<size_t>(level),
            face_infos_[face_index].level_infos.size());
        DCHECK_GE(width, 0);
        DCHECK_GE(height, 0);
        DCHECK_GE(depth, 0);
        Texture::LevelInfo& info = face_infos_[face_index].level_infos[level];

        // Update counters only if any attributes have changed. Counters are
        // comparisons between the old and new values so it must be done before any
        // assignment has been done to the LevelInfo.
        if (info.target != target || info.internal_format != internal_format || info.width != width || info.height != height || info.depth != depth || info.format != format || info.type != type) {
            if (level == 0) {
                // Calculate the mip level count.
                face_infos_[face_index].num_mip_levels = TextureManager::ComputeMipMapCount(target_, width, height, depth);

                // Update NPOT face count for the first level.
                bool prev_npot = TextureIsNPOT(info.width, info.height, info.depth);
                bool now_npot = TextureIsNPOT(width, height, depth);
                if (prev_npot != now_npot)
                    num_npot_faces_ += now_npot ? 1 : -1;

                // Signify that level 0 has been changed, so they need to be reverified.
                texture_level0_dirty_ = true;
            }

            // Signify that at least one of the mips has changed.
            texture_mips_dirty_ = true;
        }

        info.target = target;
        info.level = level;
        info.internal_format = internal_format;
        info.depth = depth;
        info.border = border;
        info.format = format;
        info.type = type;
        info.image = 0;
        info.image_state = UNBOUND;

        UpdateMipCleared(&info, width, height, cleared_rect);

        estimated_size_ -= info.estimated_size;
        GLES2Util::ComputeImageDataSizes(
            width, height, 1, format, type, 4, &info.estimated_size, NULL, NULL);
        estimated_size_ += info.estimated_size;

        max_level_set_ = std::max(max_level_set_, level);
        Update(feature_info);
        UpdateCleared();
        UpdateCanRenderCondition();
        UpdateHasImages();
        if (IsAttachedToFramebuffer()) {
            // TODO(gman): If textures tracked which framebuffers they were attached to
            // we could just mark those framebuffers as not complete.
            IncAllFramebufferStateChangeCount();
        }
    }

    bool Texture::ValidForTexture(
        GLint target,
        GLint level,
        GLint xoffset,
        GLint yoffset,
        GLint zoffset,
        GLsizei width,
        GLsizei height,
        GLsizei depth) const
    {
        size_t face_index = GLES2Util::GLTargetToFaceIndex(target);
        if (level >= 0 && face_index < face_infos_.size() && static_cast<size_t>(level) < face_infos_[face_index].level_infos.size()) {
            const LevelInfo& info = face_infos_[face_index].level_infos[level];
            int32 max_x;
            int32 max_y;
            int32 max_z;
            return SafeAddInt32(xoffset, width, &max_x) && SafeAddInt32(yoffset, height, &max_y) && SafeAddInt32(zoffset, depth, &max_z) && xoffset >= 0 && yoffset >= 0 && zoffset >= 0 && max_x <= info.width && max_y <= info.height && max_z <= info.depth;
        }
        return false;
    }

    bool Texture::GetLevelSize(
        GLint target, GLint level,
        GLsizei* width, GLsizei* height, GLsizei* depth) const
    {
        DCHECK(width);
        DCHECK(height);
        size_t face_index = GLES2Util::GLTargetToFaceIndex(target);
        if (level >= 0 && face_index < face_infos_.size() && static_cast<size_t>(level) < face_infos_[face_index].level_infos.size()) {
            const LevelInfo& info = face_infos_[face_index].level_infos[level];
            if (info.target != 0) {
                *width = info.width;
                *height = info.height;
                if (depth)
                    *depth = info.depth;
                return true;
            }
        }
        return false;
    }

    bool Texture::GetLevelType(
        GLint target, GLint level, GLenum* type, GLenum* internal_format) const
    {
        DCHECK(type);
        DCHECK(internal_format);
        size_t face_index = GLES2Util::GLTargetToFaceIndex(target);
        if (level >= 0 && face_index < face_infos_.size() && static_cast<size_t>(level) < face_infos_[face_index].level_infos.size()) {
            const LevelInfo& info = face_infos_[face_index].level_infos[level];
            if (info.target != 0) {
                *type = info.type;
                *internal_format = info.internal_format;
                return true;
            }
        }
        return false;
    }

    GLenum Texture::SetParameteri(
        const FeatureInfo* feature_info, GLenum pname, GLint param)
    {
        DCHECK(feature_info);

        if (target_ == GL_TEXTURE_EXTERNAL_OES || target_ == GL_TEXTURE_RECTANGLE_ARB) {
            if (pname == GL_TEXTURE_MIN_FILTER && (param != GL_NEAREST && param != GL_LINEAR))
                return GL_INVALID_ENUM;
            if ((pname == GL_TEXTURE_WRAP_S || pname == GL_TEXTURE_WRAP_T) && param != GL_CLAMP_TO_EDGE)
                return GL_INVALID_ENUM;
        }

        switch (pname) {
        case GL_TEXTURE_MIN_LOD:
        case GL_TEXTURE_MAX_LOD: {
            GLfloat fparam = static_cast<GLfloat>(param);
            return SetParameterf(feature_info, pname, fparam);
        }
        case GL_TEXTURE_MIN_FILTER:
            if (!feature_info->validators()->texture_min_filter_mode.IsValid(param)) {
                return GL_INVALID_ENUM;
            }
            min_filter_ = param;
            break;
        case GL_TEXTURE_MAG_FILTER:
            if (!feature_info->validators()->texture_mag_filter_mode.IsValid(param)) {
                return GL_INVALID_ENUM;
            }
            mag_filter_ = param;
            break;
        case GL_TEXTURE_WRAP_R:
            if (!feature_info->validators()->texture_wrap_mode.IsValid(param)) {
                return GL_INVALID_ENUM;
            }
            wrap_r_ = param;
            break;
        case GL_TEXTURE_WRAP_S:
            if (!feature_info->validators()->texture_wrap_mode.IsValid(param)) {
                return GL_INVALID_ENUM;
            }
            wrap_s_ = param;
            break;
        case GL_TEXTURE_WRAP_T:
            if (!feature_info->validators()->texture_wrap_mode.IsValid(param)) {
                return GL_INVALID_ENUM;
            }
            wrap_t_ = param;
            break;
        case GL_TEXTURE_COMPARE_FUNC:
            if (!feature_info->validators()->texture_compare_func.IsValid(param)) {
                return GL_INVALID_ENUM;
            }
            compare_func_ = param;
            break;
        case GL_TEXTURE_COMPARE_MODE:
            if (!feature_info->validators()->texture_compare_mode.IsValid(param)) {
                return GL_INVALID_ENUM;
            }
            compare_mode_ = param;
            break;
        case GL_TEXTURE_BASE_LEVEL:
            if (param < 0) {
                return GL_INVALID_VALUE;
            }
            base_level_ = param;
            break;
        case GL_TEXTURE_MAX_LEVEL:
            if (param < 0) {
                return GL_INVALID_VALUE;
            }
            max_level_ = param;
            break;
        case GL_TEXTURE_MAX_ANISOTROPY_EXT:
            if (param < 1) {
                return GL_INVALID_VALUE;
            }
            break;
        case GL_TEXTURE_USAGE_ANGLE:
            if (!feature_info->validators()->texture_usage.IsValid(param)) {
                return GL_INVALID_ENUM;
            }
            usage_ = param;
            break;
        default:
            NOTREACHED();
            return GL_INVALID_ENUM;
        }
        Update(feature_info);
        UpdateCleared();
        UpdateCanRenderCondition();
        return GL_NO_ERROR;
    }

    GLenum Texture::SetParameterf(
        const FeatureInfo* feature_info, GLenum pname, GLfloat param)
    {
        switch (pname) {
        case GL_TEXTURE_MIN_FILTER:
        case GL_TEXTURE_MAG_FILTER:
        case GL_TEXTURE_WRAP_R:
        case GL_TEXTURE_WRAP_S:
        case GL_TEXTURE_WRAP_T:
        case GL_TEXTURE_COMPARE_FUNC:
        case GL_TEXTURE_COMPARE_MODE:
        case GL_TEXTURE_BASE_LEVEL:
        case GL_TEXTURE_MAX_LEVEL:
        case GL_TEXTURE_USAGE_ANGLE: {
            GLint iparam = static_cast<GLint>(param);
            return SetParameteri(feature_info, pname, iparam);
        }
        case GL_TEXTURE_MIN_LOD:
            min_lod_ = param;
            break;
        case GL_TEXTURE_MAX_LOD:
            max_lod_ = param;
            break;
        case GL_TEXTURE_MAX_ANISOTROPY_EXT:
            if (param < 1.f) {
                return GL_INVALID_VALUE;
            }
            break;
        default:
            NOTREACHED();
            return GL_INVALID_ENUM;
        }
        return GL_NO_ERROR;
    }

    void Texture::Update(const FeatureInfo* feature_info)
    {
        // Update npot status.
        // Assume GL_TEXTURE_EXTERNAL_OES textures are npot, all others
        npot_ = (target_ == GL_TEXTURE_EXTERNAL_OES) || (num_npot_faces_ > 0);

        if (face_infos_.empty()) {
            texture_complete_ = false;
            cube_complete_ = false;
            return;
        }

        // Update texture_complete and cube_complete status.
        const Texture::FaceInfo& first_face = face_infos_[0];
        const Texture::LevelInfo& first_level = first_face.level_infos[0];
        const GLsizei levels_needed = first_face.num_mip_levels;

        texture_complete_ = max_level_set_ >= (levels_needed - 1) && max_level_set_ >= 0;
        cube_complete_ = (face_infos_.size() == 6) && (first_level.width == first_level.height);

        if (first_level.width == 0 || first_level.height == 0) {
            texture_complete_ = false;
        } else if (first_level.type == GL_FLOAT && !feature_info->feature_flags().enable_texture_float_linear && (min_filter_ != GL_NEAREST_MIPMAP_NEAREST || mag_filter_ != GL_NEAREST)) {
            texture_complete_ = false;
        } else if (first_level.type == GL_HALF_FLOAT_OES && !feature_info->feature_flags().enable_texture_half_float_linear && (min_filter_ != GL_NEAREST_MIPMAP_NEAREST || mag_filter_ != GL_NEAREST)) {
            texture_complete_ = false;
        }

        bool texture_level0_complete = true;
        if (cube_complete_ && texture_level0_dirty_) {
            for (size_t ii = 0; ii < face_infos_.size(); ++ii) {
                const Texture::LevelInfo& level0 = face_infos_[ii].level_infos[0];
                if (!TextureFaceComplete(first_level,
                        ii,
                        level0.target,
                        level0.internal_format,
                        level0.width,
                        level0.height,
                        level0.depth,
                        level0.format,
                        level0.type)) {
                    texture_level0_complete = false;
                    break;
                }
            }
            texture_level0_dirty_ = false;
        }
        cube_complete_ &= texture_level0_complete;

        bool texture_mips_complete = true;
        if (texture_complete_ && texture_mips_dirty_) {
            for (size_t ii = 0; ii < face_infos_.size() && texture_mips_complete;
                 ++ii) {
                const Texture::FaceInfo& face_info = face_infos_[ii];
                const Texture::LevelInfo& level0 = face_info.level_infos[0];
                for (GLsizei jj = 1; jj < levels_needed; ++jj) {
                    const Texture::LevelInfo& level_info = face_infos_[ii].level_infos[jj];
                    if (!TextureMipComplete(level0,
                            level_info.target,
                            jj,
                            level_info.internal_format,
                            level_info.width,
                            level_info.height,
                            level_info.depth,
                            level_info.format,
                            level_info.type)) {
                        texture_mips_complete = false;
                        break;
                    }
                }
            }
            texture_mips_dirty_ = false;
        }
        texture_complete_ &= texture_mips_complete;
    }

    bool Texture::ClearRenderableLevels(GLES2Decoder* decoder)
    {
        DCHECK(decoder);
        if (cleared_) {
            return true;
        }

        for (size_t ii = 0; ii < face_infos_.size(); ++ii) {
            const Texture::FaceInfo& face_info = face_infos_[ii];
            for (GLint jj = 0; jj < face_info.num_mip_levels; ++jj) {
                const Texture::LevelInfo& info = face_info.level_infos[jj];
                if (info.target != 0) {
                    if (!ClearLevel(decoder, info.target, jj)) {
                        return false;
                    }
                }
            }
        }
        UpdateSafeToRenderFrom(true);
        return true;
    }

    gfx::Rect Texture::GetLevelClearedRect(GLenum target, GLint level) const
    {
        size_t face_index = GLES2Util::GLTargetToFaceIndex(target);
        if (face_index >= face_infos_.size() || level >= static_cast<GLint>(face_infos_[face_index].level_infos.size())) {
            return gfx::Rect();
        }

        const Texture::LevelInfo& info = face_infos_[face_index].level_infos[level];

        return info.cleared_rect;
    }

    bool Texture::IsLevelCleared(GLenum target, GLint level) const
    {
        size_t face_index = GLES2Util::GLTargetToFaceIndex(target);
        if (face_index >= face_infos_.size() || level >= static_cast<GLint>(face_infos_[face_index].level_infos.size())) {
            return true;
        }

        const Texture::LevelInfo& info = face_infos_[face_index].level_infos[level];

        return info.cleared_rect == gfx::Rect(info.width, info.height);
    }

    void Texture::InitTextureMaxAnisotropyIfNeeded(GLenum target)
    {
        if (texture_max_anisotropy_initialized_)
            return;
        texture_max_anisotropy_initialized_ = true;
        GLfloat params[] = { 1.0f };
        glTexParameterfv(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, params);
    }

    bool Texture::ClearLevel(
        GLES2Decoder* decoder, GLenum target, GLint level)
    {
        DCHECK(decoder);
        size_t face_index = GLES2Util::GLTargetToFaceIndex(target);
        if (face_index >= face_infos_.size() || level >= static_cast<GLint>(face_infos_[face_index].level_infos.size())) {
            return true;
        }

        Texture::LevelInfo& info = face_infos_[face_index].level_infos[level];

        DCHECK(target == info.target);

        if (info.target == 0 || info.cleared_rect == gfx::Rect(info.width, info.height) || info.width == 0 || info.height == 0 || info.depth == 0) {
            return true;
        }

        // Clear all remaining sub regions.
        const int x[] = {
            0, info.cleared_rect.x(), info.cleared_rect.right(), info.width
        };
        const int y[] = {
            0, info.cleared_rect.y(), info.cleared_rect.bottom(), info.height
        };

        for (size_t j = 0; j < 3; ++j) {
            for (size_t i = 0; i < 3; ++i) {
                // Center of nine patch is already cleared.
                if (j == 1 && i == 1)
                    continue;

                gfx::Rect rect(x[i], y[j], x[i + 1] - x[i], y[j + 1] - y[j]);
                if (rect.IsEmpty())
                    continue;

                // NOTE: It seems kind of gross to call back into the decoder for this
                // but only the decoder knows all the state (like unpack_alignment_)
                // that's needed to be able to call GL correctly.
                bool cleared = decoder->ClearLevel(this, info.target, info.level,
                    info.format, info.type, rect.x(),
                    rect.y(), rect.width(), rect.height());
                if (!cleared)
                    return false;
            }
        }

        UpdateMipCleared(&info, info.width, info.height,
            gfx::Rect(info.width, info.height));
        return true;
    }

    void Texture::SetLevelImage(GLenum target,
        GLint level,
        gl::GLImage* image,
        ImageState state)
    {
        DCHECK_GE(level, 0);
        size_t face_index = GLES2Util::GLTargetToFaceIndex(target);
        DCHECK_LT(static_cast<size_t>(face_index),
            face_infos_.size());
        DCHECK_LT(static_cast<size_t>(level),
            face_infos_[face_index].level_infos.size());
        Texture::LevelInfo& info = face_infos_[face_index].level_infos[level];
        DCHECK_EQ(info.target, target);
        DCHECK_EQ(info.level, level);
        info.image = image;
        info.image_state = state;
        UpdateCanRenderCondition();
        UpdateHasImages();
    }

    gl::GLImage* Texture::GetLevelImage(GLint target,
        GLint level,
        ImageState* state) const
    {
        if (target != GL_TEXTURE_2D && target != GL_TEXTURE_EXTERNAL_OES && target != GL_TEXTURE_RECTANGLE_ARB) {
            return NULL;
        }

        size_t face_index = GLES2Util::GLTargetToFaceIndex(target);
        if (level >= 0 && face_index < face_infos_.size() && static_cast<size_t>(level) < face_infos_[face_index].level_infos.size()) {
            const LevelInfo& info = face_infos_[face_index].level_infos[level];
            if (info.target != 0) {
                if (state)
                    *state = info.image_state;
                return info.image.get();
            }
        }
        return NULL;
    }

    gl::GLImage* Texture::GetLevelImage(GLint target, GLint level) const
    {
        return GetLevelImage(target, level, nullptr);
    }

    void Texture::DumpLevelMemory(base::trace_event::ProcessMemoryDump* pmd,
        uint64_t client_tracing_id,
        const std::string& dump_name) const
    {
        //   for (uint32_t face_index = 0; face_index < face_infos_.size(); ++face_index) {
        //     const auto& level_infos = face_infos_[face_index].level_infos;
        //     for (uint32_t level_index = 0; level_index < level_infos.size();
        //          ++level_index) {
        //       // Skip levels with no size. Textures will have empty levels for all
        //       // potential mip levels which are not in use.
        //       if (!level_infos[level_index].estimated_size)
        //         continue;
        //
        //       // If a level has a GLImage, ask the GLImage to dump itself.
        //       if (level_infos[level_index].image) {
        //         level_infos[level_index].image->OnMemoryDump(
        //             pmd, client_tracing_id,
        //             base::StringPrintf("%s/face_%d/level_%d", dump_name.c_str(),
        //                                face_index, level_index));
        //       }
        //
        //       // If a level does not have a GLImage bound to it, then dump the
        //       // texture allocation also as the storage is not provided by the
        //       // GLImage in that case.
        //       if (level_infos[level_index].image_state != BOUND) {
        //           base::trace_event::MemoryAllocatorDump* dump = pmd->CreateAllocatorDump(
        //               base::StringPrintf("%s/face_%d/level_%d", dump_name.c_str(),
        //                   face_index, level_index));
        //           dump->AddScalar(
        //               base::trace_event::MemoryAllocatorDump::kNameSize,
        //               base::trace_event::MemoryAllocatorDump::kUnitsBytes,
        //               static_cast<uint64_t>(level_infos[level_index].estimated_size));
        //       }
        //     }
        //   }
    }

    TextureRef::TextureRef(TextureManager* manager,
        GLuint client_id,
        Texture* texture)
        : manager_(manager)
        , texture_(texture)
        , client_id_(client_id)
        , num_observers_(0)
    {
        DCHECK(manager_);
        DCHECK(texture_);
        texture_->AddTextureRef(this);
        manager_->StartTracking(this);
    }

    scoped_refptr<TextureRef> TextureRef::Create(TextureManager* manager,
        GLuint client_id,
        GLuint service_id)
    {
        return new TextureRef(manager, client_id, new Texture(service_id));
    }

    TextureRef::~TextureRef()
    {
        manager_->StopTracking(this);
        texture_->RemoveTextureRef(this, manager_->have_context_);
        manager_ = NULL;
    }

    TextureManager::TextureManager(MemoryTracker* memory_tracker,
        FeatureInfo* feature_info,
        GLint max_texture_size,
        GLint max_cube_map_texture_size,
        GLint max_rectangle_texture_size,
        GLint max_3d_texture_size,
        bool use_default_textures)
        : memory_type_tracker_(new MemoryTypeTracker(memory_tracker))
        , memory_tracker_(memory_tracker)
        , feature_info_(feature_info)
        , framebuffer_manager_(NULL)
        , max_texture_size_(max_texture_size)
        , max_cube_map_texture_size_(max_cube_map_texture_size)
        , max_rectangle_texture_size_(max_rectangle_texture_size)
        , max_3d_texture_size_(max_3d_texture_size)
        , max_levels_(ComputeMipMapCount(GL_TEXTURE_2D,
              max_texture_size,
              max_texture_size,
              max_texture_size))
        , max_cube_map_levels_(ComputeMipMapCount(GL_TEXTURE_CUBE_MAP,
              max_cube_map_texture_size,
              max_cube_map_texture_size,
              max_cube_map_texture_size))
        , max_3d_levels_(ComputeMipMapCount(GL_TEXTURE_3D,
              // Same as GL_TEXTURE_2D_ARRAY
              max_3d_texture_size,
              max_3d_texture_size,
              max_3d_texture_size))
        , use_default_textures_(use_default_textures)
        , num_unrenderable_textures_(0)
        , num_unsafe_textures_(0)
        , num_uncleared_mips_(0)
        , num_images_(0)
        , texture_count_(0)
        , have_context_(true)
    {
        for (int ii = 0; ii < kNumDefaultTextures; ++ii) {
            black_texture_ids_[ii] = 0;
        }
    }

    bool TextureManager::Initialize()
    {
        // TODO(gman): The default textures have to be real textures, not the 0
        // texture because we simulate non shared resources on top of shared
        // resources and all contexts that share resource share the same default
        // texture.
        default_textures_[kTexture2D] = CreateDefaultAndBlackTextures(
            GL_TEXTURE_2D, &black_texture_ids_[kTexture2D]);
        default_textures_[kCubeMap] = CreateDefaultAndBlackTextures(
            GL_TEXTURE_CUBE_MAP, &black_texture_ids_[kCubeMap]);

        if (feature_info_->IsES3Enabled()) {
            default_textures_[kTexture3D] = CreateDefaultAndBlackTextures(
                GL_TEXTURE_3D, &black_texture_ids_[kTexture3D]);
            default_textures_[kTexture2DArray] = CreateDefaultAndBlackTextures(
                GL_TEXTURE_2D_ARRAY, &black_texture_ids_[kTexture2DArray]);
        }

        if (feature_info_->feature_flags().oes_egl_image_external) {
            default_textures_[kExternalOES] = CreateDefaultAndBlackTextures(
                GL_TEXTURE_EXTERNAL_OES, &black_texture_ids_[kExternalOES]);
        }

        if (feature_info_->feature_flags().arb_texture_rectangle) {
            default_textures_[kRectangleARB] = CreateDefaultAndBlackTextures(
                GL_TEXTURE_RECTANGLE_ARB, &black_texture_ids_[kRectangleARB]);
        }

        // When created from InProcessCommandBuffer, we won't have a |memory_tracker_|
        // so don't register a dump provider.
        //   if (memory_tracker_) {
        //     base::trace_event::MemoryDumpManager::GetInstance()->RegisterDumpProvider(
        //         this, "gpu::TextureManager", base::ThreadTaskRunnerHandle::Get());
        //   }

        return true;
    }

    scoped_refptr<TextureRef>
    TextureManager::CreateDefaultAndBlackTextures(
        GLenum target,
        GLuint* black_texture)
    {
        static uint8 black[] = { 0, 0, 0, 255 };

        // Sampling a texture not associated with any EGLImage sibling will return
        // black values according to the spec.
        bool needs_initialization = (target != GL_TEXTURE_EXTERNAL_OES);
        bool needs_faces = (target == GL_TEXTURE_CUBE_MAP);
        bool is_3d_or_2d_array_target = (target == GL_TEXTURE_3D || target == GL_TEXTURE_2D_ARRAY);

        // Make default textures and texture for replacing non-renderable textures.
        GLuint ids[2];
        const int num_ids = use_default_textures_ ? 2 : 1;
        glGenTextures(num_ids, ids);
        for (int ii = 0; ii < num_ids; ++ii) {
            glBindTexture(target, ids[ii]);
            if (needs_initialization) {
                if (needs_faces) {
                    for (int jj = 0; jj < GLES2Util::kNumFaces; ++jj) {
                        glTexImage2D(GLES2Util::IndexToGLFaceTarget(jj), 0, GL_RGBA, 1, 1, 0,
                            GL_RGBA, GL_UNSIGNED_BYTE, black);
                    }
                } else {
                    if (is_3d_or_2d_array_target) {
                        glTexImage3D(target, 0, GL_RGBA, 1, 1, 1, 0, GL_RGBA,
                            GL_UNSIGNED_BYTE, black);
                    } else {
                        glTexImage2D(target, 0, GL_RGBA, 1, 1, 0, GL_RGBA,
                            GL_UNSIGNED_BYTE, black);
                    }
                }
            }
        }
        glBindTexture(target, 0);

        scoped_refptr<TextureRef> default_texture;
        if (use_default_textures_) {
            default_texture = TextureRef::Create(this, 0, ids[1]);
            SetTarget(default_texture.get(), target);
            if (needs_faces) {
                for (int ii = 0; ii < GLES2Util::kNumFaces; ++ii) {
                    SetLevelInfo(default_texture.get(), GLES2Util::IndexToGLFaceTarget(ii),
                        0, GL_RGBA, 1, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                        gfx::Rect(1, 1));
                }
            } else {
                if (needs_initialization) {
                    SetLevelInfo(default_texture.get(), GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 1,
                        0, GL_RGBA, GL_UNSIGNED_BYTE, gfx::Rect(1, 1));
                } else {
                    SetLevelInfo(default_texture.get(), GL_TEXTURE_EXTERNAL_OES, 0, GL_RGBA,
                        1, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, gfx::Rect(1, 1));
                }
            }
        }

        *black_texture = ids[0];
        return default_texture;
    }

    bool TextureManager::ValidForTarget(
        GLenum target, GLint level, GLsizei width, GLsizei height, GLsizei depth)
    {
        GLsizei max_size = MaxSizeForTarget(target) >> level;
        return level >= 0 && width >= 0 && height >= 0 && depth >= 0 && level < MaxLevelsForTarget(target) && width <= max_size && height <= max_size && depth <= max_size && (level == 0 || feature_info_->feature_flags().npot_ok || (!GLES2Util::IsNPOT(width) && !GLES2Util::IsNPOT(height) && !GLES2Util::IsNPOT(depth))) && (target != GL_TEXTURE_CUBE_MAP || (width == height && depth == 1)) && (target != GL_TEXTURE_2D || (depth == 1));
    }

    void TextureManager::SetTarget(TextureRef* ref, GLenum target)
    {
        DCHECK(ref);
        ref->texture()
            ->SetTarget(feature_info_.get(), target, MaxLevelsForTarget(target));
    }

    void TextureManager::SetLevelClearedRect(TextureRef* ref,
        GLenum target,
        GLint level,
        const gfx::Rect& cleared_rect)
    {
        DCHECK(ref);
        ref->texture()->SetLevelClearedRect(target, level, cleared_rect);
    }

    void TextureManager::SetLevelCleared(TextureRef* ref,
        GLenum target,
        GLint level,
        bool cleared)
    {
        DCHECK(ref);
        ref->texture()->SetLevelCleared(target, level, cleared);
    }

    bool TextureManager::ClearRenderableLevels(
        GLES2Decoder* decoder, TextureRef* ref)
    {
        DCHECK(ref);
        return ref->texture()->ClearRenderableLevels(decoder);
    }

    bool TextureManager::ClearTextureLevel(
        GLES2Decoder* decoder, TextureRef* ref,
        GLenum target, GLint level)
    {
        DCHECK(ref);
        Texture* texture = ref->texture();
        if (texture->num_uncleared_mips() == 0) {
            return true;
        }
        bool result = texture->ClearLevel(decoder, target, level);
        texture->UpdateCleared();
        return result;
    }

    void TextureManager::SetLevelInfo(TextureRef* ref,
        GLenum target,
        GLint level,
        GLenum internal_format,
        GLsizei width,
        GLsizei height,
        GLsizei depth,
        GLint border,
        GLenum format,
        GLenum type,
        const gfx::Rect& cleared_rect)
    {
        DCHECK(gfx::Rect(width, height).Contains(cleared_rect));
        DCHECK(ref);
        Texture* texture = ref->texture();

        texture->GetMemTracker()->TrackMemFree(texture->estimated_size());
        texture->SetLevelInfo(feature_info_.get(), target, level, internal_format,
            width, height, depth, border, format, type,
            cleared_rect);
        texture->GetMemTracker()->TrackMemAlloc(texture->estimated_size());
    }

    Texture* TextureManager::Produce(TextureRef* ref)
    {
        DCHECK(ref);
        return ref->texture();
    }

    TextureRef* TextureManager::Consume(
        GLuint client_id,
        Texture* texture)
    {
        DCHECK(client_id);
        scoped_refptr<TextureRef> ref(new TextureRef(this, client_id, texture));
        bool result = textures_.insert(std::make_pair(client_id, ref)).second;
        DCHECK(result);
        return ref.get();
    }

    void TextureManager::SetParameteri(
        const char* function_name, ErrorState* error_state,
        TextureRef* ref, GLenum pname, GLint param)
    {
        DCHECK(error_state);
        DCHECK(ref);
        Texture* texture = ref->texture();
        GLenum result = texture->SetParameteri(feature_info_.get(), pname, param);
        if (result != GL_NO_ERROR) {
            if (result == GL_INVALID_ENUM) {
                ERRORSTATE_SET_GL_ERROR_INVALID_ENUM(
                    error_state, function_name, param, "param");
            } else {
                ERRORSTATE_SET_GL_ERROR_INVALID_PARAMI(
                    error_state, result, function_name, pname, param);
            }
        } else {
            glTexParameteri(texture->target(), pname, param);
        }
    }

    void TextureManager::SetParameterf(
        const char* function_name, ErrorState* error_state,
        TextureRef* ref, GLenum pname, GLfloat param)
    {
        DCHECK(error_state);
        DCHECK(ref);
        Texture* texture = ref->texture();
        GLenum result = texture->SetParameterf(feature_info_.get(), pname, param);
        if (result != GL_NO_ERROR) {
            if (result == GL_INVALID_ENUM) {
                ERRORSTATE_SET_GL_ERROR_INVALID_ENUM(
                    error_state, function_name, pname, "pname");
            } else {
                ERRORSTATE_SET_GL_ERROR_INVALID_PARAMF(
                    error_state, result, function_name, pname, param);
            }
        } else {
            glTexParameterf(texture->target(), pname, param);
        }
    }

    bool TextureManager::MarkMipmapsGenerated(TextureRef* ref)
    {
        DCHECK(ref);
        Texture* texture = ref->texture();
        texture->GetMemTracker()->TrackMemFree(texture->estimated_size());
        bool result = texture->MarkMipmapsGenerated(feature_info_.get());
        texture->GetMemTracker()->TrackMemAlloc(texture->estimated_size());
        return result;
    }

    TextureRef* TextureManager::CreateTexture(
        GLuint client_id, GLuint service_id)
    {
        DCHECK_NE(0u, service_id);
        scoped_refptr<TextureRef> ref(TextureRef::Create(
            this, client_id, service_id));
        std::pair<TextureMap::iterator, bool> result = textures_.insert(std::make_pair(client_id, ref));
        DCHECK(result.second);
        return ref.get();
    }

    TextureRef* TextureManager::GetTexture(
        GLuint client_id) const
    {
        TextureMap::const_iterator it = textures_.find(client_id);
        return it != textures_.end() ? it->second.get() : NULL;
    }

    void TextureManager::RemoveTexture(GLuint client_id)
    {
        TextureMap::iterator it = textures_.find(client_id);
        if (it != textures_.end()) {
            it->second->reset_client_id();
            textures_.erase(it);
        }
    }

    void TextureManager::StartTracking(TextureRef* ref)
    {
        Texture* texture = ref->texture();
        ++texture_count_;
        num_uncleared_mips_ += texture->num_uncleared_mips();
        if (!texture->SafeToRenderFrom())
            ++num_unsafe_textures_;
        if (!texture->CanRender(feature_info_.get()))
            ++num_unrenderable_textures_;
        if (texture->HasImages())
            ++num_images_;
    }

    void TextureManager::StopTracking(TextureRef* ref)
    {
        if (ref->num_observers()) {
            for (unsigned int i = 0; i < destruction_observers_.size(); i++) {
                destruction_observers_[i]->OnTextureRefDestroying(ref);
            }
            DCHECK_EQ(ref->num_observers(), 0);
        }

        Texture* texture = ref->texture();

        --texture_count_;
        if (texture->HasImages()) {
            DCHECK_NE(0, num_images_);
            --num_images_;
        }
        if (!texture->CanRender(feature_info_.get())) {
            DCHECK_NE(0, num_unrenderable_textures_);
            --num_unrenderable_textures_;
        }
        if (!texture->SafeToRenderFrom()) {
            DCHECK_NE(0, num_unsafe_textures_);
            --num_unsafe_textures_;
        }
        num_uncleared_mips_ -= texture->num_uncleared_mips();
        DCHECK_GE(num_uncleared_mips_, 0);
    }

    MemoryTypeTracker* TextureManager::GetMemTracker()
    {
        return memory_type_tracker_.get();
    }

    Texture* TextureManager::GetTextureForServiceId(GLuint service_id) const
    {
        // This doesn't need to be fast. It's only used during slow queries.
        for (TextureMap::const_iterator it = textures_.begin();
             it != textures_.end(); ++it) {
            Texture* texture = it->second->texture();
            if (texture->service_id() == service_id)
                return texture;
        }
        return NULL;
    }

    GLsizei TextureManager::ComputeMipMapCount(GLenum target,
        GLsizei width,
        GLsizei height,
        GLsizei depth)
    {
        switch (target) {
        case GL_TEXTURE_EXTERNAL_OES:
            return 1;
        default:
            return 1 + base::bits::Log2Floor(std::max(std::max(width, height), depth));
        }
    }

    void TextureManager::SetLevelImage(TextureRef* ref,
        GLenum target,
        GLint level,
        gl::GLImage* image,
        Texture::ImageState state)
    {
        DCHECK(ref);
        ref->texture()->SetLevelImage(target, level, image, state);
    }

    size_t TextureManager::GetSignatureSize() const
    {
        return sizeof(TextureTag) + sizeof(TextureSignature);
    }

    void TextureManager::AddToSignature(
        TextureRef* ref,
        GLenum target,
        GLint level,
        std::string* signature) const
    {
        ref->texture()->AddToSignature(feature_info_.get(), target, level, signature);
    }

    void TextureManager::UpdateSafeToRenderFrom(int delta)
    {
        num_unsafe_textures_ += delta;
        DCHECK_GE(num_unsafe_textures_, 0);
    }

    void TextureManager::UpdateUnclearedMips(int delta)
    {
        num_uncleared_mips_ += delta;
        DCHECK_GE(num_uncleared_mips_, 0);
    }

    void TextureManager::UpdateCanRenderCondition(
        Texture::CanRenderCondition old_condition,
        Texture::CanRenderCondition new_condition)
    {
        if (old_condition == Texture::CAN_RENDER_NEVER || (old_condition == Texture::CAN_RENDER_ONLY_IF_NPOT && !feature_info_->feature_flags().npot_ok)) {
            DCHECK_GT(num_unrenderable_textures_, 0);
            --num_unrenderable_textures_;
        }
        if (new_condition == Texture::CAN_RENDER_NEVER || (new_condition == Texture::CAN_RENDER_ONLY_IF_NPOT && !feature_info_->feature_flags().npot_ok))
            ++num_unrenderable_textures_;
    }

    void TextureManager::UpdateNumImages(int delta)
    {
        num_images_ += delta;
        DCHECK_GE(num_images_, 0);
    }

    void TextureManager::IncFramebufferStateChangeCount()
    {
        if (framebuffer_manager_)
            framebuffer_manager_->IncFramebufferStateChangeCount();
    }

    bool TextureManager::ValidateTextureParameters(
        ErrorState* error_state, const char* function_name,
        GLenum format, GLenum type, GLenum internal_format, GLint level)
    {
        const Validators* validators = feature_info_->validators();
        if (!validators->texture_format.IsValid(format)) {
            ERRORSTATE_SET_GL_ERROR_INVALID_ENUM(
                error_state, function_name, format, "format");
            return false;
        }
        if (!validators->pixel_type.IsValid(type)) {
            ERRORSTATE_SET_GL_ERROR_INVALID_ENUM(
                error_state, function_name, type, "type");
            return false;
        }
        if (!g_format_type_validator.Get().IsValid(internal_format, format, type)) {
            ERRORSTATE_SET_GL_ERROR(
                error_state, GL_INVALID_OPERATION, function_name,
                "invalid internalformat/format/type combination");
            return false;
        }
        // For TexSubImage calls, internal_format isn't part of the parameters,
        // so its validation needs to be after the internal_format/format/type
        // combination validation. Otherwise, an unexpected INVALID_ENUM could be
        // generated instead of INVALID_OPERATION.
        if (!validators->texture_internal_format.IsValid(internal_format)) {
            ERRORSTATE_SET_GL_ERROR_INVALID_ENUM(
                error_state, function_name, internal_format, "internal_format");
            return false;
        }
        if (!feature_info_->IsES3Enabled()) {
            uint32 channels = GLES2Util::GetChannelsForFormat(format);
            if ((channels & (GLES2Util::kDepth | GLES2Util::kStencil)) != 0 && level) {
                ERRORSTATE_SET_GL_ERROR(
                    error_state, GL_INVALID_OPERATION, function_name,
                    (std::string("invalid format ") + GLES2Util::GetStringEnum(format) + " for level != 0").c_str());
                return false;
            }
        }
        return true;
    }

    // Gets the texture id for a given target.
    TextureRef* TextureManager::GetTextureInfoForTarget(
        ContextState* state, GLenum target)
    {
        TextureUnit& unit = state->texture_units[state->active_texture_unit];
        TextureRef* texture = NULL;
        switch (target) {
        case GL_TEXTURE_2D:
            texture = unit.bound_texture_2d.get();
            break;
        case GL_TEXTURE_CUBE_MAP:
        case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
        case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
        case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
            texture = unit.bound_texture_cube_map.get();
            break;
        case GL_TEXTURE_EXTERNAL_OES:
            texture = unit.bound_texture_external_oes.get();
            break;
        case GL_TEXTURE_RECTANGLE_ARB:
            texture = unit.bound_texture_rectangle_arb.get();
            break;
        case GL_TEXTURE_3D:
            texture = unit.bound_texture_3d.get();
            break;
        case GL_TEXTURE_2D_ARRAY:
            texture = unit.bound_texture_2d_array.get();
            break;
        default:
            NOTREACHED();
            return NULL;
        }
        return texture;
    }

    TextureRef* TextureManager::GetTextureInfoForTargetUnlessDefault(
        ContextState* state, GLenum target)
    {
        TextureRef* texture = GetTextureInfoForTarget(state, target);
        if (!texture)
            return NULL;
        if (texture == GetDefaultTextureInfo(target))
            return NULL;
        return texture;
    }

    bool TextureManager::ValidateTexImage(
        ContextState* state,
        const char* function_name,
        const DoTexImageArguments& args,
        TextureRef** texture_ref)
    {
        ErrorState* error_state = state->GetErrorState();
        const Validators* validators = feature_info_->validators();
        if (((args.command_type == DoTexImageArguments::kTexImage2D) && !validators->texture_target.IsValid(args.target)) || ((args.command_type == DoTexImageArguments::kTexImage3D) && !validators->texture_3_d_target.IsValid(args.target))) {
            ERRORSTATE_SET_GL_ERROR_INVALID_ENUM(
                error_state, function_name, args.target, "target");
            return false;
        }
        // TODO(ccameron): Add a separate texture from |texture_target| for
        // [Compressed]Tex[Sub]Image2D and related functions.
        // http://crbug.com/536854
        if (args.target == GL_TEXTURE_RECTANGLE_ARB) {
            ERRORSTATE_SET_GL_ERROR_INVALID_ENUM(
                error_state, function_name, args.target, "target");
            return false;
        }
        if (!ValidateTextureParameters(
                error_state, function_name, args.format, args.type,
                args.internal_format, args.level)) {
            return false;
        }
        if (!ValidForTarget(args.target, args.level,
                args.width, args.height, args.depth)
            || args.border != 0) {
            ERRORSTATE_SET_GL_ERROR(
                error_state, GL_INVALID_VALUE, function_name,
                "dimensions out of range");
            return false;
        }
        if ((GLES2Util::GetChannelsForFormat(args.format) & (GLES2Util::kDepth | GLES2Util::kStencil)) != 0 && args.pixels
            && !feature_info_->IsES3Enabled()) {
            ERRORSTATE_SET_GL_ERROR(
                error_state, GL_INVALID_OPERATION,
                function_name, "can not supply data for depth or stencil textures");
            return false;
        }

        TextureRef* local_texture_ref = GetTextureInfoForTarget(state, args.target);
        if (!local_texture_ref) {
            ERRORSTATE_SET_GL_ERROR(
                error_state, GL_INVALID_OPERATION, function_name,
                "unknown texture for target");
            return false;
        }
        if (local_texture_ref->texture()->IsImmutable()) {
            ERRORSTATE_SET_GL_ERROR(
                error_state, GL_INVALID_OPERATION, function_name,
                "texture is immutable");
            return false;
        }

        if (!memory_type_tracker_->EnsureGPUMemoryAvailable(args.pixels_size)) {
            ERRORSTATE_SET_GL_ERROR(error_state, GL_OUT_OF_MEMORY, function_name,
                "out of memory");
            return false;
        }

        // Write the TextureReference since this is valid.
        *texture_ref = local_texture_ref;
        return true;
    }

    void TextureManager::ValidateAndDoTexImage(
        DecoderTextureState* texture_state,
        ContextState* state,
        DecoderFramebufferState* framebuffer_state,
        const char* function_name,
        const DoTexImageArguments& args)
    {
        TextureRef* texture_ref;
        if (!ValidateTexImage(state, function_name, args, &texture_ref)) {
            return;
        }

        // ValidateTexImage is passed already.
        Texture* texture = texture_ref->texture();
        bool need_cube_map_workaround = texture->target() == GL_TEXTURE_CUBE_MAP && (texture_state->force_cube_complete || (texture_state->force_cube_map_positive_x_allocation && args.target != GL_TEXTURE_CUBE_MAP_POSITIVE_X));
        if (need_cube_map_workaround) {
            std::vector<GLenum> undefined_faces;
            if (texture_state->force_cube_complete) {
                int width = 0;
                int height = 0;
                for (unsigned i = 0; i < 6; i++) {
                    bool defined = texture->GetLevelSize(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                        args.level, &width, &height, nullptr);
                    if (!defined || GL_TEXTURE_CUBE_MAP_POSITIVE_X + i == args.target)
                        undefined_faces.push_back(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
                }
            } else if (texture_state->force_cube_map_positive_x_allocation && args.target != GL_TEXTURE_CUBE_MAP_POSITIVE_X) {
                int width = 0;
                int height = 0;
                if (!texture->GetLevelSize(GL_TEXTURE_CUBE_MAP_POSITIVE_X, args.level,
                        &width, &height, nullptr)) {
                    undefined_faces.push_back(GL_TEXTURE_CUBE_MAP_POSITIVE_X);
                }
                undefined_faces.push_back(args.target);
            }

            DCHECK(undefined_faces.size());
            if (!memory_type_tracker_->EnsureGPUMemoryAvailable(
                    undefined_faces.size() * args.pixels_size)) {
                ERRORSTATE_SET_GL_ERROR(state->GetErrorState(), GL_OUT_OF_MEMORY,
                    function_name, "out of memory");
                return;
            }
            DoTexImageArguments new_args = args;
            scoped_ptr<char[]> zero(new char[args.pixels_size]);
            memset(zero.get(), 0, args.pixels_size);
            for (GLenum face : undefined_faces) {
                new_args.target = face;
                if (face == args.target) {
                    new_args.pixels = args.pixels;
                } else {
                    new_args.pixels = zero.get();
                }
                DoTexImage(texture_state, state->GetErrorState(), framebuffer_state,
                    function_name, texture_ref, new_args);
            }
            return;
        }

        DoTexImage(texture_state, state->GetErrorState(), framebuffer_state,
            function_name, texture_ref, args);
    }

    bool TextureManager::ValidateTexSubImage(ContextState* state,
        const char* function_name,
        const DoTexSubImageArguments& args,
        TextureRef** texture_ref)
    {
        ErrorState* error_state = state->GetErrorState();
        const Validators* validators = feature_info_->validators();

        if (!validators->texture_target.IsValid(args.target)) {
            ERRORSTATE_SET_GL_ERROR_INVALID_ENUM(error_state, function_name,
                args.target, "target");
            return false;
        }
        if (args.width < 0) {
            ERRORSTATE_SET_GL_ERROR(error_state, GL_INVALID_VALUE, function_name,
                "width < 0");
            return false;
        }
        if (args.height < 0) {
            ERRORSTATE_SET_GL_ERROR(error_state, GL_INVALID_VALUE, function_name,
                "height < 0");
            return false;
        }
        TextureRef* local_texture_ref = GetTextureInfoForTarget(state, args.target);
        if (!local_texture_ref) {
            ERRORSTATE_SET_GL_ERROR(error_state, GL_INVALID_OPERATION, function_name,
                "unknown texture for target");
            return false;
        }
        Texture* texture = local_texture_ref->texture();
        GLenum current_type = 0;
        GLenum internal_format = 0;
        if (!texture->GetLevelType(args.target, args.level, &current_type,
                &internal_format)) {
            ERRORSTATE_SET_GL_ERROR(error_state, GL_INVALID_OPERATION, function_name,
                "level does not exist.");
            return false;
        }
        if (!ValidateTextureParameters(error_state, function_name, args.format,
                args.type, internal_format, args.level)) {
            return false;
        }
        if (args.type != current_type && !feature_info_->IsES3Enabled()) {
            ERRORSTATE_SET_GL_ERROR(error_state, GL_INVALID_OPERATION, function_name,
                "type does not match type of texture.");
            return false;
        }
        if (!texture->ValidForTexture(args.target, args.level, args.xoffset,
                args.yoffset, 0, args.width, args.height, 1)) {
            ERRORSTATE_SET_GL_ERROR(error_state, GL_INVALID_VALUE, function_name,
                "bad dimensions.");
            return false;
        }
        if ((GLES2Util::GetChannelsForFormat(args.format) & (GLES2Util::kDepth | GLES2Util::kStencil)) != 0 && !feature_info_->IsES3Enabled()) {
            ERRORSTATE_SET_GL_ERROR(
                error_state, GL_INVALID_OPERATION, function_name,
                "can not supply data for depth or stencil textures");
            return false;
        }
        DCHECK(args.pixels);
        *texture_ref = local_texture_ref;
        return true;
    }

    void TextureManager::ValidateAndDoTexSubImage(
        GLES2Decoder* decoder,
        DecoderTextureState* texture_state,
        ContextState* state,
        DecoderFramebufferState* framebuffer_state,
        const char* function_name,
        const DoTexSubImageArguments& args)
    {
        ErrorState* error_state = state->GetErrorState();
        TextureRef* texture_ref;
        if (!ValidateTexSubImage(state, function_name, args, &texture_ref)) {
            return;
        }

        Texture* texture = texture_ref->texture();
        GLsizei tex_width = 0;
        GLsizei tex_height = 0;
        bool ok = texture->GetLevelSize(args.target, args.level, &tex_width,
            &tex_height, nullptr);
        DCHECK(ok);
        if (args.xoffset != 0 || args.yoffset != 0 || args.width != tex_width || args.height != tex_height) {
            gfx::Rect cleared_rect;
            if (CombineAdjacentRects(
                    texture->GetLevelClearedRect(args.target, args.level),
                    gfx::Rect(args.xoffset, args.yoffset, args.width, args.height),
                    &cleared_rect)) {
                DCHECK_GE(cleared_rect.size().GetArea(),
                    texture->GetLevelClearedRect(args.target, args.level)
                        .size()
                        .GetArea());
                SetLevelClearedRect(texture_ref, args.target, args.level, cleared_rect);
            } else {
                // Otherwise clear part of texture level that is not already cleared.
                if (!ClearTextureLevel(decoder, texture_ref, args.target, args.level)) {
                    ERRORSTATE_SET_GL_ERROR(error_state, GL_OUT_OF_MEMORY,
                        "glTexSubImage2D", "dimensions too big");
                    return;
                }
            }
            ScopedTextureUploadTimer timer(texture_state);
            glTexSubImage2D(args.target, args.level, args.xoffset, args.yoffset,
                args.width, args.height, AdjustTexFormat(args.format),
                args.type, args.pixels);
            return;
        }

        if (!texture_state->texsubimage_faster_than_teximage && !texture->IsImmutable() && !texture->HasImages()) {
            ScopedTextureUploadTimer timer(texture_state);
            GLenum internal_format;
            GLenum tex_type;
            texture->GetLevelType(args.target, args.level, &tex_type, &internal_format);
            // NOTE: In OpenGL ES 2.0 border is always zero. If that changes we'll need
            // to look it up.
            glTexImage2D(args.target, args.level, internal_format, args.width,
                args.height, 0, AdjustTexFormat(args.format), args.type,
                args.pixels);
        } else {
            ScopedTextureUploadTimer timer(texture_state);
            glTexSubImage2D(args.target, args.level, args.xoffset, args.yoffset,
                args.width, args.height, AdjustTexFormat(args.format),
                args.type, args.pixels);
        }
        SetLevelCleared(texture_ref, args.target, args.level, true);
        return;
    }

    GLenum TextureManager::AdjustTexFormat(GLenum format) const
    {
        // TODO(bajones): GLES 3 allows for internal format and format to differ.
        // This logic may need to change as a result.
        if (gfx::GetGLImplementation() == gfx::kGLImplementationDesktopGL) {
            if (format == GL_SRGB_EXT)
                return GL_RGB;
            if (format == GL_SRGB_ALPHA_EXT)
                return GL_RGBA;
        }
        return format;
    }

    void TextureManager::DoTexImage(
        DecoderTextureState* texture_state,
        ErrorState* error_state,
        DecoderFramebufferState* framebuffer_state,
        const char* function_name,
        TextureRef* texture_ref,
        const DoTexImageArguments& args)
    {
        Texture* texture = texture_ref->texture();
        GLsizei tex_width = 0;
        GLsizei tex_height = 0;
        GLsizei tex_depth = 0;
        GLenum tex_type = 0;
        GLenum tex_format = 0;
        bool level_is_same = texture->GetLevelSize(
                                 args.target, args.level, &tex_width, &tex_height, &tex_depth)
            && texture->GetLevelType(args.target, args.level, &tex_type, &tex_format) && args.width == tex_width && args.height == tex_height && args.depth == tex_depth && args.type == tex_type && args.format == tex_format;

        if (level_is_same && !args.pixels) {
            // Just set the level texture but mark the texture as uncleared.
            SetLevelInfo(texture_ref, args.target, args.level, args.internal_format,
                args.width, args.height, args.depth, args.border, args.format,
                args.type, gfx::Rect());
            texture_state->tex_image_failed = false;
            return;
        }

        if (texture->IsAttachedToFramebuffer()) {
            framebuffer_state->clear_state_dirty = true;
        }

        if (texture_state->texsubimage_faster_than_teximage && level_is_same && args.pixels) {
            {
                ScopedTextureUploadTimer timer(texture_state);
                if (args.command_type == DoTexImageArguments::kTexImage3D) {
                    glTexSubImage3D(args.target, args.level, 0, 0, 0,
                        args.width, args.height, args.depth,
                        args.format, args.type, args.pixels);
                } else {
                    glTexSubImage2D(args.target, args.level, 0, 0, args.width, args.height,
                        AdjustTexFormat(args.format), args.type, args.pixels);
                }
            }
            SetLevelCleared(texture_ref, args.target, args.level, true);
            texture_state->tex_image_failed = false;
            return;
        }

        ERRORSTATE_COPY_REAL_GL_ERRORS_TO_WRAPPER(error_state, function_name);
        {
            ScopedTextureUploadTimer timer(texture_state);
            if (args.command_type == DoTexImageArguments::kTexImage3D) {
                glTexImage3D(args.target, args.level, args.internal_format, args.width,
                    args.height, args.depth, args.border, args.format,
                    args.type, args.pixels);
            } else {
                glTexImage2D(args.target, args.level, args.internal_format, args.width,
                    args.height, args.border, AdjustTexFormat(args.format),
                    args.type, args.pixels);
            }
        }
        GLenum error = ERRORSTATE_PEEK_GL_ERROR(error_state, function_name);
        if (error == GL_NO_ERROR) {
            SetLevelInfo(
                texture_ref, args.target, args.level, args.internal_format, args.width,
                args.height, args.depth, args.border, args.format, args.type,
                args.pixels != NULL ? gfx::Rect(args.width, args.height) : gfx::Rect());
            texture_state->tex_image_failed = false;
        }
    }

    bool TextureManager::CombineAdjacentRects(const gfx::Rect& rect1,
        const gfx::Rect& rect2,
        gfx::Rect* result)
    {
        // Return |rect2| if |rect1| is empty or |rect2| contains |rect1|.
        if (rect1.IsEmpty() || rect2.Contains(rect1)) {
            *result = rect2;
            return true;
        }

        // Return |rect1| if |rect2| is empty or |rect1| contains |rect2|.
        if (rect2.IsEmpty() || rect1.Contains(rect2)) {
            *result = rect1;
            return true;
        }

        // Return the union of |rect1| and |rect2| if they share an edge.
        if (rect1.SharesEdgeWith(rect2)) {
            *result = gfx::UnionRects(rect1, rect2);
            return true;
        }

        // Return false if it's not possible to combine |rect1| and |rect2|.
        return false;
    }

    ScopedTextureUploadTimer::ScopedTextureUploadTimer(
        DecoderTextureState* texture_state)
        : texture_state_(texture_state)
        , begin_time_(base::TimeTicks::Now())
    {
    }

    ScopedTextureUploadTimer::~ScopedTextureUploadTimer()
    {
        texture_state_->texture_upload_count++;
        texture_state_->total_texture_upload_time += base::TimeTicks::Now() - begin_time_;
    }

    bool TextureManager::OnMemoryDump(const base::trace_event::MemoryDumpArgs& args,
        base::trace_event::ProcessMemoryDump* pmd)
    {
        //   for (const auto& resource : textures_) {
        //     // Only dump memory info for textures actually owned by this TextureManager.
        //     DumpTextureRef(pmd, resource.second.get());
        //   }
        //
        //   // Also dump TextureManager internal textures, if allocated.
        //   for (int i = 0; i < kNumDefaultTextures; i++) {
        //     if (default_textures_[i]) {
        //       DumpTextureRef(pmd, default_textures_[i].get());
        //     }
        //   }

        return true;
    }

    void TextureManager::DumpTextureRef(base::trace_event::ProcessMemoryDump* pmd,
        TextureRef* ref)
    {
        //   uint32_t size = ref->texture()->estimated_size();
        //
        //   // Ignore unallocated texture IDs.
        //   if (size == 0)
        //     return;
        //
        //   std::string dump_name =
        //       base::StringPrintf("gpu/gl/textures/client_%d/texture_%d",
        //                          memory_tracker_->ClientId(), ref->client_id());
        //
        //   base::trace_event::MemoryAllocatorDump* dump =
        //       pmd->CreateAllocatorDump(dump_name);
        //   dump->AddScalar(base::trace_event::MemoryAllocatorDump::kNameSize,
        //                   base::trace_event::MemoryAllocatorDump::kUnitsBytes,
        //                   static_cast<uint64_t>(size));
        //
        //   // Add the |client_guid| which expresses shared ownership with the client
        //   // process.
        //   auto client_guid = gfx::GetGLTextureClientGUIDForTracing(
        //       memory_tracker_->ShareGroupTracingGUID(), ref->client_id());
        //   pmd->CreateSharedGlobalAllocatorDump(client_guid);
        //   pmd->AddOwnershipEdge(dump->guid(), client_guid);
        //
        //   // Add a |service_guid| which expresses shared ownership between the various
        //   // |client_guid|s.
        //   // TODO(ericrk): May need to ensure uniqueness using GLShareGroup and
        //   // potentially cross-share-group sharing via EGLImages. crbug.com/512534
        //   auto service_guid = gfx::GetGLTextureServiceGUIDForTracing(
        //       memory_tracker_->ShareGroupTracingGUID(), ref->texture()->service_id());
        //   pmd->CreateSharedGlobalAllocatorDump(service_guid);
        //
        //   int importance = 0;  // Default importance.
        //   // The link to the memory tracking |client_id| is given a higher importance
        //   // than other refs.
        //   if (ref == ref->texture()->memory_tracking_ref_)
        //     importance = 2;
        //
        //   pmd->AddOwnershipEdge(client_guid, service_guid, importance);
        //
        //   // Dump all sub-levels held by the texture. They will appear below the main
        //   // gl/textures/client_X/texture_Y dump.
        //   ref->texture()->DumpLevelMemory(pmd, memory_tracker_->ClientTracingId(),
        //                                   dump_name);
    }

} // namespace gles2
} // namespace gpu
