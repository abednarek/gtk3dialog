# GTK3 porting notes

This branch is a GTK3-only port of the maintained GTKDialog fork.  It is not a
dual GTK2/GTK3 build.  The XML language, tag names and action ordering remain
compatible wherever GTK3 can provide the same behaviour.

The current branch installs `gdlg3` rather than `gtkdialog`. Scripts that
invoke the old executable name must call `gdlg3` (or set their `GTKDIALOG`
override to its path); XML tags, variables and action syntax are unchanged.
No `gtkdialog` command alias is installed, so GTK2 and GTK3 commands can
coexist. Version `0.9.1` includes this command rename.

The port starts at `ead071db27f0978504f1b655268e52927e1440c5` from the GTK2
repository.  Historical GTK2 documentation remains historical; GTK3-specific
changes are recorded here and in newly updated user documentation.

## Initial API audit

| Category | GTK2 API found in the source | GTK3 decision |
| --- | --- | --- |
| Available with the same model | `GtkTreeView`, tree/list stores and cell renderers, `GtkNotebook`, `GtkAssistant`, `GtkToolbar`, `GtkToolPalette`, `GtkRecentChooser`, `GtkFileChooser`, `GtkTextView`, `GtkIconView`, `GtkInfoBar` | Retain the XML contract and behaviour; update signatures or deprecated convenience calls where strict compilation requires it. |
| Directional convenience classes | `GtkHBox`, `GtkVBox`, `GtkHPaned`, `GtkVPaned`, `GtkHScale`, `GtkVScale`, `GtkHScrollbar`, `GtkVScrollbar`, `GtkHSeparator`, `GtkVSeparator` | Construct `GtkBox`, `GtkPaned`, `GtkScale`, `GtkScrollbar` and `GtkSeparator` with an explicit orientation.  Historical XML tag names remain accepted. |
| Removed object and list APIs | `GtkObject`, `GtkList`, `GtkListItem`, `GtkCList`, `GtkCombo`, `GtkProgress`, `GtkItem` | Use GObject data, `GtkListStore`/`GtkTreeView`, `GtkComboBoxText`, `GtkProgressBar` and the relevant concrete menu/item classes. |
| Colour APIs | `GdkColor`, `GtkColorButton` colour accessors and `GtkColorSelection` | Use `GdkRGBA`, `GtkColorChooser` and `GtkColorChooserWidget`. Keep the `<colorbutton>` and `<colorselection>` names. |
| Font selection | `GtkFontSelection` and direct access to its internal child lists | Back `<fontselection>` with `GtkFontChooserWidget` and use only its public value and notification API. |
| Table layouts | `GtkTable` backs both `<grid>` and `<table>` | Use `GtkGrid` internally and translate the existing left/right/top/bottom attachment contract.  Do not rename either XML tag. |
| Detachable containers | `GtkHandleBox` | Keep `<handlebox>` as a static GTK3 single-child container; GTK3 has no supported detachable replacement. |
| XEmbed | `GtkSocket`, `GtkPlug`, `GdkNativeWindow` | Build the standard GTK3 X11 backend through `<gtk/gtkx.h>`.  On non-X11 GTK3 builds, report the widget as unavailable rather than adding a nonstandard Wayland embedding protocol. |
| Styling | `gtk_rc_parse`, `GdkColor`, direct widget style fields and `GtkRcStyle` | Replace runtime style changes with `GtkCssProvider` and style contexts.  GTK2 `gtkrc` syntax is not interpreted by GTK3; use GTK3 CSS. |
| Drawing | `expose-event`, `GdkEventExpose`, `gdk_draw_pixbuf` and direct windows | Use the GTK3 `draw` signal and Cairo.  Preserve drawing command order and image placement. |
| Main-loop input | `gdk_input_add` and `gdk_input_remove` | Use GLib Unix file-descriptor sources with identical callback ordering. |
| VTE | legacy `vte` 0.x API | Target `vte-2.91`; translate spawn, feed and colour calls while preserving terminal actions and output. |

## Compatibility policy

* There is no GTK2 build switch and no GTK2 dependency.
* Existing tags including `<grid>`, `<table>`, `<hbox>` and `<vbox>` keep their
  names even when their GTK3 backend class changes.
