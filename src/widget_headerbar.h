/*
 * widget_headerbar.h: GTK3 GtkHeaderBar XML container.
 * Copyright (C) 2026 Artur Bednarek <artur@unix.org.pl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef WIDGET_HEADERBAR_H
#define WIDGET_HEADERBAR_H

#include "variables.h"

GtkWidget *widget_headerbar_create(AttributeSet *Attr, tag_attr *attr,
	gint Type);
gchar *widget_headerbar_envvar_construct(GtkWidget *widget);
void widget_headerbar_refresh(variable *var);
void widget_headerbar_save(variable *var);
void widget_headerbar_clear(variable *var);
void widget_headerbar_removeselected(variable *var);

#endif
