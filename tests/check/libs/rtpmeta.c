/* GStreamer RTP meta unit tests
 * Copyright (C) 2016  Stian Selnes <stian@pexip.com>
 * Copyright (C) 2017  Havard Graff <havard@pexip.com>
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

#include <gst/gst.h>
#include <gst/check/gstcheck.h>
#include <gst/rtp/gstrtpmeta.h>
#include <gst/rtp/gstrtpaudiolevelmeta.h>
#include <gst/rtp/gstrtpbuffer.h>

GST_START_TEST (test_rtp_source_meta_set_get_sources)
{
  GstBuffer *buffer;
  GstRTPSourceMeta *meta;
  guint32 ssrc = 1000, ssrc2 = 2000;
  const guint32 csrc[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14
  };

  buffer = gst_buffer_new ();
  meta = gst_buffer_add_rtp_source_meta (buffer, &ssrc, csrc, 12);

  fail_unless_equals_int (gst_rtp_source_meta_get_source_count (meta), 12 + 1);
  fail_unless (meta->ssrc_valid);
  fail_unless_equals_int (meta->ssrc, ssrc);
  for (gint i = 0; i < 12; i++)
    fail_unless_equals_int (meta->csrc[i], csrc[i]);

  /* Unset the ssrc */
  fail_unless (gst_rtp_source_meta_set_ssrc (meta, NULL));
  fail_unless_equals_int (gst_rtp_source_meta_get_source_count (meta), 12);
  fail_if (meta->ssrc_valid);

  /* Set the ssrc again */
  fail_unless (gst_rtp_source_meta_set_ssrc (meta, &ssrc2));
  fail_unless_equals_int (gst_rtp_source_meta_get_source_count (meta), 12 + 1);
  fail_unless (meta->ssrc_valid);
  fail_unless_equals_int (meta->ssrc, ssrc2);

  /* Append multiple csrcs */
  fail_unless (gst_rtp_source_meta_append_csrc (meta, &csrc[12], 2));
  fail_unless_equals_int (gst_rtp_source_meta_get_source_count (meta), 14 + 1);
  for (gint i = 0; i < 14; i++)
    fail_unless_equals_int (meta->csrc[i], csrc[i]);

  gst_buffer_unref (buffer);
}

GST_END_TEST;

GST_START_TEST (test_rtp_source_meta_set_get_max_sources)
{
  GstBuffer *buffer;
  GstRTPSourceMeta *meta;
  guint32 ssrc = 1000;
  const guint32 csrc[16] = { 0, };

  buffer = gst_buffer_new ();
  meta = gst_buffer_add_rtp_source_meta (buffer, &ssrc, csrc, 14);

  fail_unless_equals_int (gst_rtp_source_meta_get_source_count (meta), 14 + 1);
  fail_unless_equals_int (meta->csrc_count, 14);
  fail_unless (meta->ssrc_valid);
  fail_unless_equals_int (meta->ssrc, ssrc);

  /* Append one more csrc */
  /* The source count should cap at 15 for convenient use with
   * gst_rtp_buffer-functions! */
  fail_unless (gst_rtp_source_meta_append_csrc (meta, &csrc[14], 1));
  fail_unless_equals_int (gst_rtp_source_meta_get_source_count (meta), 15);
  fail_unless_equals_int (meta->csrc_count, 15);

  /* Try to append one more csrc, but we've reached max */
  fail_if (gst_rtp_source_meta_append_csrc (meta, &csrc[15], 1));
  fail_unless_equals_int (gst_rtp_source_meta_get_source_count (meta), 15);
  fail_unless_equals_int (meta->csrc_count, 15);

  gst_buffer_unref (buffer);
}

GST_END_TEST;

GST_START_TEST (test_rtp_audio_level_meta_add_get)
{
  GstBuffer *in;
  GstBuffer *rtp;
  GstBuffer *out;
  GstRTPAudioLevelMeta *in_meta;
  GstRTPAudioLevelMeta *rtp_meta;
  GstRTPAudioLevelMeta *out_meta;
  GstRTPBuffer rtp_buf = GST_RTP_BUFFER_INIT;
  gint id = __i__;
  gint level = g_random_int_range (0, 128);
  gboolean voice_activity = g_random_int_range (0, 2);

  in = gst_buffer_new ();
  rtp = gst_rtp_buffer_new_allocate (1, 0, 0);
  out = gst_buffer_new ();

  /* add audio-level meta to a buffer (-100dB, with Voice Activity) */
  in_meta = gst_buffer_add_rtp_audio_level_meta (in, level, voice_activity);
  fail_unless (in_meta);

  rtp_meta = gst_buffer_get_rtp_audio_level_meta (in);
  fail_unless (rtp_meta);
  fail_unless_equals_int (in_meta->level, rtp_meta->level);
  fail_unless (in_meta->voice_activity == rtp_meta->voice_activity);

  /* write this meta into the rtp extensionheader */
  gst_rtp_buffer_map (rtp, GST_MAP_READWRITE, &rtp_buf);
  fail_unless (gst_rtp_audio_level_meta_add_one_byte_ext (rtp_meta, &rtp_buf,
          id));
  gst_rtp_buffer_unmap (&rtp_buf);

  /* extract the extensionheader as a audio-level meta */
  gst_rtp_buffer_map (rtp, GST_MAP_READWRITE, &rtp_buf);
  out_meta =
      gst_buffer_extract_rtp_audio_level_meta_one_byte_ext (out, &rtp_buf, id);
  fail_unless (out_meta);
  gst_rtp_buffer_unmap (&rtp_buf);

  /* verify the audio-level information is intact */
  fail_unless_equals_int (in_meta->level, out_meta->level);
  fail_unless (in_meta->voice_activity == out_meta->voice_activity);

  gst_buffer_unref (in);
  gst_buffer_unref (rtp);
  gst_buffer_unref (out);
}

GST_END_TEST;

static Suite *
rtp_meta_suite (void)
{
  Suite *s = suite_create ("rtp_meta_tests");
  TCase *tc_chain;

  suite_add_tcase (s, (tc_chain = tcase_create ("GstRTPSourceMeta")));
  tcase_add_test (tc_chain, test_rtp_source_meta_set_get_sources);
  tcase_add_test (tc_chain, test_rtp_source_meta_set_get_max_sources);

  suite_add_tcase (s, (tc_chain = tcase_create ("GstRTPAudioLevelMeta")));
  tcase_add_loop_test (tc_chain, test_rtp_audio_level_meta_add_get, 1, 15);

  return s;
}

GST_CHECK_MAIN (rtp_meta)
