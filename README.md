# GTKDialog

GTKDialog builds GTK+ 3 desktop interfaces from a compact XML-like
description. It is especially useful for shell scripts and other interpreted
programs that need a native graphical interface without a separately compiled
application.

This repository contains the maintained GTK3-only port of the GTKDialog fork.
It keeps the established 0.8.x XML interface where GTK3 can provide equivalent
behaviour, but it does not contain a GTK2 compatibility build. The current
release is `0.9.1`.
The project and source archive remain named `gtkdialog`. The current `gtk3`
branch builds and installs `gdlg3`, and distribution recipes use the same
package name to avoid a command-name collision with GTK2 and other forks.
Version `0.9.1` includes the `gdlg3` executable rename.

![Hierarchical JSON tree in the GTKDialog showcase](screenshots/08-tree-json.png)

## Building

A normal build needs:

- a C compiler, `make` and `pkg-config`;
- GTK 3.24 or later development files, including the Unix printing module;
- the JSON-GLib 1.0 development files.

JSON-GLib is needed for the supported hierarchical JSON input and output of
the tree widget and rich-tree combo boxes, and for JSON-fed menus. Install it for a
complete build of this fork.
For compatibility with older systems, `configure` still permits a reduced
build without it; the historical pipe-separated tree format remains available
in that configuration.

VTE support is optional. It is enabled automatically when VTE 2.91 version
0.60 or later and its development files are present. With VTE, the
`hangupterminal` action can ask a terminal's direct child to exit before
closing a dynamic notebook tab; see `examples/terminal/terminal_tabs`.

GtkSourceView 4 support is optional. It adds source-code editing, syntax
highlighting, line numbers, indentation controls and undo/redo actions to the
existing `edit` widget. Pass `--without-gtksourceview` to disable it explicitly.

From a release archive, build outside the source directory:

```sh
mkdir build
cd build
../configure
make
make install
```

This GitHub snapshot omits `tests/`; the full test suite is maintained
separately.

Use the usual `DESTDIR` or `--prefix` options when packaging or installing to
a non-default location. Run `../configure --help` to see all configuration
options.

Arch Linux, Alpine Linux and Void Linux package recipes are in the repository's
`packaging/` directory. They build against each distribution's
GTK3 and include JSON-GLib, VTE 2.91 and GtkSourceView 4. The recipes require
the `make dist` archive; an automatically generated Git tag archive lacks the
generated Autotools files and is not a substitute.

A Git checkout also needs Autoconf and Automake. Flex and Bison are needed
when regenerating the lexer or parser. Generate the build system first, then
use the same out-of-tree procedure:

```sh
NOCONFIGURE=1 ./autogen.sh
mkdir build
cd build
../configure
make
```

The Texinfo manual is not built by default. Pass `--enable-texinfo` to
`configure` if `makeinfo` is available and the manual should be built and
installed.

## Running a dialog

An interface is normally exported as a shell variable and selected with
`--program`:

```sh
export MAIN_DIALOG='<window title="Hello">
  <vbox border-width="12" spacing="8">
    <text><label>GTKDialog is ready.</label></text>
    <button ok></button>
  </vbox>
</window>'

gdlg3 --program=MAIN_DIALOG
```

The complete language and widget index starts at
[`doc/reference/syntax.html`](doc/reference/syntax.html). Individual reference
pages describe attributes, directives, signals, functions and conditions.

## Current GTK3 port

The fork extends the original interface without changing the default meaning
of existing widget tags and data formats. Highlights include:

- hierarchical, versioned JSON trees alongside the original flat tree input;
- editable tree text, combo, toggle, radio, spin and accelerator cells;
- progress, spinner, icon, image, Pango markup and colour renderers, with
  optional row and cell styling, plus flat rich icon-view items with stable
  values, descriptions and tooltips;
- opt-in rich combo-box, editable combo and entry-completion rows with stable
  values, icons, markup, sensitivity, styling and descriptions, plus
  JSON-backed hierarchical submenus, while the original text models remain
  the default;
