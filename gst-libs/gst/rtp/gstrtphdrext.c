/* GStreamer
 * Copyright (C) <2012> Wim Taymans <wim.taymans@gmail.com>
 * Copyright (C) <2020> Matthew Waters <matthew@centricular.com>
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
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/**
 * SECTION:gstrtphdrext
 * @title: GstRtphdrext
 * @short_description: Helper methods for dealing with RTP header extensions
 * @see_also: #GstRTPBasePayload, #GstRTPBaseDepayload, gstrtpbuffer
 *
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gstrtphdrext.h"

#include <stdlib.h>
#include <string.h>

GST_DEBUG_CATEGORY_STATIC (rtphderext_debug);
#define GST_CAT_DEFAULT (rtphderext_debug)

#define MAX_RTP_EXT_ID 256

/**
 * gst_rtp_hdrext_set_ntp_64:
 * @data: the data to write to
 * @size: the size of @data
 * @ntptime: the NTP time
 *
 * Writes the NTP time in @ntptime to the format required for the NTP-64 header
 * extension. @data must hold at least #GST_RTP_HDREXT_NTP_64_SIZE bytes.
 *
 * Returns: %TRUE on success.
 */
gboolean
gst_rtp_hdrext_set_ntp_64 (gpointer data, guint size, guint64 ntptime)
{
  g_return_val_if_fail (data != NULL, FALSE);
  g_return_val_if_fail (size >= GST_RTP_HDREXT_NTP_64_SIZE, FALSE);

  GST_WRITE_UINT64_BE (data, ntptime);

  return TRUE;
}

/**
 * gst_rtp_hdrext_get_ntp_64:
 * @data: (array length=size) (element-type guint8): the data to read from
 * @size: the size of @data
 * @ntptime: (out): the result NTP time
 *
 * Reads the NTP time from the @size NTP-64 extension bytes in @data and store the
 * result in @ntptime.
 *
 * Returns: %TRUE on success.
 */
gboolean
gst_rtp_hdrext_get_ntp_64 (gpointer data, guint size, guint64 * ntptime)
{
  g_return_val_if_fail (data != NULL, FALSE);
  g_return_val_if_fail (size >= GST_RTP_HDREXT_NTP_64_SIZE, FALSE);

  if (ntptime)
    *ntptime = GST_READ_UINT64_BE (data);

  return TRUE;
}

/**
 * gst_rtp_hdrext_set_ntp_56:
 * @data: the data to write to
 * @size: the size of @data
 * @ntptime: the NTP time
 *
 * Writes the NTP time in @ntptime to the format required for the NTP-56 header
 * extension. @data must hold at least #GST_RTP_HDREXT_NTP_56_SIZE bytes.
 *
 * Returns: %TRUE on success.
 */
gboolean
gst_rtp_hdrext_set_ntp_56 (gpointer data, guint size, guint64 ntptime)
{
  guint8 *d = data;
  gint i;

  g_return_val_if_fail (data != NULL, FALSE);
  g_return_val_if_fail (size >= GST_RTP_HDREXT_NTP_56_SIZE, FALSE);

  for (i = 0; i < 7; i++) {
    d[6 - i] = ntptime & 0xff;
    ntptime >>= 8;
  }
  return TRUE;
}

/**
 * gst_rtp_hdrext_get_ntp_56:
 * @data: (array length=size) (element-type guint8): the data to read from
 * @size: the size of @data
 * @ntptime: (out): the result NTP time
 *
 * Reads the NTP time from the @size NTP-56 extension bytes in @data and store the
 * result in @ntptime.
 *
 * Returns: %TRUE on success.
 */
gboolean
gst_rtp_hdrext_get_ntp_56 (gpointer data, guint size, guint64 * ntptime)
{
  guint8 *d = data;

  g_return_val_if_fail (data != NULL, FALSE);
  g_return_val_if_fail (size >= GST_RTP_HDREXT_NTP_56_SIZE, FALSE);

  if (ntptime) {
    gint i;

    *ntptime = 0;
    for (i = 0; i < 7; i++) {
      *ntptime <<= 8;
      *ntptime |= d[i];
    }
  }
  return TRUE;
}

struct _GstRTPHeaderExtensionPrivate
{
  guint dummy;
};

#define GET_PRIV(ext) gst_rtp_header_extension_get_instance_private (ext)

#define gst_rtp_header_extension_parent_class parent_class
G_DEFINE_ABSTRACT_TYPE_WITH_CODE (GstRTPHeaderExtension,
    gst_rtp_header_extension, GST_TYPE_ELEMENT,
    G_ADD_PRIVATE (GstRTPHeaderExtension)
    GST_DEBUG_CATEGORY_INIT (GST_CAT_DEFAULT, "rtphdrext", 0,
        "RTP Header Extensions");
    );

