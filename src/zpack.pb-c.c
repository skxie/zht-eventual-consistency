/* Generated by the protocol buffer compiler.  DO NOT EDIT! */

/* Do not generate deprecated warnings for self */
#ifndef PROTOBUF_C_NO_DEPRECATED
#define PROTOBUF_C_NO_DEPRECATED
#endif

#include "zpack.pb-c.h"
void   zpack__init
                     (ZPack         *message)
{
  static ZPack init_value = ZPACK__INIT;
  *message = init_value;
}
size_t zpack__get_packed_size
                     (const ZPack *message)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &zpack__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t zpack__pack
                     (const ZPack *message,
                      uint8_t       *out)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &zpack__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t zpack__pack_to_buffer
                     (const ZPack *message,
                      ProtobufCBuffer *buffer)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &zpack__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
ZPack *
       zpack__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (ZPack *)
     protobuf_c_message_unpack (&zpack__descriptor,
                                allocator, len, data);
}
void   zpack__free_unpacked
                     (ZPack *message,
                      ProtobufCAllocator *allocator)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &zpack__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
static const ProtobufCFieldDescriptor zpack__field_descriptors[7] =
{
  {
    "opcode",
    1,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(ZPack, opcode),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "key",
    2,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(ZPack, key),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "val",
    3,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(ZPack, val),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "valnull",
    4,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_BOOL,
    PROTOBUF_C_OFFSETOF(ZPack, has_valnull),
    PROTOBUF_C_OFFSETOF(ZPack, valnull),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "newval",
    5,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(ZPack, newval),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "newvalnull",
    6,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_BOOL,
    PROTOBUF_C_OFFSETOF(ZPack, has_newvalnull),
    PROTOBUF_C_OFFSETOF(ZPack, newvalnull),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "replicanum",
    7,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_INT32,
    PROTOBUF_C_OFFSETOF(ZPack, has_replicanum),
    PROTOBUF_C_OFFSETOF(ZPack, replicanum),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned zpack__field_indices_by_name[] = {
  1,   /* field[1] = key */
  4,   /* field[4] = newval */
  5,   /* field[5] = newvalnull */
  0,   /* field[0] = opcode */
  6,   /* field[6] = replicanum */
  2,   /* field[2] = val */
  3,   /* field[3] = valnull */
};
static const ProtobufCIntRange zpack__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 7 }
};
const ProtobufCMessageDescriptor zpack__descriptor =
{
  PROTOBUF_C_MESSAGE_DESCRIPTOR_MAGIC,
  "ZPack",
  "ZPack",
  "ZPack",
  "",
  sizeof(ZPack),
  7,
  zpack__field_descriptors,
  zpack__field_indices_by_name,
  1,  zpack__number_ranges,
  (ProtobufCMessageInit) zpack__init,
  NULL,NULL,NULL    /* reserved[123] */
};
