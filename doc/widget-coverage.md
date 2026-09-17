# GTK3 widget coverage

This document maps the current GTK3-only implementation of Gtkdialog's public
XML language. It was checked against widget constructors in `src/`, the HTML
reference in `doc/reference/`, and writable GTK3 properties. The HTML
reference remains the user-facing authority for exact syntax. The historical
GTK2 audit near the end is retained as history, not as a description of the
current backend.

The user-facing [common widget contract](reference/syntax.html#common-widget-contract)
describes the state actions, generic signals and conditions installed for
every normal GtkWidget.  Entries below describe only the type-specific value
operations and lifecycle boundary; value functions are never implied merely
by appearing in this matrix.

Gtkdialog does not try to reproduce the GTK+ C API in XML.  A normal widget
receives writable scalar GTK3 properties through XML attributes, while
constructor arguments, child packing, model data and Gtkdialog behaviour need
dedicated syntax.  Object, pointer and internal child properties deliberately
remain C-level interfaces unless an XML ownership model is defined for them.

## Coverage rules

| Property kind | XML coverage | Notes |
| --- | --- | --- |
| Boolean | Complete | `true`, `false`, `yes`, `no`, `1` and `0` |
| Signed and unsigned integer | Complete | Decimal, octal and hexadecimal input within the property's range |
| Float and double | Complete | Finite values within the property's range |
| String | Complete | Passed as UTF-8 text |
| Enum | Complete | Registered GTK+ names and nicks, case-insensitively, or numeric values |
| Flags | Complete | Registered names and nicks joined with `|`, case-insensitively, or a numeric mask |
| `GdkRGBA` boxed value | Complete | GTK3 colour specifications such as `#123456` or `red` |
| Other boxed value | Not generic | A safe textual grammar is needed for each boxed type |
| Object, pointer and parameter | Not generic | These require explicit identity, lifetime and ownership rules |
| Construct-only property | Dedicated only | It must be consumed before the GTK+ constructor; arbitrary construct-only properties remain intentionally unsupported |
| `visible` | Managed by Gtkdialog | Visibility follows Gtkdialog construction and action ordering |

Unknown attributes are retained as object data for Gtkdialog's dedicated
handlers.  A read-only property produces a warning; an invalid generic value
is not applied.  See [the syntax reference](reference/syntax.html) for the
current user contract.

## Public XML tag matrix

“Generic” below means the writable scalar properties of the native GTK3 class
and its ancestors.  “Value” covers `<variable>`, `<default>`, `<input>` and
`<output>` where those concepts apply.  “Actions” includes the tag's default
signal and explicitly documented signal names.  Container-only and decorative
tags intentionally have no value contract.

| XML tag | GTK3 backend | Dedicated XML surface beyond generic properties | Value, actions and lifecycle boundary |
| --- | --- | --- | --- |
| [`<aboutdialog>`](reference/aboutdialog.html) | `GtkAboutDialog` | Program metadata, logo and credited-person lists | Response actions; close and relaunch supported |
| [`<accellabel>`](reference/accellabel.html) | `GtkAccelLabel` | Text plus deferred binding to a named widget accelerator, independent of XML order | Label-text value and input/output; accelerator closure remains owned by the target widget |
| [`<actionbar>`](reference/actionbar.html) | `GtkActionBar` | Start, end and one center child through `actionbar-pack` | Container without an independent value; child variables and actions remain available; new GTK3-only tag |
| [`<appchooser>`](reference/appchooser.html) | `GtkAppChooserWidget` | Embedded application list for one content type | Desktop ID value, save/refresh, synchronous selection and activation actions; no programmatic selection by ID; new GTK3-only tag |
| [`<appchooserbutton>`](reference/appchooserbutton.html) | `GtkAppChooserButton` | Installed applications for one content type | Desktop ID value, save/refresh and synchronous `changed` actions; no programmatic selection by ID; new GTK3-only tag |
| [`<appchooserdialog>`](reference/appchooserdialog.html) | `GtkAppChooserDialog` | Top-level application chooser for one content type | Desktop ID value, response actions, selection and response file output, refresh and close/relaunch; no programmatic selection by ID; new GTK3-only tag |
| [`<headerbar>`](reference/headerbar.html) | `GtkHeaderBar` | Opt-in direct `<window>` titlebar with start/end/one custom center child | No independent value; window body and child actions retained; client-side decoration replaces the native WM titlebar only for that window |
| [`<alignment>`](reference/alignment.html) | `GtkBox` with widget alignment and margins | Alignment and padding; fractional scale is unavailable | Single-child container; no value |
| [`<arrow>`](reference/arrow.html) | `GtkArrow` | Direction, relief, alignment and padding | Decorative empty value; common state and explicit signal actions |
| [`<aspectframe>`](reference/aspectframe.html) | `GtkAspectFrame` | Label, alignment, ratio and obey-child constructor data | Label value; input/output and relaunch supported |
| [`<assistant>`](reference/assistant.html) | `GtkAssistant` | Page type, title, completion and side/header images | Current-page value and navigation actions; close and relaunch supported |
| [`<button>`](reference/button.html) | `GtkButton` | Label, stock/icon composition, mnemonic and response role | Click actions; no exported value |
| [`<calendar>`](reference/calendar.html) | `GtkCalendar` | ISO date parsing and selection | Date value, input/output and day-selection actions |
| [`<cellview>`](reference/cellview.html) | `GtkCellView` with a one-row `GtkListStore` | Text plus a stock, themed or file image, optional markup and fixed display-row ownership | Text value, input/output and refresh/save/clear actions; no type-specific signal |
| [`<checkbox>`](reference/checkbox.html) | `GtkCheckButton` | Label and initial Boolean state | Boolean value, input/output, activate and toggle actions |
| [`<chooser>`](reference/chooser.html) | `GtkFileChooserWidget` | Embedded open-file view, initial folder/file and filters | Path value, input/output, selection actions and legacy `when=`; relaunch supported |
| [`<colorbutton>`](reference/colorbutton.html) | `GtkColorButton` | Colour value and alpha handling | Colour value, input/output and colour-set actions |
| [`<colorchooserdialog>`](reference/colorchooserdialog.html) | `GtkColorChooserDialog` | Top-level colour chooser with optional alpha | Colour value and separate response output, input/default, synchronous response actions and close/relaunch; new GTK3-only tag |
| [`<colorselection>`](reference/colorselection.html) | `GtkColorChooserWidget` | Embedded selector, shared palette and optional opacity controls | Colour/alpha value, input/output and colour-changed actions; GTK2 palette differences are documented in the porting guide |
| [`<combobox>`](reference/combobox.html) | editable `GtkComboBoxText` | Item population and optional case handling | Text value, input/output and change actions; compatibility XML name |
| [`<comboboxtext>`](reference/comboboxtext.html) | `GtkComboBoxText`, rich `GtkListStore` or hierarchical `GtkTreeStore` | Established text items, per-item values and styling, or JSON-backed GTK3 submenus | Visible label or stable leaf value, flat and hierarchical input/output, change actions and relaunch; old text model remains the default |
| [`<comboboxentry>`](reference/comboboxentry.html) | entry-enabled `GtkComboBoxText` or opt-in rich `GtkListStore` backend | Editable entry with established text items, or a styled icon list with stable item values and optional shared-model completion with configurable matching | Selected stable value or arbitrary entry text, input/output and change/activate actions; old text model remains the default |
| [`<dialog>`](reference/dialog.html) | `GtkDialog` | Content/action areas and named response buttons | Response value and actions; close and relaunch supported |
| [`<drawingarea>`](reference/drawingarea.html) | `GtkDrawingArea` with owned raster pixbufs | Original, aspect-fit or stretched image rendering with selectable interpolation and background | Image-filename value, input/output, refresh/save/clear, expose and pointer actions |
| [`<imageview>`](reference/imageview.html) | GTK3 `GtkDrawingArea` in a `GtkScrolledWindow` | Bounded zoom, whole/width/height fitting, Ctrl-wheel scaling and drag panning without an external widget library | Image-filename value, input/output, refresh/save/clear, scaling functions and zoom/expose/pointer actions |
| [`<edit>`](reference/edit.html) | `GtkTextView` in a scrolled window, optionally `GtkSourceView` 4 | Text-buffer input, output, save/refresh behaviour, optional bottom gravity and source-editor controls | Text value, widget signals and source-buffer undo/redo actions; scroll and history state follow the text view's lifetime |
| [`<entry>`](reference/entry.html) | `GtkEntry` | Text input plus plain or opt-in rich flat completion with icons, descriptions and configurable matching | Typed text or stable selected value, input/output and changed/activate actions |
| [`<eventbox>`](reference/eventbox.html) | `GtkEventBox` | Event-action bridge | Event actions; no value |
| [`<expander>`](reference/expander.html) | `GtkExpander` | Label and initial expansion | Boolean expanded value and activate actions |
| [`<filechooserbutton>`](reference/filechooserbutton.html) | `GtkFileChooserButton` | File/folder constructor mode and filters | Path value, input/output and selection actions; relaunch supported |
| [`<filechooserdialog>`](reference/filechooserdialog.html) | GTK3 `GtkFileChooserDialog` | Open, save, select-folder and create-folder modes, multiple local paths or URIs, filters and shortcuts | Response value and actions; selection, response or current-folder output; input, refresh, clear and close/relaunch lifecycle |
| [`<fixed>`](reference/fixed.html) | `GtkFixed` | Per-child integer `x` and `y` coordinates | Absolute-position layout container; no value or dynamic movement contract |
| [`<fontbutton>`](reference/fontbutton.html) | `GtkFontButton` | Font-name value | Font value, input/output and font-set actions |
| [`<fontchooserdialog>`](reference/fontchooserdialog.html) | `GtkFontChooserDialog` | Top-level font chooser | Font value and separate response output, input/default, synchronous response actions and close/relaunch; new GTK3-only tag |
| [`<fontselection>`](reference/fontselection.html) | `GtkFontChooserWidget` | Embedded family, face, size and preview controls | Font value, input/output and logical font-changed actions |
| [`<frame>`](reference/frame.html) | `GtkFrame` | Label plus internal content box | Label value, command/file input, output, refresh/save/clear and state actions; single logical child area |
| [`<grid>`](reference/grid.html) | `GtkGrid` | Rows, columns, automatic/explicit attachment, spans and packing | Layout container; preserves `<grid>` XML semantics independently of data `<table>` |
| [`<handlebox>`](reference/handlebox.html) | static single-child `GtkBox` | One child; GTK2 handle and snap properties are accepted without detachment | Layout preserved, but attach/detach signals cannot fire in GTK3 |
| [`<gvim>`](reference/gvim.html) | `GtkSocket` in an aligned popup | X11 socket ID passed to an external GVim process | Legacy X11 embedding helper; common widget state applies to its placeholder, but it has no value operations |
| [`<socket>`](reference/socket.html) | direct `GtkSocket` | Numeric X11 window ID for an arbitrary XEmbed client | Default `plug-added` and explicit `plug-removed` actions; the socket survives client removal, while client process ownership remains external |
| [`<plug>`](reference/plug.html) | top-level `GtkPlug` | Decimal host socket ID supplied at construction | Embeds a complete GTKDialog interface in an XEmbed socket; default `embedded` actions and normal window lifecycle apply |
| [`<hbox>`](reference/hbox.html), [`<vbox>`](reference/vbox.html) | oriented `GtkBox` | Homogeneity, spacing and child packing | Layout containers; source child order is preserved; `replacebox` can replace their children from an XML template |
| [`<hbuttonbox>`](reference/hbuttonbox.html), [`<vbuttonbox>`](reference/vbuttonbox.html) | `GtkHButtonBox`, `GtkVButtonBox` | Named layout and secondary children | Button containers; no value |
| [`<hpaned>`](reference/hpaned.html), [`<vpaned>`](reference/vpaned.html) | oriented `GtkPaned` | Two-child resize/shrink packing | Divider position value and actions; launch lifecycle supported |
| [`<hscale>`](reference/hscale.html), [`<vscale>`](reference/vscale.html) | oriented `GtkScale` | Range constructor and initial value | Numeric value, input/output and value-changed actions |
| [`<hscrollbar>`](reference/hscrollbar.html), [`<vscrollbar>`](reference/vscrollbar.html) | oriented `GtkScrollbar` | Adjustment bounds, increments, page extent and initial value | Locale-independent numeric value, input/output, refresh/save and GtkRange actions |
| [`<hseparator>`](reference/hseparator.html), [`<vseparator>`](reference/vseparator.html) | oriented `GtkSeparator` | None | Decorative widgets; no value |
| [`<hsv>`](reference/hsv.html) | `GtkHSV` | H/S/V value and native selector metrics | Value, input/output, changed actions, file monitoring and relaunch supported |
| [`<hruler>`](reference/hruler.html), [`<vruler>`](reference/vruler.html) | custom Cairo/Pango `GtkDrawingArea` | Measured range, position and metric | Numeric input/output, clear-to-lower and common state actions |
| [`<iconview>`](reference/iconview.html) | `GtkIconView` with flat `GtkListStore` | Text/image rows or opt-in rich items with icons, markup, descriptions, tooltips and styling | Selected label or stable rich value, input/output and item actions; relaunch supported |
| [`<infobar>`](reference/infobar.html) | `GtkInfoBar` | Content/action areas, message type and response buttons | Response value and actions; close and relaunch supported |
| [`<layout>`](reference/layout.html) | `GtkLayout` in a `GtkScrolledWindow` | Virtual width/height and per-child integer `x`/`y` coordinates | Scrollable absolute-position container; no value or dynamic movement contract |
| [`<levelbar>`](reference/levelbar.html) | `GtkLevelBar` | Bounded continuous or discrete numeric indicator | Numeric value, input/output, refresh/save/clear and synchronous `value-changed` actions; new GTK3-only tag |
| [`<linkbutton>`](reference/linkbutton.html) | `GtkLinkButton` | URI and optional label | URI value, input/output and click actions; relaunch supported |
| [`<list>`](reference/list.html) | `GtkListBox` | Item model and selection | Selected text value, input/output and selection actions; compatibility XML name |
| [`<menu>`](reference/menu.html) | `GtkMenu` | Nested ownership and accelerator group | Menu container; window owns accelerator lifetime |
| [`<menubar>`](reference/menubar.html) | `GtkMenuBar` | Nested menu hierarchy | Menu-shell container; no value |
| [`<menubutton>`](reference/menubutton.html) | `GtkMenuButton` | One direct `<popover>` child and label | Boolean open state, default/save/clear and synchronous `toggled` actions |
| [`<menuitem>`](reference/menuitem.html) | `GtkMenuItem`, `GtkImageMenuItem`, `GtkCheckMenuItem` or `GtkRadioMenuItem` | Label/image/stock, accelerators, check/radio groups and submenu | Activation actions; Boolean state for check/radio variants |
| [`<menuitemseparator>`](reference/menuitemseparator.html), [`<separator>`](reference/separator.html) | `GtkSeparatorMenuItem` | `<separator>` is the retained historical alias | Decorative menu item; no value |
| [`<messagedialog>`](reference/messagedialog.html) | `GtkMessageDialog` | Message type, named button set and secondary text | Response value and actions; close and relaunch supported |
| [`<modelbutton>`](reference/modelbutton.html) | `GtkModelButton` | Text item with optional named submenu navigation and back-arrow placement | No independent value; synchronous clicked action; GAction-backed check/radio roles are not exposed |
| [`<notebook>`](reference/notebook.html) | `GtkNotebook` | Declarative labels or named pages built from inline/file XML templates; optional close controls | Current-page value and switch-page actions; live pages can be added, renamed, reordered and removed, and dynamic close actions run before removal |
| [`<offscreenwindow>`](reference/offscreenwindow.html) | GTK3 `GtkOffscreenWindow` | One direct child, minimum rendered size and image output | Invisible program root; refresh/save, damage actions and close/relaunch lifecycle are preserved |
| [`<overlay>`](reference/overlay.html) | `GtkOverlay` | First child as the main surface, later children layered above it with alignment and pass-through options | No independent value; child actions and common container visibility remain available; new GTK3-only tag |
| [`<pagesetupdialog>`](reference/pagesetupdialog.html) | GTK3 `GtkPageSetupUnixDialog` | Paper size, orientation, margins and optional key-file state | Response value and actions; page-setup and print-settings output; refresh, clear and close/relaunch lifecycle |
| [`<pixmap>`](reference/pixmap.html) | `GtkImage` | File, stock icon and scaled pixbuf loading | Image source input and refresh actions; pixbuf ownership is internal |
| [`<popover>`](reference/popover.html) | `GtkPopover` | One direct content child and edge position | No independent value; `closed` action and independent child controls; anchored by `<menubutton>` |
| [`<popovermenu>`](reference/popovermenu.html) | `GtkPopoverMenu` | One main page plus unique named submenu pages, each retaining ordinary child controls | No independent value; GTK native close action and named-page navigation via `<modelbutton>` |
| [`<popupmenu>`](reference/popupmenu.html) | `GtkMenu` | Standalone program root or lazily created widget context menu | Activation actions; context-menu registry and accelerator lifetime follow the originating widget, while standalone dismissal exits after pending synchronous item actions |
| [`<printdialog>`](reference/printdialog.html) | GTK3 `GtkPrintUnixDialog` | Printer, range, copy, selection, capability and optional key-file state | Response value and actions; print-settings, page-setup and printer-name output; refresh, clear and close/relaunch lifecycle |
| [`<progressbar>`](reference/progressbar.html) | `GtkProgressBar` | Fraction/text/pulse interpretation | Numeric or activity value and input actions |
| [`<radiobutton>`](reference/radiobutton.html) | `GtkRadioButton` | XML radio grouping and label; historical `<radio>` alias | Boolean value, input/output, selection by activate and toggle actions |
| [`<recentchooser>`](reference/recentchooser.html) | `GtkRecentChooserWidget` | URI selection and recent filters | URI value, input/output and selection actions; relaunch supported |
| [`<recentchooserdialog>`](reference/recentchooserdialog.html) | `GtkRecentChooserDialog` | Top-level recent-resource chooser and filters | URI selection, separate response output, synchronous response actions and close/relaunch; new GTK3-only tag |
| [`<recentchoosermenu>`](reference/recentchoosermenu.html) | `GtkRecentChooserMenu` owned by a `GtkMenuItem` | Recent-resource submenu, label, numbering and filters | Current URI value, input/output and item actions; submenu lifetime follows its menu item |
| [`<revealer>`](reference/revealer.html) | `GtkRevealer` | One child, initial reveal state and animated transition | Boolean value, input/output, synchronous `reveal-changed` actions, active conditions and `activate` toggle; new GTK3-only tag |
| [`<scalebutton>`](reference/scalebutton.html), [`<volumebutton>`](reference/volumebutton.html) | `GtkScaleButton`, `GtkVolumeButton` | Range, icon size and icon-name list | Numeric value, input/output and value-changed actions; relaunch supported |
| [`<scrolledwindow>`](reference/scrolledwindow.html) | `GtkScrolledWindow`, adding a `GtkViewport` when required | One direct child, scrollbar policies, placement and shadow | General scroll container; no value, while the child retains its own contract |
| [`<shortcutsgroup>`](reference/shortcutsgroup.html) | `GtkShortcutsGroup` | One or more shortcut entries; title and optional view filter | Exports its title; group contents are informational; new GTK3-only tag |
| [`<shortcutssection>`](reference/shortcutssection.html) | `GtkShortcutsSection` | One or more shortcut groups; title and unique section name | Exports its title; selected section belongs to the root; new GTK3-only tag |
| [`<shortcutsshortcut>`](reference/shortcutsshortcut.html) | `GtkShortcutsShortcut` | One shortcut label, accelerator and optional subtitle/type | Exports its title; no activation action because it documents a shortcut rather than implementing it; new GTK3-only tag |
| [`<shortcutswindow>`](reference/shortcutswindow.html) | `GtkShortcutsWindow` | Top-level native searchable shortcuts reference | Selected section value, input/output and `close`/`search`/`section-changed` actions; close/relaunch; new GTK3-only tag |
| [`<viewport>`](reference/viewport.html) | `GtkViewport` | One direct child, explicit scrolling bridge and shadow | Empty container value; surrounding `GtkScrolledWindow` supplies adjustments |
| [`<sizegroup>`](reference/sizegroup.html) | `GtkSizeGroup` | Mode and named widget membership | Declarative helper, not a displayed widget; window owns group lifetime |
| [`<spinbutton>`](reference/spinbutton.html) | `GtkSpinButton` | Range constructor, increments and initial value | Numeric value, input/output and value-changed actions |
| [`<stack>`](reference/stack.html) | `GtkStack` | Named direct-child pages with optional titles and animated transitions | Selected page name, input/output, refresh/save/clear and synchronous `page-changed` actions; new GTK3-only tag |
| [`<stackswitcher>`](reference/stackswitcher.html) | `GtkStackSwitcher` | Button navigation bound to a named `<stack>` regardless of XML declaration order | No independent value; selecting a page changes the stack and runs its `page-changed` actions |
| [`<stacksidebar>`](reference/stacksidebar.html) | `GtkStackSidebar` | Vertical navigation bound to a named `<stack>` regardless of XML declaration order | No independent value; selecting a page changes the stack and runs its synchronous `page-changed` actions |
| [`<listbox>`](reference/listbox.html) | `GtkListBox` and `GtkListBoxRow` | Arbitrary direct-child widgets become named, selectable rows | Selected row name, default/input/refresh/save/clear and synchronous `selection-changed` and `row-activated` actions; single/none selection only |
| [`<flowbox>`](reference/flowbox.html) | `GtkFlowBox` and `GtkFlowBoxChild` | Arbitrary direct-child widgets become named wrapping tiles with spacing, orientation and maximum-per-line controls | Selected tile name, default/input/refresh/save/clear and synchronous `selection-changed` and `child-activated` actions; single/none selection only |
| [`<placessidebar>`](reference/placessidebar.html) | `GtkPlacesSidebar` | Native places, bookmarks and volumes with local-only/recent/desktop/trash options | URI value, default/input/refresh/save/clear and synchronous `open-location` actions; the script displays the location |
| [`<switch>`](reference/switch.html) | `GtkSwitch` | GTK3 on/off control with an initial active state | Boolean value, input/output, refresh/save/clear, active conditions and synchronous `state-changed` actions; new GTK3-only tag |
| [`<searchbar>`](reference/searchbar.html) | `GtkSearchBar` | One direct `<entry>` or `<searchentry>` child and optional window key capture | Boolean search mode, input/output, refresh/save/clear, activate toggle and synchronous `search-mode-changed`; child retains independent text/actions; new GTK3-only tag |
| [`<searchentry>`](reference/searchentry.html) | `GtkSearchEntry` | Search and clear icons with optional entry completion | Text value, input/output, refresh/save/clear and ordinary synchronous `changed`; delayed `search-changed` plus next/previous/stop search signals; new GTK3-only tag |
| [`<spinner>`](reference/spinner.html) | `GtkSpinner` | Initial active state | Boolean value, active conditions, input/output and relaunch supported |
| [`<statusbar>`](reference/statusbar.html) | `GtkStatusbar` | Context stack and displayed text | Text value, input/output and update actions |
| [`<statusicon>`](reference/statusicon.html) | GTK3 `GtkStatusIcon` owned by a zero-size proxy | Theme, stock and file images, tooltip, title and positioned context menu | Image-source value, input/output and native icon actions; icon lifetime follows its containing window |
| [`<table>`](reference/table.html) | `GtkTreeView` with `GtkListStore` | Column model, headings, rows, selection and opt-in live entry filtering | Tabular selection value, input/output and row actions; compatibility XML name |
| [`<terminal>`](reference/terminal.html) | Optional VTE terminal widget | Child command, shell environment and terminal I/O | PID value and child-exited actions; `hangupterminal` requests SIGHUP for the direct child, which may otherwise outlive the widget |
| [`<text>`](reference/text.html) | `GtkLabel`, optionally in a GTK3 scrolled window | Label input, markup, file/command refresh and opt-in bottom gravity | Text value and input/output; legacy unscrolled layout remains the default |
| [`<timer>`](reference/timer.html) | hidden `GtkLabel` plus GLib timeout | Interval and enabled state | Periodic actions; source lifetime follows the owning window |
| [`<togglebutton>`](reference/togglebutton.html) | `GtkToggleButton` | Button label/icon composition and initial state | Boolean value, input/output, activate and toggle actions |
| [`<toolbar>`](reference/toolbar.html) | `GtkToolbar` | Named style/orientation and tool-item grouping | Tool container; groups and menus follow toolbar/window lifetime |
| [`<toolbutton>`](reference/toolbutton.html) | `GtkToolButton` | Label, stock/icon, tooltip and logical forwarding of internal-button input events | Click and generic event actions; no value |
| [`<toolitem>`](reference/toolitem.html) | `GtkToolItem` | Exactly one ordinary child plus native tool-item layout properties | Child keeps its own value/actions; named wrapper supports shared state actions and exports an empty value |
| [`<toolitemgroup>`](reference/toolitemgroup.html), [`<toolpalette>`](reference/toolpalette.html) | `GtkToolItemGroup`, `GtkToolPalette` | Group labels, collapse state, group and item packing, existing tool-item children | Palette has no value; group label is its value with input/output and state actions; available in GTK3 |
| [`<toggletoolbutton>`](reference/toggletoolbutton.html) | `GtkToggleToolButton` | Label/icon, initial state and logical forwarding of internal-button input events | Boolean value, input/output, toggle and generic event actions |
| [`<radiotoolbutton>`](reference/radiotoolbutton.html) | `GtkRadioToolButton` | Toolbar-local radio grouping and logical forwarding of internal-button input events | Boolean value, input/output, toggle and generic event actions |
| [`<menutoolbutton>`](reference/menutoolbutton.html) | `GtkMenuToolButton` | Directly owned nested menu and logical forwarding from its main/arrow buttons | Click/show-menu and generic event actions; menu lifetime follows the item |
| [`<separatortoolitem>`](reference/separatortoolitem.html) | `GtkSeparatorToolItem` | Draw/expand properties through GTK3 | Decorative tool item; no value |
| [`<tree>`](reference/tree.html) | `GtkTreeView` with `GtkTreeStore`; optional filter and sort view models | Typed columns, optional JSON-GLib hierarchy, stable row IDs and per-row/per-cell sensitivity, icons, selection, whole-tree expand/collapse, editable renderer families, configurable text completion from items, commands or files, and live entry filtering | Selected-row value, legacy flat or hierarchical JSON input/output, optional JSON state restoration across refreshes, and renderer actions; insensitive cells reject edit actions, and completion sources do not create rows |
| [`<window>`](reference/window.html) | `GtkWindow` | Construct-only top-level/popup type, title/icon/geometry, variable registry and top-level action ownership | Owns widget tree, timers, monitors, child processes and launch/close lifecycle |

## Puppy 0.8.5 GTK2 compatibility audit (historical)

This table records the earlier review of GTK2-visible changes between the
Puppy `0.8.4i` and `0.8.5` tags. Its middle column describes that historical
GTK2 review, not the current GTK3 backend. Current GTK3 decisions are in
[the porting notes](gtk3-porting.md).

| 0.8.5 area | Historical GTK2 status | Compatibility decision |
| --- | --- | --- |
| `variable_is_true()` and `variable_is_false()` | Implemented and tested | Uses complete widget values and strict documented Boolean parsing without the upstream fixed-size buffer |
| Embedded `<chooser>` | Implemented, documented and tested | Retains the historical folder-valued `<default>` while adding file input/output, filters, actions and the `when=` alias |
| `scroll-to-bottom` | Implemented, documented and tested | Available on `<edit>` and opt-in scrollable `<text>` with state owned by the scrolled window |
| XML comments and shell `<` redirection | Implemented and tested | Retains multiline accounting and the historical action/input shell surface |
| Early menu property application | Implemented and tested | Preserves the initial GTK2 menu requisition without changing source action order |
| Window `icon-name` | GTK2 icon property supported | The 0.8.5 call `g_set_prgname(icon-name)` is deliberately not copied: a per-window XML property must not silently mutate the process-wide application identity and existing `WM_CLASS` matching |
| Newer VTE colour API | Not applicable | The added highlight-foreground API belongs to GTK3-era VTE; the GTK2 VTE colour interface remained supported in that branch |
| `layer` and `edge` | Excluded | Layer-shell behaviour was outside the GTK2 fork and remains outside this GTK3 port |

Any future compatibility import must preserve the established XML defaults, synchronous
action order and ownership model and must receive an ordinary regression test.

## Deliberate boundaries

- `GtkAdjustment`, tree models, cell renderers, text buffers, pixbufs, filters,
  accelerator groups and similar objects are built and owned by the
  corresponding XML widget instead of being passed as arbitrary object-valued
  attributes.
- Internal child properties such as a dialog's content area are represented by
  explicit XML structure, not by object pointers.
- Legacy XML names `<combobox>`, `<list>` and `<table>` remain available for existing
  programs.  New code should use `<comboboxtext>` or `<comboboxentry>`, `<tree>`
  and `<grid>` as appropriate; removing the legacy tags would break the public
  language.
- GTK3 properties that are not writable, are construct-only, or cannot be
  represented safely as text require an explicit compatibility-reviewed XML
  feature before they can be exposed.

## Maintainer checklist

When adding or extending a tag:

1. Preserve existing XML defaults and synchronous action order.
2. Consume constructor, child-packing and model attributes before the generic
   property pass, and document who owns every allocated value.
3. Verify the common contract separately: initial `visible` and `sensitive`
   state, `show`, `hide`, `enable` and `disable`, `visible_is_*()` and
   `sensitive_is_*()` conditions, generic widget signals, and `active_is_*()`
   for every type that exposes a Boolean active state. If that type accepts
   the legacy `if true`/`if false` action prefix, verify it alone and with an
   explicit `condition` attribute, whose result historically takes precedence.
4. Define and test the default signal and every meaningful value operation;
   `<input>`, `<output>`, `refresh`, `save` and `clear` are not inherited merely
   by registering a GTK+ widget.
5. Add the tag or option to its HTML reference and an ordinary regression test.
6. Exercise close and relaunch when the implementation owns sources, monitors,
   processes, auxiliary objects or secondary windows.
7. Update this matrix if the native class, public contract or boundary changes.
