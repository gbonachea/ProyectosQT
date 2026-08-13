#!/bin/bash
# Script para empaquetar Arova con todas sus dependencias Qt
# Crea una carpeta portable estilo Firefox/Chrome

set -e

BIN_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="$BIN_DIR/build/src"
SOURCE_DIR="$BIN_DIR/src"
OUTPUT_DIR="$BIN_DIR/arova-bundle"

echo "==> Creando bundle portable en: $OUTPUT_DIR"

rm -rf "$OUTPUT_DIR"
mkdir -p "$OUTPUT_DIR"/{lib,plugins,translations,locale,resources,qtwebengine,qtwebengine_locales}

# 1. Copiar ejecutable
echo "==> Copiando ejecutable..."
cp "$BUILD_DIR/arova" "$OUTPUT_DIR/arova"

# 2. Recolectar librerías necesarias
echo "==> Recolectando librerías..."
collect_libs() {
    local binary="$1"
    while IFS= read -r line; do
        # Extraer la ruta después de '=> ' y antes de la dirección
        local path
        path=$(echo "$line" | grep -oP '=> \K/\S+' || true)
        [[ -z "$path" ]] && continue
        local real_path
        real_path=$(readlink -f "$path" 2>/dev/null || echo "$path")
        local name
        name=$(basename "$real_path")

        # Saltar librerías base del sistema
        case "$name" in
            libc.so.*|libm.so.*|libdl.so.*|librt.so.*|libpthread.so.*|libstdc++*|libgcc_s*|libGLdispatch*|libbsd.so*|libmd.so*)
                continue ;;
            libX11.so*|libxcb.so*|libXau.so*|libXdmcp.so*|libXext.so*|libXfixes.so*|libXrender.so*)
                continue ;;
            libXcomposite.so*|libXdamage.so*|libXrandr.so*|libXtst.so*|libxkbcommon.so*)
                continue ;;
            libEGL.so*|libOpenGL.so*|libGLX.so*|libdrm*|libgbm*|libxshmfence*)
                continue ;;
            libwayland-*.so*)
                continue ;;
            # Qt libraries - we want these!
            libQt6*)
                echo "$real_path" ;;
            # Other non-system libs we need
            libicudata*|libicuuc*|libicui18n*|libdouble-conversion*|libpcre2-16*)
                echo "$real_path" ;;
            libpng16*|libharfbuzz*|libfreetype*|libfontconfig*|libexpat*|libz.so*|libb2*|libmd4c*)
                echo "$real_path" ;;
            libjpeg*|libwebp*|libsharpyuv*|libopus*|libvpx*|libsnappy*|libminizip*)
                echo "$real_path" ;;
            libxml2*|libxslt*|liblcms2*|libopenjp2*|libbrotli*|libzstd*)
                echo "$real_path" ;;
            libdbus*|libglib*|libgobject*|libgio*|libgmodule*|libmount*|libblkid*)
                echo "$real_path" ;;
            libselinux*|libpcre2-8*|libffi*)
                echo "$real_path" ;;
            libnspr4*|libnss3*|libnssutil3*|libplc4*|libplds4*)
                echo "$real_path" ;;
            libevent*|libproxy*|libcurl*|libnghttp2*|libidn2*|libpsl*|libnettle*|libgnutls*)
                echo "$real_path" ;;
            libhogweed*|libgmp*|libcrypto*|librtmp*|libssh*|libldap*|liblber*|libsasl2*)
                echo "$real_path" ;;
            libp11-kit*|libtasn1*|libunistring*|libkrb5*|libk5crypto*|libcom_err*)
                echo "$real_path" ;;
            libkrb5support*|libgssapi_krb5*|libgcrypt*|libgpg-error*|libcap*)
                echo "$real_path" ;;
            liblz4*|liblzma*|libselinux*|libsystemd*)
                echo "$real_path" ;;
            libkeyutils*|libresolv*|libxkbfile*|libgraphite2*|libbrotlicommon*)
                echo "$real_path" ;;
            libduktape*|libxkbfile*)
                echo "$real_path" ;;
        esac
    done < <(ldd "$binary" 2>/dev/null)
}

declare -A COPIED
copy_lib() {
    local path="$1"
    [[ -n "${COPIED[$path]}" ]] && return
    COPIED[$path]=1
    local name
    name=$(basename "$path")
    cp -L "$path" "$OUTPUT_DIR/lib/$name" 2>/dev/null || true
}

# Recolectar del binario principal
echo "   Analizando binario..."
while IFS= read -r lib_path; do
    copy_lib "$lib_path"
done < <(collect_libs "$BUILD_DIR/arova" | sort -u)

