/* GStreamer
 * Copyright (C) <2015> Stian Selnes <stian@pexip.com>
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

#include "gstrtpmeta.h"
#include <string.h>

/**
 * gst_buffer_add_rtp_source_meta:
 * @buffer: a #GstBuffer
 * @ssrc: (allow-none) (transfer full): pointer to the SSRC
 * @csrc: (allow-none) (transfer full): pointer to the CSRCs
 * @csrc_count: number of elements in @csrc
 *
 * Attaches RTP source information to @buffer.
 *
 * @ssrc and @csrc must be freeable with g_free().
 *
 * Returns: (transfer none): the #GstRTPSourceMeta on @buffer.
 *
 * Since: 1.8
 */
GstRTPSourceMeta *
gst_buffer_add_rtp_source_meta (GstBuffer * buffer, guint32 * ssrc,
    guint * csrc, guint csrc_count)
{
  GstRTPSourceMeta *meta;

  g_return_val_if_fail (buffer != NULL, NULL);
  g_return_val_if_fail (csrc_count <= 15, NULL);
  g_return_val_if_fail ((csrc == NULL && csrc_count == 0) ||
      (csrc != NULL && csrc_count != 0), NULL);

  meta = (GstRTPSourceMeta *) gst_buffer_add_meta (buffer,
      GST_RTP_SOURCE_META_INFO, NULL);
  if (!meta)
    return NULL;

  meta->ssrc = ssrc;
  meta->csrc = csrc;
  meta->csrc_count = csrc_count;

  return meta;
}

/**
 * gst_buffer_get_rtp_source_meta:
 * @buffer: a #GstBuffer
 *
 * Find the #GstRTPSourceMeta on @buffer.
 *
 * Returns: (transfer none): the #GstRTPSourceMeta or %NULL when there
 * is no such metadata on @buffer.
 *
 * Since: 1.8
 */
GstRTPSourceMeta *
gst_buffer_get_rtp_source_meta (GstBuffer * buffer)
{
  return (GstRTPSourceMeta *) gst_buffer_get_meta (buffer,
      gst_rtp_source_meta_api_get_type());
}

static void
gst_rtp_source_meta_free (GstMeta * meta, GstBuffer * buffer)
{
  GstRTPSourceMeta * m = (GstRTPSourceMeta *) meta;

  g_free (m->ssrc);
  g_free (m->csrc);
  m->ssrc = NULL;
  m->csrc = NULL;
}

static gboolean
gst_rtp_source_meta_transform (GstBuffer * dst, GstMeta * meta,
    GstBuffer * src, GQuark type, gpointer data)
{
  if (GST_META_TRANSFORM_IS_COPY (type)) {
    GstRTPSourceMeta *smeta = (GstRTPSourceMeta *) meta;
    GstRTPSourceMeta *dmeta;
    guint32 *ssrc = NULL;
    guint32 *csrc = NULL;

    /* FIXME: HACK: Buffer should be writable at this point. */
    if (!gst_buffer_is_writable (dst))
      return FALSE;

    if (smeta->ssrc != NULL) {
      ssrc = g_new (guint32, 1);
      *ssrc = *smeta->ssrc;
    }
    if (smeta->csrc != NULL) {
      csrc = g_new (guint32, smeta->csrc_count);
      memcpy (csrc, smeta->csrc, sizeof (guint32) * smeta->csrc_count);
    }

    dmeta = gst_buffer_add_rtp_source_meta (dst, ssrc, csrc, smeta->csrc_count);
    if (dmeta == NULL)
      return FALSE;
  } else {
    /* return FALSE, if transform type is not supported */
    return FALSE;
  }

  return TRUE;
}

/**
 * gst_rtp_source_meta_get_source_count:
 * @meta: a #GstRTPSourceMeta
 *
 * Count the total number of RTP sources found in @meta, both SSRC and CSRC.
 *
 * Returns: The number of RTP sources
 *
 * Since: 1.8
 */
guint
gst_rtp_source_meta_get_source_count (const GstRTPSourceMeta * meta)
{
  guint ssrc_count = meta->ssrc != NULL ? 1 : 0;
  return meta->csrc_count + ssrc_count;
}

GType
gst_rtp_source_meta_api_get_type (void)
{
  static volatile GType type = 0;
  static const gchar *tags[] = { "origin", NULL };

  if (g_once_init_enter (&type)) {
    GType _type = gst_meta_api_type_register ("GstRTPSourceMetaAPI", tags);
    g_once_init_leave (&type, _type);
  }
  return type;
}

const GstMetaInfo *
gst_rtp_source_meta_get_info (void)
{
  static const GstMetaInfo *rtp_source_meta_info = NULL;

  if (g_once_init_enter (&rtp_source_meta_info)) {
    const GstMetaInfo *meta =
      gst_meta_register (GST_RTP_SOURCE_META_API_TYPE,
          "GstRTPSourceMeta",
          sizeof (GstRTPSourceMeta),
          NULL,
          gst_rtp_source_meta_free,
          gst_rtp_source_meta_transform);
    g_once_init_leave (&rtp_source_meta_info, meta);
  }
  return rtp_source_meta_info;
}
