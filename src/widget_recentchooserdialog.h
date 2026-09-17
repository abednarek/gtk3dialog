/*
 * widget_recentchooserdialog.h: GTK3 recent resource chooser dialog.
 * Copyright (C) 2026 Artur Bednarek <artur@unix.org.pl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef WIDGET_RECENTCHOOSERDIALOG_H
#define WIDGET_RECENTCHOOSERDIALOG_H

#include <gtk/gtk.h>
#include "attributes.h"
#include "tag_attributes.h"
#include "variables.h"

GtkWidget *widget_recentchooserdialog_create(AttributeSet *attributes,
	tag_attr *tag_attributes, gint type);
gchar *widget_recentchooserdialog_envvar_construct(GtkWidget *widget);
void widget_recentchooserdialog_fileselect(variable *var, const char *name,
	const char *value);
void widget_recentchooserdialog_refresh(variable *var);
void widget_recentchooserdialog_save(variable *var);
void widget_recentchooserdialog_clear(variable *var);
void widget_recentchooserdialog_removeselected(variable *var);

#endif
