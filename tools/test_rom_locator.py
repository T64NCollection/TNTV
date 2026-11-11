#!/usr/bin/env python3
"""Offline ROM filename locator regression.

Compiles only the isolated name-lookup helper for Linux; uses tiny fake
files (not game ROMs). Content/revision validation remains in tntv_rom_reader.
Never reads or modifies an actual ROM, Vita device, user cache or save.
"""
from pathlib import Path
import ctypes
import os
import subprocess
import tempfile

ROOT=Path(__file__).resolve().parent.parent

def main():
    with tempfile.TemporaryDirectory(prefix="tntv-locator-") as temp:
        root=Path(temp)
        library=root/"locator.so"
        subprocess.run(
            ["cc","-std=c99","-Wall","-Wextra","-Werror","-fPIC","-shared",
             "-DTNTV_ROM_LOCATOR_HOST_TEST","-I",str(ROOT/"romloader"),
             str(ROOT/"romloader/tntv_rom_locator.c"),"-o",str(library)],
            check=True)
        locator=ctypes.CDLL(str(library)).tntv_rom_find_in_directory
        locator.argtypes=(ctypes.c_char_p,ctypes.c_char_p,ctypes.c_size_t)
        locator.restype=ctypes.c_int
        folder=root/"ROM dir with spaces"
        folder.mkdir()
        def find(capacity=512):
            out=ctypes.create_string_buffer(capacity)
            ok=locator(os.fsencode(folder),out,capacity)
            return (ok,Path(os.fsdecode(out.value)).name if ok else "")
        def create(name):
            (folder/name).write_bytes(b"tiny non-ROM unit fixture")
        def clear():
            for child in folder.iterdir():
                if child.is_dir():child.rmdir()
                else:child.unlink()
        assert find()==(0,""),"empty folder must fail"
        assert find(1)==(0,""),"invalid output capacity must fail"
        assert find(5)==(0,""),"no truncated accepted filenames"
        cases=[
            ("New Tetris, The (USA).z64","New Tetris, The (USA).z64"),
            ("The New Tetris (USA).z64","The New Tetris (USA).z64"),
            ("The New Tetris (USA) [!].z64","The New Tetris (USA) [!].z64"),
            ("whatever-retail-dump.Z64","whatever-retail-dump.Z64"),
            ("other dump.v64","other dump.v64"),
            ("other dump.N64","other dump.N64"),
            ("baserom.us.z64","baserom.us.z64"),
            ("baserom.z64","baserom.z64"),
            ("baserom.v64","baserom.v64"),
            ("baserom.n64","baserom.n64"),
        ]
        for name, expected in cases:
            create(name)
            assert find()==(1,expected),(name,find())
            clear()
        for unsupported in ("game.zip","game.z65","game.z64.old","game","notarom.txt"):
            create(unsupported)
        assert find()==(0,""),"unsupported extensions ignored"
        clear()
        (folder/"folder.z64").mkdir()
        assert find()==(0,""),"directories are never considered ROMs"
        clear()
        create("Z dump.z64")
        create("A dump.z64")
        assert find()==(1,"A dump.z64"),"fallback lexical order"
        create("The New Tetris (USA).z64")
        assert find()==(1,"The New Tetris (USA).z64"),"common name before fallback"
        create("baserom.n64")
        assert find()==(1,"baserom.n64"),"legacy baserom before title alias"
        create("baserom.us.z64")
        assert find()==(1,"baserom.us.z64"),"legacy priority preserved"
        print("PASS: 10 individual filename formats, empty/invalid/short path,",
              "ignored extensions, no directory candidates, fallback ordering,",
              "title aliases and legacy precedence. No user ROM involved.")
if __name__=="__main__":
    main()
