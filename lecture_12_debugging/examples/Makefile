# Makefile for Lecture 12 Debugging Examples
# Ders 12: Debugging ve Profiling

CC = gcc
CFLAGS = -g -O0 -Wall    # -g: debug info, -O0: optimizasyon yok

TARGETS = segfault_example memory_leak gdb_practice

.PHONY: all clean test-gdb test-valgrind test-strace

all: $(TARGETS)

segfault_example: segfault_example.c
	$(CC) $(CFLAGS) -o $@ $<

memory_leak: memory_leak.c
	$(CC) $(CFLAGS) -o $@ $<

gdb_practice: gdb_practice.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS) core core.* vgcore.*

# === Test hedefleri ===

# Core dump için hazırlık
enable-core:
	@echo "Core dump etkinlestiriliyor..."
	ulimit -c unlimited
	@echo "Guncel limit: $$(ulimit -c)"

# GDB testi
test-gdb:
	@echo "=== GDB Testi ==="
	@echo "Asagidaki komutlari deneyin:"
	@echo "  gdb ./gdb_practice"
	@echo "  (gdb) break main"
	@echo "  (gdb) run"
	@echo "  (gdb) next"
	@echo "  (gdb) print x"
	@echo "  (gdb) backtrace"
	@echo "  (gdb) continue"
	@echo "  (gdb) quit"

# Valgrind testi
test-valgrind:
	@echo "=== Valgrind Testi ==="
	@if command -v valgrind > /dev/null; then \
		echo "8" | valgrind --leak-check=full ./memory_leak; \
	else \
		echo "Valgrind kurulu degil!"; \
		echo "Kurulum: sudo apt install valgrind"; \
	fi

# strace testi
test-strace:
	@echo "=== strace Testi ==="
	strace -e trace=write ./gdb_practice 2>&1 | head -30

# ltrace testi
test-ltrace:
	@echo "=== ltrace Testi ==="
	ltrace ./gdb_practice 2>&1 | head -30

# Core dump analizi
test-core:
	@echo "=== Core Dump Analizi ==="
	@echo "Once core dump etkinlestirin: ulimit -c unlimited"
	@echo "Sonra: echo 1 | ./segfault_example"
	@echo "Core dump olusacak, analiz icin:"
	@echo "  coredumpctl gdb"
	@echo "  veya: gdb ./segfault_example core"

# Performans testi
test-perf:
	@echo "=== perf Testi ==="
	@if command -v perf > /dev/null; then \
		sudo perf stat ./gdb_practice; \
	else \
		echo "perf kurulu degil!"; \
		echo "Kurulum: sudo apt install linux-tools-common"; \
	fi

# Tüm testler hakkında bilgi
info:
	@echo "Debug Ornekleri:"
	@echo "  segfault_example - Core dump analizi icin"
	@echo "  memory_leak      - Valgrind analizi icin"
	@echo "  gdb_practice     - GDB komutlari icin"
	@echo ""
	@echo "Test hedefleri:"
	@echo "  make test-gdb      - GDB kullanim ornegi"
	@echo "  make test-valgrind - Valgrind testi"
	@echo "  make test-strace   - strace testi"
	@echo "  make test-ltrace   - ltrace testi"
	@echo "  make test-core     - Core dump rehberi"
	@echo "  make test-perf     - perf testi"
