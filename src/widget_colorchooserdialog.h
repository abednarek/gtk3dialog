/*
 * widget_colorchooserdialog.h: GTK3 colour chooser dialog.
 * Copyright (C) 2026 Artur Bednarek <artur@unix.org.pl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef WIDGET_COLORCHOOSERDIALOG_H
#define WIDGET_COLORCHOOSERDIALOG_H

#include <gtk/gtk.h>
#include "attributes.h"
#include "tag_attributes.h"
#include "variables.h"

GtkWidget *widget_colorchooserdialog_create(AttributeSet *attributes,
	tag_attr *tag_attributes, gint type);
gchar *widget_colorchooserdialog_envvar_construct(GtkWidget *widget);
void widget_colorchooserdialog_fileselect(variable *var, const char *name,
	const char *value);
void widget_colorchooserdialog_refresh(variable *var);
void widget_colorchooserdialog_save(variable *var);
void widget_colorchooserdialog_clear(variable *var);
void widget_colorchooserdialog_removeselected(variable *var);

#endif
