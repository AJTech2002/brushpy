#!/usr/bin/env python3
"""Watches a directory for PNG frames written by Canvas.renderOut() and
displays the most recently updated one in a live OpenCV window.

Usage:
    python viewer.py ./artwork
"""

import argparse
import os

import cv2


def find_latest_png(directory: str) -> str | None:
    latest_path = None
    latest_mtime = -1.0
    try:
        entries = os.scandir(directory)
    except FileNotFoundError:
        return None
    with entries:
        for entry in entries:
            if not entry.name.lower().endswith(".png"):
                continue
            try:
                mtime = entry.stat().st_mtime
            except OSError:
                continue
            if mtime > latest_mtime:
                latest_mtime = mtime
                latest_path = entry.path
    return latest_path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("directory", help="Directory to watch for PNG frames")
    parser.add_argument(
        "--poll-interval",
        type=float,
        default=1.0 / 60.0,
        help="Seconds between directory polls (default: ~60fps)",
    )
    args = parser.parse_args()

    window_name = f"BrushPY Viewer - {args.directory}"
    cv2.namedWindow(window_name, cv2.WINDOW_NORMAL)

    last_path = None
    last_mtime = None
    shown_once = False

    print(f"Watching {args.directory!r} for PNG frames... (press q or Esc to quit)")

    while True:
        path = find_latest_png(args.directory)
        if path is not None:
            try:
                mtime = os.path.getmtime(path)
            except OSError:
                mtime = None

            if mtime is not None and (path != last_path or mtime != last_mtime):
                # Canvas::renderOut() writes atomically (temp file + rename),
                # so a torn read here should be rare; if it happens, just
                # retry on the next poll rather than crashing the viewer.
                image = cv2.imread(path, cv2.IMREAD_UNCHANGED)
                if image is not None:
                    cv2.imshow(window_name, image)
                    cv2.setWindowTitle(window_name, os.path.basename(path))
                    last_path = path
                    last_mtime = mtime
                    shown_once = True

        # waitKey both pumps the window's event loop and provides the poll delay.
        key = cv2.waitKey(max(1, int(args.poll_interval * 1000))) & 0xFF
        if key in (ord("q"), 27):  # 27 = Esc
            break

        # Bail out if the user closes the window via the titlebar button.
        if shown_once and cv2.getWindowProperty(window_name, cv2.WND_PROP_VISIBLE) < 1:
            break

    cv2.destroyAllWindows()


if __name__ == "__main__":
    main()