- row-aware and keyboard-accessible popup menus, including a standalone
  desktop menu program;
- scrolling text editors with optional GtkSourceView 4 source-code editing
  and undo/redo actions;
- GTK3 layout and container backends for the historical `grid`, `table`,
  `hbox`, `vbox`, paned-window, scale, scrollbar and separator tags; scrolled
  windows, explicit viewports, fixed positioning, drawing areas and large
  layout canvases;
- a GTK3-only `<revealer>` container with animated disclosure, Boolean
  input/output and synchronous state actions;
- a GTK3-only `<stack>` container with named pages, animated transitions,
  page-name input/output and synchronous selection actions;
- a GTK3-only `<stackswitcher>` control for navigating a named stack without
  changing where page-selection actions are handled;
- a GTK3-only `<stacksidebar>` for vertical navigation of the same stack pages;
- notebook pages built at runtime from named XML fragments, with per-instance
  variables and actions, explicit removal and optional close buttons;
- `<hbox>` and `<vbox>` contents replaced at runtime from XML templates with
  `replacebox`, preserving box packing and releasing old widget resources;
- a GTK3-only `<listbox>` with named rows, selected-row input/output and
  synchronous selection and activation actions;
- a GTK3-only `<flowbox>` with named selectable tiles, adaptive layout and
  independent child actions;
- a GTK3-only `<placessidebar>` exposing selected file locations as URIs
  through synchronous `open-location` actions and ordinary input/output;
- a GTK3-only `<overlay>` container for layered content and interactive
  badges, preserving each child's actions;
- a GTK3-only `<actionbar>` with start, center and end child placement;
- a GTK3-only `<headerbar>` for an opt-in client-side window titlebar with
  start, end and custom-center child controls;
- GTK3-only `<menubutton>` and `<popover>` for anchored, interactive popup
  content whose child controls retain their variables and actions;
- GTK3-only `<popovermenu>` and `<modelbutton>` for named popup submenus
  and ordinary synchronous GTKDialog item actions;
- a GTK3-only `<switch>` control with Boolean input/output and synchronous
  state actions;
- a GTK3-only `<searchentry>` that retains entry input/output and completion
  while exposing native delayed search and match-navigation signals;
- a GTK3-only `<searchbar>` for an entry or searchentry child, with optional
  keyboard capture and Boolean search-mode actions;
- a GTK3-only `<levelbar>` for bounded numeric status with input/output and
  synchronous value-change actions;
- a GTK3-only `<appchooserbutton>` for selecting an application registered
  for a content type, exporting its desktop ID and running `changed` actions;
- a GTK3-only embedded `<appchooser>` with desktop-ID output and distinct
  selection and activation actions;
- a GTK3-only `<appchooserdialog>` with desktop-ID output and synchronous
  response actions;
- a GTK3-only `<colorchooserdialog>` with colour and response output;
- a GTK3-only `<fontchooserdialog>` with font and response output;
- a GTK3-only `<recentchooserdialog>` with recent-resource URI and response
  output;
- a GTK3-only `<shortcutswindow>` with named sections, groups and shortcut
  entries, section selection and synchronous close/search actions;
- a native GTK3 image viewer with bounded zoom, aspect-preserving fit modes,
  Ctrl-wheel scaling, drag panning and original-image input/output;
- standalone scrollbars and GTK3-drawn measurement rulers, full embedded
  colour and font selectors, and a native hue/saturation/value selector;
- native accelerator labels bound to named menu items regardless of their XML
  order, and directional arrow indicators;
- notification-area status icons with theme or file images, action dispatch
  and positioned reusable context menus;
- toolbars, static handle-box compatibility containers, tool palettes and
  their GTK3 tool-item families;
- off-screen GTK3 program roots that render one declarative child hierarchy
  directly to an image file without creating a desktop window;