* Synchronous actions retain their source order.  The `exit()` action remains
  the existing termination mechanism unless an explicit behavioural comparison
  proves a replacement equivalent.
* Glade and libglade are not restored.
* Wayland support is limited to what unmodified GTK3 provides.  XEmbed remains
  an X11-only facility.
* Build products are generated out of tree and are not committed.

## Post-port verification

The GTK3 branch was compared mechanically with its
`ead071db27f0978504f1b655268e52927e1440c5` starting point after the port:

| Compatibility surface | Retained | Result |
| --- | ---: | ---: |
| Registered XML widget-type identifiers | 93/93 | 100% |
| Recognised action-command prefixes | 22/22 | 100% |
| Named signals dispatched to XML actions | 40/40 | 100% |
| Automated test programs on glibc and Alpine/musl | 6/6 on each | 100% |
| Showcase pages launched and visually inspected | 26/26 | 100% |

This means the code port is complete and the public XML/action vocabulary is
preserved. Practical behavioural compatibility is estimated at **95%** rather
than claimed as 100%: this deliberately non-normative estimate gives equal
weight to the 89 documented public tag pages and discounts the tags with
partial GTK3 semantics (`<alignment>`, `<colorselection>`, `<handlebox>` and
`<terminal>`), plus the global GTK2-to-GTK3 stylesheet syntax change. The
precise, script-relevant differences and replacements are listed below; the
estimate must not be interpreted as a coverage guarantee for arbitrary
third-party scripts.

## Backend map

