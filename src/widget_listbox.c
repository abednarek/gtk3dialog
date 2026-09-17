/*
 * widget_listbox.c: GTK3 GtkListBox XML widget.
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
#include "widget_listbox.h"

static const gchar *widget_listbox_row_name(GtkListBoxRow *row)
{
	return row == NULL ? NULL :
		g_object_get_data(G_OBJECT(row), "_listbox_name");
}

static void widget_listbox_set_value(GtkWidget *widget, const gchar *value)
{
	gchar *name = g_strdup(value);
	gint index;
	g_strstrip(name);
	for (index = 0; ; ++index) {
		GtkListBoxRow *row = gtk_list_box_get_row_at_index(
			GTK_LIST_BOX(widget), index);
		if (row == NULL)
			break;
		if (g_strcmp0(name, widget_listbox_row_name(row)) == 0) {
			gtk_list_box_select_row(GTK_LIST_BOX(widget), row);
			g_free(name);
			return;
		}
	}
	gtkdialog_warning("Unknown listbox row '%s'.", name);
	g_free(name);
}

static void widget_listbox_changed(GtkListBox *box, GtkListBoxRow *row,
	AttributeSet *Attr)
{
	(void)row;
	widget_signal_executor(GTK_WIDGET(box), Attr, "selection-changed");
}

static void widget_listbox_activated(GtkListBox *box, GtkListBoxRow *row,
	AttributeSet *Attr)
{
	(void)row;
	widget_signal_executor(GTK_WIDGET(box), Attr, "row-activated");
}

static void widget_listbox_input(variable *var, const gchar *source,
	gboolean command)
{
	FILE *input = command ? widget_opencommand(source) : fopen(source, "r");
	gchar *line;
	if (input == NULL) {
		gtkdialog_warning("Cannot read listbox input '%s'.", source);
		return;
	}
	line = widget_read_line(input);
	if (line != NULL) {
		widget_listbox_set_value(var->Widget, line);
		g_free(line);
	}
	if (command)
		widget_closecommand(input, source);
	else
		fclose(input);
}

GtkWidget *widget_listbox_create(AttributeSet *Attr, tag_attr *attr, gint Type)
{
	GtkWidget *widget = gtk_list_box_new();
	stackelement children = pop();
	gchar **names = NULL;
	gchar *value;
	gint i;
	(void)Attr;
	(void)Type;
	if (attr != NULL && (value = get_tag_attribute(attr, "names")) != NULL) {
		names = g_strsplit(value, "|", -1);
		kill_tag_attribute(attr, "names");
	}
	if (children.nwidgets < 1)
		g_error("gtkdialog: <listbox> requires at least one direct child widget.");
	if (names != NULL && g_strv_length(names) != (guint)children.nwidgets)
		g_error("gtkdialog: <listbox> names must match its number of rows.");
	for (i = 0; i < children.nwidgets; ++i) {
		GtkWidget *row = gtk_list_box_row_new();
		gchar *generated = g_strdup_printf("%d", i);
		const gchar *name = names != NULL ? names[i] : generated;
		gint previous;
		if (!*name)
			g_error("gtkdialog: <listbox> row names must be nonempty.");
		for (previous = 0; previous < i; ++previous)
			if (g_strcmp0(name, names != NULL ? names[previous] : NULL) == 0)
				g_error("gtkdialog: <listbox> row names must be unique.");
		g_object_set_data_full(G_OBJECT(row), "_listbox_name",
			g_strdup(name), g_free);
		gtk_container_add(GTK_CONTAINER(row), children.widgets[i]);
		gtk_container_add(GTK_CONTAINER(widget), row);
		gtk_widget_show(row);
		g_free(generated);
	}
	stackelement_clear(&children);
	g_strfreev(names);
	if (attr != NULL) {
		if ((value = get_tag_attribute(attr, "selection-mode")) != NULL) {
			if (g_ascii_strcasecmp(value, "none") == 0)
				gtk_list_box_set_selection_mode(GTK_LIST_BOX(widget),
					GTK_SELECTION_NONE);
			else if (g_ascii_strcasecmp(value, "single") != 0)
				gtkdialog_warning("Unsupported listbox selection-mode '%s'; using single.",
					value);
			kill_tag_attribute(attr, "selection-mode");
		}
		if ((value = get_tag_attribute(attr, "activate-on-single-click")) != NULL) {
			gboolean enabled;
			if (widget_parse_boolean(value, &enabled))
				gtk_list_box_set_activate_on_single_click(GTK_LIST_BOX(widget),
					enabled);
			else
				gtkdialog_warning("Invalid listbox activate-on-single-click '%s'.",
					value);
			kill_tag_attribute(attr, "activate-on-single-click");
		}
	}
	return widget;
}

gchar *widget_listbox_envvar_construct(GtkWidget *widget)
{
	GtkListBoxRow *row = gtk_list_box_get_selected_row(GTK_LIST_BOX(widget));
	const gchar *name = widget_listbox_row_name(row);
	return g_strdup(name != NULL ? name : "");
}

void widget_listbox_fileselect(variable *var, const char *name,
	const char *value)
{
	(void)name;
	widget_listbox_set_value(var->Widget, value);
}

void widget_listbox_refresh(variable *var)
{
	GList *element;
	gchar *input;
	gboolean initialised = GPOINTER_TO_INT(g_object_get_data(
		G_OBJECT(var->Widget), "_initialised"));
	if (!initialised && attributeset_is_avail(var->Attributes, ATTR_DEFAULT))
		widget_listbox_set_value(var->Widget,
			attributeset_get_first(&element, var->Attributes, ATTR_DEFAULT));
	input = attributeset_get_first(&element, var->Attributes, ATTR_INPUT);
	while (input != NULL) {
		if (input_is_shell_command(input))
			widget_listbox_input(var, input + 8, TRUE);
		else if (g_ascii_strncasecmp(input, "file:", 5) == 0 && input[5]) {
			if (!initialised)
				widget_file_monitor_try_create(var, input + 5);
			widget_listbox_input(var, input + 5, FALSE);
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
				var->Attributes, ATTR_WIDTH), "listbox width");
		if (attributeset_is_avail(var->Attributes, ATTR_HEIGHT))
			height = widget_parse_size_request(attributeset_get_first(&element,
				var->Attributes, ATTR_HEIGHT), "listbox height");
		gtk_widget_set_size_request(var->Widget, width, height);
	}
	if (attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "false") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "no") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "0"))
		gtk_widget_set_sensitive(var->Widget, FALSE);
	g_signal_connect(var->Widget, "row-selected",
		G_CALLBACK(widget_listbox_changed), var->Attributes);
	g_signal_connect(var->Widget, "row-activated",
		G_CALLBACK(widget_listbox_activated), var->Attributes);
}

void widget_listbox_save(variable *var)
{
	GList *element;
	gchar *output = attributeset_get_first(&element, var->Attributes, ATTR_OUTPUT);
	FILE *file;
	while (output != NULL &&
		(g_ascii_strncasecmp(output, "file:", 5) != 0 || !output[5]))
		output = attributeset_get_next(&element, var->Attributes, ATTR_OUTPUT);
	if (output == NULL) {
		gtkdialog_warning("<listbox> has no <output file> directive.");
		return;
	}
	file = fopen(output + 5, "w");
	if (file == NULL) {
		gtkdialog_warning("Cannot write listbox output '%s'.", output + 5);
		return;
	}
	{
		gchar *name = widget_listbox_envvar_construct(var->Widget);
		fputs(name, file);
		g_free(name);
	}
	widget_close_output(file, output + 5);
}

void widget_listbox_clear(variable *var)
{
	gtk_list_box_unselect_all(GTK_LIST_BOX(var->Widget));
}

void widget_listbox_removeselected(variable *var)
{
	(void)var;
	gtkdialog_warning("<removeselected> is not supported by <listbox>.");
}
