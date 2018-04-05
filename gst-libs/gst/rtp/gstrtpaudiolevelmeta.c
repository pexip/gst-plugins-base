/* GStreamer
 * Copyright (C) <2017> Havard Graff <havard@pexip.com>
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
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gstrtpaudiolevelmeta.h"

/**
 * gst_buffer_add_rtp_audio_level_meta:
 * @buffer: a #GstBuffer
 * @level: the -dBov from 0-127 (127 is silence).
 * @voice_activity: whether the buffer contains voice activity.
 *
 * Attaches RTP audio level information to @buffer. (RFC 6464)
 *
 * Returns: (transfer none): the #GstRTPAudioLevelMeta on @buffer.
 *
 * Since: 1.12
 */
GstRTPAudioLevelMeta *
gst_buffer_add_rtp_audio_level_meta (GstBuffer * buffer, guint8 level,
    gboolean voice_activity)
{
  GstRTPAudioLevelMeta *meta;

  g_return_val_if_fail (buffer != NULL, NULL);

  meta = (GstRTPAudioLevelMeta *) gst_buffer_add_meta (buffer,
      GST_RTP_AUDIO_LEVEL_META_INFO, NULL);
  if (!meta)
    return NULL;

  meta->level = level;
  meta->voice_activity = voice_activity;

  return meta;
}

/**
 * gst_buffer_extract_rtp_audio_level_meta_one_byte_ext:
 * @buffer: a #GstBuffer
 * @rtp: a #GstRTPBuffer
 * @id: The ID of the header extension to be read (between 1 and 14).
 *
 * Tries to extract RTP audio level information from the RTP packet (RFC 6464),
 * and adds this as meta on to the @buffer.
 *
 * Returns: (transfer none): the #GstRTPAudioLevelMeta on @buffer.
 *
 * Since: 1.12
 */
GstRTPAudioLevelMeta *
gst_buffer_extract_rtp_audio_level_meta_one_byte_ext (GstBuffer * buffer,
    GstRTPBuffer * rtp, guint8 id)
{
  GstRTPAudioLevelMeta *meta = NULL;
  gpointer data;

  g_return_val_if_fail (buffer != NULL, NULL);
  g_return_val_if_fail (rtp != NULL, NULL);
  g_return_val_if_fail (id > 0, NULL);
  g_return_val_if_fail (id <= 14, NULL);

  if (gst_rtp_buffer_get_extension_onebyte_header (rtp, id, 0, &data, NULL)) {
    guint8 val = *((guint8 *) data);
    guint8 level = val & 0x7F;
    gboolean voice_activity = (val & 0x80) >> 7;
    meta = gst_buffer_add_rtp_audio_level_meta (buffer, level, voice_activity);
  }

  return meta;
}

/**
 * gst_buffer_get_rtp_audio_level_meta:
 * @buffer: a #GstBuffer
 *
 * Find the #GstRTPAudioLevelMeta on @buffer.
 *
 * Returns: (transfer none): the #GstRTPAudioLevelMeta or %NULL when there
 * is no such metadata on @buffer.
 *
 * Since: 1.12
 */
GstRTPAudioLevelMeta *
gst_buffer_get_rtp_audio_level_meta (GstBuffer * buffer)
{
  return (GstRTPAudioLevelMeta *) gst_buffer_get_meta (buffer,
      gst_rtp_audio_level_meta_api_get_type ());
}

/**
 * gst_rtp_audio_level_meta_add_one_byte_ext:
 * @rtp: a #GstRTPBuffer
 * @id: The ID of the header extension to be added (between 1 and 14).
 *
 * Tries to write the audio level information using the one byte
 * extension header.
 *
 * Returns: %TRUE if the extension-header could be added, %FALSE otherwise.
 *
 * Since: 1.12
 */
gboolean
gst_rtp_audio_level_meta_add_one_byte_ext (GstRTPAudioLevelMeta * meta,
    GstRTPBuffer * rtp, guint8 id)
{
  guint8 data;

  g_return_val_if_fail (meta->level <= 127, FALSE);
  g_return_val_if_fail (id > 0, FALSE);
  g_return_val_if_fail (id <= 14, FALSE);

  data = (meta->level & 0x7F) | (meta->voice_activity << 7);

  return gst_rtp_buffer_add_extension_onebyte_header (rtp, id, &data, 1);
}

/**
 * gst_rtp_audio_level_meta_add_two_byte_ext:
 * @rtp: a #GstRTPBuffer
 * @id: The ID of the header extension to be added (between 1 and 14).
 *
 * Tries to write the audio level information using the two byte
 * extension header.
 *
 * Returns: %TRUE if the extension-header could be added, %FALSE otherwise.
 *
 * Since: 1.12
 */
gboolean
gst_rtp_audio_level_meta_add_two_byte_ext (GstRTPAudioLevelMeta * meta,
    GstRTPBuffer * rtp, guint8 id)
{
  guint16 data;

  g_return_val_if_fail (meta->level <= 127, FALSE);
  g_return_val_if_fail (id > 0, FALSE);
  g_return_val_if_fail (id <= 14, FALSE);

  data = ((meta->level & 0x7F) | (meta->voice_activity << 7)) << 8;

  return gst_rtp_buffer_add_extension_twobytes_header (rtp, 0, id, &data, 2);
}


static gboolean
gst_rtp_audio_level_meta_transform (GstBuffer * dst, GstMeta * meta,
    GstBuffer * src, GQuark type, gpointer data)
{
  if (GST_META_TRANSFORM_IS_COPY (type)) {
    GstRTPAudioLevelMeta *smeta = (GstRTPAudioLevelMeta *) meta;
    GstRTPAudioLevelMeta *dmeta;

    dmeta = gst_buffer_add_rtp_audio_level_meta (dst, smeta->level,
        smeta->voice_activity);
    if (dmeta == NULL)
      return FALSE;
  } else {
    /* return FALSE, if transform type is not supported */
    return FALSE;
  }

  return TRUE;
}

GType
gst_rtp_audio_level_meta_api_get_type (void)
{
  static volatile GType type = 0;
  static const gchar *tags[] = { NULL };

  if (g_once_init_enter (&type)) {
    GType _type = gst_meta_api_type_register ("GstRTPAudioLevelMetaAPI", tags);
    g_once_init_leave (&type, _type);
  }
  return type;
}

static gboolean
gst_rtp_audio_level_meta_init (GstMeta * meta, gpointer params,
    GstBuffer * buffer)
{
  GstRTPAudioLevelMeta *dmeta = (GstRTPAudioLevelMeta *) meta;

  dmeta->level = 127;
  dmeta->voice_activity = FALSE;

  return TRUE;
}

const GstMetaInfo *
gst_rtp_audio_level_meta_get_info (void)
{
  static const GstMetaInfo *rtp_audio_level_meta_info = NULL;

  if (g_once_init_enter (&rtp_audio_level_meta_info)) {
    const GstMetaInfo *meta =
        gst_meta_register (GST_RTP_AUDIO_LEVEL_META_API_TYPE,
        "GstRTPAudioLevelMeta",
        sizeof (GstRTPAudioLevelMeta),
        gst_rtp_audio_level_meta_init,
        (GstMetaFreeFunction) NULL,
        gst_rtp_audio_level_meta_transform);
    g_once_init_leave (&rtp_audio_level_meta_info, meta);
  }
  return rtp_audio_level_meta_info;
}