| XML name | GTK3 backend | Compatibility status |
| --- | --- | --- |
| `<hbox>`, `<vbox>` | oriented `GtkBox` | Preserved, including packing attributes and scrolling wrapper. |
| `<hpaned>`, `<vpaned>` | oriented `GtkPaned` | Preserved. |
| `<hscale>`, `<vscale>` | oriented `GtkScale` | Preserved, including marks and range actions. |
| `<hscrollbar>`, `<vscrollbar>` | oriented `GtkScrollbar` | Preserved. |
| `<hseparator>`, `<vseparator>` | oriented `GtkSeparator` | Preserved. |
| layout `<grid>` | `GtkGrid` | Preserved; historical row, column, span and packing attributes are translated. |
| data `<table>` | `GtkTreeView` with `GtkListStore` | Preserved; selection, sorting, input, output and row actions are implemented without `GtkCList`. |
| `<list>` | `GtkListBox` | Preserved for its historical single-selection contract without `GtkList`. |
| `<iconview>` | `GtkIconView` with flat `GtkListStore` | The established text/image model remains the default; `item-model="rich"` adds stable values, markup, descriptions, tooltips and row styling without changing selection or action ordering. |
| `<combobox>` | editable `GtkComboBoxText` | Preserved without `GtkCombo`. |
| `<comboboxtext>` | `GtkComboBoxText`, or opt-in `GtkComboBox` with `GtkListStore`/`GtkTreeStore` | The original text model remains the default; `item-model="rich"` adds stable row values and styling, while `rich-tree` adds JSON-backed hierarchical GTK3 submenus. |
| `<comboboxentry>` | `GtkComboBoxText`, or opt-in entry-enabled `GtkComboBox` with `GtkListStore` | The original editable text model remains the default; `item-model="rich"` adds stable row values and styled drop-down rows, while optional `GtkEntryCompletion` shares either flat model and preserves stable selected values. Unmatched entry text remains editable and exportable. |
| Empty `<notebook>` | `GtkNotebook` with zero pages | Accepted without a placeholder child; its value is `-1` while no page is selected. Existing box or notebook children can later be moved into it with `reparentwidget`; `settablabel` can rename and `reorderpage` can reposition a live page. `appendtab`/`removetab` manage named template pages, while `appendtabauto` creates and selects automatically numbered instances. Optional close buttons work on initial and dynamic pages; `tab-close-action` runs before removing a dynamic page. |
| `replacebox` action | `GtkBox` with a staged XML fragment | Replaces `<hbox>`/`<vbox>` children only after one new root is built. Removed subtrees release widget variables, timers, monitors and accelerators; the inserted child retains GTKDialog packing rules. |
| `<entry>` completion | `GtkEntryCompletion` with `GtkListStore` | The original text suggestions remain the default; `completion-model="rich"` adds stable values, icons, markup, sensitivity, styling and descriptions while later typed edits export visible text. |
| `<tree>` cell completion | `GtkEntryCompletion` attached when a cell starts editing | Opt-in flat suggestions work for editable text, markup and icon-text cells and entry-enabled combo cells. Selection still commits through the existing `cell-edited` path, preserving stored values and action order. |
| `<edit sourceview="true">` | optional `GtkSourceView` 4 | Source editing and its `undo`/`redo` actions use the GTK3-based GtkSourceView 4 API; without that optional dependency the tag retains its ordinary `GtkTextView` backend, history actions are no-ops, and the unavailable request is reported. |
| `<colorselection>` | `GtkColorChooserWidget` | Preserved with the palette limitation described below. |
| `<fontselection>` | `GtkFontChooserWidget` | Preserved through public chooser properties and notifications. |
| `<handlebox>` | static single-child `GtkBox` | Layout preserved; detachment is unavailable. |
| `<offscreenwindow>` | `GtkOffscreenWindow` | Preserved as an invisible one-child program root with redraw and image-save actions. |
| `<filechooserdialog>` | `GtkFileChooserDialog` | Preserved as a top-level open, save or folder chooser with response and selection actions. |
| `<pagesetupdialog>` | GTK3 `GtkPageSetupUnixDialog` | Preserved through the required GTK3 Unix printing module, including page and print-settings key files. |
| `<printdialog>` | GTK3 `GtkPrintUnixDialog` | Preserved through the required GTK3 Unix printing module, including capabilities, responses and page, print-settings and printer-name output. |
| `<statusicon>` | GTK3 `GtkStatusIcon` owned by a zero-size proxy | XML values, actions and lifecycle are preserved; removed native blinking is reproduced with a window-owned timer, while display still depends on a desktop notification-area host. |
| `<toolpalette>`, `<toolitemgroup>` | `GtkToolPalette`, `GtkToolItemGroup` | Preserved; these classes remain available in GTK 3.24. |
| `<socket>`, `<plug>`, `<gvim>` | GTK3 `GtkSocket` and `GtkPlug` from `gtk/gtkx.h` | Host and client contracts are preserved on X11 only; unavailable on non-X11 GTK builds. |
| `<recentchoosermenu>` | `GtkRecentChooserMenu` owned by `GtkMenuItem` | Preserved, including filters, current URI and item actions. |
| `<menubar>` and `<menu>` JSON input | `GtkMenuShell` with JSON-GLib | Optional `format="json"` file input creates nested GTK3 menu items while keeping existing XML children and legacy input unchanged. Invalid refreshes leave the previous generated items intact. |
| `<accellabel>` | `GtkAccelLabel` | Preserved, including deferred binding to accelerators on named widgets. |
| `<arrow>` | `GtkArrow` | Preserved. GTK3 deprecates the class but retains its direction, relief, alignment and padding behaviour. |
| `<hsv>` | `GtkHSV` | Preserved. GTK3 deprecates the class but retains its value, geometry and changed-signal contract. |
| `<hruler>`, `<vruler>` | custom `GtkDrawingArea` with Cairo/Pango | Range, position, metrics, input/output and lifecycle are preserved after GTK3 removed `GtkRuler`; tick rendering follows GTK3 theme colours and may differ from GTK2. |
| drawing tags | `draw` signal and Cairo | Preserved without expose-event or direct `GdkWindow` drawing. |
| `<imageview>` | `GtkDrawingArea` in a `GtkScrolledWindow` with Cairo/GdkPixbuf rendering | Preserved, including bounded zoom, fit modes, drag panning, image input/output, the retained `expose-event` XML action name and all six scaling functions. |

Deprecated GTK stock identifiers remain accepted because they are part of the
existing XML language and GTK 3.24 still implements them.  A GTK3 icon theme
is not required to provide artwork for every old stock name; scripts seeking a
theme-independent result should use `icon-name` with a current symbolic icon
or an explicit image file.

`GtkOffscreenWindow` remains available in GTK 3.24, although GTK deprecates
the class. The `<offscreenwindow>` tag keeps the later maintained-fork XML
contract: it owns exactly one direct child, `refresh` queues a redraw and
`save` writes the current rendering through GdkPixbuf. It has no normal window
manager surface, so `presentwindow` intentionally has no visual effect. This
uses only GTK3's standard backend and adds no separate Wayland protocol.

