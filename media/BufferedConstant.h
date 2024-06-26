
#ifndef media_BufferedConstant_h
#define media_BufferedConstant_h

namespace media {

const int kHttpOK = 200;
const int kHttpPartialContent = 206;

// Define the number of bytes in a megabyte.
const int kMegabyte = 1024 * 1024;

// Minimum capacity of the buffer in forward or backward direction.
//
// 2MB is an arbitrary limit; it just seems to be "good enough" in practice.
const int kMinBufferCapacity = 2 * kMegabyte;

// Maximum capacity of the buffer in forward or backward direction. This is
// effectively the largest single read the code path can handle.
// 20MB is an arbitrary limit; it just seems to be "good enough" in practice.
const int kMaxBufferCapacity = 20 * kMegabyte;

// Maximum number of bytes outside the buffer we will wait for in order to
// fulfill a read. If a read starts more than 2MB away from the data we
// currently have in the buffer, we will not wait for buffer to reach the read's
// location and will instead reset the request.
const int kForwardWaitThreshold = 2 * kMegabyte;

}

#endif // media_BufferedConstant_h