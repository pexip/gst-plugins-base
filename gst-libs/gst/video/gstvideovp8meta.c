/* GStreamer
 * Copyright (C) <2017> Pexip AS
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
 *
 * Author: John-Mark Bell <jmb@pexip.com>
 */

#include "gstvideovp8meta.h"

/**
 * gst_buffer_add_video_vp8_meta:
 * @buffer: a #GstBuffer
 *
 * Attaches VP8 metadata to @buffer with defaults for uncommon settings.
 *
 * Returns: (transfer none): the #GstVideoVP8Meta on @buffer.
 *
 * Since: 1.12
 */
GstVideoVP8Meta *
gst_buffer_add_video_vp8_meta (GstBuffer * buffer)
{
  return gst_buffer_add_video_vp8_meta_full (buffer, FALSE, FALSE, 0, 0);
}

/**
 * gst_buffer_add_video_vp8_meta_full:
 * @buffer: a #GstBuffer
 * @use_temporal_scaling: whether temporal scaling is in use
 * @layer_sync: whether the buffer contains part of a frame that depends only
 *              on the base layer
 * @temporal_layer_id: temporal layer id of the frame (fragment in this buffer
 * @tl0picidx: temporal layer zero index
 *
 * Attaches VP8 metadata to @buffer.
 *
 * Returns: (transfer none): the #GstVideoVP8Meta on @buffer.
 *
 * Since: 1.12
 */
GstVideoVP8Meta *
gst_buffer_add_video_vp8_meta_full (GstBuffer * buffer,
    gboolean use_temporal_scaling, gboolean layer_sync,
    guint8 temporal_layer_id, guint8 tl0picidx)
{
  GstVideoVP8Meta *meta;

  g_return_val_if_fail (buffer != NULL, NULL);

  meta = (GstVideoVP8Meta *) gst_buffer_add_meta (buffer,
      GST_VIDEO_VP8_META_INFO, NULL);
  if (!meta)
      return NULL;

  meta->use_temporal_scaling = use_temporal_scaling;
  meta->layer_sync = layer_sync;
  meta->temporal_layer_id = temporal_layer_id;
  meta->tl0picidx = tl0picidx;

  return meta;
}

GstVideoVP8Meta *
gst_buffer_get_video_vp8_meta (GstBuffer * buffer)
{
  return (GstVideoVP8Meta *) gst_buffer_get_meta (buffer,
      gst_video_vp8_meta_api_get_type());
}

static gboolean
gst_video_vp8_meta_transform (GstBuffer * dst, GstMeta * meta,
    GstBuffer * src, GQuark type, gpointer data)
{
  if (GST_META_TRANSFORM_IS_COPY (type)) {
    GstVideoVP8Meta *smeta = (GstVideoVP8Meta *) meta;
    GstVideoVP8Meta *dmeta;

    dmeta = gst_buffer_add_video_vp8_meta_full (dst,
        smeta->use_temporal_scaling, smeta->layer_sync,
        smeta->temporal_layer_id, smeta->tl0picidx);
    if (dmeta == NULL)
      return FALSE;
  } else {
    /* return FALSE, if transform type is not supported */
    return FALSE;
  }

  return TRUE;
}

GType
gst_video_vp8_meta_api_get_type (void)
{
  static volatile GType type = 0;
  static const gchar *tags[] = { NULL };

  if (g_once_init_enter (&type)) {
    GType _type = gst_meta_api_type_register ("GstVideoVP8MetaAPI", tags);
    g_once_init_leave (&type, _type);
  }
  return type;
}

static gboolean
gst_video_vp8_meta_init (GstMeta * meta, gpointer params,
    GstBuffer * buffer)
{
  GstVideoVP8Meta *dmeta = (GstVideoVP8Meta *) meta;

  dmeta->use_temporal_scaling = FALSE;
  dmeta->layer_sync = FALSE;
  dmeta->temporal_layer_id = 0;
  dmeta->tl0picidx = 0;

  return TRUE;
}

const GstMetaInfo *
gst_video_vp8_meta_get_info (void)
{
  static const GstMetaInfo *video_vp8_meta_info = NULL;

  if (g_once_init_enter (&video_vp8_meta_info)) {
    const GstMetaInfo *meta =
        gst_meta_register (GST_VIDEO_VP8_META_API_TYPE,
        "GstVideoVP8Meta",
        sizeof (GstVideoVP8Meta),
        gst_video_vp8_meta_init,
        (GstMetaFreeFunction) NULL,
        gst_video_vp8_meta_transform);
    g_once_init_leave (&video_vp8_meta_info, meta);
  }
  return video_vp8_meta_info;
}

