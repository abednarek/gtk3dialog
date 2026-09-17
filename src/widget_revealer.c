/*
 * widget_revealer.c: GTK3 GtkRevealer XML widget.
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
#include <string.h>
#include "config.h"
#include "gtkdialog.h"
#include "attributes.h"
#include "automaton.h"
#include "tag_attributes.h"
#include "widgets.h"
#include "widget_revealer.h"

static void widget_revealer_set_value(GtkWidget *widget,
	const gchar *value)
{
	gboolean reveal;

	if (!widget_parse_boolean(value, &reveal)) {
		gtkdialog_warning("Invalid revealer Boolean value '%s'.", value);
		return;
	}
	gtk_revealer_set_reveal_child(GTK_REVEALER(widget), reveal);
}

static void widget_revealer_changed(GtkRevealer *revealer,
	GParamSpec *property, AttributeSet *Attr)
{
	(void)property;
	widget_signal_executor(GTK_WIDGET(revealer), Attr, "reveal-changed");
}

static void widget_revealer_input(variable *var, const gchar *source,
	gboolean command)
{
	FILE *input;
	gchar *line;

	input = command ? widget_opencommand(source) : fopen(source, "r");
	if (input == NULL) {
		gtkdialog_warning("Cannot read revealer input '%s'.", source);
		return;
	}
	line = widget_read_line(input);
	if (line != NULL) {
		widget_revealer_set_value(var->Widget, line);
		g_free(line);
	}
	if (command)
		widget_closecommand(input, source);
	else
		fclose(input);
}

GtkWidget *widget_revealer_create(AttributeSet *Attr, tag_attr *attr,
	gint Type)
{
	GtkWidget *widget;
	stackelement child;
	gchar *value;

	(void)Attr;
	(void)Type;
	child = pop();
	if (child.nwidgets != 1) {
		stackelement_clear(&child);
		g_error("gtkdialog: <revealer> requires exactly one direct child widget.");
	}
	widget = gtk_revealer_new();
	gtk_container_add(GTK_CONTAINER(widget), child.widgets[0]);
	stackelement_clear(&child);
	if (attr != NULL &&
		(value = get_tag_attribute(attr, "reveal-child")) != NULL) {
		widget_revealer_set_value(widget, value);
		kill_tag_attribute(attr, "reveal-child");
	}
	return widget;
}

gchar *widget_revealer_envvar_construct(GtkWidget *widget)
{
	return g_strdup(gtk_revealer_get_reveal_child(GTK_REVEALER(widget))
		? "true" : "false");
}

void widget_revealer_fileselect(variable *var, const char *name,
	const char *value)
{
	(void)name;
	widget_revealer_set_value(var->Widget, value);
}

void widget_revealer_refresh(variable *var)
{
	GList *element;
	gchar *input;
	gboolean initialised;
	gint width = -1;
	gint height = -1;

	initialised = GPOINTER_TO_INT(g_object_get_data(
		G_OBJECT(var->Widget), "_initialised"));
	if (!initialised && attributeset_is_avail(var->Attributes, ATTR_DEFAULT))
		widget_revealer_set_value(var->Widget,
			attributeset_get_first(&element, var->Attributes, ATTR_DEFAULT));

	input = attributeset_get_first(&element, var->Attributes, ATTR_INPUT);
	while (input != NULL) {
		if (input_is_shell_command(input))
			widget_revealer_input(var, input + 8, TRUE);
		else if (g_ascii_strncasecmp(input, "file:", 5) == 0 && input[5]) {
			if (!initialised)
				widget_file_monitor_try_create(var, input + 5);
			widget_revealer_input(var, input + 5, FALSE);
		}
		input = attributeset_get_next(&element, var->Attributes, ATTR_INPUT);
	}
	if (initialised)
		return;

	if (attributeset_is_avail(var->Attributes, ATTR_WIDTH))
		width = widget_parse_size_request(attributeset_get_first(&element,
			var->Attributes, ATTR_WIDTH), "revealer width");
	if (attributeset_is_avail(var->Attributes, ATTR_HEIGHT))
		height = widget_parse_size_request(attributeset_get_first(&element,
			var->Attributes, ATTR_HEIGHT), "revealer height");
	if (width != -1 || height != -1)
		gtk_widget_set_size_request(var->Widget, width, height);
	if (attributeset_is_avail(var->Attributes, ATTR_LABEL))
		gtkdialog_warning("<label> is not supported by <revealer>.");
	if (attributeset_is_avail(var->Attributes, ATTR_ITEM))
		gtkdialog_warning("<item> is not supported by <revealer>.");
	if (attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "false") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "no") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "0"))
		gtk_widget_set_sensitive(var->Widget, FALSE);
	g_signal_connect(var->Widget, "notify::reveal-child",
		G_CALLBACK(widget_revealer_changed), var->Attributes);
}

void widget_revealer_save(variable *var)
{
	GList *element;
	gchar *output;
	FILE *file;

	output = attributeset_get_first(&element, var->Attributes, ATTR_OUTPUT);
	while (output != NULL &&
		(g_ascii_strncasecmp(output, "file:", 5) != 0 || !output[5]))
		output = attributeset_get_next(&element, var->Attributes, ATTR_OUTPUT);
	if (output == NULL) {
		gtkdialog_warning("<revealer> has no <output file> directive.");
		return;
	}
	file = fopen(output + 5, "w");
	if (file == NULL) {
		gtkdialog_warning("Cannot write revealer output '%s'.", output + 5);
		return;
	}
	fputs(gtk_revealer_get_reveal_child(GTK_REVEALER(var->Widget))
		? "true" : "false", file);
	widget_close_output(file, output + 5);
}

void widget_revealer_clear(variable *var)
{
	gtk_revealer_set_reveal_child(GTK_REVEALER(var->Widget), FALSE);
}

void widget_revealer_removeselected(variable *var)
{
	(void)var;
	gtkdialog_warning("<removeselected> is not supported by <revealer>.");
}