## GTK3-only additions

GTK3-only additions after 0.9.0 are not measured as GTK2 compatibility work.
The top-level `<shortcutswindow>` uses the native `GtkShortcutsWindow` and
contains one or more `<shortcutssection>` elements; each section contains
`<shortcutsgroup>` elements, and each group contains `<shortcutsshortcut>`
entries. GTK3 indexes the section, group and shortcut properties when they
are added, so these attributes are applied before parenting. The selected
`section-name` is the root value; default, command/file input, refresh,
file output, save and `fileselect:` select or report a section. `close` and
`search` are native keybinding signals; the root runs synchronous actions
for them, with a plain action mapped to `close`. An explicit
`section-changed` action runs when the mapped window changes section.
`closewindow:` and later `launch:` keep the established window lifecycle.
The native titlebar close button uses `delete-event`, which also unregisters
the window before it can be launched again; it is distinct from the
`close` keybinding signal.
GTK3's built-in shortcuts window draws its own search/title controls, so
it is intentionally a separate program root, not an ordinary child of
`<window>`. It has no GTK2 counterpart.
The top-level `<recentchooserdialog>` uses `GtkRecentChooserDialog`, sharing
the embedded selector's URI and filter contract. It supports selection input,
separate URI and response output, synchronous response actions and
close/relaunch. Clearing selection never deletes desktop recent-file history.
The top-level `<fontchooserdialog>` uses `GtkFontChooserDialog`, exporting
the same GTK3 font description as the embedded selector. It supports
default/input/refresh, separate font and response files, synchronous response
actions, and close/relaunch. `fileselect:` remains a file-path action and
does not set a font; `clear` only resets the stored response.
The top-level `<colorchooserdialog>` uses `GtkColorChooserDialog`, retaining
the embedded selector's `#rrggbb|alpha` value format when alpha is enabled.
It saves colour and response separately and runs synchronous response actions.
Colour input is supported; `fileselect:` remains a file-path action and is
not used for colours. `clear` only resets the stored response, because a
colour chooser always has a current colour.
The top-level `<appchooserdialog>` uses `GtkAppChooserDialog` for the same
desktop-ID value as the embedded and button variants. Dialog responses run
ordinary synchronous `response` actions; the selected ID and response name
can be saved to separate files. An `<input response="true">` can emit a
named response on refresh, but GTK3 provides no stable way to select an
application by desktop ID. The dialog can be closed and launched again.
The embedded `<appchooser>` uses `GtkAppChooserWidget` for the same
content-type-to-desktop-ID contract as `<appchooserbutton>`. Its
`application-selected` and `application-activated` signals are separate:
plain `<action>` runs on activation, while selection actions require an
explicit signal. GTK3 exposes no public stable select-by-desktop-ID method
for this widget either, so default/input/fileselect and clear are not
promised. `refresh` reloads installed applications; `save` writes the
selected ID.
The new `<appchooserbutton>` uses GTK3's installed-application registry for
a `content-type` such as `text/plain`. Its value and saved output are the
selected desktop application ID, or an empty string if none is available.
Native `changed` actions run synchronously. `refresh` reloads the registry.
GTK3 does not expose a stable select-by-desktop-ID operation for this button,
so GTKDialog does not accept `<default>`, `<input>` or `fileselect:` for it;
`clear` and `removeselected` are likewise unsupported. Scripts needing
programmatic application choice should use their own application list.
The new `<revealer>` tag uses `GtkRevealer` with one child and a Boolean
requested reveal state. Its `activate` action toggles that state and its
`reveal-changed` action runs synchronously when the state changes. Common
`show` and `hide` still control visibility of the whole widget rather than
its child reveal state. Scripts targeting GTK2 should use `<expander>` or
explicit show/hide actions instead; neither provides the same animation.

