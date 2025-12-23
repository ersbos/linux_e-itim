# hello_package.mk - Buildroot paket tanımı örneği
# Ders 10: Buildroot ve Yocto
#
# Bu dosya, özel bir uygulamayı Buildroot'a nasıl
# ekleyeceğinizi gösterir.
#
# Kullanım:
# 1. Bu dosyayı buildroot/package/hello-package/ dizinine kopyalayın
# 2. Config.in dosyasını da aynı dizine kopyalayın
# 3. buildroot/package/Config.in dosyasına ekleme yapın:
#    source "package/hello-package/Config.in"
# 4. make menuconfig ile hello-package'ı seçin
# 5. make ile derleyin

HELLO_PACKAGE_VERSION = 1.0
HELLO_PACKAGE_SITE = $(TOPDIR)/../my-packages/hello-package
HELLO_PACKAGE_SITE_METHOD = local
HELLO_PACKAGE_LICENSE = GPL-2.0
HELLO_PACKAGE_LICENSE_FILES = LICENSE

# Derleme komutları
define HELLO_PACKAGE_BUILD_CMDS
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_LDFLAGS) \
		-o $(@D)/hello_package $(@D)/hello_package.c
endef

# Kurulum komutları
define HELLO_PACKAGE_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/hello_package \
		$(TARGET_DIR)/usr/bin/hello_package
endef

# Temizlik komutları (opsiyonel)
define HELLO_PACKAGE_CLEAN_CMDS
	rm -f $(@D)/hello_package
endef

$(eval $(generic-package))
