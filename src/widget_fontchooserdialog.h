/*
 * widget_fontchooserdialog.h: GTK3 font chooser dialog.
 * Copyright (C) 2026 Artur Bednarek <artur@unix.org.pl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef WIDGET_FONTCHOOSERDIALOG_H
#define WIDGET_FONTCHOOSERDIALOG_H

#include <gtk/gtk.h>
#include "attributes.h"
#include "tag_attributes.h"
#include "variables.h"

GtkWidget *widget_fontchooserdialog_create(AttributeSet *attributes,
	tag_attr *tag_attributes, gint type);
gchar *widget_fontchooserdialog_envvar_construct(GtkWidget *widget);
void widget_fontchooserdialog_fileselect(variable *var, const char *name,
	const char *value);
void widget_fontchooserdialog_refresh(variable *var);
void widget_fontchooserdialog_save(variable *var);
void widget_fontchooserdialog_clear(variable *var);
void widget_fontchooserdialog_removeselected(variable *var);

#endif