static void
gst_rtp_header_extension_class_init (GstRTPHeaderExtensionClass * klass)
{
}

static void
gst_rtp_header_extension_init (GstRTPHeaderExtension * ext)
{
  ext->ext_id = G_MAXUINT32;
}

/**
 * gst_rtp_header_extension_get_uri:
 * @ext: a #GstRTPHeaderExtension
 *
 * Returns: the RTP extension URI for this object
 */
const gchar *
gst_rtp_header_extension_get_uri (GstRTPHeaderExtension * ext)
{
  GstRTPHeaderExtensionClass *klass;

  g_return_val_if_fail (GST_IS_RTP_HEADER_EXTENSION (ext), NULL);
  klass = GST_RTP_HEADER_EXTENSION_GET_CLASS (ext);
  g_return_val_if_fail (klass->get_uri != NULL, NULL);

  return klass->get_uri (ext);
}

/**
 * gst_rtp_header_extension_get_supported_flags:
 * @ext: a #GstRTPHeaderExtension
 *
 * Returns: the flags supported by this instance of @ext
 */
GstRTPHeaderExtensionFlags
gst_rtp_header_extension_get_supported_flags (GstRTPHeaderExtension * ext)
{
  GstRTPHeaderExtensionClass *klass;

  g_return_val_if_fail (GST_IS_RTP_HEADER_EXTENSION (ext), 0);
  klass = GST_RTP_HEADER_EXTENSION_GET_CLASS (ext);
  g_return_val_if_fail (klass->get_uri != NULL, 0);

  return klass->get_supported_flags (ext);
}

/**
 * gst_rtp_header_extension_get_max_size:
 * @ext: a #GstRTPHeaderExtension
 * @input_meta: a #GstBuffer
 *
 * This is used to know how much data a certain header extension will need for
 * both allocating the resulting data, and deciding how much payload data can
 * be generated.
 *
 * Implemntations should return as accurate a value as is possible using the
 * information given in the input @buffer.
 *
 * Returns: the maximum size of the data written by this extension
 */
gsize
gst_rtp_header_extension_get_max_size (GstRTPHeaderExtension * ext,
    const GstBuffer * input_meta)
{
  GstRTPHeaderExtensionClass *klass;

  g_return_val_if_fail (GST_IS_BUFFER (input_meta), 0);
  g_return_val_if_fail (GST_IS_RTP_HEADER_EXTENSION (ext), 0);
  klass = GST_RTP_HEADER_EXTENSION_GET_CLASS (ext);
  g_return_val_if_fail (klass->get_max_size != NULL, 0);

  return klass->get_max_size (ext, input_meta);
}

/**
 * gst_rtp_header_extension_write:
 * @ext: a #GstRTPHeaderExtension
 * @input_meta: the input #GstBuffer to read information from if necessary
 * @write_flags: #GstRTPHeaderExtensionFlags for how the extension should
 *               be written
 * @output: output RTP #GstBuffer
 * @data: location to write the rtp header extension into
 * @size: size of @data
 *
 * Writes the RTP header extension to @data using information available from
 * the input @buffer.  @data will be sized to be at least the value returned
 * from gst_rtp_header_extension_get_max_size().
 *
 * Returns: the size of the data written, < 0 on failure
 */
gsize
gst_rtp_header_extension_write (GstRTPHeaderExtension * ext,
    const GstBuffer * input_meta, GstRTPHeaderExtensionFlags write_flags,
    GstBuffer * output, guint8 * data, gsize size)
{
  GstRTPHeaderExtensionClass *klass;

  g_return_val_if_fail (GST_IS_BUFFER (input_meta), 0);
  g_return_val_if_fail (GST_IS_BUFFER (output), 0);
  g_return_val_if_fail (gst_buffer_is_writable (output), 0);
  g_return_val_if_fail (data != NULL, 0);
  g_return_val_if_fail (GST_IS_RTP_HEADER_EXTENSION (ext), 0);
  g_return_val_if_fail (ext->ext_id <= MAX_RTP_EXT_ID, 0);
  klass = GST_RTP_HEADER_EXTENSION_GET_CLASS (ext);
  g_return_val_if_fail (klass->write != NULL, 0);

  return klass->write (ext, input_meta, write_flags, output, data, size);
}

/**
 * gst_rtp_header_extension_read:
 * @ext: a #GstRTPHeaderExtension
 * @read_flags: #GstRTPHeaderExtensionFlags for how the extension should
 *               be written
 * @data: location to read the rtp header extension from
 * @size: size of @data
 * @buffer: a #GstBuffer to modify if necessary
 *
 * Read the RTP header extension from @datato @data using information available from
 * the input @buffer.  @data will be sized to be at least the value returned
 * from gst_rtp_header_extension_get_max_size().
 *
 * Returns: whether the extension could be read from @data
 */
