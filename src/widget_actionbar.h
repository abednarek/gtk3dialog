/*
 * widget_actionbar.h: GTK3 GtkActionBar XML container.
 * Copyright (C) 2026 Artur Bednarek <artur@unix.org.pl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef WIDGET_ACTIONBAR_H
#define WIDGET_ACTIONBAR_H

#include "variables.h"

GtkWidget *widget_actionbar_create(AttributeSet *Attr, tag_attr *attr,
	gint Type);
gchar *widget_actionbar_envvar_construct(GtkWidget *widget);
void widget_actionbar_fileselect(variable *var, const char *name,
	const char *value);
void widget_actionbar_refresh(variable *var);
void widget_actionbar_save(variable *var);
void widget_actionbar_clear(variable *var);
void widget_actionbar_removeselected(variable *var);

#endif
