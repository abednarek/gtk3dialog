/*
 * widget_levelbar.h: GTK3 GtkLevelBar XML widget.
 * Copyright (C) 2026 Artur Bednarek <artur@unix.org.pl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef WIDGET_LEVELBAR_H
#define WIDGET_LEVELBAR_H

#include "variables.h"

GtkWidget *widget_levelbar_create(AttributeSet *Attr, tag_attr *attr, gint Type);
gchar *widget_levelbar_envvar_construct(GtkWidget *widget);
void widget_levelbar_fileselect(variable *var, const char *name,
	const char *value);
void widget_levelbar_refresh(variable *var);
void widget_levelbar_save(variable *var);
void widget_levelbar_clear(variable *var);
void widget_levelbar_removeselected(variable *var);

#endif
