/*
 * widget_listbox.h: GTK3 GtkListBox XML widget.
 * Copyright (C) 2026 Artur Bednarek <artur@unix.org.pl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef WIDGET_LISTBOX_H
#define WIDGET_LISTBOX_H

#include "variables.h"

GtkWidget *widget_listbox_create(AttributeSet *Attr, tag_attr *attr,
	gint Type);
gchar *widget_listbox_envvar_construct(GtkWidget *widget);
void widget_listbox_fileselect(variable *var, const char *name,
	const char *value);
void widget_listbox_refresh(variable *var);
void widget_listbox_save(variable *var);
void widget_listbox_clear(variable *var);
void widget_listbox_removeselected(variable *var);

#endif
