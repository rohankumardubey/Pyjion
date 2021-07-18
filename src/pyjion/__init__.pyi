def enable() -> bool:
    ...

def disable() -> bool:
    ...

def info(f: callable) -> dict:
    ...

def dump_il(f: callable) -> bytearray:
    ...

def dump_native(f: callable) -> tuple[bytearray, int, int]:
    ...

def enable_tracing() -> None:
    ...

def disable_tracing() -> None:
    ...

def enable_profiling() -> None:
    ...

def disable_profiling() -> None:
    ...

def enable_pgc() -> None:
    ...

def disable_pgc() -> None:
    ...

def set_optimization_level(level: int) -> None:
    ...

def enable_debug() -> None:
    ...

def disable_debug() -> None:
    ...

def get_offsets(f: callable) -> tuple[tuple[int, int, int]]:
    ...

def enable_graphs() -> None:
    ...

def disable_graphs() -> None:
    ...

def get_graph(f: callable) -> str:
    ...

def init(clrjitpath: str) -> None:
    ...

def status() -> dict:
    ...

def symbols(f: callable) -> dict:
    ...

__version__: str
