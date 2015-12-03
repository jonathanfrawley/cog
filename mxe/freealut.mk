# This file is part of MXE.
# See index.html for further information.
PKG             := freealut
$(PKG)_IGNORE   :=
$(PKG)_VERSION  := 1.0.0
$(PKG)_CHECKSUM := 6e6abff8dcf16afcff13d2a45ade024b91b5d968668c4604abf779134142a072
$(PKG)_SUBDIR   := freealut-master
$(PKG)_FILE     := master.zip
$(PKG)_URL      := https://github.com/vancegroup/freealut/archive/master.zip
$(PKG)_DEPS     := gcc openal

define $(PKG)_UPDATE
    echo 'TODO: write update script for $(PKG).' >&2;
    echo $($(PKG)_VERSION)
endef

define $(PKG)_BUILD
    mkdir '$(1).build'
    cd '$(1).build' && '$(TARGET)-cmake' '$(1)'

    '$(TARGET)-cmake' --build '$(1).build' -- -j '$(JOBS)'

    '$(TARGET)-cmake' \
        -DCMAKE_INSTALL_COMPONENT=$(if $(BUILD_STATIC),static,shared)-lib \
        -P '$(1).build/cmake_install.cmake'
    '$(TARGET)-cmake' \
        -DCMAKE_INSTALL_COMPONENT=header \
        -P '$(1).build/cmake_install.cmake'
endef
