/*
 * widget_appchooserdialog.c: GTK3 application chooser dialog.
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
#include "widget_appchooser_common.h"
#include "widget_appchooserdialog.h"

#define APPCHOOSER_RESPONSE_DATA "gtkdialog-appchooserdialog-response"

extern gboolean option_centering;
extern gboolean have_geometry_xy;
extern gboolean have_geometry_dxdy;
extern gint geometry_dx;
extern gint geometry_dy;
extern gint geometry_x;
extern gint geometry_y;

static void widget_appchooserdialog_store_response(GtkWidget *widget,
	gint response)
{
	g_object_set_data_full(G_OBJECT(widget), APPCHOOSER_RESPONSE_DATA,
		widget_dialog_response_name(response), g_free);
}

static void widget_appchooserdialog_response(GtkDialog *dialog,
	gint response, AttributeSet *attributes)
{
	widget_appchooserdialog_store_response(GTK_WIDGET(dialog), response);
	widget_signal_executor(GTK_WIDGET(dialog), attributes, "response");
}

static void widget_appchooserdialog_set_inner_property(GtkWidget *chooser,
	tag_attr *attributes, const gchar *name)
{
	gchar *value;
	gboolean boolean;

	if (attributes == NULL)
		return;
	value = get_tag_attribute(attributes, name);
	if (value == NULL)
		return;
	if (widget_parse_boolean(value, &boolean))
		g_object_set(G_OBJECT(chooser), name, boolean, NULL);
	kill_tag_attribute(attributes, name);
}

GtkWidget *widget_appchooserdialog_create(AttributeSet *attributes,
	tag_attr *tag_attributes, gint type)
{
	GtkWidget *widget;
	GtkWidget *chooser;
	gchar *content_type = NULL;
	gchar *value;

	(void)attributes;
	(void)type;
	if (tag_attributes != NULL)
		content_type = get_tag_attribute(tag_attributes, "content-type");
	if (content_type == NULL || content_type[0] == '\0') {
		gtkdialog_warning("App chooser dialog needs a content-type; using text/plain.");
		content_type = "text/plain";
	}
	widget = gtk_app_chooser_dialog_new_for_content_type(NULL, 0, content_type);
	chooser = gtk_app_chooser_dialog_get_widget(GTK_APP_CHOOSER_DIALOG(widget));
	g_object_set_data_full(G_OBJECT(widget), APPCHOOSER_RESPONSE_DATA,
		g_strdup(""), g_free);
	if (tag_attributes != NULL) {
		value = get_tag_attribute(tag_attributes, "heading");
		if (value != NULL)
			gtk_app_chooser_dialog_set_heading(
				GTK_APP_CHOOSER_DIALOG(widget), value);
		value = get_tag_attribute(tag_attributes, "default-text");
		if (value != NULL)
			gtk_app_chooser_widget_set_default_text(
				GTK_APP_CHOOSER_WIDGET(chooser), value);
		widget_appchooserdialog_set_inner_property(chooser,
			tag_attributes, "show-all");
		widget_appchooserdialog_set_inner_property(chooser,
			tag_attributes, "show-default");
		widget_appchooserdialog_set_inner_property(chooser,
			tag_attributes, "show-fallback");
		widget_appchooserdialog_set_inner_property(chooser,
			tag_attributes, "show-other");
		widget_appchooserdialog_set_inner_property(chooser,
			tag_attributes, "show-recommended");
		kill_tag_attribute(tag_attributes, "content-type");
		kill_tag_attribute(tag_attributes, "heading");
		kill_tag_attribute(tag_attributes, "default-text");
	}
	if (have_geometry_dxdy)
		gtk_widget_set_size_request(widget, geometry_dx, geometry_dy);
	if (have_geometry_xy)
		gtk_window_move(GTK_WINDOW(widget), geometry_x, geometry_y);
	if (option_centering)
		gtk_window_set_position(GTK_WINDOW(widget), GTK_WIN_POS_CENTER_ALWAYS);
	return widget;
}

gchar *widget_appchooserdialog_envvar_construct(GtkWidget *widget)
{
	return widget_appchooser_selected_id(widget);
}

void widget_appchooserdialog_fileselect(variable *var, const char *name,
	const char *value)
{
	(void)var;
	(void)name;
	(void)value;
	gtkdialog_warning("App chooser dialog cannot select an application by ID.");
}

static void widget_appchooserdialog_set_response(GtkWidget *widget,
	const gchar *value, gboolean emit)
{
	gint response;

	if (!widget_dialog_parse_response(value, &response)) {
		gtkdialog_warning("Invalid app chooser dialog response '%s'; ignored.",
			value != NULL ? value : "");
		return;
	}
	if (emit)
		gtk_dialog_response(GTK_DIALOG(widget), response);
	else
		widget_appchooserdialog_store_response(widget, response);
}

static void widget_appchooserdialog_input(variable *var,
	const gchar *source, gboolean command, gboolean emit)
{
	FILE *input = command ? widget_opencommand(source) : fopen(source, "r");
	gchar *line;

	if (input == NULL) {
		gtkdialog_warning("Cannot read app chooser response input '%s'.", source);
		return;
	}
	line = widget_read_line(input);
	if (line != NULL) {
		widget_appchooserdialog_set_response(var->Widget, line, emit);
		g_free(line);
	}
	if (command)
		widget_closecommand(input, source);
	else
		fclose(input);
}

void widget_appchooserdialog_refresh(variable *var)
{
	GList *element;
	gchar *input;
	gboolean initialised;

	initialised = GPOINTER_TO_INT(g_object_get_data(
		G_OBJECT(var->Widget), "_initialised"));
	if (initialised)
		gtk_app_chooser_refresh(GTK_APP_CHOOSER(var->Widget));
	input = attributeset_get_first(&element, var->Attributes, ATTR_INPUT);
	while (input != NULL) {
		if (widget_attribute_is_true(attributeset_get_this_tagattr(
			&element, var->Attributes, ATTR_INPUT, "response"))) {
			if (input_is_shell_command(input))
				widget_appchooserdialog_input(var, input + 8, TRUE,
					initialised);
			else if (g_ascii_strncasecmp(input, "file:", 5) == 0 &&
				input[5] != '\0') {
				if (!initialised)
					widget_file_monitor_try_create(var, input + 5);
				widget_appchooserdialog_input(var, input + 5, FALSE,
					initialised);
			} else if (input[0] != '\0')
				widget_appchooserdialog_input(var, input, TRUE,
					initialised);
		} else if (!initialised) {
			gtkdialog_warning("App chooser dialog input cannot select an application by ID; use response=\"true\" for response input.");
		}
		input = attributeset_get_next(&element, var->Attributes, ATTR_INPUT);
	}
	if (initialised)
		return;
	if (attributeset_is_avail(var->Attributes, ATTR_DEFAULT))
		gtkdialog_warning("App chooser dialog does not support <default> application selection.");
	if (attributeset_is_avail(var->Attributes, ATTR_LABEL))
		gtkdialog_warning("App chooser dialog does not support <label>.");
	if (attributeset_is_avail(var->Attributes, ATTR_ITEM))
		gtkdialog_warning("App chooser dialog does not support <item>.");
	if (attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "false") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "disabled") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "no") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "0"))
		gtk_widget_set_sensitive(var->Widget, FALSE);
	g_signal_connect(G_OBJECT(var->Widget), "response",
		G_CALLBACK(widget_appchooserdialog_response), var->Attributes);
}

void widget_appchooserdialog_save(variable *var)
{
	GError *error;
	GList *element;
	gchar *directive;
	gchar *contents;
	gboolean found = FALSE;
	const gchar *response;

	directive = attributeset_get_first(&element, var->Attributes, ATTR_OUTPUT);
	while (directive != NULL) {
		if (g_ascii_strncasecmp(directive, "file:", 5) == 0 &&
			directive[5] != '\0') {
			found = TRUE;
			if (widget_attribute_is_true(attributeset_get_this_tagattr(
				&element, var->Attributes, ATTR_OUTPUT, "response"))) {
				response = g_object_get_data(G_OBJECT(var->Widget),
					APPCHOOSER_RESPONSE_DATA);
				contents = g_strdup(response != NULL ? response : "");
			} else {
				contents = widget_appchooser_selected_id(var->Widget);
			}
			error = NULL;
			if (!g_file_set_contents(directive + 5, contents, -1,
				&error))
				gtkdialog_warning("Cannot save app chooser output '%s': %s",
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
		gtkdialog_warning("App chooser dialog has no <output file> directive.");
}

void widget_appchooserdialog_clear(variable *var)
{
	g_object_set_data_full(G_OBJECT(var->Widget), APPCHOOSER_RESPONSE_DATA,
		g_strdup(""), g_free);
	gtkdialog_warning("App chooser dialog cannot clear its application selection; only its stored response was reset.");
}

void widget_appchooserdialog_removeselected(variable *var)
{
	(void)var;
	gtkdialog_warning("App chooser dialog cannot remove its application selection.");
}
