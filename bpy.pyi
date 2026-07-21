"""
Python bindings for the Metal Procedural Art Engine
"""
from __future__ import annotations
import typing
__all__: list[str] = ['Canvas', 'open']
class Canvas:
    def __init__(self) -> None:
        ...
    def render(self) -> None:
        ...
def open(arg0: typing.SupportsInt | typing.SupportsIndex, arg1: typing.SupportsInt | typing.SupportsIndex) -> None:
    ...
