# Makefile for Lecture 11 Real-Time Examples
# Ders 11: Gerçek Zamanlı Linux

CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lpthread

TARGETS = rt_scheduling memory_lock

.PHONY: all clean test

all: $(TARGETS)

rt_scheduling: rt_scheduling.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

memory_lock: memory_lock.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)

# === Test hedefleri ===

test-scheduling:
	@echo "=== RT Scheduling Testi ==="
	@echo "Not: Root yetkisi gereklidir"
	sudo ./rt_scheduling

test-memory:
	@echo "=== Memory Lock Testi ==="
	sudo ./memory_lock

test-cyclictest:
	@echo "=== cyclictest Ornegi ==="
	@echo "cyclictest kurulu olmali: sudo apt install rt-tests"
	@if command -v cyclictest > /dev/null; then \
		sudo cyclictest -l 10000 -m -Sp90 -i200 -h100 -q; \
	else \
		echo "cyclictest bulunamadi!"; \
		echo "Kurulum: sudo apt install rt-tests"; \
	fi

test-all: test-memory test-scheduling

# Bilgi
info:
	@echo "RT Ornekleri:"
	@echo "  rt_scheduling - SCHED_FIFO/SCHED_RR gosterimi"
	@echo "  memory_lock   - mlockall/mlock gosterimi"
	@echo ""
	@echo "Kullanim:"
	@echo "  make           - Derle"
	@echo "  make test-all  - Tum testleri calistir"
	@echo "  make clean     - Temizle"
