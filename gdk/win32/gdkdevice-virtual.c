/* GDK - The GIMP Drawing Kit
 * Copyright (C) 2009 Carlos Garnacho <carlosg@gnome.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include <gdk/gdkwindow.h>

#include <windowsx.h>
#include <objbase.h>

#include "gdkdisplayprivate.h"
#include "gdkdevice-virtual.h"
#include "gdkdevice-win32.h"
#include "gdkwin32.h"
#include "gdkdisplay-win32.h"

G_DEFINE_TYPE (GdkDeviceVirtual, gdk_device_virtual, GDK_TYPE_DEVICE)

void
_gdk_device_virtual_set_active (GdkDevice *device,
				GdkDevice *new_active)
{
  GdkDeviceVirtual *virtual = GDK_DEVICE_VIRTUAL (device);
  int n_axes, i;
  GdkAtom label_atom;
  GdkAxisUse use;
  gdouble min_value, max_value, resolution;

  if (virtual->active_device == new_active)
    return;

  virtual->active_device = new_active;

  if (gdk_device_get_source (device) != GDK_SOURCE_KEYBOARD)
    {
      _gdk_device_reset_axes (device);
      n_axes = gdk_device_get_n_axes (new_active);
      for (i = 0; i < n_axes; i++)
	{
	  _gdk_device_get_axis_info (new_active, i,
				     &label_atom, &use,
				     &min_value, &max_value, &resolution);
	  _gdk_device_add_axis (device,
				label_atom, use,
				min_value, max_value, resolution);
	}
    }

  g_signal_emit_by_name (G_OBJECT (device), "changed");
}

static gboolean
gdk_device_virtual_get_history (GdkDevice      *device,
				GdkWindow      *window,
				guint32         start,
				guint32         stop,
				GdkTimeCoord ***events,
				gint           *n_events)
{
  /* History is only per slave device */
  return FALSE;
}

static void
gdk_device_virtual_get_state (GdkDevice       *device,
			      GdkWindow       *window,
			      gdouble         *axes,
			      GdkModifierType *mask)
{
  GdkDeviceVirtual *virtual = GDK_DEVICE_VIRTUAL (device);
  GdkDevice *active = virtual->active_device;

  GDK_DEVICE_GET_CLASS (active)->get_state (active,
					    window, axes, mask);
}

static void
gdk_device_virtual_set_window_cursor (GdkDevice *device,
                                      GdkWindow *window,
                                      GdkCursor *cursor)
{
  if (cursor != NULL)
    {
      GdkDisplay *display = gdk_window_get_display (window);
      HCURSOR hcursor = NULL;

      if (display != NULL)
        hcursor = gdk_win32_display_get_hcursor (display, cursor);

      if (hcursor != NULL)
        SetCursor (hcursor);   
    }
}

static void
gdk_device_virtual_warp (GdkDevice *device,
			 gdouble   x,
			 gdouble   y)
{
  SetCursorPos (x - _gdk_offset_x, y - _gdk_offset_y);
}

static void
gdk_device_virtual_query_state (GdkDevice        *device,
				GdkWindow        *window,
				GdkWindow       **child_window,
				gdouble          *root_x,
				gdouble          *root_y,
				gdouble          *win_x,
				gdouble          *win_y,
				GdkModifierType  *mask)
{
  GdkDeviceVirtual *virtual = GDK_DEVICE_VIRTUAL (device);

  _gdk_device_query_state (virtual->active_device,
			   window, child_window,
			   root_x, root_y,
			   win_x, win_y,
			   mask);
}

static GdkGrabStatus
gdk_device_virtual_grab (GdkDevice    *device,
			 GdkWindow    *window,
			 gboolean      owner_events,
			 GdkEventMask  event_mask,
			 GdkWindow    *confine_to,
			 GdkCursor    *cursor,
			 guint32       time_)
{
  GdkWindowImplWin32 *impl = GDK_WINDOW_IMPL_WIN32 (window->impl);

  if (gdk_device_get_source (device) != GDK_SOURCE_KEYBOARD)
    {
      GdkWin32Display *display = GDK_WIN32_DISPLAY (gdk_device_get_display (device));
      if (display->grab_cursor != NULL)
        {
          if (GetCursor () == g_hash_table_lookup (display->cursors, display->grab_cursor))
	        SetCursor (NULL);
        }

      g_set_object (&display->grab_cursor, cursor);

      if (display->grab_cursor != NULL)
        SetCursor (g_hash_table_lookup (display->cursors, display->grab_cursor));
      else
        SetCursor (LoadCursor (NULL, IDC_ARROW));

      SetCapture (GDK_WINDOW_HWND (window));
    }

  return GDK_GRAB_SUCCESS;
}

static void
gdk_device_virtual_ungrab (GdkDevice *device,
                           guint32    time_)
{
  GdkDeviceGrabInfo *info;
  GdkDisplay *display;
  GdkWin32Display *win32_display;

  display = gdk_device_get_display (device);
  win32_display = GDK_WIN32_DISPLAY (display);
  info = _gdk_display_get_last_device_grab (display, device);

  if (info)
    info->serial_end = 0;

  if (gdk_device_get_source (device) != GDK_SOURCE_KEYBOARD)
    {
      if (win32_display->grab_cursor != NULL)
        {
          if (GetCursor () == g_hash_table_lookup (win32_display->cursors, win32_display->grab_cursor))
            SetCursor (NULL);
        }

      g_clear_object (&win32_display->grab_cursor);

      ReleaseCapture ();
    }

  _gdk_display_device_grab_update (display, device, device, 0);
}

static void
gdk_device_virtual_select_window_events (GdkDevice    *device,
					 GdkWindow    *window,
					 GdkEventMask  event_mask)
{
}

static void
gdk_device_virtual_class_init (GdkDeviceVirtualClass *klass)
{
  GdkDeviceClass *device_class = GDK_DEVICE_CLASS (klass);

  device_class->get_history = gdk_device_virtual_get_history;
  device_class->get_state = gdk_device_virtual_get_state;
  device_class->set_window_cursor = gdk_device_virtual_set_window_cursor;
  device_class->warp = gdk_device_virtual_warp;
  device_class->query_state = gdk_device_virtual_query_state;
  device_class->grab = gdk_device_virtual_grab;
  device_class->ungrab = gdk_device_virtual_ungrab;
  device_class->window_at_position = _gdk_device_win32_window_at_position;
  device_class->select_window_events = gdk_device_virtual_select_window_events;
}

static void
gdk_device_virtual_init (GdkDeviceVirtual *device_virtual)
{
}