# 3. Copiar plugins de Qt
echo "==> Copiando plugins Qt..."
QT_PLUGIN_DIR="/usr/lib/x86_64-linux-gnu/qt6/plugins"
PLUGIN_DIRS="platforms imageformats iconengines platformthemes platforminputcontexts generic printsupport tls xcbglintegrations egldeviceintegrations wayland-decoration-client wayland-graphics-integration-client wayland-graphics-integration-server wayland-shell-integration networkinformation position"

for dir in $PLUGIN_DIRS; do
    if [ -d "$QT_PLUGIN_DIR/$dir" ]; then
        mkdir -p "$OUTPUT_DIR/plugins/$dir"
        cp -L "$QT_PLUGIN_DIR/$dir"/*.so "$OUTPUT_DIR/plugins/$dir/" 2>/dev/null || true
        # Resolver dependencias de cada plugin
        for plugin in "$QT_PLUGIN_DIR/$dir"/*.so; do
            [ -f "$plugin" ] || continue
            while IFS= read -r lib_path; do
                copy_lib "$lib_path"
            done < <(collect_libs "$plugin" | sort -u)
        done
    fi
done

echo "   Librerías: $(ls "$OUTPUT_DIR/lib" | wc -l) archivos"
echo "   Plugins: $(find "$OUTPUT_DIR/plugins" -name '*.so' | wc -l) archivos"

# 4. Copiar traducciones de Qt
echo "==> Copiando traducciones Qt..."
QT_TRANSLATIONS="/usr/share/qt6/translations"
cp "$QT_TRANSLATIONS"/qt_*.qm "$OUTPUT_DIR/translations/" 2>/dev/null || true
cp "$QT_TRANSLATIONS"/qtbase_*.qm "$OUTPUT_DIR/translations/" 2>/dev/null || true
cp "$QT_TRANSLATIONS"/qtwebengine_*.qm "$OUTPUT_DIR/translations/" 2>/dev/null || true

# 5. Recursos de QtWebEngine (.pak en resources/)
echo "==> Copiando recursos QtWebEngine..."
RES_DIR="/usr/share/qt6/resources"
for f in qtwebengine_resources.pak qtwebengine_resources_100p.pak qtwebengine_resources_200p.pak qtwebengine_devtools_resources.pak; do
    [ -f "$RES_DIR/$f" ] && cp "$RES_DIR/$f" "$OUTPUT_DIR/resources/"
done

# 6. Localizaciones QtWebEngine (.pak de idiomas)
echo "==> Copiando localizaciones QtWebEngine..."
cp /usr/share/qt6/translations/qtwebengine_locales/*.pak "$OUTPUT_DIR/qtwebengine_locales/" 2>/dev/null || true

# 7. QtWebEngineProcess (junto al binario)
echo "==> Copiando QtWebEngineProcess..."
cp "/usr/lib/qt6/libexec/QtWebEngineProcess" "$OUTPUT_DIR/QtWebEngineProcess"

# 8. Traducciones de Arova
echo "==> Copiando traducciones Arova..."
cp "$SOURCE_DIR/locale"/*.qm "$OUTPUT_DIR/locale/" 2>/dev/null || true

# 9. qt.conf
echo "==> Creando qt.conf..."
cat > "$OUTPUT_DIR/qt.conf" << 'QTCONF'
[Paths]
Plugins = plugins
Translations = translations
QTCONF

# 10. Configurar RPATH
echo "==> Configurando RPATH..."
if command -v patchelf &>/dev/null; then
    patchelf --set-rpath '$ORIGIN/lib' "$OUTPUT_DIR/arova"
    patchelf --set-rpath '$ORIGIN/lib' "$OUTPUT_DIR/QtWebEngineProcess"
    echo "   RPATH configurado"
else
    echo "   AVISO: patchelf no instalado. Se usará LD_LIBRARY_PATH."
fi

# 11. Script de lanzamiento
echo "==> Creando script de lanzamiento..."
cat > "$OUTPUT_DIR/arova.sh" << 'LAUNCHER'
#!/bin/bash
DIR="$(cd "$(dirname "$0")" && pwd)"

export LD_LIBRARY_PATH="$DIR/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
export QTWEBENGINEPROCESS_PATH="$DIR/QtWebEngineProcess"

exec "$DIR/arova" "$@"
LAUNCHER
chmod +x "$OUTPUT_DIR/arova.sh"

echo ""
echo "=== Bundle creado ==="
echo "Tamaño: $(du -sh "$OUTPUT_DIR" | cut -f1)"
echo "Ruta: $OUTPUT_DIR"
echo ""
echo "Para ejecutar:"
echo "  $OUTPUT_DIR/arova.sh"
