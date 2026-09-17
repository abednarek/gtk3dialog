/*
 * widget_socket.c: GTK3 XEmbed socket widget.
 * Gtkdialog - A small utility for fast and easy GUI building.
 * Copyright (C) 2026       Artur Bednarek <artur@unix.org.pl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <gtk/gtk.h>
#ifdef GDK_WINDOWING_X11
#include <gtk/gtkx.h>
#endif

#include "config.h"
#include "gtkdialog.h"
#include "attributes.h"
#include "automaton.h"
#include "signals.h"
#include "tag_attributes.h"
#include "widgets.h"
#include "widget_socket.h"

#ifdef GDK_WINDOWING_X11
static void
widget_socket_plug_added(GtkSocket *socket, gpointer user_data)
{
	widget_signal_executor(GTK_WIDGET(socket), user_data, "plug-added");
}

static gboolean
widget_socket_plug_removed(GtkSocket *socket, gpointer user_data)
{
	widget_signal_executor(GTK_WIDGET(socket), user_data, "plug-removed");

	/* GtkSocket's default handler destroys the socket when its client exits.
	 * Keep the declared widget alive so another XEmbed client can attach. */
	return TRUE;
}
#endif

GtkWidget *
widget_socket_create(AttributeSet *Attr, tag_attr *attr, gint Type)
{
	(void)attr;
	(void)Type;
#ifdef GDK_WINDOWING_X11
	GtkWidget *widget;

	widget = gtk_socket_new();
	g_signal_connect(widget, "plug-added",
		G_CALLBACK(widget_socket_plug_added), Attr);
	g_signal_connect(widget, "plug-removed",
		G_CALLBACK(widget_socket_plug_removed), Attr);
	return widget;
#else
	(void)Attr;
	gtkdialog_warning("The <socket> widget requires the GTK3 X11 backend.");
	return gtk_drawing_area_new();
#endif
}

gchar *
widget_socket_envvar_construct(GtkWidget *widget)
{
#ifdef GDK_WINDOWING_X11
	Window socket_id;

	if (!gtk_widget_get_realized(widget) || !GTK_IS_SOCKET(widget))
		return g_strdup("");

	socket_id = gtk_socket_get_id(GTK_SOCKET(widget));
	return g_strdup_printf("%lu", (gulong)socket_id);
#else
	(void)widget;
	return g_strdup("");
#endif
}
