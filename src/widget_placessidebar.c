/*
 * widget_placessidebar.c: GTK3 GtkPlacesSidebar XML widget.
 * Copyright (C) 2026 Artur Bednarek <artur@unix.org.pl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include <gtk/gtk.h>
#include "config.h"
#include "gtkdialog.h"
#include "attributes.h"
#include "automaton.h"
#include "tag_attributes.h"
#include "widgets.h"
#include "widget_placessidebar.h"

static void widget_placessidebar_set_value(GtkWidget *widget,
	const gchar *value)
{
	GFile *location;
	gchar *uri;
	gchar *name = g_strdup(value);
	g_strstrip(name);
	if (!*name) {
		gtk_places_sidebar_set_location(GTK_PLACES_SIDEBAR(widget), NULL);
		g_object_set_data(G_OBJECT(widget), "_places_uri", NULL);
		g_free(name);
		return;
	}
	location = g_file_new_for_commandline_arg(name);
	uri = g_file_get_uri(location);
	gtk_places_sidebar_set_location(GTK_PLACES_SIDEBAR(widget), location);
	g_object_set_data_full(G_OBJECT(widget), "_places_uri", uri, g_free);
	g_object_unref(location);
	g_free(name);
}

static void widget_placessidebar_open(GtkPlacesSidebar *sidebar,
	GFile *location, GtkPlacesOpenFlags flags, AttributeSet *Attr)
{
	gchar *uri = g_file_get_uri(location);
	(void)flags;
	gtk_places_sidebar_set_location(sidebar, location);
	g_object_set_data_full(G_OBJECT(sidebar), "_places_uri", uri, g_free);
	widget_signal_executor(GTK_WIDGET(sidebar), Attr, "open-location");
}

static void widget_placessidebar_input(variable *var, const gchar *source,
	gboolean command)
{
	FILE *input = command ? widget_opencommand(source) : fopen(source, "r");
	gchar *line;
	if (input == NULL) {
		gtkdialog_warning("Cannot read places sidebar input '%s'.", source);
		return;
	}
	line = widget_read_line(input);
	if (line != NULL) {
		widget_placessidebar_set_value(var->Widget, line);
		g_free(line);
	}
	if (command)
		widget_closecommand(input, source);
	else
		fclose(input);
}

static void widget_placessidebar_boolean(GtkWidget *widget, tag_attr *attr,
	const gchar *name, void (*setter)(GtkPlacesSidebar *, gboolean))
{
	gchar *value = get_tag_attribute(attr, name);
	gboolean enabled;
	if (value == NULL)
		return;
	if (widget_parse_boolean(value, &enabled))
		setter(GTK_PLACES_SIDEBAR(widget), enabled);
	else
		gtkdialog_warning("Invalid places sidebar %s '%s'.", name, value);
	kill_tag_attribute(attr, name);
}

GtkWidget *widget_placessidebar_create(AttributeSet *Attr, tag_attr *attr,
	gint Type)
{
	GtkWidget *widget = gtk_places_sidebar_new();
	(void)Attr;
	(void)Type;
	if (attr != NULL) {
		widget_placessidebar_boolean(widget, attr, "local-only",
			gtk_places_sidebar_set_local_only);
		widget_placessidebar_boolean(widget, attr, "show-recent",
			gtk_places_sidebar_set_show_recent);
		widget_placessidebar_boolean(widget, attr, "show-desktop",
			gtk_places_sidebar_set_show_desktop);
		widget_placessidebar_boolean(widget, attr, "show-trash",
			gtk_places_sidebar_set_show_trash);
	}
	return widget;
}

gchar *widget_placessidebar_envvar_construct(GtkWidget *widget)
{
	const gchar *uri = g_object_get_data(G_OBJECT(widget), "_places_uri");
	return g_strdup(uri != NULL ? uri : "");
}

void widget_placessidebar_fileselect(variable *var, const char *name,
	const char *value)
{
	(void)name;
	widget_placessidebar_set_value(var->Widget, value);
}

void widget_placessidebar_refresh(variable *var)
{
	GList *element;
	gchar *input;
	gboolean initialised = GPOINTER_TO_INT(g_object_get_data(
		G_OBJECT(var->Widget), "_initialised"));
	if (!initialised && attributeset_is_avail(var->Attributes, ATTR_DEFAULT))
		widget_placessidebar_set_value(var->Widget,
			attributeset_get_first(&element, var->Attributes, ATTR_DEFAULT));
	input = attributeset_get_first(&element, var->Attributes, ATTR_INPUT);
	while (input != NULL) {
		if (input_is_shell_command(input))
			widget_placessidebar_input(var, input + 8, TRUE);
		else if (g_ascii_strncasecmp(input, "file:", 5) == 0 && input[5]) {
			if (!initialised)
				widget_file_monitor_try_create(var, input + 5);
			widget_placessidebar_input(var, input + 5, FALSE);
		}
		input = attributeset_get_next(&element, var->Attributes, ATTR_INPUT);
	}
	if (initialised)
		return;
	if (attributeset_is_avail(var->Attributes, ATTR_WIDTH) ||
		attributeset_is_avail(var->Attributes, ATTR_HEIGHT)) {
		gint width = -1;
		gint height = -1;
		if (attributeset_is_avail(var->Attributes, ATTR_WIDTH))
			width = widget_parse_size_request(attributeset_get_first(&element,
				var->Attributes, ATTR_WIDTH), "places sidebar width");
		if (attributeset_is_avail(var->Attributes, ATTR_HEIGHT))
			height = widget_parse_size_request(attributeset_get_first(&element,
				var->Attributes, ATTR_HEIGHT), "places sidebar height");
		gtk_widget_set_size_request(var->Widget, width, height);
	}
	if (attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "false") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "no") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "0"))
		gtk_widget_set_sensitive(var->Widget, FALSE);
	g_signal_connect(var->Widget, "open-location",
		G_CALLBACK(widget_placessidebar_open), var->Attributes);
}

void widget_placessidebar_save(variable *var)
{
	GList *element;
	gchar *output = attributeset_get_first(&element, var->Attributes, ATTR_OUTPUT);
	FILE *file;
	while (output != NULL &&
		(g_ascii_strncasecmp(output, "file:", 5) != 0 || !output[5]))
		output = attributeset_get_next(&element, var->Attributes, ATTR_OUTPUT);
	if (output == NULL) {
		gtkdialog_warning("<placessidebar> has no <output file> directive.");
		return;
	}
	file = fopen(output + 5, "w");
	if (file == NULL) {
		gtkdialog_warning("Cannot write places sidebar output '%s'.", output + 5);
		return;
	}
	{
		gchar *uri = widget_placessidebar_envvar_construct(var->Widget);
		fputs(uri, file);
		g_free(uri);
	}
	widget_close_output(file, output + 5);
}

void widget_placessidebar_clear(variable *var)
{
	widget_placessidebar_set_value(var->Widget, "");
}

void widget_placessidebar_removeselected(variable *var)
{
	(void)var;
	gtkdialog_warning("<removeselected> is not supported by <placessidebar>.");
}
