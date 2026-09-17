# Distribution package recipes

These recipes build the current GTK3 branch against each distribution's GTK3.
They include JSON-GLib, VTE 2.91 and GtkSourceView 4; there is no private GTK
stack or GTK2 build. Both the package and installed command are named `gdlg3`.
They do not install a `gtkdialog` command alias. The icon and optional Info
manual also have distinct filenames, allowing GTK2 and GTK3 packages to
coexist.

All three recipes consume a `gtkdialog-<version>.tar.gz` archive produced by
`make dist`; an automatically generated Git tag archive is not equivalent
because it lacks generated Autotools files. The checksums refer to the
`gtkdialog-0.9.1.tar.gz` attached to the GitHub `0.9.1` release, not the
automatically generated tag archive.

Use `packaging/arch/PKGBUILD` in a `makepkg` directory,
`packaging/alpine/APKBUILD` in an Alpine `abuild` package directory, or
`packaging/void/template` as
`void-packages/srcpkgs/gdlg3/template`. They configure and build in a
separate build directory within the extracted release archive. The bundled
test suite is omitted from the GitHub snapshot and is maintained separately.

The recipe syntax and package dependencies were prepared for the respective
distributions; building and installing the finished packages on Arch and Void
still requires validation there. Alpine/musl was checked separately from a
temporary build copy.
