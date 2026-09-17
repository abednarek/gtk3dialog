/*
 * widget_levelbar.c: GTK3 GtkLevelBar XML widget.
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
#include <math.h>
#include "config.h"
#include "gtkdialog.h"
#include "attributes.h"
#include "automaton.h"
#include "tag_attributes.h"
#include "widgets.h"
#include "widget_levelbar.h"

static void widget_levelbar_set_value(GtkWidget *widget, const gchar *text)
{
	gdouble value;
	if (!widget_parse_finite_double(text, &value, "levelbar value"))
		return;
	gtk_level_bar_set_value(GTK_LEVEL_BAR(widget), value);
}

static void widget_levelbar_changed(GtkLevelBar *bar, GParamSpec *property,
	AttributeSet *Attr)
{
	(void)property;
	widget_signal_executor(GTK_WIDGET(bar), Attr, "value-changed");
}

static void widget_levelbar_input(variable *var, const gchar *source,
	gboolean command)
{
	FILE *input = command ? widget_opencommand(source) : fopen(source, "r");
	gchar *line;
	if (input == NULL) {
		gtkdialog_warning("Cannot read levelbar input '%s'.", source);
		return;
	}
	line = widget_read_line(input);
	if (line != NULL) {
		widget_levelbar_set_value(var->Widget, line);
		g_free(line);
	}
	if (command)
		widget_closecommand(input, source);
	else
		fclose(input);
}

GtkWidget *widget_levelbar_create(AttributeSet *Attr, tag_attr *attr, gint Type)
{
	GtkWidget *widget;
	gdouble minimum = 0.0;
	gdouble maximum = 1.0;
	gdouble parsed;
	gchar *text;
	(void)Attr;
	(void)Type;
	if (attr != NULL) {
		text = get_tag_attribute(attr, "min-value");
		if (text != NULL) {
			if (widget_parse_finite_double(text, &parsed,
				"levelbar min-value"))
				minimum = parsed;
			kill_tag_attribute(attr, "min-value");
		}
		text = get_tag_attribute(attr, "max-value");
		if (text != NULL) {
			if (widget_parse_finite_double(text, &parsed,
				"levelbar max-value"))
				maximum = parsed;
			kill_tag_attribute(attr, "max-value");
		}
	}
	if (minimum >= maximum || !isfinite(maximum - minimum)) {
		gtkdialog_warning("Invalid levelbar interval; using 0..1.");
		minimum = 0.0;
		maximum = 1.0;
	}
	widget = gtk_level_bar_new_for_interval(minimum, maximum);
	return widget;
}

gchar *widget_levelbar_envvar_construct(GtkWidget *widget)
{
	gchar buffer[G_ASCII_DTOSTR_BUF_SIZE];
	g_ascii_dtostr(buffer, sizeof(buffer),
		gtk_level_bar_get_value(GTK_LEVEL_BAR(widget)));
	return g_strdup(buffer);
}

void widget_levelbar_fileselect(variable *var, const char *name,
	const char *value)
{
	(void)name;
	widget_levelbar_set_value(var->Widget, value);
}

void widget_levelbar_refresh(variable *var)
{
	GList *element;
	gchar *input;
	gboolean initialised = GPOINTER_TO_INT(g_object_get_data(
		G_OBJECT(var->Widget), "_initialised"));
	if (!initialised && attributeset_is_avail(var->Attributes, ATTR_DEFAULT))
		widget_levelbar_set_value(var->Widget,
			attributeset_get_first(&element, var->Attributes, ATTR_DEFAULT));
	input = attributeset_get_first(&element, var->Attributes, ATTR_INPUT);
	while (input != NULL) {
		if (input_is_shell_command(input))
			widget_levelbar_input(var, input + 8, TRUE);
		else if (g_ascii_strncasecmp(input, "file:", 5) == 0 && input[5]) {
			if (!initialised)
				widget_file_monitor_try_create(var, input + 5);
			widget_levelbar_input(var, input + 5, FALSE);
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
				var->Attributes, ATTR_WIDTH), "levelbar width");
		if (attributeset_is_avail(var->Attributes, ATTR_HEIGHT))
			height = widget_parse_size_request(attributeset_get_first(&element,
				var->Attributes, ATTR_HEIGHT), "levelbar height");
		gtk_widget_set_size_request(var->Widget, width, height);
	}
	g_signal_connect(var->Widget, "notify::value",
		G_CALLBACK(widget_levelbar_changed), var->Attributes);
}

void widget_levelbar_save(variable *var)
{
	GList *element;
	gchar *output = attributeset_get_first(&element, var->Attributes, ATTR_OUTPUT);
	gchar *text;
	FILE *file;
	while (output != NULL &&
		(g_ascii_strncasecmp(output, "file:", 5) != 0 || !output[5]))
		output = attributeset_get_next(&element, var->Attributes, ATTR_OUTPUT);
	if (output == NULL) {
		gtkdialog_warning("<levelbar> has no <output file> directive.");
		return;
	}
	file = fopen(output + 5, "w");
	if (file == NULL) {
		gtkdialog_warning("Cannot write levelbar output '%s'.", output + 5);
		return;
	}
	text = widget_levelbar_envvar_construct(var->Widget);
	fputs(text, file);
	g_free(text);
	widget_close_output(file, output + 5);
}

void widget_levelbar_clear(variable *var)
{
	gtk_level_bar_set_value(GTK_LEVEL_BAR(var->Widget),
		gtk_level_bar_get_min_value(GTK_LEVEL_BAR(var->Widget)));
}

void widget_levelbar_removeselected(variable *var)
{
	(void)var;
	gtkdialog_warning("<removeselected> is not supported by <levelbar>.");
}