- GTK3 Unix page-setup and print-dialog program roots with portable page and
  print-settings key-file input/output and selected-printer output;
- expanded file, recent-file and icon selection widgets, including native
  recent-resource submenus and top-level open, save and folder chooser dialogs;
  native dialogs, optional VTE terminals and generic XEmbed socket hosts and
  plug roots.

### Showcase

<table>
  <tr>
    <td width="50%" align="center"><strong>Editable tree renderers</strong><br><img src="screenshots/04-tree-renderers.png" alt="Tree combo cell being edited"></td>
    <td width="50%" align="center"><strong>Nested JSON tree</strong><br><img src="screenshots/08-tree-json.png" alt="Expanded JSON tree with typed renderers"></td>
  </tr>
  <tr>
    <td width="50%" align="center"><strong>Popup menus</strong><br><img src="screenshots/09-popup-menus.png" alt="Row-aware tree popup menu"></td>
    <td width="50%" align="center"><strong>GTK3 widgets</strong><br><img src="screenshots/11-gtk3-widgets.png" alt="Navigation, places and popovers"></td>
  </tr>
  <tr>
    <td width="50%" align="center"><strong>State actions</strong><br><img src="screenshots/12-state-actions.png" alt="Widget state actions and conditions"></td>
    <td width="50%" align="center"><strong>Image viewer</strong><br><img src="screenshots/29-image-viewer.png" alt="GTK3 image viewer with fit modes"></td>
  </tr>
  <tr>
    <td width="50%" align="center"><strong>Embedded terminal</strong><br><img src="screenshots/22-terminal.png" alt="GTK3 VTE terminal"></td>
    <td width="50%" align="center"><strong>Generic XEmbed socket and plug</strong><br><img src="screenshots/23-xembed-socket.png" alt="XEmbed clients hosted by GtkSocket on X11"></td>
  </tr>
  <tr>
    <td width="50%" align="center"><strong>Embedded colour selector</strong><br><img src="screenshots/25-colour-selector.png" alt="GtkColorChooserWidget with palette and opacity"></td>
    <td width="50%" align="center"><strong>Embedded font selector</strong><br><img src="screenshots/26-font-selector.png" alt="GtkFontChooserWidget with family, face, size and preview"></td>
  </tr>
</table>

Every showcase page is included in the
[`screenshots/` gallery](screenshots/README.md). To run the same interface with
a freshly built executable:

```sh
GTKDIALOG=/path/to/build/src/gdlg3 ./examples/showcase/showcase
```

The optional client-side titlebar has a separate example so the gallery's
native window-manager decorations remain consistent:

```sh
GTKDIALOG=/path/to/build/src/gdlg3 ./examples/showcase/headerbar
```

## Compatibility

Existing 0.8.x XML interfaces, widget names and the flat tree format remain
supported where GTK3 has an equivalent. The documented unavoidable differences
and migration alternatives are collected in
[`doc/gtk3-porting.md`](doc/gtk3-porting.md). New behaviour that could
reinterpret existing data is opt-in: for
example, hierarchical tree data requires `format="json"`, custom tree
renderers require `column-renderer`, and context menus require `context-menu`.

The parser stack and each container's widget list grow dynamically. There is
no architecture-specific widget-order workaround and no fixed per-container
widget limit.

## License and authors

GTKDialog is free software licensed under the GNU General Public License,
version 2 or, at your option, any later version (`GPL-2.0-or-later`). See
[`COPYING`](COPYING) for the complete GPL version 2 text. Some example icons
have separate copyright notices in the accompanying `COPYING-*` files.

See [`AUTHORS`](AUTHORS) for the original authors and contributors. This fork
is maintained by [Artur Bednarek](mailto:artur@unix.org.pl).

- [Current project](https://github.com/abednarek/gtk3dialog)
- [GTK2 fork history](https://github.com/abednarek/gtkdialog)
- [Archived upstream](https://github.com/oshazard/gtkdialog)