The new `<stack>` tag uses `GtkStack` for named pages without notebook tabs.
Its exported value is the visible page name. `names` and `titles` identify
direct child pages, while input, refresh, clear and save use that name.
`page-changed` actions run synchronously when selection changes. A GTK2
script can use `<notebook>` for multiple pages, but its tab interface and
numeric page value are intentionally different.
The `<stackswitcher>` tag binds a GTK3 `GtkStackSwitcher` to a named
`<stack>` variable, even if the switcher appears first in XML. It owns no
separate selection state: scripts attach page-change actions to the stack.
The `<stacksidebar>` tag provides vertical `GtkStackSidebar` navigation for
the same named stack. It can precede or follow the stack in XML and leaves
the selected page value and synchronous actions on `<stack>`.
The new `<listbox>` tag wraps each direct child widget in a native
`GtkListBoxRow`. Its value is a stable row name, which can be selected through
default/input/refresh and exported or saved. Selection and activation run
separate synchronous actions, while each child retains its own actions.
Only single and disabled selection are supported; multi-selection has no
stable scalar representation in the existing GTKDialog value contract.
The new `<flowbox>` tag uses `GtkFlowBox` for wrapping tile layouts. Like
`<listbox>`, direct children retain their own actions while the container
exports the selected stable name. Its initial selection is applied after
the GTK3 flow layout is mapped, since GTK3 may otherwise select the first
tile while mapping. Later refreshes select synchronously. Multi-selection
is likewise omitted from this scalar value contract.
The `<placessidebar>` tag uses GTK3's places and volume view. Its value is
a URI, including `file://` for local paths, so scripts can distinguish local
and remote locations. Opening a place runs `open-location` synchronously;
default/input/refresh track the surrounding application's current location.
GTK may not highlight a requested location that is not in its places list,
although GTKDialog retains and exports the requested URI. The caller must
show the selected folder; this widget does not implement a file view.
The new `<overlay>` tag uses `GtkOverlay`: the first direct child is the
main surface and later children are laid over it. Children keep their own
values and actions. Per-child `pass-through` requests use GTK3's overlay
child property; widget alignment and margins determine placement.
The new `<actionbar>` tag uses `GtkActionBar` to place direct children at
the start, center or end through the child's `actionbar-pack` attribute.
The container has no independent value; child variables and actions remain
the source of state. It is an addition, not a replacement for `<hbuttonbox>`.
The new `<headerbar>` tag uses `GtkHeaderBar`. When it is a direct child of
`<window>`, the window attaches it as a client-side titlebar, with one other
direct child remaining as the ordinary window body. This intentionally
changes that window's decoration from the native window-manager titlebar;
windows without `<headerbar>` keep their existing decorations. Children use
`headerbar-pack` for start/end/custom-center placement and retain their own
actions. A custom center widget replaces the built-in title/subtitle labels.
The GTK3-only `<menubutton>` owns one `<popover>`, which in turn owns one
content widget such as `<vbox>`. The button exports its open/closed Boolean
state and runs synchronous `toggled` actions; the popover runs `closed`
actions when GTK emits its native close signal. Scripts that need every
open/closed state transition should use the button's `toggled` action.
Controls inside the content keep their own
values and action order. These tags do not reinterpret historical `<menu>`
or `<popupmenu>` XML and have no GTK2 equivalent. The button anchors the
popover; a standalone `<popover>` is not a supported root widget.
The related `<popovermenu>` tag adds named pages to a `<menubutton>`.
Its first direct child is the main page; later children use a unique
`submenu` name. Plain `<modelbutton>` items navigate to these pages through
`menu-name` or run normal clicked actions. GTK's native menu activation can
close the popup before a terminal item's clicked action, so these synchronous
actions are observed in that native order. This XML backend does not create
`GAction` objects and therefore does not expose check/radio model-button
roles. Existing `<menu>` and `<popupmenu>` remain unchanged.
The new `<switch>` tag uses `GtkSwitch` for an on/off control with Boolean
input/output. `activate` toggles it and the logical `state-changed` action
runs synchronously when its `active` property changes. The exported value
tracks the visual requested state, not a separately delayed backend state.
The new `<searchentry>` tag uses `GtkSearchEntry`, while retaining the
`<entry>` value, file and completion contract. Its ordinary `changed`
actions still run synchronously; native `search-changed` actions run after
GTK3's debounce delay. Search navigation and stop signals are available
through explicitly named actions. GTK2 scripts can continue to use
`<entry>` with their own filtering actions.
The new `<searchbar>` tag uses `GtkSearchBar` around one `<entry>` or
`<searchentry>` child. It exports the bar's open/closed search mode rather
than the child's text; the child keeps its own variable and actions. An
`activate` action toggles the mode, while `search-mode-changed` actions run
synchronously. Window key forwarding is opt-in with `capture-keys="true"`
because consuming keystrokes by default would alter unrelated XML actions.
GTK2 scripts can use an ordinary entry with explicit show/hide controls.
Closing the GTK3 search bar clears its connected entry; scripts that need
to retain a hidden query should store it separately before closing.
The new `<levelbar>` tag uses `GtkLevelBar` for a bounded status indicator.
Its numeric value can be refreshed from a command or file, saved, or cleared
to the minimum. A logical `value-changed` action runs synchronously after
value changes. GTK2's `<progressbar>` can display a fraction but has a
different value and update contract, so it is not an XML alias.

