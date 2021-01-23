# Minimal run

This is a minimal tiny C program for running Windows commands with a hidden
console.

I use `run.exe wsl --exec xterm -display :0` to launch an XTerm on Windows.

# Screenshot

You can't see it running because it is designed to be invisible, but your
shortcut should look like this.

![Screenshot](xterm-example.png)

# Troubleshooting

First try this to make sure run is working:

```
run.exe notepad
```

If that does work, but starting a wsl application doesn't work, it may be a
PATH issue.

Try using the full instead, like this:

```
run.exe wsl --exec /usr/bin/program
```

If you need to set DISPLAY or some other variable, use env. For example, like
this:

```
run.exe wsl --exec env DISPLAY=:0 /usr/bin/program
```

If you have multiple distributions installed, remember to specify the one you
want, e.g.

```
run.exe wsl --distribution Debian --exec /usr/bin/foobar
```

