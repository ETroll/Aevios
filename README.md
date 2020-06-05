# ÆviOS - 3rd iteration hobby project

A dev container for development using VS Code is provided. The Dockerfile that are used is located at `containers\devcontainer`.

**NOTE:** The host needs a X11 server running at :0. If this is not the case, then change the qemu settings to use `-nographic` in the `Makefile`.

## Tips

 - For `-nographic` just enter: `Ctrl-A X` to exit
 - For qemu console enter `Ctrl-A C`, then `quit` to quit.