## Known unavoidable differences


The detailed list will be expanded as each backend is ported.  GTK3 theming is
CSS-based, so GTK2 `gtkrc` files cannot reproduce their appearance directly;
scripts should provide a GTK3 CSS provider or install a GTK3 theme.  XEmbed
cannot operate on Wayland; scripts should run under an X11 session (including
XWayland where the participating applications support it) or place the client
outside GTKDialog.

The GTK3 `GtkCellRendererAccel` backend is retained. GTK 3.24.49 can emit an
internal `gdk_window_get_window_type` critical when its editor takes the grab;
the edit and the `accel-edited`/`accel-cleared` XML actions still complete.
The regression tests tolerate only this exact GTK diagnostic. Scripts that
must avoid it can use an editable text column and GTK accelerator names such
as `Primary+o`.

VTE 2.91 removed terminal background tint and the separate dim-foreground
colour setter.  The `background-tint-color` and `dim-foreground-color`
attributes are accepted with a warning and ignored.  Use
`text-background-color` and the terminal palette for the closest portable
replacement.  Child creation deliberately uses VTE's synchronous GTK3 API so
the observable ordering of construction and actions does not change.
The optional `hangupterminal` action sends SIGHUP to the recorded direct
child before a terminal page is removed; VTE's `child-exited` callback clears
that PID, so an already-ended child is left alone. It is unavailable in a
build without VTE and does not supervise descendants.

GTK3 deprecated `GtkHandleBox` without introducing a supported detachable
container.  The `<handlebox>` tag therefore remains as a static single-child
container so existing layouts continue to load, but `handle-position`,
`snap-edge` and `shadow-type` have no effect and `child-attached` and
`child-detached` actions are not emitted.  A script that requires detachable
content should open a separately declared GTKDialog window and use its normal
show/hide actions; this also works on non-X11 GTK3 backends.

The `<alignment>` backend uses GTK3 widget alignment, expansion and margins.
The historical scale endpoints retain their common meanings: zero keeps the
child at its requested size and one lets it fill available space.  GTK3 has no
equivalent for fractional `xscale` or `yscale`, so any positive fractional
value uses fill; use nested boxes and explicit size requests when a precise
fraction is required.

The `<colorselection>` tag is backed by `GtkColorChooserWidget` because
`GtkColorSelection` was removed.  Its historical `has-opacity-control`
attribute maps to the GTK3 `use-alpha` property and remains false by default.
GTK3 owns the chooser's palette presentation, so `has-palette` is accepted for
script compatibility but cannot hide or reveal the built-in palette.  Scripts
that need a fixed application palette should use GTK3 theme CSS or compact
`<colorbutton>` controls initialized to the desired colours.

GTK3 CSS replaces the `gtkrc` parser.  The `--styles` option now loads CSS;
old `.gtkrc-2.0` files and `GTK2_RC_FILES` have no effect.  Convert the rules
to GTK3 selectors and properties, then pass the CSS file through `--styles`.
The maintained executable examples do this explicitly; historical filenames
such as `funcgtkrcCreate` and `gtk2_combobox` remain only to avoid breaking
paths used by existing scripts and no longer select a GTK2 backend.

GTK3's popup menu and stock-item APIs are deprecated but still present in the
required 3.24 series.  They remain in the backend where replacing them would
change pointer placement, activation ordering or existing stock-label
semantics.  This is GTK3 compatibility code, not a GTK2 build path.
