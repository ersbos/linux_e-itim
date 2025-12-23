#!/bin/bash
# kernel_info.sh - Kernel bilgilerini görüntüleme scripti
# Ders 5: Linux Kernel Temelleri

echo "=========================================="
echo "      KERNEL BİLGİ RAPORU"
echo "=========================================="

echo ""
echo "=== Kernel Versiyonu ==="
uname -r
echo ""

echo "=== Detaylı Bilgi ==="
uname -a
echo ""

echo "=== Kernel Yapım Tarihi ==="
cat /proc/version
echo ""

echo "=== Yüklü Modüller (ilk 20) ==="
lsmod | head -21
echo ""

echo "=== CPU Bilgisi ==="
grep -E "model name|cores|cpu MHz" /proc/cpuinfo | head -5
echo ""

echo "=== Bellek Bilgisi ==="
free -h
echo ""

echo "=== Kernel Komut Satırı ==="
cat /proc/cmdline
echo ""

echo "=== /proc/sys/kernel Dizini ==="
echo "Hostname: $(cat /proc/sys/kernel/hostname)"
echo "OS Type: $(cat /proc/sys/kernel/ostype)"
echo "OS Release: $(cat /proc/sys/kernel/osrelease)"
echo ""

echo "=== Kernel HZ (yaklaşık) ==="
if [ -f /boot/config-$(uname -r) ]; then
    grep CONFIG_HZ /boot/config-$(uname -r) | grep -v "_"
else
    echo "Kernel config dosyası bulunamadı"
fi
echo ""

echo "=========================================="
echo "              Rapor Sonu"
echo "=========================================="
