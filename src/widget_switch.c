/*
 * widget_switch.c: GTK3 GtkSwitch XML widget.
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
#include "widget_switch.h"

static void widget_switch_set_value(GtkWidget *widget, const gchar *value)
{
	gboolean active;
	if (!widget_parse_boolean(value, &active)) {
		gtkdialog_warning("Invalid switch Boolean value '%s'.", value);
		return;
	}
	gtk_switch_set_active(GTK_SWITCH(widget), active);
}

static void widget_switch_changed(GtkSwitch *sw, GParamSpec *property,
	AttributeSet *Attr)
{
	(void)property;
	widget_signal_executor(GTK_WIDGET(sw), Attr, "state-changed");
}

static void widget_switch_input(variable *var, const gchar *source,
	gboolean command)
{
	FILE *input = command ? widget_opencommand(source) : fopen(source, "r");
	gchar *line;
	if (input == NULL) {
		gtkdialog_warning("Cannot read switch input '%s'.", source);
		return;
	}
	line = widget_read_line(input);
	if (line != NULL) {
		widget_switch_set_value(var->Widget, line);
		g_free(line);
	}
	if (command)
		widget_closecommand(input, source);
	else
		fclose(input);
}

GtkWidget *widget_switch_create(AttributeSet *Attr, tag_attr *attr, gint Type)
{
	GtkWidget *widget = gtk_switch_new();
	gchar *value;
	(void)Attr;
	(void)Type;
	if (attr != NULL && (value = get_tag_attribute(attr, "active")) != NULL) {
		widget_switch_set_value(widget, value);
		kill_tag_attribute(attr, "active");
	}
	return widget;
}

gchar *widget_switch_envvar_construct(GtkWidget *widget)
{
	return g_strdup(gtk_switch_get_active(GTK_SWITCH(widget))
		? "true" : "false");
}

void widget_switch_fileselect(variable *var, const char *name,
	const char *value)
{
	(void)name;
	widget_switch_set_value(var->Widget, value);
}

void widget_switch_refresh(variable *var)
{
	GList *element;
	gchar *input;
	gboolean initialised = GPOINTER_TO_INT(g_object_get_data(
		G_OBJECT(var->Widget), "_initialised"));
	if (!initialised && attributeset_is_avail(var->Attributes, ATTR_DEFAULT))
		widget_switch_set_value(var->Widget,
			attributeset_get_first(&element, var->Attributes, ATTR_DEFAULT));
	input = attributeset_get_first(&element, var->Attributes, ATTR_INPUT);
	while (input != NULL) {
		if (input_is_shell_command(input))
			widget_switch_input(var, input + 8, TRUE);
		else if (g_ascii_strncasecmp(input, "file:", 5) == 0 && input[5]) {
			if (!initialised)
				widget_file_monitor_try_create(var, input + 5);
			widget_switch_input(var, input + 5, FALSE);
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
				var->Attributes, ATTR_WIDTH), "switch width");
		if (attributeset_is_avail(var->Attributes, ATTR_HEIGHT))
			height = widget_parse_size_request(attributeset_get_first(&element,
				var->Attributes, ATTR_HEIGHT), "switch height");
		gtk_widget_set_size_request(var->Widget, width, height);
	}
	if (attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "false") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "no") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "0"))
		gtk_widget_set_sensitive(var->Widget, FALSE);
	g_signal_connect(var->Widget, "notify::active",
		G_CALLBACK(widget_switch_changed), var->Attributes);
}

void widget_switch_save(variable *var)
{
	GList *element;
	gchar *output = attributeset_get_first(&element, var->Attributes, ATTR_OUTPUT);
	FILE *file;
	while (output != NULL &&
		(g_ascii_strncasecmp(output, "file:", 5) != 0 || !output[5]))
		output = attributeset_get_next(&element, var->Attributes, ATTR_OUTPUT);
	if (output == NULL) {
		gtkdialog_warning("<switch> has no <output file> directive.");
		return;
	}
	file = fopen(output + 5, "w");
	if (file == NULL) {
		gtkdialog_warning("Cannot write switch output '%s'.", output + 5);
		return;
	}
	fputs(gtk_switch_get_active(GTK_SWITCH(var->Widget))
		? "true" : "false", file);
	widget_close_output(file, output + 5);
}

void widget_switch_clear(variable *var)
{
	gtk_switch_set_active(GTK_SWITCH(var->Widget), FALSE);
}

void widget_switch_removeselected(variable *var)
{
	(void)var;
	gtkdialog_warning("<removeselected> is not supported by <switch>.");
}
