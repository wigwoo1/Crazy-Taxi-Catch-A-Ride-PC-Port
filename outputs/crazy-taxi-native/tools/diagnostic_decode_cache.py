"""Offline diagnostic decode cache. Never used by the native game runtime."""
class CachedDisassembler:
    def __init__(self, backend):
        self.backend = backend
        self.cache = {}
        self.hits = 0
        self.misses = 0
    def __getattr__(self, name):
        return getattr(self.backend, name)
    def disasm(self, code, address, count=0):
        if count not in (0, 1) or len(code) > 4:
            return self.backend.disasm(code, address, count=count)
        key = (bytes(code), address, count)
        if key not in self.cache:
            self.cache[key] = tuple(self.backend.disasm(code, address, count=count))
            self.misses += 1
        else:
            self.hits += 1
        return iter(self.cache[key])
