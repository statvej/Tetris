#!/usr/bin/env python3
"""End-to-end smoke test: drives the real binary through a pseudo-terminal."""
import fcntl
import os
import pty
import select
import shutil
import struct
import sys
import tempfile
import termios
import time

BINARY = os.path.abspath(sys.argv[1] if len(sys.argv) > 1 else "./tetris")
RIGHT, DOWN = b"\x1bOC", b"\x1bOB"  # keypad mode (smkx) arrow sequences


class Session:
    def __init__(self, data_dir, rows=30, cols=100):
        self.out = b""
        self.pid, self.fd = pty.fork()
        if self.pid == 0:
            os.environ.update(TERM="xterm", TETRIS_DATA_DIR=data_dir)
            os.execv(BINARY, [BINARY])
        fcntl.ioctl(self.fd, termios.TIOCSWINSZ, struct.pack("HHHH", rows, cols, 0, 0))
        self.drain(0.5)

    def drain(self, seconds=0.2):
        end = time.time() + seconds
        while time.time() < end:
            ready, _, _ = select.select([self.fd], [], [], 0.05)
            if ready:
                try:
                    self.out += os.read(self.fd, 65536)
                except OSError:
                    return

    def send(self, data, wait=0.15):
        os.write(self.fd, data)
        self.drain(wait)

    def wait_for(self, text, timeout=5.0):
        end = time.time() + timeout
        while text.encode() not in self.out and time.time() < end:
            self.drain(0.1)
        return text.encode() in self.out

    def exit_code(self, timeout=3.0):
        end = time.time() + timeout
        while time.time() < end:
            done, status = os.waitpid(self.pid, os.WNOHANG)
            if done:
                return os.waitstatus_to_exitcode(status)
            self.drain(0.1)
        os.kill(self.pid, 9)
        os.waitpid(self.pid, 0)
        return None


def read(path):
    with open(path) as f:
        return f.read()


def test_full_game_and_scores(data_dir):
    s = Session(data_dir)
    s.send(b"\n")  # Start
    for key in (b"c", b"z", b"x", b"a", b"d", b"s", b"p", b"x"):  # hold, rotate, move, pause/resume
        s.send(key)
    for _ in range(60):
        s.send(b" ", 0.05)  # hard-drop until the stack reaches the top
        if b"GAME OVER" in s.out:
            break
    assert s.wait_for("GAME OVER"), "game never ended"
    time.sleep(1.2)  # the game swallows keys for a second after the banner
    s.send(b"x")
    assert s.wait_for("NEW HIGH SCORE"), "no high score prompt"
    s.send(b"ab")
    s.send(b"\n")
    s.send(b"x")  # leave the high score table
    s.send(b"q")
    assert s.exit_code() == 0
    assert "AB" in read(os.path.join(data_dir, ".tetris_scores")), "score not saved"


def test_menu_settings_persist(data_dir):
    s = Session(data_dir)
    s.send(DOWN)  # Mode
    s.send(RIGHT)  # Marathon -> Sprint
    s.send(DOWN)  # Level
    s.send(RIGHT)
    s.send(RIGHT)  # 1 -> 3
    s.send(DOWN)  # Ghost
    s.send(RIGHT)  # off
    s.send(DOWN)  # Theme
    s.send(RIGHT)  # Mono
    s.send(DOWN)
    s.send(DOWN)  # Credits
    s.send(b"\n")
    assert s.wait_for("fstaryk"), "credits not shown"
    s.send(b"x")
    s.send(b"q")
    assert s.exit_code() == 0
    assert read(os.path.join(data_dir, ".tetris_settings")).strip() == "mode=1 level=3 ghost=0 theme=1"


def test_terminal_too_small(data_dir):
    s = Session(data_dir, rows=10, cols=30)
    assert s.wait_for("Terminal too small")
    assert s.exit_code() == 1


def main():
    for test in (test_full_game_and_scores, test_menu_settings_persist, test_terminal_too_small):
        data_dir = tempfile.mkdtemp(prefix="tetris-smoke-")
        try:
            test(data_dir)
        finally:
            shutil.rmtree(data_dir, ignore_errors=True)
        print(f"{test.__name__}: OK")


if __name__ == "__main__":
    main()
