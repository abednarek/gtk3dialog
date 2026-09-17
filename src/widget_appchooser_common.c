/*
 * Shared GtkAppChooser value and file output.
 * Copyright (C) 2026 Artur Bednarek <artur@unix.org.pl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <gtk/gtk.h>
#include "config.h"
#include "gtkdialog.h"
#include "attributes.h"
#include "automaton.h"
#include "widgets.h"
#include "widget_appchooser_common.h"

gchar *widget_appchooser_selected_id(GtkWidget *widget)
{
	GAppInfo *app = gtk_app_chooser_get_app_info(GTK_APP_CHOOSER(widget));
	const gchar *id;
	gchar *value;

	if (app == NULL)
		return g_strdup("");
	id = g_app_info_get_id(app);
	value = g_strdup(id != NULL ? id : "");
	g_object_unref(app);
	return value;
}

void widget_appchooser_save_id(variable *var)
{
	GList *element;
	gchar *directive;
	gchar *filename = NULL;
	gchar *selection;
	FILE *output;

	directive = attributeset_get_first(&element, var->Attributes, ATTR_OUTPUT);
	while (directive != NULL) {
		if (g_ascii_strncasecmp(directive, "file:", 5) == 0 &&
			directive[5] != '\0') {
			filename = directive + 5;
			break;
		}
		directive = attributeset_get_next(&element,
			var->Attributes, ATTR_OUTPUT);
	}
	if (filename == NULL) {
		gtkdialog_warning("App chooser has no <output file> directive.");
		return;
	}
	output = fopen(filename, "w");
	if (output == NULL) {
		gtkdialog_warning("Cannot write app chooser output '%s'.", filename);
		return;
	}
	selection = widget_appchooser_selected_id(var->Widget);
	fputs(selection, output);
	g_free(selection);
	widget_close_output(output, filename);
}
