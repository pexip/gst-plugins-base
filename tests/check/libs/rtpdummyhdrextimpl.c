/* GStreamer RTP header extension unit tests
 * Copyright (C) 2020 Matthew Waters <matthew@centricular.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General
 * Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>
#include <gst/check/check.h>
#include <gst/rtp/rtp.h>

/* GstRTPDummyHdrExt shared between payloading and depayloading tests */

#define GST_TYPE_RTP_DUMMY_HDR_EXT \
  (gst_rtp_dummy_hdr_ext_get_type())
#define GST_RTP_DUMMY_HDR_EXT(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_RTP_DUMMY_HDR_EXT,GstRTPDummyHdrExt))
#define GST_RTP_DUMMY_HDR_EXT_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_RTP_DUMMY_HDR_EXT,GstRTPDummyHdrExtClass))
#define GST_IS_RTP_DUMMY_HDR_EXT(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_RTP_DUMMY_HDR_EXT))
#define GST_IS_RTP_DUMMY_HDR_EXT_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_RTP_DUMMY_HDR_EXT))

#define DUMMPY_HDR_EXT_URI "gst:test:uri"

typedef struct _GstRTPDummyHdrExt GstRTPDummyHdrExt;
typedef struct _GstRTPDummyHdrExtClass GstRTPDummyHdrExtClass;

struct _GstRTPDummyHdrExt
{
  GstRTPHeaderExtension payload;

  GstRTPHeaderExtensionFlags supported_flags;
  guint read_count;
  guint write_count;
};

struct _GstRTPDummyHdrExtClass
{
  GstRTPBasePayloadClass parent_class;
};

GType gst_rtp_dummy_hdr_ext_get_type (void);

G_DEFINE_TYPE (GstRTPDummyHdrExt, gst_rtp_dummy_hdr_ext,
    GST_TYPE_RTP_HEADER_EXTENSION);

static const gchar *gst_rtp_dummy_hdr_ext_get_uri (GstRTPHeaderExtension * ext);
static GstRTPHeaderExtensionFlags
gst_rtp_dummy_hdr_ext_get_supported_flags (GstRTPHeaderExtension * ext);
static gsize gst_rtp_dummy_hdr_ext_get_max_size (GstRTPHeaderExtension * ext,
    const GstBuffer * input_meta);
static gsize gst_rtp_dummy_hdr_ext_write (GstRTPHeaderExtension * ext,
    const GstBuffer * input_meta, GstRTPHeaderExtensionFlags write_flags,
    GstBuffer * output, guint8 * data, gsize size);
static gboolean gst_rtp_dummy_hdr_ext_read (GstRTPHeaderExtension * ext,
    GstRTPHeaderExtensionFlags read_flags, const guint8 * data, gsize size,
    GstBuffer * buffer);

static void
gst_rtp_dummy_hdr_ext_class_init (GstRTPDummyHdrExtClass * klass)
{
  GstRTPHeaderExtensionClass *gstrtpheaderextension_class;
  GstElementClass *gstelement_class;

  gstrtpheaderextension_class = GST_RTP_HEADER_EXTENSION_CLASS (klass);
  gstelement_class = GST_ELEMENT_CLASS (klass);

  gstrtpheaderextension_class->get_uri = gst_rtp_dummy_hdr_ext_get_uri;
  gstrtpheaderextension_class->get_supported_flags =
      gst_rtp_dummy_hdr_ext_get_supported_flags;
  gstrtpheaderextension_class->get_max_size =
      gst_rtp_dummy_hdr_ext_get_max_size;
  gstrtpheaderextension_class->write = gst_rtp_dummy_hdr_ext_write;
  gstrtpheaderextension_class->read = gst_rtp_dummy_hdr_ext_read;

  gst_element_class_set_static_metadata (gstelement_class,
      "Dummy Test RTP Header Extension", GST_RTP_HDREXT_ELEMENT_CLASS,
      "Dummy Test RTP Header Extension", "Author <email@example.com>");
}

static void
gst_rtp_dummy_hdr_ext_init (GstRTPDummyHdrExt * dummy)
{
  dummy->supported_flags =
      GST_RTP_HEADER_EXTENSION_ONE_BYTE | GST_RTP_HEADER_EXTENSION_TWO_BYTE;
}

static GstRTPHeaderExtension *
rtp_dummy_hdr_ext_new (void)
{
  return g_object_new (GST_TYPE_RTP_DUMMY_HDR_EXT, NULL);
}

static const gchar *
gst_rtp_dummy_hdr_ext_get_uri (GstRTPHeaderExtension * ext)
{
  return DUMMPY_HDR_EXT_URI;
}

static GstRTPHeaderExtensionFlags
gst_rtp_dummy_hdr_ext_get_supported_flags (GstRTPHeaderExtension * ext)
{
  GstRTPDummyHdrExt *dummy = GST_RTP_DUMMY_HDR_EXT (ext);

  return dummy->supported_flags;
}

static gsize
gst_rtp_dummy_hdr_ext_get_max_size (GstRTPHeaderExtension * ext,
    const GstBuffer * input_meta)
{
  return 1;
}

#define TEST_DATA_BYTE 0x9d

static gsize
gst_rtp_dummy_hdr_ext_write (GstRTPHeaderExtension * ext,
    const GstBuffer * input_meta, GstRTPHeaderExtensionFlags write_flags,
    GstBuffer * output, guint8 * data, gsize size)
{
  GstRTPDummyHdrExt *dummy = GST_RTP_DUMMY_HDR_EXT (ext);

  g_assert (size >= gst_rtp_dummy_hdr_ext_get_max_size (ext, NULL));

  data[0] = TEST_DATA_BYTE;

  dummy->write_count++;

  return 1;
}

static gboolean
gst_rtp_dummy_hdr_ext_read (GstRTPHeaderExtension * ext,
    GstRTPHeaderExtensionFlags read_flags, const guint8 * data,
    gsize size, GstBuffer * buffer)
{
  GstRTPDummyHdrExt *dummy = GST_RTP_DUMMY_HDR_EXT (ext);

  fail_unless_equals_int (data[0], TEST_DATA_BYTE);

  dummy->read_count++;

  return TRUE;
}
