/* GTK3 shortcuts window and its XML children.
 * Copyright (C) 2026 Artur Bednarek <artur@unix.org.pl>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef GTKDIALOG_WIDGET_SHORTCUTS_H
#define GTKDIALOG_WIDGET_SHORTCUTS_H

#include <gtk/gtk.h>
#include "attributes.h"
#include "variables.h"
#include "tag_attributes.h"

GtkWidget *widget_shortcuts_create(AttributeSet *attributes,
	tag_attr *tag_attributes, gint type);
gchar *widget_shortcuts_envvar_construct(GtkWidget *widget, gint type);
void widget_shortcuts_fileselect(variable *var, const char *name,
	const char *value);
void widget_shortcuts_refresh(variable *var);
void widget_shortcuts_save(variable *var);
void widget_shortcuts_clear(variable *var);
void widget_shortcuts_removeselected(variable *var);

#endif
