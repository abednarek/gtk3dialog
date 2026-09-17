/*
 * widget_placessidebar.h: GTK3 GtkPlacesSidebar XML widget.
 * Copyright (C) 2026 Artur Bednarek <artur@unix.org.pl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef WIDGET_PLACESSIDEBAR_H
#define WIDGET_PLACESSIDEBAR_H

#include "variables.h"

GtkWidget *widget_placessidebar_create(AttributeSet *Attr, tag_attr *attr,
	gint Type);
gchar *widget_placessidebar_envvar_construct(GtkWidget *widget);
void widget_placessidebar_fileselect(variable *var, const char *name,
	const char *value);
void widget_placessidebar_refresh(variable *var);
void widget_placessidebar_save(variable *var);
void widget_placessidebar_clear(variable *var);
void widget_placessidebar_removeselected(variable *var);

#endif
