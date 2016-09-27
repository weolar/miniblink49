// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SerializationTag_h
#define SerializationTag_h

namespace blink {

// Serialization format is a sequence of tags followed by zero or more data arguments.
// Tags always take exactly one byte. A serialized stream first begins with
// a complete VersionTag. If the stream does not begin with a VersionTag, we assume that
// the stream is in format 0.

// This format is private to the implementation of SerializedScriptValue. Do not rely on it
// externally. It is safe to persist a SerializedScriptValue as a binary blob, but this
// code should always be used to interpret it.

// WebCoreStrings are read as (length:uint32_t, string:UTF8[length]).
// RawStrings are read as (length:uint32_t, string:UTF8[length]).
// RawUCharStrings are read as (length:uint32_t, string:UChar[length/sizeof(UChar)]).
// RawFiles are read as (path:WebCoreString, url:WebCoreStrng, type:WebCoreString).
// There is a reference table that maps object references (uint32_t) to v8::Values.
// Tokens marked with (ref) are inserted into the reference table and given the next object reference ID after decoding.
// All tags except InvalidTag, PaddingTag, ReferenceCountTag, VersionTag, GenerateFreshObjectTag
//     and GenerateFreshArrayTag push their results to the deserialization stack.
// There is also an 'open' stack that is used to resolve circular references. Objects or arrays may
//     contain self-references. Before we begin to deserialize the contents of these values, they
//     are first given object reference IDs (by GenerateFreshObjectTag/GenerateFreshArrayTag);
//     these reference IDs are then used with ObjectReferenceTag to tie the recursive knot.
enum SerializationTag {
    InvalidTag = '!', // Causes deserialization to fail.
    PaddingTag = '\0', // Is ignored (but consumed).
    UndefinedTag = '_', // -> <undefined>
    NullTag = '0', // -> <null>
    TrueTag = 'T', // -> <true>
    FalseTag = 'F', // -> <false>
    StringTag = 'S', // string:RawString -> string
    StringUCharTag = 'c', // string:RawUCharString -> string
    Int32Tag = 'I', // value:ZigZag-encoded int32 -> Integer
    Uint32Tag = 'U', // value:uint32_t -> Integer
    DateTag = 'D', // value:double -> Date (ref)
    MessagePortTag = 'M', // index:int -> MessagePort. Fills the result with transferred MessagePort.
    NumberTag = 'N', // value:double -> Number
    BlobTag = 'b', // uuid:WebCoreString, type:WebCoreString, size:uint64_t -> Blob (ref)
    BlobIndexTag = 'i', // index:int32_t -> Blob (ref)
    FileTag = 'f', // file:RawFile -> File (ref)
    FileIndexTag = 'e', // index:int32_t -> File (ref)
    DOMFileSystemTag = 'd', // type:int32_t, name:WebCoreString, uuid:WebCoreString -> FileSystem (ref)
    FileListTag = 'l', // length:uint32_t, files:RawFile[length] -> FileList (ref)
    FileListIndexTag = 'L', // length:uint32_t, files:int32_t[length] -> FileList (ref)
    ImageDataTag = '#', // width:uint32_t, height:uint32_t, pixelDataLength:uint32_t, data:byte[pixelDataLength] -> ImageData (ref)
    ObjectTag = '{', // numProperties:uint32_t -> pops the last object from the open stack;
        //                                        fills it with the last numProperties name,value pairs pushed onto the deserialization stack
    SparseArrayTag = '@', // numProperties:uint32_t, length:uint32_t -> pops the last object from the open stack;
        //                                                              fills it with the last numProperties name,value pairs pushed onto the deserialization stack
    DenseArrayTag = '$', // numProperties:uint32_t, length:uint32_t -> pops the last object from the open stack;
        //                                                             fills it with the last length elements and numProperties name,value pairs pushed onto deserialization stack
    RegExpTag = 'R', // pattern:RawString, flags:uint32_t -> RegExp (ref)
    ArrayBufferTag = 'B', // byteLength:uint32_t, data:byte[byteLength] -> ArrayBuffer (ref)
    ArrayBufferTransferTag = 't', // index:uint32_t -> ArrayBuffer. For ArrayBuffer transfer
    ArrayBufferViewTag = 'V', // subtag:byte, byteOffset:uint32_t, byteLength:uint32_t -> ArrayBufferView (ref). Consumes an ArrayBuffer from the top of the deserialization stack.
    CryptoKeyTag = 'K', // subtag:byte, props, usages:uint32_t, keyDataLength:uint32_t, keyData:byte[keyDataLength]
        //                 If subtag=AesKeyTag:
        //                     props = keyLengthBytes:uint32_t, algorithmId:uint32_t
        //                 If subtag=HmacKeyTag:
        //                     props = keyLengthBytes:uint32_t, hashId:uint32_t
        //                 If subtag=RsaHashedKeyTag:
        //                     props = algorithmId:uint32_t, type:uint32_t, modulusLengthBits:uint32_t, publicExponentLength:uint32_t, publicExponent:byte[publicExponentLength], hashId:uint32_t
        //                 If subtag=EcKeyTag:
        //                     props = algorithmId:uint32_t, type:uint32_t, namedCurve:uint32_t
    ObjectReferenceTag = '^', // ref:uint32_t -> reference table[ref]
    GenerateFreshObjectTag = 'o', // -> empty object allocated an object ID and pushed onto the open stack (ref)
    GenerateFreshSparseArrayTag = 'a', // length:uint32_t -> empty array[length] allocated an object ID and pushed onto the open stack (ref)
    GenerateFreshDenseArrayTag = 'A', // length:uint32_t -> empty array[length] allocated an object ID and pushed onto the open stack (ref)
    ReferenceCountTag = '?', // refTableSize:uint32_t -> If the reference table is not refTableSize big, fails.
    StringObjectTag = 's', //  string:RawString -> new String(string) (ref)
    NumberObjectTag = 'n', // value:double -> new Number(value) (ref)
    TrueObjectTag = 'y', // new Boolean(true) (ref)
    FalseObjectTag = 'x', // new Boolean(false) (ref)
    CompositorProxyTag = 'C', // elementId:uint64_t, bitfields:uint32_t -> CompositorProxy (ref)
    MapTag = ':', // length:uint32_t -> pops the last object from the open stack (it will be a Map);
        //                              fills it with the last length elements pushed onto the deserialization stack, treating them as key/value pairs and passing them to Map::Set;
        //                              length must be an even number.
    SetTag = ',', // length:uint32_t -> pops the last object from the open stack (it will be a Set);
        //                              fills it with the last length elements pushed onto the deserialization stack, using Set::Add
    GenerateFreshMapTag = ';', // -> empty Map allocated an object ID and pushed onto the open stack (ref)
    GenerateFreshSetTag = '\'', // -> empty Set allocated an object ID and pushed onto the open stack (ref)
    VersionTag = 0xFF // version:uint32_t -> Uses this as the file version.
};

enum ArrayBufferViewSubTag {
    ByteArrayTag = 'b',
    UnsignedByteArrayTag = 'B',
    UnsignedByteClampedArrayTag = 'C',
    ShortArrayTag = 'w',
    UnsignedShortArrayTag = 'W',
    IntArrayTag = 'd',
    UnsignedIntArrayTag = 'D',
    FloatArrayTag = 'f',
    DoubleArrayTag = 'F',
    DataViewTag = '?'
};

} // namespace blink

#endif
