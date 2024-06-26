// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GPU_COMMAND_BUFFER_SERVICE_PATH_MANAGER_H_
#define GPU_COMMAND_BUFFER_SERVICE_PATH_MANAGER_H_

#include <map>

#include "base/basictypes.h"
#include "gpu/command_buffer/service/gl_utils.h"
#include "gpu/gpu_export.h"

namespace gpu {
namespace gles2 {

    // This class keeps track of paths and their client and service ids
    // so we can correctly clear them.
    class GPU_EXPORT PathManager {
    public:
        PathManager();
        ~PathManager();

        // Must call before destruction.
        void Destroy(bool have_context);

        // Creates a path mapping between closed intervals
        // [first_client_id, last_client_id] -> [first_service_id, ...].
        void CreatePathRange(GLuint first_client_id,
            GLuint last_client_id,
            GLuint first_service_id);

        // Returns true if any path exists in the closed interval
        // [first_client_id, last_client_id].
        bool HasPathsInRange(GLuint first_client_id, GLuint last_client_id) const;

        // Gets the path id corresponding the client path id.
        // Returns false if no such service path id was not found.
        bool GetPath(GLuint client_id, GLuint* service_id) const;

        // Removes a closed interval of paths [first_client_id, last_client_id].
        void RemovePaths(GLuint first_client_id, GLuint last_client_id);

        // Mapping between client id and service id.
        // Should be used only by the implementation.
        struct PathRangeDescription {
            PathRangeDescription(GLuint last_client, GLuint first_service)
                : last_client_id(last_client)
                , first_service_id(first_service)
            {
            }
            GLuint last_client_id;
            GLuint first_service_id;
            typedef GLuint ServiceIdType;
        };
        typedef std::map<GLuint, PathRangeDescription> PathRangeMap;

    private:
        // Checks for consistency inside the book-keeping structures. Used as
        // DCHECK pre/post condition in mutating functions.
        bool CheckConsistency();

        PathRangeMap path_map_;

        DISALLOW_COPY_AND_ASSIGN(PathManager);
    };

} // namespace gles2
} // namespace gpu

#endif // GPU_COMMAND_BUFFER_SERVICE_PATH_MANAGER_H_
