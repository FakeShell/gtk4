/* GStreamer
 *
 * Copyright (C) 2014-2015 Sebastian Dröge <sebastian@centricular.com>
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

#include "gstplay-video-renderer.h"
#include "gstplay-video-renderer-private.h"

G_DEFINE_INTERFACE (GtkGstPlayVideoRenderer, gtk_gst_play_video_renderer,
    G_TYPE_OBJECT);

static void
gtk_gst_play_video_renderer_default_init (G_GNUC_UNUSED
    GtkGstPlayVideoRendererInterface * iface)
{

}

GstElement *
gtk_gst_play_video_renderer_create_video_sink (GtkGstPlayVideoRenderer * self,
    GtkGstPlay * play)
{
  GtkGstPlayVideoRendererInterface *iface;

  g_return_val_if_fail (GST_IS_PLAY_VIDEO_RENDERER (self), NULL);
  iface = GTL_GST_PLAY_VIDEO_RENDERER_GET_INTERFACE (self);
  g_return_val_if_fail (iface->create_video_sink != NULL, NULL);

  return iface->create_video_sink (self, play);
}
