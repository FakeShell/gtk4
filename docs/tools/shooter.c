#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <gdkx.h>
#include <cairo-xlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>
#include <X11/extensions/shape.h>

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <locale.h>
#include "widgets.h"
#include "shadow.h"

static GdkPixbuf *
add_border (GdkPixbuf *pixbuf)
{
  GdkPixbuf *retval;

  retval = gdk_pixbuf_new (GDK_COLORSPACE_RGB, TRUE, 8,
                           gdk_pixbuf_get_width (pixbuf) + 2,
                           gdk_pixbuf_get_height (pixbuf) + 2);

  /* Fill with solid black */
  gdk_pixbuf_fill (retval, 0xFF);
  gdk_pixbuf_copy_area (pixbuf,
                        0, 0,
                        gdk_pixbuf_get_width (pixbuf),
                        gdk_pixbuf_get_height (pixbuf),
                        retval, 1, 1);

  g_object_unref (pixbuf);

  return retval;
}

static GMainLoop *loop;

static gboolean
quit_when_idle (gpointer loop)
{
  g_main_loop_quit (loop);

  return G_SOURCE_REMOVE;
}

static void
draw_paintable (GdkPaintable *paintable,
                gpointer      out_surface)
{
  GtkSnapshot *snapshot;
  GskRenderNode *node;
  cairo_surface_t *surface;
  cairo_t *cr;

  snapshot = gtk_snapshot_new ();
  gdk_paintable_snapshot (paintable,
                          snapshot,
                          gdk_paintable_get_intrinsic_width (paintable),
                          gdk_paintable_get_intrinsic_height (paintable));
  node = gtk_snapshot_free_to_node (snapshot);

  /* If the window literally draws nothing, we assume it hasn't been mapped yet and as such
   * the invalidations were only side effects of resizes.
   */
  if (node == NULL)
    return;

  surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32,
                                        gdk_paintable_get_intrinsic_width (paintable),
                                        gdk_paintable_get_intrinsic_height (paintable));

  cr = cairo_create (surface);
  gsk_render_node_draw (node, cr);
  cairo_destroy (cr);
  gsk_render_node_unref (node);

  g_signal_handlers_disconnect_by_func (paintable, draw_paintable, out_surface);

  *(cairo_surface_t **) out_surface = surface;

  g_idle_add (quit_when_idle, loop);
}

static cairo_surface_t *
snapshot_widget (GtkWidget *widget)
{
  GdkPaintable *paintable;
  cairo_surface_t *surface;

  g_assert (gtk_widget_get_realized (widget));

  loop = g_main_loop_new (NULL, FALSE);

  /* We wait until the widget is drawn for the first time.
   *
   * We also use an inhibit mechanism, to give module functions a chance
   * to delay the snapshot.
   */
  paintable = gtk_widget_paintable_new (widget);
  g_signal_connect (paintable, "invalidate-contents", G_CALLBACK (draw_paintable), &surface);
  g_main_loop_run (loop);

  g_main_loop_unref (loop);
  g_object_unref (paintable);
  gtk_window_destroy (GTK_WINDOW (widget));

  return surface;
}

int
main (int argc, char **argv)
{
  GList *toplevels, *node;

  gtk_init ();

  toplevels = get_all_widgets ();

  for (node = toplevels; node; node = node->next)
    {
      WidgetInfo *info;
      char *filename;
      cairo_surface_t *surface;
      GdkPixbuf *pixbuf;

      info = node->data;

      gtk_widget_show (info->window);

      if (info->no_focus)
        {
          gtk_root_set_focus (GTK_ROOT (info->window), NULL);
          gdk_clipboard_set_content (gdk_display_get_primary_clipboard (gdk_display_get_default ()),
                                     NULL);
        }

      //while (1) g_main_context_iteration (NULL, 1);

      surface = snapshot_widget (info->window);

      pixbuf = gdk_pixbuf_get_from_surface (surface, 0, 0,
                                            cairo_image_surface_get_width (surface),
                                            cairo_image_surface_get_height (surface));

      if (!info->include_decorations)
        pixbuf = add_border (pixbuf);

      pixbuf = add_shadow (pixbuf);

      filename = g_strdup_printf ("./%s.png", info->name);

      gdk_pixbuf_save (pixbuf, filename, "png", NULL, NULL);
      g_free (filename);

      g_object_unref (pixbuf);
      cairo_surface_destroy (surface);
    }

  return 0;
}
