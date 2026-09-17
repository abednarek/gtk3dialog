/*
 * widget_stacksidebar.h: GTK3 GtkStackSidebar XML widget.
 * Copyright (C) 2026 Artur Bednarek <artur@unix.org.pl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef WIDGET_STACKSIDEBAR_H
#define WIDGET_STACKSIDEBAR_H

#include "variables.h"

GtkWidget *widget_stacksidebar_create(AttributeSet *Attr, tag_attr *attr,
	gint Type);
gchar *widget_stacksidebar_envvar_construct(GtkWidget *widget);
void widget_stacksidebar_refresh(variable *var);

#endif
