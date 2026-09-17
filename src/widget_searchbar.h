/*
 * widget_searchbar.h: GTK3 GtkSearchBar XML widget.
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

#ifndef WIDGET_SEARCHBAR_H
#define WIDGET_SEARCHBAR_H

#include "variables.h"

GtkWidget *widget_searchbar_create(AttributeSet *Attr, tag_attr *attr,
	gint Type);
gchar *widget_searchbar_envvar_construct(GtkWidget *widget);
void widget_searchbar_fileselect(variable *var, const char *name,
	const char *value);
void widget_searchbar_refresh(variable *var);
void widget_searchbar_save(variable *var);
void widget_searchbar_clear(variable *var);
void widget_searchbar_removeselected(variable *var);

#endif
