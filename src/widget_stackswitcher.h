/*
 * widget_stackswitcher.h: GTK3 GtkStackSwitcher XML widget.
 * Copyright (C) 2026 Artur Bednarek <artur@unix.org.pl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef WIDGET_STACKSWITCHER_H
#define WIDGET_STACKSWITCHER_H

#include "variables.h"

GtkWidget *widget_stackswitcher_create(AttributeSet *Attr, tag_attr *attr,
	gint Type);
gchar *widget_stackswitcher_envvar_construct(GtkWidget *widget);
void widget_stackswitcher_refresh(variable *var);

#endif
