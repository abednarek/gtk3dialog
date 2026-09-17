/*
 * widget_recentchooserdialog.c: GTK3 recent resource chooser dialog.
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
#include "signals.h"
#include "tag_attributes.h"
#include "widgets.h"
#include "widget_dialog.h"
#include "widget_recentchooser.h"
#include "widget_recentchooserdialog.h"

#define RECENT_DIALOG_RESPONSE_DATA "gtkdialog-recentchooserdialog-response"

extern gboolean option_centering;
extern gboolean have_geometry_xy;
extern gboolean have_geometry_dxdy;
extern gint geometry_dx;
extern gint geometry_dy;
extern gint geometry_x;
extern gint geometry_y;

static void widget_recentchooserdialog_store_response(GtkWidget *widget,
	gint response)
{
	g_object_set_data_full(G_OBJECT(widget), RECENT_DIALOG_RESPONSE_DATA,
		widget_dialog_response_name(response), g_free);
}

static void widget_recentchooserdialog_response(GtkDialog *dialog,
	gint response, AttributeSet *attributes)
{
	widget_recentchooserdialog_store_response(GTK_WIDGET(dialog), response);
	widget_signal_executor(GTK_WIDGET(dialog), attributes, "response");
}

static void widget_recentchooserdialog_selection_changed(GtkWidget *widget,
	AttributeSet *attributes)
{
	widget_signal_executor(widget, attributes, "selection-changed");
}

static void widget_recentchooserdialog_item_activated(GtkWidget *widget,
	AttributeSet *attributes)
{
	widget_signal_executor(widget, attributes, "item-activated");
}

static void widget_recentchooserdialog_set_response(GtkWidget *widget,
	const gchar *value, gboolean emit)
{
	gint response;

	if (!widget_dialog_parse_response(value, &response)) {
		gtkdialog_warning("Invalid recent chooser dialog response '%s'.", value);
		return;
	}
	if (emit)
		gtk_dialog_response(GTK_DIALOG(widget), response);
	else
		widget_recentchooserdialog_store_response(widget, response);
}

static void widget_recentchooserdialog_input(variable *var,
	const gchar *source, gboolean command, gboolean response, gboolean emit)
{
	FILE *input = command ? widget_opencommand(source) : fopen(source, "r");
	gchar *contents;

	if (input == NULL) {
		gtkdialog_warning("Cannot read recent chooser dialog input '%s'.", source);
		return;
	}
	contents = widget_read_all(input);
	if (contents != NULL) {
		if (response) {
			gchar *newline = strchr(contents, '\n');
			if (newline != NULL)
				*newline = '\0';
			g_strstrip(contents);
			widget_recentchooserdialog_set_response(var->Widget,
				contents, emit);
		} else
			widget_recentchooser_select(var->Widget, contents);
		g_free(contents);
	}
	if (command)
		widget_closecommand(input, source);
	else
		fclose(input);
}

GtkWidget *widget_recentchooserdialog_create(AttributeSet *attributes,
	tag_attr *tag_attributes, gint type)
{
	GtkWidget *widget;

	(void)attributes;
	(void)type;
	widget = gtk_recent_chooser_dialog_new(NULL, NULL,
		"_Cancel", GTK_RESPONSE_CANCEL,
		"_Open", GTK_RESPONSE_ACCEPT, NULL);
	gtk_window_set_default_size(GTK_WINDOW(widget), 560, 420);
	g_object_set_data_full(G_OBJECT(widget), RECENT_DIALOG_RESPONSE_DATA,
		g_strdup(""), g_free);
	widget_recentchooser_enable_pending_selection(widget);
	widget_recentchooser_add_filter(widget, tag_attributes);
	if (tag_attributes != NULL) {
		kill_tag_attribute(tag_attributes, "filter-name");
		kill_tag_attribute(tag_attributes, "filter-pattern");
		kill_tag_attribute(tag_attributes, "filter-mime-type");
	}
	if (have_geometry_dxdy)
		gtk_widget_set_size_request(widget, geometry_dx, geometry_dy);
	if (have_geometry_xy)
		gtk_window_move(GTK_WINDOW(widget), geometry_x, geometry_y);
	if (option_centering)
		gtk_window_set_position(GTK_WINDOW(widget), GTK_WIN_POS_CENTER_ALWAYS);
	return widget;
}

gchar *widget_recentchooserdialog_envvar_construct(GtkWidget *widget)
{
	return widget_recentchooser_envvar_construct(widget);
}

void widget_recentchooserdialog_fileselect(variable *var, const char *name,
	const char *value)
{
	(void)name;
	widget_recentchooser_select(var->Widget, value);
}

void widget_recentchooserdialog_refresh(variable *var)
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
			widget_recentchooserdialog_input(var, input + 8, TRUE,
				response, initialised);
		else if (g_ascii_strncasecmp(input, "file:", 5) == 0 &&
			input[5] != '\0') {
			if (!initialised)
				widget_file_monitor_try_create(var, input + 5);
			widget_recentchooserdialog_input(var, input + 5, FALSE,
				response, initialised);
		} else if (input[0] != '\0')
			widget_recentchooserdialog_input(var, input, TRUE,
				response, initialised);
		input = attributeset_get_next(&element, var->Attributes, ATTR_INPUT);
	}
	if (initialised)
		return;
	if (attributeset_is_avail(var->Attributes, ATTR_DEFAULT))
		widget_recentchooser_select(var->Widget,
			attributeset_get_first(&element,
				var->Attributes, ATTR_DEFAULT));
	if (attributeset_is_avail(var->Attributes, ATTR_LABEL))
		gtkdialog_warning("Recent chooser dialog does not support <label>.");
	if (attributeset_is_avail(var->Attributes, ATTR_ITEM))
		gtkdialog_warning("Recent chooser dialog does not support <item>.");
	if (attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "false") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "disabled") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "no") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "0"))
		gtk_widget_set_sensitive(var->Widget, FALSE);
	g_signal_connect(G_OBJECT(var->Widget), "response",
		G_CALLBACK(widget_recentchooserdialog_response), var->Attributes);
	g_signal_connect(G_OBJECT(var->Widget), "selection-changed",
		G_CALLBACK(widget_recentchooserdialog_selection_changed), var->Attributes);
	g_signal_connect(G_OBJECT(var->Widget), "item-activated",
		G_CALLBACK(widget_recentchooserdialog_item_activated), var->Attributes);
}

void widget_recentchooserdialog_save(variable *var)
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
					RECENT_DIALOG_RESPONSE_DATA);
				contents = g_strdup(response != NULL ? response : "");
			} else
				contents = widget_recentchooserdialog_envvar_construct(var->Widget);
			error = NULL;
			if (!g_file_set_contents(directive + 5, contents, -1, &error))
				gtkdialog_warning("Cannot save recent chooser output '%s': %s",
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
		gtkdialog_warning("Recent chooser dialog has no <output file> directive.");
}

void widget_recentchooserdialog_clear(variable *var)
{
	widget_recentchooser_select(var->Widget, "");
	g_object_set_data_full(G_OBJECT(var->Widget), RECENT_DIALOG_RESPONSE_DATA,
		g_strdup(""), g_free);
}

void widget_recentchooserdialog_removeselected(variable *var)
{
	widget_recentchooser_select(var->Widget, "");
}
