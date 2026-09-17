/*
 * widget_colorchooserdialog.c: GTK3 colour chooser dialog.
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

#include <errno.h>
#include <gtk/gtk.h>
#include "config.h"
#include "gtkdialog.h"
#include "attributes.h"
#include "automaton.h"
#include "signals.h"
#include "tag_attributes.h"
#include "widgets.h"
#include "widget_dialog.h"
#include "widget_colorchooserdialog.h"

#define COLOR_DIALOG_RESPONSE_DATA "gtkdialog-colorchooserdialog-response"

extern gboolean option_centering;
extern gboolean have_geometry_xy;
extern gboolean have_geometry_dxdy;
extern gint geometry_dx;
extern gint geometry_dy;
extern gint geometry_x;
extern gint geometry_y;

static void widget_colorchooserdialog_store_response(GtkWidget *widget,
	gint response)
{
	g_object_set_data_full(G_OBJECT(widget), COLOR_DIALOG_RESPONSE_DATA,
		widget_dialog_response_name(response), g_free);
}

static void widget_colorchooserdialog_response(GtkDialog *dialog,
	gint response, AttributeSet *attributes)
{
	widget_colorchooserdialog_store_response(GTK_WIDGET(dialog), response);
	widget_signal_executor(GTK_WIDGET(dialog), attributes, "response");
}

static gboolean widget_colorchooserdialog_set_color(GtkWidget *widget,
	const gchar *value)
{
	GdkRGBA color;
	gchar **parts;
	gchar *end;
	gint64 alpha;
	gboolean valid;

	if (value == NULL) {
		gtkdialog_warning("Color chooser dialog received an empty colour.");
		return FALSE;
	}
	parts = g_strsplit(value, "|", 3);
	valid = gdk_rgba_parse(&color, g_strstrip(parts[0]));
	if (!valid) {
		gtkdialog_warning("Invalid color chooser dialog colour '%s'.", value);
		g_strfreev(parts);
		return FALSE;
	}
	if (parts[1] != NULL) {
		errno = 0;
		alpha = g_ascii_strtoll(parts[1], &end, 10);
		while (g_ascii_isspace(*end))
			++end;
		if (errno != 0 || end == parts[1] || *end != '\0' ||
			alpha < 0 || alpha > G_MAXUINT16 || parts[2] != NULL) {
			gtkdialog_warning("Invalid color chooser dialog alpha '%s'.",
				parts[1]);
			g_strfreev(parts);
			return FALSE;
		}
		color.alpha = alpha / (gdouble)G_MAXUINT16;
	}
	gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(widget), &color);
	g_strfreev(parts);
	return TRUE;
}

static void widget_colorchooserdialog_set_response(GtkWidget *widget,
	const gchar *value, gboolean emit)
{
	gint response;

	if (!widget_dialog_parse_response(value, &response)) {
		gtkdialog_warning("Invalid color chooser dialog response '%s'.", value);
		return;
	}
	if (emit)
		gtk_dialog_response(GTK_DIALOG(widget), response);
	else
		widget_colorchooserdialog_store_response(widget, response);
}

static void widget_colorchooserdialog_input(variable *var,
	const gchar *source, gboolean command, gboolean response, gboolean emit)
{
	FILE *input = command ? widget_opencommand(source) : fopen(source, "r");
	gchar *line;

	if (input == NULL) {
		gtkdialog_warning("Cannot read color chooser dialog input '%s'.", source);
		return;
	}
	line = widget_read_line(input);
	if (line != NULL) {
		if (response)
			widget_colorchooserdialog_set_response(var->Widget, line, emit);
		else
			widget_colorchooserdialog_set_color(var->Widget, line);
		g_free(line);
	}
	if (command)
		widget_closecommand(input, source);
	else
		fclose(input);
}

GtkWidget *widget_colorchooserdialog_create(AttributeSet *attributes,
	tag_attr *tag_attributes, gint type)
{
	GtkWidget *widget;

	(void)attributes;
	(void)tag_attributes;
	(void)type;
	widget = gtk_color_chooser_dialog_new(NULL, NULL);
	g_object_set_data_full(G_OBJECT(widget), COLOR_DIALOG_RESPONSE_DATA,
		g_strdup(""), g_free);
	if (have_geometry_dxdy)
		gtk_widget_set_size_request(widget, geometry_dx, geometry_dy);
	if (have_geometry_xy)
		gtk_window_move(GTK_WINDOW(widget), geometry_x, geometry_y);
	if (option_centering)
		gtk_window_set_position(GTK_WINDOW(widget), GTK_WIN_POS_CENTER_ALWAYS);
	return widget;
}

gchar *widget_colorchooserdialog_envvar_construct(GtkWidget *widget)
{
	GdkRGBA color;
	guint red;
	guint green;
	guint blue;

	gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(widget), &color);
	red = (guint)(CLAMP(color.red, 0.0, 1.0) * 255.0 + 0.5);
	green = (guint)(CLAMP(color.green, 0.0, 1.0) * 255.0 + 0.5);
	blue = (guint)(CLAMP(color.blue, 0.0, 1.0) * 255.0 + 0.5);
	if (gtk_color_chooser_get_use_alpha(GTK_COLOR_CHOOSER(widget)))
		return g_strdup_printf("#%02x%02x%02x|%u", red, green, blue,
			(guint)(CLAMP(color.alpha, 0.0, 1.0) * G_MAXUINT16 + 0.5));
	return g_strdup_printf("#%02x%02x%02x", red, green, blue);
}

void widget_colorchooserdialog_fileselect(variable *var, const char *name,
	const char *value)
{
	(void)var;
	(void)name;
	(void)value;
	gtkdialog_warning("Color chooser dialog does not accept file selection.");
}

void widget_colorchooserdialog_refresh(variable *var)
{
	GList *element;
	gchar *input;
	gboolean initialised;
	gboolean response;

	initialised = GPOINTER_TO_INT(g_object_get_data(
		G_OBJECT(var->Widget), "_initialised"));
	input = attributeset_get_first(&element, var->Attributes, ATTR_INPUT);
	while (input != NULL) {
		response = widget_attribute_is_true(attributeset_get_this_tagattr(
			&element, var->Attributes, ATTR_INPUT, "response"));
		if (input_is_shell_command(input))
			widget_colorchooserdialog_input(var, input + 8, TRUE,
				response, initialised);
		else if (g_ascii_strncasecmp(input, "file:", 5) == 0 &&
			input[5] != '\0') {
			if (!initialised)
				widget_file_monitor_try_create(var, input + 5);
			widget_colorchooserdialog_input(var, input + 5, FALSE,
				response, initialised);
		} else if (input[0] != '\0')
			widget_colorchooserdialog_input(var, input, TRUE,
				response, initialised);
		input = attributeset_get_next(&element, var->Attributes, ATTR_INPUT);
	}
	if (initialised)
		return;
	if (attributeset_is_avail(var->Attributes, ATTR_DEFAULT))
		widget_colorchooserdialog_set_color(var->Widget,
			attributeset_get_first(&element,
				var->Attributes, ATTR_DEFAULT));
	if (attributeset_is_avail(var->Attributes, ATTR_LABEL))
		gtkdialog_warning("Color chooser dialog does not support <label>.");
	if (attributeset_is_avail(var->Attributes, ATTR_ITEM))
		gtkdialog_warning("Color chooser dialog does not support <item>.");
	if (attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "false") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "disabled") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "no") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "0"))
		gtk_widget_set_sensitive(var->Widget, FALSE);
	g_signal_connect(G_OBJECT(var->Widget), "response",
		G_CALLBACK(widget_colorchooserdialog_response), var->Attributes);
}

void widget_colorchooserdialog_save(variable *var)
{
	GError *error;
	GList *element;
	gchar *directive;
	gchar *contents;
	const gchar *response;
	gboolean found = FALSE;

	directive = attributeset_get_first(&element, var->Attributes, ATTR_OUTPUT);
	while (directive != NULL) {
		if (g_ascii_strncasecmp(directive, "file:", 5) == 0 &&
			directive[5] != '\0') {
			found = TRUE;
			if (widget_attribute_is_true(attributeset_get_this_tagattr(
				&element, var->Attributes, ATTR_OUTPUT, "response"))) {
				response = g_object_get_data(G_OBJECT(var->Widget),
					COLOR_DIALOG_RESPONSE_DATA);
				contents = g_strdup(response != NULL ? response : "");
			} else
				contents = widget_colorchooserdialog_envvar_construct(var->Widget);
			error = NULL;
			if (!g_file_set_contents(directive + 5, contents, -1, &error))
				gtkdialog_warning("Cannot save color chooser output '%s': %s",
					directive + 5, error != NULL
						? error->message : "unknown error");
			if (error != NULL)
				g_error_free(error);
			g_free(contents);
		}
		directive = attributeset_get_next(&element,
			var->Attributes, ATTR_OUTPUT);
	}
	if (!found)
		gtkdialog_warning("Color chooser dialog has no <output file> directive.");
}

void widget_colorchooserdialog_clear(variable *var)
{
	g_object_set_data_full(G_OBJECT(var->Widget), COLOR_DIALOG_RESPONSE_DATA,
		g_strdup(""), g_free);
	gtkdialog_warning("Color chooser dialog cannot clear its colour; only its stored response was reset.");
}

void widget_colorchooserdialog_removeselected(variable *var)
{
	(void)var;
	gtkdialog_warning("Color chooser dialog cannot remove its selected colour.");
}
