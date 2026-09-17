/*
 * widget_revealer.h: GTK3 GtkRevealer XML widget.
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

#ifndef WIDGET_REVEALER_H
#define WIDGET_REVEALER_H

#include "variables.h"

GtkWidget *widget_revealer_create(AttributeSet *Attr, tag_attr *attr,
	gint Type);
gchar *widget_revealer_envvar_construct(GtkWidget *widget);
void widget_revealer_fileselect(variable *var, const char *name,
	const char *value);
void widget_revealer_refresh(variable *var);
void widget_revealer_save(variable *var);
void widget_revealer_clear(variable *var);
void widget_revealer_removeselected(variable *var);

#endif