gboolean
gst_rtp_header_extension_read (GstRTPHeaderExtension * ext,
    GstRTPHeaderExtensionFlags read_flags, const guint8 * data, gsize size,
    GstBuffer * buffer)
{
  GstRTPHeaderExtensionClass *klass;

  g_return_val_if_fail (GST_IS_BUFFER (buffer), 0);
  g_return_val_if_fail (gst_buffer_is_writable (buffer), 0);
  g_return_val_if_fail (data != NULL, 0);
  g_return_val_if_fail (GST_IS_RTP_HEADER_EXTENSION (ext), 0);
  g_return_val_if_fail (ext->ext_id <= MAX_RTP_EXT_ID, 0);
  klass = GST_RTP_HEADER_EXTENSION_GET_CLASS (ext);
  g_return_val_if_fail (klass->read != NULL, 0);

  return klass->read (ext, read_flags, data, size, buffer);
}

/**
 * gst_rtp_header_extension_get_id:
 * @ext: a #GstRTPHeaderExtension
 *
 * Returns: the RTP extension id configured on @ext
 */
guint
gst_rtp_header_extension_get_id (GstRTPHeaderExtension * ext)
{
  g_return_val_if_fail (GST_IS_RTP_HEADER_EXTENSION (ext), 0);

  return ext->ext_id;
}

/**
 * gst_rtp_header_extension_set_id:
 * @ext: a #GstRTPHeaderExtension
 * @ext_id: The id of this extension
 *
 * sets the RTP extension id on @ext
 */
void
gst_rtp_header_extension_set_id (GstRTPHeaderExtension * ext, guint ext_id)
{
  g_return_if_fail (GST_IS_RTP_HEADER_EXTENSION (ext));
  g_return_if_fail (ext_id < MAX_RTP_EXT_ID);

  ext->ext_id = ext_id;
}

static gboolean
gst_rtsp_ext_list_filter (GstPluginFeature * feature, gpointer user_data)
{
  GstRTPHeaderExtension *ext;
  GstElementFactory *factory;
  gchar *uri = user_data;
  GstElement *element;
  const gchar *klass;
  guint rank;

  /* we only care about element factories */
  if (!GST_IS_ELEMENT_FACTORY (feature))
    return FALSE;

  factory = GST_ELEMENT_FACTORY (feature);

  /* only select elements with autoplugging rank */
  rank = gst_plugin_feature_get_rank (feature);
  if (rank < GST_RANK_MARGINAL)
    return FALSE;

  klass =
      gst_element_factory_get_metadata (factory, GST_ELEMENT_METADATA_KLASS);
  if (!strstr (klass, "Network") || !strstr (klass, "Extension") ||
      !strstr (klass, "RTPHeader"))
    return FALSE;

  element = gst_element_factory_create (factory, NULL);
  if (!element)
    return FALSE;

  if (!GST_IS_RTP_HEADER_EXTENSION (element)) {
    gst_object_unref (element);
    return FALSE;
  }
  ext = GST_RTP_HEADER_EXTENSION (element);

  if (uri && g_strcmp0 (uri, gst_rtp_header_extension_get_uri (ext)) != 0) {
    gst_object_unref (ext);
    return FALSE;
  }
  gst_object_unref (ext);

  return TRUE;
}

/**
 * gst_rtp_get_header_extension_list:
 *
 * Retrieve all the current registered RTP header extensions
 *
 * Returns: (transfer full) (element-type GstRTPHeaderExtension): a #GList of
 *     #GstRTPHeaderExtension. Use gst_plugin_feature_list_free() after use
 */
GList *
gst_rtp_get_header_extension_list (void)
{
  return gst_registry_feature_filter (gst_registry_get (),
      (GstPluginFeatureFilter) gst_rtsp_ext_list_filter, FALSE, NULL);
}

/**
 * gst_rtp_header_extension_create_from_uri:
 * @uri: the rtp header extension URI to search for
 *
 * Returns: (transfer full) (nullable): the #GstRTPHeaderExtension for @uri or %NULL
 */
GstRTPHeaderExtension *
gst_rtp_header_extension_create_from_uri (const gchar * uri)
{
  GList *l;

  l = gst_registry_feature_filter (gst_registry_get (),
      (GstPluginFeatureFilter) gst_rtsp_ext_list_filter, TRUE, (gpointer) uri);
  if (l) {
    GstElementFactory *factory = GST_ELEMENT_FACTORY (l->data);
    GstElement *element = gst_element_factory_create (factory, NULL);
    gst_object_unref (factory);

    return GST_RTP_HEADER_EXTENSION (element);
  }

  return NULL;
}
