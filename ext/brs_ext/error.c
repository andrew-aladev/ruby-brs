// Ruby bindings for brotli library.
// Copyright (c) 2019 AUTHORS, MIT License.

#include "brs_ext/error.h"

brs_ext_result_t brs_ext_get_decompressor_error(BrotliDecoderErrorCode error_code)
{
  switch (error_code) {
    case BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_NIBBLE:
    case BROTLI_DECODER_ERROR_FORMAT_RESERVED:
    case BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_META_NIBBLE:
    case BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_ALPHABET:
    case BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_SAME:
    case BROTLI_DECODER_ERROR_FORMAT_CL_SPACE:
    case BROTLI_DECODER_ERROR_FORMAT_HUFFMAN_SPACE:
    case BROTLI_DECODER_ERROR_FORMAT_CONTEXT_MAP_REPEAT:
    case BROTLI_DECODER_ERROR_FORMAT_BLOCK_LENGTH_1:
    case BROTLI_DECODER_ERROR_FORMAT_BLOCK_LENGTH_2:
    case BROTLI_DECODER_ERROR_FORMAT_TRANSFORM:
    case BROTLI_DECODER_ERROR_FORMAT_DICTIONARY:
    case BROTLI_DECODER_ERROR_FORMAT_WINDOW_BITS:
    case BROTLI_DECODER_ERROR_FORMAT_PADDING_1:
    case BROTLI_DECODER_ERROR_FORMAT_PADDING_2:
    case BROTLI_DECODER_ERROR_FORMAT_DISTANCE:
      return BRS_EXT_ERROR_DECOMPRESSOR_CORRUPTED_SOURCE;
    case BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MODES:
    case BROTLI_DECODER_ERROR_ALLOC_TREE_GROUPS:
    case BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MAP:
    case BROTLI_DECODER_ERROR_ALLOC_RING_BUFFER_1:
    case BROTLI_DECODER_ERROR_ALLOC_RING_BUFFER_2:
    case BROTLI_DECODER_ERROR_ALLOC_BLOCK_TYPE_TREES:
      return BRS_EXT_ERROR_ALLOCATE_FAILED;
    default:
      return BRS_EXT_ERROR_UNEXPECTED;
  }
}

static inline NORETURN(void raise_error(const char* name, const char* description))
{
  VALUE module = rb_define_module(BRS_EXT_MODULE_NAME);
  VALUE error  = rb_const_get(module, rb_intern(name));
  rb_raise(error, "%s", description);
}

void brs_ext_raise_error(brs_ext_result_t ext_result)
{
  switch (ext_result) {
    case BRS_EXT_ERROR_ALLOCATE_FAILED:
      raise_error("AllocateError", "allocate error");
    case BRS_EXT_ERROR_VALIDATE_FAILED:
      raise_error("ValidateError", "validate error");

    case BRS_EXT_ERROR_USED_AFTER_CLOSE:
      raise_error("UsedAfterCloseError", "used after closed");
    case BRS_EXT_ERROR_NOT_ENOUGH_SOURCE_BUFFER:
      raise_error("NotEnoughSourceBufferError", "not enough source buffer");
    case BRS_EXT_ERROR_NOT_ENOUGH_DESTINATION_BUFFER:
      raise_error("NotEnoughDestinationBufferError", "not enough destination buffer");
    case BRS_EXT_ERROR_DECOMPRESSOR_CORRUPTED_SOURCE:
      raise_error("DecompressorCorruptedSourceError", "decompressor received corrupted source");

    case BRS_EXT_ERROR_ACCESS_IO:
      raise_error("AccessIOError", "failed to access IO");
    case BRS_EXT_ERROR_READ_IO:
      raise_error("ReadIOError", "failed to read IO");
    case BRS_EXT_ERROR_WRITE_IO:
      raise_error("WriteIOError", "failed to write IO");

    default:
      // BRS_EXT_ERROR_UNEXPECTED
      raise_error("UnexpectedError", "unexpected error");
  }
}
